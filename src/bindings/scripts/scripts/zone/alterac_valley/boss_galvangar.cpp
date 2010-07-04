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
SDName: Boss_Galvangar
SD%Complete:
SDComment: timers should be adjusted
EndScriptData */

#include "precompiled.h"

#define YELL_AGGRO                 -2100021
#define YELL_EVADE                 -2100022

#define SPELL_CLEAVE               15284
#define SPELL_FRIGHTENING_SHOUT    19134
#define SPELL_WHIRLWIND1           15589
#define SPELL_WHIRLWIND2           13736
#define SPELL_MORTAL_STRIKE        16856


struct OREGON_DLL_DECL boss_galvangarAI : public ScriptedAI
{
    boss_galvangarAI(Creature *c) : ScriptedAI(c) {}


    uint32 CleaveTimer;
    uint32 FrighteningShoutTimer;
    uint32 Whirlwind1Timer;
    uint32 Whirlwind2Timer;
    uint32 MortalStrikeTimer;
    uint32 ResetTimer;


    void Reset()
    {
            CleaveTimer            = (1+rand()%10)*1000;
            FrighteningShoutTimer        = (2+rand()%18)*1000;
        Whirlwind1Timer            = (1+rand()%12)*1000;
        Whirlwind2Timer            = (5+rand()%15)*1000;
        MortalStrikeTimer        = (5+rand()%20)*1000;
        ResetTimer            = 5000;
    }

    void Aggro(Unit *who)
    {
        DoScriptText(YELL_AGGRO, m_creature);
    }

    void JustRespawned()
    {
        Reset();
    }

    void KilledUnit(Unit* victim){}

    void JustDied(Unit* Killer){}

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (CleaveTimer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_CLEAVE);
            CleaveTimer =  (10+rand()%6)*1000;
        }else CleaveTimer -= diff;

        if (FrighteningShoutTimer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_FRIGHTENING_SHOUT);
            FrighteningShoutTimer = (10+rand()%5)*1000;
        }else FrighteningShoutTimer -= diff;

        if (Whirlwind1Timer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_WHIRLWIND1);
            Whirlwind1Timer = (6+rand()%14)*1000;
        }else Whirlwind1Timer -= diff;

        if (Whirlwind2Timer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_WHIRLWIND2);
            Whirlwind2Timer = (10+rand()%15)*1000;
        }else Whirlwind2Timer -= diff;

        if (MortalStrikeTimer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_MORTAL_STRIKE);
            MortalStrikeTimer = (10+rand()%20)*1000;
        }else MortalStrikeTimer -= diff;

        // check if creature is not outside of building
        if (ResetTimer < diff)
        {
            float x, y, z;
            m_creature->GetPosition(x, y, z);
            if (x > -504)
        {
                DoScriptText(YELL_EVADE, m_creature);
                    EnterEvadeMode();
        }
            ResetTimer = 5000;
        }else ResetTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_galvangar(Creature *_Creature)
{
    return new boss_galvangarAI (_Creature);
}

void AddSC_boss_galvangar()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_galvangar";
    newscript->GetAI = &GetAI_boss_galvangar;
    newscript->RegisterSelf();
}
