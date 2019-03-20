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
SDName: Boss_Anzu
SD%Complete: 95
SDComment:Summon event is missing.
SDCategory: Auchindoun, Sethekk Halls
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "sethekk_halls.h"

#define SAY_INTRO1          -1900169
#define SAY_INTRO2          -1900170
#define SAY_HELP            -1900171

#define SPELL_CYCLONE       40321
#define SPELL_RIP           40199
#define SPELL_PARALYZING    40184
#define SPELL_BOMB          40303
#define SPELL_BANISH        42354
#define SPELL_SHADOWFORM    40973

#define GO_CLAW             185554

#define NPC_BROOD           23132

enum anzu
{
    EVENT_SPELL_SCREECH = 1,
    EVENT_SPELL_BOMB = 2,
    EVENT_SPELL_CYCLONE = 3,
    EVENT_ANZU_HEALTH1 = 4,
    EVENT_ANZU_HEALTH2 = 5
};

struct boss_anzuAI : public ScriptedAI
{
    boss_anzuAI(Creature* c) : ScriptedAI(c), summons(me)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
        HeroicMode = me->GetMap()->IsHeroic();

        talkTimer = 1;
        me->SetUInt32Value(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        me->AddAura(SPELL_SHADOWFORM, me);

        if (GameObject* pClaw = me->FindNearestGameObject(GO_CLAW, 20))
            pClaw->RemoveFromWorld();
    }

    ScriptedInstance* pInstance;
    EventMap events;
    uint32 talkTimer;
    SummonList summons;
    bool HeroicMode;

    void Reset()
    {
        summons.DespawnAll();

        if (GameObject* pClaw = me->FindNearestGameObject(GO_CLAW, 20))
            if (!pClaw)
                pClaw->AddToWorld();

        if (pInstance)
            pInstance->SetData(DATA_ANZUEVENT, NOT_STARTED);
    }

    void JustSummoned(Creature* summon)
    {
        summons.Summon(summon);
        summon->AI()->AttackStart(me->GetVictim());
    }

    void SummonedCreatureDies(Creature* summon, Unit*)
    {
        summons.Despawn(summon);
        
        if (summons.empty())
            me->RemoveAurasDueToSpell(SPELL_BANISH);
    }

    void EnterCombat(Unit*)
    {
        events.Reset();
        events.ScheduleEvent(EVENT_SPELL_SCREECH, 14000);
        events.ScheduleEvent(EVENT_SPELL_BOMB, 5000);
        events.ScheduleEvent(EVENT_SPELL_CYCLONE, 8000);
        events.ScheduleEvent(EVENT_ANZU_HEALTH1, 2000);
        events.ScheduleEvent(EVENT_ANZU_HEALTH2, 2001);

        if (pInstance)
            pInstance->SetData(DATA_ANZUEVENT, IN_PROGRESS);
    }

    void JustDied(Unit*)
    {
        if (pInstance)
            pInstance->SetData(DATA_ANZUEVENT, DONE);
    }

    void SummonBroods()
    {
        DoScriptText(SAY_HELP, me);
        me->CastSpell(me, SPELL_BANISH, true);
        for (int i = 0; i < 5; ++i)
        {
            Creature* brood = me->SummonCreature(NPC_BROOD, me->GetPositionX() + 20 * cos((float)i), me->GetPositionY() + 20 * sin((float)i), me->GetPositionZ() + 25.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
            brood->AI()->AttackStart(SelectTarget(SELECT_TARGET_RANDOM, 1, 45.0f, true));
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (talkTimer)
        {
            talkTimer += diff;
            if (talkTimer >= 1000 && talkTimer < 10000)
            {
                DoScriptText(SAY_INTRO1, me);
                talkTimer = 10000;
            }
            else if (talkTimer >= 16000)
            {
                me->SetUInt32Value(UNIT_FIELD_FLAGS, 0);
                me->RemoveAurasDueToSpell(SPELL_SHADOWFORM);
                DoScriptText(SAY_INTRO2, me);
                talkTimer = 0;
            }
        }
        if (!UpdateVictim())
            return;

        events.Update(diff);
        if (me->HasUnitState(UNIT_STATE_CASTING | UNIT_STATE_STUNNED))
            return;

        switch (events.ExecuteEvent())
        {
        case EVENT_SPELL_SCREECH:
            me->CastSpell(me, SPELL_PARALYZING, false);
            events.Repeat(23000);
            events.DelayEvents(3000);
            break;
        case EVENT_SPELL_BOMB:
            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.0f, true))
                me->CastSpell(target, SPELL_BOMB, false);
            events.Repeat(urand(16000, 24500));
            events.DelayEvents(3000);
            break;
        case EVENT_SPELL_CYCLONE:
            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 45.0f, true))
                me->CastSpell(target, SPELL_CYCLONE, false);
            events.Repeat(urand(22000, 27000));
            events.DelayEvents(3000);
            break;
        case EVENT_ANZU_HEALTH1:
            if (me->HealthBelowPct(66))
            {
                SummonBroods();
                events.ExecuteEvent();
                events.DelayEvents(10000);
                return;
            }
            events.Repeat(1000);
            break;
        case EVENT_ANZU_HEALTH2:
            if (me->HealthBelowPct(33))
            {
                SummonBroods();
                events.ExecuteEvent();
                events.DelayEvents(10000);
                return;
            }
            events.Repeat(1000);
            break;
        }

            DoMeleeAttackIfReady();
        
    }
};

CreatureAI* GetAI_boss_anzu(Creature* pCreature)
{
    return GetInstanceAI<boss_anzuAI>(pCreature);
}

void AddSC_boss_anzu()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "boss_anzu";
    newscript->GetAI = &GetAI_boss_anzu;
    newscript->RegisterSelf();
}
