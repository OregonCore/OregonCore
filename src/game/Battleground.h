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

#ifndef __BATTLEGROUND_H
#define __BATTLEGROUND_H

#include "Common.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Opcodes.h"
#include "ObjectMgr.h"
#include "BattlegroundMgr.h"
#include "SharedDefines.h"

enum BattlegroundSounds
{
    SOUND_HORDE_WINS                = 8454,
    SOUND_ALLIANCE_WINS             = 8455,
    SOUND_BG_START                  = 3439,
    SOUND_BG_START_L70ETC           = 11803,
};

enum BattlegroundQuests
{
    SPELL_WS_QUEST_REWARD           = 43483,
    SPELL_AB_QUEST_REWARD           = 43484,
    SPELL_AV_QUEST_REWARD           = 43475,
    SPELL_AV_QUEST_KILLED_BOSS      = 23658,
    SPELL_EY_QUEST_REWARD           = 43477,
    SPELL_AB_QUEST_REWARD_4_BASES   = 24061,
    SPELL_AB_QUEST_REWARD_5_BASES   = 24064
};

enum BattlegroundMarks
{
    ITEM_AV_MARK_OF_HONOR           = 20560,
    ITEM_WS_MARK_OF_HONOR           = 20558,
    ITEM_AB_MARK_OF_HONOR           = 20559,
    ITEM_EY_MARK_OF_HONOR           = 29024
};

enum BattlegroundMarksCount
{
    ITEM_WINNER_COUNT               = 3,
    ITEM_LOSER_COUNT                = 1
};

enum BattlegroundSpells
{
    SPELL_SUMMON_PET                = 6962,                 // Pet Summoned
    SPELL_WAITING_FOR_RESURRECT     = 2584,                 // Waiting to Resurrect
    SPELL_SPIRIT_HEAL_CHANNEL       = 22011,                // Spirit Heal Channel
    SPELL_SPIRIT_HEAL               = 22012,                // Spirit Heal
    SPELL_RESURRECTION_VISUAL       = 24171,                // Resurrection Impact Visual
    SPELL_ARENA_PREPARATION         = 32727,                // use this one, 32728 not correct
    SPELL_ALLIANCE_GOLD_FLAG        = 32724,
    SPELL_ALLIANCE_GREEN_FLAG       = 32725,
    SPELL_HORDE_GOLD_FLAG           = 35774,
    SPELL_HORDE_GREEN_FLAG          = 35775,
    SPELL_PREPARATION               = 44521,                // Preparation
    SPELL_SPIRIT_HEAL_MANA          = 44535,                // Spirit Heal
    SPELL_RECENTLY_DROPPED_FLAG     = 42792,                // Recently Dropped Flag
    SPELL_AURA_PLAYER_INACTIVE      = 43681                 // Inactive
};

enum BattlegroundTimeIntervals
{
    CHECK_PLAYER_POSITION_INVERVAL  = 1000,                 // ms
    RESURRECTION_INTERVAL           = 30000,                // ms
    REMIND_INTERVAL                 = 30000,                // ms
    INVITE_ACCEPT_WAIT_TIME         = 80000,                // ms
    TIME_TO_AUTOREMOVE              = 120000,               // ms
    MAX_OFFLINE_TIME                = 300000,               // ms
    RESPAWN_ONE_DAY                 = 86400,                // secs
    RESPAWN_IMMEDIATELY             = 0,                    // secs
    BUFF_RESPAWN_TIME               = 180,                  // secs
    BG_HONOR_SCORE_TICKS            = 330                   // points
};

enum BattlegroundStartTimeIntervals
{
    BG_START_DELAY_2M               = 120000,               // ms (2 minutes)
    BG_START_DELAY_1M               = 60000,                // ms (1 minute)
    BG_START_DELAY_30S              = 30000,                // ms (30 seconds)
    BG_START_DELAY_15S              = 15000,                // ms (15 seconds) Used only in arena
    BG_START_DELAY_NONE             = 0,                    // ms
};

enum BattlegroundBuffObjects
{
    BG_OBJECTID_SPEEDBUFF_ENTRY     = 179871,
    BG_OBJECTID_REGENBUFF_ENTRY     = 179904,
    BG_OBJECTID_BERSERKERBUFF_ENTRY = 179905
};

