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

#include "ScriptMgr.h"
#include "Config/Config.h"
#include "Database/DatabaseEnv.h"
#include "DBCStores.h"
#include "ObjectMgr.h"
#include "ScriptLoader.h"
#include "ScriptSystem.h"
#include "SpellMgr.h"
#include "GossipDef.h"
#include "CreatureAIImpl.h"
#include "Player.h"

INSTANTIATE_SINGLETON_1(ScriptMgr);

int num_sc_scripts;
Script* m_scripts[MAX_SCRIPTS];

void FillSpellSummary();
void LoadOverridenSQLData();

void ScriptMgr::LoadDatabase()
{
    pSystemMgr.LoadVersion();
    pSystemMgr.LoadScriptTexts();
    pSystemMgr.LoadScriptTextsCustom();
    pSystemMgr.LoadScriptWaypoints();
}

struct TSpellSummary
{
    uint8 Targets;                                          // set of enum SelectTarget
    uint8 Effects;                                          // set of enum SelectEffect
} extern* SpellSummary;

ScriptMgr::ScriptMgr()
{
}

ScriptMgr::~ScriptMgr()
{
    // Free Spell Summary
    delete []SpellSummary;

    // Free resources before library unload
    for (uint16 i = 0; i < MAX_SCRIPTS; ++i)
        delete m_scripts[i];

    num_sc_scripts = 0;
}

void ScriptMgr::ScriptsInit()
{
    outstring_log("   ____                              _____           _       _   ");
    outstring_log("  / __ \\                            / ____|         (_)     | |  ");
    outstring_log(" | |  | |_ __ ___  __ _  ___  _ __ | (___   ___ _ __ _ _ __ | |_ ");
    outstring_log(" | |  | | '__/ _ \\/ _` |/ _ \\| '_ \\ \\___ \\ / __| '__| | '_ \\| __|");
    outstring_log(" | |__| | | |  __/ (_| | (_) | | | |____) | (__| |  | | |_) | |_ ");
    outstring_log("  \\____/|_|  \\___|\\__, |\\___/|_| |_|_____/ \\___|_|  |_| .__/ \\__|");
    outstring_log("                   __/ |                              | |        ");
    outstring_log("                  |___/                               |_|  \n");

    //Load database (must be called after SD2Config.SetSource).
    LoadDatabase();

    for (uint16 i = 0; i < MAX_SCRIPTS; ++i)
        m_scripts[i] = NULL;

    FillSpellSummary();

    AddScripts();

    outstring_log(">> Loaded %i C++ Scripts.", num_sc_scripts);

    outstring_log(">> Load Overriden SQL Data.");
    LoadOverridenSQLData();
}

//*********************************
//*** Functions used globally ***

