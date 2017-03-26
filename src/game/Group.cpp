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
#include "Opcodes.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Player.h"
#include "World.h"
#include "ObjectMgr.h"
#include "Group.h"
#include "Formulas.h"
#include "ObjectAccessor.h"
#include "Battleground.h"
#include "MapManager.h"
#include "InstanceSaveMgr.h"
#include "Util.h"
#include "ScriptMgr.h"

Group::Group()
{
    m_leaderGuid        = 0;
    m_mainTank          = 0;
    m_mainAssistant     = 0;
    m_groupType         = GroupType(0);
    m_bgGroup           = NULL;
    m_lootMethod        = LootMethod(0);
    m_looterGuid        = 0;
    m_masterLooterGuid  = 0;
    m_lootThreshold     = ITEM_QUALITY_UNCOMMON;
    m_subGroupsCounts   = NULL;
    m_difficulty        = DIFFICULTY_NORMAL;

    for (uint8 i = 0; i < TARGETICONCOUNT; ++i)
        m_targetIcons[i] = 0;
}

Group::~Group()
{
    if (m_bgGroup)
    {
        sLog.outDebug("Group::~Group: battleground group being deleted.");
        if (m_bgGroup->GetBgRaid(ALLIANCE) == this) m_bgGroup->SetBgRaid(ALLIANCE, NULL);
        else if (m_bgGroup->GetBgRaid(HORDE) == this) m_bgGroup->SetBgRaid(HORDE, NULL);
        else sLog.outError("Group::~Group: battleground group is not linked to the correct battleground.");
    }
    Rolls::iterator itr;
    while (!RollId.empty())
    {
        itr = RollId.begin();
        Roll* r = *itr;
        RollId.erase(itr);
        delete(r);
    }

    // it is undefined whether objectmgr (which stores the groups) or instancesavemgr
    // will be unloaded first so we must be prepared for both cases
    // this may unload some instance saves
    for (uint8 i = 0; i < TOTAL_DIFFICULTIES; i++)
        for (BoundInstancesMap::iterator itr = m_boundInstances[i].begin(); itr != m_boundInstances[i].end(); ++itr)
            itr->second.save->RemoveGroup(this);

    // Sub group counters clean up
    if (m_subGroupsCounts)
        delete[] m_subGroupsCounts;
}

bool Group::Create(const uint64& guid, const char* name)
{
    m_leaderGuid = guid;
    m_leaderName = name;

    m_groupType  = isBGGroup() ? GROUPTYPE_RAID : GROUPTYPE_NORMAL;

    if (m_groupType == GROUPTYPE_RAID)
        _initRaidSubGroupsCounter();

    m_lootMethod = GROUP_LOOT;
    m_lootThreshold = ITEM_QUALITY_UNCOMMON;
    m_looterGuid = guid;
    m_masterLooterGuid = 0;

    m_difficulty = DIFFICULTY_NORMAL;
    Player* leader = sObjectMgr.GetPlayer(guid, true);

    if (!leader)
        return false;

    if (!isBGGroup())
    {
        if (leader)
            m_difficulty = leader->GetDifficulty();

        Player::ConvertInstancesToGroup(leader, this, guid);

        // store group in database
        CharacterDatabase.BeginTransaction();
        CharacterDatabase.PExecute("DELETE FROM groups WHERE leaderGuid ='%u'", GUID_LOPART(m_leaderGuid));
        CharacterDatabase.PExecute("DELETE FROM group_member WHERE leaderGuid ='%u'", GUID_LOPART(m_leaderGuid));
        CharacterDatabase.PExecute("INSERT INTO groups(leaderGuid,mainTank,mainAssistant,lootMethod,looterGuid,lootThreshold,icon1,icon2,icon3,icon4,icon5,icon6,icon7,icon8,isRaid,difficulty,masterLooterGuid) "
            "VALUES('%u','%u','%u','%u','%u','%u','" UI64FMTD "','" UI64FMTD "','" UI64FMTD "','" UI64FMTD "','" UI64FMTD "','" UI64FMTD "','" UI64FMTD "','" UI64FMTD "','%u','%u','%u')",
             GUID_LOPART(m_leaderGuid), GUID_LOPART(m_mainTank), GUID_LOPART(m_mainAssistant), uint32(m_lootMethod),
             GUID_LOPART(m_looterGuid), uint32(m_lootThreshold), m_targetIcons[0], m_targetIcons[1], m_targetIcons[2], m_targetIcons[3], m_targetIcons[4], m_targetIcons[5], m_targetIcons[6], m_targetIcons[7], isRaidGroup(), m_difficulty, m_masterLooterGuid);
    }

    if (!AddMember(guid, name))
        return false;

    if (!isBGGroup()) CharacterDatabase.CommitTransaction();

    sScriptMgr.OnGroupCreated(leader->GetGroup(), leader);
    
    return true;
}

bool Group::LoadGroupFromDB(const uint64& leaderGuid, QueryResult_AutoPtr result, bool loadMembers)
{
    if (isBGGroup())
        return false;

    if (!result)
    {
        //                                       0          1              2           3           4              5      6      7      8      9      10     11     12     13      14          15
        result = CharacterDatabase.PQuery("SELECT mainTank, mainAssistant, lootMethod, looterGuid, lootThreshold, icon1, icon2, icon3, icon4, icon5, icon6, icon7, icon8, isRaid, difficulty, masterLooterGuid FROM groups WHERE leaderGuid ='%u'", GUID_LOPART(leaderGuid));
        if (!result)
            return false;
    }

    m_leaderGuid = leaderGuid;

    // group leader not exist
    if (!sObjectMgr.GetPlayerNameByGUID(m_leaderGuid, m_leaderName))
        return false;

    m_groupType  = (*result)[13].GetBool() ? GROUPTYPE_RAID : GROUPTYPE_NORMAL;

    if (m_groupType == GROUPTYPE_RAID)
        _initRaidSubGroupsCounter();

    m_difficulty = (DungeonDifficulty)(*result)[14].GetUInt8();
    m_mainTank = (*result)[0].GetUInt64();
    m_mainAssistant = (*result)[1].GetUInt64();
    m_lootMethod = (LootMethod)(*result)[2].GetUInt8();
    m_looterGuid = MAKE_NEW_GUID((*result)[3].GetUInt32(), 0, HIGHGUID_PLAYER);
    m_lootThreshold = (ItemQualities)(*result)[4].GetUInt16();
    m_masterLooterGuid = MAKE_NEW_GUID((*result)[15].GetUInt32(), 0, HIGHGUID_PLAYER);

    for (int i = 0; i < TARGETICONCOUNT; i++)
        m_targetIcons[i] = (*result)[5 + i].GetUInt64();

    if (loadMembers)
    {
        result = CharacterDatabase.PQuery("SELECT memberGuid, assistant, subgroup FROM group_member WHERE leaderGuid ='%u'", GUID_LOPART(leaderGuid));
        if (!result)
            return false;

        do
        {
            LoadMemberFromDB((*result)[0].GetUInt32(), (*result)[2].GetUInt8(), (*result)[1].GetBool());
        }
        while (result->NextRow());

        if (GetMembersCount() < 2)                          // group too small
            return false;
    }

    return true;
}

bool Group::LoadMemberFromDB(uint32 guidLow, uint8 subgroup, bool assistant)
{
    MemberSlot member;
    member.guid = MAKE_NEW_GUID(guidLow, 0, HIGHGUID_PLAYER);

    // skip non-existed member
    if (!sObjectMgr.GetPlayerNameByGUID(member.guid, member.name))
        return false;

    member.group = subgroup;
    member.assistant = assistant;

    m_memberSlots.push_back(member);

    SubGroupCounterIncrease(subgroup);

    return true;
}

