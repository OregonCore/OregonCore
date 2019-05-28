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
 * with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef OREGON_PETAI_H
#define OREGON_PETAI_H

#include "CreatureAI.h"
#include "Timer.h"

class Creature;
class Spell;

class PetAI : public CreatureAI
{
    public:

        explicit PetAI(Creature* c);

        void UpdateAI(const uint32) override;
        static int Permissible(const Creature*);

        void KilledUnit(Unit* victim) override;
        void AttackStart(Unit* target) override;
        void MovementInform(uint32 moveType, uint32 data) override;
        void OwnerAttackedBy(Unit* attacker) override;
        void OwnerAttacked(Unit* target) override;
        void AttackedBy(Unit* attacker) override;
        void DamageDealt(Unit* victim, uint32& damage, DamageEffectType damageType) override;
        void ClearCharmInfoFlags();

        // The following aren't used by the PetAI but need to be defined to override
        //  default CreatureAI functions which interfere with the PetAI
        //
        void MoveInLineOfSight(Unit* /*who*/) override {} // CreatureAI interferes with returning pets
        void EnterEvadeMode() override {} // For fleeing, pets don't use this type of Evade mechanic

    private:
        bool _needToStop() const;
        void _stopAttack();

        void UpdateAllies();

        TimeTracker i_tracker;
        std::set<uint64> m_AllySet;
        uint32 m_updateAlliesTimer;

        Unit* SelectNextTarget();
        void HandleReturnMovement();
        void DoAttack(Unit* target, bool chase);
        bool CanAttack(Unit* target);
};
#endif

