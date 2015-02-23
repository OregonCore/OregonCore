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

#include "Object.h"
#include "Player.h"
#include "BattleGround.h"
#include "Creature.h"
#include "MapManager.h"
#include "Language.h"
#include "Chat.h"
#include "SpellAuras.h"
#include "ArenaTeam.h"
#include "World.h"
#include "Util.h"
#include "GridNotifiersImpl.h"

namespace Oregon
{
class BattleGroundChatBuilder
{
    public:
        BattleGroundChatBuilder(ChatMsg msgtype, int32 textId, Player const* source, va_list* args = NULL)
            : i_msgtype(msgtype), i_textId(textId), i_source(source), i_args(args) {}
        void operator()(WorldPacket& data, int32 loc_idx)
        {
            char const* text = sObjectMgr.GetOregonString(i_textId, loc_idx);

            if (i_args)
            {
                // we need copy va_list before use or original va_list will corrupted
                va_list ap;
                va_copy(ap, *i_args);

                char str [2048];
                vsnprintf(str, 2048, text, ap );
                va_end(ap);

                do_helper(data, &str[0]);
            }
            else
                do_helper(data, text);
        }
    private:
        void do_helper(WorldPacket& data, char const* text)
        {
            uint64 target_guid = i_source ? i_source ->GetGUID() : 0;

            data << uint8(i_msgtype);
            data << uint32(LANG_UNIVERSAL);
            data << uint64(target_guid);                // there 0 for BG messages
            data << uint32(0);                          // can be chat msg group or something
            data << uint64(target_guid);
            data << uint32(strlen(text) + 1);
            data << text;
            data << uint8(i_source ? i_source->chatTag() : uint8(0));
        }

        ChatMsg i_msgtype;
        int32 i_textId;
        Player const* i_source;
        va_list* i_args;
};

class BattleGround2ChatBuilder
{
    public:
        BattleGround2ChatBuilder(ChatMsg msgtype, int32 textId, Player const* source, int32 arg1, int32 arg2)
            : i_msgtype(msgtype), i_textId(textId), i_source(source), i_arg1(arg1), i_arg2(arg2) {}
        void operator()(WorldPacket& data, int32 loc_idx)
        {
            char const* text = sObjectMgr.GetOregonString(i_textId, loc_idx);
            char const* arg1str = i_arg1 ? sObjectMgr.GetOregonString(i_arg1, loc_idx) : "";
            char const* arg2str = i_arg2 ? sObjectMgr.GetOregonString(i_arg2, loc_idx) : "";

            char str [2048];
            snprintf(str, 2048, text, arg1str, arg2str );

            uint64 target_guid = i_source ? i_source ->GetGUID() : 0;

            data << uint8(i_msgtype);
            data << uint32(LANG_UNIVERSAL);
            data << uint64(target_guid);                // there 0 for BG messages
            data << uint32(0);                          // can be chat msg group or something
            data << uint64(target_guid);
            data << uint32(strlen(str) + 1);
            data << str;
            data << uint8(i_source ? i_source->chatTag() : uint8(0));
        }
    private:

        ChatMsg i_msgtype;
        int32 i_textId;
        Player const* i_source;
        int32 i_arg1;
        int32 i_arg2;
};
}                                                           // namespace Oregon

