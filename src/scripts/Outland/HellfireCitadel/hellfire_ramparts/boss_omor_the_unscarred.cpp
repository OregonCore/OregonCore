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
SDName: Boss_Omar_The_Unscarred
SD%Complete: 99
SDComment:
SDCategory: Hellfire Citadel, Hellfire Ramparts
EndScriptData */

#include "ScriptPCH.h"
#include "hellfire_ramparts.h"

#define SAY_AGGRO_1                 -1543009
#define SAY_AGGRO_2                 -1543010
#define SAY_AGGRO_3                 -1543011
#define SAY_SUMMON                  -1543012
#define SAY_CURSE                   -1543013
#define SAY_KILL_1                  -1543014
#define SAY_DIE                     -1543015
#define SAY_WIPE                    -1543016

#define SPELL_ORBITAL_STRIKE        30637
#define SPELL_SHADOW_WHIP           30638
#define SPELL_TREACHEROUS_AURA      30695
#define H_SPELL_BANE_OF_TREACHERY   37566
#define SPELL_DEMONIC_SHIELD        31901
#define SPELL_SHADOW_BOLT           30686
#define H_SPELL_SHADOW_BOLT         39297
#define SPELL_SUMMON_FIENDISH_HOUND 30707

struct boss_omor_the_unscarredAI : public Scripted_NoMovementAI
{
    boss_omor_the_unscarredAI(Creature* c) : Scripted_NoMovementAI(c)
    {
        pInstance = c->GetInstanceData();
        HeroicMode = me->GetMap()->IsHeroic();
    }

    ScriptedInstance* pInstance;
    bool HeroicMode;

    uint32 OrbitalStrike_Timer;
    uint32 ShadowWhip_Timer;
    uint32 Aura_Timer;
    uint32 DemonicShield_Timer;
    uint32 Shadowbolt_Timer;
    uint32 Summon_Timer;
    uint64 playerGUID;
    bool CanPullBack;

    void Reset()
    {
        DoScriptText(SAY_WIPE, me);

        OrbitalStrike_Timer = 25000;
        ShadowWhip_Timer = 2000;
        Aura_Timer = 12300;
        DemonicShield_Timer = 1000;
        Shadowbolt_Timer = 6600;
        Summon_Timer = 19600;
        playerGUID = 0;
        CanPullBack = false;

        if (pInstance)
            pInstance->SetData(DATA_OMOR, NOT_STARTED);
    }

    void EnterCombat(Unit* /*who*/)
    {
        switch (rand() % 3)
        {
        case 0:
            DoScriptText(SAY_AGGRO_1, me);
            break;
        case 1:
            DoScriptText(SAY_AGGRO_2, me);
            break;
        case 2:
            DoScriptText(SAY_AGGRO_3, me);
            break;
        }

        if (pInstance)
            pInstance->SetData(DATA_OMOR, IN_PROGRESS);
    }

    void KilledUnit(Unit* /*victim*/)
    {
        if (rand() % 2)
            return;

        DoScriptText(SAY_KILL_1, me);
    }

    void JustSummoned(Creature* summoned)
    {
        DoScriptText(SAY_SUMMON, me);

        if (Unit* random = SelectUnit(SELECT_TARGET_RANDOM, 0))
            summoned->AI()->AttackStart(random);
    }

    void JustDied(Unit* /*Killer*/)
    {
        DoScriptText(SAY_DIE, me);

        if (pInstance)
            pInstance->SetData(DATA_OMOR, DONE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (Summon_Timer <= diff)
        {
            me->InterruptNonMeleeSpells(false);
            DoCast(me, SPELL_SUMMON_FIENDISH_HOUND);
            Summon_Timer = 24100 + rand() % 2800;
        }
        else Summon_Timer -= diff;

        if (CanPullBack)
        {
            if (ShadowWhip_Timer <= diff)
            {
                if (Unit* temp = Unit::GetUnit(*me, playerGUID))
                {
                    //if unit dosen't have this flag, then no pulling back (script will attempt cast, even if orbital strike was resisted)
                    if (temp->HasUnitMovementFlag(MOVEFLAG_FALLING))
                    {
                        me->InterruptNonMeleeSpells(false);
                        DoCast(temp, SPELL_SHADOW_WHIP);
                    }
                    else if (!temp->HasUnitMovementFlag(MOVEFLAG_FALLING))
                    {
                        playerGUID = 0;
                        CanPullBack = false;
                    }
                }
                ShadowWhip_Timer = 2000;
            }
            else ShadowWhip_Timer -= diff;

        }
        else if (OrbitalStrike_Timer <= diff)
        {
            Unit* temp = NULL;
            if (me->IsWithinMeleeRange(me->getVictim()))
                temp = me->getVictim();
            else temp = SelectUnit(SELECT_TARGET_RANDOM, 0);

            if (temp && temp->GetTypeId() == TYPEID_PLAYER)
            {
                me->InterruptNonMeleeSpells(false);
                DoCast(temp, SPELL_ORBITAL_STRIKE);
                OrbitalStrike_Timer = 14000 + rand() % 2000;
                playerGUID = temp->GetGUID();

                if (playerGUID)
                {
                    CanPullBack = true;
                    ShadowWhip_Timer = 2500;
                }
            }
        }
        else OrbitalStrike_Timer -= diff;

        if ((me->GetHealth() * 100) / me->GetMaxHealth() < 20)
        {
            if (DemonicShield_Timer <= diff)
            {
                DoCast(me, SPELL_DEMONIC_SHIELD);
                DemonicShield_Timer = 15000;
            }
            else DemonicShield_Timer -= diff;
        }

        if (Aura_Timer <= diff)
        {
            DoScriptText(SAY_CURSE, me);

            if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
            {
                DoCast(pTarget, HeroicMode ? H_SPELL_BANE_OF_TREACHERY : SPELL_TREACHEROUS_AURA);
                Aura_Timer = 8000 + rand() % 8000;
            }
        }
        else Aura_Timer -= diff;

        if (Shadowbolt_Timer <= diff)
        {
            if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
            {
                if (pTarget)
                    pTarget = me->getVictim();

                DoCast(pTarget, HeroicMode ? H_SPELL_SHADOW_BOLT : SPELL_SHADOW_BOLT);
                Shadowbolt_Timer = 4000 + rand() % 3100;
            }
        }
        else Shadowbolt_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_omor_the_unscarredAI(Creature* pCreature)
{
    return new boss_omor_the_unscarredAI (pCreature);
}

void AddSC_boss_omor_the_unscarred()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_omor_the_unscarred";
    newscript->GetAI = &GetAI_boss_omor_the_unscarredAI;
    newscript->RegisterSelf();
}

