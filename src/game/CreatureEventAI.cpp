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
#include "CreatureEventAI.h"
#include "CreatureEventAIMgr.h"
#include "ObjectMgr.h"
#include "Spell.h"
#include "World.h"
#include "Cell.h"
#include "CellImpl.h"
#include "GameEventMgr.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "InstanceData.h"
#include "Spell.h"
#include "SpellMgr.h"
#include "CreatureAIImpl.h"
#include "ConditionMgr.h"

bool CreatureEventAIHolder::UpdateRepeatTimer(Creature* creature, uint32 repeatMin, uint32 repeatMax)
{
    if (repeatMin == repeatMax)
        Time = repeatMin;
    else if (repeatMax > repeatMin)
        Time = urand(repeatMin, repeatMax);
    else
    {
        sLog.outErrorDb("CreatureEventAI: Creature %u using Event %u (Type = %u) has RandomMax < RandomMin. Event repeating disabled.", creature->GetEntry(), Event.event_id, Event.event_type);
        Enabled = false;
        return false;
    }

    return true;
}

int CreatureEventAI::Permissible(const Creature* creature)
{
    if (creature->GetAIName() == "EventAI")
        return PERMIT_BASE_SPECIAL;
    return PERMIT_BASE_NO;
}

CreatureEventAI::CreatureEventAI(Creature* c) : CreatureAI(c)
{
    hasLosEvents = false;

    // Need make copy for filter unneeded steps and safe in case table reload
    CreatureEventAI_Event_Map::const_iterator CreatureEvents = CreatureEAI_Mgr.GetCreatureEventAIMap().find(me->GetEntry());
    if (CreatureEvents != CreatureEAI_Mgr.GetCreatureEventAIMap().end())
    {
        std::vector<CreatureEventAI_Event>::const_iterator i;
        for (i = (*CreatureEvents).second.begin(); i != (*CreatureEvents).second.end(); ++i)
        {

            //Debug check
            #ifndef OREGON_DEBUG
            if ((*i).event_flags & EFLAG_DEBUG_ONLY)
                continue;
            #endif
            if (((*i).event_flags & (EFLAG_HEROIC | EFLAG_NORMAL)) && me->GetMap()->IsDungeon())
            {
                if ((me->GetMap()->IsHeroic() && (*i).event_flags & EFLAG_HEROIC) ||
                    (!me->GetMap()->IsHeroic() && (*i).event_flags & EFLAG_NORMAL))
                {
                    //event flagged for instance mode
                    CreatureEventAIList.push_back(CreatureEventAIHolder(*i));

                    if ((*i).event_type == EVENT_T_OOC_LOS)
                        hasLosEvents = true;
                }
                continue;
            }
            CreatureEventAIList.push_back(CreatureEventAIHolder(*i));

            if ((*i).event_type == EVENT_T_OOC_LOS)
                hasLosEvents = true;
        }
        //EventMap had events but they were not added because they must be for instance
        if (CreatureEventAIList.empty())
            sLog.outError("CreatureEventAI: Creature %u has events but no events added to list because of instance flags.", me->GetEntry());
    }
    else
        sLog.outError("CreatureEventAI: EventMap for Creature %u is empty but creature is using CreatureEventAI.", me->GetEntry());

    bEmptyList = CreatureEventAIList.empty();
    Phase = 0;
    MeleeEnabled = true;

    InvinceabilityHpLevel = 0;

    //Handle Spawned Events
    if (!bEmptyList)
    {
        for (std::list<CreatureEventAIHolder>::iterator i = CreatureEventAIList.begin(); i != CreatureEventAIList.end(); ++i)
            if (SpawnedEventConditionsCheck((*i).Event))
                ProcessEvent(*i);
    }
}

