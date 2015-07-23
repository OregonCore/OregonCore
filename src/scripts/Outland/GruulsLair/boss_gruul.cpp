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
SDName: Boss_Gruul
SD%Complete: 25
SDComment: Ground Slam seriously messed up due to core problem
SDCategory: Gruul's Lair
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "gruuls_lair.h"

/* Yells & Quotes */
#define SAY_AGGRO                   -1565010
#define SAY_SLAM1                   -1565011
#define SAY_SLAM2                   -1565012
#define SAY_SHATTER1                -1565013
#define SAY_SHATTER2                -1565014
#define SAY_SLAY1                   -1565015
#define SAY_SLAY2                   -1565016
#define SAY_SLAY3                   -1565017
#define SAY_DEATH                   -1565018
#define EMOTE_GROW                  -1565019

/* Spells */
#define SPELL_GROWTH              36300
#define SPELL_CAVE_IN             36240
#define SPELL_GROUND_SLAM         33525                     // AoE Ground Slam applying Ground Slam to everyone with a script effect (most likely the knock back, we can code it to a set knockback)
#define SPELL_REVERBERATION       36297                     //AoE Silence
#define SPELL_SHATTER             33654
#define SPELL_MAGNETIC_PULL       28337
#define SPELL_KNOCK_BACK          24199                     //Knockback spell until correct implementation is made
#define SPELL_SHATTER_EFFECT        33671
#define SPELL_HURTFUL_STRIKE        33813
#define SPELL_STONED                33652                   //Spell is self cast
#define SPELL_GRONN_LORDS_GRASP     33572                   //Triggered by Ground Slam

