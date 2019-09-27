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

#ifndef __BATTLEGROUNDMGR_H
#define __BATTLEGROUNDMGR_H

#include "Battleground.h"
#include "Policies/Singleton.h"

class Battleground;

//TODO it is not possible to have this structure, because we should have BattlegroundSet for each queue
//so i propose to change this type to array 1..MAX_BATTLEGROUND_TYPES of sets or maps..
typedef std::map<uint32, Battleground*> BattlegroundSet;
//typedef std::map<uint32, BattlegroundQueue*> BattlegroundQueueSet;
typedef std::list<Battleground*> BGFreeSlotQueueType;

#define MAX_BATTLEGROUND_QUEUES 7                           // for level ranges 10-19, 20-29, 30-39, 40-49, 50-59, 60-69, 70+

#define MAX_BATTLEGROUND_TYPES 9                            // each BG type will be in array

#define MAX_BATTLEGROUND_QUEUE_TYPES 8

#define BATTLEGROUND_ARENA_POINT_DISTRIBUTION_DAY    86400     // seconds in a day

struct GroupQueueInfo;                                      // type predefinition
struct PlayerQueueInfo                                      // stores information for players in queue
{
    uint32  InviteTime;                                     // first invite time
    uint32  LastInviteTime;                                 // last invite time
    uint32  LastOnlineTime;                                 // for tracking and removing offline players from queue after 5 minutes
    GroupQueueInfo* GroupInfo;                              // pointer to the associated groupqueueinfo
};

struct GroupQueueInfo                                       // stores information about the group in queue (also used when joined as solo!)
{
    std::map<uint64, PlayerQueueInfo*> Players;             // player queue info map
    uint32  Team;                                           // Player team (ALLIANCE/HORDE)
    uint32  BgTypeId;                                       // battleground type id
    bool    IsRated;                                        // rated
    uint8   ArenaType;                                      // 2v2, 3v3, 5v5 or 0 when BG
    uint32  ArenaTeamId;                                    // team id if rated match
    uint32  JoinTime;                                       // time when group was added
    uint32  IsInvitedToBGInstanceGUID;                      // was invited to certain BG
    uint32  ArenaTeamRating;                                // if rated match, inited to the rating of the team
    uint32  OpponentsTeamRating;                            // for rated arena matches
};

class Battleground;
class BattlegroundQueue
{
    public:
        BattlegroundQueue();
        ~BattlegroundQueue();

        void Update(uint32 bgTypeId, uint32 queue_id, uint8 arenatype = 0, bool isRated = false, uint32 minRating = 0);

        GroupQueueInfo* AddGroup(Player* leader, uint32 BgTypeId, uint8 ArenaType, bool isRated, uint32 ArenaRating, uint32 ArenaTeamId = 0);
        void AddPlayer(Player* plr, GroupQueueInfo* ginfo);
        void RemovePlayer(uint64 guid, bool decreaseInvitedCount);
        void DecreaseGroupLength(uint32 queueId, uint32 AsGroup);
        void BGEndedRemoveInvites(Battleground* bg);

        typedef std::map<uint64, PlayerQueueInfo> QueuedPlayersMap;
        QueuedPlayersMap m_QueuedPlayers[MAX_BATTLEGROUND_QUEUES];

        typedef std::list<GroupQueueInfo*> QueuedGroupsList;
        QueuedGroupsList m_QueuedGroups[MAX_BATTLEGROUND_QUEUES];

        // class to hold pointers to the groups eligible for a specific selection pool building mode
        class EligibleGroups : public std::list<GroupQueueInfo*>
        {
            public:
                void Init(QueuedGroupsList* source, uint32 BgTypeId, uint32 side, uint32 MaxPlayers, uint8 ArenaType = 0, bool IsRated = false, uint32 MinRating = 0, uint32 MaxRating = 0, uint32 DisregardTime = 0, uint32 excludeTeam = 0);
        };

        EligibleGroups m_EligibleGroups;

