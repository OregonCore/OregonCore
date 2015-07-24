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

#ifndef OREGON_CONFUSEDGENERATOR_H
#define OREGON_CONFUSEDGENERATOR_H

#include "MovementGenerator.h"

template<class T>
class ConfusedMovementGenerator
    : public MovementGeneratorMedium< T, ConfusedMovementGenerator<T> >
{
    public:
        explicit ConfusedMovementGenerator() : i_nextMoveTime(0), i_x(0), i_y(0), i_z(0) {}

        void Initialize(T&);
        void Finalize(T&);
        void Reset(T&);
        bool Update(T&, const uint32&);

        MovementGeneratorType GetMovementGeneratorType()
        {
            return CONFUSED_MOTION_TYPE;
        }
    private:
        TimeTracker i_nextMoveTime;
        float i_x, i_y, i_z;
};
#endif

