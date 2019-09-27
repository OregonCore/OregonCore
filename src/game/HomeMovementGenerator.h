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

#ifndef OREGON_HOMEMOVEMENTGENERATOR_H
#define OREGON_HOMEMOVEMENTGENERATOR_H

#include "MovementGenerator.h"
#include "PathFinder.h"

class Creature;

template < class T >
class HomeMovementGenerator;

template <>
class HomeMovementGenerator<Creature>
    : public MovementGeneratorMedium< Creature, HomeMovementGenerator<Creature> >
{
    public:

        HomeMovementGenerator() : skipToHome(false) {}
        ~HomeMovementGenerator() {}

        void Initialize(Creature&);
        void Finalize(Creature &);
        void Reset(Creature&);
        bool Update(Creature&, const uint32&);
        MovementGeneratorType GetMovementGeneratorType()
        {
            return HOME_MOTION_TYPE;
        }

    private:
        void _setTargetLocation(Creature&);

        float ori;
        bool arrived;
        bool skipToHome;
};
#endif

