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

#include "Creature.h"
#include "CreatureGroups.h"
#include "ObjectMgr.h"
#include "Policies/SingletonImp.h"
#include "CreatureAI.h"

#define MAX_DESYNC 5.0f

INSTANTIATE_SINGLETON_1(CreatureGroupManager);

CreatureGroupInfoType   CreatureGroupMap;
CreatureGroupDataType   CreatureGroupDataMap;

void CreatureGroupManager::AddCreatureToGroup(uint32 groupId, Creature* member)
{
    Map* map = member->FindMap();
    if (!map)
        return;

    CreatureGroupHolderType::iterator itr = map->CreatureGroupHolder.find(groupId);

    //Add member to an existing group
    if (itr != map->CreatureGroupHolder.end())
    {
        sLog.outDebug("Group found: %u, inserting creature GUID: %u, Group InstanceID %u", groupId, member->GetGUIDLow(), member->GetInstanceId());
        itr->second->AddMember(member);
    }
    //Create new group
    else
    {
        sLog.outDebug("Group not found: %u. Creating new group.", groupId);
        CreatureGroup* group = new CreatureGroup(groupId);
        map->CreatureGroupHolder[groupId] = group;
        group->AddMember(member);
    }
}

void CreatureGroupManager::RemoveCreatureFromGroup(CreatureGroup* group, Creature* member)
{
    sLog.outDebug("Deleting member pointer to GUID: %u from group %u", group->GetId(), member->GetDBTableGUIDLow());
    group->RemoveMember(member);

    if (group->isEmpty())
    {
        Map* map = member->FindMap();
        if (!map)
            return;

        sLog.outDebug("Deleting group with InstanceID %u", member->GetInstanceId());
        map->CreatureGroupHolder.erase(group->GetId());
        delete group;
    }
}

void CreatureGroupManager::LoadCreatureGroups()
{
    //Clear existing map
    for (CreatureGroupInfoType::iterator itr = CreatureGroupMap.begin(); itr != CreatureGroupMap.end(); ++itr)
        delete itr->second;
    CreatureGroupMap.clear();
    CreatureGroupDataMap.clear();

    //Check Integrity of the table
    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT MAX(groupId) FROM creature_groups");

    if (!result)
    {
        sLog.outErrorDb(" ...an error occured while loading the table creature_groups (maybe it doesn't exist ?)\n");
        return;
    }

    //Get group data
    result = WorldDatabase.Query("SELECT MAX(groupId) FROM creature_group_data");

    if (!result)
    {
        sLog.outErrorDb(" ...an error occured while loading the table creature_group_data (maybe it doesn't exist ?)\n");
        return;
    }

    //Check if groups without member exist
    result = WorldDatabase.Query("SELECT COUNT(groupId) FROM creature_groups WHERE groupId NOT IN (SELECT groupId FROM creature_group_data)");

    if (result)
        sLog.outDetail(">> %u Groups without member found, groups skipped.", result->Fetch()->GetInt32());

    //Check if member without group exist
    result = WorldDatabase.Query("SELECT COUNT(groupId) FROM creature_group_data WHERE groupId NOT IN (SELECT groupId FROM creature_groups)");

    if (result)
        sLog.outDetail(">> %u Member without group found, member skipped.", result->Fetch()->GetInt32());

    //Get groups
    QueryResult_AutoPtr result_data = WorldDatabase.Query("SELECT groupId, leaderGUID, groupType FROM creature_groups WHERE groupId IN (SELECT groupId FROM creature_group_data) ORDER BY groupId");

    if (!result_data)
    {
        sLog.outString();
        sLog.outString(">> Loaded 0 creature groups. DB table `creature_groups` is empty.");
        return;
    }

    //Get member
    QueryResult_AutoPtr result_member = WorldDatabase.Query("SELECT groupId, memberGUID FROM creature_group_data WHERE groupId IN (SELECT groupId FROM creature_groups) ORDER BY groupId");

    if (!result_member)
    {
        sLog.outString();
        sLog.outString(">> Loaded 0 creature groups. DB table `creature_group_data` is empty.");
        return;
    }

    //Loading groups...
    uint64 total_groups = result_data->GetRowCount();
    uint64 total_member = result_member->GetRowCount();

    Field* fields;
    std::set<uint32> guidSet;

    GroupInfo* group_member;
    do
    {
        fields = result_data->Fetch();

        //Load group member data
        uint32 groupId = fields[0].GetUInt32();
        uint32 leaderGUID = fields[1].GetUInt32();
        uint8  groupType = fields[2].GetUInt8();

        group_member                        = new GroupInfo;
        group_member->leaderGUID            = leaderGUID;
        group_member->groupType             = groupType;

        // check data correctness
        if (guidSet.find(group_member->leaderGUID) == guidSet.end())
        {
            sLog.outErrorDb("creature_groups table leader guid %u incorrect (not exist)", group_member->leaderGUID);
            delete group_member;
            return;
        }

        CreatureGroupMap[groupId] = group_member;

        sLog.outDebug("CreatureGroup::LoadCreatureGroups: Load Group %u with Leader %u and groupType %u.", groupId, leaderGUID, groupType);

    }
    while (result_data->NextRow()) ;

    //Loading member...
    do
    {
        fields = result_member->Fetch();

        //Load group member data
        uint32 groupId = fields[0].GetUInt32();
        uint32 memberGUID = fields[1].GetUInt32();

        // check data correctness
        if (guidSet.find(memberGUID) == guidSet.end())
        {
            sLog.outErrorDb("creature_group_data table member guid %u incorrect (not exist)", memberGUID);
            delete group_member;
            continue;
        }

        CreatureGroupDataMap[memberGUID] = groupId;

        sLog.outDebug("CreatureGroup::LoadCreatureGroups: Load Member %u for Group with groupId %u.", memberGUID, groupId);

    }
    while (result_member->NextRow()) ;

    sLog.outString();
    sLog.outString(">> Loaded " UI64FMTD " groups", total_groups);
    sLog.outString(">> Loaded " UI64FMTD " creatures in groups", total_member);
    sLog.outString();
}

