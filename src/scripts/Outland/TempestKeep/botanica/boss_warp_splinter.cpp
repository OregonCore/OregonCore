/*
 * Copyright (C) 2010-2012 OregonCore <http://www.oregoncore.com/>
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
SDName: Boss_Warp_Splinter
SD%Complete: 80
SDComment: Includes Sapling (need some better control with these).
SDCategory: Tempest Keep, The Botanica
EndScriptData */

#include "ScriptPCH.h"

/*#####
# mob_treant (Sapling)
#####*/

#define SPELL_HEAL_FATHER   6262

struct mob_treantAI  : public ScriptedAI
{
    mob_treantAI (Creature *c) : ScriptedAI(c)
    {
        WarpGuid = 0;
    }

    uint64 WarpGuid;
    uint32 check_Timer;

    void Reset()
    {
        check_Timer = 0;
    }

    void EnterCombat(Unit *who) {}

    void MoveInLineOfSight(Unit*) {}

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
        {
            if (WarpGuid && check_Timer <= diff)
            {
                if (Unit *Warp = (Unit*)Unit::GetUnit(*me, WarpGuid))
                {
                    if (me->IsWithinMeleeRange(Warp,2.5f))
                    {
                        int32 CurrentHP_Treant = (int32)me->GetHealth();
                        Warp->CastCustomSpell(Warp,SPELL_HEAL_FATHER,&CurrentHP_Treant, 0, 0, true,0 ,0, me->GetGUID());
                        me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                        return;
                    }
                    me->GetMotionMaster()->MoveFollow(Warp,0,0);
                }
                check_Timer = 1000;
            } else check_Timer -= diff;
            return;
        }

        if (me->getVictim()->GetGUID() != WarpGuid)
            DoMeleeAttackIfReady();
    }
};

/*#####
# boss_warp_splinter
#####*/

#define SAY_AGGRO           -1553007
#define SAY_SLAY_1          -1553008
#define SAY_SLAY_2          -1553009
#define SAY_SUMMON_1        -1553010
#define SAY_SUMMON_2        -1553011
#define SAY_DEATH           -1553012

#define WAR_STOMP           34716
#define SUMMON_TREANTS      34727                           // DBC: 34727, 34731, 34733, 34734, 34736, 34739, 34741 (with Ancestral Life spell 34742)   // won't work (guardian summon)
#define ARCANE_VOLLEY       (HeroicMode?39133:36705)

#define CREATURE_TREANT     19949

#define TREANT_SPAWN_DIST   50                              //50 yards from Warp Splinter's spawn point

float treant_pos[6][3] =
{
    {24.301233f, 427.221100f, -27.060635f},
    {16.795492f, 359.678802f, -27.355425f},
    {53.493484f, 345.381470f, -26.196192f},
    {61.867096f, 439.362732f, -25.921030f},
    {109.861877f, 423.201630f, -27.356019f},
    {106.780159f, 355.582581f, -27.593357f}
};

struct boss_warp_splinterAI : public ScriptedAI
{
    boss_warp_splinterAI(Creature *c) : ScriptedAI(c)
    {
        HeroicMode = c->GetMap()->IsHeroic();
        Treant_Spawn_Pos_X = c->GetPositionX();
        Treant_Spawn_Pos_Y = c->GetPositionY();
    }

    uint32 War_Stomp_Timer;
    uint32 Summon_Treants_Timer;
    uint32 Arcane_Volley_Timer;
    bool HeroicMode;

    float Treant_Spawn_Pos_X;
    float Treant_Spawn_Pos_Y;

    void Reset()
    {
        War_Stomp_Timer = 25000 + rand()%15000;
        Summon_Treants_Timer = 45000;
        Arcane_Volley_Timer = 8000 + rand()%12000;

        me->SetSpeed(MOVE_RUN, 0.7f, true);
    }

    void EnterCombat(Unit *who)
    {
        DoScriptText(SAY_AGGRO, me);
    }

    void KilledUnit(Unit* victim)
    {
        switch(rand()%2)
        {
        case 0: DoScriptText(SAY_SLAY_1, me); break;
        case 1: DoScriptText(SAY_SLAY_2, me); break;
        }
    }

    void JustDied(Unit* Killer)
    {
        DoScriptText(SAY_DEATH, me);
    }

    void SummonTreants()
    {
        for (int i = 0; i < 6; ++i)
        {
            float angle = (M_PI / 3) * i;

            float X = Treant_Spawn_Pos_X + TREANT_SPAWN_DIST * cos(angle);
            float Y = Treant_Spawn_Pos_Y + TREANT_SPAWN_DIST * sin(angle);
            float O = - me->GetAngle(X,Y);

            if (Creature *pTreant = me->SummonCreature(CREATURE_TREANT,treant_pos[i][0],treant_pos[i][1],treant_pos[i][2],O,TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN,25000))
                ((mob_treantAI*)pTreant->AI())->WarpGuid = me->GetGUID();
        }
        switch(rand()%2)
        {
        case 0: DoScriptText(SAY_SUMMON_1, me); break;
        case 1: DoScriptText(SAY_SUMMON_2, me); break;
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        //Check for War Stomp
        if (War_Stomp_Timer <= diff)
        {
            DoCast(me->getVictim(),WAR_STOMP);
            War_Stomp_Timer = 25000 + rand()%15000;
        } else War_Stomp_Timer -= diff;

        //Check for Arcane Volley
        if (Arcane_Volley_Timer <= diff)
        {
            DoCast(me->getVictim(),ARCANE_VOLLEY);
            Arcane_Volley_Timer = 20000 + rand()%15000;
        } else Arcane_Volley_Timer -= diff;

        //Check for Summon Treants
        if (Summon_Treants_Timer <= diff)
        {
            SummonTreants();
            Summon_Treants_Timer = 45000;
        } else Summon_Treants_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_warp_splinter(Creature* pCreature)
{
    return new boss_warp_splinterAI (pCreature);
}

CreatureAI* GetAI_mob_treant(Creature* pCreature)
{
    return new mob_treantAI (pCreature);
}

void AddSC_boss_warp_splinter()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_warp_splinter";
    newscript->GetAI = &GetAI_boss_warp_splinter;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_warp_splinter_treant";
    newscript->GetAI = &GetAI_mob_treant;
    newscript->RegisterSelf();
}

