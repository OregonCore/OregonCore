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
#include "Database/SQLStorage.h"

#include "Player.h"
#include "GridNotifiers.h"
#include "WorldSession.h"
#include "Log.h"
#include "GridStates.h"
#include "Map.h"
#include "MapManager.h"
#include "MapInstanced.h"
#include "InstanceSaveMgr.h"
#include "Timer.h"
#include "Config/Config.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "World.h"
#include "Group.h"
#include "InstanceData.h"
#include "Policies/Singleton.h"

INSTANTIATE_SINGLETON_1(InstanceSaveManager);

InstanceSaveManager::InstanceSaveManager() : lock_instLists(false)
{
}

uint16 InstanceSaveManager::ResetTimeDelay[] = {3600, 900, 300, 60};

InstanceSaveManager::~InstanceSaveManager()
{
    // it is undefined whether this or objectmgr will be unloaded first
    // so we must be prepared for both cases
    lock_instLists = true;
    for (InstanceSaveHashMap::iterator itr = m_instanceSaveById.begin(); itr != m_instanceSaveById.end(); ++itr)
    {
        InstanceSave* save = itr->second;
        for (InstanceSave::PlayerListType::iterator itr2 = save->m_playerList.begin(), next = itr2; itr2 != save->m_playerList.end(); itr2 = next)
        {
            ++next;
            (*itr2)->UnbindInstance(save->GetMapId(), save->GetDifficulty(), true);
        }
        save->m_playerList.clear();
        for (InstanceSave::GroupListType::iterator itr2 = save->m_groupList.begin(), next = itr2; itr2 != save->m_groupList.end(); itr2 = next)
        {
            ++next;
            (*itr2)->UnbindInstance(save->GetMapId(), save->GetDifficulty(), true);
        }
        save->m_groupList.clear();
        delete save;
    }
}

/*
- adding instance into manager
- called from InstanceMap::Add, _LoadBoundInstances, LoadGroups
*/
InstanceSave* InstanceSaveManager::AddInstanceSave(uint32 mapId, uint32 instanceId, DungeonDifficulty difficulty, time_t resetTime, bool canReset, bool load)
{
    if (InstanceSave* old_save = GetInstanceSave(instanceId))
        return old_save;

    const MapEntry* entry = sMapStore.LookupEntry(mapId);
    if (!entry)
    {
        sLog.outError("InstanceSaveManager::AddInstanceSave: wrong mapid = %d, instanceid = %d!", mapId, instanceId);
        return NULL;
    }

    if (instanceId == 0)
    {
        sLog.outError("InstanceSaveManager::AddInstanceSave: mapid = %d, wrong instanceid = %d!", mapId, instanceId);
        return NULL;
    }

    if (!resetTime)
    {
        // initialize reset time
        // for normal instances if no creatures are killed the instance will reset in two hours
        if (entry->IsRaid() || difficulty == DIFFICULTY_HEROIC)
            resetTime = GetResetTimeFor(mapId);
        else
        {
            resetTime = time(NULL) + 2 * HOUR;
            // normally this will be removed soon after in InstanceMap::Add, prevent error
            ScheduleReset(true, resetTime, InstResetEvent(0, mapId, instanceId));
        }
    }

    sLog.outDebug("InstanceSaveManager::AddInstanceSave: mapid = %d, instanceid = %d", mapId, instanceId);

    InstanceSave* save = new InstanceSave(mapId, instanceId, difficulty, resetTime, canReset);
    if (!load) save->SaveToDB();

    m_instanceSaveById[instanceId] = save;
    return save;
}

InstanceSave* InstanceSaveManager::GetInstanceSave(uint32 InstanceId)
{
    InstanceSaveHashMap::iterator itr = m_instanceSaveById.find(InstanceId);
    return itr != m_instanceSaveById.end() ? itr->second : NULL;
}

void InstanceSaveManager::DeleteInstanceFromDB(uint32 instanceid)
{
    CharacterDatabase.BeginTransaction();
    CharacterDatabase.PExecute("DELETE FROM instance WHERE id = '%u'", instanceid);
    CharacterDatabase.PExecute("DELETE FROM character_instance WHERE instance = '%u'", instanceid);
    CharacterDatabase.PExecute("DELETE FROM group_instance WHERE instance = '%u'", instanceid);
    CharacterDatabase.CommitTransaction();
    // respawn times should be deleted only when the map gets unloaded
}

