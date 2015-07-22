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
SDName: Boss_Grand_Warlock_Nethekurse
SD%Complete: 99
SDComment:
SDCategory: Hellfire Citadel, Shattered Halls
EndScriptData */

/* ContentData
boss_grand_warlock_nethekurse
mob_fel_orc_convert
mob_lesser_shadow_fissure
EndContentData */

#include "ScriptPCH.h"
#include "shattered_halls.h"

struct Say
{
    int32 id;
};

static Say PeonAttacked[] =
{
    { -1540001},
    { -1540002},
    { -1540003},
    { -1540004},
};
static Say PeonDies[] =
{
    { -1540005},
    { -1540006},
    { -1540007},
    { -1540008},
};

#define SAY_INTRO           -1540000
#define SAY_TAUNT_1         -1540009
#define SAY_TAUNT_2         -1540010
#define SAY_TAUNT_3         -1540011
#define SAY_AGGRO_1         -1540012
#define SAY_AGGRO_2         -1540013
#define SAY_AGGRO_3         -1540014
#define SAY_SLAY_1          -1540015
#define SAY_SLAY_2          -1540016
#define SAY_DIE             -1540017

#define NPC_FEL_ORC                 17083

#define SPELL_DEATH_COIL            30500
#define SPELL_DARK_SPIN             30502
#define SPELL_SHADOW_FISSURE        30496
#define SPELL_SHADOW_SEAR           30735

#define SPELL_SHADOW_CLEAVE         30495
#define H_SPELL_SHADOW_SLAM         35953

#define SPELL_HEMORRHAGE            30478

#define SPELL_CONSUMPTION           30497

