/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* ScriptData
SDName: Boss_Balinda
SD%Complete: 
SDComment: Timers should be adjusted
EndScriptData */

#include "precompiled.h"

#define YELL_AGGRO        			    -2100019
#define YELL_EVADE				    -2100020

#define SPELL_ARCANE_EXPLOSION	    46608
#define SPELL_CONE_OF_COLD	    	    38384
#define SPELL_FIREBALL	                    46988
#define SPELL_FROSTBOLT	                    46987
#define SPELL_WATER_ELEMENTAL	    45067


struct OREGON_DLL_DECL boss_balindaAI : public ScriptedAI
{
    boss_balindaAI(Creature *c) : ScriptedAI(c) {}


    uint32 ArcaneExplosionTimer;
    uint32 ConeofcoldTimer;
    uint32 FireboltTimer;
    uint32 FrostboltTimer;
    uint32 ResetTimer;


    void Reset()
    {
    		ArcaneExplosionTimer		= (10+rand()%5)*1000;
    		ConeofcoldTimer			= 8000;
		FireboltTimer			= 1000;
		FrostboltTimer			= 4000;
		ResetTimer			= 5000;
    }

    void Aggro(Unit *who)
    {
        DoScriptText(YELL_AGGRO, m_creature);
    }

    void JustRespawned()
    {
        InCombat = false;
        Reset();
    }

    void KilledUnit(Unit* victim){}

    void JustDied(Unit* Killer){}

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if (ArcaneExplosionTimer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_ARCANE_EXPLOSION);
            ArcaneExplosionTimer =  (10+rand()%5)*1000;
        }else ArcaneExplosionTimer -= diff;

        if (ConeofcoldTimer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_CONE_OF_COLD);
            ConeofcoldTimer = (10+rand()%10)*1000;
        }else ConeofcoldTimer -= diff;

        if (FireboltTimer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_FIREBALL);
            FireboltTimer = (5+rand()%4)*1000;
        }else FireboltTimer -= diff;

        if (FrostboltTimer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_FROSTBOLT);
            FrostboltTimer = (4+rand()%8)*1000;
        }else FrostboltTimer -= diff;


	// check if creature is not outside of building
        if(ResetTimer < diff)
        {
            float x, y, z;
            m_creature->GetPosition(x, y, z);
            if(x > -6)
		{
		    DoScriptText(YELL_EVADE, m_creature);
            	    EnterEvadeMode();
		}
            ResetTimer = 5000;
        }else ResetTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_balinda(Creature *_Creature)
{
    return new boss_balindaAI (_Creature);
}

void AddSC_boss_balinda()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_balinda";
    newscript->GetAI = &GetAI_boss_balinda;
    newscript->RegisterSelf();
}
