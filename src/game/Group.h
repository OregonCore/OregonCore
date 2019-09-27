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

#ifndef OREGONCORE_GROUP_H
#define OREGONCORE_GROUP_H

#include "GroupReference.h"
#include "GroupRefManager.h"
#include "LootMgr.h"

#include <map>
#include <vector>

#define MAXGROUPSIZE 5
#define MAXRAIDSIZE 40
#define TARGETICONCOUNT 8

enum RollVote
{
    ROLL_PASS              = 0,
    ROLL_NEED              = 1,
    ROLL_GREED             = 2,

    // other not send by client
    ROLL_NOT_EMITED_YET    = 3,                             // send to client
    ROLL_NOT_VALID         = 4                              // not send to client
};

enum GroupMemberOnlineStatus
{
    MEMBER_STATUS_OFFLINE   = 0x0000,
    MEMBER_STATUS_ONLINE    = 0x0001,
    MEMBER_STATUS_PVP       = 0x0002,
    MEMBER_STATUS_UNK0      = 0x0004,                       // dead? (health=0)
    MEMBER_STATUS_UNK1      = 0x0008,                       // ghost? (health=1)
    MEMBER_STATUS_UNK2      = 0x0010,                       // never seen
    MEMBER_STATUS_UNK3      = 0x0020,                       // never seen
    MEMBER_STATUS_UNK4      = 0x0040,                       // appears with dead and ghost flags
    MEMBER_STATUS_UNK5      = 0x0080,                       // never seen
    MEMBER_STATUS_RAF_BUDDY = 0x0100                        // Refer-A-Friend Flag
};

enum GroupType
{
    GROUPTYPE_NORMAL = 0,
    GROUPTYPE_RAID   = 1
};

enum GroupFlagMask
{
    GROUP_ASSISTANT      = 0x01,
    GROUP_MAIN_ASSISTANT = 0x02,
    GROUP_MAIN_TANK      = 0x04,
};

class Battleground;

enum GroupUpdateFlags
{
    GROUP_UPDATE_FLAG_NONE              = 0x00000000,       // nothing
    GROUP_UPDATE_FLAG_STATUS            = 0x00000001,       // uint16, flags
    GROUP_UPDATE_FLAG_CUR_HP            = 0x00000002,       // uint16
    GROUP_UPDATE_FLAG_MAX_HP            = 0x00000004,       // uint16
    GROUP_UPDATE_FLAG_POWER_TYPE        = 0x00000008,       // uint8
    GROUP_UPDATE_FLAG_CUR_POWER         = 0x00000010,       // uint16
    GROUP_UPDATE_FLAG_MAX_POWER         = 0x00000020,       // uint16
    GROUP_UPDATE_FLAG_LEVEL             = 0x00000040,       // uint16
    GROUP_UPDATE_FLAG_ZONE              = 0x00000080,       // uint16
    GROUP_UPDATE_FLAG_POSITION          = 0x00000100,       // uint16, uint16
    GROUP_UPDATE_FLAG_AURAS             = 0x00000200,       // uint64 mask, for each bit set uint16 spellid + uint8 unk
    GROUP_UPDATE_FLAG_PET_GUID          = 0x00000400,       // uint64 pet guid
    GROUP_UPDATE_FLAG_PET_NAME          = 0x00000800,       // pet name, NULL terminated string
    GROUP_UPDATE_FLAG_PET_MODEL_ID      = 0x00001000,       // uint16, model id
    GROUP_UPDATE_FLAG_PET_CUR_HP        = 0x00002000,       // uint16 pet cur health
    GROUP_UPDATE_FLAG_PET_MAX_HP        = 0x00004000,       // uint16 pet max health
    GROUP_UPDATE_FLAG_PET_POWER_TYPE    = 0x00008000,       // uint8 pet power type
    GROUP_UPDATE_FLAG_PET_CUR_POWER     = 0x00010000,       // uint16 pet cur power
    GROUP_UPDATE_FLAG_PET_MAX_POWER     = 0x00020000,       // uint16 pet max power
    GROUP_UPDATE_FLAG_PET_AURAS         = 0x00040000,       // uint64 mask, for each bit set uint16 spellid + uint8 unk, pet auras...
    GROUP_UPDATE_PET                    = 0x0007FC00,       // all pet flags
    GROUP_UPDATE_FULL                   = 0x0007FFFF,       // all known flags
};

