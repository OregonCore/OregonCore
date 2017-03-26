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

#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "Creature.h"
#include "GameObject.h"
#include "DynamicObject.h"
#include "Corpse.h"
#include "WorldSession.h"
#include "WorldPacket.h"
#include "Item.h"
#include "GridNotifiers.h"
#include "Map.h"
#include "ObjectGuid.h"
#include "World.h"

#define CLASS_LOCK Oregon::ClassLevelLockable<ObjectAccessor, ACE_Thread_Mutex>
INSTANTIATE_SINGLETON_2(ObjectAccessor, CLASS_LOCK);
INSTANTIATE_CLASS_MUTEX(ObjectAccessor, ACE_Thread_Mutex);

ObjectAccessor::ObjectAccessor()
{
}

ObjectAccessor::~ObjectAccessor()
{
    for (Player2CorpsesMapType::const_iterator itr = i_player2corpse.begin(); itr != i_player2corpse.end(); ++itr)
    {
        itr->second->RemoveFromWorld();
        delete itr->second;
    }
}

WorldObject* ObjectAccessor::GetWorldObject(WorldObject const& p, uint64 guid)
{
    switch (GUID_HIPART(guid))
    {
    case HIGHGUID_PLAYER:        return GetPlayer(p, guid);
    case HIGHGUID_TRANSPORT:
    case HIGHGUID_MO_TRANSPORT:
    case HIGHGUID_GAMEOBJECT:    return GetGameObject(p, guid);
    case HIGHGUID_UNIT:          return GetCreature(p, guid);
    case HIGHGUID_PET:           return GetPet(p, guid);
    case HIGHGUID_DYNAMICOBJECT: return GetDynamicObject(p, guid);
    case HIGHGUID_CORPSE:        return GetCorpse(p, guid);
    default:                     return NULL;
    }
}

Object* ObjectAccessor::GetObjectByTypeMask(WorldObject const& p, uint64 guid, uint32 typemask)
{
    switch (GUID_HIPART(guid))
    {
    case HIGHGUID_ITEM:
        if (typemask & TYPEMASK_ITEM && p.GetTypeId() == TYPEID_PLAYER)
            return ((Player const&)p).GetItemByGuid(guid);
        break;
    case HIGHGUID_PLAYER:
        if (typemask & TYPEMASK_PLAYER)
            return GetPlayer(p, guid);
        break;
    case HIGHGUID_GAMEOBJECT:
        if (typemask & TYPEMASK_GAMEOBJECT)
            return GetGameObject(p, guid);
        break;
    case HIGHGUID_UNIT:
        if (typemask & TYPEMASK_UNIT)
            return GetCreature(p, guid);
        break;
    case HIGHGUID_PET:
        if (typemask & TYPEMASK_UNIT)
            return GetPet(p, guid);
        break;
    case HIGHGUID_DYNAMICOBJECT:
        if (typemask & TYPEMASK_DYNAMICOBJECT)
            return GetDynamicObject(p, guid);
        break;
    case HIGHGUID_TRANSPORT:
    case HIGHGUID_CORPSE:
    case HIGHGUID_MO_TRANSPORT:
        break;
    }

    return NULL;
}

Corpse* ObjectAccessor::GetCorpse(WorldObject const& u, uint64 guid)
{
    return GetObjectInMap(guid, u.GetMap(), (Corpse*)NULL);
}

GameObject* ObjectAccessor::GetGameObject(WorldObject const& u, uint64 guid)
{
    return GetObjectInMap(guid, u.GetMap(), (GameObject*)NULL);
}

DynamicObject* ObjectAccessor::GetDynamicObject(WorldObject const& u, uint64 guid)
{
    return GetObjectInMap(guid, u.GetMap(), (DynamicObject*)NULL);
}

Unit* ObjectAccessor::GetUnit(WorldObject const& u, uint64 guid)
{
    return GetObjectInMap(guid, u.GetMap(), (Unit*)NULL);
}

