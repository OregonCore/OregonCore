/*
* This file is part of the OregonCore Project. See AUTHORS file for Copyright information
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Common.h"
#include "G3D/Vector3.h"
#include "Cell.h"
#include "CellImpl.h"
#include "CreatureTextMgr.h"
#include "DatabaseEnv.h"
#include "GossipDef.h"
#include "GridDefines.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Group.h"
#include "Language.h"
#include "ObjectMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "SmartAI.h"
#include "SmartScript.h"
#include "SpellMgr.h"
#include "GameEventMgr.h"
#include "ScriptMgr.h"

class OregonStringTextBuilder
{
    public:
        OregonStringTextBuilder(WorldObject* obj, ChatMsg msgtype, int32 id, uint32 language, WorldObject* target)
            : _source(obj), _msgType(msgtype), _textId(id), _language(language), _target(target)
        {
        }

        size_t operator()(WorldPacket* data, int locale) const
        {
            std::string text = sObjectMgr.GetOregonString(_textId, locale);

            return CreatureTextMgr::BuildMonsterChat(data, _source, _msgType, text, Language(_language), _target, locale);
        }

        WorldObject* _source;
        ChatMsg _msgType;
        int32 _textId;
        uint32 _language;
        WorldObject* _target;
};

SmartScript::SmartScript()
{
    go = NULL;
    me = NULL;
    trigger = NULL;
    mEventPhase = 0;
    mPathId = 0;
    mTargetStorage = new ObjectListMap();
    mTextTimer = 0;
    mLastTextID = 0;
    mUseTextTimer = false;
    mTalkerEntry = 0;
    mTemplate = SMARTAI_TEMPLATE_BASIC;
    mScriptType = SMART_SCRIPT_TYPE_CREATURE;
    isProcessingTimedActionList = false;
}

SmartScript::~SmartScript()
{
    for (ObjectListMap::iterator itr = mTargetStorage->begin(); itr != mTargetStorage->end(); ++itr)
        delete itr->second;

    delete mTargetStorage;
    mCounterList.clear();
}

void SmartScript::OnReset()
{
    ResetBaseObject();
    for (SmartAIEventList::iterator i = mEvents.begin(); i != mEvents.end(); ++i)
    {
        if (!((*i).event.event_flags & SMART_EVENT_FLAG_DONT_RESET))
        {
            InitTimer((*i));
            (*i).runOnce = false;
        }
    }
    ProcessEventsFor(SMART_EVENT_RESET);
    mLastInvoker.Clear();
    mCounterList.clear();
}

void SmartScript::ProcessEventsFor(SMART_EVENT e, Unit* unit, uint32 var0, uint32 var1, bool bvar, const SpellEntry* spell, GameObject* gob)
{
    for (SmartAIEventList::iterator i = mEvents.begin(); i != mEvents.end(); ++i)
    {
        SMART_EVENT eventType = SMART_EVENT(i->GetEventType());
        if (eventType == SMART_EVENT_LINK)//special handling
            continue;

        if (eventType == e)
        {
            ConditionList conds = sConditionMgr.GetConditionsForSmartEvent(i->entryOrGuid, i->event_id, i->source_type);
            ConditionSourceInfo info = ConditionSourceInfo(unit, GetBaseObject());

            if (sConditionMgr.IsObjectMeetToConditions(info, conds))
                ProcessEvent(*i, unit, var0, var1, bvar, spell, gob);
        }
    }
}

void SmartScript::ProcessAction(SmartScriptHolder& e, Unit* unit, uint32 var0, uint32 var1, bool bvar, const SpellEntry* spell, GameObject* gob)
{
    // calc random
    if (e.GetEventType() != SMART_EVENT_LINK && e.event.event_chance < 100 && e.event.event_chance)
    {
        if (!roll_chance_i(e.event.event_chance))
            return;
    }
    e.runOnce = true;//used for repeat check

    if (unit)
        mLastInvoker = unit->GetGUID();

    if (Unit* tempInvoker = GetLastInvoker())
        sLog.outDebug("SmartScript::ProcessAction: Invoker: %s (guidlow: %u)", tempInvoker->GetName(), tempInvoker->GetGUIDLow());

    switch (e.GetActionType())
    {
        case SMART_ACTION_TALK:
        {
            ObjectList* targets = GetTargets(e, unit);
            Creature* talker = me;
            Player* targetPlayer = NULL;
            Unit* talkTarget = NULL;

            if (targets)
            {
                for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
                {
                    if (IsCreature(*itr) && !(*itr)->ToCreature()->IsPet()) // Prevented sending text to pets.
                    {
                        if (e.action.talk.useTalkTarget)
                            talkTarget = (*itr)->ToCreature();
                        else
                            talker = (*itr)->ToCreature();
                        break;
                    }
                    else if (IsPlayer(*itr))
                    {
                        targetPlayer = (*itr)->ToPlayer();
                        break;
                    }
                }

                delete targets;
            }

            if (!talker)
                break;

            mTalkerEntry = talker->GetEntry();
            mLastTextID = e.action.talk.textGroupID;
            mTextTimer = e.action.talk.duration;

            if (IsPlayer(GetLastInvoker())) // used for $vars in texts and whisper target
                talkTarget = GetLastInvoker();
            else if (targetPlayer)
                talkTarget = targetPlayer;

            mUseTextTimer = true;

            sCreatureTextMgr->SendChat(talker, uint8(e.action.talk.textGroupID), talkTarget);
            sLog.outDebug("SmartScript::ProcessAction: SMART_ACTION_TALK: talker: %s (GuidLow: %u), textGuid: %u",
                talker->GetName(), talker->GetGUIDLow(), talkTarget ? talkTarget->GetGUIDLow() : 0);
            break;
        }
        case SMART_ACTION_SIMPLE_TALK:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (targets)
            {
                for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
                {
                    if (IsCreature(*itr))
                        sCreatureTextMgr->SendChat((*itr)->ToCreature(), uint8(e.action.talk.textGroupID), IsPlayer(GetLastInvoker()) ? GetLastInvoker() : 0);
                    else if (IsPlayer(*itr) && me)
                    {
                        Unit* templastInvoker = GetLastInvoker();
                        sCreatureTextMgr->SendChat(me, uint8(e.action.talk.textGroupID), IsPlayer(templastInvoker) ? templastInvoker : 0, CHAT_MSG_ADDON, LANG_ADDON, TEXT_RANGE_NORMAL, 0, TEAM_NONE, false, (*itr)->ToPlayer());
                    }
                    //sLog.outDebug("SmartScript::ProcessAction:: SMART_ACTION_SIMPLE_TALK: talker: %s (GuidLow: %u), textGroupId: %u",
                        //(*itr)->GetName(), (*itr)->GetGUIDLow(), uint8(e.action.talk.textGroupID));
                }

                delete targets;
            }
            break;
        }
        case SMART_ACTION_PLAY_EMOTE:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (targets)
            {
                for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
                {
                    if (IsUnit(*itr))
                    {
                        (*itr)->ToUnit()->HandleEmoteCommand(e.action.emote.emote);
                        sLog.outDebug("SmartScript::ProcessAction:: SMART_ACTION_PLAY_EMOTE: target: %s (GuidLow: %u), emote: %u",
                            (*itr)->GetName(), (*itr)->GetGUIDLow(), e.action.emote.emote);
                    }
                }

                delete targets;
            }
            break;
        }
        case SMART_ACTION_SOUND:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (targets)
            {
                for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
                {
                    if (IsUnit(*itr))
                    {
                        if (e.action.sound.distance == 1)
                            (*itr)->PlayDistanceSound(e.action.sound.sound, e.action.sound.onlySelf ? (*itr)->ToPlayer() : nullptr);
                        else
                            (*itr)->PlayDirectSound(e.action.sound.sound, e.action.sound.onlySelf ? (*itr)->ToPlayer() : nullptr);

                        sLog.outDebug("SmartScript::ProcessAction:: SMART_ACTION_SOUND: target: %s (GuidLow: %u), sound: %u, onlyself: %u",
                            (*itr)->GetName(), (*itr)->GetGUIDLow(), e.action.sound.sound, e.action.sound.onlySelf);
                    }
                }

                delete targets;
            }
            break;
        }
        case SMART_ACTION_SET_FACTION:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (targets)
            {
                for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
                {
                    if (IsCreature(*itr))
                    {
                        if (e.action.faction.factionID)
                        {
                            (*itr)->ToCreature()->SetFaction(e.action.faction.factionID);
                            sLog.outDebug("SmartScript::ProcessAction:: SMART_ACTION_SET_FACTION: Creature entry %u, GuidLow %u set faction to %u",
                                (*itr)->GetEntry(), (*itr)->GetGUIDLow(), e.action.faction.factionID);
                        }
                        else
                        {
                            if (CreatureInfo const* ci = sObjectMgr.GetCreatureTemplate((*itr)->ToCreature()->GetEntry()))
                            {
                                if ((*itr)->ToCreature()->GetFaction() != ci->faction)
                                {
                                    (*itr)->ToCreature()->SetFaction(ci->faction);
                                    sLog.outDebug("SmartScript::ProcessAction:: SMART_ACTION_SET_FACTION: Creature entry %u, GuidLow %u set faction to %u",
                                        (*itr)->GetEntry(), (*itr)->GetGUIDLow(), ci->faction);
                                }
                            }
                        }
                    }
                }

                delete targets;
            }
            break;
        }
        case SMART_ACTION_MORPH_TO_ENTRY_OR_MODEL:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
            {
                if (!IsCreature(*itr))
                    continue;

                if (e.action.morphOrMount.creature || e.action.morphOrMount.model)
                {
                    //set model based on entry from creature_template
                    if (e.action.morphOrMount.creature)
                    {
                        if (CreatureInfo const* ci = sObjectMgr.GetCreatureTemplate(e.action.morphOrMount.creature))
                        {
                            uint32 displayId = ci->GetRandomValidModelId();
                            (*itr)->ToCreature()->SetDisplayId(displayId);
                            sLog.outDebug("SmartScript::ProcessAction:: SMART_ACTION_MORPH_TO_ENTRY_OR_MODEL: Creature entry %u, GuidLow %u set displayid to %u",
                                (*itr)->GetEntry(), (*itr)->GetGUIDLow(), displayId);
                        }
                    }
                    //if no param1, then use value from param2 (modelId)
                    else
                    {
                        (*itr)->ToCreature()->SetDisplayId(e.action.morphOrMount.model);
                        sLog.outDebug("SmartScript::ProcessAction:: SMART_ACTION_MORPH_TO_ENTRY_OR_MODEL: Creature entry %u, GuidLow %u set displayid to %u",
                            (*itr)->GetEntry(), (*itr)->GetGUIDLow(), e.action.morphOrMount.model);
                    }
                }
                else
                {
                    (*itr)->ToCreature()->DeMorph();
                    sLog.outDebug("SmartScript::ProcessAction:: SMART_ACTION_MORPH_TO_ENTRY_OR_MODEL: Creature entry %u, GuidLow %u demorphs.",
                        (*itr)->GetEntry(), (*itr)->GetGUIDLow());
                }
            }

            delete targets;
            break;
        }
        case SMART_ACTION_FAIL_QUEST:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
            {
                if (IsPlayer(*itr))
                {
                    (*itr)->ToPlayer()->FailQuest(e.action.quest.quest);
                    sLog.outDebug("SmartScript::ProcessAction:: SMART_ACTION_FAIL_QUEST: Player guidLow %u fails quest %u",
                        (*itr)->GetGUIDLow(), e.action.quest.quest);
                }
            }

            delete targets;
            break;
        }
        case SMART_ACTION_ADD_QUEST:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
            {
                if (IsPlayer(*itr))
                {
                    if (Quest const* q = sObjectMgr.GetQuestTemplate(e.action.quest.quest))
                    {
                        (*itr)->ToPlayer()->AddQuest(q, NULL);
                        sLog.outDebug("SmartScript::ProcessAction:: SMART_ACTION_ADD_QUEST: Player guidLow %u add quest %u",
                            (*itr)->GetGUIDLow(), e.action.quest.quest);
                    }
                }
            }

            delete targets;
            break;
        }
        case SMART_ACTION_SET_REACT_STATE:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
            {
                if (!IsCreature(*itr))
                    continue;

                (*itr)->ToCreature()->SetReactState(ReactStates(e.action.react.state));
            }

            delete targets;
            break;
        }
        case SMART_ACTION_RANDOM_EMOTE:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            uint32 emotes[SMART_ACTION_PARAM_COUNT];
            emotes[0] = e.action.randomEmote.emote1;
            emotes[1] = e.action.randomEmote.emote2;
            emotes[2] = e.action.randomEmote.emote3;
            emotes[3] = e.action.randomEmote.emote4;
            emotes[4] = e.action.randomEmote.emote5;
            emotes[5] = e.action.randomEmote.emote6;
            uint32 temp[SMART_ACTION_PARAM_COUNT];
            uint32 count = 0;
            for (uint8 i = 0; i < SMART_ACTION_PARAM_COUNT; i++)
            {
                if (emotes[i])
                {
                    temp[count] = emotes[i];
                    ++count;
                }
            }

            if (count == 0)
            {
                delete targets;
                break;
            }

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
            {
                if (IsUnit(*itr))
                {
                    uint32 emote = temp[urand(0, count - 1)];
                    (*itr)->ToUnit()->HandleEmoteCommand(emote);
                    sLog.outDebug("SmartScript::ProcessAction:: SMART_ACTION_RANDOM_EMOTE: Creature guidLow %u handle random emote %u",
                        (*itr)->GetGUIDLow(), emote);
                }
            }

            delete targets;
            break;
        }
        case SMART_ACTION_THREAT_ALL_PCT:
        {
            if (!me)
                break;

            ThreatContainer::StorageType threatList = me->getThreatManager().getThreatList();
            for (ThreatContainer::StorageType::const_iterator i = threatList.begin(); i != threatList.end(); ++i)
            {
                if (Unit* target = ObjectAccessor::GetUnit(*me, (*i)->getUnitGuid()))
                {
                    me->getThreatManager().modifyThreatPercent(target, e.action.threatPCT.threatINC ? (int32)e.action.threatPCT.threatINC : -(int32)e.action.threatPCT.threatDEC);
                    sLog.outDebug("SmartScript::ProcessAction:: SMART_ACTION_THREAT_ALL_PCT: Creature guidLow %u modify threat for unit %u, value %i",
                        me->GetGUIDLow(), target->GetGUIDLow(), e.action.threatPCT.threatINC ? (int32)e.action.threatPCT.threatINC : -(int32)e.action.threatPCT.threatDEC);
                }
            }
            break;
        }
        case SMART_ACTION_THREAT_SINGLE_PCT:
        {
            if (!me)
                break;

            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
            {
                if (IsUnit(*itr))
                {
                    me->getThreatManager().modifyThreatPercent((*itr)->ToUnit(), e.action.threatPCT.threatINC ? (int32)e.action.threatPCT.threatINC : -(int32)e.action.threatPCT.threatDEC);
                    sLog.outDebug("SmartScript::ProcessAction:: SMART_ACTION_THREAT_SINGLE_PCT: Creature guidLow %u modify threat for unit %u, value %i",
                        me->GetGUIDLow(), (*itr)->GetGUIDLow(), e.action.threatPCT.threatINC ? (int32)e.action.threatPCT.threatINC : -(int32)e.action.threatPCT.threatDEC);
                }
            }

            delete targets;
            break;
        }
        case SMART_ACTION_CALL_AREAEXPLOREDOREVENTHAPPENS:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
            {
                if (IsPlayer(*itr))
                {
                    (*itr)->ToPlayer()->GroupEventHappens(e.action.quest.quest, me);

                    sLog.outDebug("SmartScript::ProcessAction:: SMART_ACTION_CALL_AREAEXPLOREDOREVENTHAPPENS: Player guidLow %u credited quest %u",
                        (*itr)->GetGUIDLow(), e.action.quest.quest);
                }
            }

            delete targets;
            break;
        }
        case SMART_ACTION_CAST:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
            {
                if (!IsUnit(*itr))
                    continue;

                if (!(e.action.cast.flags & SMARTCAST_AURA_NOT_PRESENT) || !(*itr)->ToUnit()->HasAura(e.action.cast.spell))
                {
                    if (me)
                    {
                        if (e.action.cast.flags & SMARTCAST_INTERRUPT_PREVIOUS)
                            me->InterruptNonMeleeSpells(false);

                        if (e.action.cast.flags & SMARTCAST_COMBAT_MOVE)
                        {
                            // If cast flag SMARTCAST_COMBAT_MOVE is set combat movement will not be allowed
                            // unless target is outside spell range, out of mana, or LOS.

                            bool _allowMove = false;
                            SpellEntry const* spellInfo = sSpellStore.LookupEntry(e.action.cast.spell);
                            float maxRange = sSpellRangeStore.LookupEntry(spellInfo->rangeIndex)->maxRange;
                            float minRange = sSpellRangeStore.LookupEntry(spellInfo->rangeIndex)->minRange;
                            int32 mana = me->GetPower(POWER_MANA);
                            if (me->GetDistance(*itr) > maxRange ||
                                me->GetDistance(*itr) < minRange ||
                                !me->IsWithinLOSInMap(*itr) ||
                                mana < CalculatePowerCost(spellInfo, me, GetSpellSchoolMask(spellInfo)))
                                _allowMove = true;

                            CAST_AI(SmartAI, me->AI())->SetCombatMove(_allowMove);
                        }

                        me->CastSpell((*itr)->ToUnit(), e.action.cast.spell, (e.action.cast.flags & SMARTCAST_TRIGGERED) != 0);
                    }
                    else if (go)
                        go->CastSpell((*itr)->ToUnit(), e.action.cast.spell, (e.action.cast.flags & SMARTCAST_TRIGGERED) != 0);

                    sLog.outDebug("SmartScript::ProcessAction:: SMART_ACTION_CAST:: %s: %u casts spell %u on target %u with castflags %u",
                        (me ? me->GetObjectGUID() : go->GetObjectGUID()).GetTypeName(), me ? me->GetGUIDLow() : go->GetGUIDLow(), e.action.cast.spell, (*itr)->GetGUIDLow(), e.action.cast.flags);
                }
                else
                    sLog.outDebug("Spell %u not cast because it has flag SMARTCAST_AURA_NOT_PRESENT and the target (%s) already has the aura", e.action.cast.spell, (*itr)->GetObjectGUID().GetString().c_str());
            }

            delete targets;
            break;
        }
        case SMART_ACTION_INVOKER_CAST:
        {
            Unit* tempLastInvoker = GetLastInvoker();
            if (!tempLastInvoker)
                break;

            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
            {
                if (!IsUnit(*itr))
                    continue;

                if (!(e.action.cast.flags & SMARTCAST_AURA_NOT_PRESENT) || !(*itr)->ToUnit()->HasAura(e.action.cast.spell))
                {
                    if (e.action.cast.flags & SMARTCAST_INTERRUPT_PREVIOUS)
                        tempLastInvoker->InterruptNonMeleeSpells(false);

                    tempLastInvoker->CastSpell((*itr)->ToUnit(), e.action.cast.spell, (e.action.cast.flags & SMARTCAST_TRIGGERED) != 0);
                    sLog.outDebug("SmartScript::ProcessAction:: SMART_ACTION_INVOKER_CAST: Invoker %u casts spell %u on target %u with castflags %u",
                        tempLastInvoker->GetGUIDLow(), e.action.cast.spell, (*itr)->GetGUIDLow(), e.action.cast.flags);
                }
                else
                    sLog.outDebug("Spell %u not cast because it has flag SMARTCAST_AURA_NOT_PRESENT and the target (%s) already has the aura", e.action.cast.spell, (*itr)->GetObjectGUID().GetString().c_str());
            }

            delete targets;
            break;
        }
        case SMART_ACTION_ADD_AURA:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
            {
                if (IsUnit(*itr))
                {
                    (*itr)->ToUnit()->AddAura(e.action.cast.spell, (*itr)->ToUnit());
                    sLog.outDebug("SmartScript::ProcessAction:: SMART_ACTION_ADD_AURA: Adding aura %u to unit %u",
                        e.action.cast.spell, (*itr)->GetGUIDLow());
                }
            }

            delete targets;
            break;
        }
        case SMART_ACTION_ACTIVATE_GOBJECT:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
            {
                if (IsGameObject(*itr))
                {
                    // Activate
                    (*itr)->ToGameObject()->SetLootState(GO_READY);
                    (*itr)->ToGameObject()->UseDoorOrButton(0, false, unit);
                    sLog.outDebug("SmartScript::ProcessAction:: SMART_ACTION_ACTIVATE_GOBJECT. Gameobject %u (entry: %u) activated",
                        (*itr)->GetGUIDLow(), (*itr)->GetEntry());
                }
            }

            delete targets;
            break;
        }
        case SMART_ACTION_RESET_GOBJECT:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
            {
                if (IsGameObject(*itr))
                {
                    (*itr)->ToGameObject()->ResetDoorOrButton();
                    sLog.outDebug("SmartScript::ProcessAction:: SMART_ACTION_RESET_GOBJECT. Gameobject %u (entry: %u) reset",
                        (*itr)->GetGUIDLow(), (*itr)->GetEntry());
                }
            }

            delete targets;
            break;
        }
        case SMART_ACTION_SET_EMOTE_STATE:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
            {
                if (IsUnit(*itr))
                {
                    (*itr)->ToUnit()->SetUInt32Value(UNIT_NPC_EMOTESTATE, e.action.emote.emote);
                    sLog.outDebug("SmartScript::ProcessAction:: SMART_ACTION_SET_EMOTE_STATE. Unit %u set emotestate to %u",
                        (*itr)->GetGUIDLow(), e.action.emote.emote);
                }
            }

            delete targets;
            break;
        }
        case SMART_ACTION_SET_UNIT_FLAG:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
            {
                if (IsUnit(*itr))
                {
                    if (!e.action.unitFlag.type)
                    {
                        (*itr)->ToUnit()->SetFlag(UNIT_FIELD_FLAGS, e.action.unitFlag.flag);
                        sLog.outDebug("SmartScript::ProcessAction:: SMART_ACTION_SET_UNIT_FLAG. Unit %u added flag %u to UNIT_FIELD_FLAGS",
                        (*itr)->GetGUIDLow(), e.action.unitFlag.flag);
                    }
                    else
                    {
                        (*itr)->ToUnit()->SetFlag(UNIT_FIELD_FLAGS_2, e.action.unitFlag.flag);
                        sLog.outDebug("SmartScript::ProcessAction:: SMART_ACTION_SET_UNIT_FLAG. Unit %u added flag %u to UNIT_FIELD_FLAGS_2",
                        (*itr)->GetGUIDLow(), e.action.unitFlag.flag);
                    }
                }
            }

            delete targets;
            break;
        }
        case SMART_ACTION_REMOVE_UNIT_FLAG:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
            {
                if (IsUnit(*itr))
                {
                    if (!e.action.unitFlag.type)
                    {
                        (*itr)->ToUnit()->RemoveFlag(UNIT_FIELD_FLAGS, e.action.unitFlag.flag);
                        sLog.outDebug("SmartScript::ProcessAction:: SMART_ACTION_REMOVE_UNIT_FLAG. Unit %u removed flag %u to UNIT_FIELD_FLAGS",
                        (*itr)->GetGUIDLow(), e.action.unitFlag.flag);
                    }
                    else
                    {
                        (*itr)->ToUnit()->RemoveFlag(UNIT_FIELD_FLAGS_2, e.action.unitFlag.flag);
                        sLog.outDebug("SmartScript::ProcessAction:: SMART_ACTION_REMOVE_UNIT_FLAG. Unit %u removed flag %u to UNIT_FIELD_FLAGS_2",
                        (*itr)->GetGUIDLow(), e.action.unitFlag.flag);
                    }
                }
            }

            delete targets;
            break;
        }
        case SMART_ACTION_AUTO_ATTACK:
        {
            if (!IsSmart())
                break;

            CAST_AI(SmartAI, me->AI())->SetAutoAttack(e.action.autoAttack.attack != 0);
            sLog.outDebug("SmartScript::ProcessAction:: SMART_ACTION_AUTO_ATTACK: Creature: %u bool on = %u",
                me->GetGUIDLow(), e.action.autoAttack.attack);
            break;
        }
        case SMART_ACTION_ALLOW_COMBAT_MOVEMENT:
        {
            if (!IsSmart())
                break;

            bool move = e.action.combatMove.move != 0;
            CAST_AI(SmartAI, me->AI())->SetCombatMove(move);
            sLog.outDebug("SmartScript::ProcessAction:: SMART_ACTION_ALLOW_COMBAT_MOVEMENT: Creature %u bool on = %u",
                me->GetGUIDLow(), e.action.combatMove.move);
            break;
        }
        case SMART_ACTION_SET_EVENT_PHASE:
        {
            if (!GetBaseObject())
                break;

            SetPhase(e.action.setEventPhase.phase);
            sLog.outDebug("SmartScript::ProcessAction:: SMART_ACTION_SET_EVENT_PHASE: Creature %u set event phase %u",
                GetBaseObject()->GetGUIDLow(), e.action.setEventPhase.phase);
            break;
        }
        case SMART_ACTION_INC_EVENT_PHASE:
        {
            if (!GetBaseObject())
                break;

            IncPhase(e.action.incEventPhase.inc);
            DecPhase(e.action.incEventPhase.dec);
            sLog.outDebug("SmartScript::ProcessAction:: SMART_ACTION_INC_EVENT_PHASE: Creature %u inc event phase by %u, "
                "decrease by %u", GetBaseObject()->GetGUIDLow(), e.action.incEventPhase.inc, e.action.incEventPhase.dec);
            break;
        }
        case SMART_ACTION_EVADE:
        {
            if (!me)
                break;

            me->AI()->EnterEvadeMode();
            sLog.outDebug("SmartScript::ProcessAction:: SMART_ACTION_EVADE: Creature %u EnterEvadeMode", me->GetGUIDLow());
            break;
        }
        case SMART_ACTION_FLEE_FOR_ASSIST:
        {
            if (!me)
                break;

            me->DoFleeToGetAssistance();
            if (e.action.flee.withEmote)
            {
                OregonStringTextBuilder builder(me, CHAT_MSG_MONSTER_EMOTE, LANG_FLEE, LANG_UNIVERSAL, NULL);
                sCreatureTextMgr->SendChatPacket(me, builder, CHAT_MSG_MONSTER_EMOTE);
            }
            sLog.outDebug("SmartScript::ProcessAction:: SMART_ACTION_FLEE_FOR_ASSIST: Creature %u DoFleeToGetAssistance", me->GetGUIDLow());
            break;
        }
        case SMART_ACTION_CALL_GROUPEVENTHAPPENS:
        {
            if (!unit)
                break;

            if (IsPlayer(unit) && GetBaseObject())
            {
                unit->ToPlayer()->GroupEventHappens(e.action.quest.quest, GetBaseObject());
                sLog.outDebug("SmartScript::ProcessAction: SMART_ACTION_CALL_GROUPEVENTHAPPENS: Player %u, group credit for quest %u",
                    unit->GetGUIDLow(), e.action.quest.quest);
            }

            break;
        }
        case SMART_ACTION_COMBAT_STOP:
        {
            if (!me)
                break;

            me->CombatStop(true);
            sLog.outDebug("SmartScript::ProcessAction:: SMART_ACTION_COMBAT_STOP: %s CombatStop", me->GetObjectGUID().GetString().c_str());
            break;
        }
        case SMART_ACTION_REMOVEAURASFROMSPELL:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
            {
                if (!IsUnit(*itr))
                    continue;

                if (e.action.removeAura.spell)
                    (*itr)->ToUnit()->RemoveAurasDueToSpell(e.action.removeAura.spell);
                else
                    (*itr)->ToUnit()->RemoveAllAuras();

                sLog.outDebug("SmartScript::ProcessAction: SMART_ACTION_REMOVEAURASFROMSPELL: Unit %u, spell %u",
                    (*itr)->GetGUIDLow(), e.action.removeAura.spell);
            }

            delete targets;
            break;
        }
        case SMART_ACTION_FOLLOW:
        {
            if (!IsSmart())
                break;

            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
            {
                CAST_AI(SmartAI, me->AI())->StopFollow();
                break;
            }

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
            {
                if (IsUnit(*itr))
                {
                    CAST_AI(SmartAI, me->AI())->SetFollow((*itr)->ToUnit(), (float)e.action.follow.dist, (float)e.action.follow.angle, e.action.follow.credit, e.action.follow.entry, e.action.follow.creditType);
                    sLog.outDebug("SmartScript::ProcessAction: SMART_ACTION_FOLLOW: Creature %u following target %u",
                        me->GetGUIDLow(), (*itr)->GetGUIDLow());
                    break;
                }
            }

            delete targets;
            break;
        }
        case SMART_ACTION_RANDOM_PHASE:
        {
            if (!GetBaseObject())
                break;

            uint32 phases[SMART_ACTION_PARAM_COUNT];
            phases[0] = e.action.randomPhase.phase1;
            phases[1] = e.action.randomPhase.phase2;
            phases[2] = e.action.randomPhase.phase3;
            phases[3] = e.action.randomPhase.phase4;
            phases[4] = e.action.randomPhase.phase5;
            phases[5] = e.action.randomPhase.phase6;
            uint32 temp[SMART_ACTION_PARAM_COUNT];
            uint32 count = 0;
            for (uint8 i = 0; i < SMART_ACTION_PARAM_COUNT; i++)
            {
                if (phases[i] > 0)
                {
                    temp[count] = phases[i];
                    ++count;
                }
            }

            if (count == 0)
                break;

            uint32 phase = temp[urand(0, count - 1)];
            SetPhase(phase);
            sLog.outDebug("SmartScript::ProcessAction: SMART_ACTION_RANDOM_PHASE: Creature %u sets event phase to %u",
                GetBaseObject()->GetGUIDLow(), phase);
            break;
        }
        case SMART_ACTION_RANDOM_PHASE_RANGE:
        {
            if (!GetBaseObject())
                break;

            uint32 phase = urand(e.action.randomPhaseRange.phaseMin, e.action.randomPhaseRange.phaseMax);
            SetPhase(phase);
            sLog.outDebug("SmartScript::ProcessAction: SMART_ACTION_RANDOM_PHASE_RANGE: Creature %u sets event phase to %u",
                GetBaseObject()->GetGUIDLow(), phase);
            break;
        }
        case SMART_ACTION_CALL_KILLEDMONSTER:
        {
            if (e.target.type == SMART_TARGET_NONE || e.target.type == SMART_TARGET_SELF) // Loot recipient and his group members
            {
                if (!me)
                    break;

                if (Player* player = me->GetLootRecipient())
                {
                    player->RewardPlayerAndGroupAtEvent(e.action.killedMonster.creature, player);
                    sLog.outDebug("SmartScript::ProcessAction: SMART_ACTION_CALL_KILLEDMONSTER: Player %u, Killcredit: %u",
                        player->GetGUIDLow(), e.action.killedMonster.creature);
                }
            }
            else // Specific target type
            {
                ObjectList* targets = GetTargets(e, unit);
                if (!targets)
                    break;

                for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
                {
                    if (IsPlayer(*itr))
                    {
                        (*itr)->ToPlayer()->KilledMonsterCredit(e.action.killedMonster.creature);
                        sLog.outDebug("SmartScript::ProcessAction: SMART_ACTION_CALL_KILLEDMONSTER: Player %u, Killcredit: %u",
                            (*itr)->GetGUIDLow(), e.action.killedMonster.creature);
                    }
                }

                delete targets;
            }
            break;
        }
        case SMART_ACTION_SET_INST_DATA:
        {
            WorldObject* obj = GetBaseObject();
            if (!obj)
                obj = unit;

            if (!obj)
                break;

            InstanceData* instance = obj->GetInstanceData();
            if (!instance)
            {
                sLog.outError("SmartScript: Event %u attempt to set instance data without instance script. EntryOrGuid %d", e.GetEventType(), e.entryOrGuid);
                break;
            }

            instance->SetData(e.action.setInstanceData.field, e.action.setInstanceData.data);
            sLog.outDebug("SmartScript::ProcessAction: SMART_ACTION_SET_INST_DATA: Field: %u, data: %u",
                e.action.setInstanceData.field, e.action.setInstanceData.data);
            break;
        }
        case SMART_ACTION_SET_INST_DATA64:
        {
            WorldObject* obj = GetBaseObject();
            if (!obj)
                obj = unit;

            if (!obj)
                break;

            InstanceData* instance = obj->GetInstanceData();
            if (!instance)
            {
                sLog.outError("SmartScript: Event %u attempt to set instance data without instance script. EntryOrGuid %d", e.GetEventType(), e.entryOrGuid);
                break;
            }

            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            instance->SetData64(e.action.setInstanceData64.field, targets->front()->GetGUID());
            sLog.outDebug("SmartScript::ProcessAction: SMART_ACTION_SET_INST_DATA64: Field: %u, data: %s",
                e.action.setInstanceData64.field, targets->front()->GetObjectGUID().GetString().c_str());

            delete targets;
            break;
        }
        case SMART_ACTION_UPDATE_TEMPLATE:
        {
            ObjectList* targets = GetTargets(e, unit);

            if (!targets)
                break;

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
                if (IsCreature(*itr))
                    (*itr)->ToCreature()->UpdateEntry(e.action.updateTemplate.creature);

            delete targets;
            break;
        }
        case SMART_ACTION_DIE:
        {
            if (me && !me->isDead())
            {
                me->Kill(me);
                sLog.outDebug("SmartScript::ProcessAction: SMART_ACTION_DIE: Creature %u", me->GetGUIDLow());
            }
            break;
        }
        case SMART_ACTION_SET_IN_COMBAT_WITH_ZONE:
        {
            if (me)
            {
                me->SetInCombatWithZone();
                sLog.outDebug("SmartScript::ProcessAction: SMART_ACTION_SET_IN_COMBAT_WITH_ZONE: Creature %u", me->GetGUIDLow());
            }
            break;
        }
        case SMART_ACTION_CALL_FOR_HELP:
        {
            if (me)
            {
                me->CallForHelp((float)e.action.callHelp.range);
                if (e.action.callHelp.withEmote)
                {
                    OregonStringTextBuilder builder(me, CHAT_MSG_MONSTER_EMOTE, LANG_CALL_FOR_HELP, LANG_UNIVERSAL, NULL);
                    sCreatureTextMgr->SendChatPacket(me, builder, CHAT_MSG_MONSTER_EMOTE);
                }
                sLog.outDebug("SmartScript::ProcessAction: SMART_ACTION_CALL_FOR_HELP: Creature %u", me->GetGUIDLow());
            }
            break;
        }
        case SMART_ACTION_SET_SHEATH:
        {
            if (me)
            {
                me->SetSheath(SheathState(e.action.setSheath.sheath));
                sLog.outDebug("SmartScript::ProcessAction: SMART_ACTION_SET_SHEATH: Creature %u, State: %u",
                    me->GetGUIDLow(), e.action.setSheath.sheath);
            }
            break;
        }
        case SMART_ACTION_FORCE_DESPAWN:
        {
            ObjectList* targets = GetTargets(e, unit);

            if (!targets)
                break;

            // there should be at least a world update tick before despawn, to avoid breaking linked actions
            int32 const respawnDelay = std::max<int32>(e.action.forceDespawn.delay, 1);

            for (WorldObject* target : *targets)
            {
                if (Creature* creatureTarget = target->ToCreature())
                {
                    if (SmartAI* smartAI = CAST_AI(SmartAI, creatureTarget->AI()))
                    {
                        smartAI->SetDespawnTime(respawnDelay);
                        smartAI->StartDespawn();
                    }
                    else
                        creatureTarget->DespawnOrUnsummon(respawnDelay);
                }
                else if (GameObject* goTarget = target->ToGameObject())
                    goTarget->SetRespawnTime(respawnDelay);
            }

            delete targets;
            break;
        }
        case SMART_ACTION_SET_INGAME_PHASE_MASK:
        {
            // No phasing on tbc
            break;
        }
        case SMART_ACTION_MOUNT_TO_ENTRY_OR_MODEL:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
            {
                if (!IsUnit(*itr))
                    continue;

                if (e.action.morphOrMount.creature || e.action.morphOrMount.model)
                {
                    if (e.action.morphOrMount.creature > 0)
                    {
                        if (CreatureInfo const* cInfo = sObjectMgr.GetCreatureTemplate(e.action.morphOrMount.creature))
                            (*itr)->ToUnit()->Mount(cInfo->GetRandomValidModelId());
                    }
                    else
                        (*itr)->ToUnit()->Mount(e.action.morphOrMount.model);
                }
                else
                    (*itr)->ToUnit()->Dismount();
            }

            delete targets;
            break;
        }
        case SMART_ACTION_SET_INVINCIBILITY_HP_LEVEL:
        {
            if (!me)
                break;

            SmartAI* ai = CAST_AI(SmartAI, me->AI());

            if (!ai)
                break;

            if (e.action.invincHP.percent)
                ai->SetInvincibilityHpLevel(me->CountPctFromMaxHealth(e.action.invincHP.percent));
            else
                ai->SetInvincibilityHpLevel(e.action.invincHP.minHP);

            break;
        }
        case SMART_ACTION_SET_DATA:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
            {
                if (IsCreature(*itr))
                    (*itr)->ToCreature()->AI()->SetData(e.action.setData.field, e.action.setData.data);
                else if (IsGameObject(*itr))
                    (*itr)->ToGameObject()->AI()->SetData(e.action.setData.field, e.action.setData.data);
            }

            delete targets;
            break;
        }
        case SMART_ACTION_MOVE_FORWARD:
        {
            if (!me)
                break;

            float x, y, z;
            me->GetClosePoint(x, y, z, me->GetObjectSize() / 3, (float)e.action.moveRandom.distance);
            me->GetMotionMaster()->MovePoint(SMART_RANDOM_POINT, x, y, z);
            break;
        }
        case SMART_ACTION_MOVE_OFFSET:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
            {
                if (!IsCreature(*itr))
                    continue;

                Position pos = (*itr)->GetPosition();

                // Use forward/backward/left/right cartesian plane movement
                float x, y, z, o;
                o = pos.GetOrientation();
                x = pos.GetPositionX() + (std::cos(o - (M_PI / 2))*e.target.x) + (std::cos(o)*e.target.y);
                y = pos.GetPositionY() + (std::sin(o - (M_PI / 2))*e.target.x) + (std::sin(o)*e.target.y);
                z = pos.GetPositionZ() + e.target.z;
                (*itr)->ToCreature()->GetMotionMaster()->MovePoint(SMART_RANDOM_POINT, x, y, z);
            }
            break;
        }
        case SMART_ACTION_SET_VISIBILITY:
        {
            if (me)
                me->SetVisible(e.action.visibility.state ? true : false);
            break;
        }
        case SMART_ACTION_SET_ACTIVE:
        {
            if (WorldObject* baseObj = GetBaseObject())
                baseObj->setActive(e.action.active.state != 0);

            break;
        }
        case SMART_ACTION_ATTACK_START:
        {
            if (!me)
                break;

            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
            {
                if (IsUnit(*itr))
                {
                    me->AI()->AttackStart((*itr)->ToUnit());
                    break;
                }
            }

            delete targets;
            break;
        }
        case SMART_ACTION_SUMMON_CREATURE:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (targets)
            {
                float x, y, z, o;
                for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
                {
                    (*itr)->GetPosition(x, y, z, o);
                    x += e.target.x;
                    y += e.target.y;
                    z += e.target.z;
                    o += e.target.o;
                    if (Creature* summon = GetBaseObject()->SummonCreature(e.action.summonCreature.creature, x, y, z, o, (TempSummonType)e.action.summonCreature.type, e.action.summonCreature.duration))
                        if (e.action.summonCreature.attackInvoker)
                            summon->AI()->AttackStart((*itr)->ToUnit());
                }

                delete targets;
            }

            if (e.GetTargetType() != SMART_TARGET_POSITION)
                break;

            if (Creature* summon = GetBaseObject()->SummonCreature(e.action.summonCreature.creature, e.target.x, e.target.y, e.target.z, e.target.o, (TempSummonType)e.action.summonCreature.type, e.action.summonCreature.duration))
                if (unit && e.action.summonCreature.attackInvoker)
                    summon->AI()->AttackStart(unit);
            break;
        }
        case SMART_ACTION_SUMMON_GO:
        {
            if (!GetBaseObject())
                break;

            ObjectList* targets = GetTargets(e, unit);
            if (targets)
            {
                float x, y, z, o;
                for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
                {
                    if (!IsUnit(*itr))
                        continue;

                    (*itr)->GetPosition(x, y, z, o);
                    x += e.target.x;
                    y += e.target.y;
                    z += e.target.z;
                    o += e.target.o;
                    GetBaseObject()->SummonGameObject(e.action.summonGO.entry, x, y, z, o, 0, 0, 0, 0, e.action.summonGO.despawnTime);
                }

                delete targets;
            }

            if (e.GetTargetType() != SMART_TARGET_POSITION)
                break;

            GetBaseObject()->SummonGameObject(e.action.summonGO.entry, e.target.x, e.target.y, e.target.z, e.target.o, 0, 0, 0, 0, e.action.summonGO.despawnTime);
            break;
        }
        case SMART_ACTION_KILL_UNIT:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
            {
                if (!IsUnit(*itr))
                    continue;

                (*itr)->ToUnit()->Kill((*itr)->ToUnit());
            }

            delete targets;
            break;
        }
        case SMART_ACTION_INSTALL_AI_TEMPLATE:
        {
            InstallTemplate(e);
            break;
        }
        case SMART_ACTION_ADD_ITEM:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
            {
                if (!IsPlayer(*itr))
                    continue;

                (*itr)->ToPlayer()->AddItem(e.action.item.entry, e.action.item.count);
            }

            delete targets;
            break;
        }
        case SMART_ACTION_REMOVE_ITEM:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
            {
                if (!IsPlayer(*itr))
                    continue;

                (*itr)->ToPlayer()->DestroyItemCount(e.action.item.entry, e.action.item.count, true);
            }

            delete targets;
            break;
        }
        case SMART_ACTION_STORE_TARGET_LIST:
        {
            ObjectList* targets = GetTargets(e, unit);
            StoreTargetList(targets, e.action.storeTargets.id);
            break;
        }
        case SMART_ACTION_TELEPORT:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
            {
                if (IsPlayer(*itr))
                    (*itr)->ToPlayer()->TeleportTo(e.action.teleport.mapID, e.target.x, e.target.y, e.target.z, e.target.o);
                else if (IsCreature(*itr))
                    (*itr)->ToCreature()->NearTeleportTo(e.target.x, e.target.y, e.target.z, e.target.o);
            }

            delete targets;
            break;
        }
        case SMART_ACTION_SET_FLY:
        {
            if (!IsSmart())
                break;

            CAST_AI(SmartAI, me->AI())->SetFly(e.action.setFly.fly != 0);
            break;
        }
        case SMART_ACTION_SET_RUN:
        {
            if (!IsSmart())
                break;

            CAST_AI(SmartAI, me->AI())->SetRun(e.action.setRun.run != 0);
            break;
        }
        case SMART_ACTION_SET_SWIM:
        {
            if (!IsSmart())
                break;

            CAST_AI(SmartAI, me->AI())->SetSwim(e.action.setSwim.swim != 0);
            break;
        }
        case SMART_ACTION_SET_COUNTER:
        {
            if (ObjectList* targets = GetTargets(e, unit))
            {
                for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
                {
                    if (IsCreature(*itr))
                    {
                        if (SmartAI* ai = CAST_AI(SmartAI, (*itr)->ToCreature()->AI()))
                            ai->GetScript()->StoreCounter(e.action.setCounter.counterId, e.action.setCounter.value, e.action.setCounter.reset);
                        else
                            sLog.outError("SmartScript: Action target for SMART_ACTION_SET_COUNTER is not using SmartAI, skipping");
                    }
                    else if (IsGameObject(*itr))
                    {
                        if (SmartGameObjectAI* ai = CAST_AI(SmartGameObjectAI, (*itr)->ToGameObject()->AI()))
                            ai->GetScript()->StoreCounter(e.action.setCounter.counterId, e.action.setCounter.value, e.action.setCounter.reset);
                        else
                            sLog.outError("SmartScript: Action target for SMART_ACTION_SET_COUNTER is not using SmartGameObjectAI, skipping");
                    }
                }

                delete targets;
            }
            else
                StoreCounter(e.action.setCounter.counterId, e.action.setCounter.value, e.action.setCounter.reset);

            break;
        }
        case SMART_ACTION_WP_START:
        {
            if (!IsSmart())
                break;

            bool run = e.action.wpStart.run != 0;
            uint32 entry = e.action.wpStart.pathID;
            bool repeat = e.action.wpStart.repeat != 0;
            ObjectList* targets = GetTargets(e, unit);
            StoreTargetList(targets, SMART_ESCORT_TARGETS);
            me->SetReactState((ReactStates)e.action.wpStart.reactState);
            CAST_AI(SmartAI, me->AI())->StartPath(run, entry, repeat, unit);

            uint32 quest = e.action.wpStart.quest;
            uint32 DespawnTime = e.action.wpStart.despawnTime;
            CAST_AI(SmartAI, me->AI())->mEscortQuestID = quest;
            CAST_AI(SmartAI, me->AI())->SetDespawnTime(DespawnTime);
            break;
        }
        case SMART_ACTION_WP_PAUSE:
        {
            if (!IsSmart())
                break;

            uint32 delay = e.action.wpPause.delay;
            CAST_AI(SmartAI, me->AI())->PausePath(delay, e.GetEventType() == SMART_EVENT_WAYPOINT_REACHED ? false : true);
            break;
        }
        case SMART_ACTION_WP_STOP:
        {
            if (!IsSmart())
                break;

            uint32 DespawnTime = e.action.wpStop.despawnTime;
            uint32 quest = e.action.wpStop.quest;
            bool fail = e.action.wpStop.fail != 0;
            CAST_AI(SmartAI, me->AI())->StopPath(DespawnTime, quest, fail);
            break;
        }
        case SMART_ACTION_WP_RESUME:
        {
            if (!IsSmart())
                break;

            CAST_AI(SmartAI, me->AI())->ResumePath();
            break;
        }
        case SMART_ACTION_SET_ORIENTATION:
        {
            if (!me)
                break;

            if (e.GetTargetType() == SMART_TARGET_SELF)
                me->SetFacingTo(me->GetHomePosition().GetOrientation());
            else if (e.GetTargetType() == SMART_TARGET_POSITION)
                me->SetFacingTo(e.target.o);
            else if (ObjectList* targets = GetTargets(e, unit))
            {
                if (!targets->empty())
                    me->SetFacingToObject(*targets->begin());

                delete targets;
            }

            break;
        }
        case SMART_ACTION_PLAYMOVIE:
        {
            // no exist in tbc
            break;
        }
        case SMART_ACTION_MOVE_TO_POS:
        {
            if (!IsSmart())
                break;

            WorldObject* target = NULL;

            if (e.GetTargetType() == SMART_TARGET_CREATURE_RANGE || e.GetTargetType() == SMART_TARGET_CREATURE_GUID ||
                e.GetTargetType() == SMART_TARGET_CREATURE_DISTANCE || e.GetTargetType() == SMART_TARGET_GAMEOBJECT_RANGE ||
                e.GetTargetType() == SMART_TARGET_GAMEOBJECT_GUID || e.GetTargetType() == SMART_TARGET_GAMEOBJECT_DISTANCE ||
                e.GetTargetType() == SMART_TARGET_CLOSEST_CREATURE || e.GetTargetType() == SMART_TARGET_CLOSEST_GAMEOBJECT ||
                e.GetTargetType() == SMART_TARGET_OWNER_OR_SUMMONER || e.GetTargetType() == SMART_TARGET_ACTION_INVOKER ||
                e.GetTargetType() == SMART_TARGET_CLOSEST_ENEMY || e.GetTargetType() == SMART_TARGET_CLOSEST_FRIENDLY)
            {
                ObjectList* targets = GetTargets(e, unit);
                if (!targets)
                    break;

                target = targets->front();
                delete targets;
            }

            if (!target)
            {
                G3D::Vector3 dest(e.target.x, e.target.y, e.target.z);
                me->GetMotionMaster()->MovePoint(e.action.MoveToPos.pointId, dest.x, dest.y, dest.z);
            }
            else
                me->GetMotionMaster()->MovePoint(e.action.MoveToPos.pointId, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ());
            break;
        }
        case SMART_ACTION_RESPAWN_TARGET:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
            {
                if (IsCreature(*itr))
                    (*itr)->ToCreature()->Respawn();
                else if (IsGameObject(*itr))
                    (*itr)->ToGameObject()->SetRespawnTime(e.action.RespawnTarget.goRespawnTime);
            }

            delete targets;
            break;
        }
        case SMART_ACTION_CLOSE_GOSSIP:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
                if (IsPlayer(*itr))
                    (*itr)->ToPlayer()->PlayerTalkClass->CloseGossip();

            delete targets;
            break;
        }
        case SMART_ACTION_EQUIP:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
            {
                if (Creature* npc = (*itr)->ToCreature())
                {
                    uint32 slot[3];
                    uint32 equipId = e.action.equip.entry;
                    if (equipId)
                    {
                        EquipmentInfo const* einfo = sObjectMgr.GetEquipmentInfo(equipId);
                        if (!einfo)
                        {
                            sLog.outError("SmartScript: SMART_ACTION_EQUIP uses non-existent equipment info id %u for creature %u", equipId, npc->GetEntry());
                            break;
                        }

                        npc->SetCurrentEquipmentId(equipId);
                        slot[0] = einfo->equipentry[0];
                        slot[1] = einfo->equipentry[1];
                        slot[2] = einfo->equipentry[2];

                        for (uint8 i = 0; i < MAX_VIRTUAL_ITEM_SLOT; ++i)
                            npc->SetVirtualItem(VirtualItemSlot(i), einfo->equipentry[i]);

                    }
                    else
                    {
                        slot[0] = e.action.equip.slot1;
                        slot[1] = e.action.equip.slot2;
                        slot[2] = e.action.equip.slot3;

                        for (uint8 i = 0; i < MAX_VIRTUAL_ITEM_SLOT; ++i)
                            npc->SetVirtualItem(VirtualItemSlot(i), slot[i]);
                    }
                }
            }

            delete targets;
            break;
        }
        case SMART_ACTION_CREATE_TIMED_EVENT:
        {
            SmartEvent ne = SmartEvent();
            ne.type = (SMART_EVENT)SMART_EVENT_UPDATE;
            ne.event_chance = e.action.timeEvent.chance;
            if (!ne.event_chance) ne.event_chance = 100;

            ne.minMaxRepeat.min = e.action.timeEvent.min;
            ne.minMaxRepeat.max = e.action.timeEvent.max;
            ne.minMaxRepeat.repeatMin = e.action.timeEvent.repeatMin;
            ne.minMaxRepeat.repeatMax = e.action.timeEvent.repeatMax;

            ne.event_flags = 0;
            if (!ne.minMaxRepeat.repeatMin && !ne.minMaxRepeat.repeatMax)
                ne.event_flags |= SMART_EVENT_FLAG_NOT_REPEATABLE;

            SmartAction ac = SmartAction();
            ac.type = (SMART_ACTION)SMART_ACTION_TRIGGER_TIMED_EVENT;
            ac.timeEvent.id = e.action.timeEvent.id;

            SmartScriptHolder ev = SmartScriptHolder();
            ev.event = ne;
            ev.event_id = e.action.timeEvent.id;
            ev.target = e.target;
            ev.action = ac;
            InitTimer(ev);
            mStoredEvents.push_back(ev);
            break;
        }
        case SMART_ACTION_TRIGGER_TIMED_EVENT:
            ProcessEventsFor((SMART_EVENT)SMART_EVENT_TIMED_EVENT_TRIGGERED, NULL, e.action.timeEvent.id);
            break;
        case SMART_ACTION_REMOVE_TIMED_EVENT:
            mRemIDs.push_back(e.action.timeEvent.id);
            break;
        case SMART_ACTION_OVERRIDE_SCRIPT_BASE_OBJECT:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            for (ObjectList::iterator itr = targets->begin(); itr != targets->end(); ++itr)
            {
                if (IsCreature(*itr))
                {
                    if (!meOrigGUID && me)
                        meOrigGUID = me->GetGUID();
                    if (!goOrigGUID && go)
                        goOrigGUID = go->GetGUID();
                    go = NULL;
                    me = (*itr)->ToCreature();
                    break;
                }
                else if (IsGameObject(*itr))
                {
                    if (!meOrigGUID && me)
                        meOrigGUID = me->GetGUID();
                    if (!goOrigGUID && go)
                        goOrigGUID = go->GetGUID();
                    go = (*itr)->ToGameObject();
                    me = NULL;
                    break;
                }
            }

            delete targets;
            break;
        }
        case SMART_ACTION_RESET_SCRIPT_BASE_OBJECT:
            ResetBaseObject();
            break;
        case SMART_ACTION_CALL_SCRIPT_RESET:
            SetPhase(0);
            OnReset();
            break;
        case SMART_ACTION_SET_RANGED_MOVEMENT:
        {
            if (!IsSmart())
                break;

            float attackDistance = float(e.action.setRangedMovement.distance);
            float attackAngle = float(e.action.setRangedMovement.angle) / 180.0f * float(M_PI);

            ObjectList* targets = GetTargets(e, unit);
            if (targets)
            {
                for (ObjectList::iterator itr = targets->begin(); itr != targets->end(); ++itr)
                    if (Creature* target = (*itr)->ToCreature())
                        if (IsSmart(target) && target->GetVictim())
                            if (CAST_AI(SmartAI, target->AI())->CanCombatMove())
                                target->GetMotionMaster()->MoveChase(target->GetVictim(), attackDistance, attackAngle);

                delete targets;
            }
            break;
        }
        case SMART_ACTION_CALL_TIMED_ACTIONLIST:
        {
            if (e.GetTargetType() == SMART_TARGET_NONE)
            {
                sLog.outError("SmartScript: Entry %d SourceType %u Event %u Action %u is using TARGET_NONE(0) for Script9 target. Please correct target_type in database.", e.entryOrGuid, e.GetScriptType(), e.GetEventType(), e.GetActionType());
                break;
            }

            if (ObjectList* targets = GetTargets(e, unit))
            {
                for (ObjectList::iterator itr = targets->begin(); itr != targets->end(); ++itr)
                {
                    if (Creature* target = (*itr)->ToCreature())
                    {
                        if (IsSmart(target))
                            CAST_AI(SmartAI, target->AI())->SetScript9(e, e.action.timedActionList.id, GetLastInvoker());
                    }
                    else if (GameObject* goTarget = (*itr)->ToGameObject())
                    {
                        if (IsSmartGO(goTarget))
                            CAST_AI(SmartGameObjectAI, goTarget->AI())->SetScript9(e, e.action.timedActionList.id, GetLastInvoker());
                    }
                }

                delete targets;
            }
            break;
        }
        case SMART_ACTION_SET_NPC_FLAG:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
                if (IsCreature(*itr))
                    (*itr)->ToUnit()->SetUInt32Value(UNIT_NPC_FLAGS, e.action.unitFlag.flag);

            delete targets;
            break;
        }
        case SMART_ACTION_ADD_NPC_FLAG:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
                if (IsCreature(*itr))
                    (*itr)->ToUnit()->SetFlag(UNIT_NPC_FLAGS, e.action.unitFlag.flag);

            delete targets;
            break;
        }
        case SMART_ACTION_REMOVE_NPC_FLAG:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
                if (IsCreature(*itr))
                    (*itr)->ToUnit()->RemoveFlag(UNIT_NPC_FLAGS, e.action.unitFlag.flag);

            delete targets;
            break;
        }
        case SMART_ACTION_CROSS_CAST:
        {
            ObjectList* casters = GetTargets(CreateEvent(SMART_EVENT_UPDATE_IC, 0, 0, 0, 0, 0, 0, SMART_ACTION_NONE, 0, 0, 0, 0, 0, 0, (SMARTAI_TARGETS)e.action.cast.targetType, e.action.cast.targetParam1, e.action.cast.targetParam2, e.action.cast.targetParam3, 0), unit);
            if (!casters)
                break;

            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
            {
                delete casters; // casters already validated, delete now
                break;
            }

            for (ObjectList::const_iterator itr = casters->begin(); itr != casters->end(); ++itr)
            {
                if (!IsUnit(*itr))
                    continue;

                Unit* targetUnit = (*itr)->ToUnit();

                bool interruptedSpell = false;

                for (ObjectList::const_iterator it = targets->begin(); it != targets->end(); ++it)
                {
                    if (!IsUnit(*it))
                        continue;

                    if (!(e.action.cast.flags & SMARTCAST_AURA_NOT_PRESENT) || !(*it)->ToUnit()->HasAura(e.action.cast.spell))
                    {
                        if (!interruptedSpell && e.action.cast.flags & SMARTCAST_INTERRUPT_PREVIOUS)
                        {
                            targetUnit->InterruptNonMeleeSpells(false);
                            interruptedSpell = true;
                        }

                        targetUnit->CastSpell((*it)->ToUnit(), e.action.cast.spell, (e.action.cast.flags & SMARTCAST_TRIGGERED) != 0);
                    }
                    else
                        sLog.outDebug("Spell %u not cast because it has flag SMARTCAST_AURA_NOT_PRESENT and the target (%s) already has the aura", e.action.cast.spell, (*it)->GetObjectGUID().GetString().c_str());
                }
            }

            delete targets;
            delete casters;
            break;
        }
        case SMART_ACTION_CALL_RANDOM_TIMED_ACTIONLIST:
        {
            uint32 actions[SMART_ACTION_PARAM_COUNT];
            actions[0] = e.action.randTimedActionList.entry1;
            actions[1] = e.action.randTimedActionList.entry2;
            actions[2] = e.action.randTimedActionList.entry3;
            actions[3] = e.action.randTimedActionList.entry4;
            actions[4] = e.action.randTimedActionList.entry5;
            actions[5] = e.action.randTimedActionList.entry6;
            uint32 temp[SMART_ACTION_PARAM_COUNT];
            uint32 count = 0;
            for (uint8 i = 0; i < SMART_ACTION_PARAM_COUNT; i++)
            {
                if (actions[i] > 0)
                {
                    temp[count] = actions[i];
                    ++count;
                }
            }

            if (count == 0)
                break;

            uint32 id = temp[urand(0, count - 1)];
            if (e.GetTargetType() == SMART_TARGET_NONE)
            {
                sLog.outError("SmartScript: Entry %d SourceType %u Event %u Action %u is using TARGET_NONE(0) for Script9 target. Please correct target_type in database.", e.entryOrGuid, e.GetScriptType(), e.GetEventType(), e.GetActionType());
                break;
            }

            ObjectList* targets = GetTargets(e, unit);
            if (targets)
            {
                for (ObjectList::iterator itr = targets->begin(); itr != targets->end(); ++itr)
                {
                    if (Creature* target = (*itr)->ToCreature())
                    {
                        if (IsSmart(target))
                            CAST_AI(SmartAI, target->AI())->SetScript9(e, id, GetLastInvoker());
                    }
                    else if (GameObject* goTarget = (*itr)->ToGameObject())
                    {
                        if (IsSmartGO(goTarget))
                            CAST_AI(SmartGameObjectAI, goTarget->AI())->SetScript9(e, id, GetLastInvoker());
                    }
                }

                delete targets;
            }
            break;
        }
        case SMART_ACTION_CALL_RANDOM_RANGE_TIMED_ACTIONLIST:
        {
            uint32 id = urand(e.action.randTimedActionList.entry1, e.action.randTimedActionList.entry2);
            if (e.GetTargetType() == SMART_TARGET_NONE)
            {
                sLog.outError("SmartScript: Entry %d SourceType %u Event %u Action %u is using TARGET_NONE(0) for Script9 target. Please correct target_type in database.", e.entryOrGuid, e.GetScriptType(), e.GetEventType(), e.GetActionType());
                break;
            }

            ObjectList* targets = GetTargets(e, unit);
            if (targets)
            {
                for (ObjectList::iterator itr = targets->begin(); itr != targets->end(); ++itr)
                {
                    if (Creature* target = (*itr)->ToCreature())
                    {
                        if (IsSmart(target))
                            CAST_AI(SmartAI, target->AI())->SetScript9(e, id, GetLastInvoker());
                    }
                    else if (GameObject* goTarget = (*itr)->ToGameObject())
                    {
                        if (IsSmartGO(goTarget))
                            CAST_AI(SmartGameObjectAI, goTarget->AI())->SetScript9(e, id, GetLastInvoker());
                    }
                }

                delete targets;
            }
            break;
        }
        case SMART_ACTION_ACTIVATE_TAXI:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
                if (IsPlayer(*itr))
                    (*itr)->ToPlayer()->ActivateTaxiPathTo(e.action.taxi.id);

            delete targets;
            break;
        }
        case SMART_ACTION_RANDOM_MOVE:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            bool foundTarget = false;

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
            {
                if (IsCreature((*itr)))
                {
                    foundTarget = true;

                    if (e.action.moveRandom.distance)
                        (*itr)->ToCreature()->GetMotionMaster()->MoveRandom((float)e.action.moveRandom.distance);
                    else
                        (*itr)->ToCreature()->GetMotionMaster()->MoveIdle();
                }
            }

            if (!foundTarget && me && IsCreature(me))
            {
                if (e.action.moveRandom.distance)
                    me->GetMotionMaster()->MoveRandom((float)e.action.moveRandom.distance);
                else
                    me->GetMotionMaster()->MoveIdle();
            }

            delete targets;
            break;
        }
        case SMART_ACTION_SET_UNIT_FIELD_BYTES_1:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;
            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
                if (IsUnit(*itr))
                    (*itr)->ToUnit()->SetByteFlag(UNIT_FIELD_BYTES_1, e.action.setunitByte.type, e.action.setunitByte.byte1);

            delete targets;
            break;
        }
        case SMART_ACTION_REMOVE_UNIT_FIELD_BYTES_1:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
                if (IsUnit(*itr))
                    (*itr)->ToUnit()->RemoveByteFlag(UNIT_FIELD_BYTES_1, e.action.delunitByte.type, e.action.delunitByte.byte1);

            delete targets;
            break;
        }
        case SMART_ACTION_INTERRUPT_SPELL:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
                if (IsUnit(*itr))
                    (*itr)->ToUnit()->InterruptNonMeleeSpells(e.action.interruptSpellCasting.withDelayed != 0, e.action.interruptSpellCasting.spell_id, e.action.interruptSpellCasting.withInstant != 0);

            delete targets;
            break;
        }
        case SMART_ACTION_SEND_GO_CUSTOM_ANIM:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
                if (IsGameObject(*itr))
                    (*itr)->ToGameObject()->SendObjectCustomAnim((*itr)->ToGameObject()->GetGUID(), e.action.sendGoCustomAnim.anim);

            delete targets;
            break;
        }
        case SMART_ACTION_SET_DYNAMIC_FLAG:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
                if (IsUnit(*itr))
                    (*itr)->ToUnit()->SetUInt32Value(UNIT_DYNAMIC_FLAGS, e.action.unitFlag.flag);

            delete targets;
            break;
        }
        case SMART_ACTION_ADD_DYNAMIC_FLAG:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
                if (IsUnit(*itr))
                    (*itr)->ToUnit()->SetFlag(UNIT_DYNAMIC_FLAGS, e.action.unitFlag.flag);

            delete targets;
            break;
        }
        case SMART_ACTION_REMOVE_DYNAMIC_FLAG:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
                if (IsUnit(*itr))
                    (*itr)->ToUnit()->RemoveFlag(UNIT_DYNAMIC_FLAGS, e.action.unitFlag.flag);

            delete targets;
            break;
        }
        case SMART_ACTION_JUMP_TO_POS:
        {
            // no exist in tbc

            break;
        }
        case SMART_ACTION_GO_SET_LOOT_STATE:
        {
            ObjectList* targets = GetTargets(e, unit);

            if (!targets)
                break;

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
                if (IsGameObject(*itr))
                    (*itr)->ToGameObject()->SetLootState((LootState)e.action.setGoLootState.state);

            delete targets;
            break;
        }
        case SMART_ACTION_SEND_TARGET_TO_TARGET:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            ObjectList* storedTargets = GetTargetList(e.action.sendTargetToTarget.id);
            if (!storedTargets)
            {
                delete targets;
                break;
            }

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
            {
                if (IsCreature(*itr))
                {
                    if (SmartAI* ai = CAST_AI(SmartAI, (*itr)->ToCreature()->AI()))
                        ai->GetScript()->StoreTargetList(new ObjectList(*storedTargets), e.action.sendTargetToTarget.id);   // store a copy of target list
                    else
                        sLog.outError("SmartScript: Action target for SMART_ACTION_SEND_TARGET_TO_TARGET is not using SmartAI, skipping");
                }
                else if (IsGameObject(*itr))
                {
                    if (SmartGameObjectAI* ai = CAST_AI(SmartGameObjectAI, (*itr)->ToGameObject()->AI()))
                        ai->GetScript()->StoreTargetList(new ObjectList(*storedTargets), e.action.sendTargetToTarget.id);   // store a copy of target list
                    else
                        sLog.outError("SmartScript: Action target for SMART_ACTION_SEND_TARGET_TO_TARGET is not using SmartGameObjectAI, skipping");
                }
            }

            delete targets;
            break;
        }
        case SMART_ACTION_SEND_GOSSIP_MENU:
        {
            if (!GetBaseObject())
                break;

            sLog.outDebug("SmartScript::ProcessAction:: SMART_ACTION_SEND_GOSSIP_MENU: gossipMenuId %d, gossipNpcTextId %d",
                e.action.sendGossipMenu.gossipMenuId, e.action.sendGossipMenu.gossipNpcTextId);

            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
            {
                if (Player* player = (*itr)->ToPlayer())
                {
                    if (e.action.sendGossipMenu.gossipMenuId)
                        player->PrepareGossipMenu(GetBaseObject(), e.action.sendGossipMenu.gossipMenuId);
                    else
                        player->PlayerTalkClass->ClearMenus();

                    player->SEND_GOSSIP_MENU(e.action.sendGossipMenu.gossipNpcTextId, GetBaseObject()->GetGUID());
                }
            }

            delete targets;
            break;
        }
        case SMART_ACTION_SET_HOME_POS:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
            {
                if (IsCreature(*itr))
                {
                    if (e.GetTargetType() == SMART_TARGET_SELF)
                        (*itr)->ToCreature()->SetHomePosition(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
                    else if (e.GetTargetType() == SMART_TARGET_POSITION)
                        (*itr)->ToCreature()->SetHomePosition(e.target.x, e.target.y, e.target.z, e.target.o);
                    else if (e.GetTargetType() == SMART_TARGET_CREATURE_RANGE || e.GetTargetType() == SMART_TARGET_CREATURE_GUID ||
                             e.GetTargetType() == SMART_TARGET_CREATURE_DISTANCE || e.GetTargetType() == SMART_TARGET_GAMEOBJECT_RANGE ||
                             e.GetTargetType() == SMART_TARGET_GAMEOBJECT_GUID || e.GetTargetType() == SMART_TARGET_GAMEOBJECT_DISTANCE ||
                             e.GetTargetType() == SMART_TARGET_CLOSEST_CREATURE || e.GetTargetType() == SMART_TARGET_CLOSEST_GAMEOBJECT ||
                             e.GetTargetType() == SMART_TARGET_OWNER_OR_SUMMONER || e.GetTargetType() == SMART_TARGET_ACTION_INVOKER ||
                             e.GetTargetType() == SMART_TARGET_CLOSEST_ENEMY || e.GetTargetType() == SMART_TARGET_CLOSEST_FRIENDLY)
                    {
                        (*itr)->ToCreature()->SetHomePosition((*itr)->GetPositionX(), (*itr)->GetPositionY(), (*itr)->GetPositionZ(), (*itr)->GetOrientation());
                    }
                    else
                        sLog.outError("SmartScript: Action target for SMART_ACTION_SET_HOME_POS is invalid, skipping");
                }
            }

            delete targets;
            break;
        }
        case SMART_ACTION_SET_HEALTH_REGEN:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
                if (IsCreature(*itr))
                    (*itr)->ToCreature()->setRegeneratingHealth(e.action.setHealthRegen.regenHealth != 0);

            delete targets;
            break;
        }
        case SMART_ACTION_SET_ROOT:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
                if (IsCreature(*itr))
                    (*itr)->ToCreature()->SetControlled(e.action.setRoot.root != 0, UNIT_STATE_ROOT);

            delete targets;
            break;
        }
        case SMART_ACTION_SET_GO_FLAG:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
                if (IsGameObject(*itr))
                    (*itr)->ToGameObject()->SetUInt32Value(GAMEOBJECT_FLAGS, e.action.goFlag.flag);

            delete targets;
            break;
        }
        case SMART_ACTION_ADD_GO_FLAG:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
                if (IsGameObject(*itr))
                    (*itr)->ToGameObject()->SetFlag(GAMEOBJECT_FLAGS, e.action.goFlag.flag);

            delete targets;
            break;
        }
        case SMART_ACTION_REMOVE_GO_FLAG:
        {
            ObjectList* targets = GetTargets(e, unit);
            if (!targets)
                break;

            for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
                if (IsGameObject(*itr))
                    (*itr)->ToGameObject()->RemoveFlag(GAMEOBJECT_FLAGS, e.action.goFlag.flag);

            delete targets;
            break;
        }
        case SMART_ACTION_SUMMON_CREATURE_GROUP:
        {
            std::list<TempSummon*> summonList;
            GetBaseObject()->SummonCreatureGroup(e.action.creatureGroup.group, &summonList);

            for (std::list<TempSummon*>::const_iterator itr = summonList.begin(); itr != summonList.end(); ++itr)
                if (unit && e.action.creatureGroup.attackInvoker)
                    (*itr)->AI()->AttackStart(unit);

            break;
        }
        case SMART_ACTION_SET_POWER:
        {
            ObjectList* targets = GetTargets(e, unit);

            if (targets)
                for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
                    if (IsUnit(*itr))
                        (*itr)->ToUnit()->SetPower(Powers(e.action.power.powerType), e.action.power.newPower);

            delete targets;
            break;
        }
        case SMART_ACTION_ADD_POWER:
        {
            ObjectList* targets = GetTargets(e, unit);

            if (targets)
                for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
                    if (IsUnit(*itr))
                        (*itr)->ToUnit()->SetPower(Powers(e.action.power.powerType), (*itr)->ToUnit()->GetPower(Powers(e.action.power.powerType)) + e.action.power.newPower);

            delete targets;
            break;
        }
        case SMART_ACTION_REMOVE_POWER:
        {
            ObjectList* targets = GetTargets(e, unit);

            if (targets)
                for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
                    if (IsUnit(*itr))
                        (*itr)->ToUnit()->SetPower(Powers(e.action.power.powerType), (*itr)->ToUnit()->GetPower(Powers(e.action.power.powerType)) - e.action.power.newPower);

            delete targets;
            break;
        }
        case SMART_ACTION_GAME_EVENT_STOP:
        {
            uint32 eventId = e.action.gameEventStop.id;
            if (!sGameEventMgr.IsActiveEvent(eventId))
            {
                sLog.outError("SmartScript::ProcessAction: At case SMART_ACTION_GAME_EVENT_STOP, inactive event (id: %u)", eventId);
                break;
            }
            sGameEventMgr.StopEvent(eventId, true);
            break;
        }
        case SMART_ACTION_GAME_EVENT_START:
        {
            uint32 eventId = e.action.gameEventStart.id;
            if (sGameEventMgr.IsActiveEvent(eventId))
            {
                sLog.outDebug("SmartScript::ProcessAction: At case SMART_ACTION_GAME_EVENT_START, already activated event (id: %u)", eventId);
                break;
            }
            sGameEventMgr.StartEvent(eventId, true);
            break;
        }
        case SMART_ACTION_START_CLOSEST_WAYPOINT:
        {
            uint32 waypoints[SMART_ACTION_PARAM_COUNT];
            waypoints[0] = e.action.closestWaypointFromList.wp1;
            waypoints[1] = e.action.closestWaypointFromList.wp2;
            waypoints[2] = e.action.closestWaypointFromList.wp3;
            waypoints[3] = e.action.closestWaypointFromList.wp4;
            waypoints[4] = e.action.closestWaypointFromList.wp5;
            waypoints[5] = e.action.closestWaypointFromList.wp6;
            float distanceToClosest = std::numeric_limits<float>::max();
            WayPoint* closestWp = NULL;

            ObjectList* targets = GetTargets(e, unit);
            if (targets)
            {
                for (ObjectList::iterator itr = targets->begin(); itr != targets->end(); ++itr)
                {
                    if (Creature* target = (*itr)->ToCreature())
                    {
                        if (IsSmart(target))
                        {
                            for (uint8 i = 0; i < SMART_ACTION_PARAM_COUNT; i++)
                            {
                                if (!waypoints[i])
                                    continue;

                                WPPath* path = sSmartWaypointMgr->GetPath(waypoints[i]);

                                if (!path || path->empty())
                                    continue;

                                WPPath::const_iterator itrWp = path->find(0);

                                if (itrWp != path->end())
                                {
                                    if (WayPoint* wp = itrWp->second)
                                    {
                                        float distToThisPath = target->GetDistance(wp->x, wp->y, wp->z);

                                        if (distToThisPath < distanceToClosest)
                                        {
                                            distanceToClosest = distToThisPath;
                                            closestWp = wp;
                                        }
                                    }
                                }
                            }

                            if (closestWp)
                                CAST_AI(SmartAI, target->AI())->StartPath(false, closestWp->id, true);
                        }
                    }
                }

                delete targets;
            }
            break;
        }
        case SMART_ACTION_RANDOM_SOUND:
        {
            std::vector<uint32> sounds;

            for (uint8 i = 0; i < SMART_ACTION_PARAM_COUNT - 1; i++)
            {
                if (e.action.randomSound.sound[i])
                    sounds.push_back(e.action.randomSound.sound[i]);
            }

            bool onlySelf = e.action.randomSound.onlySelf != 0;

            ObjectList* targets = GetTargets(e, unit);
            if (targets)
            {
                for (ObjectList::const_iterator itr = targets->begin(); itr != targets->end(); ++itr)
                {
                    if (IsUnit(*itr))
                    {
                        uint32 sound = sounds[urand(0, sounds.size() - 1)];
                        (*itr)->PlayDirectSound(sound, e.action.randomSound.onlySelf ? (*itr)->ToPlayer() : nullptr);
                        //sLog.outDebug("SmartScript::ProcessAction:: SMART_ACTION_RANDOM_SOUND: target: %s (%s), sound: %u, onlyself: %u",
                          //  (*itr)->GetName().c_str(), (*itr)->GetGUID().c_str(), sound, e.action.randomSound.onlySelf);
                    }
                }

                delete targets;
                break;
            }
        }
        default:
            sLog.outError("SmartScript::ProcessAction: Entry %d SourceType %u, Event %u, Unhandled Action type %u", e.entryOrGuid, e.GetScriptType(), e.event_id, e.GetActionType());
            break;
    }

    if (e.link && e.link != e.event_id)
    {
        SmartScriptHolder& linked = SmartAIMgr::FindLinkedEvent(mEvents, e.link);
        if (linked)
            ProcessEvent(linked, unit, var0, var1, bvar, spell, gob);
        else
            sLog.outDebug("SmartScript::ProcessAction: Entry %d SourceType %u, Event %u, Link Event %u not found or invalid, skipped.", e.entryOrGuid, e.GetScriptType(), e.event_id, e.link);
    }
}

void SmartScript::ProcessTimedAction(SmartScriptHolder& e, uint32 const& min, uint32 const& max, Unit* unit, uint32 var0, uint32 var1, bool bvar, const SpellEntry* spell, GameObject* gob)
{
    ConditionList const conds = sConditionMgr.GetConditionsForSmartEvent(e.entryOrGuid, e.event_id, e.source_type);
    ConditionSourceInfo info = ConditionSourceInfo(unit, GetBaseObject());

    if (sConditionMgr.IsObjectMeetToConditions(info, conds))
        ProcessAction(e, unit, var0, var1, bvar, spell, gob);

    RecalcTimer(e, min, max);
}

void SmartScript::InstallTemplate(SmartScriptHolder const& e)
{
    if (!GetBaseObject())
        return;
    if (mTemplate)
    {
        sLog.outError("SmartScript::InstallTemplate: Entry %d SourceType %u AI Template can not be set more then once, skipped.", e.entryOrGuid, e.GetScriptType());
        return;
    }
    mTemplate = (SMARTAI_TEMPLATE)e.action.installTtemplate.id;
    switch ((SMARTAI_TEMPLATE)e.action.installTtemplate.id)
    {
        case SMARTAI_TEMPLATE_CASTER:
            {
                AddEvent(SMART_EVENT_UPDATE_IC, 0, 0, 0, e.action.installTtemplate.param2, e.action.installTtemplate.param3, 0, SMART_ACTION_CAST, e.action.installTtemplate.param1, e.target.raw.param1, 0, 0, 0, 0, SMART_TARGET_VICTIM, 0, 0, 0, 1);
                AddEvent(SMART_EVENT_RANGE, 0, e.action.installTtemplate.param4, 300, 0, 0, 0, SMART_ACTION_ALLOW_COMBAT_MOVEMENT, 1, 0, 0, 0, 0, 0, SMART_TARGET_NONE, 0, 0, 0, 1);
                AddEvent(SMART_EVENT_RANGE, 0, 0, e.action.installTtemplate.param4>10?e.action.installTtemplate.param4-10:0, 0, 0, 0, SMART_ACTION_ALLOW_COMBAT_MOVEMENT, 0, 0, 0, 0, 0, 0, SMART_TARGET_NONE, 0, 0, 0, 1);
                AddEvent(SMART_EVENT_MANA_PCT, 0, e.action.installTtemplate.param5-15>100?100:e.action.installTtemplate.param5+15, 100, 1000, 1000, 0, SMART_ACTION_SET_EVENT_PHASE, 1, 0, 0, 0, 0, 0, SMART_TARGET_NONE, 0, 0, 0, 0);
                AddEvent(SMART_EVENT_MANA_PCT, 0, 0, e.action.installTtemplate.param5, 1000, 1000, 0, SMART_ACTION_SET_EVENT_PHASE, 0, 0, 0, 0, 0, 0, SMART_TARGET_NONE, 0, 0, 0, 0);
                AddEvent(SMART_EVENT_MANA_PCT, 0, 0, e.action.installTtemplate.param5, 1000, 1000, 0, SMART_ACTION_ALLOW_COMBAT_MOVEMENT, 1, 0, 0, 0, 0, 0, SMART_TARGET_NONE, 0, 0, 0, 0);
                break;
            }
        case SMARTAI_TEMPLATE_TURRET:
            {
                AddEvent(SMART_EVENT_UPDATE_IC, 0, 0, 0, e.action.installTtemplate.param2, e.action.installTtemplate.param3, 0, SMART_ACTION_CAST, e.action.installTtemplate.param1, e.target.raw.param1, 0, 0, 0, 0, SMART_TARGET_VICTIM, 0, 0, 0, 0);
                AddEvent(SMART_EVENT_JUST_CREATED, 0, 0, 0, 0, 0, 0, SMART_ACTION_ALLOW_COMBAT_MOVEMENT, 0, 0, 0, 0, 0, 0, SMART_TARGET_NONE, 0, 0, 0, 0);
                break;
            }
        case SMARTAI_TEMPLATE_CAGED_NPC_PART:
            {
                if (!me)
                    return;
                //store cage as id1
                AddEvent(SMART_EVENT_DATA_SET, 0, 0, 0, 0, 0, 0, SMART_ACTION_STORE_TARGET_LIST, 1, 0, 0, 0, 0, 0, SMART_TARGET_CLOSEST_GAMEOBJECT, e.action.installTtemplate.param1, 10, 0, 0);

                 //reset(close) cage on hostage(me) respawn
                AddEvent(SMART_EVENT_UPDATE, SMART_EVENT_FLAG_NOT_REPEATABLE, 0, 0, 0, 0, 0, SMART_ACTION_RESET_GOBJECT, 0, 0, 0, 0, 0, 0, SMART_TARGET_GAMEOBJECT_DISTANCE, e.action.installTtemplate.param1, 5, 0, 0);

                AddEvent(SMART_EVENT_DATA_SET, 0, 0, 0, 0, 0, 0, SMART_ACTION_SET_RUN, e.action.installTtemplate.param3, 0, 0, 0, 0, 0, SMART_TARGET_NONE, 0, 0, 0, 0);
                AddEvent(SMART_EVENT_DATA_SET, 0, 0, 0, 0, 0, 0, SMART_ACTION_SET_EVENT_PHASE, 1, 0, 0, 0, 0, 0, SMART_TARGET_NONE, 0, 0, 0, 0);

                AddEvent(SMART_EVENT_UPDATE, SMART_EVENT_FLAG_NOT_REPEATABLE, 1000, 1000, 0, 0, 0, SMART_ACTION_MOVE_FORWARD, e.action.installTtemplate.param4, 0, 0, 0, 0, 0, SMART_TARGET_NONE, 0, 0, 0, 1);
                 //phase 1: give quest credit on movepoint reached
                AddEvent(SMART_EVENT_MOVEMENTINFORM, 0, POINT_MOTION_TYPE, SMART_RANDOM_POINT, 0, 0, 0, SMART_ACTION_SET_DATA, 0, 0, 0, 0, 0, 0, SMART_TARGET_STORED, 1, 0, 0, 1);
                //phase 1: despawn after time on movepoint reached
                AddEvent(SMART_EVENT_MOVEMENTINFORM, 0, POINT_MOTION_TYPE, SMART_RANDOM_POINT, 0, 0, 0, SMART_ACTION_FORCE_DESPAWN, e.action.installTtemplate.param2, 0, 0, 0, 0, 0, SMART_TARGET_NONE, 0, 0, 0, 1);

                if (sCreatureTextMgr->TextExist(me->GetEntry(), (uint8)e.action.installTtemplate.param5))
                    AddEvent(SMART_EVENT_MOVEMENTINFORM, 0, POINT_MOTION_TYPE, SMART_RANDOM_POINT, 0, 0, 0, SMART_ACTION_TALK, e.action.installTtemplate.param5, 0, 0, 0, 0, 0, SMART_TARGET_NONE, 0, 0, 0, 1);
                break;
            }
        case SMARTAI_TEMPLATE_CAGED_GO_PART:
            {
                if (!go)
                    return;
                //store hostage as id1
                AddEvent(SMART_EVENT_GO_STATE_CHANGED, 0, 2, 0, 0, 0, 0, SMART_ACTION_STORE_TARGET_LIST, 1, 0, 0, 0, 0, 0, SMART_TARGET_CLOSEST_CREATURE, e.action.installTtemplate.param1, 10, 0, 0);
                //store invoker as id2
                AddEvent(SMART_EVENT_GO_STATE_CHANGED, 0, 2, 0, 0, 0, 0, SMART_ACTION_STORE_TARGET_LIST, 2, 0, 0, 0, 0, 0, SMART_TARGET_NONE, 0, 0, 0, 0);
                //signal hostage
                AddEvent(SMART_EVENT_GO_STATE_CHANGED, 0, 2, 0, 0, 0, 0, SMART_ACTION_SET_DATA, 0, 0, 0, 0, 0, 0, SMART_TARGET_STORED, 1, 0, 0, 0);
                //when hostage raeched end point, give credit to invoker
                if (e.action.installTtemplate.param2)
                    AddEvent(SMART_EVENT_DATA_SET, 0, 0, 0, 0, 0, 0, SMART_ACTION_CALL_KILLEDMONSTER, e.action.installTtemplate.param1, 0, 0, 0, 0, 0, SMART_TARGET_STORED, 2, 0, 0, 0);
                else
                    AddEvent(SMART_EVENT_GO_STATE_CHANGED, 0, 2, 0, 0, 0, 0, SMART_ACTION_CALL_KILLEDMONSTER, e.action.installTtemplate.param1, 0, 0, 0, 0, 0, SMART_TARGET_STORED, 2, 0, 0, 0);
                break;
            }
        case SMARTAI_TEMPLATE_BASIC:
        default:
            return;
    }
}

void SmartScript::AddEvent(SMART_EVENT e, uint32 event_flags, uint32 event_param1, uint32 event_param2, uint32 event_param3, uint32 event_param4, uint32 event_param5, SMART_ACTION action, uint32 action_param1, uint32 action_param2, uint32 action_param3, uint32 action_param4, uint32 action_param5, uint32 action_param6, SMARTAI_TARGETS t, uint32 target_param1, uint32 target_param2, uint32 target_param3, uint32 phaseMask)
{
    mInstallEvents.push_back(CreateEvent(e, event_flags, event_param1, event_param2, event_param3, event_param4, event_param5, action, action_param1, action_param2, action_param3, action_param4, action_param5, action_param6, t, target_param1, target_param2, target_param3, phaseMask));
}

SmartScriptHolder SmartScript::CreateEvent(SMART_EVENT e, uint32 event_flags, uint32 event_param1, uint32 event_param2, uint32 event_param3, uint32 event_param4, uint32 event_param5, SMART_ACTION action, uint32 action_param1, uint32 action_param2, uint32 action_param3, uint32 action_param4, uint32 action_param5, uint32 action_param6, SMARTAI_TARGETS t, uint32 target_param1, uint32 target_param2, uint32 target_param3, uint32 phaseMask)
{
    SmartScriptHolder script;
    script.event.type = e;
    script.event.raw.param1 = event_param1;
    script.event.raw.param2 = event_param2;
    script.event.raw.param3 = event_param3;
    script.event.raw.param4 = event_param4;
    script.event.raw.param5 = event_param5;
    script.event.event_phase_mask = phaseMask;
    script.event.event_flags = event_flags;
    script.event.event_chance = 100;

    script.action.type = action;
    script.action.raw.param1 = action_param1;
    script.action.raw.param2 = action_param2;
    script.action.raw.param3 = action_param3;
    script.action.raw.param4 = action_param4;
    script.action.raw.param5 = action_param5;
    script.action.raw.param6 = action_param6;

    script.target.type = t;
    script.target.raw.param1 = target_param1;
    script.target.raw.param2 = target_param2;
    script.target.raw.param3 = target_param3;

    script.source_type = SMART_SCRIPT_TYPE_CREATURE;
    InitTimer(script);
    return script;
}

ObjectList* SmartScript::GetTargets(SmartScriptHolder const& e, Unit* invoker /*= NULL*/)
{
    Unit* scriptTrigger = NULL;
    if (invoker)
        scriptTrigger = invoker;
    else if (Unit* tempLastInvoker = GetLastInvoker())
        scriptTrigger = tempLastInvoker;

    WorldObject* baseObject = GetBaseObject();

    ObjectList* l = new ObjectList();
    switch (e.GetTargetType())
    {
        case SMART_TARGET_SELF:
            if (baseObject)
                l->push_back(baseObject);
            break;
        case SMART_TARGET_VICTIM:
            if (me)
                if (Unit* victim = me->GetVictim())
                    l->push_back(victim);
            break;
        case SMART_TARGET_HOSTILE_SECOND_AGGRO:
            if (me)
                if (Unit* u = me->AI()->SelectTarget(SELECT_TARGET_TOPAGGRO, 1))
                    l->push_back(u);
            break;
        case SMART_TARGET_HOSTILE_LAST_AGGRO:
            if (me)
                if (Unit* u = me->AI()->SelectTarget(SELECT_TARGET_BOTTOMAGGRO, 0))
                    l->push_back(u);
            break;
        case SMART_TARGET_HOSTILE_RANDOM:
            if (me)
                if (Unit* u = me->AI()->SelectTarget(SELECT_TARGET_RANDOM, 0))
                    l->push_back(u);
            break;
        case SMART_TARGET_HOSTILE_RANDOM_NOT_TOP:
            if (me)
                if (Unit* u = me->AI()->SelectTarget(SELECT_TARGET_RANDOM, 1))
                    l->push_back(u);
            break;
        case SMART_TARGET_ACTION_INVOKER:
            if (scriptTrigger)
                l->push_back(scriptTrigger);
            break;
        case SMART_TARGET_ACTION_INVOKER_VEHICLE:
            // no exist in tbc
            break;
        case SMART_TARGET_INVOKER_PARTY:
            if (scriptTrigger)
            {
                if (Player* player = scriptTrigger->ToPlayer())
                {
                    if (Group* group = player->GetGroup())
                    {
                        for (GroupReference* groupRef = group->GetFirstMember(); groupRef != NULL; groupRef = groupRef->next())
                            if (Player* member = groupRef->GetSource())
                                if (member->IsInMap(player))
                                    l->push_back(member);
                    }
                    // We still add the player to the list if there is no group. If we do
                    // this even if there is a group (thus the else-check), it will add the
                    // same player to the list twice. We don't want that to happen.
                    else
                        l->push_back(scriptTrigger);
                }
            }
            break;
        case SMART_TARGET_CREATURE_RANGE:
        {
            // will always return a valid pointer, even if empty list
            ObjectList* units = GetWorldObjectsInDist((float)e.target.unitRange.maxDist);
            for (ObjectList::const_iterator itr = units->begin(); itr != units->end(); ++itr)
            {
                if (!IsCreature(*itr))
                    continue;

                if (me && me->GetGUID() == (*itr)->GetGUID())
                    continue;

                if ((!e.target.unitRange.creature || (*itr)->ToCreature()->GetEntry() == e.target.unitRange.creature) && baseObject->IsInRange(*itr, float(e.target.unitRange.minDist), float(e.target.unitRange.maxDist)))
                    l->push_back(*itr);
            }

            delete units;
            break;
        }
        case SMART_TARGET_CREATURE_DISTANCE:
        {
            // will always return a valid pointer, even if empty list
            ObjectList* units = GetWorldObjectsInDist((float)e.target.unitDistance.dist);
            for (ObjectList::const_iterator itr = units->begin(); itr != units->end(); ++itr)
            {
                if (!IsCreature(*itr))
                    continue;

                if (me && me->GetGUID() == (*itr)->GetGUID())
                    continue;

                if (!e.target.unitDistance.creature || (*itr)->ToCreature()->GetEntry() == e.target.unitDistance.creature)
                    l->push_back(*itr);
            }

            delete units;
            break;
        }
        case SMART_TARGET_GAMEOBJECT_DISTANCE:
        {
            // will always return a valid pointer, even if empty list
            ObjectList* units = GetWorldObjectsInDist((float)e.target.goDistance.dist);
            for (ObjectList::const_iterator itr = units->begin(); itr != units->end(); ++itr)
            {
                if (!IsGameObject(*itr))
                    continue;

                if (go && go->GetGUID() == (*itr)->GetGUID())
                    continue;

                if (!e.target.goDistance.entry || (*itr)->ToGameObject()->GetEntry() == e.target.goDistance.entry)
                    l->push_back(*itr);
            }

            delete units;
            break;
        }
        case SMART_TARGET_GAMEOBJECT_RANGE:
        {
            // will always return a valid pointer, even if empty list
            ObjectList* units = GetWorldObjectsInDist((float)e.target.goRange.maxDist);
            for (ObjectList::const_iterator itr = units->begin(); itr != units->end(); ++itr)
            {
                if (!IsGameObject(*itr))
                    continue;

                if (go && go->GetGUID() == (*itr)->GetGUID())
                    continue;

                if ((!e.target.goRange.entry && (*itr)->ToGameObject()->GetEntry() == e.target.goRange.entry) && baseObject->IsInRange(*itr, float(e.target.goRange.minDist), float(e.target.goRange.maxDist)))
                    l->push_back(*itr);
            }

            delete units;
            break;
        }
        case SMART_TARGET_CREATURE_GUID:
        {
            if (!scriptTrigger && !baseObject)
            {
                sLog.outError("SMART_TARGET_CREATURE_GUID can not be used without invoker");
                break;
            }

            if (Creature* target = FindCreatureNear(scriptTrigger ? scriptTrigger : baseObject, e.target.unitGUID.dbGuid))
                if (!e.target.unitGUID.entry || target->GetEntry() == e.target.unitGUID.entry)
                    l->push_back(target);
            break;
        }
        case SMART_TARGET_GAMEOBJECT_GUID:
        {
            if (!scriptTrigger && !baseObject)
            {
                sLog.outError("SMART_TARGET_GAMEOBJECT_GUID can not be used without invoker");
                break;
            }

            if (GameObject* target = FindGameObjectNear(scriptTrigger ? scriptTrigger : baseObject, e.target.goGUID.dbGuid))
                if (!e.target.goGUID.entry || target->GetEntry() == e.target.goGUID.entry)
                    l->push_back(target);
            break;
        }
        case SMART_TARGET_PLAYER_RANGE:
        {
            // will always return a valid pointer, even if empty list
            ObjectList* units = GetWorldObjectsInDist((float)e.target.playerRange.maxDist);
            if (!units->empty() && baseObject)
                for (ObjectList::const_iterator itr = units->begin(); itr != units->end(); ++itr)
                    if (IsPlayer(*itr) && baseObject->IsInRange(*itr, (float)e.target.playerRange.minDist, (float)e.target.playerRange.maxDist))
                        l->push_back(*itr);

            delete units;
            break;
        }
        case SMART_TARGET_PLAYER_DISTANCE:
        {
            // will always return a valid pointer, even if empty list
            ObjectList* units = GetWorldObjectsInDist((float)e.target.playerDistance.dist);
            for (ObjectList::const_iterator itr = units->begin(); itr != units->end(); ++itr)
                if (IsPlayer(*itr))
                    l->push_back(*itr);

            delete units;
            break;
        }
        case SMART_TARGET_STORED:
        {
            ObjectListMap::iterator itr = mTargetStorage->find(e.target.stored.id);
            if (itr != mTargetStorage->end())
            {
                ObjectList* objectList = itr->second->GetObjectList();
                l->assign(objectList->begin(), objectList->end());
            }

            return l;
        }
        case SMART_TARGET_CLOSEST_CREATURE:
        {
            if (Creature* target = GetClosestCreatureWithEntry(baseObject, e.target.closest.entry, float(e.target.closest.dist ? e.target.closest.dist : 100), !e.target.closest.dead))
                l->push_back(target);
            break;
        }
        case SMART_TARGET_CLOSEST_GAMEOBJECT:
        {
            if (GameObject* target = GetClosestGameObjectWithEntry(baseObject, e.target.closest.entry, float(e.target.closest.dist ? e.target.closest.dist : 100)))
                l->push_back(target);
            break;
        }
        case SMART_TARGET_CLOSEST_PLAYER:
        {
            if (me)
                if (Player* target = me->SelectNearestPlayer(float(e.target.playerDistance.dist)))
                    l->push_back(target);
            break;
        }
        case SMART_TARGET_OWNER_OR_SUMMONER:
        {
            if (me)
            {
                ObjectGuid charmerOrOwnerGuid = me->GetCharmerOrOwnerGUID();

                if (!charmerOrOwnerGuid)
                    charmerOrOwnerGuid = me->GetCreatorGUID();

                if (Unit* owner = ObjectAccessor::GetUnit(*me, charmerOrOwnerGuid))
                    l->push_back(owner);
            }
            break;
        }
        case SMART_TARGET_THREAT_LIST:
        {
            if (me)
            {
                ThreatContainer::StorageType threatList = me->getThreatManager().getThreatList();
                for (ThreatContainer::StorageType::const_iterator i = threatList.begin(); i != threatList.end(); ++i)
                    if (Unit* temp = ObjectAccessor::GetUnit(*me, (*i)->getUnitGuid()))
                        l->push_back(temp);
            }
            break;
        }
        case SMART_TARGET_CLOSEST_ENEMY:
        {
            if (me)
                if (Unit* target = me->SelectNearestTarget(e.target.closestAttackable.maxDist, e.target.closestAttackable.playerOnly != 0))
                    l->push_back(target);
            break;
        }
        case SMART_TARGET_CLOSEST_FRIENDLY:
        {
            if (me)
                if (Unit* target = DoFindClosestFriendlyInRange(e.target.closestFriendly.maxDist, e.target.closestFriendly.playerOnly != 0))
                    l->push_back(target);
            break;
        }
        case SMART_TARGET_POSITION:
        case SMART_TARGET_NONE:
        default:
            break;
    }

    if (l->empty())
    {
        delete l;
        l = NULL;
    }

    return l;
}