void InstanceSaveManager::RemoveInstanceSave(uint32 InstanceId)
{
    InstanceSaveHashMap::iterator itr = m_instanceSaveById.find(InstanceId);
    if (itr != m_instanceSaveById.end())
    {
        // save the resettime for normal instances only when they get unloaded
        if (time_t resettime = itr->second->GetResetTimeForDB())
            CharacterDatabase.PExecute("UPDATE instance SET resettime = '" UI64FMTD "' WHERE id = '%u'", (uint64)resettime, InstanceId);
        delete itr->second;
        m_instanceSaveById.erase(itr);
    }
}

InstanceSave::InstanceSave(uint16 MapId, uint32 InstanceId, DungeonDifficulty difficulty, time_t resetTime, bool canReset)
    : m_mapid(MapId), m_instanceid(InstanceId), m_resetTime(resetTime),
      m_difficulty(difficulty), m_canReset(canReset)
{
}

InstanceSave::~InstanceSave()
{
    // the players and groups must be unbound before deleting the save
    ASSERT(m_playerList.empty() && m_groupList.empty());
}

/*
    Called from AddInstanceSave
*/
void InstanceSave::SaveToDB()
{
    // save instance data too
    std::string data;

    Map* map = MapManager::Instance().FindMap(GetMapId(), m_instanceid);
    if (map)
    {
        ASSERT(map->IsDungeon());
        if (InstanceData* iData = ((InstanceMap*)map)->GetInstanceData())
        {
            data = iData->GetSaveData();
            if (!data.empty())
                CharacterDatabase.escape_string(data);
        }
    }

    CharacterDatabase.PExecute("INSERT INTO instance VALUES ('%u', '%u', '" UI64FMTD "', '%u', '%s')", m_instanceid, GetMapId(), (uint64)GetResetTimeForDB(), GetDifficulty(), data.c_str());
}

time_t InstanceSave::GetResetTimeForDB()
{
    // only save the reset time for normal instances
    const MapEntry* entry = sMapStore.LookupEntry(GetMapId());
    if (!entry || entry->IsRaid() || GetDifficulty() == DIFFICULTY_HEROIC)
        return 0;
    else
        return GetResetTime();
}

// to cache or not to cache, that is the question
InstanceTemplate const* InstanceSave::GetTemplate()
{
    return sObjectMgr.GetInstanceTemplate(m_mapid);
}

MapEntry const* InstanceSave::GetMapEntry()
{
    return sMapStore.LookupEntry(m_mapid);
}

void InstanceSave::DeleteFromDB()
{
    InstanceSaveManager::DeleteInstanceFromDB(GetInstanceId());
}

/* true if the instance save is still valid */
bool InstanceSave::UnloadIfEmpty()
{
    if (m_playerList.empty() && m_groupList.empty())
    {
        if (!sInstanceSaveMgr.lock_instLists)
            sInstanceSaveMgr.RemoveInstanceSave(GetInstanceId());
        return false;
    }
    else
        return true;
}

void InstanceSaveManager::_DelHelper(DatabaseType& db, const char* fields, const char* table, const char* queryTail, ...)
{
    Tokens fieldTokens = StrSplit(fields, ", ");
    ASSERT(fieldTokens.size() != 0);

    va_list ap;
    char szQueryTail [MAX_QUERY_LEN];
    va_start(ap, queryTail);
    vsnprintf(szQueryTail, MAX_QUERY_LEN, queryTail, ap);
    va_end(ap);

    QueryResult_AutoPtr result = db.PQuery("SELECT %s FROM %s %s", fields, table, szQueryTail);
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            std::ostringstream ss;
            for (size_t i = 0; i < fieldTokens.size(); i++)
            {
                std::string fieldValue = fields[i].GetCppString();
                db.escape_string(fieldValue);
                ss << (i != 0 ? " AND " : "") << fieldTokens[i] << " = '" << fieldValue << "'";
            }
            db.DirectPExecute("DELETE FROM %s WHERE %s", table, ss.str().c_str());
        }
        while (result->NextRow());
    }
}

