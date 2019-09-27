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

#ifndef OREGON_GRIDNOTIFIERSIMPL_H
#define OREGON_GRIDNOTIFIERSIMPL_H

#include "GridNotifiers.h"
#include "WorldPacket.h"
#include "Corpse.h"
#include "Player.h"
#include "UpdateData.h"
#include "CreatureAI.h"
#include "SpellAuras.h"

template<class T>
inline void
Oregon::VisibleNotifier::Visit(GridRefManager<T>& m)
{

    for (typename GridRefManager<T>::iterator iter = m.begin(); iter != m.end(); ++iter)
    {
        i_player.UpdateVisibilityOf(iter->GetSource(), i_data, i_visibleNow);
        vis_guids.erase(iter->GetSource()->GetGUID());
    }
}

inline void
Oregon::ObjectUpdater::Visit(CreatureMapType& m)
{
    for (CreatureMapType::iterator iter = m.begin(); iter != m.end(); ++iter)
        if (iter->GetSource()->IsInWorld())
            iter->GetSource()->Update(i_timeDiff);
}

// SEARCHERS & LIST SEARCHERS & WORKERS

// WorldObject searchers & workers

template<class Check>
void Oregon::WorldObjectSearcher<Check>::Visit(GameObjectMapType& m)
{
    // already found
    if (i_object)
        return;

    for (GameObjectMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
    {
        if (i_check(itr->GetSource()))
        {
            i_object = itr->GetSource();
            return;
        }
    }
}

template<class Check>
void Oregon::WorldObjectSearcher<Check>::Visit(PlayerMapType& m)
{
    // already found
    if (i_object)
        return;

    for (PlayerMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
    {
        if (i_check(itr->GetSource()))
        {
            i_object = itr->GetSource();
            return;
        }
    }
}

template<class Check>
void Oregon::WorldObjectSearcher<Check>::Visit(CreatureMapType& m)
{
    // already found
    if (i_object)
        return;

    for (CreatureMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
    {
        if (i_check(itr->GetSource()))
        {
            i_object = itr->GetSource();
            return;
        }
    }
}

template<class Check>
void Oregon::WorldObjectSearcher<Check>::Visit(CorpseMapType& m)
{
    // already found
    if (i_object)
        return;

    for (CorpseMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
    {
        if (i_check(itr->GetSource()))
        {
            i_object = itr->GetSource();
            return;
        }
    }
}

template<class Check>
void Oregon::WorldObjectSearcher<Check>::Visit(DynamicObjectMapType& m)
{
    // already found
    if (i_object)
        return;

    for (DynamicObjectMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
    {
        if (i_check(itr->GetSource()))
        {
            i_object = itr->GetSource();
            return;
        }
    }
}

template<class Check>
void Oregon::WorldObjectListSearcher<Check>::Visit(PlayerMapType& m)
{
    for (PlayerMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
        if (i_check(itr->GetSource()))
            i_objects.push_back(itr->GetSource());
}

template<class Check>
void Oregon::WorldObjectListSearcher<Check>::Visit(CreatureMapType& m)
{
    for (CreatureMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
        if (i_check(itr->GetSource()))
            i_objects.push_back(itr->GetSource());
}

template<class Check>
void Oregon::WorldObjectListSearcher<Check>::Visit(CorpseMapType& m)
{
    for (CorpseMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
        if (i_check(itr->GetSource()))
            i_objects.push_back(itr->GetSource());
}

template<class Check>
void Oregon::WorldObjectListSearcher<Check>::Visit(GameObjectMapType& m)
{
    for (GameObjectMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
        if (i_check(itr->GetSource()))
            i_objects.push_back(itr->GetSource());
}

template<class Check>
void Oregon::WorldObjectListSearcher<Check>::Visit(DynamicObjectMapType& m)
{
    for (DynamicObjectMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
        if (i_check(itr->GetSource()))
            i_objects.push_back(itr->GetSource());
}

// Gameobject searchers

template<class Check>
void Oregon::GameObjectSearcher<Check>::Visit(GameObjectMapType& m)
{
    // already found
    if (i_object)
        return;

    for (GameObjectMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
    {
        if (i_check(itr->GetSource()))
        {
            i_object = itr->GetSource();
            return;
        }
    }
}

template<class Check>
void Oregon::GameObjectLastSearcher<Check>::Visit(GameObjectMapType& m)
{
    for (GameObjectMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
    {
        if (i_check(itr->GetSource()))
            i_object = itr->GetSource();
    }
}

template<class Check>
void Oregon::GameObjectListSearcher<Check>::Visit(GameObjectMapType& m)
{
    for (GameObjectMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
        if (i_check(itr->GetSource()))
            i_objects.push_back(itr->GetSource());
}

// Unit searchers

template<class Check>
void Oregon::UnitSearcher<Check>::Visit(CreatureMapType& m)
{
    // already found
    if (i_object)
        return;

    for (CreatureMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
    {
        if (i_check(itr->GetSource()))
        {
            i_object = itr->GetSource();
            return;
        }
    }
}

template<class Check>
void Oregon::UnitSearcher<Check>::Visit(PlayerMapType& m)
{
    // already found
    if (i_object)
        return;

    for (PlayerMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
    {
        if (i_check(itr->GetSource()))
        {
            i_object = itr->GetSource();
            return;
        }
    }
}

template<class Check>
void Oregon::UnitLastSearcher<Check>::Visit(CreatureMapType& m)
{
    for (CreatureMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
    {
        if (i_check(itr->GetSource()))
            i_object = itr->GetSource();
    }
}

template<class Check>
void Oregon::UnitLastSearcher<Check>::Visit(PlayerMapType& m)
{
    for (PlayerMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
    {
        if (i_check(itr->GetSource()))
            i_object = itr->GetSource();
    }
}

template<class Check>
void Oregon::UnitListSearcher<Check>::Visit(PlayerMapType& m)
{
    for (PlayerMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
        if (i_check(itr->GetSource()))
            i_objects.push_back(itr->GetSource());
}

template<class Check>
void Oregon::UnitListSearcher<Check>::Visit(CreatureMapType& m)
{
    for (CreatureMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
        if (i_check(itr->GetSource()))
            i_objects.push_back(itr->GetSource());
}

// Creature searchers

template<class Check>
void Oregon::CreatureSearcher<Check>::Visit(CreatureMapType& m)
{
    // already found
    if (i_object)
        return;

    for (CreatureMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
    {
        if (i_check(itr->GetSource()))
        {
            i_object = itr->GetSource();
            return;
        }
    }
}

template<class Check>
void Oregon::CreatureLastSearcher<Check>::Visit(CreatureMapType& m)
{
    for (CreatureMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
    {
        if (i_check(itr->GetSource()))
            i_object = itr->GetSource();
    }
}

template<class Check>
void Oregon::CreatureListSearcher<Check>::Visit(CreatureMapType& m)
{
    for (CreatureMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
        if (i_check(itr->GetSource()))
            i_objects.push_back(itr->GetSource());
}

template<class Check>
void Oregon::PlayerListSearcher<Check>::Visit(PlayerMapType& m)
{
    for (PlayerMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
        if (i_check(itr->GetSource()))
            i_objects.push_back(itr->GetSource());
}

template<class Check>
void Oregon::PlayerSearcher<Check>::Visit(PlayerMapType& m)
{
    // already found
    if (i_object)
        return;

    for (PlayerMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
    {
        if (i_check(itr->GetSource()))
        {
            i_object = itr->GetSource();
            return;
        }
    }
}

template<class Builder>
void Oregon::LocalizedPacketDo<Builder>::operator()(Player* p)
{
    uint32 loc_idx = p->GetSession()->GetSessionDbLocaleIndex();
    uint32 cache_idx = loc_idx + 1;
    WorldPacket* data;

    // create if not cached yet
    if (i_data_cache.size() < cache_idx + 1 || !i_data_cache[cache_idx])
    {
        if (i_data_cache.size() < cache_idx + 1)
            i_data_cache.resize(cache_idx + 1);

        data = new WorldPacket(SMSG_MESSAGECHAT, 200);

        i_builder(*data, loc_idx);

        i_data_cache[cache_idx] = data;
    }
    else
        data = i_data_cache[cache_idx];

    p->SendDirectMessage(data);
}

template<class Builder>
void Oregon::LocalizedPacketListDo<Builder>::operator()(Player* p)
{
    uint32 loc_idx = p->GetSession()->GetSessionDbLocaleIndex();
    uint32 cache_idx = loc_idx + 1;
    WorldPacketList* data_list;

    // create if not cached yet
    if (i_data_cache.size() < cache_idx + 1 || i_data_cache[cache_idx].empty())
    {
        if (i_data_cache.size() < cache_idx + 1)
            i_data_cache.resize(cache_idx + 1);

        data_list = &i_data_cache[cache_idx];

        i_builder(*data_list, loc_idx);
    }
    else
        data_list = &i_data_cache[cache_idx];

    for (size_t i = 0; i < data_list->size(); ++i)
        p->SendDirectMessage((*data_list)[i]);
}

#endif                                                      // OREGON_GRIDNOTIFIERSIMPL_H