bool CreatureEventAI::ProcessEvent(CreatureEventAIHolder& pHolder, Unit* pActionInvoker)
{
    if (!pHolder.Enabled || pHolder.Time)
        return false;

    //Check the inverse phase mask (event doesn't trigger if current phase bit is set in mask)
    if (pHolder.Event.event_inverse_phase_mask & (1 << Phase))
        return false;

    CreatureEventAI_Event const& event = pHolder.Event;

    //Check event conditions based on the event type, also reset events
    switch (event.event_type)
    {
    case EVENT_T_TIMER:
        if (!me->IsInCombat())
            return false;

        //Repeat Timers
        pHolder.UpdateRepeatTimer(me, event.timer.repeatMin, event.timer.repeatMax);
        break;
    case EVENT_T_TIMER_OOC:
        if (me->IsInCombat() || me->IsInEvadeMode())
            return false;

        //Repeat Timers
        pHolder.UpdateRepeatTimer(me, event.timer.repeatMin, event.timer.repeatMax);
        break;
    case EVENT_T_HP:
        {
            if (!me->IsInCombat() || !me->GetMaxHealth())
                return false;

            uint32 perc = (me->GetHealth() * 100) / me->GetMaxHealth();

            if (perc > event.percent_range.percentMax || perc < event.percent_range.percentMin)
                return false;

            //Repeat Timers
            pHolder.UpdateRepeatTimer(me, event.percent_range.repeatMin, event.percent_range.repeatMax);
            break;
        }
    case EVENT_T_MANA:
        {
            if (!me->IsInCombat() || !me->GetMaxPower(POWER_MANA))
                return false;

            uint32 perc = (me->GetPower(POWER_MANA) * 100) / me->GetMaxPower(POWER_MANA);

            if (perc > event.percent_range.percentMax || perc < event.percent_range.percentMin)
                return false;

            //Repeat Timers
            pHolder.UpdateRepeatTimer(me, event.percent_range.repeatMin, event.percent_range.repeatMax);
            break;
        }
    case EVENT_T_AGGRO:
        break;
    case EVENT_T_KILL:
        //Repeat Timers
        pHolder.UpdateRepeatTimer(me, event.kill.repeatMin, event.kill.repeatMax);
        break;
    case EVENT_T_DEATH:
    case EVENT_T_EVADE:
        break;
    case EVENT_T_SPELLHIT:
        //Spell hit is special case, param1 and param2 handled within CreatureEventAI::SpellHit

        //Repeat Timers
        pHolder.UpdateRepeatTimer(me, event.spell_hit.repeatMin, event.spell_hit.repeatMax);
        break;
    case EVENT_T_RANGE:
        //Repeat Timers
        pHolder.UpdateRepeatTimer(me, event.range.repeatMin, event.range.repeatMax);
        break;
    case EVENT_T_OOC_LOS:
        //Repeat Timers
        pHolder.UpdateRepeatTimer(me, event.ooc_los.repeatMin, event.ooc_los.repeatMax);
        break;
    case EVENT_T_SPAWNED:
        break;
    case EVENT_T_TARGET_HP:
        {
            if (!me->IsInCombat() || !me->GetVictim() || !me->GetVictim()->GetMaxHealth())
                return false;

            uint32 perc = (me->GetVictim()->GetHealth() * 100) / me->GetVictim()->GetMaxHealth();

            if (perc > event.percent_range.percentMax || perc < event.percent_range.percentMin)
                return false;

            //Repeat Timers
            pHolder.UpdateRepeatTimer(me, event.percent_range.repeatMin, event.percent_range.repeatMax);
            break;
        }
    case EVENT_T_TARGET_CASTING:
        if (!me->IsInCombat() || !me->GetVictim() || !me->GetVictim()->IsNonMeleeSpellCast(false, false, true))
            return false;

        //Repeat Timers
        pHolder.UpdateRepeatTimer(me, event.target_casting.repeatMin, event.target_casting.repeatMax);
        break;
    case EVENT_T_FRIENDLY_HP:
        {
            if (!me->IsInCombat())
                return false;

            Unit* pUnit = DoSelectLowestHpFriendly(event.friendly_hp.radius, event.friendly_hp.hpDeficit);
            if (!pUnit)
                return false;

            pActionInvoker = pUnit;

            //Repeat Timers
            pHolder.UpdateRepeatTimer(me, event.friendly_hp.repeatMin, event.friendly_hp.repeatMax);
            break;
        }
    case EVENT_T_FRIENDLY_IS_CC:
        {
            if (!me->IsInCombat())
                return false;

            std::list<Creature*> pList;
            DoFindFriendlyCC(pList, event.friendly_is_cc.radius);

            //List is empty
            if (pList.empty())
                return false;

            //We don't really care about the whole list, just return first available
            pActionInvoker = *(pList.begin());

            //Repeat Timers
            pHolder.UpdateRepeatTimer(me, event.friendly_is_cc.repeatMin, event.friendly_is_cc.repeatMax);
            break;
        }
    case EVENT_T_FRIENDLY_MISSING_BUFF:
        {
            std::list<Creature*> pList;
            DoFindFriendlyMissingBuff(pList, event.friendly_buff.radius, event.friendly_buff.spellId);

            //List is empty
            if (pList.empty())
                return false;

            //We don't really care about the whole list, just return first available
            pActionInvoker = *(pList.begin());

            //Repeat Timers
            pHolder.UpdateRepeatTimer(me, event.friendly_buff.repeatMin, event.friendly_buff.repeatMax);
            break;
        }
    case EVENT_T_SUMMONED_UNIT:
    case EVENT_T_SUMMONED_JUST_DIED:
    case EVENT_T_SUMMONED_JUST_DESPAWN:
        {
            //Prevent event from occuring on no unit or non creatures
            if (!pActionInvoker || pActionInvoker->GetTypeId() != TYPEID_UNIT)
                return false;

            //Creature id doesn't match up
            if (pActionInvoker->ToCreature()->GetEntry() != event.summon_unit.creatureId)
                return false;

            //Repeat Timers
            pHolder.UpdateRepeatTimer(me, event.summon_unit.repeatMin, event.summon_unit.repeatMax);
            break;
        }
    case EVENT_T_TARGET_MANA:
        {
            if (!me->IsInCombat() || !me->GetVictim() || !me->GetVictim()->GetMaxPower(POWER_MANA))
                return false;

            uint32 perc = (me->GetVictim()->GetPower(POWER_MANA) * 100) / me->GetVictim()->GetMaxPower(POWER_MANA);

            if (perc > event.percent_range.percentMax || perc < event.percent_range.percentMin)
                return false;

            //Repeat Timers
            pHolder.UpdateRepeatTimer(me, event.percent_range.repeatMin, event.percent_range.repeatMax);
            break;
        }
    case EVENT_T_REACHED_HOME:
    case EVENT_T_RECEIVE_EMOTE:
        break;
    case EVENT_T_BUFFED:
        {
            //Note: checked only aura for effect 0, if need check aura for effect 1/2 then
            // possible way: pack in event.buffed.amount 2 uint16 (ammount+effectIdx)
            Aura* aura = me->GetAura(event.buffed.spellId, 0);
            if (!aura || uint32(aura->GetStackAmount()) < event.buffed.amount)
                return false;

            //Repeat Timers
            pHolder.UpdateRepeatTimer(me, event.buffed.repeatMin, event.buffed.repeatMax);
            break;
        }
    case EVENT_T_TARGET_BUFFED:
        {
            //Prevent event from occuring on no unit
            if (!pActionInvoker)
                return false;

            //Note: checked only aura for effect 0, if need check aura for effect 1/2 then
            // possible way: pack in event.buffed.amount 2 uint16 (ammount+effectIdx)
            Aura* aura = pActionInvoker->GetAura(event.buffed.spellId, 0);
            if (!aura || uint32(aura->GetStackAmount()) < event.buffed.amount)
                return false;

            //Repeat Timers
            pHolder.UpdateRepeatTimer(me, event.buffed.repeatMin, event.buffed.repeatMax);
            break;
        }
    case EVENT_T_MISSING_AURA:
        {
            //Note: checked only aura for effect 0, if need check aura for effect 1/2 then
            // possible way: pack in event.buffed.amount 2 uint16 (ammount+effectIdx)
            Aura* aura = me->GetAura(event.buffed.spellId, 0);
            if (aura && uint32(aura->GetStackAmount()) >= event.buffed.amount)
                return false;

            //Repeat Timers
            pHolder.UpdateRepeatTimer(me, event.buffed.repeatMin, event.buffed.repeatMax);
            break;
        }
    case EVENT_T_TARGET_MISSING_AURA:
        {
            if (!me->IsInCombat() || !me->GetVictim())
                return false;

            Aura* aura = me->GetVictim()->GetAura(event.buffed.spellId, 0);
            if (aura && uint32(aura->GetStackAmount()) >= event.buffed.amount)
                return false;

            //Repeat Timers
            pHolder.UpdateRepeatTimer(me, event.buffed.repeatMin, event.buffed.repeatMax);
            break;
        }
    default:
        sLog.outErrorDb("CreatureEventAI: Creature %u using Event %u has invalid Event Type(%u), missing from ProcessEvent() Switch.", me->GetEntry(), pHolder.Event.event_id, pHolder.Event.event_type);
        break;
    }

    //Disable non-repeatable events
    if (!(pHolder.Event.event_flags & EFLAG_REPEATABLE))
        pHolder.Enabled = false;

    //Store random here so that all random actions match up
    uint32 rnd = rand();

    //Return if chance for event is not met
    if (pHolder.Event.event_chance <= rnd % 100)
        return false;

    //Process actions
    for (uint8 j = 0; j < MAX_ACTIONS; ++j)
        ProcessAction(pHolder.Event.action[j], rnd, pHolder.Event.event_id, pActionInvoker);

    return true;
}

