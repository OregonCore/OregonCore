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
#include "CreatureFormations.h"
#include "ObjectMgr.h"

#include "CreatureAI.h"

#define MAX_DESYNC 5.0f

CreatureFormationInfoType   CreatureFormationMap;
CreatureFormationDataType   CreatureFormationDataMap;

void CreatureFormationManager::AddCreatureToFormation(uint32 formationId, Creature *member)
{
    Map *map = member->FindMap();
    if (!map)
        return;

    CreatureFormationHolderType::iterator itr = map->CreatureFormationHolder.find(formationId);

    //Add member to an existing formation
    if (itr != map->CreatureFormationHolder.end())
    {
        sLog.outDebug("Formation found: %u, inserting creature GUID: %u, Formation InstanceID %u", formationId, member->GetGUIDLow(), member->GetInstanceId());
        itr->second->AddMember(member);
    }
    //Create new formation
    else
    {
        sLog.outDebug("Formation not found: %u. Creating new formation.", formationId);
        CreatureFormation* formation = new CreatureFormation(formationId);
        map->CreatureFormationHolder[formationId] = formation;
        formation->AddMember(member);
    }
}

void CreatureFormationManager::RemoveCreatureFromFormation(CreatureFormation *formation, Creature *member)
{
    sLog.outDebug("Deleting member pointer to GUID: %u from formation %u", formation->GetId(), member->GetDBTableGUIDLow());
    formation->RemoveMember(member);

    if (formation->isEmpty())
    {
        Map *map = member->FindMap();
        if (!map)
            return;

        sLog.outDebug("Deleting formation with InstanceID %u", member->GetInstanceId());
        map->CreatureFormationHolder.erase(formation->GetId());
        delete formation;
    }
}

void CreatureFormationManager::LoadCreatureFormations()
{
    // Clear existing map (we need to call delete)
    UnloadCreatureFormations();

    //Check Integrity of the table creature_formations
    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT MAX(formationId) FROM creature_formations");

    if (!result)
    {
        sLog.outErrorDb(" ...an error occured while loading the table creature_formationss (maybe it doesn't exist ?)\n");
        return;
    }

    //Check Integrity of the table creature_formation_data
    result = WorldDatabase.Query("SELECT MAX(formationId) FROM creature_formation_data");

    if (!result)
    {
        sLog.outErrorDb(" ...an error occured while loading the table creature_formation_data (maybe it doesn't exist ?)\n");
        return;
    }

    //Check if formations without member exist
    result = WorldDatabase.Query("SELECT COUNT(formationId) FROM creature_formations WHERE formationId NOT IN (SELECT formationId FROM creature_formation_data)");

    if (result)
    {
        sLog.outDetail(">> %u Formations without member found, formations skipped.",result->Fetch()->GetInt32());
    }

    //Check if member without formation exist
    result = WorldDatabase.Query("SELECT COUNT(formationId) FROM creature_formation_data WHERE formationId NOT IN (SELECT formationId FROM creature_formations)");

    if (result)
    {
        sLog.outDetail(">> %u Member without formation found, member skipped.",result->Fetch()->GetInt32());
    }

    //Get formations
    QueryResult_AutoPtr result_data = WorldDatabase.Query("SELECT formationId, leaderGUID, formationAI FROM creature_formations WHERE formationId IN (SELECT formationId FROM creature_formation_data) ORDER BY formationId");

    if (!result_data)
    {
        sLog.outString();
        sLog.outString(">> Loaded 0 creature formations. DB table `creature_formations` is empty.");
        return;
    }

    //Get member
    QueryResult_AutoPtr result_member = WorldDatabase.Query("SELECT formationId, memberGUID, dist, angle FROM creature_formation_data WHERE formationId IN (SELECT formationId FROM creature_formations) ORDER BY formationId");

    if (!result_member)
    {
        sLog.outString();
        sLog.outString(">> Loaded 0 formation formations. DB table `creature_formation_data` is empty.");
        return;
    }

    std::set<uint32> guidSet;

    QueryResult_AutoPtr guidResult = WorldDatabase.Query("SELECT guid FROM creature");

    if (guidResult)
    {
        do 
        {
            Field *fields = guidResult->Fetch();
            uint32 guid = fields[0].GetUInt32();

            guidSet.insert(guid);

        } while (guidResult->NextRow());
    }

    //Loading formations...
    uint64 total_formations = result_data->GetRowCount();
    uint64 total_member = result_member->GetRowCount();

    Field *fields;

    FormationInfo *formation_info;

    do
    {
        fields = result_data->Fetch();
        
        //Load formation member data
        uint32 formationId = fields[0].GetUInt32();
        uint32 leaderGUID = fields[1].GetUInt32();
        uint8  formationAI = fields[2].GetUInt8();

        formation_info                        = new FormationInfo;
        formation_info->leaderGUID            = leaderGUID;
        formation_info->formationAI           = formationAI; 

        // check data correctness
        if (guidSet.find(formation_info->leaderGUID) == guidSet.end())
        {
            sLog.outErrorDb("creature_formations table leader guid %u incorrect (not exist)", formation_info->leaderGUID);
            delete formation_info;
            continue;
        }

        CreatureFormationMap[formationId] = formation_info;

        sLog.outDebug("CreatureFormation::LoadCreatureFormations: Load Formation %u with Leader %u and formationAI %u.", formationId, leaderGUID, formationAI);

    }
    while (result_data->NextRow()) ;


    //Loading member...
    FormationData *formation_data;

    do
    {
        fields = result_member->Fetch();
        
        //Load formation member data
        uint32 formationId = fields[0].GetUInt32();
        uint32 memberGUID = fields[1].GetUInt32();
        float follow_dist = fields[2].GetFloat();
        float follow_angle = fields[3].GetFloat() * M_PI / 180;

        formation_data                        = new FormationData;
        formation_data->formationId           = formationId;
        formation_data->follow_dist           = follow_dist; 
        formation_data->follow_angle          = follow_angle; 

        // check data correctness
        if (guidSet.find(memberGUID) == guidSet.end())
        {
            sLog.outErrorDb("creature_formation_data table member guid %u incorrect (not exist)", memberGUID);
            delete formation_data;
            continue;
        }

        CreatureFormationDataMap[memberGUID] = formation_data;

        sLog.outDebug("CreatureFormation::LoadCreatureFormations: Load Member %u for Formation with formationId %u.", memberGUID, formation_data->formationId);

    }
    while (result_member->NextRow()) ;

    sLog.outString();
    sLog.outString(">> Loaded " UI64FMTD " formations", total_formations);
    sLog.outString(">> Loaded " UI64FMTD " creatures in formations", total_member);
    sLog.outString();
}

