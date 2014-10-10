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
SDName: Boss_Sapphiron
SD%Complete: 0
SDComment: Place Holder
SDCategory: Naxxramas
EndScriptData */

#include "ScriptPCH.h"

#define EMOTE_BREATH            -1533082
#define EMOTE_ENRAGE            -1533083

#define SPELL_ICEBOLT           28522
#define SPELL_FROST_BREATH      29318
#define SPELL_FROST_AURA        28531
#define SPELL_LIFE_DRAIN        28542
#define SPELL_BLIZZARD          28547
#define SPELL_BESERK            26662

struct boss_sapphironAI : public ScriptedAI
{
    boss_sapphironAI(Creature* c) : ScriptedAI(c) {}

    uint32 Icebolt_Count;
    uint32 Icebolt_Timer;
    uint32 FrostBreath_Timer;
    uint32 FrostAura_Timer;
    uint32 LifeDrain_Timer;
    uint32 Blizzard_Timer;
    uint32 Fly_Timer;
    uint32 Fly2_Timer;
    uint32 Beserk_Timer;
    uint32 phase;
    bool IsInFly;
    uint32 land_Timer;

    void Reset()
    {
        FrostAura_Timer = 2000;
        LifeDrain_Timer = 24000;
        Blizzard_Timer = 20000;
        Fly_Timer = 45000;
        Icebolt_Timer = 4000;
        land_Timer = 0;
        Beserk_Timer = 0;
        phase = 1;
        Icebolt_Count = 0;
        IsInFly = false;

        me->RemoveUnitMovementFlag(MOVEFLAG_LEVITATING | MOVEFLAG_ONTRANSPORT);
    }

    void EnterCombat(Unit* /*who*/)
    {
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (phase == 1)
        {
            if (FrostAura_Timer <= diff)
            {
                DoCastVictim(SPELL_FROST_AURA);
                FrostAura_Timer = 5000;
            }
            else FrostAura_Timer -= diff;

            if (LifeDrain_Timer <= diff)
            {
                if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                    DoCast(pTarget, SPELL_LIFE_DRAIN);
                LifeDrain_Timer = 24000;
            }
            else LifeDrain_Timer -= diff;

            if (Blizzard_Timer <= diff)
            {
                if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                    DoCast(pTarget, SPELL_BLIZZARD);
                Blizzard_Timer = 20000;
            }
            else Blizzard_Timer -= diff;

            if (me->GetHealth() * 100 / me->GetMaxHealth() > 10)
            {
                if (Fly_Timer <= diff)
                {
                    phase = 2;
                    me->HandleEmoteCommand(EMOTE_ONESHOT_LIFTOFF);
                    me->AddUnitMovementFlag(MOVEFLAG_LEVITATING | MOVEFLAG_ONTRANSPORT);
                    me->GetMotionMaster()->Clear(false);
                    me->GetMotionMaster()->MoveIdle();
                    me->SetHover(true);
                    Icebolt_Timer = 4000;
                    Icebolt_Count = 0;
                    IsInFly = true;
                }
                else Fly_Timer -= diff;
            }
        }

        if (phase == 2)
        {
            if (Icebolt_Timer <= diff && Icebolt_Count < 5)
            {
                if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                {
                    DoCast(pTarget, SPELL_ICEBOLT);
                    ++Icebolt_Count;
                    error_log("Count incremented");
                }
                FrostBreath_Timer = 6000;
                Icebolt_Timer = 4000;
            }
            else Icebolt_Timer -= diff;

            if (Icebolt_Count == 5 && IsInFly && FrostBreath_Timer <= diff)
            {
                DoScriptText(EMOTE_BREATH, me);
                DoCastVictim(SPELL_FROST_BREATH);
                land_Timer = 2000;
                IsInFly = false;
                FrostBreath_Timer = 6000;
            }
            else FrostBreath_Timer -= diff;

            if (!IsInFly && land_Timer <= diff)
            {
                phase = 1;
                me->HandleEmoteCommand(EMOTE_ONESHOT_LAND);
                me->RemoveUnitMovementFlag(MOVEFLAG_LEVITATING | MOVEFLAG_ONTRANSPORT);
                me->GetMotionMaster()->Clear(false);
                me->GetMotionMaster()->MoveChase(me->getVictim());
                me->SetHover(true);
                land_Timer = 0;
                Fly_Timer = 67000;
            }
            else land_Timer -= diff;
        }

        if ((me->GetHealth() * 100) / me->GetMaxHealth() <= 10)
        {
            if (Beserk_Timer <= diff)
            {
                DoScriptText(EMOTE_ENRAGE, me);
                DoCast(me, SPELL_BESERK);
                Beserk_Timer = 300000;
            }
            else Beserk_Timer -= diff;
        }

        if (phase != 2)
            DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_sapphiron(Creature* pCreature)
{
    return new boss_sapphironAI (pCreature);
}

void AddSC_boss_sapphiron()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "boss_sapphiron";
    newscript->GetAI = &GetAI_boss_sapphiron;
    newscript->RegisterSelf();
}