void CreatureEventAI::ProcessAction(CreatureEventAI_Action const& action, uint32 rnd, uint32 EventId, Unit* pActionInvoker)
{
    switch (action.type)
    {
    case ACTION_T_TEXT:
    case ACTION_T_CHANCED_TEXT:
        {
            if (!action.text.TextId[0])
                return;

            int32 temp = 0;

            if (action.type == ACTION_T_TEXT)
            {
                if (action.text.TextId[1] && action.text.TextId[2])
                    temp = action.text.TextId[urand(0, 2)];
                else if (action.text.TextId[1] && urand(0, 1))
                    temp = action.text.TextId[1];
                else
                    temp = action.text.TextId[0];
            }
            // ACTION_T_CHANCED_TEXT, chance hits
            else if (urand(0, 99) < action.chanced_text.chance)
            {
                if (action.chanced_text.TextId[0] && action.chanced_text.TextId[1])
                    temp = action.chanced_text.TextId[urand(0, 1)];
                else
                    temp = action.chanced_text.TextId[0];
            }

            if (temp)
            {
                Unit* target = NULL;

                if (pActionInvoker)
                {
                    if (pActionInvoker->GetTypeId() == TYPEID_PLAYER)
                        target = pActionInvoker;
                    else if (Unit* owner = pActionInvoker->GetOwner())
                    {
                        if (owner->GetTypeId() == TYPEID_PLAYER)
                            target = owner;
                    }
                }
                else
                {
                    target = me->GetVictim();
                    if (target && target->GetTypeId() != TYPEID_PLAYER)
                        if (Unit* owner = target->GetOwner())
                            if (owner->GetTypeId() == TYPEID_PLAYER)
                                target = owner;
                }

                DoScriptText(temp, me, target);
            }
            break;
        }
    case ACTION_T_SET_FACTION:
        {
            if (action.set_faction.factionId)
                me->SetFaction(action.set_faction.factionId);
            else
            {
                if (CreatureInfo const* ci = GetCreatureTemplateStore(me->GetEntry()))
                {
                    //if no id provided, assume reset and then use default
                    if (me->GetFaction() != ci->faction)
                        me->SetFaction(ci->faction);
                }
            }
            break;
        }
    case ACTION_T_MORPH_TO_ENTRY_OR_MODEL:
        {
            if (action.morph.creatureId || action.morph.modelId)
            {
                //set model based on entry from creature_template
                if (action.morph.creatureId)
                {
                    if (CreatureInfo const* ci = GetCreatureTemplateStore(action.morph.creatureId))
                    {
                        uint32 display_id = sObjectMgr.ChooseDisplayId(0, ci);
                        me->SetDisplayId(display_id);
                    }
                }
                //if no param1, then use value from param2 (modelId)
                else
                    me->SetDisplayId(action.morph.modelId);
            }
            else
                me->DeMorph();
            break;
        }
    case ACTION_T_SOUND:
        me->SendPlaySound(action.sound.soundId, false);
        break;
    case ACTION_T_EMOTE:
        me->HandleEmoteCommand(action.emote.emoteId);
        break;
    case ACTION_T_RANDOM_SOUND:
        {
            int32 temp = GetRandActionParam(rnd, action.random_sound.soundId1, action.random_sound.soundId2, action.random_sound.soundId3);
            if (temp >= 0)
                me->SendPlaySound(temp, false);
            break;
        }
    case ACTION_T_RANDOM_EMOTE:
        {
            int32 temp = GetRandActionParam(rnd, action.random_emote.emoteId1, action.random_emote.emoteId2, action.random_emote.emoteId3);
            if (temp >= 0)
                me->HandleEmoteCommand(temp);
            break;
        }
    case ACTION_T_CAST:
        {
            Unit* target = GetTargetByType(action.cast.target, pActionInvoker);
            const SpellEntry* spell = sSpellStore.LookupEntry(action.cast.spellId);

            SpellCastResult castResult = CanCast(target, spell, action.cast.castFlags);

            switch (castResult)
            {
                case SPELL_FAILED_NO_POWER:
                case SPELL_FAILED_OUT_OF_RANGE:
                case SPELL_FAILED_LINE_OF_SIGHT:
                {
                    // Melee current victim if flag not set
                    if (!(action.cast.castFlags & CAST_NO_MELEE_IF_OOM))
                    {
                        switch (me->GetMotionMaster()->GetCurrentMovementGeneratorType())
                        {
                            case CHASE_MOTION_TYPE:
                            case FOLLOW_MOTION_TYPE:
                            case ASSISTANCE_MOTION_TYPE:
                            case IDLE_MOTION_TYPE:
                                SetCombatMovement(true);
                                break;
                            default:
                                break;
                        }
                    }
                    break;
                }
                case SPELL_CAST_OK:
                    /* @TODO: Correctly switch between melee/ranged states.
                    SetCombatMovement(false);*/
                default:
                    break;
            }

            break;
        }
    case ACTION_T_SUMMON:
        {
            Unit* target = GetTargetByType(action.summon.target, pActionInvoker);

            Creature* pCreature = NULL;

            if (action.summon.duration)
                pCreature = me->SummonCreature(action.summon.creatureId, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, action.summon.duration);
            else
                pCreature = me->SummonCreature(action.summon.creatureId, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 0);

            if (!pCreature)
                sLog.outErrorDb("CreatureEventAI: failed to spawn creature %u. Spawn event %d is on creature %d", action.summon.creatureId, EventId, me->GetEntry());
            else if (action.summon.target != TARGET_T_SELF && target)
                pCreature->AI()->AttackStart(target);
            break;
        }
    case ACTION_T_THREAT_SINGLE_PCT:
        if (Unit* target = GetTargetByType(action.threat_single_pct.target, pActionInvoker))
            me->getThreatManager().modifyThreatPercent(target, action.threat_single_pct.percent);
        break;
    case ACTION_T_THREAT_ALL_PCT:
        {
            ThreatContainer::StorageType const& threatList = me->getThreatManager().getThreatList();
            for (ThreatContainer::StorageType::const_iterator i = threatList.begin(); i != threatList.end(); ++i)
                if (Unit* unit = Unit::GetUnit(*me, (*i)->getUnitGuid()))
                    me->getThreatManager().modifyThreatPercent(unit, action.threat_all_pct.percent);
            break;
        }
    case ACTION_T_QUEST_EVENT:
        if (Unit* target = GetTargetByType(action.quest_event.target, pActionInvoker))
            if (target->GetTypeId() == TYPEID_PLAYER)
                target->ToPlayer()->AreaExploredOrEventHappens(action.quest_event.questId);
        break;
    case ACTION_T_CAST_EVENT:
        if (Unit* target = GetTargetByType(action.cast_event.target, pActionInvoker))
            if (target->GetTypeId() == TYPEID_PLAYER)
                target->ToPlayer()->CastedCreatureOrGO(action.cast_event.creatureId, me->GetGUID(), action.cast_event.spellId);
        break;
    case ACTION_T_SET_UNIT_FIELD:
        {
            Unit* target = GetTargetByType(action.set_unit_field.target, pActionInvoker);

            // not allow modify important for integrity object fields
            if (action.set_unit_field.field < OBJECT_END || action.set_unit_field.field >= UNIT_END)
                return;

            if (target)
                target->SetUInt32Value(action.set_unit_field.field, action.set_unit_field.value);

            break;
        }
    case ACTION_T_SET_UNIT_FLAG:
        if (Unit* target = GetTargetByType(action.unit_flag.target, pActionInvoker))
            target->SetFlag(UNIT_FIELD_FLAGS, action.unit_flag.value);
        break;
    case ACTION_T_REMOVE_UNIT_FLAG:
        if (Unit* target = GetTargetByType(action.unit_flag.target, pActionInvoker))
            target->RemoveFlag(UNIT_FIELD_FLAGS, action.unit_flag.value);
        break;
    case ACTION_T_AUTO_ATTACK:
        MeleeEnabled = action.auto_attack.state != 0;
        break;
    case ACTION_T_COMBAT_MOVEMENT:
        // ignore no affect case
        if (CombatMovementEnabled == (action.combat_movement.state != 0) || me->IsNonMeleeSpellCast(false))
            return;

        SetCombatMovement(action.combat_movement.state != 0);

        if (CombatMovementEnabled && action.combat_movement.melee && me->IsInCombat() && me->GetVictim())
            me->SendMeleeAttackStart(me->GetVictim());
        else if (action.combat_movement.melee && me->IsInCombat() && me->GetVictim())
            me->SendMeleeAttackStop(me->GetVictim());
        break;
    case ACTION_T_SET_PHASE:
        Phase = action.set_phase.phase;
        break;
    case ACTION_T_INC_PHASE:
        {
            int32 new_phase = int32(Phase) + action.set_inc_phase.step;
            if (new_phase < 0)
            {
                sLog.outErrorDb("CreatureEventAI: Event %d decrease Phase under 0. CreatureEntry = %d", EventId, me->GetEntry());
                Phase = 0;
            }
            else if (new_phase >= MAX_PHASE)
            {
                sLog.outErrorDb("CreatureEventAI: Event %d incremented Phase above %u. Phase mask cannot be used with phases past %u. CreatureEntry = %d", EventId, MAX_PHASE - 1, MAX_PHASE - 1, me->GetEntry());
                Phase = MAX_PHASE - 1;
            }
            else
                Phase = new_phase;

            break;
        }
    case ACTION_T_EVADE:
        EnterEvadeMode();
        break;
    case ACTION_T_FLEE_FOR_ASSIST:
        me->DoFleeToGetAssistance();
        break;
    case ACTION_T_QUEST_EVENT_ALL:
        if (pActionInvoker && pActionInvoker->GetTypeId() == TYPEID_PLAYER)
        {
            if (Unit* Temp = Unit::GetUnit(*me, pActionInvoker->GetGUID()))
                if (Temp->GetTypeId() == TYPEID_PLAYER)
                    Temp->ToPlayer()->GroupEventHappens(action.quest_event_all.questId, me);
        }
        break;
    case ACTION_T_CAST_EVENT_ALL:
        {
            ThreatContainer::StorageType const& threatList = me->getThreatManager().getThreatList();
            for (ThreatContainer::StorageType::const_iterator i = threatList.begin(); i != threatList.end(); ++i)
                if (Unit* unit = Unit::GetUnit(*me, (*i)->getUnitGuid()))
                    if (unit->GetTypeId() == TYPEID_PLAYER)
                        unit->ToPlayer()->CastedCreatureOrGO(action.cast_event_all.creatureId, me->GetGUID(), action.cast_event_all.spellId);
            break;
        }
    case ACTION_T_REMOVEAURASFROMSPELL:
        if (Unit* target = GetTargetByType(action.remove_aura.target, pActionInvoker))
            target->RemoveAurasDueToSpell(action.remove_aura.spellId);
        break;
    case ACTION_T_RANGED_MOVEMENT:
        AttackDistance = (float)action.ranged_movement.distance;
        AttackAngle = action.ranged_movement.angle / 180.0f * M_PI;

        if (CombatMovementEnabled)
        {
            if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() & TARGETED_MOTION_TYPE)
            {
                me->GetMotionMaster()->Clear(false);
                me->GetMotionMaster()->MoveChase(me->GetVictim(), AttackDistance, AttackAngle);
            }
        }
        break;
    case ACTION_T_RANDOM_PHASE:
        Phase = GetRandActionParam(rnd, action.random_phase.phase1, action.random_phase.phase2, action.random_phase.phase3);
        break;
    case ACTION_T_RANDOM_PHASE_RANGE:
        if (action.random_phase_range.phaseMin <= action.random_phase_range.phaseMax)
            Phase = urand(action.random_phase_range.phaseMin, action.random_phase_range.phaseMax);
        else
            sLog.outErrorDb("CreatureEventAI: ACTION_T_RANDOM_PHASE_RANGE cannot have Param2 < Param1. Event = %d. CreatureEntry = %d", EventId, me->GetEntry());
        break;
    case ACTION_T_SUMMON_ID:
        {
            Unit* target = GetTargetByType(action.summon_id.target, pActionInvoker);

            CreatureEventAI_Summon_Map::const_iterator i = CreatureEAI_Mgr.GetCreatureEventAISummonMap().find(action.summon_id.spawnId);
            if (i == CreatureEAI_Mgr.GetCreatureEventAISummonMap().end())
            {
                sLog.outErrorDb("CreatureEventAI: failed to spawn creature %u. Summon map index %u does not exist. EventID %d. CreatureID %d", action.summon_id.creatureId, action.summon_id.spawnId, EventId, me->GetEntry());
                return;
            }

            Creature* pCreature = NULL;
            if ((*i).second.SpawnTimeSecs)
                pCreature = me->SummonCreature(action.summon_id.creatureId, (*i).second.position_x, (*i).second.position_y, (*i).second.position_z, (*i).second.orientation, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, (*i).second.SpawnTimeSecs);
            else
                pCreature = me->SummonCreature(action.summon_id.creatureId, (*i).second.position_x, (*i).second.position_y, (*i).second.position_z, (*i).second.orientation, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 0);

            if (!pCreature)
                sLog.outErrorDb("CreatureEventAI: failed to spawn creature %u. EventId %d.Creature %d", action.summon_id.creatureId, EventId, me->GetEntry());
            else if (action.summon_id.target != TARGET_T_SELF && target)
                pCreature->AI()->AttackStart(target);

            break;
        }
    case ACTION_T_KILLED_MONSTER:
        //first attempt player who tapped creature
        if (Player* pPlayer = me->GetLootRecipient())
            pPlayer->RewardPlayerAndGroupAtEvent(action.killed_monster.creatureId, pPlayer);    // pPlayer as param is a hacky solution not to use GUID
        else
        {
            //if not available, use pActionInvoker
            if (Unit* pTarget = GetTargetByType(action.killed_monster.target, pActionInvoker))
                if (Player* pPlayer2 = pTarget->GetCharmerOrOwnerPlayerOrPlayerItself())
                    pPlayer2->RewardPlayerAndGroupAtEvent(action.killed_monster.creatureId, pPlayer2);
        }
        break;
    case ACTION_T_SET_INST_DATA:
        {
            InstanceData* pInst = (InstanceData*)me->GetInstanceData();
            if (!pInst)
            {
                sLog.outErrorDb("CreatureEventAI: Event %d attempt to set instance data without instance script. Creature %d", EventId, me->GetEntry());
                return;
            }

            pInst->SetData(action.set_inst_data.field, action.set_inst_data.value);
            break;
        }
    case ACTION_T_SET_INST_DATA64:
        {
            Unit* target = GetTargetByType(action.set_inst_data64.target, pActionInvoker);
            if (!target)
            {
                sLog.outErrorDb("CreatureEventAI: Event %d attempt to set instance data64 but Target == NULL. Creature %d", EventId, me->GetEntry());
                return;
            }

            InstanceData* pInst = (InstanceData*)me->GetInstanceData();
            if (!pInst)
            {
                sLog.outErrorDb("CreatureEventAI: Event %d attempt to set instance data64 without instance script. Creature %d", EventId, me->GetEntry());
                return;
            }

            pInst->SetData(action.set_inst_data64.field, target->GetGUID());
            break;
        }
    case ACTION_T_UPDATE_TEMPLATE:
        if (me->GetEntry() == action.update_template.creatureId)
        {

            sLog.outErrorDb("CreatureEventAI: Event %d ACTION_T_UPDATE_TEMPLATE call with param1 == current entry. Creature %d", EventId, me->GetEntry());
            return;
        }

        me->UpdateEntry(action.update_template.creatureId, action.update_template.team ? HORDE : ALLIANCE);
        break;
    case ACTION_T_DIE:
        if (me->isDead())
        {

            sLog.outErrorDb("CreatureEventAI: Event %d ACTION_T_DIE on dead creature. Creature %d", EventId, me->GetEntry());
            return;
        }
        me->DealDamage(me, me->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        break;
    case ACTION_T_ZONE_COMBAT_PULSE:
        {
            me->SetInCombatWithZone();
            break;
        }
    case ACTION_T_CALL_FOR_HELP:
        {
            me->CallForHelp(action.call_for_help.radius);
            break;
        }
        break;

    // Oregon ONLY
    case ACTION_T_SET_AGGRESSIVE:
        me->SetReactState(ReactStates(action.raw.param1));
        break;
    case ACTION_T_ATTACK_START_PULSE:
        AttackStart(me->SelectNearestTarget((float)action.raw.param1));
        break;
    case ACTION_T_SUMMON_GO:
        {
            GameObject* pObject = NULL;

            float x, y, z;
            me->GetPosition(x, y, z);
            pObject = me->SummonGameObject(action.raw.param1, x, y, z, 0, 0, 0, 0, 0, action.raw.param2);
            if (!pObject)
                sLog.outErrorDb("OSCR: EventAI failed to spawn object %u. Spawn event %d is on creature %d", action.raw.param1, EventId, me->GetEntry());
            break;
        }

    case ACTION_T_SET_SHEATH:
        {
            me->SetSheath(SheathState(action.set_sheath.sheath));
            break;
        }
    case ACTION_T_FORCE_DESPAWN:
        {
            me->ForcedDespawn();
            break;
        }
    case ACTION_T_SET_INVINCIBILITY_HP_LEVEL:
        {
            if (action.invincibility_hp_level.is_percent)
                InvinceabilityHpLevel = me->GetMaxHealth() * action.invincibility_hp_level.hp_level / 100;
            else
                InvinceabilityHpLevel = action.invincibility_hp_level.hp_level;
            break;
        }
    case ACTION_T_MOUNT_TO_ENTRY_OR_MODEL:
        {
            if (action.mount.creatureId || action.mount.modelId)
            {
                // set model based on entry from creature_template
                if (action.mount.creatureId)
                {
                    if (CreatureInfo const* cInfo = GetCreatureTemplateStore(action.mount.creatureId))
                    {
                        uint32 display_id = sObjectMgr.ChooseDisplayId(0, cInfo);
                        me->Mount(display_id);
                    }
                }
                //if no param1, then use value from param2 (modelId)
                else
                    me->Mount(action.mount.modelId);
            }
            else
                me->Dismount();

            break;
        }
    }
}

