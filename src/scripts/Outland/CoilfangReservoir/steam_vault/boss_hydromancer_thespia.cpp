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
SDName: Boss_Hydromancer_Thespia
SD%Complete: 80
SDComment: Needs additional adjustments (when instance script is adjusted)
SDCategory: Coilfang Resevoir, The Steamvault
EndScriptData */

/* ContentData
boss_hydromancer_thespia
mob_coilfang_waterelemental
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "steam_vault.h"

enum HydromancerThespia
{
    SAY_SUMMON                      = -1545000,
    SAY_AGGRO_1                     = -1545001,
    SAY_AGGRO_2                     = -1545002,
    SAY_AGGRO_3                     = -1545003,
    SAY_SLAY_1                      = -1545004,
    SAY_SLAY_2                      = -1545005,
    SAY_DEAD                        = -1545006,

    SPELL_LIGHTNING_CLOUD           = 25033,
    SPELL_LUNG_BURST                = 31481,
    SPELL_ENVELOPING_WINDS          = 31718,

    EVENT_SPELL_LIGHTNING           = 1,
    EVENT_SPELL_LUNG                = 2,
    EVENT_SPELL_ENVELOPING          = 3

};

struct boss_thespiaAI : public ScriptedAI
{
    boss_thespiaAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }

    ScriptedInstance* pInstance;
    EventMap events;

    void Reset()
    {
        events.Reset();
        if (pInstance && me->IsAlive())
            pInstance->SetData(TYPE_HYDROMANCER_THESPIA, NOT_STARTED);
    }

    void JustDied(Unit* /*Killer*/)
    {
        DoScriptText(SAY_DEAD, me);

        if (pInstance)
            pInstance->SetData(TYPE_HYDROMANCER_THESPIA, DONE);
    }

    void KilledUnit(Unit* /*victim*/)
    {
        DoScriptText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
    }

    void EnterCombat(Unit* /*who*/)
    {

        DoScriptText(RAND(SAY_AGGRO_1, SAY_AGGRO_2, SAY_AGGRO_3), me);
        events.ScheduleEvent(EVENT_SPELL_LIGHTNING, 15000);
        events.ScheduleEvent(EVENT_SPELL_LUNG, 7000);
        events.ScheduleEvent(EVENT_SPELL_ENVELOPING, 9000);

        if (pInstance)
            pInstance->SetData(TYPE_HYDROMANCER_THESPIA, IN_PROGRESS);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        switch (events.ExecuteEvent())
        {
        case EVENT_SPELL_LIGHTNING:
            for (uint8 i = 0; i < (HeroicMode ? 2 : 1); ++i)
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                    me->CastSpell(target, SPELL_LIGHTNING_CLOUD, false);
            events.Repeat(urand(15000, 25000));
            break;
        case EVENT_SPELL_LUNG:
            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                DoCast(target, SPELL_LUNG_BURST);
            events.Repeat(urand(7000, 12000));
            break;
        case EVENT_SPELL_ENVELOPING:
            for (uint8 i = 0; i < (HeroicMode ? 2 : 1); ++i)
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                    me->CastSpell(target, SPELL_ENVELOPING_WINDS, false);
            events.Repeat(urand(10000, 15000));
            break;
        }
        DoMeleeAttackIfReady();
    }
};

#define SPELL_WATER_BOLT_VOLLEY     34449
#define H_SPELL_WATER_BOLT_VOLLEY   37924

struct mob_coilfang_waterelementalAI : public ScriptedAI
{
    mob_coilfang_waterelementalAI(Creature* c) : ScriptedAI(c) {}

    bool HeroicMode;
    uint32 WaterBoltVolley_Timer;

    void Reset()
    {
        HeroicMode = me->GetMap()->IsHeroic();
        WaterBoltVolley_Timer = 3000 + rand() % 3000;
    }

    void EnterCombat(Unit* /*who*/) { }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (WaterBoltVolley_Timer <= diff)
        {
            DoCast(me, HeroicMode ? H_SPELL_WATER_BOLT_VOLLEY : SPELL_WATER_BOLT_VOLLEY);
            WaterBoltVolley_Timer = 7000 + rand() % 5000;
        }
        else WaterBoltVolley_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_thespiaAI(Creature* pCreature)
{
    return GetInstanceAI<boss_thespiaAI>(pCreature);
}

CreatureAI* GetAI_mob_coilfang_waterelementalAI(Creature* pCreature)
{
    return new mob_coilfang_waterelementalAI (pCreature);
}

void AddSC_boss_hydromancer_thespia()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_hydromancer_thespia";
    newscript->GetAI = &GetAI_boss_thespiaAI;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_coilfang_waterelemental";
    newscript->GetAI = &GetAI_mob_coilfang_waterelementalAI;
    newscript->RegisterSelf();
}

