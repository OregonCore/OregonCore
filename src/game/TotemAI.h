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

#ifndef OREGON_TOTEMAI_H
#define OREGON_TOTEMAI_H

#include "CreatureAI.h"
#include "Timer.h"

class Creature;
class Totem;

class TotemAI : public CreatureAI
{
    public:

        TotemAI(Creature* c);

        void MoveInLineOfSight(Unit*);
        void AttackStart(Unit*);
        void EnterEvadeMode();
        bool IsVisible(Unit*) const;

        void UpdateAI(const uint32);
        static int Permissible(const Creature*);

    private:
        Totem& i_totem;
        uint64 i_victimGuid;
};
#endif

