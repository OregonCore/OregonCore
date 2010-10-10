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
SDName: Boss_Herod
SD%Complete: 95
SDComment: Should in addition spawn Myrmidons in the hallway outside
SDCategory: Scarlet Monastery
EndScriptData */

#include "ScriptPCH.h"
#include "ScriptedEscortAI.h"

#define SAY_AGGRO                   -1189000
#define SAY_WHIRLWIND               -1189001
#define SAY_ENRAGE                  -1189002
#define SAY_KILL                    -1189003
#define EMOTE_ENRAGE                -1189004

#define SPELL_RUSHINGCHARGE         8260
#define SPELL_CLEAVE                15496
#define SPELL_WHIRLWIND             8989
#define SPELL_FRENZY                8269

#define ENTRY_SCARLET_TRAINEE       6575
#define ENTRY_SCARLET_MYRMIDON      4295

struct boss_herodAI : public ScriptedAI
{
    boss_herodAI(Creature *c) : ScriptedAI(c) {}

    bool Enrage;

    uint32 Cleave_Timer;
    uint32 Whirlwind_Timer;


    void Reset()
    {
        Enrage = false;
        Cleave_Timer = 12000;
        Whirlwind_Timer = 60000;
    }

    void EnterCombat(Unit *who)
    {
        DoScriptText(SAY_AGGRO, me);
        DoCast(me,SPELL_RUSHINGCHARGE);
    }

     void KilledUnit(Unit *victim)
     {
         DoScriptText(SAY_KILL, me);
     }

     void JustDied(Unit* killer)
     {
         for (uint8 i = 0; i < 20; ++i)
             me->SummonCreature(ENTRY_SCARLET_TRAINEE, 1939.18f, -431.58f, 17.09f, 6.22f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 600000);
     }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        //If we are <30% hp goes Enraged
        if (!Enrage && me->GetHealth()*100 / me->GetMaxHealth() <= 30 && !me->IsNonMeleeSpellCasted(false))
        {
            DoScriptText(EMOTE_ENRAGE, me);
            DoScriptText(SAY_ENRAGE, me);
            DoCast(me,SPELL_FRENZY);
            Enrage = true;
        }

        //Cleave_Timer
        if (Cleave_Timer <= diff)
        {
            DoCast(me->getVictim(),SPELL_CLEAVE);
            Cleave_Timer = 12000;
        } else Cleave_Timer -= diff;

        // Whirlwind_Timer
        if (Whirlwind_Timer <= diff)
        {
            DoScriptText(SAY_WHIRLWIND, me);
            DoCast(me->getVictim(),SPELL_WHIRLWIND);
            Whirlwind_Timer = 30000;
        } else Whirlwind_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_herod(Creature* pCreature)
{
    return new boss_herodAI (pCreature);
}


float Location[12][3]=
{
    {1945.81f, -431.54f, 16.36f},
    {1946.21f, -436.41f, 16.36f},
    {1950.01f, -444.11f, 14.63f},
    {1956.08f, -449.34f, 13.12f},
    {1966.59f, -450.55f, 11.27f},
    {1976.09f, -447.51f, 11.27f},
    {1983.42f, -435.85f, 11.27f},
    {1978.17f, -428.81f, 11.27f},
    {1973.97f, -422.08f, 9.04f},
    {1963.84f, -418.90f, 6.17f},
    {1961.22f, -422.74f, 6.17f},
    {1964.80f, -431.26f, 6.17f}
};

uint32 Wait[12][1]=
{
    {0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{600000}
};


struct mob_scarlet_traineeAI : public npc_escortAI
{
    mob_scarlet_traineeAI(Creature *c) : npc_escortAI(c)
    {
        Start_Timer = urand(1000,6000);
    }

    uint32 Start_Timer;

    void Reset() {}
    void WaypointReached(uint32 uiPoint) {}
    void EnterCombat(Unit* who) {}

    void UpdateAI(const uint32 diff)
    {
        if (Start_Timer)
        {
            if (Start_Timer <= diff)
            {
                Start(true,true);
                Start_Timer = 0;
            } else Start_Timer -= diff;
        }

        npc_escortAI::UpdateAI(diff);
    }
};

CreatureAI* GetAI_mob_scarlet_trainee(Creature* pCreature)
{
    mob_scarlet_traineeAI* thisAI = new mob_scarlet_traineeAI(pCreature);

    for (uint32 i = 0; i < 12; ++i)
        thisAI->AddWaypoint(i, Location[i][0], Location[i][1], Location[i][2], Wait[i][0]);

    return ((CreatureAI*)thisAI);
}

void AddSC_boss_herod()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_herod";
    newscript->GetAI = &GetAI_boss_herod;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_scarlet_trainee";
    newscript->GetAI = &GetAI_mob_scarlet_trainee;
    newscript->RegisterSelf();
}
