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
SDName: Boss_Onyxia
SD%Complete: 90
SDComment: Spell Heated Ground is wrong, flying animation, visual for area effect
SDCategory: Onyxia's Lair
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"

#define SAY_AGGRO                   -1249000
#define SAY_KILL                    -1249001
#define SAY_PHASE_2_TRANS           -1249002
#define SAY_PHASE_3_TRANS           -1249003
#define EMOTE_BREATH                -1249004

#define SPELL_WINGBUFFET            18500
#define SPELL_FLAMEBREATH           18435
#define SPELL_CLEAVE                19983
#define SPELL_TAILSWEEP             15847
#define SPELL_KNOCK_AWAY            19633

#define SPELL_ENGULFINGFLAMES       20019
#define SPELL_DEEPBREATH            23461
#define SPELL_FIREBALL              18392

#define SPELL_BELLOWINGROAR         18431
#define SPELL_HEATED_GROUND         22191       //Wrong Spell

#define SPELL_SUMMONWHELP           17646

#define CREATURE_WHELP      11262

static float MovementLocations[4][3] =
{
    { -64.0523f, -213.0619f, -68.2985f},
    {12.4636f, -220.01490f, -68.0548f},
    { -38.8391f, -182.3220f, -68.9457f},
    { -37.0390f, -244.8760f, -68.1278f}
};

static float SpawnLocations[4][3] =
{
    { -30.127f, -254.463f, -89.440f},
    { -30.817f, -177.106f, -89.258f},
    {14.480f, -241.560f, -85.6300f},
    {17.372f, -190.840f, -85.2810f},
};

struct boss_onyxiaAI : public ScriptedAI
{
    boss_onyxiaAI(Creature* c) : ScriptedAI(c) {}

    uint32 Phase;

    uint32 FlameBreathTimer;
    uint32 CleaveTimer;
    uint32 TailSweepTimer;
    uint32 MovementTimer;
    uint32 EngulfingFlamesTimer;
    uint32 SummonWhelpsTimer;
    uint32 BellowingRoarTimer;
    uint32 WingBuffetTimer;
    uint32 KnockAwayTimer;
    uint32 FireballTimer;

    bool InitialSpawn;

    void Reset()
    {
        Phase = 1;

        FlameBreathTimer = 20000;
        TailSweepTimer = 2000;
        CleaveTimer = 15000;
        MovementTimer = 3000;
        EngulfingFlamesTimer = 15000;
        SummonWhelpsTimer = 45000;
        BellowingRoarTimer = 30000;
        WingBuffetTimer = 17000;
        KnockAwayTimer = 15000;
        FireballTimer = 18000;

        InitialSpawn = true;
    }

    void EnterCombat(Unit* /*who*/)
    {
        DoScriptText(SAY_AGGRO, me);
        DoZoneInCombat();
    }

    void JustDied(Unit* /*Killer*/)
    {
    }

    void KilledUnit(Unit* /*victim*/)
    {
        DoScriptText(SAY_KILL, me);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (HealthBelowPct(60) && (Phase == 1))
        {
            Phase = 2;
            me->HandleEmoteCommand(EMOTE_ONESHOT_LIFTOFF);
            me->SetLevitate(true);
            me->SetHover(true);
            me->GetMotionMaster()->Clear(false);
            me->GetMotionMaster()->MoveIdle();
            DoScriptText(SAY_PHASE_2_TRANS, me);
        }

        if (HealthBelowPct(40) && (Phase == 2))
        {
            Phase = 3;
            me->SetLevitate(false);
            me->HandleEmoteCommand(EMOTE_ONESHOT_LAND);
            me->SetHover(false);
            me->GetMotionMaster()->MovePoint(0, -10.6155f, -219.357f, -87.7344f);
            DoStartMovement(me->GetVictim());
            me->SetWalk(false);
            DoScriptText(SAY_PHASE_3_TRANS, me);
        }

        if (Phase == 1 || Phase == 3)
        {
            if (FlameBreathTimer <= diff)
            {
                DoCastVictim( SPELL_FLAMEBREATH);
                FlameBreathTimer = 15000;
            }
            else FlameBreathTimer -= diff;

            if (TailSweepTimer <= diff)
            {
                Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 1);
                if (pTarget && !me->HasInArc(float(M_PI), pTarget))
                    DoCast(pTarget, SPELL_TAILSWEEP);

                TailSweepTimer = 10000;
            }
            else TailSweepTimer -= diff;

            if (CleaveTimer <= diff)
            {
                DoCastVictim( SPELL_CLEAVE);
                CleaveTimer = 10000;
            }
            else CleaveTimer -= diff;

            if (WingBuffetTimer <= diff)
            {
                DoCastVictim( SPELL_WINGBUFFET);
                WingBuffetTimer = 7000 + ((rand() % 8) * 1000);
            }
            else WingBuffetTimer -= diff;

            if (KnockAwayTimer <= diff)
            {
                if (rand() <= 30)
                    DoCastVictim( SPELL_KNOCK_AWAY);
                KnockAwayTimer = 15000;
            }
            else KnockAwayTimer -= diff;

            if (Phase == 3)
            {
                if (BellowingRoarTimer <= diff)
                {
                    DoCastVictim( SPELL_BELLOWINGROAR);

                    BellowingRoarTimer = 30000;
                }
                else BellowingRoarTimer -= diff;

                if (SummonWhelpsTimer <= diff)
                {
                    SummonWhelps(Phase);

                    SummonWhelpsTimer = 45000;
                }
                else SummonWhelpsTimer -= diff;
            }

            DoMeleeAttackIfReady();
        }

