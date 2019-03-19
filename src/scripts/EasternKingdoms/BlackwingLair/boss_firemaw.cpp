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
SDName: Boss_Firemaw
SD%Complete: 100
SDComment:
SDCategory: Blackwing Lair
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"

#define SPELL_SHADOWFLAME       22539
#define SPELL_WINGBUFFET        23339
#define SPELL_FLAMEBUFFET       23341

struct boss_firemawAI : public ScriptedAI
{
    boss_firemawAI(Creature* c) : ScriptedAI(c) {}

    uint32 ShadowFlame_Timer;
    uint32 WingBuffet_Timer;
    uint32 FlameBuffet_Timer;

    void Reset()
    {
        ShadowFlame_Timer = 30000;                          //These times are probably wrong
        WingBuffet_Timer = 24000;
        FlameBuffet_Timer = 5000;
    }

    void EnterCombat(Unit* /*who*/)
    {
        DoZoneInCombat();
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        //ShadowFlame_Timer
        if (ShadowFlame_Timer <= diff)
        {
            DoCastVictim( SPELL_SHADOWFLAME);
            ShadowFlame_Timer = urand(15000, 18000);
        }
        else ShadowFlame_Timer -= diff;

        //WingBuffet_Timer
        if (WingBuffet_Timer <= diff)
        {
            DoCastVictim( SPELL_WINGBUFFET);
            if (DoGetThreat(me->GetVictim()))
                DoModifyThreatPercent(me->GetVictim(), -75);

            WingBuffet_Timer = 25000;
        }
        else WingBuffet_Timer -= diff;

        //FlameBuffet_Timer
        if (FlameBuffet_Timer <= diff)
        {
            DoCastVictim( SPELL_FLAMEBUFFET);
            FlameBuffet_Timer = 5000;
        }
        else FlameBuffet_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_firemaw(Creature* pCreature)
{
    return new boss_firemawAI (pCreature);
}

void AddSC_boss_firemaw()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "boss_firemaw";
    newscript->GetAI = &GetAI_boss_firemaw;
    newscript->RegisterSelf();
}

