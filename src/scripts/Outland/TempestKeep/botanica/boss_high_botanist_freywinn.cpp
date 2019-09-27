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
SDName: Boss_High_Botanist_Freywinn
SD%Complete: 95
SDComment: some strange visual related to tree form(if aura lost before normal duration end).
SDCategory: Tempest Keep, The Botanica
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"

#define SAY_AGGRO                   -1553000
#define SAY_KILL_1                  -1553001
#define SAY_KILL_2                  -1553002
#define SAY_TREE_1                  -1553003
#define SAY_TREE_2                  -1553004
#define SAY_DEATH                   -1553005

#define SPELL_TRANQUILITY           34550
#define SPELL_TREE_FORM             34551

#define SPELL_SUMMON_FRAYER         34557
#define ENTRY_FRAYER                19953

#define SPELL_PLANT_WHITE           34759
#define SPELL_PLANT_GREEN           34761
#define SPELL_PLANT_BLUE            34762
#define SPELL_PLANT_RED             34763

enum Misc
{
    NPC_FRAYER = 19953,

    EVENT_SUMMON_SEEDLING = 1,
    EVENT_TREE_FORM = 2,
    EVENT_CHECK_FRAYERS = 3,
    EVENT_RESTORE_COMBAT = 4
};


struct boss_high_botanist_freywinnAI : public ScriptedAI
{
    boss_high_botanist_freywinnAI(Creature* c) : ScriptedAI(c), summons(me) {}

    SummonList summons;
    EventMap events;

    void Reset() {}

    void EnterCombat(Unit* /*who*/)
    {
        DoScriptText(SAY_AGGRO, me);
        events.ScheduleEvent(EVENT_SUMMON_SEEDLING, 6000);
        events.ScheduleEvent(EVENT_TREE_FORM, 30000);
    }

    void JustSummoned(Creature* summoned)
    {
        //  Forced Summon Frayers to Attack players
        if (me->GetVictim())
            summoned->AI()->AttackStart(me->GetVictim());

        summons.Summon(summoned);
    }

    void KilledUnit(Unit* victim)
    {
        if (victim->GetTypeId() == TYPEID_PLAYER)
            DoScriptText(RAND( SAY_KILL_1, SAY_KILL_2), me);
    }

    void JustDied(Unit* /*Killer*/)
    {
        DoScriptText(SAY_DEATH, me);
    }

    void SummonedCreatureDies(Creature* summon, Unit*)
    {
        summons.Despawn(summon);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        switch (events.ExecuteEvent())
        {
        case EVENT_SUMMON_SEEDLING:
            me->CastSpell(me, RAND(SPELL_PLANT_WHITE, SPELL_PLANT_GREEN, SPELL_PLANT_BLUE, SPELL_PLANT_RED), false);
            events.ScheduleEvent(EVENT_SUMMON_SEEDLING, 6000);
            break;
        case EVENT_TREE_FORM:
            events.Reset();
            events.SetPhase(1);
            events.ScheduleEvent(EVENT_CHECK_FRAYERS, 1000);
            events.ScheduleEvent(EVENT_TREE_FORM, 75000);
            events.ScheduleEvent(EVENT_RESTORE_COMBAT, 46000);

            DoScriptText(RAND(SAY_TREE_1, SAY_TREE_2), me);
            me->RemoveAllAuras();
            me->GetMotionMaster()->MoveIdle();
            me->GetMotionMaster()->Clear(false);

            for (int i = 0; i < 3; ++i)
                me->SummonCreature(ENTRY_FRAYER, me->GetPosition(), TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 60000) + i;

            me->CastSpell(me, SPELL_TRANQUILITY, true);
            me->CastSpell(me, SPELL_TREE_FORM, true);
            break;
        case EVENT_RESTORE_COMBAT:
            events.SetPhase(0);
            events.ScheduleEvent(EVENT_SUMMON_SEEDLING, 6000);
            me->GetMotionMaster()->MoveChase(me->GetVictim());
            break;

        case EVENT_CHECK_FRAYERS:
            if (!summons.HasEntry(NPC_FRAYER))
            {
                me->InterruptNonMeleeSpells(true);
                me->RemoveAllAuras();
                events.RescheduleEvent(EVENT_RESTORE_COMBAT, 0);
                events.RescheduleEvent(EVENT_TREE_FORM, 30000);
                break;
            }
            events.ScheduleEvent(EVENT_CHECK_FRAYERS, 500);
            break;
        }

        if (!events.IsInPhase(1))
            DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_high_botanist_freywinn(Creature* pCreature)
{
    return new boss_high_botanist_freywinnAI(pCreature);
}

void AddSC_boss_high_botanist_freywinn()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_high_botanist_freywinn";
    newscript->GetAI = &GetAI_boss_high_botanist_freywinn;
    newscript->RegisterSelf();
}

