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
SDName: Boss_Mandokir
SD%Complete: 90
SDComment: Ohgan function needs improvements.
SDCategory: Zul'Gurub
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "zulgurub.h"

enum Texts
{
    SAY_AGGRO           =   -1309015,
    SAY_DING_KILL       =   -1309016,
    SAY_GRATS_JINDO     =   -1309017,
    SAY_GAZE            =   -1309018,
    SAY_GAZE_WHISPER    =   -1309019
};

enum Spells
{
    SPELL_CHARGE          = 24408,
    SPELL_CLEAVE          = 7160,
    SPELL_FEAR            = 29321,
    SPELL_WHIRLWIND       = 15589,
    SPELL_MORTAL_STRIKE   = 16856,
    SPELL_ENRAGE          = 24318,
    SPELL_GAZE            = 24314,
    SPELL_LEVEL_UP        = 24312,
    SPELL_MOUNT           = 23243,

    // Ohgan's Spells
    SPELL_SUNDERARMOR     = 24317
};

enum Summons
{
    OHGAN                 = 14988
};

struct boss_mandokirAI : public ScriptedAI
{
    boss_mandokirAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }

    uint32 KillCount;
    uint32 Gaze_Timer;
    uint32 TargetInRange;
    uint32 Cleave_Timer;
    uint32 Whirlwind_Timer;
    uint32 Fear_Timer;
    uint32 MortalStrike_Timer;
    uint32 Check_Timer;
    float targetX;
    float targetY;
    float targetZ;

    ScriptedInstance* pInstance;

    bool endGaze;
    bool someGazed;
    bool RaptorDead;
    bool CombatStart;

    uint64 GazeTarget;

    void Reset()
    {
        KillCount = 0;
        Gaze_Timer = 33000;
        Cleave_Timer = 7000;
        Whirlwind_Timer = 20000;
        Fear_Timer = 1000;
        MortalStrike_Timer = 1000;
        Check_Timer = 1000;

        targetX = 0.0f;
        targetY = 0.0f;
        targetZ = 0.0f;
        TargetInRange = 0;

        GazeTarget = 0;

        someGazed = false;
        endGaze = false;
        RaptorDead = false;
        CombatStart = false;

        DoCast(me, SPELL_MOUNT);
    }

    void KilledUnit(Unit* victim)
    {
        if (victim->GetTypeId() == TYPEID_PLAYER)
        {
            ++KillCount;

            if (KillCount == 3)
            {
                DoScriptText(SAY_DING_KILL, me);

                if (pInstance)
                {
                    if (uint64 JindoGUID = pInstance->GetData64(DATA_JINDO))
                    {
                        if (Unit* jTemp = Unit::GetUnit(*me, JindoGUID))
                        {
                            if (jTemp->IsAlive())
                                DoScriptText(SAY_GRATS_JINDO, jTemp);
                        }
                    }
                }
                DoCast(me, SPELL_LEVEL_UP, true);
                KillCount = 0;
            }
        }
    }

    void EnterCombat(Unit* /*who*/)
    {
        DoScriptText(SAY_AGGRO, me);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (me->getVictim() && me->IsAlive())
        {
            if (!CombatStart)
            {
                // At combat Start Mandokir is mounted so we must unmount it first
                me->Dismount();

                // And summon his raptor
                me->SummonCreature(OHGAN, me->getVictim()->GetPositionX(), me->getVictim()->GetPositionY(), me->getVictim()->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                CombatStart = true;
            }

            if (Gaze_Timer <= diff)                         // Every 20 seconds Mandokir will check this
            {
                if (GazeTarget)
                {
                    Unit* pUnit = Unit::GetUnit(*me, GazeTarget);

                    if (pUnit && (
                            targetX != pUnit->GetPositionX() ||
                            targetY != pUnit->GetPositionY() ||
                            targetZ != pUnit->GetPositionZ() ||
                            pUnit->IsInCombat()))
                    {
                        if (me->IsWithinMeleeRange(pUnit))
                            DoCast(pUnit, 24316);
                        else
                        {
                            DoCast(pUnit, SPELL_CHARGE);
                            //me->SendMonsterMove(pUnit->GetPositionX(), pUnit->GetPositionY(), pUnit->GetPositionZ(), 0, true,1);
                            AttackStart(pUnit);
                        }
                    }
                }
                someGazed = false;
                Gaze_Timer = 20000;
            }
            else
                Gaze_Timer -= diff;

            if (Gaze_Timer < 8000 && !someGazed)            // 8 second(cast time + expire time) before the check for the gaze effect Mandokir will cast gaze debuff on a random target
            {
                if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                {
                    DoScriptText(SAY_GAZE, me, pTarget);
                    DoCast(pTarget, SPELL_GAZE);
                    me->MonsterWhisper(SAY_GAZE_WHISPER, pTarget->GetGUID());
                    GazeTarget = pTarget->GetGUID();
                    someGazed = true;
                    endGaze = true;
                }
            }

            if (Gaze_Timer < 1000 && endGaze)               // 1 second before the debuff expires, check whether the GazeTarget is in LoS
            {
                Unit* pUnit = Unit::GetUnit(*me, GazeTarget);
                if (pUnit)
                {
                    targetX = pUnit->GetPositionX();
                    targetY = pUnit->GetPositionY();
                    targetZ = pUnit->GetPositionZ();
                }
                endGaze = false;
            }

            if (!someGazed)
            {
                // Cleave
                if (Cleave_Timer <= diff)
                {
                    DoCastVictim( SPELL_CLEAVE);
                    Cleave_Timer = 7000;
                }
                else
                    Cleave_Timer -= diff;

                // Whirlwind
                if (Whirlwind_Timer <= diff)
                {
                    DoCast(me, SPELL_WHIRLWIND);
                    Whirlwind_Timer = 18000;
                }
                else
                    Whirlwind_Timer -= diff;

                // If more than 3 targets in melee range Mandokir will cast fear
                if (Fear_Timer <= diff)
                {
                    TargetInRange = 0;

                    std::list<HostileReference*>::const_iterator i = me->getThreatManager().getThreatList().begin();
                    for (; i != me->getThreatManager().getThreatList().end(); ++i)
                    {
                        Unit* pUnit = Unit::GetUnit(*me, (*i)->getUnitGuid());
                        if (pUnit && me->IsWithinMeleeRange(pUnit))
                            ++TargetInRange;
                    }

                    if (TargetInRange > 3)
                        DoCastVictim( SPELL_FEAR);

                    Fear_Timer = 4000;
                }
                else
                    Fear_Timer -= diff;

                // Mortal Strike if target is below 50% hp
                if (me->getVictim() && me->getVictim()->GetHealth() < me->getVictim()->GetMaxHealth() * 0.5f)
                {
                    if (MortalStrike_Timer <= diff)
                    {
                        DoCastVictim( SPELL_MORTAL_STRIKE);
                        MortalStrike_Timer = 15000;
                    }
                    else
                        MortalStrike_Timer -= diff;
                }
            }

            // Checking if Ohgan is dead. If yes Mandokir will enrage.
            if (Check_Timer <= diff)
            {
                if (pInstance)
                {
                    if (pInstance->GetData(TYPE_OHGAN) == DONE)
                    {
                        if (!RaptorDead)
                        {
                            DoCast(me, SPELL_ENRAGE);
                            RaptorDead = true;
                        }
                    }
                }

                Check_Timer = 1000;
            }
            else
                Check_Timer -= diff;

            DoMeleeAttackIfReady();
        }
    }
};

