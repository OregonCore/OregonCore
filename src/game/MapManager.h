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

#ifndef OREGON_MAPMANAGER_H
#define OREGON_MAPMANAGER_H

#include "Platform/Define.h"
#include "Policies/Singleton.h"
#include "ace/Thread_Mutex.h"
#include "Common.h"
#include "Map.h"
#include "GridStates.h"
#include "MapUpdater.h"

class Transport;

class MapManager : public Oregon::Singleton<MapManager, Oregon::ClassLevelLockable<MapManager, ACE_Thread_Mutex> >
{

        friend class Oregon::OperatorNew<MapManager>;
        typedef UNORDERED_MAP<uint32, Map*> MapMapType;
        typedef std::pair<UNORDERED_MAP<uint32, Map*>::iterator, bool>  MapMapPair;

    public:

        Map* CreateMap(uint32 mapId, const WorldObject* obj, uint32 loginInstanceId = 0);
        Map const* CreateBaseMap(uint32 id) const
        {
            return const_cast<MapManager*>(this)->_createBaseMap(id);
        }
        Map* FindMap(uint32 mapid, uint32 instanceId = 0) const;

        uint16 GetAreaFlag(uint32 mapid, float x, float y, float z) const
        {
            Map const* m = CreateBaseMap(mapid);
            return m->GetAreaFlag(x, y, z);
        }
        uint32 GetAreaId(uint32 mapid, float x, float y, float z)
        {
            return Map::GetAreaId(GetAreaFlag(mapid, x, y, z), mapid);
        }
        uint32 GetZoneId(uint32 mapid, float x, float y, float z)
        {
            return Map::GetZoneId(GetAreaFlag(mapid, x, y, z), mapid);
        }

        void Initialize(void);
        void Update(time_t);

        void SetGridCleanUpDelay(uint32 t)
        {
            if (t < MIN_GRID_DELAY)
                i_gridCleanUpDelay = MIN_GRID_DELAY;
            else
                i_gridCleanUpDelay = t;
        }

        void SetMapUpdateInterval(uint32 t)
        {
            if (t > MIN_MAP_UPDATE_DELAY)
                t = MIN_MAP_UPDATE_DELAY;

            i_timer.SetInterval(t);
            i_timer.Reset();
        }

        //void LoadGrid(int mapid, float x, float y, const WorldObject* obj, bool no_unload = false);
        void UnloadAll();

        static bool ExistMapAndVMap(uint32 mapid, float x, float y);
        static bool IsValidMAP(uint32 mapid);

        static bool IsValidMapCoord(uint32 mapid, float x, float y)
        {
            return IsValidMAP(mapid) && Oregon::IsValidMapCoord(x, y);
        }

        static bool IsValidMapCoord(uint32 mapid, float x, float y, float z)
        {
            return IsValidMAP(mapid) && Oregon::IsValidMapCoord(x, y, z);
        }

        static bool IsValidMapCoord(uint32 mapid, float x, float y, float z, float o)
        {
            return IsValidMAP(mapid) && Oregon::IsValidMapCoord(x, y, z, o);
        }

        static bool IsValidMapCoord(WorldLocation const& loc)
        {
            return IsValidMapCoord(loc.GetMapId(), loc.GetPositionX(), loc.GetPositionY(), loc.GetPositionZ(), loc.GetOrientation());
        }

        // modulos a radian orientation to the range of 0..2PI
        static float NormalizeOrientation(float o)
        {
            // fmod only supports positive numbers. Thus we have
            // to emulate negative numbers
            if (o < 0)
            {
                float mod = o * -1;
                mod = fmod(mod, 2.0f * static_cast<float>(M_PI));
                mod = -mod + 2.0f * M_PI;
                return mod;
            }
            return fmod(o, 2.0f * static_cast<float>(M_PI));
        }

        void DoDelayedMovesAndRemoves();

        void LoadTransports();

        typedef std::set<Transport*> TransportSet;
        TransportSet m_Transports;

        typedef std::map<uint32, TransportSet> TransportMap;
        TransportMap m_TransportsByMap;

        Map::EnterState PlayerCannotEnter(uint32 mapid, Player* player, bool loginCheck = false);
        uint32 GenerateInstanceId()
        {
            return ++i_MaxInstanceId;
        }
        void InitMaxInstanceId();
        void InitializeVisibilityDistanceInfo();

        /* statistics */
        uint32 GetNumInstances();
        uint32 GetNumPlayersInInstances();

        MapUpdater * GetMapUpdater() { return &m_updater; }

    private:
        // debugging code, should be deleted some day
        void checkAndCorrectGridStatesArray();              // just for debugging to find some memory overwrites
        GridState* i_GridStates[MAX_GRID_STATE];            // shadow entries to the global array in Map.cpp
        int i_GridStateErrorCount;
    private:
        MapManager();
        ~MapManager();

        MapManager(const MapManager&);
        MapManager& operator=(const MapManager&);

        Map* _createBaseMap(uint32 id);
        Map* _findMap(uint32 id) const
        {
            MapMapType::const_iterator iter = i_maps.find(id);
            return (iter == i_maps.end() ? NULL : iter->second);
        }

        typedef Oregon::ClassLevelLockable<MapManager, ACE_Thread_Mutex>::Lock Guard;
        uint32 i_gridCleanUpDelay;
        MapMapType i_maps;
        IntervalTimer i_timer;

        uint32 i_MaxInstanceId;
        MapUpdater m_updater;
};
#endif