ObjectList* SmartScript::GetWorldObjectsInDist(float dist)
{
    ObjectList* targets = new ObjectList();
    WorldObject* obj = GetBaseObject();

    if (obj)
    {
        Oregon::AllWorldObjectsInRange u_check(obj, dist);
        Oregon::WorldObjectListSearcher<Oregon::AllWorldObjectsInRange> searcher(obj, *targets, u_check);
        obj->VisitNearbyObject(dist, searcher);
    }
    return targets;
}

void SmartScript::ProcessEvent(SmartScriptHolder& e, Unit* unit, uint32 var0, uint32 var1, bool bvar, const SpellEntry* spell, GameObject* gob)
{
    if (!e.active && e.GetEventType() != SMART_EVENT_LINK)
        return;

    if ((e.event.event_phase_mask && !IsInPhase(e.event.event_phase_mask)) || ((e.event.event_flags & SMART_EVENT_FLAG_NOT_REPEATABLE) && e.runOnce))
        return;

    switch (e.GetEventType())
    {
        case SMART_EVENT_LINK://special handling
            ProcessAction(e, unit, var0, var1, bvar, spell, gob);
            break;
        //called from Update tick
        case SMART_EVENT_UPDATE:
            ProcessTimedAction(e, e.event.minMaxRepeat.repeatMin, e.event.minMaxRepeat.repeatMax);
            break;
        case SMART_EVENT_UPDATE_OOC:
            if (me && me->IsInCombat())
                return;
            ProcessTimedAction(e, e.event.minMaxRepeat.repeatMin, e.event.minMaxRepeat.repeatMax);
            break;
        case SMART_EVENT_UPDATE_IC:
            if (!me || !me->IsInCombat())
                return;
            ProcessTimedAction(e, e.event.minMaxRepeat.repeatMin, e.event.minMaxRepeat.repeatMax);
            break;
        case SMART_EVENT_HEALT_PCT:
        {
            if (!me || !me->IsInCombat() || !me->GetMaxHealth())
                return;
            uint32 perc = (uint32)me->GetHealthPct();
            if (perc > e.event.minMaxRepeat.max || perc < e.event.minMaxRepeat.min)
                return;
            ProcessTimedAction(e, e.event.minMaxRepeat.repeatMin, e.event.minMaxRepeat.repeatMax);
            break;
        }
        case SMART_EVENT_TARGET_HEALTH_PCT:
        {
            if (!me || !me->IsInCombat() || !me->GetVictim() || !me->GetVictim()->GetMaxHealth())
                return;
            uint32 perc = (uint32)me->GetVictim()->GetHealthPct();
            if (perc > e.event.minMaxRepeat.max || perc < e.event.minMaxRepeat.min)
                return;
            ProcessTimedAction(e, e.event.minMaxRepeat.repeatMin, e.event.minMaxRepeat.repeatMax, me->GetVictim());
            break;
        }
        case SMART_EVENT_MANA_PCT:
        {
            if (!me || !me->IsInCombat() || !me->GetMaxPower(POWER_MANA))
                return;
            uint32 perc = uint32(100.0f * me->GetPower(POWER_MANA) / me->GetMaxPower(POWER_MANA));
            if (perc > e.event.minMaxRepeat.max || perc < e.event.minMaxRepeat.min)
                return;
            ProcessTimedAction(e, e.event.minMaxRepeat.repeatMin, e.event.minMaxRepeat.repeatMax);
            break;
        }
        case SMART_EVENT_TARGET_MANA_PCT:
        {
            if (!me || !me->IsInCombat() || !me->GetVictim() || !me->GetVictim()->GetMaxPower(POWER_MANA))
                return;
            uint32 perc = uint32(100.0f * me->GetVictim()->GetPower(POWER_MANA) / me->GetVictim()->GetMaxPower(POWER_MANA));
            if (perc > e.event.minMaxRepeat.max || perc < e.event.minMaxRepeat.min)
                return;
            ProcessTimedAction(e, e.event.minMaxRepeat.repeatMin, e.event.minMaxRepeat.repeatMax, me->GetVictim());
            break;
        }
        case SMART_EVENT_RANGE:
        {
            if (!me || !me->IsInCombat() || !me->GetVictim())
                return;

            if (me->IsInRange(me->GetVictim(), (float)e.event.minMaxRepeat.min, (float)e.event.minMaxRepeat.max))
                ProcessTimedAction(e, e.event.minMaxRepeat.repeatMin, e.event.minMaxRepeat.repeatMax, me->GetVictim());
            break;
        }
        case SMART_EVENT_VICTIM_CASTING:
        {
            if (!me || !me->IsInCombat())
                return;

            Unit* victim = me->GetVictim();

            if (!victim || !victim->IsNonMeleeSpellCast(false, false, true))
                return;

            if (e.event.targetCasting.spellId > 0)
                if (Spell* currSpell = victim->GetCurrentSpell(CURRENT_GENERIC_SPELL))
                    if (currSpell->m_spellInfo->Id != e.event.targetCasting.spellId)
                        return;

            ProcessTimedAction(e, e.event.targetCasting.repeatMin, e.event.targetCasting.repeatMax, me->GetVictim());
            break;
        }
        case SMART_EVENT_FRIENDLY_HEALTH:
        {
            if (!me || !me->IsInCombat())
                return;

            Unit* target = DoSelectLowestHpFriendly((float)e.event.friendlyHealth.radius, e.event.friendlyHealth.hpDeficit);
            if (!target || !target->IsInCombat())
                return;
            ProcessTimedAction(e, e.event.friendlyHealth.repeatMin, e.event.friendlyHealth.repeatMax, target);
            break;
        }
        case SMART_EVENT_FRIENDLY_IS_CC:
        {
            if (!me || !me->IsInCombat())
                return;

            std::list<Creature*> pList;
            DoFindFriendlyCC(pList, (float)e.event.friendlyCC.radius);
            if (pList.empty())
                return;
            ProcessTimedAction(e, e.event.friendlyCC.repeatMin, e.event.friendlyCC.repeatMax, *pList.begin());
            break;
        }
        case SMART_EVENT_FRIENDLY_MISSING_BUFF:
        {
            std::list<Creature*> pList;
            DoFindFriendlyMissingBuff(pList, (float)e.event.missingBuff.radius, e.event.missingBuff.spell);

            if (pList.empty())
                return;

            ProcessTimedAction(e, e.event.missingBuff.repeatMin, e.event.missingBuff.repeatMax, *pList.begin());
            break;
        }
        case SMART_EVENT_HAS_AURA:
        {
            if (!me)
                return;
            uint32 count = me->GetAuraCount(e.event.aura.spell);
            if ((!e.event.aura.count && !count) || (e.event.aura.count && count >= e.event.aura.count))
                ProcessTimedAction(e, e.event.aura.repeatMin, e.event.aura.repeatMax);
            break;
        }
        case SMART_EVENT_TARGET_BUFFED:
        {
            if (!me || !me->GetVictim())
                return;
            uint32 count = me->GetVictim()->GetAuraCount(e.event.aura.spell);
            if (count < e.event.aura.count)
                return;
            ProcessTimedAction(e, e.event.aura.repeatMin, e.event.aura.repeatMax);
            break;
        }
        //no params
        case SMART_EVENT_AGGRO:
        case SMART_EVENT_DEATH:
        case SMART_EVENT_EVADE:
        case SMART_EVENT_REACHED_HOME:
        case SMART_EVENT_CHARMED:
        case SMART_EVENT_CHARMED_TARGET:
        case SMART_EVENT_CORPSE_REMOVED:
        case SMART_EVENT_AI_INIT:
        case SMART_EVENT_TRANSPORT_ADDPLAYER:
        case SMART_EVENT_TRANSPORT_REMOVE_PLAYER:
        case SMART_EVENT_QUEST_ACCEPTED:
        case SMART_EVENT_QUEST_OBJ_COPLETETION:
        case SMART_EVENT_QUEST_COMPLETION:
        case SMART_EVENT_QUEST_REWARDED:
        case SMART_EVENT_QUEST_FAIL:
        case SMART_EVENT_JUST_SUMMONED:
        case SMART_EVENT_RESET:
        case SMART_EVENT_JUST_CREATED:
        case SMART_EVENT_GOSSIP_HELLO:
        case SMART_EVENT_FOLLOW_COMPLETED:
        case SMART_EVENT_ON_SPELLCLICK:
            ProcessAction(e, unit, var0, var1, bvar, spell, gob);
            break;
        case SMART_EVENT_IS_BEHIND_TARGET:
            {
                if (!me)
                    return;

                if (Unit* victim = me->GetVictim())
                {
                    if (!victim->HasInArc(static_cast<float>(M_PI), me))
                        ProcessTimedAction(e, e.event.behindTarget.cooldownMin, e.event.behindTarget.cooldownMax, victim);
                }
                break;
            }
        case SMART_EVENT_RECEIVE_EMOTE:
            if (e.event.emote.emote == var0)
            {
                RecalcTimer(e, e.event.emote.cooldownMin, e.event.emote.cooldownMax);
                ProcessAction(e, unit);
            }
            break;
        case SMART_EVENT_KILL:
        {
            if (!me || !unit)
                return;
            if (e.event.kill.playerOnly && unit->GetTypeId() != TYPEID_PLAYER)
                return;
            if (e.event.kill.creature && unit->GetEntry() != e.event.kill.creature)
                return;
            RecalcTimer(e, e.event.kill.cooldownMin, e.event.kill.cooldownMax);
            ProcessAction(e, unit);
            break;
        }
        case SMART_EVENT_SPELLHIT_TARGET:
        case SMART_EVENT_SPELLHIT:
        {
            if (!spell)
                return;
            if ((!e.event.spellHit.spell || spell->Id == e.event.spellHit.spell) &&
                (!e.event.spellHit.school || (spell->SchoolMask & e.event.spellHit.school)))
                {
                    RecalcTimer(e, e.event.spellHit.cooldownMin, e.event.spellHit.cooldownMax);
                    ProcessAction(e, unit, 0, 0, bvar, spell);
                }
            break;
        }
        case SMART_EVENT_OOC_LOS:
        {
            if (!me || me->IsInCombat())
                return;
            //can trigger if closer than fMaxAllowedRange
            float range = (float)e.event.los.maxDist;

            //if range is ok and we are actually in LOS
            if (me->IsWithinDistInMap(unit, range) && me->IsWithinLOSInMap(unit))
            {
                //if friendly event&&who is not hostile OR hostile event&&who is hostile
                if ((e.event.los.noHostile && !me->IsHostileTo(unit)) ||
                    (!e.event.los.noHostile && me->IsHostileTo(unit)))
                {
                    RecalcTimer(e, e.event.los.cooldownMin, e.event.los.cooldownMax);
                    ProcessAction(e, unit);
                }
            }
            break;
        }
        case SMART_EVENT_IC_LOS:
        {
            if (!me || !me->IsInCombat())
                return;
            //can trigger if closer than fMaxAllowedRange
            float range = (float)e.event.los.maxDist;

            //if range is ok and we are actually in LOS
            if (me->IsWithinDistInMap(unit, range) && me->IsWithinLOSInMap(unit))
            {
                //if friendly event&&who is not hostile OR hostile event&&who is hostile
                if ((e.event.los.noHostile && !me->IsHostileTo(unit)) ||
                    (!e.event.los.noHostile && me->IsHostileTo(unit)))
                {
                    RecalcTimer(e, e.event.los.cooldownMin, e.event.los.cooldownMax);
                    ProcessAction(e, unit);
                }
            }
            break;
        }
        case SMART_EVENT_RESPAWN:
        {
            if (!GetBaseObject())
                return;
            if (e.event.respawn.type == SMART_SCRIPT_RESPAWN_CONDITION_MAP && GetBaseObject()->GetMapId() != e.event.respawn.map)
                return;
            if (e.event.respawn.type == SMART_SCRIPT_RESPAWN_CONDITION_AREA && GetBaseObject()->GetZoneId() != e.event.respawn.area)
                return;
            ProcessAction(e);
            break;
        }
        case SMART_EVENT_SUMMONED_UNIT:
        {
            if (!IsCreature(unit))
                return;
            if (e.event.summoned.creature && unit->GetEntry() != e.event.summoned.creature)
                return;
            RecalcTimer(e, e.event.summoned.cooldownMin, e.event.summoned.cooldownMax);
            ProcessAction(e, unit);
            break;
        }
        case SMART_EVENT_RECEIVE_HEAL:
        case SMART_EVENT_DAMAGED:
        case SMART_EVENT_DAMAGED_TARGET:
        {
            if (var0 > e.event.minMaxRepeat.max || var0 < e.event.minMaxRepeat.min)
                return;
            RecalcTimer(e, e.event.minMaxRepeat.repeatMin, e.event.minMaxRepeat.repeatMax);
            ProcessAction(e, unit);
            break;
        }
        case SMART_EVENT_MOVEMENTINFORM:
        {
            if ((e.event.movementInform.type && var0 != e.event.movementInform.type) || (e.event.movementInform.id && var1 != e.event.movementInform.id))
                return;
            ProcessAction(e, unit, var0, var1);
            break;
        }
        case SMART_EVENT_TRANSPORT_RELOCATE:
        case SMART_EVENT_WAYPOINT_START:
        {
            if (e.event.waypoint.pathID && var0 != e.event.waypoint.pathID)
                return;
            ProcessAction(e, unit, var0);
            break;
        }
        case SMART_EVENT_WAYPOINT_REACHED:
        case SMART_EVENT_WAYPOINT_RESUMED:
        case SMART_EVENT_WAYPOINT_PAUSED:
        case SMART_EVENT_WAYPOINT_STOPPED:
        case SMART_EVENT_WAYPOINT_ENDED:
        {
            if (!me || (e.event.waypoint.pointID && var0 != e.event.waypoint.pointID) || (e.event.waypoint.pathID && GetPathId() != e.event.waypoint.pathID))
                return;
            ProcessAction(e, unit);
            break;
        }
        case SMART_EVENT_SUMMON_DESPAWNED:
        case SMART_EVENT_INSTANCE_PLAYER_ENTER:
        {
            if (e.event.instancePlayerEnter.team && var0 != e.event.instancePlayerEnter.team)
                return;
            RecalcTimer(e, e.event.instancePlayerEnter.cooldownMin, e.event.instancePlayerEnter.cooldownMax);
            ProcessAction(e, unit, var0);
            break;
        }
        case SMART_EVENT_ACCEPTED_QUEST:
        case SMART_EVENT_REWARD_QUEST:
        {
            if (e.event.quest.quest && var0 != e.event.quest.quest)
                return;
            ProcessAction(e, unit, var0);
            break;
        }
        case SMART_EVENT_TRANSPORT_ADDCREATURE:
        {
            if (e.event.transportAddCreature.creature && var0 != e.event.transportAddCreature.creature)
                return;
            ProcessAction(e, unit, var0);
            break;
        }
        case SMART_EVENT_AREATRIGGER_ONTRIGGER:
        {
            if (e.event.areatrigger.id && var0 != e.event.areatrigger.id)
                return;
            ProcessAction(e, unit, var0);
            break;
        }
        case SMART_EVENT_TEXT_OVER:
        {
            if (var0 != e.event.textOver.textGroupID || (e.event.textOver.creatureEntry && e.event.textOver.creatureEntry != var1))
                return;
            ProcessAction(e, unit, var0);
            break;
        }
        case SMART_EVENT_DATA_SET:
        {
            if (e.event.dataSet.id != var0 || e.event.dataSet.value != var1)
                return;
            RecalcTimer(e, e.event.dataSet.cooldownMin, e.event.dataSet.cooldownMax);
            ProcessAction(e, unit, var0, var1);
            break;
        }
        case SMART_EVENT_PASSENGER_REMOVED:
        case SMART_EVENT_PASSENGER_BOARDED:
        {
            if (!unit)
                return;
            RecalcTimer(e, e.event.minMax.repeatMin, e.event.minMax.repeatMax);
            ProcessAction(e, unit);
            break;
        }
        case SMART_EVENT_TIMED_EVENT_TRIGGERED:
        {
            if (e.event.timedEvent.id == var0)
                ProcessAction(e, unit);
            break;
        }
        case SMART_EVENT_GOSSIP_SELECT:
        {
            sLog.outDebug("SmartScript: Gossip Select:  menu %u action %u", var0, var1);//little help for scripters
            if (e.event.gossip.sender != var0 || e.event.gossip.action != var1)
                return;
            ProcessAction(e, unit, var0, var1);
            break;
        }
        case SMART_EVENT_DUMMY_EFFECT:
        {
            if (e.event.dummy.spell != var0 || e.event.dummy.effIndex != var1)
                return;
            ProcessAction(e, unit, var0, var1);
            break;
        }
        case SMART_EVENT_GAME_EVENT_START:
        case SMART_EVENT_GAME_EVENT_END:
        {
            if (e.event.gameEvent.gameEventId != var0)
                return;
            ProcessAction(e, NULL, var0);
            break;
        }
        case SMART_EVENT_GO_STATE_CHANGED:
        {
            if (e.event.goStateChanged.state != var0)
                return;
            ProcessAction(e, unit, var0, var1);
            break;
        }
        case SMART_EVENT_GO_EVENT_INFORM:
        {
            if (e.event.eventInform.eventId != var0)
                return;
            ProcessAction(e, NULL, var0);
            break;
        }
        case SMART_EVENT_ACTION_DONE:
        {
            if (e.event.doAction.eventId != var0)
                return;
            ProcessAction(e, unit, var0);
            break;
        }
        case SMART_EVENT_FRIENDLY_HEALTH_PCT:
        {
            if (!me || !me->IsInCombat())
                return;

            ObjectList* _targets = NULL;

            switch (e.GetTargetType())
            {
                case SMART_TARGET_CREATURE_RANGE:
                case SMART_TARGET_CREATURE_GUID:
                case SMART_TARGET_CREATURE_DISTANCE:
                case SMART_TARGET_CLOSEST_CREATURE:
                case SMART_TARGET_CLOSEST_PLAYER:
                case SMART_TARGET_PLAYER_RANGE:
                case SMART_TARGET_PLAYER_DISTANCE:
                    _targets = GetTargets(e);
                    break;
                default:
                    return;
            }

            if (!_targets)
                return;

            Unit* target = NULL;

            for (ObjectList::const_iterator itr = _targets->begin(); itr != _targets->end(); ++itr)
            {
                if (IsUnit(*itr) && me->IsFriendlyTo((*itr)->ToUnit()) && (*itr)->ToUnit()->IsAlive() && (*itr)->ToUnit()->IsInCombat())
                {
                    uint32 healthPct = uint32((*itr)->ToUnit()->GetHealthPct());

                    if (healthPct > e.event.friendlyHealthPct.maxHpPct || healthPct < e.event.friendlyHealthPct.minHpPct)
                        continue;

                    target = (*itr)->ToUnit();
                    break;
                }
            }

            delete _targets;

            if (!target)
                return;

            ProcessTimedAction(e, e.event.friendlyHealthPct.repeatMin, e.event.friendlyHealthPct.repeatMax, target);
            break;
        }
        case SMART_EVENT_DISTANCE_CREATURE:
        {
            if (!me)
                return;

            WorldObject* creature = NULL;

            if (e.event.distance.guid != 0)
            {
                creature = FindCreatureNear(me, e.event.distance.guid);

                if (!creature)
                    return;

                if (!me->IsInRange(creature, 0, (float)e.event.distance.dist))
                    return;
            }
            else if (e.event.distance.entry != 0)
            {
                std::list<Creature*> list;
                me->GetCreatureListWithEntryInGrid(list, e.event.distance.entry, (float)e.event.distance.dist);

                if (!list.empty())
                    creature = list.front();
            }

            if (creature)
                ProcessTimedAction(e, e.event.distance.repeat, e.event.distance.repeat);

            break;
        }
        case SMART_EVENT_DISTANCE_GAMEOBJECT:
        {
            if (!me)
                return;

            WorldObject* gameobject = NULL;

            if (e.event.distance.guid != 0)
            {
                gameobject = FindGameObjectNear(me, e.event.distance.guid);

                if (!gameobject)
                    return;

                if (!me->IsInRange(gameobject, 0, (float)e.event.distance.dist))
                    return;
            }
            else if (e.event.distance.entry != 0)
            {
                std::list<GameObject*> list;
                me->GetGameObjectListWithEntryInGrid(list, e.event.distance.entry, (float)e.event.distance.dist);

                if (!list.empty())
                    gameobject = list.front();
            }

            if (gameobject)
                ProcessTimedAction(e, e.event.distance.repeat, e.event.distance.repeat);

            break;
        }
        case SMART_EVENT_COUNTER_SET:
            if (GetCounterId(e.event.counter.id) != 0 && GetCounterValue(e.event.counter.id) == e.event.counter.value)
                ProcessTimedAction(e, e.event.counter.cooldownMin, e.event.counter.cooldownMax);
            break;
        default:
            sLog.outError("SmartScript::ProcessEvent: Unhandled Event type %u", e.GetEventType());
            break;
    }
}

