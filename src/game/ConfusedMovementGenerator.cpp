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

#include "Creature.h"
#include "MapManager.h"
#include "Opcodes.h"
#include "ConfusedMovementGenerator.h"
#include "VMapFactory.h"
#include "MoveSplineInit.h"
#include "MoveSpline.h"

template<class T>
void ConfusedMovementGenerator<T>::Initialize(T& unit)
{
    float const wanderDistance = 4;
    float x, y, z;
    x = unit.GetPositionX();
    y = unit.GetPositionY();
    z = unit.GetPositionZ();

    Map const* map = unit.GetBaseMap();

    i_nextMove = 1;

    bool is_water_ok, is_land_ok;
    _InitSpecific(unit, is_water_ok, is_land_ok);

    VMAP::IVMapManager* vMaps = VMAP::VMapFactory::createOrGetVMapManager();

    for (uint8 idx = 0; idx <= MAX_CONF_WAYPOINTS; ++idx)
    {
        float wanderX = x + wanderDistance * (float)rand_norm() - wanderDistance / 2;
        float wanderY = y + wanderDistance * (float)rand_norm() - wanderDistance / 2;
        Oregon::NormalizeMapCoord(wanderX);
        Oregon::NormalizeMapCoord(wanderY);

        float new_z = map->GetHeight(wanderX, wanderY, z, true);
        if (new_z > INVALID_HEIGHT && unit.IsWithinLOS(wanderX, wanderY, new_z))
        {
            // Don't move in water if we're not already in
            // Don't move on land if we're not already on it either
            bool is_water_now = map->IsInWater(x, y, z);
            bool is_water_next = map->IsInWater(wanderX, wanderY, new_z);
            if ((is_water_now && !is_water_next && !is_land_ok) || (!is_water_now && is_water_next && !is_water_ok))
            {
                i_waypoints[idx][0] = idx > 0 ? i_waypoints[idx - 1][0] : x; // Back to previous location
                i_waypoints[idx][1] = idx > 0 ? i_waypoints[idx - 1][1] : y;
                i_waypoints[idx][2] = idx > 0 ? i_waypoints[idx - 1][2] : z;
                continue;
            }

            i_waypoints[idx][0] = wanderX;
            i_waypoints[idx][1] = wanderY;
            i_waypoints[idx][2] = new_z;

            // prevent falling down over an edge and check vmap if possible
            if (z > i_waypoints[idx][2] + 3.0f ||
                (vMaps && !vMaps->isInLineOfSight(map->GetId(), x, y, z + 2.0f, i_waypoints[idx][0], i_waypoints[idx][1], i_waypoints[idx][2])))
            {
                i_waypoints[idx][0] = idx > 0 ? i_waypoints[idx - 1][0] : x;
                i_waypoints[idx][1] = idx > 0 ? i_waypoints[idx - 1][1] : y;
                i_waypoints[idx][2] = idx > 0 ? i_waypoints[idx - 1][2] : z;
            }
        }
        else    // Back to previous location
        {
            i_waypoints[idx][0] = idx > 0 ? i_waypoints[idx - 1][0] : x;
            i_waypoints[idx][1] = idx > 0 ? i_waypoints[idx - 1][1] : y;
            i_waypoints[idx][2] = idx > 0 ? i_waypoints[idx - 1][2] : z;
            continue;
        }
    }

    unit.SetUInt64Value(UNIT_FIELD_TARGET, 0);
    unit.AddUnitState(UNIT_STATE_CONFUSED);
    unit.SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_CONFUSED);
    unit.CastStop();

    if (!unit.IsAlive() || unit.IsStopped())
        return;

    unit.StopMoving();
}

template<>
void ConfusedMovementGenerator<Creature>::_InitSpecific(Creature& creature, bool& is_water_ok, bool& is_land_ok)
{
    is_water_ok = creature.canSwim();
    is_land_ok  = creature.canWalk();
}

template<>
void ConfusedMovementGenerator<Player>::_InitSpecific(Player&, bool& is_water_ok, bool& is_land_ok)
{
    is_water_ok = true;
    is_land_ok  = true;
}

template<class T>
void ConfusedMovementGenerator<T>::Reset(T& unit)
{
    i_nextMove = 1;
    i_nextMoveTime.Reset(0);

    if (!unit.IsAlive() || unit.IsStopped())
        return;

    unit.StopMoving();
    unit.AddUnitState(UNIT_STATE_CONFUSED);
}

template<class T>
bool ConfusedMovementGenerator<T>::Update(T& unit, const uint32& diff)
{
    if (unit.HasUnitState(UNIT_STATE_ROOT | UNIT_STATE_STUNNED | UNIT_STATE_DISTRACTED))
        return true;

    if (i_nextMoveTime.Passed())
    {
        if (unit.movespline->Finalized())
            {
                // arrived, stop and wait a bit
                unit.ClearUnitState(UNIT_STATE_MOVE);

                i_nextMove = urand(1, MAX_CONF_WAYPOINTS);
                i_nextMoveTime.Reset(urand(100, 1000));
            }
        }
    else
    {
        // waiting for next move
        i_nextMoveTime.Update(diff);
        if (i_nextMoveTime.Passed())
        {
            // start moving
            ASSERT(i_nextMove <= MAX_CONF_WAYPOINTS);
            const float x = i_waypoints[i_nextMove][0];
            const float y = i_waypoints[i_nextMove][1];
            const float z = i_waypoints[i_nextMove][2];
            Movement::MoveSplineInit init(unit);
            init.MoveTo(x, y, z, true);
            init.SetWalk(true);
            init.Launch();
        }
    }
    return true;
}

template<>
void ConfusedMovementGenerator<Player>::Finalize(Player& unit)
{
    unit.RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_CONFUSED);
    unit.ClearUnitState(UNIT_STATE_CONFUSED);
    unit.StopMoving();
}

template<>
void ConfusedMovementGenerator<Creature>::Finalize(Creature& unit)
{
    unit.RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_CONFUSED);
    unit.ClearUnitState(UNIT_STATE_CONFUSED);

    if (unit.getVictim())
        unit.SetUInt64Value(UNIT_FIELD_TARGET, unit.getVictim()->GetGUID());
}

template void ConfusedMovementGenerator<Player>::Initialize(Player& player);
template void ConfusedMovementGenerator<Creature>::Initialize(Creature& creature);
template void ConfusedMovementGenerator<Player>::Reset(Player& player);
template void ConfusedMovementGenerator<Creature>::Reset(Creature& creature);
template bool ConfusedMovementGenerator<Player>::Update(Player& player, const uint32& diff);
template bool ConfusedMovementGenerator<Creature>::Update(Creature& creature, const uint32& diff);
