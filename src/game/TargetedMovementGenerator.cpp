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

#include "ByteBuffer.h"
#include "TargetedMovementGenerator.h"
#include "Errors.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "World.h"
#include "Unit.h"
#include "Pet.h"
#include "MoveSplineInit.h"
#include "MoveSpline.h"
#include <cmath>

template<class T, typename D>
void TargetedMovementGeneratorMedium<T, D>::_setTargetLocation(T& owner, bool updateDestination)
{
    if (!i_target.isValid() || !i_target->IsInWorld())
        return;

    if (owner.HasUnitState(UNIT_STATE_ROOT | UNIT_STATE_STUNNED | UNIT_STATE_DISTRACTED) || owner.isDead())
        return;

    if (owner.GetTypeId() == TYPEID_UNIT && !i_target->isInAccessiblePlaceFor(owner.ToCreature()))
        return;

    float x, y, z;
    
    // i_path can be NULL in case this is the first call for this MMGen (via Update)
    // Can happen for example if no path was created on MMGen-Initialize because of the owner being stunned
    if (updateDestination || !i_path)
    {
        owner.GetPosition(x, y, z);

        // prevent redundant micro-movement for pets, other followers.
        if (!RequiresNewPosition(owner, x, y, z))
        {
            if (!owner.movespline->Finalized())
                return;
        }
        else
        {
    if (!i_offset)
    {
        // to nearest random contact position
        i_target->GetRandomContactPoint(&owner, x, y, z, 0, CONTACT_DISTANCE);

        // Sometimes target is available only from certain angles
        if (fabsf(i_target->GetPositionZ() - z) > owner.GetObjectSize())
        {
            float angles[] = { 0.f, 90.f, 180.f, 270.f, 45.f, 125.f, 225.f, 315.f };
            bool needExact = true;
            for (uint32 i = 0; i < sizeof(angles) / sizeof(*angles); i++)
            {
                i_target->GetClosePoint(x, y, z, owner.GetObjectSize(), CONTACT_DISTANCE, angles[i]);
                if (fabsf(i_target->GetPositionZ() - z) <= owner.GetObjectSize())
                {
                    needExact = false;
                    break;
                }
            }

            if (needExact)
                i_target->GetPosition(x, y, z);
        }
    }
    else if (!i_angle && !owner.HasUnitState(UNIT_STATE_FOLLOW))
    {
        // caster chase
        i_target->GetContactPoint(&owner, x, y, z, i_offset * urand(80, 95) * 0.01f);
    }
    else
    {
        // to at i_offset distance from target and i_angle from target facing
        i_target->GetClosePoint(x, y, z, owner.GetObjectSize(), i_offset, i_angle);
    }
        }
    }
    else
    {
        // the destination has not changed, we just need to refresh the path (usually speed change)
        G3D::Vector3 end = i_path->getEndPosition();
        x = end.x;
        y = end.y;
        z = end.z;
    }

    if (!i_path)
        i_path = new PathInfo(&owner);

    // allow pets following their master to cheat while generating paths
    bool forceDest = (owner.GetTypeId() == TYPEID_UNIT && ((Creature*)&owner)->IsPet()
        && owner.HasUnitState(UNIT_STATE_FOLLOW));

    bool result = i_path->Update(x, y, z, forceDest);
    if (!result || (i_path->getPathType() & PATHFIND_NOPATH))
    {
        // Cant reach target
        m_speedChanged = true;
        return;
    }

    i_targetReached = false;
    m_speedChanged = false;
    owner.AddUnitState(UNIT_STATE_CHASE);

    Movement::MoveSplineInit init(owner);
    init.MovebyPath(i_path->getFullPath());
    init.SetWalk(((D*)this)->EnableWalking());

    init.Launch();
}

