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
SDName: Boss_Jeklik
SD%Complete: 95
SDComment: Timers need to be verified.
SDCategory: Zul'Gurub
EndScriptData */

#include "ScriptPCH.h"
#include "zulgurub.h"

enum Texts
{
    SAY_AGGRO      = -1309002,
    SAY_RAIN_FIRE  = -1309003,
    SAY_DEATH      = -1309004
};

enum Spells
{
    SPELL_CHARGE            = 22911,
    SPELL_SONICBURST        = 23918,
    SPELL_SCREECH           = 6605,
    SPELL_SHADOW_WORD_PAIN  = 23952,
    SPELL_MIND_FLAY         = 23953,
    SPELL_CHAIN_MIND_FLAY   = 26044,   // Correct spell unknown, however unable to find on WoWHead or WoWWiki but was added in 1.12 so possibily they used this spell and it's correct?
    SPELL_GREATERHEAL       = 23954,
    SPELL_BAT_FORM          = 23966,

    // Batrider's Spell
    SPELL_BOMB              = 23970
};

struct boss_jeklikAI : public ScriptedAI
{
    boss_jeklikAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }

    ScriptedInstance* pInstance;

    uint32 Charge_Timer;
    uint32 SonicBurst_Timer;
    uint32 Screech_Timer;
    uint32 SpawnBats_Timer;
    uint32 ShadowWordPain_Timer;
    uint32 MindFlay_Timer;
    uint32 ChainMindFlay_Timer;
    uint32 GreaterHeal_Timer;
    uint32 SpawnFlyingBats_Timer;

    bool PhaseTwo;

    void Reset()
    {
        Charge_Timer = 20000;
        SonicBurst_Timer = 20000;
        Screech_Timer = 13000;
        SpawnBats_Timer = 60000;
        ShadowWordPain_Timer = 6000;
        MindFlay_Timer = 11000;
        ChainMindFlay_Timer = 26000;
        GreaterHeal_Timer = 50000;
        SpawnFlyingBats_Timer = 10000;

        PhaseTwo = false;
    }

    void EnterCombat(Unit* /*who*/)
    {
        DoScriptText(SAY_AGGRO, me);
        DoCast(me, SPELL_BAT_FORM);
    }

    void JustDied(Unit* /*Killer*/)
    {
        DoScriptText(SAY_DEATH, me);

        if (pInstance)
            pInstance->SetData(TYPE_JEKLIK, DONE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (me->getVictim() && me->IsAlive())
        {
            if ((me->GetHealth() * 100 / me->GetMaxHealth() > 50))
            {
                if (Charge_Timer <= diff)
                {
                    if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                    {
                        DoCast(pTarget, SPELL_CHARGE);
                        AttackStart(pTarget);
                    }

                    Charge_Timer = 15000 + rand() % 15000;
                }
                else
                    Charge_Timer -= diff;

                if (SonicBurst_Timer <= diff)
                {
                    DoCastVictim( SPELL_SONICBURST);
                    SonicBurst_Timer = 20000;
                }
                else
                    SonicBurst_Timer -= diff;

                if (Screech_Timer <= diff)
                {
                    DoCastVictim( SPELL_SCREECH);
                    Screech_Timer = 18000 + rand() % 8000;
                }
                else
                    Screech_Timer -= diff;

                if (SpawnBats_Timer <= diff)
                {
                    Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);

                    Creature* Bat = NULL;
                    Bat = me->SummonCreature(11368, -12291.6220f, -1380.2640f, 144.8304f, 5.483f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                    if (pTarget && Bat) Bat ->AI()->AttackStart(pTarget);

                    Bat = me->SummonCreature(11368, -12289.6220f, -1380.2640f, 144.8304f, 5.483f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                    if (pTarget && Bat) Bat ->AI()->AttackStart(pTarget);

                    Bat = me->SummonCreature(11368, -12293.6220f, -1380.2640f, 144.8304f, 5.483f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                    if (pTarget && Bat) Bat ->AI()->AttackStart(pTarget);

                    Bat = me->SummonCreature(11368, -12291.6220f, -1380.2640f, 144.8304f, 5.483f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                    if (pTarget && Bat) Bat ->AI()->AttackStart(pTarget);

                    Bat = me->SummonCreature(11368, -12289.6220f, -1380.2640f, 144.8304f, 5.483f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                    if (pTarget && Bat) Bat ->AI()->AttackStart(pTarget);

                    Bat = me->SummonCreature(11368, -12293.6220f, -1380.2640f, 144.8304f, 5.483f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                    if (pTarget && Bat) Bat ->AI()->AttackStart(pTarget);

                    SpawnBats_Timer = 60000;
                }
                else
                    SpawnBats_Timer -= diff;
            }
            else
            {
                if (PhaseTwo)
                {
                    if (PhaseTwo && ShadowWordPain_Timer <= diff)
                    {
                        if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                        {
                            DoCast(pTarget, SPELL_SHADOW_WORD_PAIN);
                            ShadowWordPain_Timer = 12000 + rand() % 6000;
                        }
                    }

                    ShadowWordPain_Timer -= diff;

                    if (MindFlay_Timer <= diff)
                    {
                        DoCastVictim( SPELL_MIND_FLAY);
                        MindFlay_Timer = 16000;
                    }

                    MindFlay_Timer -= diff;

                    if (ChainMindFlay_Timer <= diff)
                    {
                        me->InterruptNonMeleeSpells(false);
                        DoCastVictim( SPELL_CHAIN_MIND_FLAY);
                        ChainMindFlay_Timer = 15000 + rand() % 15000;
                    }

                    ChainMindFlay_Timer -= diff;

                    if (GreaterHeal_Timer <= diff)
                    {
                        me->InterruptNonMeleeSpells(false);
                        DoCast(me, SPELL_GREATERHEAL);
                        GreaterHeal_Timer = 25000 + rand() % 10000;
                    }

                    GreaterHeal_Timer -= diff;

                    if (SpawnFlyingBats_Timer <= diff)
                    {
                        Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);
                        if (!pTarget)
                            return;

                        Creature* FlyingBat = me->SummonCreature(14965, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ() + 15, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                        if (FlyingBat)
                            FlyingBat->AI()->AttackStart(pTarget);

                        SpawnFlyingBats_Timer = 10000 + rand() % 5000;
                    }
                    else
                        SpawnFlyingBats_Timer -= diff;
                }
                else
                {
                    me->SetDisplayId(15219);
                    DoResetThreat();
                    PhaseTwo = true;
                }
            }

            DoMeleeAttackIfReady();
        }
    }
};

// Flying Bat
struct mob_batriderAI : public ScriptedAI
{
    mob_batriderAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }

    ScriptedInstance* pInstance;

    uint32 Bomb_Timer;
    uint32 Despawn_Timer;

    void Reset()
    {
        Bomb_Timer = 2500;
        Despawn_Timer = 5000;

        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);    // not attackable so will despawn at Despawn_Timer
    }

    void EnterCombat(Unit* /*who*/) {}

    void UpdateAI (const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        // Bomb_Timer
        if (Bomb_Timer <= diff)
        {
            if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
            {
                DoCast(pTarget, SPELL_BOMB);
                Bomb_Timer = 2500;
            }
        }
        else
            Bomb_Timer -= diff;

        // Despawn_Timer
        if (Despawn_Timer <= diff)
        {
            if (pInstance)
            {
                if (pInstance->GetData(TYPE_JEKLIK) == DONE)
                {
                    me->setDeathState(JUST_DIED);
                    me->RemoveCorpse();
                    return;
                }
            }

            Despawn_Timer = 5000;    // given enough time to throw bomb then despawn
        }
        else
            Despawn_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_jeklik(Creature* pCreature)
{
    return new boss_jeklikAI (pCreature);
}

CreatureAI* GetAI_mob_batrider(Creature* pCreature)
{
    return new mob_batriderAI (pCreature);
}

void AddSC_boss_jeklik()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "boss_jeklik";
    newscript->GetAI = &GetAI_boss_jeklik;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_batrider";
    newscript->GetAI = &GetAI_mob_batrider;
    newscript->RegisterSelf();
}

