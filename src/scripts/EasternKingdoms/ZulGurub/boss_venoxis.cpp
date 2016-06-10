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
SDName: Boss_Venoxis
SD%Complete: 100
SDComment:
SDCategory: Zul'Gurub
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "zulgurub.h"

enum Texts
{
    SAY_TRANSFORM   =   -1309000,
    SAY_DEATH       =   -1309001
};

enum Spells
{
    SPELL_HOLY_FIRE    = 23860,
    SPELL_HOLY_WRATH   = 23979,
    SPELL_VENOMSPIT    = 23862,
    SPELL_HOLY_NOVA    = 23858,
    SPELL_POISON_CLOUD = 23861,
    SPELL_SNAKE_FORM   = 23849,
    SPELL_RENEW        = 23895,
    SPELL_BERSERK      = 23537,
    SPELL_DISPELL      = 23859
};

struct boss_venoxisAI : public ScriptedAI
{
    boss_venoxisAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }

    ScriptedInstance* pInstance;

    uint32 HolyFire_Timer;
    uint32 HolyWrath_Timer;
    uint32 VenomSpit_Timer;
    uint32 Renew_Timer;
    uint32 PoisonCloud_Timer;
    uint32 HolyNova_Timer;
    uint32 Dispell_Timer;
    uint32 TargetInRange;

    bool PhaseTwo;
    bool InBerserk;

    void Reset()
    {
        HolyFire_Timer = 10000;
        HolyWrath_Timer = 60500;
        VenomSpit_Timer = 5500;
        Renew_Timer = 30500;
        PoisonCloud_Timer = 2000;
        HolyNova_Timer = 5000;
        Dispell_Timer = 35000;
        TargetInRange = 0;

        PhaseTwo = false;
        InBerserk = false;
    }

    void EnterCombat(Unit* /*who*/)
    {
    }

    void JustDied(Unit* /*Killer*/)
    {
        DoScriptText(SAY_DEATH, me);
        if (pInstance)
            pInstance->SetData(TYPE_VENOXIS, DONE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (HealthAbovePct(50))
        {
            if (Dispell_Timer <= diff)
            {
                DoCast(me, SPELL_DISPELL);
                Dispell_Timer = 15000 + rand() % 15000;
            }
            else
                Dispell_Timer -= diff;

            if (Renew_Timer <= diff)
            {
                DoCast(me, SPELL_RENEW);
                Renew_Timer = 20000 + rand() % 10000;
            }
            else
                Renew_Timer -= diff;

            if (HolyWrath_Timer <= diff)
            {
                DoCastVictim( SPELL_HOLY_WRATH);
                HolyWrath_Timer = 15000 + rand() % 10000;
            }
            else
                HolyWrath_Timer -= diff;

            if (HolyNova_Timer <= diff)
            {
                TargetInRange = 0;
                for (uint8 i = 0; i < 10; ++i)
                {
                    if (Unit* pTarget = SelectUnit(SELECT_TARGET_TOPAGGRO, i))
                        if (me->IsWithinMeleeRange(pTarget))
                            ++TargetInRange;
                }

                if (TargetInRange > 1)
                {
                    DoCastVictim( SPELL_HOLY_NOVA);
                    HolyNova_Timer = 1000;
                }
                else
                    HolyNova_Timer = 2000;

            }
            else
                HolyNova_Timer -= diff;

            if (HolyFire_Timer <= diff && TargetInRange < 3)
            {
                if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                    DoCast(pTarget, SPELL_HOLY_FIRE);

                HolyFire_Timer = 8000;
            }
            else
                HolyFire_Timer -= diff;
        }
        else
        {
            if (!PhaseTwo)
            {
                DoScriptText(SAY_TRANSFORM, me);
                me->InterruptNonMeleeSpells(false);
                DoCast(me, SPELL_SNAKE_FORM);
                me->SetObjectScale(2.0f);
                const CreatureInfo* cinfo = me->GetCreatureTemplate();
                me->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, (cinfo->mindmg + ((cinfo->mindmg / 100) * 25)));
                me->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, (cinfo->maxdmg + ((cinfo->maxdmg / 100) * 25)));
                me->UpdateDamagePhysical(BASE_ATTACK);
                DoResetThreat();
                PhaseTwo = true;
            }

            if (PhaseTwo && PoisonCloud_Timer <= diff)
            {
                DoCastVictim( SPELL_POISON_CLOUD);
                PoisonCloud_Timer = 15000;
            }

            PoisonCloud_Timer -= diff;

            if (PhaseTwo && VenomSpit_Timer <= diff)
            {
                if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                    DoCast(pTarget, SPELL_VENOMSPIT);

                VenomSpit_Timer = 15000 + rand() % 5000;
            }
            else
                VenomSpit_Timer -= diff;

            if (PhaseTwo && HealthBelowPct(10))
            {
                if (!InBerserk)
                {
                    me->InterruptNonMeleeSpells(false);
                    DoCast(me, SPELL_BERSERK);
                    InBerserk = true;
                }
            }
        }
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_venoxis(Creature* pCreature)
{
    return new boss_venoxisAI (pCreature);
}

void AddSC_boss_venoxis()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "boss_venoxis";
    newscript->GetAI = &GetAI_boss_venoxis;
    newscript->RegisterSelf();
}

