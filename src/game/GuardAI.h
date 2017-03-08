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

#ifndef OREGON_GUARDAI_H
#define OREGON_GUARDAI_H

#include "ScriptedCreature.h"
#include "Timer.h"

class Creature;

class GuardAI : public ScriptedAI
{
        enum GuardState
        {
            STATE_NORMAL = 1,
            STATE_LOOK_AT_VICTIM = 2
        };

    public:
        explicit GuardAI(Creature* creature);

        void EnterEvadeMode();
        void JustDied(Unit*);

        static int Permissible(const Creature*);
        bool CanSeeAlways(WorldObject const* obj) override;
    private:
        uint64 i_victimGuid;
        GuardState i_state;
        TimeTracker i_tracker;
};
#endif