void DoScriptText(int32 iTextEntry, WorldObject* pSource, Unit* pTarget)
{
    if (!pSource)
    {
        error_log("OSCR: DoScriptText entry %i, invalid Source pointer.", iTextEntry);
        return;
    }

    if (iTextEntry >= 0)
    {
        error_log("OSCR: DoScriptText with source entry %u (TypeId=%u, guid=%u) attempts to process text entry %i, but text entry must be negative.", pSource->GetEntry(), pSource->GetTypeId(), pSource->GetGUIDLow(), iTextEntry);
        return;
    }

    const StringTextData* pData = pSystemMgr.GetTextData(iTextEntry);

    if (!pData)
    {
        error_log("OSCR: DoScriptText with source entry %u (TypeId=%u, guid=%u) could not find text entry %i.", pSource->GetEntry(), pSource->GetTypeId(), pSource->GetGUIDLow(), iTextEntry);
        return;
    }

    debug_log("OSCR: DoScriptText: text entry=%i, Sound=%u, Type=%u, Language=%u, Emote=%u", iTextEntry, pData->uiSoundId, pData->uiType, pData->uiLanguage, pData->uiEmote);

    if (pData->uiSoundId)
    {
        if (GetSoundEntriesStore()->LookupEntry(pData->uiSoundId))
            pSource->SendPlaySound(pData->uiSoundId, false);
        else
            error_log("OSCR: DoScriptText entry %i tried to process invalid sound id %u.", iTextEntry, pData->uiSoundId);
    }

    if (pData->uiEmote)
    {
        if (pSource->GetTypeId() == TYPEID_UNIT || pSource->GetTypeId() == TYPEID_PLAYER)
            ((Unit*)pSource)->HandleEmoteCommand(pData->uiEmote);
        else
            error_log("OSCR: DoScriptText entry %i tried to process emote for invalid TypeId (%u).", iTextEntry, pSource->GetTypeId());
    }

    switch (pData->uiType)
    {
    case CHAT_TYPE_SAY:
        pSource->MonsterSay(iTextEntry, pData->uiLanguage, pTarget ? pTarget->GetGUID() : 0);
        break;
    case CHAT_TYPE_YELL:
        pSource->MonsterYell(iTextEntry, pData->uiLanguage, pTarget ? pTarget->GetGUID() : 0);
        break;
    case CHAT_TYPE_TEXT_EMOTE:
        pSource->MonsterTextEmote(iTextEntry, pTarget ? pTarget->GetGUID() : 0);
        break;
    case CHAT_TYPE_BOSS_EMOTE:
        pSource->MonsterTextEmote(iTextEntry, pTarget ? pTarget->GetGUID() : 0, true);
        break;
    case CHAT_TYPE_WHISPER:
        {
            if (pTarget && pTarget->GetTypeId() == TYPEID_PLAYER)
                pSource->MonsterWhisper(iTextEntry, pTarget->GetGUID());
            else
                error_log("OSCR: DoScriptText entry %i cannot whisper without target unit (TYPEID_PLAYER).", iTextEntry);
        }
        break;
    case CHAT_TYPE_BOSS_WHISPER:
        {
            if (pTarget && pTarget->GetTypeId() == TYPEID_PLAYER)
                pSource->MonsterWhisper(iTextEntry, pTarget->GetGUID(), true);
            else
                error_log("OSCR: DoScriptText entry %i cannot whisper without target unit (TYPEID_PLAYER).", iTextEntry);
        }
        break;
    case CHAT_TYPE_ZONE_YELL:
        pSource->MonsterYellToZone(iTextEntry, pData->uiLanguage, pTarget ? pTarget->GetGUID() : 0);
        break;
    }
}

void Script::RegisterSelf()
{
    // try to find scripts which try to use another script's allocated memory
    // that means didn't allocate memory for script
    for (uint16 i = 0; i < MAX_SCRIPTS; ++i)
    {
        // somebody forgot to allocate memory for a script by a method like this: newscript = new Script
        if (m_scripts[i] == this)
        {
            error_log("ScriptName: '%s' - Forgot to allocate memory, so this script and/or the script before that can't work.", Name.c_str());
            // don't register it
            // and don't delete it because its memory is used for another script
            return;
        }
    }

    int id = GetScriptId(Name.c_str());
    if (id)
    {
        // try to find the script in assigned scripts
        bool IsExist = false;
        for (uint16 i = 0; i < MAX_SCRIPTS; ++i)
        {
            if (m_scripts[i])
            {
                // if the assigned script's name and the new script's name is the same
                if (m_scripts[i]->Name == Name)
                {
                    IsExist = true;
                    break;
                }
            }
        }

        // if the script doesn't assigned -> assign it!
        if (!IsExist)
        {
            m_scripts[id] = this;
            ++num_sc_scripts;
        }
        // if the script is already assigned -> delete it!
        else
        {
            // @todo write a better error message than this one :)
            error_log("ScriptName: '%s' already assigned with the same ScriptName, so the script can't work.", Name.c_str());
            delete this;
        }
    }
    else
    {
        if (Name.find("example") == std::string::npos)
            error_db_log("OSCR: RegisterSelf, but script named %s does not have ScriptName assigned in database.", (this)->Name.c_str());
        delete this;
    }
}

void ScriptMgr::OnLogin(Player* player)
{
    Script* tmpscript = m_scripts[GetScriptId("scripted_on_events")];
    if (!tmpscript || !tmpscript->OnLogin) return;
    tmpscript->OnLogin(player);
}

void ScriptMgr::OnLogout(Player* player)
{
    Script* tmpscript = m_scripts[GetScriptId("scripted_on_events")];
    if (!tmpscript || !tmpscript->OnLogout) return;
    tmpscript->OnLogout(player);
}