Creature* ObjectAccessor::GetCreature(WorldObject const& u, uint64 guid)
{
    return GetObjectInMap(guid, u.GetMap(), (Creature*)NULL);
}

Pet* ObjectAccessor::GetPet(WorldObject const& u, uint64 guid)
{
    return GetObjectInMap(guid, u.GetMap(), (Pet*)NULL);
}

Player* ObjectAccessor::GetPlayer(WorldObject const& u, uint64 guid)
{
    return GetObjectInMap(guid, u.GetMap(), (Player*)NULL);
}

Creature* ObjectAccessor::GetCreatureOrPet(WorldObject const& u, uint64 guid)
{
    if (IS_PET_GUID(guid))
        return GetPet(u, guid);

    if (IS_CREATURE_GUID(guid))
        return GetCreature(u, guid);

    return NULL;
}

Pet* ObjectAccessor::FindPet(uint64 guid)
{
    return GetObjectInWorld(guid, (Pet*)NULL);
}

Player* ObjectAccessor::FindPlayer(uint64 guid, bool force)
{
    if (!force)
        return GetObjectInWorld(guid, (Player*)NULL);

    Guard guard(*HashMapHolder<Player>::GetLock());

    HashMapHolder<Player>::MapType& m = HashMapHolder<Player>::GetContainer();
    for (HashMapHolder<Player>::MapType::iterator iter = m.begin(); iter != m.end(); ++iter)
        if (iter->second->GetGUID() == guid)
            return iter->second;

    return NULL;
}

Unit* ObjectAccessor::FindUnit(uint64 guid)
{
    return GetObjectInWorld(guid, (Unit*)NULL);
}

Player* ObjectAccessor::FindPlayerByName(const char* name, bool force)
{
    Guard guard(*HashMapHolder<Player>::GetLock());

    HashMapHolder<Player>::MapType& m = HashMapHolder<Player>::GetContainer();
    for (HashMapHolder<Player>::MapType::iterator iter = m.begin(); iter != m.end(); ++iter)
        if (!strcmp(name, iter->second->GetName()) && (iter->second->IsInWorld() || force))
            return iter->second;

    return NULL;
}

Player* ObjectAccessor::FindPlayerByAccountId(uint64 Id, bool force)
{
    Guard guard(*HashMapHolder<Player>::GetLock());
    HashMapHolder<Player>::MapType& m = HashMapHolder<Player>::GetContainer();
    for (HashMapHolder<Player>::MapType::iterator iter = m.begin(); iter != m.end(); ++iter)
        if (iter->second->GetSession()->GetAccountId() == Id && (iter->second->IsInWorld() || force))
            return iter->second;

    return NULL;
}

void ObjectAccessor::SaveAllPlayers()
{
    Guard guard(*HashMapHolder<Player>::GetLock());
    HashMapHolder<Player>::MapType& m = HashMapHolder<Player>::GetContainer();
    for (HashMapHolder<Player>::MapType::iterator itr = m.begin(); itr != m.end(); ++itr)
        itr->second->SaveToDB();
}

Corpse* ObjectAccessor::GetCorpseForPlayerGUID(uint64 guid)
{
    Guard guard(i_corpseGuard);

    Player2CorpsesMapType::iterator iter = i_player2corpse.find(guid);
    if (iter == i_player2corpse.end())
        return NULL;

    ASSERT(iter->second->GetType() != CORPSE_BONES);

    return iter->second;
}