void Group::ConvertToRaid()
{
    m_groupType = GROUPTYPE_RAID;

    _initRaidSubGroupsCounter();

    if (!isBGGroup())
        CharacterDatabase.PExecute("UPDATE groups SET isRaid = 1 WHERE leaderGuid='%u'", GUID_LOPART(m_leaderGuid));
    SendUpdate();
}

bool Group::AddInvite(Player* player, bool isLeader)
{
    if (!player || player->GetGroupInvite())
        return false;
    Group* group = player->GetGroup();
    if (group && group->isBGGroup())
        group = player->GetOriginalGroup();
    if (group)
        return false;

    RemoveInvite(player);

    m_invitees.insert(player);

    player->SetGroupInvite(this);

    if (!isLeader)
        sScriptMgr.OnGroupPlayerInvited(this, player);

    return true;
}

bool Group::AddLeaderInvite(Player* player)
{
    if (!AddInvite(player, true))
        return false;

    m_leaderGuid = player->GetGUID();
    m_leaderName = player->GetName();
    return true;
}

uint32 Group::RemoveInvite(Player* player)
{
    m_invitees.erase(player);

    player->SetGroupInvite(NULL);
    return GetMembersCount();
}

void Group::RemoveAllInvites()
{
    for (InvitesList::iterator itr = m_invitees.begin(); itr != m_invitees.end(); ++itr)
        (*itr)->SetGroupInvite(NULL);

    m_invitees.clear();
}

Player* Group::GetInvited(const uint64& guid) const
{
    for (InvitesList::const_iterator itr = m_invitees.begin(); itr != m_invitees.end(); ++itr)
    {
        if ((*itr)->GetGUID() == guid)
            return (*itr);
    }
    return NULL;
}

Player* Group::GetInvited(const std::string& name) const
{
    for (InvitesList::const_iterator itr = m_invitees.begin(); itr != m_invitees.end(); ++itr)
    {
        if ((*itr)->GetName() == name)
            return (*itr);
    }
    return NULL;
}

bool Group::AddMember(const uint64& guid, const char* name)
{
    if (!_addMember(guid, name))
        return false;
    SendUpdate();

    Player* player = sObjectMgr.GetPlayer(guid, true);
    if (player)
    {
        if (!IsLeader(player->GetGUID()) && !isBGGroup())
        {
            // reset the new member's instances, unless he is currently in one of them
            // including raid/heroic instances that they are not permanently bound to!
            player->ResetInstances(INSTANCE_RESET_GROUP_JOIN);

            if (player->getLevel() >= LEVELREQUIREMENT_HEROIC && player->GetDifficulty() != GetDifficulty())
            {
                player->SetDifficulty(m_difficulty);
                player->SendDungeonDifficulty(true);
            }
            sScriptMgr.OnGroupPlayerJoined(this, player);
        }
        player->SetGroupUpdateFlag(GROUP_UPDATE_FULL);
        UpdatePlayerOutOfRange(player);
    }

    return true;
}

uint32 Group::RemoveMember(const uint64& guid, const RemoveMethod& method /* = GROUP_REMOVEMETHOD_DEFAULT */, uint64 kicker /* = 0 */, const char* reason /* = NULL */)
{
    BroadcastGroupUpdate();

    // remove member and change leader (if need) only if strong more 2 members _before_ member remove
    if (GetMembersCount() > (isBGGroup() ? 1 : 2))           // in BG group case allow 1 members group
    {
        bool leaderChanged = _removeMember(guid);

        if (Player* player = sObjectMgr.GetPlayer(guid, true))
        {
            WorldPacket data;

            sScriptMgr.OnGroupPlayerRemoved(this, player, method, kicker, reason);

            if (method == GROUP_REMOVEMETHOD_KICK)
            {
                data.Initialize(SMSG_GROUP_UNINVITE, 0);
                player->GetSession()->SendPacket(&data);
            }

            // we already removed player from group and in player->GetGroup() is his original group!
            if (Group* group = player->GetGroup())
                group->SendUpdate();
            else
            {
                data.Initialize(SMSG_GROUP_LIST, 24);
                data << uint64(0) << uint64(0) << uint64(0);
                player->GetSession()->SendPacket(&data);
            }

            _homebindIfInstance(player);
            
        }

        if (leaderChanged)
        {
            WorldPacket data(SMSG_GROUP_SET_LEADER, (m_memberSlots.front().name.size() + 1));
            data << m_memberSlots.front().name;
            BroadcastPacket(&data, true);
        }

        SendUpdate();
    }
    // if group before remove <= 2 disband it
    else
        Disband(true);

    
    return m_memberSlots.size();
}

void Group::ChangeLeader(const uint64& guid)
{
    member_citerator slot = _getMemberCSlot(guid);

    if (slot == m_memberSlots.end())
        return;

    Player* oldLeader = sObjectMgr.GetPlayer(this->GetLeaderGUID());
    _setLeader(guid);
    Player* newLeader = sObjectMgr.GetPlayer(guid);

    if (oldLeader && newLeader)
        sScriptMgr.OnGroupLeaderChanged(this, oldLeader, newLeader);

    WorldPacket data(SMSG_GROUP_SET_LEADER, slot->name.size() + 1);
    data << slot->name;
    BroadcastPacket(&data, true);
    SendUpdate();
}

void Group::Disband(bool hideDestroy)
{
    Player* player = sObjectMgr.GetPlayer(this->GetLeaderGUID());
    
    if (player && player->GetSession())
        sScriptMgr.OnGroupDisbanded(this, player);

    for (member_citerator citr = m_memberSlots.begin(); citr != m_memberSlots.end(); ++citr)
    {
        player = sObjectMgr.GetPlayer(citr->guid, true);
        if (!player)
            continue;

        // we cannot call _removeMember because it would invalidate member iterator
        // if we are removing player from battleground raid
        if (isBGGroup())
            player->RemoveFromBattlegroundRaid();
        else
        {
            // we can remove player who is in battleground from his original group
            if (player->GetOriginalGroup() == this)
                player->SetOriginalGroup(NULL);
            else
                player->SetGroup(NULL);
        }

        if (!player->GetSession())
            continue;

        WorldPacket data;
        if (!hideDestroy)
        {
            data.Initialize(SMSG_GROUP_DESTROYED, 0);
            player->GetSession()->SendPacket(&data);
        }

        // we already removed player from group and in player->GetGroup() is his original group, send update
        if (Group* group = player->GetGroup())
            group->SendUpdate();
        else
        {
            data.Initialize(SMSG_GROUP_LIST, 24);
            data << uint64(0) << uint64(0) << uint64(0);
            player->GetSession()->SendPacket(&data);
        }

        _homebindIfInstance(player);
    }
    RollId.clear();
    m_memberSlots.clear();

    RemoveAllInvites();

    if (!isBGGroup())
    {
        CharacterDatabase.BeginTransaction();
        CharacterDatabase.PExecute("DELETE FROM groups WHERE leaderGuid='%u'", GUID_LOPART(m_leaderGuid));
        CharacterDatabase.PExecute("DELETE FROM group_member WHERE leaderGuid='%u'", GUID_LOPART(m_leaderGuid));
        CharacterDatabase.CommitTransaction();
        ResetInstances(INSTANCE_RESET_GROUP_DISBAND, NULL);
    }

    m_leaderGuid = 0;
    m_leaderName = "";
}

/*********************************************************/
/***                  LOOT SYSTEM                     ***/
/*********************************************************/

