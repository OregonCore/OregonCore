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

#include "ObjectGridLoader.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Creature.h"
#include "GameObject.h"
#include "DynamicObject.h"
#include "Corpse.h"
#include "World.h"
#include "CellImpl.h"
#include "CreatureAI.h"

void ObjectGridEvacuator::Visit(CreatureMapType &m)
{
    // creature in unloading grid can have respawn point in another grid
    // if it will be unloaded then it will not respawn in original grid until unload/load original grid
    // move to respawn point to prevent this case. For player view in respawn grid this will be normal respawn.
    for (CreatureMapType::iterator iter = m.begin(); iter != m.end();)
    {
        Creature* c = iter->GetSource();
        ++iter;

        ASSERT(!c->IsPet() && "ObjectGridRespawnMover don't must be called for pets");

        Cell const& cur_cell  = c->GetCurrentCell();

        float resp_x, resp_y, resp_z;
        c->GetRespawnPosition(resp_x, resp_y, resp_z);
        CellCoord resp_val = Oregon::ComputeCellCoord(resp_x, resp_y);
        Cell resp_cell(resp_val);

        if (cur_cell.DiffGrid(resp_cell))
        {
            c->GetMap()->CreatureRespawnRelocation(c);
            // false result ignored: will be unload with other creatures at grid
        }
    }
}

// for loading world object at grid loading (Corpses)
class ObjectWorldLoader
{
    public:
        explicit ObjectWorldLoader(ObjectGridLoader& gloader)
            : i_cell(gloader.i_cell), i_map(gloader.i_map), i_corpses (0)
        {}

        void Visit(CorpseMapType& m);

        template<class T> void Visit(GridRefManager<T>&) { }

    private:
        Cell i_cell;
        Map* i_map;
    public:
        uint32 i_corpses;
};

template<class T> void AddUnitState(T* /*obj*/, CellCoord const& /*cellCoord*/)
{
}

template<> void AddUnitState(Creature* obj, CellCoord const& cellCoord)
{
    Cell cell(cellCoord);

    obj->SetCurrentCell(cell);
}

template <class T>
void AddObjectHelper(CellCoord& cell, GridRefManager<T>& m, uint32& count, Map* map, T* obj)
{
    obj->AddToGrid(m);
    AddUnitState(obj, cell);
    obj->AddToWorld();
    if (obj->isActiveObject())
        map->AddToActive(obj);

    ++count;
}

template <class T>
void LoadHelper(CellGuidSet const& guid_set, CellCoord& cell, GridRefManager<T>& m, uint32& count, Map* map)
{
    for (CellGuidSet::const_iterator i_guid = guid_set.begin(); i_guid != guid_set.end(); ++i_guid)
    {
        T* obj = new T;
        uint32 guid = *i_guid;
        //sLog.outString("DEBUG: LoadHelper from table: %s for (guid: %u) Loading",table,guid);
        if (!obj->LoadFromDB(guid, map))
        {
            delete obj;
            continue;
        }

        AddObjectHelper(cell, m, count, map, obj);
    }
}

void LoadHelper(CellCorpseSet const& cell_corpses, CellCoord& cell, CorpseMapType& m, uint32& count, Map* map)
{
    if (cell_corpses.empty())
        return;

    for (CellCorpseSet::const_iterator itr = cell_corpses.begin(); itr != cell_corpses.end(); ++itr)
    {
        if (itr->second != map->GetInstanceId())
            continue;

        uint32 player_guid = itr->first;

        Corpse* obj = ObjectAccessor::Instance().GetCorpseForPlayerGUID(player_guid);
        if (!obj)
            continue;

        // @todo this is a hack
        // corpse's map should be reset when the map is unloaded
        // but it may still exist when the grid is unloaded but map is not
        // in that case map == currMap
        obj->SetMap(map);

        if (obj->IsInGrid())
        {
            obj->AddToWorld();
            continue;
        }

        AddObjectHelper(cell, m, count, map, obj);
    }
}

void ObjectGridLoader::Visit(GameObjectMapType& m)
{
    CellCoord cellCoord = i_cell.GetCellCoord();
    CellObjectGuids const& cell_guids = sObjectMgr.GetCellObjectGuids(i_map->GetId(), i_map->GetSpawnMode(), cellCoord.GetId());
    LoadHelper(cell_guids.gameobjects, cellCoord, m, i_gameObjects, i_map);
}

