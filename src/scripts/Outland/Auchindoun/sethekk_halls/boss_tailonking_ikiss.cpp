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
SDName: Boss_Talon_King_Ikiss
SD%Complete: 95
SDComment: Heroic supported. Some details missing, but most are spell related.
SDCategory: Auchindoun, Sethekk Halls
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "sethekk_halls.h"

#define SAY_INTRO                   -1556007

#define SAY_AGGRO_1                 -1556008
#define SAY_AGGRO_2                 -1556009
#define SAY_AGGRO_3                 -1556010

#define SAY_SLAY_1                  -1556011
#define SAY_SLAY_2                  -1556012
#define SAY_DEATH                   -1556013
#define EMOTE_ARCANE_EXP            -1556015

#define SPELL_BLINK                 38194
#define SPELL_BLINK_TELEPORT        38203
#define SPELL_MANA_SHIELD           38151
#define SPELL_ARCANE_BUBBLE         9438
#define H_SPELL_SLOW                35032

#define SPELL_POLYMORPH             38245
#define H_SPELL_POLYMORPH           43309

#define SPELL_ARCANE_VOLLEY         35059
#define H_SPELL_ARCANE_VOLLEY       40424

#define SPELL_ARCANE_EXPLOSION      38197
#define H_SPELL_ARCANE_EXPLOSION    40425

enum events
{
    EVENT_SPELL_BLINK = 1,
    EVENT_SPELL_POLYMORPH = 2,
    EVENT_SPELL_SLOW = 3,
    EVENT_SPELL_ARCANE_VOLLEY = 4,
    EVENT_SPELL_ARCANE_EXPLO = 5,
    EVENT_HEALTH_CHECK = 6,
    EVENT_SPELL_BLINK_2 = 7
};

struct boss_talon_king_ikissAI : public ScriptedAI
{
    boss_talon_king_ikissAI(Creature* c) : ScriptedAI(c)
    {
        IsHeroic = me->GetMap()->IsHeroic();
        pInstance =(ScriptedInstance*)c->GetInstanceData();
    }

    ScriptedInstance* pInstance;
    EventMap events;
    bool spoken;
    bool IsHeroic;


    void Reset()
    {
        spoken = false;

        if (pInstance)
            pInstance->SetData(DATA_IKISSEVENT, NOT_STARTED);
    }

    void MoveInLineOfSight(Unit* who)
    {
        if (!spoken && who->GetTypeId() == TYPEID_PLAYER && who->IsAlive())
        {
            DoScriptText(SAY_INTRO, me);
            spoken = true;

            ScriptedAI::MoveInLineOfSight(who);
        }
    }

    void EnterCombat(Unit*)
    {
        DoScriptText(RAND(SAY_AGGRO_1, SAY_AGGRO_2, SAY_AGGRO_3), me);

        if (pInstance)
            pInstance->SetData(DATA_IKISSEVENT, IN_PROGRESS);

        events.ScheduleEvent(EVENT_SPELL_BLINK, 35000);
        events.ScheduleEvent(EVENT_SPELL_ARCANE_VOLLEY, 5000);
        events.ScheduleEvent(EVENT_SPELL_POLYMORPH, 8000);
        events.ScheduleEvent(EVENT_HEALTH_CHECK, 2000);
        if (me->GetMap()->IsHeroic())
            events.ScheduleEvent(EVENT_SPELL_SLOW, urand(15000, 25000));
    }

    void JustDied(Unit*)
    {
        DoScriptText(SAY_DEATH, me);

        if (pInstance)
            pInstance->SetData(DATA_IKISSEVENT, DONE);
    }

    void KilledUnit(Unit*)
    {
        DoScriptText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);
        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;


        switch (events.ExecuteEvent())
        {
        case EVENT_SPELL_ARCANE_VOLLEY:
            me->CastSpell(me, IsHeroic ? H_SPELL_ARCANE_VOLLEY : SPELL_ARCANE_VOLLEY, false);
            events.ScheduleEvent(EVENT_SPELL_ARCANE_VOLLEY, urand(7000, 12000));
            break;
        case EVENT_SPELL_POLYMORPH:
            if (Unit* target = (IsHeroic? SelectTarget(SELECT_TARGET_RANDOM, 0) : SelectTarget(SELECT_TARGET_TOPAGGRO, 1)))
                me->CastSpell(target, IsHeroic? H_SPELL_POLYMORPH :SPELL_POLYMORPH, false);
            events.ScheduleEvent(EVENT_SPELL_POLYMORPH, urand(15000, 17500));
            break;
        case EVENT_SPELL_SLOW:
            me->CastSpell(me, H_SPELL_SLOW, false);
            events.ScheduleEvent(EVENT_SPELL_SLOW, urand(15000, 30000));
            break;
        case EVENT_HEALTH_CHECK:
            if (me->HealthBelowPct(20))
            {
                me->CastSpell(me, SPELL_MANA_SHIELD, false);
                events.ExecuteEvent();
                return;
            }
            events.Repeat(1000);
            break;
        case EVENT_SPELL_BLINK:
            DoScriptText(EMOTE_ARCANE_EXP, me);
            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
            {
                me->CastSpell(target, SPELL_BLINK, false);
                me->NearTeleportTo(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), target->GetOrientation());

                DoCast(target, SPELL_BLINK_TELEPORT);
            }
            events.Repeat(urand(35000, 40000));
            events.DelayEvents(500);
            events.ScheduleEvent(EVENT_SPELL_BLINK_2, 0);
            return;
        case EVENT_SPELL_BLINK_2:
            me->CastSpell(me, IsHeroic? H_SPELL_ARCANE_EXPLOSION : SPELL_ARCANE_EXPLOSION, false);
            me->CastSpell(me, SPELL_ARCANE_BUBBLE, true);
            events.ExecuteEvent();
            break;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_talon_king_ikiss(Creature* pCreature)
{
    return GetInstanceAI<boss_talon_king_ikissAI>(pCreature);
}

void AddSC_boss_talon_king_ikiss()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "boss_talon_king_ikiss";
    newscript->GetAI = &GetAI_boss_talon_king_ikiss;
    newscript->RegisterSelf();
}
