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

#ifndef OREGON_TARGETEDMOVEMENTGENERATOR_H
#define OREGON_TARGETEDMOVEMENTGENERATOR_H

#include "MovementGenerator.h"
#include "FollowerReference.h"
#include "PathFinder.h"

class TargetedMovementGeneratorBase
{
    public:
        TargetedMovementGeneratorBase(Unit& target)
        {
            i_target.link(&target, this);
        }
        void stopFollowing() { }
    protected:
        FollowerReference i_target;
};

template<class T, typename D>
class TargetedMovementGeneratorMedium
    : public MovementGeneratorMedium< T, D >, public TargetedMovementGeneratorBase
{
    protected:
        TargetedMovementGeneratorMedium(Unit& target, float offset = 0, float angle = 0) :
            TargetedMovementGeneratorBase(target),
            m_evadeTimer(urand(4000, 8000)),
            i_offset(offset), i_angle(angle),
            i_recheckDistance(0), i_path(NULL),
            m_speedChanged(false), i_targetReached(false)
        {
        }
        ~TargetedMovementGeneratorMedium()
        {
            delete i_path;
        }

    public:
        bool Update(T&, const uint32&);

        Unit* GetTarget() const { return i_target.getTarget(); }

        bool IsReachable() const
        {
            return (i_path) ? (i_path->getPathType() & PATHFIND_NORMAL) : true;
        }

        void unitSpeedChanged() { m_speedChanged = true; }

        virtual void MovementInform(T&) { }

    protected:
        void _setTargetLocation(T&, bool updateDestination);
        bool RequiresNewPosition(T& owner, float x, float y, float z) const;

        TimeTrackerSmall i_recheckDistance;
        float i_offset;
        float i_angle;
        bool m_speedChanged : 1;
        bool i_targetReached : 1;

        PathInfo* i_path;
        uint32 m_evadeTimer;
};

template<class T>
class ChaseMovementGenerator : public TargetedMovementGeneratorMedium<T, ChaseMovementGenerator<T> >
{
public:
    ChaseMovementGenerator(Unit& target, float offset, float angle)
        : TargetedMovementGeneratorMedium<T, ChaseMovementGenerator<T> >(target, offset, angle) {}
    ~ChaseMovementGenerator() {}

    MovementGeneratorType GetMovementGeneratorType() { return CHASE_MOTION_TYPE; }

    void Initialize(T&);
    void Finalize(T&);
    void Reset(T&);
    void MovementInform(T&);

    bool EnableWalking(T&) const { return false; }
    void _reachTarget(T&);
    void _updateSpeed(T&) {}
};

template<class T>
class FollowMovementGenerator : public TargetedMovementGeneratorMedium<T, FollowMovementGenerator<T> >
{
public:
    FollowMovementGenerator(Unit& target)
        : TargetedMovementGeneratorMedium<T, FollowMovementGenerator<T> >(target) {}
    FollowMovementGenerator(Unit& target, float offset, float angle)
        : TargetedMovementGeneratorMedium<T, FollowMovementGenerator<T> >(target, offset, angle) {}
    ~FollowMovementGenerator() {}

    MovementGeneratorType GetMovementGeneratorType() { return FOLLOW_MOTION_TYPE; }

    void Initialize(T&);
    void Finalize(T&);
    void Reset(T&);
    void MovementInform(T&);

    bool EnableWalking(T&) const;
    void _reachTarget(T&) {}
    void _updateSpeed(T&);
};

#endif