void InstanceSaveManager::CleanupInstances()
{

    // load reset times and clean expired instances
    sInstanceSaveMgr.LoadResetTimes();

    // clean character/group - instance binds with invalid group/characters
    _DelHelper(CharacterDatabase, "character_instance.guid, instance", "character_instance", "LEFT JOIN characters ON character_instance.guid = characters.guid WHERE characters.guid IS NULL");
    _DelHelper(CharacterDatabase, "group_instance.leaderGuid, instance", "group_instance", "LEFT JOIN characters ON group_instance.leaderGuid = characters.guid LEFT JOIN groups ON group_instance.leaderGuid = groups.leaderGuid WHERE characters.guid IS NULL OR groups.leaderGuid IS NULL");

    // clean instances that do not have any players or groups bound to them
    _DelHelper(CharacterDatabase, "id, map, difficulty", "instance", "LEFT JOIN character_instance ON character_instance.instance = id LEFT JOIN group_instance ON group_instance.instance = id WHERE character_instance.instance IS NULL AND group_instance.instance IS NULL");

    // clean invalid instance references in other tables
    _DelHelper(CharacterDatabase, "character_instance.guid, instance", "character_instance", "LEFT JOIN instance ON character_instance.instance = instance.id WHERE instance.id IS NULL");
    _DelHelper(CharacterDatabase, "group_instance.leaderGuid, instance", "group_instance", "LEFT JOIN instance ON group_instance.instance = instance.id WHERE instance.id IS NULL");

    // creature_respawn and gameobject_respawn are in another database
    // first, obtain total instance set
    std::set<uint32> InstanceSet;
    QueryResult_AutoPtr result = CharacterDatabase.Query("SELECT id FROM instance");
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            InstanceSet.insert(fields[0].GetUInt32());
        }
        while (result->NextRow());
    }

    // creature_respawn
    result = WorldDatabase.Query("SELECT DISTINCT(instance) FROM creature_respawn WHERE instance <> 0");
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            if (InstanceSet.find(fields[0].GetUInt32()) == InstanceSet.end())
                WorldDatabase.DirectPExecute("DELETE FROM creature_respawn WHERE instance = '%u'", fields[0].GetUInt32());
        }
        while (result->NextRow());
    }

    // gameobject_respawn
    result = WorldDatabase.Query("SELECT DISTINCT(instance) FROM gameobject_respawn WHERE instance <> 0");
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            if (InstanceSet.find(fields[0].GetUInt32()) == InstanceSet.end())
                WorldDatabase.DirectPExecute("DELETE FROM gameobject_respawn WHERE instance = '%u'", fields[0].GetUInt32());
        }
        while (result->NextRow());
    }

    // characters
    result = CharacterDatabase.Query("SELECT DISTINCT(instance_id) FROM characters WHERE instance_id <> 0");
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            if (InstanceSet.find(fields[0].GetUInt32()) == InstanceSet.end())
                CharacterDatabase.DirectPExecute("UPDATE characters SET instance_id = '0' WHERE instance_id = '%u'", fields[0].GetUInt32());
        }
        while (result->NextRow());
    }

    // corpse
    result = CharacterDatabase.Query("SELECT DISTINCT(instance) FROM corpse WHERE instance <> 0");
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            if (InstanceSet.find(fields[0].GetUInt32()) == InstanceSet.end())
                CharacterDatabase.DirectPExecute("UPDATE corpse SET instance = '0' WHERE instance = '%u'", fields[0].GetUInt32());
        }
        while (result->NextRow());
    }

    sLog.outString(">> Initialized %u instances", (uint32)InstanceSet.size());
}