void ScriptMgr::OnPVPKill(Player* killer, Player* killed)
{
    Script* tmpscript = m_scripts[GetScriptId("scripted_on_events")];
    if (!tmpscript || !tmpscript->OnPVPKill) return;
    tmpscript->OnPVPKill(killer, killed);
}

void ScriptMgr::OnCreatureKill(Player* killer, Creature* killed)
{
    Script* tmpscript = m_scripts[GetScriptId("scripted_on_events")];
    if (!tmpscript || !tmpscript->OnCreatureKill) return;
    tmpscript->OnCreatureKill(killer, killed);
}

void ScriptMgr::OnPlayerKilledByCreature(Creature* killer, Player* killed)
{
    Script* tmpscript = m_scripts[GetScriptId("scripted_on_events")];
    if (!tmpscript || !tmpscript->OnPlayerKilledByCreature) return;
    tmpscript->OnPlayerKilledByCreature(killer, killed);
}

void ScriptMgr::OnPlayerLevelChanged(Player* player, uint8 oldLevel, uint8 newLevel)
{
    Script* tmpscript = m_scripts[GetScriptId("scripted_on_events")];
    if (!tmpscript || !tmpscript->OnLevelChanged) return;
    tmpscript->OnLevelChanged(player, oldLevel, newLevel);
}

void ScriptMgr::OnPlayerTalentsReset(Player* player, bool no_cost)
{
    Script* tmpscript = m_scripts[GetScriptId("scripted_on_events")];
    if (!tmpscript || !tmpscript->OnTalentsReset) return;
    tmpscript->OnTalentsReset(player, no_cost);
}

void ScriptMgr::OnGroupCreated(Group* pGroup, Player* pPlayer)
{
    Script* tmpscript = m_scripts[GetScriptId("scripted_on_events")];
    if (!tmpscript || !tmpscript->OnGroupCreated) return;
    tmpscript->OnGroupCreated(pGroup, pPlayer);
}

void ScriptMgr::OnGroupPlayerInvited(Group* pGroup, Player* pPlayer)
{
    Script* tmpscript = m_scripts[GetScriptId("scripted_on_events")];
    if (!tmpscript || !tmpscript->OnGroupPlayerInvited) return;
    tmpscript->OnGroupPlayerInvited(pGroup, pPlayer);
}

void ScriptMgr::OnGroupPlayerJoined(Group* pGroup, Player* pPlayer)
{
    Script* tmpscript = m_scripts[GetScriptId("scripted_on_events")];
    if (!tmpscript || !tmpscript->OnGroupPlayerJoined) return;
    tmpscript->OnGroupPlayerJoined(pGroup, pPlayer);
}

void ScriptMgr::OnGroupPlayerRemoved(Group* pGroup, Player* pPlayer, uint8 method, uint64 kicker, const char* reason)
{
    Script* tmpscript = m_scripts[GetScriptId("scripted_on_events")];
    if (!tmpscript || !tmpscript->OnGroupPlayerRemoved) return;
    tmpscript->OnGroupPlayerRemoved(pGroup, pPlayer, method, kicker, reason);
}

void ScriptMgr::OnGroupLeaderChanged(Group* pGroup, Player* pOldLeader, Player* pNewLeader)
{
    Script* tmpscript = m_scripts[GetScriptId("scripted_on_events")];
    if (!tmpscript || !tmpscript->OnGroupLeaderChanged) return;
    tmpscript->OnGroupLeaderChanged(pGroup, pOldLeader, pNewLeader);
}

void ScriptMgr::OnGroupDisbanded(Group* pGroup, Player* pLeader)
{
    Script* tmpscript = m_scripts[GetScriptId("scripted_on_events")];
    if (!tmpscript || !tmpscript->OnGroupDisbanded) return;
    tmpscript->OnGroupDisbanded(pGroup, pLeader);
}

char const* ScriptMgr::ScriptsVersion()
{
    return "Integrated Oregon Scripts";
}

bool ScriptMgr::GossipHello (Player* pPlayer, Creature* pCreature)
{
    Script* tmpscript = m_scripts[pCreature->GetScriptId()];
    if (!tmpscript || !tmpscript->pGossipHello) return false;

    pPlayer->PlayerTalkClass->ClearMenus();
    return tmpscript->pGossipHello(pPlayer, pCreature);
}

