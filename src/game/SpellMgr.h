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

#ifndef _SPELLMGR_H
#define _SPELLMGR_H

// For static or at-server-startup loaded spell data
// For more high level function for sSpellStore data

#include "SharedDefines.h"
#include "DBCStructure.h"
#include "DBCStores.h"
#include "Database/SQLStorage.h"
#include "SpellAuraDefines.h"
#include "DisableMgr.h"

#include "Utilities/UnorderedMap.h"
#include "VMapFactory.h"
#include <map>

class Unit;
class Player;
class Spell;

enum SpellFamilyNames
{
    SPELLFAMILY_GENERIC     = 0,
    SPELLFAMILY_UNK1        = 1,                            // events, holidays
    // 2 - unused
    SPELLFAMILY_MAGE        = 3,
    SPELLFAMILY_WARRIOR     = 4,
    SPELLFAMILY_WARLOCK     = 5,
    SPELLFAMILY_PRIEST      = 6,
    SPELLFAMILY_DRUID       = 7,
    SPELLFAMILY_ROGUE       = 8,
    SPELLFAMILY_HUNTER      = 9,
    SPELLFAMILY_PALADIN     = 10,
    SPELLFAMILY_SHAMAN      = 11,
    SPELLFAMILY_UNK2        = 12,
    SPELLFAMILY_POTION      = 13
};

enum SpellEffectTargetTypes
{
    SPELL_REQUIRE_NONE,
    SPELL_REQUIRE_UNIT,
    SPELL_REQUIRE_DEST,
    SPELL_REQUIRE_ITEM,
    SPELL_REQUIRE_CASTER,
};

enum SpellSelectTargetTypes
{
    TARGET_TYPE_DEFAULT,
    TARGET_TYPE_UNIT_CASTER,
    TARGET_TYPE_UNIT_TARGET,
    TARGET_TYPE_UNIT_NEARBY,
    TARGET_TYPE_AREA_SRC,
    TARGET_TYPE_AREA_DST,
    TARGET_TYPE_AREA_CONE,
    TARGET_TYPE_DEST_CASTER,
    TARGET_TYPE_DEST_TARGET,
    TARGET_TYPE_DEST_DEST,
    TARGET_TYPE_DEST_SPECIAL,
    TARGET_TYPE_CHANNEL,
};

//Some SpellFamilyFlags
#define SPELLFAMILYFLAG_ROGUE_VANISH            0x000000800LL
#define SPELLFAMILYFLAG_ROGUE_STEALTH           0x000400000LL
#define SPELLFAMILYFLAG_ROGUE_BACKSTAB          0x000800004LL
#define SPELLFAMILYFLAG_ROGUE_SAP               0x000000080LL
#define SPELLFAMILYFLAG_ROGUE_FEINT             0x008000000LL
#define SPELLFAMILYFLAG_ROGUE_KIDNEYSHOT        0x000200000LL
#define SPELLFAMILYFLAG_ROGUE__FINISHING_MOVE   0x9003E0000LL
#define SPELLFAMILYFLAG_WARRIOR_SUNDERARMOR     0x000004000LL
#define SPELLFAMILYFLAG_SHAMAN_FROST_SHOCK      0x080000000LL

// Spell clasification
enum SpellSpecific
{
    SPELL_NORMAL            = 0,
    SPELL_SEAL              = 1,
    SPELL_AURA              = 3,
    SPELL_STING             = 4,
    SPELL_CURSE             = 5,
    SPELL_ASPECT            = 6,
    SPELL_TRACKER           = 7,
    SPELL_WARLOCK_ARMOR     = 8,
    SPELL_MAGE_ARMOR        = 9,
    SPELL_ELEMENTAL_SHIELD  = 10,
    SPELL_MAGE_POLYMORPH    = 11,
    SPELL_JUDGEMENT         = 13,
    SPELL_WARLOCK_CORRUPTION = 17,
    SPELL_DRINK             = 19,
    SPELL_FOOD              = 20,
    SPELL_CHARM             = 21,
    SPELL_WARRIOR_ENRAGE    = 22,
    SPELL_ARMOR_REDUCE      = 23,
};

#define SPELL_LINKED_MAX_SPELLS  200000

enum SpellLinkedType
{
    SPELL_LINK_CAST     = 0,            // +: cast; -: remove
    SPELL_LINK_HIT      = 1 * 200000,
    SPELL_LINK_AURA     = 2 * 200000,   // +: aura; -: immune
    SPELL_LINK_REMOVE   = 0,
};

SpellSpecific GetSpellSpecific(uint32 spellId);

// Different spell properties
inline float GetSpellRadiusForHostile(SpellRadiusEntry const* radius)
{
    return (radius ? radius->radiusHostile : 0);
}
inline float GetSpellRadiusForFriend(SpellRadiusEntry const* radius)
{
    return (radius ? radius->radiusFriend : 0);
}
uint32 GetSpellCastTime(SpellEntry const* spellInfo, Spell const* spell = NULL);
inline float GetSpellMinRange(SpellRangeEntry const* range)
{
    return (range ? range->minRange : 0);
}
inline float GetSpellMaxRange(SpellRangeEntry const* range)
{
    return (range ? range->maxRange : 0);
}
inline uint32 GetSpellRangeType(SpellRangeEntry const* range)
{
    return (range ? range->type : 0);
}
inline uint32 GetSpellRecoveryTime(SpellEntry const* spellInfo)
{
    return spellInfo->RecoveryTime > spellInfo->CategoryRecoveryTime ? spellInfo->RecoveryTime : spellInfo->CategoryRecoveryTime;
}
int32 GetSpellDuration(SpellEntry const* spellInfo);
int32 GetSpellMaxDuration(SpellEntry const* spellInfo);

inline bool IsSpellIgnoringLOS(SpellEntry const* spellInfo)
{
    if (spellInfo->AttributesEx2 & SPELL_ATTR_EX2_IGNORE_LOS)
        return true;

    if (!VMAP::VMapFactory::checkSpellForLoS(spellInfo->Id))
        return true;

    if (sDisableMgr.IsDisabledFor(DISABLE_TYPE_SPELL, spellInfo->Id, NULL, SPELL_DISABLE_LOS))
        return true;

    // Spells with area destinations also belong here
    if (spellInfo->Targets & (TARGET_FLAG_SOURCE_LOCATION | TARGET_FLAG_DEST_LOCATION))
        return true;

    return false;
}

