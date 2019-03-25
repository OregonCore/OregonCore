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

enum MekgineerSteamrigger
{
    SAY_MECHANICS               = - 1545007,
    SAY_AGGRO_1                 = - 1545008,
    SAY_AGGRO_2                 = - 1545009,
    SAY_AGGRO_3                 = - 1545010,
    SAY_AGGRO_4                 = - 1545011,
    SAY_SLAY_1                  = - 1545012,
    SAY_SLAY_2                  = - 1545013,
    SAY_SLAY_3                  = - 1545014,
    SAY_DEATH                   = - 1545015,

    SPELL_SUPER_SHRINK_RAY      = 31485,
    SPELL_SAW_BLADE             = 31486,
    SPELL_ELECTRIFIED_NET       = 35107,
    SPELL_REPAIR_N              = 31532,
    SPELL_REPAIR_H              = 37936,

    NPC_STREAMRIGGER_MECHANIC   = 17951,

    EVENT_CHECK_HP25            = 1,
    EVENT_CHECK_HP50            = 2,
    EVENT_CHECK_HP75            = 3,
    EVENT_SPELL_SHRINK          = 4,
    EVENT_SPELL_SAW             = 5,
    EVENT_SPELL_NET             = 6
};

struct boss_mekgineer_steamriggerAI : public ScriptedAI
{
    boss_mekgineer_steamriggerAI(Creature* c) : ScriptedAI(c), summons(me)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }

    ScriptedInstance* pInstance;
    EventMap events;
    SummonList summons;

    void Reset()
    {
        events.Reset();
        if (pInstance && me->IsAlive())
            pInstance->SetData(TYPE_MEKGINEER_STEAMRIGGER, NOT_STARTED);
    }

    void JustDied(Unit* /*Killer*/)
    {
        DoScriptText(SAY_DEATH, me);

        summons.DespawnAll();

        if (pInstance)
            pInstance->SetData(TYPE_MEKGINEER_STEAMRIGGER, DONE);
    }

    void KilledUnit(Unit* victim)
    {
        DoScriptText(RAND(SAY_SLAY_1, SAY_SLAY_2, SAY_SLAY_3), me);
    }

    void EnterCombat(Unit* /*who*/)
    {
        DoScriptText(RAND(SAY_AGGRO_1, SAY_AGGRO_2, SAY_AGGRO_3), me);

        events.ScheduleEvent(EVENT_SPELL_SHRINK, 20000);
        events.ScheduleEvent(EVENT_SPELL_SAW, 15000);
        events.ScheduleEvent(EVENT_SPELL_NET, 10000);
        events.ScheduleEvent(EVENT_CHECK_HP75, 5000);
        events.ScheduleEvent(EVENT_CHECK_HP50, 5000);
        events.ScheduleEvent(EVENT_CHECK_HP25, 5000);

        if (pInstance)
            pInstance->SetData(TYPE_MEKGINEER_STEAMRIGGER, IN_PROGRESS);
    }

    //no known summon spells exist
    void SummonMechanichs()
    {
        DoScriptText(SAY_MECHANICS, me);

        me->SummonCreature(NPC_STREAMRIGGER_MECHANIC, me->GetPositionX() + 15.0f, me->GetPositionY() + 15.0f, me->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
        me->SummonCreature(NPC_STREAMRIGGER_MECHANIC, me->GetPositionX() - 15.0f, me->GetPositionY() + 15.0f, me->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
        me->SummonCreature(NPC_STREAMRIGGER_MECHANIC, me->GetPositionX() - 15.0f, me->GetPositionY() - 15.0f, me->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
        if (urand(0, 1))
            me->SummonCreature(NPC_STREAMRIGGER_MECHANIC, me->GetPositionX() + 15.0f, me->GetPositionY() - 15.0f, me->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);

    }


    void JustSummoned(Creature* cr)
    {
        cr->GetMotionMaster()->MoveFollow(me, 0.0f, 0.0f);

        summons.Summon(cr);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        switch (uint32 eventId = events.ExecuteEvent())
        {
        case EVENT_SPELL_SHRINK:
            me->CastSpell(me->GetVictim(), SPELL_SUPER_SHRINK_RAY, false);
            events.Repeat(20000);
            break;
        case EVENT_SPELL_SAW:
            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1))
                me->CastSpell(target, SPELL_SAW_BLADE, false);
            else
                me->CastSpell(me->GetVictim(), SPELL_SAW_BLADE, false);
            events.Repeat(15000);
            break;
        case EVENT_SPELL_NET:
            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                me->CastSpell(target, SPELL_ELECTRIFIED_NET, false);
            events.Repeat(10000);
            break;
        case EVENT_CHECK_HP25:
        case EVENT_CHECK_HP50:
        case EVENT_CHECK_HP75:
            if (me->HealthBelowPct(eventId * 25))
            {
                SummonMechanichs();
                events.ExecuteEvent();
                return;
            }
            events.Repeat(2000);
            break;
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
    uint32 repairTimer;
    uint64 bossGUID;

    void Reset()
    {
        repairTimer = 0;
        bossGUID = 0;
        if (pInstance)
                bossGUID = pInstance->GetData64(DATA_MEKGINEERSTEAMRIGGER);
    }

    void MoveInLineOfSight(Unit* /*who*/){}

    void EnterCombat(Unit* /*who*/) { }

    void UpdateAI(const uint32 diff)
    {
        repairTimer += diff;

        if (repairTimer >= 2000)
        {
            repairTimer = 0;
            if (Unit* boss = ObjectAccessor::GetUnit(*me, bossGUID))
            {
                if (me->IsWithinDistInMap(boss, 13.0f))
                    if (!me->HasUnitState(UNIT_STATE_CASTING))
                        me->CastSpell(me, HeroicMode ? SPELL_REPAIR_H : SPELL_REPAIR_N, false);
            }
            return;
        }

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