void Group::SendLootStartRoll(uint32 CountDown, const Roll& r)
{
    WorldPacket data(SMSG_LOOT_START_ROLL, (8 + 4 + 4 + 4 + 4 + 4));
    data << uint64(r.itemGUID);                             // guid of rolled item
    data << uint32(r.totalPlayersRolling);                  // maybe the number of players rolling for it???
    data << uint32(r.itemid);                               // the itemEntryId for the item that shall be rolled for
    data << uint32(r.itemRandomSuffix);                     // randomSuffix
    data << uint32(r.itemRandomPropId);                     // item random property ID
    data << uint32(CountDown);                              // the countdown time to choose "need" or "greed"

    for (Roll::PlayerVote::const_iterator itr = r.playerVote.begin(); itr != r.playerVote.end(); ++itr)
    {
        Player* p = sObjectMgr.GetPlayer(itr->first);
        if (!p || !p->GetSession())
            continue;

        if (itr->second != ROLL_NOT_VALID)
            p->GetSession()->SendPacket(&data);
    }
}

void Group::SendLootRoll(const uint64& SourceGuid, const uint64& TargetGuid, uint8 RollNumber, uint8 RollType, const Roll& r)
{
    WorldPacket data(SMSG_LOOT_ROLL, (8 + 4 + 8 + 4 + 4 + 4 + 1 + 1 + 1));
    data << uint64(SourceGuid);                             // guid of the item rolled
    data << uint32(0);                                      // unknown, maybe amount of players
    data << uint64(TargetGuid);
    data << uint32(r.itemid);                               // the itemEntryId for the item that shall be rolled for
    data << uint32(r.itemRandomSuffix);                     // randomSuffix
    data << uint32(r.itemRandomPropId);                     // Item random property ID
    data << uint8(RollNumber);                              // 0: "Need for: [item name]" > 127: "you passed on: [item name]"      Roll number
    data << uint8(RollType);                                // 0: "Need for: [item name]" 0: "You have selected need for [item name] 1: need roll 2: greed roll
    data << uint8(0);                                       // auto pass on NeedBeforeGreed loot because player cannot use the object

    for (Roll::PlayerVote::const_iterator itr = r.playerVote.begin(); itr != r.playerVote.end(); ++itr)
    {
        Player* p = sObjectMgr.GetPlayer(itr->first, true);
        if (!p || !p->GetSession())
            continue;

        if (itr->second != ROLL_NOT_VALID)
            p->GetSession()->SendPacket(&data);
    }
}

void Group::SendLootRollWon(const uint64& SourceGuid, const uint64& TargetGuid, uint8 RollNumber, uint8 RollType, const Roll& r)
{
    WorldPacket data(SMSG_LOOT_ROLL_WON, (8 + 4 + 4 + 4 + 4 + 8 + 1 + 1));
    data << uint64(SourceGuid);                             // guid of the item rolled
    data << uint32(0);                                      // unknown, maybe amount of players
    data << uint32(r.itemid);                               // the itemEntryId for the item that shall be rolled for
    data << uint32(r.itemRandomSuffix);                     // randomSuffix
    data << uint32(r.itemRandomPropId);                     // Item random property
    data << uint64(TargetGuid);                             // guid of the player who won.
    data << uint8(RollNumber);                              // rollnumber realted to SMSG_LOOT_ROLL
    data << uint8(RollType);                                // Rolltype related to SMSG_LOOT_ROLL

    for (Roll::PlayerVote::const_iterator itr = r.playerVote.begin(); itr != r.playerVote.end(); ++itr)
    {
        Player* p = sObjectMgr.GetPlayer(itr->first, true);
        if (!p || !p->GetSession())
            continue;

        if (itr->second != ROLL_NOT_VALID)
            p->GetSession()->SendPacket(&data);
    }
}

void Group::SendLootAllPassed(uint32 NumberOfPlayers, const Roll& r)
{
    WorldPacket data(SMSG_LOOT_ALL_PASSED, (8 + 4 + 4 + 4 + 4));
    data << uint64(r.itemGUID);                             // Guid of the item rolled
    data << uint32(NumberOfPlayers);                        // The number of players rolling for it???
    data << uint32(r.itemid);                               // The itemEntryId for the item that shall be rolled for
    data << uint32(r.itemRandomPropId);                     // Item random property ID
    data << uint32(r.itemRandomSuffix);                     // Item random suffix ID

    for (Roll::PlayerVote::const_iterator itr = r.playerVote.begin(); itr != r.playerVote.end(); ++itr)
    {
        Player* p = sObjectMgr.GetPlayer(itr->first, true);
        if (!p || !p->GetSession())
            continue;

        if (itr->second != ROLL_NOT_VALID)
            p->GetSession()->SendPacket(&data);
    }
}

// notify group members which player is the allowed looter for the given creature
void Group::SendLooter(Creature* creature, Player* groupLooter)
{
    ASSERT(creature);

    WorldPacket data(SMSG_LOOT_LIST, (8+8));
    data << uint64(creature->GetGUID());

    if (GetLootMethod() == MASTER_LOOT && creature->loot.hasOverThresholdItem())
        data << GetMasterLooterGuid();
    else
        data << uint8(0);

    if (groupLooter)
        data << groupLooter->GetPackGUID();
    else
        data << uint8(0);

    BroadcastPacket(&data, false);
}

void Group::GroupLoot(const uint64& playerGUID, Loot* loot, WorldObject* object)
{
    std::vector<LootItem>::iterator i;
    ItemTemplate const* item;
    uint8 itemSlot = 0;
    Player* player = sObjectMgr.GetPlayer(playerGUID);
    Group* group = player->GetGroup();

    for (i = loot->items.begin(); i != loot->items.end(); ++i, ++itemSlot)
    {
        if (i->freeforall)
            continue;

        item = sObjectMgr.GetItemTemplate(i->itemid);
        if (!item)
        {
            //sLog.outDebug("Group::GroupLoot: missing item prototype for item with id: %d", i->itemid);
            continue;
        }

        //roll for over-threshold item if it's one-player loot
        if (item->Quality >= uint32(m_lootThreshold))
        {
            uint64 newitemGUID = MAKE_NEW_GUID(sObjectMgr.GenerateLowGuid(HIGHGUID_ITEM), 0, HIGHGUID_ITEM);
            Roll* r = new Roll(newitemGUID, *i);

            //a vector is filled with only near party members
            for (GroupReference* itr = GetFirstMember(); itr != NULL; itr = itr->next())
            {
                Player* member = itr->GetSource();
                if (!member || !member->GetSession())
                    continue;
                if (i->AllowedForPlayer(member))
                {
                    if (member->IsWithinDistInMap(object, sWorld.getConfig(CONFIG_GROUP_XP_DISTANCE), false))
                    {
                        r->playerVote[member->GetGUID()] = ROLL_NOT_EMITED_YET;
                        ++r->totalPlayersRolling;
                    }
                }
            }

            r->setLoot(loot);
            r->itemSlot = itemSlot;

            group->SendLootStartRoll(60000, *r);

            loot->items[itemSlot].is_blocked = true;
            object->m_groupLootTimer = 60000;
            object->lootingGroupLeaderGUID = GetLeaderGUID();

            RollId.push_back(r);
        }
        else
            i->is_underthreshold = 1;
    }
}