void SmartScript::InitTimer(SmartScriptHolder& e)
{
    switch (e.GetEventType())
    {
        //set only events which have initial timers
        case SMART_EVENT_UPDATE:
        case SMART_EVENT_UPDATE_IC:
        case SMART_EVENT_UPDATE_OOC:
            RecalcTimer(e, e.event.minMaxRepeat.min, e.event.minMaxRepeat.max);
            break;
        case SMART_EVENT_DISTANCE_CREATURE:
        case SMART_EVENT_DISTANCE_GAMEOBJECT:
            RecalcTimer(e, e.event.distance.repeat, e.event.distance.repeat);
            break;
        default:
            e.active = true;
            break;
    }
}
void SmartScript::RecalcTimer(SmartScriptHolder& e, uint32 min, uint32 max)
{
    // min/max was checked at loading!
    e.timer = urand(min, max);
    e.active = e.timer ? false : true;
}

void SmartScript::UpdateTimer(SmartScriptHolder& e, uint32 const diff)
{
    if (e.GetEventType() == SMART_EVENT_LINK)
        return;

    if (e.event.event_phase_mask && !IsInPhase(e.event.event_phase_mask))
        return;

    if (e.GetEventType() == SMART_EVENT_UPDATE_IC && (!me || !me->IsInCombat()))
        return;

    if (e.GetEventType() == SMART_EVENT_UPDATE_OOC && (me && me->IsInCombat())) //can be used with me=NULL (go script)
        return;

    if (e.timer < diff)
    {
        // delay spell cast event if another spell is being cast
        if (e.GetActionType() == SMART_ACTION_CAST)
        {
            if (!(e.action.cast.flags & SMARTCAST_INTERRUPT_PREVIOUS))
            {
                if (me && me->HasUnitState(UNIT_STATE_CASTING))
                {
                    e.timer = 1;
                    return;
                }
            }
        }

        // Delay flee for assist event if stunned or rooted
        if (e.GetActionType() == SMART_ACTION_FLEE_FOR_ASSIST)
        {
            if (me && me->HasUnitState(UNIT_STATE_ROOT | UNIT_STATE_STUNNED))
            {
                e.timer = 1;
                return;
            }
        }

        e.active = true;//activate events with cooldown
        switch (e.GetEventType())//process ONLY timed events
        {
            case SMART_EVENT_UPDATE:
            case SMART_EVENT_UPDATE_OOC:
            case SMART_EVENT_UPDATE_IC:
            case SMART_EVENT_HEALT_PCT:
            case SMART_EVENT_TARGET_HEALTH_PCT:
            case SMART_EVENT_MANA_PCT:
            case SMART_EVENT_TARGET_MANA_PCT:
            case SMART_EVENT_RANGE:
            case SMART_EVENT_VICTIM_CASTING:
            case SMART_EVENT_FRIENDLY_HEALTH:
            case SMART_EVENT_FRIENDLY_IS_CC:
            case SMART_EVENT_FRIENDLY_MISSING_BUFF:
            case SMART_EVENT_HAS_AURA:
            case SMART_EVENT_TARGET_BUFFED:
            case SMART_EVENT_IS_BEHIND_TARGET:
            case SMART_EVENT_FRIENDLY_HEALTH_PCT:
            case SMART_EVENT_DISTANCE_CREATURE:
            case SMART_EVENT_DISTANCE_GAMEOBJECT:
            {
                ProcessEvent(e);
                if (e.GetScriptType() == SMART_SCRIPT_TYPE_TIMED_ACTIONLIST)
                {
                    e.enableTimed = false;//disable event if it is in an ActionList and was processed once
                    for (SmartAIEventList::iterator i = mTimedActionList.begin(); i != mTimedActionList.end(); ++i)
                    {
                        //find the first event which is not the current one and enable it
                        if (i->event_id > e.event_id)
                        {
                            i->enableTimed = true;
                            break;
                        }
                    }
                }
                break;
            }
        }
    }
    else
        e.timer -= diff;
}