const uint32 Buff_Entries[3] = { BG_OBJECTID_SPEEDBUFF_ENTRY, BG_OBJECTID_REGENBUFF_ENTRY, BG_OBJECTID_BERSERKERBUFF_ENTRY };

enum BattlegroundStatus
{
    STATUS_NONE         = 0,                                // first status, should mean bg is not instance
    STATUS_WAIT_QUEUE   = 1,                                // means bg is empty and waiting for queue
    STATUS_WAIT_JOIN    = 2,                                // this means, that BG has already started and it is waiting for more players
    STATUS_IN_PROGRESS  = 3,                                // means bg is running
    STATUS_WAIT_LEAVE   = 4                                 // means some faction has won BG and it is ending
};

struct BattlegroundPlayer
{
    uint32  LastOnlineTime;                                 // for tracking and removing offline players from queue after 5 minutes
    uint32  Team;                                           // Player's team
};

struct BattlegroundObjectInfo
{
    BattlegroundObjectInfo() : object(NULL), timer(0), spellid(0) {}

    GameObject*  object;
    int32       timer;
    uint32      spellid;
};

#define MAX_QUEUED_PLAYERS_MAP 7

enum BattlegroundTypeId
{
    BATTLEGROUND_AV     = 1,
    BATTLEGROUND_WS     = 2,
    BATTLEGROUND_AB     = 3,
    BATTLEGROUND_NA     = 4,
    BATTLEGROUND_BE     = 5,
    BATTLEGROUND_AA     = 6,
    BATTLEGROUND_EY     = 7,
    BATTLEGROUND_RL     = 8
};
#define MAX_BATTLEGROUND_TYPE_ID 8

// handle the queue types and bg types separately to enable joining queue for different sized arenas at the same time
enum BattlegroundQueueTypeId
{
    BATTLEGROUND_QUEUE_AV     = 1,
    BATTLEGROUND_QUEUE_WS     = 2,
    BATTLEGROUND_QUEUE_AB     = 3,
    BATTLEGROUND_QUEUE_EY     = 4,
    BATTLEGROUND_QUEUE_2v2     = 5,
    BATTLEGROUND_QUEUE_3v3     = 6,
    BATTLEGROUND_QUEUE_5v5     = 7,
};

enum ScoreType
{
    SCORE_KILLING_BLOWS         = 1,
    SCORE_DEATHS                = 2,
    SCORE_HONORABLE_KILLS       = 3,
    SCORE_BONUS_HONOR           = 4,
    //EY, but in MSG_PVP_LOG_DATA opcode!
    SCORE_DAMAGE_DONE           = 5,
    SCORE_HEALING_DONE          = 6,
    //WS
    SCORE_FLAG_CAPTURES         = 7,
    SCORE_FLAG_RETURNS          = 8,
    //AB
    SCORE_BASES_ASSAULTED       = 9,
    SCORE_BASES_DEFENDED        = 10,
    //AV
    SCORE_GRAVEYARDS_ASSAULTED  = 11,
    SCORE_GRAVEYARDS_DEFENDED   = 12,
    SCORE_TOWERS_ASSAULTED      = 13,
    SCORE_TOWERS_DEFENDED       = 14,
    SCORE_MINES_CAPTURED        = 15,
    SCORE_LEADERS_KILLED        = 16,
    SCORE_SECONDARY_OBJECTIVES  = 17
                                  // TODO : implement them
};

enum ArenaType
{
    ARENA_TYPE_2v2          = 2,
    ARENA_TYPE_3v3          = 3,
    ARENA_TYPE_5v5          = 5
};

enum BattlegroundType
{
    TYPE_BATTLEGROUND     = 3,
    TYPE_ARENA            = 4
};

enum BattlegroundWinner
{
    WINNER_HORDE            = 0,
    WINNER_ALLIANCE         = 1,
    WINNER_NONE             = 2
};

enum BattlegroundTeamId
{
    BG_TEAM_ALLIANCE        = 0,
    BG_TEAM_HORDE           = 1
};
#define BG_TEAMS_COUNT  2

