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
#include "RandomMovementGenerator.h"
#include "Map.h"
#include "Utilities/Util.h"
#include "CreatureGroups.h"
#include "MoveSplineInit.h"
#include "MoveSpline.h"

#define RUNNING_CHANCE_RANDOMMV 20                                  //will be "1 / RUNNING_CHANCE_RANDOMMV"

template<>
void
RandomMovementGenerator<Creature>::_setRandomLocation(Creature& creature)
{
    if (creature.HasUnitState(UNIT_STATE_CASTING) && !creature.CanMoveDuringChannel())
    {
        creature.CastStop();
        return;
    }

    float X, Y, Z, nx, ny, nz, ori, dist;
    creature.GetHomePosition(X, Y, Z, ori);
    Map const* map = creature.GetBaseMap();

    // For 2D/3D system selection
    //bool is_land_ok  = creature.canWalk();
    //bool is_water_ok = creature.canSwim();
    bool is_air_ok   = creature.CanFly();

    const float angle = rand_norm() * (M_PI * 2);
    const float range = rand_norm() * wander_distance;
    const float distanceX = range * cos(angle);
    const float distanceY = range * sin(angle);

    nx = X + distanceX;
    ny = Y + distanceY;

    // prevent invalid coordinates generation
    Oregon::NormalizeMapCoord(nx);
    Oregon::NormalizeMapCoord(ny);

    dist = (nx - X) * (nx - X) + (ny - Y) * (ny - Y);
    
    if (is_air_ok)                                          // 3D system above ground and above water (flying mode)
    {
        // Limit height change
        const float distanceZ = rand_norm() * sqrtf(dist) / 2.0f;
        nz = Z + distanceZ;
        float tz = map->GetHeight(nx, ny, nz - 2.0f, false); // Map check only, vmap needed here but need to alter vmaps checks for height.
        float wz = map->GetWaterLevel(nx, ny);

        // Problem here, we must fly above the ground and water, not under. Let's try on next tick
        if (tz >= nz || wz >= nz)
            return;
    }
    //else if (is_water_ok)                                 // 3D system under water and above ground (swimming mode)
    else                                                    // 2D only
    {
        dist = dist >= 100.0f ? 10.0f : sqrtf(dist);
        // The fastest way to get an accurate result 90% of the time.
        // Better result can be obtained like 99% accuracy with a ray light, but the cost is too high and the code is too long.
        nz = map->GetHeight(nx, ny, Z + dist - 2.0f, false);

        if (fabs(nz - Z) > dist)                            // Map check
        {
            nz = map->GetHeight(nx, ny, Z, true);    // Vmap Horizontal or above

            if (fabs(nz - Z) > dist)
            {
                // Vmap Higher
                nz = map->GetHeight(nx, ny, Z + dist - 2.0f, true);

                // let's forget this bad coords where a z cannot be find and retry at next tick
                if (fabs(nz - Z) > dist)
                    return;
            }
        }
    }

    if (is_air_ok)
        i_nextMoveTime.Reset(0);
    else
    {
        if (roll_chance_i(MOVEMENT_RANDOM_MMGEN_CHANCE_NO_BREAK))
            i_nextMoveTime.Reset(urand(5000, 10000));
        else
            i_nextMoveTime.Reset(urand(50, 400));
    }

    creature.AddUnitState(UNIT_STATE_ROAMING);

    Movement::MoveSplineInit init(creature);
    init.MoveTo(nx, ny, nz, true);

    if (creature.CanFly())
        init.SetFly();

    if (creature.IsPet() && creature.GetOwner() && !creature.IsWithinDist(creature.GetOwner(), PET_FOLLOW_DIST + 2.5f))
        init.SetWalk(false);
    else
        init.SetWalk(true);
    init.Launch();
        if (roll_chance_i(MOVEMENT_RANDOM_MMGEN_CHANCE_NO_BREAK))
            i_nextMoveTime.Reset(50);
        else
        i_nextMoveTime.Reset(urand(1500, 10000));       // Keep a short wait time
}

template<>
void RandomMovementGenerator<Creature>::Initialize(Creature& creature)
{
    if (!creature.IsAlive())
        return;

    if (!wander_distance)
        wander_distance = creature.GetRespawnRadius();

    creature.AddUnitState(UNIT_STATE_ROAMING);
    _setRandomLocation(creature);
}

template<>
void RandomMovementGenerator<Creature>::Reset(Creature& creature)
{
    Initialize(creature);
}

template<>
void RandomMovementGenerator<Creature>::Finalize(Creature& creature)
{
    creature.ClearUnitState(UNIT_STATE_ROAMING);
    creature.SetWalk(!creature.HasUnitState(UNIT_STATE_RUNNING_STATE));
}

template<>
bool RandomMovementGenerator<Creature>::Update(Creature& creature, const uint32& diff)
{
    if (!creature.IsAlive())
        return false;

    if (creature.HasUnitState(UNIT_STATE_ROOT | UNIT_STATE_STUNNED | UNIT_STATE_DISTRACTED))
    {
        i_nextMoveTime.Reset(0);  // Expire the timer
        creature.ClearUnitState(UNIT_STATE_ROAMING);
        return true;
    }

    if (creature.movespline->Finalized())
    {
        i_nextMoveTime.Update(diff);
        if (i_nextMoveTime.Passed())
            _setRandomLocation(creature);
    }
    return true;
}