inline float GetSpellRadius(SpellEntry const* spellInfo, uint32 effectIdx, bool positive)
{
    return positive
           ? GetSpellRadiusForFriend(sSpellRadiusStore.LookupEntry(spellInfo->EffectRadiusIndex[effectIdx]))
           : GetSpellRadiusForHostile(sSpellRadiusStore.LookupEntry(spellInfo->EffectRadiusIndex[effectIdx]));
}

inline float GetSpellMaxRange(SpellEntry const* spellInfo)
{
    return GetSpellMaxRange(sSpellRangeStore.LookupEntry(spellInfo->rangeIndex));
}

inline float GetSpellMinRange(SpellEntry const* spellInfo)
{
    return GetSpellMinRange(sSpellRangeStore.LookupEntry(spellInfo->rangeIndex));
}

inline float GetSpellMinRange(uint32 id)
{
    SpellEntry const* spellInfo = GetSpellStore()->LookupEntry(id);
    if (!spellInfo) return 0;
    return GetSpellMinRange(spellInfo);
}

inline float GetSpellMaxRange(uint32 id)
{
    SpellEntry const* spellInfo = GetSpellStore()->LookupEntry(id);
    if (!spellInfo) return 0;
    return GetSpellMaxRange(spellInfo);
}

inline bool IsSpellHaveEffect(SpellEntry const* spellInfo, SpellEffects effect)
{
    for (int i = 0; i < 3; ++i)
        if (spellInfo->Effect[i] == effect)
            return true;
    return false;
}

inline bool IsSealSpell(SpellEntry const* spellInfo)
{
    //Collection of all the seal family flags. No other paladin spell has any of those.
    return spellInfo->SpellFamilyName == SPELLFAMILY_PALADIN &&
           (spellInfo->SpellFamilyFlags & 0x4000A000200LL) &&
           spellInfo->EffectImplicitTargetA[0] == TARGET_UNIT_CASTER;
}

inline bool IsElementalShield(SpellEntry const* spellInfo)
{
    // family flags 10 (Lightning), 42 (Earth), 37 (Water), proc shield from T2 8 pieces bonus
    return (spellInfo->SpellFamilyFlags & 0x42000000400LL) || spellInfo->Id == 23552;
}

inline bool IsSpellRemoveAllMovementAndControlLossEffects(SpellEntry const* spellProto)
{
    return spellProto->EffectApplyAuraName[EFFECT_0] == SPELL_AURA_MECHANIC_IMMUNITY &&
           spellProto->EffectMiscValue[EFFECT_0] == 1 &&
           spellProto->EffectApplyAuraName[EFFECT_1] == 0 &&
           spellProto->EffectApplyAuraName[EFFECT_2] == 0 &&
           (spellProto->AttributesEx & SPELL_ATTR_EX_DISPEL_AURAS_ON_IMMUNITY);
}

inline bool IsDeathOnlySpell(SpellEntry const* spellInfo)
{
    return spellInfo->AttributesEx3 & SPELL_ATTR_EX3_CAST_ON_DEAD
           || spellInfo->Id == 2584;
}

uint32 CalculatePowerCost(SpellEntry const* spellInfo, Unit const* caster, SpellSchoolMask schoolMask);
int32 CompareAuraRanks(uint32 spellId_1, uint32 effIndex_1, uint32 spellId_2, uint32 effIndex_2);
bool IsSingleFromSpellSpecificPerCaster(uint32 spellSpec1, uint32 spellSpec2);
bool IsSingleFromSpellSpecificPerTarget(uint32 spellSpec1, uint32 spellSpec2);
bool IsPassiveSpell(uint32 spellId);
bool IsPassiveSpell(SpellEntry const* spellInfo);
bool IsAutocastableSpell(uint32 spellId);

inline bool IsDeathPersistentSpell(SpellEntry const* spellInfo)
{
    switch (spellInfo->Id)
    {
        case 40214:         // Dragonmaw Illusion
        case 35480:
        case 35481:
        case 35482:         // Human Illusion
        case 35483:
        case 39824:         // Human Illusion
            return true;
    }

    return spellInfo->AttributesEx3 & SPELL_ATTR_EX3_DEATH_PERSISTENT;
}

inline bool IsNonCombatSpell(SpellEntry const* spellInfo)
{
    return (spellInfo->Attributes & SPELL_ATTR_CANT_USED_IN_COMBAT) != 0;
}

bool IsPositiveSpell(uint32 spellId);
bool IsPositiveEffect(uint32 spellId, uint32 effIndex);
bool IsPositiveTarget(uint32 targetA, uint32 targetB);

inline bool IsSelfCastEffect(SpellEntry const* spellInfo, uint32 eff)
{
    switch (spellInfo->EffectImplicitTargetA[eff])
    {
        case TARGET_NONE:
        case TARGET_UNIT_CASTER:
        case TARGET_UNIT_TARGET_ANY:
            return true;
        default:
            break;
    }

    return false;
}

bool IsSingleTargetSpell(SpellEntry const* spellInfo);
bool IsSingleTargetSpells(SpellEntry const* spellInfo1, SpellEntry const* spellInfo2);

bool IsAuraAddedBySpell(uint32 auraType, uint32 spellId);

bool IsSpellAllowedInLocation(SpellEntry const* spellInfo, uint32 map_id, uint32 zone_id, uint32 area_id);

extern bool IsAreaEffectTarget[TOTAL_SPELL_TARGETS];
inline bool IsAreaOfEffectSpell(SpellEntry const* spellInfo)
{
    if (IsAreaEffectTarget[spellInfo->EffectImplicitTargetA[EFFECT_0]] || IsAreaEffectTarget[spellInfo->EffectImplicitTargetB[EFFECT_0]])
        return true;
    if (IsAreaEffectTarget[spellInfo->EffectImplicitTargetA[EFFECT_1]] || IsAreaEffectTarget[spellInfo->EffectImplicitTargetB[EFFECT_1]])
        return true;
    if (IsAreaEffectTarget[spellInfo->EffectImplicitTargetA[EFFECT_2]] || IsAreaEffectTarget[spellInfo->EffectImplicitTargetB[EFFECT_2]])
        return true;
    return false;
}

inline bool IsFarUnitTargetEffect(uint32 effect)
{
    switch (effect)
    {
        case SPELL_EFFECT_SUMMON_PLAYER:
        case SPELL_EFFECT_RESURRECT_NEW:
            return true;
    }

    return false;
}

