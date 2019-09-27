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

#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "UpdateData.h"
#include "MapManager.h"
#include "Transports.h"
#include "ObjectAccessor.h"
#include "CellImpl.h"
#include "SpellAuras.h"

using namespace Oregon;

void
VisibleNotifier::SendToSelf()
{
    // at this moment i_clientGUIDs have guids that not iterate at grid level checks
    // but exist one case when this possible and object not out of range: transports
    if (Transport* transport = i_player.GetTransport())
        for (Transport::PlayerSet::const_iterator itr = transport->GetPassengers().begin(); itr != transport->GetPassengers().end(); ++itr)
        {
            if (vis_guids.find((*itr)->GetGUID()) != vis_guids.end())
            {
                vis_guids.erase((*itr)->GetGUID());

                switch ((*itr)->GetTypeId())
                {
                case TYPEID_GAMEOBJECT:
                    i_player.UpdateVisibilityOf((*itr)->ToGameObject(), i_data, i_visibleNow);
                    break;
                case TYPEID_PLAYER:
                    i_player.UpdateVisibilityOf((*itr)->ToPlayer(), i_data, i_visibleNow);
                    (*itr)->ToPlayer()->UpdateVisibilityOf(&i_player);
                    break;
                case TYPEID_UNIT:
                    i_player.UpdateVisibilityOf((*itr)->ToCreature(), i_data, i_visibleNow);
                    break;
                } 
            }
        }

    for (Player::ClientGUIDs::const_iterator it = vis_guids.begin(); it != vis_guids.end(); ++it)
    {
        i_player.m_clientGUIDs.erase(*it);
        i_data.AddOutOfRangeGUID(*it);

        if (IS_PLAYER_GUID(*it))
        {
            Player* plr = ObjectAccessor::FindPlayer(*it, true);
            if (plr && !plr->isNeedNotify(NOTIFY_VISIBILITY_CHANGED))
                plr->UpdateVisibilityOf(&i_player);
        }
    }

    if (!i_data.HasData())
        return;

    WorldPacket packet;
    i_data.BuildPacket(&packet);
    i_player.GetSession()->SendPacket(&packet);

    for (std::set<Unit*>::const_iterator it = i_visibleNow.begin(); it != i_visibleNow.end(); ++it)
        i_player.SendAuraDurationsForTarget(*it);
}

void
VisibleChangesNotifier::Visit(PlayerMapType& m)
{
    for (PlayerMapType::iterator iter = m.begin(); iter != m.end(); ++iter)
    {
        if (iter->GetSource() == &i_object)
            continue;

        iter->GetSource()->UpdateVisibilityOf(&i_object);

        if (iter->GetSource()->HasSharedVision())
            for (SharedVisionList::const_iterator i = iter->GetSource()->GetSharedVisionList().begin();
                i != iter->GetSource()->GetSharedVisionList().end(); ++i)
                if ((*i)->m_seer == iter->GetSource())
                    (*i)->UpdateVisibilityOf(&i_object);
    }
}

void
VisibleChangesNotifier::Visit(CreatureMapType& m)
{
    for (CreatureMapType::iterator iter = m.begin(); iter != m.end(); ++iter)
        if (!iter->GetSource()->GetSharedVisionList().empty())
            for (SharedVisionList::const_iterator i = iter->GetSource()->GetSharedVisionList().begin();
                 i != iter->GetSource()->GetSharedVisionList().end(); ++i)
                if ((*i)->m_seer == iter->GetSource())
                    (*i)->UpdateVisibilityOf(&i_object);
}

void
VisibleChangesNotifier::Visit(DynamicObjectMapType& m)
{
    for (DynamicObjectMapType::iterator iter = m.begin(); iter != m.end(); ++iter)
        if (iter->GetSource()->GetTypeId() == TYPEID_PLAYER) // better check to see if the target it a player.
            if (Player* caster = (Player*)iter->GetSource()->GetCaster())
                if (caster->m_seer == iter->GetSource())
                    caster->UpdateVisibilityOf(&i_object);
}

inline void CreatureUnitRelocationWorker(Creature* c, Unit* u)
{
    if (!u->IsAlive() || !c->IsAlive() || c == u || u->IsInFlight())
        return;

    if (c->HasReactState(REACT_AGGRESSIVE) && !c->HasUnitState(UNIT_STATE_SIGHTLESS))
        if (c->IsAIEnabled && c->CanSeeOrDetect(u, false, true))
            c->AI()->MoveInLineOfSight_Safe(u);
        else
            if (u->GetTypeId() == TYPEID_PLAYER && u->HasStealthAura() && c->IsAIEnabled && c->CanSeeOrDetect(u, false, true, true))
                c->AI()->TriggerAlert(u);
}

