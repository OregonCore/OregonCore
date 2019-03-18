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
SDName: Boss_Shade_of_Aran
SD%Complete: 98
SDComment: Elementals should die when Aran is killed.
SDCategory: Karazhan
EndScriptData */

#include "WorldPacket.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedSimpleAI.h"
#include "karazhan.h"
#include "Player.h"

enum Text
{
    SAY_AGGRO1                 = -1532073,
    SAY_AGGRO2                 = -1532074,
    SAY_AGGRO3                 = -1532075,
    SAY_FLAMEWREATH1           = -1532076,
    SAY_FLAMEWREATH2           = -1532077,
    SAY_BLIZZARD1              = -1532078,
    SAY_BLIZZARD2              = -1532079,
    SAY_EXPLOSION1             = -1532080,
    SAY_EXPLOSION2             = -1532081,
    SAY_DRINK                  = -1532082,               //Low Mana / AoE Pyroblast
    SAY_ELEMENTALS             = -1532083,
    SAY_KILL1                  = -1532084,
    SAY_KILL2                  = -1532085,
    SAY_TIMEOVER               = -1532086,
    SAY_DEATH                  = -1532087,
    SAY_ATIESH                 = -1532088                //Atiesh is equipped by a raid member
};

enum AranSpells
{
    SPELL_FROSTBOLT        = 29954,
    SPELL_FIREBALL         = 29953,
    SPELL_ARCMISSLE        = 29955,
    SPELL_CHAINSOFICE      = 29991,
    SPELL_DRAGONSBREATH    = 29964,   //Not used since 2.1.0
    SPELL_MASSSLOW         = 30035,
    SPELL_FLAME_WREATH     = 30004, // triggers 29946 on targets
    SPELL_SUMMON_BLIZZ     = 29969, // script target on npc 17161 - triggers spell 29952 on target
    SPELL_AOE_CS           = 29961,
    SPELL_PLAYERPULL       = 32265,
    SPELL_AEXPLOSION       = 29973,
    SPELL_MASS_POLY        = 29963,
    SPELL_BLINK_CENTER     = 29967,
    SPELL_ELEMENTALS       = 29962,
    SPELL_CONJURE          = 29975,
    SPELL_DRINK            = 30024,
    SPELL_POTION           = 32453,
    SPELL_AOE_PYROBLAST    = 29978,
    SPELL_ARCANE_BUBBLE    = 6724
};

enum MiscSpells
{
    SPELL_CIRCULAR_BLIZZARD    = 29952,
    SPELL_WATERBOLT            = 31012,
    SPELL_SHADOW_PYRO          = 29978
};

//Creatures
enum Creatures
{
    NPC_WATER_ELEMENTAL   = 17167,
    NPC_SHADOW_OF_ARAN    = 18254,
    NPC_ARAN_BLIZZARD     = 17161
};

enum SuperSpell
{
    SUPER_FLAME = 0,
    SUPER_BLIZZARD,
    SUPER_AE,
};

enum Items
{
    ITEM_ATIESH_MAGE            = 22589,
    ITEM_ATIESH_WARLOCK         = 22630,
    ITEM_ATIESH_PRIEST          = 22631,
    ITEM_ATIESH_DRUID           = 22632
};

uint32 const AtieshStaves[4] =
{
    ITEM_ATIESH_MAGE,
    ITEM_ATIESH_WARLOCK,
    ITEM_ATIESH_PRIEST,
    ITEM_ATIESH_DRUID,
};

