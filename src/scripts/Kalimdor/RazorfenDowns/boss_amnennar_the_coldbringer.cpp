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
SDName: Boss_Amnennar_the_coldbringer
SD%Complete: 100
SDComment:
SDCategory: Razorfen Downs
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"

#define SAY_AGGRO               -1129000
#define SAY_SUMMON60            -1129001
#define SAY_SUMMON30            -1129002
#define SAY_HP                  -1129003
#define SAY_KILL                -1129004

#define SPELL_AMNENNARSWRATH    13009
#define SPELL_FROSTBOLT         15530
#define SPELL_FROST_NOVA        15531
#define SPELL_FROST_SPECTRES    12642

struct boss_amnennar_the_coldbringerAI : public ScriptedAI
{
    boss_amnennar_the_coldbringerAI(Creature* c) : ScriptedAI(c) {}

    uint32 AmnenarsWrath_Timer;
    uint32 FrostBolt_Timer;
    uint32 FrostNova_Timer;
    bool Spectrals70;
    bool Spectrals55;
    bool Spectrals30;
    bool Hp;

    void Reset()
    {
        AmnenarsWrath_Timer = 8000;
        FrostBolt_Timer = 1000;
        FrostNova_Timer = 10000 + rand() % 5000;
        Spectrals70 = false;
        Spectrals55 = false;
        Spectrals30 = false;
        Hp = false;
    }

    void EnterCombat(Unit* /*who*/)
    {
        DoScriptText(SAY_AGGRO, me);
    }

    void KilledUnit()
    {
        DoScriptText(SAY_KILL, me);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        //AmnenarsWrath_Timer
        if (AmnenarsWrath_Timer <= diff)
        {
            DoCastVictim( SPELL_AMNENNARSWRATH);
            AmnenarsWrath_Timer = 12000;
        }
        else AmnenarsWrath_Timer -= diff;

        //FrostBolt_Timer
        if (FrostBolt_Timer <= diff)
        {
            DoCastVictim( SPELL_FROSTBOLT);
            FrostBolt_Timer = 8000;
        }
        else FrostBolt_Timer -= diff;

        if (FrostNova_Timer <= diff)
        {
            DoCast(me, SPELL_FROST_NOVA);
            FrostNova_Timer = 15000;
        }
        else FrostNova_Timer -= diff;

        if (!Spectrals70 && HealthBelowPct(70))
        {
            DoScriptText(SAY_SUMMON60, me);
            DoCastVictim( SPELL_FROST_SPECTRES);
            Spectrals70 = true;
        }

        if (!Hp && HealthBelowPct(50))
        {
            DoScriptText(SAY_HP, me);
            Hp = true;
        }

        if (!Spectrals55 && HealthBelowPct(55))
        {
            DoScriptText(SAY_SUMMON30, me);
            DoCastVictim( SPELL_FROST_SPECTRES);
            Spectrals55 = true;
        }


        if (!Spectrals30 && HealthBelowPct(30))
        {
           DoScriptText(SAY_SUMMON30, me);
           DoCastVictim(SPELL_FROST_SPECTRES);
           Spectrals30 = true;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_amnennar_the_coldbringer(Creature* pCreature)
{
    return new boss_amnennar_the_coldbringerAI (pCreature);
}

void AddSC_boss_amnennar_the_coldbringer()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "boss_amnennar_the_coldbringer";
    newscript->GetAI = &GetAI_boss_amnennar_the_coldbringer;
    newscript->RegisterSelf();
}

