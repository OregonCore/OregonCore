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
SDName: Boss_Warmaster_Voone
SD%Complete: 100
SDComment:
SDCategory: Blackrock Spire
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"

#define SPELL_SNAPKICK          15618
#define SPELL_CLEAVE            15579
#define SPELL_UPPERCUT          10966
#define SPELL_MORTALSTRIKE      16856
#define SPELL_PUMMEL            15615
#define SPELL_THROWAXE          16075

struct boss_warmastervooneAI : public ScriptedAI
{
    boss_warmastervooneAI(Creature* c) : ScriptedAI(c) {}

    uint32 Snapkick_Timer;
    uint32 Cleave_Timer;
    uint32 Uppercut_Timer;
    uint32 MortalStrike_Timer;
    uint32 Pummel_Timer;
    uint32 ThrowAxe_Timer;

    void Reset()
    {
        Snapkick_Timer = 8000;
        Cleave_Timer = 14000;
        Uppercut_Timer = 20000;
        MortalStrike_Timer = 12000;
        Pummel_Timer = 32000;
        ThrowAxe_Timer = 1000;
    }

    void EnterCombat(Unit* /*who*/)
    {
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!UpdateVictim())
            return;

        //Snapkick_Timer
        if (Snapkick_Timer <= diff)
        {
            DoCastVictim( SPELL_SNAPKICK);
            Snapkick_Timer = 6000;
        }
        else Snapkick_Timer -= diff;

        //Cleave_Timer
        if (Cleave_Timer <= diff)
        {
            DoCastVictim( SPELL_CLEAVE);
            Cleave_Timer = 12000;
        }
        else Cleave_Timer -= diff;

        //Uppercut_Timer
        if (Uppercut_Timer <= diff)
        {
            DoCastVictim( SPELL_UPPERCUT);
            Uppercut_Timer = 14000;
        }
        else Uppercut_Timer -= diff;

        //MortalStrike_Timer
        if (MortalStrike_Timer <= diff)
        {
            DoCastVictim( SPELL_MORTALSTRIKE);
            MortalStrike_Timer = 10000;
        }
        else MortalStrike_Timer -= diff;

        //Pummel_Timer
        if (Pummel_Timer <= diff)
        {
            DoCastVictim( SPELL_PUMMEL);
            Pummel_Timer = 16000;
        }
        else Pummel_Timer -= diff;

        //ThrowAxe_Timer
        if (ThrowAxe_Timer <= diff)
        {
            DoCastVictim( SPELL_THROWAXE);
            ThrowAxe_Timer = 8000;
        }
        else ThrowAxe_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_warmastervoone(Creature* pCreature)
{
    return new boss_warmastervooneAI (pCreature);
}

void AddSC_boss_warmastervoone()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "boss_warmaster_voone";
    newscript->GetAI = &GetAI_boss_warmastervoone;
    newscript->RegisterSelf();
}

