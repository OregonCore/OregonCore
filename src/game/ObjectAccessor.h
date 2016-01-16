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

#ifndef OREGON_OBJECTACCESSOR_H
#define OREGON_OBJECTACCESSOR_H

#include "Platform/Define.h"
#include "Policies/Singleton.h"
#include <ace/Thread_Mutex.h>
#include "Utilities/UnorderedMap.h"
#include "Policies/ThreadingModel.h"

#include "ByteBuffer.h"
#include "UpdateData.h"

#include "GridDefines.h"
#include "Object.h"
#include "Player.h"

#include <set>

class Creature;
class Corpse;
class Unit;
class GameObject;
class DynamicObject;
class WorldObject;
class Map;

template <class T>
class HashMapHolder
{
    public:

        typedef UNORDERED_MAP<uint64, T*> MapType;
        typedef ACE_Thread_Mutex LockType;
        typedef Oregon::GeneralLock<LockType > Guard;

        static void Insert(T* o)
        {
            Guard guard(i_lock);
            m_objectMap[o->GetGUID()] = o;
        }

        static void Remove(T* o)
        {
            Guard guard(i_lock);
            m_objectMap.erase(o->GetGUID());
        }

        static T* Find(uint64 guid)
        {
            Guard guard(i_lock);
            typename MapType::iterator itr = m_objectMap.find(guid);
            return (itr != m_objectMap.end()) ? itr->second : NULL;
        }

        static MapType& GetContainer()
        {
            return m_objectMap;
        }

        static LockType* GetLock()
        {
            return &i_lock;
        }
    private:

        //Non instanceable only static
        HashMapHolder() {}

        static LockType i_lock;
        static MapType  m_objectMap;
};

class ObjectAccessor : public Oregon::Singleton<ObjectAccessor, Oregon::ClassLevelLockable<ObjectAccessor, ACE_Thread_Mutex> >
{
        friend class Oregon::OperatorNew<ObjectAccessor>;
        ObjectAccessor();
        ~ObjectAccessor();
        ObjectAccessor(const ObjectAccessor&);
        ObjectAccessor& operator=(const ObjectAccessor&);

    public:

        typedef UNORDERED_MAP<uint64, Corpse*> Player2CorpsesMapType;
        typedef UNORDERED_MAP<Player*, UpdateData>::value_type UpdateDataValueType;

        // returns object if is in world
        template<class T> static T* GetObjectInWorld(uint64 guid, T* /*typeSpecifier*/)
        {
            return HashMapHolder<T>::Find(guid);
        }

        // Player may be not in world while in ObjectAccessor
        static Player* GetObjectInWorld(uint64 guid, Player* /*typeSpecifier*/)
        {
            Player* player = HashMapHolder<Player>::Find(guid);
            if (player && player->IsInWorld())
                return player;
            return NULL;
        }

        static Unit* GetObjectInWorld(uint64 guid, Unit* /*typeSpecifier*/)
        {
            if (IS_PLAYER_GUID(guid))
                return (Unit*)GetObjectInWorld(guid, (Player*)NULL);

            if (IS_PET_GUID(guid))
                return (Unit*)GetObjectInWorld(guid, (Pet*)NULL);

            return (Unit*)GetObjectInWorld(guid, (Creature*)NULL);
        }

        // returns object if is in map
        template<class T> static T* GetObjectInMap(uint64 guid, Map* map, T* /*typeSpecifier*/)
        {
            assert(map);
            if (T* obj = GetObjectInWorld(guid, (T*)NULL))
                if (obj->GetMap() == map)
                    return obj;
            return NULL;
        }

