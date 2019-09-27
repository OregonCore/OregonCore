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

#include "Unit.h"
#include "SpellMgr.h"
#include "ObjectMgr.h"
#include "SpellAuraDefines.h"
#include "DBCStores.h"
#include "Chat.h"
#include "Spell.h"
#include "CreatureAI.h"
#include "BattlegroundMgr.h"

bool IsAreaEffectTarget[TOTAL_SPELL_TARGETS];

SpellMgr::SpellMgr()
{
    for (int i = 0; i < TOTAL_SPELL_EFFECTS; ++i)
    {
        switch (i)
        {
        case SPELL_EFFECT_PERSISTENT_AREA_AURA: //27
        case SPELL_EFFECT_SUMMON:               //28
        case SPELL_EFFECT_TRIGGER_MISSILE:      //32
        case SPELL_EFFECT_TRANS_DOOR:           //50 summon object
        case SPELL_EFFECT_SUMMON_PET:           //56
        case SPELL_EFFECT_ADD_FARSIGHT:         //72
        case SPELL_EFFECT_SUMMON_OBJECT_WILD:   //76
        case SPELL_EFFECT_SUMMON_OBJECT_SLOT1:  //104
        case SPELL_EFFECT_SUMMON_OBJECT_SLOT2:  //105
        case SPELL_EFFECT_SUMMON_OBJECT_SLOT3:  //106
        case SPELL_EFFECT_SUMMON_OBJECT_SLOT4:  //107
        case SPELL_EFFECT_SUMMON_DEAD_PET:      //109
        case SPELL_EFFECT_TRIGGER_SPELL_2:      //151 ritual of summon
            EffectTargetType[i] = SPELL_REQUIRE_DEST;
            break;
        case SPELL_EFFECT_PARRY: // 0
        case SPELL_EFFECT_BLOCK: // 0
        case SPELL_EFFECT_SKILL: // always with dummy 3 as A
        //case SPELL_EFFECT_LEARN_SPELL: // 0 may be 5 pet
        case SPELL_EFFECT_TRADE_SKILL: // 0 or 1
        case SPELL_EFFECT_PROFICIENCY: // 0
            EffectTargetType[i] = SPELL_REQUIRE_NONE;
            break;
        case SPELL_EFFECT_ENCHANT_ITEM:
        case SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY:
        case SPELL_EFFECT_DISENCHANT:
        case SPELL_EFFECT_FEED_PET:
        case SPELL_EFFECT_PROSPECTING:
            EffectTargetType[i] = SPELL_REQUIRE_ITEM;
            break;
        //caster must be pushed otherwise no sound
        case SPELL_EFFECT_APPLY_AREA_AURA_PARTY:
        case SPELL_EFFECT_APPLY_AREA_AURA_FRIEND:
        case SPELL_EFFECT_APPLY_AREA_AURA_ENEMY:
        case SPELL_EFFECT_APPLY_AREA_AURA_PET:
        case SPELL_EFFECT_APPLY_AREA_AURA_OWNER:
            EffectTargetType[i] = SPELL_REQUIRE_CASTER;
            break;
        default:
            EffectTargetType[i] = SPELL_REQUIRE_UNIT;
            break;
        }
    }

    for (int i = 0; i < TOTAL_SPELL_TARGETS; ++i)
    {
        switch (i)
        {
        case TARGET_UNIT_CASTER:
        case TARGET_UNIT_CASTER_FISHING:
        case TARGET_UNIT_MASTER:
        case TARGET_UNIT_PET:
        case TARGET_UNIT_PARTY_CASTER:
        case TARGET_UNIT_RAID_CASTER:
            SpellTargetType[i] = TARGET_TYPE_UNIT_CASTER;
            break;
        case TARGET_UNIT_TARGET_MINIPET:
        case TARGET_UNIT_TARGET_ALLY:
        case TARGET_UNIT_TARGET_RAID:
        case TARGET_UNIT_TARGET_ANY:
        case TARGET_UNIT_TARGET_ENEMY:
        case TARGET_UNIT_TARGET_PARTY:
        case TARGET_UNIT_PARTY_TARGET:
        case TARGET_UNIT_CLASS_TARGET:
        case TARGET_UNIT_CHAINHEAL:
            SpellTargetType[i] = TARGET_TYPE_UNIT_TARGET;
            break;
        case TARGET_UNIT_NEARBY_ENEMY:
        case TARGET_UNIT_NEARBY_PARTY:
        case TARGET_UNIT_NEARBY_ALLY:
        case TARGET_UNIT_NEARBY_ENTRY:
        case TARGET_UNIT_NEARBY_RAID:
            SpellTargetType[i] = TARGET_TYPE_UNIT_NEARBY;
            break;
        case TARGET_UNIT_AREA_ENEMY_SRC:
        case TARGET_UNIT_AREA_ALLY_SRC:
        case TARGET_UNIT_AREA_ENTRY_SRC:
        case TARGET_UNIT_AREA_PARTY_SRC:
        case TARGET_GAMEOBJECT_SRC_AREA:
            SpellTargetType[i] = TARGET_TYPE_AREA_SRC;
            break;
        case TARGET_UNIT_AREA_ENEMY_DST:
        case TARGET_UNIT_AREA_ALLY_DST:
        case TARGET_UNIT_AREA_ENTRY_DST:
        case TARGET_UNIT_AREA_PARTY_DST:
        case TARGET_GAMEOBJECT_DEST_AREA:
            SpellTargetType[i] = TARGET_TYPE_AREA_DST;
            break;
        case TARGET_UNIT_CONE_ENEMY:
        case TARGET_UNIT_CONE_ALLY:
        case TARGET_UNIT_CONE_ENTRY:
        case TARGET_UNIT_CONE_ENEMY_UNKNOWN:
            SpellTargetType[i] = TARGET_TYPE_AREA_CONE;
            break;
        case TARGET_DST_CASTER:
        case TARGET_SRC_CASTER:
        case TARGET_MINION:
        case TARGET_DEST_CASTER_FRONT_LEAP:
        case TARGET_DEST_CASTER_FRONT:
        case TARGET_DEST_CASTER_BACK:
        case TARGET_DEST_CASTER_RIGHT:
        case TARGET_DEST_CASTER_LEFT:
        case TARGET_DEST_CASTER_FRONT_LEFT:
        case TARGET_DEST_CASTER_BACK_LEFT:
        case TARGET_DEST_CASTER_BACK_RIGHT:
        case TARGET_DEST_CASTER_FRONT_RIGHT:
        case TARGET_DEST_CASTER_RANDOM:
        case TARGET_DEST_CASTER_RADIUS:
            SpellTargetType[i] = TARGET_TYPE_DEST_CASTER;
            break;
        case TARGET_DST_TARGET_ENEMY:
        case TARGET_DEST_TARGET_ANY:
        case TARGET_DEST_TARGET_FRONT:
        case TARGET_DEST_TARGET_BACK:
        case TARGET_DEST_TARGET_RIGHT:
        case TARGET_DEST_TARGET_LEFT:
        case TARGET_DEST_TARGET_FRONT_LEFT:
        case TARGET_DEST_TARGET_BACK_LEFT:
        case TARGET_DEST_TARGET_BACK_RIGHT:
        case TARGET_DEST_TARGET_FRONT_RIGHT:
        case TARGET_DEST_TARGET_RANDOM:
        case TARGET_DEST_TARGET_RADIUS:
            SpellTargetType[i] = TARGET_TYPE_DEST_TARGET;
            break;
        case TARGET_DEST_DYNOBJ_ENEMY:
        case TARGET_DEST_DYNOBJ_ALLY:
        case TARGET_DEST_DYNOBJ_NONE:
        case TARGET_DEST_DEST:
        case TARGET_DEST_TRAJ:
        case TARGET_DEST_DEST_FRONT_LEFT:
        case TARGET_DEST_DEST_BACK_LEFT:
        case TARGET_DEST_DEST_BACK_RIGHT:
        case TARGET_DEST_DEST_FRONT_RIGHT:
        case TARGET_DEST_DEST_FRONT:
        case TARGET_DEST_DEST_BACK:
        case TARGET_DEST_DEST_RIGHT:
        case TARGET_DEST_DEST_LEFT:
        case TARGET_DEST_DEST_RANDOM:
            SpellTargetType[i] = TARGET_TYPE_DEST_DEST;
            break;
        case TARGET_DST_DB:
        case TARGET_DST_HOME:
        case TARGET_DST_NEARBY_ENTRY:
            SpellTargetType[i] = TARGET_TYPE_DEST_SPECIAL;
            break;
        case TARGET_UNIT_CHANNEL:
        case TARGET_DEST_CHANNEL:
            SpellTargetType[i] = TARGET_TYPE_CHANNEL;
            break;
        default:
            SpellTargetType[i] = TARGET_TYPE_DEFAULT;
        }
    }

    for (int i = 0; i < TOTAL_SPELL_TARGETS; ++i)
    {
        switch (i)
        {
        case TARGET_UNIT_AREA_ENEMY_DST:
        case TARGET_UNIT_AREA_ENEMY_SRC:
        case TARGET_UNIT_AREA_ALLY_DST:
        case TARGET_UNIT_AREA_ALLY_SRC:
        case TARGET_UNIT_AREA_ENTRY_DST:
        case TARGET_UNIT_AREA_ENTRY_SRC:
        case TARGET_UNIT_AREA_PARTY_DST:
        case TARGET_UNIT_AREA_PARTY_SRC:
        case TARGET_UNIT_PARTY_TARGET:
        case TARGET_UNIT_PARTY_CASTER:
        case TARGET_UNIT_CONE_ENEMY:
        case TARGET_UNIT_CONE_ALLY:
        case TARGET_UNIT_CONE_ENEMY_UNKNOWN:
        case TARGET_UNIT_RAID_CASTER:
            IsAreaEffectTarget[i] = true;
            break;
        default:
            IsAreaEffectTarget[i] = false;
            break;
        }
    }
}

SpellMgr::~SpellMgr()
{
    CreatureAI::ClearAISpellInfo();
}

SpellMgr& SpellMgr::Instance()
{
    static SpellMgr spellMgr;
    return spellMgr;
}

int32 GetSpellDuration(SpellEntry const* spellInfo)
{
    if (!spellInfo)
        return 0;
    SpellDurationEntry const* du = sSpellDurationStore.LookupEntry(spellInfo->DurationIndex);
    if (!du)
        return 0;
    return (du->Duration[0] == -1) ? -1 : abs(du->Duration[0]);
}

int32 GetSpellMaxDuration(SpellEntry const* spellInfo)
{
    if (!spellInfo)
        return 0;
    SpellDurationEntry const* du = sSpellDurationStore.LookupEntry(spellInfo->DurationIndex);
    if (!du)
        return 0;
    return (du->Duration[2] == -1) ? -1 : abs(du->Duration[2]);
}

uint32 GetSpellCastTime(SpellEntry const* spellInfo, Spell const* spell)
{
    SpellCastTimesEntry const* spellCastTimeEntry = sSpellCastTimesStore.LookupEntry(spellInfo->CastingTimeIndex);

    // not all spells have cast time index and this is all is pasiive abilities
    if (!spellCastTimeEntry)
        return 0;

    int32 castTime = spellCastTimeEntry->CastTime;

    if (spell)
    {
        if (Player* modOwner = spell->GetCaster()->GetSpellModOwner())
            modOwner->ApplySpellMod(spellInfo->Id, SPELLMOD_CASTING_TIME, castTime, spell);

        if (!(spellInfo->Attributes & (SPELL_ATTR0_ABILITY | SPELL_ATTR0_TRADESPELL)))
            castTime = int32(castTime * spell->GetCaster()->GetFloatValue(UNIT_MOD_CAST_SPEED));
        else
        {
            if (spell->IsRangedSpell() && !spell->IsAutoRepeat())
                castTime = int32(castTime * spell->GetCaster()->m_modAttackSpeedPct[RANGED_ATTACK]);
        }
    }

    if (spellInfo->Attributes & SPELL_ATTR0_RANGED && (!spell || !spell->IsAutoRepeat()))
        castTime += 500;

    return (castTime > 0) ? uint32(castTime) : 0;
}

bool IsPassiveSpell(uint32 spellId)
{
    SpellEntry const* spellInfo = sSpellStore.LookupEntry(spellId);
    if (!spellInfo)
        return false;
    return IsPassiveSpell(spellInfo);
}

bool IsPassiveSpell(SpellEntry const* spellInfo)
{
   return (spellInfo->Attributes & SPELL_ATTR0_PASSIVE) != 0;
}

bool IsAutocastableSpell(uint32 spellId)
{
    SpellEntry const* spellInfo = sSpellStore.LookupEntry(spellId);
    if (!spellInfo)
        return false;
    if (spellInfo->Attributes & SPELL_ATTR0_PASSIVE)
        return false;
    if (spellInfo->AttributesEx & SPELL_ATTR1_UNAUTOCASTABLE_BY_PET)
        return false;
    return true;
}

uint32 CalculatePowerCost(SpellEntry const* spellInfo, Unit const* caster, SpellSchoolMask schoolMask)
{
    // Spell drain all exist power on cast (Only paladin lay of Hands)
    if (spellInfo->AttributesEx & SPELL_ATTR1_DRAIN_ALL_POWER)
    {
        // If power type - health drain all
        if (spellInfo->powerType == POWER_HEALTH)
            return caster->GetHealth();
        // Else drain all power
        if (spellInfo->powerType < MAX_POWERS)
            return caster->GetPower(Powers(spellInfo->powerType));
        sLog.outError("CalculateManaCost: Unknown power type '%d' in spell %d", spellInfo->powerType, spellInfo->Id);
        return 0;
    }

    // Base powerCost
    int32 powerCost = spellInfo->manaCost;
    // PCT cost from total amount
    if (spellInfo->ManaCostPercentage)
    {
        switch (spellInfo->powerType)
        {
        // health as power used
        case POWER_HEALTH:
            powerCost += spellInfo->ManaCostPercentage * caster->GetCreateHealth() / 100;
            break;
        case POWER_MANA:
            powerCost += spellInfo->ManaCostPercentage * caster->GetCreateMana() / 100;
            break;
        case POWER_RAGE:
        case POWER_FOCUS:
        case POWER_ENERGY:
        case POWER_HAPPINESS:
            powerCost += spellInfo->ManaCostPercentage * caster->GetMaxPower(Powers(spellInfo->powerType)) / 100;
            break;
        default:
            sLog.outError("CalculateManaCost: Unknown power type '%d' in spell %d", spellInfo->powerType, spellInfo->Id);
            return 0;
        }
    }
    SpellSchools school = GetFirstSchoolInMask(schoolMask);
    // Flat mod from caster auras by spell school
    powerCost += caster->GetInt32Value(UNIT_FIELD_POWER_COST_MODIFIER + school);
    // Shiv - costs 20 + weaponSpeed*10 energy (apply only to non-triggered spell with energy cost)
    if (spellInfo->AttributesEx4 & SPELL_ATTR4_SPELL_VS_EXTEND_COST)
        powerCost += caster->GetAttackTime(OFF_ATTACK) / 100;
    // Apply cost mod by spell
    if (Player* modOwner = caster->GetSpellModOwner())
        modOwner->ApplySpellMod(spellInfo->Id, SPELLMOD_COST, powerCost);

    if (!caster->IsControlledByPlayer() && spellInfo->Attributes & SPELL_ATTR0_LEVEL_DAMAGE_CALCULATION)
    {
        GtNPCManaCostScalerEntry const* spellScaler = sGtNPCManaCostScalerStore.LookupEntry(spellInfo->spellLevel - 1);
        GtNPCManaCostScalerEntry const* casterScaler = sGtNPCManaCostScalerStore.LookupEntry(caster->getLevel() - 1);
        if (spellScaler && casterScaler)
            powerCost *= casterScaler->ratio / spellScaler->ratio;
    }

    // PCT mod from user auras by school
    powerCost = int32(powerCost * (1.0f + caster->GetFloatValue(UNIT_FIELD_POWER_COST_MULTIPLIER + school)));
    if (powerCost < 0)
        powerCost = 0;
    return powerCost;
}