void PlayerRelocationNotifier::Visit(PlayerMapType& m)
{
    for (PlayerMapType::iterator iter = m.begin(); iter != m.end(); ++iter)
    {
        Player* plr = iter->GetSource();

        vis_guids.erase(plr->GetGUID());

        i_player.UpdateVisibilityOf(plr, i_data, i_visibleNow);

        if (plr->m_seer->isNeedNotify(NOTIFY_VISIBILITY_CHANGED))
            continue;

        plr->UpdateVisibilityOf(&i_player);
    }
}

void PlayerRelocationNotifier::Visit(CreatureMapType& m)
{
    bool relocated_for_ai = (&i_player == i_player.m_seer);

    for (CreatureMapType::iterator iter = m.begin(); iter != m.end(); ++iter)
    {
        Creature* c = iter->GetSource();

        vis_guids.erase(c->GetGUID());

        i_player.UpdateVisibilityOf(c, i_data, i_visibleNow);

        if (relocated_for_ai && !c->isNeedNotify(NOTIFY_VISIBILITY_CHANGED))
            CreatureUnitRelocationWorker(c, &i_player);
    }
}

void CreatureRelocationNotifier::Visit(PlayerMapType& m)
{
    for (PlayerMapType::iterator iter = m.begin(); iter != m.end(); ++iter)
    {
        Player* pl = iter->GetSource();

        if (!pl->m_seer->isNeedNotify(NOTIFY_VISIBILITY_CHANGED))
            pl->UpdateVisibilityOf(&i_creature);

        CreatureUnitRelocationWorker(&i_creature, pl);
    }
}

void CreatureRelocationNotifier::Visit(CreatureMapType& m)
{
    if (!i_creature.IsAlive())
        return;

    for (CreatureMapType::iterator iter = m.begin(); iter != m.end(); ++iter)
    {
        Creature* c = iter->GetSource();

        //check distance to improve performance
        if (!i_creature._IsWithinDist(c, i_radius, true))
            continue;

        CreatureUnitRelocationWorker(&i_creature, c);

        if (!c->isNeedNotify(NOTIFY_VISIBILITY_CHANGED))
            CreatureUnitRelocationWorker(c, &i_creature);
    }
}

void DelayedUnitRelocation::Visit(CreatureMapType& m)
{
    for (CreatureMapType::iterator iter = m.begin(); iter != m.end(); ++iter)
    {
        Creature* unit = iter->GetSource();
        if (!unit->isNeedNotify(NOTIFY_VISIBILITY_CHANGED))
            continue;

        CreatureRelocationNotifier relocate(*unit, i_radius);

        TypeContainerVisitor<CreatureRelocationNotifier, WorldTypeMapContainer > c2world_relocation(relocate);
        TypeContainerVisitor<CreatureRelocationNotifier, GridTypeMapContainer >  c2grid_relocation(relocate);

        cell.Visit(p, c2world_relocation, i_map, *unit, i_radius);
        cell.Visit(p, c2grid_relocation, i_map, *unit, i_radius);
    }
}

void DelayedUnitRelocation::Visit(PlayerMapType& m)
{
    for (PlayerMapType::iterator iter = m.begin(); iter != m.end(); ++iter)
    {
        Player* player = iter->GetSource();
        WorldObject const* viewPoint = player->m_seer;

        if (!viewPoint->isNeedNotify(NOTIFY_VISIBILITY_CHANGED))
            continue;

        if (player != viewPoint && !viewPoint->IsPositionValid())
            continue;

        CellCoord pair2(Oregon::ComputeCellCoord(viewPoint->GetPositionX(), viewPoint->GetPositionY()));
        Cell cell2(pair2);
        //cell.SetNoCreate(); need load cells around viewPoint or player, that's why its commented

        PlayerRelocationNotifier relocate(*player);
        TypeContainerVisitor<PlayerRelocationNotifier, WorldTypeMapContainer > c2world_relocation(relocate);
        TypeContainerVisitor<PlayerRelocationNotifier, GridTypeMapContainer >  c2grid_relocation(relocate);

        cell2.Visit(pair2, c2world_relocation, i_map, *viewPoint, i_radius);
        cell2.Visit(pair2, c2grid_relocation, i_map, *viewPoint, i_radius);

        relocate.SendToSelf();
    }
}

void AIRelocationNotifier::Visit(CreatureMapType& m)
{
    for (CreatureMapType::iterator iter = m.begin(); iter != m.end(); ++iter)
    {
        Creature* c = iter->GetSource();
        CreatureUnitRelocationWorker(c, &i_unit);
        if (isCreature)
            CreatureUnitRelocationWorker((Creature*)&i_unit, c);
    }
}