enum BattlegroundStartingEvents
{
    BG_STARTING_EVENT_NONE  = 0x00,
    BG_STARTING_EVENT_1     = 0x01,
    BG_STARTING_EVENT_2     = 0x02,
    BG_STARTING_EVENT_3     = 0x04,
    BG_STARTING_EVENT_4     = 0x08
};

enum BattlegroundStartingEventsIds
{
    BG_STARTING_EVENT_FIRST     = 0,
    BG_STARTING_EVENT_SECOND    = 1,
    BG_STARTING_EVENT_THIRD     = 2,
    BG_STARTING_EVENT_FOURTH    = 3
};
#define BG_STARTING_EVENT_COUNT 4

enum BattlegroundJoinError
{
    BG_JOIN_ERR_OK = 0,
    BG_JOIN_ERR_OFFLINE_MEMBER = 1,
    BG_JOIN_ERR_GROUP_TOO_MANY = 2,
    BG_JOIN_ERR_MIXED_FACTION = 3,
    BG_JOIN_ERR_MIXED_LEVELS = 4,
    BG_JOIN_ERR_MIXED_ARENATEAM = 5,
    BG_JOIN_ERR_GROUP_MEMBER_ALREADY_IN_QUEUE = 6,
    BG_JOIN_ERR_GROUP_DESERTER = 7,
    BG_JOIN_ERR_ALL_QUEUES_USED = 8,
    BG_JOIN_ERR_GROUP_NOT_ENOUGH = 9
};

class BattlegroundScore
{
    public:
        BattlegroundScore() : KillingBlows(0), Deaths(0), HonorableKills(0),
            BonusHonor(0), DamageDone(0), HealingDone(0)
        {}
        virtual ~BattlegroundScore() {}                     //virtual destructor is used when deleting score from scores map

        uint32 KillingBlows;
        uint32 Deaths;
        uint32 HonorableKills;
        uint32 BonusHonor;
        uint32 DamageDone;
        uint32 HealingDone;
};

enum BGHonorMode
{
    BG_NORMAL = 0,
    BG_HOLIDAY,
    BG_HONOR_MODE_NUM
};

/*
This class is used to:
1. Add player to battleground
2. Remove player from battleground
3. some certain cases, same for all battlegrounds
4. It has properties same for all battlegrounds
*/
class Battleground
{
        friend class BattlegroundMgr;

    public:
        /* Construction */
        Battleground();
        /*Battleground(const Battleground& bg);*/
        virtual ~Battleground();
        virtual void Update(uint32 diff);                   // must be implemented in BG subclass of BG specific update code, but must in begginning call parent version
        virtual bool SetupBattleground()                    // must be implemented in BG subclass
        {
            return true;
        }
        void Reset();                                       // resets all common properties for battlegrounds
        virtual void StartingEventCloseDoors() {}
        virtual void StartingEventOpenDoors() {}
        virtual void ResetBGSubclass()                      // must be implemented in BG subclass
        {
        }

        /* Battleground */
        // Get methods:
        char const* GetName() const
        {
            return m_Name;
        }
        uint32 GetTypeID() const
        {
            return m_TypeID;
        }
        uint32 GetQueueType() const
        {
            return m_Queue_type;
        }
        uint32 GetInstanceID() const
        {
            return m_InstanceID;
        }
        uint32 GetStatus() const
        {
            return m_Status;
        }
        uint32 GetStartTime() const
        {
            return m_StartTime;
        }
        uint32 GetEndTime() const
        {
            return m_EndTime;
        }
        uint32 GetLastResurrectTime() const
        {
            return m_LastResurrectTime;
        }
        uint32 GetMaxPlayers() const
        {
            return m_MaxPlayers;
        }
        uint32 GetMinPlayers() const
        {
            return m_MinPlayers;
        }

        uint32 GetMinLevel() const
        {
            return m_LevelMin;
        }
        uint32 GetMaxLevel() const
        {
            return m_LevelMax;
        }

        uint32 GetMaxPlayersPerTeam() const
        {
            return m_MaxPlayersPerTeam;
        }
        uint32 GetMinPlayersPerTeam() const
        {
            return m_MinPlayersPerTeam;
        }