void ObjectAccessor::RemoveCorpse(Corpse* corpse)
{
    ASSERT(corpse && corpse->GetType() != CORPSE_BONES);

    if (corpse->FindMap())
    {
        corpse->DestroyForNearbyPlayers();
        corpse->FindMap()->RemoveFromMap(corpse, false);
    }
    else
        corpse->RemoveFromWorld();

    // Critical section
    {
        Guard guard(i_corpseGuard);

        Player2CorpsesMapType::iterator iter = i_player2corpse.find(corpse->GetOwnerGUID());
        if (iter == i_player2corpse.end())
            return;

        // build mapid*cellid -> guid_set map
        CellCoord cellCoord = Oregon::ComputeCellCoord(corpse->GetPositionX(), corpse->GetPositionY());
        sObjectMgr.DeleteCorpseCellData(corpse->GetMapId(), cellCoord.GetId(), GUID_LOPART(corpse->GetOwnerGUID()));

        i_player2corpse.erase(iter);
    }
}

void ObjectAccessor::AddCorpse(Corpse* corpse)
{
    ASSERT(corpse && corpse->GetType() != CORPSE_BONES);

    // Critical section
    {
        Guard guard(i_corpseGuard);

        ASSERT(i_player2corpse.find(corpse->GetOwnerGUID()) == i_player2corpse.end());
        i_player2corpse[corpse->GetOwnerGUID()] = corpse;

        // build mapid*cellid -> guid_set map
        CellCoord cellCoord = Oregon::ComputeCellCoord(corpse->GetPositionX(), corpse->GetPositionY());
        sObjectMgr.AddCorpseCellData(corpse->GetMapId(), cellCoord.GetId(), GUID_LOPART(corpse->GetOwnerGUID()), corpse->GetInstanceId());
    }
}

void ObjectAccessor::AddCorpsesToGrid(GridCoord const& gridpair, GridType& grid, Map* map)
{
    Guard guard(i_corpseGuard);

    for (Player2CorpsesMapType::iterator iter = i_player2corpse.begin(); iter != i_player2corpse.end(); ++iter)
    {
        if (iter->second->IsInGrid())
        {
            //TODO: add this assert later
            //ASSERT(iter->second->GetGridCoord() == gridpair);
            continue;
        }

        if (iter->second->GetGridCoord() == gridpair)
        {
            // verify, if the corpse in our instance (add only corpses which are)
            if (map->Instanceable())
            {
                if (iter->second->GetInstanceId() == map->GetInstanceId())
                    grid.AddWorldObject(iter->second);
            }
            else
                grid.AddWorldObject(iter->second);
        }
    }
}

Corpse* ObjectAccessor::ConvertCorpseForPlayer(uint64 player_guid, bool insignia)
{
    Corpse* corpse = GetCorpseForPlayerGUID(player_guid);
    if (!corpse)
    {
        //in fact this function is called from several places
        //even when player doesn't have a corpse, not an error
        //sLog.outError("Try remove corpse that not in map for GUID %ul", player_guid);
        return NULL;
    }

    float corpseX = corpse->GetPositionX();
    float corpseY = corpse->GetPositionY();
    float corpseZ = corpse->GetPositionZ();
    float corpseO = corpse->GetOrientation();

    /// @todo: Units die in various ways, for example blood elf woman lies left (-90°)
    /// but undead woman lies to the front - so we need to gather some info in order
    /// to modify corpseO properly to fit the model.

    DEBUG_LOG("Deleting Corpse and spawned bones.");

    Map* map = corpse->FindMap();

    // remove corpse from player_guid -> corpse map
    RemoveCorpse(corpse);

    // remove corpse from DB
    corpse->DeleteFromDB();

    Corpse* bones = NULL;
    // create the bones only if the map and the grid is loaded at the corpse's location
    // ignore bones creating option in case insignia
    if (map && (insignia ||
                (map->IsBattlegroundOrArena() ? sWorld.getConfig(CONFIG_DEATH_BONES_BG_OR_ARENA) : sWorld.getConfig(CONFIG_DEATH_BONES_WORLD))) &&
        !map->IsRemovalGrid(corpse->GetPositionX(), corpse->GetPositionY()))
    {
        // Create bones, don't change Corpse
        bones = new Corpse;
        bones->Create(corpse->GetGUIDLow(), map);

        for (int i = 3; i < CORPSE_END; ++i)                    // don't overwrite guid and object type
            bones->SetUInt32Value(i, corpse->GetUInt32Value(i));

        bones->SetGridCoord(corpse->GetGridCoord());
        // bones->m_time = m_time;                              // don't overwrite time
        // bones->m_inWorld = m_inWorld;                        // don't overwrite in-world state
        // bones->m_type = m_type;                              // don't overwrite type
        bones->Relocate(corpseX, corpseY, corpseZ, corpseO);

        bones->SetUInt32Value(CORPSE_FIELD_FLAGS, CORPSE_FLAG_UNK2 | CORPSE_FLAG_BONES);
        bones->SetUInt64Value(CORPSE_FIELD_OWNER, 0);

        for (int i = 0; i < EQUIPMENT_SLOT_END; ++i)
        {
            if (corpse->GetUInt32Value(CORPSE_FIELD_ITEM + i))
                bones->SetUInt32Value(CORPSE_FIELD_ITEM + i, 0);
        }

        // add bones in grid store if grid loaded where corpse placed
        map->AddToMap(bones);
    }

    // all references to the corpse should be removed at this point
    delete corpse;

    return bones;
}

