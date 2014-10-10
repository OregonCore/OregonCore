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

#ifndef _GROUPS_H
#define _GROUPS_H

#include "Common.h"

class CreatureGroup;

struct GroupInfo
{
    uint32 leaderGUID;
    uint8  groupType;
};

class CreatureGroupManager
{
    public:
        void AddCreatureToGroup(uint32 group_id, Creature* creature);
        void RemoveCreatureFromGroup(CreatureGroup* group, Creature* creature);
        void LoadCreatureGroups();
};

#define group_mgr (*ACE_Singleton<CreatureGroupManager, ACE_Null_Mutex>::instance())

typedef UNORDERED_MAP<uint32/*groupId*/, GroupInfo*>   CreatureGroupInfoType;
typedef UNORDERED_MAP<uint32 /*memberGUID*/, uint32/*groupId*/>   CreatureGroupDataType;

extern CreatureGroupInfoType    CreatureGroupMap;
extern CreatureGroupDataType    CreatureGroupDataMap;

class CreatureGroup
{
    private:
        typedef std::map<Creature*, GroupInfo*>  CreatureGroupMemberType;
        CreatureGroupMemberType m_members;

        uint32 m_groupID;

    public:
        //Group cannot be created empty
        explicit CreatureGroup(uint32 id) : m_groupID(id) {}
        ~CreatureGroup()
        {
            sLog.outDebug("Destroying group");
        }

        uint32 GetId() const
        {
            return m_groupID;
        }
        bool isEmpty() const
        {
            return m_members.empty();
        }

        void AddMember(Creature* member);
        void RemoveMember(Creature* member);

        void MemberAttackStart(Creature* member, Unit* target);
        bool IsAllowedToRespawn(Creature* member);
};

#endif