void ObjectGridLoader::Visit(CreatureMapType& m)
{
    CellCoord cellCoord = i_cell.GetCellCoord();
    CellObjectGuids const& cell_guids = sObjectMgr.GetCellObjectGuids(i_map->GetId(), i_map->GetSpawnMode(), cellCoord.GetId());
    LoadHelper(cell_guids.creatures, cellCoord, m, i_creatures, i_map);
}

void ObjectWorldLoader::Visit(CorpseMapType& m)
{
    CellCoord cellCoord = i_cell.GetCellCoord();
    // corpses are always added to spawn mode 0 and they are spawned by their instance id
    CellObjectGuids const& cell_guids = sObjectMgr.GetCellObjectGuids(i_map->GetId(), 0, cellCoord.GetId());
    LoadHelper(cell_guids.corpses, cellCoord, m, i_corpses, i_map);
}

void ObjectGridLoader::LoadN(void)
{
    i_gameObjects = 0;
    i_creatures = 0;
    i_corpses = 0;
    i_cell.data.Part.cell_y = 0;
    for (unsigned int x = 0; x < MAX_NUMBER_OF_CELLS; ++x)
    {
        i_cell.data.Part.cell_x = x;
        for (unsigned int y = 0; y < MAX_NUMBER_OF_CELLS; ++y)
        {
            i_cell.data.Part.cell_y = y;
            //Load creatures and game objects
            {
                TypeContainerVisitor<ObjectGridLoader, GridTypeMapContainer> visitor(*this);
                i_grid.VisitGrid(x, y, visitor);
            }

            //Load corpses (not bones)
            {
                ObjectWorldLoader worker(*this);
                TypeContainerVisitor<ObjectWorldLoader, WorldTypeMapContainer> visitor(worker);
                i_grid.VisitGrid(x, y, visitor);
                i_corpses += worker.i_corpses;
            }
        }
    }
    sLog.outDebug("%u GameObjects, %u Creatures, and %u Corpses/Bones loaded for grid %u on map %u", i_gameObjects, i_creatures, i_corpses, i_grid.GetGridId(), i_map->GetId());
}

template<class T>
void ObjectGridUnloader::Visit(GridRefManager<T> &m)
{
    while (!m.isEmpty())
    {
        T* obj = m.getFirst()->GetSource();
        // if option set then object already saved at this moment
        if (!sWorld.getConfig(CONFIG_SAVE_RESPAWN_TIME_IMMEDIATELY))
            obj->SaveRespawnTime();
        //Some creatures may summon other temp summons in CleanupsBeforeDelete()
        //So we need this even after cleaner (maybe we can remove cleaner)
        obj->CleanupsBeforeDelete();
        ///- object will get delinked from the manager when deleted
        delete obj;
    }
}

void ObjectGridStoper::Visit(CreatureMapType &m)
{
    // stop any fights at grid de-activation and remove dynobjects created at cast by creatures
    for (CreatureMapType::iterator iter = m.begin(); iter != m.end(); ++iter)
    {
        iter->GetSource()->RemoveAllDynObjects();
        if (iter->GetSource()->IsInCombat() || !iter->GetSource()->getThreatManager().areThreatListsEmpty())
        {
            iter->GetSource()->CombatStop();
            iter->GetSource()->DeleteThreatList();
            if (iter->GetSource()->IsAIEnabled)
                iter->GetSource()->AI()->EnterEvadeMode();
        }
    }
}

template<class T>
void ObjectGridCleaner::Visit(GridRefManager<T> &m)
{
    for (typename GridRefManager<T>::iterator iter = m.begin(); iter != m.end(); ++iter)
        iter->GetSource()->CleanupsBeforeDelete();
}

template void ObjectGridUnloader::Visit(CreatureMapType&);
template void ObjectGridUnloader::Visit(GameObjectMapType&);
template void ObjectGridUnloader::Visit(DynamicObjectMapType&);
template void ObjectGridUnloader::Visit(CorpseMapType&);
template void ObjectGridCleaner::Visit(CreatureMapType&);
template void ObjectGridCleaner::Visit<GameObject>(GameObjectMapType&);
template void ObjectGridCleaner::Visit<DynamicObject>(DynamicObjectMapType&);
template void ObjectGridCleaner::Visit<Corpse>(CorpseMapType&);