void CreatureEventAI::JustRespawned()
{
    Reset();

    if (bEmptyList)
        return;

    //Handle Spawned Events
    for (std::list<CreatureEventAIHolder>::iterator i = CreatureEventAIList.begin(); i != CreatureEventAIList.end(); ++i)
        if (SpawnedEventConditionsCheck((*i).Event))
            ProcessEvent(*i);
}

void CreatureEventAI::Reset()
{
    EventUpdateTime = EVENT_UPDATE_TIME;
    EventDiff = 0;

    if (bEmptyList)
        return;

    //Reset all events to enabled
    for (std::list<CreatureEventAIHolder>::iterator i = CreatureEventAIList.begin(); i != CreatureEventAIList.end(); ++i)
    {
        CreatureEventAI_Event const& event = (*i).Event;
        switch (event.event_type)
        {
        //Reset all out of combat timers
        case EVENT_T_TIMER_OOC:
            {
                if ((*i).UpdateRepeatTimer(me, event.timer.initialMin, event.timer.initialMax))
                    (*i).Enabled = true;
                break;
            }
            //default:
            //@todo enable below code line / verify this is correct to enable events previously disabled (ex. aggro yell), instead of enable this in void EnterCombat()
            //(*i).Enabled = true;
            //(*i).Time = 0;
            //break;
        }
    }
}