void Group::NeedBeforeGreed(const uint64& playerGUID, Loot* loot, WorldObject* object)
{
    ItemTemplate const* item;
    Player* player = sObjectMgr.GetPlayer(playerGUID);
    Group* group = player->GetGroup();

    uint8 itemSlot = 0;
    for (std::vector<LootItem>::iterator i = loot->items.begin(); i != loot->items.end(); ++i, ++itemSlot)
    {
        item = sObjectMgr.GetItemTemplate(i->itemid);

        //only roll for one-player items, not for ones everyone can get
        if (item->Quality >= uint32(m_lootThreshold) && !i->freeforall)
        {
            uint64 newitemGUID = MAKE_NEW_GUID(sObjectMgr.GenerateLowGuid(HIGHGUID_ITEM), 0, HIGHGUID_ITEM);
            Roll* r = new Roll(newitemGUID, *i);

            for (GroupReference* itr = GetFirstMember(); itr != NULL; itr = itr->next())
            {
                Player* playerToRoll = itr->GetSource();
                if (!playerToRoll || !playerToRoll->GetSession())
                    continue;

                if (playerToRoll->CanUseItem(item) && i->AllowedForPlayer(playerToRoll))
                {
                    if (playerToRoll->GetDistance2d(object) < sWorld.getConfig(CONFIG_GROUP_XP_DISTANCE))
                    {
                        r->playerVote[playerToRoll->GetGUID()] = ROLL_NOT_EMITED_YET;
                        ++r->totalPlayersRolling;
                    }
                }
            }

            if (r->totalPlayersRolling > 0)
            {
                r->setLoot(loot);
                r->itemSlot = itemSlot;

                group->SendLootStartRoll(60000, *r);

                loot->items[itemSlot].is_blocked = true;

                RollId.push_back(r);
            }
            else
                delete r;
        }
        else
            i->is_underthreshold = 1;
    }
}

void Group::MasterLoot(const uint64& playerGUID, Loot* loot, WorldObject* object)
{
    Player* player = sObjectMgr.GetPlayer(playerGUID);
    if (!player)
        return;

    sLog.outDebug("Group::MasterLoot (SMSG_LOOT_MASTER_LIST, 330) player = [%s].", player->GetName());

    for (std::vector<LootItem>::iterator i = loot->items.begin(); i != loot->items.end(); ++i)
    {
        if (i->freeforall)
            continue;

        i->is_blocked = !i->is_underthreshold;
    }

    for (std::vector<LootItem>::iterator i = loot->quest_items.begin(); i != loot->quest_items.end(); ++i)
        i->is_blocked = !i->is_underthreshold;

    uint32 real_count = 0;

    WorldPacket data(SMSG_LOOT_MASTER_LIST, 330);
    data << (uint8)GetMembersCount();

    for (GroupReference* itr = GetFirstMember(); itr != NULL; itr = itr->next())
    {
        Player* looter = itr->GetSource();
        if (!looter->IsInWorld())
            continue;

        if (looter->GetDistance2d(object) < sWorld.getConfig(CONFIG_GROUP_XP_DISTANCE))
        {
            data << uint64(looter->GetGUID());
            ++real_count;
        }
    }

    data.put<uint8>(0, real_count);

    for (GroupReference* itr = GetFirstMember(); itr != NULL; itr = itr->next())
    {
        Player* looter = itr->GetSource();
        if (looter->GetDistance2d(object) < sWorld.getConfig(CONFIG_GROUP_XP_DISTANCE))
            looter->GetSession()->SendPacket(&data);
    }
}

void Group::CountRollVote(const uint64& playerGUID, const uint64& Guid, uint32 NumberOfPlayers, uint8 Choice)
{
    Rolls::iterator rollI = GetRoll(Guid);
    if (rollI == RollId.end())
        return;
    Roll* roll = *rollI;

    Roll::PlayerVote::iterator itr = roll->playerVote.find(playerGUID);
    // this condition means that player joins to the party after roll begins
    if (itr == roll->playerVote.end())
        return;

    if (roll->getLoot())
        if (roll->getLoot()->items.empty())
            return;

    switch (Choice)
    {
    case ROLL_PASS:                                     // Player choose pass
            SendLootRoll(0, playerGUID, 128, 128, *roll);
            ++roll->totalPass;
            itr->second = ROLL_PASS;
            break;
    case ROLL_NEED:                                     // player choose Need
            SendLootRoll(0, playerGUID, 0, 0, *roll);
            ++roll->totalNeed;
            itr->second = ROLL_NEED;
            break;
    case ROLL_GREED:                                    // player choose Greed
            SendLootRoll(0, playerGUID, 128, 2, *roll);
            ++roll->totalGreed;
            itr->second = ROLL_GREED;
            break;
    default:                                            // Roll removed case
        break;
    }

    if (roll->totalPass + roll->totalGreed + roll->totalNeed >= roll->totalPlayersRolling)
        CountTheRoll(rollI, NumberOfPlayers);
}

//called when roll timer expires
void Group::EndRoll()
{
    Rolls::iterator itr;
    while (!RollId.empty())
    {
        //need more testing here, if rolls disappear
        itr = RollId.begin();
        CountTheRoll(itr, GetMembersCount());               //i don't have to edit player votes, who didn't vote ... he will pass
    }
}

void Group::CountTheRoll(Rolls::iterator rollI, uint32 NumberOfPlayers)
{
    Roll* roll = *rollI;
    if (!roll->isValid())                                    // is loot already deleted ?
    {
        RollId.erase(rollI);
        delete roll;
        return;
    }
    //end of the roll
    if (roll->totalNeed > 0)
    {
        if (!roll->playerVote.empty())
        {
            uint8 maxresul = 0;
            uint64 maxguid  = (*roll->playerVote.begin()).first;
            Player* player;

            for (Roll::PlayerVote::const_iterator itr = roll->playerVote.begin(); itr != roll->playerVote.end(); ++itr)
            {
                if (itr->second != ROLL_NEED)
                    continue;

                uint8 randomN = urand(1, 100);
                SendLootRoll(0, itr->first, randomN, ROLL_NEED, *roll);
                if (maxresul < randomN)
                {
                    maxguid  = itr->first;
                    maxresul = randomN;
                }
            }
            SendLootRollWon(0, maxguid, maxresul, ROLL_NEED, *roll);
            player = sObjectMgr.GetPlayer(maxguid);

            if (player && player->GetSession())
            {
                ItemPosCountVec dest;
                LootItem* item = &(roll->getLoot()->items[roll->itemSlot]);
                uint8 msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, roll->itemid, item->count);
                if (msg == EQUIP_ERR_OK)
                {
                    item->is_looted = true;
                    roll->getLoot()->NotifyItemRemoved(roll->itemSlot);
                    --roll->getLoot()->unlootedCount;
                    player->StoreNewItem(dest, roll->itemid, true, item->randomPropertyId);
                }
                else
                {
                    item->is_blocked = false;
                    player->SendEquipError(msg, NULL, NULL);
                }
            }
        }
    }
    else if (roll->totalGreed > 0)
    {
        if (!roll->playerVote.empty())
        {
            uint8 maxresul = 0;
            uint64 maxguid = (*roll->playerVote.begin()).first;
            Player* player;

            Roll::PlayerVote::iterator itr;
            for (itr = roll->playerVote.begin(); itr != roll->playerVote.end(); ++itr)
            {
                if (itr->second != ROLL_GREED)
                    continue;

                uint8 randomN = urand(1, 100);
                SendLootRoll(0, itr->first, randomN, itr->second, *roll);
                if (maxresul < randomN)
                {
                    maxguid  = itr->first;
                    maxresul = randomN;
                }
            }
            SendLootRollWon(0, maxguid, maxresul, ROLL_GREED, *roll);
            player = sObjectMgr.GetPlayer(maxguid);

            if (player && player->GetSession())
            {
                ItemPosCountVec dest;
                LootItem* item = &(roll->getLoot()->items[roll->itemSlot]);
                uint8 msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, roll->itemid, item->count);
                if (msg == EQUIP_ERR_OK)
                {
                    item->is_looted = true;
                    roll->getLoot()->NotifyItemRemoved(roll->itemSlot);
                    --roll->getLoot()->unlootedCount;
                    player->StoreNewItem(dest, roll->itemid, true, item->randomPropertyId);
                }
                else
                {
                    item->is_blocked = false;
                    player->SendEquipError(msg, NULL, NULL);
                }
            }
        }
    }
    else
    {
        SendLootAllPassed(NumberOfPlayers, *roll);

        // remove is_blocked so that the item is lootable by all players
        LootItem* item = &(roll->itemSlot >= roll->getLoot()->items.size() ? roll->getLoot()->quest_items[roll->itemSlot - roll->getLoot()->items.size()] : roll->getLoot()->items[roll->itemSlot]);
        if (item)
            item->is_blocked = false;
    }
    RollId.erase(rollI);
    delete roll;
}