/*bool IsNoStackAuraDueToAura(uint32 spellId_1, uint32 effIndex_1, uint32 spellId_2, uint32 effIndex_2)
{
    SpellEntry const *spellInfo_1 = sSpellStore.LookupEntry(spellId_1);
    SpellEntry const *spellInfo_2 = sSpellStore.LookupEntry(spellId_2);
    if (!spellInfo_1 || !spellInfo_2) return false;
    if (spellInfo_1->Id == spellId_2) return false;

    if (spellInfo_1->Effect[effIndex_1] != spellInfo_2->Effect[effIndex_2] ||
        spellInfo_1->EffectItemType[effIndex_1] != spellInfo_2->EffectItemType[effIndex_2] ||
        spellInfo_1->EffectMiscValue[effIndex_1] != spellInfo_2->EffectMiscValue[effIndex_2] ||
        spellInfo_1->EffectApplyAuraName[effIndex_1] != spellInfo_2->EffectApplyAuraName[effIndex_2])
        return false;

    return true;
}*/

int32 CompareAuraRanks(uint32 spellId_1, uint32 effIndex_1, uint32 spellId_2, uint32 effIndex_2)
{
    SpellEntry const* spellInfo_1 = sSpellStore.LookupEntry(spellId_1);
    SpellEntry const* spellInfo_2 = sSpellStore.LookupEntry(spellId_2);
    if (!spellInfo_1 || !spellInfo_2) return 0;
    if (spellId_1 == spellId_2) return 0;

    int32 diff = spellInfo_1->EffectBasePoints[effIndex_1] - spellInfo_2->EffectBasePoints[effIndex_2];
    if (spellInfo_1->EffectBasePoints[effIndex_1] + 1 < 0 && spellInfo_2->EffectBasePoints[effIndex_2] + 1 < 0) return -diff;
    else return diff;
}

SpellSpecific GetSpellSpecific(uint32 spellId)
{
    SpellEntry const* spellInfo = sSpellStore.LookupEntry(spellId);
    if (!spellInfo)
        return SPELL_SPECIFIC_NORMAL;

    switch (spellInfo->SpellFamilyName)
    {
    case SPELLFAMILY_GENERIC:
        {
            // Food / Drinks (mostly)
            if (spellInfo->AuraInterruptFlags & AURA_INTERRUPT_FLAG_NOT_SEATED)
            {
                for (int i = 0; i < MAX_SPELL_EFFECTS; i++)
                {
                    if (spellInfo->EffectApplyAuraName[i] == SPELL_AURA_MOD_POWER_REGEN)
                        return SPELL_SPECIFIC_DRINK;

                    if (spellInfo->EffectApplyAuraName[i] == SPELL_AURA_MOD_REGEN ||
                        spellInfo->EffectApplyAuraName[i] == SPELL_AURA_OBS_MOD_HEALTH)
                        return SPELL_SPECIFIC_FOOD;
                }
            }

            switch (spellInfo->Id)
            {
            case 12880: // warrior's Enrage rank 1
            case 14201: //           Enrage rank 2
            case 14202: //           Enrage rank 3
            case 14203: //           Enrage rank 4
            case 14204: //           Enrage rank 5
            case 12292: //             Death Wish
                return SPELL_SPECIFIC_WARRIOR_ENRAGE;
                break;
            default:
                break;
            }
            break;
        }
    case SPELLFAMILY_MAGE:
        {
            // family flags 18(Molten), 25(Frost/Ice), 28(Mage)
            if (spellInfo->SpellFamilyFlags & 0x12040000)
                return SPELL_SPECIFIC_MAGE_ARMOR;

            if ((spellInfo->SpellFamilyFlags & 0x1000000) && spellInfo->EffectApplyAuraName[0] == SPELL_AURA_MOD_CONFUSE)
                return SPELL_SPECIFIC_MAGE_POLYMORPH;

            break;
        }
    case SPELLFAMILY_WARRIOR:
        {
            // Sunder Armor (vs Expose Armor)
            if (spellInfo->SpellFamilyFlags & 0x00000000004000LL)
                return SPELL_SPECIFIC_ARMOR_REDUCE;

            break;
        }
    case SPELLFAMILY_WARLOCK:
        {
            // only warlock curses have this
            if (spellInfo->Dispel == DISPEL_CURSE)
                return SPELL_SPECIFIC_CURSE; 

            // family flag 37 (only part spells have family name)
            if (spellInfo->SpellFamilyFlags & 0x2000000000LL)
                return SPELL_SPECIFIC_WARLOCK_ARMOR;

            //seed of corruption and corruption
            if (spellInfo->SpellFamilyFlags & 0x1000000002LL)
                return SPELL_SPECIFIC_WARLOCK_CORRUPTION; 
            break;
        }
    case SPELLFAMILY_HUNTER:
        {
            // only hunter stings have this
            if (spellInfo->Dispel == DISPEL_POISON)
                return SPELL_SPECIFIC_STING;

            break;
        }
    case SPELLFAMILY_PALADIN:
        {
            if (IsSealSpell(spellInfo))
                return SPELL_SPECIFIC_SEAL;

            if ((spellInfo->SpellFamilyFlags & 0x00000820180400LL) && (spellInfo->AttributesEx3 & 0x200))
                return SPELL_SPECIFIC_JUDGEMENT;

            for (int i = 0; i < MAX_SPELL_EFFECTS; i++)
            {
                // only paladin auras have this
                if (spellInfo->Effect[i] == SPELL_EFFECT_APPLY_AREA_AURA_PARTY)
                    return SPELL_SPECIFIC_AURA;
            }
            break;
        }
    case SPELLFAMILY_SHAMAN:
        {
            if (IsElementalShield(spellInfo))
                return SPELL_SPECIFIC_ELEMENTAL_SHIELD;

            break;
        }
    case SPELLFAMILY_ROGUE:
        {
            // Expose Armor (vs Sunder Armor)
            if (spellInfo->SpellFamilyFlags & 0x00000000080000LL)
                return SPELL_SPECIFIC_ARMOR_REDUCE;
            break;
        }
    }

    // only warlock armor/skin have this (in additional to family cases)
    if (spellInfo->SpellVisual == 130 && spellInfo->SpellIconID == 89)
        return SPELL_SPECIFIC_WARLOCK_ARMOR;

    // only hunter aspects have this (but not all aspects in hunter family)
    if (spellInfo->activeIconID == 122 && (GetSpellSchoolMask(spellInfo) & SPELL_SCHOOL_MASK_NATURE) &&
        (spellInfo->Attributes & 0x50000) != 0 && (spellInfo->Attributes & 0x9000010) == 0)
        return SPELL_SPECIFIC_ASPECT;

    for (int i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        if (spellInfo->Effect[i] == SPELL_EFFECT_APPLY_AURA)
        {
            switch (spellInfo->EffectApplyAuraName[i])
            {
            case SPELL_AURA_MOD_CHARM:
            case SPELL_AURA_MOD_POSSESS_PET:
            case SPELL_AURA_MOD_POSSESS:
                return SPELL_SPECIFIC_CHARM;
            case SPELL_AURA_TRACK_CREATURES:
            case SPELL_AURA_TRACK_RESOURCES:
            case SPELL_AURA_TRACK_STEALTHED:
                return SPELL_SPECIFIC_TRACKER;
            }
        }
    }

    return SPELL_SPECIFIC_NORMAL;
}

bool IsPositiveTarget(uint32 targetA, uint32 targetB)
{
    // non-positive targets
    switch (targetA)
    {
    case TARGET_UNIT_NEARBY_ENEMY:
    case TARGET_UNIT_TARGET_ENEMY:
    case TARGET_UNIT_AREA_ENEMY_SRC:
    case TARGET_UNIT_AREA_ENEMY_DST:
    case TARGET_UNIT_CONE_ENEMY:
    case TARGET_DEST_DYNOBJ_ENEMY:
    case TARGET_DST_TARGET_ENEMY:
    case TARGET_UNIT_CHANNEL:
        return false;
    default:
        break;
    }
    if (targetB)
        return IsPositiveTarget(targetB, 0);
    return true;
}