void CreatureEventAI::JustReachedHome()
{
    if (!bEmptyList)
    {
        for (std::list<CreatureEventAIHolder>::iterator i = CreatureEventAIList.begin(); i != CreatureEventAIList.end(); ++i)
        {
            if ((*i).Event.event_type == EVENT_T_REACHED_HOME)
                ProcessEvent(*i);
        }
    }

    Reset();
}

void CreatureEventAI::EnterEvadeMode()
{
    CreatureAI::EnterEvadeMode();

    if (bEmptyList)
        return;

    //Handle Evade events
    for (std::list<CreatureEventAIHolder>::iterator i = CreatureEventAIList.begin(); i != CreatureEventAIList.end(); ++i)
    {
        if ((*i).Event.event_type == EVENT_T_EVADE)
            ProcessEvent(*i);
    }
}

void CreatureEventAI::JustDied(Unit* killer)
{
    Reset();

    if (bEmptyList)
        return;

    //Handle Evade events
    for (std::list<CreatureEventAIHolder>::iterator i = CreatureEventAIList.begin(); i != CreatureEventAIList.end(); ++i)
    {
        if ((*i).Event.event_type == EVENT_T_DEATH)
            ProcessEvent(*i, killer);
    }

    // reset phase after any death state events
    Phase = 0;
}