bool SmartScript::CheckTimer(SmartScriptHolder const& e) const
{
    return e.active;
}

void SmartScript::InstallEvents()
{
    if (!mInstallEvents.empty())
    {
        for (SmartAIEventList::iterator i = mInstallEvents.begin(); i != mInstallEvents.end(); ++i)
            mEvents.push_back(*i);//must be before UpdateTimers

        mInstallEvents.clear();
    }
}

void SmartScript::OnUpdate(uint32 const diff)
{
    if ((mScriptType == SMART_SCRIPT_TYPE_CREATURE || mScriptType == SMART_SCRIPT_TYPE_GAMEOBJECT) && !GetBaseObject())
        return;

    InstallEvents();//before UpdateTimers

    for (SmartAIEventList::iterator i = mEvents.begin(); i != mEvents.end(); ++i)
        UpdateTimer(*i, diff);

    if (!mStoredEvents.empty())
        for (SmartAIEventList::iterator i = mStoredEvents.begin(); i != mStoredEvents.end(); ++i)
             UpdateTimer(*i, diff);

    bool needCleanup = true;
    if (!mTimedActionList.empty())
    {
        isProcessingTimedActionList = true;
        for (SmartAIEventList::iterator i = mTimedActionList.begin(); i != mTimedActionList.end(); ++i)
        {
            if ((*i).enableTimed)
            {
                UpdateTimer(*i, diff);
                needCleanup = false;
            }
        }

        isProcessingTimedActionList = false;
    }
    if (needCleanup)
        mTimedActionList.clear();

    if (!mRemIDs.empty())
    {
        for (std::list<uint32>::iterator i = mRemIDs.begin(); i != mRemIDs.end(); ++i)
        {
             RemoveStoredEvent((*i));
        }
    }
    if (mUseTextTimer && me)
    {
        if (mTextTimer < diff)
        {
            uint32 textID = mLastTextID;
            mLastTextID = 0;
            uint32 entry = mTalkerEntry;
            mTalkerEntry = 0;
            mTextTimer = 0;
            mUseTextTimer = false;
            ProcessEventsFor(SMART_EVENT_TEXT_OVER, NULL, textID, entry);
        } else mTextTimer -= diff;
    }
}

