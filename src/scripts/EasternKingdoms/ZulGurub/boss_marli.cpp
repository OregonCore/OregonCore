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
SDName: Boss_Marli
SD%Complete: 95
SDComment: Unknown correct Thrash timer. Threat not resetting for entire raid upon charge (core issue?)
SDCategory: Zul'Gurub
EndScriptData */

#include "ScriptPCH.h"
#include "zulgurub.h"

enum Texts
{
    SAY_AGGRO           =    -1309005,
    SAY_TRANSFORM       =    -1309006,
    SAY_SPIDER_SPAWN    =    -1309007,
    SAY_DEATH           =    -1309008
};

enum Spells
{
    SPELL_CHARGE           =  22911,
    SPELL_ASPECT_OF_MARLI  =  24686,    // A stun spell
    SPELL_ENVOLWINGWEB     =  24110,
    SPELL_POISONVOLLEY     =  24099,
    SPELL_SPIDER_FORM      =  24084,
    SPELL_DRAIN_LIFE       =  24300,
    SPELL_THRASH           =  3391,

    // Spider Spell
    SPELL_LEVELUP          =  24312     // Most likely not right spell
};

struct boss_marliAI : public ScriptedAI
{
    boss_marliAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }

    ScriptedInstance* pInstance;

    uint32 SpawnStartSpiders_Timer;
    uint32 PoisonVolley_Timer;
    uint32 SpawnSpider_Timer;
    uint32 Charge_Timer;
    uint32 Aspect_Timer;
    uint32 DrainLife_Timer;
    uint32 Thrash_Timer;
    uint32 Transform_Timer;
    uint32 TransformBack_Timer;

    bool Spawned;
    bool PhaseTwo;

    void Reset()
    {
        SpawnStartSpiders_Timer = 1000;
        PoisonVolley_Timer = 15000;
        SpawnSpider_Timer = 30000;
        Charge_Timer = 1500;
        Aspect_Timer = 12000;
        DrainLife_Timer = 20000;
        Thrash_Timer = urand(5000, 9000);
        Transform_Timer = 45000;
        TransformBack_Timer = 45000;

        Spawned = false;
        PhaseTwo = false;
    }

    void EnterCombat(Unit* /*who*/)
    {
        DoScriptText(SAY_AGGRO, me);
    }

    void JustDied(Unit* /*Killer*/)
    {
        DoScriptText(SAY_DEATH, me);
        if (pInstance)
            pInstance->SetData(TYPE_MARLI, DONE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (me->getVictim() && me->IsAlive())
        {
            if (!PhaseTwo && DrainLife_Timer <= diff)
            {
                Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);
                DoCast(pTarget, SPELL_DRAIN_LIFE);
                DrainLife_Timer = 20000;
            }
            else
                DrainLife_Timer -= diff;

            if (PoisonVolley_Timer <= diff)
            {
                DoCastVictim( SPELL_POISONVOLLEY);
                PoisonVolley_Timer = 10000 + rand() % 10000;
            }
            else
                PoisonVolley_Timer -= diff;

            if (!PhaseTwo && Aspect_Timer <= diff)
            {
                DoCastVictim( SPELL_ASPECT_OF_MARLI);
                Aspect_Timer = 13000 + rand() % 5000;
            }
            else
                Aspect_Timer -= diff;

            if (!Spawned && SpawnStartSpiders_Timer <= diff)
            {
                DoScriptText(SAY_SPIDER_SPAWN, me);

                Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);
                if (!pTarget)
                    return;

                Creature* Spider = NULL;

                Spider = me->SummonCreature(15041, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                if (Spider)
                    Spider->AI()->AttackStart(pTarget);
                Spider = me->SummonCreature(15041, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                if (Spider)
                    Spider->AI()->AttackStart(pTarget);
                Spider = me->SummonCreature(15041, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                if (Spider)
                    Spider->AI()->AttackStart(pTarget);
                Spider = me->SummonCreature(15041, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                if (Spider)
                    Spider->AI()->AttackStart(pTarget);

                Spawned = true;
            }
            else
                SpawnStartSpiders_Timer -= diff;

            if (SpawnSpider_Timer <= diff)
            {
                Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);
                if (!pTarget)
                    return;

                Creature* Spider = me->SummonCreature(15041, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                if (Spider)
                    Spider->AI()->AttackStart(pTarget);
                SpawnSpider_Timer = 12000 + rand() % 5000;
            }
            else
                SpawnSpider_Timer -= diff;

            if (!PhaseTwo && Transform_Timer <= diff)
            {
                me->InterruptNonMeleeSpells(false);
                DoScriptText(SAY_TRANSFORM, me);
                DoCast(me, SPELL_SPIDER_FORM);
                const CreatureInfo* cinfo = me->GetCreatureTemplate();
                me->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, (cinfo->mindmg + ((cinfo->mindmg / 100) * 35)));
                me->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, (cinfo->maxdmg + ((cinfo->maxdmg / 100) * 35)));
                me->UpdateDamagePhysical(BASE_ATTACK);
                DoCastVictim( SPELL_ENVOLWINGWEB);

                PhaseTwo = true;
                Transform_Timer = 35000 + rand() % 25000;
            }
            else
                Transform_Timer -= diff;

            if (PhaseTwo)
            {
                if (Charge_Timer <= diff)
                {
                    Unit* pTarget = NULL;
                    int i = 0;
                    while (i < 3)                           // max 3 tries to get a random target with power_mana
                    {
                        ++i;
                        pTarget = SelectTarget(SELECT_TARGET_RANDOM, 1, 100, true);  // not aggro leader
                        if (pTarget && pTarget->getPowerType() == POWER_MANA)
                            i = 3;
                    }
                    if (pTarget)
                    {
                        DoCast(pTarget, SPELL_CHARGE);

                        if (DoGetThreat(me->getVictim()))
                            DoModifyThreatPercent(me->getVictim(), -100);

                        AttackStart(pTarget);
                    }

                    Charge_Timer = 15000 + rand() % 5000;
                }
                else
                    Charge_Timer -= diff;

                if (Thrash_Timer <= diff)
                {
                    DoCast(me, SPELL_THRASH);
                    Thrash_Timer = urand(5000, 9000);
                }
                else
                    Thrash_Timer -= diff;

                if (TransformBack_Timer <= diff)
                {
                    me->SetDisplayId(15220);
                    const CreatureInfo* cinfo = me->GetCreatureTemplate();
                    me->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, (cinfo->mindmg + ((cinfo->mindmg / 100) * 1)));
                    me->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, (cinfo->maxdmg + ((cinfo->maxdmg / 100) * 1)));
                    me->UpdateDamagePhysical(BASE_ATTACK);
                    Charge_Timer = 1500;  // reset charge timer so each transform she charges

                    PhaseTwo = false;
                    TransformBack_Timer = 25000 + rand() % 15000;
                }
                else
                    TransformBack_Timer -= diff;

            }

            DoMeleeAttackIfReady();
        }
    }
};

