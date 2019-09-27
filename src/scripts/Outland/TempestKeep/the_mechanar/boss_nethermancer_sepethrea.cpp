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
SDName: Boss_Nethermancer_Sepethrea
SD%Complete: 90
SDComment: Need adjustments to initial summons
SDCategory: Tempest Keep, The Mechanar
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "mechanar.h"

#define SAY_AGGRO                       -1554013
#define SAY_SUMMON                      -1554014
#define SAY_DRAGONS_BREATH_1            -1554015
#define SAY_DRAGONS_BREATH_2            -1554016
#define SAY_SLAY1                       -1554017
#define SAY_SLAY2                       -1554018
#define SAY_DEATH                       -1554019

#define SPELL_SUMMON_RAGIN_FLAMES       35275
#define H_SPELL_SUMMON_RAGIN_FLAMES     39084

#define SPELL_FROST_ATTACK              35263
#define SPELL_ARCANE_BLAST              35314
#define SPELL_DRAGONS_BREATH            35250
#define SPELL_KNOCKBACK                 37317
#define SPELL_SOLARBURN                 35267

enum NetherMancerSepthrea
{
    //npcs
    NPC_RAGING_FLAMES               = 20481,

    //events
    EVENT_FROST_ATTACK              = 1,
    EVENT_ARCANE_BLAST              = 2,
    EVENT_DRAGONS_BREATH            = 3,
    EVENT_KNOCK_BACK                = 4,
    EVENT_SOLAR_BURN                = 5
};

struct boss_nethermancer_sepethreaAI : public BossAI
{
    boss_nethermancer_sepethreaAI(Creature* c) : BossAI(c, DATA_NETHERMANCER_SEPRETHREA) {}

    bool isHeroic = me->GetMap()->IsHeroic();

    void EnterCombat(Unit* who)
    {
        _EnterCombat();
        
        events.ScheduleEvent(EVENT_FROST_ATTACK, 6000);
        events.ScheduleEvent(EVENT_ARCANE_BLAST, 14000);
        events.ScheduleEvent(EVENT_DRAGONS_BREATH, 18000);

        DoScriptText(SAY_AGGRO, me);

        if (isHeroic)
        {
            DoCast(H_SPELL_SUMMON_RAGIN_FLAMES);
            me->SummonCreature(20481, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 300);
        }
        else
            DoCast(SPELL_SUMMON_RAGIN_FLAMES);

        DoScriptText(SAY_SUMMON, me);
    }

    void JustSummoned(Creature* summon)
    {
        summons.Summon(summon);
        if (Unit* victim = me->GetVictim())
        {
            summon->AI()->AttackStart(victim);
            summon->AddThreat(victim, 1000.0f);
            summon->SetInCombatWithZone();
        }
    }

    void KilledUnit(Unit* victim)
    {
        if (victim->GetTypeId() == TYPEID_PLAYER)
        {
            DoScriptText(RAND(SAY_SLAY1, SAY_SLAY2), me);
        }
    }

    void JustDied(Unit* /*Killer*/)
    {
        events.Reset();
        DoScriptText(SAY_DEATH, me);

        if (instance)
            instance->SetData(DATA_NETHERMANCER_SEPRETHREA, DONE);

        for (SummonList::const_iterator itr = summons.begin(); itr != summons.end(); ++itr)
            if (Creature* summon = ObjectAccessor::GetCreature(*me, *itr))
                me->DealDamage(summon, summon->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NONE, NULL, false);
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!UpdateVictim())
            return;

        events.Update(diff);
        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;


        switch (events.ExecuteEvent())
        {
        case EVENT_FROST_ATTACK:
            DoCastVictim(SPELL_FROST_ATTACK);
            events.ScheduleEvent(EVENT_FROST_ATTACK, 8000);
            break;
        case EVENT_ARCANE_BLAST:
            DoCastVictim(SPELL_ARCANE_BLAST);
            events.ScheduleEvent(EVENT_ARCANE_BLAST, 12000);
            break;
        case EVENT_DRAGONS_BREATH:
            DoCastVictim(SPELL_DRAGONS_BREATH);
            events.ScheduleEvent(EVENT_DRAGONS_BREATH, 16000);
            if (roll_chance_i(50))
                DoScriptText(RAND(SAY_DRAGONS_BREATH_1, SAY_DRAGONS_BREATH_2), me);
            break;
        case EVENT_KNOCK_BACK:
            DoCastVictim(SPELL_KNOCKBACK);
            events.ScheduleEvent(EVENT_KNOCK_BACK, 15000 + rand() % 10000);
            break;
        case EVENT_SOLAR_BURN:
            DoCastVictim(SPELL_SOLARBURN);
            events.ScheduleEvent(EVENT_SOLAR_BURN, 30000);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_nethermancer_sepethrea(Creature* pCreature)
{
    return GetInstanceAI<boss_nethermancer_sepethreaAI>(pCreature);
}

#define SPELL_INFERNO                   35268
#define SPELL_FIRE_TAIL                 35278
#define H_SPELL_INFERNO                 39346

enum raginFlames
{
    EVENT_SPELL_FIRE_TAIL = 1,
    EVENT_SPELL_INFERNO = 2
};


struct mob_ragin_flamesAI : public ScriptedAI
{
    mob_ragin_flamesAI(Creature* c) : ScriptedAI(c)
    {
    }
    
    EventMap events;
    bool isHeroic = me->GetMap()->IsHeroic();

    void Reset()
    {
        me->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_ALL, true);
        me->SetSpeed(MOVE_RUN, HeroicMode ? 0.7f : 0.5f);
    }

    void EnterCombat(Unit* /*who*/)
    {
        events.ScheduleEvent(EVENT_SPELL_FIRE_TAIL, 500);
        events.ScheduleEvent(EVENT_SPELL_INFERNO, urand(10000, 20000));
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        switch (events.ExecuteEvent())
        {
        case EVENT_SPELL_INFERNO:
            if (me->IsWithinCombatRange(me->GetVictim(), 5.0f))
            {
                me->CastSpell(me, SPELL_INFERNO, true);
                events.ScheduleEvent(EVENT_SPELL_INFERNO, 20000);
            }
            else
                events.ScheduleEvent(EVENT_SPELL_INFERNO, 1000);
            break;
        case EVENT_SPELL_FIRE_TAIL:
            me->CastSpell(me, SPELL_FIRE_TAIL, true);
            events.ScheduleEvent(EVENT_SPELL_FIRE_TAIL, 500);
            break;
        }
        DoMeleeAttackIfReady();
    }

};
CreatureAI* GetAI_mob_ragin_flames(Creature* pCreature)
{
    return GetInstanceAI<mob_ragin_flamesAI>(pCreature);
}

void AddSC_boss_nethermancer_sepethrea()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "boss_nethermancer_sepethrea";
    newscript->GetAI = &GetAI_boss_nethermancer_sepethrea;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_ragin_flames";
    newscript->GetAI = &GetAI_mob_ragin_flames;
    newscript->RegisterSelf();
}