inline bool IsFarDestTargetEffect(uint32 effect)
{
    return (effect == SPELL_EFFECT_TELEPORT_UNITS);
}

inline bool IsAreaAuraEffect(uint32 effect)
{
    if (effect == SPELL_EFFECT_APPLY_AREA_AURA_PARTY    ||
        effect == SPELL_EFFECT_APPLY_AREA_AURA_FRIEND   ||
        effect == SPELL_EFFECT_APPLY_AREA_AURA_ENEMY    ||
        effect == SPELL_EFFECT_APPLY_AREA_AURA_PET      ||
        effect == SPELL_EFFECT_APPLY_AREA_AURA_OWNER)
        return true;
    return false;
}
inline bool IsDispel(SpellEntry const* spellInfo)
{
    //spellsteal is also dispel
    return IsSpellHaveEffect(spellInfo, SPELL_EFFECT_DISPEL) ||
           IsSpellHaveEffect(spellInfo, SPELL_EFFECT_DISPEL_MECHANIC);
}
inline bool IsDispelSpell(SpellEntry const* spellInfo)
{
    //spellsteal is also dispel
    if (spellInfo->Effect[0] == SPELL_EFFECT_STEAL_BENEFICIAL_BUFF ||
        spellInfo->Effect[1] == SPELL_EFFECT_STEAL_BENEFICIAL_BUFF ||
        spellInfo->Effect[2] == SPELL_EFFECT_STEAL_BENEFICIAL_BUFF
        || IsDispel(spellInfo))
        return true;
    return false;
}

inline bool isSpellBreakStealth(SpellEntry const* spellInfo)
{
    return !(spellInfo->AttributesEx & SPELL_ATTR_EX_NOT_BREAK_STEALTH);
}

inline bool IsAutoRepeatRangedSpell(SpellEntry const* spellInfo)
{
    return (spellInfo->Attributes & SPELL_ATTR_RANGED) && (spellInfo->AttributesEx2 & SPELL_ATTR_EX2_AUTOREPEAT_FLAG);
}

SpellCastResult GetErrorAtShapeshiftedCast (SpellEntry const* spellInfo, uint32 form);

inline bool IsChanneledSpell(SpellEntry const* spellInfo)
{
    return (spellInfo->AttributesEx & (SPELL_ATTR_EX_CHANNELED_1 | SPELL_ATTR_EX_CHANNELED_2));
}

inline bool NeedsComboPoints(SpellEntry const* spellInfo)
{
    return (spellInfo->AttributesEx & (SPELL_ATTR_EX_REQ_COMBO_POINTS1 | SPELL_ATTR_EX_REQ_COMBO_POINTS2));
}

inline SpellSchoolMask GetSpellSchoolMask(SpellEntry const* spellInfo)
{
    return SpellSchoolMask(spellInfo->SchoolMask);
}

inline uint32 GetSpellMechanicMask(SpellEntry const* spellInfo, int32 effect)
{
    uint32 mask = 0;
    if (spellInfo->Mechanic)
        mask |= 1 << spellInfo->Mechanic;
    if (spellInfo->EffectMechanic[effect])
        mask |= 1 << spellInfo->EffectMechanic[effect];
    return mask;
}

inline Mechanics GetEffectMechanic(SpellEntry const* spellInfo, int32 effect)
{
    if (spellInfo->EffectMechanic[effect])
        return Mechanics(spellInfo->EffectMechanic[effect]);
    if (spellInfo->Mechanic)
        return Mechanics(spellInfo->Mechanic);
    return MECHANIC_NONE;
}

inline uint32 GetDispelMask(DispelType dispel)
{
    // If dispell all
    if (dispel == DISPEL_ALL)
        return DISPEL_ALL_MASK;
    else
        return (1 << dispel);
}

// Diminishing Returns interaction with spells
DiminishingGroup GetDiminishingReturnsGroupForSpell(SpellEntry const* spellproto, bool triggered);
bool IsDiminishingReturnsGroupDurationLimited(DiminishingGroup group);
DiminishingReturnsType GetDiminishingReturnsGroupType(DiminishingGroup group);

// Spell affects related declarations (accessed using SpellMgr functions)
typedef std::map<uint32, uint64> SpellAffectMap;

// Spell proc event related declarations (accessed using SpellMgr functions)
enum ProcFlags
{
    PROC_FLAG_NONE                            = 0x00000000,

    PROC_FLAG_KILLED                          = 0x00000001,    // 00 Killed by agressor - not sure about this flag
    PROC_FLAG_KILL                            = 0x00000002,    // 01 Kill target (in most cases need XP/Honor reward)

    PROC_FLAG_DONE_MELEE_AUTO_ATTACK          = 0x00000004,    // 02 Done melee auto attack
    PROC_FLAG_TAKEN_MELEE_AUTO_ATTACK         = 0x00000008,    // 03 Taken melee auto attack

    PROC_FLAG_DONE_SPELL_MELEE_DMG_CLASS      = 0x00000010,    // 04 Done attack by Spell that has dmg class melee
    PROC_FLAG_TAKEN_SPELL_MELEE_DMG_CLASS     = 0x00000020,    // 05 Taken attack by Spell that has dmg class melee

    PROC_FLAG_DONE_RANGED_AUTO_ATTACK         = 0x00000040,    // 06 Done ranged auto attack
    PROC_FLAG_TAKEN_RANGED_AUTO_ATTACK        = 0x00000080,    // 07 Taken ranged auto attack

    PROC_FLAG_DONE_SPELL_RANGED_DMG_CLASS     = 0x00000100,    // 08 Done attack by Spell that has dmg class ranged
    PROC_FLAG_TAKEN_SPELL_RANGED_DMG_CLASS    = 0x00000200,    // 09 Taken attack by Spell that has dmg class ranged

    PROC_FLAG_DONE_SPELL_NONE_DMG_CLASS_POS   = 0x00000400,    // 10 Done positive spell that has dmg class none
    PROC_FLAG_TAKEN_SPELL_NONE_DMG_CLASS_POS  = 0x00000800,    // 11 Taken positive spell that has dmg class none

    PROC_FLAG_DONE_SPELL_NONE_DMG_CLASS_NEG   = 0x00001000,    // 12 Done negative spell that has dmg class none
    PROC_FLAG_TAKEN_SPELL_NONE_DMG_CLASS_NEG  = 0x00002000,    // 13 Taken negative spell that has dmg class none

    PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_POS  = 0x00004000,    // 14 Done positive spell that has dmg class magic
    PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_POS = 0x00008000,    // 15 Taken positive spell that has dmg class magic

    PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_NEG  = 0x00010000,    // 16 Done negative spell that has dmg class magic
    PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_NEG = 0x00020000,    // 17 Taken negative spell that has dmg class magic

    PROC_FLAG_DONE_PERIODIC                   = 0x00040000,    // 18 Successful do periodic (damage / healing)
    PROC_FLAG_TAKEN_PERIODIC                  = 0x00080000,    // 19 Taken spell periodic (damage / healing)

    PROC_FLAG_TAKEN_DAMAGE                    = 0x00100000,    // 20 Taken any damage
    PROC_FLAG_DONE_TRAP_ACTIVATION            = 0x00200000,    // 21 On trap activation (possibly needs name change to ON_GAMEOBJECT_CAST or USE)

    PROC_FLAG_DONE_MAINHAND_ATTACK            = 0x00400000,    // 22 Done main-hand melee attacks (spell and autoattack)
    PROC_FLAG_DONE_OFFHAND_ATTACK             = 0x00800000,    // 23 Done off-hand melee attacks (spell and autoattack)

    PROC_FLAG_DEATH                           = 0x01000000,    // 24 Died in any way

    // flag masks
    AUTO_ATTACK_PROC_FLAG_MASK                = PROC_FLAG_DONE_MELEE_AUTO_ATTACK | PROC_FLAG_TAKEN_MELEE_AUTO_ATTACK
                                                | PROC_FLAG_DONE_RANGED_AUTO_ATTACK | PROC_FLAG_TAKEN_RANGED_AUTO_ATTACK,

    MELEE_PROC_FLAG_MASK                      = PROC_FLAG_DONE_MELEE_AUTO_ATTACK | PROC_FLAG_TAKEN_MELEE_AUTO_ATTACK
                                                | PROC_FLAG_DONE_SPELL_MELEE_DMG_CLASS | PROC_FLAG_TAKEN_SPELL_MELEE_DMG_CLASS
                                                | PROC_FLAG_DONE_MAINHAND_ATTACK | PROC_FLAG_DONE_OFFHAND_ATTACK,

    RANGED_PROC_FLAG_MASK                     = PROC_FLAG_DONE_RANGED_AUTO_ATTACK | PROC_FLAG_TAKEN_RANGED_AUTO_ATTACK
                                                | PROC_FLAG_DONE_SPELL_RANGED_DMG_CLASS | PROC_FLAG_TAKEN_SPELL_RANGED_DMG_CLASS,

    SPELL_PROC_FLAG_MASK                      = PROC_FLAG_DONE_SPELL_MELEE_DMG_CLASS | PROC_FLAG_TAKEN_SPELL_MELEE_DMG_CLASS
                                                | PROC_FLAG_DONE_SPELL_RANGED_DMG_CLASS | PROC_FLAG_TAKEN_SPELL_RANGED_DMG_CLASS
                                                | PROC_FLAG_DONE_SPELL_NONE_DMG_CLASS_POS | PROC_FLAG_TAKEN_SPELL_NONE_DMG_CLASS_POS
                                                | PROC_FLAG_DONE_SPELL_NONE_DMG_CLASS_NEG | PROC_FLAG_TAKEN_SPELL_NONE_DMG_CLASS_NEG
                                                | PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_POS | PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_POS
                                                | PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_NEG | PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_NEG,

    SPELL_CAST_PROC_FLAG_MASK                  = SPELL_PROC_FLAG_MASK | PROC_FLAG_DONE_TRAP_ACTIVATION | RANGED_PROC_FLAG_MASK,

    PERIODIC_PROC_FLAG_MASK                    = PROC_FLAG_DONE_PERIODIC | PROC_FLAG_TAKEN_PERIODIC,

    DONE_HIT_PROC_FLAG_MASK                    = PROC_FLAG_DONE_MELEE_AUTO_ATTACK | PROC_FLAG_DONE_RANGED_AUTO_ATTACK
                                                 | PROC_FLAG_DONE_SPELL_MELEE_DMG_CLASS | PROC_FLAG_DONE_SPELL_RANGED_DMG_CLASS
                                                 | PROC_FLAG_DONE_SPELL_NONE_DMG_CLASS_POS | PROC_FLAG_DONE_SPELL_NONE_DMG_CLASS_NEG
                                                 | PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_POS | PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_NEG
                                                 | PROC_FLAG_DONE_PERIODIC | PROC_FLAG_DONE_MAINHAND_ATTACK | PROC_FLAG_DONE_OFFHAND_ATTACK,

    TAKEN_HIT_PROC_FLAG_MASK                   = PROC_FLAG_TAKEN_MELEE_AUTO_ATTACK | PROC_FLAG_TAKEN_RANGED_AUTO_ATTACK
                                                 | PROC_FLAG_TAKEN_SPELL_MELEE_DMG_CLASS | PROC_FLAG_TAKEN_SPELL_RANGED_DMG_CLASS
                                                 | PROC_FLAG_TAKEN_SPELL_NONE_DMG_CLASS_POS | PROC_FLAG_TAKEN_SPELL_NONE_DMG_CLASS_NEG
                                                 | PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_POS | PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_NEG
                                                 | PROC_FLAG_TAKEN_PERIODIC | PROC_FLAG_TAKEN_DAMAGE,

    REQ_SPELL_PHASE_PROC_FLAG_MASK             = SPELL_PROC_FLAG_MASK & DONE_HIT_PROC_FLAG_MASK
};

#define MELEE_BASED_TRIGGER_MASK (PROC_FLAG_DONE_MELEE_AUTO_ATTACK      | \
                                  PROC_FLAG_TAKEN_MELEE_AUTO_ATTACK     | \
                                  PROC_FLAG_DONE_SPELL_MELEE_DMG_CLASS  | \
                                  PROC_FLAG_TAKEN_SPELL_MELEE_DMG_CLASS | \
                                  PROC_FLAG_DONE_RANGED_AUTO_ATTACK     | \
                                  PROC_FLAG_TAKEN_RANGED_AUTO_ATTACK    | \
                                  PROC_FLAG_DONE_SPELL_RANGED_DMG_CLASS | \
                                  PROC_FLAG_TAKEN_SPELL_RANGED_DMG_CLASS)

