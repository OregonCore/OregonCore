/*
 * Copyright (C) 2010-2014 OregonCore <http://www.oregoncore.com/>
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2012 MaNGOS <http://getmangos.com/>
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
#include "DestinationHolderImp.h"
#include "World.h"
#include "Unit.h"
#include "Pet.h"
 
#define SMALL_ALPHA 0.05f

#include <cmath>
 
template<class T>
TargetedMovementGenerator<T>::TargetedMovementGenerator(Unit &target, float offset, float angle, bool _usePathfinding):
TargetedMovementGeneratorBase(target), i_offset(offset), i_angle(angle), m_usePathfinding(_usePathfinding), i_recalculateTravel(false), i_path(NULL), m_pathPointsSent(0)
{
    target.GetPosition(i_targetX, i_targetY, i_targetZ);
}
 
template<class T>
bool TargetedMovementGenerator<T>::_setTargetLocation(T &owner)
{
    if (!i_target.isValid() || !i_target->IsInWorld())
        return false;

    if (owner.hasUnitState(UNIT_STAT_ROOT | UNIT_STAT_STUNNED | UNIT_STAT_DISTRACTED))
        return false;

    if (owner.GetTypeId() == TYPEID_UNIT && !i_target->isInAccessiblePlaceFor(owner.ToCreature()))
        return false;

    float x, y, z;
    Traveller<T> traveller(owner);
    if (i_destinationHolder.HasDestination() && !m_pathPointsSent)
    {
        if (i_destinationHolder.HasArrived())
        {
            // prevent redundant micro-movement
            if (!i_offset)
            {
                if (i_target->IsWithinMeleeRange(&owner))
                    return false;
            }
            else if (!i_angle && !owner.hasUnitState(UNIT_STAT_FOLLOW))
            {
                if (i_target->IsWithinDistInMap(&owner, i_offset))
                    return false;
            }
            else
            {
                if (i_target->IsWithinDistInMap(&owner, i_offset + 1.0f))
                    return false;
            }
        }
        else
        {
            bool stop = false;
            if (!i_offset)
            {
                if (i_target->IsWithinMeleeRange(&owner, 0.0f))
                    stop = true;
            }
            else if (!i_angle && !owner.hasUnitState(UNIT_STAT_FOLLOW))
            {
                if (i_target->IsWithinDist(&owner, i_offset * 0.8f))
                    stop = true;
            }

            if (stop)
            {
                owner.GetPosition(x, y, z);
 
                if (owner.GetTypeId() == TYPEID_UNIT && (owner.HasUnitMovementFlag(MOVEFLAG_CAN_FLY) || owner.IsInWater() || i_target->IsInWater()))
                    z = i_target->GetPositionZ();
 
                if(m_usePathfinding)
                {                
                    bool newPathCalculated = true;
 
                    if (!i_path)
                        i_path = new PathInfo(&owner, x, y, z);
                    else
                        newPathCalculated = i_path->Update(x, y, z);
 
                    // nothing we can do here ...
                    if(i_path->getPathType() & PATHFIND_NOPATH)
                        return true;
 
                    PointPath pointPath = i_path->getFullPath();
 
                    if (i_destinationHolder.HasArrived() && m_pathPointsSent)
                        --m_pathPointsSent;
 
                    i_path->getNextPosition(x, y, z);
                    i_destinationHolder.SetDestination(traveller, x, y, z, false);
 
                    // send the path if:
                    //    we have brand new path
                    //    we have visited almost all of the previously sent points
                    //    movespeed has changed
                    //    the owner is stopped (caused by some movement effects)
                    if (newPathCalculated || m_pathPointsSent < 2 || i_recalculateTravel || owner.IsStopped())
                    {
                        // send 10 nodes, or send all nodes if there are less than 10 left
                        m_pathPointsSent = std::min<uint32>(10, pointPath.size() - 1);
                        uint32 endIndex = m_pathPointsSent + 1;
 
                        // dist to next node + world-unit length of the path
                        x -= owner.GetPositionX();
                        y -= owner.GetPositionY();
                        z -= owner.GetPositionZ();
                        float dist = sqrt(x*x + y*y + z*z) + pointPath.GetTotalLength(1, endIndex);
 
                        // calculate travel time, set spline, then send path
                        uint32 traveltime = uint32(dist / (traveller.Speed()*0.001f));
 
                        owner.SendMonsterMoveByPath(pointPath, 1, endIndex, traveltime);
 
                        return false;
                    }
                }
                else
                {
                    i_destinationHolder.SetDestination(traveller, x, y, z);
                    i_destinationHolder.StartTravel(traveller, false);
                    owner.StopMoving();
                    return false;
                }
            }
        }

        if (i_target->GetExactDistSq(i_targetX, i_targetY, i_targetZ) < 0.01f)
            return false;
    }

    if (!i_offset)
    {
        // to nearest random contact position
        i_target->GetRandomContactPoint(&owner, x, y, z, 0, CONTACT_DISTANCE);
        
        // Sometimes target is available only from certain angles
        if (fabsf(i_target->GetPositionZ() - z) > owner.GetObjectSize())
        {
            float angles[] = { 0.f, 90.f, 180.f, 270.f, 45.f, 125.f, 225.f, 315.f };
            bool needExact = true;
            for (uint32 i = 0; i < sizeof(angles)/sizeof(*angles); i++)
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
    else if (!i_angle && !owner.hasUnitState(UNIT_STAT_FOLLOW))
    {
        // caster chase
        i_target->GetContactPoint(&owner, x, y, z, i_offset * urand(80, 95) * 0.01f);
    }
    else
    {
        // to at i_offset distance from target and i_angle from target facing
        i_target->GetClosePoint(x, y, z, owner.GetObjectSize(), i_offset,i_angle);
    }

    /*
        We MUST not check the distance difference and avoid setting the new location for smaller distances.
        By that we risk having far too many GetContactPoint() calls freezing the whole system.
        In TargetedMovementGenerator<T>::Update() we check the distance to the target and at
        some range we calculate a new position. The calculation takes some processor cycles due to vmaps.
        If the distance to the target it too large to ignore,
        but the distance to the new contact point is short enough to be ignored,
        we will calculate a new contact point each update loop, but will never move to it.
        The system will freeze.
        ralf

        //We don't update Mob Movement, if the difference between New destination and last destination is < BothObjectSize
        float  bothObjectSize = i_target->GetObjectSize() + owner.GetObjectSize() + CONTACT_DISTANCE;
        if (i_destinationHolder.HasDestination() && i_destinationHolder.GetDestinationDiff(x,y,z) < bothObjectSize)
            return;
    */
 
    if(m_usePathfinding)
    {
 
        bool forceDest = false;
        // allow pets to cheat while generating paths as they should ALWAYS be able to reach thier target.
        if (owner.GetTypeId() == TYPEID_UNIT
            && owner.ToCreature()
            && owner.ToCreature()->isPet())
            forceDest = true;
 
        bool newPathCalculated = true;
        if (!i_path)
            i_path = new PathInfo(&owner, x, y, z, forceDest);
        else
        newPathCalculated = i_path->Update(x, y, z, forceDest);
 
        // nothing we can do here ...
        if (i_path->getPathType() & PATHFIND_NOPATH)
            return true;
 
        if (i_destinationHolder.HasArrived() && m_pathPointsSent)
            --m_pathPointsSent;
 
        i_path->getNextPosition(x, y, z);
        i_destinationHolder.SetDestination(traveller, x, y, z, false);
 
        // send the path if:
        //    we have brand new path
        //    we have visited almost all of the previously sent points
        //    movespeed has changed
        //    the owner is stopped (caused by some movement effects)
        if (newPathCalculated || m_pathPointsSent < 2 || i_recalculateTravel || owner.IsStopped())
        {
            PointPath pointPath = i_path->getFullPath();
            // send 10 nodes, or send all nodes if there are less than 10 left
            m_pathPointsSent = std::min<uint32>(10, pointPath.size() - 1);
            uint32 endIndex = m_pathPointsSent + 1;
 
            // dist to next node + world-unit length of the path
            x -= owner.GetPositionX();
            y -= owner.GetPositionY();
            z -= owner.GetPositionZ();
            float dist = sqrt(x*x + y*y + z*z) + pointPath.GetTotalLength(1, endIndex);
 
            // calculate travel time, set spline, then send path
            uint32 traveltime = uint32(dist / (traveller.Speed()*0.001f));
 
            owner.SendMonsterMoveByPath(pointPath, 1, endIndex, traveltime);
        }
 
        owner.addUnitState(UNIT_STAT_CHASE);
    }    
    else
    {
        i_destinationHolder.SetDestination(traveller, x, y, z);
        owner.addUnitState(UNIT_STAT_CHASE);
        i_destinationHolder.StartTravel(traveller);
    }
 
 
    return true;
}