        if (Phase == 2)
        {
            if (InitialSpawn)
            {
                InitialSpawn = false;

                for (uint32 i = 0; i < 10; ++i)
                {
                    uint32 random = rand() % 4;
                    Creature* Whelp = me->SummonCreature(CREATURE_WHELP, SpawnLocations[random][0], SpawnLocations[random][1], SpawnLocations[random][2], 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 30000);
                    if (Whelp)
                        Whelp->AI()->AttackStart(SelectUnit(SELECT_TARGET_RANDOM, 0));
                }
            }

            if (EngulfingFlamesTimer <= diff)
            {
                DoCast(SelectUnit(SELECT_TARGET_RANDOM, 0), SPELL_ENGULFINGFLAMES);
                me->HandleEmoteCommand(ANIM_FLY);

                EngulfingFlamesTimer = 10000;
            }
            else EngulfingFlamesTimer -= diff;

            if (FireballTimer <= diff)
            {
                DoCast(SelectUnit(SELECT_TARGET_RANDOM, 0), SPELL_FIREBALL);

                FireballTimer = 18000;
            }
            else FireballTimer -= diff;

            if (MovementTimer <= diff)
            {
                if (rand() % 100 < 30)
                {
                    DoScriptText(EMOTE_BREATH, me);
                    DoCastVictim( SPELL_DEEPBREATH);
                }
                else ChangePosition();

                MovementTimer = 25000;
            }
            else MovementTimer -= diff;

            if (SummonWhelpsTimer <= diff)
            {
                SummonWhelps(Phase);

                SummonWhelpsTimer = 45000;
            }
            else SummonWhelpsTimer -= diff;
        }
    }

    void ChangePosition()
    {
        uint32 random = rand() % 4;
        if (random < 4)
            me->GetMotionMaster()->MovePoint(0, MovementLocations[random][0], MovementLocations[random][1], MovementLocations[random][2]);
    }

    void SummonWhelps(uint32 Phase)
    {
        if (Phase == 2)
        {
            uint32 max = rand() % 10;
            for (uint32 i = 0; i < max; ++i)
            {
                uint32 random = rand() % 3;
                Creature* Whelp = me->SummonCreature(CREATURE_WHELP, SpawnLocations[random][0], SpawnLocations[random][1], SpawnLocations[random][2], 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 30000);
                if (Whelp)
                    Whelp->AI()->AttackStart(SelectUnit(SELECT_TARGET_RANDOM, 0));
            }
        }

        if (Phase == 3)
        {
            uint32 max = rand() % 10 + 1;
            if (max < 5)
            {
                for (uint32 i = 0; i < max; ++i)
                {
                    uint32 random = rand() % 4;
                    Creature* Whelp = me->SummonCreature(CREATURE_WHELP, SpawnLocations[random][0], SpawnLocations[random][1], SpawnLocations[random][2], 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 30000);
                    if (Whelp)
                        Whelp->AI()->AttackStart(SelectUnit(SELECT_TARGET_RANDOM, 0));
                }
            }
        }
    }
};

CreatureAI* GetAI_boss_onyxiaAI(Creature* pCreature)
{
    return new boss_onyxiaAI (pCreature);
}

void AddSC_boss_onyxia()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "boss_onyxia";
    newscript->GetAI = &GetAI_boss_onyxiaAI;
    newscript->RegisterSelf();
}

