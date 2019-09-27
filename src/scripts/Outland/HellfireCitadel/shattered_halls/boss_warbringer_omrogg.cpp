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
SDName: Boss_Warbringer_Omrogg
SD%Complete: 99
SDComment: Heroic enabled. Spell timing may need additional tweaks
SDCategory: Hellfire Citadel, Shattered Halls
EndScriptData */

/* ContentData
mob_omrogg_heads
boss_warbringer_omrogg
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "shattered_halls.h"

#define ENTRY_LEFT_HEAD             19523
#define ENTRY_RIGHT_HEAD            19524

struct Yell
{
    int32 id;
    uint32 creature;
};

static Yell GoCombat[] =
{
    { -1540018, ENTRY_LEFT_HEAD},
    { -1540019, ENTRY_LEFT_HEAD},
    { -1540020, ENTRY_LEFT_HEAD},
};
static Yell GoCombatDelay[] =
{
    { -1540021, ENTRY_RIGHT_HEAD},
    { -1540022, ENTRY_RIGHT_HEAD},
    { -1540023, ENTRY_RIGHT_HEAD},
};

static Yell Threat[] =
{
    { -1540024, ENTRY_LEFT_HEAD},
    { -1540025, ENTRY_RIGHT_HEAD},
    { -1540026, ENTRY_LEFT_HEAD},
    { -1540027, ENTRY_LEFT_HEAD},
};
static Yell ThreatDelay1[] =
{
    { -1540028, ENTRY_RIGHT_HEAD},
    { -1540029, ENTRY_LEFT_HEAD},
    { -1540030, ENTRY_RIGHT_HEAD},
    { -1540031, ENTRY_RIGHT_HEAD},
};
static Yell ThreatDelay2[] =
{
    { -1540032, ENTRY_LEFT_HEAD},
    { -1540033, ENTRY_RIGHT_HEAD},
    { -1540034, ENTRY_LEFT_HEAD},
    { -1540035, ENTRY_LEFT_HEAD},
};

static Yell Killing[] =
{
    { -1540036, ENTRY_LEFT_HEAD},
    { -1540037, ENTRY_RIGHT_HEAD},
};
static Yell KillingDelay[] =
{
    { -1540038, ENTRY_RIGHT_HEAD},
    { -1000000, ENTRY_LEFT_HEAD},
};

#define YELL_DIE_L                  -1540039
#define YELL_DIE_R                  -1540040
#define EMOTE_ENRAGE                -1540041

#define SPELL_BLAST_WAVE            30600
#define SPELL_FEAR                  30584
#define SPELL_THUNDERCLAP           30633

#define SPELL_BURNING_MAUL          30598
#define H_SPELL_BURNING_MAUL        36056

struct mob_omrogg_headsAI : public ScriptedAI
{
    mob_omrogg_headsAI(Creature* c) : ScriptedAI(c) {}

    bool DeathYell;
    uint32 Death_Timer;

    void Reset() {}
    void EnterCombat(Unit* /*who*/) { }

    void DoDeathYell()
    {
        Death_Timer = 4000;
        DeathYell = true;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!DeathYell)
            return;

        if (Death_Timer <= diff)
        {
            DoScriptText(YELL_DIE_R, me);
            DeathYell = false;
        }
        else Death_Timer -= diff;
    }
};

