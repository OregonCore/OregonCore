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
SDName: Boss_Maiden_of_Virtue
SD%Complete: 100
SDComment:
SDCategory: Karazhan
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "karazhan.h"

#define SAY_AGGRO               -1532018
#define SAY_SLAY1               -1532019
#define SAY_SLAY2               -1532020
#define SAY_SLAY3               -1532021
#define SAY_REPENTANCE1         -1532022
#define SAY_REPENTANCE2         -1532023
#define SAY_DEATH               -1532024

enum eSpells 
{
    SPELL_REPENTANCE    = 29511,
    SPELL_HOLYFIRE      = 29522,
    SPELL_HOLYWRATH     = 32445,
    SPELL_HOLYGROUND    = 29512,
    SPELL_BERSERK       = 26662
};

enum eEvents
{
    EVENT_REPENTANCE    = 1,
    EVENT_HOLYFIRE      = 2,
    EVENT_HOLYWRATH     = 3,
    EVENT_HOLYGROUND    = 4,
    EVENT_ENRAGE        = 5
};

struct boss_maiden_of_virtueAI : public BossAI
{
    boss_maiden_of_virtueAI(Creature* c) : BossAI(c, TYPE_MAIDEN) { }

    void Reset()
    {
        _Reset();
    }

    void KilledUnit(Unit* /*Victim*/)
    {
        if (urand(0, 1) == 0)
            DoScriptText(RAND(SAY_SLAY1, SAY_SLAY2, SAY_SLAY3), me);
    }

    void JustDied(Unit* /*Killer*/)
    {
        DoScriptText(SAY_DEATH, me);
        _JustDied();
    }

    void EnterCombat(Unit* /*who*/)
    {
        _EnterCombat();
        DoScriptText(SAY_AGGRO, me);

        events.ScheduleEvent(EVENT_REPENTANCE, urand(33, 45) * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_HOLYFIRE, 12 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_HOLYWRATH, urand(15, 25) * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_HOLYGROUND, 3 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_ENRAGE, 600 * IN_MILLISECONDS);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
        return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_REPENTANCE:
                    DoCastVictim(SPELL_REPENTANCE);
                    DoScriptText(RAND(SAY_REPENTANCE1, SAY_REPENTANCE2), me);
                    events.ScheduleEvent(EVENT_REPENTANCE, urand(33, 45) * IN_MILLISECONDS);
                    break;
                case EVENT_HOLYFIRE:
                    if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 50, true))
                        DoCast(pTarget, SPELL_HOLYFIRE);
                    events.ScheduleEvent(EVENT_HOLYFIRE, 12 * IN_MILLISECONDS);
                    break;
                case EVENT_HOLYWRATH:
                    if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 80, true))
                        DoCast(pTarget, SPELL_HOLYWRATH);
                    events.ScheduleEvent(EVENT_HOLYWRATH, urand(15, 25) * IN_MILLISECONDS);
                    break;
                case EVENT_HOLYGROUND:
                    DoCast(me, SPELL_HOLYGROUND, true);
                    events.ScheduleEvent(EVENT_HOLYGROUND, 3 * IN_MILLISECONDS);
                    break;
                case EVENT_ENRAGE:
                    DoCast(me, SPELL_BERSERK, true);
                    break;
                default:
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }

};

CreatureAI* GetAI_boss_maiden_of_virtue(Creature* pCreature)
{
    return new boss_maiden_of_virtueAI (pCreature);
}

void AddSC_boss_maiden_of_virtue()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "boss_maiden_of_virtue";
    newscript->GetAI = &GetAI_boss_maiden_of_virtue;
    newscript->RegisterSelf();
}