void Group::SetTargetIcon(uint8 id, uint64 guid)
{
    if (id >= TARGETICONCOUNT)
        return;

    // clean other icons
    if (guid != 0)
        for (int i = 0; i < TARGETICONCOUNT; i++)
            if (m_targetIcons[i] == guid)
                SetTargetIcon(i, 0);

    m_targetIcons[id] = guid;

    WorldPacket data(MSG_RAID_TARGET_UPDATE, (1 + 1 + 8));
    data << (uint8)0;
    data << uint8(id);
    data << uint64(guid);
    BroadcastPacket(&data, true);
}

void Group::GetDataForXPAtKill(Unit const* victim, uint32& count, uint32& sum_level, Player*& member_with_max_level, Player*& not_gray_member_with_max_level)
{
    for (GroupReference* itr = GetFirstMember(); itr != NULL; itr = itr->next())
    {
        Player* member = itr->GetSource();
        if (!member || !member->IsAlive())                   // only for alive
            continue;

        if (!member->IsAtGroupRewardDistance(victim))        // at req. distance
            continue;

        ++count;
        sum_level += member->getLevel();
        if (!member_with_max_level || member_with_max_level->getLevel() < member->getLevel())
            member_with_max_level = member;

        uint32 gray_level = Oregon::XP::GetGrayLevel(member->getLevel());
        // if the victim is higher level than the gray level of the currently examined group member,
        // then set not_gray_member_with_max_level if needed.
        if (victim->getLevel() > gray_level && (!not_gray_member_with_max_level
                                                || not_gray_member_with_max_level->getLevel() < member->getLevel()))
            not_gray_member_with_max_level = member;
    }
}

void Group::SendTargetIconList(WorldSession* session)
{
    if (!session)
        return;

    WorldPacket data(MSG_RAID_TARGET_UPDATE, (1 + TARGETICONCOUNT * 9));
    data << uint8(1);                                       // list targets

    for (int i = 0; i < TARGETICONCOUNT; ++i)
    {
        if (m_targetIcons[i] == 0)
            continue;

        data << uint8(i);
        data << uint64(m_targetIcons[i]);
    }

    session->SendPacket(&data);
}

void Group::SendUpdate()
{
    Player* player;
    for (member_citerator citr = m_memberSlots.begin(); citr != m_memberSlots.end(); ++citr)
    {
        player = sObjectMgr.GetPlayer(citr->guid, true);
        if (!player || !player->GetSession() || player->GetGroup() != this)
            continue;

        // client would crash if this is sent while he's at aloading screen
        if (!player->IsInWorld())
        {
            player->ScheduleDelayedOperation(DELAYED_GROUP_RESTORE);
            continue;
        }

        // guess size
        WorldPacket data(SMSG_GROUP_LIST, (1 + 1 + 1 + 1 + 8 + 4 + GetMembersCount() * 20));
        data << (uint8)m_groupType;                         // group type
        data << (uint8)(isBGGroup() ? 1 : 0);               // 2.0.x, isBattlegroundGroup?
        data << (uint8)(citr->group);                       // groupid
        data << (uint8)(GetFlags(*citr));                   // group flags
        data << uint64(0x50000000FFFFFFFELL);               // related to voice chat?
        data << uint32(GetMembersCount() - 1);
        for (member_citerator citr2 = m_memberSlots.begin(); citr2 != m_memberSlots.end(); ++citr2)
        {
            if (citr->guid == citr2->guid)
                continue;

            Player* member = sObjectMgr.GetPlayer(citr2->guid, true);
            uint8 onlineState = (member) ? MEMBER_STATUS_ONLINE : MEMBER_STATUS_OFFLINE;
            onlineState = onlineState | ((isBGGroup()) ? MEMBER_STATUS_PVP : 0);

            data << citr2->name;
            data << (uint64)citr2->guid;
            // online-state
            data << (uint8)(onlineState);
            data << (uint8)(citr2->group);                  // groupid
            data << (uint8)(GetFlags(*citr2));              // group flags
        }

        data << uint64(m_leaderGuid);                       // leader guid
        if (GetMembersCount() - 1)
        {
            data << uint8(m_lootMethod);                    // loot method

            if (m_lootMethod == MASTER_LOOT)
                data << uint64(m_masterLooterGuid);         // master looter guid
            else
                data << uint64(0);

            data << uint8(m_lootThreshold);                 // loot threshold
            data << uint8(m_difficulty);                    // Heroic Mod Group
        }
        player->GetSession()->SendPacket(&data);
    }
}

void Group::UpdatePlayerOutOfRange(Player* player)
{
    if (!player || !player->IsInWorld())
        return;

    if (player->GetGroupUpdateFlag() == GROUP_UPDATE_FLAG_NONE)
        return;

    WorldPacket data;
    player->GetSession()->BuildPartyMemberStatsChangedPacket(player, &data);

    Player* member;
    for (GroupReference* itr = GetFirstMember(); itr != NULL; itr = itr->next())
    {
        member = itr->GetSource();
        if (member && member != player && (!member->IsInMap(player) || !member->IsWithinDist(player, member->GetSightRange(), false)))
            member->GetSession()->SendPacket(&data);
    }
}

void Group::BroadcastPacket(WorldPacket* packet, bool ignorePlayersInBGRaid, int group, uint64 ignore)
{
    for (GroupReference* itr = GetFirstMember(); itr != NULL; itr = itr->next())
    {
        Player* pl = itr->GetSource();
        if (!pl || (ignore != 0 && pl->GetGUID() == ignore) || (ignorePlayersInBGRaid && pl->GetGroup() != this))
            continue;

        if (pl->GetSession() && (group == -1 || itr->getSubGroup() == group))
            pl->GetSession()->SendPacket(packet);
    }
}

void Group::BroadcastReadyCheck(WorldPacket* packet)
{
    for (GroupReference* itr = GetFirstMember(); itr != NULL; itr = itr->next())
    {
        Player* pl = itr->GetSource();
        if (pl && pl->GetSession())
            if (IsLeader(pl->GetGUID()) || IsAssistant(pl->GetGUID()))
                pl->GetSession()->SendPacket(packet);
    }
}

void Group::OfflineReadyCheck()
{
    for (member_citerator citr = m_memberSlots.begin(); citr != m_memberSlots.end(); ++citr)
    {
        Player* pl = sObjectMgr.GetPlayer(citr->guid, true);
        if (!pl || !pl->GetSession())
        {
            WorldPacket data(MSG_RAID_READY_CHECK_CONFIRM, 9);
            data << uint64(citr->guid);
            data << uint8(0);
            BroadcastReadyCheck(&data);
        }
    }
}

bool Group::_addMember(const uint64& guid, const char* name, bool isAssistant)
{
    // get first not-full group
    uint8 groupid = 0;
    if (m_subGroupsCounts)
    {
        bool groupFound = false;
        for (; groupid < MAXRAIDSIZE / MAXGROUPSIZE; ++groupid)
        {
            if (m_subGroupsCounts[groupid] < MAXGROUPSIZE)
            {
                groupFound = true;
                break;
            }
        }
        // We are raid group and no one slot is free
        if (!groupFound)
            return false;
    }

    return _addMember(guid, name, isAssistant, groupid);
}

