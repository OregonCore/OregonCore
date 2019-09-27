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

#include "GuardAI.h"
#include "Errors.h"
#include "Player.h"
#include "ObjectAccessor.h"

int GuardAI::Permissible(const Creature* creature)
{
    if (creature->IsGuard())
        return PERMIT_BASE_SPECIAL;

    return PERMIT_BASE_NO;
}

GuardAI::GuardAI(Creature* creature) : ScriptedAI(creature), i_victimGuid(0), i_state(STATE_NORMAL), i_tracker(TIME_INTERVAL_LOOK)
{
}

bool GuardAI::CanSeeAlways(WorldObject const* obj)
{
    if (!obj->isType(TYPEMASK_UNIT))
        return false;

    ThreatContainer::StorageType threatList = me->getThreatManager().getThreatList();
    for (ThreatContainer::StorageType::const_iterator itr = threatList.begin(); itr != threatList.end(); ++itr)
        if ((*itr)->getUnitGuid() == obj->GetGUID())
            return true;

    return false;
}

void GuardAI::EnterEvadeMode()
{
    if (!me->IsAlive())
    {
        me->GetMotionMaster()->MoveIdle();
        me->CombatStop(true);
        me->DeleteThreatList();
        return;
    }

    Unit* victim = ObjectAccessor::GetUnit(*me, i_victimGuid);

    if (!victim)
        DEBUG_LOG("Creature stopped attacking because victim is non exist [guid=%u]", me->GetGUIDLow());
    else if (!victim ->IsAlive())
        DEBUG_LOG("Creature stopped attacking because victim is dead [guid=%u]", me->GetGUIDLow());
    else if (victim ->HasStealthAura())
        DEBUG_LOG("Creature stopped attacking because victim is using stealth [guid=%u]", me->GetGUIDLow());
    else if (victim ->IsInFlight())
        DEBUG_LOG("Creature stopped attacking because victim is flying away [guid=%u]", me->GetGUIDLow());
    else
        DEBUG_LOG("Creature stopped attacking because victim outran him [guid=%u]", me->GetGUIDLow());

    me->RemoveAllAuras();
    me->DeleteThreatList();
    me->CombatStop(true);

    // Remove TargetedMovementGenerator from MotionMaster stack list, and add HomeMovementGenerator instead
    if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() & TARGETED_MOTION_TYPE)
        me->GetMotionMaster()->MoveTargetedHome();
}

void GuardAI::JustDied(Unit* killer)
{
    if (Player* player = killer->GetCharmerOrOwnerPlayerOrPlayerItself())
        me->SendZoneUnderAttackMessage(player);
}

