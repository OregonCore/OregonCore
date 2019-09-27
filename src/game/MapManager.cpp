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

#include "MapManager.h"
#include "InstanceSaveMgr.h"
#include "Database/DatabaseEnv.h"
#include "Log.h"
#include "ObjectAccessor.h"
#include "Transports.h"
#include "GridDefines.h"
#include "MapInstanced.h"
#include "World.h"
#include "Corpse.h"
#include "ObjectMgr.h"

#define CLASS_LOCK Oregon::ClassLevelLockable<MapManager, ACE_Thread_Mutex>
INSTANTIATE_SINGLETON_2(MapManager, CLASS_LOCK);
INSTANTIATE_CLASS_MUTEX(MapManager, ACE_Thread_Mutex);

extern GridState* si_GridStates[];                          // debugging code, should be deleted some day

MapManager::MapManager()
{
    i_gridCleanUpDelay = sWorld.getConfig(CONFIG_INTERVAL_GRIDCLEAN);
    i_timer.SetInterval(sWorld.getConfig(CONFIG_INTERVAL_MAPUPDATE));
}

MapManager::~MapManager()
{
    for (MapMapType::iterator iter = i_maps.begin(); iter != i_maps.end(); ++iter)
        delete iter->second;

    for (TransportSet::iterator i = m_Transports.begin(); i != m_Transports.end(); ++i)
        delete *i;

    Map::DeleteStateMachine();
}

void MapManager::Initialize()
{
    Map::InitStateMachine();

    // debugging code, should be deleted some day
    {
        for (uint8 i = 0; i < MAX_GRID_STATE; ++i)
            i_GridStates[i] = si_GridStates[i];

        i_GridStateErrorCount = 0;
    }
    int num_threads(sWorld.getConfig(CONFIG_NUMTHREADS));
    // Start mtmaps if needed.
    if (num_threads > 0 && m_updater.activate(num_threads) == -1)
        abort();

    InitMaxInstanceId();
}

void MapManager::InitializeVisibilityDistanceInfo()
{
    for (MapMapType::iterator iter = i_maps.begin(); iter != i_maps.end(); ++iter)
        (*iter).second->InitVisibilityDistance();
}

// debugging code, should be deleted some day
void MapManager::checkAndCorrectGridStatesArray()
{
    bool ok = true;
    for (int i = 0; i < MAX_GRID_STATE; i++)
    {
        if (i_GridStates[i] != si_GridStates[i])
        {
            sLog.outError("MapManager::checkGridStates(), GridState: si_GridStates is currupt !!!");
            ok = false;
            si_GridStates[i] = i_GridStates[i];
        }
        #if 0 // OREGON_DEBUG
        // inner class checking only when compiled with debug
        if (!si_GridStates[i]->checkMagic())
        {
            ok = false;
            si_GridStates[i]->setMagic();
        }
        #endif
    }
    if (!ok)
        ++i_GridStateErrorCount;
    if (i_GridStateErrorCount > 2)
        ASSERT(false);                                      // force a crash. Too many errors
}

Map* MapManager::_createBaseMap(uint32 id)
{
    Map* m = _findMap(id);

    if (m == NULL)
    {
        Guard guard(*this);

        const MapEntry* entry = sMapStore.LookupEntry(id);
        if (entry && entry->Instanceable())
            m = new MapInstanced(id, i_gridCleanUpDelay);
        else
            m = new Map(id, i_gridCleanUpDelay, 0, DIFFICULTY_NORMAL);
        i_maps[id] = m;
    }

    ASSERT(m != NULL);
    return m;
}

Map* MapManager::CreateMap(uint32 id, const WorldObject* obj, uint32 loginInstanceId)
{
    ASSERT(obj);
    //if (!obj->IsInWorld()) sLog.outError("GetMap: called for map %d with object (typeid %d, guid %d, mapid %d, instanceid %d) who is not in world!", id, obj->GetTypeId(), obj->GetGUIDLow(), obj->GetMapId(), obj->GetInstanceId());
    Map* m = _createBaseMap(id);

    if (m && (obj->GetTypeId() == TYPEID_PLAYER) && m->Instanceable())
        m = ((MapInstanced*)m)->CreateInstanceForPlayer(id, (Player*)obj, loginInstanceId);

    return m;
}

Map* MapManager::FindMap(uint32 mapid, uint32 instanceId) const
{
    Map* map = _findMap(mapid);
    if (!map)
        return NULL;

    if (!map->Instanceable())
        return instanceId == 0 ? map : NULL;

    return ((MapInstanced*)map)->FindInstanceMap(instanceId);
}