void SmartScript::FillScript(SmartAIEventList e, WorldObject* obj, AreaTriggerEntry const* at)
{
    if (e.empty())
    {
        if (obj)
            sLog.outDebug("SmartScript: EventMap for Entry %u is empty but is using SmartScript.", obj->GetEntry());
        if (at)
            sLog.outDebug("SmartScript: EventMap for AreaTrigger %u is empty but is using SmartScript.", at->id);
        return;
    }
    for (SmartAIEventList::iterator i = e.begin(); i != e.end(); ++i)
    {
        #ifndef TRINITY_DEBUG
            if ((*i).event.event_flags & SMART_EVENT_FLAG_DEBUG_ONLY)
                continue;
        #endif

        if ((*i).event.event_flags & SMART_EVENT_FLAG_DIFFICULTY_ALL)//if has instance flag add only if in it
        {
            if (obj && obj->GetMap()->IsDungeon())
            {
                if ((1 << (obj->GetMap()->GetSpawnMode()+1)) & (*i).event.event_flags)
                {
                    mEvents.push_back((*i));
                }
            }
            continue;
        }
        mEvents.push_back((*i));//NOTE: 'world(0)' events still get processed in ANY instance mode
    }
}

void SmartScript::GetScript()
{
    SmartAIEventList e;
    if (me)
    {
        e = sSmartScriptMgr->GetScript(-((int32)me->GetDBTableGUIDLow()), mScriptType);
        if (e.empty())
            e = sSmartScriptMgr->GetScript((int32)me->GetEntry(), mScriptType);
        FillScript(e, me, NULL);
    }
    else if (go)
    {
        e = sSmartScriptMgr->GetScript(-((int32)go->GetDBTableGUIDLow()), mScriptType);
        if (e.empty())
            e = sSmartScriptMgr->GetScript((int32)go->GetEntry(), mScriptType);
        FillScript(e, go, NULL);
    }
    else if (trigger)
    {
        e = sSmartScriptMgr->GetScript((int32)trigger->id, mScriptType);
        FillScript(e, NULL, trigger);
    }
}

