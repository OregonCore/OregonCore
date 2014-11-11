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

#include "PointMovementGenerator.h"
#include "Errors.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "MapManager.h"
#include "DestinationHolderImp.h"
#include "World.h"
#include "PathFinder.h"

//----- Point Movement Generator
template<class T>
void PointMovementGenerator<T>::Initialize(T& unit)
{
    if (!unit.IsStopped())
        unit.StopMoving();
    Traveller<T> traveller(unit);

    if (m_usePathfinding)
    {
        PathInfo path(&unit, i_x, i_y, i_z);
        PointPath pointPath = path.getFullPath();

        float speed = traveller.Speed() * 0.001f; // in ms
        uint32 traveltime = uint32(pointPath.GetTotalLength() / speed);
        if (unit.GetTypeId() != TYPEID_UNIT)
            unit.SetUnitMovementFlags(SPLINEFLAG_WALKMODE);
        unit.SendMonsterMoveByPath(pointPath, 1, pointPath.size(), traveltime);

        PathNode p = pointPath[pointPath.size() - 1];
        i_destinationHolder.SetDestination(traveller, p.x, p.y, p.z, false);
    }
    else
        i_destinationHolder.SetDestination(traveller, i_x, i_y, i_z, true);
}

template<class T>
bool PointMovementGenerator<T>::Update(T& unit, const uint32& diff)
{
    if (unit.HasUnitState(UNIT_STATE_ROOT | UNIT_STATE_STUNNED))
    {
        if (unit.HasUnitState(UNIT_STATE_CHARGING))
            return false;
        else
            return true;
    }

    Traveller<T> traveller(unit);

    i_destinationHolder.UpdateTraveller(traveller, diff);

    if (i_destinationHolder.HasArrived())
    {
        unit.ClearUnitState(UNIT_STATE_MOVE);
        arrived = true;
        return false;
    }

    return true;
}

template<class T>
void PointMovementGenerator<T>:: Finalize(T& unit)
{
    if (unit.HasUnitState(UNIT_STATE_CHARGING))
        unit.ClearUnitState(UNIT_STATE_CHARGING | UNIT_STATE_JUMPING);
    if (arrived) // without this crash!
        MovementInform(unit);
}

template<class T>
void PointMovementGenerator<T>::MovementInform(T& /*unit*/)
{
}

template <> void PointMovementGenerator<Creature>::MovementInform(Creature& unit)
{
    if (id == EVENT_FALL_GROUND)
    {
        unit.setDeathState(JUST_DIED);
        unit.SetFlying(true);
    }
    unit.AI()->MovementInform(POINT_MOTION_TYPE, id);
}

template void PointMovementGenerator<Player>::Initialize(Player&);
template bool PointMovementGenerator<Player>::Update(Player&, const uint32& diff);
template void PointMovementGenerator<Player>::MovementInform(Player&);
template void PointMovementGenerator<Player>::Finalize(Player&);

template void PointMovementGenerator<Creature>::Initialize(Creature&);
template bool PointMovementGenerator<Creature>::Update(Creature&, const uint32& diff);
template void PointMovementGenerator<Creature>::Finalize(Creature&);

void AssistanceMovementGenerator::Finalize(Creature& unit)
{
    unit.SetNoCallAssistance(false);
    unit.CallAssistance();
    if (unit.IsAlive())
        unit.GetMotionMaster()->MoveSeekAssistanceDistract(sWorld.getConfig(CONFIG_CREATURE_FAMILY_ASSISTANCE_DELAY));
}

