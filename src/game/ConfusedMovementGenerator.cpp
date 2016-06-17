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
#include "Opcodes.h"
#include "Player.h"
#include "ConfusedMovementGenerator.h"
#include "MoveSplineInit.h"
#include "MoveSpline.h"

template<class T>
void ConfusedMovementGenerator<T>::Initialize(T& unit)
{
    unit.SetUInt64Value(UNIT_FIELD_TARGET, 0);
    unit.AddUnitState(UNIT_STATE_CONFUSED);
    unit.SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_CONFUSED);
    unit.CastStop();
    unit.GetPosition(i_x, i_y, i_z);

    if (!unit.IsAlive() || unit.IsStopped())
        return;

    unit.StopMoving();
}

template<class T>
void ConfusedMovementGenerator<T>::Reset(T& unit)
{
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
            i_nextMoveTime.Reset(urand(800, 1500));
        }
    }
    else
    {
        // waiting for next move
        i_nextMoveTime.Update(diff);
        if (i_nextMoveTime.Passed())
        {
            float dest = 4.0f * (float)rand_norm() - 2.0f;
            Position pos;
            pos.Relocate(i_x, i_y, i_z);
            unit.MovePositionToFirstCollision(pos, dest, 0.0f);

            PathInfo path(&unit);
            path.setPathLengthLimit(30.0f);
            bool result = path.Update(pos.m_positionX, pos.m_positionY, pos.m_positionZ);
            if (!result || (path.getPathType() & PATHFIND_NOPATH))
            {
                i_nextMoveTime.Reset(100);
                return true;
            }

            // start moving
            Movement::MoveSplineInit init(unit);
            init.MovebyPath(path.getFullPath());
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

    if (unit.GetVictim())
        unit.SetUInt64Value(UNIT_FIELD_TARGET, unit.GetVictim()->GetGUID());
}

template void ConfusedMovementGenerator<Player>::Initialize(Player& player);
template void ConfusedMovementGenerator<Creature>::Initialize(Creature& creature);
template void ConfusedMovementGenerator<Player>::Reset(Player& player);
template void ConfusedMovementGenerator<Creature>::Reset(Creature& creature);
template bool ConfusedMovementGenerator<Player>::Update(Player& player, const uint32& diff);
template bool ConfusedMovementGenerator<Creature>::Update(Creature& creature, const uint32& diff);
