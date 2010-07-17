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
SDName: Boss_Kurinnaxx
SD%Complete: 95
SDComment: maybe wrong Timer
SDCategory: Ruins of Ahn'Qiraj
EndScriptData */

#include "precompiled.h"

#define SPELL_MORTALWOUND       25646
#define SPELL_SANDTRAP          25648
#define SPELL_ENRAGE            26527

#define SPELL_THRASH            3391
#define SPELL_SUMMON            26446
#define SPELL_SLASH             25814

struct OREGON_DLL_DECL boss_kurinnaxxAI : public ScriptedAI
{
    boss_kurinnaxxAI(Creature *c) : ScriptedAI(c) {}

    uint32 MORTALWOUND_Timer;
    uint32 SANDTRAP_Timer;
    uint32 THRASH_Timer;
    uint32 SUMMON_Timer;
    uint32 SLASH_Timer;

    bool enraged;
    bool sandtrap;

    void Reset()
    {
        MORTALWOUND_Timer = 5000;
        SANDTRAP_Timer = 10000;
        THRASH_Timer = 7000;
        SLASH_Timer = 8500;
        SUMMON_Timer = 12000;

        sandtrap = false;
        enraged = false;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        //If we are <30% cast enrage
        if (!enraged && me->GetHealth()*100 / me->GetMaxHealth() <= 30 && !me->IsNonMeleeSpellCasted(false))
        {
            enraged = true;
            DoCast(me,SPELL_ENRAGE,true);
        } else if (enraged && !me->HasAura(SPELL_ENRAGE,0))
        {
            DoCast(me,SPELL_ENRAGE,true);
        }


        //MORTALWOUND_Timer
        if (MORTALWOUND_Timer < diff)
        {
            DoCast(me->getVictim(),SPELL_MORTALWOUND);
            MORTALWOUND_Timer = 6000 + rand()%2000;
        } else MORTALWOUND_Timer -= diff;

        if (THRASH_Timer < diff)
        {
            DoCast(me,SPELL_THRASH);
            THRASH_Timer = 3000+ rand()%5000;
        } else THRASH_Timer -= diff;

        if (SLASH_Timer < diff)
        {
            DoCast(me->getVictim(),SPELL_SLASH);
            SLASH_Timer = 5000 + rand()%5000;
        } else SLASH_Timer -= diff;

        if (SUMMON_Timer < diff)
        {
            Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM,1,70,true);
            if (pTarget)
            {
                DoCast(pTarget,SPELL_SUMMON);
            }
            SUMMON_Timer = 8000 + rand()%2000;
        } else SUMMON_Timer -= diff;

        //SANDTRAP_Timer
        if (SANDTRAP_Timer < diff)
        {
            if (!sandtrap)
            {
                Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM,0,50,true);

                if (pTarget)
                {
                    pTarget->CastSpell(pTarget,SPELL_SANDTRAP,true,0,0,me->GetGUID());
                    sandtrap = true;
                }
                SANDTRAP_Timer = 5000;
            } else
            {
                GameObject* trap = FindGameObject(180647,100,me);

                if (trap)
                {
                    float x,y,z;
                    trap->GetPosition(x,y,z);

                    //trap->CastSpell((Unit*)trap,25656);
                    Creature* trigger = me->SummonCreature(15426,x,y,z,0,TEMPSUMMON_TIMED_DESPAWN,2000);

                    trigger->CastSpell(trigger,25656,false);
                    trap->Delete();
                }
                SANDTRAP_Timer = 5000;
                sandtrap = false;
            }
        } else SANDTRAP_Timer -= diff;


        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_kurinnaxx(Creature *_Creature)
{
    return new boss_kurinnaxxAI (_Creature);
}

void AddSC_boss_kurinnaxx()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name="boss_kurinnaxx";
    newscript->GetAI = &GetAI_boss_kurinnaxx;
    newscript->RegisterSelf();
}

