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
SDName: Boss_The_Maker
SD%Complete: 99
SDComment: Need core support for mind control
SDCategory: Hellfire Citadel, Blood Furnace
EndScriptData */

#include "ScriptPCH.h"
#include "blood_furnace.h"

enum Texts
{
    SAY_AGGRO_1                = -1542009,
    SAY_AGGRO_2                = -1542010,
    SAY_AGGRO_3                = -1542011,
    SAY_KILL_1                 = -1542012,
    SAY_KILL_2                 = -1542013,
    SAY_DIE                    = -1542014
};

enum Spells
{
    SPELL_ACID_SPRAY            = 38153,
    SPELL_EXPLODING_BREAKER     = 30925,
    H_SPELL_EXPLODING_BREAKER   = 40059,
    SPELL_KNOCKDOWN             = 20276,
    SPELL_DOMINATION            = 25772
};

struct boss_the_makerAI : public ScriptedAI
{
    boss_the_makerAI(Creature* creature) : ScriptedAI(creature) 
	{
            instance = creature->GetInstanceData();
	}

    ScriptedInstance* instance;

    uint32 ExplodingBreaker_Timer;
    uint32 Domination_Timer;
    uint32 AcidSpray_Timer;
    uint32 Knockdown_Timer;

    void Reset()
    {
        AcidSpray_Timer = 15000;
        ExplodingBreaker_Timer = 6000;
        Domination_Timer = 120000;
        Knockdown_Timer = 10000;

        if (instance)
            instance->SetData(DATA_MAKEREVENT, NOT_STARTED);
    }

    void EnterCombat(Unit* /*who*/)
    {
        DoScriptText(RAND(SAY_AGGRO_1, SAY_AGGRO_2, SAY_AGGRO_3), me);

        if (instance)
            instance->SetData(DATA_MAKEREVENT, IN_PROGRESS);
    }

    void KilledUnit(Unit* /*victim*/)
    {
        DoScriptText(RAND(SAY_KILL_1, SAY_KILL_2), me);
    }

    void JustDied(Unit* /*Killer*/)
    {
        DoScriptText(SAY_DIE, me);

        if (instance)
            instance->SetData(DATA_MAKEREVENT, DONE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (AcidSpray_Timer <= diff)
        {
            DoCastVictim(SPELL_ACID_SPRAY);
            AcidSpray_Timer = 15000+rand()%8000;
        }
        else
             AcidSpray_Timer -=diff;

        if (ExplodingBreaker_Timer <= diff)
        {
            if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0))
                DoCast(target, HeroicMode ? H_SPELL_EXPLODING_BREAKER : SPELL_EXPLODING_BREAKER);
            ExplodingBreaker_Timer = 4000+rand()%8000;
        }
        else
            ExplodingBreaker_Timer -=diff;

        /* // Disabled until Core Support for mind control
        if (Domination_Timer <= diff)
        {
            Unit* pTarget;
            pTarget = SelectUnit(SELECT_TARGET_RANDOM,0);
            DoCast(pTarget,SPELL_DOMINATION);
            Domination_Timer = 60000;
        }
        else
            Domination_Timer -=diff; */
           
        if (Knockdown_Timer <= diff)
        {
            DoCast(me->getVictim(), SPELL_KNOCKDOWN);
            Knockdown_Timer = 4000+rand()%8000;
        }
        else
            Knockdown_Timer -=diff;

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

