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

#ifndef OREGONCORE_TOTEM_H
#define OREGONCORE_TOTEM_H

#include "TemporarySummon.h"

enum TotemType
{
    TOTEM_PASSIVE    = 0,
    TOTEM_ACTIVE     = 1,
};

#define SENTRY_TOTEM_ENTRY  3968

class Totem : public Minion
{
    public:
        explicit Totem(SummonPropertiesEntry const* properties, Unit* owner);
        ~Totem() override {};
        void Update(uint32 time) override;
        void InitStats(uint32 duration) override;
        void InitSummon() override;
        void UnSummon();
        uint32 GetSpell(uint8 slot = 0) const { return m_spells[slot]; }
        uint32 GetTotemDuration() const
        {
            return m_duration;
        }
        TotemType GetTotemType() const
        {
            return m_type;
        }

        bool UpdateStats(Stats /*stat*/) override
        {
            return true;
        }
        bool UpdateAllStats() override
        {
            return true;
        }
        void UpdateResistances(uint32 /*school*/) override {}
        void UpdateArmor() override {}
        void UpdateMaxHealth() override {}
        void UpdateMaxPower(Powers /*power*/) override {}
        void UpdateAttackPowerAndDamage(bool /*ranged*/) override {}
        void UpdateDamagePhysical(WeaponAttackType /*attType*/) override {}

        bool IsImmuneToSpellEffect(SpellEntry const* spellInfo, uint32 index, bool castOnSelf) const override;

    protected:
        TotemType m_type;
        uint32 m_duration;
};
#endif