        // class to select and invite groups to bg
        class SelectionPool
        {
            public:
                void Init(EligibleGroups* curr);
                void AddGroup(GroupQueueInfo* group);
                void RemoveGroup(GroupQueueInfo* group);
                uint32 GetPlayerCount() const
                {
                    return PlayerCount;
                }
                bool Build(uint32 MinPlayers, uint32 MaxPlayers, EligibleGroups::iterator startitr);
            public:
                std::list<GroupQueueInfo*> SelectedGroups;
            private:
                uint32 PlayerCount;
                EligibleGroups* m_CurrEligGroups;
        };

        enum SelectionPoolBuildMode
        {
            NORMAL_ALLIANCE,
            NORMAL_HORDE,
            ONESIDE_ALLIANCE_TEAM1,
            ONESIDE_ALLIANCE_TEAM2,
            ONESIDE_HORDE_TEAM1,
            ONESIDE_HORDE_TEAM2,

            NUM_SELECTION_POOL_TYPES
        };

        SelectionPool m_SelectionPools[NUM_SELECTION_POOL_TYPES];

        bool BuildSelectionPool(uint32 bgTypeId, uint32 queue_id, uint32 MinPlayers, uint32 MaxPlayers, SelectionPoolBuildMode mode, uint8 ArenaType = 0, bool isRated = false, uint32 MinRating = 0, uint32 MaxRating = 0, uint32 DisregardTime = 0, uint32 excludeTeam = 0);

    private:

        bool InviteGroupToBG(GroupQueueInfo* ginfo, Battleground* bg, uint32 side);
};

/*
    This class is used to invite player to BG again, when minute lasts from his first invitation
    it is capable to solve all possibilities
*/
class BGQueueInviteEvent : public BasicEvent
{
    public:
        BGQueueInviteEvent(uint64 pl_guid, uint32 BgInstanceGUID) : m_PlayerGuid(pl_guid), m_BgInstanceGUID(BgInstanceGUID) {};
        virtual ~BGQueueInviteEvent() {};

        virtual bool Execute(uint64 e_time, uint32 p_time);
        virtual void Abort(uint64 e_time);
    private:
        uint64 m_PlayerGuid;
        uint32 m_BgInstanceGUID;
};

/*
    This class is used to remove player from BG queue after 2 minutes from first invitation
*/
class BGQueueRemoveEvent : public BasicEvent
{
    public:
        BGQueueRemoveEvent(uint64 pl_guid, uint32 bgInstanceGUID, uint32 playersTeam) : m_PlayerGuid(pl_guid), m_BgInstanceGUID(bgInstanceGUID), m_PlayersTeam(playersTeam) {};
        virtual ~BGQueueRemoveEvent() {};

        virtual bool Execute(uint64 e_time, uint32 p_time);
        virtual void Abort(uint64 e_time);
    private:
        uint64 m_PlayerGuid;
        uint32 m_BgInstanceGUID;
        uint32 m_PlayersTeam;
};

class BattlegroundMgr
{
    public:
        /* Construction */
        BattlegroundMgr();
        ~BattlegroundMgr();
        void Update(uint32 diff);

        /* Packet Building */
        void BuildPlayerJoinedBattlegroundPacket(WorldPacket* data, Player* plr);
        void BuildPlayerLeftBattlegroundPacket(WorldPacket* data, const uint64& guid);
        void BuildBattlegroundListPacket(WorldPacket* data, uint64 guid, Player* plr, uint32 bgTypeId);
        void BuildGroupJoinedBattlegroundPacket(WorldPacket* data, uint32 bgTypeId);
        void BuildUpdateWorldStatePacket(WorldPacket* data, uint32 field, uint32 value);
        void BuildPvpLogDataPacket(WorldPacket* data, Battleground* bg);
        void BuildBattlegroundStatusPacket(WorldPacket* data, Battleground* bg, uint32 team, uint8 QueueSlot, uint8 StatusID, uint32 Time1, uint32 Time2, uint32 arenatype = 0, uint8 israted = 0);
        void BuildPlaySoundPacket(WorldPacket* data, uint32 soundid);

