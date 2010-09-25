/* Copyright (C) 2006 - 2008 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/* ScriptData
SDName: Boss_Brutallus
SD%Complete: 80
SDComment: Find a way to start the intro, best code for the intro
EndScriptData */

#include "precompiled.h"
#include "def_sunwell_plateau.h"

enum Quotes
{
 YELL_INTRO                 =   -1580017,
 YELL_INTRO_BREAK_ICE       =   -1580018,
 YELL_INTRO_CHARGE          =   -1580019,
 YELL_INTRO_KILL_MADRIGOSA  =   -1580020,
 YELL_INTRO_TAUNT           =   -1580021,

 YELL_MADR_ICE_BARRIER      =   -1580031,
 YELL_MADR_INTRO            =   -1580032,
 YELL_MADR_ICE_BLOCK        =   -1580033,
 YELL_MADR_TRAP             =   -1580034,
 YELL_MADR_DEATH            =   -1580035,

 YELL_AGGRO                 =   -1580022,
 YELL_KILL1                 =   -1580023,
 YELL_KILL2                 =   -1580024,
 YELL_KILL3                 =   -1580025,
 YELL_LOVE1                 =   -1580026,
 YELL_LOVE2                 =   -1580027,
 YELL_LOVE3                 =   -1580028,
 YELL_BERSERK               =   -1580029,
 YELL_DEATH                 =   -1580030
};

enum Spells
{
    SPELL_METEOR_SLASH                 =   45150,
    SPELL_BURN                         =   46394,
    SPELL_STOMP                        =   45185,
    SPELL_BERSERK                      =   26662,
    SPELL_DUAL_WIELD                   =   42459,

    SPELL_INTRO_FROST_BLAST            =   45203,
    SPELL_INTRO_FROSTBOLT              =   44843,
    SPELL_INTRO_ENCAPSULATE            =   45665,
    SPELL_INTRO_ENCAPSULATE_CHANELLING =   45661
};

#define FELMYST 25038