// Spawn of Marli
struct mob_spawn_of_marliAI : public ScriptedAI
{
    mob_spawn_of_marliAI(Creature* c) : ScriptedAI(c) {}

    uint32 Grow_Timer;
    float Growth_Level;

    void Reset()
    {
        Grow_Timer = 1000;
        Growth_Level = 1;  // increases each time Grow_Timer is called to increase scale & damage.
    }

    void EnterCombat(Unit* /*who*/)
    {
        Growth_Level = 1;
    }

    void JustDied(Unit* /*Killer*/)
    {
        me->SetObjectScale(1.0f);
    }

    void UpdateAI (const uint32 diff)
    {
        // Return since we have no target
        if (!UpdateVictim())
            return;

        // Grow_Timer
        if (Grow_Timer <= diff)
        {
            const CreatureInfo* cinfo = me->GetCreatureTemplate();
            me->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, (cinfo->maxdmg * (1 + 0.1f * Growth_Level)));
            me->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, (cinfo->maxdmg * (1 + 0.1f * Growth_Level)));
            me->UpdateDamagePhysical(BASE_ATTACK);
            me->SetObjectScale(1.0f + Growth_Level * 0.1f);
            Grow_Timer = 1000;
            ++Growth_Level;
        }
        else
            Grow_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_marli(Creature* pCreature)
{
    return new boss_marliAI (pCreature);
}

CreatureAI* GetAI_mob_spawn_of_marli(Creature* pCreature)
{
    return new mob_spawn_of_marliAI (pCreature);
}

void AddSC_boss_marli()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_marli";
    newscript->GetAI = &GetAI_boss_marli;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_spawn_of_marli";
    newscript->GetAI = &GetAI_mob_spawn_of_marli;
    newscript->RegisterSelf();
}