void CreatureGroup::AddMember(Creature* member)
{
    if (!member)
        return;

    sLog.outDebug("CreatureGroup::AddMember: Adding unit GUID: %u to group.", member->GetGUIDLow());

    m_members[member] = CreatureGroupMap.find(m_groupID)->second;
    member->SetGroup(this);
}

void CreatureGroup::RemoveMember(Creature* member)
{
    if (!member)
        return;

    m_members.erase(member);
    member->SetGroup(NULL);
}

void CreatureGroup::MemberAttackStart(Creature* member, Unit* target)
{
    for (CreatureGroupMemberType::iterator itr = m_members.begin(); itr != m_members.end(); ++itr)
    {
        Creature* pCreature = itr->first;

        sLog.outDebug("CreatureGroup::MemberAttackStart: group member instanceId %u .", member->GetInstanceId());

        //Skip one check
        if (pCreature == member)
            continue;

        if (!pCreature->isAlive())
            continue;

        if (pCreature->getVictim())
            continue;

        if (pCreature->canAttack(target))
            pCreature->AI()->AttackStart(target);
    }
}

bool CreatureGroup::IsAllowedToRespawn(Creature* member)
{
    uint8 groupType = CreatureGroupMap[m_groupID]->groupType;

    if (!member->GetMap()->IsRaid() || groupType == 0)
        return false;

    bool exist = false;
    //Check if at least one groupmember lives if no -> no respawn.
    for (CreatureGroupMemberType::iterator itr = m_members.begin(); itr != m_members.end(); ++itr)
        if (itr->first->isAlive())
            exist = true;

    //Check if an groupmember is in combat, if yes -> no respawn.
    for (CreatureGroupMemberType::iterator itr = m_members.begin(); itr != m_members.end(); ++itr)
        if (itr->first->isInCombat())
            exist = false;

    if (exist)
        sLog.outDebug("CreatureGroup::IsAllowedToRespawn: group member instanceId %u can respawn.", member->GetInstanceId());

    return exist;
}