struct OREGON_DLL_DECL boss_brutallusAI : public ScriptedAI
{
    boss_brutallusAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
    }

    ScriptedInstance* pInstance;
    Unit* Madrigosa;

    uint32 SlashTimer;
    uint32 BurnTimer;
    uint32 StompTimer;
    uint32 BerserkTimer;

    uint32 IntroPhase;
    uint32 IntroPhaseTimer;
    uint32 IntroFrostBoltTimer;

    bool Intro;
    bool IsIntro;
    bool Enraged;

    void Reset()
    {
        SlashTimer = 11000;
        StompTimer = 30000;
        BurnTimer = 60000;
        BerserkTimer = 360000;

        IntroPhase = 0;
        IntroPhaseTimer = 0;
        IntroFrostBoltTimer = 0;

        IsIntro = false;
        Enraged = false;
        Intro = true; //for debug

        me->CastSpell(me, SPELL_DUAL_WIELD, true);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        Madrigosa = Unit::GetUnit(*me, pInstance->GetData64(DATA_MADRIGOSA));
        //Creature* boss = Unit::GetCreature((*me),AzgalorGUID);
        if (!Madrigosa) error_log("Madrigosa ist nicht zu finden");

        if (Intro && Madrigosa){
            if (!Madrigosa->isAlive())
                EndIntro();
            else error_log("Madrigosa is Tod");
        }
        else
            EndIntro();

        if (pInstance)
            pInstance->SetData(DATA_BRUTALLUS_EVENT, NOT_STARTED);
    }

    void EnterCombat(Unit * /*who*/)
    {
        DoScriptText(YELL_AGGRO, me);

        if (pInstance)
            pInstance->SetData(DATA_BRUTALLUS_EVENT, IN_PROGRESS);
    }

    void KilledUnit(Unit* /*victim*/)
    {
        switch(rand()%3)
        {
            case 0: DoScriptText(YELL_KILL1, me); break;
            case 1: DoScriptText(YELL_KILL2, me); break;
            case 2: DoScriptText(YELL_KILL3, me); break;
        }
    }

    void JustDied(Unit* /*Killer*/)
    {
        DoScriptText(YELL_DEATH, me);

        if (pInstance)
        {
            pInstance->SetData(DATA_BRUTALLUS_EVENT, DONE);
            float x,y,z;
            me->GetPosition(x,y,z);
            me->SummonCreature(FELMYST, x,y, z+30, me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 0);
        }
    }

    void StartIntro()
    {
        if (!Intro)
            return;
        error_log("Start Intro");
        if (Madrigosa){
            Madrigosa->setDeathState(ALIVE);
            Madrigosa->setActive(true);
            IsIntro = true;
        }
    }

    void EndIntro()
    {
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        Intro = false;
        IsIntro = false;
        error_log("End Intro");
    }

    void DoIntro()
    {
        if (!Madrigosa)
            return;

        switch(IntroPhase)
        {
            case 0:
                DoScriptText(YELL_MADR_ICE_BARRIER, Madrigosa);
                IntroPhaseTimer = 7000;
                ++IntroPhase;
                break;
            case 1:
                me->SetInFront(Madrigosa);
                Madrigosa->SetInFront(me);
                DoScriptText(YELL_MADR_INTRO, Madrigosa, me);
                IntroPhaseTimer = 9000;
                ++IntroPhase;
                break;
            case 2:
                DoScriptText(YELL_INTRO, me, Madrigosa);
                IntroPhaseTimer = 13000;
                ++IntroPhase;
                break;
            case 3:
                DoCast(me, SPELL_INTRO_FROST_BLAST);
                Madrigosa->AddUnitMovementFlag(MOVEFLAG_ONTRANSPORT | MOVEFLAG_LEVITATING);
                IntroFrostBoltTimer = 3000;
                IntroPhaseTimer = 28000;
                ++IntroPhase;
                break;
            case 4:
                DoScriptText(YELL_INTRO_BREAK_ICE, me);
                IntroPhaseTimer = 6000;
                ++IntroPhase;
                break;
            case 5:
                Madrigosa->CastSpell(me, SPELL_INTRO_ENCAPSULATE_CHANELLING, false);
                DoScriptText(YELL_MADR_TRAP, Madrigosa);
                DoCast(me, SPELL_INTRO_ENCAPSULATE);
                IntroPhaseTimer = 11000;
                ++IntroPhase;
                break;
            case 6:
                me->SetSpeed(MOVE_RUN, 4.0f, true);
                DoScriptText(YELL_INTRO_CHARGE, me);
                IntroPhaseTimer = 5000;
                ++IntroPhase;
                break;
            case 7:
                me->DealDamage(Madrigosa, Madrigosa->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, true);
                DoScriptText(YELL_MADR_DEATH, Madrigosa);
                me->SetHealth(me->GetMaxHealth());
                me->AttackStop();
                me->SetSpeed(MOVE_RUN, 1.0f, true);
                IntroPhaseTimer = 4000;
                ++IntroPhase;
                break;
            case 8:
                DoScriptText(YELL_INTRO_KILL_MADRIGOSA, me);
                me->SetOrientation(0.14);
                Madrigosa->setDeathState(CORPSE);
                IntroPhaseTimer = 8000;
                ++IntroPhase;
                break;
            case 9:
                DoScriptText(YELL_INTRO_TAUNT, me);
                IntroPhaseTimer = 5000;
                ++IntroPhase;
                break;
            case 10:
                EndIntro();
                break;
        }
    }

    void MoveInLineOfSight(Unit *who)
    {
        if (pInstance && Intro)
            pInstance->SetData(DATA_BRUTALLUS_EVENT, SPECIAL);
    }

    void UpdateAI(const uint32 diff)
    {
        if (Intro && !IsIntro)
            StartIntro();

        if (IsIntro)
        {
            if (IntroPhaseTimer <= diff)
                DoIntro();
            else IntroPhaseTimer -= diff;

            if (IntroPhase == 3 + 1)
            {
                if (IntroFrostBoltTimer <= diff)
                {
                    if (Madrigosa){
                        Madrigosa->CastSpell(me, SPELL_INTRO_FROSTBOLT, false);
                        IntroFrostBoltTimer = 2000;
                    }
                } else IntroFrostBoltTimer -= diff;
            }
        }

        if (!UpdateVictim() || IsIntro)
            return;

        if (SlashTimer <= diff)
        {
            DoCast(me->getVictim(), SPELL_METEOR_SLASH);
            SlashTimer = 11000;
        } else SlashTimer -= diff;

        if (StompTimer <= diff)
        {
            switch(rand()%3)
            {
                case 0: DoScriptText(YELL_LOVE1, me); break;
                case 1: DoScriptText(YELL_LOVE2, me); break;
                case 2: DoScriptText(YELL_LOVE3, me); break;
            }
            DoCast(me->getVictim(), SPELL_STOMP);
            StompTimer = 30000;
        } else StompTimer -= diff;

        if (BurnTimer <= diff)
        {
            if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                pTarget->CastSpell(pTarget, SPELL_BURN, true);
            BurnTimer = 60000;
        } else BurnTimer -= diff;

        if (BerserkTimer <= diff && !Enraged)
        {
            DoScriptText(YELL_BERSERK, me);
            DoCast(me, SPELL_BERSERK);
            Enraged = true;
        } else BerserkTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_brutallus(Creature* pCreature)
{
    return new boss_brutallusAI (pCreature);
}

void AddSC_boss_brutallus()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_brutallus";
    newscript->GetAI = &GetAI_boss_brutallus;
    newscript->RegisterSelf();
}
