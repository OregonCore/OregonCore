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
SDName: Boss_Darkweaver_Syth
SD%Complete: 95
SDComment: Shock spells/times need more work. Heroic partly implemented.
SDCategory: Auchindoun, Sethekk Halls
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "sethekk_halls.h"

#define SAY_SUMMON                  -1556000

#define SAY_AGGRO_1                 -1556001
#define SAY_AGGRO_2                 -1556002
#define SAY_AGGRO_3                 -1556003

#define SAY_SLAY_1                  -1556004
#define SAY_SLAY_2                  -1556005

#define SAY_DEATH                   -1556006

#define SPELL_FROST_SHOCK           21401 //37865
#define SPELL_FLAME_SHOCK           34354
#define SPELL_SHADOW_SHOCK          30138
#define SPELL_ARCANE_SHOCK          37132

#define SPELL_CHAIN_LIGHTNING       15659 //15305

#define SPELL_SUMMON_SYTH_FIRE      33537                   // Spawns 19203
#define SPELL_SUMMON_SYTH_ARCANE    33538                   // Spawns 19205
#define SPELL_SUMMON_SYTH_FROST     33539                   // Spawns 19204
#define SPELL_SUMMON_SYTH_SHADOW    33540                   // Spawns 19206

#define SPELL_FLAME_BUFFET          (HeroicMode?38141:33526)
#define SPELL_ARCANE_BUFFET         (HeroicMode?38138:33527)
#define SPELL_FROST_BUFFET          (HeroicMode?38142:33528)
#define SPELL_SHADOW_BUFFET         (HeroicMode?38143:33529)

enum events
{
    // Darkweaver Syth
    EVENT_FLAME_SHOCK       = 1,
    EVENT_ARCANE_SHOCK      = 2,
    EVENT_FROST_SHOCK       = 3,
    EVENT_SHADOW_SHOCK      = 4,
    EVENT_CHAIN_LIGHTNING   = 5,
    EVENT_CHECK_HEALTH1     = 6,
    EVENT_CHECK_HEALTH2     = 7,
    EVENT_CHECK_HEALTH3     = 8,

    // Elementals
    EVENT_FLAME_BUFFET      = 9,
    EVENT_ARCANE_BUFFET     = 10,
    EVENT_FROST_BUFFET      = 11,
    EVENT_SHADOW_BUFFET     = 12
};

