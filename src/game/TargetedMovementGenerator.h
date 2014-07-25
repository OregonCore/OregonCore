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

#ifndef OREGON_TARGETEDMOVEMENTGENERATOR_H
#define OREGON_TARGETEDMOVEMENTGENERATOR_H

#include "MovementGenerator.h"
#include "DestinationHolder.h"
#include "Traveller.h"
#include "FollowerReference.h"
#include "PathFinder.h"
 
class TargetedMovementGeneratorBase
{
    public:
        TargetedMovementGeneratorBase(Unit &target) { i_target.link(&target, this); }
        void stopFollowing() { }
    protected:
        FollowerReference i_target;
};

template<class T>
class TargetedMovementGenerator
: public MovementGeneratorMedium< T, TargetedMovementGenerator<T> >, public TargetedMovementGeneratorBase
{
    public:
        TargetedMovementGenerator(Unit &target, float offset = 0, float angle = 0, bool _usePathfinding = true);
        ~TargetedMovementGenerator() {delete i_path;}
 
        void Initialize(T &);
        void Finalize(T &);
        void Reset(T &);
        bool Update(T &, const uint32 &);
        MovementGeneratorType GetMovementGeneratorType() { return TARGETED_MOTION_TYPE; }

        void MovementInform(T &);

        Unit* GetTarget() const;

        bool GetDestination(float &x, float &y, float &z) const
        {
            if (i_destinationHolder.HasArrived() || !i_destinationHolder.HasDestination()) return false;
            i_destinationHolder.GetDestination(x,y,z);
            return true;
        }
 
        bool IsReachable() const
        {
            return (i_path) ? (i_path->getPathType() & PATHFIND_NORMAL) : true;
        }
 
        void unitSpeedChanged() { i_recalculateTravel=true; }
        void UpdateFinalDistance(float fDistance);
    private:

        bool _setTargetLocation(T &);

        float i_offset;
        float i_angle;
        DestinationHolder< Traveller<T> > i_destinationHolder;
        bool i_recalculateTravel;
        float i_targetX, i_targetY, i_targetZ;
        bool m_usePathfinding;
        PathInfo *i_path;
        uint32 m_pathPointsSent;
};
#endif