/*
    checks that do not require a map to be created
    will send transfer error messages on fail
*/
Map::EnterState MapManager::PlayerCannotEnter(uint32 mapid, Player* player, bool loginCheck)
{
    const MapEntry* entry = sMapStore.LookupEntry(mapid);
    if (!entry)
        return Map::CANNOT_ENTER_NO_ENTRY;

    if (!entry->IsDungeon())
        return Map::CAN_ENTER;

    InstanceTemplate const* instance = sObjectMgr.GetInstanceTemplate(mapid);
    if (!instance)
        return Map::CANNOT_ENTER_UNINSTANCED_DUNGEON;

    //The player has a heroic mode and tries to enter into instance which has no a heroic mode
    if (!entry->SupportsHeroicMode() && player->GetDifficulty() == DIFFICULTY_HEROIC)
        return Map::CANNOT_ENTER_DIFFICULTY_UNAVAILABLE;

    //Bypass checks for GMs
    if (player->IsGameMaster())
        return Map::CAN_ENTER;

    const char* mapName = entry->name[player->GetSession()->GetSessionDbcLocale()];

    Group* group = player->GetGroup();
    if (entry->IsRaid()) // can only enter in a raid group
        if ((!group || !group->isRaidGroup()) && !sWorld.getConfig(CONFIG_INSTANCE_IGNORE_RAID))
            return Map::CANNOT_ENTER_NOT_IN_RAID;


    if (!player->IsAlive())
    {
        if (Corpse* corpse = player->GetCorpse())
        {
            // let enter in ghost mode in instance that connected to inner instance with corpse
            uint32 instance_map = corpse->GetMapId();
            do
            {
                if (instance_map == mapid)
                    break;

                InstanceTemplate const* instance = sObjectMgr.GetInstanceTemplate(instance_map);
                instance_map = instance ? instance->parent : 0;
            }
            while (instance_map);

            if (!instance_map)
                return Map::CANNOT_ENTER_CORPSE_IN_DIFFERENT_INSTANCE;

            sLog.outDebug("MAP: Player '%s' has corpse in instance '%s' and can enter", player->GetName(), mapName);
        }
        else
            sLog.outDebug("Map::CanEnter - player '%s' is dead but doesn't have a corpse!", player->GetName());
    }

    //Get instance where player's group is bound & its map
    if (!loginCheck && group)
    {
        InstanceGroupBind* boundInstance = group->GetBoundInstance(entry);
        if (boundInstance && boundInstance->save)
            if (Map* boundMap = MapManager::Instance().FindMap(mapid, boundInstance->save->GetInstanceId()))
                if (Map::EnterState denyReason = boundMap->CannotEnter(player))
                    return denyReason;
    }

    // @todo Implement 5 dungeons per hour limit

    if (player->Satisfy(sObjectMgr.GetAccessRequirement(instance->access_id), mapid, true))
        return Map::CAN_ENTER;
    else
        return Map::CANNOT_ENTER_UNSPECIFIED_REASON;
}

void MapManager::Update(time_t diff)
{
    i_timer.Update(diff);
    if (!i_timer.Passed())
        return;

    MapMapType::iterator iter = i_maps.begin();
    for (; iter != i_maps.end(); ++iter)
    {
        if (m_updater.activated())
            m_updater.schedule_update(*iter->second, i_timer.GetCurrent());
        else
            iter->second->Update(i_timer.GetCurrent());
    }
    if (m_updater.activated())
        m_updater.wait();

    for (iter = i_maps.begin(); iter != i_maps.end(); ++iter)
        iter->second->DelayedUpdate(uint32(i_timer.GetCurrent()));

    ObjectAccessor::Instance().Update(i_timer.GetCurrent());
    for (TransportSet::iterator iter = m_Transports.begin(); iter != m_Transports.end(); ++iter)
        (*iter)->Update(i_timer.GetCurrent());

    i_timer.SetCurrent(0);
}

void MapManager::DoDelayedMovesAndRemoves()
{
}

bool MapManager::ExistMapAndVMap(uint32 mapid, float x, float y)
{
    GridCoord p = Oregon::ComputeGridCoord(x, y);

    int gx = (MAX_NUMBER_OF_GRIDS - 1) - p.x_coord;
    int gy = (MAX_NUMBER_OF_GRIDS - 1) - p.y_coord;

    return Map::ExistMap(mapid, gx, gy) && Map::ExistVMap(mapid, gx, gy);
}

bool MapManager::IsValidMAP(uint32 mapid)
{
    MapEntry const* mEntry = sMapStore.LookupEntry(mapid);
    return mEntry && (!mEntry->Instanceable() || sObjectMgr.GetInstanceTemplate(mapid));
}

void MapManager::UnloadAll()
{
    for (MapMapType::iterator iter = i_maps.begin(); iter != i_maps.end(); ++iter)
        iter->second->UnloadAll();

    while (!i_maps.empty())
    {
        delete i_maps.begin()->second;
        i_maps.erase(i_maps.begin());
    }

    if (m_updater.activated())
        m_updater.deactivate();
}

void MapManager::InitMaxInstanceId()
{
    i_MaxInstanceId = 0;

    QueryResult_AutoPtr result = CharacterDatabase.Query("SELECT MAX(id) FROM instance");
    if (result)
        i_MaxInstanceId = result->Fetch()[0].GetUInt32();
}

uint32 MapManager::GetNumInstances()
{
    Guard guard(*this);

    uint32 ret = 0;
    for (MapMapType::iterator itr = i_maps.begin(); itr != i_maps.end(); ++itr)
    {
        Map* map = itr->second;
        if (!map->Instanceable())
            continue;
        MapInstanced::InstancedMaps& maps = ((MapInstanced*)map)->GetInstancedMaps();
        for (MapInstanced::InstancedMaps::iterator mitr = maps.begin(); mitr != maps.end(); ++mitr)
            if (mitr->second->IsDungeon()) ret++;
    }
    return ret;
}

uint32 MapManager::GetNumPlayersInInstances()
{
    Guard guard(*this);

    uint32 ret = 0;
    for (MapMapType::iterator itr = i_maps.begin(); itr != i_maps.end(); ++itr)
    {
        Map* map = itr->second;
        if (!map->Instanceable())
            continue;
        MapInstanced::InstancedMaps& maps = ((MapInstanced*)map)->GetInstancedMaps();
        for (MapInstanced::InstancedMaps::iterator mitr = maps.begin(); mitr != maps.end(); ++mitr)
            if (mitr->second->IsDungeon())
                ret += ((InstanceMap*)mitr->second)->GetPlayers().getSize();
    }
    return ret;
}