void InstanceSaveManager::PackInstances()
{
    // this routine renumbers player instance associations in such a way so they start from 1 and go up

    // obtain set of all associations
    std::vector<uint32> InstanceSet;

    // all valid ids are in the instance table
    // any associations to ids not in this table are assumed to be
    // cleaned already in CleanupInstances
    if (QueryResult_AutoPtr result = CharacterDatabase.Query("SELECT id FROM instance ORDER BY id ASC"))
    {
        do
        {
            Field* fields = result->Fetch();
            InstanceSet.push_back(fields[0].GetUInt32());
        }
        while (result->NextRow());
    }


    uint32 InstanceNumber = 1;

    WorldDatabase.Execute(    "PREPARE stmt0 FROM \"UPDATE creature_respawn   SET instance    = ? WHERE instance    = ?\"");
    WorldDatabase.Execute(    "PREPARE stmt1 FROM \"UPDATE gameobject_respawn SET instance    = ? WHERE instance    = ?\"");
    CharacterDatabase.Execute("PREPARE stmt2 FROM \"UPDATE characters         SET instance_id = ? WHERE instance_id = ?\"");
    CharacterDatabase.Execute("PREPARE stmt3 FROM \"UPDATE corpse             SET instance    = ? WHERE instance    = ?\"");
    CharacterDatabase.Execute("PREPARE stmt4 FROM \"UPDATE character_instance SET instance    = ? WHERE instance    = ?\"");
    CharacterDatabase.Execute("PREPARE stmt5 FROM \"UPDATE instance           SET id          = ? WHERE id          = ?\"");
    CharacterDatabase.Execute("PREPARE stmt6 FROM \"UPDATE group_instance     SET instance    = ? WHERE instance    = ?\"");

    // we have got ids sorted properly
    for (std::vector<uint32>::iterator i = InstanceSet.begin(); i != InstanceSet.end(); ++i)
    {
        if (*i != InstanceNumber)
        {
            // remap instance id
            WorldDatabase.PExecute(    "SET @i=%u", InstanceNumber);
            WorldDatabase.PExecute(    "SET @j=%u", *i);
            CharacterDatabase.PExecute("SET @i=%u", InstanceNumber);
            CharacterDatabase.PExecute("SET @j=%u", *i);

            WorldDatabase.Execute(    "EXECUTE stmt0 USING @i,@j");
            WorldDatabase.Execute(    "EXECUTE stmt1 USING @i,@j");
            CharacterDatabase.Execute("EXECUTE stmt2 USING @i,@j");
            CharacterDatabase.Execute("EXECUTE stmt3 USING @i,@j");
            CharacterDatabase.Execute("EXECUTE stmt4 USING @i,@j");
            CharacterDatabase.Execute("EXECUTE stmt5 USING @i,@j");
            CharacterDatabase.Execute("EXECUTE stmt6 USING @i,@j");
        }

        ++InstanceNumber;
    }

    WorldDatabase.Execute(    "DEALLOCATE PREPARE stmt0");
    WorldDatabase.Execute(    "DEALLOCATE PREPARE stmt1");
    CharacterDatabase.Execute("DEALLOCATE PREPARE stmt2");
    CharacterDatabase.Execute("DEALLOCATE PREPARE stmt3");
    CharacterDatabase.Execute("DEALLOCATE PREPARE stmt4");
    CharacterDatabase.Execute("DEALLOCATE PREPARE stmt5");
    CharacterDatabase.Execute("DEALLOCATE PREPARE stmt6");

    sLog.outString(">> Instance numbers remapped, next instance id is %u", InstanceNumber);
}

