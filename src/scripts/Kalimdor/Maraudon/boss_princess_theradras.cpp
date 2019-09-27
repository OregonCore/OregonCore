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
SDName: Boss_Princess_Theradras
SD%Complete: 100
SDComment:
SDCategory: Maraudon
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"

#define SPELL_DUSTFIELD             21909
#define SPELL_BOULDER               21832
#define SPELL_THRASH                3391
#define SPELL_REPULSIVEGAZE         21869

struct boss_ptheradrasAI : public ScriptedAI
{
    boss_ptheradrasAI(Creature* c) : ScriptedAI(c) {}

    uint32 Dustfield_Timer;
    uint32 Boulder_Timer;
    uint32 Thrash_Timer;
    uint32 RepulsiveGaze_Timer;

    void Reset()
    {
        Dustfield_Timer = 8000;
        Boulder_Timer = 2000;
        Thrash_Timer = 5000;
        RepulsiveGaze_Timer = 23000;
    }

    void EnterCombat(Unit* /*who*/)
    {
    }

    void JustDied(Unit* /*killer*/)
    {
        me->SummonCreature(12238, 28.067f, 61.875f, -123.405f, 4.67f, TEMPSUMMON_TIMED_DESPAWN, 600000);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        //Dustfield_Timer
        if (Dustfield_Timer <= diff)
        {
            DoCast(me, SPELL_DUSTFIELD);
            Dustfield_Timer = 14000;
        }
        else Dustfield_Timer -= diff;

        //Boulder_Timer
        if (Boulder_Timer <= diff)
        {
            Unit* pTarget = NULL;
            pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);
            if (pTarget)
                DoCast(pTarget, SPELL_BOULDER);
            Boulder_Timer = 10000;
        }
        else Boulder_Timer -= diff;

        //RepulsiveGaze_Timer
        if (RepulsiveGaze_Timer <= diff)
        {
            DoCastVictim( SPELL_REPULSIVEGAZE);
            RepulsiveGaze_Timer = 20000;
        }
        else RepulsiveGaze_Timer -= diff;

        //Thrash_Timer
        if (Thrash_Timer <= diff)
        {
            DoCast(me, SPELL_THRASH);
            Thrash_Timer = 18000;
        }
        else Thrash_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_ptheradras(Creature* pCreature)
{
    return new boss_ptheradrasAI (pCreature);
}

void AddSC_boss_ptheradras()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "boss_princess_theradras";
    newscript->GetAI = &GetAI_boss_ptheradras;
    newscript->RegisterSelf();
}

