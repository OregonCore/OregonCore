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
SDName: Boss_Talon_King_Ikiss
SD%Complete: 95
SDComment: Heroic supported. Some details missing, but most are spell related.
SDCategory: Auchindoun, Sethekk Halls
EndScriptData */

#include "ScriptPCH.h"
#include "sethekk_halls.h"

#define SAY_INTRO                   -1556007

#define SAY_AGGRO_1                 -1556008
#define SAY_AGGRO_2                 -1556009
#define SAY_AGGRO_3                 -1556010

#define SAY_SLAY_1                  -1556011
#define SAY_SLAY_2                  -1556012
#define SAY_DEATH                   -1556013
#define EMOTE_ARCANE_EXP            -1556015

#define SPELL_BLINK                 38194
#define SPELL_BLINK_TELEPORT        38203
#define SPELL_MANA_SHIELD           38151
#define SPELL_ARCANE_BUBBLE         9438
#define H_SPELL_SLOW                35032

#define SPELL_POLYMORPH             38245
#define H_SPELL_POLYMORPH           43309

#define SPELL_ARCANE_VOLLEY         35059
#define H_SPELL_ARCANE_VOLLEY       40424

#define SPELL_ARCANE_EXPLOSION      38197
#define H_SPELL_ARCANE_EXPLOSION    40425

struct boss_talon_king_ikissAI : public ScriptedAI
{
    boss_talon_king_ikissAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
        HeroicMode = me->GetMap()->IsHeroic();
    }

    ScriptedInstance* pInstance;

    bool HeroicMode;

    uint32 ArcaneVolley_Timer;
    uint32 Sheep_Timer;
    uint32 Blink_Timer;
    uint32 Slow_Timer;

    bool ManaShield;
    bool Blink;
    bool Intro;

    void Reset()
    {
        ArcaneVolley_Timer = 5000;
        Sheep_Timer = 8000;
        Blink_Timer = 35000;
        Slow_Timer = 15000+rand()%15000;
        Blink = false;
        Intro = false;
        ManaShield = false;

        if (pInstance)
            pInstance->SetData(DATA_IKISSEVENT, NOT_STARTED);
    }

    void MoveInLineOfSight(Unit *who)
    {
        if (!me->getVictim() && who->isTargetableForAttack() && (me->IsHostileTo(who)) && who->isInAccessiblePlaceFor (me))
        {
            if (!Intro && me->IsWithinDistInMap(who, 100))
            {
                Intro = true;
                DoScriptText(SAY_INTRO, me);
            }

            if (!me->canFly() && me->GetDistanceZ(who) > CREATURE_Z_ATTACK_RANGE)
                return;

            float attackRadius = me->GetAttackDistance(who);
            if (me->IsWithinDistInMap(who, attackRadius) && me->IsWithinLOSInMap(who))
            {
                //who->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);
                AttackStart(who);
            }
        }
    }

    void EnterCombat(Unit *)
    {
        switch(rand()%3)
        {
            case 0: DoScriptText(SAY_AGGRO_1, me); break;
            case 1: DoScriptText(SAY_AGGRO_2, me); break;
            case 2: DoScriptText(SAY_AGGRO_3, me); break;
        }

        if (pInstance)
            pInstance->SetData(DATA_IKISSEVENT, IN_PROGRESS);
    }

    void JustDied(Unit*)
    {
        DoScriptText(SAY_DEATH, me);

        if (pInstance)
            pInstance->SetData(DATA_IKISSEVENT, DONE);
    }

    void KilledUnit(Unit*)
    {
        switch(rand()%2)
        {
            case 0: DoScriptText(SAY_SLAY_1, me); break;
            case 1: DoScriptText(SAY_SLAY_2, me); break;
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (Blink)
        {
            DoCast(me,HeroicMode ? H_SPELL_ARCANE_EXPLOSION : SPELL_ARCANE_EXPLOSION);
            me->CastSpell(me,SPELL_ARCANE_BUBBLE,true);
            Blink = false;
        }

        if (ArcaneVolley_Timer <= diff)
        {
            DoCast(me,HeroicMode ? H_SPELL_ARCANE_VOLLEY : SPELL_ARCANE_VOLLEY);
            ArcaneVolley_Timer = 10000+rand()%5000;
        } else ArcaneVolley_Timer -= diff;

        if (Sheep_Timer <= diff)
        {
            //second top aggro target in normal, random target in heroic correct?
            Unit *pTarget = NULL;
            pTarget = HeroicMode ? SelectUnit(SELECT_TARGET_RANDOM,0) : SelectUnit(SELECT_TARGET_TOPAGGRO,1);
            if (pTarget)
                DoCast(pTarget,HeroicMode ? H_SPELL_POLYMORPH : SPELL_POLYMORPH);
            Sheep_Timer = 15000+rand()%2500;
        } else Sheep_Timer -= diff;

        //may not be correct time to cast
        if (!ManaShield && ((me->GetHealth()*100) / me->GetMaxHealth() < 20))
        {
            DoCast(me,SPELL_MANA_SHIELD);
            ManaShield = true;
        }

        if (HeroicMode)
        {
            if (Slow_Timer <= diff)
            {
                DoCast(me,H_SPELL_SLOW);
                Slow_Timer = 15000+rand()%25000;
            } else Slow_Timer -= diff;
        }

        if (Blink_Timer <= diff)
        {
            DoScriptText(EMOTE_ARCANE_EXP, me);

            if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM,0))
            {
                if (me->IsNonMeleeSpellCast(false))
                    me->InterruptNonMeleeSpells(false);

                //Spell doesn't work, but we use for visual effect at least
                DoCast(pTarget,SPELL_BLINK);

                float X = pTarget->GetPositionX();
                float Y = pTarget->GetPositionY();
                float Z = pTarget->GetPositionZ();

                DoTeleportTo(X,Y,Z);

                DoCast(pTarget,SPELL_BLINK_TELEPORT);
                Blink = true;
            }
            Blink_Timer = 35000+rand()%5000;
        } else Blink_Timer -= diff;

        if (!Blink)
            DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_talon_king_ikiss(Creature* pCreature)
{
    return new boss_talon_king_ikissAI (pCreature);
}

void AddSC_boss_talon_king_ikiss()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_talon_king_ikiss";
    newscript->GetAI = &GetAI_boss_talon_king_ikiss;
    newscript->RegisterSelf();
}