template<class T, typename D>
bool TargetedMovementGeneratorMedium<T, D>::Update(T& owner, const uint32& time_diff)
{
    if (!i_target.isValid() || !i_target->IsInWorld())
        return false;

    if (!owner.IsAlive())
        return true;

    if (owner.HasUnitState(UNIT_STATE_ROOT | UNIT_STATE_STUNNED | UNIT_STATE_FLEEING | UNIT_STATE_DISTRACTED))
        return true;

    // prevent movement while casting spells with cast time or channel time
    if (owner.IsNonMeleeSpellCast(false, false,  true) || owner.HasUnitState(UNIT_STATE_CASTING))
    {
        if (!owner.IsStopped())
            owner.StopMoving();
        return true;
    }

    if (!owner.HasAuraType(SPELL_AURA_MOD_INVISIBILITY) && !owner.canSeeOrDetect(i_target.getTarget(), true))
    {
        owner.AttackStop();
        if (owner.GetOwner())
            owner.GetMotionMaster()->MoveFollow(owner.GetOwner(), PET_FOLLOW_DIST, owner.GetFollowAngle());
        else
            owner.StopMoving();

        return true;
    }

    // prevent crash after creature killed pet
    if (!owner.HasUnitState(UNIT_STATE_FOLLOW) && owner.getVictim() != i_target.getTarget())
        return true;

    if (i_path && i_path->getPathType() & PATHFIND_NOPATH)
    {
        if (Creature* me = owner.ToCreature())
        {
            if (m_evadeTimer <= time_diff)
            {
                if (me->AI())
                    me->AI()->EnterEvadeMode();
            }
            else
                m_evadeTimer -= time_diff;
        }
        return true;
    }

    bool targetMoved = false;
    i_recheckDistance.Update(time_diff);
    if (i_recheckDistance.Passed())
    {
        i_recheckDistance.Reset(this->GetMovementGeneratorType() == FOLLOW_MOTION_TYPE ? 50 : 100);
        G3D::Vector3 dest = owner.movespline->FinalDestination();
        targetMoved = RequiresNewPosition(owner, dest.x, dest.y, dest.z);
    }

    if (m_speedChanged || targetMoved)
        _setTargetLocation(owner, targetMoved);

    if (owner.movespline->Finalized())
    {
        if (i_angle == 0.f && !owner.HasInArc(0.01f, i_target.getTarget()))
            owner.SetInFront(i_target.getTarget());

        if (!i_targetReached)
        {
            i_targetReached = true;
            static_cast<D*>(this)->_reachTarget(owner);
        }
    }

    // Implemented for PetAI to handle resetting flags when pet owner reached
    if (owner.movespline->Finalized())
        MovementInform(owner);

    return true;
}

template<class T, typename D>
bool TargetedMovementGeneratorMedium<T, D>::RequiresNewPosition(T& owner, float x, float y, float z) const
{
    bool targetMoved;

    //More distance let have better performance, less distance let have more sensitive reaction at target move.
    float dist = owner.GetCombatReach() + sWorld.getRate(RATE_TARGET_POS_RECALCULATION_RANGE);

    // GetClosePoint() will always return a point on the ground, so we need to
    // handle the difference in elevation when the creature is flying
    if (owner.GetTypeId() == TYPEID_UNIT && ((Creature*)&owner)->canFly())
        targetMoved = i_target->GetDistanceSqr(x, y, z) > dist * dist;
    else
        targetMoved = i_target->GetDistance2d(x, y) > dist;

    // then, if the target is in range, check also Line of Sight.
    if (!targetMoved)
        targetMoved = !i_target->IsWithinLOSInMap(&owner);

    return targetMoved;
}

template<class T>
void ChaseMovementGenerator<T>::_reachTarget(T& owner)
{
    if (owner.IsWithinMeleeRange(this->i_target.getTarget()))
        owner.Attack(this->i_target.getTarget(), true);
}

template<>
void ChaseMovementGenerator<Player>::Initialize(Player& owner)
{
    owner.AddUnitState(UNIT_STATE_CHASE);                    // _MOVE set in _SetTargetLocation after required checks
    _setTargetLocation(owner, true);
    m_evadeTimer = urand(4000, 8000);
}

template<>
void ChaseMovementGenerator<Creature>::Initialize(Creature& owner)
{
    owner.SetWalk(false);                            // Chase movement is running
    owner.AddUnitState(UNIT_STATE_CHASE);                    // _MOVE set in _SetTargetLocation after required checks
    _setTargetLocation(owner, true);
    m_evadeTimer = urand(4000, 8000);
}

template<class T>
void ChaseMovementGenerator<T>::Finalize(T& owner)
{
    owner.ClearUnitState(UNIT_STATE_CHASE);
}

template<class T>
void ChaseMovementGenerator<T>::Reset(T& owner)
{
    Initialize(owner);
}

template<class T>
void ChaseMovementGenerator<T>::MovementInform(T& /*unit*/)
{
}

template<>
bool FollowMovementGenerator<Creature>::EnableWalking() const
{
    return i_target.isValid() && i_target->IsWalking();
}

template<>
bool FollowMovementGenerator<Player>::EnableWalking() const
{
    return false;
}

template<>
void FollowMovementGenerator<Player>::_updateSpeed(Player& /*u*/)
{
    // nothing to do for Player
}