enum RemoveMethod
{
    GROUP_REMOVEMETHOD_DEFAULT = 0,
    GROUP_REMOVEMETHOD_KICK    = 1,
    GROUP_REMOVEMETHOD_LEAVE   = 2,
};


#define GROUP_UPDATE_FLAGS_COUNT          20

// 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,11,12,13,14,15,16,17,18,19
static const uint8 GroupUpdateLength[GROUP_UPDATE_FLAGS_COUNT] = { 0, 2, 2, 2, 1, 2, 2, 2, 2, 4, 8, 8, 1, 2, 2, 2, 1, 2, 2, 8};

class InstanceSave;

class Roll : public LootValidatorRef
{
    public:
        Roll(uint64 _guid, LootItem const& li)
            : itemGUID(_guid), itemid(li.itemid), itemRandomPropId(li.randomPropertyId), itemRandomSuffix(li.randomSuffix),
              totalPlayersRolling(0), totalNeed(0), totalGreed(0), totalPass(0), itemSlot(0) {}
        ~Roll() override { }
        void setLoot(Loot* pLoot)
        {
            link(pLoot, this);
        }
        Loot* getLoot()
        {
            return getTarget();
        }
        void targetObjectBuildLink() override;

        uint64 itemGUID;
        uint32 itemid;
        int32  itemRandomPropId;
        uint32 itemRandomSuffix;
        typedef std::map<uint64, RollVote> PlayerVote;
        PlayerVote playerVote;                              //vote position correspond with player position (in group)
        uint8 totalPlayersRolling;
        uint8 totalNeed;
        uint8 totalGreed;
        uint8 totalPass;
        uint8 itemSlot;
};

struct InstanceGroupBind
{
    InstanceSave* save;
    bool perm;
    /* permanent InstanceGroupBinds exist iff the leader has a permanent
       PlayerInstanceBind for the same instance. */
    InstanceGroupBind() : save(NULL), perm(false) {}
};

/** request member stats checken **/
/** @todo uninvite people that not accepted invite **/
class Group
{
    public:
        struct MemberSlot
        {
            uint64      guid;
            std::string name;
            uint8       group;
            bool        assistant;
        };
        typedef std::list<MemberSlot> MemberSlotList;
        typedef MemberSlotList::const_iterator member_citerator;

        typedef UNORDERED_MAP< uint32 /*mapId*/, InstanceGroupBind> BoundInstancesMap;
    protected:
        typedef MemberSlotList::iterator member_witerator;
        typedef std::set<Player*> InvitesList;

        typedef std::vector<Roll*> Rolls;

    public:
        Group();
        ~Group();

        // group manipulation methods
        bool   Create(const uint64& guid, const char* name);
        bool   LoadGroupFromDB(const uint64& leaderGuid, QueryResult_AutoPtr result = QueryResult_AutoPtr(NULL), bool loadMembers = true);
        bool   LoadMemberFromDB(uint32 guidLow, uint8 subgroup, bool assistant);
        bool   AddInvite(Player* player, bool leader);
        uint32 RemoveInvite(Player* player);
        void   RemoveAllInvites();
        bool   AddLeaderInvite(Player* player);
        bool   AddMember(const uint64& guid, const char* name);
        uint32 RemoveMember(const uint64 &guid, const RemoveMethod &method = GROUP_REMOVEMETHOD_DEFAULT, uint64 kicker = 0, const char* reason = NULL);
        void   ChangeLeader(const uint64& guid);
        void   SetLootMethod(LootMethod method)
        {
            m_lootMethod = method;
        }
        void   SetLooterGuid(const uint64& guid)
        {
            m_looterGuid = guid;
        }
        void   SetMasterLooterGuid(uint64 guid) { m_masterLooterGuid = guid; }
        void   UpdateLooterGuid(WorldObject* object, bool ifneed = false);
        void   SetLootThreshold(ItemQualities threshold)
        {
            m_lootThreshold = threshold;
        }
        void   Disband(bool hideDestroy = false);

