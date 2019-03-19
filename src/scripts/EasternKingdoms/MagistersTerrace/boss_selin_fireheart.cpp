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
 * with this program. If not, see <https://www.gnu.org/licenses/>.
 */

/* ScriptData
SDName: Boss_Selin_Fireheart
SD%Complete: 90
SDComment: Heroic and Normal Support. Needs further testing.
SDCategory: Magister's Terrace
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "magisters_terrace.h"

enum Texts
{
    SAY_AGGRO                      = -1585000,
    SAY_ENERGY                     = -1585001,
    SAY_EMPOWERED                  = -1585002,
    SAY_KILL_1                     = -1585003,
    SAY_KILL_2                     = -1585004,
    SAY_DEATH                      = -1585005,
    EMOTE_CRYSTAL                  = -1585006
};

enum CrystalSpells
{
    SPELL_FEL_CRYSTAL_COSMETIC     = 44374,
    SPELL_FEL_CRYSTAL_DUMMY        = 44329,
    SPELL_FEL_CRYSTAL_VISUAL       = 44355,
    SPELL_MANA_RAGE                = 44320      // This spell triggers 44321, which changes scale and regens mana Requires an entry in spell_script_target
};

enum SelinSpells
{
    SPELL_DRAIN_LIFE               = 44294,
    SPELL_FEL_EXPLOSION            = 44314,

    H_SPELL_DRAIN_MANA             = 46153,
    H_SPELL_DRAIN_LIFE             = 46155
};

#define CRYSTALS_NUMBER                 5
#define DATA_CRYSTALS                   6

#define CREATURE_FEL_CRYSTAL            24722

struct boss_selin_fireheartAI : public ScriptedAI
{
    boss_selin_fireheartAI(Creature* creature) : ScriptedAI(creature)
    {
        instance = (ScriptedInstance*)creature->GetInstanceData();

        Crystals.clear();
        // GUIDs per instance is static, so we only need to load them once.
        if (instance)
        {
            uint32 size = instance->GetData(DATA_FEL_CRYSTAL_SIZE);
            for (uint8 i = 0; i < size; ++i)
            {
                uint64 guid = instance->GetData64(DATA_FEL_CRYSTAL);
                debug_log("OSCR: Selin: Adding Fel Crystal %llu to list", guid);
                Crystals.push_back(guid);
            }
        }
        Heroic = creature->GetMap()->IsHeroic();
    }

    ScriptedInstance* instance;

    std::list<uint64> Crystals;

    uint32 DrainLifeTimer;
    uint32 DrainManaTimer;
    uint32 FelExplosionTimer;
    uint32 DrainCrystalTimer;
    uint32 EmpowerTimer;

    bool IsDraining;
    bool DrainingCrystal;
    bool Heroic;
    uint64 CrystalGUID;                                     // This will help us create a pointer to the crystal we are draining. We store GUIDs, never units in case unit is deleted/offline (offline if player of course).

    void Reset()
    {
        if (instance)
        {
            //for (uint8 i = 0; i < CRYSTALS_NUMBER; ++i)
            for (std::list<uint64>::const_iterator itr = Crystals.begin(); itr != Crystals.end(); ++itr)
            {
                //Unit* unit = Unit::GetUnit(*me, FelCrystals[i]);
                Unit* unit = Unit::GetUnit(*me, *itr);
                if (unit)
                {
                    if (!unit->IsAlive())
                        CAST_CRE(unit)->Respawn();      // Let the core handle setting death state, etc.

                    // Only need to set unselectable flag. You can't attack unselectable units so non_attackable flag is not necessary here.
                    unit->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                }
            }

            instance->HandleGameObject(instance->GetData64(DATA_SELIN_ENCOUNTER_DOOR), true);
            // Open the big encounter door. Close it in Aggro and open it only in JustDied(and here)
            // Small door opened after event are expected to be closed by default
            // Set Inst data for encounter
            instance->SetData(DATA_SELIN_EVENT, NOT_STARTED);
        }
        else error_log(ERROR_INST_DATA);

        DrainLifeTimer = 3000 + rand() % 4000;
        DrainManaTimer = DrainLifeTimer + 5000;
        FelExplosionTimer = 2100;
        DrainCrystalTimer = 10000 + rand() % 5000;
        DrainCrystalTimer = 20000 + rand() % 5000;
        EmpowerTimer = 10000;

        IsDraining = false;
        DrainingCrystal = false;
        CrystalGUID = 0;
    }

    void SelectNearestCrystal()
    {
        if (Crystals.empty())
            return;

        //float ShortestDistance = 0;
        CrystalGUID = 0;
        Unit* pCrystal = NULL;
        Unit* CrystalChosen = NULL;
        //for (uint8 i =  0; i < CRYSTALS_NUMBER; ++i)
        for (std::list<uint64>::const_iterator itr = Crystals.begin(); itr != Crystals.end(); ++itr)
        {
            pCrystal = NULL;
            //pCrystal = Unit::GetUnit(*me, FelCrystals[i]);
            pCrystal = Unit::GetUnit(*me, *itr);
            if (pCrystal && pCrystal->IsAlive())
            {
                // select nearest
                if (!CrystalChosen || me->GetDistanceOrder(pCrystal, CrystalChosen, false))
                {
                    CrystalGUID = pCrystal->GetGUID();
                    CrystalChosen = pCrystal;               // Store a copy of pCrystal so we don't need to recreate a pointer to closest crystal for the movement and yell.
                }
            }
        }
        if (CrystalChosen)
        {
            DoScriptText(SAY_ENERGY, me);
            DoScriptText(EMOTE_CRYSTAL, me);

            CrystalChosen->CastSpell(CrystalChosen, SPELL_FEL_CRYSTAL_COSMETIC, true);

            float x, y, z;                                  // coords that we move to, close to the crystal.
            CrystalChosen->GetClosePoint(x, y, z, me->GetObjectSize(), CONTACT_DISTANCE);

            me->SetWalk(false);
            me->GetMotionMaster()->MovePoint(1, x, y, z);
            DrainingCrystal = true;
        }
    }

    void ShatterRemainingCrystals()
    {
        if (Crystals.empty())
            return;

        //for (uint8 i = 0; i < CRYSTALS_NUMBER; ++i)
        for (std::list<uint64>::const_iterator itr = Crystals.begin(); itr != Crystals.end(); ++itr)
        {
            //Creature* pCrystal = (Unit::GetCreature(*me, FelCrystals[i]));
            Creature* pCrystal = Unit::GetCreature(*me, *itr);
            if (pCrystal && pCrystal->IsAlive())
                pCrystal->Kill(pCrystal);
        }
    }

    void EnterCombat(Unit* /*who*/)
    {
        DoScriptText(SAY_AGGRO, me);

        if (instance)
            instance->HandleGameObject(instance->GetData64(DATA_SELIN_ENCOUNTER_DOOR), false);
        //Close the encounter door, open it in JustDied/Reset
    }

    void KilledUnit(Unit* /*victim*/)
    {
        DoScriptText(RAND(SAY_KILL_1, SAY_KILL_2), me);
    }

    void MovementInform(uint32 type, uint32 id)
    {
        if (type == POINT_MOTION_TYPE && id == 1)
        {
            Unit* CrystalChosen = Unit::GetUnit(*me, CrystalGUID);
            if (CrystalChosen && CrystalChosen->IsAlive())
            {
                // Make the crystal attackable
                // We also remove NON_ATTACKABLE in case the database has it set.
                CrystalChosen->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE + UNIT_FLAG_NOT_SELECTABLE);
                CrystalChosen->CastSpell(me, SPELL_MANA_RAGE, true);
                IsDraining = true;
            }
            else
            {
                // Make an error message in case something weird happened here
                error_log("OSCR: Selin Fireheart unable to drain crystal as the crystal is either dead or despawned");
                DrainingCrystal = false;
            }
        }
    }

    void JustDied(Unit* /*killer*/)
    {
        DoScriptText(SAY_DEATH, me);

        if (!instance)
            return;

        instance->SetData(DATA_SELIN_EVENT, DONE);                                          // Encounter complete!
        instance->HandleGameObject(instance->GetData64(DATA_SELIN_ENCOUNTER_DOOR), true);   // Open the encounter door
        instance->HandleGameObject(instance->GetData64(DATA_SELIN_DOOR), true);             // Open the door leading further in
        ShatterRemainingCrystals();
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (!DrainingCrystal)
        {
            uint32 maxPowerMana = me->GetMaxPower(POWER_MANA);
            if (maxPowerMana && ((me->GetPower(POWER_MANA) * 100 / maxPowerMana) < 10))
            {
                if (DrainLifeTimer <= diff)
                {
                    DoCast(SelectUnit(SELECT_TARGET_RANDOM, 0), Heroic ? H_SPELL_DRAIN_LIFE : SPELL_DRAIN_LIFE);
                    DrainLifeTimer = 10000;
                }
                else DrainLifeTimer -= diff;

                // Heroic only
                if (Heroic)
                {
                    if (DrainManaTimer <= diff)
                    {
                        DoCast(SelectUnit(SELECT_TARGET_RANDOM, 1), H_SPELL_DRAIN_MANA);
                        DrainManaTimer = 10000;
                    }
                    else DrainManaTimer -= diff;
                }
            }

            if (FelExplosionTimer <= diff)
            {
                if (!me->IsNonMeleeSpellCast(false))
                {
                    DoCast(me, SPELL_FEL_EXPLOSION);
                    FelExplosionTimer = 2000;
                }
            }
            else FelExplosionTimer -= diff;

            // If below 10% mana, start recharging
            maxPowerMana = me->GetMaxPower(POWER_MANA);
            if (maxPowerMana && ((me->GetPower(POWER_MANA) * 100 / maxPowerMana) < 10))
            {
                if (DrainCrystalTimer <= diff)
                {
                    SelectNearestCrystal();
                    if (Heroic)
                        DrainCrystalTimer = 10000 + rand() % 5000;
                    else
                        DrainCrystalTimer = 20000 + rand() % 5000;
                }
                else DrainCrystalTimer -= diff;
            }

        }
        else
        {
            if (IsDraining)
            {
                if (EmpowerTimer <= diff)
                {
                    IsDraining = false;
                    DrainingCrystal = false;

                    DoScriptText(SAY_EMPOWERED, me);

                    Unit* CrystalChosen = Unit::GetUnit(*me, CrystalGUID);
                    if (CrystalChosen && CrystalChosen->IsAlive())
                        // Use Deal Damage to kill it, not setDeathState.
                        CrystalChosen->Kill(CrystalChosen);

                    CrystalGUID = 0;

                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveChase(me->GetVictim());
                }
                else EmpowerTimer -= diff;
            }
        }

        DoMeleeAttackIfReady();                             // No need to check if we are draining crystal here, as the spell has a stun.
    }
};

