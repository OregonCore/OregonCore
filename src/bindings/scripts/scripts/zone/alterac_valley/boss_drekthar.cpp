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
SDName: Boss_DrekThar
SD%Complete: 50%
SDComment: Some spells listed on wowwiki but doesn't exist on wowhead
EndScriptData */

#include "precompiled.h"

#define YELL_AGGRO                 -2100000

#define YELL_EVADE           -2100001
#define YELL_RESPAWN           -2100002

#define YELL_RANDOM1           -2100003
#define YELL_RANDOM2           -2100004
#define YELL_RANDOM3           -2100005
#define YELL_RANDOM4           -2100006
#define YELL_RANDOM5           -2100007


#define SPELL_WHIRLWIND               15589
#define SPELL_WHIRLWIND2       13736
#define SPELL_KNOCKDOWN           19128
#define SPELL_FRENZY           8269
#define SPELL_SWEEPING_STRIKES     18765 // not sure
#define SPELL_CLEAVE               20677 // not sure
#define SPELL_WINDFURY           35886 // not sure
#define SPELL_STORMPIKE               51876 // not sure


struct OREGON_DLL_DECL boss_drektharAI : public ScriptedAI
{
    boss_drektharAI(Creature *c) : ScriptedAI(c) {}


    uint32 WhirlwindTimer;
    uint32 Whirlwind2Timer;
    uint32 KnockdownTimer;
    uint32 FrenzyTimer;
    uint32 YellTimer;
    uint32 ResetTimer;

    void Reset()
    {
        WhirlwindTimer        = (rand()%20)*1000;
        Whirlwind2Timer        = (rand()%25)*1000;
        KnockdownTimer        = 12000;
    FrenzyTimer        = 6000;
    ResetTimer        = 5000;
    YellTimer               = (20+rand()%10)*1000; //20 to 30 seconds
    }

    void Aggro(Unit *who)
    {
        DoScriptText(YELL_AGGRO, m_creature);
    }

    void JustRespawned()
    {
        Reset();
    DoScriptText(YELL_RESPAWN, m_creature);
    }

    void KilledUnit(Unit* victim){}

    void JustDied(Unit* Killer){}

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (WhirlwindTimer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_WHIRLWIND);
            WhirlwindTimer =  (8+rand()%10)*1000;
        } else WhirlwindTimer -= diff;

        if (Whirlwind2Timer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_WHIRLWIND2);
            Whirlwind2Timer = (7+rand()%18)*1000;
        } else Whirlwind2Timer -= diff;

        if (KnockdownTimer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_KNOCKDOWN);
            KnockdownTimer = (10+rand()%5)*1000;
        } else KnockdownTimer -= diff;

        if (FrenzyTimer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_FRENZY);
            FrenzyTimer = (20+rand()%5)*1000;
        } else FrenzyTimer -= diff;

        if (YellTimer < diff) {
        switch(rand()%4)
        {
            case 0: DoScriptText(YELL_RANDOM1, m_creature); break;
            case 1: DoScriptText(YELL_RANDOM2, m_creature); break;
            case 2: DoScriptText(YELL_RANDOM3, m_creature); break;
            case 3: DoScriptText(YELL_RANDOM4, m_creature); break;
            case 4: DoScriptText(YELL_RANDOM5, m_creature); break;
        }
        YellTimer = (20+rand()%10)*1000; //20 to 30 seconds
        } else YellTimer -= diff;

// check if creature is not outside of building
        if (ResetTimer < diff)
        {
            float x, y, z;
            m_creature->GetPosition(x, y, z);
            if (y < -260)
        {
            DoScriptText(YELL_EVADE, m_creature);
                EnterEvadeMode();
        }
            ResetTimer = 5000;
        } else ResetTimer -= diff;

        DoMeleeAttackIfReady();
    }
};


CreatureAI* GetAI_boss_drekthar(Creature *_Creature)
{
    return new boss_drektharAI (_Creature);
}

void AddSC_boss_drekthar()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_drekthar";
    newscript->GetAI = &GetAI_boss_drekthar;
    newscript->RegisterSelf();
}