enum ProcFlagsEx
{
    PROC_EX_NONE                = 0x0000000,                 // If none can tigger on Hit/Crit only (passive spells MUST defined by SpellFamily flag)
    PROC_EX_NORMAL_HIT          = 0x0000001,                 // If set only from normal hit (only damage spells)
    PROC_EX_CRITICAL_HIT        = 0x0000002,
    PROC_EX_MISS                = 0x0000004,
    PROC_EX_RESIST              = 0x0000008,
    PROC_EX_DODGE               = 0x0000010,
    PROC_EX_PARRY               = 0x0000020,
    PROC_EX_BLOCK               = 0x0000040,
    PROC_EX_EVADE               = 0x0000080,
    PROC_EX_IMMUNE              = 0x0000100,
    PROC_EX_DEFLECT             = 0x0000200,
    PROC_EX_ABSORB              = 0x0000400,
    PROC_EX_REFLECT             = 0x0000800,
    PROC_EX_INTERRUPT           = 0x0001000,                 // Melee hit result can be Interrupt (not used)
    PROC_EX_RESERVED1           = 0x0002000,
    PROC_EX_RESERVED2           = 0x0004000,
    PROC_EX_RESERVED3           = 0x0008000,
    PROC_EX_EX_TRIGGER_ALWAYS   = 0x0010000,                 // If set trigger always ( no matter another flags) used for drop charges
    PROC_EX_EX_ONE_TIME_TRIGGER = 0x0020000,                 // If set trigger always but only one time (not used)
    PROC_EX_INTERNAL_HOT        = 0x1000000,                 // Only for internal use
    PROC_EX_INTERNAL_DOT        = 0x2000000                  // Only for internal use
};

struct SpellProcEventEntry
{
    uint32      schoolMask;                                 // if nonzero - bit mask for matching proc condition based on spell candidate's school: Fire=2, Mask=1<<(2-1)=2
    uint32      spellFamilyName;                            // if nonzero - for matching proc condition based on candidate spell's SpellFamilyNamer value
    uint64      spellFamilyMask;                            // if nonzero - for matching proc condition based on candidate spell's SpellFamilyFlags (like auras 107 and 108 do)
    uint32      procFlags;                                  // bitmask for matching proc event
    uint32      procEx;                                     // proc Extend info (see ProcFlagsEx)
    float       ppmRate;                                    // for melee (ranged?) damage spells - proc rate per minute. if zero, falls back to flat chance from Spell.dbc
    float       customChance;                               // Owerride chance (in most cases for debug only)
    uint32      cooldown;                                   // hidden cooldown used for some spell proc events, applied to _triggered_spell_
};

typedef UNORDERED_MAP<uint32, SpellProcEventEntry> SpellProcEventMap;

struct SpellEnchantProcEntry
{
    uint32      customChance;
    float       PPMChance;
    uint32      procEx;
};

typedef UNORDERED_MAP<uint32, SpellEnchantProcEntry> SpellEnchantProcEventMap;

enum SpellGroup
{
    SPELL_GROUP_ELIXIR_BATTLE = 1,
    SPELL_GROUP_ELIXIR_GUARDIAN = 2,
    SPELL_GROUP_ELIXIR_UNSTABLE = 3,
    SPELL_GROUP_ELIXIR_SHATTRATH = 4,
    SPELL_GROUP_CORE_RANGE_MAX = 5,
};

#define SPELL_GROUP_DB_RANGE_MIN 1000

//                  spell_id, group_id
typedef std::multimap<uint32, SpellGroup > SpellSpellGroupMap;
typedef std::pair<SpellSpellGroupMap::const_iterator, SpellSpellGroupMap::const_iterator> SpellSpellGroupMapBounds;

//                      group_id, spell_id
typedef std::multimap<SpellGroup, int32> SpellGroupSpellMap;
typedef std::pair<SpellGroupSpellMap::const_iterator, SpellGroupSpellMap::const_iterator> SpellGroupSpellMapBounds;

enum SpellGroupStackRule
{
    SPELL_GROUP_STACK_RULE_DEFAULT = 0,
    SPELL_GROUP_STACK_RULE_EXCLUSIVE = 1,
    SPELL_GROUP_STACK_RULE_EXCLUSIVE_FROM_SAME_CASTER = 2,
};

#define SPELL_GROUP_STACK_RULE_MAX 3

typedef std::map<SpellGroup, SpellGroupStackRule> SpellGroupStackMap;

struct SpellThreatEntry
{
    int32       flatMod;                                    // flat threat-value for this Spell  - default: 0
    float       pctMod;                                     // threat-multiplier for this Spell  - default: 1.0f
    float       apPctMod;                                   // Pct of AP that is added as Threat - default: 0.0f
};

typedef std::map<uint32, SpellThreatEntry> SpellThreatMap;

// Spell script target related declarations (accessed using sSpellMgr functions)
enum SpellScriptTargetType
{
    SPELL_TARGET_TYPE_GAMEOBJECT = 0,
    SPELL_TARGET_TYPE_CREATURE   = 1,
    SPELL_TARGET_TYPE_DEAD       = 2
};

#define MAX_SPELL_TARGET_TYPE 3

struct SpellTargetEntry
{
    SpellTargetEntry(SpellScriptTargetType type_, uint32 targetEntry_) : type(type_), targetEntry(targetEntry_) {}
    SpellScriptTargetType type;
    uint32 targetEntry;
};

// coordinates for spells (accessed using sSpellMgr functions)
struct SpellTargetPosition
{
    uint32 target_mapId;
    float  target_X;
    float  target_Y;
    float  target_Z;
    float  target_Orientation;
};

typedef UNORDERED_MAP<uint32, SpellTargetPosition> SpellTargetPositionMap;

// Spell pet auras
class PetAura
{
    public:
        PetAura()
        {
            auras.clear();
        }

        PetAura(uint32 petEntry, uint32 aura, bool _removeOnChangePet, int _damage) :
            removeOnChangePet(_removeOnChangePet), damage(_damage)
        {
            auras[petEntry] = aura;
        }

        uint32 GetAura(uint32 petEntry) const
        {
            std::map<uint32, uint32>::const_iterator itr = auras.find(petEntry);
            if (itr != auras.end())
                return itr->second;
            else
            {
                std::map<uint32, uint32>::const_iterator itr = auras.find(0);
                if (itr != auras.end())
                    return itr->second;
                else
                    return 0;
            }
        }