        // properties accessories
        bool IsFull() const
        {
            return (m_groupType == GROUPTYPE_NORMAL) ? (m_memberSlots.size() >= MAXGROUPSIZE) : (m_memberSlots.size() >= MAXRAIDSIZE);
        }
        bool isRaidGroup() const
        {
            return m_groupType == GROUPTYPE_RAID;
        }
        bool isBGGroup()   const
        {
            return m_bgGroup != NULL;
        }
        bool IsCreated()   const
        {
            return GetMembersCount() > 0;
        }
        const uint64& GetLeaderGUID() const
        {
            return m_leaderGuid;
        }
        const char* GetLeaderName() const
        {
            return m_leaderName.c_str();
        }
        LootMethod    GetLootMethod() const
        {
            return m_lootMethod;
        }
        uint64 GetLooterGuid() const
        {
            if (GetLootMethod() == FREE_FOR_ALL)
                return 0;
            return m_looterGuid;
        }
        uint64 GetMasterLooterGuid() const { return m_masterLooterGuid; }
        ItemQualities GetLootThreshold() const
        {
            return m_lootThreshold;
        }

        // member manipulation methods
        bool IsMember(const uint64& guid) const
        {
            return _getMemberCSlot(guid) != m_memberSlots.end();
        }
        bool IsLeader(const uint64& guid) const
        {
            return (GetLeaderGUID() == guid);
        }
        uint64 GetMemberGUID(const std::string& name)
        {
            for (member_citerator itr = m_memberSlots.begin(); itr != m_memberSlots.end(); ++itr)
            {
                if (itr->name == name)
                    return itr->guid;
            }
            return 0;
        }
        bool IsAssistant(uint64 guid) const
        {
            member_citerator mslot = _getMemberCSlot(guid);
            if (mslot == m_memberSlots.end())
                return false;

            return mslot->assistant;
        }
        Player* GetInvited(const uint64& guid) const;
        Player* GetInvited(const std::string& name) const;

        bool SameSubGroup(uint64 guid1, const uint64& guid2) const
        {
            member_citerator mslot2 = _getMemberCSlot(guid2);
            if (mslot2 == m_memberSlots.end())
                return false;

            return SameSubGroup(guid1, &*mslot2);
        }

        bool SameSubGroup(uint64 guid1, MemberSlot const* slot2) const
        {
            member_citerator mslot1 = _getMemberCSlot(guid1);
            if (mslot1 == m_memberSlots.end() || !slot2)
                return false;

            return (mslot1->group == slot2->group);
        }

        bool HasFreeSlotSubGroup(uint8 subgroup) const
        {
            return (m_subGroupsCounts && m_subGroupsCounts[subgroup] < MAXGROUPSIZE);
        }

        bool SameSubGroup(Player const* member1, Player const* member2) const;

        MemberSlotList const& GetMemberSlots() const
        {
            return m_memberSlots;
        }
        GroupReference* GetFirstMember()
        {
            return m_memberMgr.getFirst();
        }
        GroupReference const* GetFirstMember() const { return m_memberMgr.getFirst(); }
        uint32 GetMembersCount() const
        {
            return m_memberSlots.size();
        }
        void GetDataForXPAtKill(Unit const* victim, uint32& count, uint32& sum_level, Player*& member_with_max_level, Player*& not_gray_member_with_max_level);
        uint8  GetMemberGroup(uint64 guid) const
        {
            member_citerator mslot = _getMemberCSlot(guid);
            if (mslot == m_memberSlots.end())
                return (MAXRAIDSIZE / MAXGROUPSIZE + 1);

            return mslot->group;
        }

        // some additional raid methods
        void ConvertToRaid();