template<class T>
void TargetedMovementGenerator<T>::Initialize(T &owner)
{
    if (owner.isInCombat())
        owner.RemoveUnitMovementFlag(MOVEFLAG_WALK_MODE);
 
    _setTargetLocation(owner);
    m_evadeTimer = urand(4000, 8000);
}

template<class T>
void TargetedMovementGenerator<T>::Finalize(T &owner)
{
    owner.clearUnitState(UNIT_STAT_CHASE);
}

template<class T>
void TargetedMovementGenerator<T>::Reset(T &owner)
{
    Initialize(owner);
}

template<class T>
bool TargetedMovementGenerator<T>::Update(T &owner, const uint32 & time_diff)
{
    if (!i_target.isValid() || !i_target->IsInWorld())
        return false;

    if (!&owner || !owner.isAlive())
        return true;

    if (owner.hasUnitState(UNIT_STAT_ROOT | UNIT_STAT_STUNNED | UNIT_STAT_FLEEING | UNIT_STAT_DISTRACTED))
        return true;

    // prevent movement while casting spells with cast time or channel time
    if (owner.IsNonMeleeSpellCasted(false, false,  true) || owner.hasUnitState(UNIT_STAT_CASTING))
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
    if (!owner.hasUnitState(UNIT_STAT_FOLLOW) && owner.getVictim() != i_target.getTarget())
        return true;
 
    if (m_usePathfinding)
    {
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
 
        Traveller<T> traveller(owner);
 
        if (!i_destinationHolder.HasDestination())
            _setTargetLocation(owner);
 
        if (i_destinationHolder.UpdateTraveller(traveller, time_diff, i_recalculateTravel || owner.IsStopped()))
        {
            // put targeted movement generators on a higher priority
            if (owner.GetCombatReach())
                i_destinationHolder.ResetUpdate(100);
 
            float x,y,z;
            i_target->GetPosition(x, y, z);
            PathNode next_point(x, y, z);
 
            bool targetMoved = false, needNewDest = false;
            bool forceRecalc = i_recalculateTravel || owner.IsStopped();
            if (i_path && !forceRecalc)
            {
                PathNode end_point = i_path->getEndPosition();
                next_point = i_path->getNextPosition();
 
                //More distance let have better performance, less distance let have more sensitive reaction at target move.
                float dist = owner.GetCombatReach() + sWorld.getRate(RATE_TARGET_POS_RECALCULATION_RANGE);
 
                needNewDest = i_destinationHolder.HasArrived() && (!i_path->inRange(next_point, i_path->getActualEndPosition(), dist, dist) || !owner.IsWithinLOSInMap(i_target.getTarget()));
 
                if(!needNewDest)
                {
                    // GetClosePoint() will always return a point on the ground, so we need to
                    // handle the difference in elevation when the creature is flying
                    if (owner.GetTypeId() == TYPEID_UNIT && ((Creature*)&owner)->canFly())
                        targetMoved = i_target->GetDistanceSqr(end_point.x, end_point.y, end_point.z) > dist*dist;
                    else
                        targetMoved = i_target->GetDistance2d(end_point.x, end_point.y) > dist;
                }
            }
 
            // target moved
            if (!i_path || targetMoved || needNewDest || forceRecalc)
            {
                // (re)calculate path
                _setTargetLocation(owner);
 
                next_point = i_path->getNextPosition();
 
                // Set new Angle For Map::
                owner.SetOrientation(owner.GetAngle(next_point.x, next_point.y));
            }
            // Update the Angle of the target only for Map::, no need to send packet for player
            else if (!i_angle && !owner.HasInArc(0.01f, next_point.x, next_point.y))
                owner.SetOrientation(owner.GetAngle(next_point.x, next_point.y));
 
            if ((owner.IsStopped() && !i_destinationHolder.HasArrived()) || i_recalculateTravel)
            {
                i_recalculateTravel = false;
                //Angle update will take place into owner.StopMoving()
                owner.SetInFront(i_target.getTarget());
 
                owner.StopMoving();
                if(owner.IsWithinMeleeRange(i_target.getTarget()) && !owner.hasUnitState(UNIT_STAT_FOLLOW))
                    owner.Attack(i_target.getTarget(), true);
            }
        }
    }
    else
    {
        Traveller<T> traveller(owner);
 
        if (!i_destinationHolder.HasDestination())
        {
            _setTargetLocation(owner);
        }
        else if (owner.IsStopped() && !i_destinationHolder.HasArrived())
        {
            owner.addUnitState(UNIT_STAT_CHASE);
            i_destinationHolder.StartTravel(traveller);
            return true;
        }
 
        if (i_destinationHolder.UpdateTraveller(traveller, time_diff))
        {
            // target moved
            if (i_targetX != i_target->GetPositionX() || i_targetY != i_target->GetPositionY()
                || i_targetZ != i_target->GetPositionZ())
            {
                if(_setTargetLocation(owner) || !owner.hasUnitState(UNIT_STAT_FOLLOW))
                    owner.SetInFront(i_target.getTarget());
                i_target->GetPosition(i_targetX, i_targetY, i_targetZ);
            }
 
            if ((owner.IsStopped() && !i_destinationHolder.HasArrived()) || i_recalculateTravel)
            {
                i_recalculateTravel = false;
                //Angle update will take place into owner.StopMoving()
                owner.SetInFront(i_target.getTarget());
 
                owner.StopMoving();
                if(owner.IsWithinMeleeRange(i_target.getTarget()) && !owner.hasUnitState(UNIT_STAT_FOLLOW))
                    owner.Attack(i_target.getTarget(),true);
            }
        }
    }

    // Implemented for PetAI to handle resetting flags when pet owner reached
    if (i_destinationHolder.HasArrived())
        MovementInform(owner);

    return true;
}