void InstanceSaveManager::LoadResetTimes()
{
    time_t now = time(NULL);
    time_t today = (now / DAY) * DAY;

    // NOTE: Use DirectPExecute for tables that will be queried later

    // get the current reset times for normal instances (these may need to be updated)
    // these are only kept in memory for InstanceSaves that are loaded later
    // resettime = 0 in the DB for raid/heroic instances so those are skipped
    typedef std::map<uint32, std::pair<uint32, uint64> > ResetTimeMapType;
    ResetTimeMapType InstResetTime;
    QueryResult_AutoPtr result = CharacterDatabase.Query("SELECT id, map, resettime FROM instance WHERE resettime > 0");
    if (result)
    {
        do
        {
            if (uint64 resettime = (*result)[2].GetUInt64())
            {
                uint32 id = (*result)[0].GetUInt32();
                uint32 mapid = (*result)[1].GetUInt32();
                InstResetTime[id] = std::pair<uint32, uint64>(mapid, resettime);
            }
        }
        while (result->NextRow());

        // update reset time for normal instances with the max creature respawn time + X hours
        result = WorldDatabase.Query("SELECT MAX(respawntime), instance FROM creature_respawn WHERE instance > 0 GROUP BY instance");
        if (result)
        {
            do
            {
                Field* fields = result->Fetch();
                uint32 instance = fields[1].GetUInt32();
                uint64 resettime = fields[0].GetUInt64() + 2 * HOUR;
                ResetTimeMapType::iterator itr = InstResetTime.find(instance);
                if (itr != InstResetTime.end() && itr->second.second != resettime)
                {
                    CharacterDatabase.DirectPExecute("UPDATE instance SET resettime = '" UI64FMTD "' WHERE id = '%u'", resettime, instance);
                    itr->second.second = resettime;
                }
            }
            while (result->NextRow());
        }

        // schedule the reset times
        for (ResetTimeMapType::iterator itr = InstResetTime.begin(); itr != InstResetTime.end(); ++itr)
            if (itr->second.second > uint32(now))
                ScheduleReset(true, itr->second.second, InstResetEvent(0, itr->second.first, itr->first));
    }

    // load the global respawn times for raid/heroic instances
    uint32 diff = sWorld.getConfig(CONFIG_INSTANCE_RESET_TIME_HOUR) * HOUR;
    m_resetTimeByMapId.resize(sMapStore.GetNumRows() + 1);
    result = CharacterDatabase.Query("SELECT mapid, resettime FROM instance_reset");
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 mapid = fields[0].GetUInt32();
            if (!sObjectMgr.GetInstanceTemplate(mapid))
            {
                sLog.outError("InstanceSaveManager::LoadResetTimes: invalid mapid %u in instance_reset!", mapid);
                CharacterDatabase.DirectPExecute("DELETE FROM instance_reset WHERE mapid = '%u'", mapid);
                continue;
            }

            // update the reset time if the hour in the configs changes
            uint64 oldresettime = fields[1].GetUInt64();
            uint64 newresettime = (oldresettime / DAY) * DAY + diff;
            if (oldresettime != newresettime)
                CharacterDatabase.DirectPExecute("UPDATE instance_reset SET resettime = '" UI64FMTD "' WHERE mapid = '%u'", newresettime, mapid);

            m_resetTimeByMapId[mapid] = newresettime;
        }
        while (result->NextRow());
    }

    // clean expired instances, references to them will be deleted in CleanupInstances
    // must be done before calculating new reset times
    _DelHelper(CharacterDatabase, "id, map, difficulty", "instance", "LEFT JOIN instance_reset ON mapid = map WHERE (instance.resettime < '" UI64FMTD "' AND instance.resettime > '0') OR (NOT instance_reset.resettime IS NULL AND instance_reset.resettime < '" UI64FMTD "')",  (uint64)now, (uint64)now);

    // calculate new global reset times for expired instances and those that have never been reset yet
    // add the global reset times to the priority queue
    for (uint32 i = 0; i < sInstanceTemplate.MaxEntry; i++)
    {
        InstanceTemplate* temp = (InstanceTemplate*)sObjectMgr.GetInstanceTemplate(i);
        if (!temp) continue;
        // only raid/heroic maps have a global reset time
        const MapEntry* entry = sMapStore.LookupEntry(temp->map);
        if (!entry || !entry->HasResetTime())
            continue;

        // the reset_delay must be at least one day
        uint32 period = uint32(((temp->reset_delay * sWorld.getRate(RATE_INSTANCE_RESET_TIME))/DAY) * DAY);
        if (period < DAY)
            period = DAY;

        time_t t = m_resetTimeByMapId[temp->map];
        if (!t)
        {
            // initialize the reset time
            t = today + period + diff;
            CharacterDatabase.DirectPExecute("INSERT INTO instance_reset VALUES ('%u','" UI64FMTD "')", i, (uint64)t);
        }

        if (t < now)
        {
            // assume that expired instances have already been cleaned
            // calculate the next reset time
            t = (t / DAY) * DAY;
            t += ((today - t) / period + 1) * period + diff;
            CharacterDatabase.DirectPExecute("UPDATE instance_reset SET resettime = '" UI64FMTD "' WHERE mapid = '%u'", (uint64)t, i);
        }

        m_resetTimeByMapId[temp->map] = t;

        // schedule the global reset/warning
        uint8 type;
        for (type = 1; type < 4; ++type)
            if (t - ResetTimeDelay[type-1] > now)
                break;

        ScheduleReset(true, t - ResetTimeDelay[type-1], InstResetEvent(type, i));
    }
}

