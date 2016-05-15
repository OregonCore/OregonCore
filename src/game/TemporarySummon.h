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

#ifndef OREGONCORE_TEMPSUMMON_H
#define OREGONCORE_TEMPSUMMON_H

#include "Creature.h"

enum SummonerType
{
    SUMMONER_TYPE_CREATURE = 0,
    SUMMONER_TYPE_GAMEOBJECT = 1,
    SUMMONER_TYPE_MAP = 2
};

/// Stores data for temp summons
struct TempSummonData
{
    uint32 entry;        ///< Entry of summoned creature
    Position pos;        ///< Position, where should be creature spawned
    TempSummonType type; ///< Summon type, see TempSummonType for available types
    uint32 time;         ///< Despawn time, usable only with certain temp summon types
};

class TempSummon : public Creature
{
    public:
        explicit TempSummon(SummonPropertiesEntry const* properties, Unit* owner, bool isWorldObject);
        ~TempSummon() override {};
        void Update(uint32 time) override;
        virtual void InitStats(uint32 lifetime);
        virtual void InitSummon();
        void UnSummon(uint32 msTime = 0);
        void RemoveFromWorld() override;
        void SetTempSummonType(TempSummonType type);
        TempSummonType GetTempSummonType() const
        {
            return m_type;
        }
        void SaveToDB() override;
        Unit* GetSummoner() const;
        void SetSummoner(Unit* summoner) { m_summonerGUID = summoner->GetGUID(); }

        SummonPropertiesEntry const* m_Properties;
    private:
        TempSummonType m_type;
        uint32 m_timer;
        uint32 m_lifetime;
        uint64 m_summonerGUID;
};

class Minion : public TempSummon
{
    public:
        Minion(SummonPropertiesEntry const* properties, Unit* owner, bool isWorldObject);
        void InitStats(uint32 duration) override;
        void RemoveFromWorld() override;
        Unit* GetOwner()
        {
            return m_owner;
        }
        float GetFollowAngle() const override
        {
            return m_followAngle;
        }
        void SetFollowAngle(float angle)
        {
            m_followAngle = angle;
        }
        bool IsGuardianPet() const;
    protected:
        Unit* const m_owner;
        float m_followAngle;
};

class Guardian : public Minion
{
    public:
        Guardian(SummonPropertiesEntry const* properties, Unit* owner, bool isWorldObject);
        void InitStats(uint32 duration) override;
        bool InitStatsForLevel(uint32 level);
        void InitSummon() override;

        bool UpdateStats(Stats stat) override;
        bool UpdateAllStats() override;
        void UpdateResistances(uint32 school) override;
        void UpdateArmor() override;
        void UpdateMaxHealth() override;
        void UpdateMaxPower(Powers power) override;
        void UpdateAttackPowerAndDamage(bool ranged = false) override;
        void UpdateDamagePhysical(WeaponAttackType attType) override;

        int32 GetBonusDamage()
        {
            return m_bonusdamage;
        }
        void SetBonusDamage(int32 damage)
        {
            m_bonusdamage = damage;
        }
    protected:
        int32   m_bonusdamage;
};

class Puppet : public Minion
{
    public:
        Puppet(SummonPropertiesEntry const* properties, Unit* owner);
        void InitStats(uint32 duration) override;
        void InitSummon() override;
        void Update(uint32 time) override;
        void RemoveFromWorld() override;
    protected:
        Player* m_owner;
};

class UnsummonDelayEvent : public BasicEvent
{
public:
    UnsummonDelayEvent(Creature& owner) : BasicEvent(), m_owner(owner) { }
    bool Execute(uint64 e_time, uint32 p_time) override;

private:
    Creature& m_owner;
};

#endif

