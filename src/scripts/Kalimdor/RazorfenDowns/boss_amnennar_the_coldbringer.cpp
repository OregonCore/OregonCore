/* Copyright (C) 2006 - 2008 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/* ScriptData
SDName: Boss_Amnennar_the_coldbringer
SD%Complete: 100
SDComment:
SDCategory: Razorfen Downs
EndScriptData */

#include "ScriptPCH.h"

#define SAY_0             "You'll never leave this place... alive."
#define SAY_1             "Come, spirits, attend your master."
#define SAY_SLAY          "Too...easy!"
#define SOUND_AGGRO       5825
#define SOUND_SLAY        5826
#define SOUND_SUMMON      5829

#define SPELL_AMNENNARSWRATH        13009
#define SPELL_FROSTBOLT             10179

struct boss_amnennar_the_coldbringerAI : public ScriptedAI
{
    boss_amnennar_the_coldbringerAI(Creature *c) : ScriptedAI(c) {}

    uint32 AmnenarsWrath_Timer;
    uint32 FrostBolt_Timer;
    bool Spectrals;
    int Rand;
    int RandX;
    int RandY;
    Creature* Summoned;

    void Reset()
    {
        AmnenarsWrath_Timer = 8000;
        FrostBolt_Timer = 1000;
        Spectrals = false;
    }

    void EnterCombat(Unit *who)
    {
        DoYell(SAY_0,LANG_UNIVERSAL,NULL);
        DoPlaySoundToSet(me,SOUND_AGGRO);
    }

    void KilledUnit()
    {
        DoYell(SAY_SLAY, LANG_UNIVERSAL, NULL);
        DoPlaySoundToSet(me, SOUND_SLAY);
    }

    void SummonSpectrals(Unit* victim)
    {
        Rand = rand()%5;
        switch (rand()%2)
        {
            case 0: RandX = 0 - Rand; break;
            case 1: RandX = 0 + Rand; break;
        }
        Rand = 0;
        Rand = rand()%5;
        switch (rand()%2)
        {
            case 0: RandY = 0 - Rand; break;
            case 1: RandY = 0 + Rand; break;
        }
        Rand = 0;
        Summoned = DoSpawnCreature(8585, float(RandX), float(RandY), 0, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 60000);
        if (Summoned)
            ((CreatureAI*)Summoned->AI())->AttackStart(victim);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        //AmnenarsWrath_Timer
        if (AmnenarsWrath_Timer <= diff)
        {
            DoCast(me->getVictim(),SPELL_AMNENNARSWRATH);
            AmnenarsWrath_Timer = 12000;
        } else AmnenarsWrath_Timer -= diff;

        //FrostBolt_Timer
        if (FrostBolt_Timer <= diff)
        {
            Unit *pTarget = NULL;
            pTarget = SelectUnit(SELECT_TARGET_RANDOM,0);
            if (pTarget) DoCast(pTarget,SPELL_FROSTBOLT);

            FrostBolt_Timer = 8000;
        } else FrostBolt_Timer -= diff;

        if (!Spectrals && me->GetHealth()*100 / me->GetMaxHealth() < 50)
        {
            DoYell(SAY_1, LANG_UNIVERSAL, NULL);
            DoPlaySoundToSet(me, SOUND_SUMMON);

            Unit *pTarget = NULL;
            pTarget = SelectUnit(SELECT_TARGET_RANDOM,0);

            if (pTarget)
            {
                SummonSpectrals(pTarget);
                SummonSpectrals(pTarget);
                SummonSpectrals(pTarget);
            }
            Spectrals = true;
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
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_amnennar_the_coldbringer";
    newscript->GetAI = &GetAI_boss_amnennar_the_coldbringer;
    newscript->RegisterSelf();
}