        template<class T> static T* GetObjectInWorld(uint32 mapid, float x, float y, uint64 guid, T* /*fake*/)
        {
            T* obj = HashMapHolder<T>::Find(guid);
            if (!obj || obj->GetMapId() != mapid)
                return NULL;

            CellCoord p = Oregon::ComputeCellCoord(x, y);
            if (p.x_coord >= TOTAL_NUMBER_OF_CELLS_PER_MAP || p.y_coord >= TOTAL_NUMBER_OF_CELLS_PER_MAP)
            {
                sLog.outError("ObjectAccessor::GetObjectInWorld: invalid coordinates supplied X:%f Y:%f grid cell [%u:%u]", x, y, p.x_coord, p.y_coord);
                return NULL;
            }

            CellCoord q = Oregon::ComputeCellCoord(obj->GetPositionX(), obj->GetPositionY());
            if (q.x_coord >= TOTAL_NUMBER_OF_CELLS_PER_MAP || q.y_coord >= TOTAL_NUMBER_OF_CELLS_PER_MAP)
            {
                sLog.outError("ObjectAccessor::GetObjecInWorld: object (GUID: %u TypeId: %u) has invalid coordinates X:%f Y:%f grid cell [%u:%u]", obj->GetGUIDLow(), obj->GetTypeId(), obj->GetPositionX(), obj->GetPositionY(), q.x_coord, q.y_coord);
                return NULL;
            }

            int32 dx = int32(p.x_coord) - int32(q.x_coord);
            int32 dy = int32(p.y_coord) - int32(q.y_coord);

            if (dx > -2 && dx < 2 && dy > -2 && dy < 2)
                return obj;
            else
                return NULL;
        }

        // these functions return objects only if in map of specified object
        static WorldObject* GetWorldObject(WorldObject const&, uint64);
        static Object* GetObjectByTypeMask(WorldObject const&, uint64, uint32 typemask);
        static Corpse* GetCorpse(WorldObject const& u, uint64 guid);
        static GameObject* GetGameObject(WorldObject const& u, uint64 guid);
        static DynamicObject* GetDynamicObject(WorldObject const& u, uint64 guid);
        static Unit* GetUnit(WorldObject const&, uint64 guid);
        static Creature* GetCreature(WorldObject const& u, uint64 guid);
        static Pet* GetPet(WorldObject const&, uint64 guid);
        static Player* GetPlayer(WorldObject const&, uint64 guid);
        static Creature* GetCreatureOrPet(WorldObject const&, uint64);

        // these functions return objects if found in whole world
        // ACCESS LIKE THAT IS NOT THREAD SAFE
        static Pet* FindPet(uint64);
        static Player* FindPlayer(uint64, bool force = false);
        static Unit* FindUnit(uint64);
        Player* FindPlayerByName(const char* name, bool force = false);
        Player* FindPlayerByAccountId(uint64 Id, bool force = false);

        // when using this, you must use the hashmapholder's lock
        HashMapHolder<Player>::MapType& GetPlayers()
        {
            return HashMapHolder<Player>::GetContainer();
        }

        // when using this, you must use the hashmapholder's lock
        static HashMapHolder<Creature>::MapType const& GetCreatures()
        {
            return HashMapHolder<Creature>::GetContainer();
        }

        // when using this, you must use the hashmapholder's lock
        static HashMapHolder<GameObject>::MapType const& GetGameObjects()
        {
            return HashMapHolder<GameObject>::GetContainer();
        }

        template<class T> void AddObject(T* object)
        {
            HashMapHolder<T>::Insert(object);
        }

        template<class T> void RemoveObject(T* object)
        {
            HashMapHolder<T>::Remove(object);
        }

        void RemoveObject(Player* pl)
        {
            HashMapHolder<Player>::Remove(pl);
            RemoveUpdateObject((Object*)pl);
        }

        void SaveAllPlayers();

        void AddUpdateObject(Object* obj)
        {
            Guard guard(i_updateGuard);
            i_objects.insert(obj);
        }

        void RemoveUpdateObject(Object* obj)
        {
            Guard guard(i_updateGuard);
            i_objects.erase(obj);
        }

        void Update(uint32 diff);

        Corpse* GetCorpseForPlayerGUID(uint64 guid);
        void RemoveCorpse(Corpse* corpse);
        void AddCorpse(Corpse* corpse);
        void AddCorpsesToGrid(GridCoord const& gridpair, GridType& grid, Map* map);
        Corpse* ConvertCorpseForPlayer(uint64 player_guid, bool insignia = false);
        void RemoveOldCorpses();

        typedef ACE_Thread_Mutex LockType;
        typedef Oregon::GeneralLock<LockType> Guard;

    private:

        Player2CorpsesMapType i_player2corpse;

        static void _buildChangeObjectForPlayer(WorldObject*, UpdateDataMapType&);
        static void _buildPacket(Player*, Object*, UpdateDataMapType&);
        void _update();

        std::set<Object*> i_objects;

        LockType i_updateGuard;
        LockType i_corpseGuard;
};
#endif