bool ScriptMgr::GossipSelect(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    debug_log("OSCR: Gossip selection, sender: %d, action: %d", uiSender, uiAction);

    Script* tmpscript = m_scripts[pCreature->GetScriptId()];
    if (!tmpscript || !tmpscript->pGossipSelect) return false;

    pPlayer->PlayerTalkClass->ClearMenus();
    return tmpscript->pGossipSelect(pPlayer, pCreature, uiSender, uiAction);
}

bool ScriptMgr::GossipSelectWithCode(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction, const char* sCode)
{
    debug_log("OSCR: Gossip selection with code, sender: %d, action: %d", uiSender, uiAction);

    Script* tmpscript = m_scripts[pCreature->GetScriptId()];
    if (!tmpscript || !tmpscript->pGossipSelectWithCode) return false;

    pPlayer->PlayerTalkClass->ClearMenus();
    return tmpscript->pGossipSelectWithCode(pPlayer, pCreature, uiSender, uiAction, sCode);
}

bool ScriptMgr::GOSelect(Player* pPlayer, GameObject* pGO, uint32 uiSender, uint32 uiAction)
{
    if (!pGO)
        return false;
    debug_log("OSCR: Gossip selection, sender: %d, action: %d", uiSender, uiAction);

    Script* tmpscript = m_scripts[pGO->GetGOInfo()->ScriptId];
    if (!tmpscript || !tmpscript->pGOSelect) return false;

    pPlayer->PlayerTalkClass->ClearMenus();
    return tmpscript->pGOSelect(pPlayer, pGO, uiSender, uiAction);
}

bool ScriptMgr::GOSelectWithCode(Player* pPlayer, GameObject* pGO, uint32 uiSender, uint32 uiAction, const char* sCode)
{
    if (!pGO)
        return false;
    debug_log("OSCR: Gossip selection, sender: %d, action: %d", uiSender, uiAction);

    Script* tmpscript = m_scripts[pGO->GetGOInfo()->ScriptId];
    if (!tmpscript || !tmpscript->pGOSelectWithCode) return false;

    pPlayer->PlayerTalkClass->ClearMenus();
    return tmpscript->pGOSelectWithCode(pPlayer, pGO, uiSender , uiAction, sCode);
}

bool ScriptMgr::QuestAccept(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
    Script* tmpscript = m_scripts[pCreature->GetScriptId()];
    if (!tmpscript || !tmpscript->pQuestAccept) return false;

    pPlayer->PlayerTalkClass->ClearMenus();
    return tmpscript->pQuestAccept(pPlayer, pCreature, pQuest);
}

bool ScriptMgr::QuestSelect(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
    Script* tmpscript = m_scripts[pCreature->GetScriptId()];
    if (!tmpscript || !tmpscript->pQuestSelect) return false;

    pPlayer->PlayerTalkClass->ClearMenus();
    return tmpscript->pQuestSelect(pPlayer, pCreature, pQuest);
}

bool ScriptMgr::QuestComplete(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
    Script* tmpscript = m_scripts[pCreature->GetScriptId()];
    if (!tmpscript || !tmpscript->pQuestComplete) return false;

    pPlayer->PlayerTalkClass->ClearMenus();
    return tmpscript->pQuestComplete(pPlayer, pCreature, pQuest);
}

bool ScriptMgr::ChooseReward(Player* pPlayer, Creature* pCreature, Quest const* pQuest, uint32 opt)
{
    Script* tmpscript = m_scripts[pCreature->GetScriptId()];
    if (!tmpscript || !tmpscript->pChooseReward) return false;

    pPlayer->PlayerTalkClass->ClearMenus();
    return tmpscript->pChooseReward(pPlayer, pCreature, pQuest, opt);
}

uint32 ScriptMgr::NPCDialogStatus(Player* pPlayer, Creature* pCreature)
{
    Script* tmpscript = m_scripts[pCreature->GetScriptId()];
    if (!tmpscript || !tmpscript->pNPCDialogStatus) return 100;

    pPlayer->PlayerTalkClass->ClearMenus();
    return tmpscript->pNPCDialogStatus(pPlayer, pCreature);
}

