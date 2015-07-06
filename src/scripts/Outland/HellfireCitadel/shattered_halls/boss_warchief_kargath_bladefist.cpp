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
SDName: Boss_Warchief_Kargath_Bladefist
SD%Complete: 99
SDComment:
SDCategory: Hellfire Citadel, Shattered Halls
EndScriptData */

/* ContentData
boss_warchief_kargath_bladefist
EndContentData */

#include "ScriptPCH.h"
#include "shattered_halls.h"

#define SAY_AGGRO1                      -1540042
#define SAY_AGGRO2                      -1540043
#define SAY_AGGRO3                      -1540044
#define SAY_SLAY1                       -1540045
#define SAY_SLAY2                       -1540046
#define SAY_DEATH                       -1540047

#define SPELL_BLADE_DANCE               30739
#define H_SPELL_CHARGE                  25821

#define TARGET_NUM                      5

#define MOB_SHATTERED_ASSASSIN          17695
#define MOB_HEARTHEN_GUARD              17621
#define MOB_SHARPSHOOTER_GUARD          17622
#define MOB_REAVER_GUARD                17623

float AssassEntrance[3] = {275.136f, -84.29f, 2.3f}; // y +-8
float AssassExit[3] = {184.233f, -84.29f, 2.3f}; // y +-8
float AddsEntrance[3] = {306.036f, -84.29f, 1.93f};

