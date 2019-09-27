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
SDName: Boss_Pandemonius
SD%Complete: 75
SDComment: Not known how void blast is done (amount of rapid cast seems to be related to players in party). All mobs remaining in surrounding area should aggro when engaged.
SDCategory: Auchindoun, Mana Tombs
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"

#define SAY_AGGRO_1                     -1557008
#define SAY_AGGRO_2                     -1557009
#define SAY_AGGRO_3                     -1557010

#define SAY_KILL_1                      -1557011
#define SAY_KILL_2                      -1557012

#define SAY_DEATH                       -1557013

#define EMOTE_DARK_SHELL                -1557014

#define SPELL_VOID_BLAST                32325
#define H_SPELL_VOID_BLAST              38760
#define SPELL_DARK_SHELL                32358
#define H_SPELL_DARK_SHELL              38759

enum events
{
    EVENT_VOIDBLAST     = 1,
    EVENT_DARKSHELL     = 2,
};

struct boss_pandemoniusAI : public ScriptedAI
{
    boss_pandemoniusAI(Creature* c) : ScriptedAI(c)
    {
        HeroicMode = me->GetMap()->IsHeroic();
    }

    bool HeroicMode;
    uint32 VoidBlast_Counter;
    EventMap events;

    void Reset()
    {
        VoidBlast_Counter = 0;
    }

    void JustDied(Unit*)
    {
        DoScriptText(SAY_DEATH, me);
    }

    void KilledUnit(Unit*)
    {
        DoScriptText(RAND(SAY_KILL_1, SAY_KILL_2), me);
    }

    void EnterCombat(Unit*)
    {
        DoScriptText(RAND(SAY_AGGRO_1, SAY_AGGRO_2, SAY_AGGRO_3), me);
        events.ScheduleEvent(EVENT_VOIDBLAST, 30000);
        events.ScheduleEvent(EVENT_DARKSHELL, 20000);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        events.Update(diff);

        Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0);

        switch (events.ExecuteEvent())
        {
        case EVENT_VOIDBLAST:
            DoCast(target, HeroicMode ? H_SPELL_VOID_BLAST : SPELL_VOID_BLAST);
            events.Repeat(25000 + rand() % 10000);
            break;
        case EVENT_DARKSHELL:
            DoCast(me, HeroicMode ? H_SPELL_DARK_SHELL : SPELL_DARK_SHELL);
            DoScriptText(EMOTE_DARK_SHELL, me);
            events.Repeat(20000);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_pandemonius(Creature* pCreature)
{
    return new boss_pandemoniusAI (pCreature);
}

void AddSC_boss_pandemonius()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "boss_pandemonius";
    newscript->GetAI = &GetAI_boss_pandemonius;
    newscript->RegisterSelf();
}