void CreatureEventAI::KilledUnit(Unit* victim)
{
    if (bEmptyList || victim->GetTypeId() != TYPEID_PLAYER)
        return;

    for (std::list<CreatureEventAIHolder>::iterator i = CreatureEventAIList.begin(); i != CreatureEventAIList.end(); ++i)
    {
        if ((*i).Event.event_type == EVENT_T_KILL)
            ProcessEvent(*i, victim);
    }
}

void CreatureEventAI::JustSummoned(Creature* pUnit)
{
    if (bEmptyList || !pUnit)
        return;

    for (std::list<CreatureEventAIHolder>::iterator i = CreatureEventAIList.begin(); i != CreatureEventAIList.end(); ++i)
    {
        if ((*i).Event.event_type == EVENT_T_SUMMONED_UNIT)
            ProcessEvent(*i, pUnit);
    }
}

void CreatureEventAI::EnterCombat(Unit* enemy)
{
    me->InterruptNonMeleeSpells(false);
    //Check for on combat start events
    if (!bEmptyList)
    {
        for (std::list<CreatureEventAIHolder>::iterator i = CreatureEventAIList.begin(); i != CreatureEventAIList.end(); ++i)
        {
            CreatureEventAI_Event const& event = (*i).Event;
            switch (event.event_type)
            {
            case EVENT_T_AGGRO:
                (*i).Enabled = true;
                ProcessEvent(*i, enemy);
                break;
            //Reset all in combat timers
            case EVENT_T_TIMER:
                if ((*i).UpdateRepeatTimer(me, event.timer.initialMin, event.timer.initialMax))
                    (*i).Enabled = true;
                break;
            //All normal events need to be re-enabled and their time set to 0
            default:
                (*i).Enabled = true;
                (*i).Time = 0;
                break;
            }
        }
    }

    EventUpdateTime = EVENT_UPDATE_TIME;
    EventDiff = 0;
}

void CreatureEventAI::AttackStart(Unit* who)
{
    if (!who)
        return;

    if (me->Attack(who, MeleeEnabled))
    {
        if (CombatMovementEnabled)
            me->GetMotionMaster()->MoveChase(who, AttackDistance, AttackAngle);
        else
        {
            me->GetMotionMaster()->MoveIdle();
            me->StopMoving();
        }
    }
}

void CreatureEventAI::MoveInLineOfSight(Unit* who)
{
    //Check for OOC LOS Event
    if (hasLosEvents)
    {
        //those checks are done in CreatureAI::MoveInLineOfSight as well,
        //for this reason moved into this if

        if (!who)
            return;

        if (me->GetVictim())
            return;

        for (std::list<CreatureEventAIHolder>::iterator itr = CreatureEventAIList.begin(); itr != CreatureEventAIList.end(); ++itr)
        {
            CreatureEventAI_Event aiEvent = (*itr).Event;

            if (aiEvent.event_type == EVENT_T_OOC_LOS)
            {
                bool isHostile = me->IsHostileTo(who);

                //if friendly event && who is not hostile OR hostile event && who is hostile
                if ((aiEvent.ooc_los.noHostile && !isHostile) ||
                    (!aiEvent.ooc_los.noHostile && isHostile))
                {
                    //can trigger if closer than fMaxAllowedRange
                    float fMaxAllowedRange = aiEvent.ooc_los.maxRange;

                    //if range is ok and we are actually in LOS
                    if (me->IsWithinDistInMap(who, fMaxAllowedRange) && me->IsWithinLOSInMap(who))
                    {
                        ProcessEvent(*itr, who);
                    }
                }
            }
        }
    }

    CreatureAI::MoveInLineOfSight(who);
}

void CreatureEventAI::SpellHit(Unit* pUnit, const SpellEntry* pSpell)
{

    if (bEmptyList)
        return;

    for (std::list<CreatureEventAIHolder>::iterator i = CreatureEventAIList.begin(); i != CreatureEventAIList.end(); ++i)
        if ((*i).Event.event_type == EVENT_T_SPELLHIT)
            //If spell id matches (or no spell id) & if spell school matches (or no spell school)
            if (!(*i).Event.spell_hit.spellId || pSpell->Id == (*i).Event.spell_hit.spellId)
                if (pSpell->SchoolMask & (*i).Event.spell_hit.schoolMask)
                    ProcessEvent(*i, pUnit);
}

