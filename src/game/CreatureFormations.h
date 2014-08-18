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

#ifndef _FORMATIONS_H
#define _FORMATIONS_H

#include "Common.h"
#include <ace/Basic_Types.h>
#include <ace/Singleton.h>
#include <ace/Thread_Mutex.h>

class CreatureFormation;

struct FormationInfo
{
    uint32 leaderGUID;
    uint8 formationAI;
};

struct FormationData
{
    uint32 formationId;
    float follow_dist;
    float follow_angle;
};

struct Formation
{
    uint32 leaderGUID;
    float follow_dist;
    float follow_angle;
    uint8 formationAI;
};


class CreatureFormationManager
{
    friend class ACE_Singleton<CreatureFormationManager, ACE_Null_Mutex>;
    public:
        void AddCreatureToFormation(uint32 formation_id, Creature *creature);
        void RemoveCreatureFromFormation(CreatureFormation *formation, Creature *creature);
        void LoadCreatureFormations();
        void UnloadCreatureFormations();
};

#ifdef formation_mgr
#undef formation_mgr
#endif

#define formation_mgr (*ACE_Singleton<CreatureFormationManager, ACE_Null_Mutex>::instance())

typedef UNORDERED_MAP<uint32/*formationId*/, FormationInfo*>   CreatureFormationInfoType;
typedef UNORDERED_MAP<uint32 /*memberGUID*/, FormationData*>   CreatureFormationDataType;

extern CreatureFormationInfoType    CreatureFormationMap;
extern CreatureFormationDataType    CreatureFormationDataMap;

class CreatureFormation
{
    private:
        Creature *m_leader;
        typedef std::map<Creature*, Formation*>  CreatureFormationMemberType;
        CreatureFormationMemberType m_members;

        uint32 m_formationID;
        bool m_Formed;

    public:
        //Fromation cannot be created empty
        explicit CreatureFormation(uint32 id) : m_leader(NULL), m_formationID(id), m_Formed(false) {}
        ~CreatureFormation() { sLog.outDebug("Destroying formation"); }

        Creature* getLeader() const { return m_leader; }
        uint32 GetId() const { return m_formationID; }
        bool isEmpty() const { return m_members.empty(); }
        bool isFormed() const { return m_Formed; }

        void AddMember(Creature *member);
        void RemoveMember(Creature *member);
        void Reset(bool dismiss);

        void LeaderMoveTo(float x, float y, float z);
        void MemberAttackStart(Creature* member, Unit *target);
};

#endif