        void AddAura(uint32 petEntry, uint32 aura)
        {
            auras[petEntry] = aura;
        }

        bool IsRemovedOnChangePet() const
        {
            return removeOnChangePet;
        }

        int32 GetDamage() const
        {
            return damage;
        }

    private:
        std::map<uint32, uint32> auras;
        bool removeOnChangePet;
        int32 damage;
};
typedef std::map<uint32, PetAura> SpellPetAuraMap;

// Spell rank chain  (accessed using sSpellMgr functions)
struct SpellChainNode
{
    uint32 prev;
    uint32 next;
    uint32 first;
    uint32 last;
    uint8  rank;
};

typedef UNORDERED_MAP<uint32, SpellChainNode> SpellChainMap;

//                 spell_id  req_spell
typedef UNORDERED_MAP<uint32, uint32> SpellRequiredMap;

typedef std::multimap<uint32, uint32> SpellsRequiringSpellMap;

// Spell learning properties (accessed using sSpellMgr functions)
struct SpellLearnSkillNode
{
    uint32 skill;
    uint32 value;                                           // 0  - max skill value for player level
    uint32 maxvalue;                                        // 0  - max skill value for player level
};

typedef std::map<uint32, SpellLearnSkillNode> SpellLearnSkillMap;

struct SpellLearnSpellNode
{
    uint32 spell;
    bool autoLearned;
};

typedef std::multimap<uint32, SpellLearnSpellNode> SpellLearnSpellMap;

typedef std::multimap<uint32, SkillLineAbilityEntry const*> SkillLineAbilityMap;

inline bool IsPrimaryProfessionSkill(uint32 skill)
{
    SkillLineEntry const* pSkill = sSkillLineStore.LookupEntry(skill);
    if (!pSkill)
        return false;

    if (pSkill->categoryId != SKILL_CATEGORY_PROFESSION)
        return false;

    return true;
}

inline bool IsProfessionSkill(uint32 skill)
{
    return  IsPrimaryProfessionSkill(skill) || skill == SKILL_FISHING || skill == SKILL_COOKING || skill == SKILL_FIRST_AID;
}

enum SpellCustomAttributes
{
    //SPELL_ATTR_CU_PLAYERS_ONLY      0x00000001
    SPELL_ATTR_CU_CONE_BACK        = 0x00000002,
    SPELL_ATTR_CU_CONE_LINE        = 0x00000004,
    SPELL_ATTR_CU_SHARE_DAMAGE     = 0x00000008,
    SPELL_ATTR_CU_AURA_HOT         = 0x00000010,
    SPELL_ATTR_CU_AURA_DOT         = 0x00000020,
    SPELL_ATTR_CU_AURA_CC          = 0x00000040,
    SPELL_ATTR_CU_AURA_SPELL       = 0x00000080,
    SPELL_ATTR_CU_DIRECT_DAMAGE    = 0x00000100,
    SPELL_ATTR_CU_CHARGE           = 0x00000200,
    SPELL_ATTR_CU_LINK_CAST        = 0x00000400,
    SPELL_ATTR_CU_LINK_HIT         = 0x00000800,
    SPELL_ATTR_CU_LINK_AURA        = 0x00001000,
    SPELL_ATTR_CU_LINK_REMOVE      = 0x00002000,
    SPELL_ATTR_CU_MOVEMENT_IMPAIR  = 0x00004000,
    SPELL_ATTR_CU_IGNORE_ARMOR     = 0x00008000, 
    SPELL_ATTR_CU_CAST_BY_ITEM_ONLY= 0x00010000, //!< must be cast from item, never directly
    SPELL_ATTR_CU_FIXED_AMOUNT     = 0x00020000, //!< ignore bonus healing/damage
    SPELL_ATTR_CU_ANY_TARGET       = 0x00040000  //!< approves cast on any target, even when its non-attackable @see Spell::CheckTarget, useful with crafted/forced targets
};

typedef std::vector<uint32> SpellCustomAttribute;

typedef std::map<int32, std::vector<int32> > SpellLinkedMap;

enum SpellDummyConditionBitmask
{
    SDC_BTM_CASTER_MUST_NOT_HAVE_PET      = 1 << 0,
    SDC_BTM_CASTER_MUST_NOT_HAVE_CHARM    = 1 << 1,
    SDC_BTM_CASTER_MUST_BE_UNDERWATER     = 1 << 2,
    SDC_BTM_TARGET_MUST_BE_CREATURE       = 1 << 3,
    SDC_BTM_TARGET_MUST_BE_PLAYER         = 1 << 4,
    SDC_BTM_TARGET_MUST_BE_FRIENDLY       = 1 << 5,
    SDC_BTM_TARGET_MUST_BE_HOSTILE        = 1 << 6,
    SDC_BTM_TARGET_MUST_BE_DEAD           = 1 << 7,
    SDC_BTM_CASTER_MUST_BE_OUT_OF_COMBAT  = 1 << 8,
    SDC_BTM_TARGET_MUST_NOT_BE_HIER_LEVEL = 1 << 9,
    // bits 10 - 17 are reserved
    SDC_BTM_NEEDS_SCRIPT_CHECK            = 1 << 18,
    SDC_BTM_TARGET_TYPE_NONE              = 1 << 19,
    SDC_BTM_TARGET_TYPE_BEAST             = 1 << 20,
    SDC_BTM_TARGET_TYPE_DRAGONKIN         = 1 << 21,
    SDC_BTM_TARGET_TYPE_DEMON             = 1 << 20,
    SDC_BTM_TARGET_TYPE_ELEMENTAL         = 1 << 22,
    SDC_BTM_TARGET_TYPE_GIANT             = 1 << 23,
    SDC_BTM_TARGET_TYPE_UNDEAD            = 1 << 24,
    SDC_BTM_TARGET_TYPE_HUMANOID          = 1 << 25,
    SDC_BTM_TARGET_TYPE_CRITTER           = 1 << 26,
    SDC_BTM_TARGET_TYPE_MECHANICAL        = 1 << 27,
    SDC_BTM_TARGET_TYPE_10                = 1 << 28,
    SDC_BTM_TARGET_TYPE_TOTEM             = 1 << 29,
    SDC_BTM_TARGET_TYPE_NON_COMBAT_PET    = 1 << 30,
    SDC_BTM_TARGET_TYPE_GAS_CLOUD         = 1 << 31
};