struct boss_aranAI : public ScriptedAI
{
    boss_aranAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }

    ScriptedInstance* pInstance;

    uint32 SecondarySpellTimer;
    uint32 NormalCastTimer;
    uint32 SuperCastTimer;
    uint32 BerserkTimer;
    uint32 CloseDoorTimer;                                  // Don't close the door right on aggro in case some people are still entering.

    uint8 LastSuperSpell;

    uint32 FlameWreathTimer;
    uint32 FlameWreathCheckTime;
    uint64 FlameWreathTarget[3];
    float FWTargPosX[3];
    float FWTargPosY[3];

    uint32 CurrentNormalSpell;
    uint32 ArcaneCooldown;
    uint32 FireCooldown;
    uint32 FrostCooldown;

    uint32 DragonsbreathCooldown;
    uint32 AETimer;
    uint32 Wait_Timer;

    uint32 DrinkInturruptTimer;

    bool ElementalsSpawned;
    bool Drinking;
    bool DrinkInturrupted;
    bool CastAE;
    bool SeenAtiesh;

    float x_cord;
    float y_cord;
    float z_cord;

    void Reset()
    {
        SecondarySpellTimer = 5000;
        NormalCastTimer = 0;
        SuperCastTimer = 35000;
        BerserkTimer = 720000;
        CloseDoorTimer = 15000;

        LastSuperSpell = rand() % 3;

        FlameWreathTimer = 0;
        FlameWreathCheckTime = 0;

        CurrentNormalSpell = 0;
        ArcaneCooldown = 0;
        FireCooldown = 0;
        FrostCooldown = 0;
        DragonsbreathCooldown = 0;
        DrinkInturruptTimer = 10000;

        ElementalsSpawned = false;
        Drinking = false;
        DrinkInturrupted = false;

        CastAE = false;
        AETimer = 2000;

        me->ApplySpellImmune(0, IMMUNITY_ID, 11719, true);
        me->ApplySpellImmune(0, IMMUNITY_ID, 1714, true);

        me->ApplySpellImmune(0, IMMUNITY_ID, 27282, true);
        me->ApplySpellImmune(0, IMMUNITY_ID, 26892, true);
        me->ApplySpellImmune(0, IMMUNITY_ID, 26786, true);
        me->ApplySpellImmune(0, IMMUNITY_ID, 27283, true);
        me->ApplySpellImmune(0, IMMUNITY_ID, 26969, true);
        me->ApplySpellImmune(0, IMMUNITY_ID, 25347, true);
        me->ApplySpellImmune(0, IMMUNITY_ID, 11343, true);
        me->ApplySpellImmune(0, IMMUNITY_ID, 13230, true);
        me->ApplySpellImmune(0, IMMUNITY_ID, 11358, true);
        me->ApplySpellImmune(0, IMMUNITY_ID, 11400, true);
        me->ApplySpellImmune(0, IMMUNITY_ID, 11342, true);
        me->ApplySpellImmune(0, IMMUNITY_ID, 3421, true);
        me->ApplySpellImmune(0, IMMUNITY_ID, 13229, true);
        me->ApplySpellImmune(0, IMMUNITY_ID, 11357, true);
        me->ApplySpellImmune(0, IMMUNITY_ID, 11341, true);
        me->ApplySpellImmune(0, IMMUNITY_ID, 13228, true);
        me->ApplySpellImmune(0, IMMUNITY_ID, 8694, true);
        me->ApplySpellImmune(0, IMMUNITY_ID, 2837, true);
        me->ApplySpellImmune(0, IMMUNITY_ID, 8691, true);
        me->ApplySpellImmune(0, IMMUNITY_ID, 13220, true);
        me->ApplySpellImmune(0, IMMUNITY_ID, 2835, true);
        me->ApplySpellImmune(0, IMMUNITY_ID, 8687, true);
        me->ApplySpellImmune(0, IMMUNITY_ID, 5763, true);
        me->ApplySpellImmune(0, IMMUNITY_ID, 8681, true);
        me->ApplySpellImmune(0, IMMUNITY_ID, 3420, true);

        if (pInstance)
        {
            // Not in progress
            pInstance->SetData(TYPE_ARAN, NOT_STARTED);
            pInstance->HandleGameObject(pInstance->GetData64(DATA_GO_LIBRARY_DOOR), true);
        }
    }

    void KilledUnit(Unit* /*victim*/)
    {
        DoScriptText(RAND(SAY_KILL1, SAY_KILL2), me);
    }

    void JustDied(Unit* /*victim*/)
    {
        DoScriptText(SAY_DEATH, me);

        if (pInstance)
        {
            pInstance->SetData(TYPE_ARAN, DONE);
            pInstance->HandleGameObject(pInstance->GetData64(DATA_GO_LIBRARY_DOOR), true);
        }
    }

    void EnterCombat(Unit* /*who*/)
    {
        DoScriptText(RAND(SAY_AGGRO1, SAY_AGGRO2, SAY_AGGRO3), me);

        if (pInstance)
        {
            pInstance->SetData(TYPE_ARAN, IN_PROGRESS);
            pInstance->HandleGameObject(pInstance->GetData64(DATA_GO_LIBRARY_DOOR), false);
        }
    }

    void FlameWreathEffect()
    {
        std::vector<Unit*> targets;
        std::list<HostileReference*> t_list = me->getThreatManager().getThreatList();

        if (!t_list.size())
            return;

        //store the threat list in a different container
        for (std::list<HostileReference*>::const_iterator itr = t_list.begin(); itr != t_list.end(); ++itr)
        {
            Unit* pTarget = Unit::GetUnit(*me, (*itr)->getUnitGuid());
            //only on alive players
            if (pTarget && pTarget->IsAlive() && pTarget->GetTypeId() == TYPEID_PLAYER)
                targets.push_back(pTarget);
        }

        //cut down to size if we have more than 3 targets
        while (targets.size() > 3)
            targets.erase(targets.begin() + rand() % targets.size());

        uint32 i = 0;
        for (std::vector<Unit*>::const_iterator itr = targets.begin(); itr != targets.end(); ++itr)
        {
            if (*itr)
            {
                FlameWreathTarget[i] = (*itr)->GetGUID();
                FWTargPosX[i] = (*itr)->GetPositionX();
                FWTargPosY[i] = (*itr)->GetPositionY();
                me->CastSpell((*itr), SPELL_FLAME_WREATH, true);
                ++i;
            }
        }
    }

    void SetImmune(bool apply)
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_INTERRUPT_CAST, apply);                                                
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, apply); 
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (CloseDoorTimer)
        {
            if (CloseDoorTimer <= diff)
            {
                if (pInstance)
                {
                    pInstance->HandleGameObject(pInstance->GetData64(DATA_GO_LIBRARY_DOOR), false);
                    CloseDoorTimer = 0;
                }
            }
            else CloseDoorTimer -= diff;
        }

        //Cooldowns for casts
        if (ArcaneCooldown)
        {
            if (ArcaneCooldown >= diff)
                ArcaneCooldown -= diff;
            else ArcaneCooldown = 2000;
        }

        if (FireCooldown)
        {
            if (FireCooldown >= diff)
                FireCooldown -= diff;
            else FireCooldown = 2000;
        }

        if (FrostCooldown)
        {
            if (FrostCooldown >= diff)
                FrostCooldown -= diff;
            else FrostCooldown = 2000;
        }

        if (!Drinking && me->GetMaxPower(POWER_MANA) && (me->GetPower(POWER_MANA) * 100 / me->GetMaxPower(POWER_MANA)) < 20)
        {
            Drinking = true;
            me->InterruptNonMeleeSpells(false);

            DoScriptText(SAY_DRINK, me);

            if (!DrinkInturrupted)
            {
                DoCast(me, SPELL_MASS_POLY, true);
                DoCast(me, SPELL_CONJURE, false);
                DoCast(me, SPELL_DRINK, false);
                me->SetStandState(UNIT_STAND_STATE_SIT);
                DrinkInturruptTimer = 10000;
            }
        }

        //Drink Inturrupt
        if (Drinking && DrinkInturrupted)
        {
            Drinking = false;
            me->RemoveAurasDueToSpell(SPELL_DRINK);
            me->SetStandState(UNIT_STAND_STATE_STAND);
            me->SetPower(POWER_MANA, me->GetMaxPower(POWER_MANA) - 32000);
            DoCast(me, SPELL_POTION, false);
        }

        //Drink Inturrupt Timer
        if (Drinking && !DrinkInturrupted)
            if (DrinkInturruptTimer >= diff)
                DrinkInturruptTimer -= diff;
            else
            {
                me->SetStandState(UNIT_STAND_STATE_STAND);
                DoCast(me, SPELL_POTION, true);
                DoCast(me, SPELL_AOE_PYROBLAST, false);
                DrinkInturrupted = true;
                Drinking = false;
            }

        //Don't execute any more code if we are drinking
        if (Drinking)
            return;

        //Normal casts
        if (NormalCastTimer <= diff)
        {
            if (!me->IsNonMeleeSpellCast(false))
            {
                Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true);
                if (!target)
                    return;

                uint32 Spells[3];
                uint8 AvailableSpells = 0;

                //Check for what spells are not on cooldown
                if (!ArcaneCooldown)
                {
                    Spells[AvailableSpells] = SPELL_ARCMISSLE;
                    ++AvailableSpells;
                }
                if (!FireCooldown)
                {
                    Spells[AvailableSpells] = SPELL_FIREBALL;
                    ++AvailableSpells;
                }
                if (!FrostCooldown)
                {
                    Spells[AvailableSpells] = SPELL_FROSTBOLT;
                    ++AvailableSpells;
                }

                //If no available spells wait 1 second and try again
                if (AvailableSpells)
                {
                    CurrentNormalSpell = Spells[rand() % AvailableSpells];
                    SetImmune(false);
                    DoCast(target, CurrentNormalSpell);
                }
            }
            NormalCastTimer = 2500;
        }
        else NormalCastTimer -= diff;

        if (SecondarySpellTimer <= diff)
        {
            switch (urand(0, 1))
            {
            case 0:
                DoCast(me, SPELL_AOE_CS);
                break;
            case 1:
                if (Unit* pUnit = SelectUnit(SELECT_TARGET_RANDOM, 0))
                    DoCast(pUnit, SPELL_CHAINSOFICE);
                break;
            case 2:
                if (Unit* pUnit = SelectUnit(SELECT_TARGET_RANDOM, 0))
                    DoCast(pUnit, SPELL_DRAGONSBREATH);
                break;
            }
            SecondarySpellTimer = urand(3000, 8000);
        }
        else SecondarySpellTimer -= diff;

        if (SuperCastTimer <= diff)
        {
            uint8 Available[2];

            switch (LastSuperSpell)
            {
                case SUPER_AE:
                    Available[0] = SUPER_FLAME;
                    Available[1] = SUPER_BLIZZARD;
                    break;
                case SUPER_FLAME:
                    Available[0] = SUPER_AE;
                    Available[1] = SUPER_BLIZZARD;
                    break;
                case SUPER_BLIZZARD:
                    Available[0] = SUPER_FLAME;
                    Available[1] = SUPER_AE;
                    break;
            }

            LastSuperSpell = Available[urand(0, 1)];
            SetImmune(true);
 
            Map::PlayerList const &PlayerList = pInstance->instance->GetPlayers();
            WorldPacket data;
            switch (LastSuperSpell)
            {
            case SUPER_AE:
                DoScriptText(RAND(SAY_EXPLOSION1, SAY_EXPLOSION2), me);

                DoCast(me, SPELL_BLINK_CENTER, true);
                DoCast(me, SPELL_PLAYERPULL, true);
                DoCast(me, SPELL_MASSSLOW, true);
                DoCast(me, SPELL_ARCANE_BUBBLE, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_INTERRUPT_CAST, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, true);
                AETimer = 1500;
                CastAE = true;
                break;

            case SUPER_FLAME:
                DoScriptText(RAND(SAY_FLAMEWREATH1, SAY_FLAMEWREATH2), me);

                FlameWreathTimer = 20000;
                FlameWreathCheckTime = 500;

                FlameWreathTarget[0] = 0;
                FlameWreathTarget[1] = 0;
                FlameWreathTarget[2] = 0;

                FlameWreathEffect();
                SecondarySpellTimer = 20500;
                break;

            case SUPER_BLIZZARD:
                DoScriptText(RAND(SAY_BLIZZARD1, SAY_BLIZZARD2), me);

                Creature* Blizzard = NULL;
                Blizzard = me->SummonCreature(NPC_ARAN_BLIZZARD, -11179.080f, -1905.279f, 232.008f, 2.9f, TEMPSUMMON_TIMED_DESPAWN, 25000);
                if (Blizzard)
                {
                    Blizzard->SetInCombatWithZone();
                    Blizzard->SetFaction(me->GetFaction());
                    me->CastSpell(Blizzard, SPELL_SUMMON_BLIZZ, false);
                    Blizzard->CastSpell(Blizzard, SPELL_CIRCULAR_BLIZZARD, false);
                    Blizzard->GetMotionMaster()->MovePath(110110101, false);
                }
                break;
            }

            SuperCastTimer = urand(35000, 40000);
        }
        else SuperCastTimer -= diff;

        if (!ElementalsSpawned && HealthBelowPct(40))
        {
            ElementalsSpawned = true;

            Creature* ElementalOne   = NULL;
            Creature* ElementalTwo   = NULL;
            Creature* ElementalThree = NULL;
            Creature* ElementalFour  = NULL;

            ElementalOne     = me->SummonCreature(NPC_WATER_ELEMENTAL, -11168.1f, -1939.29f, 232.092f, 1.46f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 90000);
            ElementalTwo     = me->SummonCreature(NPC_WATER_ELEMENTAL, -11138.2f, -1915.38f, 232.092f, 3.00f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 90000);
            ElementalThree   = me->SummonCreature(NPC_WATER_ELEMENTAL, -11161.7f, -1885.36f, 232.092f, 4.59f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 90000);
            ElementalFour    = me->SummonCreature(NPC_WATER_ELEMENTAL, -11192.4f, -1909.36f, 232.092f, 6.19f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 90000);

            if (ElementalOne)
            {
                Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);
                if (!pTarget)
                    return;

                DoStartNoMovement(pTarget);
                ElementalOne->SetInCombatWithZone();
                ElementalOne->CombatStart(pTarget);
                ElementalOne->SetFaction(me->GetFaction());
                ElementalOne->SetRooted(true);
                ElementalOne->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FROST, true);
                ElementalOne->SetStatFlatModifier(UNIT_MOD_RESISTANCE_FROST,  BASE_VALUE, 0);
            }

            if (ElementalTwo)
            {
                Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);
                if (!pTarget)
                    return;

                DoStartNoMovement(pTarget);
                ElementalTwo->SetInCombatWithZone();
                ElementalTwo->CombatStart(pTarget);
                ElementalTwo->SetFaction(me->GetFaction());
                ElementalTwo->SetRooted(true);
                ElementalTwo->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FROST, true);
                ElementalTwo->SetStatFlatModifier(UNIT_MOD_RESISTANCE_FROST,  BASE_VALUE, 0);
            }

            if (ElementalThree)
            {
                Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);
                if (!pTarget)
                    return;

                DoStartNoMovement(pTarget);
                ElementalThree->SetInCombatWithZone();
                ElementalThree->CombatStart(pTarget);
                ElementalThree->SetFaction(me->GetFaction());
                ElementalThree->SetRooted(true);
                ElementalThree->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FROST, true);
                ElementalThree->SetStatFlatModifier(UNIT_MOD_RESISTANCE_FROST,  BASE_VALUE, 0);
            }

            if (ElementalFour)
            {
                Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);
                if (!pTarget)
                    return;

                DoStartNoMovement(pTarget);
                ElementalFour->SetInCombatWithZone();
                ElementalFour->CombatStart(pTarget);
                ElementalFour->SetFaction(me->GetFaction());
                ElementalFour->SetRooted(true);
                ElementalFour->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FROST, true);
                ElementalFour->SetStatFlatModifier(UNIT_MOD_RESISTANCE_FROST,  BASE_VALUE, 0);
            }

            DoScriptText(SAY_ELEMENTALS, me);
            DoScriptText(SAY_TIMEOVER, me);

            BerserkTimer = 60000;
        }
        else BerserkTimer -= diff;

        //Flame Wreath check
        if (FlameWreathTimer)
        {
            if (FlameWreathTimer >= diff)
                FlameWreathTimer -= diff;
            else FlameWreathTimer = 0;

            if (FlameWreathCheckTime <= diff)
            {
                for (uint8 i = 0; i < 3; ++i)
                {
                    if (!FlameWreathTarget[i])
                        continue;

                    Unit* unit = Unit::GetUnit(*me, FlameWreathTarget[i]);
                    if (unit && !unit->IsWithinDist2d(FWTargPosX[i], FWTargPosY[i], 3))
                    {
                        unit->CastSpell(unit, 20476, true, 0, 0, me->GetGUID());
                        unit->CastSpell(unit, 11027, true);
                        FlameWreathTarget[i] = 0;
                    }
                }
                FlameWreathCheckTime = 500;
            }
            else FlameWreathCheckTime -= diff;
        }

        if (ArcaneCooldown && FireCooldown && FrostCooldown && DragonsbreathCooldown)
            DoMeleeAttackIfReady();

        if (CastAE)
            if (AETimer < diff)
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_INTERRUPT_CAST, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, true);
                me->CastSpell(me, SPELL_AEXPLOSION, false);
                AETimer = 1500;
                CastAE = false;
            }
            else AETimer -= diff;
    }

    void DamageTaken(Unit* /*pAttacker*/, uint32& damage)
    {
        if (!DrinkInturrupted && Drinking && damage)
            DrinkInturrupted = true;
    }

    void SpellHit(Unit* /*pAttacker*/, const SpellEntry* Spell)
    {
        //We only care about inturrupt effects and only if they are durring a spell currently being casted
        if ((Spell->Effect[0] != SPELL_EFFECT_INTERRUPT_CAST &&
             Spell->Effect[1] != SPELL_EFFECT_INTERRUPT_CAST &&
             Spell->Effect[2] != SPELL_EFFECT_INTERRUPT_CAST) || !me->IsNonMeleeSpellCast(false))
            return;

        //Inturrupt effect
        me->InterruptNonMeleeSpells(false);

        //Normally we would set the cooldown equal to the spell duration
        //but we do not have access to the DurationStore

        switch (CurrentNormalSpell)
        {
            case SPELL_ARCMISSLE: ArcaneCooldown = 3500; break;
            case SPELL_FIREBALL: FireCooldown = 3500; break;
            case SPELL_FROSTBOLT: FrostCooldown = 3500; break;
        }
    }

    void MoveInLineOfSight(Unit* who)
    {
        ScriptedAI::MoveInLineOfSight(who);

        if (SeenAtiesh || me->IsInCombat() || me->GetDistance2d(who) > me->GetAttackDistance(who) + 10.0f)
            return;

        Player* player = who->ToPlayer();
        if (!player)
            return;

        for (uint32 id : AtieshStaves)
        {
            if (!PlayerHasWeaponEquipped(player, id))
                continue;

            SeenAtiesh = true;
            DoScriptText(SAY_ATIESH, me);
            me->SetFacingTo(me->GetAngle(player));
            me->ClearUnitState(UNIT_STATE_MOVING);
            me->GetMotionMaster()->MoveDistract(7 * IN_MILLISECONDS);
            break;
        }
    }

    private:
        bool PlayerHasWeaponEquipped(Player* player, uint32 itemEntry)
        {
            Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
            if (item && item->GetEntry() == itemEntry)
                return true;

            return false;
        }
};

