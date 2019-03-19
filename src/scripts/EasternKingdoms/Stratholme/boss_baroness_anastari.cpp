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
SDName: Boss_Baroness_Anastari
SD%Complete: 90
SDComment: MC disabled
SDCategory: Stratholme
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "stratholme.h"

#define SPELL_BANSHEEWAIL   16565
#define SPELL_BANSHEECURSE    16867
#define SPELL_SILENCE    18327
//#define SPELL_POSSESS   17244

struct boss_baroness_anastariAI : public ScriptedAI
{
    boss_baroness_anastariAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)me->GetInstanceData();
    }

    ScriptedInstance* pInstance;

    uint32 BansheeWail_Timer;
    uint32 BansheeCurse_Timer;
    uint32 Silence_Timer;
    //uint32 Possess_Timer;

    void Reset()
    {
        BansheeWail_Timer = 1000;
        BansheeCurse_Timer = 11000;
        Silence_Timer = 13000;
        //Possess_Timer = 35000;
    }

    void EnterCombat(Unit* /*who*/)
    {
    }

    void JustDied(Unit* /*Killer*/)
    {
        if (pInstance)
            pInstance->SetData(TYPE_BARONESS, IN_PROGRESS);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        //BansheeWail
        if (BansheeWail_Timer <= diff)
        {
            if (rand() % 100 < 95)
                DoCastVictim( SPELL_BANSHEEWAIL);
            //4 seconds until we should cast this again
            BansheeWail_Timer = 4000;
        }
        else BansheeWail_Timer -= diff;

        //BansheeCurse
        if (BansheeCurse_Timer <= diff)
        {
            if (rand() % 100 < 75)
                DoCastVictim( SPELL_BANSHEECURSE);
            //18 seconds until we should cast this again
            BansheeCurse_Timer = 18000;
        }
        else BansheeCurse_Timer -= diff;

        //Silence
        if (Silence_Timer <= diff)
        {
            if (rand() % 100 < 80)
                DoCastVictim( SPELL_SILENCE);
            //13 seconds until we should cast this again
            Silence_Timer = 13000;
        }
        else Silence_Timer -= diff;

        //Possess
        /*           if (Possess_Timer <= diff)
        {
        //Cast
          if (rand()%100 < 65)
        {
        Unit* pTarget = NULL;
        pTarget = SelectUnit(SELECT_TARGET_RANDOM,0);
        if (pTarget)DoCast(pTarget, SPELL_POSSESS);
        }
        //50 seconds until we should cast this again
        Possess_Timer = 50000;
        } else Possess_Timer -= diff;
        */

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_baroness_anastari(Creature* pCreature)
{
    return GetInstanceAI<boss_baroness_anastariAI>(pCreature);
}

void AddSC_boss_baroness_anastari()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "boss_baroness_anastari";
    newscript->GetAI = &GetAI_boss_baroness_anastari;
    newscript->RegisterSelf();
}