        void SetBattlegroundGroup(Battleground* bg)
        {
            m_bgGroup = bg;
        }
        uint32 CanJoinBattlegroundQueue(uint32 bgTypeId, uint32 bgQueueType, uint32 MinPlayerCount, uint32 MaxPlayerCount, bool isRated, uint32 arenaSlot);

        void ChangeMembersGroup(const uint64& guid, const uint8& group);
        void ChangeMembersGroup(Player* player, const uint8& group);

        void SetAssistant(uint64 guid, const bool& state)
        {
            if (!isRaidGroup())
                return;
            if (_setAssistantFlag(guid, state))
                SendUpdate();
        }
        void SetMainTank(uint64 guid)
        {
            if (!isRaidGroup())
                return;

            if (_setMainTank(guid))
                SendUpdate();
        }
        void SetMainAssistant(uint64 guid)
        {
            if (!isRaidGroup())
                return;

            if (_setMainAssistant(guid))
                SendUpdate();
        }

        void SetTargetIcon(uint8 id, uint64 guid);
        void SetDifficulty(DungeonDifficulty difficulty);
        DungeonDifficulty GetDifficulty()
        {
            return m_difficulty;
        }
        uint16 InInstance();
        bool InCombatToInstance(uint32 instanceId);
        void ResetInstances(uint8 method, Player* SendMsgTo);

        // -no description-
        //void SendInit(WorldSession *session);
        void SendTargetIconList(WorldSession* session);
        void SendUpdate();
        void UpdatePlayerOutOfRange(Player* pPlayer);
        // ignore: GUID of player that will be ignored

        template<class Worker>
        void BroadcastWorker(Worker& worker)
        {
            for (GroupReference* itr = GetFirstMember(); itr != NULL; itr = itr->next())
                worker(itr->GetSource());
        }

        template<class Worker>
        void BroadcastWorker(Worker const& worker) const
        {
            for (GroupReference const* itr = GetFirstMember(); itr != NULL; itr = itr->next())
                worker(itr->GetSource());
        }
        void BroadcastPacket(WorldPacket* packet, bool ignorePlayersInBGRaid, int group = -1, uint64 ignore = 0);
        void BroadcastReadyCheck(WorldPacket* packet);
        void OfflineReadyCheck();

        /*********************************************************/
        /***                  LOOT SYSTEM                     ***/
        /*********************************************************/

        void SendLootStartRoll(uint32 CountDown, const Roll& r);
        void SendLootRoll(const uint64& SourceGuid, const uint64& TargetGuid, uint8 RollNumber, uint8 RollType, const Roll& r);
        void SendLootRollWon(const uint64& SourceGuid, const uint64& TargetGuid, uint8 RollNumber, uint8 RollType, const Roll& r);
        void SendLootAllPassed(uint32 NumberOfPlayers, const Roll& r);
        void SendLooter(Creature* creature, Player* pLooter);
        void GroupLoot(const uint64& playerGUID, Loot* loot, WorldObject* object);
        void NeedBeforeGreed(const uint64& playerGUID, Loot* loot, WorldObject* object);
        void MasterLoot(const uint64& playerGUID, Loot* loot, WorldObject* object);
        Rolls::iterator GetRoll(uint64 Guid)
        {
            Rolls::iterator iter;
            for (iter = RollId.begin(); iter != RollId.end(); ++iter)
            {
                if ((*iter)->itemGUID == Guid && (*iter)->isValid())
                    return iter;
            }
            return RollId.end();
        }
        void CountTheRoll(Rolls::iterator roll, uint32 NumberOfPlayers);
        void CountRollVote(const uint64& playerGUID, const uint64& Guid, uint32 NumberOfPlayers, uint8 Choise);
        void EndRoll();

        void LinkMember(GroupReference* pRef)
        {
            m_memberMgr.insertFirst(pRef);
        }
        void DelinkMember(GroupReference* /*pRef*/) { }