        int GetStartDelayTime() const
        {
            return m_StartDelayTime;
        }
        uint8 GetArenaType() const
        {
            return m_ArenaType;
        }
        uint8 GetWinner() const
        {
            return m_Winner;
        }
        uint32 GetBattlemasterEntry() const;

        // Set methods:
        void SetName(char const* Name)
        {
            m_Name = Name;
        }
        void SetTypeID(uint32 TypeID)
        {
            m_TypeID = TypeID;
        }
        void SetQueueType(uint32 ID)
        {
            m_Queue_type = ID;
        }
        void SetInstanceID(uint32 InstanceID)
        {
            m_InstanceID = InstanceID;
        }
        void SetStatus(uint32 Status)
        {
            m_Status = Status;
        }
        void SetStartTime(uint32 Time)
        {
            m_StartTime = Time;
        }
        void SetEndTime(uint32 Time)
        {
            m_EndTime = Time;
        }
        void SetLastResurrectTime(uint32 Time)
        {
            m_LastResurrectTime = Time;
        }
        void SetMaxPlayers(uint32 MaxPlayers)
        {
            m_MaxPlayers = MaxPlayers;
        }
        void SetMinPlayers(uint32 MinPlayers)
        {
            m_MinPlayers = MinPlayers;
        }
        void SetLevelRange(uint32 min, uint32 max)
        {
            m_LevelMin = min;
            m_LevelMax = max;
        }
        void SetRated(bool state)
        {
            m_IsRated = state;
        }
        void SetArenaType(uint8 type)
        {
            m_ArenaType = type;
        }
        void SetArenaorBGType(bool _isArena)
        {
            m_IsArena = _isArena;
        }
        void SetWinner(uint8 winner)
        {
            m_Winner = winner;
        }

        void ModifyStartDelayTime(int diff)
        {
            m_StartDelayTime -= diff;
        }
        void SetStartDelayTime(int Time)
        {
            m_StartDelayTime = Time;
        }

        void SetMaxPlayersPerTeam(uint32 MaxPlayers)
        {
            m_MaxPlayersPerTeam = MaxPlayers;
        }
        void SetMinPlayersPerTeam(uint32 MinPlayers)
        {
            m_MinPlayersPerTeam = MinPlayers;
        }

        void AddToBGFreeSlotQueue();                        //this queue will be useful when more battlegrounds instances will be available
        void RemoveFromBGFreeSlotQueue();                   //this method could delete whole BG instance, if another free is available

        void _CheckSafePositions(uint32 diff);
        void SetStartMaxDist(float startMaxDist) { m_StartMaxDist = startMaxDist; }
        float GetStartMaxDist() const { return m_StartMaxDist; }

        void DecreaseInvitedCount(uint32 team)
        {
            (team == ALLIANCE) ? --m_InvitedAlliance : --m_InvitedHorde;
        }
        void IncreaseInvitedCount(uint32 team)
        {
            (team == ALLIANCE) ? ++m_InvitedAlliance : ++m_InvitedHorde;
        }
        uint32 GetInvitedCount(uint32 team) const
        {
            if (team == ALLIANCE)
                return m_InvitedAlliance;
            else
                return m_InvitedHorde;
        }
        bool HasFreeSlotsForTeam(uint32 Team) const;
        bool HasFreeSlots() const;
        uint32 GetFreeSlotsForTeam(uint32 Team) const;

        bool isArena() const
        {
            return m_IsArena;
        }
        bool isBattleground() const
        {
            return !m_IsArena;
        }
        bool isRated() const
        {
            return m_IsRated;
        }

        typedef std::map<uint64, BattlegroundPlayer> BattlegroundPlayerMap;
        BattlegroundPlayerMap const& GetPlayers() const
        {
            return m_Players;
        }
        uint32 GetPlayersSize() const
        {
            return m_Players.size();
        }
        uint32 GetRemovedPlayersSize() const
        {
            return m_RemovedPlayers.size();
        }

        typedef std::map<uint64, BattlegroundScore*> BattlegroundScoreMap;
        BattlegroundScoreMap::const_iterator GetPlayerScoresBegin() const
        {
            return m_PlayerScores.begin();
        }
        BattlegroundScoreMap::const_iterator GetPlayerScoresEnd() const
        {
            return m_PlayerScores.end();
        }
        uint32 GetPlayerScoresSize() const
        {
            return m_PlayerScores.size();
        }