uint32 ScriptMgr::GODialogStatus(Player* pPlayer, GameObject* pGO)
{
    Script* tmpscript = m_scripts[pGO->GetGOInfo()->ScriptId];
    if (!tmpscript || !tmpscript->pGODialogStatus) return 100;

    pPlayer->PlayerTalkClass->ClearMenus();
    return tmpscript->pGODialogStatus(pPlayer, pGO);
}

bool ScriptMgr::ItemHello(Player* pPlayer, Item* pItem, Quest const* pQuest)
{
    Script* tmpscript = m_scripts[pItem->GetProto()->ScriptId];
    if (!tmpscript || !tmpscript->pItemHello) return false;

    pPlayer->PlayerTalkClass->ClearMenus();
    return tmpscript->pItemHello(pPlayer, pItem, pQuest);
}

bool ScriptMgr::ItemQuestAccept(Player* pPlayer, Item* pItem, Quest const* pQuest)
{
    Script* tmpscript = m_scripts[pItem->GetProto()->ScriptId];
    if (!tmpscript || !tmpscript->pItemQuestAccept) return false;

    pPlayer->PlayerTalkClass->ClearMenus();
    return tmpscript->pItemQuestAccept(pPlayer, pItem, pQuest);
}

bool ScriptMgr::GOHello(Player* pPlayer, GameObject* pGO)
{
    Script* tmpscript = m_scripts[pGO->GetGOInfo()->ScriptId];
    if (!tmpscript || !tmpscript->pGOHello) return false;

    pPlayer->PlayerTalkClass->ClearMenus();
    return tmpscript->pGOHello(pPlayer, pGO);
}

bool ScriptMgr::GOQuestAccept(Player* pPlayer, GameObject* pGO, Quest const* pQuest)
{
    Script* tmpscript = m_scripts[pGO->GetGOInfo()->ScriptId];
    if (!tmpscript || !tmpscript->pGOQuestAccept) return false;

    pPlayer->PlayerTalkClass->ClearMenus();
    return tmpscript->pGOQuestAccept(pPlayer, pGO, pQuest);
}

bool ScriptMgr::GOChooseReward(Player* pPlayer, GameObject* pGO, Quest const* pQuest, uint32 opt)
{
    Script* tmpscript = m_scripts[pGO->GetGOInfo()->ScriptId];
    if (!tmpscript || !tmpscript->pGOChooseReward) return false;

    pPlayer->PlayerTalkClass->ClearMenus();
    return tmpscript->pGOChooseReward(pPlayer, pGO, pQuest, opt);
}

bool ScriptMgr::AreaTrigger(Player* pPlayer, AreaTriggerEntry const* atEntry)
{
    Script* tmpscript = m_scripts[GetAreaTriggerScriptId(atEntry->id)];
    if (!tmpscript || !tmpscript->pAreaTrigger) return false;

    return tmpscript->pAreaTrigger(pPlayer, atEntry);
}

CreatureAI* ScriptMgr::GetAI(Creature* pCreature)
{
    Script* tmpscript = m_scripts[pCreature->GetScriptId()];
    if (!tmpscript || !tmpscript->GetAI) return NULL;

    return tmpscript->GetAI(pCreature);
}

bool ScriptMgr::ItemUse(Player* pPlayer, Item* pItem, SpellCastTargets const& targets)
{
    Script* tmpscript = m_scripts[pItem->GetProto()->ScriptId];
    if (!tmpscript || !tmpscript->pItemUse) return false;

    return tmpscript->pItemUse(pPlayer, pItem, targets);
}

bool ScriptMgr::EffectDummyCreature(Unit* caster, uint32 spellId, uint32 effIndex, Creature* crTarget)
{
    Script* tmpscript = m_scripts[crTarget->GetScriptId()];

    if (!tmpscript || !tmpscript->pEffectDummyCreature) return false;

    return tmpscript->pEffectDummyCreature(caster, spellId, effIndex, crTarget);
}

InstanceData* ScriptMgr::CreateInstanceData(Map* map)
{
    if (!map->IsDungeon()) return NULL;

    Script* tmpscript = m_scripts[((InstanceMap*)map)->GetScriptId()];
    if (!tmpscript || !tmpscript->GetInstanceData) return NULL;

    return tmpscript->GetInstanceData(map);
}



