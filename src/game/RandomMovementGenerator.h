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

#ifndef OREGON_RANDOMMOTIONGENERATOR_H
#define OREGON_RANDOMMOTIONGENERATOR_H

#include "MovementGenerator.h"

// define chance for creature to not stop after reaching a waypoint
#define MOVEMENT_RANDOM_MMGEN_CHANCE_NO_BREAK 50

template<class T>
class RandomMovementGenerator
    : public MovementGeneratorMedium< T, RandomMovementGenerator<T> >
{
    public:
        // Wander dist is related on db spawn dist. So what if we wanna set random movement on summoned creature?!
        RandomMovementGenerator(float spawn_dist = 0.0f) : i_nextMoveTime(0), i_nextMove(0), wander_distance(spawn_dist) {}

        void _setRandomLocation(T&);
        void Initialize(T&);
        void Finalize(T&);
        void Reset(T&);
        bool Update(T&, const uint32&);
        void UpdateMapPosition(uint32 mapid, float& x , float& y, float& z)
        {
            this->i_destinationHolder.GetLocationNow(mapid, x, y, z);
        }
        MovementGeneratorType GetMovementGeneratorType()
        {
            return RANDOM_MOTION_TYPE;
        }
    private:
        TimeTrackerSmall i_nextMoveTime;

        float wander_distance;
        uint32 i_nextMove;
};
#endif