template<class Do>
void BattleGround::BroadcastWorker(Do& _do)
{
    for (std::map<uint64, BattleGroundPlayer>::iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
        if (Player* plr = ObjectAccessor::FindPlayer(MAKE_NEW_GUID(itr->first, 0, HIGHGUID_PLAYER)))
            _do(plr);
}

BattleGround::BattleGround()
{
    m_TypeID            = 0;
    m_InstanceID        = 0;
    m_Status            = 0;
    m_EndTime           = 0;
    m_LastResurrectTime = 0;
    m_Queue_type        = MAX_BATTLEGROUND_QUEUES;
    m_InvitedAlliance   = 0;
    m_InvitedHorde      = 0;
    m_ArenaType         = 0;
    m_IsArena           = false;
    m_Winner            = 2;
    m_StartTime         = 0;
    m_ValidStartPositionTimer = 0;
    m_Events            = 0;
    m_IsRated           = false;
    m_BuffChange        = false;
    m_Name              = "";
    m_LevelMin          = 0;
    m_LevelMax          = 0;
    m_InBGFreeSlotQueue = false;
    m_SetDeleteThis     = false;

    m_MaxPlayersPerTeam = 0;
    m_MaxPlayers        = 0;
    m_MinPlayersPerTeam = 0;
    m_MinPlayers        = 0;

    m_MapId             = 0;
    m_Map               = NULL;
    m_StartMaxDist      = 0.0f;

    m_TeamStartLocX[BG_TEAM_ALLIANCE]   = 0;
    m_TeamStartLocX[BG_TEAM_HORDE]      = 0;

    m_TeamStartLocY[BG_TEAM_ALLIANCE]   = 0;
    m_TeamStartLocY[BG_TEAM_HORDE]      = 0;

    m_TeamStartLocZ[BG_TEAM_ALLIANCE]   = 0;
    m_TeamStartLocZ[BG_TEAM_HORDE]      = 0;

    m_TeamStartLocO[BG_TEAM_ALLIANCE]   = 0;
    m_TeamStartLocO[BG_TEAM_HORDE]      = 0;

    m_ArenaTeamIds[BG_TEAM_ALLIANCE]   = 0;
    m_ArenaTeamIds[BG_TEAM_HORDE]      = 0;

    m_ArenaTeamRatingChanges[BG_TEAM_ALLIANCE]   = 0;
    m_ArenaTeamRatingChanges[BG_TEAM_HORDE]      = 0;

    m_BgRaids[BG_TEAM_ALLIANCE]         = NULL;
    m_BgRaids[BG_TEAM_HORDE]            = NULL;

    m_PlayersCount[BG_TEAM_ALLIANCE]    = 0;
    m_PlayersCount[BG_TEAM_HORDE]       = 0;

    m_TeamScores[BG_TEAM_ALLIANCE]      = 0;
    m_TeamScores[BG_TEAM_HORDE]         = 0;

    m_PrematureCountDown = false;
    m_PrematureCountDown = 0;

    m_HonorMode = BG_NORMAL;

    m_StartDelayTimes[BG_STARTING_EVENT_FIRST]  = BG_START_DELAY_2M;
    m_StartDelayTimes[BG_STARTING_EVENT_SECOND] = BG_START_DELAY_1M;
    m_StartDelayTimes[BG_STARTING_EVENT_THIRD]  = BG_START_DELAY_30S;
    m_StartDelayTimes[BG_STARTING_EVENT_FOURTH] = BG_START_DELAY_NONE;
    //we must set to some default existing values
    m_StartMessageIds[BG_STARTING_EVENT_FIRST]  = LANG_BG_WS_START_TWO_MINUTES;
    m_StartMessageIds[BG_STARTING_EVENT_SECOND] = LANG_BG_WS_START_ONE_MINUTE;
    m_StartMessageIds[BG_STARTING_EVENT_THIRD]  = LANG_BG_WS_START_HALF_MINUTE;
    m_StartMessageIds[BG_STARTING_EVENT_FOURTH] = LANG_BG_WS_HAS_BEGUN;
}

BattleGround::~BattleGround()
{
    // remove objects and creatures
    // (this is done automatically in mapmanager update, when the instance is reset after the reset time)
    int size = m_BgCreatures.size();
    for (int i = 0; i < size; ++i)
        DelCreature(i);

    size = m_BgObjects.size();
    for (int i = 0; i < size; ++i)
        DelObject(i);

    if (GetInstanceID())                                     // not spam by useless queries in case BG templates
    {
        // delete creature and go respawn times
        WorldDatabase.PExecute("DELETE FROM creature_respawn WHERE instance = '%u'", GetInstanceID());
        WorldDatabase.PExecute("DELETE FROM gameobject_respawn WHERE instance = '%u'", GetInstanceID());
        // delete instance from db
        CharacterDatabase.PExecute("DELETE FROM instance WHERE id = '%u'", GetInstanceID());
        // remove from battlegrounds
    }

    sBattleGroundMgr.RemoveBattleGround(GetInstanceID());
    // unload map
    if (m_Map)
        m_Map->SetUnload();
    // remove from bg free slot queue
    this->RemoveFromBGFreeSlotQueue();

    for (BattleGroundScoreMap::const_iterator itr = m_PlayerScores.begin(); itr != m_PlayerScores.end(); ++itr)
        delete itr->second;
}

void BattleGround::Update(uint32 diff)
{
    if (!GetPlayersSize() && !GetRemovedPlayersSize() && !GetReviveQueueSize())
        //BG is empty
        return;

    m_StartTime += diff;

    // WorldPacket data;

    if (GetRemovedPlayersSize())
    {
        for (std::map<uint64, uint8>::iterator itr = m_RemovedPlayers.begin(); itr != m_RemovedPlayers.end(); ++itr)
        {
            Player* plr = sObjectMgr.GetPlayer(itr->first);
            switch (itr->second)
            {
            //following code is handled by event:
            /*case 0:
                sBattleGroundMgr.m_BattleGroundQueues[GetTypeID()].RemovePlayer(itr->first);
                //RemovePlayerFromQueue(itr->first);
                if (plr)
                {
                    sBattleGroundMgr.BuildBattleGroundStatusPacket(&data, this, plr->GetTeam(), plr->GetBattleGroundQueueIndex(m_TypeID), STATUS_NONE, 0, 0);
                    plr->GetSession()->SendPacket(&data);
                }
                break;*/
            case 1:                                     // currently in bg and was removed from bg
                if (plr)
                    RemovePlayerAtLeave(itr->first, true, true);
                else
                    RemovePlayerAtLeave(itr->first, false, false);
                break;
            case 2:                                     // revive queue
                RemovePlayerFromResurrectQueue(itr->first);
                break;
            default:
                sLog.outError("BattleGround: Unknown remove player case!");
            }
        }
        m_RemovedPlayers.clear();
    }

    // remove offline players from bg after 5 minutes
    if (GetPlayersSize())
    {
        for (std::map<uint64, BattleGroundPlayer>::iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
        {
            Player* plr = sObjectMgr.GetPlayer(itr->first);
            itr->second.LastOnlineTime += diff;

            if (plr)
            {
                itr->second.LastOnlineTime = 0;                 // update last online time
                if (!plr->IsPvP())                              // force all players PvP on
                    plr->UpdatePvP(true, true);
            }
            else if (itr->second.LastOnlineTime >= MAX_OFFLINE_TIME)
                m_RemovedPlayers[itr->first] = 1;           // add to remove list (BG)
        }
    }

    /*********************************************************/
    /***        BATTLEGROUND RESSURECTION SYSTEM           ***/
    /*********************************************************/

    //this should be handled by spell system
    m_LastResurrectTime += diff;
    if (m_LastResurrectTime >= RESURRECTION_INTERVAL)
    {
        if (GetReviveQueueSize())
        {
            for (std::map<uint64, std::vector<uint64> >::iterator itr = m_ReviveQueue.begin(); itr != m_ReviveQueue.end(); ++itr)
            {
                Creature* sh = NULL;
                for (std::vector<uint64>::iterator itr2 = (itr->second).begin(); itr2 != (itr->second).end(); ++itr2)
                {
                    Player* plr = sObjectMgr.GetPlayer(*itr2);
                    if (!plr)
                        continue;

                    if (!sh && plr->IsInWorld())
                    {
                        sh = plr->GetMap()->GetCreature(itr->first);
                        // only for visual effect
                        if (sh)
                            sh->CastSpell(sh, SPELL_SPIRIT_HEAL, true);   // Spirit Heal, effect 117
                    }

                    plr->CastSpell(plr, SPELL_RESURRECTION_VISUAL, true);   // Resurrection visual
                    m_ResurrectQueue.push_back(*itr2);
                }
                (itr->second).clear();
            }

            m_ReviveQueue.clear();
            m_LastResurrectTime = 0;
        }
        else
            // queue is clear and time passed, just update last resurrection time
            m_LastResurrectTime = 0;
    }
    else if (m_LastResurrectTime > 500)    // Resurrect players only half a second later, to see spirit heal effect on NPC
    {
        for (std::vector<uint64>::iterator itr = m_ResurrectQueue.begin(); itr != m_ResurrectQueue.end(); ++itr)
        {
            Player* plr = sObjectMgr.GetPlayer(*itr);
            if (!plr)
                continue;
            plr->ResurrectPlayer(1.0f);
            plr->CastSpell(plr, SPELL_SPIRIT_HEAL_MANA, true);
            ObjectAccessor::Instance().ConvertCorpseForPlayer(*itr);
        }
        m_ResurrectQueue.clear();
    }

    /*********************************************************/
    /***          BATTLEGROUND BALLANCE SYSTEM            ***/
    /*********************************************************/

    // if less then minimum players are in on one side, then start premature finish timer
    if (GetStatus() == STATUS_IN_PROGRESS && !isArena() && sBattleGroundMgr.GetPrematureFinishTime() && (GetPlayersCountByTeam(ALLIANCE) < GetMinPlayersPerTeam() || GetPlayersCountByTeam(HORDE) < GetMinPlayersPerTeam()))
    {
        if (!m_PrematureCountDown)
        {
            m_PrematureCountDown = true;
            m_PrematureCountDownTimer = sBattleGroundMgr.GetPrematureFinishTime();
        }
        else if (m_PrematureCountDownTimer < diff)
        {
            // time's up!
            EndBattleGround(0); // noone wins
            m_PrematureCountDown = false;
        }
        else if (!sBattleGroundMgr.isTesting())
        {
            uint32 newtime = m_PrematureCountDownTimer - diff;
            // announce every minute
            if (newtime > (MINUTE * IN_MILLISECONDS))
            {
                if (newtime / (MINUTE * IN_MILLISECONDS) != m_PrematureCountDownTimer / (MINUTE * IN_MILLISECONDS))
                    PSendMessageToAll(LANG_BATTLEGROUND_PREMATURE_FINISH_WARNING, CHAT_MSG_SYSTEM, NULL, (uint32)(m_PrematureCountDownTimer / (MINUTE * IN_MILLISECONDS)));
            }
            else
            {
                //announce every 15 seconds
                if (newtime / (15 * IN_MILLISECONDS) != m_PrematureCountDownTimer / (15 * IN_MILLISECONDS))
                    PSendMessageToAll(LANG_BATTLEGROUND_PREMATURE_FINISH_WARNING_SECS, CHAT_MSG_SYSTEM, NULL, (uint32)(m_PrematureCountDownTimer / IN_MILLISECONDS));
            }
            m_PrematureCountDownTimer = newtime;
        }
    }
    else if (m_PrematureCountDown)
        m_PrematureCountDown = false;

    /*********************************************************/
    /***          BATTLEGROUND STARTING SYSTEM            ***/
    /*********************************************************/

    if (GetStatus() == STATUS_WAIT_JOIN && GetPlayersSize())
    {
        ModifyStartDelayTime(diff);
        _CheckSafePositions(diff);

        if (!(m_Events & BG_STARTING_EVENT_1))
        {
            m_Events |= BG_STARTING_EVENT_1;

            // setup here, only when at least one player has ported to the map
            if (!SetupBattleGround())
            {
                EndNow();
                return;
            }

            StartingEventCloseDoors();
            SetStartDelayTime(m_StartDelayTimes[BG_STARTING_EVENT_FIRST]);
            //first start warning - 2 or 1 minute
            SendMessageToAll(m_StartMessageIds[BG_STARTING_EVENT_FIRST], CHAT_MSG_BG_SYSTEM_NEUTRAL);
        }
        // After 1 minute or 30 seconds, warning is signalled
        else if (GetStartDelayTime() <= m_StartDelayTimes[BG_STARTING_EVENT_SECOND] && !(m_Events & BG_STARTING_EVENT_2))
        {
            m_Events |= BG_STARTING_EVENT_2;
            SendMessageToAll(m_StartMessageIds[BG_STARTING_EVENT_SECOND], CHAT_MSG_BG_SYSTEM_NEUTRAL);
        }
        // After 30 or 15 seconds, warning is signalled
        else if (GetStartDelayTime() <= m_StartDelayTimes[BG_STARTING_EVENT_THIRD] && !(m_Events & BG_STARTING_EVENT_3))
        {
            m_Events |= BG_STARTING_EVENT_3;
            SendMessageToAll(m_StartMessageIds[BG_STARTING_EVENT_THIRD], CHAT_MSG_BG_SYSTEM_NEUTRAL);
        }
        // delay expired (after 2 or 1 minute)
        else if (GetStartDelayTime() <= 0 && !(m_Events & BG_STARTING_EVENT_4))
        {
            m_Events |= BG_STARTING_EVENT_4;

            StartingEventOpenDoors();

            SendMessageToAll(m_StartMessageIds[BG_STARTING_EVENT_FOURTH], CHAT_MSG_BG_SYSTEM_NEUTRAL);
            SetStatus(STATUS_IN_PROGRESS);
            SetStartDelayTime(m_StartDelayTimes[BG_STARTING_EVENT_FOURTH]);

            //remove preparation
            if (isArena())
            {
                for (BattleGroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
                    if (Player* plr = sObjectMgr.GetPlayer(itr->first))
                        plr->RemoveAurasDueToSpell(SPELL_ARENA_PREPARATION);

                CheckArenaWinConditions();
            }
            else
            {
                PlaySoundToAll(SOUND_BG_START);

                for (BattleGroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
                    if (Player* plr = sObjectMgr.GetPlayer(itr->first))
                        plr->RemoveAurasDueToSpell(SPELL_PREPARATION);

                //Announce BG starting
                Announce();
            }
        }
    }

    /*********************************************************/
    /***          BATTLEGROUND ENDING SYSTEM              ***/
    /*********************************************************/

    if (GetStatus() == STATUS_WAIT_LEAVE)
    {
        // remove all players from battleground after 2 minutes
        m_EndTime += diff;
        if (m_EndTime >= TIME_TO_AUTOREMOVE)                 // 2 minutes
        {
            for (std::map<uint64, BattleGroundPlayer>::iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
            {
                m_RemovedPlayers[itr->first] = 1;           // add to remove list (BG)
            }
            // do not change any battleground's private variables
        }
    }
}

void BattleGround::SetTeamStartLoc(uint32 TeamID, float X, float Y, float Z, float O)
{
    uint8 idx = GetTeamIndexByTeamId(TeamID);
    m_TeamStartLocX[idx] = X;
    m_TeamStartLocY[idx] = Y;
    m_TeamStartLocZ[idx] = Z;
    m_TeamStartLocO[idx] = O;
}

void BattleGround::SendPacketToAll(WorldPacket* packet)
{
    for (std::map<uint64, BattleGroundPlayer>::iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
    {
        if (itr->second.LastOnlineTime)
            continue;

        Player* plr = sObjectMgr.GetPlayer(itr->first);
        if (plr)
            plr->GetSession()->SendPacket(packet);
        else
            sLog.outError("BattleGround: Player (GUID: %u) not found!", GUID_LOPART(itr->first));
    }
}

void BattleGround::SendPacketToTeam(uint32 TeamID, WorldPacket* packet, Player* sender, bool self)
{
    for (std::map<uint64, BattleGroundPlayer>::iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
    {
        if (itr->second.LastOnlineTime)
            continue;

        Player* plr = sObjectMgr.GetPlayer(itr->first);

        if (!plr)
        {
            sLog.outError("BattleGround: Player (GUID: %u) not found!", GUID_LOPART(itr->first));
            continue;
        }

        if (!self && sender == plr)
            continue;

        uint32 team = itr->second.Team;
        if (!team) team = plr->GetTeam();

        if (team == TeamID)
            plr->GetSession()->SendPacket(packet);
    }
}

void BattleGround::PlaySoundToAll(uint32 SoundID)
{
    WorldPacket data;
    sBattleGroundMgr.BuildPlaySoundPacket(&data, SoundID);
    SendPacketToAll(&data);
}

void BattleGround::PlaySoundToTeam(uint32 SoundID, uint32 TeamID)
{
    WorldPacket data;

    for (std::map<uint64, BattleGroundPlayer>::iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
    {
        if (itr->second.LastOnlineTime)
            continue;

        Player* plr = sObjectMgr.GetPlayer(itr->first);

        if (!plr)
        {
            sLog.outError("BattleGround: Player (GUID: %u) not found!", GUID_LOPART(itr->first));
            continue;
        }

        uint32 team = itr->second.Team;
        if (!team) team = plr->GetTeam();

        if (team == TeamID)
        {
            sBattleGroundMgr.BuildPlaySoundPacket(&data, SoundID);
            plr->GetSession()->SendPacket(&data);
        }
    }
}

void BattleGround::CastSpellOnTeam(uint32 SpellID, uint32 TeamID)
{
    for (std::map<uint64, BattleGroundPlayer>::iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
    {
        if (itr->second.LastOnlineTime)
            continue;

        Player* plr = sObjectMgr.GetPlayer(itr->first);

        if (!plr)
        {
            sLog.outError("BattleGround: Player (GUID: %u) not found!", GUID_LOPART(itr->first));
            continue;
        }

        uint32 team = itr->second.Team;
        if (!team) team = plr->GetTeam();

        if (team == TeamID)
            plr->CastSpell(plr, SpellID, true);
    }
}

void BattleGround::YellToAll(Creature* creature, const char* text, uint32 language)
{
    for (std::map<uint64, BattleGroundPlayer>::iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
    {
        if (itr->second.LastOnlineTime)
            continue;

        WorldPacket data(SMSG_MESSAGECHAT, 200);
        Player* plr = sObjectMgr.GetPlayer(itr->first);
        if (!plr)
        {
            sLog.outError("BattleGround: Player (GUID: %u) not found!", GUID_LOPART(itr->first));
            continue;
        }
        creature->BuildMonsterChat(&data, CHAT_MSG_MONSTER_YELL, text, language, creature->GetName(), itr->first);
        plr->GetSession()->SendPacket(&data);
    }
}


void BattleGround::RewardHonorToTeam(uint32 Honor, uint32 TeamID)
{
    for (std::map<uint64, BattleGroundPlayer>::iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
    {
        if (itr->second.LastOnlineTime)
            continue;

        Player* plr = sObjectMgr.GetPlayer(itr->first);

        if (!plr)
        {
            sLog.outError("BattleGround: Player (GUID: %u) not found!", GUID_LOPART(itr->first));
            continue;
        }

        uint32 team = itr->second.Team;
        if (!team) team = plr->GetTeam();

        if (team == TeamID)
            UpdatePlayerScore(plr, SCORE_BONUS_HONOR, Honor);
    }
}

void BattleGround::RewardReputationToTeam(uint32 faction_id, uint32 Reputation, uint32 TeamID)
{
    FactionEntry const* factionEntry = sFactionStore.LookupEntry(faction_id);
    if (!factionEntry)
        return;

    for (std::map<uint64, BattleGroundPlayer>::iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
    {
        if (itr->second.LastOnlineTime)
            continue;

        Player* plr = sObjectMgr.GetPlayer(itr->first);

        if (!plr)
        {
            sLog.outError("BattleGround: Player (GUID: %u) not found!", GUID_LOPART(itr->first));
            continue;
        }

        uint32 team = itr->second.Team;
        if (!team) team = plr->GetTeam();

        if (team == TeamID)
        {
            uint32 repGain = Reputation;
            AddPct(repGain, plr->GetTotalAuraModifier(SPELL_AURA_MOD_REPUTATION_GAIN));
            AddPct(repGain, plr->GetTotalAuraModifierByMiscValue(SPELL_AURA_MOD_FACTION_REPUTATION_GAIN, faction_id));
            plr->ModifyFactionReputation(factionEntry, Reputation);
        }
    }
}

void BattleGround::UpdateWorldState(uint32 Field, uint32 Value)
{
    WorldPacket data;
    sBattleGroundMgr.BuildUpdateWorldStatePacket(&data, Field, Value);
    SendPacketToAll(&data);
}

void BattleGround::UpdateWorldStateForPlayer(uint32 Field, uint32 Value, Player* Source)
{
    WorldPacket data;
    sBattleGroundMgr.BuildUpdateWorldStatePacket(&data, Field, Value);
    Source->GetSession()->SendPacket(&data);
}

void BattleGround::EndBattleGround(uint32 winner)
{
    this->RemoveFromBGFreeSlotQueue();
    uint32 almost_winning_team = HORDE;
    ArenaTeam* winner_arena_team = NULL;
    ArenaTeam* loser_arena_team = NULL;
    uint32 loser_rating = 0;
    uint32 winner_rating = 0;
    WorldPacket data;
    int32 winmsg_id = 0;

    if (winner == ALLIANCE)
    {
        winmsg_id = isBattleGround() ? LANG_BG_A_WINS : LANG_ARENA_GOLD_WINS;

        PlaySoundToAll(SOUND_ALLIANCE_WINS);                // alliance wins sound

        SetWinner(WINNER_ALLIANCE);
    }
    else if (winner == HORDE)
    {
        winmsg_id = isBattleGround() ? LANG_BG_H_WINS : LANG_ARENA_GREEN_WINS;

        PlaySoundToAll(SOUND_HORDE_WINS);                   // horde wins sound

        SetWinner(WINNER_HORDE);
    }
    else
        SetWinner(3);

    SetStatus(STATUS_WAIT_LEAVE);
    m_EndTime = 0;

    // arena rating calculation
    if (isArena() && isRated())
    {
        if (winner == ALLIANCE)
        {
            winner_arena_team = sObjectMgr.GetArenaTeamById(GetArenaTeamIdForTeam(ALLIANCE));
            loser_arena_team = sObjectMgr.GetArenaTeamById(GetArenaTeamIdForTeam(HORDE));
        }
        else if (winner == HORDE)
        {
            winner_arena_team = sObjectMgr.GetArenaTeamById(GetArenaTeamIdForTeam(HORDE));
            loser_arena_team = sObjectMgr.GetArenaTeamById(GetArenaTeamIdForTeam(ALLIANCE));
        }
        if (winner_arena_team && loser_arena_team)
        {
            loser_rating = loser_arena_team->GetStats().rating;
            winner_rating = winner_arena_team->GetStats().rating;
            int32 winner_change = winner_arena_team->WonAgainst(loser_rating);
            int32 loser_change = loser_arena_team->LostAgainst(winner_rating);
            sLog.outDebug("--- Winner rating: %u, Loser rating: %u, Winner change: %u, Losser change: %u ---", winner_rating, loser_rating, winner_change, loser_change);
            if (winner == ALLIANCE)
            {
                SetArenaTeamRatingChangeForTeam(ALLIANCE, winner_change);
                SetArenaTeamRatingChangeForTeam(HORDE, loser_change);
            }
            else
            {
                SetArenaTeamRatingChangeForTeam(HORDE, winner_change);
                SetArenaTeamRatingChangeForTeam(ALLIANCE, loser_change);
            }
            sLog.outArena("Arena match Type: %u for Team1Id: %u - Team2Id: %u ended. WinnerTeamId: %u. Winner rating: %u, Loser rating: %u. RatingChange: %i.", m_ArenaType, m_ArenaTeamIds[BG_TEAM_ALLIANCE], m_ArenaTeamIds[BG_TEAM_HORDE], winner_arena_team->GetId(), winner_rating, loser_rating, winner_change);
            if (sWorld.getConfig(CONFIG_ARENA_LOG_EXTENDED_INFO))
                for (BattleGround::BattleGroundScoreMap::const_iterator itr = GetPlayerScoresBegin(); itr != GetPlayerScoresEnd(); ++itr)
                    if (Player* player = sObjectMgr.GetPlayer(itr->first))
                        sLog.outArena("Statistics for %s (GUID: " UI64FMTD ", Team: %d, IP: %s): %u damage, %u healing, %u killing blows", player->GetName(), itr->first, player->GetArenaTeamId(m_ArenaType == 5 ? 2 : m_ArenaType == 3), player->GetSession()->GetRemoteAddress().c_str(), itr->second->DamageDone, itr->second->HealingDone, itr->second->KillingBlows);
        }
        else
        {
            SetArenaTeamRatingChangeForTeam(ALLIANCE, 0);
            SetArenaTeamRatingChangeForTeam(HORDE, 0);
        }
    }

    if (!isArena())
    {

        if (m_score[GetTeamIndexByTeamId(ALLIANCE)] == m_score[GetTeamIndexByTeamId(HORDE)])
            almost_winning_team = 0;         //no real winner
        if (m_score[GetTeamIndexByTeamId(ALLIANCE)] > m_score[GetTeamIndexByTeamId(HORDE)])
            almost_winning_team = ALLIANCE;

    }

    for (std::map<uint64, BattleGroundPlayer>::iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
    {
        Player* plr = sObjectMgr.GetPlayer(itr->first);
        uint32 team = itr->second.Team;

        if (!plr)
        {
            //if rated arena match - make member lost!
            if (isArena() && isRated() && winner_arena_team && loser_arena_team)
            {
                if (team == winner)
                    winner_arena_team->OfflineMemberLost(itr->first, loser_rating);
                else
                    loser_arena_team->OfflineMemberLost(itr->first, winner_rating);
            }
            sLog.outError("BattleGround: Player (GUID: %u) not found!", GUID_LOPART(itr->first));
            continue;
        }

        // should remove spirit of redemption
        if (plr->HasAuraType(SPELL_AURA_SPIRIT_OF_REDEMPTION))
            plr->RemoveSpellsCausingAura(SPELL_AURA_MOD_SHAPESHIFT);

        if (!plr->IsAlive())
        {
            plr->ResurrectPlayer(1.0f);
            plr->SpawnCorpseBones();
        }

        //this line is obsolete - team is set ALWAYS
        //if (!team) team = plr->GetTeam();

        // per player calculation
        if (isArena() && isRated() && winner_arena_team && loser_arena_team)
        {
            if (team == winner)
                winner_arena_team->MemberWon(plr, loser_rating);
            else
                loser_arena_team->MemberLost(plr, winner_rating);
        }

        if (team == winner)
        {
            RewardMark(plr, ITEM_WINNER_COUNT);
            UpdatePlayerScore(plr, SCORE_BONUS_HONOR, 20);
            RewardQuest(plr);
        }
        else if (winner != 0)
            RewardMark(plr, ITEM_LOSER_COUNT);
        else if (winner == 0)
        {
            if (sWorld.getConfig(CONFIG_BATTLEGROUND_PREMATURE_REWARD))
            {
                if (almost_winning_team == team)                  // player's team had more points
                    RewardMark(plr, ITEM_WINNER_COUNT);
                else
                    RewardMark(plr, ITEM_LOSER_COUNT);           // if scores were the same, each team gets 1 mark.
            }
        }

        plr->SetHealth(plr->GetMaxHealth());
        plr->SetPower(POWER_MANA, plr->GetMaxPower(POWER_MANA));
        plr->CombatStopWithPets(true);

        BlockMovement(plr);

        sBattleGroundMgr.BuildPvpLogDataPacket(&data, this);
        plr->GetSession()->SendPacket(&data);

        uint32 bgQueueTypeId = sBattleGroundMgr.BGQueueTypeId(GetTypeID(), GetArenaType());
        sBattleGroundMgr.BuildBattleGroundStatusPacket(&data, this, plr->GetTeam(), plr->GetBattleGroundQueueIndex(bgQueueTypeId), STATUS_IN_PROGRESS, TIME_TO_AUTOREMOVE, GetStartTime());
        plr->GetSession()->SendPacket(&data);
    }

    if (isArena() && isRated() && winner_arena_team && loser_arena_team)
    {
        // update arena points only after increasing the player's match count!
        //obsolete: winner_arena_team->UpdateArenaPointsHelper();
        //obsolete: loser_arena_team->UpdateArenaPointsHelper();
        // save the stat changes
        winner_arena_team->SaveToDB();
        loser_arena_team->SaveToDB();
        // send updated arena team stats to players
        // this way all arena team members will get notified, not only the ones who participated in this match
        winner_arena_team->NotifyStatsChanged();
        loser_arena_team->NotifyStatsChanged();
    }

    // inform invited players about the removal
    sBattleGroundMgr.m_BattleGroundQueues[sBattleGroundMgr.BGQueueTypeId(GetTypeID(), GetArenaType())].BGEndedRemoveInvites(this);

    if (winmsg_id)
        SendMessageToAll(winmsg_id, CHAT_MSG_BG_SYSTEM_NEUTRAL);
}

uint32 BattleGround::GetBattlemasterEntry() const
{
    switch (GetTypeID())
    {
    case BATTLEGROUND_AV:
        return 15972;
    case BATTLEGROUND_WS:
        return 14623;
    case BATTLEGROUND_AB:
        return 14879;
    case BATTLEGROUND_EY:
        return 22516;
    case BATTLEGROUND_NA:
        return 20200;
    default:
        return 0;
    }
}

void BattleGround::RewardMark(Player* plr, uint32 count)
{
    // 'Inactive' this aura prevents the player from gaining honor points and battleground tokens
    if (plr->GetDummyAura(SPELL_AURA_PLAYER_INACTIVE))
        return;

    if (!plr || !count)
        return;

    BattleGroundMarks mark;
    switch (GetTypeID())
    {
    case BATTLEGROUND_AV:
        mark = ITEM_AV_MARK_OF_HONOR;
        break;
    case BATTLEGROUND_WS:
        mark = ITEM_WS_MARK_OF_HONOR;
        break;
    case BATTLEGROUND_AB:
        mark = ITEM_AB_MARK_OF_HONOR;
        break;
    case BATTLEGROUND_EY:
        mark = ITEM_EY_MARK_OF_HONOR;
        break;
    default:
        return;
    }

    if (sObjectMgr.GetItemPrototype(mark))
    {
        ItemPosCountVec dest;
        uint32 no_space_count = 0;
        uint8 msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, mark, count, &no_space_count);
        if (msg != EQUIP_ERR_OK)                       // convert to possible store amount
            count -= no_space_count;

        if (!dest.empty())                // can add some
            if (Item* item = plr->StoreNewItem(dest, mark, true, 0))
                plr->SendNewItem(item, count, false, true);

        if (no_space_count > 0)
            SendRewardMarkByMail(plr, mark, no_space_count);
    }
}

void BattleGround::SendRewardMarkByMail(Player* plr, uint32 mark, uint32 count)
{
    uint32 bmEntry = GetBattlemasterEntry();
    if (!bmEntry)
        return;

    ItemPrototype const* markProto = sObjectMgr.GetItemPrototype(mark);
    if (!markProto)
        return;

    if (Item* markItem = Item::CreateItem(mark, count, plr))
    {
        // save new item before send
        markItem->SaveToDB();                               // save for prevent lost at next mail load, if send fail then item will deleted

        // subject: item name
        std::string subject = markProto->Name1;
        int loc_idx = plr->GetSession()->GetSessionDbLocaleIndex();
        if (loc_idx >= 0)
            if (ItemLocale const* il = sObjectMgr.GetItemLocale(markProto->ItemId))
                if (il->Name.size() > size_t(loc_idx) && !il->Name[loc_idx].empty())
                    subject = il->Name[loc_idx];

        // text
        std::string textFormat = plr->GetSession()->GetOregonString(LANG_BG_MARK_BY_MAIL);
        char textBuf[300];
        snprintf(textBuf, 300, textFormat.c_str(), GetName(), GetName());
        uint32 itemTextId = sObjectMgr.CreateItemText(textBuf);

        MailDraft(subject, itemTextId)
        .AddItem(markItem)
        .SendMailTo(plr, MailSender(MAIL_CREATURE, bmEntry));
    }
}

void BattleGround::RewardQuest(Player* plr)
{
    // 'Inactive' this aura prevents the player from gaining honor points and battleground tokens
    if (plr->GetDummyAura(SPELL_AURA_PLAYER_INACTIVE))
        return;

    uint32 quest;
    switch (GetTypeID())
    {
    case BATTLEGROUND_AV:
        quest = SPELL_AV_QUEST_REWARD;
        break;
    case BATTLEGROUND_WS:
        quest = SPELL_WS_QUEST_REWARD;
        break;
    case BATTLEGROUND_AB:
        quest = SPELL_AB_QUEST_REWARD;
        break;
    case BATTLEGROUND_EY:
        quest = SPELL_EY_QUEST_REWARD;
        break;
    default:
        return;
    }

    plr->CastSpell(plr, quest, true);
}

void BattleGround::BlockMovement(Player* plr)
{
    plr->SetClientControl(plr, 0);                          // movement disabled NOTE: the effect will be automatically removed by client when the player is teleported from the battleground, so no need to send with uint8(1) in RemovePlayerAtLeave()
}

void BattleGround::RemovePlayerAtLeave(uint64 guid, bool Transport, bool SendPacket)
{
    uint32 team = GetPlayerTeam(guid);
    bool participant = false;
    // Remove from lists/maps
    std::map<uint64, BattleGroundPlayer>::iterator itr = m_Players.find(guid);
    if (itr != m_Players.end())
    {
        UpdatePlayersCountByTeam(team, true);               // -1 player
        m_Players.erase(itr);
        // check if the player was a participant of the match, or only entered through gm command (goname)
        participant = true;
    }

    BattleGroundScoreMap::iterator itr2 = m_PlayerScores.find(guid);
    if (itr2 != m_PlayerScores.end())
    {
        delete itr2->second;                                // delete player's score
        m_PlayerScores.erase(itr2);
    }

    RemovePlayerFromResurrectQueue(guid);

    Player* player = sObjectMgr.GetPlayer(guid);

    if (player)
    {
        // should remove spirit of redemption
        if (player->HasAuraType(SPELL_AURA_SPIRIT_OF_REDEMPTION))
            player->RemoveSpellsCausingAura(SPELL_AURA_MOD_SHAPESHIFT);

        player->RemoveAurasByType(SPELL_AURA_MOUNTED);

        if (!player->IsAlive())                              // resurrect on exit
        {
            player->ResurrectPlayer(1.0f);
            player->SpawnCorpseBones();
        }
    } // try to resurrect the offline player. If he is alive, nothing will happen.
    else ObjectAccessor::Instance().ConvertCorpseForPlayer(guid);

    RemovePlayer(player, guid);                                // BG subclass specific code

    if (participant)
    {
        if (player)
        {
            player->ClearAfkReports();

            if (!team) team = player->GetTeam();

            uint32 bgQueueTypeId = sBattleGroundMgr.BGQueueTypeId(GetTypeID(), GetArenaType());
            // if arena, remove the specific arena auras
            if (isArena())
            {
                player->RemoveArenaAuras(true);    // removes debuffs / dots etc., we don't want the player to die after porting out

                // summon old pet if there was one and there isn't a current pet
                if (!player->GetGuardianPet() && player->GetTemporaryUnsummonedPetNumber())
                {
                    Pet* NewPet = new Pet(player);
                    if (!NewPet->LoadPetFromDB(player, 0, (player)->GetTemporaryUnsummonedPetNumber(), true))
                        delete NewPet;

                    (player)->SetTemporaryUnsummonedPetNumber(0);
                }

                if (isRated() && GetStatus() == STATUS_IN_PROGRESS)
                {
                    // left a rated match while the encounter was in progress, consider as loser
                    ArenaTeam* winner_arena_team = sObjectMgr.GetArenaTeamById(GetArenaTeamIdForTeam(GetOtherTeam(team)));
                    ArenaTeam* loser_arena_team = sObjectMgr.GetArenaTeamById(GetArenaTeamIdForTeam(team));
                    if (winner_arena_team && loser_arena_team)
                        loser_arena_team->MemberLost(player, winner_arena_team->GetRating());
                }
            }

            WorldPacket data;
            if (SendPacket)
            {
                sBattleGroundMgr.BuildBattleGroundStatusPacket(&data, this, team, player->GetBattleGroundQueueIndex(bgQueueTypeId), STATUS_NONE, 0, 0);
                player->GetSession()->SendPacket(&data);
            }

            // this call is important, because player, when joins to battleground, this method is not called, so it must be called when leaving bg
            player->RemoveBattleGroundQueueId(bgQueueTypeId);
        }
        else
        {
            if (isRated() && GetStatus() == STATUS_IN_PROGRESS)
            {
                // left a rated match while the encounter was in progress, consider as loser
                ArenaTeam* others_arena_team = sObjectMgr.GetArenaTeamById(GetArenaTeamIdForTeam(GetOtherTeam(team)));
                ArenaTeam* players_arena_team = sObjectMgr.GetArenaTeamById(GetArenaTeamIdForTeam(team));
                if (others_arena_team && players_arena_team)
                    players_arena_team->OfflineMemberLost(guid, others_arena_team->GetRating());
            }
        }

        // remove from raid group if player is member
        if (Group* group = GetBgRaid(team))
        {
            if (!group->RemoveMember(guid, 0))                      // group was disbanded
            {
                SetBgRaid(team, NULL);
                delete group;
            }
        }
        DecreaseInvitedCount(team);
        // we should update battleground queue, but only if bg isn't ending
        if (isBattleGround() && GetStatus() < STATUS_WAIT_LEAVE)
        {
            // a player has left the battleground, so there are free slots -> add to queue
            AddToBGFreeSlotQueue();
        }
        // Let others know
        WorldPacket data;
        sBattleGroundMgr.BuildPlayerLeftBattleGroundPacket(&data, guid);
        SendPacketToTeam(team, &data, player, false);
    }

    if (player)
    {
        // Do next only if found in battleground
        player->SetBattleGroundId(0);                          // We're not in BG.
        // reset destination bg team
        player->SetBGTeam(0);

        if (Transport)
            player->TeleportToBGEntryPoint();

        sLog.outDetail("BATTLEGROUND: Removed player %s from BattleGround.", player->GetName());
    }

    if (!GetPlayersSize() && !GetInvitedCount(HORDE) && !GetInvitedCount(ALLIANCE))
    {
        // if no players left AND no invitees left, set this bg to delete in next update
        // direct deletion could cause crashes
        m_SetDeleteThis = true;
        // return to prevent addition to freeslotqueue
        return;
    }
}

// this method is called when no players remains in battleground
void BattleGround::Reset()
{
    SetQueueType(MAX_BATTLEGROUND_QUEUES);
    SetWinner(WINNER_NONE);
    SetStatus(STATUS_WAIT_QUEUE);
    SetStartTime(0);
    SetEndTime(0);
    SetLastResurrectTime(0);
    SetArenaType(0);
    SetRated(false);

    m_Events = 0;

    if (m_InvitedAlliance > 0 || m_InvitedHorde > 0)
        sLog.outError("BattleGround system: bad counter, m_InvitedAlliance: %d, m_InvitedHorde: %d", m_InvitedAlliance, m_InvitedHorde);

    m_InvitedAlliance = 0;
    m_InvitedHorde = 0;
    m_InBGFreeSlotQueue = false;

    m_Players.clear();

    for (BattleGroundScoreMap::const_iterator itr = m_PlayerScores.begin(); itr != m_PlayerScores.end(); ++itr)
        delete itr->second;
    m_PlayerScores.clear();

    ResetBGSubclass();
}

void BattleGround::StartBattleGround()
{
    SetStartTime(0);
    SetLastResurrectTime(0);
    if (m_IsRated)
        sLog.outArena("Arena match type: %u for Team1Id: %u - Team2Id: %u started.", m_ArenaType, m_ArenaTeamIds[BG_TEAM_ALLIANCE], m_ArenaTeamIds[BG_TEAM_HORDE]);
}

void BattleGround::AddPlayer(Player* plr)
{
    // remove afk from player
    if (plr->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_AFK))
        plr->ToggleAFK();

    // score struct must be created in inherited class

    uint64 guid = plr->GetGUID();
    uint32 team = plr->GetBGTeam();

    BattleGroundPlayer bp;
    bp.LastOnlineTime = 0;
    bp.Team = team;

    // Add to list/maps
    m_Players[guid] = bp;

    UpdatePlayersCountByTeam(team, false);                  // +1 player

    WorldPacket data;
    sBattleGroundMgr.BuildPlayerJoinedBattleGroundPacket(&data, plr);
    SendPacketToTeam(team, &data, plr, false);

    plr->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);
    plr->CombatStop();
    plr->getHostileRefManager().deleteReferences();

    // add arena specific auras
    if (isArena())
    {
        plr->RemoveArenaSpellCooldowns();
        plr->RemoveArenaAuras();
        plr->RemoveAllEnchantments(TEMP_ENCHANTMENT_SLOT, true);
        if (team == ALLIANCE)                                // gold
        {
            if (plr->GetTeam() == HORDE)
                plr->CastSpell(plr, SPELL_HORDE_GOLD_FLAG, true);
            else
                plr->CastSpell(plr, SPELL_ALLIANCE_GOLD_FLAG, true);
        }
        else                                                // green
        {
            if (plr->GetTeam() == HORDE)
                plr->CastSpell(plr, SPELL_HORDE_GREEN_FLAG, true);
            else
                plr->CastSpell(plr, SPELL_ALLIANCE_GREEN_FLAG, true);
        }

        plr->DestroyConjuredItems(true);

        Pet* pet = plr->GetPet();
        if (pet)
        {
            if (pet->getPetType() == SUMMON_PET || pet->getPetType() == HUNTER_PET)
            {
                (plr)->SetTemporaryUnsummonedPetNumber(pet->GetCharmInfo()->GetPetNumber());
                (plr)->SetOldPetSpell(pet->GetUInt32Value(UNIT_CREATED_BY_SPELL));
            }
            (plr)->RemovePet(NULL, PET_SAVE_NOT_IN_SLOT);
        }
        else
            (plr)->SetTemporaryUnsummonedPetNumber(0);

        if (GetStatus() == STATUS_WAIT_JOIN)                 // not started yet
        {
            plr->CastSpell(plr, SPELL_ARENA_PREPARATION, true);
            plr->ResetAllPowers();
        }
    }
    else
    {
        if (GetStatus() == STATUS_WAIT_JOIN)                 // not started yet
            plr->CastSpell(plr, SPELL_PREPARATION, true);   // reduces all mana cost of spells.
    }

    // Log
    sLog.outDetail("BATTLEGROUND: Player %s joined the battle.", plr->GetName());
}

// This method should be called when player logs out from running battleground
void BattleGround::EventPlayerLoggedOut(Player* player)
{
    if (GetStatus() == STATUS_IN_PROGRESS)
    {
        if (isBattleGround())
            EventPlayerDroppedFlag(player);
    }

    if (isArena())
        player->LeaveBattleground();
}

/* This method should be called only once ... it adds pointer to queue */
void BattleGround::AddToBGFreeSlotQueue()
{
    // make sure to add only once
    if (!m_InBGFreeSlotQueue)
    {
        sBattleGroundMgr.BGFreeSlotQueue[m_TypeID].push_front(this);
        m_InBGFreeSlotQueue = true;
    }
}

/* This method removes this battleground from free queue - it must be called when deleting battleground - not used now*/
void BattleGround::RemoveFromBGFreeSlotQueue()
{
    // set to be able to re-add if needed
    m_InBGFreeSlotQueue = false;
    // uncomment this code when battlegrounds will work like instances
    for (std::list<BattleGround*>::iterator itr = sBattleGroundMgr.BGFreeSlotQueue[m_TypeID].begin(); itr != sBattleGroundMgr.BGFreeSlotQueue[m_TypeID].end(); ++itr)
    {
        if ((*itr)->GetInstanceID() == m_InstanceID)
        {
            sBattleGroundMgr.BGFreeSlotQueue[m_TypeID].erase(itr);
            return;
        }
    }
}

// get the number of free slots for team
// works in similar way that HasFreeSlotsForTeam did, but this is needed for join as group
uint32 BattleGround::GetFreeSlotsForTeam(uint32 Team) const
{
    //if BG is starting ... invite anyone
    if (GetStatus() == STATUS_WAIT_JOIN)
        return (GetInvitedCount(Team) < GetMaxPlayersPerTeam()) ? GetMaxPlayersPerTeam() - GetInvitedCount(Team) : 0;
    //if BG is already started .. do not allow to join too much players of one faction
    uint32 otherTeam;
    uint32 otherIn;
    if (Team == ALLIANCE)
    {
        otherTeam = GetInvitedCount(HORDE);
        otherIn = GetPlayersCountByTeam(HORDE);
    }
    else
    {
        otherTeam = GetInvitedCount(ALLIANCE);
        otherIn = GetPlayersCountByTeam(ALLIANCE);
    }
    if (GetStatus() == STATUS_IN_PROGRESS)
    {
        // difference based on ppl invited (not necessarily entered battle)
        // default: allow 0
        uint32 diff = 0;
        // allow join one person if the sides are equal (to fill up bg to minplayersperteam)
        if (otherTeam == GetInvitedCount(Team))
            diff = 1;
        // allow join more ppl if the other side has more players
        else if (otherTeam > GetInvitedCount(Team))
            diff = otherTeam - GetInvitedCount(Team);

        // difference based on max players per team (don't allow inviting more)
        uint32 diff2 = (GetInvitedCount(Team) < GetMaxPlayersPerTeam()) ? GetMaxPlayersPerTeam() - GetInvitedCount(Team) : 0;

        // difference based on players who already entered
        // default: allow 0
        uint32 diff3 = 0;
        // allow join one person if the sides are equal (to fill up bg minplayersperteam)
        if (otherIn == GetPlayersCountByTeam(Team))
            diff3 = 1;
        // allow join more ppl if the other side has more players
        else if (otherIn > GetPlayersCountByTeam(Team))
            diff3 = otherIn - GetPlayersCountByTeam(Team);
        // or other side has less than minPlayersPerTeam
        else if (GetInvitedCount(Team) <= GetMinPlayersPerTeam())
            diff3 = GetMinPlayersPerTeam() - GetInvitedCount(Team) + 1;

        // return the minimum of the 3 differences

        // min of diff and diff 2
        diff = diff < diff2 ? diff : diff2;

        // min of diff, diff2 and diff3
        return diff < diff3 ? diff : diff3 ;
    }

    return 0;
}

bool BattleGround::HasFreeSlots() const
{
    return GetPlayersSize() < GetMaxPlayers();
}

void BattleGround::UpdatePlayerScore(Player* Source, uint32 type, uint32 value)
{
    //this procedure is called from virtual function implemented in bg subclass
    BattleGroundScoreMap::const_iterator itr = m_PlayerScores.find(Source->GetGUID());

    if (itr == m_PlayerScores.end())                        // player not found...
        return;

    switch (type)
    {
    case SCORE_KILLING_BLOWS:                           // Killing blows
        itr->second->KillingBlows += value;
        break;
    case SCORE_DEATHS:                                  // Deaths
        itr->second->Deaths += value;
        break;
    case SCORE_HONORABLE_KILLS:                         // Honorable kills
        itr->second->HonorableKills += value;
        break;
    case SCORE_BONUS_HONOR:                             // Honor bonus
        // do not add honor in arenas
        if (isBattleGround())
        {
            // reward honor instantly
            if (Source->RewardHonor(NULL, 1, value))
                itr->second->BonusHonor += value;
        }
        break;
    //used only in EY, but in MSG_PVP_LOG_DATA opcode
    case SCORE_DAMAGE_DONE:                             // Damage Done
        itr->second->DamageDone += value;
        break;
    case SCORE_HEALING_DONE:                            // Healing Done
        itr->second->HealingDone += value;
        break;
    default:
        sLog.outError("BattleGround: Unknown player score type %u", type);
        break;
    }
}

void BattleGround::AddPlayerToResurrectQueue(uint64 npc_guid, uint64 player_guid)
{
    m_ReviveQueue[npc_guid].push_back(player_guid);

    Player* plr = sObjectMgr.GetPlayer(player_guid);
    if (!plr)
        return;

    plr->CastSpell(plr, SPELL_WAITING_FOR_RESURRECT, true);
    SpellEntry const* spellInfo = sSpellStore.LookupEntry(SPELL_WAITING_FOR_RESURRECT);
    if (spellInfo)
    {
        Aura* Aur = CreateAura(spellInfo, 0, NULL, plr);
        plr->AddAura(Aur);
    }
}

void BattleGround::RemovePlayerFromResurrectQueue(uint64 player_guid)
{
    for (std::map<uint64, std::vector<uint64> >::iterator itr = m_ReviveQueue.begin(); itr != m_ReviveQueue.end(); ++itr)
    {
        for (std::vector<uint64>::iterator itr2 = (itr->second).begin(); itr2 != (itr->second).end(); ++itr2)
        {
            if (*itr2 == player_guid)
            {
                (itr->second).erase(itr2);

                Player* plr = sObjectMgr.GetPlayer(player_guid);
                if (!plr)
                    return;

                plr->RemoveAurasDueToSpell(SPELL_WAITING_FOR_RESURRECT);

                return;
            }
        }
    }
}

bool BattleGround::AddObject(uint32 type, uint32 entry, float x, float y, float z, float o, float rotation0, float rotation1, float rotation2, float rotation3, uint32 /*respawnTime*/)
{
    Map* map = GetBgMap();
    if (!map)
        return false;

    // must be created this way, adding to godatamap would add it to the base map of the instance
    // and when loading it (in go::LoadFromDB()), a new guid would be assigned to the object, and a new object would be created
    // so we must create it specific for this instance
    GameObject* go = new GameObject;
    if (!go->Create(sObjectMgr.GenerateLowGuid(HIGHGUID_GAMEOBJECT), entry, map, x, y, z, o, rotation0, rotation1, rotation2, rotation3, 100, GO_STATE_READY))
    {
        sLog.outErrorDb("Gameobject template %u not found in database! BattleGround not created!", entry);
        sLog.outError("Cannot create gameobject template %u! BattleGround not created!", entry);
        delete go;
        return false;
    }
    /*
        uint32 guid = go->GetGUIDLow();

        // without this, UseButtonOrDoor caused the crash, since it tried to get go info from godata
        // iirc that was changed, so adding to go data map is no longer required if that was the only function using godata from GameObject without checking if it existed
        GameObjectData& data = sObjectMgr.NewGOData(guid);

        data.id             = entry;
        data.mapid          = GetMapId();
        data.posX           = x;
        data.posY           = y;
        data.posZ           = z;
        data.orientation    = o;
        data.rotation0      = rotation0;
        data.rotation1      = rotation1;
        data.rotation2      = rotation2;
        data.rotation3      = rotation3;
        data.spawntimesecs  = respawnTime;
        data.spawnMask      = 1;
        data.animprogress   = 100;
        data.go_state       = 1;
    */
    // add to world, so it can be later looked up from HashMapHolder
    map->AddToMap(go);
    m_BgObjects[type] = go->GetGUID();
    return true;
}

//some doors aren't despawned so we cannot handle their closing in gameobject::update()
//it would be nice to correctly implement GO_ACTIVATED state and open/close doors in gameobject code
void BattleGround::DoorClose(uint32 type)
{
    GameObject* obj = GetBgMap()->GetGameObject(m_BgObjects[type]);
    if (obj)
    {
        //if doors are open, close it
        if (obj->getLootState() == GO_ACTIVATED && obj->GetGoState() != GO_STATE_READY)
        {
            //change state to allow door to be closed
            obj->SetLootState(GO_READY);
            obj->UseDoorOrButton(RESPAWN_ONE_DAY);
        }
    }
    else
        sLog.outError("BattleGround: Door object not found (cannot close doors)");
}

void BattleGround::DoorOpen(uint32 type)
{
    GameObject* obj = GetBgMap()->GetGameObject(m_BgObjects[type]);
    if (obj)
    {
        //change state to be sure they will be opened
        obj->SetLootState(GO_READY);
        obj->UseDoorOrButton(RESPAWN_ONE_DAY);
    }
    else
        sLog.outError("BattleGround: Door object not found! - doors will be closed.");
}

GameObject* BattleGround::GetBGObject(uint32 type)
{
    GameObject* obj = GetBgMap()->GetGameObject(m_BgObjects[type]);
    if (!obj)
        sLog.outError("couldn't get gameobject %i", type);
    return obj;
}

Creature* BattleGround::GetBGCreature(uint32 type)
{
    Creature* creature = GetBgMap()->GetCreature(m_BgCreatures[type]);
    if (!creature)
        sLog.outError("couldn't get creature %i", type);
    return creature;
}

void BattleGround::SpawnBGObject(uint32 type, uint32 respawntime)
{
    Map* map = GetBgMap();
    if (!map)
        return;
    if (respawntime == 0)
    {
        GameObject* obj = map->GetGameObject(m_BgObjects[type]);
        if (obj)
        {
            //we need to change state from GO_JUST_DEACTIVATED to GO_READY in case battleground is starting again
            if (obj->getLootState() == GO_JUST_DEACTIVATED)
                obj->SetLootState(GO_READY);
            obj->SetRespawnTime(0);
            map->AddToMap(obj);
        }
    }
    else
    {
        GameObject* obj = map->GetGameObject(m_BgObjects[type]);
        if (obj)
        {
            map->AddToMap(obj);
            obj->SetRespawnTime(respawntime);
            obj->SetLootState(GO_JUST_DEACTIVATED);
        }
    }
}

Creature* BattleGround::AddCreature(uint32 entry, uint32 type, uint32 teamval, float x, float y, float z, float o, uint32 /*respawntime*/)
{
    Map* map = GetBgMap();
    if (!map)
        return NULL;

    Creature* pCreature = new Creature;
    if (!pCreature->Create(sObjectMgr.GenerateLowGuid(HIGHGUID_UNIT), map, entry, teamval, x, y, z, o))
    {
        sLog.outError("Can't create creature entry: %u", entry);
        delete pCreature;
        return NULL;
    }

    pCreature->SetHomePosition(x, y, z, o);

    //pCreature->SetDungeonDifficulty(0);

    map->AddToMap(pCreature);
    m_BgCreatures[type] = pCreature->GetGUID();

    return  pCreature;
}
/*
void BattleGround::SpawnBGCreature(uint32 type, uint32 respawntime)
{
    Map * map = MapManager::Instance().FindMap(GetMapId(),GetInstanceId());
    if (!map)
        return false;

    if (respawntime == 0)
    {
        Creature* obj = HashMapHolder<Creature>::Find(m_BgCreatures[type]);
        if (obj)
        {
            //obj->Respawn();                               // bugged
            obj->SetRespawnTime(0);
            sObjectMgr.SaveCreatureRespawnTime(obj->GetGUIDLow(), GetInstanceID(), 0);
            map->Add(obj);
        }
    }
    else
    {
        Creature* obj = HashMapHolder<Creature>::Find(m_BgCreatures[type]);
        if (obj)
        {
            obj->setDeathState(DEAD);
            obj->SetRespawnTime(respawntime);
            map->Add(obj);
        }
    }
}
*/
bool BattleGround::DelCreature(uint32 type)
{
    if (!m_BgCreatures[type])
        return true;

    Creature* cr = GetBgMap()->GetCreature(m_BgCreatures[type]);
    if (!cr)
    {
        sLog.outError("Can't find creature guid: %u", GUID_LOPART(m_BgCreatures[type]));
        return false;
    }
    //@todo only delete creature after not in combat
    cr->AddObjectToRemoveList();
    m_BgCreatures[type] = 0;
    return true;
}

bool BattleGround::DelObject(uint32 type)
{
    if (!m_BgObjects[type])
        return true;

    GameObject* obj = GetBgMap()->GetGameObject(m_BgObjects[type]);
    if (!obj)
    {
        sLog.outError("Can't find gobject guid: %u", GUID_LOPART(m_BgObjects[type]));
        return false;
    }

    obj->SetRespawnTime(0);                                 // not save respawn time
    obj->Delete();
    m_BgObjects[type] = 0;
    return true;
}

bool BattleGround::AddSpiritGuide(uint32 type, float x, float y, float z, float o, uint32 team)
{
    uint32 entry = 0;

    if (team == ALLIANCE)
        entry = 13116;
    else
        entry = 13117;

    Creature* pCreature = AddCreature(entry, type, team, x, y, z, o);
    if (!pCreature)
    {
        sLog.outError("Can't create Spirit guide. BattleGround not created!");
        EndNow();
        return false;
    }

    pCreature->setDeathState(DEAD);

    pCreature->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, pCreature->GetGUID());
    // aura
    pCreature->SetUInt32Value(UNIT_FIELD_AURA, SPELL_SPIRIT_HEAL_CHANNEL);
    pCreature->SetUInt32Value(UNIT_FIELD_AURAFLAGS, 0x00000009);
    pCreature->SetUInt32Value(UNIT_FIELD_AURALEVELS, 0x0000003C);
    pCreature->SetUInt32Value(UNIT_FIELD_AURAAPPLICATIONS, 0x000000FF);
    // casting visual effect
    pCreature->SetUInt32Value(UNIT_CHANNEL_SPELL, SPELL_SPIRIT_HEAL_CHANNEL);
    // correct cast speed
    pCreature->SetFloatValue(UNIT_MOD_CAST_SPEED, 1.0f);

    //pCreature->CastSpell(pCreature, SPELL_SPIRIT_HEAL_CHANNEL, true);

    return true;
}

void BattleGround::SendMessageToAll(int32 entry, ChatMsg type, Player const* source)
{
    Oregon::BattleGroundChatBuilder bg_builder(type, entry, source);
    Oregon::LocalizedPacketDo<Oregon::BattleGroundChatBuilder> bg_do(bg_builder);
    BroadcastWorker(bg_do);
}

void BattleGround::PSendMessageToAll(int32 entry, ChatMsg type, Player const* source, ...)
{
    va_list ap;
    va_start(ap, source);

    Oregon::BattleGroundChatBuilder bg_builder(type, entry, source, &ap);
    Oregon::LocalizedPacketDo<Oregon::BattleGroundChatBuilder> bg_do(bg_builder);
    BroadcastWorker(bg_do);

    va_end(ap);
}

void BattleGround::SendMessage2ToAll(int32 entry, ChatMsg type, Player const* source, int32 arg1, int32 arg2)
{
    Oregon::BattleGround2ChatBuilder bg_builder(type, entry, source, arg1, arg2);
    Oregon::LocalizedPacketDo<Oregon::BattleGround2ChatBuilder> bg_do(bg_builder);
    BroadcastWorker(bg_do);
}

void BattleGround::EndNow()
{
    RemoveFromBGFreeSlotQueue();
    SetStatus(STATUS_WAIT_LEAVE);
    SetEndTime(TIME_TO_AUTOREMOVE);
    // inform invited players about the removal
    sBattleGroundMgr.m_BattleGroundQueues[sBattleGroundMgr.BGQueueTypeId(GetTypeID(), GetArenaType())].BGEndedRemoveInvites(this);
}

// Battleground messages are localized using the dbc lang, they are not client language dependent
const char* BattleGround::GetOregonString(int32 entry)
{
    // FIXME: now we have different DBC locales and need localized message for each target client
    return sObjectMgr.GetOregonStringForDBCLocale(entry);
}

/*
important notice:
buffs aren't spawned/despawned when players captures anything
buffs are in their positions when battleground starts
*/
void BattleGround::HandleTriggerBuff(uint64 const& go_guid)
{
    GameObject* obj = GetBgMap()->GetGameObject(go_guid);
    if (!obj || obj->GetGoType() != GAMEOBJECT_TYPE_TRAP || !obj->isSpawned())
        return;

    //change buff type, when buff is used:
    int32 index = m_BgObjects.size() - 1;
    while (index >= 0 && m_BgObjects[index] != go_guid)
        index--;
    if (index < 0)
    {
        sLog.outError("BattleGround (Type: %u) has buff gameobject (Guid: %u Entry: %u Type:%u) but it hasn't that object in its internal data", GetTypeID(), GUID_LOPART(go_guid), obj->GetEntry(), obj->GetGoType());
        return;
    }

    //randomly select new buff
    uint8 buff = urand(0, 2);
    uint32 entry = obj->GetEntry();
    if (m_BuffChange && entry != Buff_Entries[buff])
    {
        //despawn current buff
        SpawnBGObject(index, RESPAWN_ONE_DAY);
        //set index for new one
        for (uint8 currBuffTypeIndex = 0; currBuffTypeIndex < 3; ++currBuffTypeIndex)
            if (entry == Buff_Entries[currBuffTypeIndex])
            {
                index -= currBuffTypeIndex;
                index += buff;
            }
    }

    SpawnBGObject(index, BUFF_RESPAWN_TIME);
}

void BattleGround::HandleKillPlayer(Player* victim, Player* killer)
{
    // Keep in mind that for arena this will have to be changed a bit

    // Add +1 deaths
    UpdatePlayerScore(victim, SCORE_DEATHS, 1);

    // Add +1 kills to group and +1 killing_blows to killer
    if (killer)
    {
        // Don't reward credit for killing ourselves, like fall damage of hellfire (warlock)
        if (killer == victim)
            return;

        UpdatePlayerScore(killer, SCORE_HONORABLE_KILLS, 1);
        UpdatePlayerScore(killer, SCORE_KILLING_BLOWS, 1);

        for (std::map<uint64, BattleGroundPlayer>::iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
        {
            Player* creditedPlayer = sObjectMgr.GetPlayer(itr->first);
            if (!creditedPlayer || creditedPlayer == killer)
                continue;

            if (creditedPlayer->GetTeam() == killer->GetTeam() && creditedPlayer->IsAtGroupRewardDistance(victim))
                UpdatePlayerScore(creditedPlayer, SCORE_HONORABLE_KILLS, 1);
        }
    }

    // To be able to remove insignia -- ONLY IN Battlegrounds
    if (!isArena())
        victim->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE);
}

// return the player's team based on battlegroundplayer info
// used in same faction arena matches mainly
uint32 BattleGround::GetPlayerTeam(uint64 guid)
{
    std::map<uint64, BattleGroundPlayer>::const_iterator itr = m_Players.find(guid);
    if (itr != m_Players.end())
        return itr->second.Team;
    return 0;
}

uint32 BattleGround::GetOtherTeam(uint32 teamId)
{
    return (teamId) ? ((teamId == ALLIANCE) ? HORDE : ALLIANCE) : 0;
}

bool BattleGround::IsPlayerInBattleGround(uint64 guid)
{
    std::map<uint64, BattleGroundPlayer>::const_iterator itr = m_Players.find(guid);
    if (itr != m_Players.end())
        return true;
    return false;
}

void BattleGround::PlayerRelogin(uint64 guid)
{
    if (GetStatus() != STATUS_WAIT_LEAVE)
        return;

    Player* plr = sObjectMgr.GetPlayer(guid);
    if (!plr)
    {
        sLog.outError("BattleGround: Player (GUID: %u) not found!", GUID_LOPART(guid));
        return;
    }

    WorldPacket data;
    uint32 bgQueueTypeId = BattleGroundMgr::BGQueueTypeId(GetTypeID(), GetArenaType());

    BlockMovement(plr);

    sBattleGroundMgr.BuildPvpLogDataPacket(&data, this);
    plr->GetSession()->SendPacket(&data);

    sBattleGroundMgr.BuildBattleGroundStatusPacket(&data, this, plr->GetTeam(), plr->GetBattleGroundQueueIndex(bgQueueTypeId), STATUS_IN_PROGRESS, TIME_TO_AUTOREMOVE, GetStartTime());
    plr->GetSession()->SendPacket(&data);
}

uint32 BattleGround::GetAlivePlayersCountByTeam(uint32 Team) const
{
    int count = 0;
    for (std::map<uint64, BattleGroundPlayer>::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
    {
        if (itr->second.Team == Team)
        {
            Player* pl = sObjectMgr.GetPlayer(itr->first);
            if (pl && pl->IsAlive() && !pl->HasByteFlag(UNIT_FIELD_BYTES_2, 3, FORM_SPIRITOFREDEMPTION))
                ++count;
        }
    }
    return count;
}

void BattleGround::SetHoliday(bool is_holiday)
{
    if (is_holiday)
        m_HonorMode = BG_HOLIDAY;
    else
        m_HonorMode = BG_NORMAL;
}

int32 BattleGround::GetObjectType(uint64 guid)
{
    for (uint32 i = 0; i < m_BgObjects.size(); ++i)
        if (m_BgObjects[i] == guid)
            return i;
    sLog.outError("BattleGround: cheating? a player used a gameobject which isnt supposed to be a usable object!");
    return -1;
}

void BattleGround::HandleKillUnit(Creature* /*creature*/, Player* /*killer*/)
{
}

void BattleGround::CheckArenaWinConditions()
{
    if (!GetAlivePlayersCountByTeam(ALLIANCE) && GetPlayersCountByTeam(HORDE))
        EndBattleGround(HORDE);
    else if (GetPlayersCountByTeam(ALLIANCE) && !GetAlivePlayersCountByTeam(HORDE))
        EndBattleGround(ALLIANCE);
}

WorldSafeLocsEntry const* BattleGround::GetClosestGraveYard(Player* player)
{
    return sObjectMgr.GetClosestGraveYard(player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetMapId(), player->GetTeam());
}

void BattleGround::Announce()
{
    //send world message
    if (sWorld.getConfig(CONFIG_BATTLEGROUND_QUEUE_ANNOUNCER_ENABLE) && sWorld.getConfig(CONFIG_BATTLEGROUND_QUEUE_ANNOUNCER_ONSTART))
    {
        uint32 queue_id = 0;

        for (std::map<uint64, BattleGroundPlayer>::iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
            if (Player* plr = sObjectMgr.GetPlayer(itr->first))
                queue_id = plr->GetBattleGroundQueueIdFromLevel();

        char const* bgName = GetName();

        uint32 q_min_level = Player::GetMinLevelForBattleGroundQueueId(queue_id);
        uint32 q_max_level = Player::GetMaxLevelForBattleGroundQueueId(queue_id);

        // replace hardcoded max level by player max level for nice output
        if (q_max_level > sWorld.getConfig(CONFIG_MAX_PLAYER_LEVEL))
            q_max_level = sWorld.getConfig(CONFIG_MAX_PLAYER_LEVEL);

        sWorld.SendWorldText(LANG_BG_QUEUE_ANNOUNCE_START, bgName, q_min_level, q_max_level);
    }
}

inline void BattleGround::_CheckSafePositions(uint32 diff)
{
    float maxDist = GetStartMaxDist();
    if (!maxDist)
        return;

    m_ValidStartPositionTimer += diff;
    if (m_ValidStartPositionTimer >= CHECK_PLAYER_POSITION_INVERVAL)
    {
        m_ValidStartPositionTimer = 0;

        Position pos;
        float x, y, z, o;
        for (BattleGroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
            if (Player* player = ObjectAccessor::FindPlayer(itr->first))
            {
                player->GetPosition(&pos);
                GetTeamStartLoc(player->GetBGTeam(), x, y, z, o);
                if (pos.GetExactDistSq(x, y, z) > maxDist)
                {
                    sLog.outDebug("BATTLEGROUND: Sending %s back to start location (map: %u) (possible exploit)", player->GetName(), GetMapId());
                    player->TeleportTo(GetMapId(), x, y, z, o);
                }
            }
    }
}