struct boss_darkweaver_sythAI : public ScriptedAI
{
    boss_darkweaver_sythAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
        HeroicMode = me->GetMap()->IsHeroic();
    }

    EventMap events;
    ScriptedInstance* pInstance;
    bool summon90, summon50, summon10;
    bool HeroicMode;
    Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);

    void Reset()
    {
        summon90 = false;
        summon50 = false;
        summon10 = false;

        if (pInstance)
            pInstance->SetData(DATA_SYTHEVENT, NOT_STARTED);
    }

    void EnterCombat(Unit*)
    {
        DoScriptText(RAND(SAY_AGGRO_1, SAY_AGGRO_2, SAY_AGGRO_3), me);

        if (pInstance)
            pInstance->SetData(DATA_SYTHEVENT, IN_PROGRESS);

        events.ScheduleEvent(EVENT_FLAME_SHOCK, 2000);
        events.ScheduleEvent(EVENT_ARCANE_SHOCK, 4000);
        events.ScheduleEvent(EVENT_FROST_SHOCK, 6000);
        events.ScheduleEvent(EVENT_SHADOW_SHOCK, 8000);
        events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, 15000);
        events.ScheduleEvent(EVENT_CHECK_HEALTH1, 500);
    }

    void JustDied(Unit*)
    {
        DoScriptText(SAY_DEATH, me);

        if (pInstance)
            pInstance->SetData(DATA_SYTHEVENT, DONE);
    }

    void KilledUnit(Unit*)
    {
        DoScriptText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
    }

    void JustSummoned(Creature* summoned)
    {
        if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
            summoned->AI()->AttackStart(pTarget);
    }

    void SythSummoning()
    {
        DoScriptText(SAY_SUMMON, me);

        if (me->IsNonMeleeSpellCast(false))
            me->InterruptNonMeleeSpells(false);

        DoCast(me, SPELL_SUMMON_SYTH_ARCANE, true); //front
        DoCast(me, SPELL_SUMMON_SYTH_FIRE, true);   //back
        DoCast(me, SPELL_SUMMON_SYTH_FROST, true);  //left
        DoCast(me, SPELL_SUMMON_SYTH_SHADOW, true); //right
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        switch (events.ExecuteEvent())
        {
        case EVENT_CHECK_HEALTH1:
            if (((me->GetHealth() * 100) / me->GetMaxHealth() < 90) && !summon90)
            {
                SythSummoning();
                summon90 = true;
                events.ScheduleEvent(EVENT_CHECK_HEALTH2, 500);
            }
            events.ScheduleEvent(EVENT_CHECK_HEALTH1, 500);
            break;
        case EVENT_CHECK_HEALTH2:
            if (((me->GetHealth() * 100) / me->GetMaxHealth() < 50) && !summon50)
            {
                SythSummoning();
                summon50 = true;
                events.ScheduleEvent(EVENT_CHECK_HEALTH3, 500);
            }
            events.ScheduleEvent(EVENT_CHECK_HEALTH2, 500);
            break;
        case EVENT_CHECK_HEALTH3:
            if (((me->GetHealth() * 100) / me->GetMaxHealth() < 10) && !summon10)
            {
                SythSummoning();
                summon10 = true;
            }
            events.ScheduleEvent(EVENT_CHECK_HEALTH3, 500);
            break;
        case EVENT_FLAME_SHOCK:
            DoCast(pTarget, SPELL_FLAME_SHOCK);
            events.ScheduleEvent(EVENT_FLAME_SHOCK, 10000 + rand() % 5000);
            break;
        case EVENT_ARCANE_SHOCK:
            DoCast(pTarget, SPELL_ARCANE_SHOCK);
            events.ScheduleEvent(EVENT_ARCANE_SHOCK, 10000 + rand() % 5000);
            break;
        case EVENT_FROST_SHOCK:
            DoCast(pTarget, SPELL_FROST_SHOCK);
            events.ScheduleEvent(EVENT_FROST_SHOCK, 10000 + rand() % 5000);
            break;
        case EVENT_SHADOW_SHOCK:
            DoCast(pTarget, SPELL_SHADOW_SHOCK);
            events.ScheduleEvent(EVENT_SHADOW_SHOCK, 10000 + rand() % 5000);
            break;
        case EVENT_CHAIN_LIGHTNING:
            DoCast(pTarget, SPELL_CHAIN_LIGHTNING);
            events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, 25000);
            break;

        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_darkweaver_syth(Creature* pCreature)
{
    return GetInstanceAI<boss_darkweaver_sythAI>(pCreature);
}

/* ELEMENTALS */

struct mob_syth_fireAI : public ScriptedAI
{
    mob_syth_fireAI(Creature* c) : ScriptedAI(c) {}

    EventMap events;
    Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);

    void Reset()
    {
        me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FIRE, true);
    }

    void EnterCombat(Unit*) 
    {
        events.ScheduleEvent(EVENT_FLAME_SHOCK, 2500);
        events.ScheduleEvent(EVENT_FLAME_BUFFET, 5000);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;
        events.Update(diff);

        switch (events.ExecuteEvent())
        {
        case EVENT_FLAME_SHOCK:
            DoCast(pTarget, SPELL_FLAME_SHOCK);
            events.ScheduleEvent(EVENT_FLAME_SHOCK, 5000);
            break;
        case EVENT_FLAME_BUFFET:
            DoCast(pTarget, SPELL_FLAME_BUFFET);
            events.ScheduleEvent(EVENT_FLAME_BUFFET, 5000);
            break;
        }
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_syth_fire(Creature* pCreature)
{
    return new mob_syth_fireAI (pCreature);
}

struct mob_syth_arcaneAI : public ScriptedAI
{
    mob_syth_arcaneAI(Creature* c) : ScriptedAI(c) { }

    EventMap events;
    Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);

    void Reset()
    {
        me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_ARCANE, true);
    }

    void EnterCombat(Unit*) 
    { 
        events.ScheduleEvent(EVENT_ARCANE_SHOCK, 2500);
        events.ScheduleEvent(EVENT_ARCANE_BUFFET, 5000);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        switch (events.ExecuteEvent())
        {
        case EVENT_ARCANE_SHOCK:
            DoCast(pTarget, SPELL_ARCANE_SHOCK);
            events.ScheduleEvent(EVENT_ARCANE_SHOCK, 5000);
            break;
        case EVENT_ARCANE_BUFFET:
            DoCast(pTarget, SPELL_ARCANE_BUFFET);
            events.ScheduleEvent(EVENT_ARCANE_BUFFET, 5000);
            break;

        }
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_syth_arcane(Creature* pCreature)
{
    return new mob_syth_arcaneAI (pCreature);
}

struct mob_syth_frostAI : public ScriptedAI
{
    mob_syth_frostAI(Creature* c) : ScriptedAI(c) {}

    EventMap events;
    Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);

    void Reset()
    {
        me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FROST, true);
    }

    void EnterCombat(Unit*) 
    { 
        events.ScheduleEvent(EVENT_FROST_SHOCK, 2500);
        events.ScheduleEvent(EVENT_FROST_BUFFET, 5000);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        switch (events.ExecuteEvent())
        {
        case EVENT_FROST_SHOCK:
            DoCast(pTarget, SPELL_FROST_SHOCK);
            events.ScheduleEvent(EVENT_FROST_SHOCK, 5000);
            break;
        case EVENT_FROST_BUFFET:
            DoCast(pTarget, SPELL_FROST_BUFFET);
            events.ScheduleEvent(EVENT_FROST_BUFFET, 5000);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_syth_frost(Creature* pCreature)
{
    return new mob_syth_frostAI (pCreature);
}

struct mob_syth_shadowAI : public ScriptedAI
{
    mob_syth_shadowAI(Creature* c) : ScriptedAI(c) {}

    EventMap events;
    Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);

    void Reset()
    {
        me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_SHADOW, true);
    }

    void EnterCombat(Unit*) 
    {
        events.ScheduleEvent(EVENT_SHADOW_SHOCK, 2500);
        events.ScheduleEvent(EVENT_SHADOW_BUFFET, 5000);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        switch (events.ExecuteEvent())
        {
        case EVENT_SHADOW_SHOCK:
            DoCast(pTarget, SPELL_SHADOW_SHOCK); 
            events.ScheduleEvent(EVENT_SHADOW_SHOCK, 5000);
            break;
        case EVENT_SHADOW_BUFFET:
            DoCast(pTarget, SPELL_SHADOW_BUFFET);
            events.ScheduleEvent(EVENT_SHADOW_BUFFET, 5000);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_syth_shadow(Creature* pCreature)
{
    return new mob_syth_shadowAI (pCreature);
}

void AddSC_boss_darkweaver_syth()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "boss_darkweaver_syth";
    newscript->GetAI = &GetAI_boss_darkweaver_syth;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_syth_fire";
    newscript->GetAI = &GetAI_mob_syth_arcane;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_syth_arcane";
    newscript->GetAI = &GetAI_mob_syth_arcane;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_syth_frost";
    newscript->GetAI = &GetAI_mob_syth_frost;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_syth_shadow";
    newscript->GetAI = &GetAI_mob_syth_shadow;
    newscript->RegisterSelf();
}