void DynamicObjectUpdater::VisitHelper(Unit* target)
{
    if (!target->IsAlive() || target->IsInFlight())
        return;

    if (target->GetTypeId() == TYPEID_UNIT && ((Creature*)target)->IsTotem())
        return;

    if (!i_dynobject.IsWithinDistInMap(target, i_dynobject.GetRadius()))
        return;

    if (!i_dynobject.IsWithinLOSInMap(target))
        return;

    //Check targets for not_selectable unit flag and remove
    if (target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC))
        return;

    // Evade target
    if (target->GetTypeId() == TYPEID_UNIT && ((Creature*)target)->IsInEvadeMode())
        return;

    //Check player targets and remove if in GM mode or GM invisibility (for not self casting case)
    if (target->GetTypeId() == TYPEID_PLAYER && target != i_check && (((Player*)target)->IsGameMaster() || !((Player*)target)->IsVisible()))
        return;

    if (i_dynobject.IsAffecting(target))
        return;

    SpellEntry const* spellInfo = sSpellStore.LookupEntry(i_dynobject.GetSpellId());
    uint32 eff_index  = i_dynobject.GetEffIndex();

    if (spellInfo->EffectImplicitTargetB[eff_index] == TARGET_DEST_DYNOBJ_ALLY
        || spellInfo->EffectImplicitTargetB[eff_index] == TARGET_UNIT_AREA_ALLY_DST)
    {
        if (!i_check->IsFriendlyTo(target))
            return;
    }
    else if (i_check->GetTypeId() == TYPEID_PLAYER)
    {
        if (i_check->IsFriendlyTo(target))
            return;
    }
    else
    {
        if (!i_check->IsHostileTo(target))
            return;
    }

    // Check target immune to spell or aura
    if (target->IsImmuneToSpell(spellInfo) || target->IsImmuneToSpellEffect(spellInfo, eff_index, false))
        return;

    // Apply PersistentAreaAura on target
    PersistentAreaAura* Aur = new PersistentAreaAura(spellInfo, eff_index, NULL, target, i_dynobject.GetCaster());
    target->AddAura(Aur);
    i_dynobject.AddAffected(target);
}

void DynamicObjectUpdater::Visit(CreatureMapType& m)
{
    for (CreatureMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
        VisitHelper(itr->GetSource());
}

void DynamicObjectUpdater::Visit(PlayerMapType& m)
{
    for (PlayerMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
        VisitHelper(itr->GetSource());
}

void
MessageDistDeliverer::Visit(PlayerMapType& m)
{
    for (PlayerMapType::iterator iter = m.begin(); iter != m.end(); ++iter)
    {
        Player* target = iter->GetSource();

        if (target->GetExactDist2dSq(i_source) > i_distSq)
            continue;

        // Send packet to all who are sharing the player's vision
        if (!target->GetSharedVisionList().empty())
        {
            SharedVisionList::const_iterator i = target->GetSharedVisionList().begin();
            for (; i != target->GetSharedVisionList().end(); ++i)
                if ((*i)->m_seer == target)
                    SendPacket(*i);
        }

        if (target->m_seer == target)
            SendPacket(target);
    }
}

void
MessageDistDeliverer::Visit(CreatureMapType& m)
{
    for (CreatureMapType::iterator iter = m.begin(); iter != m.end(); ++iter)
    {
        if (iter->GetSource()->GetExactDist2dSq(i_source) > i_distSq)
            continue;

        // Send packet to all who are sharing the creature's vision
        if (!iter->GetSource()->GetSharedVisionList().empty())
        {
            SharedVisionList::const_iterator i = iter->GetSource()->GetSharedVisionList().begin();
            for (; i != iter->GetSource()->GetSharedVisionList().end(); ++i)
                if ((*i)->m_seer == iter->GetSource())
                    SendPacket(*i);
        }
    }
}

void
MessageDistDeliverer::Visit(DynamicObjectMapType& m)
{
    for (DynamicObjectMapType::iterator iter = m.begin(); iter != m.end(); ++iter)
    {
        if (iter->GetSource()->GetExactDist2dSq(i_source) > i_distSq)
            continue;

        if (iter->GetSource()->GetTypeId() == TYPEID_PLAYER)
        {
            // Send packet back to the caster if the caster has vision of dynamic object
            Player* caster = iter->GetSource()->GetCaster()->ToPlayer();
            if (caster && caster->m_seer == iter->GetSource())
                SendPacket(caster);
        }
    }
}

/*
void
MessageDistDeliverer::VisitObject(Player* plr)
{
    if (!i_ownTeamOnly || (i_source.GetTypeId() == TYPEID_PLAYER && plr->GetTeam() == ((Player&)i_source).GetTeam()))
    {
        SendPacket(plr);
    }
}
*/

template<class T> void
ObjectUpdater::Visit(GridRefManager<T>& m)
{
    for (typename GridRefManager<T>::iterator iter = m.begin(); iter != m.end(); ++iter)
    {
        if (iter->GetSource()->IsInWorld())
            iter->GetSource()->Update(i_timeDiff);
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

template void ObjectUpdater::Visit<GameObject>(GameObjectMapType&);
template void ObjectUpdater::Visit<DynamicObject>(DynamicObjectMapType&);

