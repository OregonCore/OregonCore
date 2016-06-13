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

#ifndef _INSTANCESAVEMGR_H
#define _INSTANCESAVEMGR_H

#include "Platform/Define.h"
#include "Policies/Singleton.h"
#include "ace/Thread_Mutex.h"
#include <list>
#include <map>
#include "Utilities/UnorderedMap.h"
#include "Database/DatabaseEnv.h"

struct InstanceTemplate;
struct MapEntry;
class Player;
class Group;

/*
    Holds the information necessary for creating a new map for an existing instance
    Is referenced in three cases:
    - player-instance binds for solo players (not in group)
    - player-instance binds for permanent heroic/raid saves
    - group-instance binds (both solo and permanent) cache the player binds for the group leader
*/
class InstanceSave
{
        friend class InstanceSaveManager;
    public:
        /* Created either when:
           - any new instance is being generated
           - the first time a player bound to InstanceId logs in
           - when a group bound to the instance is loaded */
        InstanceSave(uint16 MapId, uint32 InstanceId, DungeonDifficulty difficulty, time_t resetTime, bool canReset);

        /* Unloaded when m_playerList and m_groupList become empty
           or when the instance is reset */
        ~InstanceSave();

        uint8 GetPlayerCount()
        {
            return m_playerList.size();
        }
        uint8 GetGroupCount()
        {
            return m_groupList.size();
        }

        /* A map corresponding to the InstanceId/MapId does not always exist.
        InstanceSave objects may be created on player logon but the maps are
        created and loaded only when a player actually enters the instance. */
        uint32 GetInstanceId()
        {
            return m_instanceid;
        }
        uint32 GetMapId()
        {
            return m_mapid;
        }

        /* Saved when the instance is generated for the first time */
        void SaveToDB();
        /* When the instance is being reset (permanently deleted) */
        void DeleteFromDB();

        /* for normal instances this corresponds to max(creature respawn time) + X hours
           for raid/heroic instances this caches the global respawn time for the map */
        time_t GetResetTime()
        {
            return m_resetTime;
        }
        void SetResetTime(time_t resetTime)
        {
            m_resetTime = resetTime;
        }
        time_t GetResetTimeForDB();

        InstanceTemplate const* GetTemplate();
        MapEntry const* GetMapEntry();

        /* online players bound to the instance (perm/solo)
           does not include the members of the group unless they have permanent saves */
        void AddPlayer(Player* player)
        {
            m_playerList.push_back(player);
        }
        bool RemovePlayer(Player* player)
        {
            m_playerList.remove(player);
            return UnloadIfEmpty();
        }
        /* all groups bound to the instance */
        void AddGroup(Group* group)
        {
            m_groupList.push_back(group);
        }
        bool RemoveGroup(Group* group)
        {
            m_groupList.remove(group);
            return UnloadIfEmpty();
        }

        /* instances cannot be reset (except at the global reset time)
           if there are players permanently bound to it
           this is cached for the case when those players are offline */
        bool CanReset()
        {
            return m_canReset;
        }
        void SetCanReset(bool canReset)
        {
            m_canReset = canReset;
        }

        /* currently it is possible to omit this information from this structure
           but that would depend on a lot of things that can easily change in future */
        DungeonDifficulty GetDifficulty()
        {
            return m_difficulty;
        }

        typedef std::list<Player*> PlayerListType;
        typedef std::list<Group*> GroupListType;
    private:
        bool UnloadIfEmpty();
        /* the only reason the instSave-object links are kept is because
           the object-instSave links need to be broken at reset time
           @todo maybe it's enough to just store the number of players/groups */
        uint16 m_mapid;
        uint32 m_instanceid;
        time_t m_resetTime;
        DungeonDifficulty m_difficulty;
        bool m_canReset;
        PlayerListType m_playerList;
        GroupListType m_groupList;
};

class InstanceSaveManager : public Oregon::Singleton<InstanceSaveManager, Oregon::ClassLevelLockable<InstanceSaveManager, ACE_Thread_Mutex> >
{
        friend class InstanceSave;
    public:
        InstanceSaveManager();
        ~InstanceSaveManager();

        typedef std::map<uint32 /*InstanceId*/, InstanceSave*> InstanceSaveMap;
        typedef UNORDERED_MAP<uint32 /*InstanceId*/, InstanceSave*> InstanceSaveHashMap;
        typedef std::map<uint32 /*mapId*/, InstanceSaveMap> InstanceSaveMapMap;

        /* resetTime is a global propery of each (raid/heroic) map
           all instances of that map reset at the same time */
        struct InstResetEvent
        {
            uint8 type;
            uint16 mapid;
            uint16 instanceId;
            InstResetEvent(uint8 t = 0, uint16 m = 0, uint16 i = 0) : type(t), mapid(m), instanceId(i) {}
            bool operator == (const InstResetEvent& e)
            {
                return e.instanceId == instanceId;
            }
        };
        typedef std::multimap<time_t /*resetTime*/, InstResetEvent> ResetTimeQueue;
        typedef std::vector<time_t /*resetTime*/> ResetTimeVector;

        void CleanupInstances();
        void PackInstances();

        void LoadResetTimes();
        time_t GetResetTimeFor(uint32 mapid)
        {
            return m_resetTimeByMapId[mapid];
        }
        void ScheduleReset(bool add, time_t time, InstResetEvent event);

        void Update();

        InstanceSave* AddInstanceSave(uint32 mapId, uint32 instanceId, DungeonDifficulty difficulty, time_t resetTime, bool canReset, bool load = false);
        void RemoveInstanceSave(uint32 InstanceId);
        static void DeleteInstanceFromDB(uint32 instanceid);

        InstanceSave* GetInstanceSave(uint32 InstanceId);

        /* statistics */
        uint32 GetNumInstanceSaves()
        {
            return m_instanceSaveById.size();
        }
        uint32 GetNumBoundPlayersTotal();
        uint32 GetNumBoundGroupsTotal();

    protected:
        static uint16 ResetTimeDelay[];

    private:
        void _ResetOrWarnAll(uint32 mapid, bool warn, time_t resetTime);
        void _ResetInstance(uint32 mapid, uint32 instanceId);
        void _ResetSave(InstanceSaveHashMap::iterator& itr);
        void _DelHelper(DatabaseType& db, const char* fields, const char* table, const char* queryTail, ...);
        // used during global instance resets
        bool lock_instLists;
        // fast lookup by instance id
        InstanceSaveHashMap m_instanceSaveById;
        // fast lookup for reset times
        ResetTimeVector m_resetTimeByMapId;
        ResetTimeQueue m_resetTimeQueue;
};

#define sInstanceSaveMgr Oregon::Singleton<InstanceSaveManager>::Instance()
#endif