struct boss_warchief_kargath_bladefistAI : public ScriptedAI
{
    boss_warchief_kargath_bladefistAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
        HeroicMode = me->GetMap()->IsHeroic();
    }

    ScriptedInstance* pInstance;
    bool HeroicMode;

    std::vector<uint64> adds;
    std::vector<uint64> assassins;

    uint32 Charge_timer;
    uint32 Blade_Dance_Timer;
    uint32 Summon_Assistant_Timer;
    uint32 resetcheck_timer;
    uint32 Wait_Timer;

    uint32 Assassins_Timer;

    uint32 summoned;
    bool InBlade;

    uint32 target_num;

    void Reset()
    {
        removeAdds();

        me->SetSpeed(MOVE_RUN, 2);
        me->SetWalk(false);

        summoned = 1;
        InBlade = false;
        Wait_Timer = 0;

        Charge_timer = 0;
        Blade_Dance_Timer = 30000;
        Summon_Assistant_Timer = 15000;
        Assassins_Timer = 5000;
        resetcheck_timer = 5000;

        if (pInstance)
            pInstance->SetData(DATA_KARGATH, NOT_STARTED);
    }

    void EnterCombat(Unit* /*who*/)
    {
        DoScriptText(RAND(SAY_AGGRO1, SAY_AGGRO2, SAY_AGGRO3), me);

        if (pInstance)
        {
            pInstance->SetData(DATA_KARGATH, IN_PROGRESS);

            if (pInstance->GetData64(DATA_WARBRINGER))
            {
                Creature* pWar = Unit::GetCreature(*me, pInstance->GetData64(DATA_WARBRINGER));
                if (pWar && pWar->IsAlive())
                    pWar->AI()->AttackStart(me->getVictim());
            }
        }
    }

    void JustSummoned(Creature* summoned)
    {
        switch (summoned->GetEntry())
        {
        case MOB_HEARTHEN_GUARD:
        case MOB_SHARPSHOOTER_GUARD:
        case MOB_REAVER_GUARD:
            summoned->AI()->AttackStart(SelectUnit(SELECT_TARGET_RANDOM, 0));
            adds.push_back(summoned->GetGUID());
            break;
        case MOB_SHATTERED_ASSASSIN:
            assassins.push_back(summoned->GetGUID());
            break;
        }
    }

    void KilledUnit(Unit* victim)
    {
        if (victim->GetTypeId() == TYPEID_PLAYER)
            DoScriptText(RAND(SAY_SLAY1, SAY_SLAY2), me);
    }

    void JustDied(Unit* /*Killer*/)
    {
        DoScriptText(SAY_DEATH, me);
        removeAdds();

        if (pInstance)
            pInstance->SetData(DATA_KARGATH, DONE);
    }

    void MovementInform(uint32 type, uint32 id)
    {
        if (InBlade)
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (id != 1)
                return;

            if (target_num > 0) // to prevent loops
            {
                Wait_Timer = 1;
                DoCast(me, SPELL_BLADE_DANCE, true);
                target_num--;
            }
        }
    }

    void removeAdds()
    {
        for (std::vector<uint64>::iterator itr = adds.begin(); itr != adds.end(); ++itr)
        {
            Unit* temp = Unit::GetUnit((*me), *itr);
            if (temp && temp->IsAlive())
            {
                (*temp).GetMotionMaster()->Clear(true);
                me->DealDamage(temp, temp->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                CAST_CRE(temp)->RemoveCorpse();
            }
        }
        adds.clear();

        for (std::vector<uint64>::iterator itr = assassins.begin(); itr != assassins.end(); ++itr)
        {
            Unit* temp = Unit::GetUnit((*me), *itr);
            if (temp && temp->IsAlive())
            {
                (*temp).GetMotionMaster()->Clear(true);
                me->DealDamage(temp, temp->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                CAST_CRE(temp)->RemoveCorpse();
            }
        }
        assassins.clear();
    }
    void SpawnAssassin()
    {
        me->SummonCreature(MOB_SHATTERED_ASSASSIN, AssassEntrance[0], AssassEntrance[1] + 8, AssassEntrance[2], 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
        me->SummonCreature(MOB_SHATTERED_ASSASSIN, AssassEntrance[0], AssassEntrance[1] - 8, AssassEntrance[2], 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
        me->SummonCreature(MOB_SHATTERED_ASSASSIN, AssassExit[0], AssassExit[1] + 8, AssassExit[2], 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
        me->SummonCreature(MOB_SHATTERED_ASSASSIN, AssassExit[0], AssassExit[1] - 8, AssassExit[2], 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (Assassins_Timer)
        {
            if (Assassins_Timer <= diff)
            {
                SpawnAssassin();
                Assassins_Timer = 0;
            }
            else Assassins_Timer -= diff;
        }
        if (InBlade)
        {
            if (Wait_Timer)
            {
                if (Wait_Timer <= diff)
                {
                    if (target_num <= 0)
                    {
                        // stop bladedance
                        InBlade = false;
                        me->SetSpeed(MOVE_RUN, 2);
                        (*me).GetMotionMaster()->MoveChase(me->getVictim());
                        Blade_Dance_Timer = 30000;
                        Wait_Timer = 0;
                        if (HeroicMode)
                            Charge_timer = 5000;
                    }
                    else
                    {
                        //move in bladedance
                        float x, y, randx, randy;
                        randx = float(rand() % 40);
                        randy = float(rand() % 40);
                        x = 210 + randx ;
                        y = -60 - randy ;
                        (*me).GetMotionMaster()->MovePoint(1, x, y, me->GetPositionZ());
                        Wait_Timer = 0;
                    }
                }
                else Wait_Timer -= diff;
            }
        }
        else
        {
            if (Blade_Dance_Timer)
            {
                if (Blade_Dance_Timer <= diff)
                {
                    target_num = TARGET_NUM;
                    Wait_Timer = 1;
                    InBlade = true;
                    Blade_Dance_Timer = 0;
                    me->SetSpeed(MOVE_RUN, 4);
                    return;
                }
                else Blade_Dance_Timer -= diff;
            }
            if (Charge_timer)
            {
                if (Charge_timer <= diff)
                {
                    DoCast(SelectUnit(SELECT_TARGET_RANDOM, 0), H_SPELL_CHARGE);
                    Charge_timer = 0;
                }
                else Charge_timer -= diff;
            }
            if (Summon_Assistant_Timer <= diff)
            {
                for (uint32 i = 0; i < summoned; i++)
                {
                    switch (rand() % 3)
                    {
                    case 0:
                        me->SummonCreature(MOB_HEARTHEN_GUARD, AddsEntrance[0], AddsEntrance[1], AddsEntrance[2], 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
                        break;
                    case 1:
                        me->SummonCreature(MOB_SHARPSHOOTER_GUARD, AddsEntrance[0], AddsEntrance[1], AddsEntrance[2], 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
                        break;
                    case 2:
                        me->SummonCreature(MOB_REAVER_GUARD, AddsEntrance[0], AddsEntrance[1], AddsEntrance[2], 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
                        break;
                    }
                }
                if (rand() % 100 < 6) summoned++;
                Summon_Assistant_Timer = 15000 + (rand() % 5000) ;
            }
            else Summon_Assistant_Timer -= diff;

            DoMeleeAttackIfReady();
        }

        if (resetcheck_timer <= diff)
        {
            float tempx;
            tempx = me->GetPositionX();
            if (tempx > 255 || tempx < 205)
            {
                EnterEvadeMode();
                return;
            }
            resetcheck_timer = 5000;
        }
        else resetcheck_timer -= diff;
    }
};

CreatureAI* GetAI_boss_warchief_kargath_bladefist(Creature* pCreature)
{
    return new boss_warchief_kargath_bladefistAI (pCreature);
}

void AddSC_boss_warchief_kargath_bladefist()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "boss_warchief_kargath_bladefist";
    newscript->GetAI = &GetAI_boss_warchief_kargath_bladefist;
    newscript->RegisterSelf();
}