enum SpellDummyCondition
{
    SDC_NONE                  = 0,
    SDC_AURA_TARGET           = 1,
    SDC_AURA_CASTER           = 2,
    SDC_QUEST_TAKEN           = 3,
    SDC_QUEST_REWARDED        = 4,
    SDC_HAS_SPELL             = 5,
    SDC_ACTIVE_EVENT          = 6,
    SDC_TEAM                  = 7,
    SDC_CASTER_HP_PCT         = 8,
    SDC_TARGET_HP_PCT         = 9,
    SDC_CASTER_MANA_PCT       = 10,
    SDC_TARGET_MANA_PCT       = 11,
    SDC_TARGET_IN_ARC         = 12,
    SDC_TARGET_EXACT_ENTRY    = 13,
    SDC_TARGET_TYPE_FLAGS     = 14,
    SDC_TARGET_FAMILY         = 15
};

struct SpellDummyConditionEntry
{
    uint32 entry;
    SpellEffIndex effIndex;
    uint32 bitMaskCondition;

    struct
    {
        SpellDummyCondition condition;
        int32 data;
    }
    conditions[5];
};

typedef std::multimap<uint32, SpellDummyConditionEntry> SpellDummyConditionMap;

class SpellMgr
{
        // Constructors
    public:
        SpellMgr();
        ~SpellMgr();

        // Accessors (const or static functions)
    public:
        // Spell affects
        uint64 GetSpellAffectMask(uint16 spellId, uint8 effectId) const
        {
            SpellAffectMap::const_iterator itr = mSpellAffectMap.find((spellId << 8) + effectId);
            if (itr != mSpellAffectMap.end())
                return itr->second;
            return 0;
        }

        bool IsAffectedBySpell(SpellEntry const* spellInfo, uint32 spellId, uint8 effectId, uint64 familyFlags) const;

        inline bool IsPositionTarget(uint32 target)
        {
            switch (SpellTargetType[target])
            {
                case TARGET_TYPE_DEST_CASTER:
                case TARGET_TYPE_DEST_TARGET:
                case TARGET_TYPE_DEST_DEST:
                    return true;
                default:
                break;
            }
            return false;
        }

        SpellThreatEntry const* GetSpellThreatEntry(uint32 spellID) const
        {
            SpellThreatMap::const_iterator itr = mSpellThreatMap.find(spellID);
            if (itr != mSpellThreatMap.end())
                return &itr->second;
            else
            {
                uint32 firstSpell = GetFirstSpellInChain(spellID);
                SpellThreatMap::const_iterator itr = mSpellThreatMap.find(firstSpell);
                if (itr != mSpellThreatMap.end())
                    return &itr->second;
            }
            return NULL;
        }

        // Spell proc events
        SpellProcEventEntry const* GetSpellProcEvent(uint32 spellId) const
        {
            SpellProcEventMap::const_iterator itr = mSpellProcEventMap.find(spellId);
            if (itr != mSpellProcEventMap.end())
                return &itr->second;
            return NULL;
        }

        static bool IsSpellProcEventCanTriggeredBy(SpellProcEventEntry const* spellProcEvent, uint32 EventProcFlag, SpellEntry const* procSpell, uint32 procFlags, uint32 procExtra, bool active);

        SpellEnchantProcEntry const* GetSpellEnchantProcEvent(uint32 enchId) const
        {
            SpellEnchantProcEventMap::const_iterator itr = mSpellEnchantProcEventMap.find(enchId);
            if (itr != mSpellEnchantProcEventMap.end())
                return &itr->second;
            return NULL;
        }

        // Spell target coordinates
        SpellTargetPosition const* GetSpellTargetPosition(uint32 spell_id) const
        {
            SpellTargetPositionMap::const_iterator itr = mSpellTargetPositions.find(spell_id);
            if (itr != mSpellTargetPositions.end())
                return &itr->second;
            return NULL;
        }

        // Spell ranks chains
        SpellChainNode const* GetSpellChainNode(uint32 spell_id) const
        {
            SpellChainMap::const_iterator itr = mSpellChains.find(spell_id);
            if (itr == mSpellChains.end())
                return NULL;

            return &itr->second;
        }

        uint32 GetSpellRequired(uint32 spell_id) const
        {
            SpellRequiredMap::const_iterator itr = mSpellReq.find(spell_id);
            if (itr == mSpellReq.end())
                return 0;

            return itr->second;
        }

        uint32 GetFirstSpellInChain(uint32 spell_id) const
        {
            if (SpellChainNode const* node = GetSpellChainNode(spell_id))
                return node->first;

            return spell_id;
        }

        uint32 GetPrevSpellInChain(uint32 spell_id) const
        {
            if (SpellChainNode const* node = GetSpellChainNode(spell_id))
                return node->prev;

            return 0;
        }

        SpellsRequiringSpellMap const& GetSpellsRequiringSpell() const
        {
            return mSpellsReqSpell;
        }

        // Note: not use rank for compare to spell ranks: spell chains isn't linear order
        // Use IsHighRankOfSpell instead
        uint8 GetSpellRank(uint32 spell_id) const
        {
            if (SpellChainNode const* node = GetSpellChainNode(spell_id))
                return node->rank;

            return 0;
        }

        uint32 GetLastSpellInChain(uint32 spell_id) const
        {
            if (SpellChainNode const* node = GetSpellChainNode(spell_id))
                return node->last;

            return spell_id;
        }

        uint8 IsHighRankOfSpell(uint32 spell1, uint32 spell2) const
        {
            SpellChainMap::const_iterator itr1 = mSpellChains.find(spell1);
            SpellChainMap::const_iterator itr2 = mSpellChains.find(spell2);
            if (itr1 == mSpellChains.end() || itr2 == mSpellChains.end())
                return false;

            if (itr1->second.first == itr2->second.first)
                if (itr1->second.rank > itr2->second.rank)
                    return true;

            return false;
        }

        bool IsRankSpellDueToSpell(SpellEntry const* spellInfo_1, uint32 spellId_2) const;
        static bool canStackSpellRanks(SpellEntry const* spellInfo);
        bool IsNoStackSpellDueToSpell(uint32 spellId_1, uint32 spellId_2, bool sameCaster) const;

        SpellEntry const* SelectAuraRankForPlayerLevel(SpellEntry const* spellInfo, uint32 playerLevel) const;

