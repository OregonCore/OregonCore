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
SDName: Boss_Thaddius
SD%Complete: 0
SDComment: Merge Feugen & Stalagg with this script
SDCategory: Naxxramas
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"

// move it later to header file used by whole instance
enum eEncounters
{
    EVENT_FUEGEN   = 0,
    EVENT_STALAGG  = 1,
    EVENT_THADDIUS = 2
                     // rest later
};

//Stalagg
#define SAY_STAL_AGGRO          -1533023
#define SAY_STAL_SLAY           -1533024
#define SAY_STAL_DEATH          -1533025

//Feugen
#define SAY_FEUG_AGGRO          -1533026
#define SAY_FEUG_SLAY           -1533027
#define SAY_FEUG_DEATH          -1533028

//Thaddus
#define SAY_GREET               -1533029
#define SAY_AGGRO1              -1533030
#define SAY_AGGRO2              -1533031
#define SAY_AGGRO3              -1533032
#define SAY_SLAY                -1533033
#define SAY_ELECT               -1533034
#define SAY_DEATH               -1533035
#define SAY_SCREAM1             -1533036
#define SAY_SCREAM2             -1533037
#define SAY_SCREAM3             -1533038
#define SAY_SCREAM4             -1533039

#define GO_TESLA_COIL1    181477
#define GO_TESLA_COIL2    181478    //those 2 are not spawned

enum eSpells
{
    // Fuegen
    FEUG_TESLA_PASSIVE            = 28109,
    SPELL_MANA_BURN               = 28135,

    // Stalagg
    STAL_TESLA_PASSIVE            = 28097,
    SPELL_POWER_SURGE             = 28134,

    // shared
    SPELL_WAR_STOMP               = 28125,
    SPELL_MAGNETIC_PULL           = 28338, // scripteffect to script

    // Thaddius
    SPELL_SELF_STUN               = 28160,
    SPELL_BALL_LIGHTNING          = 28299,
    SPELL_POLARITY_SHIFT          = 28089, // dummy to script

    SPELL_CHAIN_LIGHTNING         = 28167,
    SPELL_BERSERK                 = 26662
};

enum eEvents
{
    // 1st phase shared
    EVENT_CHECK_COIL     = 1,
    EVENT_WAR_STOMP      = 2,
    EVENT_PULL_TANK      = 3,

    // Fuegen
    EVENT_MANA_BURN      = 4,

    // Stalagg
    EVENT_POWER_SURGE    = 5,

    // Thaddius
    EVENT_POLARITY_SHIFT = 6,
    EVENT_BERSERK        = 7
};

struct boss_thaddiusAI : public BossAI
{
    boss_thaddiusAI(Creature* c) : BossAI(c, EVENT_THADDIUS) {}

    void Reset()
    {
        //ClearCastQueue();

        events.ScheduleEvent(EVENT_POLARITY_SHIFT, 30000);
        events.ScheduleEvent(EVENT_BERSERK, 300000);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);
        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_POLARITY_SHIFT:
                {
                    //AddAOESpellToCast(SPELL_POLARITY_SHIFT);
                    events.ScheduleEvent(EVENT_POLARITY_SHIFT, 30000);
                    break;
                }
            case EVENT_BERSERK:
                {
                    //AddSpellToCast(me, SPELL_BERSERK);
                    break;
                }
            default:
                break;
            }
        }

        //CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

struct boss_stalaggAI : public BossAI
{
    boss_stalaggAI(Creature* c) : BossAI(c, EVENT_STALAGG) {}

    void Reset()
    {
        //ClearCastQueue();

        // proper timers
        events.ScheduleEvent(EVENT_CHECK_COIL, 2000);
        events.ScheduleEvent(EVENT_PULL_TANK, 20500);

        // guessed timers, to FIX
        events.ScheduleEvent(EVENT_POWER_SURGE, 10000);
        events.ScheduleEvent(EVENT_WAR_STOMP, 30000);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);
        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_CHECK_COIL:
                {
                    events.ScheduleEvent(EVENT_CHECK_COIL, 2000);
                    break;
                }
            case EVENT_POWER_SURGE:
                {
                    //AddSpellToCast(me, SPELL_POWER_SURGE);
                    events.ScheduleEvent(EVENT_POWER_SURGE, 20000); // guessed timer
                    break;
                }
            case EVENT_WAR_STOMP:
                {
                    //AddSpellToCast(me, SPELL_WAR_STOMP);
                    events.ScheduleEvent(EVENT_WAR_STOMP, 15000); // guessed timer
                    break;
                }
            case EVENT_PULL_TANK:
                {
                    //Unit* pFuegen = NULL;
                    // Select Fuegens as target, rest will be handled in scripteffect
                    //AddSpellToCast(pFuegen, SPELL_MAGNETIC_PULL);
                    events.ScheduleEvent(EVENT_PULL_TANK, 20500);
                    break;
                }
            default:
                break;
            }
        }

        //CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

struct boss_fuegenAI : public BossAI
{
    boss_fuegenAI(Creature* c): BossAI(c, EVENT_FUEGEN) {}

    void Reset()
    {
        //ClearCastQueue();

        // proper timers
        events.ScheduleEvent(EVENT_CHECK_COIL, 2000);
        events.ScheduleEvent(EVENT_PULL_TANK, 20500);

        // guessed timers, to FIX
        events.ScheduleEvent(EVENT_MANA_BURN, 10000);
        events.ScheduleEvent(EVENT_WAR_STOMP, 30000);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);
        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_CHECK_COIL:
                {
                    events.ScheduleEvent(EVENT_CHECK_COIL, 2000);
                    break;
                }
            case EVENT_MANA_BURN:
                {
                    //AddAOESpellToCast(SPELL_MANA_BURN);
                    events.ScheduleEvent(EVENT_MANA_BURN, 10000); // guessed timer
                    break;
                }
            case EVENT_WAR_STOMP:
                {
                    //AddAOESpellToCast(SPELL_WAR_STOMP);
                    events.ScheduleEvent(EVENT_WAR_STOMP, 15000); // guessed timer
                    break;
                }
            case EVENT_PULL_TANK:
                {
                    //Unit* pStalagg = NULL;
                    // Select Stalaggs as target, rest will be handled in scripteffect
                    //AddSpellToCast(pStalagg, SPELL_MAGNETIC_PULL);
                    events.ScheduleEvent(EVENT_PULL_TANK, 20500);
                    break;
                }
            default:
                break;
            }
        }

        //CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

/*
28089 just sample, move it to scripteffec or effectdummy later ;]

    if (unitTarget->GetTypeId() == TYPEID_PLAYER)
    {
        // those spells have 1 min duration so they need be removed to prevent stack of both types
        unitTarget->RemoveAurasDueToSpell(28059);
        unitTarget->RemoveAurasDueToSpell(28084);

        // Apply positive or negative charge
        m_caster->CastSpell(unitTarget, RAND(28059, 28084), true);
    }
*/