void CreatureEventAI::UpdateAI(const uint32 diff)
{
    //Check if we are in combat (also updates calls threat update code)
    bool Combat = UpdateVictim();

    if (!bEmptyList)
    {
        //Events are only updated once every EVENT_UPDATE_TIME ms to prevent lag with large amount of events
        if (EventUpdateTime <= diff)
        {
            EventDiff += diff;

            //Check for time based events
            for (std::list<CreatureEventAIHolder>::iterator i = CreatureEventAIList.begin(); i != CreatureEventAIList.end(); ++i)
            {
                //Decrement Timers
                if ((*i).Time)
                {
                    if (EventDiff <= (*i).Time)
                    {
                        //Do not decrement timers if event cannot trigger in this phase
                        if (!((*i).Event.event_inverse_phase_mask & (1 << Phase)))
                            (*i).Time -= EventDiff;

                        //Skip processing of events that have time remaining
                        continue;
                    }
                    else (*i).Time = 0;
                }

                //Events that are updated every EVENT_UPDATE_TIME
                switch ((*i).Event.event_type)
                {
                case EVENT_T_TIMER_OOC:
                    ProcessEvent(*i);
                    break;
                case EVENT_T_TIMER:
                case EVENT_T_MANA:
                case EVENT_T_HP:
                case EVENT_T_TARGET_HP:
                case EVENT_T_TARGET_CASTING:
                case EVENT_T_FRIENDLY_HP:
                    if (me->GetVictim())
                        ProcessEvent(*i);
                    break;
                case EVENT_T_RANGE:
                    if (me->GetVictim())
                        if (me->IsInMap(me->GetVictim()))
                            if (me->IsInRange(me->GetVictim(), (float)(*i).Event.range.minDist, (float)(*i).Event.range.maxDist))
                                ProcessEvent(*i);
                    break;
                }
            }

            EventDiff = 0;
            EventUpdateTime = EVENT_UPDATE_TIME;
        }
        else
        {
            EventDiff += diff;
            EventUpdateTime -= diff;
        }
    }

    //Melee Auto-Attack
    if (Combat && MeleeEnabled)
        DoMeleeAttackIfReady();
}

inline uint32 CreatureEventAI::GetRandActionParam(uint32 rnd, uint32 param1, uint32 param2, uint32 param3)
{
    switch (rnd % 3)
    {
    case 0:
        return param1;
    case 1:
        return param2;
    case 2:
        return param3;
    }
    return 0;
}

inline int32 CreatureEventAI::GetRandActionParam(uint32 rnd, int32 param1, int32 param2, int32 param3)
{
    switch (rnd % 3)
    {
    case 0:
        return param1;
    case 1:
        return param2;
    case 2:
        return param3;
    }
    return 0;
}

inline Unit* CreatureEventAI::GetTargetByType(uint32 Target, Unit* pActionInvoker)
{
    switch (Target)
    {
    case TARGET_T_SELF:
        return me;
    case TARGET_T_HOSTILE:
        return me->GetVictim();
    case TARGET_T_HOSTILE_SECOND_AGGRO:
        return SelectTarget(SELECT_TARGET_TOPAGGRO, 1);
    case TARGET_T_HOSTILE_LAST_AGGRO:
        return SelectTarget(SELECT_TARGET_BOTTOMAGGRO, 0);
    case TARGET_T_HOSTILE_RANDOM:
        return SelectTarget(SELECT_TARGET_RANDOM, 0);
    case TARGET_T_HOSTILE_RANDOM_NOT_TOP:
        return SelectTarget(SELECT_TARGET_RANDOM, 1);
    case TARGET_T_ACTION_INVOKER:
        return pActionInvoker;
    default:
        return NULL;
    };
}

Unit* CreatureEventAI::DoSelectLowestHpFriendly(float range, uint32 MinHPDiff)
{
    CellCoord p(Oregon::ComputeCellCoord(me->GetPositionX(), me->GetPositionY()));
    Cell cell(p);
    cell.SetNoCreate();

    Unit* pUnit = NULL;

    Oregon::MostHPMissingInRange u_check(me, range, MinHPDiff);
    Oregon::UnitLastSearcher<Oregon::MostHPMissingInRange> searcher(me, pUnit, u_check);

    TypeContainerVisitor<Oregon::UnitLastSearcher<Oregon::MostHPMissingInRange>, GridTypeMapContainer >  grid_unit_searcher(searcher);

    cell.Visit(p, grid_unit_searcher, *me->GetMap(), *me, range);
    return pUnit;
}

void CreatureEventAI::DoFindFriendlyCC(std::list<Creature*>& _list, float range)
{
    CellCoord p(Oregon::ComputeCellCoord(me->GetPositionX(), me->GetPositionY()));
    Cell cell(p);
    cell.SetNoCreate();

    Oregon::FriendlyCCedInRange u_check(me, range);
    Oregon::CreatureListSearcher<Oregon::FriendlyCCedInRange> searcher(me, _list, u_check);

    TypeContainerVisitor<Oregon::CreatureListSearcher<Oregon::FriendlyCCedInRange>, GridTypeMapContainer >  grid_creature_searcher(searcher);

    cell.Visit(p, grid_creature_searcher, *me->GetMap(), *me, range);
}

void CreatureEventAI::DoFindFriendlyMissingBuff(std::list<Creature*>& _list, float range, uint32 spellid)
{
    CellCoord p(Oregon::ComputeCellCoord(me->GetPositionX(), me->GetPositionY()));
    Cell cell(p);
    cell.SetNoCreate();

    Oregon::FriendlyMissingBuffInRange u_check(me, range, spellid);
    Oregon::CreatureListSearcher<Oregon::FriendlyMissingBuffInRange> searcher(me, _list, u_check);

    TypeContainerVisitor<Oregon::CreatureListSearcher<Oregon::FriendlyMissingBuffInRange>, GridTypeMapContainer >  grid_creature_searcher(searcher);

    cell.Visit(p, grid_creature_searcher, *me->GetMap(), *me, range);
}

//*********************************
//*** Functions used globally ***