        // Spell learning
        SpellLearnSkillNode const* GetSpellLearnSkill(uint32 spell_id) const
        {
            SpellLearnSkillMap::const_iterator itr = mSpellLearnSkills.find(spell_id);
            if (itr != mSpellLearnSkills.end())
                return &itr->second;
            else
                return NULL;
        }

        bool IsSpellLearnSpell(uint32 spell_id) const
        {
            return mSpellLearnSpells.count(spell_id) != 0;
        }

        SpellLearnSpellMap::const_iterator GetBeginSpellLearnSpell(uint32 spell_id) const
        {
            return mSpellLearnSpells.lower_bound(spell_id);
        }

        SpellLearnSpellMap::const_iterator GetEndSpellLearnSpell(uint32 spell_id) const
        {
            return mSpellLearnSpells.upper_bound(spell_id);
        }

        bool IsSpellLearnToSpell(uint32 spell_id1, uint32 spell_id2) const
        {
            SpellLearnSpellMap::const_iterator b = GetBeginSpellLearnSpell(spell_id1);
            SpellLearnSpellMap::const_iterator e = GetEndSpellLearnSpell(spell_id1);
            for (SpellLearnSpellMap::const_iterator i = b; i != e; ++i)
                if (i->second.spell == spell_id2)
                    return true;
            return false;
        }

        static bool IsProfessionSpell(uint32 spellId);
        static bool IsPrimaryProfessionSpell(uint32 spellId);
        bool IsPrimaryProfessionFirstRankSpell(uint32 spellId) const;

        // Spell correctess for client using
        static bool IsSpellValid(SpellEntry const* spellInfo, Player* pl = NULL, bool msg = true);

        // Spell Groups table
        SpellSpellGroupMapBounds GetSpellSpellGroupMapBounds(uint32 spell_id) const;
        bool IsSpellMemberOfSpellGroup(uint32 spellid, SpellGroup groupid) const;

        SpellGroupSpellMapBounds GetSpellGroupSpellMapBounds(SpellGroup group_id) const;
        void GetSetOfSpellsInSpellGroup(SpellGroup group_id, std::set<uint32>& foundSpells) const;
        void GetSetOfSpellsInSpellGroup(SpellGroup group_id, std::set<uint32>& foundSpells, std::set<SpellGroup>& usedGroups) const;

        // Spell Group Stack Rules table
        SpellGroupStackRule CheckSpellGroupStackRules(uint32 spellInfo1, uint32 spellInfo2) const;

        SkillLineAbilityMap::const_iterator GetBeginSkillLineAbilityMap(uint32 spell_id) const
        {
            return mSkillLineAbilityMap.lower_bound(spell_id);
        }

        SkillLineAbilityMap::const_iterator GetEndSkillLineAbilityMap(uint32 spell_id) const
        {
            return mSkillLineAbilityMap.upper_bound(spell_id);
        }

        PetAura const* GetPetAura(uint32 spell_id)
        {
            SpellPetAuraMap::const_iterator itr = mSpellPetAuraMap.find(spell_id);
            if (itr != mSpellPetAuraMap.end())
                return &itr->second;
            else
                return NULL;
        }

        uint32 GetSpellCustomAttr(uint32 spell_id) const
        {
            if (spell_id >= mSpellCustomAttr.size())
                return 0;
            else
                return mSpellCustomAttr[spell_id];
            /*SpellCustomAttrMap::const_iterator itr = mSpellCustomAttrMap.find(spell_id);
            if (itr != mSpellCustomAttrMap.end())
                return itr->second;
            else
                return 0;*/
        }

        const std::vector<int32>* GetSpellLinked(int32 spell_id) const
        {
            SpellLinkedMap::const_iterator itr = mSpellLinkedMap.find(spell_id);
            return itr != mSpellLinkedMap.end() ? &(itr->second) : NULL;
        }

        SpellEffectTargetTypes EffectTargetType[TOTAL_SPELL_EFFECTS];
        SpellSelectTargetTypes SpellTargetType[TOTAL_SPELL_TARGETS];

        const SpellDummyConditionEntry* GetSpellDummyCondition(uint32 spellId, uint32 effIndex) const
        {
            typedef SpellDummyConditionMap::const_iterator Iterator;
            std::pair<Iterator, Iterator> range = mSpellDummyConditionMap.equal_range(spellId);

            for (Iterator it = range.first; it != range.second; it++)
                if (it->second.effIndex == effIndex)
                    return &(it->second);

            return NULL;
        }

        // Modifiers
    public:
        static SpellMgr& Instance();

        // Loading data at server startup
        void LoadSpellChains();
        void LoadSpellRequired();
        void LoadSpellLearnSkills();
        void LoadSpellLearnSpells();
        void LoadSpellAffects();
        void LoadSpellGroups();
        void LoadSpellProcEvents();
        void LoadSpellTargetPositions();
        void LoadSpellThreats();
        void LoadSkillLineAbilityMap();
        void LoadSpellPetAuras();
        void LoadSpellCustomAttr();
        void LoadSpellCustomCooldowns();
        void LoadSpellLinked();
        void LoadSpellEnchantProcData();
        void LoadSpellDummyCondition();
        void LoadSpellGroupStackRules();

    private:
        SpellChainMap      mSpellChains;
        SpellsRequiringSpellMap   mSpellsReqSpell;
        SpellRequiredMap   mSpellReq;
        SpellLearnSkillMap mSpellLearnSkills;
        SpellLearnSpellMap mSpellLearnSpells;
        SpellTargetPositionMap mSpellTargetPositions;
        SpellThreatMap     mSpellThreatMap;
        SpellAffectMap     mSpellAffectMap;
        SpellSpellGroupMap mSpellSpellGroup;
        SpellGroupSpellMap mSpellGroupSpell;
        SpellProcEventMap  mSpellProcEventMap;
        SkillLineAbilityMap mSkillLineAbilityMap;
        SpellPetAuraMap     mSpellPetAuraMap;
        SpellCustomAttribute  mSpellCustomAttr;
        SpellLinkedMap      mSpellLinkedMap;
        SpellGroupStackMap   mSpellGroupStack;
        SpellEnchantProcEventMap     mSpellEnchantProcEventMap;
        SpellDummyConditionMap       mSpellDummyConditionMap;
};

#define sSpellMgr SpellMgr::Instance()
#endif

