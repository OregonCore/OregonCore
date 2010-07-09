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
SDName: Boss_Bloodboil
SD%Complete: 80
SDComment: Bloodboil not working correctly, missing enrage
SDCategory: Black Temple
EndScriptData */

#include "precompiled.h"
#include "def_black_temple.h"

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
struct ObjectDistanceOrderReversed : public std::binary_function<const WorldObject, const WorldObject, bool>
{
    const Unit* m_pSource;
    ObjectDistanceOrderReversed(const Unit* pSource) : m_pSource(pSource) {};

    bool operator()(const WorldObject* pLeft, const WorldObject* pRight) const
    {
        return !m_pSource->GetDistanceOrder(pLeft, pRight);
    }
};

struct OREGON_DLL_DECL boss_gurtogg_bloodboilAI : public ScriptedAI
{
    boss_gurtogg_bloodboilAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = (c->GetInstanceData());
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

        DoCast(m_creature,SPELL_ACIDIC_WOUND,true);

        m_creature->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, false);
        m_creature->ApplySpellImmune(0, IMMUNITY_EFFECT,SPELL_EFFECT_ATTACK_ME, false);
    }

    void EnterCombat(Unit *who)
    {
        DoZoneInCombat();
        DoScriptText(SAY_AGGRO, m_creature);
        if (pInstance)
            pInstance->SetData(DATA_GURTOGGBLOODBOILEVENT, IN_PROGRESS);
    }

    void KilledUnit(Unit *victim)
    {
        switch(rand()%2)
        {
        case 0: DoScriptText(SAY_SLAY1, m_creature); break;
        case 1: DoScriptText(SAY_SLAY2, m_creature); break;
        }
    }

    void JustDied(Unit *victim)
    {
        if (pInstance)
            pInstance->SetData(DATA_GURTOGGBLOODBOILEVENT, DONE);

        DoScriptText(SAY_DEATH, m_creature);
    }

    // Note: This seems like a very complicated fix. The fix needs to be handled by the core, as implementation of limited-target AoE spells are still not limited.
    void CastBloodboil()
    {
        std::list<Unit *> targets;
        Map *map = m_creature->GetMap();
        if (map->IsDungeon())
        {
            InstanceMap::PlayerList const &PlayerList = ((InstanceMap*)map)->GetPlayers();
            for (InstanceMap::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
            {
                if (Player* i_pl = i->getSource())
                {
                     if (i_pl && i_pl->isAlive())
                        targets.push_back(i_pl);
                }
            }
        }

        //Sort the list of players
        targets.sort(ObjectDistanceOrderReversed(m_creature));
        //Resize so we only get top 5
        targets.resize(5);

        //Aura each player in the targets list with Bloodboil.
        for (std::list<Unit *>::iterator itr = targets.begin(); itr != targets.end(); ++itr)
        {
            Unit* target = *itr;
            if (target && target->isAlive())
                m_creature->AddAura(SPELL_BLOODBOIL, target);
        }
        targets.clear();
    }

    void RevertThreatOnTarget(uint64 guid)
    {
        Unit* pUnit = NULL;
        pUnit = Unit::GetUnit((*m_creature), guid);
        if (pUnit)
        {
            if (DoGetThreat(pUnit))
                DoModifyThreatPercent(pUnit, -100);
            if (TargetThreat)
                m_creature->AddThreat(pUnit, TargetThreat);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (!m_creature->HasAura(SPELL_BERSERK, 0))
        {
            if (EnrageTimer < diff)
            {
                DoCast(m_creature, SPELL_BERSERK);
                switch(rand()%2)
                {
                case 0: DoScriptText(SAY_ENRAGE1, m_creature); break;
                case 1: DoScriptText(SAY_ENRAGE2, m_creature); break;
                }
            } else EnrageTimer -= diff;
        }

        if (ArcingSmashTimer < diff)
        {
            DoCast(m_creature->getVictim(),Phase1 ? SPELL_ARCING_SMASH_1 : SPELL_ARCING_SMASH_2);
            ArcingSmashTimer = 10000;
        } else ArcingSmashTimer -= diff;

        if (FelBreathTimer < diff)
        {
            DoCast(m_creature->getVictim(),Phase1 ? SPELL_FELBREATH_1 : SPELL_FELBREATH_2);
            FelBreathTimer = 25000;
        } else FelBreathTimer -= diff;

        if (EjectTimer < diff)
        {
            DoCast(m_creature->getVictim(),Phase1 ? SPELL_EJECT_1 : SPELL_EJECT_2);
            EjectTimer = 15000;
        } else EjectTimer -= diff;


        if (Charge_Timer < diff)
        {
            if (m_creature->GetDistance2d(m_creature->getVictim()) > 15)
                DoCast(m_creature->getVictim(),SPELL_CHARGE);
            Charge_Timer = 10000;
        } else Charge_Timer -= diff;

        if (Phase1)
        {
            if (BewilderingStrikeTimer < diff)
            {
                DoCast(m_creature->getVictim(), SPELL_BEWILDERING_STRIKE);
                BewilderingStrikeTimer = 20000;
            } else BewilderingStrikeTimer -= diff;

            if (BloodboilTimer < diff)
            {
                if (BloodboilCount < 5)                      // Only cast it five times.
                {
                    CastBloodboil(); // Causes issues on windows, so is commented out.
                    //DoCast(m_creature->getVictim(), SPELL_BLOODBOIL);
                    ++BloodboilCount;
                    BloodboilTimer = 10000;
                }
            } else BloodboilTimer -= diff;
        }

        if (!Phase1)
        {
            if (FelGeyserTimer < diff)
            {
                DoCast(m_creature->getVictim(), SPELL_FEL_GEYSER);
                FelGeyserTimer = 30000;
            } else FelGeyserTimer -= diff;

            if (m_creature->getVictim()->IsImmunedToDamage(SPELL_SCHOOL_MASK_ALL,true))
                m_creature->getThreatManager().modifyThreatPercent(m_creature->getVictim(),-100);
        }

        if (PhaseChangeTimer < diff)
        {
            if (Phase1)
            {
                Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0,100,true);
                if (target && target->isAlive())
                {
                    Phase1 = false;

                    TargetThreat = DoGetThreat(target);
                    TargetGUID = target->GetGUID();
                    if (DoGetThreat(target))
                        DoModifyThreatPercent(target, -100);
                    m_creature->AddThreat(target, 50000000.0f);
                    target->CastSpell(m_creature, SPELL_TAUNT_GURTOGG, true);
                    m_creature->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                    m_creature->ApplySpellImmune(0, IMMUNITY_EFFECT,SPELL_EFFECT_ATTACK_ME, true);

                    // If VMaps are disabled, this spell can call the whole instance
                    DoCast(m_creature, SPELL_INSIGNIFIGANCE, true);
                    DoCast(target,SPELL_FEL_RAGE_1, true);
                    DoCast(target,SPELL_FEL_RAGE_2, true);
                    DoCast(target,SPELL_FEL_RAGE_3, true);
                    DoCast(target,SPELL_FEL_RAGE_SCALE, true);

                    //Cast this without triggered so that it appears in combat logs and shows visual.
                    DoCast(m_creature, SPELL_FEL_RAGE_SELF);

                    switch(rand()%2)
                    {
                    case 0: DoScriptText(SAY_SPECIAL1, m_creature); break;
                    case 1: DoScriptText(SAY_SPECIAL2, m_creature); break;
                    }

                    FelGeyserTimer = 1000;
                    PhaseChangeTimer = 30000;
                }
            } else                                           // Encounter is a loop pretty much. Phase 1 -> Phase 2 -> Phase 1 -> Phase 2 till death or enrage
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
                m_creature->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, false);
                m_creature->ApplySpellImmune(0, IMMUNITY_EFFECT,SPELL_EFFECT_ATTACK_ME, false);
            }
        } else PhaseChangeTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_gurtogg_bloodboil(Creature *_Creature)
{
    return new boss_gurtogg_bloodboilAI (_Creature);
}

void AddSC_boss_gurtogg_bloodboil()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name="boss_gurtogg_bloodboil";
    newscript->GetAI = &GetAI_boss_gurtogg_bloodboil;
    newscript->RegisterSelf();
}