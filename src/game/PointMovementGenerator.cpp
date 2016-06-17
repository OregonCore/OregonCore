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
#include "ScriptedInstance.h"
#include "TemporarySummon.h"
#include "MapManager.h"
#include "World.h"
#include "PathFinder.h"
#include "MoveSplineInit.h"
#include "MoveSpline.h"

//----- Point Movement Generator
template<class T>
void PointMovementGenerator<T>::Initialize(T& unit)
{
    if (!unit.IsStopped())
        unit.StopMoving();

    unit.AddUnitState(UNIT_STATE_ROAMING);
    Movement::MoveSplineInit init(unit);
    init.MoveTo(i_x, i_y, i_z, m_usePathfinding);
    if (speed > 0.0f)
        init.SetVelocity(speed);
    init.Launch();
}

template<class T>
void PointMovementGenerator<T>:: Finalize(T& unit)
{
    if (unit.HasUnitState(UNIT_STATE_CHARGING))
        unit.ClearUnitState(UNIT_STATE_CHARGING | UNIT_STATE_JUMPING);

    if (unit.movespline->Finalized())
        MovementInform(unit);
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

    if (unit.movespline->Finalized())
    {
        unit.ClearUnitState(UNIT_STATE_MOVE);
        return false;
    }

    return true;
}

template<class T>
void PointMovementGenerator<T>::MovementInform(T& /*unit*/)
{
}

template <> void PointMovementGenerator<Creature>::MovementInform(Creature& unit)
{
    unit.AI()->MovementInform(POINT_MOTION_TYPE, id);

    if (unit.IsSummon())
    {
        if (TempSummon* pSummon = (TempSummon*)(&unit))
        {
            if (Unit* pSummoner = pSummon->GetSummoner())
            {
            if (pSummoner->GetTypeId() == TYPEID_UNIT)
            {
                    if (ScriptedInstance* pInstance = (ScriptedInstance*)pSummoner->GetInstanceData())
                        pInstance->MovementInform(&unit, POINT_MOTION_TYPE, id);

                    if (pSummoner->ToCreature()->AI())
                        pSummoner->ToCreature()->AI()->SummonedMovementInform(&unit, POINT_MOTION_TYPE, id);
                }
            }
        }
    }
}

template void PointMovementGenerator<Player>::Initialize(Player&);
template void PointMovementGenerator<Player>::Finalize(Player&);
template void PointMovementGenerator<Player>::Reset(Player&);
template bool PointMovementGenerator<Player>::Update(Player&, const uint32& diff);
template void PointMovementGenerator<Player>::MovementInform(Player&);

template void PointMovementGenerator<Creature>::Initialize(Creature&);
template void PointMovementGenerator<Creature>::Finalize(Creature&);
template void PointMovementGenerator<Creature>::Reset(Creature&);
template bool PointMovementGenerator<Creature>::Update(Creature&, const uint32& diff);

void AssistanceMovementGenerator::Finalize(Unit& unit)
{
    unit.ToCreature()->SetNoCallAssistance(false);
    unit.ToCreature()->CallAssistance();
    if (unit.IsAlive())
        unit.GetMotionMaster()->MoveSeekAssistanceDistract(sWorld.getConfig(CONFIG_CREATURE_FAMILY_ASSISTANCE_DELAY));
}

bool EffectMovementGenerator::Update(Unit& unit, const uint32&)
{
    return !unit.movespline->Finalized();
}

void EffectMovementGenerator::Finalize(Unit& unit)
{
    if (unit.GetTypeId() != TYPEID_UNIT)
        return;

    // Need restore previous movement since we have no proper states system
    if (unit.IsAlive() && !unit.HasUnitState(UNIT_STATE_CONFUSED | UNIT_STATE_FLEEING | UNIT_STATE_DISTRACTED))
    {
        if (Unit* victim = unit.GetVictim())
            unit.GetMotionMaster()->MoveChase(victim);
        else
            unit.GetMotionMaster()->Initialize();
    }
    else if (!unit.IsAlive())
    {
        unit.GetMotionMaster()->MoveIdle();
    }

    if (unit.ToCreature()->AI())
        unit.ToCreature()->AI()->MovementInform(EFFECT_MOTION_TYPE, m_Id);
}
