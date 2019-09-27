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

#ifndef OREGON_FLEEINGMOVEMENTGENERATOR_H
#define OREGON_FLEEINGMOVEMENTGENERATOR_H

#include "MovementGenerator.h"
#include "MapManager.h"

template<class T>
class FleeingMovementGenerator
    : public MovementGeneratorMedium< T, FleeingMovementGenerator<T> >
{
    public:
        FleeingMovementGenerator(uint64 fright) : i_frightGUID(fright), i_nextCheckTime(0) {}

        void Initialize(T&);
        void Finalize(T&);
        void Reset(T&);
        bool Update(T&, const uint32&);

        MovementGeneratorType GetMovementGeneratorType()
        {
            return FLEEING_MOTION_TYPE;
        }

    private:
        void _setTargetLocation(T& owner);
        void _getPoint(T& owner, float& x, float& y, float& z);

        uint64 i_frightGUID;
        TimeTracker i_nextCheckTime;
};

class TimedFleeingMovementGenerator
    : public FleeingMovementGenerator<Creature>
{
    public:
        TimedFleeingMovementGenerator(uint64 fright, uint32 time) :
            FleeingMovementGenerator<Creature>(fright),
            i_totalFleeTime(time) {}

        MovementGeneratorType GetMovementGeneratorType()
        {
            return TIMED_FLEEING_MOTION_TYPE;
        }
        bool Update(Unit&, const uint32&);
        void Finalize(Unit&);

    private:
        TimeTracker i_totalFleeTime;
};

#endif