// Ohgan
struct mob_ohganAI : public ScriptedAI
{
    mob_ohganAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }

    uint32 SunderArmor_Timer;
    ScriptedInstance* pInstance;

    void Reset()
    {
        SunderArmor_Timer = 5000;
    }

    void EnterCombat(Unit* /*who*/) {}

    void JustDied(Unit* /*Killer*/)
    {
        if (pInstance)
            pInstance->SetData(TYPE_OHGAN, DONE);
    }

    void UpdateAI (const uint32 diff)
    {
        // Return since we have no target
        if (!UpdateVictim())
            return;

        // SunderArmor_Timer
        if (SunderArmor_Timer <= diff)
        {
            DoCastVictim( SPELL_SUNDERARMOR);
            SunderArmor_Timer = 10000 + rand() % 5000;
        }
        else
            SunderArmor_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_mandokir(Creature* pCreature)
{
    return new boss_mandokirAI (pCreature);
}

CreatureAI* GetAI_mob_ohgan(Creature* pCreature)
{
    return new mob_ohganAI (pCreature);
}

void AddSC_boss_mandokir()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_mandokir";
    newscript->GetAI = &GetAI_boss_mandokir;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_ohgan";
    newscript->GetAI = &GetAI_mob_ohgan;
    newscript->RegisterSelf();
}

