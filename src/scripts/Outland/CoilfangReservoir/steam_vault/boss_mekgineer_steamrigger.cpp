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
SDName: Boss_Mekgineer_Steamrigger
SD%Complete: 60
SDComment: Mechanics' interrrupt heal doesn't work very well, also a proper movement needs to be implemented -> summon further away and move towards pTarget to repair.
SDCategory: Coilfang Resevoir, The Steamvault
EndScriptData */

/* ContentData
boss_mekgineer_steamrigger
mob_steamrigger_mechanic
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "steam_vault.h"

#define SAY_MECHANICS               -1545007
#define SAY_AGGRO_1                 -1545008
#define SAY_AGGRO_2                 -1545009
#define SAY_AGGRO_3                 -1545010
#define SAY_AGGRO_4                 -1545011
#define SAY_SLAY_1                  -1545012
#define SAY_SLAY_2                  -1545013
#define SAY_SLAY_3                  -1545014
#define SAY_DEATH                   -1545015

#define SPELL_SUPER_SHRINK_RAY      31485
#define SPELL_SAW_BLADE             31486
#define SPELL_ELECTRIFIED_NET       35107
#define H_SPELL_ENRAGE              1                       //corrent enrage spell not known

#define ENTRY_STREAMRIGGER_MECHANIC 17951

struct boss_mekgineer_steamriggerAI : public ScriptedAI
{
    boss_mekgineer_steamriggerAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
        HeroicMode = me->GetMap()->IsHeroic();
    }

    ScriptedInstance* pInstance;
    bool HeroicMode;

    uint32 Shrink_Timer;
    uint32 Saw_Blade_Timer;
    uint32 Electrified_Net_Timer;
    bool Summon75;
    bool Summon50;
    bool Summon25;

    void Reset()
    {
        Shrink_Timer = 20000;
        Saw_Blade_Timer = 15000;
        Electrified_Net_Timer = 10000;

        Summon75 = false;
        Summon50 = false;
        Summon25 = false;

        if (pInstance && me->IsAlive())
            pInstance->SetData(TYPE_MEKGINEER_STEAMRIGGER, NOT_STARTED);
    }

    void JustDied(Unit* /*Killer*/)
    {
        DoScriptText(SAY_DEATH, me);

        if (pInstance)
            pInstance->SetData(TYPE_MEKGINEER_STEAMRIGGER, DONE);
    }

    void KilledUnit(Unit* /*victim*/)
    {
        switch (rand() % 3)
        {
        case 0:
            DoScriptText(SAY_SLAY_1, me);
            break;
        case 1:
            DoScriptText(SAY_SLAY_2, me);
            break;
        case 2:
            DoScriptText(SAY_SLAY_3, me);
            break;
        }
    }

    void EnterCombat(Unit* /*who*/)
    {
        switch (rand() % 3)
        {
        case 0:
            DoScriptText(SAY_AGGRO_1, me);
            break;
        case 1:
            DoScriptText(SAY_AGGRO_2, me);
            break;
        case 2:
            DoScriptText(SAY_AGGRO_3, me);
            break;
        }

        if (pInstance)
            pInstance->SetData(TYPE_MEKGINEER_STEAMRIGGER, IN_PROGRESS);
    }

    //no known summon spells exist
    void SummonMechanichs()
    {
        DoScriptText(SAY_MECHANICS, me);

        DoSpawnCreature(ENTRY_STREAMRIGGER_MECHANIC, 5, 5, 0, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 240000);
        DoSpawnCreature(ENTRY_STREAMRIGGER_MECHANIC, -5, 5, 0, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 240000);
        DoSpawnCreature(ENTRY_STREAMRIGGER_MECHANIC, -5, -5, 0, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 240000);

        if (rand() % 2)
            DoSpawnCreature(ENTRY_STREAMRIGGER_MECHANIC, 5, -7, 0, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 240000);
        if (rand() % 2)
            DoSpawnCreature(ENTRY_STREAMRIGGER_MECHANIC, 7, -5, 0, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 240000);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (Shrink_Timer <= diff)
        {
            DoCastVictim(SPELL_SUPER_SHRINK_RAY);
            Shrink_Timer = 20000;
        }
        else Shrink_Timer -= diff;

        if (Saw_Blade_Timer <= diff)
        {
            if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 1))
                DoCast(pTarget, SPELL_SAW_BLADE);
            else
                DoCastVictim(SPELL_SAW_BLADE);

            Saw_Blade_Timer = 15000;
        }
        else Saw_Blade_Timer -= diff;

        if (Electrified_Net_Timer <= diff)
        {
            DoCastVictim(SPELL_ELECTRIFIED_NET);
            Electrified_Net_Timer = 10000;
        }
        else Electrified_Net_Timer -= diff;

        if (!Summon75)
        {
            if (HealthBelowPct(75))
            {
                SummonMechanichs();
                Summon75 = true;
            }
        }

        if (!Summon50)
        {
            if (HealthBelowPct(50))
            {
                SummonMechanichs();
                Summon50 = true;
            }
        }

        if (!Summon25)
        {
            if (HealthBelowPct(25))
            {
                SummonMechanichs();
                Summon25 = true;
            }
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_mekgineer_steamrigger(Creature* pCreature)
{
    return GetInstanceAI<boss_mekgineer_steamriggerAI>(pCreature);
}

#define SPELL_DISPEL_MAGIC          17201
#define SPELL_REPAIR                31532
#define H_SPELL_REPAIR              37936

#define MAX_REPAIR_RANGE            (13.0f)                 //we should be at least at this range for repair
#define MIN_REPAIR_RANGE            (7.0f)                  //we can stop movement at this range to repair but not required

struct mob_steamrigger_mechanicAI : public ScriptedAI
{
    mob_steamrigger_mechanicAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
        HeroicMode = me->GetMap()->IsHeroic();
    }

    ScriptedInstance* pInstance;
    bool HeroicMode;

    uint32 Repair_Timer;

    void Reset()
    {
        Repair_Timer = 2000;
    }

    void MoveInLineOfSight(Unit* /*who*/)
    {
        //react only if attacked
        return;
    }

    void EnterCombat(Unit* /*who*/) { }

    void UpdateAI(const uint32 diff)
    {
        if (Repair_Timer <= diff)
        {
            if (pInstance && pInstance->GetData64(DATA_MEKGINEERSTEAMRIGGER) && pInstance->GetData(TYPE_MEKGINEER_STEAMRIGGER) == IN_PROGRESS)
            {
                if (Unit* pMekgineer = Unit::GetUnit((*me), pInstance->GetData64(DATA_MEKGINEERSTEAMRIGGER)))
                {
                    if (me->IsWithinDistInMap(pMekgineer, MAX_REPAIR_RANGE))
                    {
                        //are we already channeling? Doesn't work very well, find better check?
                        if (!me->GetUInt32Value(UNIT_CHANNEL_SPELL))
                        {
                            //me->GetMotionMaster()->MovementExpired();
                            //me->GetMotionMaster()->MoveIdle();

                            DoCast(me, HeroicMode ? H_SPELL_REPAIR : SPELL_REPAIR, true);
                        }
                        Repair_Timer = 5000;
                    }
                    else
                    {
                        //me->GetMotionMaster()->MovementExpired();
                        //me->GetMotionMaster()->MoveFollow(pMekgineer,0,0);
                    }
                }
            }
            else Repair_Timer = 5000;
        }
        else Repair_Timer -= diff;

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_steamrigger_mechanic(Creature* pCreature)
{
    return GetInstanceAI<mob_steamrigger_mechanicAI>(pCreature);
}

void AddSC_boss_mekgineer_steamrigger()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_mekgineer_steamrigger";
    newscript->GetAI = &GetAI_boss_mekgineer_steamrigger;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_steamrigger_mechanic";
    newscript->GetAI = &GetAI_mob_steamrigger_mechanic;
    newscript->RegisterSelf();
}