struct water_elementalAI : public ScriptedAI
{
    water_elementalAI(Creature* c) : ScriptedAI(c) {}

    uint32 CastTimer;

    void Reset()
    {
        CastTimer = 2000 + (rand() % 3000);
    }

    void EnterCombat(Unit* /*who*/) {}

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (CastTimer <= diff)
        {
            DoCastVictim( SPELL_WATERBOLT);
            CastTimer = urand(2000, 5000);
        }
        else CastTimer -= diff;
    }
};

CreatureAI* GetAI_boss_aran(Creature* pCreature)
{
    return new boss_aranAI (pCreature);
}

CreatureAI* GetAI_water_elemental(Creature* pCreature)
{
    return new water_elementalAI (pCreature);
}

// CONVERT TO ACID
CreatureAI* GetAI_shadow_of_aran(Creature* pCreature)
{
    outstring_log("OSCR: Convert simpleAI script for Creature Entry %u to ACID", pCreature->GetEntry());
    SimpleAI* ai = new SimpleAI (pCreature);

    ai->Spell[0].Enabled = true;
    ai->Spell[0].Spell_Id = SPELL_SHADOW_PYRO;
    ai->Spell[0].Cooldown = 5000;
    ai->Spell[0].First_Cast = 1000;
    ai->Spell[0].Cast_Target_Type = CAST_HOSTILE_TARGET;

    ai->EnterEvadeMode();

    return ai;
}

void AddSC_boss_shade_of_aran()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "boss_shade_of_aran";
    newscript->GetAI = &GetAI_boss_aran;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_shadow_of_aran";
    newscript->GetAI = &GetAI_shadow_of_aran;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_aran_elemental";
    newscript->GetAI = &GetAI_water_elemental;
    newscript->RegisterSelf();
}