void SmartScript::OnInitialize(WorldObject* obj, AreaTriggerEntry const* at)
{
    if (obj)//handle object based scripts
    {
        switch (obj->GetTypeId())
        {
            case TYPEID_UNIT:
                mScriptType = SMART_SCRIPT_TYPE_CREATURE;
                me = obj->ToCreature();
                sLog.outDebug("SmartScript::OnInitialize: source is Creature %u", me->GetEntry());
                break;
            case TYPEID_GAMEOBJECT:
                mScriptType = SMART_SCRIPT_TYPE_GAMEOBJECT;
                go = obj->ToGameObject();
                sLog.outDebug("SmartScript::OnInitialize: source is GameObject %u", go->GetEntry());
                break;
            default:
                sLog.outError("SmartScript::OnInitialize: Unhandled TypeID !WARNING!");
                return;
        }
    } else if (at)
    {
        mScriptType = SMART_SCRIPT_TYPE_AREATRIGGER;
        trigger = at;
        sLog.outDebug("SmartScript::OnInitialize: source is AreaTrigger %u", trigger->id);
    }
    else
    {
        sLog.outError("SmartScript::OnInitialize: !WARNING! Initialized objects are NULL.");
        return;
    }

    GetScript();//load copy of script

    for (SmartAIEventList::iterator i = mEvents.begin(); i != mEvents.end(); ++i)
        InitTimer((*i));//calculate timers for first time use

    ProcessEventsFor(SMART_EVENT_AI_INIT);
    InstallEvents();
    ProcessEventsFor(SMART_EVENT_JUST_CREATED);
}