void CreatureFormation::AddMember(Creature *member)
{
    if (!member)
        return;

    uint32 memberGUID = member->GetDBTableGUIDLow();

    sLog.outDebug("CreatureFormation::AddMember: Adding unit GUID: %u to formation.", memberGUID);
    
    Formation *formation;
    formation = new Formation;

    //Check if it is a leader
    if (member->GetDBTableGUIDLow() == CreatureFormationMap.find(m_formationID)->second->leaderGUID)
    {
        sLog.outDebug("Unit GUID: %u is formation leader.", memberGUID);
        m_leader = member;
        formation->follow_dist = 0;
        formation->follow_angle = 0;
    }
    else
    {
        formation->follow_dist = CreatureFormationDataMap.find(memberGUID)->second->follow_dist;
        formation->follow_angle = CreatureFormationDataMap.find(memberGUID)->second->follow_angle;
    }
    
    formation->formationAI = CreatureFormationMap.find(m_formationID)->second->formationAI; 
    formation->leaderGUID = CreatureFormationMap.find(m_formationID)->second->leaderGUID; 

    m_members[member] = formation;
    member->SetFormation(this);
}

void CreatureFormation::RemoveMember(Creature *member)
{
    if (!member)
        return;

    if (m_leader == member)
        m_leader = NULL;
    
    m_members.erase(member);
    member->SetFormation(NULL);
}

void CreatureFormation::MemberAttackStart(Creature *member, Unit *target)
{
    uint8 formationAI = CreatureFormationMap[m_formationID]->formationAI;

    if (!formationAI)
        return;

    if (formationAI == 1 && member != m_leader)
        return;

    for (CreatureFormationMemberType::iterator itr = m_members.begin(); itr != m_members.end(); ++itr)
    {
        Creature* pCreature = itr->first;

        if (m_leader) // avoid crash if leader was killed and reset.
            sLog.outDebug("CreatureFormation::MemberAttackStart: formation instance id %u calls member instid %u", m_leader->GetInstanceId(), member->GetInstanceId());

        //Skip one check
        if (pCreature == member)
            continue;

        if (!pCreature->isAlive())
            continue;

        if (pCreature->getVictim())
            continue;

        if (pCreature->IsAIEnabled)
            if (pCreature->canAttack(target))
                pCreature->AI()->AttackStart(target);
    }
}

void CreatureFormation::Reset(bool dismiss)
{
    for (CreatureFormationMemberType::iterator itr = m_members.begin(); itr != m_members.end(); ++itr)
    {
        Creature* pCreature = itr->first;

        if (pCreature != m_leader && pCreature->isAlive())
        {
            if (dismiss)
                pCreature->GetMotionMaster()->Initialize();
            else
                pCreature->GetMotionMaster()->MoveIdle(MOTION_SLOT_IDLE);
            sLog.outDebug("Set %s movement for member GUID: %u", dismiss ? "default" : "idle", pCreature->GetGUIDLow());
        }
    }
    m_Formed = !dismiss;
}

void CreatureFormation::LeaderMoveTo(float x, float y, float z)
{
    if (!m_leader)
        return;

    float pathangle = atan2(m_leader->GetPositionY() - y, m_leader->GetPositionX() - x);

    for (CreatureFormationMemberType::iterator itr = m_members.begin(); itr != m_members.end(); ++itr)
    {
        Creature *pCreature = itr->first;
        if (pCreature == m_leader || !pCreature->isAlive() || pCreature->getVictim())
            continue;

        float angle = itr->second->follow_angle;
        float dist = itr->second->follow_dist;

        float dx = x + cos(angle + pathangle) * dist;
        float dy = y + sin(angle + pathangle) * dist;
        float dz = z;

        Oregon::NormalizeMapCoord(dx);
        Oregon::NormalizeMapCoord(dy);

        pCreature->UpdateGroundPositionZ(dx, dy, dz);

        if (pCreature->IsWithinDist(m_leader, dist + MAX_DESYNC))
            pCreature->SetUnitMovementFlags(m_leader->GetUnitMovementFlags());
        else
            pCreature->RemoveUnitMovementFlag(MOVEFLAG_WALK_MODE);

        pCreature->GetMotionMaster()->MovePoint(0, dx, dy, dz);
        pCreature->SetHomePosition(dx, dy, dz, pathangle);
    }
}

void CreatureFormationManager::UnloadCreatureFormations()
{
    for (CreatureFormationInfoType::iterator it = CreatureFormationMap.begin(); it != CreatureFormationMap.end(); )
    {
        delete it->second;
        it = CreatureFormationMap.erase(it);
    }

    for (CreatureFormationDataType::iterator it = CreatureFormationDataMap.begin(); it != CreatureFormationDataMap.end(); )
    {
        delete it->second;
        it = CreatureFormationDataMap.erase(it);
    }
}

