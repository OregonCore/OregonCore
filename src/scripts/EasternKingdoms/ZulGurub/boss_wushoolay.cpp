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
SDName: Boss_Wushoolay
SD%Complete: 100
SDComment:
SDCategory: Zul'Gurub
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "zulgurub.h"

#define SPELL_LIGHTNINGCLOUD         25033
#define SPELL_LIGHTNINGWAVE          24819

struct boss_wushoolayAI : public ScriptedAI
{
    boss_wushoolayAI(Creature* c) : ScriptedAI(c) {}

    uint32 LightningCloud_Timer;
    uint32 LightningWave_Timer;

    void Reset()
    {
        LightningCloud_Timer = 5000 + rand() % 5000;
        LightningWave_Timer = 8000 + rand() % 8000;
    }

    void EnterCombat(Unit* /*who*/)
    {
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        //LightningCloud_Timer
        if (LightningCloud_Timer <= diff)
        {
            DoCastVictim( SPELL_LIGHTNINGCLOUD);
            LightningCloud_Timer = 15000 + rand() % 5000;
        }
        else LightningCloud_Timer -= diff;

        //LightningWave_Timer
        if (LightningWave_Timer <= diff)
        {
            Unit* pTarget = NULL;
            pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);
            if (pTarget) DoCast(pTarget, SPELL_LIGHTNINGWAVE);

            LightningWave_Timer = 12000 + rand() % 4000;
        }
        else LightningWave_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_wushoolay(Creature* pCreature)
{
    return new boss_wushoolayAI (pCreature);
}

void AddSC_boss_wushoolay()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "boss_wushoolay";
    newscript->GetAI = &GetAI_boss_wushoolay;
    newscript->RegisterSelf();
}

