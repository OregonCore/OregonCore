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
SDName: Boss_Bloodboil
SD%Complete: 80
SDComment: Bloodboil not working correctly, missing enrage
SDCategory: Black Temple
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "black_temple.h"

//Speech'n'Sound
#define SAY_AGGRO               -1564029
#define SAY_SLAY1               -1564030
#define SAY_SLAY2               -1564031
#define SAY_SPECIAL1            -1564032
#define SAY_SPECIAL2            -1564033
#define SAY_ENRAGE1             -1564034
#define SAY_ENRAGE2             -1564035
#define SAY_DEATH               -1564036

//Spells
//Phase 1
#define SPELL_ARCING_SMASH_1        40599
#define SPELL_ACIDIC_WOUND          40484 //Trigger Aura
#define SPELL_BEWILDERING_STRIKE    40491
#define SPELL_EJECT_1               40486
#define SPELL_FELBREATH_1           40508
#define SPELL_BLOODBOIL             42005

//Phase 2
#define SPELL_ARCING_SMASH_2        40457
#define SPELL_EJECT_2               40597
#define SPELL_FELBREATH_2           40595
#define SPELL_FEL_GEYSER            40593

//Fel Rage
#define SPELL_INSIGNIFIGANCE        40618
#define SPELL_TAUNT_GURTOGG         40603
#define SPELL_FEL_RAGE_SELF         40594
#define SPELL_FEL_RAGE_1            40604
#define SPELL_FEL_RAGE_2            40616
#define SPELL_FEL_RAGE_3            41625
#define SPELL_FEL_RAGE_SCALE        46787

#define SPELL_CHARGE                40602
#define SPELL_BERSERK               45078

#define COORD_Z_HOME                64.0f

//This is used to sort the players by distance in preparation for the Bloodboil cast.

