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
SDName: Boss Pathaleon the Calculator
SD%Complete: 50
SDComment: Event missing. Script for himself 99% blizzlike.
SDCategory: Tempest Keep, The Mechanar
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"

#define SAY_AGGRO                       -1554020
#define SAY_DOMINATION_1                -1554021
#define SAY_DOMINATION_2                -1554022
#define SAY_SUMMON                      -1554023
#define SAY_ENRAGE                      -1554024
#define SAY_SLAY_1                      -1554025
#define SAY_SLAY_2                      -1554026
#define SAY_DEATH                       -1554027

// Spells to be casted
#define SPELL_MANA_TAP                36021
#define SPELL_ARCANE_TORRENT          36022
#define SPELL_DOMINATION              35280
#define H_SPELL_ARCANE_EXPLOSION        15453
#define SPELL_FRENZY                    36992

#define SPELL_SUMMON_NETHER_WRAITH_1    35285               //Spells work, but not implemented
#define SPELL_SUMMON_NETHER_WRAITH_2    35286
#define SPELL_SUMMON_NETHER_WRAITH_3    35287
#define SPELL_SUMMON_NETHER_WRAITH_4    35288

// Add Spells
#define SPELL_DETONATION              35058
#define SPELL_ARCANE_MISSILES         35034

struct boss_pathaleon_the_calculatorAI : public ScriptedAI
{
    boss_pathaleon_the_calculatorAI(Creature* c) : ScriptedAI(c), summons(me)
    {
        HeroicMode = me->GetMap()->IsHeroic();
    }

    uint32 Summon_Timer;
    SummonList summons;
    uint32 ManaTap_Timer;
    uint32 ArcaneTorrent_Timer;
    uint32 Domination_Timer;
    uint32 ArcaneExplosion_Timer;
    bool HeroicMode;
    bool Enraged;

    uint32 Counter;

    void Reset()
    {
        Summon_Timer = 30000;
        ManaTap_Timer = 12000 + rand() % 8000;
        ArcaneTorrent_Timer = 16000 + rand() % 9000;
        Domination_Timer = 25000 + rand() % 15000;
        ArcaneExplosion_Timer = 8000 + rand() % 5000;

        Enraged = false;

        Counter = 0;
        summons.DespawnAll();
    }
    void EnterCombat(Unit* /*who*/)
    {
        DoScriptText(SAY_AGGRO, me);
    }

    void KilledUnit(Unit* /*victim*/)
    {
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

    void JustDied(Unit* /*Killer*/)
    {
        DoScriptText(SAY_DEATH, me);

        summons.DespawnAll();
    }

    void JustSummoned(Creature* summon)
    {
        summons.Summon(summon);
    }
    void SummonedCreatureDespawn(Creature* summon)
    {
        summons.Despawn(summon);
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!UpdateVictim())
            return;

        if (Summon_Timer <= diff)
        {
            for (int i = 0; i < 3; i++)
            {
                Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);
                Creature* Wraith = me->SummonCreature(21062, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
                if (pTarget && Wraith)
                    Wraith->AI()->AttackStart(pTarget);
            }
            DoScriptText(SAY_SUMMON, me);
            Summon_Timer = 30000 + rand() % 15000;
        }
        else Summon_Timer -= diff;

        if (ManaTap_Timer <= diff)
        {
            DoCastVictim(SPELL_MANA_TAP);
            ManaTap_Timer = 14000 + rand() % 8000;
        }
        else ManaTap_Timer -= diff;

        if (ArcaneTorrent_Timer <= diff)
        {
            DoCastVictim(SPELL_ARCANE_TORRENT);
            ArcaneTorrent_Timer = 12000 + rand() % 6000;
        }
        else ArcaneTorrent_Timer -= diff;

        if (Domination_Timer <= diff)
        {
            if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 1))
            {
                switch (rand() % 2)
                {
                case 0:
                    DoScriptText(SAY_DOMINATION_1, me);
                    break;
                case 1:
                    DoScriptText(SAY_DOMINATION_2, me);
                    break;
                }

                DoCast(pTarget, SPELL_DOMINATION);
            }
            Domination_Timer = 25000 + rand() % 5000;
        }
        else Domination_Timer -= diff;

        //Only casting if Heroic Mode is used
        if (HeroicMode)
        {
            if (ArcaneExplosion_Timer <= diff)
            {
                DoCastVictim(H_SPELL_ARCANE_EXPLOSION);
                ArcaneExplosion_Timer = 10000 + rand() % 4000;
            }
            else ArcaneExplosion_Timer -= diff;
        }

        if (!Enraged && HealthBelowPct(20))
        {
            DoCast(me, SPELL_FRENZY);
            DoScriptText(SAY_ENRAGE, me);
            Enraged = true;

        }

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_pathaleon_the_calculator(Creature* pCreature)
{
    return new boss_pathaleon_the_calculatorAI (pCreature);
}

struct mob_nether_wraithAI : public ScriptedAI
{
    mob_nether_wraithAI(Creature* c) : ScriptedAI(c) {}

    uint32 ArcaneMissiles_Timer;
    uint32 Detonation_Timer;
    uint32 Die_Timer;
    bool Detonation;

    void Reset()
    {
        ArcaneMissiles_Timer = 1000 + rand() % 3000;
        Detonation_Timer = 20000;
        Die_Timer = 2200;
        Detonation = false;

    }

    void EnterCombat(Unit* /*who*/)
    {
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (ArcaneMissiles_Timer <= diff)
        {
            if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 1))
                DoCast(pTarget, SPELL_ARCANE_MISSILES);
            else
                DoCastVictim(SPELL_ARCANE_MISSILES);

            ArcaneMissiles_Timer = 5000 + rand() % 5000;
        }
        else ArcaneMissiles_Timer -= diff;

        if (!Detonation)
        {
            if (Detonation_Timer <= diff)
            {
                DoCast(me, SPELL_DETONATION);
                Detonation = true;
            }
            else Detonation_Timer -= diff;
        }

        if (Detonation)
        {
            if (Die_Timer <= diff)
            {
                me->setDeathState(JUST_DIED);
                me->RemoveCorpse();
            }
            else Die_Timer -= diff;
        }

        DoMeleeAttackIfReady();
    }

};
CreatureAI* GetAI_mob_nether_wraith(Creature* pCreature)
{
    return new mob_nether_wraithAI (pCreature);
}

void AddSC_boss_pathaleon_the_calculator()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "boss_pathaleon_the_calculator";
    newscript->GetAI = &GetAI_boss_pathaleon_the_calculator;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_nether_wraith";
    newscript->GetAI = &GetAI_mob_nether_wraith;
    newscript->RegisterSelf();
}