        InstanceGroupBind* BindToInstance(InstanceSave* save, bool permanent, bool load = false);
        void UnbindInstance(uint32 mapid, uint8 difficulty, bool unload = false);
        InstanceGroupBind* GetBoundInstance(Player* player);
        InstanceGroupBind* GetBoundInstance(Map* aMap);
        InstanceGroupBind* GetBoundInstance(MapEntry const* mapEntry);
        InstanceGroupBind* GetBoundInstance(DungeonDifficulty difficulty, uint32 mapId);
        BoundInstancesMap& GetBoundInstances(DungeonDifficulty difficulty);

        // FG: evil hacks
        void BroadcastGroupUpdate(void);

    protected:
        bool _addMember(const uint64& guid, const char* name, bool isAssistant = false);
        bool _addMember(const uint64& guid, const char* name, bool isAssistant, uint8 group);
        bool _removeMember(const uint64& guid);             // returns true if leader has changed
        void _setLeader(const uint64& guid);

        void _removeRolls(const uint64& guid);

        bool _setMembersGroup(const uint64& guid, const uint8& group);
        bool _setAssistantFlag(const uint64& guid, const bool& state);
        bool _setMainTank(const uint64& guid);
        bool _setMainAssistant(const uint64& guid);

        void _homebindIfInstance(Player* player);

        void _initRaidSubGroupsCounter()
        {
            // Sub group counters initialization
            if (!m_subGroupsCounts)
                m_subGroupsCounts = new uint8[MAXRAIDSIZE / MAXGROUPSIZE];

            memset((void*)m_subGroupsCounts, 0, (MAXRAIDSIZE / MAXGROUPSIZE)*sizeof(uint8));

            for (member_citerator itr = m_memberSlots.begin(); itr != m_memberSlots.end(); ++itr)
                ++m_subGroupsCounts[itr->group];
        }

        member_citerator _getMemberCSlot(uint64 Guid) const
        {
            for (member_citerator itr = m_memberSlots.begin(); itr != m_memberSlots.end(); ++itr)
            {
                if (itr->guid == Guid)
                    return itr;
            }
            return m_memberSlots.end();
        }

        member_witerator _getMemberWSlot(uint64 Guid)
        {
            for (member_witerator itr = m_memberSlots.begin(); itr != m_memberSlots.end(); ++itr)
            {
                if (itr->guid == Guid)
                    return itr;
            }
            return m_memberSlots.end();
        }

        void SubGroupCounterIncrease(uint8 subgroup)
        {
            if (m_subGroupsCounts)
                ++m_subGroupsCounts[subgroup];
        }

        void SubGroupCounterDecrease(uint8 subgroup)
        {
            if (m_subGroupsCounts)
                --m_subGroupsCounts[subgroup];
        }

        GroupFlagMask GetFlags(MemberSlot const& slot) const
        {
            uint8 flags = 0;
            if (slot.assistant)
                flags |= GROUP_ASSISTANT;
            if (slot.guid == m_mainAssistant)
                flags |= GROUP_MAIN_ASSISTANT;
            if (slot.guid == m_mainTank)
                flags |= GROUP_MAIN_TANK;
            return GroupFlagMask(flags);
        }
        MemberSlotList      m_memberSlots;
        GroupRefManager     m_memberMgr;
        InvitesList         m_invitees;
        uint64              m_leaderGuid;
        std::string         m_leaderName;
        uint64              m_mainTank;
        uint64              m_mainAssistant;
        GroupType           m_groupType;
        DungeonDifficulty   m_difficulty;
        Battleground*       m_bgGroup;
        uint64              m_targetIcons[TARGETICONCOUNT];
        LootMethod          m_lootMethod;
        ItemQualities       m_lootThreshold;
        uint64              m_looterGuid;
        uint64              m_masterLooterGuid;
        Rolls               RollId;
        BoundInstancesMap   m_boundInstances[TOTAL_DIFFICULTIES];
        uint8*              m_subGroupsCounts;
};

#define sScriptMgr Oregon::Singleton<ScriptMgr>::Instance()
#endif

