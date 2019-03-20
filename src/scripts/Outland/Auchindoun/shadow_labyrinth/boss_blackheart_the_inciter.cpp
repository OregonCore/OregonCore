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
SDName: Boss_Blackheart_the_Inciter
SD%Complete: 75
SDComment: Incite Chaos not functional since core lacks Mind Control support
SDCategory: Auchindoun, Shadow Labyrinth
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "shadow_labyrinth.h"

#define SPELL_INCITE_CHAOS    33676
#define SPELL_INCITE_CHAOS_B  33684                         //debuff applied to each member of party
#define SPELL_CHARGE          33709
#define SPELL_WAR_STOMP       33707

#define SAY_INTRO1          -1555008
#define SAY_INTRO2          -1555009
#define SAY_INTRO3          -1555010
#define SAY_AGGRO1          -1555011
#define SAY_AGGRO2          -1555012
#define SAY_AGGRO3          -1555013
#define SAY_SLAY1           -1555014
#define SAY_SLAY2           -1555015
#define SAY_HELP            -1555016
#define SAY_DEATH           -1555017

#define SAY2_INTRO1         -1555018
#define SAY2_INTRO2         -1555019
#define SAY2_INTRO3         -1555020
#define SAY2_AGGRO1         -1555021
#define SAY2_AGGRO2         -1555022
#define SAY2_AGGRO3         -1555023
#define SAY2_SLAY1          -1555024
#define SAY2_SLAY2          -1555025
#define SAY2_HELP           -1555026
#define SAY2_DEATH          -1555027

enum events
{
    EVENT_SPELL_INCITE = 1,
    EVENT_INCITE_WAIT = 2,
    EVENT_SPELL_CHARGE = 3,
    EVENT_SPELL_KNOCKBACK = 4
};

struct boss_blackheart_the_inciterAI : public ScriptedAI
{
    boss_blackheart_the_inciterAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }

    ScriptedInstance* pInstance;
    EventMap events;
    bool InciteChaos;

    void Reset()
    {
        InciteChaos = false;
        events.Reset();

        if (pInstance)
            pInstance->SetData(DATA_BLACKHEARTTHEINCITEREVENT, NOT_STARTED);
    }

    void KilledUnit(Unit* victim)
    {

        //check if victim is player
        if (victim->GetTypeId() == TYPEID_PLAYER)
            DoScriptText(RAND(SAY_SLAY1, SAY_SLAY2), me);
    }

    void JustDied(Unit*)
    {
        DoScriptText(SAY_DEATH, me);

        if (pInstance)
            pInstance->SetData(DATA_BLACKHEARTTHEINCITEREVENT, DONE);
    }

    void EnterCombat(Unit*)
    {
        DoScriptText(RAND(SAY_AGGRO1, SAY2_AGGRO2, SAY_AGGRO3), me);

        if (pInstance)
            pInstance->SetData(DATA_BLACKHEARTTHEINCITEREVENT, IN_PROGRESS);

        events.ScheduleEvent(EVENT_SPELL_INCITE, 20000);
        events.ScheduleEvent(EVENT_INCITE_WAIT, 15000);
        events.ScheduleEvent(EVENT_SPELL_CHARGE, 0);
        events.ScheduleEvent(EVENT_SPELL_KNOCKBACK, 15000);
    }

    void EnterEvadeMode()
    {
        if (InciteChaos && SelectTargetFromPlayerList(100.0f, NULL, false))
            return;
        CreatureAI::EnterEvadeMode();
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!UpdateVictim())
            return;

        events.Update(diff);

        switch (events.ExecuteEvent())
        {
        case EVENT_INCITE_WAIT:
            InciteChaos = false;
            break;
        case EVENT_SPELL_INCITE:
        {
            me->CastSpell(me, SPELL_INCITE_CHAOS, false);

            std::list<HostileReference*> t_list = me->getThreatManager().getThreatList();
            for (std::list<HostileReference*>::const_iterator itr = t_list.begin(); itr != t_list.end(); ++itr)
            {
                Unit* target = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid());
                if (target && target->GetTypeId() == TYPEID_PLAYER)
                    me->CastSpell(target, SPELL_INCITE_CHAOS_B, true);
            }

            DoResetThreat();
            InciteChaos = true;
            events.DelayEvents(15000);
            events.Repeat(40000);
            events.ScheduleEvent(EVENT_INCITE_WAIT, 15000);
            break;
        }
        case EVENT_SPELL_CHARGE:
            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                me->CastSpell(target, SPELL_CHARGE, false);
            events.Repeat(urand(15000, 25000));
            break;
        case EVENT_SPELL_KNOCKBACK:
            me->CastSpell(me, SPELL_WAR_STOMP, false);
            events.Repeat(urand(18000, 24000));
            break;
        }

        if (InciteChaos)
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_blackheart_the_inciter(Creature* pCreature)
{
    return GetInstanceAI<boss_blackheart_the_inciterAI>(pCreature);
}

void AddSC_boss_blackheart_the_inciter()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "boss_blackheart_the_inciter";
    newscript->GetAI = &GetAI_boss_blackheart_the_inciter;
    newscript->RegisterSelf();
}

