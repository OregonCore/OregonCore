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
SDName: Boss_Anubrekhan
SD%Complete: 70
SDComment:
SDCategory: Naxxramas
EndScriptData */

#include "ScriptPCH.h"

#define SAY_GREET           -1533000
#define SAY_AGGRO1          -1533001
#define SAY_AGGRO2          -1533002
#define SAY_AGGRO3          -1533003
#define SAY_TAUNT1          -1533004
#define SAY_TAUNT2          -1533005
#define SAY_TAUNT3          -1533006
#define SAY_TAUNT4          -1533007
#define SAY_SLAY            -1533008

#define SPELL_IMPALE        28783                           //May be wrong spell id. Causes more dmg than I expect
#define H_SPELL_IMPALE      56090
#define SPELL_LOCUSTSWARM   28785                           //This is a self buff that triggers the dmg debuff
#define H_SPELL_LOCUSTSWARM 54021

//invalid
#define SPELL_SUMMONGUARD   29508                           //Summons 1 crypt guard at targeted location

#define SPELL_SELF_SPAWN_5  29105                           //This spawns 5 corpse scarabs ontop of us (most likely the player casts this on death)
#define SPELL_SELF_SPAWN_10 28864                           //This is used by the crypt guards when they die

struct boss_anubrekhanAI : public ScriptedAI
{
    boss_anubrekhanAI(Creature* c) : ScriptedAI(c) {}

    uint32 Impale_Timer;
    uint32 LocustSwarm_Timer;
    uint32 Summon_Timer;
    bool HasTaunted;

    void Reset()
    {
        Impale_Timer = 15000;                               //15 seconds
        LocustSwarm_Timer = 80000 + (rand()%40000);         //Random time between 80 seconds and 2 minutes for initial cast
        Summon_Timer = LocustSwarm_Timer + 45000;           //45 seconds after initial locust swarm
    }

    void KilledUnit(Unit* Victim)
    {
        //Force the player to spawn corpse scarabs via spell
        Victim->CastSpell(Victim, SPELL_SELF_SPAWN_5, true);

        if (rand()%5)
            return;

         DoScriptText(SAY_SLAY, me);
    }

    void EnterCombat(Unit* /*who*/)
    {
        switch(rand()%3)
        {
        case 0: DoScriptText(SAY_AGGRO1, me); break;
        case 1: DoScriptText(SAY_AGGRO2, me); break;
        case 2: DoScriptText(SAY_AGGRO3, me); break;
        }
    }

    void MoveInLineOfSight(Unit *who)
    {

            if (!HasTaunted && me->IsWithinDistInMap(who, 60.0f))
            {
                switch(rand()%5)
                {
                case 0: DoScriptText(SAY_GREET, me); break;
                case 1: DoScriptText(SAY_TAUNT1, me); break;
                case 2: DoScriptText(SAY_TAUNT2, me); break;
                case 3: DoScriptText(SAY_TAUNT3, me); break;
                case 4: DoScriptText(SAY_TAUNT4, me); break;
                }
                HasTaunted = true;
            }
        ScriptedAI::MoveInLineOfSight(who);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        //Impale_Timer
        if (Impale_Timer <= diff)
        {
            //Cast Impale on a random target
            //Do NOT cast it when we are afflicted by locust swarm
            if (!me->HasAura(SPELL_LOCUSTSWARM,1))
            {
                if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM,0))
                    DoCast(pTarget,SPELL_IMPALE);
            }

            Impale_Timer = 15000;
        } else Impale_Timer -= diff;

        //LocustSwarm_Timer
        if (LocustSwarm_Timer <= diff)
        {
            DoCast(me, SPELL_LOCUSTSWARM);
            LocustSwarm_Timer = 90000;
        } else LocustSwarm_Timer -= diff;

        //Summon_Timer
        if (Summon_Timer <= diff)
        {
            DoCast(me, SPELL_SUMMONGUARD);
            Summon_Timer = 45000;
        } else Summon_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_anubrekhan(Creature* pCreature)
{
    return new boss_anubrekhanAI (pCreature);
}

void AddSC_boss_anubrekhan()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_anubrekhan";
    newscript->GetAI = &GetAI_boss_anubrekhan;
    newscript->RegisterSelf();
}