        uint32 GetReviveQueueSize() const
        {
            return m_ReviveQueue.size();
        }

        void AddPlayerToResurrectQueue(uint64 npc_guid, uint64 player_guid);
        void RemovePlayerFromResurrectQueue(uint64 player_guid);

        void StartBattleground();

        GameObject* GetBGObject(uint32 type);
        Creature* GetBGCreature(uint32 type);
        /* Location */
        void SetMapId(uint32 MapID)
        {
            m_MapId = MapID;
        }
        uint32 GetMapId() const
        {
            return m_MapId;
        }

        /* Map pointers */
        void SetBgMap(BattlegroundMap* map)
        {
            m_Map = map;
        }
        BattlegroundMap* GetBgMap()
        {
            ASSERT(m_Map);
            return m_Map;
        }

        void SetTeamStartLoc(uint32 TeamID, float X, float Y, float Z, float O);
        void GetTeamStartLoc(uint32 TeamID, float& X, float& Y, float& Z, float& O) const
        {
            uint8 idx = GetTeamIndexByTeamId(TeamID);
            X = m_TeamStartLocX[idx];
            Y = m_TeamStartLocY[idx];
            Z = m_TeamStartLocZ[idx];
            O = m_TeamStartLocO[idx];
        }

        /* Packet Transfer */
        // method that should fill worldpacket with actual world states (not yet implemented for all battlegrounds!)
        virtual void FillInitialWorldStates(WorldPacket& /*data*/) {}
        void SendPacketToTeam(uint32 TeamID, WorldPacket* packet, Player* sender = NULL, bool self = true);
        void SendPacketToAll(WorldPacket* packet);
        void YellToAll(Creature* creature, const char* text, uint32 language);

        template<class Do>
        void BroadcastWorker(Do& _do);

        void PlaySoundToTeam(uint32 SoundID, uint32 TeamID);
        void PlaySoundToAll(uint32 SoundID);
        void CastSpellOnTeam(uint32 SpellID, uint32 TeamID);
        void RewardHonorToTeam(uint32 Honor, uint32 TeamID);
        void RewardReputationToTeam(uint32 faction_id, uint32 Reputation, uint32 TeamID);
        void RewardMark(Player* plr, uint32 count);
        void SendRewardMarkByMail(Player* plr, uint32 mark, uint32 count);
        void RewardQuest(Player* plr);
        void UpdateWorldState(uint32 Field, uint32 Value);
        void UpdateWorldStateForPlayer(uint32 Field, uint32 Value, Player* Source);
        void EndBattleground(uint32 winner);
        void BlockMovement(Player* plr);

        void SendMessageToAll(int32 entry, ChatMsg type, Player const* source = NULL);
        void PSendMessageToAll(int32 entry, ChatMsg type, Player const* source, ...);

        // specialized version with 2 string id args
        void SendMessage2ToAll(int32 entry, ChatMsg type, Player const* source, int32 strId1 = 0, int32 strId2 = 0);

        /* Raid Group */
        Group* GetBgRaid(uint32 TeamID) const
        {
            return TeamID == ALLIANCE ? m_BgRaids[BG_TEAM_ALLIANCE] : m_BgRaids[BG_TEAM_HORDE];
        }
        void SetBgRaid(uint32 TeamID, Group* bg_raid)
        {
            Group*& old_raid = TeamID == ALLIANCE ? m_BgRaids[BG_TEAM_ALLIANCE] : m_BgRaids[BG_TEAM_HORDE];
            if (old_raid) old_raid->SetBattlegroundGroup(NULL);
            if (bg_raid) bg_raid->SetBattlegroundGroup(this);
            old_raid = bg_raid;
        }

        virtual void UpdatePlayerScore(Player* Source, uint32 type, uint32 value);