struct boss_gruulAI : public ScriptedAI
{
    boss_gruulAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }

    ScriptedInstance* pInstance;

    uint32 Growth_Timer;
    uint32 CaveIn_Timer;
    uint32 GroundSlamTimer;
    uint32 GroundSlamStage;
    uint32 PerformingGroundSlam;
    uint32 HurtfulStrike_Timer;
    uint32 Reverberation_Timer;

    void Reset()
    {
        Growth_Timer = 30000;
        CaveIn_Timer = 40000;
        GroundSlamTimer = 35000;
        GroundSlamStage = 0;
        PerformingGroundSlam = false;
        HurtfulStrike_Timer = 8000;
        Reverberation_Timer = 60000 + 45000;

        if (pInstance)
            pInstance->SetData(DATA_GRUULEVENT, NOT_STARTED);
    }

    void JustDied(Unit* /*Killer*/)
    {
        DoScriptText(SAY_DEATH, me);

        if (pInstance)
            pInstance->SetData(DATA_GRUULEVENT, DONE);
    }

    void EnterCombat(Unit* /*who*/)
    {
        DoScriptText(SAY_AGGRO, me);
        DoZoneInCombat();

        if (pInstance)
            pInstance->SetData(DATA_GRUULEVENT, IN_PROGRESS);
    }

    void KilledUnit()
    {
        switch (rand() % 3)
        {
        case 0:
            DoScriptText(SAY_SLAY1, me);
            break;
        case 1:
            DoScriptText(SAY_SLAY2, me);
            break;
        case 2:
            DoScriptText(SAY_SLAY3, me);
            break;
        }
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!UpdateVictim())
            return;

        // Growth
        // Gruul can cast this spell up to 30 times
        if (Growth_Timer <= diff)
        {
            DoCast(me, SPELL_GROWTH);
            DoScriptText(EMOTE_GROW, me);
            Growth_Timer = 30000;
        }
        else Growth_Timer -= diff;

        if (PerformingGroundSlam)
        {
            if (GroundSlamTimer <= diff)
            {
                switch (GroundSlamStage)
                {
                case 0:
                    {
                        //Begin the whole ordeal
                        std::list<HostileReference*>& m_threatlist = me->getThreatManager().getThreatList();

                        std::vector<Unit*> knockback_targets;

                        //First limit the list to only players
                        for (std::list<HostileReference*>::iterator itr = m_threatlist.begin(); itr != m_threatlist.end(); ++itr)
                        {
                            Unit* pTarget = Unit::GetUnit(*me, (*itr)->getUnitGuid());

                            if (pTarget && pTarget->GetTypeId() == TYPEID_PLAYER)
                                knockback_targets.push_back(pTarget);
                        }

                        //Now to totally disoriend those players
                        for (std::vector<Unit*>::iterator itr = knockback_targets.begin(); itr != knockback_targets.end(); ++itr)
                        {
                            Unit* target = *itr;
                            Unit* target2 = *(knockback_targets.begin() + rand() % knockback_targets.size());

                            if (target && target2)
                            {
                                switch (rand() % 2)
                                {
                                case 0:
                                    target2->CastSpell(target, SPELL_MAGNETIC_PULL, true, NULL, NULL, me->GetGUID());
                                    break;
                                case 1:
                                    target2->CastSpell(target, SPELL_KNOCK_BACK, true, NULL, NULL, me->GetGUID());
                                    break;
                                }
                            }
                        }

                        GroundSlamTimer = 7000;
                        break;
                    }

                case 1:
                    {
                        //Players are going to get stoned
                        std::list<HostileReference*>& m_threatlist = me->getThreatManager().getThreatList();

                        for (std::list<HostileReference*>::iterator itr = m_threatlist.begin(); itr != m_threatlist.end(); ++itr)
                        {
                            Unit* pTarget = Unit::GetUnit(*me, (*itr)->getUnitGuid());

                            if (pTarget)
                            {
                                pTarget->RemoveAurasDueToSpell(SPELL_GRONN_LORDS_GRASP);
                                pTarget->CastSpell(pTarget, SPELL_STONED, true, NULL, NULL, me->GetGUID());
                            }
                        }

                        GroundSlamTimer = 5000;

                        break;
                    }

                case 2:
                    {
                        DoCast(me, SPELL_SHATTER);
                        GroundSlamTimer = 1000;
                        break;
                    }

                case 3:
                    {
                        //Shatter takes effect
                        // Not Needet Anymore Handled in Spell SPELL_SHATTER
                        //std::list<HostileReference*>& m_threatlist = me->getThreatManager().getThreatList();
                        //for (std::list<HostileReference*>::iterator itr = m_threatlist.begin(); itr != m_threatlist.end(); ++itr)
                        //{
                        //    Unit* target = Unit::GetUnit(*me, (*itr)->getUnitGuid());
                        //    if (target)
                        //    {
                        //        target->RemoveAurasDueToSpell(SPELL_STONED);
                        //        if (target->GetTypeId() == TYPEID_PLAYER)
                        //            target->CastSpell(target, SPELL_SHATTER_EFFECT, false, NULL, NULL, me->GetGUID());
                        //    }
                        //}

                        me->GetMotionMaster()->Clear();

                        Unit* victim = me->getVictim();
                        if (victim)
                        {
                            me->GetMotionMaster()->MoveChase(victim);
                            me->SetUInt64Value(UNIT_FIELD_TARGET, victim->GetGUID());
                        }

                        PerformingGroundSlam = false;

                        GroundSlamTimer = 120000;
                        HurtfulStrike_Timer = 8000;
                        if (Reverberation_Timer < 10000)     //Give a little time to the players to undo the damage from shatter
                            Reverberation_Timer += 10000;

                        break;
                    }
                }

                GroundSlamStage++;
            }
            else
                GroundSlamTimer -= diff;
        }
        else
        {
            // Hurtful Strike
            if (HurtfulStrike_Timer <= diff)
            {
                Unit* pTarget = NULL;
                pTarget = SelectUnit(SELECT_TARGET_TOPAGGRO, 1);

                if (pTarget && me->IsWithinMeleeRange(me->getVictim()))
                    DoCast(pTarget, SPELL_HURTFUL_STRIKE);
                else
                    DoCastVictim(SPELL_HURTFUL_STRIKE);

                HurtfulStrike_Timer = 8000;
            }
            else HurtfulStrike_Timer -= diff;

            // Reverberation
            if (Reverberation_Timer <= diff)
            {
                DoCastVictim( SPELL_REVERBERATION, true);
                Reverberation_Timer = 30000;
            }
            else Reverberation_Timer -= diff;

            // Cave In
            if (CaveIn_Timer <= diff)
            {
                if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                    DoCast(pTarget, SPELL_CAVE_IN);

                CaveIn_Timer = 20000;
            }
            else CaveIn_Timer -= diff;

            // Ground Slam, Gronn Lord's Grasp, Stoned, Shatter
            if (GroundSlamTimer <= diff)
            {
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveIdle();
                me->SetUInt64Value(UNIT_FIELD_TARGET, 0);

                PerformingGroundSlam = true;
                GroundSlamTimer = 0;
                GroundSlamStage = 0;
                DoCastVictim( SPELL_GROUND_SLAM);
            }
            else GroundSlamTimer -= diff;

            DoMeleeAttackIfReady();
        }
    }
};

CreatureAI* GetAI_boss_gruul(Creature* pCreature)
{
    return new boss_gruulAI (pCreature);
}

void AddSC_boss_gruul()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "boss_gruul";
    newscript->GetAI = &GetAI_boss_gruul;
    newscript->RegisterSelf();
}

