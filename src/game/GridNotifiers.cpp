/*
 * Copyright (C) 2005-2008 MaNGOS <http://www.mangosproject.org/>
 *
 * Copyright (C) 2008 Trinity <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "GridNotifiers.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "UpdateData.h"
#include "Item.h"
#include "Map.h"
#include "MapManager.h"
#include "Transports.h"
#include "ObjectAccessor.h"

using namespace Oregon;

void
Player2PlayerNotifier::Visit(PlayerMapType &m)
{
    for (PlayerMapType::iterator iter=m.begin(); iter != m.end(); ++iter)
    {
        Player* plr = iter->getSource();
        if (plr == &i_player)
            continue;

        vis_guids.erase(plr->GetGUID());
        i_player.UpdateVisibilityOf(plr,i_data,i_visibleNow);

        // force == true - update i_player visibility for all player in cell, ignore optimization flags.
        if (!force && (plr->NotifyExecuted(NOTIFY_PLAYER_VISIBILITY) || plr->isNeedNotify(NOTIFY_PLAYER_VISIBILITY)))
            continue;

        plr->UpdateVisibilityOf(&i_player);
    }
}

void
VisibleNotifier::SendToSelf()
{
    for (Player::ClientGUIDs::const_iterator it = vis_guids.begin();it != vis_guids.end();++it)
    {   //player guids processed in Player2PlayerNotifier
        if (IS_PLAYER_GUID(*it))
            continue;

        i_player.m_clientGUIDs.erase(*it);
        i_data.AddOutOfRangeGUID(*it);
    }

    if (!i_data.HasData())
        return;

    WorldPacket packet;
    i_data.BuildPacket(&packet);
    i_player.GetSession()->SendPacket(&packet);

    for (std::set<Unit*>::const_iterator it = i_visibleNow.begin(); it != i_visibleNow.end(); ++it)
        i_player.SendInitialVisiblePackets(*it);
}

void
Player2PlayerNotifier::SendToSelf()
{
    // at this moment i_clientGUIDs have guids that not iterate at grid level checks
    // but exist one case when this possible and object not out of range: transports
    if (Transport* transport = i_player.GetTransport())
        for (Transport::PlayerSet::const_iterator itr = transport->GetPassengers().begin();itr!=transport->GetPassengers().end();++itr)
        {
            if (vis_guids.find((*itr)->GetGUID()) != vis_guids.end())
            {
                vis_guids.erase((*itr)->GetGUID());

                i_player.UpdateVisibilityOf((*itr), i_data, i_visibleNow);

                if (!(*itr)->isNeedNotify(NOTIFY_PLAYER_VISIBILITY))
                    (*itr)->UpdateVisibilityOf(&i_player);
            }
        }

    for (Player::ClientGUIDs::const_iterator it = vis_guids.begin();it != vis_guids.end(); ++it)
    {
        //since its player-player notifier we work only with player guids
        if (!IS_PLAYER_GUID(*it))
            continue;

        i_player.m_clientGUIDs.erase(*it);
        i_data.AddOutOfRangeGUID(*it);
        Player* plr = ObjectAccessor::FindPlayer(*it);
        if (plr && plr->IsInWorld() && !plr->NotifyExecuted(NOTIFY_PLAYER_VISIBILITY) && !plr->isNeedNotify(NOTIFY_PLAYER_VISIBILITY))
            plr->UpdateVisibilityOf(&i_player);
    }

    if (!i_data.HasData())
        return;

    WorldPacket packet;
    i_data.BuildPacket(&packet);
    i_player.GetSession()->SendPacket(&packet);

    for (std::set<Unit*>::const_iterator it = i_visibleNow.begin(); it != i_visibleNow.end(); ++it)
        i_player.SendInitialVisiblePackets(*it);
}

void
VisibleChangesNotifier::Visit(PlayerMapType &m)
{
    for (PlayerMapType::iterator iter=m.begin(); iter != m.end(); ++iter)
    {
        if (iter->getSource() == &i_object)
            continue;

        iter->getSource()->UpdateVisibilityOf(&i_object);

        if (!iter->getSource()->GetSharedVisionList().empty())
            for (SharedVisionList::const_iterator i = iter->getSource()->GetSharedVisionList().begin();
                i != iter->getSource()->GetSharedVisionList().end(); ++i)
                    (*i)->UpdateVisibilityOf(&i_object);
    }
}

void
Deliverer::Visit(PlayerMapType &m)
{
    for (PlayerMapType::iterator iter = m.begin(); iter != m.end(); ++iter)
    {
        if (!i_dist || iter->getSource()->GetDistance(&i_source) <= i_dist)
        {
            // Send packet to all who are sharing the player's vision
            if (!iter->getSource()->GetSharedVisionList().empty())
            {
                SharedVisionList::const_iterator it = iter->getSource()->GetSharedVisionList().begin();
                for (; it != iter->getSource()->GetSharedVisionList().end(); ++it)
                    SendPacket(*it);
            }

            VisitObject(iter->getSource());
        }
    }
}

void
Deliverer::Visit(CreatureMapType &m)
{
    for (CreatureMapType::iterator iter = m.begin(); iter != m.end(); ++iter)
    {
        if (!i_dist || iter->getSource()->GetDistance(&i_source) <= i_dist)
        {
            // Send packet to all who are sharing the creature's vision
            if (!iter->getSource()->GetSharedVisionList().empty())
            {
                SharedVisionList::const_iterator it = iter->getSource()->GetSharedVisionList().begin();
                for (; it != iter->getSource()->GetSharedVisionList().end(); ++it)
                    SendPacket(*it);
            }
        }
    }
}

void
Deliverer::Visit(DynamicObjectMapType &m)
{
    for (DynamicObjectMapType::iterator iter = m.begin(); iter != m.end(); ++iter)
    {
        if (IS_PLAYER_GUID(iter->getSource()->GetCasterGUID()))
        {
            // Send packet back to the caster if the caster has vision of dynamic object
            Player* caster = iter->getSource()->GetCaster()->ToPlayer();
            if (caster && caster->GetUInt64Value(PLAYER_FARSIGHT) == iter->getSource()->GetGUID() &&
                (!i_dist || iter->getSource()->GetDistance(&i_source) <= i_dist))
                SendPacket(caster);
        }
    }
}

void
Deliverer::SendPacket(Player* plr)
{
    if (!plr)
        return;

    // Don't send the packet to self if not supposed to
    if (!i_toSelf && plr == &i_source)
        return;

    // Don't send the packet to possesor if not supposed to
    if (!i_toPossessor && plr->isPossessing() && plr->GetCharmGUID() == i_source.GetGUID())
        return;

    if (plr_list.find(plr->GetGUID()) == plr_list.end())
    {
        if (WorldSession* session = plr->GetSession())
            session->SendPacket(i_message);
        plr_list.insert(plr->GetGUID());
    }
}

void
MessageDeliverer::VisitObject(Player* plr)
{
    SendPacket(plr);
}

void
MessageDistDeliverer::VisitObject(Player* plr)
{
    if (!i_ownTeamOnly || (i_source.GetTypeId() == TYPEID_PLAYER && plr->GetTeam() == ((Player&)i_source).GetTeam()))
    {
        SendPacket(plr);
    }
}

template<class T> void
ObjectUpdater::Visit(GridRefManager<T> &m)
{
    for (typename GridRefManager<T>::iterator iter = m.begin(); iter != m.end(); ++iter)
    {
        if (iter->getSource()->IsInWorld())
            iter->getSource()->Update(i_timeDiff);
    }
}

bool CannibalizeObjectCheck::operator()(Corpse* u)
{
    // ignore bones
    if (u->GetType() == CORPSE_BONES)
        return false;

    Player* owner = ObjectAccessor::FindPlayer(u->GetOwnerGUID());

    if (!owner || i_funit->IsFriendlyTo(owner))
        return false;

    if (i_funit->IsWithinDistInMap(u, i_range))
        return true;

    return false;
}

template void ObjectUpdater::Visit<GameObject>(GameObjectMapType &);
template void ObjectUpdater::Visit<DynamicObject>(DynamicObjectMapType &);
