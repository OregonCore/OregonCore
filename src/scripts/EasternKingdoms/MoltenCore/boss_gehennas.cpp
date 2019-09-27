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
SDName: Boss_Gehennas
SD%Complete: 90
SDComment: Adds MC NYI
SDCategory: Molten Core
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"

#define SPELL_SHADOWBOLT            19728
#define SPELL_RAINOFFIRE            19717
#define SPELL_GEHENNASCURSE         19716

struct boss_gehennasAI : public ScriptedAI
{
    boss_gehennasAI(Creature* c) : ScriptedAI(c) {}

    uint32 ShadowBolt_Timer;
    uint32 RainOfFire_Timer;
    uint32 GehennasCurse_Timer;

    void Reset()
    {
        ShadowBolt_Timer = 6000;
        RainOfFire_Timer = 10000;
        GehennasCurse_Timer = 12000;
    }

    void EnterCombat(Unit* /*who*/) {}

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        //ShadowBolt_Timer
        if (ShadowBolt_Timer <= diff)
        {
            if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 1))
                DoCast(pTarget, SPELL_SHADOWBOLT);

            ShadowBolt_Timer = 7000;
        }
        else ShadowBolt_Timer -= diff;

        //RainOfFire_Timer
        if (RainOfFire_Timer <= diff)
        {
            if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                DoCast(pTarget, SPELL_RAINOFFIRE);

            RainOfFire_Timer = urand(4000, 12000);
        }
        else RainOfFire_Timer -= diff;

        //GehennasCurse_Timer
        if (GehennasCurse_Timer <= diff)
        {
            DoCastVictim( SPELL_GEHENNASCURSE);
            GehennasCurse_Timer = urand(22000, 30000);
        }
        else GehennasCurse_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_gehennas(Creature* pCreature)
{
    return new boss_gehennasAI (pCreature);
}

void AddSC_boss_gehennas()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "boss_gehennas";
    newscript->GetAI = &GetAI_boss_gehennas;
    newscript->RegisterSelf();
}