void ObjectAccessor::RemoveOldCorpses()
{
    time_t now = time(NULL);
    Player2CorpsesMapType::iterator next;
    for (Player2CorpsesMapType::iterator itr = i_player2corpse.begin(); itr != i_player2corpse.end(); itr = next)
    {
        next = itr;
        ++next;

        if (!itr->second->IsExpired(now))
            continue;

        ConvertCorpseForPlayer(itr->first);
    }
}

void ObjectAccessor::Update(uint32 /*diff*/)
{
    UpdateDataMapType update_players;

    // Critical section
    {
        Guard guard(i_updateGuard);

        while (!i_objects.empty())
        {
            Object* obj = *i_objects.begin();
            ASSERT(obj && obj->IsInWorld());
            i_objects.erase(i_objects.begin());
            obj->BuildUpdate(update_players);
        }
    }

    WorldPacket packet;                                     // here we allocate a std::vector with a size of 0x10000
    for (UpdateDataMapType::iterator iter = update_players.begin(); iter != update_players.end(); ++iter)
    {
        iter->second.BuildPacket(&packet);
        iter->first->GetSession()->SendPacket(&packet);
        packet.clear();                                     // clean the string
    }
}

// Define the static members of HashMapHolder

template <class T> UNORDERED_MAP< uint64, T* > HashMapHolder<T>::m_objectMap;
template <class T> ACE_Thread_Mutex HashMapHolder<T>::i_lock;

// Global definitions for the hashmap storage

template class HashMapHolder<Player>;
template class HashMapHolder<Pet>;
template class HashMapHolder<GameObject>;
template class HashMapHolder<DynamicObject>;
template class HashMapHolder<Creature>;
template class HashMapHolder<Corpse>;

template Player* ObjectAccessor::GetObjectInWorld<Player>(uint32 mapid, float x, float y, uint64 guid, Player* /*fake*/);
template Pet* ObjectAccessor::GetObjectInWorld<Pet>(uint32 mapid, float x, float y, uint64 guid, Pet* /*fake*/);
template Creature* ObjectAccessor::GetObjectInWorld<Creature>(uint32 mapid, float x, float y, uint64 guid, Creature* /*fake*/);
template Corpse* ObjectAccessor::GetObjectInWorld<Corpse>(uint32 mapid, float x, float y, uint64 guid, Corpse* /*fake*/);
template GameObject* ObjectAccessor::GetObjectInWorld<GameObject>(uint32 mapid, float x, float y, uint64 guid, GameObject* /*fake*/);
template DynamicObject* ObjectAccessor::GetObjectInWorld<DynamicObject>(uint32 mapid, float x, float y, uint64 guid, DynamicObject* /*fake*/);

