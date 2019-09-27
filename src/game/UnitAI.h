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

#ifndef OREGON_UNITAI_H
#define OREGON_UNITAI_H

#include "Platform/Define.h"
#include <list>
#include "Unit.h"
#include "QuestDef.h"
#include "Utilities/EventMap.h"

class Unit;
class Player;
struct AISpellInfoType;

//Selection method used by SelectTarget
enum SelectAggroTarget
{
    SELECT_TARGET_RANDOM = 0,                               //Just selects a random target
    SELECT_TARGET_TOPAGGRO,                                 //Selects targes from top aggro to bottom
    SELECT_TARGET_BOTTOMAGGRO,                              //Selects targets from bottom aggro to top
    SELECT_TARGET_NEAREST,
    SELECT_TARGET_FARTHEST,
};

class UnitAI
{
    protected:
        Unit* const me;
    public:
        explicit UnitAI(Unit* u) : me(u) {}
        virtual ~UnitAI() {}

        virtual bool CanAIAttack(const Unit* /*who*/) const
        {
            return true;
        }
        virtual void AttackStart(Unit*);
        virtual void UpdateAI(const uint32 /*diff*/) = 0;

        virtual void InitializeAI()
        {
            if (!me->isDead()) Reset();
        }

        virtual void Reset() {};

        virtual void EventHappens(uint32) {}
        virtual void ScheduleEvent(uint32 data, uint32 delay);

        // Called when unit is charmed
        virtual void OnCharmed(bool /*apply*/) = 0;

        // Pass parameters between AI
        virtual void DoAction(const int32 param = 0)
        {
            UNUSED(param);
        }
        virtual uint32 GetData(uint32 id = 0)
        {
            UNUSED(id);
            return 0;
        }
        virtual void SetData(uint32 /*id*/, uint32 /*value*/) {}
        virtual void SetGUID(const uint64& /*guid*/, int32 id = 0)
        {
            UNUSED(id);
        }
        virtual uint64 GetGUID(int32 id = 0)
        {
            UNUSED(id);
            return 0;
        }

        // Called at any Damage to any victim (before damage apply)
        virtual void DamageDealt(Unit* /*victim*/, uint32& /*damage*/, DamageEffectType /*damageType*/) { }

        Unit* SelectTarget(SelectAggroTarget target, uint32 position = 0, float dist = 0, bool playerOnly = false, int32 aura = 0);
        void SelectTargetList(std::list<Unit*>& targetList, uint32 num, SelectAggroTarget target, float dist = 0, bool playerOnly = false, int32 aura = 0);

        void AttackStartCaster(Unit* victim, float dist);

        // Called when the creature receives heal
        virtual void HealReceived(Unit* /*done_by*/, uint32& /*addhealth*/) { }

        void DoCast(uint32 spellId);
        void DoCast(Unit* victim, uint32 spellId, bool triggered = false);
        void DoCastSelf(uint32 spellId, bool triggered = false) { DoCast(me, spellId, triggered); }
        void DoCastVictim(uint32 spellId, bool triggered = false);
        void DoCastAOE(uint32 spellId, bool triggered = false);

        float DoGetSpellMaxRange(uint32 spellId, bool positive = false);

        void DoMeleeAttackIfReady();
        bool DoSpellAttackIfReady(uint32 spell);

        virtual void sGossipHello(Player* /*player*/) { }
        virtual void sGossipSelect(Player* /*player*/, uint32 /*sender*/, uint32 /*action*/) { }
        virtual void sGossipSelectCode(Player* /*player*/, uint32 /*sender*/, uint32 /*action*/, char const* /*code*/) { }
        virtual void sQuestAccept(Player* /*player*/, Quest const* /*quest*/) { }
        virtual void sQuestSelect(Player* /*player*/, Quest const* /*quest*/) { }
        virtual void sQuestReward(Player* /*player*/, Quest const* /*quest*/, uint32 /*opt*/) { }
        virtual bool sOnDummyEffect(Unit* /*caster*/, uint32 /*spellId*/, SpellEffIndex /*effIndex*/) { return false; }
        virtual void sOnGameEvent(bool /*start*/, uint16 /*eventId*/) { }

        static AISpellInfoType* AISpellInfo;
        static void FillAISpellInfo();
        static void ClearAISpellInfo();
};

class PlayerAI : public UnitAI
{
    protected:
        Player* const me;
    public:
        explicit PlayerAI(Player* p) : UnitAI((Unit*)p), me(p) {}

        void OnCharmed(bool apply) override;
};

class SimpleCharmedAI : public PlayerAI
{
    public:
        void UpdateAI(const uint32 diff) override;
};

class ScriptEvent : public BasicEvent
{
    public:
        ScriptEvent(Unit* unit, uint32 data = 0) : BasicEvent(), m_unit(unit), m_data(data) {}
        bool Execute(uint64, uint32) override
        {
            if (m_unit->IsAIEnabled)
                m_unit->i_AI->EventHappens(m_data);
            return true;
        }

    private:
        Unit* m_unit;
        uint32 m_data;
};

inline void UnitAI::ScheduleEvent(uint32 data, uint32 delay)
{
    me->m_Events.AddEvent(new ScriptEvent(me, data), me->m_Events.CalculateTime(delay), false);
}

#endif