bool Group::_addMember(const uint64& guid, const char* name, bool isAssistant, uint8 group)
{
    if (IsFull())
        return false;

    if (!guid)
        return false;

    Player* player = sObjectMgr.GetPlayer(guid, true);

    MemberSlot member;
    member.guid      = guid;
    member.name      = name;
    member.group     = group;
    member.assistant = isAssistant;
    m_memberSlots.push_back(member);

    SubGroupCounterIncrease(group);

    if (player)
    {
        player->SetGroupInvite(NULL);
        // if player is in group and he is being added to BG raid group, then call SetBattlegroundRaid()
        if (player->GetGroup() && isBGGroup())
            player->SetBattlegroundRaid(this, group);
        // if player is in bg raid and we are adding him to normal group, then call SetOriginalGroup()
        else if (player->GetGroup())
            player->SetOriginalGroup(this, group);
        // if player is not in group, then call set group
        else
            player->SetGroup(this, group);

        if (player->IsInWorld())
        {
            // if the same group invites the player back, cancel the homebind timer
            InstanceGroupBind* bind = GetBoundInstance(player);
            if (bind && bind->save->GetInstanceId() == player->GetInstanceId())
                player->m_InstanceValid = true;
        }
    }

    if (!isRaidGroup())                                      // reset targetIcons for non-raid-groups
    {
        for (uint8 i = 0; i < TARGETICONCOUNT; ++i)
            m_targetIcons[i] = 0;
    }

    if (!isBGGroup())
    {
        // insert into group table
        CharacterDatabase.PExecute("INSERT INTO group_member(leaderGuid,memberGuid,assistant,subgroup) VALUES('%u','%u','%u','%u')", GUID_LOPART(m_leaderGuid), GUID_LOPART(member.guid), ((member.assistant == 1) ? 1 : 0), member.group);
    }

    // ONCREATEGROUP HOOK

    return true;
}

bool Group::_removeMember(const uint64& guid)
{
    Player* player = sObjectMgr.GetPlayer(guid, true);
    if (player)
    {
        // if we are removing player from battleground raid
        if (isBGGroup())
            player->RemoveFromBattlegroundRaid();
        else
        {
            // we can remove player who is in battleground from his original group
            if (player->GetOriginalGroup() == this)
                player->SetOriginalGroup(NULL);
            else
                player->SetGroup(NULL);
        }
    }

    _removeRolls(guid);

    member_witerator slot = _getMemberWSlot(guid);
    if (slot != m_memberSlots.end())
    {
        SubGroupCounterDecrease(slot->group);

        m_memberSlots.erase(slot);
    }

    if (!isBGGroup())
        CharacterDatabase.PExecute("DELETE FROM group_member WHERE memberGuid='%u'", GUID_LOPART(guid));

    if (m_leaderGuid == guid)                                // leader was removed
    {
        if (GetMembersCount() > 0)
            _setLeader(m_memberSlots.front().guid);
        return true;
    }

    return false;
}

void Group::_setLeader(const uint64& guid)
{
    member_citerator slot = _getMemberCSlot(guid);
    if (slot == m_memberSlots.end())
        return;

    if (!isBGGroup())
    {
        // @todo set a time limit to have this function run rarely cause it can be slow
        CharacterDatabase.BeginTransaction();

        // update the group's bound instances when changing leaders

        // remove all permanent binds from the group
        // in the DB also remove solo binds that will be replaced with permbinds
        // from the new leader
        CharacterDatabase.PExecute(
            "DELETE FROM group_instance WHERE leaderguid='%u' AND (permanent = 1 OR "
            "instance IN (SELECT instance FROM character_instance WHERE guid = '%u')"
            ")", GUID_LOPART(m_leaderGuid), GUID_LOPART(slot->guid)
        );

        Player* player = sObjectMgr.GetPlayer(slot->guid, true);
        if (player)
        {
            for (uint8 i = 0; i < TOTAL_DIFFICULTIES; i++)
            {
                for (BoundInstancesMap::iterator itr = m_boundInstances[i].begin(); itr != m_boundInstances[i].end();)
                {
                    if (itr->second.perm)
                    {
                        itr->second.save->RemoveGroup(this);
                        m_boundInstances[i].erase(itr++);
                    }
                    else
                        ++itr;
                }
            }
        }

        // update the group's solo binds to the new leader
        CharacterDatabase.PExecute("UPDATE group_instance SET leaderGuid='%u' WHERE leaderGuid = '%u'", GUID_LOPART(slot->guid), GUID_LOPART(m_leaderGuid));

        // copy the permanent binds from the new leader to the group
        // overwriting the solo binds with permanent ones if necessary
        // in the DB those have been deleted already
        Player::ConvertInstancesToGroup(player, this, slot->guid);

        // update the group leader
        CharacterDatabase.PExecute("UPDATE groups SET leaderGuid='%u' WHERE leaderGuid='%u'", GUID_LOPART(slot->guid), GUID_LOPART(m_leaderGuid));
        CharacterDatabase.PExecute("UPDATE group_member SET leaderGuid='%u' WHERE leaderGuid='%u'", GUID_LOPART(slot->guid), GUID_LOPART(m_leaderGuid));
        CharacterDatabase.CommitTransaction();
    }

    m_leaderGuid = slot->guid;
    m_leaderName = slot->name;
}

void Group::_removeRolls(const uint64& guid)
{
    for (Rolls::iterator it = RollId.begin(); it < RollId.end(); ++it)
    {
        Roll* roll = *it;
        Roll::PlayerVote::iterator itr2 = roll->playerVote.find(guid);
        if (itr2 == roll->playerVote.end())
            continue;

        if (itr2->second == ROLL_GREED)
            --roll->totalGreed;
        if (itr2->second == ROLL_NEED)
            --roll->totalNeed;
        if (itr2->second == ROLL_PASS)
            --roll->totalPass;
        if (itr2->second != ROLL_NOT_VALID)
            --roll->totalPlayersRolling;

        roll->playerVote.erase(itr2);

        CountRollVote(guid, roll->itemGUID, GetMembersCount() - 1, 3);
    }
}

bool Group::_setMembersGroup(const uint64& guid, const uint8& group)
{
    member_witerator slot = _getMemberWSlot(guid);
    if (slot == m_memberSlots.end())
        return false;

    slot->group = group;

    SubGroupCounterIncrease(group);

    if (!isBGGroup()) CharacterDatabase.PExecute("UPDATE group_member SET subgroup='%u' WHERE memberGuid='%u'", group, GUID_LOPART(guid));

    return true;
}

bool Group::_setAssistantFlag(const uint64& guid, const bool& state)
{
    member_witerator slot = _getMemberWSlot(guid);
    if (slot == m_memberSlots.end())
        return false;

    slot->assistant = state;
    if (!isBGGroup())
        CharacterDatabase.PExecute("UPDATE group_member SET assistant='%u' WHERE memberGuid='%u'", (state == true) ? 1 : 0, GUID_LOPART(guid));
    return true;
}

bool Group::_setMainTank(const uint64& guid)
{
    member_citerator slot = _getMemberCSlot(guid);
    if (slot == m_memberSlots.end())
        return false;

    if (m_mainAssistant == guid)
        _setMainAssistant(0);
    m_mainTank = guid;
    if (!isBGGroup())
        CharacterDatabase.PExecute("UPDATE groups SET mainTank='%u' WHERE leaderGuid='%u'", GUID_LOPART(m_mainTank), GUID_LOPART(m_leaderGuid));
    return true;
}

bool Group::_setMainAssistant(const uint64& guid)
{
    member_witerator slot = _getMemberWSlot(guid);
    if (slot == m_memberSlots.end())
        return false;

    if (m_mainTank == guid)
        _setMainTank(0);
    m_mainAssistant = guid;
    if (!isBGGroup())
        CharacterDatabase.PExecute("UPDATE groups SET mainAssistant='%u' WHERE leaderGuid='%u'", GUID_LOPART(m_mainAssistant), GUID_LOPART(m_leaderGuid));
    return true;
}