        uint8 GetTeamIndexByTeamId(uint32 Team) const
        {
            return Team == ALLIANCE ? BG_TEAM_ALLIANCE : BG_TEAM_HORDE;
        }
        uint32 GetPlayersCountByTeam(uint32 Team) const
        {
            return m_PlayersCount[GetTeamIndexByTeamId(Team)];
        }
        uint32 GetAlivePlayersCountByTeam(uint32 Team) const;   // used in arenas to correctly handle death in spirit of redemption (killer = killed) cases
        void UpdatePlayersCountByTeam(uint32 Team, bool remove)
        {
            if (remove)
                --m_PlayersCount[GetTeamIndexByTeamId(Team)];
            else
                ++m_PlayersCount[GetTeamIndexByTeamId(Team)];
        }

        // used for rated arena battles
        void SetArenaTeamIdForTeam(uint32 Team, uint32 ArenaTeamId)
        {
            m_ArenaTeamIds[GetTeamIndexByTeamId(Team)] = ArenaTeamId;
        }
        uint32 GetArenaTeamIdForTeam(uint32 Team) const
        {
            return m_ArenaTeamIds[GetTeamIndexByTeamId(Team)];
        }
        void SetArenaTeamRatingChangeForTeam(uint32 Team, int32 RatingChange)
        {
            m_ArenaTeamRatingChanges[GetTeamIndexByTeamId(Team)] = RatingChange;
        }
        int32 GetArenaTeamRatingChangeForTeam(uint32 Team) const
        {
            return m_ArenaTeamRatingChanges[GetTeamIndexByTeamId(Team)];
        }
        void CheckArenaWinConditions();

        /* Triggers handle */
        // must be implemented in BG subclass
        virtual void HandleAreaTrigger(Player* /*Source*/, uint32 /*Trigger*/) {}
        // must be implemented in BG subclass if need AND call base class generic code
        virtual void HandleKillPlayer(Player* player, Player* killer);
        virtual void HandleKillUnit(Creature* /*unit*/, Player* /*killer*/);

        /* Battleground events */
        virtual void EventPlayerDroppedFlag(Player* /*player*/) {}
        virtual void EventPlayerClickedOnFlag(Player* /*player*/, GameObject* /*target_obj*/) {}
        virtual void EventPlayerCapturedFlag(Player* /*player*/) {}
        void EventPlayerLoggedOut(Player* player);

        /* Death related */
        virtual WorldSafeLocsEntry const* GetClosestGraveYard(Player* player);

        virtual void AddPlayer(Player* plr);                // must be implemented in BG subclass

        virtual void RemovePlayerAtLeave(uint64 guid, bool Transport, bool SendPacket);
        // can be extended in in BG subclass

        void HandleTriggerBuff(uint64 const& go_guid);
        void SetHoliday(bool is_holiday);

        // @todo make this protected:
        typedef std::vector<uint64> BGObjects;
        typedef std::vector<uint64> BGCreatures;
        BGObjects m_BgObjects;
        BGCreatures m_BgCreatures;
        void SpawnBGObject(uint32 type, uint32 respawntime);
        bool AddObject(uint32 type, uint32 entry, float x, float y, float z, float o, float rotation0, float rotation1, float rotation2, float rotation3, uint32 respawnTime = 0);
        //        void SpawnBGCreature(uint32 type, uint32 respawntime);
        Creature* AddCreature(uint32 entry, uint32 type, uint32 teamval, float x, float y, float z, float o, uint32 respawntime = 0);
        bool DelCreature(uint32 type);
        bool DelObject(uint32 type);
        bool AddSpiritGuide(uint32 type, float x, float y, float z, float o, uint32 team);
        int32 GetObjectType(uint64 guid);

        void DoorOpen(uint32 type);
        void DoorClose(uint32 type);
        const char* GetOregonString(int32 entry);

        virtual bool HandlePlayerUnderMap(Player* /*plr*/)
        {
            return false;
        }

        // since arenas can be AvA or Hvh, we have to get the "temporary" team of a player
        uint32 GetPlayerTeam(uint64 guid);
        uint32 GetOtherTeam(uint32 teamId);
        bool IsPlayerInBattleground(uint64 guid);
        void PlayerRelogin(uint64 guid);
        void RewardXPAtKill(Player* killer, Player* victim);

        void SetDeleteThis()
        {
            m_SetDeleteThis = true;
        }

        /* virtual score-array - get's used in bg-subclasses */
        int32 m_TeamScores[BG_TEAMS_COUNT];