template<class T>
Unit* TargetedMovementGenerator<T>::GetTarget() const
{
    return i_target.getTarget();
}

template<class T>
void TargetedMovementGenerator<T>::MovementInform(T & /*unit*/)
{
}

template <> void TargetedMovementGenerator<Creature>::MovementInform(Creature &unit)
{
    // Pass back the GUIDLow of the target. If it is pet's owner then PetAI will handle
    unit.AI()->MovementInform(TARGETED_MOTION_TYPE, i_target.getTarget()->GetGUIDLow());
}

template void TargetedMovementGenerator<Player>::MovementInform(Player&); // Not implemented for players
template TargetedMovementGenerator<Player>::TargetedMovementGenerator(Unit &target, float offset, float angle, bool _usePathfinding);
template TargetedMovementGenerator<Creature>::TargetedMovementGenerator(Unit &target, float offset, float angle, bool _usePathfinding);
template bool TargetedMovementGenerator<Player>::_setTargetLocation(Player &);
template bool TargetedMovementGenerator<Creature>::_setTargetLocation(Creature &);
template void TargetedMovementGenerator<Player>::Initialize(Player &);
template void TargetedMovementGenerator<Creature>::Initialize(Creature &);
template void TargetedMovementGenerator<Player>::Finalize(Player &);
template void TargetedMovementGenerator<Creature>::Finalize(Creature &);
template void TargetedMovementGenerator<Player>::Reset(Player &);
template void TargetedMovementGenerator<Creature>::Reset(Creature &);
template bool TargetedMovementGenerator<Player>::Update(Player &, const uint32 &);
template bool TargetedMovementGenerator<Creature>::Update(Creature &, const uint32 &);
template Unit* TargetedMovementGenerator<Player>::GetTarget() const;
template Unit* TargetedMovementGenerator<Creature>::GetTarget() const;