void SmartScript::OnMoveInLineOfSight(Unit* who)
{
    if (!me)
        return;

    ProcessEventsFor(me->IsInCombat() ? SMART_EVENT_IC_LOS : SMART_EVENT_OOC_LOS, who);
}

/*
void SmartScript::UpdateAIWhileCharmed(const uint32 diff) { }

void SmartScript::DoAction(const int32 param) { }

uint32 SmartScript::GetData(uint32 id)
{
    return 0;
}

void SmartScript::SetData(uint32 id, uint32 value) { }

void SmartScript::SetGUID(uint64 guid, int32 id) { }

uint64 SmartScript::GetGUID(int32 id)
{
    return 0;
}

void SmartScript::MovepointStart(uint32 id) { }

void SmartScript::SetRun(bool run) { }

void SmartScript::SetMovePathEndAction(SMART_ACTION action) { }

uint32 SmartScript::DoChat(int8 id, uint64 whisperGuid)
{
    return 0;
}*/
// SmartScript end

Unit* SmartScript::DoSelectLowestHpFriendly(float range, uint32 MinHPDiff)
{
    if (!me)
        return NULL;

    CellCoord p(Oregon::ComputeCellCoord(me->GetPositionX(), me->GetPositionY()));
    Cell cell(p);
    cell.SetNoCreate();

    Unit* unit = NULL;

    Oregon::MostHPMissingInRange u_check(me, range, MinHPDiff);
    Oregon::UnitLastSearcher<Oregon::MostHPMissingInRange> searcher(me, unit, u_check);

    TypeContainerVisitor<Oregon::UnitLastSearcher<Oregon::MostHPMissingInRange>, GridTypeMapContainer >  grid_unit_searcher(searcher);

    cell.Visit(p, grid_unit_searcher, *me->GetMap(), *me, range);
    return unit;
}

