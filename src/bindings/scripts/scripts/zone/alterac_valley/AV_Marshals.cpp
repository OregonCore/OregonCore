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
SDName: AV Marshals
SD%Complete:
SDComment: Returns AV marshals to inside the building
SDCategory: Bosses
EndScriptData */

#include "precompiled.h"

#define SPELL_CHARGE               22911
#define SPELL_CLEAVE               40504
#define SPELL_DEMORALIZING_SHOUT   23511
#define SPELL_ENRAGE               8599
#define SPELL_WHIRLWIND1           15589
#define SPELL_WHIRLWIND2           13736

struct OREGON_DLL_DECL AV_MarshalsAI : public ScriptedAI
{
     AV_MarshalsAI(Creature *c) : ScriptedAI(c) {Reset();}

     uint32 ChargeTimer;
     uint32 CleaveTimer;
     uint32 DemoralizingShoutTimer;
     uint32 Whirlwind1Timer;
     uint32 Whirlwind2Timer;
     uint32 EnrageTimer;
     uint32 ResetTimer;
     uint32 Aura1;
     uint32 Aura2;
     uint32 Aura3;
     uint32 Aura4;


     void Reset()
     {
        ChargeTimer             = (2+rand()%10)*1000;
        CleaveTimer                = (1+rand()%10)*1000;
        DemoralizingShoutTimer  = (2+rand()%18)*1000;
        Whirlwind1Timer            = (1+rand()%12)*1000;
        Whirlwind2Timer            = (5+rand()%15)*1000;
        EnrageTimer                = (5+rand()%20)*1000;
        ResetTimer                = 5000;

        //switch for aures//
        Aura1                   = 0;
        Aura2                   = 0;
        Aura3                   = 0;
        Aura4                   = 0;

     }
     void Aggro(Unit *who){}

     void JustRespawned()
     {
         Reset();
     }

     void KilledUnit(Unit* victim){}

     void JustDied(Unit* Killer){}

     void UpdateAI(const uint32 diff)
     {
          //Adding the auras//
        if (me->GetEntry() == 14762 && Aura1 == 0) //North Marshal
        {
            DoCast(me, 45828);
            Aura1 = 1;
        }
        if (me->GetEntry() == 14763 && Aura2 == 0) //South Marshal
        {
            DoCast(me, 45829);
            Aura2 = 1;
        }
        if (me->GetEntry() == 14764 && Aura3 == 0) //Icewing Marshal
        {
            DoCast(me, 45831);
            Aura3 = 1;
        }
        if (me->GetEntry() == 14765 && Aura4 == 0) //Stonehearth Marshal
        {
            DoCast(me, 45830);
            Aura4 = 1;
        }
        if (!UpdateVictim())
            return;
        if (ChargeTimer <= diff)
        {
            DoCast(me->getVictim(), SPELL_CHARGE);
            ChargeTimer = (10+rand()%15)*1000;
        } else ChargeTimer -= diff;

        if (CleaveTimer <= diff)
        {
            DoCast(me->getVictim(), SPELL_CLEAVE);
            CleaveTimer =  (10+rand()%6)*1000;
        } else CleaveTimer -= diff;

        if (DemoralizingShoutTimer <= diff)
        {
            DoCast(me->getVictim(), SPELL_DEMORALIZING_SHOUT);
            DemoralizingShoutTimer = (10+rand()%5)*1000;
        } else DemoralizingShoutTimer -= diff;

        if (Whirlwind1Timer <= diff)
        {
            DoCast(me->getVictim(), SPELL_WHIRLWIND1);
            Whirlwind1Timer = (6+rand()%14)*1000;
        } else Whirlwind1Timer -= diff;

        if (Whirlwind2Timer <= diff)
        {
            DoCast(me->getVictim(), SPELL_WHIRLWIND2);
            Whirlwind2Timer = (10+rand()%15)*1000;
        } else Whirlwind2Timer -= diff;

        if (EnrageTimer <= diff)
        {
            DoCast(me->getVictim(), SPELL_ENRAGE);
            EnrageTimer = (10+rand()%20)*1000;
        } else EnrageTimer -= diff;


        // check if creature is not outside of building
        if (ResetTimer <= diff)
        {
            float x, y, z;
            me->GetPosition(x, y, z);
            if (x < 678)
            {
                EnterEvadeMode();
            }
            ResetTimer = 5000;
        } else ResetTimer -= diff;

        DoMeleeAttackIfReady();
     }
};

CreatureAI* GetAI_AV_Marshals(Creature *_Creature)
{
    return new AV_MarshalsAI (_Creature);
}

void AddSC_AV_Marshals()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "AV_Marshals";
    newscript->GetAI = &GetAI_AV_Marshals;
    newscript->RegisterSelf();
}