struct boss_grand_warlock_nethekurseAI : public ScriptedAI
{
    boss_grand_warlock_nethekurseAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
        HeroicMode = me->GetMap()->IsHeroic();
    }

    ScriptedInstance* pInstance;
    bool HeroicMode;

    bool IntroOnce;
    bool IsIntroEvent;
    bool IsMainEvent;
    bool SpinOnce;
    bool Phase;

    std::vector<uint64> OrcGUID;
    std::list<Creature*> orcs;

    uint32 PeonEngagedCount;
    uint32 PeonKilledCount;

    uint32 IntroEvent_Timer;
    uint32 DeathCoil_Timer;
    uint32 ShadowFissure_Timer;
    uint32 Cleave_Timer;

    void Reset()
    {
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        IsIntroEvent = false;
        IntroOnce = false;
        IsMainEvent = false;
        SpinOnce = false;
        Phase = false;

        PeonEngagedCount = 0;
        PeonKilledCount = 0;

        IntroEvent_Timer = 90000;                           //how long before getting bored and kills his minions?
        DeathCoil_Timer = 20000;
        ShadowFissure_Timer = 8000;
        Cleave_Timer = 5000;

        me->GetCreatureListWithEntryInGrid(orcs, 17083, 40.0f);
        OrcGUID.clear();
        for (std::list<Creature*>::iterator itr = orcs.begin(); itr != orcs.end(); itr++)
            OrcGUID.push_back((*itr)->GetGUID());

        if (pInstance)
            pInstance->SetData(TYPE_NETHEKURSE, NOT_STARTED);
    }

    void DoYellForPeonEnterCombat()
    {
        if (PeonEngagedCount >= 4)
            return;

        DoScriptText(PeonAttacked[PeonEngagedCount].id, me);
        ++PeonEngagedCount;
    }

    void DoYellForPeonDeath()
    {
        if (PeonKilledCount >= 4)
            return;

        DoScriptText(PeonDies[PeonKilledCount].id, me);
        ++PeonKilledCount;

        if (PeonKilledCount == 4)
        {
            IsIntroEvent = false;
            IsMainEvent = true;
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }
    }

    void DoKillPeons()
    {
        switch (rand() % 3)
        {
        case 0:
            DoScriptText(SAY_TAUNT_1, me);
            break;
        case 1:
            DoScriptText(SAY_TAUNT_2, me);
            break;
        case 2:
            DoScriptText(SAY_TAUNT_3, me);
            break;
        }

        for (std::vector<uint64>::const_iterator itr = OrcGUID.begin(); itr != OrcGUID.end(); ++itr)
        {
            if (Creature* pOrc = Creature::GetCreature(*me, *itr))
            {
                if (pOrc->IsAlive())
                {
                    DoCast(pOrc, SPELL_SHADOW_SEAR);
                    pOrc->DealDamage(pOrc, pOrc->GetHealth(), 0, DIRECT_DAMAGE);
                }
            }
        }

        IsIntroEvent = false;
        PeonEngagedCount = 4;
        PeonKilledCount = 4;
        IsMainEvent = true;
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void AttackStart(Unit* who)
    {
        if (IsIntroEvent || !IsMainEvent)
            return;

        if (me->Attack(who, true))
        {
            if (Phase)
                DoStartNoMovement(who);
            else
                DoStartMovement(who);
        }
    }

    void MoveInLineOfSight(Unit* who)
    {
        if (!me->getVictim() && who->isTargetableForAttack() && (me->IsHostileTo(who)) && who->isInAccessiblePlaceFor (me))
        {
            if (!IntroOnce && me->IsWithinDistInMap(who, 40))
            {
                DoScriptText(SAY_INTRO, me);
                IntroOnce = true;
                IsIntroEvent = true;

                if (pInstance)
                    pInstance->SetData(TYPE_NETHEKURSE, IN_PROGRESS);
            }

            if (!me->canFly() && me->GetDistanceZ(who) > CREATURE_Z_ATTACK_RANGE)
                return;

            if (IsIntroEvent || !IsMainEvent)
                return;

            float attackRadius = me->GetAttackDistance(who);
            if (me->IsWithinDistInMap(who, attackRadius) && me->IsWithinLOSInMap(who))
            {
                //who->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);
                me->InterruptNonMeleeSpells(false);
                AttackStart(who);
            }
        }
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
    }

    void JustSummoned(Creature* summoned)
    {
        summoned->setFaction(14);
        summoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        summoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void KilledUnit(Unit* /*victim*/)
    {
        switch (rand() % 2)
        {
        case 0:
            DoScriptText(SAY_SLAY_1, me);
            break;
        case 1:
            DoScriptText(SAY_SLAY_2, me);
            break;
        }
    }

    void EnterEvadeMode()
    {
        me->RemoveAllAuras();
        me->DeleteThreatList();
        me->CombatStop(true);
        Reset();

        if (!me->IsAlive())
            return;

        if (pInstance)
        {
            pInstance->SetData(TYPE_NETHEKURSE, FAIL);
            float fRespX, fRespY, fRespZ;
            me->GetRespawnCoord(fRespX, fRespY, fRespZ);
            me->GetMotionMaster()->MovePoint(0, fRespX, fRespY, fRespZ);
        }
        else
            me->GetMotionMaster()->MoveTargetedHome();

        for (std::vector<uint64>::const_iterator itr = OrcGUID.begin(); itr != OrcGUID.end(); ++itr)
        {
            if (Creature* pOrc = Creature::GetCreature(*me, *itr))
            {
                if (!pOrc->IsAlive())
                {
                    pOrc->ForcedDespawn();
                    pOrc->Respawn();
                }
            }
        }
    }

    void JustDied(Unit* /*Killer*/)
    {
        DoScriptText(SAY_DIE, me);

        if (!pInstance)
            return;

        pInstance->SetData(TYPE_NETHEKURSE, DONE);
    }

    void MovementInform(uint32 uiMotionType, uint32 /*uiPointId*/)
    {
        if (uiMotionType == POINT_MOTION_TYPE)
        {
            for (std::vector<uint64>::const_iterator itr = OrcGUID.begin(); itr != OrcGUID.end(); ++itr)
            {
                if (Creature* pOrc = Creature::GetCreature(*me, *itr))
                    me->SetFacingToObject(pOrc);

            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (IsIntroEvent)
        {
            if (!pInstance)
                return;

            if (pInstance->GetData(TYPE_NETHEKURSE) == IN_PROGRESS)
            {
                if (IntroEvent_Timer <= diff)
                    DoKillPeons();
                else IntroEvent_Timer -= diff;
            }
        }

        if (!UpdateVictim())
            return;

        if (!IsMainEvent)
            return;

        if (Phase)
        {
            if (!SpinOnce)
            {
                DoCastVictim(SPELL_DARK_SPIN);
                SpinOnce = true;
            }

            if (Cleave_Timer <= diff)
            {
                DoCastVictim((HeroicMode ? H_SPELL_SHADOW_SLAM : SPELL_SHADOW_CLEAVE));
                Cleave_Timer = 6000 + rand() % 2500;
            }
            else Cleave_Timer -= diff;

        }
        else
        {
            if (ShadowFissure_Timer <= diff)
            {
                if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                    DoCast(pTarget, SPELL_SHADOW_FISSURE);
                ShadowFissure_Timer = 7500 + rand() % 7500;
            }
            else ShadowFissure_Timer -= diff;

            if (DeathCoil_Timer <= diff)
            {
                if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                    DoCast(pTarget, SPELL_DEATH_COIL);
                DeathCoil_Timer = 15000 + rand() % 5000;
            }
            else DeathCoil_Timer -= diff;

            if ((me->GetHealth() * 100) / me->GetMaxHealth() <= 20)
                Phase = true;

            DoMeleeAttackIfReady();
        }
    }
};

struct mob_fel_orc_convertAI : public ScriptedAI
{
    mob_fel_orc_convertAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }

    ScriptedInstance* pInstance;
    uint32 Hemorrhage_Timer;

    void Reset()
    {
        me->SetNoCallAssistance(true);              //we don't want any assistance (WE R HEROZ!)
        Hemorrhage_Timer = 3000;
    }

    void MoveInLineOfSight(Unit* /*who*/)
    {
        return;
    }

    void EnterCombat(Unit* /*who*/)
    {
        if (pInstance)
        {
            if (pInstance->GetData64(DATA_NETHEKURSE))
            {
                Creature* pKurse = Unit::GetCreature(*me, pInstance->GetData64(DATA_NETHEKURSE));
                if (pKurse)
                    ((boss_grand_warlock_nethekurseAI*)pKurse->AI())->DoYellForPeonEnterCombat();
            }

            if (pInstance->GetData(TYPE_NETHEKURSE) == IN_PROGRESS)
                return;
            else pInstance->SetData(TYPE_NETHEKURSE, IN_PROGRESS);
        }
    }

    void JustDied(Unit* /*Killer*/)
    {
        if (pInstance)
        {
            if (pInstance->GetData64(DATA_NETHEKURSE))
            {
                Creature* pKurse = Unit::GetCreature(*me, pInstance->GetData64(DATA_NETHEKURSE));
                if (pKurse)
                    ((boss_grand_warlock_nethekurseAI*)pKurse->AI())->DoYellForPeonDeath();
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (Hemorrhage_Timer <= diff)
        {
            DoCastVictim(SPELL_HEMORRHAGE);
            Hemorrhage_Timer = 15000;
        }
        else Hemorrhage_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

//NOTE: this creature are also summoned by other spells, for different creatures
struct mob_lesser_shadow_fissureAI : public ScriptedAI
{
    mob_lesser_shadow_fissureAI(Creature* c) : ScriptedAI(c) {}

    bool Start;
    uint32 Stop_Timer;

    void Reset()
    {
        Start = false;
        Stop_Timer = 30000;
    }

    void EnterCombat(Unit* /*who*/) { }

    void MoveInLineOfSight(Unit* /*who*/)
    {
        return;
    }

    void AttackStart(Unit* /*who*/)
    {
        return;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!Start)
        {
            me->CastSpell(me, SPELL_CONSUMPTION, false);
            Start = true;
        }

        if (Stop_Timer <= diff)
        {
            me->setDeathState(JUST_DIED);
            me->SetHealth(0);
            me->CombatStop();
            me->DeleteThreatList();
        }
        else Stop_Timer -= diff;
    }
};

CreatureAI* GetAI_boss_grand_warlock_nethekurse(Creature* pCreature)
{
    return new boss_grand_warlock_nethekurseAI (pCreature);
}

CreatureAI* GetAI_mob_fel_orc_convert(Creature* pCreature)
{
    return new mob_fel_orc_convertAI (pCreature);
}

CreatureAI* GetAI_mob_lesser_shadow_fissure(Creature* pCreature)
{
    return new mob_lesser_shadow_fissureAI (pCreature);
}

void AddSC_boss_grand_warlock_nethekurse()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_grand_warlock_nethekurse";
    newscript->GetAI = &GetAI_boss_grand_warlock_nethekurse;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_fel_orc_convert";
    newscript->GetAI = &GetAI_mob_fel_orc_convert;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_lesser_shadow_fissure";
    newscript->GetAI = &GetAI_mob_lesser_shadow_fissure;
    newscript->RegisterSelf();
}