void SmartScript::DoFindFriendlyCC(std::list<Creature*>& _list, float range)
{
    if (!me)
        return;

    CellCoord p(Oregon::ComputeCellCoord(me->GetPositionX(), me->GetPositionY()));
    Cell cell(p);
    cell.SetNoCreate();

    Oregon::FriendlyCCedInRange u_check(me, range);
    Oregon::CreatureListSearcher<Oregon::FriendlyCCedInRange> searcher(me, _list, u_check);

    TypeContainerVisitor<Oregon::CreatureListSearcher<Oregon::FriendlyCCedInRange>, GridTypeMapContainer >  grid_creature_searcher(searcher);

    cell.Visit(p, grid_creature_searcher, *me->GetMap(), *me, range);
}

void SmartScript::DoFindFriendlyMissingBuff(std::list<Creature*>& list, float range, uint32 spellid)
{
    if (!me)
        return;

    CellCoord p(Oregon::ComputeCellCoord(me->GetPositionX(), me->GetPositionY()));
    Cell cell(p);
    cell.SetNoCreate();

    Oregon::FriendlyMissingBuffInRange u_check(me, range, spellid);
    Oregon::CreatureListSearcher<Oregon::FriendlyMissingBuffInRange> searcher(me, list, u_check);

    TypeContainerVisitor<Oregon::CreatureListSearcher<Oregon::FriendlyMissingBuffInRange>, GridTypeMapContainer >  grid_creature_searcher(searcher);

    cell.Visit(p, grid_creature_searcher, *me->GetMap(), *me, range);
}

Unit* SmartScript::DoFindClosestFriendlyInRange(float range, bool playerOnly)
{
    if (!me)
        return NULL;

    Unit* unit = NULL;
    Oregon::AnyFriendlyUnitInObjectRangeCheck u_check(me, me, range, playerOnly);
    Oregon::UnitLastSearcher<Oregon::AnyFriendlyUnitInObjectRangeCheck> searcher(me, unit, u_check);
    me->VisitNearbyObject(range, searcher);
    return unit;
}

void SmartScript::SetScript9(SmartScriptHolder& e, uint32 entry)
{
    //do NOT clear mTimedActionList if it's being iterated because it will invalidate the iterator and delete
    // any SmartScriptHolder contained like the "e" parameter passed to this function
    if (isProcessingTimedActionList)
    {
        sLog.outError("Entry %d SourceType %u Event %u Action %u is trying to overwrite timed action list from a timed action, this is not allowed!.", e.entryOrGuid, e.GetScriptType(), e.GetEventType(), e.GetActionType());
        return;
    }

    mTimedActionList.clear();
    mTimedActionList = sSmartScriptMgr->GetScript(entry, SMART_SCRIPT_TYPE_TIMED_ACTIONLIST);
    if (mTimedActionList.empty())
        return;
    for (SmartAIEventList::iterator i = mTimedActionList.begin(); i != mTimedActionList.end(); ++i)
    {
        i->enableTimed = i == mTimedActionList.begin();//enable processing only for the first action

        if (e.action.timedActionList.timerType == 0)
            i->event.type = SMART_EVENT_UPDATE_OOC;
        else if (e.action.timedActionList.timerType == 1)
            i->event.type = SMART_EVENT_UPDATE_IC;
        else if (e.action.timedActionList.timerType > 1)
            i->event.type = SMART_EVENT_UPDATE;

        InitTimer((*i));
    }
}

Unit* SmartScript::GetLastInvoker()
{
    return ObjectAccessor::FindUnit(mLastInvoker);
}