void InstanceSaveManager::ScheduleReset(bool add, time_t time, InstResetEvent event)
{
    if (add) m_resetTimeQueue.insert(std::pair<time_t, InstResetEvent>(time, event));
    else
    {
        // find the event in the queue and remove it
        ResetTimeQueue::iterator itr;
        std::pair<ResetTimeQueue::iterator, ResetTimeQueue::iterator> range;
        range = m_resetTimeQueue.equal_range(time);
        for (itr = range.first; itr != range.second; ++itr)
            if (itr->second == event)
            {
                m_resetTimeQueue.erase(itr);
                return;
            }
        // in case the reset time changed (should happen very rarely), we search the whole queue
        if (itr == range.second)
        {
            for (itr = m_resetTimeQueue.begin(); itr != m_resetTimeQueue.end(); ++itr)
                if (itr->second == event)
                {
                    m_resetTimeQueue.erase(itr);
                    return;
                }
            if (itr == m_resetTimeQueue.end())
                sLog.outError("InstanceSaveManager::ScheduleReset: cannot cancel the reset, the event(%d,%d,%d) was not found!", event.type, event.mapid, event.instanceId);
        }
    }
}

void InstanceSaveManager::Update()
{
    time_t now = time(NULL), t;

    while (!m_resetTimeQueue.empty() && (t = m_resetTimeQueue.begin()->first) < now)
    {
        InstResetEvent& event = m_resetTimeQueue.begin()->second;
        if (event.type == 0)
        {
            // for individual normal instances, max creature respawn + X hours
            _ResetInstance(event.mapid, event.instanceId);
            m_resetTimeQueue.erase(m_resetTimeQueue.begin());
        }
        else
        {
            // global reset/warning for a certain map
            time_t resetTime = GetResetTimeFor(event.mapid);
            _ResetOrWarnAll(event.mapid, event.type != 4, resetTime);
            if (event.type != 4)
            {
                // schedule the next warning/reset
                event.type++;
                ScheduleReset(true, resetTime - ResetTimeDelay[event.type-1], event);
            }
            m_resetTimeQueue.erase(m_resetTimeQueue.begin());
        }
    }
}

void InstanceSaveManager::_ResetSave(InstanceSaveHashMap::iterator& itr)
{
    // unbind all players bound to the instance
    // do not allow UnbindInstance to automatically unload the InstanceSaves
    lock_instLists = true;
    InstanceSave::PlayerListType& pList = itr->second->m_playerList;
    while (!pList.empty())
    {
        Player* player = *(pList.begin());
        player->UnbindInstance(itr->second->GetMapId(), itr->second->GetDifficulty(), true);
    }
    InstanceSave::GroupListType& gList = itr->second->m_groupList;
    while (!gList.empty())
    {
        Group* group = *(gList.begin());
        group->UnbindInstance(itr->second->GetMapId(), itr->second->GetDifficulty(), true);
    }
    InstanceSaveHashMap::iterator next;
    next = itr;
    ++next;
    delete itr->second;
    m_instanceSaveById.erase(itr);
    itr = next;
    lock_instLists = false;
}

void InstanceSaveManager::_ResetInstance(uint32 mapid, uint32 instanceId)
{
    sLog.outDebug("InstanceSaveMgr::_ResetInstance %u, %u", mapid, instanceId);
    Map* map = (MapInstanced*)MapManager::Instance().CreateBaseMap(mapid);
    if (!map->Instanceable())
        return;

    InstanceSaveHashMap::iterator itr = m_instanceSaveById.find(instanceId);
    if (itr != m_instanceSaveById.end())
        _ResetSave(itr);

    DeleteInstanceFromDB(instanceId);                       // even if save not loaded

    Map* iMap = ((MapInstanced*)map)->FindInstanceMap(instanceId);

    if (iMap)
    {
        if (iMap->IsDungeon())
            ((InstanceMap*)iMap)->Reset(INSTANCE_RESET_RESPAWN_DELAY);
    }
    else sObjectMgr.DeleteRespawnTimeForInstance(instanceId);   // even if map is not loaded
}

