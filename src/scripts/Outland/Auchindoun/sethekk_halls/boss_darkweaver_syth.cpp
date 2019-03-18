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

struct boss_darkweaver_sythAI : public ScriptedAI
{
    boss_darkweaver_sythAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
        HeroicMode = me->GetMap()->IsHeroic();
    }

    ScriptedInstance* pInstance;

    uint32 flameshock_timer;
    uint32 arcaneshock_timer;
    uint32 frostshock_timer;
    uint32 shadowshock_timer;
    uint32 chainlightning_timer;

    bool summon90;
    bool summon50;
    bool summon10;
    bool HeroicMode;

    void Reset()
    {
        flameshock_timer = 2000;
        arcaneshock_timer = 4000;
        frostshock_timer = 6000;
        shadowshock_timer = 8000;
        chainlightning_timer = 15000;

        summon90 = false;
        summon50 = false;
        summon10 = false;

        if (pInstance)
            pInstance->SetData(DATA_SYTHEVENT, NOT_STARTED);
    }

    void EnterCombat(Unit*)
    {
        switch (rand() % 3)
        {
        case 0:
            DoScriptText(SAY_AGGRO_1, me);
            break;
        case 1:
            DoScriptText(SAY_AGGRO_2, me);
            break;
        case 2:
            DoScriptText(SAY_AGGRO_3, me);
            break;
        }

        if (pInstance)
            pInstance->SetData(DATA_SYTHEVENT, IN_PROGRESS);
    }

    void JustDied(Unit*)
    {
        DoScriptText(SAY_DEATH, me);

        if (pInstance)
            pInstance->SetData(DATA_SYTHEVENT, DONE);
    }

    void KilledUnit(Unit*)
    {
        if (rand() % 2)
            return;

        switch (rand() % 2)
        {
        case 0:
            DoScriptText(SAY_SLAY_1, me);
            break;
        case 1:
            DoScriptText(SAY_SLAY_2, me);
            break;
        }
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

        if (((me->GetHealth() * 100) / me->GetMaxHealth() < 90) && !summon90)
        {
            SythSummoning();
            summon90 = true;
        }

        if (((me->GetHealth() * 100) / me->GetMaxHealth() < 50) && !summon50)
        {
            SythSummoning();
            summon50 = true;
        }

        if (((me->GetHealth() * 100) / me->GetMaxHealth() < 10) && !summon10)
        {
            SythSummoning();
            summon10 = true;
        }

        if (flameshock_timer <= diff)
        {
            if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                DoCast(pTarget, SPELL_FLAME_SHOCK);

            flameshock_timer = 10000 + rand() % 5000;
        }
        else flameshock_timer -= diff;

        if (arcaneshock_timer <= diff)
        {
            if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                DoCast(pTarget, SPELL_ARCANE_SHOCK);

            arcaneshock_timer = 10000 + rand() % 5000;
        }
        else arcaneshock_timer -= diff;

        if (frostshock_timer <= diff)
        {
            if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                DoCast(pTarget, SPELL_FROST_SHOCK);

            frostshock_timer = 10000 + rand() % 5000;
        }
        else frostshock_timer -= diff;

        if (shadowshock_timer <= diff)
        {
            if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                DoCast(pTarget, SPELL_SHADOW_SHOCK);

            shadowshock_timer = 10000 + rand() % 5000;
        }
        else shadowshock_timer -= diff;

        if (chainlightning_timer <= diff)
        {
            if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                DoCast(pTarget, SPELL_CHAIN_LIGHTNING);

            chainlightning_timer = 25000;
        }
        else chainlightning_timer -= diff;

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
    mob_syth_fireAI(Creature* c) : ScriptedAI(c)

    {
        HeroicMode = me->GetMap()->IsHeroic();
    }

    uint32 flameshock_timer;
    uint32 flamebuffet_timer;
    bool HeroicMode;

    void Reset()
    {
        me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FIRE, true);
        flameshock_timer = 2500;
        flamebuffet_timer = 5000;
    }

    void EnterCombat(Unit*) { }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (flameshock_timer <= diff)
        {
            if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                DoCast(pTarget, SPELL_FLAME_SHOCK);

            flameshock_timer = 5000;
        }
        else flameshock_timer -= diff;

        if (flamebuffet_timer <= diff)
        {
            if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                DoCast(pTarget, SPELL_FLAME_BUFFET);

            flamebuffet_timer = 5000;
        }
        else flamebuffet_timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_syth_fire(Creature* pCreature)
{
    return new mob_syth_fireAI (pCreature);
}

struct mob_syth_arcaneAI : public ScriptedAI
{
    mob_syth_arcaneAI(Creature* c) : ScriptedAI(c)

    {
        HeroicMode = me->GetMap()->IsHeroic();
    }

    uint32 arcaneshock_timer;
    uint32 arcanebuffet_timer;
    bool HeroicMode;

    void Reset()
    {
        me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_ARCANE, true);
        arcaneshock_timer = 2500;
        arcanebuffet_timer = 5000;
    }

    void EnterCombat(Unit*) { }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (arcaneshock_timer <= diff)
        {
            if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                DoCast(pTarget, SPELL_ARCANE_SHOCK);

            arcaneshock_timer = 5000;
        }
        else arcaneshock_timer -= diff;

        if (arcanebuffet_timer <= diff)
        {
            if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                DoCast(pTarget, SPELL_ARCANE_BUFFET);

            arcanebuffet_timer = 5000;
        }
        else arcanebuffet_timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_syth_arcane(Creature* pCreature)
{
    return new mob_syth_arcaneAI (pCreature);
}

struct mob_syth_frostAI : public ScriptedAI
{
    mob_syth_frostAI(Creature* c) : ScriptedAI(c)

    {
        HeroicMode = me->GetMap()->IsHeroic();
    }

    uint32 frostshock_timer;
    uint32 frostbuffet_timer;
    bool HeroicMode;

    void Reset()
    {
        me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FROST, true);
        frostshock_timer = 2500;
        frostbuffet_timer = 5000;
    }

    void EnterCombat(Unit*) { }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (frostshock_timer <= diff)
        {
            if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                DoCast(pTarget, SPELL_FROST_SHOCK);

            frostshock_timer = 5000;
        }
        else frostshock_timer -= diff;

        if (frostbuffet_timer <= diff)
        {
            if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                DoCast(pTarget, SPELL_FROST_BUFFET);

            frostbuffet_timer = 5000;
        }
        else frostbuffet_timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_syth_frost(Creature* pCreature)
{
    return new mob_syth_frostAI (pCreature);
}

struct mob_syth_shadowAI : public ScriptedAI
{
    mob_syth_shadowAI(Creature* c) : ScriptedAI(c)

    {
        HeroicMode = me->GetMap()->IsHeroic();
    }

    uint32 shadowshock_timer;
    uint32 shadowbuffet_timer;
    bool HeroicMode;

    void Reset()
    {
        me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_SHADOW, true);
        shadowshock_timer = 2500;
        shadowbuffet_timer = 5000;
    }

    void EnterCombat(Unit*) { }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (shadowshock_timer <= diff)
        {
            if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                DoCast(pTarget, SPELL_SHADOW_SHOCK);

            shadowshock_timer = 5000;
        }
        else shadowshock_timer -= diff;

        if (shadowbuffet_timer <= diff)
        {
            if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                DoCast(pTarget, SPELL_SHADOW_BUFFET);

            shadowbuffet_timer = 5000;
        }
        else shadowbuffet_timer -= diff;

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