template<>
void FollowMovementGenerator<Creature>::_updateSpeed(Creature& u)
{
    // pet only sync speed with owner
    if (!((Creature&)u).IsPet() || !i_target.isValid() || i_target->GetObjectGUID().GetRawValue() != u.GetOwnerGUID())
        return;

    u.UpdateSpeed(MOVE_RUN, true);
    u.UpdateSpeed(MOVE_WALK, true);
    u.UpdateSpeed(MOVE_SWIM, true);
}

template<>
void FollowMovementGenerator<Player>::Initialize(Player& owner)
{
    owner.AddUnitState(UNIT_STATE_FOLLOW);                   // _MOVE set in _SetTargetLocation after required checks
    _updateSpeed(owner);
    _setTargetLocation(owner, true);
    m_evadeTimer = urand(4000, 8000);
}

template<>
void FollowMovementGenerator<Creature>::Initialize(Creature& owner)
{
    owner.AddUnitState(UNIT_STATE_FOLLOW);                   // _MOVE set in _SetTargetLocation after required checks
    _updateSpeed(owner);
    _setTargetLocation(owner, true);
    m_evadeTimer = urand(4000, 8000);
}

template<class T>
void FollowMovementGenerator<T>::Finalize(T& owner)
{
    owner.ClearUnitState(UNIT_STATE_FOLLOW);
    _updateSpeed(owner);
}

template<class T>
void FollowMovementGenerator<T>::Reset(T& owner)
{
    Initialize(owner);
}

template<class T>
void FollowMovementGenerator<T>::MovementInform(T& /*unit*/)
{
}

template<>
void FollowMovementGenerator<Creature>::MovementInform(Creature& unit)
{
    // Pass back the GUIDLow of the target. If it is pet's owner then PetAI will handle
    unit.AI()->MovementInform(FOLLOW_MOTION_TYPE, i_target.getTarget()->GetGUIDLow());
}

//-----------------------------------------------//
template void TargetedMovementGeneratorMedium<Player, ChaseMovementGenerator<Player> >::_setTargetLocation(Player&, bool);
template void TargetedMovementGeneratorMedium<Player, FollowMovementGenerator<Player> >::_setTargetLocation(Player&, bool);
template void TargetedMovementGeneratorMedium<Creature, ChaseMovementGenerator<Creature> >::_setTargetLocation(Creature&, bool);
template void TargetedMovementGeneratorMedium<Creature, FollowMovementGenerator<Creature> >::_setTargetLocation(Creature&, bool);
template bool TargetedMovementGeneratorMedium<Player, ChaseMovementGenerator<Player> >::Update(Player&, const uint32&);
template bool TargetedMovementGeneratorMedium<Player, FollowMovementGenerator<Player> >::Update(Player&, const uint32&);
template bool TargetedMovementGeneratorMedium<Creature, ChaseMovementGenerator<Creature> >::Update(Creature&, const uint32&);
template bool TargetedMovementGeneratorMedium<Creature, FollowMovementGenerator<Creature> >::Update(Creature&, const uint32&);
template bool TargetedMovementGeneratorMedium<Player, ChaseMovementGenerator<Player> >::IsReachable() const;
template bool TargetedMovementGeneratorMedium<Player, FollowMovementGenerator<Player> >::IsReachable() const;
template bool TargetedMovementGeneratorMedium<Creature, ChaseMovementGenerator<Creature> >::IsReachable() const;
template bool TargetedMovementGeneratorMedium<Creature, FollowMovementGenerator<Creature> >::IsReachable() const;

template void ChaseMovementGenerator<Player>::_reachTarget(Player&);
template void ChaseMovementGenerator<Creature>::_reachTarget(Creature&);
template void ChaseMovementGenerator<Player>::Finalize(Player&);
template void ChaseMovementGenerator<Creature>::Finalize(Creature&);
template void ChaseMovementGenerator<Player>::Reset(Player&);
template void ChaseMovementGenerator<Creature>::Reset(Creature&);
template void ChaseMovementGenerator<Player>::MovementInform(Player&);
template void ChaseMovementGenerator<Creature>::MovementInform(Creature&);

template void FollowMovementGenerator<Player>::Finalize(Player&);
template void FollowMovementGenerator<Creature>::Finalize(Creature&);
template void FollowMovementGenerator<Player>::Reset(Player&);
template void FollowMovementGenerator<Creature>::Reset(Creature&);
template void FollowMovementGenerator<Player>::MovementInform(Player&);
