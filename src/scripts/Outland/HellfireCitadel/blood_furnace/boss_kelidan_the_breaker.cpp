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

/* ScriptData
SDName: Boss_Kelidan_The_Breaker
SD%Complete: 100
SDComment:
SDCategory: Hellfire Citadel, Blood Furnace
EndScriptData */

/* ContentData
boss_kelidan_the_breaker
mob_shadowmoon_channeler
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "blood_furnace.h"
#include "SpellAuras.h"

enum eKelidan
{
    SAY_WAKE                    = -1542000,
    SAY_ADD_AGGRO_1             = -1542001,
    SAY_ADD_AGGRO_2             = -1542002,
    SAY_ADD_AGGRO_3             = -1542003,
    SAY_KILL_1                  = -1542004,
    SAY_KILL_2                  = -1542005,
    SAY_NOVA                    = -1542006,
    SAY_DIE                     = -1542007,

    SPELL_CORRUPTION            = 30938,
    SPELL_EVOCATION             = 30935,
    SPELL_FIRE_NOVA             = 33132,
    H_SPELL_FIRE_NOVA           = 37371,
    SPELL_SHADOW_BOLT_VOLLEY    = 28599,
    H_SPELL_SHADOW_BOLT_VOLLEY  = 40070,
    SPELL_BURNING_NOVA          = 30940,
    SPELL_VORTEX                = 37370,

    ENTRY_KELIDAN               = 17377,
    ENTRY_CHANNELER             = 17653,

    SPELL_SHADOW_BOLT           = 12739,
    H_SPELL_SHADOW_BOLT         = 15472,

    SPELL_MARK_OF_SHADOW = 30937,
    SPELL_CHANNELING = 39123,


    //events
    EVENT_SPELL_VOLLEY          = 1,
    EVENT_SPELL_CORRUPTION      = 2,
    EVENT_SPELL_BURNING_NOVA    = 3,
    EVENT_SPELL_FIRE_NOVA       = 4,
    EVENT_SPELL_SHADOW_BOLT     = 5,
    EVENT_SPELL_MARK            = 6,

    // Actions
    ACTION_CHANNELER_ENGAGED    = 1,
    ACTION_CHANNELER_DIED       = 2,

    ACTION_ACTIVATE_ADDS        = 92,
	
    SPELL_BUFF					             = 30939
};

const float ShadowmoonChannelers[5][4] =
{
    {302.0f, -87.0f, -24.4f, 0.157f},
    {321.0f, -63.5f, -24.6f, 4.887f},
    {346.0f, -74.5f, -24.6f, 3.595f},
    {344.0f, -103.5f, -24.5f, 2.356f},
    {316.0f, -109.0f, -24.6f, 1.257f}
};

class BurningNovaAura : public Aura
{
    public:
        BurningNovaAura(SpellEntry* spell, uint32 eff, Unit* pTarget, Unit* caster) : Aura(spell, eff, NULL, pTarget, caster, NULL) {}
};

struct boss_kelidan_the_breakerAI : public ScriptedAI
{
    boss_kelidan_the_breakerAI(Creature* creature) : ScriptedAI(creature)
    {
        instance = (ScriptedInstance*)creature->GetInstanceData();

        for (uint8 i = 0; i < 5; ++i)
            Channelers[i] = 0;
    }

    ScriptedInstance* instance;
    EventMap events;
    uint32 check_Timer;
    bool Firenova;
    bool addYell;
    uint64 Channelers[5];

    void Reset()
    {
        check_Timer = 0;
        Firenova = false;
        addYell = false;
        SummonChannelers();
        ApplyImunities(true);
        me->SetReactState(REACT_PASSIVE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        if (instance)
            instance->SetData(DATA_KELIDANEVENT, NOT_STARTED);
    }

    void ApplyImunities(bool apply)
    {
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, apply);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CONFUSED, apply);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISTRACT, apply);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, apply);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, apply);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SILENCE, apply);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SLEEP, apply);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, apply);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, apply);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, apply);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_KNOCKOUT, apply);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, apply);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, apply);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SHACKLE, apply);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, apply);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, apply);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DAZE, apply);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, apply);
    }

    void EnterCombat(Unit* who)
    {
        events.ScheduleEvent(EVENT_SPELL_VOLLEY, 1000);
        events.ScheduleEvent(EVENT_SPELL_CORRUPTION, 5000);
        events.ScheduleEvent(EVENT_SPELL_BURNING_NOVA, 15000);

        DoScriptText(SAY_WAKE, me);
        me->InterruptNonMeleeSpells(true);
        if (instance)
            instance->SetData(DATA_KELIDANEVENT, IN_PROGRESS);
    }

    void KilledUnit(Unit* /*victim*/)
    {
        if (rand() % 2)
            return;

        DoScriptText(RAND(SAY_KILL_1, SAY_KILL_2), me);
    }

    void DoAction(int32 param)
    {
        if (param == ACTION_CHANNELER_ENGAGED)
        {
            if (!addYell)
            {
                addYell = true;
                DoScriptText(RAND(SAY_ADD_AGGRO_1, SAY_ADD_AGGRO_2, SAY_ADD_AGGRO_3), me);

                for (uint8 i = 0; i < 5; ++i)
                {
                    Creature* channeler = ObjectAccessor::GetCreature(*me, Channelers[i]);
                    if (channeler && !channeler->IsInCombat())
                        DoZoneInCombat(channeler);
                }
            }
        }
        else if (param == ACTION_CHANNELER_DIED)
        {
            for (uint8 i = 0; i < 5; ++i)
            {
                Creature* channeler = ObjectAccessor::GetCreature(*me, Channelers[i]);
                if (channeler && channeler->IsAlive())
                    return;
            }
            me->SetReactState(REACT_AGGRESSIVE);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NON_ATTACKABLE);
            if (Unit* target = me->SelectNearestPlayer(100.0f))
                AttackStart(target);
        }
    }

    void CheckChannelers()
    {
        if (addYell)
        {
            if (!SelectTargetFromPlayerList(100.0f, NULL, false))
                EnterEvadeMode();
            return;
        }

        SummonChannelers();
        for (uint8 i = 0; i < 5; ++i)
        {
            Creature* channeler = ObjectAccessor::GetCreature(*me, Channelers[i]);
            if (channeler && !channeler->HasUnitState(UNIT_STATE_CASTING) && !channeler->IsInCombat())
            {
                Creature* target = ObjectAccessor::GetCreature(*me, Channelers[(i + 2) % 5]);
                if (target)
                    channeler->AI()->DoCast(target, SPELL_CHANNELING, false);
            }
        }
    }

    void SummonChannelers()
    {
        for (uint8 i = 0; i < 5; ++i)
        {
            Creature* channeler = ObjectAccessor::GetCreature(*me, Channelers[i]);
            if (channeler && channeler->isDead())
            {
                channeler->DespawnOrUnsummon(1);
                channeler = NULL;
            }
            if (!channeler)
                channeler = me->SummonCreature(NPC_CHANNELER, ShadowmoonChannelers[i][0], ShadowmoonChannelers[i][1], ShadowmoonChannelers[i][2], ShadowmoonChannelers[i][3], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 300000);

            Channelers[i] = channeler ? channeler->GetGUID() : 0;
        }
    }


    void JustDied(Unit* /*Killer*/)
    {
        DoScriptText(SAY_DIE, me);
        if (instance)
            instance->SetData(DATA_KELIDANEVENT, DONE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
        {
            check_Timer += diff;
            if (check_Timer >= 5000)
            {
                check_Timer = 0;
                CheckChannelers();
                if (!me->HasUnitState(UNIT_STATE_CASTING))
                    me->CastSpell(me, SPELL_EVOCATION, false);
            }
            return;
        }

        events.Update(diff);
        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        switch (events.ExecuteEvent())
        {
        case EVENT_SPELL_VOLLEY:
            me->CastSpell(me, SPELL_SHADOW_BOLT_VOLLEY, false);
            events.ScheduleEvent(EVENT_SPELL_VOLLEY, urand(8000, 13000));
            break;
        case EVENT_SPELL_CORRUPTION:
            me->CastSpell(me, SPELL_CORRUPTION, false);
            events.ScheduleEvent(EVENT_SPELL_CORRUPTION, urand(30000, 50000));
            break;
        case EVENT_SPELL_BURNING_NOVA:
            DoScriptText(SAY_NOVA, me);
            ApplyImunities(false);
            me->AddAura(SPELL_BURNING_NOVA, me);
            ApplyImunities(true);

            if (me->GetMap()->IsHeroic())
                DoTeleportAll(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());

            events.DelayEvents(6000, 0);
            events.ScheduleEvent(EVENT_SPELL_BURNING_NOVA, urand(25000, 32000));
            events.ScheduleEvent(EVENT_SPELL_FIRE_NOVA, 5000);
            break;
        case EVENT_SPELL_FIRE_NOVA:
            me->CastSpell(me, SPELL_FIRE_NOVA, true);
            events.ExecuteEvent();
            break;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_kelidan_the_breaker(Creature* pCreature)
{
    return new boss_kelidan_the_breakerAI (pCreature);
}

/*######
## mob_shadowmoon_channeler
######*/



struct mob_shadowmoon_channelerAI : public ScriptedAI
{
    mob_shadowmoon_channelerAI(Creature* creature) : ScriptedAI(creature)
    {
        instance = (ScriptedInstance*)creature->GetInstanceData();
    }

    ScriptedInstance* instance;

    EventMap events;

    void Reset()
    {
        events.Reset();
    }

    void EnterCombat(Unit* who)
    {
        if (Creature* kelidan = me->FindNearestCreature(ENTRY_KELIDAN, 100))
            kelidan->AI()->DoAction(ACTION_CHANNELER_ENGAGED);

        me->InterruptNonMeleeSpells(false);
        events.ScheduleEvent(EVENT_SPELL_SHADOW_BOLT, urand(1500, 3500));
        events.ScheduleEvent(EVENT_SPELL_MARK, urand(5000, 6500));
    }

    void JustDied(Unit* Killer)
    {
        if (Creature* Kelidan = me->FindNearestCreature(ENTRY_KELIDAN, 100))
            CAST_AI(boss_kelidan_the_breakerAI, Kelidan->AI())->DoAction(ACTION_CHANNELER_DIED);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        switch (events.ExecuteEvent())
        {
            case EVENT_SPELL_SHADOW_BOLT:
                me->CastSpell(me->GetVictim(), me->GetMap()->IsHeroic() ? H_SPELL_SHADOW_BOLT : SPELL_SHADOW_BOLT, false);
                events.ScheduleEvent(EVENT_SPELL_SHADOW_BOLT, urand(6000, 7500));
                break;
            case EVENT_SPELL_MARK:
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                    me->CastSpell(target, SPELL_MARK_OF_SHADOW, false);
                events.ScheduleEvent(EVENT_SPELL_MARK, urand(16000, 17500));
                break;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_shadowmoon_channeler(Creature* pCreature)
{
    return new mob_shadowmoon_channelerAI (pCreature);
}

struct npc_kelridan_triggerAI : public ScriptedAI
{
    ScriptedInstance* pInstance;

    uint32 buffTimer;

    npc_kelridan_triggerAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }

    void Reset()
    {
        buffTimer = 5000;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
        {
            if (buffTimer <= diff)
            {
                DoCast(me, SPELL_BUFF);
                buffTimer = 5000;
            }
            else buffTimer -= diff;
        }
    }
};

CreatureAI* GetAI_npc_kelridan_trigger(Creature* pCreature)
{
	return new npc_kelridan_triggerAI(pCreature);
}



void AddSC_boss_kelidan_the_breaker()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_kelidan_the_breaker";
    newscript->GetAI = &GetAI_boss_kelidan_the_breaker;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_shadowmoon_channeler";
    newscript->GetAI = &GetAI_mob_shadowmoon_channeler;
    newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_kelridan_trigger";
	newscript->GetAI = &GetAI_npc_kelridan_trigger;
    newscript->RegisterSelf();
}