struct boss_gurtogg_bloodboilAI : public ScriptedAI
{
    boss_gurtogg_bloodboilAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }

    ScriptedInstance* pInstance;

    uint64 TargetGUID;

    float TargetThreat;

    uint32 BloodboilTimer;
    uint32 BloodboilCount;

    uint32 FelGeyserTimer;
    uint32 BewilderingStrikeTimer;

    uint32 ArcingSmashTimer;
    uint32 FelBreathTimer;
    uint32 EjectTimer;

    uint32 PhaseChangeTimer;

    uint32 EnrageTimer;

    uint32 Charge_Timer;

    bool Phase1;

    void Reset()
    {
        if (pInstance)
            pInstance->SetData(DATA_GURTOGGBLOODBOILEVENT, NOT_STARTED);

        TargetGUID = 0;

        TargetThreat = 0;

        BloodboilTimer = 10000;
        BloodboilCount = 0;

        FelGeyserTimer = 1000;
        BewilderingStrikeTimer = 15000;

        ArcingSmashTimer = 19000;
        FelBreathTimer = 25000;
        EjectTimer = 10000;

        PhaseChangeTimer = 65000;
        EnrageTimer = 600000;

        Phase1 = true;

        Charge_Timer = 30000;

        DoCast(me, SPELL_ACIDIC_WOUND, true);

        me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
    }

    void EnterCombat(Unit* /*who*/)
    {
        DoZoneInCombat();
        DoScriptText(SAY_AGGRO, me);
        if (pInstance)
            pInstance->SetData(DATA_GURTOGGBLOODBOILEVENT, IN_PROGRESS);
    }

    void KilledUnit(Unit* /*victim*/)
    {
        DoScriptText(RAND(SAY_SLAY1, SAY_SLAY2), me);
    }

    void JustDied(Unit* /*victim*/)
    {
        if (pInstance)
            pInstance->SetData(DATA_GURTOGGBLOODBOILEVENT, DONE);

        DoScriptText(SAY_DEATH, me);
    }

    // Note: This seems like a very complicated fix. The fix needs to be handled by the core, as implementation of limited-target AoE spells are still not limited.
    void CastBloodboil()
    {
        std::list<Unit* > targets;
        Map* map = me->GetMap();
        if (map->IsDungeon())
        {
            InstanceMap::PlayerList const& PlayerList = ((InstanceMap*)map)->GetPlayers();
            for (InstanceMap::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
            {
                if (Player* i_pl = i->getSource())
                {
                    if (i_pl && i_pl->IsAlive())
                        targets.push_back(i_pl);
                }
            }
        }

        //Sort the list of players
        targets.sort(Oregon::ObjectDistanceOrderPred(me, false));
        //Resize so we only get top 5
        targets.resize(5);

        //Aura each player in the targets list with Bloodboil.
        for (std::list<Unit* >::iterator itr = targets.begin(); itr != targets.end(); ++itr)
        {
            Unit* pTarget = *itr;
            if (pTarget && pTarget->IsAlive())
                me->AddAura(SPELL_BLOODBOIL, pTarget);
        }
        targets.clear();
    }

    void RevertThreatOnTarget(uint64 guid)
    {
        Unit* pUnit = NULL;
        pUnit = Unit::GetUnit((*me), guid);
        if (pUnit)
        {
            if (DoGetThreat(pUnit))
                DoModifyThreatPercent(pUnit, -100);
            if (TargetThreat)
                me->AddThreat(pUnit, TargetThreat);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (!me->HasAura(SPELL_BERSERK, 0))
        {
            if (EnrageTimer <= diff)
            {
                DoCast(me, SPELL_BERSERK);
                switch (rand() % 2)
                {
                case 0:
                    DoScriptText(SAY_ENRAGE1, me);
                    break;
                case 1:
                    DoScriptText(SAY_ENRAGE2, me);
                    break;
                }
            }
            else EnrageTimer -= diff;
        }

        if (ArcingSmashTimer <= diff)
        {
            DoCastVictim(Phase1 ? SPELL_ARCING_SMASH_1 : SPELL_ARCING_SMASH_2);
            ArcingSmashTimer = 10000;
        }
        else ArcingSmashTimer -= diff;

        if (FelBreathTimer <= diff)
        {
            DoCastVictim(Phase1 ? SPELL_FELBREATH_1 : SPELL_FELBREATH_2);
            FelBreathTimer = 25000;
        }
        else FelBreathTimer -= diff;

        if (EjectTimer <= diff)
        {
            DoCastVictim(Phase1 ? SPELL_EJECT_1 : SPELL_EJECT_2);
            EjectTimer = 15000;
        }
        else EjectTimer -= diff;


        if (Charge_Timer <= diff)
        {
            if (me->GetDistance2d(me->getVictim()) > 15)
                DoCastVictim(SPELL_CHARGE);
            Charge_Timer = 10000;
        }
        else Charge_Timer -= diff;

        if (Phase1)
        {
            if (BewilderingStrikeTimer <= diff)
            {
                DoCastVictim( SPELL_BEWILDERING_STRIKE);
                BewilderingStrikeTimer = 20000;
            }
            else BewilderingStrikeTimer -= diff;

            if (BloodboilTimer <= diff)
            {
                if (BloodboilCount < 5)                      // Only cast it five times.
                {
                    CastBloodboil(); // Causes issues on windows, so is commented out.
                    //DoCastVictim( SPELL_BLOODBOIL);
                    ++BloodboilCount;
                    BloodboilTimer = 10000;
                }
            }
            else BloodboilTimer -= diff;
        }

        if (!Phase1)
        {
            if (FelGeyserTimer <= diff)
            {
                DoCastVictim( SPELL_FEL_GEYSER);
                FelGeyserTimer = 30000;
            }
            else FelGeyserTimer -= diff;

            if (me->getVictim() && me->getVictim()->IsImmunedToDamage(SPELL_SCHOOL_MASK_ALL, true))
                me->getThreatManager().modifyThreatPercent(me->getVictim(), -100);
        }

        if (PhaseChangeTimer <= diff)
        {
            if (Phase1)
            {
                Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);
                if (pTarget && pTarget->IsAlive())
                {
                    Phase1 = false;

                    TargetThreat = DoGetThreat(pTarget);
                    TargetGUID = pTarget->GetGUID();
                    if (DoGetThreat(pTarget))
                        DoModifyThreatPercent(pTarget, -100);
                    me->AddThreat(pTarget, 50000000.0f);
                    pTarget->CastSpell(me, SPELL_TAUNT_GURTOGG, true);
                    me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                    me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);

                    // If VMaps are disabled, this spell can call the whole instance
                    DoCast(me, SPELL_INSIGNIFIGANCE, true);
                    DoCast(pTarget, SPELL_FEL_RAGE_1, true);
                    DoCast(pTarget, SPELL_FEL_RAGE_2, true);
                    DoCast(pTarget, SPELL_FEL_RAGE_3, true);
                    DoCast(pTarget, SPELL_FEL_RAGE_SCALE, true);

                    //Cast this without triggered so that it appears in combat logs and shows visual.
                    DoCast(me, SPELL_FEL_RAGE_SELF);

                    DoScriptText(RAND(SAY_SPECIAL1, SAY_SPECIAL2), me);

                    FelGeyserTimer = 1000;
                    PhaseChangeTimer = 30000;
                }
            }
            else                                             // Encounter is a loop pretty much. Phase 1 -> Phase 2 -> Phase 1 -> Phase 2 till death or enrage
            {
                if (TargetGUID)
                    RevertThreatOnTarget(TargetGUID);
                TargetGUID = 0;
                Phase1 = true;
                BloodboilTimer = 10000;
                BloodboilCount = 0;
                ArcingSmashTimer += 2000;
                FelBreathTimer += 2000;
                EjectTimer += 2000;
                PhaseChangeTimer = 65000;
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, false);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, false);
            }
        }
        else PhaseChangeTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_gurtogg_bloodboil(Creature* pCreature)
{
    return new boss_gurtogg_bloodboilAI (pCreature);
}

void AddSC_boss_gurtogg_bloodboil()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "boss_gurtogg_bloodboil";
    newscript->GetAI = &GetAI_boss_gurtogg_bloodboil;
    newscript->RegisterSelf();
}
