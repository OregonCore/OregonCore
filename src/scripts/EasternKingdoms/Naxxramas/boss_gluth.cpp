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
SDName: Boss_Gluth
SD%Complete: 100
SDComment:
SDCategory: Naxxramas
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"

#define SPELL_MORTALWOUND       25646
#define SPELL_DECIMATE          28374
#define SPELL_TERRIFYINGROAR    29685
#define SPELL_FRENZY            19812
#define SPELL_ENRAGE            28747

#define ADD_1X 3269.590f
#define ADD_1Y -3161.287f
#define ADD_1Z 297.423f

#define ADD_2X 3277.797f
#define ADD_2Y -3170.352f
#define ADD_2Z 297.423f

#define ADD_3X 3267.049f
#define ADD_3Y -3172.820f
#define ADD_3Z 297.423f

#define ADD_4X 3252.157f
#define ADD_4Y -3132.135f
#define ADD_4Z 297.423f

#define ADD_5X 3259.990f
#define ADD_5Y -3126.590f
#define ADD_5Z 297.423f

#define ADD_6X 3259.815f
#define ADD_6Y -3137.576f
#define ADD_6Z 297.423f

#define ADD_7X 3308.030f
#define ADD_7Y -3132.135f
#define ADD_7Z 297.423f

#define ADD_8X 3303.046f
#define ADD_8Y -3180.682f
#define ADD_8Z 297.423f

#define ADD_9X 3313.283f
#define ADD_9Y -3180.766f
#define ADD_9Z 297.423f

struct boss_gluthAI : public ScriptedAI
{
    boss_gluthAI(Creature* c) : ScriptedAI(c) {}

    uint32 MortalWound_Timer;
    uint32 Decimate_Timer;
    uint32 TerrifyingRoar_Timer;
    uint32 Frenzy_Timer;
    uint32 Enrage_Timer;
    uint32 Summon_Timer;

    void Reset()
    {
        MortalWound_Timer = 8000;
        Decimate_Timer = 100000;
        TerrifyingRoar_Timer = 21000;
        Frenzy_Timer = 15000;
        Enrage_Timer = 304000;
        Summon_Timer = 10000;
    }

    void EnterCombat(Unit* /*who*/)
    {
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        //MortalWound_Timer
        if (MortalWound_Timer <= diff)
        {
            DoCastVictim(SPELL_MORTALWOUND);
            MortalWound_Timer = 10000;
        }
        else MortalWound_Timer -= diff;

        //Decimate_Timer
        if (Decimate_Timer <= diff)
        {
            DoCastVictim(SPELL_DECIMATE);
            Decimate_Timer = 100000;
        }
        else Decimate_Timer -= diff;

        //TerrifyingRoar_Timer
        if (TerrifyingRoar_Timer <= diff)
        {
            DoCastVictim(SPELL_TERRIFYINGROAR);
            TerrifyingRoar_Timer = 20000;
        }
        else TerrifyingRoar_Timer -= diff;

        //Frenzy_Timer
        if (Frenzy_Timer <= diff)
        {
            DoCast(me, SPELL_FRENZY);
            Frenzy_Timer = 10500;
        }
        else Frenzy_Timer -= diff;

        //Enrage_Timer
        if (Enrage_Timer <= diff)
        {
            DoCast(me, SPELL_ENRAGE);
            Enrage_Timer = 61000;
        }
        else Enrage_Timer -= diff;

        //Summon_Timer
        if (Summon_Timer <= diff)
        {
            Unit* pTarget = NULL;
            Unit* SummonedZombies = NULL;

            SummonedZombies = me->SummonCreature(16360, ADD_1X, ADD_1Y, ADD_1Z, 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 80000);
            SummonedZombies = me->SummonCreature(16360, ADD_2X, ADD_2Y, ADD_2Z, 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 80000);
            SummonedZombies = me->SummonCreature(16360, ADD_3X, ADD_3Y, ADD_3Z, 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 80000);
            SummonedZombies = me->SummonCreature(16360, ADD_4X, ADD_4Y, ADD_4Z, 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 80000);
            SummonedZombies = me->SummonCreature(16360, ADD_5X, ADD_5Y, ADD_5Z, 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 80000);
            SummonedZombies = me->SummonCreature(16360, ADD_6X, ADD_6Y, ADD_6Z, 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 80000);
            SummonedZombies = me->SummonCreature(16360, ADD_7X, ADD_7Y, ADD_7Z, 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 80000);
            SummonedZombies = me->SummonCreature(16360, ADD_8X, ADD_8Y, ADD_8Z, 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 80000);
            SummonedZombies = me->SummonCreature(16360, ADD_9X, ADD_9Y, ADD_9Z, 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 80000);

            if (SummonedZombies)
            {
                pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);
                if (pTarget)
                    SummonedZombies->AddThreat(pTarget, 1.0f);
            }

            Summon_Timer = 28000;
        }
        else Summon_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_gluth(Creature* pCreature)
{
    return new boss_gluthAI (pCreature);
}

void AddSC_boss_gluth()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "boss_gluth";
    newscript->GetAI = &GetAI_boss_gluth;
    newscript->RegisterSelf();
}