        void Announce();

    protected:
        //this method is called, when BG cannot spawn its own spirit guide, or something is wrong, It correctly ends Battleground
        void EndNow();

        /* Scorekeeping */
        BattlegroundScoreMap m_PlayerScores;                // Player scores
        // must be implemented in BG subclass
        virtual void RemovePlayer(Player* /*player*/, uint64 /*guid*/) {}

        /* Player lists, those need to be accessible by inherited classes */
        BattlegroundPlayerMap  m_Players;
        // Spirit Guide guid + Player list GUIDS
        std::map<uint64, std::vector<uint64> >  m_ReviveQueue;

        /*
        these are important variables used for starting messages
        */
        uint8 m_Events;
        BattlegroundStartTimeIntervals m_StartDelayTimes[BG_STARTING_EVENT_COUNT];
        //this must be filled in constructors!
        uint32 m_StartMessageIds[BG_STARTING_EVENT_COUNT];

        bool   m_BuffChange;
        uint32 m_score[2];                    //array that keeps general team scores, used to determine who gets most marks when bg ends prematurely

        BGHonorMode m_HonorMode;
    private:
        /* Battleground */
        uint32 m_TypeID;                                    //Battleground type, defined in enum BattlegroundTypeId
        uint32 m_InstanceID;                                //Battleground Instance's GUID!
        uint32 m_Status;
        uint32 m_StartTime;
        uint32 m_ValidStartPositionTimer;
        uint32 m_EndTime;
        uint32 m_LastResurrectTime;
        uint32 m_Queue_type;
        uint8  m_ArenaType;                                 // 2=2v2, 3=3v3, 5=5v5
        bool   m_InBGFreeSlotQueue;                         // used to make sure that BG is only once inserted into the BattlegroundMgr.BGFreeSlotQueue[bgTypeId] deque
        bool   m_SetDeleteThis;                             // used for safe deletion of the bg after end / all players leave
        // this variable is not used .... it can be found in many other ways... but to store it in BG object instance is useless
        //uint8  m_BattlegroundType;                        // 3=BG, 4=arena
        //instead of uint8 (in previous line) is bool used
        bool   m_IsArena;
        uint8  m_Winner;                                    // 0=alliance, 1=horde, 2=none
        int32  m_StartDelayTime;
        bool   m_IsRated;                                   // is this battle rated?
        bool   m_PrematureCountDown;
        uint32 m_PrematureCountDownTimer;
        char const* m_Name;

        /* Player lists */
        std::vector<uint64> m_ResurrectQueue;               // Player GUID
        std::map<uint64, uint8> m_RemovedPlayers;           // uint8 is remove type (0 - bgqueue, 1 - bg, 2 - resurrect queue)

        /* Invited counters are useful for player invitation to BG - do not allow, if BG is started to one faction to have 2 more players than another faction */
        /* Invited counters will be changed only when removing already invited player from queue, removing player from battleground and inviting player to BG */
        /* Invited players counters*/
        uint32 m_InvitedAlliance;
        uint32 m_InvitedHorde;

        /* Raid Group */
        Group* m_BgRaids[BG_TEAMS_COUNT];                                // 0 - alliance, 1 - horde

        /* Players count by team */
        uint32 m_PlayersCount[BG_TEAMS_COUNT];

        /* Arena team ids by team */
        uint32 m_ArenaTeamIds[BG_TEAMS_COUNT];

        int32 m_ArenaTeamRatingChanges[BG_TEAMS_COUNT];

        /* Limits */
        uint32 m_LevelMin;
        uint32 m_LevelMax;
        uint32 m_MaxPlayersPerTeam;
        uint32 m_MaxPlayers;
        uint32 m_MinPlayersPerTeam;
        uint32 m_MinPlayers;

        /* Start location */
        uint32 m_MapId;
        BattlegroundMap* m_Map;
        float m_TeamStartLocX[BG_TEAMS_COUNT];
        float m_TeamStartLocY[BG_TEAMS_COUNT];
        float m_TeamStartLocZ[BG_TEAMS_COUNT];
        float m_TeamStartLocO[BG_TEAMS_COUNT];
        float m_StartMaxDist;
};
#endif