void InstanceSaveManager::_ResetOrWarnAll(uint32 mapid, bool warn, time_t resetTime)
{
    // global reset for all instances of the given map
    // note: this isn't fast but it's meant to be executed very rarely
    Map const* map = MapManager::Instance().CreateBaseMap(mapid);
    if (!map->Instanceable())
        return;

    time_t now = time(NULL);

    if (!warn)
    {
        // this is called one minute before the reset time
        InstanceTemplate const* temp = sObjectMgr.GetInstanceTemplate(mapid);
        if (!temp || !temp->reset_delay)
        {
            sLog.outError("InstanceSaveManager::ResetOrWarnAll: no instance template or reset delay for map %d", mapid);
            return;
        }

        // remove all binds to instances of the given map
        for (InstanceSaveHashMap::iterator itr = m_instanceSaveById.begin(); itr != m_instanceSaveById.end();)
        {
            if (itr->second->GetMapId() == mapid)
                _ResetSave(itr);
            else
                ++itr;
        }

        // delete them from the DB, even if not loaded
        CharacterDatabase.BeginTransaction();
        CharacterDatabase.PExecute("DELETE FROM character_instance USING character_instance LEFT JOIN instance ON character_instance.instance = id WHERE map = '%u'", mapid);
        CharacterDatabase.PExecute("DELETE FROM group_instance USING group_instance LEFT JOIN instance ON group_instance.instance = id WHERE map = '%u'", mapid);
        CharacterDatabase.PExecute("DELETE FROM instance WHERE map = '%u'", mapid);
        CharacterDatabase.CommitTransaction();

        // calculate the next reset time
        uint32 diff = sWorld.getConfig(CONFIG_INSTANCE_RESET_TIME_HOUR) * HOUR;

        // the reset_delay must be at least one day
        uint32 period = uint32(((temp->reset_delay * sWorld.getRate(RATE_INSTANCE_RESET_TIME))/DAY) * DAY);
        if (period < DAY)
            period = DAY;

        uint64 next_reset = ((resetTime + MINUTE) / DAY * DAY) + period + diff;

        // schedule next reset.
        m_resetTimeByMapId[mapid] = (time_t) next_reset;
        ScheduleReset(true, (time_t) (next_reset - 3600), InstResetEvent(1, mapid));

        // update it in the DB
        CharacterDatabase.PExecute("UPDATE instance_reset SET resettime = '" UI64FMTD "' WHERE mapid = '%d'", next_reset, mapid);
    }

    MapInstanced::InstancedMaps& instMaps = ((MapInstanced*)map)->GetInstancedMaps();
    MapInstanced::InstancedMaps::iterator mitr;
    uint32 timeLeft;

    for (mitr = instMaps.begin(); mitr != instMaps.end(); ++mitr)
    {
        Map* map2 = mitr->second;
        if (!map2->IsDungeon())
            continue;

        if (warn)
        {            
            if (now <= resetTime)
                timeLeft = 0;
            else
                timeLeft = uint32(now - resetTime);

            ((InstanceMap*)map2)->SendResetWarnings(timeLeft);
        }
        else
            ((InstanceMap*)map2)->Reset(INSTANCE_RESET_GLOBAL);
    }

    // @todo delete creature/gameobject respawn times even if the maps are not loaded
}

uint32 InstanceSaveManager::GetNumBoundPlayersTotal()
{
    uint32 ret = 0;
    for (InstanceSaveHashMap::iterator itr = m_instanceSaveById.begin(); itr != m_instanceSaveById.end(); ++itr)
        ret += itr->second->GetPlayerCount();
    return ret;
}

uint32 InstanceSaveManager::GetNumBoundGroupsTotal()
{
    uint32 ret = 0;
    for (InstanceSaveHashMap::iterator itr = m_instanceSaveById.begin(); itr != m_instanceSaveById.end(); ++itr)
        ret += itr->second->GetGroupCount();
    return ret;
}