void CreatureEventAI::DoScriptText(int32 textEntry, WorldObject* pSource, Unit* target)
{
    if (!pSource)
    {
        sLog.outErrorDb("CreatureEventAI: DoScriptText entry %i, invalid Source pointer.", textEntry);
        return;
    }

    if (textEntry >= 0)
    {
        sLog.outErrorDb("CreatureEventAI: DoScriptText with source entry %u (TypeId=%u, guid=%u) attempts to process text entry %i, but text entry must be negative.", pSource->GetEntry(), pSource->GetTypeId(), pSource->GetGUIDLow(), textEntry);
        return;
    }

    CreatureEventAI_TextMap::const_iterator i = CreatureEAI_Mgr.GetCreatureEventAITextMap().find(textEntry);

    if (i == CreatureEAI_Mgr.GetCreatureEventAITextMap().end())
    {
        sLog.outErrorDb("CreatureEventAI: DoScriptText with source entry %u (TypeId=%u, guid=%u) could not find text entry %i.", pSource->GetEntry(), pSource->GetTypeId(), pSource->GetGUIDLow(), textEntry);
        return;
    }

    sLog.outDebug("CreatureEventAI: DoScriptText: text entry=%i, Sound=%u, Type=%u, Language=%u, Emote=%u", textEntry, (*i).second.SoundId, (*i).second.Type, (*i).second.Language, (*i).second.Emote);

    if ((*i).second.SoundId)
    {
        if (GetSoundEntriesStore()->LookupEntry((*i).second.SoundId))
            pSource->SendPlaySound((*i).second.SoundId, false);
        else
            sLog.outErrorDb("CreatureEventAI: DoScriptText entry %i tried to process invalid sound id %u.", textEntry, (*i).second.SoundId);
    }

    if ((*i).second.Emote)
    {
        if (pSource->GetTypeId() == TYPEID_UNIT || pSource->GetTypeId() == TYPEID_PLAYER)
            ((Unit*)pSource)->HandleEmoteCommand((*i).second.Emote);
        else
            sLog.outErrorDb("CreatureEventAI: DoScriptText entry %i tried to process emote for invalid TypeId (%u).", textEntry, pSource->GetTypeId());
    }

    switch ((*i).second.Type)
    {
    case CHAT_TYPE_SAY:
        pSource->MonsterSay(textEntry, (*i).second.Language, target ? target->GetGUID() : 0);
        break;
    case CHAT_TYPE_YELL:
        pSource->MonsterYell(textEntry, (*i).second.Language, target ? target->GetGUID() : 0);
        break;
    case CHAT_TYPE_TEXT_EMOTE:
        pSource->MonsterTextEmote(textEntry, target ? target->GetGUID() : 0);
        break;
    case CHAT_TYPE_BOSS_EMOTE:
        pSource->MonsterTextEmote(textEntry, target ? target->GetGUID() : 0, true);
        break;
    case CHAT_TYPE_WHISPER:
        {
            if (target && target->GetTypeId() == TYPEID_PLAYER)
                pSource->MonsterWhisper(textEntry, target->GetGUID());
            else sLog.outErrorDb("CreatureEventAI: DoScriptText entry %i cannot whisper without target unit (TYPEID_PLAYER).", textEntry);
        }
        break;
    case CHAT_TYPE_BOSS_WHISPER:
        {
            if (target && target->GetTypeId() == TYPEID_PLAYER)
                pSource->MonsterWhisper(textEntry, target->GetGUID(), true);
            else sLog.outErrorDb("CreatureEventAI: DoScriptText entry %i cannot whisper without target unit (TYPEID_PLAYER).", textEntry);
        }
        break;
    case CHAT_TYPE_ZONE_YELL:
        pSource->MonsterYellToZone(textEntry, (*i).second.Language, target ? target->GetGUID() : 0);
        break;
    }
}

SpellCastResult CreatureEventAI::CanCast(Unit* target, SpellEntry const* spell, uint32 flags)
{
    Unit* caster = me;

    if (flags & CAST_FORCE_TARGET_SELF)
        caster = target;

    if (!spell)
    {
        sLog.outErrorDb("CreatureEventAI::CanCast by creature entry %u attempt to cast spell but spell does not exist.", me->GetEntry());
        return SPELL_FAILED_UNKNOWN;
    }

    // Allowed to cast only if not casting (unless we interrupt ourself) or if spell is triggered
    if (!caster->IsNonMeleeSpellCast(false) || (flags & (CAST_TRIGGERED | CAST_INTURRUPT_PREVIOUS)))
    {
        // No target so we can't cast
        if (!target)
            return SPELL_FAILED_BAD_TARGETS;

        // If cast flag CAST_AURA_NOT_PRESENT is active, check if target already has aura on them
        if (flags & CAST_AURA_NOT_PRESENT)
        {
            if (target->HasAura(spell->Id))
                return SPELL_FAILED_TARGET_AURASTATE;
        }

        // Silenced so we can't cast
        if (!(flags & CAST_TRIGGERED) && me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED))
            return SPELL_FAILED_SILENCED;

        // Target is not within LoS
        if (!caster->IsWithinLOSInMap(target) && !caster->IsWithinLOS(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ()))
            return SPELL_FAILED_LINE_OF_SIGHT;

        // Check for power
        if (!(flags & CAST_TRIGGERED) && me->GetPower((Powers)spell->powerType) < CalculatePowerCost(spell, me, GetSpellSchoolMask(spell)))
            return SPELL_FAILED_NO_POWER;

        SpellRangeEntry const* TempRange = NULL;
        TempRange = GetSpellRangeStore()->LookupEntry(spell->rangeIndex);

        //Unit is out of range of this spell
        if (!me->IsInRange(target, TempRange->minRange, TempRange->maxRange))
            return SPELL_FAILED_OUT_OF_RANGE;

        // Interrupt any previous spell
        if (flags & CAST_INTURRUPT_PREVIOUS && caster->IsNonMeleeSpellCast(false))
            caster->InterruptNonMeleeSpells(false);

        caster->CastSpell(target, spell, flags & CAST_TRIGGERED, NULL, NULL);
        return SPELL_CAST_OK;
    }
    else
        return SPELL_FAILED_SPELL_IN_PROGRESS;
}

void CreatureEventAI::ReceiveEmote(Player* pPlayer, uint32 text_emote)
{
    if (bEmptyList)
        return;

    for (std::list<CreatureEventAIHolder>::iterator itr = CreatureEventAIList.begin(); itr != CreatureEventAIList.end(); ++itr)
    {
        if ((*itr).Event.event_type == EVENT_T_RECEIVE_EMOTE)
        {
            if ((*itr).Event.receive_emote.emoteId != text_emote)
                return;

            Condition* cond = new Condition();
            cond->Type = ConditionType((*itr).Event.receive_emote.condition);            
            cond->ConditionValue1 = (*itr).Event.receive_emote.conditionValue1;
            cond->ConditionValue2 = (*itr).Event.receive_emote.conditionValue2;
            
            ConditionSourceInfo info(pPlayer);
            if (cond->Meets(info))
            {
                sLog.outDebug("CreatureEventAI: ReceiveEmote CreatureEventAI: Condition ok, processing");
                ProcessEvent(*itr, pPlayer);
            }
        }
    }
}

void CreatureEventAI::DamageTaken(Unit* /*done_by*/, uint32& damage)
{
    if (InvinceabilityHpLevel > 0 && me->GetHealth() < InvinceabilityHpLevel + damage)
    {
        if (me->GetHealth() <= InvinceabilityHpLevel)
            damage = 0;
        else
            damage = me->GetHealth() - InvinceabilityHpLevel;
    }
}

bool CreatureEventAI::SpawnedEventConditionsCheck(CreatureEventAI_Event const& event)
{
    if (event.event_type != EVENT_T_SPAWNED)
        return false;

    switch (event.spawned.condition)
    {
    case SPAWNED_EVENT_ALWAY:
        // always
        return true;
    case SPAWNED_EVENT_MAP:
        // map ID check
        return me->GetMapId() == event.spawned.conditionValue1;
    case SPAWNED_EVENT_ZONE:
        // zone ID check
        return me->GetZoneId() == event.spawned.conditionValue1 || me->GetAreaId() == event.spawned.conditionValue1;
    default:
        break;
    }

    return false;
}