bool Group::SameSubGroup(Player const* member1, Player const* member2) const
{
    if (!member1 || !member2) return false;
    if (member1->GetGroup() != this || member2->GetGroup() != this) return false;
    else return member1->GetSubGroup() == member2->GetSubGroup();
}

// allows setting subgroup for offline members
void Group::ChangeMembersGroup(const uint64& guid, const uint8& group)
{
    if (!isRaidGroup())
        return;
    Player* player = sObjectMgr.GetPlayer(guid, true);

    if (player)
    {
        uint8 prevSubGroup = player->GetSubGroup();
        if (player->GetGroup() == this)
            player->GetGroupRef().setSubGroup(group);
        // if player is in BG raid, it is possible that he is also in normal raid - and that normal raid is stored in m_originalGroup reference
        else
        {
            prevSubGroup = player->GetOriginalSubGroup();
            player->GetOriginalGroupRef().setSubGroup(group);
        }

        SubGroupCounterDecrease(prevSubGroup);

        if (_setMembersGroup(guid, group))
            SendUpdate();
    }
    else
        // This methods handles itself groupcounter decrease
        ChangeMembersGroup(player, group);
}

// only for online members
void Group::ChangeMembersGroup(Player* player, const uint8& group)
{
    if (!player || !isRaidGroup())
        return;

    if (_setMembersGroup(player->GetGUID(), group))
    {
        uint8 prevSubGroup;
        prevSubGroup = player->GetSubGroup();

        SubGroupCounterDecrease(prevSubGroup);

        player->GetGroupRef().setSubGroup(group);
        SendUpdate();
    }
}

void Group::UpdateLooterGuid(WorldObject* object, bool ifneed)
{
    // round robin style looting applies for all low
    // quality items in each loot method except free for all
    if (GetLootMethod() == FREE_FOR_ALL)
        return;

    uint64 oldLooterGUID = GetLooterGuid();
    member_citerator guid_itr = _getMemberCSlot(oldLooterGUID);
    if (guid_itr != m_memberSlots.end())
    {
        if (ifneed)
        {
            // not update if only update if need and ok
            Player* looter = ObjectAccessor::FindPlayer(guid_itr->guid);
            if (looter && looter->IsWithinDistInMap(object, sWorld.getConfig(CONFIG_GROUP_XP_DISTANCE), false))
                return;
        }
        ++guid_itr;
    }

    // search next after current
    Player* pNewLooter = NULL;
    for (member_citerator itr = guid_itr; itr != m_memberSlots.end(); ++itr)
    {
        if (Player* player = ObjectAccessor::FindPlayer(itr->guid))
            if (player->IsWithinDistInMap(object, sWorld.getConfig(CONFIG_GROUP_XP_DISTANCE), false))
            {
                pNewLooter = player;
                break;
            }
    }

    if (!pNewLooter)
    {
        // search from start
        for (member_citerator itr = m_memberSlots.begin(); itr != guid_itr; ++itr)
        {
            if (Player* player = ObjectAccessor::FindPlayer(itr->guid))
                if (player->IsWithinDistInMap(object, sWorld.getConfig(CONFIG_GROUP_XP_DISTANCE), false))
                {
                    pNewLooter = player;
                    break;
                }
        }
    }

    if (pNewLooter)
    {
        if (oldLooterGUID != pNewLooter->GetGUID())
        {
            SetLooterGuid(pNewLooter->GetGUID());
            SendUpdate();
        }
    }
    else
    {
        SetLooterGuid(0);
        SendUpdate();
    }
}

uint32 Group::CanJoinBattlegroundQueue(uint32 bgTypeId, uint32 bgQueueType, uint32 MinPlayerCount, uint32 MaxPlayerCount, bool isRated, uint32 arenaSlot)
{
    // check for min / max count
    uint32 memberscount = GetMembersCount();
    if (memberscount < MinPlayerCount)
        return BG_JOIN_ERR_GROUP_NOT_ENOUGH;
    if (memberscount > MaxPlayerCount)
        return BG_JOIN_ERR_GROUP_TOO_MANY;

    // get a player as reference, to compare other players' stats to (arena team id, queue id based on level, etc.)
    Player* reference = GetFirstMember()->GetSource();
    // no reference found, can't join this way
    if (!reference)
        return BG_JOIN_ERR_OFFLINE_MEMBER;

    uint32 bgQueueId = reference->GetBattlegroundQueueIdFromLevel();
    uint32 arenaTeamId = reference->GetArenaTeamId(arenaSlot);
    uint32 team = reference->GetTeam();

    // check every member of the group to be able to join
    for (GroupReference* itr = GetFirstMember(); itr != NULL; itr = itr->next())
    {
        Player* member = itr->GetSource();
        // offline member? don't let join
        if (!member)
            return BG_JOIN_ERR_OFFLINE_MEMBER;
        // don't allow cross-faction join as group
        if (member->GetTeam() != team)
            return BG_JOIN_ERR_MIXED_FACTION;
        // not in the same battleground level braket, don't let join
        if (member->GetBattlegroundQueueIdFromLevel() != bgQueueId)
            return BG_JOIN_ERR_MIXED_LEVELS;
        // don't let join rated matches if the arena team id doesn't match
        if (isRated && member->GetArenaTeamId(arenaSlot) != arenaTeamId)
            return BG_JOIN_ERR_MIXED_ARENATEAM;
        // don't let join if someone from the group is already in that bg queue
        if (member->InBattlegroundQueueForBattlegroundQueueType(bgQueueType))
            return BG_JOIN_ERR_GROUP_MEMBER_ALREADY_IN_QUEUE;
        // check for deserter debuff in case not arena queue
        if (bgTypeId != BATTLEGROUND_AA && !member->CanJoinToBattleground())
            return BG_JOIN_ERR_GROUP_DESERTER;
        // check if member can join any more battleground queues
        if (!member->HasFreeBattlegroundQueueId())
            return BG_JOIN_ERR_ALL_QUEUES_USED;
    }
    return BG_JOIN_ERR_OK;
}

//===================================================
//============== Roll ===============================
//===================================================

void Roll::targetObjectBuildLink()
{
    // called from link()
    getTarget()->addLootValidatorRef(this);
}

void Group::SetDifficulty(DungeonDifficulty difficulty)
{
    m_difficulty = difficulty;
    if (!isBGGroup()) CharacterDatabase.PExecute("UPDATE groups SET difficulty = %u WHERE leaderGuid ='%u'", m_difficulty, GUID_LOPART(m_leaderGuid));

    for (GroupReference* itr = GetFirstMember(); itr != NULL; itr = itr->next())
    {
        Player* player = itr->GetSource();
        if (!player->GetSession() || player->getLevel() < LEVELREQUIREMENT_HEROIC)
            continue;
        player->SetDifficulty(difficulty);
        player->SendDungeonDifficulty(true);
    }
}

bool Group::InCombatToInstance(uint32 instanceId)
{
    for (GroupReference* itr = GetFirstMember(); itr != NULL; itr = itr->next())
    {
        Player* pPlayer = itr->GetSource();
        if (pPlayer && pPlayer->getAttackers().size() && pPlayer->GetInstanceId() == instanceId && (pPlayer->GetMap()->IsRaid() || pPlayer->GetMap()->IsHeroic()))
            for (std::set<Unit*>::const_iterator i = pPlayer->getAttackers().begin(); i != pPlayer->getAttackers().end(); ++i)
                if ((*i) && (*i)->GetTypeId() == TYPEID_UNIT && (*i)->ToCreature()->GetCreatureTemplate()->flags_extra & CREATURE_FLAG_EXTRA_INSTANCE_BIND)
                    return true;
    }
    return false;
}