struct boss_warbringer_omroggAI : public ScriptedAI
{
    boss_warbringer_omroggAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
        HeroicMode = me->GetMap()->IsHeroic();
    }

    ScriptedInstance* pInstance;
    bool HeroicMode;

    uint64 LeftHead;
    uint64 RightHead;
    int iaggro;
    int ithreat;
    int ikilling;

    bool AggroYell;
    bool ThreatYell;
    bool ThreatYell2;
    bool KillingYell;

    uint32 Delay_Timer;
    uint32 BlastWave_Timer;
    uint32 BlastCount;
    uint32 Fear_Timer;
    uint32 BurningMaul_Timer;
    uint32 ThunderClap_Timer;
    uint32 ResetThreat_Timer;

    void Reset()
    {
        LeftHead = 0;
        RightHead = 0;

        AggroYell = false;
        ThreatYell = false;
        ThreatYell2 = false;
        KillingYell = false;

        Delay_Timer = 4000;
        BlastWave_Timer = 0;
        BlastCount = 0;
        Fear_Timer = 8000;
        BurningMaul_Timer = 25000;
        ThunderClap_Timer = 15000;
        ResetThreat_Timer = 30000;

        if (pInstance)
            pInstance->SetData(TYPE_WARBRINGER, NOT_STARTED);
    }

    void DoYellForThreat()
    {
        if (LeftHead && RightHead)
        {
            Unit* Left  = Unit::GetUnit(*me, LeftHead);
            Unit* Right = Unit::GetUnit(*me, RightHead);

            if (!Left || !Right)
                return;

            ithreat = rand() % 4;

            Unit* source = (Left->GetEntry() == Threat[ithreat].creature ? Left : Right);

            DoScriptText(Threat[ithreat].id, source);

            Delay_Timer = 3500;
            ThreatYell = true;
        }
    }

    void EnterCombat(Unit* /*who*/)
    {
        DoSpawnCreature(ENTRY_LEFT_HEAD, 0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN, 1800000);
        DoSpawnCreature(ENTRY_RIGHT_HEAD, 0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN, 1800000);

        if (Unit* Left = Unit::GetUnit(*me, LeftHead))
        {
            iaggro = rand() % 3;

            DoScriptText(GoCombat[iaggro].id, Left);

            Delay_Timer = 3500;
            AggroYell = true;
        }

        if (pInstance)
            pInstance->SetData(TYPE_WARBRINGER, IN_PROGRESS);
    }

    void JustSummoned(Creature* summoned)
    {
        if (summoned->GetEntry() == ENTRY_LEFT_HEAD)
            LeftHead = summoned->GetGUID();

        if (summoned->GetEntry() == ENTRY_RIGHT_HEAD)
            RightHead = summoned->GetGUID();

        //summoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        //summoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        summoned->SetVisible(false);
    }

    void KilledUnit(Unit* /*victim*/)
    {
        if (LeftHead && RightHead)
        {
            Unit* Left  = Unit::GetUnit(*me, LeftHead);
            Unit* Right = Unit::GetUnit(*me, RightHead);

            if (!Left || !Right)
                return;

            ikilling = rand() % 2;

            Unit* source = (Left->GetEntry() == Killing[ikilling].creature ? Left : Right);

            switch (ikilling)
            {
            case 0:
                DoScriptText(Killing[ikilling].id, source);
                Delay_Timer = 3500;
                KillingYell = true;
                break;
            case 1:
                DoScriptText(Killing[ikilling].id, source);
                KillingYell = false;
                break;
            }
        }
    }

    void JustDied(Unit* /*Killer*/)
    {
        if (LeftHead && RightHead)
        {
            Unit* Left  = Unit::GetUnit(*me, LeftHead);
            Unit* Right = Unit::GetUnit(*me, RightHead);

            if (!Left || !Right)
                return;

            DoScriptText(YELL_DIE_L, Left);

            ((mob_omrogg_headsAI*)CAST_CRE(Right)->AI())->DoDeathYell();
        }

        if (pInstance)
            pInstance->SetData(TYPE_WARBRINGER, DONE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (Delay_Timer <= diff)
        {
            Delay_Timer = 3500;

            if (!LeftHead || !RightHead)
                return;

            Unit* Left  = Unit::GetUnit(*me, LeftHead);
            Unit* Right = Unit::GetUnit(*me, RightHead);

            if (!Left || !Right)
                return;

            if (AggroYell)
            {
                DoScriptText(GoCombatDelay[iaggro].id, Right);
                AggroYell = false;
            }

            if (ThreatYell2)
            {
                Unit* source = (Left->GetEntry() == ThreatDelay2[ithreat].creature ? Left : Right);

                DoScriptText(ThreatDelay2[ithreat].id, source);
                ThreatYell2 = false;
            }

            if (ThreatYell)
            {
                Unit* source = (Left->GetEntry() == ThreatDelay1[ithreat].creature ? Left : Right);

                DoScriptText(ThreatDelay1[ithreat].id, source);
                ThreatYell = false;
                ThreatYell2 = true;
            }

            if (KillingYell)
            {
                Unit* source = (Left->GetEntry() == KillingDelay[ikilling].creature ? Left : Right);

                DoScriptText(KillingDelay[ikilling].id, source);
                KillingYell = false;
            }
        }
        else Delay_Timer -= diff;

        if (!UpdateVictim())
            return;

        if (BlastCount && BlastWave_Timer <= diff)
        {
            DoCast(me, SPELL_BLAST_WAVE);
            BlastWave_Timer = 5000;
            ++BlastCount;

            if (BlastCount == 3)
                BlastCount = 0;
        }
        else BlastWave_Timer -= diff;

        if (BurningMaul_Timer <= diff)
        {
            DoScriptText(EMOTE_ENRAGE, me);
            DoCast(me, HeroicMode ? H_SPELL_BURNING_MAUL : SPELL_BURNING_MAUL);
            BurningMaul_Timer = 40000;
            BlastWave_Timer = 16000;
            BlastCount = 1;
        }
        else BurningMaul_Timer -= diff;

        if (ResetThreat_Timer <= diff)
        {
            if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
            {
                DoYellForThreat();
                DoResetThreat();
                me->AddThreat(pTarget, 0.0f);
            }
            ResetThreat_Timer = 35000 + rand() % 10000;
        }
        else ResetThreat_Timer -= diff;

        if (Fear_Timer <= diff)
        {
            DoCast(me, SPELL_FEAR);
            Fear_Timer = 15000 + rand() % 25000;
        }
        else Fear_Timer -= diff;

        if (ThunderClap_Timer <= diff)
        {
            DoCast(me, SPELL_THUNDERCLAP);
            ThunderClap_Timer = 25000 + rand() % 15000;
        }
        else ThunderClap_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_warbringer_omrogg(Creature* pCreature)
{
    return GetInstanceAI<boss_warbringer_omroggAI>(pCreature);
}

CreatureAI* GetAI_mob_omrogg_heads(Creature* pCreature)
{
    return new mob_omrogg_headsAI (pCreature);
}

void AddSC_boss_warbringer_omrogg()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_warbringer_omrogg";
    newscript->GetAI = &GetAI_boss_warbringer_omrogg;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_omrogg_heads";
    newscript->GetAI = &GetAI_mob_omrogg_heads;
    newscript->RegisterSelf();
}