CreatureAI* GetAI_boss_selin_fireheart(Creature* creature)
{
    return GetInstanceAI<boss_selin_fireheartAI>(creature);
};

struct mob_fel_crystalAI : public ScriptedAI
{
    mob_fel_crystalAI(Creature* creature) : ScriptedAI(creature) {}

    void Reset() {}
    void EnterCombat(Unit* /*who*/) {}
    void AttackStart(Unit* /*who*/) {}
    void MoveInLineOfSight(Unit* /*who*/) {}
    void UpdateAI(const uint32 /*diff*/) {}

    void JustDied(Unit* /*killer*/)
    {
        if (ScriptedInstance* instance = (ScriptedInstance*)me->GetInstanceData())
        {
            Creature* Selin = (Unit::GetCreature(*me, instance->GetData64(DATA_SELIN)));
            if (Selin && Selin->IsAlive())
            {
                if (CAST_AI(boss_selin_fireheartAI, Selin->AI())->CrystalGUID == me->GetGUID())
                {
                    // Set this to false if we are the Creature that Selin is draining so his AI flows properly
                    CAST_AI(boss_selin_fireheartAI, Selin->AI())->DrainingCrystal = false;
                    CAST_AI(boss_selin_fireheartAI, Selin->AI())->IsDraining = false;
                    CAST_AI(boss_selin_fireheartAI, Selin->AI())->EmpowerTimer = 10000;
                    if (Selin->GetVictim())
                    {
                        Selin->AI()->AttackStart(Selin->GetVictim());
                        Selin->GetMotionMaster()->MoveChase(Selin->GetVictim());
                    }
                }
            }
        }
        else error_log(ERROR_INST_DATA);
    }
};

CreatureAI* GetAI_mob_fel_crystal(Creature* creature)
{
    return GetInstanceAI<mob_fel_crystalAI>(creature);
};

void AddSC_boss_selin_fireheart()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_selin_fireheart";
    newscript->GetAI = &GetAI_boss_selin_fireheart;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_fel_crystal";
    newscript->GetAI = &GetAI_mob_fel_crystal;
    newscript->RegisterSelf();
}