void Group::ResetInstances(uint8 method, Player* SendMsgTo)
{
    if (isBGGroup())
        return;

    // method can be INSTANCE_RESET_ALL, INSTANCE_RESET_CHANGE_DIFFICULTY, INSTANCE_RESET_GROUP_DISBAND

    // we assume that when the difficulty changes, all instances that can be reset will be
    uint8 diff = GetDifficulty();

    for (BoundInstancesMap::iterator itr = m_boundInstances[diff].begin(); itr != m_boundInstances[diff].end();)
    {
        InstanceSave* p = itr->second.save;
        const MapEntry* entry = sMapStore.LookupEntry(itr->first);
        if (!entry || (!p->CanReset() && method != INSTANCE_RESET_GROUP_DISBAND))
        {
            ++itr;
            continue;
        }

        if (method == INSTANCE_RESET_ALL)
        {
            // the "reset all instances" method can only reset normal maps
            if (diff == DIFFICULTY_HEROIC || entry->IsRaid())
            {
                ++itr;
                continue;
            }
        }

        bool isEmpty = true;
        // if the map is loaded, reset it
        Map* map = MapManager::Instance().FindMap(p->GetMapId(), p->GetInstanceId());
        if (map && map->IsDungeon())
        {
            if (p->CanReset())
                isEmpty = ((InstanceMap*)map)->Reset(method);
            else
                isEmpty = !map->HavePlayers();
        }

        if (SendMsgTo)
        {
            if (isEmpty) SendMsgTo->SendResetInstanceSuccess(p->GetMapId());
            else SendMsgTo->SendResetInstanceFailed(0, p->GetMapId());
        }

        if (isEmpty || method == INSTANCE_RESET_GROUP_DISBAND || method == INSTANCE_RESET_CHANGE_DIFFICULTY)
        {
            // do not reset the instance, just unbind if others are permanently bound to it
            if (p->CanReset()) p->DeleteFromDB();
            else CharacterDatabase.PExecute("DELETE FROM group_instance WHERE instance = '%u'", p->GetInstanceId());
            // i don't know for sure if hash_map iterators
            m_boundInstances[diff].erase(itr);
            itr = m_boundInstances[diff].begin();
            // this unloads the instance save unless online players are bound to it
            // (eg. permanent binds or GM solo binds)
            p->RemoveGroup(this);
        }
        else
            ++itr;
    }
}

InstanceGroupBind* Group::GetBoundInstance(Player* player)
{
    uint32 mapid = player->GetMapId();
    MapEntry const* mapEntry = sMapStore.LookupEntry(mapid);
    return GetBoundInstance(mapEntry);
}

InstanceGroupBind* Group::GetBoundInstance(Map* aMap)
{
    // Currently spawn numbering not different from map difficulty
    DungeonDifficulty difficulty = GetDifficulty();
    return GetBoundInstance(difficulty, aMap->GetId());
}

InstanceGroupBind* Group::GetBoundInstance(MapEntry const* mapEntry)
{
    if (!mapEntry || !mapEntry->IsDungeon())
        return NULL;

    DungeonDifficulty difficulty = GetDifficulty();
    return GetBoundInstance(difficulty, mapEntry->MapID);
}

InstanceGroupBind* Group::GetBoundInstance(DungeonDifficulty difficulty, uint32 mapId)
{
    BoundInstancesMap::iterator itr = m_boundInstances[difficulty].find(mapId);
    if (itr != m_boundInstances[difficulty].end())
        return &itr->second;
    else
        return NULL;
}

Group::BoundInstancesMap& Group::GetBoundInstances(DungeonDifficulty difficulty)
{
    return m_boundInstances[difficulty];
}

InstanceGroupBind* Group::BindToInstance(InstanceSave* save, bool permanent, bool load)
{
    if (save && !isBGGroup())
    {
        InstanceGroupBind& bind = m_boundInstances[save->GetDifficulty()][save->GetMapId()];
        if (bind.save)
        {
            // when a boss is killed or when copying the player's binds to the group
            if (permanent != bind.perm || save != bind.save)
                if (!load) CharacterDatabase.PExecute("UPDATE group_instance SET instance = '%u', permanent = '%u' WHERE leaderGuid = '%u' AND instance = '%u'", save->GetInstanceId(), permanent, GUID_LOPART(GetLeaderGUID()), bind.save->GetInstanceId());
        }
        else if (!load) CharacterDatabase.PExecute("INSERT INTO group_instance (leaderGuid, instance, permanent) VALUES ('%u', '%u', '%u')", GUID_LOPART(GetLeaderGUID()), save->GetInstanceId(), permanent);

        if (bind.save != save)
        {
            if (bind.save) bind.save->RemoveGroup(this);
            save->AddGroup(this);
        }

        bind.save = save;
        bind.perm = permanent;
        if (!load) sLog.outDebug("Group::BindToInstance: %d is now bound to map %d, instance %d, difficulty %d", GUID_LOPART(GetLeaderGUID()), save->GetMapId(), save->GetInstanceId(), save->GetDifficulty());
        return &bind;
    }
    else
        return NULL;
}

void Group::UnbindInstance(uint32 mapid, uint8 difficulty, bool unload)
{
    BoundInstancesMap::iterator itr = m_boundInstances[difficulty].find(mapid);
    if (itr != m_boundInstances[difficulty].end())
    {
        if (!unload)
            CharacterDatabase.PExecute("DELETE FROM group_instance WHERE leaderGuid = '%u' AND instance = '%u'", GUID_LOPART(GetLeaderGUID()), itr->second.save->GetInstanceId());
        itr->second.save->RemoveGroup(this);                // save can become invalid
        m_boundInstances[difficulty].erase(itr);
    }
}

void Group::_homebindIfInstance(Player* player)
{
    if (player && !player->IsGameMaster() && sMapStore.LookupEntry(player->GetMapId())->IsDungeon())
    {
        // leaving the group in an instance, the homebind timer is started
        // unless the player is permanently saved to the instance
        InstanceSave* save = sInstanceSaveMgr.GetInstanceSave(player->GetInstanceId());
        InstancePlayerBind* playerBind = save ? player->GetBoundInstance(save->GetMapId(), save->GetDifficulty()) : NULL;
        if (!playerBind || !playerBind->perm)
            player->m_InstanceValid = false;
    }
}

void Group::BroadcastGroupUpdate(void)
{
    // FG: HACK: force flags update on group leave - for values update hack
    // -- not very efficient but safe
    for (member_citerator citr = m_memberSlots.begin(); citr != m_memberSlots.end(); ++citr)
    {
        if (Player* pp = sObjectMgr.GetPlayer(citr->guid))
        {
            pp->ForceValuesUpdateAtIndex(UNIT_FIELD_HEALTH);
            pp->ForceValuesUpdateAtIndex(UNIT_FIELD_MAXHEALTH);
            pp->ForceValuesUpdateAtIndex(UNIT_FIELD_BYTES_2);
            pp->ForceValuesUpdateAtIndex(UNIT_FIELD_FACTIONTEMPLATE);

            if (Pet* pet = pp->GetPet())
            {
                pet->ForceValuesUpdateAtIndex(UNIT_FIELD_HEALTH);
                pet->ForceValuesUpdateAtIndex(UNIT_FIELD_MAXHEALTH);
                pet->ForceValuesUpdateAtIndex(UNIT_FIELD_BYTES_2);
                pet->ForceValuesUpdateAtIndex(UNIT_FIELD_FACTIONTEMPLATE);
            }

            DEBUG_LOG("-- Forced group value update for '%s'", pp->GetName());
        }
    }
}

