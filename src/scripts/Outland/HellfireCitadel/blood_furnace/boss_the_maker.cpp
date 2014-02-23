/*
 * Copyright (C) 2010-2014 OregonCore <http://www.oregoncore.com/>
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2012 ScriptDev2 <http://www.scriptdev2.com/>
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
SDName: Boss_The_Maker
SD%Complete: 99
SDComment:
SDCategory: Hellfire Citadel, Blood Furnace
EndScriptData */

#include "ScriptPCH.h"
#include "blood_furnace.h"

#define SAY_AGGRO_1                 -1542009
#define SAY_AGGRO_2                 -1542010
#define SAY_AGGRO_3                 -1542011
#define SAY_KILL_1                  -1542012
#define SAY_KILL_2                  -1542013
#define SAY_DIE                     -1542014

#define SPELL_EXPLODING_BREAKER     30925
#define SPELL_EXPLODING_BREAKER_H   40059
#define SPELL_DOMINATION            30923

struct boss_the_makerAI : public ScriptedAI
{
    boss_the_makerAI(Creature *c) : ScriptedAI(c) 
	{
            pInstance = c->GetInstanceData();
            HeroicMode = me->GetMap()->IsHeroic();
	}

    ScriptedInstance* pInstance;
    bool HeroicMode;

    uint32 ExplodingBreaker_Timer;
    uint32 Domination_Timer;

    void Reset()
    {
        ExplodingBreaker_Timer = 9000;
        Domination_Timer = 60000;
        if (pInstance)
            pInstance->SetData(DATA_MAKEREVENT, NOT_STARTED);
    }

    void EnterCombat(Unit* /*who*/)
    {
        switch(rand()%3)
        {
            case 0: DoScriptText(SAY_AGGRO_1, me); break;
            case 1: DoScriptText(SAY_AGGRO_2, me); break;
            case 2: DoScriptText(SAY_AGGRO_3, me); break;
        }

        if (pInstance)
            pInstance->SetData(DATA_MAKEREVENT, IN_PROGRESS);
    }

    void KilledUnit(Unit* /*victim*/)
    {
        switch(rand()%2)
        {
            case 0: DoScriptText(SAY_KILL_1, me); break;
            case 1: DoScriptText(SAY_KILL_2, me); break;
        }
    }

    void JustDied(Unit* /*Killer*/)
    {
        DoScriptText(SAY_DIE, me);

        if (pInstance)
            pInstance->SetData(DATA_MAKEREVENT, DONE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (ExplodingBreaker_Timer <= diff)
        {
            if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM,0))
                DoCast(pTarget,HeroicMode ? SPELL_EXPLODING_BREAKER_H : SPELL_EXPLODING_BREAKER);
            ExplodingBreaker_Timer = 9000+rand()%2000;
        }
        else ExplodingBreaker_Timer -=diff;

        if (Domination_Timer <= diff)
        {
            Unit *pTarget;
            pTarget = SelectUnit(SELECT_TARGET_RANDOM,0);
            DoCast(pTarget,SPELL_DOMINATION);
            Domination_Timer = 60000;
        }
        else Domination_Timer -=diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_the_makerAI(Creature* pCreature)
{
    return new boss_the_makerAI (pCreature);
}

void AddSC_boss_the_maker()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_the_maker";
    newscript->GetAI = &GetAI_boss_the_makerAI;
    newscript->RegisterSelf();
}