bool IsPositiveEffect(uint32 spellId, uint32 effIndex)
{
    SpellEntry const* spellproto = sSpellStore.LookupEntry(spellId);
    if (!spellproto)
        return false;

    // not found a single positive spell with this attribute
    if (spellproto->Attributes & SPELL_ATTR0_NEGATIVE_1)
        return false;

    switch (spellproto->SpellFamilyName)
    {
        case SPELLFAMILY_GENERIC:
            switch (spellproto->Id)
            {
                case 6716:                                          // Test of Faith
                case 23333:                                         // Warsong Flag
                case 23335:                                         // Silverwing Flag
                case 34976:                                         // Netherstorm Flag
                    return true;
                case 1852:                                          // Silenced (GM)
                case 46392:                                         // Focused Assault
                case 46393:                                         // Brutal Assault
                case 43437:                                         // Paralyzed
                case 28441:                                         // AB Effect 000
                case 37675:                                         // Chaos Blast
                case 41519:                                         // Mark of Stormrage
                case 34877:                                         // Custodian of Time
                case 34700:                                         // Allergic Reaction
                case 31719:                                         // Suspension
                case 43501:                                         // Siphon Soul (Hexlord Spell)
                case 30457:                                         // Complete vulnerability
                case 30529:                                         // Recently In Game - Chess Event
                case 37465:                                         // Rain of Fire
                case 45661:                                         // Encapsulate (Felmyst - Sunwell Plateau)
                case 45662:
                case 45665:
                    return false;
                default:
                    break;
            }
            break;
        case SPELLFAMILY_MAGE:
        {
            // Amplify Magic, Dampen Magic
            if (spellproto->SpellFamilyFlags == 0x00002000)
                return true;

            switch (spellproto->Id)
            {
                case 31579:                                         // Arcane Empowerment Rank1 talent aura with one positive and one negative (check not needed in wotlk)
                case 31582:                                         // Arcane Empowerment Rank2
                case 31583:                                         // Arcane Empowerment Rank3
                    return true;
                default:
                    break;
            }
        }
        break;
        default:
            break;
    }

    switch (spellproto->Mechanic)
    {
        case MECHANIC_IMMUNE_SHIELD:
            return true;
        default:
            break;
    }

    switch (spellproto->Effect[effIndex])
    {
        // always positive effects (check before target checks that provided non-positive result in some case for positive effects)
        case SPELL_EFFECT_HEAL:
        case SPELL_EFFECT_LEARN_SPELL:
        case SPELL_EFFECT_SKILL_STEP:
        case SPELL_EFFECT_HEAL_PCT:
        case SPELL_EFFECT_ENERGIZE_PCT:
            return true;
        case SPELL_EFFECT_APPLY_AREA_AURA_ENEMY:
            return false;

        // non-positive aura use
        case SPELL_EFFECT_APPLY_AURA:
        case SPELL_EFFECT_APPLY_AREA_AURA_FRIEND:
        {
            switch (spellproto->EffectApplyAuraName[effIndex])
            {
                case SPELL_AURA_MOD_DAMAGE_DONE:            // dependent from bas point sign (negative -> negative)
                case SPELL_AURA_MOD_STAT:
                case SPELL_AURA_MOD_SKILL:
                case SPELL_AURA_MOD_DODGE_PERCENT:
                case SPELL_AURA_MOD_HEALING_PCT:
                case SPELL_AURA_MOD_HEALING_DONE:
                case SPELL_AURA_MOD_DAMAGE_PERCENT_DONE:
                    if (spellproto->EffectBasePoints[effIndex] + int32(spellproto->EffectBaseDice[effIndex]) < 0)
                        return false;
                    break;
                case SPELL_AURA_MOD_DAMAGE_TAKEN:           // dependent from bas point sign (positive -> negative)
                    if (spellproto->EffectBasePoints[effIndex] + int32(spellproto->EffectBaseDice[effIndex]) > 0)
                        return false;
                    break;
                case SPELL_AURA_MOD_SPELL_CRIT_CHANCE:
                    if (spellproto->EffectBasePoints[effIndex] + int32(spellproto->EffectBaseDice[effIndex]) > 0)
                        return true;                        // some expected positive spells have SPELL_ATTR1_NEGATIVE
                    break;
                case SPELL_AURA_ADD_TARGET_TRIGGER:
                    return true;
                case SPELL_AURA_PERIODIC_TRIGGER_SPELL_WITH_VALUE:
                case SPELL_AURA_PERIODIC_TRIGGER_SPELL:
                    if (spellId != spellproto->EffectTriggerSpell[effIndex])
                    {
                        uint32 spellTriggeredId = spellproto->EffectTriggerSpell[effIndex];
                        SpellEntry const* spellTriggeredProto = sSpellStore.LookupEntry(spellTriggeredId);

                        if (spellTriggeredProto)
                        {
                            // non-positive targets of main spell return early
                            for (int i = 0; i < MAX_SPELL_EFFECTS; ++i)
                            {
                                // if non-positive trigger cast targeted to positive target this main cast is non-positive
                                // this will place this spell auras as debuffs
                                if (!IsPositiveTarget(spellTriggeredProto->EffectImplicitTargetA[i], spellTriggeredProto->EffectImplicitTargetB[i]) && !IsPositiveEffect(spellTriggeredId, i))
                                    return false;
                            }
                        }
                    }
                    break;
                case SPELL_AURA_PROC_TRIGGER_SPELL:
                    // many positive auras have negative triggered spells at damage for example and this not make it negative (it can be canceled for example)
                    break;
                case SPELL_AURA_MOD_STUN:                   //have positive and negative spells, we can't sort its correctly at this moment.
                    if (effIndex == 0 && spellproto->Effect[1] == 0 && spellproto->Effect[2] == 0)
                        return false;                       // but all single stun aura spells is negative
                    break;
                case SPELL_AURA_MOD_PACIFY_SILENCE:
                    if (spellproto->Id == 24740)            // Wisp Costume
                        return true;
                    return false;
                case SPELL_AURA_MOD_ROOT:
                case SPELL_AURA_MOD_SILENCE:
                case SPELL_AURA_GHOST:
                case SPELL_AURA_PERIODIC_LEECH:
                case SPELL_AURA_MOD_STALKED:
                case SPELL_AURA_PERIODIC_DAMAGE_PERCENT:
                    return false;
                case SPELL_AURA_PERIODIC_DAMAGE:            // used in positive spells also.
                    // part of negative spell if casted at self (prevent cancel)
                    if (spellproto->EffectImplicitTargetA[effIndex] == TARGET_UNIT_CASTER)
                        return false;
                    break;
                case SPELL_AURA_MOD_DECREASE_SPEED:         // used in positive spells also
                    // part of positive spell if casted at self
                    if (spellproto->EffectImplicitTargetA[effIndex] != TARGET_UNIT_CASTER)
                        return false;
                    // but not this if this first effect (didn't find better check)
                    if (spellproto->Attributes & SPELL_ATTR0_NEGATIVE_1 && effIndex == 0)
                        return false;
                    break;
                case SPELL_AURA_MECHANIC_IMMUNITY:
                {
                    // non-positive immunities
                    switch (spellproto->EffectMiscValue[effIndex])
                    {
                        case MECHANIC_BANDAGE:
                        case MECHANIC_SHIELD:
                        case MECHANIC_MOUNT:
                        case MECHANIC_INVULNERABILITY:
                            return false;
                        default:
                            break;
                    }
                    break;
                }
                case SPELL_AURA_ADD_FLAT_MODIFIER:          // mods
                case SPELL_AURA_ADD_PCT_MODIFIER:
                {
                    // non-positive mods
                    switch (spellproto->EffectMiscValue[effIndex])
                    {
                        case SPELLMOD_COST:                 // dependent from bas point sign (negative -> positive)
                            if (spellproto->EffectBasePoints[effIndex] + int32(spellproto->EffectBaseDice[effIndex]) > 0)
                            {
                                if (spellproto->Id == 12042)    // Arcane Power is a positive spell
                                    return true;

                                return false;
                            }
                            break;
                        default:
                            break;
                    }
                    break;
                }
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }

    // non-positive targets
    if (!IsPositiveTarget(spellproto->EffectImplicitTargetA[effIndex], spellproto->EffectImplicitTargetB[effIndex]))
        return false;

    // ok, positive
    return true;
}

bool IsPositiveSpell(uint32 spellId)
{
    // spells with at least one negative effect are considered negative
    // some self-applied spells have negative effects but in self casting case negative check ignored.
    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        if (!IsPositiveEffect(spellId, i))
            return false;
    return true;
}

bool IsSingleTargetSpell(SpellEntry const* spellInfo)
{
    // all other single target spells have if it has AttributesEx5
    if (spellInfo->AttributesEx5 & SPELL_ATTR5_SINGLE_TARGET_SPELL)
        return true;

    // TODO - need found Judgements rule
    switch (GetSpellSpecific(spellInfo->Id))
    {
    case SPELL_SPECIFIC_JUDGEMENT:
        return true;
    default:
        break;
    }

    // single target triggered spell.
    // Not real client side single target spell, but it' not triggered until prev. aura expired.
    // This is allow store it in single target spells list for caster for spell proc checking
    if (spellInfo->Id == 38324)                                // Regeneration (triggered by 38299 (HoTs on Heals))
        return true;

    return false;
}

bool IsSingleTargetSpells(SpellEntry const* spellInfo1, SpellEntry const* spellInfo2)
{
    // TODO - need better check
    // Equal icon and spellfamily
    if (spellInfo1->SpellFamilyName == spellInfo2->SpellFamilyName &&
        spellInfo1->SpellIconID == spellInfo2->SpellIconID)
        return true;

    // TODO - need found Judgements rule
    SpellSpecific spec1 = GetSpellSpecific(spellInfo1->Id);
    // spell with single target specific types
    switch (spec1)
    {
    case SPELL_SPECIFIC_JUDGEMENT:
    case SPELL_SPECIFIC_MAGE_POLYMORPH:
        if (GetSpellSpecific(spellInfo2->Id) == spec1)
            return true;
    default:
        break;
    }

    return false;
}

bool IsAuraAddedBySpell(uint32 auraType, uint32 spellId)
{
    SpellEntry const* spellproto = sSpellStore.LookupEntry(spellId);
    if (!spellproto) return false;

    for (int i = 0; i < MAX_SPELL_EFFECTS; i++)
        if (spellproto->EffectApplyAuraName[i] == auraType)
            return true;
    return false;
}

SpellCastResult GetErrorAtShapeshiftedCast (SpellEntry const* spellInfo, uint32 form)
{
    // talents that learn spells can have stance requirements that need ignore
    // (this requirement only for client-side stance show in talent description)
    if (GetTalentSpellCost(spellInfo->Id) > 0 &&
        (spellInfo->Effect[0] == SPELL_EFFECT_LEARN_SPELL || spellInfo->Effect[1] == SPELL_EFFECT_LEARN_SPELL || spellInfo->Effect[2] == SPELL_EFFECT_LEARN_SPELL))
        return SPELL_CAST_OK;

    uint32 stanceMask = (form ? 1 << (form - 1) : 0);

    if (stanceMask & spellInfo->StancesNot)                 // can explicitly not be casted in this stance
        return SPELL_FAILED_NOT_SHAPESHIFT;

    if (stanceMask & spellInfo->Stances)                    // can explicitly be casted in this stance
        return SPELL_CAST_OK;

    bool actAsShifted = false;
    if (form > 0)
    {
        SpellShapeshiftEntry const* shapeInfo = sSpellShapeshiftStore.LookupEntry(form);
        if (!shapeInfo)
        {
            sLog.outError("GetErrorAtShapeshiftedCast: unknown shapeshift %u", form);
            return SPELL_CAST_OK;
        }
        actAsShifted = !(shapeInfo->flags1 & 1);            // shapeshift acts as normal form for spells
    }

    if (actAsShifted)
    {
        if (spellInfo->Attributes & SPELL_ATTR0_NOT_SHAPESHIFT) // not while shapeshifted
            return SPELL_FAILED_NOT_SHAPESHIFT;
        else if (spellInfo->Stances != 0)                   // needs other shapeshift
            return SPELL_FAILED_ONLY_SHAPESHIFT;
    }
    else
    {
        // needs shapeshift
        if (!(spellInfo->AttributesEx2 & SPELL_ATTR2_NOT_NEED_SHAPESHIFT) && spellInfo->Stances != 0)
            return SPELL_FAILED_ONLY_SHAPESHIFT;
    }

    return SPELL_CAST_OK;
}

void SpellMgr::LoadSpellTargetPositions()
{
    mSpellTargetPositions.clear();                                // need for reload case

    uint32 count = 0;

    //                                                       0   1           2                  3                  4                  5
    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT id, target_map, target_position_x, target_position_y, target_position_z, target_orientation FROM spell_target_position");
    if (!result)
    {



        sLog.outString(">> Loaded %u spell target coordinates", count);
        return;
    }


    do
    {
        Field* fields = result->Fetch();


        uint32 Spell_ID = fields[0].GetUInt32();

        SpellTargetPosition st;

        st.target_mapId       = fields[1].GetUInt32();
        st.target_X           = fields[2].GetFloat();
        st.target_Y           = fields[3].GetFloat();
        st.target_Z           = fields[4].GetFloat();
        st.target_Orientation = fields[5].GetFloat();

        MapEntry const* mapEntry = sMapStore.LookupEntry(st.target_mapId);
        if (!mapEntry)
        {
            sLog.outErrorDb("Spell (ID:%u) target map (ID: %u) does not exist in Map.dbc.", Spell_ID, st.target_mapId);
            continue;
        }

        if (st.target_X == 0 && st.target_Y == 0 && st.target_Z == 0)
        {
            sLog.outErrorDb("Spell (ID:%u) target coordinates not provided.", Spell_ID);
            continue;
        }

        SpellEntry const* spellInfo = sSpellStore.LookupEntry(Spell_ID);
        if (!spellInfo)
        {
            sLog.outErrorDb("Spell (ID:%u) listed in spell_target_position does not exist.", Spell_ID);
            continue;
        }

        bool found = false;
        for (int i = 0; i < MAX_SPELL_EFFECTS; ++i)
        {
            if (spellInfo->EffectImplicitTargetA[i] == TARGET_DST_DB || spellInfo->EffectImplicitTargetB[i] == TARGET_DST_DB)
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            sLog.outErrorDb("Spell (Id: %u) listed in spell_target_position does not have target TARGET_DST_DB (17).", Spell_ID);
            continue;
        }

        mSpellTargetPositions[Spell_ID] = st;
        ++count;

    }
    while (result->NextRow());

    sLog.outString(">> Loaded %u spell teleport coordinates", count);
}

void SpellMgr::LoadSpellAffects()
{
    mSpellAffectMap.clear();                                // need for reload case

    uint32 count = 0;

    //                                                       0      1         2
    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT entry, effectId, SpellFamilyMask FROM spell_affect");
    if (!result)
    {

        sLog.outString(">> Loaded %u spell affect definitions", count);
        return;
    }


    do
    {
        Field* fields = result->Fetch();


        uint16 entry = fields[0].GetUInt16();
        uint8 effectId = fields[1].GetUInt8();

        SpellEntry const* spellInfo = sSpellStore.LookupEntry(entry);

        if (!spellInfo)
        {
            sLog.outErrorDb("Spell %u listed in spell_affect does not exist", entry);
            continue;
        }

        if (effectId >= 3)
        {
            sLog.outErrorDb("Spell %u listed in spell_affect has invalid effect index (%u)", entry, effectId);
            continue;
        }

        if ((spellInfo->Effect[effectId]              != SPELL_EFFECT_APPLY_AURA) ||
            (spellInfo->EffectApplyAuraName[effectId] != SPELL_AURA_DUMMY &&
             spellInfo->EffectApplyAuraName[effectId] != SPELL_AURA_ADD_FLAT_MODIFIER &&
             spellInfo->EffectApplyAuraName[effectId] != SPELL_AURA_ADD_PCT_MODIFIER  &&
             spellInfo->EffectApplyAuraName[effectId] != SPELL_AURA_ADD_TARGET_TRIGGER))
        {
            sLog.outErrorDb("Spell %u listed in spell_affect does not have SPELL_AURA_ADD_FLAT_MODIFIER (%u) or SPELL_AURA_ADD_PCT_MODIFIER (%u) or SPELL_AURA_ADD_TARGET_TRIGGER (%u) for effect index (%u)", entry, SPELL_AURA_ADD_FLAT_MODIFIER, SPELL_AURA_ADD_PCT_MODIFIER, SPELL_AURA_ADD_TARGET_TRIGGER, effectId);
            continue;
        }

        uint64 spellAffectMask = fields[2].GetUInt64();

        // Spell.dbc have own data for low part of SpellFamilyMask
        if (spellInfo->EffectItemType[effectId])
        {
            if (spellInfo->EffectItemType[effectId] == spellAffectMask)
            {
                sLog.outErrorDb("Spell %u listed in spell_affect has redundant (same with EffectItemType%d) data for effect index (%u), skipped.", entry, effectId + 1, effectId);
                continue;
            }
        }

        mSpellAffectMap.insert(SpellAffectMap::value_type((entry << 8) + effectId, spellAffectMask));

        ++count;
    }
    while (result->NextRow());

    sLog.outString(">> Loaded %u spell affect definitions", count);

    for (uint32 id = 0; id < sSpellStore.GetNumRows(); ++id)
    {
        SpellEntry const* spellInfo = sSpellStore.LookupEntry(id);
        if (!spellInfo)
            continue;

        for (int effectId = 0; effectId < MAX_SPELL_EFFECTS; ++effectId)
        {
            if (spellInfo->Effect[effectId] != SPELL_EFFECT_APPLY_AURA ||
                (spellInfo->EffectApplyAuraName[effectId] != SPELL_AURA_ADD_FLAT_MODIFIER &&
                 spellInfo->EffectApplyAuraName[effectId] != SPELL_AURA_ADD_PCT_MODIFIER  &&
                 spellInfo->EffectApplyAuraName[effectId] != SPELL_AURA_ADD_TARGET_TRIGGER))
                continue;

            if (spellInfo->EffectItemType[effectId] != 0)
                continue;

            if (mSpellAffectMap.find((id << 8) + effectId) != mSpellAffectMap.end())
                continue;

            sLog.outErrorDb("Spell %u (%s) misses spell_affect for effect %u", id, spellInfo->SpellName[sWorld.GetDefaultDbcLocale()], effectId);
        }
    }
}

bool SpellMgr::IsAffectedBySpell(SpellEntry const* spellInfo, uint32 spellId, uint8 effectId, uint64 familyFlags) const
{
    // false for spellInfo == NULL
    if (!spellInfo)
        return false;

    SpellEntry const* affect_spell = sSpellStore.LookupEntry(spellId);
    // false for affect_spell == NULL
    if (!affect_spell)
        return false;

    // False if spellFamily not equal
    if (affect_spell->SpellFamilyName != spellInfo->SpellFamilyName)
        return false;

    // If familyFlags == 0
    if (!familyFlags)
    {
        // Get it from spellAffect table
        familyFlags = GetSpellAffectMask(spellId, effectId);
        // false if familyFlags == 0
        if (!familyFlags)
            return false;
    }

    // true
    if (familyFlags & spellInfo->SpellFamilyFlags)
        return true;

    return false;
}

void SpellMgr::LoadSpellProcEvents()
{
    mSpellProcEventMap.clear();                             // need for reload case

    uint32 count = 0;

    //                                                       0      1           2                3                4          5       6        7             8
    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT entry, SchoolMask, SpellFamilyName, SpellFamilyMask, procFlags, procEx, ppmRate, CustomChance, Cooldown FROM spell_proc_event");
    if (!result)
    {
        sLog.outString(">> Loaded %u spell proc event conditions", count);
        return;
    }

    uint32 customProc = 0;
    do
    {
        Field* fields = result->Fetch();

        uint16 entry = fields[0].GetUInt16();

        const SpellEntry* spell = sSpellStore.LookupEntry(entry);
        if (!spell)
        {
            sLog.outErrorDb("Spell %u listed in spell_proc_event does not exist", entry);
            continue;
        }

        SpellProcEventEntry spe;

        spe.schoolMask      = fields[1].GetUInt32();
        spe.spellFamilyName = fields[2].GetUInt32();
        spe.spellFamilyMask = fields[3].GetUInt64();
        spe.procFlags       = fields[4].GetUInt32();
        spe.procEx          = fields[5].GetUInt32();
        spe.ppmRate         = fields[6].GetFloat();
        spe.customChance    = fields[7].GetFloat();
        spe.cooldown        = fields[8].GetUInt32();

        mSpellProcEventMap[entry] = spe;

        if (spell->procFlags == 0)
        {
            if (spe.procFlags == 0)
            {
                sLog.outErrorDb("Spell %u listed in spell_proc_event probally not triggered spell", entry);
                continue;
            }
            customProc++;
        }
        ++count;
    }
    while (result->NextRow());

    if (customProc)
        sLog.outString(">> Loaded %u custom spell proc event conditions +%u custom",  count, customProc);
    else
        sLog.outString(">> Loaded %u spell proc event conditions", count);
}


bool SpellMgr::IsSpellProcEventCanTriggeredBy(SpellProcEventEntry const* spellProcEvent, uint32 EventProcFlag, SpellEntry const* procSpell, uint32 procFlags, uint32 procExtra, bool active)
{
    // No extra req need
    uint32 procEvent_procEx = PROC_EX_NONE;

    // check prockFlags for condition
    if ((procFlags & EventProcFlag) == 0)
        return false;

    /* Check Periodic Auras

    * Both hots and dots can trigger if spell has no PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_POS
        nor PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_NEG

    * Only Hots can trigger if spell has PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_POS

    * Only dots can trigger if spell has both positivity flags or PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_NEG

    */

    if (procFlags & PROC_FLAG_DONE_PERIODIC)
    {
        if (EventProcFlag & PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_NEG)
        {
            if (!(procExtra & PROC_EX_INTERNAL_DOT))
                return false;
        }
        else if (EventProcFlag & PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_POS
                 && !(procExtra & PROC_EX_INTERNAL_HOT))
            return false;
    }

    if (procFlags & PROC_FLAG_TAKEN_PERIODIC)
    {
        if (EventProcFlag & PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_NEG)
        {
            if (!(procExtra & PROC_EX_INTERNAL_DOT))
                return false;
        }
        else if (EventProcFlag & PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_POS
                 && !(procExtra & PROC_EX_INTERNAL_HOT))
            return false;
    }

    // Always trigger for this
    if (EventProcFlag & (PROC_FLAG_KILLED | PROC_FLAG_KILL | PROC_FLAG_DONE_TRAP_ACTIVATION))
        return true;

    if (spellProcEvent)     // Exist event data
    {
        // Store extra req
        procEvent_procEx = spellProcEvent->procEx;

        // For melee triggers
        if (procSpell == NULL)
        {
            // Check (if set) for school (melee attack have Normal school)
            if (spellProcEvent->schoolMask && (spellProcEvent->schoolMask & SPELL_SCHOOL_MASK_NORMAL) == 0)
                return false;
        }
        else // For spells need check school/spell family/family mask
        {
            // Potions can trigger only if spellfamily given
            if (procSpell->SpellFamilyName == SPELLFAMILY_POTION)
            {
                if (procSpell->SpellFamilyName == spellProcEvent->spellFamilyName)
                    return true;
                return false;
            }

            // Check (if set) for school
            if (spellProcEvent->schoolMask && (spellProcEvent->schoolMask & procSpell->SchoolMask) == 0)
                return false;

            // Check (if set) for spellFamilyName
            if (spellProcEvent->spellFamilyName && (spellProcEvent->spellFamilyName != procSpell->SpellFamilyName))
                return false;

            // spellFamilyName is Ok need check for spellFamilyMask if present
            if (spellProcEvent->spellFamilyMask)
            {
                if ((spellProcEvent->spellFamilyMask & procSpell->SpellFamilyFlags) == 0)
                    return false;
                active = true; // Spell added manualy -> so its active spell
            }
        }
    }
    // potions can trigger only if have spell_proc entry
    else if (procSpell && procSpell->SpellFamilyName == SPELLFAMILY_POTION)
        return false;

    // Check for extra req (if none) and hit/crit
    if (procEvent_procEx == PROC_EX_NONE)
    {
        // No extra req, so can trigger only for active (damage/healing present) and hit/crit
        if ((procExtra & (PROC_EX_NORMAL_HIT | PROC_EX_CRITICAL_HIT)) && active)
            return true;
    }
    else // Passive spells hits here only if resist/reflect/immune/evade
    {
        // Exist req for PROC_EX_EX_TRIGGER_ALWAYS
        if (procEvent_procEx & PROC_EX_EX_TRIGGER_ALWAYS)
            return true;
        // Passive spells cant trigger if need hit
        if ((procEvent_procEx & PROC_EX_NORMAL_HIT) && !active)
            return false;
        // Check Extra Requirement like (hit/crit/miss/resist/parry/dodge/block/immune/reflect/absorb and other)
        if (procEvent_procEx & procExtra)
            return true;
    }
    return false;
}

void SpellMgr::LoadSpellGroups()
{
    mSpellSpellGroup.clear();                                  // need for reload case
    mSpellGroupSpell.clear();

    uint32 count = 0;

    //                                                       0   1
    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT id, spell_id FROM spell_group");
    if ( !result )
    {
        sLog.outString( ">> Loaded %u spell group definitions", count );
        return;
    }

    std::set<uint32> groups;

    do
    {
        Field* fields = result->Fetch();

        uint32 group_id = fields[0].GetUInt32();
        if (group_id <= SPELL_GROUP_DB_RANGE_MIN && group_id >= SPELL_GROUP_CORE_RANGE_MAX)
        {
            sLog.outErrorDb("SpellGroup id %u listed in `spell_groups` is in core range, but is not defined in core!", group_id);
            continue;
        }
        int32 spell_id = fields[1].GetInt32();

        groups.insert(std::set<uint32>::value_type(group_id));
        mSpellGroupSpell.insert(SpellGroupSpellMap::value_type((SpellGroup)group_id, spell_id));

    }
    while ( result->NextRow() );

    for (SpellGroupSpellMap::iterator itr = mSpellGroupSpell.begin(); itr != mSpellGroupSpell.end() ; )
    {
        if (itr->second < 0)
        {
            if (groups.find(abs(itr->second)) == groups.end())
            {
                sLog.outErrorDb("SpellGroup id %u listed in `spell_groups` does not exist", abs(itr->second));
                mSpellGroupSpell.erase(itr++);
            }
            else
                ++itr;
        }
        else
        {
            SpellEntry const* spellInfo = sSpellStore.LookupEntry(itr->second);

            if (!spellInfo)
            {
                sLog.outErrorDb("Spell %u listed in `spell_group` does not exist", itr->second);
                mSpellGroupSpell.erase(itr++);
            }
            else if (GetSpellRank(itr->second) > 1)
            {
                sLog.outErrorDb("Spell %u listed in `spell_group` is not first rank of spell", itr->second);
                mSpellGroupSpell.erase(itr++);
            }
            else
                ++itr;
        }
    }

    for (std::set<uint32>::iterator groupItr = groups.begin() ; groupItr != groups.end() ; ++groupItr)
    {
        std::set<uint32> spells;
        GetSetOfSpellsInSpellGroup(SpellGroup(*groupItr), spells);

        for (std::set<uint32>::iterator spellItr = spells.begin() ; spellItr != spells.end() ; ++spellItr)
        {
            ++count;
            mSpellSpellGroup.insert(SpellSpellGroupMap::value_type(*spellItr, SpellGroup(*groupItr)));
        }
    }

    sLog.outString( ">> Loaded %u spell group definitions", count );
}

void SpellMgr::LoadSpellGroupStackRules()
{
    mSpellGroupStack.clear();                                  // need for reload case

    uint32 count = 0;

    //                                                       0         1
    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT group_id, stack_rule FROM spell_group_stack_rules");
    if ( !result )
    {
        sLog.outString( ">> Loaded %u spell group stack rules", count );
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        uint32 group_id = fields[0].GetUInt32();
        uint8 stack_rule = fields[1].GetUInt32();
        if (stack_rule >= SPELL_GROUP_STACK_RULE_MAX)
        {
            sLog.outErrorDb("SpellGroupStackRule %u listed in `spell_group_stack_rules` does not exist", stack_rule);
            continue;
        }

        SpellGroupSpellMapBounds spellGroup = GetSpellGroupSpellMapBounds((SpellGroup)group_id);

        if (spellGroup.first == spellGroup.second)
        {
            sLog.outErrorDb("SpellGroup id %u listed in `spell_group_stack_rules` does not exist", group_id);
            continue;
        }

        mSpellGroupStack[(SpellGroup)group_id] = (SpellGroupStackRule)stack_rule;

        ++count;
    }
    while ( result->NextRow() );

    sLog.outString( ">> Loaded %u spell group stack rules", count );
}

void SpellMgr::LoadSpellThreats()
{
    mSpellThreatMap.clear();                                // need for reload case

    uint32 count = 0;

    //                                                0      1        2       3
    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT entry, flatMod, pctMod, apPctMod FROM spell_threat");

    if (!result)
    {
        sLog.outString( ">> Loaded %u aggro generating spells", count);
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        uint32 entry = fields[0].GetUInt32();

        if (!sSpellStore.LookupEntry(entry))
        {
            sLog.outErrorDb("Spell %u listed in `spell_threat` does not exist", entry);
            continue;
        }

        SpellThreatEntry ste;
        ste.flatMod  = fields[1].GetInt16();
        ste.pctMod   = fields[2].GetFloat();
        ste.apPctMod = fields[3].GetFloat();

        mSpellThreatMap[entry] = ste;
        count++;
    }
    while (result->NextRow());

    sLog.outString(">> Loaded %u SpellThreatEntries.", count);
}

void SpellMgr::LoadSpellEnchantProcData()
{
    mSpellEnchantProcEventMap.clear();                             // need for reload case

    uint32 count = 0;

    //                                                       0      1             2          3
    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT entry, customChance, PPMChance, procEx FROM spell_enchant_proc_data");
    if (!result)
    {
        sLog.outString(">> Loaded %u spell enchant proc event conditions", count);
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        uint32 enchantId = fields[0].GetUInt32();

        SpellItemEnchantmentEntry const* ench = sSpellItemEnchantmentStore.LookupEntry(enchantId);
        if (!ench)
        {
            sLog.outErrorDb("Enchancment %u listed in spell_enchant_proc_data does not exist", enchantId);
            continue;
        }

        SpellEnchantProcEntry spe;

        spe.customChance = fields[1].GetUInt32();
        spe.PPMChance = fields[2].GetFloat();
        spe.procEx = fields[3].GetUInt32();

        mSpellEnchantProcEventMap[enchantId] = spe;

        ++count;
    }
    while (result->NextRow());

    sLog.outString(">> Loaded %u enchant proc data definitions", count);
}

void SpellMgr::LoadSpellDummyCondition()
{
    //                                                             0        1          2                3          4        5          6     7          8        9        10          11      12
    if (QueryResult_AutoPtr result = WorldDatabase.PQuery("SELECT entry, effIndex, bitMaskCondition, condition0, data0, condition1, data1, condition2, data2, condition3, data3, condition4, data4 FROM spell_dummy_condition ORDER BY entry ASC"))
    {
        Field* field;
        SpellDummyConditionEntry sdcEntry;

        do
        {
            field = result->Fetch();

            sdcEntry.entry = field[0].GetUInt32();
            sdcEntry.effIndex = SpellEffIndex(field[1].GetUInt32());
            sdcEntry.bitMaskCondition = field[2].GetUInt32();

            for (uint32 i = 0; i < 5; i++) // 5 condition - data pairs
            {
                sdcEntry.conditions[i].condition = static_cast<SpellDummyCondition>(field[3 + i * 2].GetUInt32());
                sdcEntry.conditions[i].data = field[4 + i * 2].GetInt32();
            }

            mSpellDummyConditionMap.insert( std::pair<uint32, SpellDummyConditionEntry>(sdcEntry.entry, sdcEntry) );
        }
        while (result->NextRow());

        sLog.outString(">> Loaded %llu spell dummy conditions", result->GetRowCount());
        return;
    }

    sLog.outString(">> Loaded 0 spell dummy conditions");
}

bool SpellMgr::IsRankSpellDueToSpell(SpellEntry const* spellInfo_1, uint32 spellId_2) const
{
    SpellEntry const* spellInfo_2 = sSpellStore.LookupEntry(spellId_2);
    if (!spellInfo_1 || !spellInfo_2) return false;
    if (spellInfo_1->Id == spellId_2) return false;

    return GetFirstSpellInChain(spellInfo_1->Id) == GetFirstSpellInChain(spellId_2);
}

/**
 * Checks whether spell should stack in spellbook with its ranks.
 * If spell can stack it means that all its ranks will be shown in the spellbook.
 * @param spellinfo Pointer to the spell entry
 * @returns true if spell ranks stack, false otherwise
 */
bool SpellMgr::canStackSpellRanks(SpellEntry const* spellInfo)
{
    if (spellInfo->powerType != POWER_MANA && spellInfo->powerType != POWER_HEALTH)
    {
        // These spells do not cost energy/rage/focus and their cost does not
        // change between ranks so we will allow only the highest rank in spellbook

        // One exception - Faerie Fire (feral) need to stack in order to properly show
        // up in talent tree otherwise on higher rank it will be seen as not used
        if (spellInfo->SpellFamilyName == SPELLFAMILY_DRUID && spellInfo->SpellFamilyFlags & 0x400)
            return true;

        return false;
    }

    if (IsProfessionSpell(spellInfo->Id))
        return false;

    // All stance spells. if any better way, change it.
    for (int i = 0; i < MAX_SPELL_EFFECTS; i++)
    {
        // Paladin aura Spell
        if (spellInfo->SpellFamilyName == SPELLFAMILY_PALADIN
            && spellInfo->Effect[i] == SPELL_EFFECT_APPLY_AREA_AURA_PARTY)
            return false;
        // Druid form Spell
        if (spellInfo->SpellFamilyName == SPELLFAMILY_DRUID
            && spellInfo->Effect[i] == SPELL_EFFECT_APPLY_AURA
            && spellInfo->EffectApplyAuraName[i] == SPELL_AURA_MOD_SHAPESHIFT)
            return false;
        // Rogue Stealth
        if (spellInfo->SpellFamilyName == SPELLFAMILY_ROGUE
            && spellInfo->Effect[i] == SPELL_EFFECT_APPLY_AURA
            && spellInfo->EffectApplyAuraName[i] == SPELL_AURA_MOD_SHAPESHIFT)
            return false;
    }
    return true;
}

/**
* @name IsNoStackSpellDueToSpell
* @brief Determines whether a spell can stack, based on parameters of another spell
* @param spellId_1 Spell to compare
* @param spellId_2 Spell we are comparing to
* @param sameCaster Are these spells casted by the same entity?
*/
bool SpellMgr::IsNoStackSpellDueToSpell(uint32 spellId_1, uint32 spellId_2, bool sameCaster) const
{
    SpellEntry const* spellInfo_1 = sSpellStore.LookupEntry(spellId_1);
    SpellEntry const* spellInfo_2 = sSpellStore.LookupEntry(spellId_2);

    if (!spellInfo_1 || !spellInfo_2)
        return false;

    SpellGroupStackRule stackRule = CheckSpellGroupStackRules(spellInfo_1->Id, spellInfo_2->Id);
    if (stackRule)
    {
        if (stackRule == SPELL_GROUP_STACK_RULE_EXCLUSIVE)
            return true;
        if (sameCaster && stackRule == SPELL_GROUP_STACK_RULE_EXCLUSIVE_FROM_SAME_CASTER)
            return true;
        if (stackRule != SPELL_GROUP_STACK_RULE_DEFAULT)
            return false;
    }

    if (spellInfo_1->SpellFamilyName != spellInfo_2->SpellFamilyName)
        return false;

    if (!sameCaster)
    {
        // Iterate through effects
        for (uint32 i = 0; i < MAX_SPELL_EFFECTS; ++i)
            if (spellInfo_1->Effect[i] == SPELL_EFFECT_APPLY_AURA
                || spellInfo_1->Effect[i] == SPELL_EFFECT_PERSISTENT_AREA_AURA)
                // not area auras (shaman totem)
                switch (spellInfo_1->EffectApplyAuraName[i])
                {
                // DOT or HOT from different casters will stack
                case SPELL_AURA_PERIODIC_DAMAGE:
                case SPELL_AURA_PERIODIC_HEAL:
                case SPELL_AURA_PERIODIC_TRIGGER_SPELL:
                case SPELL_AURA_PERIODIC_ENERGIZE:
                case SPELL_AURA_PERIODIC_MANA_LEECH:
                case SPELL_AURA_PERIODIC_LEECH:
                case SPELL_AURA_POWER_BURN_MANA:
                case SPELL_AURA_OBS_MOD_MANA:
                case SPELL_AURA_OBS_MOD_HEALTH:
                case SPELL_AURA_PERIODIC_TRIGGER_SPELL_WITH_VALUE:
                    return false;
                default:
                    break;
                }
    }

    // Iterate through specifics
    SpellSpecific spellSpec = GetSpellSpecific(spellId_1);
    switch (spellSpec)
    {
        case SPELL_SPECIFIC_STING:
        case SPELL_SPECIFIC_CURSE:
        case SPELL_SPECIFIC_ASPECT:
        case SPELL_SPECIFIC_WARLOCK_CORRUPTION:
        case SPELL_SPECIFIC_JUDGEMENT:
        case SPELL_SPECIFIC_TRACKER:
           return sameCaster == (spellSpec == GetSpellSpecific(spellId_2));
        default:
            break;
    }

    // generic spells
    if (!spellInfo_1->SpellFamilyName)
    {
        if (spellInfo_1->SpellIconID == spellInfo_2->SpellIconID &&
            spellInfo_1->SpellIconID != 0 && spellInfo_2->SpellIconID != 0)
            return true;
    }

    // check for class spells
    else
    {
        if (spellInfo_1->SpellFamilyFlags != spellInfo_2->SpellFamilyFlags)
            return false;
    }

    //use data of highest rank spell(needed for spells which ranks have different effects)
    spellInfo_1 = sSpellStore.LookupEntry(GetLastSpellInChain(spellId_1));
    spellInfo_2 = sSpellStore.LookupEntry(GetLastSpellInChain(spellId_2));

    //if spells have exactly the same effect they cannot stack
    for (uint32 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        if (spellInfo_1->Effect[i] != spellInfo_2->Effect[i]
            || spellInfo_1->EffectApplyAuraName[i] != spellInfo_2->EffectApplyAuraName[i]
            || spellInfo_1->EffectMiscValue[i] != spellInfo_2->EffectMiscValue[i]) // paladin resist aura
            return false; // need itemtype check? need an example to add that check

    return true;
}

SpellSpellGroupMapBounds SpellMgr::GetSpellSpellGroupMapBounds(uint32 spell_id) const
{
    spell_id = GetFirstSpellInChain(spell_id);
    return mSpellSpellGroup.equal_range(spell_id);
}

bool SpellMgr::IsSpellMemberOfSpellGroup(uint32 spellid, SpellGroup groupid) const
{
    SpellSpellGroupMapBounds spellGroup = GetSpellSpellGroupMapBounds(spellid);
    for (SpellSpellGroupMap::const_iterator itr = spellGroup.first; itr != spellGroup.second; ++itr)
    {
        if (itr->second == groupid)
            return true;
    }
    return false;
}

SpellGroupSpellMapBounds SpellMgr::GetSpellGroupSpellMapBounds(SpellGroup group_id) const
{
    return mSpellGroupSpell.equal_range(group_id);
}

void SpellMgr::GetSetOfSpellsInSpellGroup(SpellGroup group_id, std::set<uint32>& foundSpells) const
{
    std::set<SpellGroup> usedGroups;
    GetSetOfSpellsInSpellGroup(group_id, foundSpells, usedGroups);
}

void SpellMgr::GetSetOfSpellsInSpellGroup(SpellGroup group_id, std::set<uint32>& foundSpells, std::set<SpellGroup>& usedGroups) const
{
    if (usedGroups.find(group_id) != usedGroups.end())
        return;
    usedGroups.insert(group_id);

    SpellGroupSpellMapBounds groupSpell = GetSpellGroupSpellMapBounds(group_id);
    for (SpellGroupSpellMap::const_iterator itr = groupSpell.first; itr != groupSpell.second; ++itr)
    {
        if (itr->second < 0)
        {
            SpellGroup currGroup = (SpellGroup)abs(itr->second);
            GetSetOfSpellsInSpellGroup(currGroup, foundSpells, usedGroups);
        }
        else
        {
            foundSpells.insert(itr->second);
        }
    }
}

SpellGroupStackRule SpellMgr::CheckSpellGroupStackRules(uint32 spellid_1, uint32 spellid_2) const
{
    spellid_1 = GetFirstSpellInChain(spellid_1);
    spellid_2 = GetFirstSpellInChain(spellid_2);
    if (spellid_1 == spellid_2)
        return SPELL_GROUP_STACK_RULE_DEFAULT;
    // find SpellGroups which are common for both spells
    SpellSpellGroupMapBounds spellGroup1 = GetSpellSpellGroupMapBounds(spellid_1);
    std::set<SpellGroup> groups;
    for (SpellSpellGroupMap::const_iterator itr = spellGroup1.first; itr != spellGroup1.second; ++itr)
    {
        if (IsSpellMemberOfSpellGroup(spellid_2, itr->second))
        {
            bool add = true;
            SpellGroupSpellMapBounds groupSpell = GetSpellGroupSpellMapBounds(itr->second);
            for (SpellGroupSpellMap::const_iterator itr2 = groupSpell.first; itr2 != groupSpell.second; ++itr2)
            {
                if (itr2->second < 0)
                {
                    SpellGroup currGroup = (SpellGroup)abs(itr2->second);
                    if (IsSpellMemberOfSpellGroup(spellid_1, currGroup) && IsSpellMemberOfSpellGroup(spellid_2, currGroup))
                    {
                        add = false;
                        break;
                    }
                }
            }
            if (add)
                groups.insert(itr->second);
        }
    }

    SpellGroupStackRule rule = SPELL_GROUP_STACK_RULE_DEFAULT;

    for (std::set<SpellGroup>::iterator itr = groups.begin(); itr!= groups.end(); ++itr)
    {
        SpellGroupStackMap::const_iterator found = mSpellGroupStack.find(*itr);
        if (found != mSpellGroupStack.end())
            rule = found->second;
        if (rule)
            break;
    }
    return rule;
}

bool SpellMgr::IsProfessionSpell(uint32 spellId)
{
    SpellEntry const* spellInfo = sSpellStore.LookupEntry(spellId);
    if (!spellInfo)
        return false;

    if (spellInfo->Effect[1] != SPELL_EFFECT_SKILL)
        return false;

    uint32 skill = spellInfo->EffectMiscValue[1];

    return IsProfessionSkill(skill);
}

bool SpellMgr::IsPrimaryProfessionSpell(uint32 spellId)
{
    SpellEntry const* spellInfo = sSpellStore.LookupEntry(spellId);
    if (!spellInfo)
        return false;

    if (spellInfo->Effect[1] != SPELL_EFFECT_SKILL)
        return false;

    uint32 skill = spellInfo->EffectMiscValue[1];

    return IsPrimaryProfessionSkill(skill);
}

bool SpellMgr::IsPrimaryProfessionFirstRankSpell(uint32 spellId) const
{
    return IsPrimaryProfessionSpell(spellId) && GetSpellRank(spellId) == 1;
}

SpellEntry const* SpellMgr::SelectAuraRankForPlayerLevel(SpellEntry const* spellInfo, uint32 playerLevel) const
{
    // ignore passive spells
    if (IsPassiveSpell(spellInfo->Id))
        return spellInfo;

    bool needRankSelection = false;
    for (int i = 0; i < 3; i++)
    {
        if (IsPositiveEffect(spellInfo->Id, i) && (
                spellInfo->Effect[i] == SPELL_EFFECT_APPLY_AURA ||
                spellInfo->Effect[i] == SPELL_EFFECT_APPLY_AREA_AURA_PARTY
            ))
        {
            needRankSelection = true;
            break;
        }
    }

    // not required
    if (!needRankSelection)
        return spellInfo;

    for (uint32 nextSpellId = spellInfo->Id; nextSpellId != 0; nextSpellId = GetPrevSpellInChain(nextSpellId))
    {
        SpellEntry const* nextSpellInfo = sSpellStore.LookupEntry(nextSpellId);
        if (!nextSpellInfo)
            break;

        // if found appropriate level
        if (playerLevel + 10 >= nextSpellInfo->spellLevel)
            return nextSpellInfo;

        // one rank less then
    }

    // not found
    return NULL;
}

void SpellMgr::LoadSpellRequired()
{
    mSpellsReqSpell.clear();                                   // need for reload case
    mSpellReq.clear();                                         // need for reload case

    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT spell_id, req_spell from spell_required");

    if (!result)
    {
        sLog.outString(">> Loaded 0 spell required records");
        sLog.outErrorDb("spell_required table is empty!");
        return;
    }
    uint32 rows = 0;

    do
    {
        Field* fields = result->Fetch();

        uint32 spell_id = fields[0].GetUInt32();
        uint32 spell_req = fields[1].GetUInt32();

        mSpellsReqSpell.insert (std::pair<uint32, uint32>(spell_req, spell_id));
        mSpellReq[spell_id] = spell_req;
        ++rows;
    }
    while (result->NextRow());

    sLog.outString(">> Loaded %u spell required records", rows);
}

struct SpellRankEntry
{
    uint32 SkillId;
    char const* SpellName;
    uint32 DurationIndex;
    uint32 RangeIndex;
    uint32 SpellVisual;
    uint32 ProcFlags;
    uint64 SpellFamilyFlags;
    uint32 TargetAuraState;
    uint32 ManaCost;

    bool operator()(const SpellRankEntry& _Left, const SpellRankEntry& _Right)const
    {
        return (_Left.SkillId != _Right.SkillId ? _Left.SkillId < _Right.SkillId
                : _Left.SpellName != _Right.SpellName ? _Left.SpellName < _Right.SpellName
                : _Left.ProcFlags != _Right.ProcFlags ? _Left.ProcFlags < _Right.ProcFlags
                : _Left.SpellFamilyFlags != _Right.SpellFamilyFlags ? _Left.SpellFamilyFlags < _Right.SpellFamilyFlags
                : (_Left.SpellVisual != _Right.SpellVisual) && (!_Left.SpellVisual || !_Right.SpellVisual) ? _Left.SpellVisual < _Right.SpellVisual
                : (_Left.ManaCost != _Right.ManaCost) && (!_Left.ManaCost || !_Right.ManaCost) ? _Left.ManaCost < _Right.ManaCost
                : (_Left.DurationIndex != _Right.DurationIndex) && (!_Left.DurationIndex || !_Right.DurationIndex) ? _Left.DurationIndex < _Right.DurationIndex
                : (_Left.RangeIndex != _Right.RangeIndex) && (!_Left.RangeIndex || !_Right.RangeIndex || _Left.RangeIndex == 1 || !_Right.RangeIndex == 1) ? _Left.RangeIndex < _Right.RangeIndex
                : _Left.TargetAuraState < _Right.TargetAuraState
               );
    }
};

struct SpellRankValue
{
    uint32 Id;
    char const* Rank;
};

void SpellMgr::LoadSpellChains()
{
    mSpellChains.clear();                                   // need for reload case

    std::vector<uint32> ChainedSpells;
    for (uint32 ability_id = 0; ability_id < sSkillLineAbilityStore.GetNumRows(); ability_id++)
    {
        SkillLineAbilityEntry const* AbilityInfo = sSkillLineAbilityStore.LookupEntry(ability_id);
        if (!AbilityInfo)
            continue;
        if (AbilityInfo->spellId == 20154) //exception to these rules (not needed in 3.0.3)
            continue;
        if (!AbilityInfo->forward_spellid)
            continue;
        ChainedSpells.push_back(AbilityInfo->forward_spellid);
    }

    std::multimap<SpellRankEntry, SpellRankValue, SpellRankEntry> RankMap;

    for (uint32 ability_id = 0; ability_id < sSkillLineAbilityStore.GetNumRows(); ability_id++)
    {
        SkillLineAbilityEntry const* AbilityInfo = sSkillLineAbilityStore.LookupEntry(ability_id);
        if (!AbilityInfo)
            continue;

        //get only spell with lowest ability_id to prevent doubles
        uint32 spell_id = AbilityInfo->spellId;
        if (spell_id == 20154) //exception to these rules (not needed in 3.0.3)
            continue;
        bool found = false;
        for (uint32 i = 0; i < ChainedSpells.size(); i++)
        {
            if (ChainedSpells.at(i) == spell_id)
                found = true;
        }
        if (found)
            continue;

        if (mSkillLineAbilityMap.lower_bound(spell_id)->second->id != ability_id)
            continue;
        SpellEntry const* SpellInfo = sSpellStore.LookupEntry(spell_id);
        if (!SpellInfo)
            continue;
        std::string sRank = SpellInfo->Rank[sWorld.GetDefaultDbcLocale()];
        if (sRank.empty())
            continue;
        //exception to polymorph spells-make pig and turtle other chain than sheep
        if ((SpellInfo->SpellFamilyName == SPELLFAMILY_MAGE) && (SpellInfo->SpellFamilyFlags & 0x1000000) && (SpellInfo->SpellIconID != 82))
            continue;

        SpellRankEntry entry;
        SpellRankValue value;
        entry.SkillId = AbilityInfo->skillId;
        entry.SpellName = SpellInfo->SpellName[sWorld.GetDefaultDbcLocale()];
        entry.DurationIndex = SpellInfo->DurationIndex;
        entry.RangeIndex = SpellInfo->rangeIndex;
        entry.ProcFlags = SpellInfo->procFlags;
        entry.SpellFamilyFlags = SpellInfo->SpellFamilyFlags;
        entry.TargetAuraState = SpellInfo->TargetAuraState;
        entry.SpellVisual = SpellInfo->SpellVisual;
        entry.ManaCost = SpellInfo->manaCost;

        for (;;)
        {
            AbilityInfo = mSkillLineAbilityMap.lower_bound(spell_id)->second;
            value.Id = spell_id;
            value.Rank = SpellInfo->Rank[sWorld.GetDefaultDbcLocale()];
            RankMap.insert(std::pair<SpellRankEntry, SpellRankValue>(entry, value));
            spell_id = AbilityInfo->forward_spellid;
            SpellInfo = sSpellStore.LookupEntry(spell_id);
            if (!SpellInfo)
                break;
        }
    }


    uint32 count = 0;

    for (std::multimap<SpellRankEntry, SpellRankValue, SpellRankEntry>::iterator itr = RankMap.begin(); itr != RankMap.end();)
    {
        SpellRankEntry entry = itr->first;
        //trac errors in extracted data
        std::multimap<char const*, std::multimap<SpellRankEntry, SpellRankValue, SpellRankEntry>::iterator> RankErrorMap;
        for (std::multimap<SpellRankEntry, SpellRankValue, SpellRankEntry>::iterator itr2 = RankMap.lower_bound(entry); itr2 != RankMap.upper_bound(entry); itr2++)
            RankErrorMap.insert(std::pair<char const*, std::multimap<SpellRankEntry, SpellRankValue, SpellRankEntry>::iterator>(itr2->second.Rank, itr2));
        for (std::multimap<char const*, std::multimap<SpellRankEntry, SpellRankValue, SpellRankEntry>::iterator>::iterator itr2 = RankErrorMap.begin(); itr2 != RankErrorMap.end();)
        {
            char const* err_entry = itr2->first;
            uint32 rank_count = RankErrorMap.count(itr2->first);
            if (rank_count > 1)
                for (itr2 = RankErrorMap.lower_bound(err_entry); itr2 != RankErrorMap.upper_bound(err_entry); itr2++)
                {
                    sLog.outDebug("There is a duplicate rank entry (%s) for spell: %u", itr2->first, itr2->second->second.Id);
                    sLog.outDebug("Spell %u removed from chain data.", itr2->second->second.Id);
                    RankMap.erase(itr2->second);
                    itr = RankMap.lower_bound(entry);
                }
            else
                itr2++;
        }
        //do not proceed for spells with less than 2 ranks
        uint32 spell_max_rank = RankMap.count(entry);
        if (spell_max_rank < 2)
        {
            itr = RankMap.upper_bound(entry);
            continue;
        }

        itr = RankMap.upper_bound(entry);

        //order spells by spells by spellLevel
        std::list<uint32> RankedSpells;
        uint32 min_spell_lvl = 0;
        std::multimap<SpellRankEntry, SpellRankValue, SpellRankEntry>::iterator min_itr;
        for (; RankMap.count(entry);)
        {
            for (std::multimap<SpellRankEntry, SpellRankValue, SpellRankEntry>::iterator itr2 = RankMap.lower_bound(entry); itr2 != RankMap.upper_bound(entry); itr2++)
            {
                SpellEntry const* SpellInfo = sSpellStore.LookupEntry(itr2->second.Id);
                if (SpellInfo->spellLevel < min_spell_lvl || itr2 == RankMap.lower_bound(entry))
                {
                    min_spell_lvl = SpellInfo->spellLevel;
                    min_itr = itr2;
                }
            }
            RankedSpells.push_back(min_itr->second.Id);
            RankMap.erase(min_itr);
        }

        //use data from talent.dbc
        uint16 talent_id = 0;
        for (std::list<uint32>::iterator itr2 = RankedSpells.begin(); itr2 != RankedSpells.end();)
        {
            if (TalentSpellPos const* TalentPos = GetTalentSpellPos(*itr2))
            {
                talent_id = TalentPos->talent_id;
                RankedSpells.erase(itr2);
                itr2 = RankedSpells.begin();
            }
            else
                itr2++;
        }
        if (talent_id)
        {
            TalentEntry const* TalentInfo = sTalentStore.LookupEntry(talent_id);
            for (uint8 rank = 5; rank; rank--)
            {
                if (TalentInfo->RankID[rank - 1])
                    RankedSpells.push_front(TalentInfo->RankID[rank - 1]);
            }
        }

        count++;

        itr = RankMap.upper_bound(entry);
        uint32 spell_rank = 1;
        for (std::list<uint32>::iterator itr2 = RankedSpells.begin(); itr2 != RankedSpells.end(); spell_rank++)
        {
            uint32 spell_id = *itr2;
            mSpellChains[spell_id].rank = spell_rank;
            mSpellChains[spell_id].first = RankedSpells.front();
            mSpellChains[spell_id].last = RankedSpells.back();

            itr2++;

            if (spell_rank < 2)
                mSpellChains[spell_id].prev = 0;

            if (spell_id == RankedSpells.back())
                mSpellChains[spell_id].next = 0;
            else
            {
                mSpellChains[*itr2].prev = spell_id;
                mSpellChains[spell_id].next = *itr2;
            }
        }
    }

    QueryResult_AutoPtr result = WorldDatabase.PQuery("SELECT `spell_id`, `prev_spell_id`,`next_spell_id`,`first_spell_id`,`last_spell_id`, `rank` FROM `spell_ranks`");

    if (!result)
        return;

    std::vector<uint32> spell_id;
    std::vector<uint32> prev_spell_id;
    std::vector < uint32> next_spell_id;
    std::vector<uint32>first_spell_id;
    std::vector<uint32>last_spell_id;
    std::vector<uint32>rank;

    do
    {
        Field* fields = result->Fetch();
        spell_id.push_back(fields[0].GetUInt32());
        prev_spell_id.push_back(fields[1].GetUInt32());
        next_spell_id.push_back(fields[2].GetUInt32());
        first_spell_id.push_back(fields[3].GetUInt32());
        last_spell_id.push_back(fields[4].GetUInt32());
        rank.push_back(fields[5].GetUInt32());

    } while (result->NextRow());

    for (int8 i = 0; i < spell_id.size(); ++i)
    {
        mSpellChains[spell_id[i]].prev = prev_spell_id[i];
        mSpellChains[spell_id[i]].next = next_spell_id[i];
        mSpellChains[spell_id[i]].first = first_spell_id[i];
        mSpellChains[spell_id[i]].last = last_spell_id[i];
        mSpellChains[spell_id[i]].rank = rank[i];
    }


    //uncomment these two lines to print yourself list of spell_chains on startup
    //    for (UNORDERED_MAP<uint32, SpellChainNode>::iterator itr=mSpellChains.begin();itr != mSpellChains.end();itr++)
    //       sLog.outString("Id: %u, Rank: %d , %s",itr->first,itr->second.rank, sSpellStore.LookupEntry(itr->first)->Rank[sWorld.GetDefaultDbcLocale()]);

    sLog.outString(">> Loaded %u spell chains", count);
}

void SpellMgr::LoadSpellLearnSkills()
{
    mSpellLearnSkills.clear();                              // need for reload case

    // search auto-learned skills and add its to map also for use in unlearn spells/talents
    uint32 dbc_count = 0;
    for (uint32 spell = 0; spell < sSpellStore.GetNumRows(); ++spell)
    {
        SpellEntry const* entry = sSpellStore.LookupEntry(spell);

        if (!entry)
            continue;

        for (int i = 0; i < MAX_SPELL_EFFECTS; ++i)
        {
            if (entry->Effect[i] == SPELL_EFFECT_SKILL)
            {
                SpellLearnSkillNode dbc_node;
                dbc_node.skill    = entry->EffectMiscValue[i];
                if (dbc_node.skill != SKILL_RIDING)
                    dbc_node.value    = 1;
                else
                    dbc_node.value    = (entry->EffectBasePoints[i] + 1) * 75;
                dbc_node.maxvalue = (entry->EffectBasePoints[i] + 1) * 75;

                mSpellLearnSkills[spell] = dbc_node;
                ++dbc_count;
                break;
            }
        }
    }

    sLog.outString(">> Loaded %u Spell Learn Skills from DBC", dbc_count);
}

void SpellMgr::LoadSpellLearnSpells()
{
    mSpellLearnSpells.clear();                              // need for reload case

    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT entry, SpellID FROM spell_learn_spell");
    if (!result)
    {

        sLog.outString(">> Loaded 0 spell learn spells");
        sLog.outErrorDb("spell_learn_spell table is empty!");
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();

        uint32 spell_id    = fields[0].GetUInt32();

        SpellLearnSpellNode node;
        node.spell      = fields[1].GetUInt32();
        node.autoLearned = false;

        if (!sSpellStore.LookupEntry(spell_id))
        {
            sLog.outErrorDb("Spell %u listed in spell_learn_spell does not exist", spell_id);
            continue;
        }

        if (!sSpellStore.LookupEntry(node.spell))
        {
            sLog.outErrorDb("Spell %u listed in spell_learn_spell does not exist", node.spell);
            continue;
        }

        mSpellLearnSpells.insert(SpellLearnSpellMap::value_type(spell_id, node));

        ++count;
    }
    while (result->NextRow());

    // search auto-learned spells and add its to map also for use in unlearn spells/talents
    uint32 dbc_count = 0;
    for (uint32 spell = 0; spell < sSpellStore.GetNumRows(); ++spell)
    {
        SpellEntry const* entry = sSpellStore.LookupEntry(spell);

        if (!entry)
            continue;

        for (int i = 0; i < MAX_SPELL_EFFECTS; ++i)
        {
            if (entry->Effect[i] == SPELL_EFFECT_LEARN_SPELL)
            {
                SpellLearnSpellNode dbc_node;
                dbc_node.spell       = entry->EffectTriggerSpell[i];
                dbc_node.autoLearned = true;

                SpellLearnSpellMap::const_iterator db_node_begin = GetBeginSpellLearnSpell(spell);
                SpellLearnSpellMap::const_iterator db_node_end   = GetEndSpellLearnSpell(spell);

                bool found = false;
                for (SpellLearnSpellMap::const_iterator itr = db_node_begin; itr != db_node_end; ++itr)
                {
                    if (itr->second.spell == dbc_node.spell)
                    {
                        sLog.outErrorDb("Spell %u auto-learn spell %u in spell.dbc then the record in spell_learn_spell is redundant, please fix DB.",
                                        spell, dbc_node.spell);
                        found = true;
                        break;
                    }
                }

                if (!found)                                  // add new spell-spell pair if not found
                {
                    mSpellLearnSpells.insert(SpellLearnSpellMap::value_type(spell, dbc_node));
                    ++dbc_count;
                }
            }
        }
    }

    sLog.outString(">> Loaded %u spell learn spells + %u found in DBC", count, dbc_count);
}

void SpellMgr::LoadSpellPetAuras()
{
    mSpellPetAuraMap.clear();                                  // need for reload case

    uint32 count = 0;

    //                                                       0      1    2
    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT spell, pet, aura FROM spell_pet_auras");
    if (!result)
    {
        sLog.outString(">> Loaded %u spell pet auras", count);
        return;
    }


    do
    {
        Field* fields = result->Fetch();

        uint16 spell = fields[0].GetUInt16();
        uint16 pet = fields[1].GetUInt16();
        uint16 aura = fields[2].GetUInt16();

        SpellPetAuraMap::iterator itr = mSpellPetAuraMap.find(spell);
        if (itr != mSpellPetAuraMap.end())
            itr->second.AddAura(pet, aura);
        else
        {
            SpellEntry const* spellInfo = sSpellStore.LookupEntry(spell);
            if (!spellInfo)
            {
                sLog.outErrorDb("Spell %u listed in spell_pet_auras does not exist", spell);
                continue;
            }
            int i = 0;
            for (; i < MAX_SPELL_EFFECTS; ++i)
                if ((spellInfo->Effect[i] == SPELL_EFFECT_APPLY_AURA &&
                     spellInfo->EffectApplyAuraName[i] == SPELL_AURA_DUMMY) ||
                    spellInfo->Effect[i] == SPELL_EFFECT_DUMMY)
                    break;

            if (i == 3)
            {
                sLog.outError("Spell %u listed in spell_pet_auras does not have dummy aura or dummy effect", spell);
                continue;
            }

            SpellEntry const* spellInfo2 = sSpellStore.LookupEntry(aura);
            if (!spellInfo2)
            {
                sLog.outErrorDb("Aura %u listed in spell_pet_auras does not exist", aura);
                continue;
            }

            PetAura pa(pet, aura, spellInfo->EffectImplicitTargetA[i] == TARGET_UNIT_PET, spellInfo->EffectBasePoints[i] + spellInfo->EffectBaseDice[i]);
            mSpellPetAuraMap[spell] = pa;
        }

        ++count;
    }
    while (result->NextRow());

    sLog.outString(">> Loaded %u spell pet auras", count);
}

// set data in core for now
void SpellMgr::LoadSpellCustomAttr()
{
    mSpellCustomAttr.resize(GetSpellStore()->GetNumRows());

    SpellEntry* spellInfo;
    for (uint32 i = 0; i < GetSpellStore()->GetNumRows(); ++i)
    {
        mSpellCustomAttr[i] = 0;
        spellInfo = (SpellEntry*)GetSpellStore()->LookupEntry(i);
        if (!spellInfo)
            continue;

        bool auraSpell = true;
        for (uint8 j = 0; j < MAX_SPELL_EFFECTS; ++j)
        {
            if (spellInfo->Effect[j])
                if (spellInfo->Effect[j] != SPELL_EFFECT_APPLY_AURA
                    || SpellTargetType[spellInfo->EffectImplicitTargetA[j]] != TARGET_TYPE_UNIT_TARGET)
                    //ignore target party for now
                {
                    auraSpell = false;
                    break;
                }
        }
        if (auraSpell)
            mSpellCustomAttr[i] |= SPELL_ATTR_CU_AURA_SPELL;

        for (uint32 j = 0; j < MAX_SPELL_EFFECTS; ++j)
        {
            switch (spellInfo->EffectApplyAuraName[j])
            {
            case SPELL_AURA_PERIODIC_DAMAGE:
            case SPELL_AURA_PERIODIC_DAMAGE_PERCENT:
            case SPELL_AURA_PERIODIC_LEECH:
                mSpellCustomAttr[i] |= SPELL_ATTR_CU_AURA_DOT;
                break;
            case SPELL_AURA_PERIODIC_HEAL:
            case SPELL_AURA_OBS_MOD_HEALTH:
                mSpellCustomAttr[i] |= SPELL_ATTR_CU_AURA_HOT;
                break;
            case SPELL_AURA_MOD_ROOT:
            case SPELL_AURA_MOD_DECREASE_SPEED:
                mSpellCustomAttr[i] |= SPELL_ATTR_CU_MOVEMENT_IMPAIR;
                break;
            default:
                break;
            }

            switch (spellInfo->Effect[j])
            {
            case SPELL_EFFECT_SCHOOL_DAMAGE:
            case SPELL_EFFECT_WEAPON_DAMAGE:
            case SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL:
            case SPELL_EFFECT_NORMALIZED_WEAPON_DMG:
            case SPELL_EFFECT_WEAPON_PERCENT_DAMAGE:
            case SPELL_EFFECT_HEAL:
                mSpellCustomAttr[i] |= SPELL_ATTR_CU_DIRECT_DAMAGE;
                break;
            case SPELL_EFFECT_CHARGE:
            case SPELL_EFFECT_CHARGE_DEST:
                if (!spellInfo->speed && !spellInfo->SpellFamilyName)
                    spellInfo->speed = SPEED_CHARGE;
                mSpellCustomAttr[i] |= SPELL_ATTR_CU_CHARGE;
                break;
            case SPELL_EFFECT_PICKPOCKET:
                mSpellCustomAttr[i] |= SPELL_ATTR_CU_PICKPOCKET;
                break;
            case SPELL_EFFECT_TRIGGER_SPELL:
                if (IsPositionTarget(spellInfo->EffectImplicitTargetA[j]) ||
                    spellInfo->Targets & (TARGET_FLAG_SOURCE_LOCATION | TARGET_FLAG_DEST_LOCATION))
                    spellInfo->Effect[j] = SPELL_EFFECT_TRIGGER_MISSILE;
                break;
            case SPELL_EFFECT_SELF_RESURRECT:
                // Self-Ressurect spells shouldn't be usable in arenas
                spellInfo->AttributesEx4 |= SPELL_ATTR4_NOT_USABLE_IN_ARENA;
                break;
            }
        }

        for (uint8 j = 0; j < MAX_SPELL_EFFECTS; ++j)
        {
            switch (spellInfo->EffectApplyAuraName[j])
            {
            case SPELL_AURA_MOD_POSSESS:
            case SPELL_AURA_MOD_CONFUSE:
            case SPELL_AURA_MOD_CHARM:
            case SPELL_AURA_MOD_FEAR:
            case SPELL_AURA_MOD_STUN:
                mSpellCustomAttr[i] |= SPELL_ATTR_CU_AURA_CC;
                mSpellCustomAttr[i] &= ~SPELL_ATTR_CU_MOVEMENT_IMPAIR;
                break;
            }
        }

        for (uint32 j = 0; j < 16; j++)
        {
            if (spellInfo->ToolTip[j] && *spellInfo->ToolTip[j])
            {
                spellInfo->AttributesEx3 |= SPELL_ATTR3_HAS_VISUAL_EFFECT;
                break;
            }
        }

        if (spellInfo->SpellVisual == 3879)
            mSpellCustomAttr[i] |= SPELL_ATTR_CU_CONE_BACK;

        // Natural Shapeshifter
        if (spellInfo->Id == 16834 || spellInfo->Id == 16835)
        {
            SpellEntry const* baseInfo = sSpellStore.LookupEntry(16833);
            spellInfo->DurationIndex = baseInfo->DurationIndex;
        }

        switch (spellInfo->Category)
        {
            case SPELL_CATEGORY_CONJURED_POTIONS:
                // these spells have 2min shared cooldown, But the time doesn't seem to be in dbcs.
                // I have really no idea from where client gets the data, perhaps hardcoded like this?
                spellInfo->RecoveryTime = 2 * 60 * IN_MILLISECONDS;
                spellInfo->StartRecoveryTime = 2 * 60 * IN_MILLISECONDS;
                spellInfo->StartRecoveryCategory = 2 * 60 * IN_MILLISECONDS;
                break;
        }

        switch (i)
        {
        case 26029: // dark glare
        case 37433: // spout
        case 43140:
        case 43215: // flame breath
            mSpellCustomAttr[i] |= SPELL_ATTR_CU_CONE_LINE;
            break;
        case 24340:
        case 26558:
        case 28884:     // Meteor
        case 36837:
        case 38903:
        case 41276:     // Meteor
        case 26789:                             // Shard of the Fallen Star
        case 31436:                             // Malevolent Cleave
        case 35181:                             // Dive Bomb
        case 40810:
        case 43267:
        case 43268:     // Saber Lash
        case 42384:                             // Brutal Swipe
        case 45150:                             // Meteor Slash
            mSpellCustomAttr[i] |= SPELL_ATTR_CU_SHARE_DAMAGE;
            switch (i) // Saber Lash Targets
            {
            case 40810:
                spellInfo->MaxAffectedTargets = 3;
                break;
            case 43267:
            case 43268:
                spellInfo->MaxAffectedTargets = 2;
                break;
            }
            break;
        case 44978:
        case 45001:
        case 45002:     // Wild Magic
        case 45004:
        case 45006:
        case 45010:     // Wild Magic
        case 31347: // Doom
        case 41635: // Prayer of Mending
        case 44869: // Spectral Blast
        case 45027: // Revitalize
        case 45976: // Muru Portal Channel
        case 39365: // Thundering Storm
        case 41071: // Raise Dead
        case 41172: // Rapid Shot
        case 40834: // Agonizing Flames
            spellInfo->MaxAffectedTargets = 1;
            break;
        case 36384: // Skartax Purple Beam
            spellInfo->MaxAffectedTargets = 2;
            break;
        case 41357: // L1 Acane Charge
        case 41376: // Spite
        case 39992: // Needle Spine
        case 29576: // Multi-Shot
        case 37790: // Spread Shot
        case 45248: // Shadow Blades
        case 41303: // Soul Drain
            spellInfo->MaxAffectedTargets = 3;
            break;
        case 38310: // Multi-Shot
            spellInfo->MaxAffectedTargets = 4;
            break;
        case 32205: // Place Burning Blade Pyre
            spellInfo->AttributesEx2 |= SPELL_ATTR2_IGNORE_LOS;
            break;
        case 42005: // Bloodboil
        case 38296: // Spitfire Totem
        case 37676: // Insidious Whisper
        case 46008: // Negative Energy
        case 45641: // Fire Bloom
        case 5484:  // Howl Of Terror
        case 46771: // Flame Sear
            spellInfo->MaxAffectedTargets = 5;
            break;
        case 15286: // Vampiric Embrace
        case 34914: // Vampiric Touch
            spellInfo->Attributes |= SPELL_ATTR0_NOT_SHAPESHIFT;
            spellInfo->AttributesEx2 |= SPELL_ATTR2_NOT_NEED_SHAPESHIFT;
            spellInfo->Stances = FORM_SHADOW;
            break;
        case 40827: // Sinful Beam
        case 40859: // Sinister Beam
        case 40860: // Vile Beam
        case 40861: // Wicked Beam
            spellInfo->MaxAffectedTargets = 10;
            break;
        case 46841: // Escape to the Isle of Quel'Denas
            spellInfo->EffectImplicitTargetB[0] = TARGET_UNIT_CASTER;
            break;
        case 8122:
        case 8124:
        case 10888:
        case 10890: // Psychic Scream
        case 12494: // Frostbite
            spellInfo->Attributes |= SPELL_ATTR0_HEARTBEAT_RESIST_CHECK;
            break;
        case 38794:
        case 33711: //Murmur's Touch
            spellInfo->MaxAffectedTargets = 1;
            spellInfo->EffectTriggerSpell[0] = 33760;
            break;
        case 36350: //They Must Burn Bomb Aura (self)
            spellInfo->EffectTriggerSpell[0] = 36325; // They Must Burn Bomb Drop (DND)
            break;
        case 44949: // Whirlwind's offhand attack - @todo remove this (50% weapon damage effect)
            spellInfo->Effect[1] = 0;
            break;
        case 12723: // Sweeping Strikes proc
            mSpellCustomAttr[i] |= SPELL_ATTR_CU_IGNORE_ARMOR;
            spellInfo->Attributes |= SPELL_ATTR0_IMPOSSIBLE_DODGE_PARRY_BLOCK;
            break;
        case 24905: // Moonkin form -> elune's touch
            spellInfo->EffectImplicitTargetA[2] = TARGET_UNIT_CASTER;
            break;
        case 7922:                        // Charge stun
        case 25274:                       // Intercept stun
        case 2094:                        // Blind
        case 20424:                       // Seal of Command Trigger
            spellInfo->speed = 590.0f;    // Minor delay
            break;
        case 32220:                       // Judgement of Blood
            spellInfo->speed = 5.0f;      // Minor delay
            break;
        case 1833:                        // Cheap Shot
            spellInfo->speed = 1230.0f;   // Tiny delay
            break;
        case 26679:                       // Deadly Throw
            spellInfo->speed = 0;         // Instant
            break;
        case 41013:     // Parasitic Shadowfiend Passive
            spellInfo->EffectApplyAuraName[0] = 4; // proc debuff, and summon infinite fiends
            break;
        case 1543: // Flare
            spellInfo->speed = 0;
            break;
        case 27892:     // To Anchor 1
        case 27928:     // To Anchor 1
        case 27935:     // To Anchor 1
        case 27915:     // Anchor to Skulls
        case 27931:     // Anchor to Skulls
        case 27937:     // Anchor to Skulls
            spellInfo->rangeIndex = 13;
            break;
        case 34580:
            mSpellCustomAttr[i] |= SPELL_ATTR_CU_IGNORE_ARMOR;
            break;
        case 39384:
            spellInfo->Effect[1] = 0; //Delete this effect, could be fixed also by implementing script_targets for each spell effect..
            spellInfo->rangeIndex = 16;//1y, don't know why, but with 5y (dbc value) all fields around are effected too, so somethings wrong in range check for those spells propably..
            break;
        case 6774:
            spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_INITIAL_AGGRO; // slice and dice no longer gives combat or remove stealth
            spellInfo->AttributesEx |= SPELL_ATTR1_NOT_BREAK_STEALTH;
            break;
        case 29200: // Purify Helboar Meat
            spellInfo->EffectImplicitTargetA[0] = TARGET_UNIT_CASTER;
            spellInfo->EffectImplicitTargetB[0] = 0;
            break;
        case 35460: // Fury of the Dreghood Elders
            spellInfo->EffectImplicitTargetA[1] = TARGET_TYPE_UNIT_TARGET;
            break;
        case 4073:  // Mechanical Dragonling
        case 19804: // Arcanite Dragonling
        case 12749: // Mithril Mechanical Dragonling
        case 13258: // Summon Goblin Bomb
        case 13166: // Battle Chicken
            mSpellCustomAttr[i] |= SPELL_ATTR_CU_CAST_BY_ITEM_ONLY;
            break;
        case 13279: // Gnomish Death Ray (Dummy Target)
            spellInfo->EffectImplicitTargetA[0] = TARGET_TYPE_UNIT_TARGET;
            spellInfo->EffectImplicitTargetA[1] = TARGET_TYPE_UNIT_TARGET;
            break;
        case 31828: // Blessed Life (Rank 1)
        case 31829: // Blessed Life (Rank 2)
        case 31830: // Blessed Life (Rank 3)
            spellInfo->EffectApplyAuraName[0] = SPELL_AURA_REUSED_BLESSED_LIFE;
            break;
        case 39095: // Amplify Damage (Prince Malchaazar)
            spellInfo->EffectImplicitTargetA[0] = TARGET_UNIT_TARGET_ENEMY;
            spellInfo->EffectImplicitTargetB[0] = 0;
            break;
        case 42389: // Nalorakk's Mangle
            spellInfo->EffectImplicitTargetA[1] = TARGET_UNIT_TARGET_ENEMY;
            spellInfo->EffectImplicitTargetB[1] = 0;
            break;
        case 29955: // "Arcane Missiles implict correctly target"
            spellInfo->EffectImplicitTargetA[0] = TARGET_UNIT_CASTER;
            break;
        case 33206: // "Pain Suppression dispel resistance"
            spellInfo->AttributesEx4 |= SPELL_ATTR4_NOT_STEALABLE;
            break;
        case 19970: // Entangling Roots (Rank 6) -- Nature's Grasp Proc
        case 19971: // Entangling Roots (Rank 5) -- Nature's Grasp Proc
        case 19972: // Entangling Roots (Rank 4) -- Nature's Grasp Proc
        case 19973: // Entangling Roots (Rank 3) -- Nature's Grasp Proc
        case 19974: // Entangling Roots (Rank 2) -- Nature's Grasp Proc
        case 19975: // Entangling Roots (Rank 1) -- Nature's Grasp Proc
        case 27010: // Entangling Roots (Rank 7) -- Nature's Grasp Proc
            spellInfo->CastingTimeIndex = 1;
            break;
        case 19596: // Bestial Swiftness - append additional aura
            spellInfo->EffectApplyAuraName[0] = SPELL_AURA_REUSED_INCREASE_PET_OUTDOOR_SPEED;
            break;
        case 1206: // "REUSE" - we turn it into 30% outdoor movement speed bonus (for Bestial Swiftness)
            spellInfo->Effect[0] = SPELL_EFFECT_APPLY_AURA;
            spellInfo->EffectApplyAuraName[0] = SPELL_AURA_MOD_SPEED_NOT_STACK;
            spellInfo->EffectBasePoints[0] = 29; // all spells claiming they increase speed by 30% increase it just by 29%
            spellInfo->Attributes = (SPELL_ATTR0_PASSIVE | SPELL_ATTR0_OUTDOORS_ONLY);
            break;
        case 31789: // Righteous Defense
            spellInfo->EffectTriggerSpell[1] = 31790;
            break;
        case 47129: // Totemic Beacon
            spellInfo->EffectImplicitTargetA[1] = TARGET_NONE;
            break;
        case 40255: // Molten Flame
            // Molten Fire triggers itself, resulting in infinite cycling,
            // memory eating and a non-avoidable crash
            spellInfo->Effect[1] = 0;
            break;
        // Explosive Trap - fix initial dmg and proc
        case 13812: // rank 1
        case 14314: // rank 2
        case 14315: // rank 3
        case 27026: // rank 4
            spellInfo->EffectImplicitTargetA[0] = TARGET_UNIT_TARGET_ENEMY;
            break;
        // Frost Trap & Snake Trap - fix proc
        case 13810:
        case 45145:
            spellInfo->Effect[2] = SPELL_EFFECT_DUMMY;
            spellInfo->EffectImplicitTargetA[2] = TARGET_UNIT_TARGET_ENEMY;
            break;
        case 33778: // Lifebloom (rank 1) 
        case   379: // Earth Shield
        case 38395: // Siphon Essence
        case 34299: // Improved Leader of the Pack (heal)
        case 27813: // Blessed Recovery rank 1
        case 27817: // Blessed Recovery rank 2
        case 27818: // Blessed Recovery rank 3
        case 28880: // Gift of Narru
            mSpellCustomAttr[i] |= SPELL_ATTR_CU_FIXED_AMOUNT;
            break;
        case 35372: // Protectorate Igniter
            spellInfo->EffectImplicitTargetA[0] = TARGET_UNIT_TARGET_ENEMY;
            mSpellCustomAttr[i] |= SPELL_ATTR_CU_ANY_TARGET;
            break;
        case 30298: // Tree Disguise
            spellInfo->InterruptFlags |= SPELL_INTERRUPT_FLAG_MOVEMENT;
            break;
        case 45391: // Vapor Select
            spellInfo->AttributesEx3 |= SPELL_ATTR3_CANT_MISS;
            spellInfo->MaxAffectedTargets = 1;
            break;
        case 45399: // Demonic Vapor Trail Periodic
            spellInfo->rangeIndex = 2;
            break;
        case 28730: // Arcane Torrent (mana)
            spellInfo->EffectImplicitTargetA[1] = TARGET_UNIT_CASTER;
            break;
        case 25046: // Arcane Torrent (energy)
            spellInfo->Effect[1] = SPELL_EFFECT_DUMMY;
            spellInfo->EffectImplicitTargetA[1] = TARGET_UNIT_CASTER;
            break;
        case 23505: // Berserking
        case 23451: // Speed
            spellInfo->Attributes |= SPELL_ATTR0_NEGATIVE_1;
            break;
        case 20271: // Paladin's Judgement
            spellInfo->AttributesEx3 &= ~SPELL_ATTR3_CANT_TRIGGER_PROC;
            break;
        case 1953: // Blink
            spellInfo->DurationIndex = 328; // 250ms
            break;
        case 33240: // Infernal
            spellInfo->EffectImplicitTargetA[EFFECT_0] = TARGET_DEST_TARGET_ANY;
            spellInfo->EffectImplicitTargetA[EFFECT_1] = TARGET_DEST_TARGET_ANY;
            spellInfo->EffectImplicitTargetA[EFFECT_2] = TARGET_DEST_TARGET_ANY;
            break;
        case 34444: // Summon Khadgar's Servent
            spellInfo->DurationIndex = 30;
            break;
        default:
            break;
        }

        switch (spellInfo->SpellFamilyName)
        {
        case SPELLFAMILY_WARRIOR:
            // Shout
            if (spellInfo->SpellFamilyFlags & 0x0000000000020000LL)
                mSpellCustomAttr[i] |= SPELL_ATTR_CU_AURA_CC;
            break;
        case SPELLFAMILY_DRUID:
            // Roar
            if (spellInfo->SpellFamilyFlags & 0x0000000800000000LL)
                mSpellCustomAttr[i] |= SPELL_ATTR_CU_AURA_CC;
            // Rake
            else if (spellInfo->SpellFamilyFlags & 0x1000LL)
                mSpellCustomAttr[i] |= SPELL_ATTR_CU_IGNORE_ARMOR;
            break;
        }
    }

    /* Fix some weird values in SpellItemEnchantment */
    for (uint32 i = 0; i <= 3340; i++)
    {
        if (SpellItemEnchantmentEntry* ench = const_cast<SpellItemEnchantmentEntry*>(sSpellItemEnchantmentStore.LookupEntry(i)))
        {
            switch (ench->ID)
            {
            // Flametongue Weapon
            case    5:
                ench->type[0] = ITEM_ENCHANTMENT_TYPE_COMBAT_SPELL;
                ench->spellid[0] =  8026;
                break; // rank 1
            case    4:
                ench->type[0] = ITEM_ENCHANTMENT_TYPE_COMBAT_SPELL;
                ench->spellid[0] =  8028;
                break; // rank 2
            case    3:
                ench->type[0] = ITEM_ENCHANTMENT_TYPE_COMBAT_SPELL;
                ench->spellid[0] =  8029;
                break; // rank 3
            case  523:
                ench->type[0] = ITEM_ENCHANTMENT_TYPE_COMBAT_SPELL;
                ench->spellid[0] = 10445;
                break; // rank 4
            case 1665:
                ench->type[0] = ITEM_ENCHANTMENT_TYPE_COMBAT_SPELL;
                ench->spellid[0] = 16343;
                break; // rank 5
            case 1666:
                ench->type[0] = ITEM_ENCHANTMENT_TYPE_COMBAT_SPELL;
                ench->spellid[0] = 16344;
                break; // rank 6
            case 2634:
                ench->type[0] = ITEM_ENCHANTMENT_TYPE_COMBAT_SPELL;
                ench->spellid[0] = 25488;
                break; // rank 7
            }
        }
    }

    CreatureAI::FillAISpellInfo();
}

void SpellMgr::LoadSpellCustomCooldowns()
{
    uint32 count = 0;
    SpellEntry* spellInfo;

    //                                                       0              1
    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT spellid, cooldown FROM spell_cooldown");
    if (!result)
    {
        sLog.outString(">> Loaded %u custom spell cooldowns", count);
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        int32 spellid = fields[0].GetInt32();
        uint32 cooldown = fields[1].GetUInt32();

        spellInfo = (SpellEntry*)GetSpellStore()->LookupEntry(spellid);
        if (!spellInfo)
        {
            sLog.outErrorDb("Spell %i listed in spell_cooldown does not exist", spellid);
            continue;
        }

        if (spellid > 0 && cooldown > 0)
            spellInfo->RecoveryTime = cooldown;

        ++count;
    }
    while (result->NextRow());

    sLog.outString(">> Loaded %u custom spell cooldowns", count);
}

void SpellMgr::LoadSpellLinked()
{
    mSpellLinkedMap.clear();    // need for reload case
    uint32 count = 0;

    //                                                       0              1             2
    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT spell_trigger, spell_effect, type FROM spell_linked_spell");
    if (!result)
    {
        sLog.outString(">> Loaded %u linked spells", count);
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        int32 trigger = fields[0].GetInt32();
        int32 effect = fields[1].GetInt32();
        int32 type = fields[2].GetInt32();

        SpellEntry const* spellInfo = sSpellStore.LookupEntry(abs(trigger));
        if (!spellInfo)
        {
            sLog.outErrorDb("Spell Trigger %i listed in spell_linked_spell does not exist", trigger);
            continue;
        }
        spellInfo = sSpellStore.LookupEntry(abs(effect));
        if (!spellInfo)
        {
            sLog.outErrorDb("Spell Effect %i listed in spell_linked_spell does not exist", effect);
            continue;
        }

        if (trigger > 0)
        {
            switch (type)
            {
            case 0:
                mSpellCustomAttr[trigger] |= SPELL_ATTR_CU_LINK_CAST;
                break;
            case 1:
                mSpellCustomAttr[trigger] |= SPELL_ATTR_CU_LINK_HIT;
                break;
            case 2:
                mSpellCustomAttr[trigger] |= SPELL_ATTR_CU_LINK_AURA;
                break;
            }
        }
        else
            mSpellCustomAttr[-trigger] |= SPELL_ATTR_CU_LINK_REMOVE;

        if (type) //we will find a better way when more types are needed
        {
            if (trigger > 0)
                trigger += SPELL_LINKED_MAX_SPELLS * type;
            else
                trigger -= SPELL_LINKED_MAX_SPELLS * type;
        }
        mSpellLinkedMap[trigger].push_back(effect);

        ++count;
    }
    while (result->NextRow());

    sLog.outString(">> Loaded %u linked spells", count);
}

// Some checks for spells, to prevent adding depricated/broken spells for trainers, spell book, etc
bool SpellMgr::IsSpellValid(SpellEntry const* spellInfo, Player* pl, bool msg)
{
    // not exist
    if (!spellInfo)
        return false;

    bool need_check_reagents = false;

    // check effects
    for (int i = 0; i < 3; ++i)
    {
        switch (spellInfo->Effect[i])
        {
        case 0:
            continue;

        // craft spell for crafting non-existed item (break client recipes list show)
        case SPELL_EFFECT_CREATE_ITEM:
            {
                if (!ObjectMgr::GetItemTemplate(spellInfo->EffectItemType[i]))
                {
                    if (msg)
                    {
                        if (pl)
                            ChatHandler(pl).PSendSysMessage("Craft spell %u create not-exist in DB item (Entry: %u) and then...", spellInfo->Id, spellInfo->EffectItemType[i]);
                        else
                            sLog.outErrorDb("Craft spell %u create not-exist in DB item (Entry: %u) and then...", spellInfo->Id, spellInfo->EffectItemType[i]);
                    }
                    return false;
                }

                need_check_reagents = true;
                break;
            }
        case SPELL_EFFECT_LEARN_SPELL:
            {
                SpellEntry const* spellInfo2 = sSpellStore.LookupEntry(spellInfo->EffectTriggerSpell[i]);
                if (!IsSpellValid(spellInfo2, pl, msg))
                {
                    if (msg)
                    {
                        if (pl)
                            ChatHandler(pl).PSendSysMessage("Spell %u learn to broken spell %u, and then...", spellInfo->Id, spellInfo->EffectTriggerSpell[i]);
                        else
                            sLog.outErrorDb("Spell %u learn to invalid spell %u, and then...", spellInfo->Id, spellInfo->EffectTriggerSpell[i]);
                    }
                    return false;
                }
                break;
            }
        }
    }

    if (need_check_reagents)
    {
        for (int j = 0; j < 8; ++j)
        {
            if (spellInfo->Reagent[j] > 0 && !ObjectMgr::GetItemTemplate(spellInfo->Reagent[j]))
            {
                if (msg)
                {
                    if (pl)
                        ChatHandler(pl).PSendSysMessage("Craft spell %u has invalid reagent in DB item (Entry: %u) and then...", spellInfo->Id, spellInfo->Reagent[j]);
                    else
                        sLog.outErrorDb("Craft spell %u has invalid reagent in DB item (Entry: %u) and then...", spellInfo->Id, spellInfo->Reagent[j]);
                }
                return false;
            }
        }
    }

    return true;
}

SpellCastResult IsSpellAllowedInLocation(SpellEntry const* spellInfo, uint32 map_id, uint32 zone_id, uint32 area_id)
{
    // normal case
    if (spellInfo->AreaId && spellInfo->AreaId != zone_id && spellInfo->AreaId != area_id)
        return SPELL_FAILED_REQUIRES_AREA;

    // continent limitation (virtual continent)
    if (spellInfo->HasAttribute(SPELL_ATTR4_CAST_ONLY_IN_OUTLAND))
    {
        uint32 v_map = GetVirtualMapForMapAndZone(map_id, zone_id);
        MapEntry const* mapEntry = sMapStore.LookupEntry(v_map);
        if (!mapEntry || mapEntry->addon < 1 || !mapEntry->IsContinent())
            return SPELL_FAILED_REQUIRES_AREA;
    }

    // raid instance limitation
    if (spellInfo->HasAttribute(SPELL_ATTR6_NOT_IN_RAID_INSTANCE))
    {
        MapEntry const* mapEntry = sMapStore.LookupEntry(map_id);
        if (!mapEntry || mapEntry->IsRaid())
            return SPELL_FAILED_TARGET_NOT_IN_RAID;
    }

    // special cases zone check (maps checked by multimap common id)
    switch (spellInfo->Id)
    {
    case 46838:                                         // Shattrath Flask of Pure Death
    case 41607:                                         // Shattrath Flask of Fortification
    case 41605:                                         // Shattrath Flask of Mighty Restoration
    case 41604:                                         // Shattrath Flask of Supreme Power
    case 41606:                                         // Shattrath Flask of Relentless Assault
    case 46840:                                         // Shattrath Flask of Blinding Light
    {
        MapEntry const* mapEntry = sMapStore.LookupEntry(map_id);
        if (!mapEntry)
            return SPELL_FAILED_REQUIRES_AREA;

                        // Tempest Keep        // Black Temple          // Serpentshrine Cavern     // Sunwell               // Mount Hyjal
        if (mapEntry->multimap_id == 206 || mapEntry->MapID == 564 || mapEntry->MapID == 548 || mapEntry->MapID == 580 || mapEntry->MapID == 534)
            return SPELL_CAST_OK;
        else
            return SPELL_FAILED_DONT_REPORT;
    }
    case 23333:                                         // Warsong Flag
    case 23335:                                         // Silverwing Flag
    case 46392:                                         // Focused Assault
    case 46393:                                         // Brutal Assault
        {
            MapEntry const* mapEntry = sMapStore.LookupEntry(map_id);
            if (!mapEntry)
                return SPELL_FAILED_REQUIRES_AREA;

            if (!mapEntry->IsBattleground())
                return SPELL_FAILED_REQUIRES_AREA;

            if (zone_id == 3277)
                return SPELL_CAST_OK;

            return SPELL_FAILED_REQUIRES_AREA;
        }
    case 34976:                                         // Netherstorm Flag
        {
            MapEntry const* mapEntry = sMapStore.LookupEntry(map_id);
            if (!mapEntry)
                return SPELL_FAILED_REQUIRES_AREA;

            if (!mapEntry->IsBattleground())
                return SPELL_FAILED_REQUIRES_AREA;

            if (zone_id == 3820)
                return SPELL_CAST_OK;

            return SPELL_FAILED_REQUIRES_AREA;
        }
    case 32724:                                         // Gold Team (Alliance)
    case 32725:                                         // Green Team (Alliance)
    case 32727:                                         // Arena Preparation
    case 35774:                                         // Gold Team (Horde)
    case 35775:                                         // Green Team (Horde)
        {
            MapEntry const* mapEntry = sMapStore.LookupEntry(map_id);
            if (!mapEntry)
                return SPELL_FAILED_REQUIRES_AREA;

            //the follow code doesn't work.
            //if (!mapEntry->IsBattleArena())
            //    return false;

            //this is the working code, HACK
            if (zone_id == 3702 || zone_id == 3968 || zone_id == 3698)
                return SPELL_CAST_OK;

            return SPELL_FAILED_REQUIRES_AREA;
        }
    case 41618:                                         // Bottled Nethergon Energy
    case 41620:                                         // Bottled Nethergon Vapor
        {
            MapEntry const* mapEntry = sMapStore.LookupEntry(map_id);
            if (!mapEntry)
                return SPELL_FAILED_REQUIRES_AREA;

            return mapEntry->multimap_id == 206 ? SPELL_CAST_OK : SPELL_FAILED_REQUIRES_AREA;
        }
    case 41617:                                         // Cenarion Mana Salve
    case 41619:                                         // Cenarion Healing Salve
        {
            MapEntry const* mapEntry = sMapStore.LookupEntry(map_id);
            if (!mapEntry)
                return SPELL_FAILED_REQUIRES_AREA;

            return mapEntry->multimap_id == 207 ? SPELL_CAST_OK : SPELL_FAILED_REQUIRES_AREA;
        }
    case 40216:                                         // Dragonmaw Illusion
    case 42016:                                         // Dragonmaw Illusion
        return (area_id == 3759 || area_id == 3966 || area_id == 3939) ? SPELL_CAST_OK : SPELL_FAILED_REQUIRES_AREA;
    case 2584:                                          // Waiting to Resurrect
    case 22011:                                         // Spirit Heal Channel
    case 22012:                                         // Spirit Heal
    case 24171:                                         // Resurrection Impact Visual
    case 42792:                                         // Recently Dropped Flag
    case 43681:                                         // Inactive
    case 44535:                                         // Spirit Heal (mana)
    case 44521:                                         // Preparation
        {
            MapEntry const* mapEntry = sMapStore.LookupEntry(map_id);
            if (!mapEntry)
                return SPELL_FAILED_REQUIRES_AREA;

            if (!mapEntry->IsBattleground())
                return SPELL_FAILED_REQUIRES_AREA;
        }
    }

    return SPELL_CAST_OK;
}

void SpellMgr::LoadSkillLineAbilityMap()
{
    mSkillLineAbilityMap.clear();

    uint32 count = 0;

    for (uint32 i = 0; i < sSkillLineAbilityStore.GetNumRows(); i++)
    {
        SkillLineAbilityEntry const* SkillInfo = sSkillLineAbilityStore.LookupEntry(i);
        if (!SkillInfo)
            continue;

        mSkillLineAbilityMap.insert(SkillLineAbilityMap::value_type(SkillInfo->spellId, SkillInfo));
        ++count;
    }

    sLog.outString(">> Loaded %u SkillLineAbility MultiMap", count);
}

DiminishingGroup GetDiminishingReturnsGroupForSpell(SpellEntry const* spellproto, bool triggered)
{
    if (!spellproto || IsPositiveSpell(spellproto->Id))
        return DIMINISHING_NONE;

    // Explicit Diminishing Groups
    switch (spellproto->SpellFamilyName)
    {
    // Event + General spells
    case SPELLFAMILY_UNK1:
        return DIMINISHING_NONE;
    case SPELLFAMILY_MAGE:
        {
            // Item "Magic Dust" shouldn't be limited in PvP and
            // does not share DR with other sleep mechanics.
             if (spellproto->SpellIconID == 44)
                return DIMINISHING_NONE;
            break;
            // Frost Nova / Freeze (Water Elemental)
            if (spellproto->SpellIconID == 193)
                return DIMINISHING_CONTROLLED_ROOT;
            break;
        }
    case SPELLFAMILY_WARRIOR:
        {
            // Hamstring - limit duration to 10s in PvP
            if (spellproto->SpellFamilyFlags & 0x00000000002LL)
                return DIMINISHING_LIMITONLY;
            break;
        }
    case SPELLFAMILY_WARLOCK:
        {
            // Death Coil
            if (spellproto->SpellFamilyFlags & 0x00000080000LL)
                return DIMINISHING_HORROR;
            // Seduction
            else if (spellproto->SpellFamilyFlags & 0x00040000000LL)
                return DIMINISHING_FEAR;
            // Curses/etc
            else if (spellproto->SpellFamilyFlags & 0x00080000000LL)
                return DIMINISHING_LIMITONLY;
            // Unstable affliction dispel silence
            else if (spellproto->Id == 31117)
                return DIMINISHING_SILENCE;
            break;
        }
    case SPELLFAMILY_DRUID:
        {
            // Cyclone
            if (spellproto->SpellFamilyFlags & 0x02000000000LL)
                return DIMINISHING_BLIND_CYCLONE;
            // Nature's Grasp trigger
            if (spellproto->SpellFamilyFlags & 0x00000000200LL && spellproto->Attributes == 0x49010000)
                return DIMINISHING_CONTROLLED_ROOT;
            break;
        }
    case SPELLFAMILY_ROGUE:
        {
            // Kidney Shot
            if (spellproto->SpellFamilyFlags & 0x00000200000LL)
                return DIMINISHING_KIDNEYSHOT;
            // Gouge
            else if (spellproto->SpellFamilyFlags & 0x00000000008LL)
                return DIMINISHING_DISORIENT;
            // Blind
            else if (spellproto->SpellFamilyFlags & 0x00001000000LL)
                return DIMINISHING_BLIND_CYCLONE;
            break;
        }
    case SPELLFAMILY_HUNTER:
        {
            // Freezing trap
            if (spellproto->SpellFamilyFlags & 0x00000000008LL)
                return DIMINISHING_FREEZE;
            // Intimidation
            else if (spellproto->Id == 24394)
                return DIMINISHING_CONTROLLED_STUN;
            // Entrapment (own diminishing)
            else if (spellproto->SpellVisual == 7484 && spellproto->SpellIconID == 20)
                return DIMINISHING_ENTRAPMENT;
            break;
        }
    case SPELLFAMILY_PALADIN:
        {
            // Turn Evil shared diminishing with fear
            if (spellproto->Id == 10326)
                return DIMINISHING_FEAR;
            break;
        }
    default:
        {
            if (spellproto->Id == 12494) // frostbite
                return DIMINISHING_ROOT;
            break;
        }
    }

    // Lastly - Set diminishing depending on mechanic
    uint32 mechanic = spellproto->GetAllEffectsMechanicMask();
    if (mechanic & (1 << MECHANIC_CHARM))
        return DIMINISHING_MIND_CONTROL;
    if (mechanic & (1 << MECHANIC_SLEEP))
        return DIMINISHING_SLEEP;
    if (mechanic & ((1 << MECHANIC_SAPPED) | (1 << MECHANIC_POLYMORPH) | (1 << MECHANIC_SHACKLE)))
        return DIMINISHING_DISORIENT;
    if (mechanic & (1 << MECHANIC_KNOCKOUT))
        return DIMINISHING_DISORIENT;
    if (mechanic & (1 << MECHANIC_DISARM))
        return DIMINISHING_DISARM;
    if (mechanic & (1 << MECHANIC_FEAR))
        return DIMINISHING_FEAR;
    if (mechanic & (1 << MECHANIC_STUN))
        return triggered ? DIMINISHING_STUN : DIMINISHING_CONTROLLED_STUN;
    if (mechanic & (1 << MECHANIC_BANISH))
        return DIMINISHING_BANISH;
    if (mechanic & (1 << MECHANIC_ROOT))
        return triggered ? DIMINISHING_ROOT : DIMINISHING_CONTROLLED_ROOT;
    if (mechanic & (1 << MECHANIC_FREEZE))
        return DIMINISHING_FREEZE;
    if (mechanic & (1 << MECHANIC_HORROR))
        return DIMINISHING_HORROR;

    return DIMINISHING_NONE;
}

bool IsDiminishingReturnsGroupDurationLimited(DiminishingGroup group)
{
    switch (group)
    {
    case DIMINISHING_CONTROLLED_STUN:
    case DIMINISHING_STUN:
    case DIMINISHING_KIDNEYSHOT:
    case DIMINISHING_SLEEP:
    case DIMINISHING_CONTROLLED_ROOT:
    case DIMINISHING_ROOT:
    case DIMINISHING_FEAR:
    case DIMINISHING_WARLOCK_FEAR:
    case DIMINISHING_MIND_CONTROL:
    case DIMINISHING_DISORIENT:
    case DIMINISHING_FREEZE:
    case DIMINISHING_BLIND_CYCLONE:
    case DIMINISHING_BANISH:
    case DIMINISHING_LIMITONLY:
        return true;
    default:
        break;
    }
    return false;
}

DiminishingReturnsType GetDiminishingReturnsGroupType(DiminishingGroup group)
{
    switch (group)
    {
    case DIMINISHING_BLIND_CYCLONE:
    case DIMINISHING_CONTROLLED_STUN:
    case DIMINISHING_STUN:
    case DIMINISHING_KIDNEYSHOT:
        return DRTYPE_ALL;
    case DIMINISHING_SLEEP:
    case DIMINISHING_CONTROLLED_ROOT:
    case DIMINISHING_ROOT:
    case DIMINISHING_FEAR:
    case DIMINISHING_MIND_CONTROL:
    case DIMINISHING_DISORIENT:
    case DIMINISHING_DISARM:
    case DIMINISHING_HORROR:
    case DIMINISHING_FREEZE:
    case DIMINISHING_BANISH:
    case DIMINISHING_WARLOCK_FEAR:
    case DIMINISHING_SILENCE:
        return DRTYPE_PLAYER;
    default:
        break;
    }

    return DRTYPE_NONE;
}