        /* Player invitation */
        // called from Queue update, or from Addplayer to queue
        void InvitePlayer(Player* plr, uint32 bgInstanceGUID, uint32 team);

        /* Battlegrounds */
        BattlegroundSet::iterator GetBattlegroundsBegin()
        {
            return m_Battlegrounds.begin();
        };
        BattlegroundSet::iterator GetBattlegroundsEnd()
        {
            return m_Battlegrounds.end();
        };

        Battleground* GetBattleground(uint32 InstanceID)
        {
            if (!InstanceID)
                return NULL;
            BattlegroundSet::iterator i = m_Battlegrounds.find(InstanceID);
            if (i != m_Battlegrounds.end())
                return i->second;
            else
                return NULL;
        };

        Battleground* GetBattlegroundTemplate(uint32 bgTypeId);
        Battleground* CreateNewBattleground(uint32 bgTypeId, uint8 arenaType, bool isRated);

        uint32 CreateBattleground(uint32 bgTypeId, uint32 MinPlayersPerTeam, uint32 MaxPlayersPerTeam, uint32 LevelMin, uint32 LevelMax, char* BattlegroundName, uint32 MapID, float Team1StartLocX, float Team1StartLocY, float Team1StartLocZ, float Team1StartLocO, float Team2StartLocX, float Team2StartLocY, float Team2StartLocZ, float Team2StartLocO, float StartMaxDist);

        void AddBattleground(uint32 ID, Battleground* BG)
        {
            m_Battlegrounds[ID] = BG;
        };
        void RemoveBattleground(uint32 instanceID);

        void CreateInitialBattlegrounds();
        void DeleteAlllBattlegrounds();

        void SendToBattleground(Player* pl, uint32 bgTypeId);

        /* Battleground queues */
        //these queues are instantiated when creating BattlegroundMrg
        BattlegroundQueue m_BattlegroundQueues[MAX_BATTLEGROUND_QUEUE_TYPES]; // public, because we need to access them in BG handler code

        BGFreeSlotQueueType BGFreeSlotQueue[MAX_BATTLEGROUND_TYPES];

        void SendAreaSpiritHealerQueryOpcode(Player* pl, Battleground* bg, uint64 guid);

        bool IsArenaType(uint32 bgTypeId) const;
        bool IsBattlegroundType(uint32 bgTypeId) const;
        static uint32 BGQueueTypeId(uint32 bgTypeId, uint8 arenaType);
        uint32 BGTemplateId(uint32 bgQueueTypeId) const;
        uint8 BGArenaType(uint32 bgQueueTypeId) const;

        uint32 GetMaxRatingDifference() const
        {
            return m_MaxRatingDifference;
        }
        uint32 GetRatingDiscardTimer() const
        {
            return m_RatingDiscardTimer;
        }

        void InitAutomaticArenaPointDistribution();
        void DistributeArenaPoints();
        uint32 GetPrematureFinishTime() const
        {
            return m_PrematureFinishTimer;
        }
        void ToggleArenaTesting();
        void ToggleTesting();
        bool isArenaTesting() const
        {
            return m_ArenaTesting;
        }
        bool isTesting() const
        {
            return m_Testing;
        }

        void SetHolidayWeekends(uint32 mask);
    private:

        /* Battlegrounds */
        BattlegroundSet m_Battlegrounds;
        uint32 m_MaxRatingDifference;
        uint32 m_RatingDiscardTimer;
        uint32 m_NextRatingDiscardUpdate;
        bool   m_AutoDistributePoints;
        uint64 m_NextAutoDistributionTime;
        uint32 m_AutoDistributionTimeChecker;
        uint32 m_PrematureFinishTimer;
        bool   m_ArenaTesting;
        bool   m_Testing;
};

#define sBattlegroundMgr Oregon::Singleton<BattlegroundMgr>::Instance()
#endif

