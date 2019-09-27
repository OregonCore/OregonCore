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

#include "Common.h"
#include "Database/DatabaseEnv.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Opcodes.h"
#include "Log.h"
#include "UpdateMask.h"
#include "World.h"
#include "ObjectMgr.h"
#include "SpellMgr.h"
#include "Player.h"
#include "Unit.h"
#include "Spell.h"
#include "SpellAuras.h"
#include "DynamicObject.h"
#include "Group.h"
#include "UpdateData.h"
#include "MapManager.h"
#include "ObjectAccessor.h"
#include "Totem.h"
#include "Creature.h"
#include "Formulas.h"
#include "Battleground.h"
#include "OutdoorPvP.h"
#include "OutdoorPvPMgr.h"
#include "CreatureAI.h"
#include "Utilities/Util.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "ScriptMgr.h"
#include "CellImpl.h"

#define NULL_AURA_SLOT 0xFF

pAuraHandler AuraHandler[TOTAL_AURAS] =
{
    &Aura::HandleNULL,                                      //  0 SPELL_AURA_NONE
    &Aura::HandleBindSight,                                 //  1 SPELL_AURA_BIND_SIGHT
    &Aura::HandleModPossess,                                //  2 SPELL_AURA_MOD_POSSESS
    &Aura::HandlePeriodicDamage,                            //  3 SPELL_AURA_PERIODIC_DAMAGE
    &Aura::HandleAuraDummy,                                 //  4 SPELL_AURA_DUMMY
    &Aura::HandleModConfuse,                                //  5 SPELL_AURA_MOD_CONFUSE
    &Aura::HandleModCharm,                                  //  6 SPELL_AURA_MOD_CHARM
    &Aura::HandleModFear,                                   //  7 SPELL_AURA_MOD_FEAR
    &Aura::HandlePeriodicHeal,                              //  8 SPELL_AURA_PERIODIC_HEAL
    &Aura::HandleModAttackSpeed,                            //  9 SPELL_AURA_MOD_ATTACKSPEED
    &Aura::HandleModThreat,                                 // 10 SPELL_AURA_MOD_THREAT
    &Aura::HandleModTaunt,                                  // 11 SPELL_AURA_MOD_TAUNT
    &Aura::HandleAuraModStun,                               // 12 SPELL_AURA_MOD_STUN
    &Aura::HandleModDamageDone,                             // 13 SPELL_AURA_MOD_DAMAGE_DONE
    &Aura::HandleNoImmediateEffect,                         // 14 SPELL_AURA_MOD_DAMAGE_TAKEN implemented in Unit::MeleeDamageBonus and Unit::SpellDamageBonus
    &Aura::HandleNoImmediateEffect,                         // 15 SPELL_AURA_DAMAGE_SHIELD    implemented in Unit::DoAttackDamage
    &Aura::HandleModStealth,                                // 16 SPELL_AURA_MOD_STEALTH
    &Aura::HandleModStealthDetect,                          // 17 SPELL_AURA_MOD_STEALTH_DETECT
    &Aura::HandleModInvisibility,                           // 18 SPELL_AURA_MOD_INVISIBILITY
    &Aura::HandleModInvisibilityDetect,                     // 19 SPELL_AURA_MOD_INVISIBILITY_DETECTION
    &Aura::HandleAuraModTotalHealthPercentRegen,            // 20 SPELL_AURA_OBS_MOD_HEALTH
    &Aura::HandleAuraModTotalManaPercentRegen,              // 21 SPELL_AURA_OBS_MOD_MANA
    &Aura::HandleAuraModResistance,                         // 22 SPELL_AURA_MOD_RESISTANCE
    &Aura::HandlePeriodicTriggerSpell,                      // 23 SPELL_AURA_PERIODIC_TRIGGER_SPELL
    &Aura::HandlePeriodicEnergize,                          // 24 SPELL_AURA_PERIODIC_ENERGIZE
    &Aura::HandleAuraModPacify,                             // 25 SPELL_AURA_MOD_PACIFY
    &Aura::HandleAuraModRoot,                               // 26 SPELL_AURA_MOD_ROOT
    &Aura::HandleAuraModSilence,                            // 27 SPELL_AURA_MOD_SILENCE
    &Aura::HandleNoImmediateEffect,                         // 28 SPELL_AURA_REFLECT_SPELLS        implement in Unit::SpellHitResult
    &Aura::HandleAuraModStat,                               // 29 SPELL_AURA_MOD_STAT
    &Aura::HandleAuraModSkill,                              // 30 SPELL_AURA_MOD_SKILL
    &Aura::HandleAuraModIncreaseSpeed,                      // 31 SPELL_AURA_MOD_INCREASE_SPEED
    &Aura::HandleAuraModIncreaseMountedSpeed,               // 32 SPELL_AURA_MOD_INCREASE_MOUNTED_SPEED
    &Aura::HandleAuraModDecreaseSpeed,                      // 33 SPELL_AURA_MOD_DECREASE_SPEED
    &Aura::HandleAuraModIncreaseHealth,                     // 34 SPELL_AURA_MOD_INCREASE_HEALTH
    &Aura::HandleAuraModIncreaseEnergy,                     // 35 SPELL_AURA_MOD_INCREASE_ENERGY
    &Aura::HandleAuraModShapeshift,                         // 36 SPELL_AURA_MOD_SHAPESHIFT
    &Aura::HandleAuraModEffectImmunity,                     // 37 SPELL_AURA_EFFECT_IMMUNITY
    &Aura::HandleAuraModStateImmunity,                      // 38 SPELL_AURA_STATE_IMMUNITY
    &Aura::HandleAuraModSchoolImmunity,                     // 39 SPELL_AURA_SCHOOL_IMMUNITY
    &Aura::HandleAuraModDmgImmunity,                        // 40 SPELL_AURA_DAMAGE_IMMUNITY
    &Aura::HandleAuraModDispelImmunity,                     // 41 SPELL_AURA_DISPEL_IMMUNITY
    &Aura::HandleAuraProcTriggerSpell,                      // 42 SPELL_AURA_PROC_TRIGGER_SPELL  implemented in Unit::ProcDamageAndSpellFor and Unit::HandleProcTriggerSpell
    &Aura::HandleNoImmediateEffect,                         // 43 SPELL_AURA_PROC_TRIGGER_DAMAGE implemented in Unit::ProcDamageAndSpellFor
    &Aura::HandleAuraTrackCreatures,                        // 44 SPELL_AURA_TRACK_CREATURES
    &Aura::HandleAuraTrackResources,                        // 45 SPELL_AURA_TRACK_RESOURCES
    &Aura::HandleUnused,                                    // 46 SPELL_AURA_MOD_PARRY_SKILL    obsolete?
    &Aura::HandleAuraModParryPercent,                       // 47 SPELL_AURA_MOD_PARRY_PERCENT
    &Aura::HandleUnused,                                    // 48 SPELL_AURA_MOD_DODGE_SKILL    obsolete?
    &Aura::HandleAuraModDodgePercent,                       // 49 SPELL_AURA_MOD_DODGE_PERCENT
    &Aura::HandleUnused,                                    // 50 SPELL_AURA_MOD_BLOCK_SKILL    obsolete?
    &Aura::HandleAuraModBlockPercent,                       // 51 SPELL_AURA_MOD_BLOCK_PERCENT
    &Aura::HandleAuraModCritPercent,                        // 52 SPELL_AURA_MOD_CRIT_PERCENT
    &Aura::HandlePeriodicLeech,                             // 53 SPELL_AURA_PERIODIC_LEECH
    &Aura::HandleModHitChance,                              // 54 SPELL_AURA_MOD_HIT_CHANCE
    &Aura::HandleModSpellHitChance,                         // 55 SPELL_AURA_MOD_SPELL_HIT_CHANCE
    &Aura::HandleAuraTransform,                             // 56 SPELL_AURA_TRANSFORM
    &Aura::HandleModSpellCritChance,                        // 57 SPELL_AURA_MOD_SPELL_CRIT_CHANCE
    &Aura::HandleAuraModIncreaseSwimSpeed,                  // 58 SPELL_AURA_MOD_INCREASE_SWIM_SPEED
    &Aura::HandleNoImmediateEffect,                         // 59 SPELL_AURA_MOD_DAMAGE_DONE_CREATURE implemented in Unit::MeleeDamageBonus and Unit::SpellDamageBonus
    &Aura::HandleAuraModPacifyAndSilence,                   // 60 SPELL_AURA_MOD_PACIFY_SILENCE
    &Aura::HandleAuraModScale,                              // 61 SPELL_AURA_MOD_SCALE
    &Aura::HandleNULL,                                      // 62 SPELL_AURA_PERIODIC_HEALTH_FUNNEL
    &Aura::HandleUnused,                                    // 63 SPELL_AURA_PERIODIC_MANA_FUNNEL obsolete?
    &Aura::HandlePeriodicManaLeech,                         // 64 SPELL_AURA_PERIODIC_MANA_LEECH
    &Aura::HandleModCastingSpeed,                           // 65 SPELL_AURA_MOD_CASTING_SPEED
    &Aura::HandleFeignDeath,                                // 66 SPELL_AURA_FEIGN_DEATH
    &Aura::HandleAuraModDisarm,                             // 67 SPELL_AURA_MOD_DISARM
    &Aura::HandleAuraModStalked,                            // 68 SPELL_AURA_MOD_STALKED
    &Aura::HandleSchoolAbsorb,                              // 69 SPELL_AURA_SCHOOL_ABSORB implemented in Unit::CalcAbsorbResist
    &Aura::HandleUnused,                                    // 70 SPELL_AURA_EXTRA_ATTACKS      Useless, used by only one spell that has only visual effect
    &Aura::HandleModSpellCritChanceShool,                   // 71 SPELL_AURA_MOD_SPELL_CRIT_CHANCE_SCHOOL
    &Aura::HandleModPowerCostPCT,                           // 72 SPELL_AURA_MOD_POWER_COST_SCHOOL_PCT
    &Aura::HandleModPowerCost,                              // 73 SPELL_AURA_MOD_POWER_COST_SCHOOL
    &Aura::HandleAuraReflectSpellSchool,                    // 74 SPELL_AURA_REFLECT_SPELLS_SCHOOL  implemented in Unit::SpellHitResult
    &Aura::HandleNoImmediateEffect,                         // 75 SPELL_AURA_MOD_LANGUAGE
    &Aura::HandleFarSight,                                  // 76 SPELL_AURA_FAR_SIGHT
    &Aura::HandleModMechanicImmunity,                       // 77 SPELL_AURA_MECHANIC_IMMUNITY
    &Aura::HandleAuraMounted,                               // 78 SPELL_AURA_MOUNTED
    &Aura::HandleModDamagePercentDone,                      // 79 SPELL_AURA_MOD_DAMAGE_PERCENT_DONE
    &Aura::HandleModPercentStat,                            // 80 SPELL_AURA_MOD_PERCENT_STAT
    &Aura::HandleNoImmediateEffect,                         // 81 SPELL_AURA_SPLIT_DAMAGE_PCT
    &Aura::HandleWaterBreathing,                            // 82 SPELL_AURA_WATER_BREATHING
    &Aura::HandleModBaseResistance,                         // 83 SPELL_AURA_MOD_BASE_RESISTANCE
    &Aura::HandleModRegen,                                  // 84 SPELL_AURA_MOD_REGEN
    &Aura::HandleModPowerRegen,                             // 85 SPELL_AURA_MOD_POWER_REGEN
    &Aura::HandleChannelDeathItem,                          // 86 SPELL_AURA_CHANNEL_DEATH_ITEM
    &Aura::HandleNoImmediateEffect,                         // 87 SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN implemented in Unit::MeleeDamageBonus and Unit::SpellDamageBonus
    &Aura::HandleNoImmediateEffect,                         // 88 SPELL_AURA_MOD_HEALTH_REGEN_PERCENT
    &Aura::HandlePeriodicDamagePCT,                         // 89 SPELL_AURA_PERIODIC_DAMAGE_PERCENT
    &Aura::HandleUnused,                                    // 90 SPELL_AURA_MOD_RESIST_CHANCE  Useless
    &Aura::HandleNoImmediateEffect,                         // 91 SPELL_AURA_MOD_DETECT_RANGE implemented in Creature::GetAttackDistance
    &Aura::HandlePreventFleeing,                            // 92 SPELL_AURA_PREVENTS_FLEEING
    &Aura::HandleModUnattackable,                           // 93 SPELL_AURA_MOD_UNATTACKABLE
    &Aura::HandleNoImmediateEffect,                         // 94 SPELL_AURA_INTERRUPT_REGEN implemented in Player::RegenerateAll
    &Aura::HandleAuraGhost,                                 // 95 SPELL_AURA_GHOST
    &Aura::HandleNoImmediateEffect,                         // 96 SPELL_AURA_SPELL_MAGNET implemented in Spell::SelectMagnetTarget
    &Aura::HandleManaShield,                                // 97 SPELL_AURA_MANA_SHIELD implemented in Unit::CalcAbsorbResist
    &Aura::HandleAuraModSkill,                              // 98 SPELL_AURA_MOD_SKILL_TALENT
    &Aura::HandleAuraModAttackPower,                        // 99 SPELL_AURA_MOD_ATTACK_POWER
    &Aura::HandleUnused,                                    //100 SPELL_AURA_AURAS_VISIBLE obsolete? all player can see all auras now
    &Aura::HandleModResistancePercent,                      //101 SPELL_AURA_MOD_RESISTANCE_PCT
    &Aura::HandleNoImmediateEffect,                         //102 SPELL_AURA_MOD_MELEE_ATTACK_POWER_VERSUS implemented in Unit::MeleeDamageBonus
    &Aura::HandleAuraModTotalThreat,                        //103 SPELL_AURA_MOD_TOTAL_THREAT
    &Aura::HandleAuraWaterWalk,                             //104 SPELL_AURA_WATER_WALK
    &Aura::HandleAuraFeatherFall,                           //105 SPELL_AURA_FEATHER_FALL
    &Aura::HandleAuraHover,                                 //106 SPELL_AURA_HOVER
    &Aura::HandleAddModifier,                               //107 SPELL_AURA_ADD_FLAT_MODIFIER
    &Aura::HandleAddModifier,                               //108 SPELL_AURA_ADD_PCT_MODIFIER
    &Aura::HandleNoImmediateEffect,                         //109 SPELL_AURA_ADD_TARGET_TRIGGER
    &Aura::HandleModPowerRegenPCT,                          //110 SPELL_AURA_MOD_POWER_REGEN_PERCENT
    &Aura::HandleNoImmediateEffect,                         //111 SPELL_AURA_ADD_CASTER_HIT_TRIGGER implemented in Spell::HandleHitTriggerAura
    &Aura::HandleNoImmediateEffect,                         //112 SPELL_AURA_OVERRIDE_CLASS_SCRIPTS another Dummy spells
    &Aura::HandleNoImmediateEffect,                         //113 SPELL_AURA_MOD_RANGED_DAMAGE_TAKEN implemented in Unit::MeleeDamageBonus
    &Aura::HandleNoImmediateEffect,                         //114 SPELL_AURA_MOD_RANGED_DAMAGE_TAKEN_PCT implemented in Unit::MeleeDamageBonus
    &Aura::HandleNoImmediateEffect,                         //115 SPELL_AURA_MOD_HEALING                 implemented in Unit::SpellBaseHealingBonusForVictim
    &Aura::HandleNoImmediateEffect,                         //116 SPELL_AURA_MOD_REGEN_DURING_COMBAT
    &Aura::HandleNoImmediateEffect,                         //117 SPELL_AURA_MOD_MECHANIC_RESISTANCE     implemented in Unit::MagicSpellHitResult
    &Aura::HandleNoImmediateEffect,                         //118 SPELL_AURA_MOD_HEALING_PCT             implemented in Unit::SpellHealingBonus
    &Aura::HandleUnused,                                    //119 SPELL_AURA_SHARE_PET_TRACKING useless
    &Aura::HandleAuraUntrackable,                           //120 SPELL_AURA_UNTRACKABLE
    &Aura::HandleAuraEmpathy,                               //121 SPELL_AURA_EMPATHY
    &Aura::HandleModOffhandDamagePercent,                   //122 SPELL_AURA_MOD_OFFHAND_DAMAGE_PCT
    &Aura::HandleModTargetResistance,                       //123 SPELL_AURA_MOD_TARGET_RESISTANCE
    &Aura::HandleAuraModRangedAttackPower,                  //124 SPELL_AURA_MOD_RANGED_ATTACK_POWER
    &Aura::HandleNoImmediateEffect,                         //125 SPELL_AURA_MOD_MELEE_DAMAGE_TAKEN implemented in Unit::MeleeDamageBonus
    &Aura::HandleNoImmediateEffect,                         //126 SPELL_AURA_MOD_MELEE_DAMAGE_TAKEN_PCT implemented in Unit::MeleeDamageBonus
    &Aura::HandleNoImmediateEffect,                         //127 SPELL_AURA_RANGED_ATTACK_POWER_ATTACKER_BONUS implemented in Unit::MeleeDamageBonus
    &Aura::HandleModPossessPet,                             //128 SPELL_AURA_MOD_POSSESS_PET
    &Aura::HandleAuraModIncreaseSpeed,                      //129 SPELL_AURA_MOD_SPEED_ALWAYS
    &Aura::HandleAuraModIncreaseMountedSpeed,               //130 SPELL_AURA_MOD_MOUNTED_SPEED_ALWAYS
    &Aura::HandleNoImmediateEffect,                         //131 SPELL_AURA_MOD_RANGED_ATTACK_POWER_VERSUS implemented in Unit::MeleeDamageBonus
    &Aura::HandleAuraModIncreaseEnergyPercent,              //132 SPELL_AURA_MOD_INCREASE_ENERGY_PERCENT
    &Aura::HandleAuraModIncreaseHealthPercent,              //133 SPELL_AURA_MOD_INCREASE_HEALTH_PERCENT
    &Aura::HandleAuraModRegenInterrupt,                     //134 SPELL_AURA_MOD_MANA_REGEN_INTERRUPT
    &Aura::HandleModHealingDone,                            //135 SPELL_AURA_MOD_HEALING_DONE
    &Aura::HandleNoImmediateEffect,                         //136 SPELL_AURA_MOD_HEALING_DONE_PERCENT   implemented in Unit::SpellHealingBonus
    &Aura::HandleModTotalPercentStat,                       //137 SPELL_AURA_MOD_TOTAL_STAT_PERCENTAGE
    &Aura::HandleHaste,                                     //138 SPELL_AURA_MOD_HASTE
    &Aura::HandleForceReaction,                             //139 SPELL_AURA_FORCE_REACTION
    &Aura::HandleAuraModRangedHaste,                        //140 SPELL_AURA_MOD_RANGED_HASTE
    &Aura::HandleRangedAmmoHaste,                           //141 SPELL_AURA_MOD_RANGED_AMMO_HASTE
    &Aura::HandleAuraModBaseResistancePCT,                  //142 SPELL_AURA_MOD_BASE_RESISTANCE_PCT
    &Aura::HandleAuraModResistanceExclusive,                //143 SPELL_AURA_MOD_RESISTANCE_EXCLUSIVE
    &Aura::HandleNoImmediateEffect,                         //144 SPELL_AURA_SAFE_FALL                  implemented in WorldSession::HandleMovementOpcodes
    &Aura::HandleUnused,                                    //145 SPELL_AURA_CHARISMA obsolete?
    &Aura::HandleUnused,                                    //146 SPELL_AURA_PERSUADED obsolete?
    &Aura::HandleNULL,                                      //147 SPELL_AURA_MECHANIC_IMMUNITY_MASK
    &Aura::HandleAuraRetainComboPoints,                     //148 SPELL_AURA_RETAIN_COMBO_POINTS
    &Aura::HandleNoImmediateEffect,                         //149 SPELL_AURA_RESIST_PUSHBACK
    &Aura::HandleShieldBlockValue,                          //150 SPELL_AURA_MOD_SHIELD_BLOCKVALUE_PCT
    &Aura::HandleAuraTrackStealthed,                        //151 SPELL_AURA_TRACK_STEALTHED
    &Aura::HandleNoImmediateEffect,                         //152 SPELL_AURA_MOD_DETECTED_RANGE implemented in Creature::GetAttackDistance
    &Aura::HandleNoImmediateEffect,                         //153 SPELL_AURA_SPLIT_DAMAGE_FLAT
    &Aura::HandleModStealthLevel,                           //154 SPELL_AURA_MOD_STEALTH_LEVEL
    &Aura::HandleNoImmediateEffect,                         //155 SPELL_AURA_MOD_WATER_BREATHING
    &Aura::HandleNoImmediateEffect,                         //156 SPELL_AURA_MOD_REPUTATION_GAIN
    &Aura::HandleNULL,                                      //157 SPELL_AURA_PET_DAMAGE_MULTI
    &Aura::HandleShieldBlockValue,                          //158 SPELL_AURA_MOD_SHIELD_BLOCKVALUE
    &Aura::HandleNoImmediateEffect,                         //159 SPELL_AURA_NO_PVP_CREDIT      only for Honorless Target spell
    &Aura::HandleNoImmediateEffect,                         //160 SPELL_AURA_MOD_AOE_AVOIDANCE                 implemented in Unit::MagicSpellHitResult
    &Aura::HandleNoImmediateEffect,                         //161 SPELL_AURA_MOD_HEALTH_REGEN_IN_COMBAT
    &Aura::HandleAuraPowerBurn,                             //162 SPELL_AURA_POWER_BURN_MANA
    &Aura::HandleNoImmediateEffect,                         //163 SPELL_AURA_MOD_CRIT_DAMAGE_BONUS_MELEE
    &Aura::HandleUnused,                                    //164 useless, only one test spell
    &Aura::HandleNoImmediateEffect,                         //165 SPELL_AURA_MELEE_ATTACK_POWER_ATTACKER_BONUS implemented in Unit::MeleeDamageBonus
    &Aura::HandleAuraModAttackPowerPercent,                 //166 SPELL_AURA_MOD_ATTACK_POWER_PCT
    &Aura::HandleAuraModRangedAttackPowerPercent,           //167 SPELL_AURA_MOD_RANGED_ATTACK_POWER_PCT
    &Aura::HandleNoImmediateEffect,                         //168 SPELL_AURA_MOD_DAMAGE_DONE_VERSUS            implemented in Unit::SpellDamageBonus, Unit::MeleeDamageBonus
    &Aura::HandleNoImmediateEffect,                         //169 SPELL_AURA_MOD_CRIT_PERCENT_VERSUS           implemented in Unit::DealDamageBySchool, Unit::DoAttackDamage, Unit::SpellCriticalBonus
    &Aura::HandleNULL,                                      //170 SPELL_AURA_DETECT_AMORE       only for Detect Amore spell
    &Aura::HandleAuraModIncreaseSpeed,                      //171 SPELL_AURA_MOD_SPEED_NOT_STACK
    &Aura::HandleAuraModIncreaseMountedSpeed,               //172 SPELL_AURA_MOD_MOUNTED_SPEED_NOT_STACK
    &Aura::HandleUnused,                                    //173 SPELL_AURA_ALLOW_CHAMPION_SPELLS  only for Proclaim Champion spell
    &Aura::HandleModSpellDamagePercentFromStat,             //174 SPELL_AURA_MOD_SPELL_DAMAGE_OF_STAT_PERCENT  implemented in Unit::SpellBaseDamageBonus (by default intellect, dependent from SPELL_AURA_MOD_SPELL_HEALING_OF_STAT_PERCENT)
    &Aura::HandleModSpellHealingPercentFromStat,            //175 SPELL_AURA_MOD_SPELL_HEALING_OF_STAT_PERCENT implemented in Unit::SpellBaseHealingBonus
    &Aura::HandleSpiritOfRedemption,                        //176 SPELL_AURA_SPIRIT_OF_REDEMPTION   only for Spirit of Redemption spell, die at aura end
    &Aura::HandleNULL,                                      //177 SPELL_AURA_AOE_CHARM
    &Aura::HandleNoImmediateEffect,                         //178 SPELL_AURA_MOD_DEBUFF_RESISTANCE          implemented in Unit::MagicSpellHitResult
    &Aura::HandleNoImmediateEffect,                         //179 SPELL_AURA_MOD_ATTACKER_SPELL_CRIT_CHANCE implemented in Unit::SpellCriticalBonus
    &Aura::HandleNoImmediateEffect,                         //180 SPELL_AURA_MOD_FLAT_SPELL_DAMAGE_VERSUS   implemented in Unit::SpellDamageBonus
    &Aura::HandleUnused,                                    //181 SPELL_AURA_MOD_FLAT_SPELL_CRIT_DAMAGE_VERSUS unused
    &Aura::HandleAuraModResistenceOfStatPercent,            //182 SPELL_AURA_MOD_RESISTANCE_OF_STAT_PERCENT
    &Aura::HandleNULL,                                      //183 SPELL_AURA_MOD_CRITICAL_THREAT
    &Aura::HandleNoImmediateEffect,                         //184 SPELL_AURA_MOD_ATTACKER_MELEE_HIT_CHANCE  implemented in Unit::RollMeleeOutcomeAgainst
    &Aura::HandleNoImmediateEffect,                         //185 SPELL_AURA_MOD_ATTACKER_RANGED_HIT_CHANCE implemented in Unit::RollMeleeOutcomeAgainst
    &Aura::HandleNoImmediateEffect,                         //186 SPELL_AURA_MOD_ATTACKER_SPELL_HIT_CHANCE  implemented in Unit::MagicSpellHitResult
    &Aura::HandleNoImmediateEffect,                         //187 SPELL_AURA_MOD_ATTACKER_MELEE_CRIT_CHANCE  implemented in Unit::GetUnitCriticalChance
    &Aura::HandleNoImmediateEffect,                         //188 SPELL_AURA_MOD_ATTACKER_RANGED_CRIT_CHANCE implemented in Unit::GetUnitCriticalChance
    &Aura::HandleModRating,                                 //189 SPELL_AURA_MOD_RATING
    &Aura::HandleNULL,                                      //190 SPELL_AURA_MOD_FACTION_REPUTATION_GAIN
    &Aura::HandleAuraModUseNormalSpeed,                     //191 SPELL_AURA_USE_NORMAL_MOVEMENT_SPEED
    &Aura::HandleModMeleeRangedSpeedPct,                    //192 SPELL_AURA_HASTE_MELEE
    &Aura::HandleModCombatSpeedPct,                         //193 SPELL_AURA_MELEE_SLOW (in fact combat (any type attack) speed pct)
    &Aura::HandleUnused,                                    //194 SPELL_AURA_MOD_DEPRICATED_1 not used now (old SPELL_AURA_MOD_SPELL_DAMAGE_OF_INTELLECT)
    &Aura::HandleUnused,                                    //195 SPELL_AURA_MOD_DEPRICATED_2 not used now (old SPELL_AURA_MOD_SPELL_HEALING_OF_INTELLECT)
    &Aura::HandleNULL,                                      //196 SPELL_AURA_MOD_COOLDOWN
    &Aura::HandleNoImmediateEffect,                         //197 SPELL_AURA_MOD_ATTACKER_SPELL_AND_WEAPON_CRIT_CHANCE implemented in Unit::SpellCriticalBonus Unit::GetUnitCriticalChance
    &Aura::HandleUnused,                                    //198 SPELL_AURA_MOD_ALL_WEAPON_SKILLS
    &Aura::HandleNoImmediateEffect,                         //199 SPELL_AURA_MOD_INCREASES_SPELL_PCT_TO_HIT  implemented in Unit::MagicSpellHitResult
    &Aura::HandleNoImmediateEffect,                         //200 SPELL_AURA_MOD_XP_PCT implemented in Player::GiveXP
    &Aura::HandleAuraAllowFlight,                           //201 SPELL_AURA_FLY                             this aura enable flight mode...
    &Aura::HandleNoImmediateEffect,                         //202 SPELL_AURA_CANNOT_BE_DODGED                implemented in Unit::RollPhysicalOutcomeAgainst
    &Aura::HandleNoImmediateEffect,                         //203 SPELL_AURA_MOD_ATTACKER_MELEE_CRIT_DAMAGE  implemented in Unit::CalculateMeleeDamage and Unit::CalculateSpellDamage
    &Aura::HandleNoImmediateEffect,                         //204 SPELL_AURA_MOD_ATTACKER_RANGED_CRIT_DAMAGE implemented in Unit::CalculateMeleeDamage and Unit::CalculateSpellDamage
    &Aura::HandleNULL,                                      //205 vulnerable to school dmg?
    &Aura::HandleAuraModIncreaseFlightSpeed,                //206 SPELL_AURA_MOD_FLIGHT_SPEED
    &Aura::HandleAuraModIncreaseFlightSpeed,                //207 SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED
    &Aura::HandleAuraModIncreaseFlightSpeed,                //208 SPELL_AURA_MOD_FLIGHT_SPEED_STACKING
    &Aura::HandleAuraModIncreaseFlightSpeed,                //209 SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED_STACKING
    &Aura::HandleAuraModIncreaseFlightSpeed,                //210 SPELL_AURA_MOD_FLIGHT_SPEED_NOT_STACKING
    &Aura::HandleAuraModIncreaseFlightSpeed,                //211 SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED_NOT_STACKING
    &Aura::HandleAuraModRangedAttackPowerOfStatPercent,     //212 SPELL_AURA_MOD_RANGED_ATTACK_POWER_OF_STAT_PERCENT
    &Aura::HandleNoImmediateEffect,                         //213 SPELL_AURA_MOD_RAGE_FROM_DAMAGE_DEALT implemented in Player::RewardRage
    &Aura::HandleNULL,                                      //214 Tamed Pet Passive
    &Aura::HandleArenaPreparation,                          //215 SPELL_AURA_ARENA_PREPARATION
    &Aura::HandleModCastingSpeed,                           //216 SPELL_AURA_HASTE_SPELLS
    &Aura::HandleUnused,                                    //217                                   unused
    &Aura::HandleAuraModRangedHaste,                        //218 SPELL_AURA_HASTE_RANGED
    &Aura::HandleModManaRegen,                              //219 SPELL_AURA_MOD_MANA_REGEN_FROM_STAT
    &Aura::HandleNULL,                                      //220 SPELL_AURA_MOD_RATING_FROM_STAT
    &Aura::HandleNULL,                                      //221 ignored
    &Aura::HandleUnused,                                    //222 unused
    &Aura::HandleNULL,                                      //223 Cold Stare
    &Aura::HandleUnused,                                    //224 unused
    &Aura::HandleNoImmediateEffect,                         //225 SPELL_AURA_PRAYER_OF_MENDING
    &Aura::HandleAuraPeriodicDummy,                         //226 SPELL_AURA_PERIODIC_DUMMY
    &Aura::HandlePeriodicTriggerSpellWithValue,             //227 SPELL_AURA_PERIODIC_TRIGGER_SPELL_WITH_VALUE
    &Aura::HandleNoImmediateEffect,                         //228 stealth detection
    &Aura::HandleNULL,                                      //229 SPELL_AURA_MOD_AOE_DAMAGE_AVOIDANCE
    &Aura::HandleAuraModIncreaseMaxHealth,                  //230 Commanding Shout
    &Aura::HandleNoImmediateEffect,                         //231 SPELL_AURA_PROC_TRIGGER_SPELL_WITH_VALUE
    &Aura::HandleNoImmediateEffect,                         //232 SPELL_AURA_MECHANIC_DURATION_MOD           implement in Unit::CalculateSpellDuration
    &Aura::HandleNULL,                                      //233 set model id to the one of the creature with id m_modifier.m_miscvalue
    &Aura::HandleNoImmediateEffect,                         //234 SPELL_AURA_MECHANIC_DURATION_MOD_NOT_STACK implement in Unit::CalculateSpellDuration
    &Aura::HandleAuraModDispelResist,                       //235 SPELL_AURA_MOD_DISPEL_RESIST               implement in Unit::MagicSpellHitResult
    &Aura::HandleUnused,                                    //236 unused
    &Aura::HandleModSpellDamagePercentFromAttackPower,      //237 SPELL_AURA_MOD_SPELL_DAMAGE_OF_ATTACK_POWER  implemented in Unit::SpellBaseDamageBonus
    &Aura::HandleModSpellHealingPercentFromAttackPower,     //238 SPELL_AURA_MOD_SPELL_HEALING_OF_ATTACK_POWER implemented in Unit::SpellBaseHealingBonus
    &Aura::HandleAuraModScale,                              //239 SPELL_AURA_MOD_SCALE_2 only in Noggenfogger Elixir (16595) before 2.3.0 aura 61
    &Aura::HandleAuraModExpertise,                          //240 SPELL_AURA_MOD_EXPERTISE
    &Aura::HandleForceMoveForward,                          //241 Forces the player to move forward
    &Aura::HandleUnused,                                    //242 SPELL_AURA_MOD_SPELL_DAMAGE_FROM_HEALING
    &Aura::HandleUnused,                                    //243 used by two test spells
    &Aura::HandleComprehendLanguage,                        //244 Comprehend language
    &Aura::HandleUnused,                                    //245 SPELL_AURA_MOD_DURATION_OF_MAGIC_EFFECTS
    &Aura::HandleUnused,                                    //246 unused
    &Aura::HandleAuraCloneCaster,                           //247 SPELL_AURA_CLONE_CASTER
    &Aura::HandleNoImmediateEffect,                         //248 SPELL_AURA_MOD_COMBAT_RESULT_CHANCE         implemented in Unit::RollMeleeOutcomeAgainst
    &Aura::HandleNULL,                                      //249
    &Aura::HandleAuraModIncreaseHealth,                     //250 SPELL_AURA_MOD_INCREASE_HEALTH_2
    &Aura::HandleNULL,                                      //251 SPELL_AURA_MOD_ENEMY_DODGE
    &Aura::HandleNoImmediateEffect,                         //252 SPELL_AURA_REUSED_BLESSED_LIFE
    &Aura::HandleIncreasePetOutdoorSpeed,                   //253 SPELL_INCREASE_PET_OUTDOOR_SPEED (used for bestial swiftness)
    &Aura::HandleUnused,                                    //254 unused
    &Aura::HandleUnused,                                    //255 unused
    &Aura::HandleUnused,                                    //256 unused
    &Aura::HandleUnused,                                    //257 unused
    &Aura::HandleUnused,                                    //258 unused
    &Aura::HandleUnused,                                    //259 unused
    &Aura::HandleUnused,                                    //260 unused
    &Aura::HandleNULL                                       //261 SPELL_AURA_261 some phased state (44856 spell)
};

Aura::Aura(SpellEntry const* spellproto, uint32 eff, int32* currentBasePoints, Unit* target, Unit* caster, Item* castItem) :
    m_procCharges(0), m_spellmod(NULL), m_effIndex(eff), m_caster_guid(0), m_target(target), m_tickNumber(0),
    m_timeCla(1000), m_castItemGuid(castItem ? castItem->GetGUID() : 0), m_removeMode(AURA_REMOVE_BY_DEFAULT), m_auraSlot(MAX_AURAS),
    m_positive(false), m_permanent(false), m_isPeriodic(false), m_isAreaAura(false),
    m_isPersistent(false), m_isRemovedOnShapeLost(true), m_isRemoved(false), m_in_use(false),
    m_periodicTimer(0), m_amplitude(0), m_PeriodicEventId(0), m_AuraDRGroup(DIMINISHING_NONE)
    , m_stackAmount(1)
{
    ASSERT(target);

    ASSERT(spellproto && spellproto == sSpellStore.LookupEntry(spellproto->Id) && "`info` must be pointer to sSpellStore element");

    m_spellProto = spellproto;

    int32 damage;
    if (currentBasePoints)
    {
        damage = *currentBasePoints;
        m_currentBasePoints = damage - 1;
    }
    else
    {
        m_currentBasePoints = m_spellProto->EffectBasePoints[eff];
        if (caster)
            damage = caster->CalculateSpellDamage(m_spellProto, m_effIndex, m_currentBasePoints, target);
        else
            damage = m_currentBasePoints + 1;
    }

    m_isPassive = IsPassiveSpell(GetId());
    m_positive = IsPositiveEffect(GetId(), m_effIndex);

    m_applyTime = time(NULL);

    m_isSingleTargetAura = IsSingleTargetSpell(m_spellProto);

    if (!caster)
    {
        m_caster_guid = target->GetGUID();
        //damage = m_currentBasePoints+1;                     // stored value-1
        m_maxduration = target->CalculateSpellDuration(m_spellProto, m_effIndex, target);
    }
    else
    {
        m_caster_guid = caster->GetGUID();

        //damage        = caster->CalculateSpellDamage(m_spellProto,m_effIndex,m_currentBasePoints,target);
        m_maxduration = caster->CalculateSpellDuration(m_spellProto, m_effIndex, target);

        if (!damage && castItem && castItem->GetItemSuffixFactor())
        {
            ItemRandomSuffixEntry const* item_rand_suffix = sItemRandomSuffixStore.LookupEntry(abs(castItem->GetItemRandomPropertyId()));
            if (item_rand_suffix)
            {
                for (int k = 0; k < 3; k++)
                {
                    SpellItemEnchantmentEntry const* pEnchant = sSpellItemEnchantmentStore.LookupEntry(item_rand_suffix->enchant_id[k]);
                    if (pEnchant)
                    {
                        for (int t = 0; t < 3; t++)
                            if (pEnchant->spellid[t] == m_spellProto->Id)
                            {
                                damage = uint32((item_rand_suffix->prefix[k] * castItem->GetItemSuffixFactor()) / 10000);
                                break;
                            }
                    }

                    if (damage)
                        break;
                }
            }
        }
    }

    if (m_maxduration == -1 || (m_isPassive && m_spellProto->DurationIndex == 0))
        m_permanent = true;

    Player* modOwner = caster ? caster->GetSpellModOwner() : NULL;

    if (!m_permanent && modOwner)
        modOwner->ApplySpellMod(GetId(), SPELLMOD_DURATION, m_maxduration);

    m_duration = m_maxduration;

    DEBUG_LOG("Aura: construct Spellid : %u, Aura : %u Duration : %d Target : %d Damage : %d", m_spellProto->Id, m_spellProto->EffectApplyAuraName[eff], m_maxduration, m_spellProto->EffectImplicitTargetA[eff], damage);

    m_effIndex = eff;
    SetModifier(AuraType(m_spellProto->EffectApplyAuraName[eff]), damage, m_spellProto->EffectAmplitude[eff], m_spellProto->EffectMiscValue[eff]);

    if (modOwner)
        modOwner->ApplySpellMod(GetId(), SPELLMOD_ACTIVATION_TIME, m_periodicTimer);

    // Start periodic on next tick or at aura apply
    if (!(m_spellProto->AttributesEx5 & SPELL_ATTR5_START_PERIODIC_AT_APPLY))
        m_periodicTimer += m_modifier.periodictime;

    m_isDeathPersist = IsDeathPersistentSpell(m_spellProto);

    if (m_spellProto->procCharges)
    {
        m_procCharges = m_spellProto->procCharges;

        if (modOwner)
            modOwner->ApplySpellMod(GetId(), SPELLMOD_CHARGES, m_procCharges);
    }
    else
        m_procCharges = -1;

    m_amplitude = m_modifier.periodictime;
    if (IsChanneledSpell(m_spellProto) && caster)
        caster->ModSpellDurationTime(m_spellProto, m_amplitude, NULL);

    m_isRemovedOnShapeLost = (m_caster_guid == m_target->GetGUID() && m_spellProto->Stances &&
                              !(m_spellProto->AttributesEx2 & 0x80000) && !(m_spellProto->Attributes & 0x10000));

    //  WeakenedSoul       Sweeping Strikes
    if (GetId() == 6788 || GetId() == 12328)
        m_isRemovedOnShapeLost = false;
}

Aura::~Aura()
{
}

AreaAura::AreaAura(SpellEntry const* spellproto, uint32 eff, int32* currentBasePoints, Unit* target,
                   Unit* caster, Item* castItem) : Aura(spellproto, eff, currentBasePoints, target, caster, castItem)
{
    m_isAreaAura = true;

    // caster == NULL in constructor args if target == caster in fact
    Unit* caster_ptr = caster ? caster : target;

    if (spellproto->Effect[eff] == SPELL_EFFECT_APPLY_AREA_AURA_ENEMY)
        m_radius = GetSpellRadius(spellproto, m_effIndex, false);
    else
        m_radius = GetSpellRadius(spellproto, m_effIndex, true);

    if (Player* modOwner = caster_ptr->GetSpellModOwner())
        modOwner->ApplySpellMod(GetId(), SPELLMOD_RADIUS, m_radius);

    switch (spellproto->Effect[eff])
    {
    case SPELL_EFFECT_APPLY_AREA_AURA_PARTY:
        m_areaAuraType = AREA_AURA_PARTY;
        if (target->GetTypeId() == TYPEID_UNIT && target->ToCreature()->IsTotem())
            m_modifier.m_auraname = SPELL_AURA_NONE;
        break;
    case SPELL_EFFECT_APPLY_AREA_AURA_FRIEND:
        m_areaAuraType = AREA_AURA_FRIEND;
        break;
    case SPELL_EFFECT_APPLY_AREA_AURA_ENEMY:
        m_areaAuraType = AREA_AURA_ENEMY;
        if (target == caster_ptr)
            m_modifier.m_auraname = SPELL_AURA_NONE;    // Do not do any effect on self
        break;
    case SPELL_EFFECT_APPLY_AREA_AURA_PET:
        m_areaAuraType = AREA_AURA_PET;
        break;
    case SPELL_EFFECT_APPLY_AREA_AURA_OWNER:
        m_areaAuraType = AREA_AURA_OWNER;
        if (target == caster_ptr)
            m_modifier.m_auraname = SPELL_AURA_NONE;
        break;
    default:
        sLog.outError("Wrong spell effect in AreaAura constructor");
        ASSERT(false);
        break;
    }
}

AreaAura::~AreaAura()
{
}

PersistentAreaAura::PersistentAreaAura(SpellEntry const* spellproto, uint32 eff, int32* currentBasePoints, Unit* target,
                                       Unit* caster, Item* castItem) : Aura(spellproto, eff, currentBasePoints, target, caster, castItem)
{
    m_isPersistent = true;
}

PersistentAreaAura::~PersistentAreaAura()
{
}

Aura* CreateAura(SpellEntry const* spellproto, uint32 eff, int32* currentBasePoints, Unit* target, Unit* caster, Item* castItem)
{
    if (IsAreaAuraEffect(spellproto->Effect[eff]))
        return new AreaAura(spellproto, eff, currentBasePoints, target, caster, castItem);

    return new Aura(spellproto, eff, currentBasePoints, target, caster, castItem);
}

Unit* Aura::GetCaster() const
{
    if (m_caster_guid == m_target->GetGUID())
        return m_target;

    //return ObjectAccessor::GetUnit(*m_target,m_caster_guid);
    //must return caster even if it's in another grid/map
    Unit* unit = ObjectAccessor::GetObjectInWorld(m_caster_guid, (Unit*)NULL);
    return unit && unit->IsInWorld() ? unit : NULL;
}

void Aura::SetModifier(AuraType t, int32 a, uint32 pt, int32 miscValue)
{
    m_modifier.m_auraname = t;
    m_modifier.m_amount = a;
    m_modifier.m_miscvalue = miscValue;
    m_modifier.periodictime = pt;
}

void Aura::Update(uint32 diff)
{
    if (m_duration > 0)
    {
        m_duration -= diff;
        if (m_duration < 0)
            m_duration = 0;
        m_timeCla -= diff;

        // GetEffIndex() == 0 prevent double/triple apply manaPerSecond/manaPerSecondPerLevel to same spell with many auras
        // all spells with manaPerSecond/manaPerSecondPerLevel have aura in effect 0
        if (GetEffIndex() == 0 && m_timeCla <= 0)
        {
            if (Unit* caster = GetCaster())
            {
                Powers powertype = Powers(m_spellProto->powerType);
                int32 manaPerSecond = m_spellProto->manaPerSecond + m_spellProto->manaPerSecondPerLevel * caster->getLevel();
                m_timeCla = 1 * IN_MILLISECONDS;
                if (manaPerSecond)
                {
                    if (powertype == POWER_HEALTH)
                        caster->ModifyHealth(-manaPerSecond);
                    else
                        caster->ModifyPower(powertype, -manaPerSecond);
                }
            }
        }
    }

    // Channeled aura required check distance from caster except in possessed cases
    Unit* pRealTarget = (GetSpellProto()->EffectApplyAuraName[m_effIndex] == SPELL_AURA_PERIODIC_TRIGGER_SPELL &&
                         sSpellStore.LookupEntry(GetSpellProto()->EffectTriggerSpell[m_effIndex]) &&
                         !IsAreaOfEffectSpell(sSpellStore.LookupEntry(GetSpellProto()->EffectTriggerSpell[m_effIndex])) &&
                         GetTriggerTarget()) ? GetTriggerTarget() : m_target;

    if (IsChanneledSpell(m_spellProto) && !pRealTarget->isPossessed() && pRealTarget->GetGUID() != GetCasterGUID())
    {
        Unit* caster = GetCaster();
        if (!caster)
        {
            m_target->RemoveAura(GetId(), GetEffIndex());
            return;
        }

        // Get spell range
        float radius;
        SpellModOp mod;
        if (m_spellProto->EffectRadiusIndex[GetEffIndex()])
        {
            radius = GetSpellRadius(m_spellProto, GetEffIndex(), false);
            mod = SPELLMOD_RADIUS;
        }
        else
        {
            radius = GetSpellMaxRange(sSpellRangeStore.LookupEntry(m_spellProto->rangeIndex));
            mod = SPELLMOD_RANGE;
        }

        if (Player* modOwner = caster->GetSpellModOwner())
            modOwner->ApplySpellMod(GetId(), mod, radius, NULL);

        if (!caster->IsWithinDistInMap(pRealTarget, radius))
            return;
    }

    if (m_isPeriodic && (m_duration >= 0 || m_isPassive || m_permanent))
    {
        m_periodicTimer -= diff;

        if (m_periodicTimer <= 0)                            // tick also at m_periodicTimer == 0 to prevent lost last tick in case max m_duration == (max m_periodicTimer)*N
        {

            if (m_modifier.m_auraname == SPELL_AURA_MOD_REGEN ||
                m_modifier.m_auraname == SPELL_AURA_MOD_POWER_REGEN ||
                // Cannibalize, eating items and other spells
                m_modifier.m_auraname == SPELL_AURA_OBS_MOD_HEALTH ||
                // Eating items and other spells
                m_modifier.m_auraname == SPELL_AURA_OBS_MOD_MANA)
            {
                ApplyModifier(true);
                return;
            }
            // update before applying (aura can be removed in TriggerSpell or PeriodicTick calls)
            m_periodicTimer += m_amplitude;//m_modifier.periodictime;
            ++m_tickNumber;

            if (!m_target->HasUnitState(UNIT_STATE_ISOLATED))
                PeriodicTick();
        }
    }
}

bool AreaAura::CheckTarget(Unit* target)
{
    if (target->HasAura(GetId(), m_effIndex))
        return false;

    // Skip some targets (TODO: Might require better checks, also unclear how the actual caster must/can be handled)
    if (GetSpellProto()->AttributesEx3 & SPELL_ATTR3_PLAYERS_ONLY && target->GetTypeId() != TYPEID_PLAYER)
        return false;

    // some special cases
    switch (GetId())
    {
    case 45828: // AV Marshal's HP/DMG auras
    case 45829:
    case 45831:
    case 45830:
    case 45822: // AV Warmaster's HP/DMG auras
    case 45823:
    case 45824:
    case 45826:
        {
            switch (target->GetEntry())
            {
            // alliance
            case 14762: // Dun Baldar North Marshal
            case 14763: // Dun Baldar South Marshal
            case 14764: // Icewing Marshal
            case 14765: // Stonehearth Marshal
            case 11948: // Vandar Stormspike
            // horde
            case 14772: // East Frostwolf Warmaster
            case 14776: // Tower Point Warmaster
            case 14773: // Iceblood Warmaster
            case 14777: // West Frostwolf Warmaster
            case 11946: // Drek'thar
                return true;
            default:
                break;
            }
            return false;
        }
        break;
    default:
        break;
    }
    return true;
}

void AreaAura::Update(uint32 diff)
{
    // update for the caster of the aura
    if (m_caster_guid == m_target->GetGUID())
    {
        Unit* caster = m_target;

        if (!caster->HasUnitState(UNIT_STATE_ISOLATED))
        {
            std::list<Unit* > targets;

            switch (m_areaAuraType)
            {
            case AREA_AURA_PARTY:
                caster->GetPartyMember(targets, m_radius);
                break;
            case AREA_AURA_FRIEND:
                {
                    Oregon::AnyFriendlyUnitInObjectRangeCheck u_check(caster, caster, m_radius);
                    Oregon::UnitListSearcher<Oregon::AnyFriendlyUnitInObjectRangeCheck> searcher(targets, u_check);
                    caster->VisitNearbyObject(m_radius, searcher);
                    break;
                }
            case AREA_AURA_ENEMY:
                {
                    Oregon::AnyAoETargetUnitInObjectRangeCheck u_check(caster, caster, m_radius); // No GetCharmer in searcher
                    Oregon::UnitListSearcher<Oregon::AnyAoETargetUnitInObjectRangeCheck> searcher(targets, u_check);
                    caster->VisitNearbyObject(m_radius, searcher);
                    break;
                }
            case AREA_AURA_OWNER:
            case AREA_AURA_PET:
                {
                    if (Unit* owner = caster->GetCharmerOrOwner())
                        targets.push_back(owner);
                    break;
                }
            }

            for (std::list<Unit* >::iterator tIter = targets.begin(); tIter != targets.end(); tIter++)
            {
                if (!CheckTarget(*tIter))
                    continue;

                if (SpellEntry const* actualSpellInfo = sSpellMgr.SelectAuraRankForPlayerLevel(GetSpellProto(), (*tIter)->getLevel()))
                {
                    //int32 actualBasePoints = m_currentBasePoints;
                    // recalculate basepoints for lower rank (all AreaAura spell not use custom basepoints?)
                    //if (actualSpellInfo != GetSpellProto())
                    //    actualBasePoints = actualSpellInfo->EffectBasePoints[m_effIndex];
                    AreaAura* aur;
                    if (actualSpellInfo == GetSpellProto())
                        aur = new AreaAura(actualSpellInfo, m_effIndex, &m_modifier.m_amount, (*tIter), caster, NULL);
                    else
                        aur = new AreaAura(actualSpellInfo, m_effIndex, NULL, (*tIter), caster, NULL);
                    (*tIter)->AddAura(aur);

                    if (m_areaAuraType == AREA_AURA_ENEMY)
                        caster->CombatStart(*tIter);
                }
            }
        }
        Aura::Update(diff);
    }
    else                                                    // aura at non-caster
    {
        Unit* tmp_target = m_target;
        Unit* caster = GetCaster();
        uint32 tmp_spellId = GetId(), tmp_effIndex = m_effIndex;

        // WARNING: the aura may get deleted during the update
        // DO NOT access its members after update!
        Aura::Update(diff);

        // remove aura if out-of-range from caster (after teleport for example)
        // or caster is isolated or caster no longer has the aura
        // or caster is (no longer) friendly
        bool needFriendly = (m_areaAuraType == AREA_AURA_ENEMY ? false : true);
        if (!caster || caster->HasUnitState(UNIT_STATE_ISOLATED) ||
            !caster->IsWithinDistInMap(tmp_target, m_radius)    ||
            !caster->HasAura(tmp_spellId, tmp_effIndex)         ||
            caster->IsFriendlyTo(tmp_target) != needFriendly
           )
            tmp_target->RemoveAura(tmp_spellId, tmp_effIndex);
        else if (m_areaAuraType == AREA_AURA_PARTY)         // check if in same sub group
        {
            if (!tmp_target->IsInPartyWith(caster))
                tmp_target->RemoveAura(tmp_spellId, tmp_effIndex);
        }
        else if (m_areaAuraType == AREA_AURA_PET || m_areaAuraType == AREA_AURA_OWNER)
        {
            if (tmp_target->GetGUID() != caster->GetCharmerOrOwnerGUID())
                tmp_target->RemoveAura(tmp_spellId, tmp_effIndex);
        }
    }
}

void PersistentAreaAura::Update(uint32 diff)
{
    bool remove = false;

    // remove the aura if its caster or the dynamic object causing it was removed
    // or if the target moves too far from the dynamic object
    Unit* caster = GetCaster();
    if (caster)
    {
        DynamicObject* dynObj = caster->GetDynObject(GetId(), GetEffIndex());
        if (dynObj)
        {
            if (!m_target->IsWithinDistInMap(dynObj, dynObj->GetRadius()))
                remove = true;
        }
        else
            remove = true;
    }
    else
        remove = true;

    Unit* tmp_target = m_target;
    uint32 tmp_id = GetId(), tmp_index = GetEffIndex();

    // WARNING: the aura may get deleted during the update
    // DO NOT access its members after update!
    Aura::Update(diff);

    if (remove)
        tmp_target->RemoveAura(tmp_id, tmp_index);
}

void Aura::ApplyModifier(bool apply, bool Real)
{
    if (IsRemoved())
        return;

    AuraType aura = m_modifier.m_auraname;

    m_in_use = true;
    if (aura < TOTAL_AURAS)
        (*this.*AuraHandler [aura])(apply, Real);
    m_in_use = false;
}

void Aura::UpdateAuraDuration()
{
    if (m_auraSlot >= MAX_AURAS || m_isPassive)
        return;

    if (m_target->GetTypeId() == TYPEID_PLAYER)
    {
        WorldPacket data(SMSG_UPDATE_AURA_DURATION, 5);
        data << (uint8)m_auraSlot << (uint32)m_duration;
        m_target->ToPlayer()->SendDirectMessage(&data);

        data.Initialize(SMSG_SET_EXTRA_AURA_INFO, (8 + 1 + 4 + 4 + 4));
        data << m_target->GetPackGUID();
        data << uint8(m_auraSlot);
        data << uint32(GetId());
        data << uint32(GetAuraMaxDuration());
        data << uint32(GetAuraDuration());
        m_target->ToPlayer()->SendDirectMessage(&data);
    }

    // not send in case player loading (will not work anyway until player not added to map), sent in visibility change code
    if (m_target->GetTypeId() == TYPEID_PLAYER && m_target->ToPlayer()->GetSession()->PlayerLoading())
        return;

    Unit* caster = GetCaster();

    if (caster && caster->GetTypeId() == TYPEID_PLAYER)
    {
        SendAuraDurationForCaster(caster->ToPlayer());

        Group* CasterGroup = caster->ToPlayer()->GetGroup();
        if (CasterGroup && (sSpellMgr.GetSpellCustomAttr(GetId()) & SPELL_ATTR_CU_AURA_CC))
        {
            for (GroupReference* itr = CasterGroup->GetFirstMember(); itr != NULL; itr = itr->next())
            {
                Player* player = itr->GetSource();
                if (player && player != caster)
                    SendAuraDurationForCaster(player);
            }
        }
    }
}

void Aura::SendAuraDurationForCaster(Player* caster)
{
    if (caster == m_target)
        return;

    WorldPacket data(SMSG_SET_EXTRA_AURA_INFO_NEED_UPDATE, (8 + 1 + 4 + 4 + 4));
    data << m_target->GetPackGUID();
    data << uint8(m_auraSlot);
    data << uint32(GetId());
    data << uint32(GetAuraMaxDuration());                   // full
    data << uint32(GetAuraDuration());                      // remain
    caster->GetSession()->SendPacket(&data);
}

void Aura::_AddAura()
{
    if (!GetId())
        return;
    if (!m_target)
        return;

    // we can found aura in NULL_AURA_SLOT and then need store state instead check slot != NULL_AURA_SLOT
    bool secondaura = false;
    uint8 slot = NULL_AURA_SLOT;

    for (uint8 i = 0; i < 3; i++)
    {
        Unit::spellEffectPair spair = Unit::spellEffectPair(GetId(), i);
        for (Unit::AuraMap::const_iterator itr = m_target->GetAuras().lower_bound(spair); itr != m_target->GetAuras().upper_bound(spair); ++itr)
        {
            // allow use single slot only by auras from same caster and item
            if (itr->second->GetCasterGUID() == GetCasterGUID() && itr->second->GetCastItemGUID() == GetCastItemGUID())
            {
                secondaura = true;
                slot = itr->second->GetAuraSlot();
                break;
            }
        }

        if (secondaura)
            break;
    }

    Unit* caster = GetCaster();

    // not call total regen auras at adding
    switch (m_modifier.m_auraname)
    {
    case SPELL_AURA_OBS_MOD_HEALTH:
    case SPELL_AURA_OBS_MOD_MANA:
        m_periodicTimer = m_amplitude;//m_modifier.periodictime;
        break;
    case SPELL_AURA_MOD_REGEN:
    case SPELL_AURA_MOD_POWER_REGEN:
    case SPELL_AURA_MOD_MANA_REGEN_FROM_STAT:
        m_amplitude = 5000;
        m_periodicTimer = m_amplitude;
        break;
    default:
        break;
    }

    // register aura
    if (getDiminishGroup() != DIMINISHING_NONE)
        m_target->ApplyDiminishingAura(getDiminishGroup(), true);

    // Show the buff (if any) on the target
    if ((m_spellProto->AttributesEx3 & SPELL_ATTR3_HAS_VISUAL_EFFECT || !IsPassive()) &&
        (m_spellProto->Effect[GetEffIndex()] != SPELL_EFFECT_APPLY_AREA_AURA_ENEMY || m_target != caster))
    {
        if (!secondaura)                                     // new slot need
        {
            if (IsPositive())                               // empty positive slot
            {
                for (uint8 i = 0; i < MAX_POSITIVE_AURAS; i++)
                {
                    if (m_target->GetUInt32Value((uint16)(UNIT_FIELD_AURA + i)) == 0)
                    {
                        slot = i;
                        break;
                    }
                }
            }
            else                                            // empty negative slot
            {
                for (uint8 i = MAX_POSITIVE_AURAS; i < MAX_AURAS; i++)
                {
                    if (m_target->GetUInt32Value((uint16)(UNIT_FIELD_AURA + i)) == 0)
                    {
                        slot = i;
                        break;
                    }
                }
            }

            SetAuraSlot(slot);

            // Not update fields for not first spell's aura, all data already in fields
            if (slot < MAX_AURAS)                        // slot found
            {
                SetAura(slot, false);
                SetAuraFlag(slot, true);
                SetAuraLevel(slot, caster ? caster->getLevel() : sWorld.getConfig(CONFIG_MAX_PLAYER_LEVEL));
                UpdateAuraCharges();

                // update for out of range group members
                m_target->UpdateAuraForGroup(slot);
            }
        }
        else                                                // use found slot
            SetAuraSlot(slot);

        UpdateSlotCounterAndDuration();

        // Update Seals information
        if (IsSealSpell(GetSpellProto()))
            m_target->ModifyAuraState(AURA_STATE_JUDGEMENT, true);

        // Conflagrate aura state
        if (GetSpellProto()->SpellFamilyName == SPELLFAMILY_WARLOCK && (GetSpellProto()->SpellFamilyFlags & 4))
            m_target->ModifyAuraState(AURA_STATE_IMMOLATE, true);

        if (GetSpellProto()->SpellFamilyName == SPELLFAMILY_DRUID
            && (GetSpellProto()->SpellFamilyFlags == 0x40 || GetSpellProto()->SpellFamilyFlags == 0x10))
            m_target->ModifyAuraState(AURA_STATE_SWIFTMEND, true);
    }
}

void Aura::_RemoveAura()
{
    Unit* caster = GetCaster();

    if (caster && IsPersistent())
    {
        DynamicObject* dynObj = caster->GetDynObject(GetId(), GetEffIndex());
        if (dynObj)
            dynObj->RemoveAffected(m_target);
    }

    // unregister aura
    if (getDiminishGroup() != DIMINISHING_NONE)
        m_target->ApplyDiminishingAura(getDiminishGroup(), false);

    //passive auras do not get put in slots
    // Note: but totem can be not accessible for aura target in time remove (to far for find in grid)
    //if (m_isPassive && !(caster && caster->GetTypeId() == TYPEID_UNIT && caster->ToCreature()->IsTotem()))
    //    return;

    uint8 slot = GetAuraSlot();

    if (slot >= MAX_AURAS)                                   // slot not set
        return;

    if (m_target->GetUInt32Value((uint16)(UNIT_FIELD_AURA + slot)) == 0)
        return;

    bool samespell = false;

    // find other aura in same slot (current already removed from list)
    for (uint8 i = 0; i < 3; i++)
    {
        Unit::spellEffectPair spair = Unit::spellEffectPair(GetId(), i);
        for (Unit::AuraMap::const_iterator itr = m_target->GetAuras().lower_bound(spair); itr != m_target->GetAuras().upper_bound(spair); ++itr)
        {
            if (itr->second->GetAuraSlot() == slot)
            {
                samespell = true;
                break;
            }
        }
        if (samespell)
            break;
    }

    // only remove icon when the last aura of the spell is removed (current aura already removed from list)
    if (!samespell)
    {
        SetAura(slot, true);
        SetAuraFlag(slot, false);
        SetAuraLevel(slot, caster ? caster->getLevel() : sWorld.getConfig(CONFIG_MAX_PLAYER_LEVEL));

        SetAuraApplication(slot, 0);
        // update for out of range group members
        m_target->UpdateAuraForGroup(slot);

        if (IsSealSpell(GetSpellProto()))
            m_target->ModifyAuraState(AURA_STATE_JUDGEMENT, false);

        // Conflagrate aura state
        if (GetSpellProto()->SpellFamilyName == SPELLFAMILY_WARLOCK && (GetSpellProto()->SpellFamilyFlags & 4))
            m_target->ModifyAuraState(AURA_STATE_IMMOLATE, false);

        // Swiftmend aura state
        if (GetSpellProto()->SpellFamilyName == SPELLFAMILY_DRUID
            && (GetSpellProto()->SpellFamilyFlags == 0x40 || GetSpellProto()->SpellFamilyFlags == 0x10))
        {
            bool found = false;
            Unit::AuraList const& RejorRegr = m_target->GetAurasByType(SPELL_AURA_PERIODIC_HEAL);
            for (Unit::AuraList::const_iterator i = RejorRegr.begin(); i != RejorRegr.end(); ++i)
            {
                if ((*i)->GetSpellProto()->SpellFamilyName == SPELLFAMILY_DRUID
                    && ((*i)->GetSpellProto()->SpellFamilyFlags == 0x40 || (*i)->GetSpellProto()->SpellFamilyFlags == 0x10))
                {
                    found = true;
                    break;
                }
            }
            if (!found)
                m_target->ModifyAuraState(AURA_STATE_SWIFTMEND, false);
        }

        // reset cooldown state for spells
        if (caster && caster->GetTypeId() == TYPEID_PLAYER)
        {
            if (GetSpellProto()->Attributes & SPELL_ATTR0_DISABLED_WHILE_ACTIVE)
                caster->ToPlayer()->SendCooldownEvent(GetSpellProto());
        }
    }
    m_isRemoved = true;
}

void Aura::SetAuraFlag(uint32 slot, bool add)
{
    uint32 index    = slot / 4;
    uint32 byte     = (slot % 4) * 8;
    uint32 val      = m_target->GetUInt32Value(UNIT_FIELD_AURAFLAGS + index);
    val &= ~((uint32)AFLAG_MASK << byte);
    if (add)
    {
        if (IsPositive())
            val |= ((uint32)AFLAG_POSITIVE << byte);
        else
            val |= ((uint32)AFLAG_NEGATIVE << byte);
    }
    m_target->SetUInt32Value(UNIT_FIELD_AURAFLAGS + index, val);
}

void Aura::SetAuraLevel(uint32 slot, uint32 level)
{
    uint32 index    = slot / 4;
    uint32 byte     = (slot % 4) * 8;
    uint32 val      = m_target->GetUInt32Value(UNIT_FIELD_AURALEVELS + index);
    val &= ~(0xFF << byte);
    val |= (level << byte);
    m_target->SetUInt32Value(UNIT_FIELD_AURALEVELS + index, val);
}

void Aura::SetAuraApplication(uint32 slot, int8 count)
{
    uint32 index    = slot / 4;
    uint32 byte     = (slot % 4) * 8;
    uint32 val      = m_target->GetUInt32Value(UNIT_FIELD_AURAAPPLICATIONS + index);
    val &= ~(0xFF << byte);
    val |= ((uint8(count)) << byte);
    m_target->SetUInt32Value(UNIT_FIELD_AURAAPPLICATIONS + index, val);
}

void Aura::UpdateSlotCounterAndDuration()
{
    uint8 slot = GetAuraSlot();
    if (slot >= MAX_AURAS)
        return;

    // Three possibilities:
    // Charge = 0; Stack >= 0
    // Charge = 1; Stack >= 0
    // Charge > 1; Stack = 0
    if (m_procCharges < 2)
        SetAuraApplication(slot, m_stackAmount - 1);

    UpdateAuraDuration();
}

/*********************************************************/
/***              BASIC AURA FUNCTION                 ***/
/*********************************************************/
void Aura::HandleAddModifier(bool apply, bool Real)
{
    if (m_target->GetTypeId() != TYPEID_PLAYER || !Real)
        return;

    SpellEntry const* spellInfo = GetSpellProto();
    if (!spellInfo)
        return;

    if (m_modifier.m_miscvalue >= MAX_SPELLMOD)
        return;

    if (apply)
    {
        // Add custom charges for some mod aura
        switch (m_spellProto->Id)
        {
        case 17941:    // Shadow Trance
        case 22008:    // Netherwind Focus
        case 34754:    // Clearcasting
        case 34936:    // Backlash

            m_procCharges = 1;
            break;
        }

        SpellModifier* mod = new SpellModifier;
        mod->op = SpellModOp(m_modifier.m_miscvalue);
        mod->value = GetModifierValue();
        mod->type = SpellModType(m_modifier.m_auraname);    // SpellModType value == spell aura types
        mod->spellId = GetId();
        mod->effectId = m_effIndex;
        mod->lastAffected = NULL;

        uint64 spellAffectMask = sSpellMgr.GetSpellAffectMask(GetId(), m_effIndex);

        if (spellAffectMask)
            mod->mask = spellAffectMask;
        else
            mod->mask = spellInfo->EffectItemType[m_effIndex];

        if (m_procCharges > 0)
            mod->charges = m_procCharges;
        else
            mod->charges = 0;

        m_spellmod = mod;
    }

    uint64 spellFamilyMask = m_spellmod->mask;

    m_target->ToPlayer()->AddSpellMod(m_spellmod, apply);

    // reapply some passive spells after add/remove related spellmods
    if (spellInfo->SpellFamilyName == SPELLFAMILY_WARRIOR && (spellFamilyMask & 0x0000100000000000LL))
    {
        m_target->RemoveAurasDueToSpell(45471);

        if (apply)
            m_target->CastSpell(m_target, 45471, true);
    }

    if (spellInfo->SpellFamilyName == SPELLFAMILY_PALADIN && (spellFamilyMask & 0x0000100000000000LL)) // Spiritual Attunement
    {
        if (m_target->HasAura(31785, 0)) // rank 1
        {
            m_target->RemoveAurasDueToSpell(31785);
            m_target->CastSpell(m_target, 31785, true);
        }
        if (m_target->HasAura(33776, 0)) // rank 2
        {
            m_target->RemoveAurasDueToSpell(33776);
            m_target->CastSpell(m_target, 33776, true);
        }
    }
}

void Aura::TriggerSpell()
{
    Unit* caster = GetCaster();
    Unit* target = GetTriggerTarget();

    if (!caster || !target)
        return;

    // generic casting code with custom spells and target/caster customs
    uint32 trigger_spell_id = GetSpellProto()->EffectTriggerSpell[m_effIndex];

    uint64 originalCasterGUID = GetCasterGUID();

    SpellEntry const* triggeredSpellInfo = sSpellStore.LookupEntry(trigger_spell_id);
    SpellEntry const* auraSpellInfo = GetSpellProto();
    uint32 auraId = auraSpellInfo->Id;

    // specific code for cases with no trigger spell provided in field
    if (triggeredSpellInfo == NULL)
    {
        switch (auraSpellInfo->SpellFamilyName)
        {
        case SPELLFAMILY_GENERIC:
            {
                switch (auraId)
                {
                // Firestone Passive (1-5 ranks)
                case 758:
                case 17945:
                case 17947:
                case 17949:
                case 27252:
                    {
                        if (caster->GetTypeId() != TYPEID_PLAYER)
                            return;
                        Item* item = caster->ToPlayer()->GetWeaponForAttack(BASE_ATTACK);
                        if (!item)
                            return;
                        uint32 enchant_id = 0;
                        switch (GetId())
                        {
                        case   758:
                            enchant_id = 1803;
                            break;   // Rank 1
                        case 17945:
                            enchant_id = 1823;
                            break;   // Rank 2
                        case 17947:
                            enchant_id = 1824;
                            break;   // Rank 3
                        case 17949:
                            enchant_id = 1825;
                            break;   // Rank 4
                        case 27252:
                            enchant_id = 2645;
                            break;   // Rank 5
                        default:
                            return;
                        }
                        // remove old enchanting before applying new
                        caster->ToPlayer()->ApplyEnchantment(item, TEMP_ENCHANTMENT_SLOT, false);
                        item->SetEnchantment(TEMP_ENCHANTMENT_SLOT, enchant_id, m_modifier.periodictime + 1000, 0);
                        // add new enchanting
                        caster->ToPlayer()->ApplyEnchantment(item, TEMP_ENCHANTMENT_SLOT, true);
                        return;
                    }
                //                    // Periodic Mana Burn
                //                    case 812: break;
                //                    // Polymorphic Ray
                //                    case 6965: break;
                //                    // Fire Nova (1-7 ranks)
                //                    case 8350:
                //                    case 8508:
                //                    case 8509:
                //                    case 11312:
                //                    case 11313:
                //                    case 25540:
                //                    case 25544:
                //                        break;
                // Thaumaturgy Channel
                //dont work, used db support (see on spell_linked_spell)
                case 9712:
                    trigger_spell_id = 21029;
                    break;
                //                    // Egan's Blaster
                //                    case 17368: break;
                //                    // Haunted
                //                    case 18347: break;
                //                    // Ranshalla Waiting
                //                    case 18953: break;
                //                    // Inferno
                //                    case 19695: break;
                //                    // Frostwolf Muzzle DND
                //                    case 21794: break;
                //                    // Alterac Ram Collar DND
                //                    case 21866: break;
                //                    // Celebras Waiting
                //                    case 21916: break;
                // Brood Affliction: Bronze
                case 23170:
                    {
                        m_target->CastSpell(m_target, 23171, true, 0, this);
                        return;
                    }
                //                    // Mark of Frost
                //                    case 23184: break;
                // Restoration
                case 23493:
                    {
                        int32 heal = caster->GetMaxHealth() / 10;
                        caster->ModifyHealth(heal);
                        caster->SendHealSpellLog(caster, 23493, heal);

                        if (int32 mana = caster->GetMaxPower(POWER_MANA))
                        {
                            mana /= 10;
                            caster->EnergizeBySpell(caster, 23493, mana, POWER_MANA);
                        }
                        break;
                    }
                //                    // Stoneclaw Totem Passive TEST
                //                    case 23792: break;
                //                    // Axe Flurry
                //                    case 24018: break;
                //                    // Mark of Arlokk
                //                    case 24210: break;
                //                    // Restoration
                //                    case 24379: break;
                //                    // Happy Pet
                //                    case 24716: break;
                //                    // Dream Fog
                //                    case 24780: break;
                //                    // Cannon Prep
                //                    case 24832: break;
                //                    // Shadow Bolt Whirl
                case 24834:
                    {
                        uint32 spellForTick[8] = { 24820, 24821, 24822, 24823, 24835, 24836, 24837, 24838 };
                        uint32 totalTick = m_maxduration / m_modifier.periodictime;
                        if (totalTick < 8)
                        {
                            trigger_spell_id = spellForTick[totalTick];

                            // casted in left/right (but triggered spell have wide forward cone)
                            float forward = m_target->GetOrientation();
                            float angle = m_target->GetOrientation() + ( totalTick % 2 == 0 ? M_PI / 2 : - M_PI / 2);
                            m_target->SetOrientation(angle);
                            target->CastSpell(target, trigger_spell_id, true, NULL, this);
                            m_target->SetOrientation(forward);
                        }
                        return;
                    }
                //                    // Stink Trap
                //                    case 24918: break;
                //                    // Mark of Nature
                //                    case 25041: break;
                //                    // Agro Drones
                //                    case 25152: break;
                //                    // Consume
                //                    case 25371: break;
                //                    // Pain Spike
                //                    case 25572: break;
                //                    // Rotate 360
                //                    case 26009: break;
                //                    // Rotate -360
                //                    case 26136: break;
                //                    // Consume
                //                    case 26196: break;
                //                    // Berserk
                //                    case 26615: break;
                //                    // Defile
                //                    case 27177: break;
                //                    // Teleport: IF/UC
                //                    case 27601: break;
                //                    // Five Fat Finger Exploding Heart Technique
                //                    case 27673: break;
                // Nitrous Boost
                case 27746:
                    {
                        if (caster->GetPower(POWER_MANA) >= 10)
                            caster->EnergizeBySpell(caster, 27746, -10, POWER_MANA);
                        else
                        {
                            caster->RemoveAurasDueToSpell(27746);
                            return;
                        }
                    }
                    break;
                //                    // Steam Tank Passive
                //                    case 27747: break;
                //                    // Frost Blast
                //                    case 27808: break;
                //                    // Detonate Mana
                //                    case 27819: break;
                //                    // Controller Timer
                //                    case 28095: break;
                //                    // Stalagg Chain
                //                    case 28096: break;
                //                    // Stalagg Tesla Passive
                //                    case 28097: break;
                //                    // Feugen Tesla Passive
                //                    case 28109: break;
                //                    // Feugen Chain
                //                    case 28111: break;
                //                    // Mark of Didier
                //                    case 28114: break;
                //                    // Communique Timer, camp
                //                    case 28346: break;
                //                    // Icebolt
                //                    case 28522: break;
                //                    // Silithyst
                //                    case 29519: break;
                //                    // Inoculate Nestlewood Owlkin
                case 29528:
                    trigger_spell_id = 28713;
                    break;
                //                    // Overload
                case 29768:
                    {
                        int32 BasePoints = int32(GetModifier()->m_amount);
                        m_target->CastCustomSpell(m_target, 29766, &BasePoints, NULL, NULL, true, NULL, this);
                        return;
                    }
                //                    // Return Fire
                //                    case 29788: break;
                //                    // Return Fire
                //                    case 29793: break;
                //                    // Return Fire
                //                    case 29794: break;
                //                    // Guardian of Icecrown Passive
                //                    case 29897: break;
                // Feed Captured Animal
                case 29917:
                    trigger_spell_id = 29916;
                    break;
                //                    // Flame Wreath
                //                    case 29946: break;
                //                    // Flame Wreath
                //                    case 29947: break;
                //                    // Mind Exhaustion Passive
                //                    case 30025: break;
                //                    // Nether Beam - Serenity
                //                    case 30401: break;
                // Extract Gas
                case 30427:
                    {
                        // move loot to player inventory and despawn target
                        if (caster->GetTypeId() == TYPEID_PLAYER &&
                            target->GetTypeId() == TYPEID_UNIT &&
                            target->ToCreature()->GetCreatureTemplate()->type == CREATURE_TYPE_GAS_CLOUD)
                        {
                            Player* player = caster->ToPlayer();
                            Creature* creature = target->ToCreature();
                            // missing lootid has been reported on startup - just return
                            if (!creature->GetCreatureTemplate()->SkinLootId)
                                return;
                            Loot* loot = &creature->loot;
                            loot->clear();
                            loot->FillLoot(creature->GetCreatureTemplate()->SkinLootId, LootTemplates_Skinning, player, true);
                            for (uint8 i = 0; i < loot->items.size(); i++)
                            {
                                LootItem* item = loot->LootItemInSlot(i, player);
                                ItemPosCountVec dest;
                                uint8 msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, item->itemid, item->count);
                                if (msg == EQUIP_ERR_OK)
                                {
                                    Item* newitem = player->StoreNewItem(dest, item->itemid, true, item->randomPropertyId);

                                    player->SendNewItem(newitem, uint32(item->count), false, false, true);
                                }
                                else
                                    player->SendEquipError(msg, NULL, NULL);
                            }
                            creature->ForcedDespawn();
                        }
                        return;
                        break;
                    }
                // Quake
                case 30576:
                    trigger_spell_id = 30571;
                    break;
                //                    // Burning Maul
                //                    case 30598: break;
                //                    // Regeneration
                //                    case 30799:
                //                    case 30800:
                //                    case 30801:
                //                        break;
                //                    // Despawn Self - Smoke cloud
                //                    case 31269: break;
                //                    // Time Rift Periodic
                //                    case 31320: break;
                //                    // Corrupt Medivh
                //                    case 31326: break;
                // Doom
                case 31347:
                    {
                        m_target->CastSpell(m_target, 31350, true);
                        m_target->DealDamage(m_target, m_target->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                        return;
                    }
                // Spellcloth
                case 31373:
                    {
                        // Summon Elemental after create item
                        caster->SummonCreature(17870, 0, 0, 0, caster->GetOrientation(), TEMPSUMMON_DEAD_DESPAWN, 0);
                        return;
                    }
                //                    // Bloodmyst Tesla
                //                    case 31611: break;
                // Doomfire dot
                case 31944:
                    {
                        int32 BasePoints = int32(GetModifier()->m_amount);
                        m_target->CastCustomSpell( m_target, 31969, &BasePoints, NULL, NULL, true, NULL, this, m_target->GetGUID() );  /* X */

                        ApplyModifier(false);
                        GetModifier()->m_amount -= 150;
                        ApplyModifier(true);
                        return;
                    }
                //                    // Teleport Test
                //                    case 32236: break;
                //                    // Earthquake
                //                    case 32686: break;
                //                    // Possess
                //                    case 33401: break;
                //                    // Draw Shadows
                //                    case 33563: break;
                //                    // Murmur's Touch
                //                    case 33711: break;
                // Flame Quills
                case 34229:
                    {
                        // cast 24 spells 34269-34289, 34314-34316
                        for (uint32 spell_id = 34269; spell_id != 34290; ++spell_id)
                            caster->CastSpell(m_target, spell_id, true);
                        for (uint32 spell_id = 34314; spell_id != 34317; ++spell_id)
                            caster->CastSpell(m_target, spell_id, true);
                        return;
                    }
                //                    // Gravity Lapse
                //                    case 34480: break;
                //                    // Tornado
                //                    case 34683: break;
                //                    // Frostbite Rotate
                //                    case 34748: break;
                //                    // Arcane Flurry
                //                    case 34821: break;
                //                    // Interrupt Shutdown
                //                    case 35016: break;
                //                    // Interrupt Shutdown
                //                    case 35176: break;
                //                    // Inferno
                //                    case 35268: break;
                //                    // Salaadin's Tesla
                //                    case 35515: break;
                //                    // Ethereal Channel (Red)
                //                    case 35518: break;
                //                    // Nether Vapor
                //                    case 35879: break;
                //                    // Dark Portal Storm
                //                    case 36018: break;
                //                    // Burning Maul
                //                    case 36056: break;
                //                    // Living Grove Defender Lifespan
                //                    case 36061: break;
                //                    // Professor Dabiri Talks
                //                    case 36064: break;
                //                    // Kael Gaining Power
                //                    case 36091: break;
                //                    // They Must Burn Bomb Aura
                //                    case 36344: break;
                //                    // They Must Burn Bomb Aura (self)
                //                    case 36350: break;
                //                    // Stolen Ravenous Ravager Egg
                //                    case 36401: break;
                //                    // Activated Cannon
                //                    case 36410: break;
                //                    // Stolen Ravenous Ravager Egg
                //                    case 36418: break;
                //                    // Enchanted Weapons
                //                    case 36510: break;
                //                    // Cursed Scarab Periodic
                //                    case 36556: break;
                //                    // Cursed Scarab Despawn Periodic
                //                    case 36561: break;
                //                    // Vision Guide
                //                    case 36573: break;
                //                    // Cannon Charging (platform)
                //                    case 36785: break;
                //                    // Cannon Charging (self)
                //                    case 36860: break;
                // Remote Toy
                case 37027:
                    trigger_spell_id = 37029;
                    break;
                //                    // Mark of Death
                //                    case 37125: break;
                //                    // Arcane Flurry
                //                    case 37268: break;
                //                    // Spout
                //                    case 37429: break;
                //                    // Spout
                //                    case 37430: break;
                //                    // Karazhan - Chess NPC AI, Snapshot timer
                //                    case 37440: break;
                //                    // Karazhan - Chess NPC AI, action timer
                //                    case 37504: break;
                //                    // Karazhan - Chess: Is Square OCCUPIED aura (DND)
                //                    case 39400: break;
                //                    // Banish
                //                    case 37546: break;
                //                    // Shriveling Gaze
                //                    case 37589: break;
                //                    // Fake Aggro Radius (2 yd)
                //                    case 37815: break;
                //                    // Corrupt Medivh
                //                    case 37853: break;
                // Eye of Grillok
                case 38495:
                    {
                        if (m_target->FindNearestCreature(22177, 10.0f, true))
                        {
                            if (caster->GetTypeId() == TYPEID_PLAYER)
                            {
                                Creature* bunny = caster->FindNearestCreature(22177, 10.0f, true);
                                caster->CastSpell(bunny, 38530, true);
                                caster->RemoveAura(38495, 0);
                            }
                        }
                        return;
                    }
                // Absorb Eye of Grillok (Zezzak's Shard)
                case 38554:
                    {
                        if (m_target->GetTypeId() != TYPEID_UNIT)
                            return;

                        caster->CastSpell(caster, 38495, true);

                        Creature* creatureTarget = m_target->ToCreature();

                        creatureTarget->ForcedDespawn();
                        return;
                    }
                //                    // Magic Sucker Device timer
                //                    case 38672: break;
                //                    // Tomb Guarding Charging
                //                    case 38751: break;
                //                    // Murmur's Touch
                //                    case 38794: break;
                //                    // Activate Nether-wraith Beacon (31742 Nether-wraith Beacon item)
                //                    case 39105: break;
                //                    // Drain World Tree Visual
                //                    case 39140: break;
                //                    // Quest - Dustin's Undead Dragon Visual aura
                //                    case 39259: break;
                //                    // Hellfire - The Exorcism, Jules releases darkness, aura
                //                    case 39306: break;
                //                    // Inferno
                case 39346:
                    {
                        int32 BasePoints = int32(GetModifier()->m_amount);
                        m_target->CastCustomSpell(m_target, 35283, &BasePoints, NULL, NULL, true, NULL, this);
                        return;
                    }
                //                    // Enchanted Weapons
                //                    case 39489: break;
                //                    // Shadow Bolt Whirl
                //                    case 39630: break;
                //                    // Shadow Bolt Whirl
                //                    case 39634: break;
                //                    // Shadow Inferno
                case 39645:
                    {
                        int32 BasePoints = int32(GetModifier()->m_amount);
                        m_target->CastCustomSpell(m_target, 39646, &BasePoints, NULL, NULL, true, NULL, this);
                        return;
                    }
                // Tear of Azzinoth Summon Channel - it's not really supposed to do anything,and this only prevents the console spam
                case 39857:
                    trigger_spell_id = 39856;
                    break;
                //                    // Soulgrinder Ritual Visual (Smashed)
                //                    case 39974: break;
                //                    // Simon Game Pre-game timer
                //                    case 40041: break;
                //                    // Knockdown Fel Cannon: The Aggro Check Aura
                //                    case 40113: break;
                //                    // Spirit Lance
                //                    case 40157: break;
                //                    // Demon Transform 2
                //                    case 40398: break;
                //                    // Demon Transform 1
                //                    case 40511: break;
                //                    // Ancient Flames
                //                    case 40657: break;
                //                    // Ethereal Ring Cannon: Cannon Aura
                //                    case 40734: break;
                //                    // Cage Trap
                //                    case 40760: break;
                //                    // Random Periodic
                //                    case 40867: break;
                // Prismatic Shield
                case 40879:
                    {
                        switch (rand() % 6)
                        {
                        case 0:
                            trigger_spell_id = 40880;
                            break;
                        case 1:
                            trigger_spell_id = 40882;
                            break;
                        case 2:
                            trigger_spell_id = 40883;
                            break;
                        case 3:
                            trigger_spell_id = 40891;
                            break;
                        case 4:
                            trigger_spell_id = 40896;
                            break;
                        case 5:
                            trigger_spell_id = 40897;
                            break;
                        }
                    }
                    break;
                // Aura of Desire
                case 41350:
                    {
                        Unit::AuraList const& mMod = m_target->GetAurasByType(SPELL_AURA_MOD_INCREASE_ENERGY_PERCENT);
                        for (Unit::AuraList::const_iterator i = mMod.begin(); i != mMod.end(); ++i)
                        {
                            if ((*i)->GetId() == 41350)
                            {
                                (*i)->ApplyModifier(false);
                                (*i)->GetModifier()->m_amount -= 5;
                                (*i)->ApplyModifier(true);
                                break;
                            }
                        }
                    }
                    break;
                //                    // Dementia
                case 41404:
                    {
                        if (rand() % 2)
                            trigger_spell_id = 41406;
                        else
                            trigger_spell_id = 41409;
                    }
                    break;
                //                    // Chaos Form
                //                    case 41629: break;
                //                    // Alert Drums
                //                    case 42177: break;
                //                    // Spout
                //                    case 42581: break;
                //                    // Spout
                //                    case 42582: break;
                //                    // Return to the Spirit Realm
                //                    case 44035: break;
                //                    // Curse of Boundless Agony
                //                    case 45050: break;
                //                    // Earthquake
                //                    case 46240: break;
                // Personalized Weather
                case 46736:
                    trigger_spell_id = 46737;
                    break;
                //                    // Stay Submerged
                //                    case 46981: break;
                //                    // Dragonblight Ram
                //                    case 47015: break;
                //                    // Party G.R.E.N.A.D.E.
                //                    case 51510: break;
                default:
                    break;
                }
                break;
            }
        case SPELLFAMILY_MAGE:
            {
                switch (auraId)
                {
                // Invisibility
                case 66:
                    {
                        if (!m_duration)
                            m_target->CastSpell(m_target, 32612, true, NULL, this);
                        return;
                    }
                default:
                    break;
                }
                break;
            }
        //            case SPELLFAMILY_WARRIOR:
        //            {
        //                switch(auraId)
        //                {
        //                    // Wild Magic
        //                    case 23410: break;
        //                    // Corrupted Totems
        //                    case 23425: break;
        //                    default:
        //                        break;
        //                }
        //                break;
        //            }
        //            case SPELLFAMILY_PRIEST:
        //            {
        //                switch(auraId)
        //                {
        //                    // Blue Beam
        //                    case 32930: break;
        //                    // Fury of the Dreghood Elders
        //                    case 35460: break;
        //                    default:
        //                        break;
        //                }
        //               break;
        //           }
        case SPELLFAMILY_DRUID:
            {
                switch (auraId)
                {
                // Cat Form
                // trigger_spell_id not set and unknown effect triggered in this case, ignoring for while
                case 768:
                    return;
                // Frenzied Regeneration
                case 22842:
                case 22895:
                case 22896:
                case 26999:
                    {
                        int32 LifePerRage = GetModifier()->m_amount;

                        int32 lRage = m_target->GetPower(POWER_RAGE);
                        if (lRage > 100)                                     // rage stored as rage*10
                            lRage = 100;
                        m_target->ModifyPower(POWER_RAGE, -lRage);
                        int32 FRTriggerBasePoints = int32(lRage * LifePerRage / 10);
                        m_target->CastCustomSpell(m_target, 22845, &FRTriggerBasePoints, NULL, NULL, true, NULL, this);
                        return;
                    }
                default:
                    break;
                }
                break;
            }
        case SPELLFAMILY_HUNTER:
            {
                switch (auraId)
                {
                //Frost Trap Aura
                case 13810:
                    if (caster->GetTypeId() == TYPEID_PLAYER)
                    {
                        float f_chance = 0;
                        Unit::AuraList const& auraTriggerSpell = caster->GetAurasByType(SPELL_AURA_PROC_TRIGGER_SPELL);
                        for (Unit::AuraList::const_iterator itr = auraTriggerSpell.begin(); itr != auraTriggerSpell.end(); ++itr)
                        {
                            switch ((*itr)->GetSpellProto()->Id)
                            {
                            case 19184:
                            case 19387:
                            case 19388:
                                f_chance = (*itr)->GetSpellProto()->procChance;
                                break;
                            }
                        }

                        if (roll_chance_f(f_chance))
                        {
                            target = m_target;
                            trigger_spell_id = 19185;
                        }
                        else
                            return;
                    }
                    break;
                default:
                    break;
                }
                break;
            }
        case SPELLFAMILY_SHAMAN:
            {
                switch (auraId)
                {
                // Lightning Shield (The Earthshatterer set trigger after cast Lighting Shield)
                case 28820:
                    {
                        // Need remove self if Lightning Shield not active
                        Unit::AuraMap const& auras = target->GetAuras();
                        for (Unit::AuraMap::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
                        {
                            SpellEntry const* spell = itr->second->GetSpellProto();
                            if (spell->SpellFamilyName == SPELLFAMILY_SHAMAN &&
                                spell->SpellFamilyFlags & 0x0000000000000400L)
                                return;
                        }
                        target->RemoveAurasDueToSpell(28820);
                        return;
                    }
                // Totemic Mastery (Skyshatter Regalia (Shaman Tier 6) - bonus)
                case 38443:
                    {
                        bool all = true;
                        for (int i = SUMMON_SLOT_TOTEM; i < MAX_TOTEM_SLOT; ++i)
                        {
                            if (!caster->m_SummonSlot[i])
                            {
                                all = false;
                                break;
                            }
                        }

                        if (all)
                            caster->CastSpell(caster, 38437, true);
                        else
                            caster->RemoveAurasDueToSpell(38437);
                        return;
                    }
                default:
                    break;
                }
                break;
            }
        default:
            break;
        }
        // Reget trigger spell proto
        triggeredSpellInfo = sSpellStore.LookupEntry(trigger_spell_id);
        if (triggeredSpellInfo == NULL)
        {
            Creature* c = target->ToCreature();
            if (!c || !caster || !sScriptMgr.EffectDummyCreature(caster, GetId(), SpellEffIndex(GetEffIndex()), target->ToCreature()) ||
                !c->AI()->sOnDummyEffect(caster, GetId(), SpellEffIndex(GetEffIndex())))

            sLog.outError("Aura::TriggerSpell: Spell %u has 0 in EffectTriggered[%d], unhandled custom case?", GetId(), GetEffIndex());
            return;
        }
    }
    else
    {
        // Spell exist but require custom code
        switch (auraId)
        {
        // Curse of Idiocy
        case 1010:
            {
                // @todo spell casted by result in correct way mostly
                // BUT:
                // 1) target show casting at each triggered cast: target don't must show casting animation for any triggered spell
                //      but must show affect apply like item casting
                // 2) maybe aura must be replace by new with accumulative stat mods instead stacking

                // prevent cast by triggered auras
                if (m_caster_guid == m_target->GetGUID())
                    return;

                // stop triggering after each affected stats lost > 90
                int32 intellectLoss = 0;
                int32 spiritLoss = 0;

                Unit::AuraList const& mModStat = m_target->GetAurasByType(SPELL_AURA_MOD_STAT);
                for (Unit::AuraList::const_iterator i = mModStat.begin(); i != mModStat.end(); ++i)
                {
                    if ((*i)->GetId() == 1010)
                    {
                        switch ((*i)->GetModifier()->m_miscvalue)
                        {
                        case STAT_INTELLECT:
                            intellectLoss += (*i)->GetModifierValue();
                            break;
                        case STAT_SPIRIT:
                            spiritLoss   += (*i)->GetModifierValue();
                            break;
                        default:
                            break;
                        }
                    }
                }

                if (intellectLoss <= -90 && spiritLoss <= -90)
                    return;

                caster = target;
                originalCasterGUID = 0;
                break;
            }
        // Mana Tide
        case 16191:
            {
                caster->CastCustomSpell(target, trigger_spell_id, &m_modifier.m_amount, NULL, NULL, true, NULL, this, originalCasterGUID);
                return;
            }
        // Negative Energy Periodic
        case 46284:
            {
                caster->CastCustomSpell(trigger_spell_id, SPELLVALUE_MAX_TARGETS, m_tickNumber / 10 + 1, NULL, true, NULL, this, originalCasterGUID);
                return;
            }
        // Rod of Purification - for quest 10839 (Veil Skith: Darkstone of Terokk)
        case 38736:
            {
                if (Unit* caster = GetCaster())
                    caster->CastSpell(target, trigger_spell_id, true, NULL, this);
                return;
            }
        }
    }
    if (!GetSpellMaxRange(sSpellRangeStore.LookupEntry(triggeredSpellInfo->rangeIndex)))
        target = m_target;    //for druid dispel poison
    m_target->CastSpell(target, triggeredSpellInfo, true, 0, this, originalCasterGUID);
}

Unit* Aura::GetTriggerTarget() const
{
    Unit* target = ObjectAccessor::GetUnit(*m_target,
                                           /*m_target->GetTypeId() == TYPEID_PLAYER ?
                                           m_target->ToPlayer()->GetSelection() :*/
                                           m_target->GetUInt64Value(UNIT_FIELD_TARGET));
    return target ? target : m_target;
}

void Aura::TriggerSpellWithValue()
{
    const uint64& casterGUID = GetCasterGUID();
    Unit* target = GetTriggerTarget();

    if (!casterGUID || !target)
        return;

    // generic casting code with custom spells and target/caster customs
    uint32 trigger_spell_id = GetSpellProto()->EffectTriggerSpell[m_effIndex];
    int32  basepoints0 = GetModifier()->m_amount;

    target->CastCustomSpell(target, trigger_spell_id, &basepoints0, NULL, NULL, true, NULL, this, casterGUID);
}

/*********************************************************/
/***                 AURA EFFECTS                     ***/
/*********************************************************/

void Aura::HandleAuraDummy(bool apply, bool Real)
{
    // spells required only Real aura add/remove
    if (!Real)
        return;

    Unit* caster = GetCaster();

    // AT APPLY
    if (apply)
    {
        switch (GetId())
        {
        case 31261:
        case 29266:
            m_target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNK_29);
            m_target->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH);
            m_target->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
            m_target->AddUnitState(UNIT_STATE_DIED);
            return;
        case 1515:                                      // Tame beast
            // FIX_ME: this is 2.0.12 threat effect replaced in 2.1.x by dummy aura, must be checked for correctness
            if (caster && m_target->CanHaveThreatList())
                m_target->AddThreat(caster, 10.0f);
            return;
        case 7057:                                      // Haunting Spirits
            // expected to tick with 30 sec period (tick part see in Aura::PeriodicTick)
            m_isPeriodic = true;
            m_modifier.periodictime = 30 * IN_MILLISECONDS;
            m_periodicTimer = 30 * IN_MILLISECONDS;
            return;
        case 13139:                                     // net-o-matic
            // root to self part of (root_target->charge->root_self sequence
            if (caster)
                caster->CastSpell(caster, 13138, true, NULL, this);
            return;
        case 37096:                                     // Blood Elf Disguise
            if (caster)
            {
                if (caster->getGender() == GENDER_FEMALE)
                    caster->CastSpell(m_target, 37095, true, NULL, this);
                else
                    caster->CastSpell(m_target, 37093, true, NULL, this);
            }
            return;
        case 30019: //control piece
            if (caster && caster->GetTypeId() == TYPEID_PLAYER)
                caster->ToPlayer()->TeleportTo(532, -11105.08f, -1845.65f, 229.65f, 5.42f);
            if (caster && caster->GetCharm())
                caster->GetCharm()->GetCharmInfo()->SetIsAtStay(true);
            return;
        case 39850:                                     // Rocket Blast
            if (roll_chance_i(20))                       // backfire stun
                m_target->CastSpell(m_target, 51581, true, NULL, this);
            return;
        case 43873:                                     // Headless Horseman Laugh
            m_target->PlayDistanceSound(11965);
            return;
        case 46354:                                     // Blood Elf Illusion
            {
                if (caster)
                {
                    if (caster->getGender() == GENDER_FEMALE)
                        caster->CastSpell(m_target, 46356, true, NULL, this);
                    else
                        caster->CastSpell(m_target, 46355, true, NULL, this);
                        break;
                }
                return;
            }
        case 32260: //Chess: Deactivate Own Field
            {
                //Set No_Movement Flag, to disable Field
                m_target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_REMOVE_CLIENT_CONTROL);
                //Add Tempsummon
                GetCaster()->SummonCreature(22519, m_target->GetPositionX(), m_target->GetPositionY(), m_target->GetPositionZ(), m_target->GetOrientation());
                return;
            }
        case 32261: //Chess: Create Move Marker
            {
                if (m_target->GetEntry() == 17305 || m_target->GetEntry() == 17208) //Only white or black squares are valid targets
                    GetCaster()->SummonCreature(22519, m_target->GetPositionX(), m_target->GetPositionY(), m_target->GetPositionZ(), m_target->GetOrientation());
                return;
            }
        case 46699:                                     // Requires No Ammo
            if (m_target->GetTypeId() == TYPEID_PLAYER)
                m_target->ToPlayer()->RemoveAmmo();      // not use ammo and not allow use
            return;
        case 39246:                                     // Q: The Big Bone Worm
            {
                if (!m_target)
                    return;

                if (Unit* caster = GetCaster())
                {
                    if (urand(0, 10) > 2)
                    {
                        int32 count = urand(0, 1) ? 2 : 4;
                        for (int i = 0; i < count; ++i)
                            caster->SummonCreature(urand(0, 1) ? 22482 : 22483, m_target->GetPositionX(), m_target->GetPositionY(), m_target->GetPositionZ(), m_target->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000);
                    }
                    else
                        caster->SummonCreature(22038, m_target->GetPositionX(), m_target->GetPositionY(), m_target->GetPositionZ(), m_target->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000);
                }
                return;
            }
        case 39238:                                     // Fumping
            {
                if (!m_target)
                    return;

                if (Unit* caster = GetCaster())
                {
                    if (urand(0, 1) == 0)
                        caster->SummonCreature(22482, m_target->GetPositionX(), m_target->GetPositionY(), m_target->GetPositionZ(), m_target->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000);
                    else
                    {
                        caster->SummonCreature(22483, m_target->GetPositionX(), m_target->GetPositionY(), m_target->GetPositionZ(), m_target->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000);
                        caster->SummonCreature(22483, m_target->GetPositionX(), m_target->GetPositionY(), m_target->GetPositionZ(), m_target->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000);
                        caster->SummonCreature(22483, m_target->GetPositionX(), m_target->GetPositionY(), m_target->GetPositionZ(), m_target->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000);
                    }
                }
                return;
            }
        }

        // Earth Shield
        if (caster && GetSpellProto()->SpellFamilyName == SPELLFAMILY_SHAMAN && (GetSpellProto()->SpellFamilyFlags & 0x40000000000LL))
        {
            // prevent double apply bonuses
            if (m_target->GetTypeId() != TYPEID_PLAYER || !m_target->ToPlayer()->GetSession()->PlayerLoading())
                m_modifier.m_amount = caster->SpellHealingBonus(GetSpellProto(), m_modifier.m_amount, SPELL_DIRECT_DAMAGE, m_target);
            return;
        }
    }
    // AT REMOVE
    else
    {
        if ((IsQuestTameSpell(GetId())) && caster && caster->IsAlive() && m_target->IsAlive())
        {
            uint32 finalSpellId = 0;
            switch (GetId())
            {
                case 19548: finalSpellId = 19597; break;
                case 19674: finalSpellId = 19677; break;
                case 19687: finalSpellId = 19676; break;
                case 19688: finalSpellId = 19678; break;
                case 19689: finalSpellId = 19679; break;
                case 19692: finalSpellId = 19680; break;
                case 19693: finalSpellId = 19684; break;
                case 19694: finalSpellId = 19681; break;
                case 19696: finalSpellId = 19682; break;
                case 19697: finalSpellId = 19683; break;
                case 19699: finalSpellId = 19685; break;
                case 19700: finalSpellId = 19686; break;
                case 30646: finalSpellId = 30647; break;
                case 30653: finalSpellId = 30648; break;
                case 30654: finalSpellId = 30652; break;
                case 30099: finalSpellId = 30100; break;
                case 30102: finalSpellId = 30103; break;
                case 30105: finalSpellId = 30104; break;
            }

            if (finalSpellId)
                caster->CastSpell(m_target, finalSpellId, true, NULL, this);

            return;
        }

        switch (GetId())
        {
        case 2584:                                     // Waiting to Resurrect
            {
                // Waiting to resurrect spell cancel, we must remove player from resurrect queue
                if (m_target->GetTypeId() == TYPEID_PLAYER)
                    if (Battleground* bg = m_target->ToPlayer()->GetBattleground())
                        bg->RemovePlayerFromResurrectQueue(m_target->GetGUID());
                return;
            }
        case 28169:                                     // Mutating Injection
            {
                // Mutagen Explosion
                m_target->CastSpell(m_target, 28206, true, NULL, this);
                // Poison Cloud
                m_target->CastSpell(m_target, 28240, true, NULL, this);
                return;
            }
        case 36730:                                     // Flame Strike
            {
                m_target->CastSpell(m_target, 36731, true, NULL, this);
                return;
            }
        case 44191:                                     // Flame Strike
            {
                if (m_target->GetMap()->IsDungeon())
                {
                    uint32 spellId = m_target->GetMap()->IsHeroic() ? 46163 : 44190;

                    m_target->CastSpell(m_target, spellId, true, NULL, this);
                }
                return;
            }
        case 45934:                                     // Dark Fiend
            {
                // Kill target if dispelled
                if (m_removeMode == AURA_REMOVE_BY_DISPEL)
                    m_target->DealDamage(m_target, m_target->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                return;
            }
        case 43681: // Inactive
            {
                if (!m_target || m_target->GetTypeId() != TYPEID_PLAYER || m_removeMode != AURA_REMOVE_BY_EXPIRE)
                    return;

                if (m_target->GetMap()->IsBattleground())
                    m_target->ToPlayer()->LeaveBattleground();
                break;
            }
        case 46308:                                     // Burning Winds - casted only at creatures at spawn
                m_target->CastSpell(m_target, 47287, true, NULL, this);
                break;
        case 34477:                                     // Misdirection
            {
                m_target->SetReducedThreatPercent(0, 0);
                return;
            }
        case 30019: //control piece
            {
                m_target->CastSpell(m_target, 30529, true, NULL, this); //conrol piece debuff
                if (m_target->GetTypeId() == TYPEID_PLAYER)
                    m_target->ToPlayer()->StopCastingCharm();
                return;
            }
        case 31261:
        case 29266:
            {
                m_target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNK_29);
                m_target->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH);
                m_target->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
                m_target->ClearUnitState(UNIT_STATE_DIED);
                return;
            }
        }
    }

    // AT APPLY & REMOVE

    switch (m_spellProto->SpellFamilyName)
    {
    case SPELLFAMILY_GENERIC:
        {
            // Unstable Power
            if (GetId() == 24658)
            {
                uint32 spellId = 24659;
                if (apply)
                {
                    const SpellEntry* spell = sSpellStore.LookupEntry(spellId);
                    if (!spell)
                        return;
                    for (uint32 i = 0; i < spell->StackAmount; ++i)
                        caster->CastSpell(m_target, spell->Id, true, NULL, NULL, GetCasterGUID());
                    return;
                }
                m_target->RemoveAurasDueToSpell(spellId);
                return;
            }
            // Restless Strength
            if (GetId() == 24661)
            {
                uint32 spellId = 24662;
                if (apply)
                {
                    const SpellEntry* spell = sSpellStore.LookupEntry(spellId);
                    if (!spell)
                        return;
                    for (uint32 i = 0; i < spell->StackAmount; ++i)
                        caster->CastSpell(m_target, spell->Id, true, NULL, NULL, GetCasterGUID());
                    return;
                }
                m_target->RemoveAurasDueToSpell(spellId);
                return;
            }
            // Victorious
            if (GetId() == 32216 && m_target->getClass() == CLASS_WARRIOR)
            {
                m_target->ModifyAuraState(AURA_STATE_WARRIOR_VICTORY_RUSH, apply);
                return;
            }
            //Summon Fire Elemental
            if (GetId() == 40133 && caster)
            {
                Unit* owner = caster->GetOwner();
                if (owner && owner->GetTypeId() == TYPEID_PLAYER)
                {
                    if (apply)
                        owner->CastSpell(owner, 8985, true);
                    else
                        owner->ToPlayer()->RemovePet(NULL, PET_SAVE_NOT_IN_SLOT, true);
                }
                return;
            }

            //Summon Earth Elemental
            if (GetId() == 40132 && caster)
            {
                Unit* owner = caster->GetOwner();
                if (owner && owner->GetTypeId() == TYPEID_PLAYER)
                {
                    if (apply)
                        owner->CastSpell(owner, 19704, true);
                    else
                        owner->ToPlayer()->RemovePet(NULL, PET_SAVE_NOT_IN_SLOT, true);
                }
                return;
            }

            if (GetId() == 126 && caster) // Eye of Kilrogg (Temporarily UnSumoon Main Pet)
            {
                Player* player = caster->ToPlayer();
                if (!player)
                    return;

                if (apply)
                    player->UnsummonPetTemporaryIfAny();
                else
                    player->ResummonTemporaryUnsummonedPetIfAny();
                return;
            }

            // As the crow flies
            if (GetId() == 31606)
            {
                if (apply)
                    SetAuraDuration(110000);
                else if (Player* player = m_target->ToPlayer())
                    player->AreaExploredOrEventHappens(9718);
            }

            // Zul'Aman Object Visual (Strange Gong)
            if (GetId() == 45222)
                if (apply)
                    m_target->CastSpell(m_target, 45225, false, NULL, this);
            break;
        }
    case SPELLFAMILY_MAGE:
        {
            // Hypothermia
            if (GetId() == 41425)
            {
                m_target->ModifyAuraState(AURA_STATE_HYPOTHERMIA, apply);
                return;
            }
            break;
        }
    case SPELLFAMILY_DRUID:
        {
            // Lifebloom
            if (GetSpellProto()->SpellFamilyFlags & 0x1000000000LL)
            {
                if (apply)
                {
                    if (caster)
                        // prevent double apply bonuses
                        if (m_target->GetTypeId() != TYPEID_PLAYER || !m_target->ToPlayer()->GetSession()->PlayerLoading())
                            m_modifier.m_amount = caster->SpellHealingBonus(GetSpellProto(), m_modifier.m_amount, SPELL_DIRECT_DAMAGE, m_target);
                }
                // Do final heal for real !apply
                else if (Real)
                {
                    if (GetAuraDuration() <= 0 || m_removeMode == AURA_REMOVE_BY_DISPEL)
                    {
                        // final heal
                        if (m_target->IsInWorld())
                            m_target->CastCustomSpell(m_target, 33778, &m_modifier.m_amount, NULL, NULL, true, NULL, this, GetCasterGUID());
                    }
                }
                return;
            }

            // Predatory Strikes
            if (m_target->GetTypeId() == TYPEID_PLAYER && GetSpellProto()->SpellIconID == 1563)
            {
                m_target->ToPlayer()->UpdateAttackPowerAndDamage();
                return;
            }
            // Idol of the Emerald Queen
            if (GetId() == 34246 && m_target->GetTypeId() == TYPEID_PLAYER)
            {
                if (apply)
                {
                    SpellModifier* mod = new SpellModifier;
                    mod->op = SPELLMOD_DOT;
                    mod->value = m_modifier.m_amount / 7;
                    mod->type = SPELLMOD_FLAT;
                    mod->spellId = GetId();
                    mod->effectId = m_effIndex;
                    mod->lastAffected = NULL;
                    mod->mask = 0x001000000000LL;
                    mod->charges = 0;

                    m_spellmod = mod;
                }

                m_target->ToPlayer()->AddSpellMod(m_spellmod, apply);
                return;
            }
            break;
        }
    case SPELLFAMILY_HUNTER:
        {
            // Improved Aspect of the Viper
            if (GetId() == 38390 && m_target->GetTypeId() == TYPEID_PLAYER)
            {
                if (apply)
                {
                    // + effect value for Aspect of the Viper
                    SpellModifier* mod = new SpellModifier;
                    mod->op = SPELLMOD_EFFECT1;
                    mod->value = m_modifier.m_amount;
                    mod->type = SPELLMOD_FLAT;
                    mod->spellId = GetId();
                    mod->effectId = m_effIndex;
                    mod->lastAffected = NULL;
                    mod->mask = 0x4000000000000LL;
                    mod->charges = 0;

                    m_spellmod = mod;
                }

                m_target->ToPlayer()->AddSpellMod(m_spellmod, apply);
                return;
            }
            break;
        }
    case SPELLFAMILY_SHAMAN:
        {
            // Improved Weapon Totems
            if (GetSpellProto()->SpellIconID == 57 && m_target->GetTypeId() == TYPEID_PLAYER)
            {
                if (apply)
                {
                    SpellModifier* mod = new SpellModifier;
                    mod->op = SPELLMOD_EFFECT1;
                    mod->value = m_modifier.m_amount;
                    mod->type = SPELLMOD_PCT;
                    mod->spellId = GetId();
                    mod->effectId = m_effIndex;
                    mod->lastAffected = NULL;
                    switch (m_effIndex)
                    {
                    case 0:
                        mod->mask = 0x00200000000LL;    // Windfury Totem
                        break;
                    case 1:
                        mod->mask = 0x00400000000LL;    // Flametongue Totem
                        break;
                    }
                    mod->charges = 0;

                    m_spellmod = mod;
                }

                m_target->ToPlayer()->AddSpellMod(m_spellmod, apply);
                return;
            }

            // Sentry Totem
            if (GetId() == 6495 && caster->GetTypeId() == TYPEID_PLAYER)
            {
                if (apply)
                {
                    uint64 guid = caster->m_SummonSlot[3];
                    if (guid)
                    {
                        Creature* totem = caster->GetMap()->GetCreature(guid);
                        if (totem && totem->IsTotem())
                            caster->ToPlayer()->CastSpell(totem, 6277, true);
                    }
                }
                else
                    caster->ToPlayer()->StopCastingBindSight();
                return;
            }
            break;
        }
    }

    // pet auras
    if (PetAura const* petSpell = sSpellMgr.GetPetAura(GetId()))
    {
        if (apply)
            m_target->AddPetAura(petSpell);
        else
            m_target->RemovePetAura(petSpell);
        return;
    }
}

void Aura::HandleAuraMounted(bool apply, bool Real)
{
    // only at real add/remove aura
    if (!Real)
        return;

    if (apply)
    {
        CreatureInfo const* ci = sObjectMgr.GetCreatureTemplate(m_modifier.m_miscvalue);
        if (!ci)
        {
            sLog.outErrorDb("AuraMounted: creature_template='%u' not found in database (only need it modelid)", m_modifier.m_miscvalue);
            return;
        }

        uint32 team = 0;
        if (m_target->GetTypeId() == TYPEID_PLAYER)
            team = m_target->ToPlayer()->GetTeam();

        uint32 display_id = sObjectMgr.ChooseDisplayId(team, ci);
        CreatureModelInfo const* minfo = sObjectMgr.GetCreatureModelRandomGender(display_id);
        if (minfo)
            display_id = minfo->modelid;

        m_target->Mount(display_id, m_spellProto->Id);
    }
    else
    {
        m_target->Dismount();
        m_target->RemoveAurasByType(SPELL_AURA_MOUNTED);
    }
}

void Aura::HandleAuraWaterWalk(bool apply, bool Real)
{
    // only at real add/remove aura
    if (!Real)
        return;

    if (!apply)
    {
        // do not remove unit flag if there are more than this auraEffect of that kind on unit
        if (m_target->HasAuraType(GetAuraType()))
            return;
    }

    m_target->SetWaterWalking(apply);
}

void Aura::HandleAuraFeatherFall(bool apply, bool Real)
{
    // only at real add/remove aura
    if (!Real)
        return;

    if (!apply)
    {
        // do not remove unit flag if there are more than this auraEffect of that kind on unit
        if (m_target->HasAuraType(GetAuraType()))
            return;
    }

    m_target->SetFeatherFall(apply);

    // start fall from current height
    if (!apply && m_target->GetTypeId() == TYPEID_PLAYER)
        ((Player*)m_target)->SetFallInformation(0, m_target->GetPositionZ());
}

void Aura::HandleAuraHover(bool apply, bool Real)
{
    // only at real add/remove aura
    if (!Real)
        return;

    if (!apply)
    {
        // do not remove unit flag if there are more than this auraEffect of that kind on unit
        if (m_target->HasAuraType(GetAuraType()))
            return;
    }

    m_target->SetHover(apply);
}

void Aura::HandleWaterBreathing(bool /*apply*/, bool /*Real*/)
{
    // update timers in client
    if (m_target->GetTypeId() == TYPEID_PLAYER)
        m_target->ToPlayer()->UpdateMirrorTimers();
}

void Aura::HandleAuraModShapeshift(bool apply, bool Real)
{
    if (!Real)
        return;

    uint32 modelid = 0;
    Powers PowerType = POWER_MANA;
    ShapeshiftForm form = ShapeshiftForm(m_modifier.m_miscvalue);

    switch (form)
    {
        case FORM_CAT:
            if (Player::TeamForRace(m_target->getRace()) == ALLIANCE)
                modelid = 892;
            else
                modelid = 8571;
            PowerType = POWER_ENERGY;
            break;
        case FORM_TRAVEL:
            modelid = 918;
            break;
        case FORM_AQUA:
            if (Player::TeamForRace(m_target->getRace()) == ALLIANCE)
                modelid = 2428;
            else
                modelid = 2428;
            break;
        case FORM_BEAR:
            if (Player::TeamForRace(m_target->getRace()) == ALLIANCE)
                modelid = 2281;
            else
                modelid = 2289;
            PowerType = POWER_RAGE;
            break;
        case FORM_DIREBEAR:
            if (Player::TeamForRace(m_target->getRace()) == ALLIANCE)
                modelid = 2281;
            else
                modelid = 2289;
            PowerType = POWER_RAGE;
            break;
        case FORM_FLIGHT:
            if (Player::TeamForRace(m_target->getRace()) == ALLIANCE)
                modelid = 20857;
            else
                modelid = 20872;
            break;
        case FORM_MOONKIN:
            if (Player::TeamForRace(m_target->getRace()) == ALLIANCE)
                modelid = 15374;
            else
                modelid = 15375;
            break;
        case FORM_FLIGHT_EPIC:
            if (Player::TeamForRace(m_target->getRace()) == ALLIANCE)
                modelid = 21243;
            else
                modelid = 21244;
            break;
        case FORM_BATTLESTANCE:
        case FORM_BERSERKERSTANCE:
        case FORM_DEFENSIVESTANCE:
            PowerType = POWER_RAGE;
            break;
        default:
            SpellShapeshiftEntry const* formEntry = sSpellShapeshiftStore.LookupEntry(form);
            if (formEntry && formEntry->modelID_A)
                modelid = formEntry->modelID_A;
    }

    // remove polymorph before changing display id to keep new display id
    switch (form)
    {
        case FORM_CAT:
        case FORM_TREE:
        case FORM_TRAVEL:
        case FORM_AQUA:
        case FORM_BEAR:
        case FORM_DIREBEAR:
        case FORM_FLIGHT_EPIC:
        case FORM_FLIGHT:
        case FORM_MOONKIN:
        {
            // remove movement affects
            m_target->RemoveMovementImpairingAuras();

            // and polymorphic affects
            if (m_target->IsPolymorphed())
                m_target->RemoveAurasDueToSpell(m_target->getTransForm());
        }
        case FORM_GHOSTWOLF:
        {
            // remove water walk aura. @todo there is probably better way to do this
            m_target->RemoveSpellsCausingAura(SPELL_AURA_WATER_WALK);
            break;
        }
        default:
            break;
    }

    if (apply)
    {
        // remove other shapeshift before applying a new one
        if (m_target->m_ShapeShiftFormSpellId)
            m_target->RemoveAurasDueToSpell(m_target->m_ShapeShiftFormSpellId, this);

        m_target->SetByteValue(UNIT_FIELD_BYTES_2, 3, form);
        // force update as too quick shapeshifting and back
        // causes the value to stay the same serverside
        // causes issues clientside (player gets stuck)
        m_target->ForceValuesUpdateAtIndex(UNIT_FIELD_BYTES_2);

        if (modelid > 0)
            m_target->SetDisplayId(modelid);

        if (PowerType != POWER_MANA)
        {
            // reset power to default values only at power change
            if (m_target->getPowerType() != PowerType)
                m_target->setPowerType(PowerType);

            switch (form)
            {
            case FORM_CAT:
            case FORM_BEAR:
            case FORM_DIREBEAR:
                {
                    // get furor proc chance
                    uint32 FurorChance = 0;
                    Unit::AuraList const& mDummy = m_target->GetAurasByType(SPELL_AURA_DUMMY);
                    for (Unit::AuraList::const_iterator i = mDummy.begin(); i != mDummy.end(); ++i)
                    {
                        if ((*i)->GetSpellProto()->SpellIconID == 238)
                        {
                            FurorChance = (*i)->GetModifier()->m_amount;
                            break;
                        }
                    }

                    if (m_modifier.m_miscvalue == FORM_CAT)
                    {
                        m_target->SetPower(POWER_ENERGY, 0);
                        if (urand(1, 100) <= FurorChance)
                            m_target->CastSpell(m_target, 17099, true, NULL, this);
                    }
                    else
                    {
                        m_target->SetPower(POWER_RAGE, 0);
                        if (urand(1, 100) <= FurorChance)
                            m_target->CastSpell(m_target, 17057, true, NULL, this);
                    }
                    break;
                }
            case FORM_BATTLESTANCE:
            case FORM_DEFENSIVESTANCE:
            case FORM_BERSERKERSTANCE:
                {
                    uint32 Rage_val = 0;
                    // Stance mastery + Tactical mastery (both passive, and last have aura only in defense stance, but need apply at any stance switch)
                    if (m_target->GetTypeId() == TYPEID_PLAYER)
                    {
                        PlayerSpellMap const& sp_list = m_target->ToPlayer()->GetSpellMap();
                        for (PlayerSpellMap::const_iterator itr = sp_list.begin(); itr != sp_list.end(); ++itr)
                        {
                            if (itr->second.state == PLAYERSPELL_REMOVED) continue;
                            SpellEntry const* spellInfo = sSpellStore.LookupEntry(itr->first);
                            if (spellInfo && spellInfo->SpellFamilyName == SPELLFAMILY_WARRIOR && spellInfo->SpellIconID == 139)
                                Rage_val += m_target->CalculateSpellDamage(spellInfo, 0, spellInfo->EffectBasePoints[0], m_target) * 10;
                        }
                    }

                    if (m_target->GetPower(POWER_RAGE) > Rage_val)
                        m_target->SetPower(POWER_RAGE, Rage_val);
                    break;
                }
            default:
                break;
            }
        }

        m_target->m_ShapeShiftFormSpellId = GetId();
        m_target->m_ShapeShiftModelId     = modelid;
        m_target->m_form = form;
    }
    else
    {
        if (modelid > 0)
            m_target->SetDisplayId(m_target->GetNativeDisplayId());
        m_target->SetByteValue(UNIT_FIELD_BYTES_2, 3, FORM_NONE);
        if (m_target->getClass() == CLASS_DRUID)
            m_target->setPowerType(POWER_MANA);
        m_target->m_ShapeShiftFormSpellId = 0;
        m_target->m_form = FORM_NONE;

        switch (form)
        {
            // Nordrassil Harness - bonus
            case FORM_BEAR:
            case FORM_DIREBEAR:
            case FORM_CAT:
                if (Aura* dummy = m_target->GetDummyAura(37315))
                    m_target->CastSpell(m_target, 37316, true, NULL, dummy);
                break;
            // Nordrassil Regalia - bonus
            case FORM_MOONKIN:
                if (Aura* dummy = m_target->GetDummyAura(37324))
                    m_target->CastSpell(m_target, 37325, true, NULL, dummy);
                break;
            default:
                break;
        }

        // Remove auras linked to forms, such as Vanish and Stealth
        m_target->RemoveAurasWithAttribute(SPELL_ATTR2_UNK1);

        /* We need to re-apply any of transform auras:
           Great example is OHF, when you are in cat form and unshapeshift,
           you should be a human not your original model */
        Unit::AuraList const& trans = m_target->GetAurasByType(SPELL_AURA_TRANSFORM);
        for (Unit::AuraList::const_iterator i = trans.begin(); i != trans.end(); i++)
            (*i)->ApplyModifier(true, true);
    }

    // adding/removing linked auras
    // add/remove the shapeshift aura's boosts
    HandleShapeshiftBoosts(apply);

    if (m_target->GetTypeId() == TYPEID_PLAYER)
        m_target->ToPlayer()->InitDataForForm();
}

void Aura::HandleAuraTransform(bool apply, bool Real)
{
    if (apply)
    {
        // Shapeshifts (that change model) have higher priority than transforms (if not negative)
        if (m_target->HasShapeshiftChangingModel() && IsPositiveSpell(GetId()))
            return;

        // special case (spell specific functionality)
        if (m_modifier.m_miscvalue == 0)
        {
            switch (GetId())
            {
            case 16739:                 // Orb of Deception
                {
                    if (m_target->GetTypeId() != TYPEID_PLAYER)
                        return;

                    switch (m_target->getRace())
                    {
                        // Blood Elf
                        case RACE_BLOODELF:
                            m_target->SetDisplayId(m_target->getGender() == GENDER_FEMALE ? 17830 : 17829);
                            break;
                        // Orc
                        case RACE_ORC:
                            m_target->SetDisplayId(m_target->getGender() == GENDER_FEMALE ? 10140 : 10139);
                            break;
                        // Troll
                        case RACE_TROLL:
                            m_target->SetDisplayId(m_target->getGender() == GENDER_FEMALE ? 10134 : 10135);
                            break;
                        // Tauren
                        case RACE_TAUREN:
                            m_target->SetDisplayId(m_target->getGender() == GENDER_FEMALE ? 10147 : 10136);
                            break;
                        // Undead
                        case RACE_UNDEAD_PLAYER:
                            m_target->SetDisplayId(m_target->getGender() == GENDER_FEMALE ? 10145 : 10146);
                            break;
                        // Draenei
                        case RACE_DRAENEI:
                            m_target->SetDisplayId(m_target->getGender() == GENDER_FEMALE ? 17828 : 17827);
                            break;
                        // Dwarf
                        case RACE_DWARF:
                            m_target->SetDisplayId(m_target->getGender() == GENDER_FEMALE ? 10142 : 10141);
                            break;
                        // Gnome
                        case RACE_GNOME:
                            m_target->SetDisplayId(m_target->getGender() == GENDER_FEMALE ? 10149 : 10148);
                            break;
                        // Human
                        case RACE_HUMAN:
                            m_target->SetDisplayId(m_target->getGender() == GENDER_FEMALE ? 10138 : 10137);
                            break;
                        // Night Elf
                        case RACE_NIGHTELF:
                            m_target->SetDisplayId(m_target->getGender() == GENDER_FEMALE ? 10144 : 10143);
                            break;
                        default:
                            break;
                    }
                    break;
                }
                // Murloc costume
                case 42365:
                    m_target->SetDisplayId(21723);
                    break;
                // Dread Corsair - Pirate Day
                case 50531:
                case 50517:
                {
                    if (m_target->GetTypeId() != TYPEID_PLAYER)
                        return;

                    switch (m_target->getRace())
                    {
                        // Blood Elf
                        case RACE_BLOODELF:
                            m_target->SetDisplayId(m_target->getGender() == GENDER_MALE ? 25032 : 25043);
                            break;
                        // Orc
                        case RACE_ORC:
                            m_target->SetDisplayId(m_target->getGender() == GENDER_MALE ? 25039 : 25050);
                            break;
                        // Troll
                        case RACE_TROLL:
                            m_target->SetDisplayId(m_target->getGender() == GENDER_MALE ? 25041 : 25052);
                            break;
                        // Tauren
                        case RACE_TAUREN:
                            m_target->SetDisplayId(m_target->getGender() == GENDER_MALE ? 25040 : 25051);
                            break;
                        // Undead
                        case RACE_UNDEAD_PLAYER:
                            m_target->SetDisplayId(m_target->getGender() == GENDER_MALE ? 25042 : 25053);
                            break;
                        // Draenei
                        case RACE_DRAENEI:
                            m_target->SetDisplayId(m_target->getGender() == GENDER_MALE ? 25033 : 25044);
                            break;
                        // Dwarf
                        case RACE_DWARF:
                            m_target->SetDisplayId(m_target->getGender() == GENDER_MALE ? 25034 : 25045);
                            break;
                        // Gnome
                        case RACE_GNOME:
                            m_target->SetDisplayId(m_target->getGender() == GENDER_MALE ? 25035 : 25046);
                            break;
                        // Human
                        case RACE_HUMAN:
                            m_target->SetDisplayId(m_target->getGender() == GENDER_MALE ? 25037 : 25048);
                            break;
                        // Night Elf
                        case RACE_NIGHTELF:
                            m_target->SetDisplayId(m_target->getGender() == GENDER_MALE ? 25038 : 25049);
                            break;
                        default:
                            break;
                    }
                    break;
                }
            default:
                break;
            }
        }
        else
        {
            uint32 model_id;

            CreatureInfo const* ci = sObjectMgr.GetCreatureTemplate(m_modifier.m_miscvalue);
            if (!ci)
            {
                model_id = 16358;                            //pig pink ^_^
                sLog.outError("Auras: unknown creature id = %d (only need its modelid) Form Spell Aura Transform in Spell ID = %d", m_modifier.m_miscvalue, GetId());
            }
            else
                model_id = ci->GetRandomValidModelId();

            m_target->SetDisplayId(model_id);

            // Dragonmaw Illusion (set mount model also)
            if (GetId() == 42016 && m_target->GetMountID() && !m_target->GetAurasByType(SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED).empty())
                m_target->SetUInt32Value(UNIT_FIELD_MOUNTDISPLAYID, 16314);
        }

        // update active transform spell only not set or not overwriting negative by positive case
        if (!m_target->getTransForm() || !IsPositiveSpell(GetId()) || IsPositiveSpell(m_target->getTransForm()))
            m_target->setTransForm(GetId());

        // polymorph case
        if (Real && m_target->GetTypeId() == TYPEID_PLAYER && m_target->IsPolymorphed())
        {
            // for players, start regeneration after 1s (in polymorph fast regeneration case)
            // only if caster is Player (after patch 2.4.2)
            if (GetCaster()->IsPlayer())
                m_target->ToPlayer()->setRegenTimer(1*IN_MILLISECONDS);

            //dismount polymorphed target (after patch 2.4.2)
            if (m_target->IsMounted())
                m_target->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);
        }
    }
    else
    {
        if (m_target->getTransForm() == GetId())
            m_target->setTransForm(0);

        if (m_target->HasShapeshiftChangingModel())
            m_target->SetDisplayId(m_target->m_ShapeShiftModelId);
        else
            m_target->SetDisplayId(m_target->GetNativeDisplayId());

        // apply default equipment for creature case
        if (m_target->GetTypeId() == TYPEID_UNIT)
            ((Creature*)m_target)->LoadEquipment(((Creature*)m_target)->GetCreatureTemplate()->equipmentId, true);

        // re-apply some from still active with preference negative cases
        Unit::AuraList const& otherTransforms = m_target->GetAurasByType(SPELL_AURA_TRANSFORM);
        if (!otherTransforms.empty())
        {
            // look for other transform auras
            Aura* handledAura = *otherTransforms.begin();
            for (Unit::AuraList::const_iterator i = otherTransforms.begin(); i != otherTransforms.end(); ++i)
            {
                // negative auras are preferred
                if (!IsPositiveSpell((*i)->GetSpellProto()->Id))
                {
                    handledAura = *i;
                    break;
                }
            }
            handledAura->ApplyModifier(true);
        }

        // Dragonmaw Illusion (restore mount model)
        if (GetId() == 42016 && m_target->GetMountID() == 16314)
        {
            if (!m_target->GetAurasByType(SPELL_AURA_MOUNTED).empty())
            {
                uint32 cr_id = m_target->GetAurasByType(SPELL_AURA_MOUNTED).front()->GetModifier()->m_miscvalue;
                if (CreatureInfo const* ci = sObjectMgr.GetCreatureTemplate(cr_id))
                {
                    uint32 team = 0;
                    if (m_target->GetTypeId() == TYPEID_PLAYER)
                        team = m_target->ToPlayer()->GetTeam();

                    uint32 display_id = sObjectMgr.ChooseDisplayId(team, ci);
                    CreatureModelInfo const* minfo = sObjectMgr.GetCreatureModelRandomGender(display_id);
                    if (minfo)
                        display_id = minfo->modelid;

                    m_target->SetUInt32Value(UNIT_FIELD_MOUNTDISPLAYID, display_id);
                }
            }
        }
    }
}

void Aura::HandleForceReaction(bool apply, bool Real)
{
    if (!Real)
        return;

    Player* player = m_target->ToPlayer();
    if (!player)
        return;

    uint32 factionId = m_modifier.m_miscvalue;
    uint32 factionRank = m_modifier.m_amount;

    player->GetReputationMgr().ApplyForceReaction(factionId, ReputationRank(factionRank), apply);
    player->GetReputationMgr().SendForceReactions();
}

void Aura::HandleAuraModSkill(bool apply, bool /*Real*/)
{
    if (m_target->GetTypeId() != TYPEID_PLAYER)
        return;

    uint32 prot = GetSpellProto()->EffectMiscValue[m_effIndex];
    int32 points = GetModifierValue();

    m_target->ToPlayer()->ModifySkillBonus(prot, ((apply) ? points: -points), GetAuraType() == SPELL_AURA_MOD_SKILL_TALENT);
    if (prot == SKILL_DEFENSE)
        m_target->ToPlayer()->UpdateDefenseBonusesMod();
}

void Aura::HandleChannelDeathItem(bool apply, bool Real)
{
    if (apply || m_removeMode != AURA_REMOVE_BY_DEATH)
        return;

    Unit* caster = GetCaster();
    if (!caster || caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* plCaster = caster->ToPlayer();
    Unit* target = GetTarget();

    if (Real)
    {
        if (!target)
            return;

        // Item amount
        if (GetAmount() <= 0)
            return;

        SpellEntry const* spellInfo = GetSpellProto();
        if (spellInfo->EffectItemType[m_effIndex] == 0)
            return;

        // Soul Shard
        if (GetSpellProto()->EffectItemType[m_effIndex] == 6265)
        {
            // Soul Shard only from units that grant XP or honor
            if (!plCaster->isHonorOrXPTarget(target) ||
                (target->GetTypeId() == TYPEID_UNIT && !target->ToCreature()->isTappedBy(plCaster)))
                return;
        }

        //Adding items
        uint32 noSpaceForCount = 0;
        uint32 count = m_modifier.m_amount;

        ItemPosCountVec dest;
        uint8 msg = plCaster->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, spellInfo->EffectItemType[m_effIndex], count, &noSpaceForCount);
        if (msg != EQUIP_ERR_OK)
        {
            count-=noSpaceForCount;
            plCaster->SendEquipError(msg, NULL, NULL);
            if (count == 0)
                return;
        }

        Item* newitem = plCaster->StoreNewItem(dest, spellInfo->EffectItemType[m_effIndex], true);
        if (!newitem)
        {
            plCaster->SendEquipError(EQUIP_ERR_ITEM_NOT_FOUND, NULL, NULL);
            return;
        }
        plCaster->SendNewItem(newitem, count, true, true);
    }
}

void Aura::HandleBindSight(bool apply, bool /*Real*/)
{
    Unit* caster = GetCaster();
    if (!caster || caster->GetTypeId() != TYPEID_PLAYER)
        return;

    caster->ToPlayer()->SetViewpoint(m_target, apply);
}

//@todo complete
void Aura::HandleFarSight(bool /*apply*/, bool /*Real*/)
{
    /*Unit* caster = GetCaster();
    if (!caster || caster->GetTypeId() != TYPEID_PLAYER)
        return;

    caster->ToPlayer()->SetFarSight(apply ? m_target->GetGUID() : NULL);*/
}

void Aura::HandleAuraTrackCreatures(bool apply, bool /*Real*/)
{
    if (m_target->GetTypeId() != TYPEID_PLAYER)
        return;

    if (apply)
        m_target->RemoveNoStackAurasDueToAura(this);

    m_target->SetUInt32Value(PLAYER_TRACK_CREATURES, apply ? ((uint32)1) << (m_modifier.m_miscvalue - 1) : 0);
}

void Aura::HandleAuraTrackResources(bool apply, bool /*Real*/)
{
    if (m_target->GetTypeId() != TYPEID_PLAYER)
        return;

    if (apply)
        m_target->RemoveNoStackAurasDueToAura(this);

    m_target->SetUInt32Value(PLAYER_TRACK_RESOURCES, apply ? ((uint32)1) << (m_modifier.m_miscvalue - 1) : 0);
}

void Aura::HandleAuraTrackStealthed(bool apply, bool /*Real*/)
{
    if (m_target->GetTypeId() != TYPEID_PLAYER)
        return;

    if (apply)
        m_target->RemoveNoStackAurasDueToAura(this);
    else
    {
        // do not remove unit flag if there are more than this auraEffect of that kind on unit
        if (m_target->HasAuraType(GetAuraType()))
            return;
    }

    m_target->ApplyModFlag(PLAYER_FIELD_BYTES, PLAYER_FIELD_BYTE_TRACK_STEALTHED, apply);
}

void Aura::HandleAuraModScale(bool apply, bool /*Real*/)
{
    float scale = m_target->GetObjectScale();
    ApplyPercentModFloatVar(scale, float(GetModifierValue()), apply);
    m_target->SetObjectScale(scale);
}

void Aura::HandleModPossess(bool apply, bool Real)
{
    if (!Real)
        return;

    Unit* caster = GetCaster();
    if (caster && caster->GetTypeId() == TYPEID_UNIT)
    {
        HandleModCharm(apply, Real);
        return;
    }

    if (apply)
    {
        if (int32(m_target->getLevel()) > m_modifier.m_amount)
            return;

        m_target->SetCharmedBy(caster, CHARM_TYPE_POSSESS);
    }
    else
        m_target->RemoveCharmedBy(caster);
}

void Aura::HandleModPossessPet(bool apply, bool Real)
{
    if (!Real)
        return;

    Unit* caster = GetCaster();
    if (!caster || caster->GetTypeId() != TYPEID_PLAYER)
        return;

    if (m_target->GetTypeId() != TYPEID_UNIT || !m_target->IsPet())
        return;

    Pet* pet = m_target->ToPet();

    if (apply)
    {
        if (caster->ToPlayer()->GetPet() != pet)
            return;

        // Must clear current motion or pet leashes back to owner after a few yards
        //  when under spell 'Eyes of the Beast'
        pet->GetMotionMaster()->Clear();
        pet->SetCharmedBy(caster, CHARM_TYPE_POSSESS);
    }
    else
    {
        pet->RemoveCharmedBy(caster);

        if (!pet->IsWithinDistInMap(caster, pet->GetMap()->GetVisibilityRange()))
            pet->Remove(PET_SAVE_NOT_IN_SLOT, true);
        else
        {
            // Reinitialize the pet bar and make the pet come back to the owner
            caster->ToPlayer()->PetSpellInitialize();

            // Follow owner only if not fighting or owner didn't click "stay" at new location
            // This may be confusing because pet bar shows "stay" when under the spell but it retains
            //  the "follow" flag. Player MUST click "stay" while under the spell.
            if (!pet->GetVictim() && !pet->GetCharmInfo()->HasCommandState(COMMAND_STAY))
            {
                pet->GetMotionMaster()->MoveFollow(caster, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
                pet->GetCharmInfo()->SetCommandState(COMMAND_FOLLOW);
            }
        }
    }
}

void Aura::HandleModCharm(bool apply, bool Real)
{
    if (!Real)
        return;

    Unit* caster = GetCaster();

    if (apply)
    {
        if (int32(m_target->getLevel()) > m_modifier.m_amount)
            return;

        m_target->SetCharmedBy(caster, CHARM_TYPE_CHARM);
    }
    else
        m_target->RemoveCharmedBy(caster);
}

void Aura::HandleModConfuse(bool apply, bool Real)
{
    if (!Real)
        return;

    m_target->SetControlled(apply, UNIT_STATE_CONFUSED);
}

void Aura::HandleModFear(bool apply, bool Real)
{
    if (!Real)
        return;

    m_target->SetControlled(apply, UNIT_STATE_FLEEING);
}

void Aura::HandleFeignDeath(bool apply, bool Real)
{
    if (!Real)
        return;

    if (apply)
    {
        /*
        WorldPacket data(SMSG_FEIGN_DEATH_RESISTED, 9);
        data<<m_target->GetGUID();
        data<<uint8(0);
        m_target->SendMessageToSet(&data,true);
        */

        std::list<Unit*> targets;
        Oregon::AnyUnfriendlyUnitInObjectRangeCheck u_check(m_target, m_target, m_target->GetMap()->GetVisibilityRange());
        Oregon::UnitListSearcher<Oregon::AnyUnfriendlyUnitInObjectRangeCheck> searcher(targets, u_check);
        m_target->VisitNearbyObject(m_target->GetMap()->GetVisibilityRange(), searcher);
        for (std::list<Unit*>::iterator iter = targets.begin(); iter != targets.end(); ++iter)
        {
            if (!(*iter)->HasUnitState(UNIT_STATE_CASTING))
                continue;

            for (uint32 i = CURRENT_FIRST_NON_MELEE_SPELL; i < CURRENT_MAX_SPELL; i++)
            {
                if ((*iter)->GetCurrentSpell(i)
                && (*iter)->GetCurrentSpell(i)->m_targets.getUnitTargetGUID() == m_target->GetGUID())
                    (*iter)->InterruptSpell(CurrentSpellTypes(i), false);
            }
        }
        m_target->CombatStop();
        m_target->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_UNATTACKABLE);

        // prevent interrupt message
        if (m_caster_guid == m_target->GetGUID() && m_target->GetCurrentSpell(CURRENT_GENERIC_SPELL))
            m_target->GetCurrentSpell(CURRENT_GENERIC_SPELL)->finish();
        m_target->InterruptNonMeleeSpells(true);
        m_target->getHostileRefManager().deleteReferences();

        m_target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNK_29);            // blizz like 2.0.x
        m_target->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH);    // blizz like 2.0.x
        m_target->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);         // blizz like 2.0.x
        m_target->AddUnitState(UNIT_STATE_DIED);

        if (Creature* creature = m_target->ToCreature())
            creature->SetReactState(REACT_PASSIVE);
    }
    else
    {
        /*
        WorldPacket data(SMSG_FEIGN_DEATH_RESISTED, 9);
        data<<m_target->GetGUID();
        data<<uint8(1);
        m_target->SendMessageToSet(&data,true);
        */

        m_target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNK_29);         // blizz like 2.0.x
        m_target->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH); // blizz like 2.0.x
        m_target->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);      // blizz like 2.0.x
        m_target->ClearUnitState(UNIT_STATE_DIED);

        if (Creature* creature = m_target->ToCreature())
            creature->InitializeReactState();
    }
}

void Aura::HandleAuraModDisarm(bool apply, bool Real)
{
    if (!Real)
        return;

    if (!apply && m_target->HasAuraType(SPELL_AURA_MOD_DISARM))
        return;

    // not sure for it's correctness
    if (apply)
        m_target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISARMED);
    else
        m_target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISARMED);

    if (m_target->GetTypeId() == TYPEID_PLAYER)
    {
        // main-hand attack speed already set to special value for feral form already and don't must change and reset at remove.
        if (m_target->ToPlayer()->IsInFeralForm())
            return;

        if (apply)
            m_target->SetAttackTime(BASE_ATTACK, BASE_ATTACK_TIME);
        else
            m_target->ToPlayer()->SetRegularAttackTime();
    }
    else
    {
        // creature does not have equipment
        if (apply && !m_target->ToCreature()->GetCurrentEquipmentId())
            return;
    }

    m_target->UpdateDamagePhysical(BASE_ATTACK);
}

void Aura::HandleAuraModStun(bool apply, bool Real)
{
    if (!Real)
        return;

    m_target->SetControlled(apply, UNIT_STATE_STUNNED);
}

void Aura::HandleModStealth(bool apply, bool Real)
{
    StealthType type = StealthType(GetMiscValue());

    if (apply)
    {
        // only at real aura add
        if (Real)
        {
            m_target->m_stealth.AddFlag(type);
            m_target->m_stealth.AddValue(type, GetAmount());

            m_target->SetStandFlags(UNIT_STAND_FLAGS_CREEP);
            if (m_target->GetTypeId() == TYPEID_PLAYER)
                m_target->SetFlag(PLAYER_FIELD_BYTES2, PLAYER_FIELD_BYTE2_STEALTH);

            // for RACE_NIGHTELF stealth
            if (m_target->GetTypeId() == TYPEID_PLAYER && GetId() == 20580)
                m_target->CastSpell(m_target, 21009, true, NULL, this);
        }

        std::list<Unit*> targets;
        Oregon::AnyUnfriendlyUnitInObjectRangeCheck u_check(m_target, m_target, m_target->GetMap()->GetVisibilityRange());
        Oregon::UnitListSearcher<Oregon::AnyUnfriendlyUnitInObjectRangeCheck> searcher(targets, u_check);
        m_target->VisitNearbyObject(m_target->GetMap()->GetVisibilityRange(), searcher);
        for (std::list<Unit*>::iterator iter = targets.begin(); iter != targets.end(); ++iter)
        {
            if (!(*iter)->HasUnitState(UNIT_STATE_CASTING))
                continue;

            for (uint32 i = CURRENT_FIRST_NON_MELEE_SPELL; i < CURRENT_MAX_SPELL; i++)
            {
                if ((*iter)->GetCurrentSpell(i)
                    && (*iter)->GetCurrentSpell(i)->m_targets.getUnitTargetGUID() == m_target->GetGUID())
                    (*iter)->InterruptSpell(CurrentSpellTypes(i), false);
            }
        }
    }
    else
    {
        // only at real aura remove
        if (Real)
        {
            // for RACE_NIGHTELF stealth
            if (m_target->GetTypeId() == TYPEID_PLAYER && GetId() == 20580)
                m_target->RemoveAurasDueToSpell(21009);

            m_target->m_stealth.AddValue(type, -GetAmount());

            if (!m_target->HasAuraType(SPELL_AURA_MOD_STEALTH)) // if last SPELL_AURA_MOD_STEALTH
            {
                m_target->m_stealth.DelFlag(type);

                m_target->RemoveStandFlags(UNIT_STAND_FLAGS_CREEP);
                if (m_target->GetTypeId() == TYPEID_PLAYER)
                    m_target->RemoveFlag(PLAYER_FIELD_BYTES2, PLAYER_FIELD_BYTE2_STEALTH);
            }
        }
    }

    // Master of Subtlety
    Unit::AuraList const& mDummyAuras = m_target->GetAurasByType(SPELL_AURA_DUMMY);
    for (Unit::AuraList::const_iterator i = mDummyAuras.begin(); i != mDummyAuras.end(); ++i)
    {
        if ((*i)->GetSpellProto()->SpellIconID == 2114 && Real)
        {
            if (apply)
            {
                int32 bp = (*i)->GetModifier()->m_amount;
                m_target->CastCustomSpell(m_target, 31665, &bp, NULL, NULL, true);
            }
            else
                m_target->CastSpell(m_target, 31666, true);
            break;
        }
    }

    // call functions which may have additional effects after chainging state of unit
    if (apply && Real)
    {
        // drop flag at stealth in bg
        m_target->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_UNATTACKABLE);
    }

    m_target->UpdateObjectVisibility(false);
}

void Aura::HandleModStealthLevel(bool apply, bool Real)
{
    StealthType type = StealthType(GetMiscValue());

    if (apply)
        m_target->m_stealth.AddValue(type, GetAmount());
    else
        m_target->m_stealth.AddValue(type, -GetAmount());

    m_target->UpdateObjectVisibility(false);
}

void Aura::HandleModInvisibility(bool apply, bool Real)
{
    InvisibilityType type = InvisibilityType(GetMiscValue());

    if (apply)
    {
        if (Real && m_target->GetTypeId() == TYPEID_PLAYER)
            m_target->SetFlag(PLAYER_FIELD_BYTES2, PLAYER_FIELD_BYTE2_INVISIBILITY_GLOW);   // apply glow vision

        m_target->m_invisibility.AddFlag(type);
        m_target->m_invisibility.AddValue(type, GetAmount());
    }
    else
    {
        if (!m_target->HasAuraType(SPELL_AURA_MOD_INVISIBILITY))
        {
            // if not have different invisibility auras.
            // remove glow vision
            if (m_target->GetTypeId() == TYPEID_PLAYER)
                m_target->RemoveFlag(PLAYER_FIELD_BYTES2, PLAYER_FIELD_BYTE2_INVISIBILITY_GLOW);

            m_target->m_invisibility.DelFlag(type);
        }
        else
        {
            bool found = false;
            Unit::AuraList const& invisAuras = m_target->GetAurasByType(SPELL_AURA_MOD_INVISIBILITY);
            for (Unit::AuraList::const_iterator i = invisAuras.begin(); i != invisAuras.end(); ++i)
            {
                if (GetMiscValue() == (*i)->GetMiscValue())
                {
                    found = true;
                    break;
                }
            }
            if (!found)
                m_target->m_invisibility.DelFlag(type);
        }

        m_target->m_invisibility.AddValue(type, -GetAmount());
    }

    // call functions which may have additional effects after chainging state of unit
    if (apply && Real)
        m_target->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_UNATTACKABLE);  // drop flag at invisibiliy in bg

    m_target->UpdateObjectVisibility(false);
}

void Aura::HandleModStealthDetect(bool apply, bool Real)
{
    StealthType type = StealthType(GetMiscValue());

    if (apply)
    {
        m_target->m_stealthDetect.AddFlag(type);
        m_target->m_stealthDetect.AddValue(type, GetAmount());
    }
    else
    {
        if (!m_target->HasAuraType(SPELL_AURA_MOD_STEALTH_DETECT))
            m_target->m_stealthDetect.DelFlag(type);

        m_target->m_stealthDetect.AddValue(type, -GetAmount());
    }

    m_target->UpdateObjectVisibility(false);
}

void Aura::HandleModInvisibilityDetect(bool apply, bool Real)
{
    InvisibilityType type = InvisibilityType(GetMiscValue());

    if (apply)
    {
        m_target->m_invisibilityDetect.AddFlag(type);
        m_target->m_invisibilityDetect.AddValue(type, GetAmount());
    }
    else
    {
        if (!m_target->HasAuraType(SPELL_AURA_MOD_INVISIBILITY_DETECT))
            m_target->m_invisibilityDetect.DelFlag(type);

        m_target->m_invisibilityDetect.AddValue(type, -GetAmount());
    }

    m_target->UpdateObjectVisibility(false);
}

void Aura::HandleAuraModRoot(bool apply, bool Real)
{
    // only at real add/remove aura
    if (!Real)
        return;

    m_target->SetControlled(apply, UNIT_STATE_ROOT);
}

void Aura::HandleAuraModSilence(bool apply, bool Real)
{
    // only at real add/remove aura
    if (!Real)
        return;

    if (apply)
    {
        m_target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED);

        // call functions which may have additional effects after chainging state of unit
        // Stop cast only spells vs PreventionType == SPELL_PREVENTION_TYPE_SILENCE
        for (uint32 i = CURRENT_MELEE_SPELL; i < CURRENT_MAX_SPELL; ++i)
            if (Spell* spell = m_target->GetCurrentSpell(i))
                if (spell->m_spellInfo->PreventionType == SPELL_PREVENTION_TYPE_SILENCE)
                    // Stop spells on prepare or casting state
                    m_target->InterruptSpell(CurrentSpellTypes(i), false);
    }
    else
    {
        // do not remove unit flag if there are more than this auraEffect of that kind on unit
        if (m_target->HasAuraType(SPELL_AURA_MOD_SILENCE) || m_target->HasAuraType(SPELL_AURA_MOD_PACIFY_SILENCE))
            return;

        m_target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED);
    }
}

void Aura::HandleModThreat(bool apply, bool Real)
{
    // only at real add/remove aura
    if (!Real)
        return;

    if (!m_target->IsAlive())
        return;

    Unit* caster = GetCaster();

    if (!caster || !caster->IsAlive())
        return;

    int level_diff = 0;
    int multiplier = 0;
    switch (GetId())
    {
        // Arcane Shroud
        case 26400:
            level_diff = m_target->getLevel() - 60;
            multiplier = 2;
            break;
        // The Eye of Diminution
        case 28862:
            level_diff = m_target->getLevel() - 60;
            multiplier = 1;
            break;
    }
    if (level_diff > 0)
        m_modifier.m_amount += multiplier * level_diff;

    if (m_target->GetTypeId() == TYPEID_PLAYER)
        for (int8 i = 0; i < MAX_SPELL_SCHOOL; i++)
            if (GetMiscValue() & int32(1 << i))
                ApplyPercentModFloatVar(m_target->m_threatModifier[i], float(GetModifierValue()), apply);

}

void Aura::HandleAuraModTotalThreat(bool apply, bool Real)
{
    // only at real add/remove aura
    if (!Real)
        return;

    if (!m_target->IsAlive() || m_target->GetTypeId() != TYPEID_PLAYER)
        return;

    Unit* caster = GetCaster();

    if (caster && caster->IsAlive())
        m_target->getHostileRefManager().addTempThreat((float)GetModifierValue(), apply);
}

void Aura::HandleModTaunt(bool apply, bool Real)
{
    // only at real add/remove aura
    if (!Real)
        return;

    if (!m_target->IsAlive() || !m_target->CanHaveThreatList())
        return;

    Unit* caster = GetCaster();

    if (!caster || !caster->IsAlive())
        return;

    if (apply)
        m_target->TauntApply(caster);
    else
    {
        // When taunt aura fades out, mob will switch to previous target if current has less than 1.1 * secondthreat
        m_target->TauntFadeOut(caster);
    }
}

/*********************************************************/
/***                 MODIFY SPEED                     ***/
/*********************************************************/
void Aura::HandleAuraModIncreaseSpeed(bool /*apply*/, bool Real)
{
    // all applied/removed only at real aura add/remove
    if (!Real)
        return;

    m_target->UpdateSpeed(MOVE_RUN, true);
}

void Aura::HandleAuraModIncreaseMountedSpeed(bool apply, bool Real)
{
    HandleAuraModIncreaseSpeed(apply, Real);
}

void Aura::HandleAuraModIncreaseFlightSpeed(bool apply, bool Real)
{
    // all applied/removed only at real aura add/remove
    if (!Real)
        return;

    // Enable Fly mode for flying mounts
    if (GetAuraType() == SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED)
    {
        if (m_target->SetCanFly(apply))
            if (!apply && !m_target->IsLevitating())
                m_target->GetMotionMaster()->MoveFall();

        if (!apply && m_target->GetTypeId() == TYPEID_UNIT && !m_target->IsLevitating())
            m_target->GetMotionMaster()->MoveFall();

        // Players on flying mounts must be immune to polymorph
        if (m_target->GetTypeId() == TYPEID_PLAYER)
            m_target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, apply);

        // Dragonmaw Illusion (overwrite mount model, mounted aura already applied)
        if (apply && m_target->HasAura(42016, 0) && m_target->GetMountID())
            m_target->SetUInt32Value(UNIT_FIELD_MOUNTDISPLAYID, 16314);
    }

    m_target->UpdateSpeed(MOVE_FLIGHT, true);
}

void Aura::HandleAuraModIncreaseSwimSpeed(bool /*apply*/, bool Real)
{
    // all applied/removed only at real aura add/remove
    if (!Real)
        return;

    m_target->UpdateSpeed(MOVE_SWIM, true);
}

void Aura::HandleAuraModDecreaseSpeed(bool apply, bool Real)
{
    // all applied/removed only at real aura add/remove
    if (!Real)
        return;

    /// @todo: Not sure if all spells with this aura shouldn't dispel stealth \
    /// perhaps all not passive negative area auras should?
    switch (m_spellProto->Id)
    {
        case 12323: // Piercing Howl
            if (apply)
                m_target->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);
            break;
    }

    m_target->UpdateSpeed(MOVE_RUN, true);
    m_target->UpdateSpeed(MOVE_SWIM, true);
    m_target->UpdateSpeed(MOVE_FLIGHT, true);
    m_target->UpdateSpeed(MOVE_RUN_BACK, true);
    m_target->UpdateSpeed(MOVE_SWIM_BACK, true);
    m_target->UpdateSpeed(MOVE_FLIGHT_BACK, true);
}

void Aura::HandleAuraModUseNormalSpeed(bool /*apply*/, bool Real)
{
    // all applied/removed only at real aura add/remove
    if (!Real)
        return;

    m_target->UpdateSpeed(MOVE_RUN, true);
    m_target->UpdateSpeed(MOVE_SWIM, true);
    m_target->UpdateSpeed(MOVE_FLIGHT, true);
}

/*********************************************************/
/***                    IMMUNITY                      ***/
/*********************************************************/

void Aura::HandleModMechanicImmunity(bool apply, bool /*Real*/)
{
    uint32 mechanic = 1 << m_modifier.m_miscvalue;

    //immune movement impairment and loss of control
    if (IsSpellRemoveAllMovementAndControlLossEffects(GetSpellProto()))
        mechanic = IMMUNE_TO_MOVEMENT_IMPAIRMENT_AND_LOSS_CONTROL_MASK;

    if (apply && GetSpellProto()->AttributesEx & SPELL_ATTR1_DISPEL_AURAS_ON_IMMUNITY)
    {
        Unit::AuraMap& Auras = m_target->GetAuras();
        for (Unit::AuraMap::iterator iter = Auras.begin(), next; iter != Auras.end(); iter = next)
        {
            next = iter;
            ++next;
            SpellEntry const* spell = iter->second->GetSpellProto();
            if (!(spell->Attributes & SPELL_ATTR0_UNAFFECTED_BY_INVULNERABILITY)  // spells unaffected by invulnerability
                && !iter->second->IsPositive()                                    // only remove negative spells
                && spell->Id != GetId())
            {
                //check for mechanic mask
                if (GetSpellMechanicMask(spell, iter->second->GetEffIndex()) & mechanic)
                {
                    m_target->RemoveAurasDueToSpell(spell->Id);
                    if (Auras.empty())
                        break;
                    else
                        next = Auras.begin();
                }
            }
        }
    }

    m_target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, m_modifier.m_miscvalue, apply);

    // special cases
    switch (m_modifier.m_miscvalue)
    {
        case MECHANIC_INVULNERABILITY:
            m_target->ModifyAuraState(AURA_STATE_FORBEARANCE, apply);
            break;
        case MECHANIC_SHIELD:
            m_target->ModifyAuraState(AURA_STATE_WEAKENED_SOUL, apply);
            break;
    }

    // Bestial Wrath
    if (GetSpellProto()->SpellFamilyName == SPELLFAMILY_HUNTER && GetSpellProto()->Id == 19574)
    {
        // The Beast Within cast on owner if talent present
        if (Unit* owner = m_target->GetOwner())
        {
            // Search talent
            Unit::AuraList const& m_dummyAuras = owner->GetAurasByType(SPELL_AURA_DUMMY);
            for (Unit::AuraList::const_iterator i = m_dummyAuras.begin(); i != m_dummyAuras.end(); ++i)
            {
                if ((*i)->GetSpellProto()->SpellIconID == 2229)
                {
                    if (apply)
                        owner->CastSpell(owner, 34471, true, 0, this);
                    else
                        owner->RemoveAurasDueToSpell(34471);
                    break;
                }
            }
        }
    }

    // The Beast Within and Bestial Wrath - immunity
    if (GetId() == 19574 || GetId() == 34471)
    {
        if (apply)
        {
            m_target->CastSpell(m_target, 24395, true);
            m_target->CastSpell(m_target, 24396, true);
            m_target->CastSpell(m_target, 24397, true);
            m_target->CastSpell(m_target, 26592, true);
        }
        else
        {
            m_target->RemoveAurasDueToSpell(24395);
            m_target->RemoveAurasDueToSpell(24396);
            m_target->RemoveAurasDueToSpell(24397);
            m_target->RemoveAurasDueToSpell(26592);
        }
    }
}

void Aura::HandleAuraModEffectImmunity(bool apply, bool Real)
{
    if (!Real)
        return;

    m_target->ApplySpellImmune(GetId(), IMMUNITY_EFFECT, m_modifier.m_miscvalue, apply);

    // when removing flag aura, handle flag drop
    Player* player = m_target->ToPlayer();
    if (!apply && player && (GetSpellProto()->AuraInterruptFlags & AURA_INTERRUPT_FLAG_UNATTACKABLE))
    {
        if (player->InBattleground())
        {
            if (Battleground* bg = player->GetBattleground())
                bg->EventPlayerDroppedFlag(player);
        }
        else
            sOutdoorPvPMgr.HandleDropFlag(player, GetSpellProto()->Id);
    }
}

void Aura::HandleAuraModStateImmunity(bool apply, bool Real)
{
    if (!Real)
        return;

    if (apply && GetSpellProto()->AttributesEx & SPELL_ATTR1_DISPEL_AURAS_ON_IMMUNITY)
    {
        Unit::AuraList const& auraList = m_target->GetAurasByType(AuraType(m_modifier.m_miscvalue));
        for (Unit::AuraList::const_iterator itr = auraList.begin(); itr != auraList.end();)
        {
            if (auraList.front() != this)                   // skip itself aura (it already added)
            {
                m_target->RemoveAurasDueToSpell(auraList.front()->GetId());
                itr = auraList.begin();
            }
            else
                ++itr;
        }
    }

    m_target->ApplySpellImmune(GetId(), IMMUNITY_STATE, m_modifier.m_miscvalue, apply);
}

void Aura::HandleAuraModSchoolImmunity(bool apply, bool Real)
{
    if (apply && m_modifier.m_miscvalue == SPELL_SCHOOL_MASK_NORMAL)
        m_target->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_UNATTACKABLE);

    m_target->ApplySpellImmune(GetId(), IMMUNITY_SCHOOL, m_modifier.m_miscvalue, apply);

    if (Real && apply && GetSpellProto()->AttributesEx & SPELL_ATTR1_DISPEL_AURAS_ON_IMMUNITY)
    {
        if (IsPositiveSpell(GetId()))                        //Only positive immunity removes auras
        {
            uint32 school_mask = m_modifier.m_miscvalue;
            Unit::AuraMap& Auras = m_target->GetAuras();
            for (Unit::AuraMap::iterator iter = Auras.begin(), next; iter != Auras.end(); iter = next)
            {
                next = iter;
                ++next;
                SpellEntry const* spell = iter->second->GetSpellProto();
                if ((GetSpellSchoolMask(spell) & school_mask)//Check for school mask
                    && !(spell->Attributes & SPELL_ATTR0_UNAFFECTED_BY_INVULNERABILITY)   //Spells unaffected by invulnerability
                    && !iter->second->IsPositive()          //Don't remove positive spells
                    && spell->Id != GetId())               //Don't remove self
                {
                    m_target->RemoveAurasDueToSpell(spell->Id);
                    if (Auras.empty())
                        break;
                    else
                        next = Auras.begin();
                }
            }
        }
    }
    if (Real && GetSpellProto()->Mechanic == MECHANIC_BANISH)
    {
        if (apply)
            m_target->AddUnitState(UNIT_STATE_ISOLATED);
        else
            m_target->ClearUnitState(UNIT_STATE_ISOLATED);
    }
}

void Aura::HandleAuraModDmgImmunity(bool apply, bool /*Real*/)
{
    m_target->ApplySpellImmune(GetId(), IMMUNITY_DAMAGE, m_modifier.m_miscvalue, apply);
}

void Aura::HandleAuraModDispelImmunity(bool apply, bool Real)
{
    // all applied/removed only at real aura add/remove
    if (!Real)
        return;

    m_target->ApplySpellDispelImmunity(m_spellProto, DispelType(m_modifier.m_miscvalue), apply);

    if (GetId() == 20594)   //stoneform
    {
        //handle diseases
        m_target->ApplySpellDispelImmunity(m_spellProto, DISPEL_DISEASE, apply);

        //remove bleed auras
        const uint32 mechanic = 1 << MECHANIC_BLEED;

        if (apply)
        {
            Unit::AuraMap& Auras = m_target->GetAuras();
            for (Unit::AuraMap::iterator iter = Auras.begin(); iter != Auras.end(); ++iter)
            {
                SpellEntry const* spell = iter->second->GetSpellProto();
                if (!(spell->Attributes & SPELL_ATTR0_UNAFFECTED_BY_INVULNERABILITY)  // spells unaffected by invulnerability
                    && !iter->second->IsPositive()                                    // only remove negative spells
                    && spell->Id != GetId()
                    && (GetSpellMechanicMask(spell, iter->second->GetEffIndex()) & mechanic))
                {
                    m_target->RemoveAurasDueToSpell(spell->Id);
                    iter = Auras.begin();
                }
            }
        }

        m_target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_BLEED, apply);
    }
}

void Aura::HandleAuraProcTriggerSpell(bool apply, bool Real)
{
    if (!Real)
        return;

    if (apply)
    {
        // some spell have charges by functionality not have its in spell data
        switch (GetId())
        {
            case 28200:                                     // Ascendance (Talisman of Ascendance trinket)
                m_procCharges = 6;
                UpdateAuraCharges();
                break;
            default:
                break;
        }
    }
}

void Aura::HandleAuraModStalked(bool apply, bool /*Real*/)
{
    // used by spells: Hunter's Mark, Mind Vision, Syndicate Tracker (MURP) DND
    if (apply)
        m_target->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_TRACK_UNIT);
    else
    {
        // do not remove unit flag if there are more than this auraEffect of that kind on unit
        if (!m_target->HasAuraType(GetAuraType()))
            m_target->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_TRACK_UNIT);
    }

    // call functions which may have additional effects after chainging state of unit
    m_target->UpdateObjectVisibility(false);
}

/*********************************************************/
/***                  PERIODIC                        ***/
/*********************************************************/

void Aura::HandlePeriodicTriggerSpell(bool apply, bool /*Real*/)
{
    if (m_periodicTimer <= 0)
        m_periodicTimer += m_amplitude;

    m_isPeriodic = apply;

    // Curse of the Plaguebringer
    if (!apply && m_spellProto->Id == 29213 && m_removeMode != AURA_REMOVE_BY_DISPEL)
    {
        // Cast Wrath of the Plaguebringer if not dispelled
        m_target->CastSpell(m_target, 29214, true, 0, this);
    }

    // Wrath of the Astromancer
    if (!apply && m_spellProto->Id == 42783)
        m_target->CastSpell(m_target, 42787, true, 0, this);
}

void Aura::HandlePeriodicTriggerSpellWithValue(bool apply, bool /*Real*/)
{
    if (m_periodicTimer <= 0)
        m_periodicTimer += m_modifier.periodictime;

    m_isPeriodic = apply;
}

void Aura::HandlePeriodicEnergize(bool apply, bool /*Real*/)
{
    if (m_periodicTimer <= 0)
        m_periodicTimer += m_modifier.periodictime;

    m_isPeriodic = apply;

    if (GetId() == 5229)
        m_target->UpdateArmor();

}

void Aura::HandleAuraPeriodicDummy(bool apply, bool Real)
{
    // spells required only Real aura add/remove
    if (!Real)
        return;

    SpellEntry const* spell = GetSpellProto();
    switch (spell->SpellFamilyName)
    {
    case SPELLFAMILY_ROGUE:
        {
            // Master of Subtlety
            if (spell->Id == 31666 && !apply && Real)
            {
                m_target->RemoveAurasDueToSpell(31665);
                break;
            }
            break;
        }
    case SPELLFAMILY_HUNTER:
        {
            // Aspect of the Viper
            if (spell->SpellFamilyFlags & 0x0004000000000000LL)
            {
                // Update regen on remove
                if (!apply && m_target->GetTypeId() == TYPEID_PLAYER)
                    m_target->ToPlayer()->UpdateManaRegen();
                break;
            }
            break;
        }
    }

    m_isPeriodic = apply;
}

void Aura::HandlePeriodicHeal(bool apply, bool Real)
{
    if (m_periodicTimer <= 0)
        m_periodicTimer += m_amplitude;

    m_isPeriodic = apply;

    // only at real apply
    if (Real && apply && GetSpellProto()->Mechanic == MECHANIC_BANDAGE)
    {
        // provided m_target as original caster to prevent apply aura caster selection for this negative buff
        m_target->CastSpell(m_target, 11196, true, NULL, this, m_target->GetGUID());
    }

    // For prevent double apply bonuses
    bool loading = (m_target->GetTypeId() == TYPEID_PLAYER && m_target->ToPlayer()->GetSession()->PlayerLoading());

    if (!loading && apply)
    {
        switch (m_spellProto->SpellFamilyName)
        {
        case SPELLFAMILY_DRUID:
            {
                // Rejuvenation
                if (m_spellProto->SpellFamilyFlags & 0x0000000000000010LL)
                {
                    if (Unit* caster = GetCaster())
                    {
                        Unit::AuraList const& classScripts = caster->GetAurasByType(SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
                        for (Unit::AuraList::const_iterator k = classScripts.begin(); k != classScripts.end(); ++k)
                        {
                            int32 tickcount = GetSpellDuration(m_spellProto) / m_spellProto->EffectAmplitude[m_effIndex];
                            switch ((*k)->GetModifier()->m_miscvalue)
                            {
                            case 4953:                          // Increased Rejuvenation Healing - Harold's Rejuvenating Broach Aura
                            case 4415:                          // Increased Rejuvenation Healing - Idol of Rejuvenation Aura
                                {
                                    m_modifier.m_amount += (*k)->GetModifier()->m_amount / tickcount;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void Aura::HandlePeriodicDamage(bool apply, bool Real)
{
    // spells required only Real aura add/remove
    if (!Real)
        return;

    if (m_periodicTimer <= 0)
        m_periodicTimer += m_amplitude;

    m_isPeriodic = apply;

    // For prevent double apply bonuses
    bool loading = (m_target->GetTypeId() == TYPEID_PLAYER && m_target->ToPlayer()->GetSession()->PlayerLoading());

    Unit* caster = GetCaster();

    switch (m_spellProto->SpellFamilyName)
    {
    case SPELLFAMILY_GENERIC:
        {
            // Pounce Bleed
            if (m_spellProto->SpellIconID == 147 && m_spellProto->SpellVisual == 0)
            {
                // $AP*0.18/6 bonus per tick
                if (apply && !loading && caster)
                    m_modifier.m_amount += int32(caster->GetTotalAttackPowerValue(BASE_ATTACK) * 3 / 100);
                return;
            }
            break;
        }
    case SPELLFAMILY_WARRIOR:
        {
            // Rend
            if (m_spellProto->SpellFamilyFlags & 0x0000000000000020LL)
            {
                // 0.00743*(($MWB+$mwb)/2+$AP/14*$MWS) bonus per tick
                if (apply && !loading && caster)
                {
                    float ap = caster->GetTotalAttackPowerValue(BASE_ATTACK);
                    int32 mws = caster->GetAttackTime(BASE_ATTACK);
                    float mwb_min = caster->GetWeaponDamageRange(BASE_ATTACK, MINDAMAGE);
                    float mwb_max = caster->GetWeaponDamageRange(BASE_ATTACK, MAXDAMAGE);
                    // WARNING! in 3.0 multiplier 0.00743f change to 0.6
                    m_modifier.m_amount += int32(((mwb_min + mwb_max) / 2 + ap * mws / 14000) * 0.00743f);
                }
                return;
            }
            break;
        }
    case SPELLFAMILY_DRUID:
        {
            // Rake
            if (m_spellProto->SpellFamilyFlags & 0x0000000000001000LL)
            {
                // $AP*0.06/3 bonus per tick
                if (apply && !loading && caster)
                    m_modifier.m_amount += int32(caster->GetTotalAttackPowerValue(BASE_ATTACK) * 2 / 100);
                return;
            }
            // Lacerate
            if (m_spellProto->SpellFamilyFlags & 0x000000010000000000LL)
            {
                // $AP*0.05/5 bonus per tick
                if (apply && !loading && caster)
                    m_modifier.m_amount += int32(caster->GetTotalAttackPowerValue(BASE_ATTACK) / 100);
                return;
            }
            // Rip
            if (m_spellProto->SpellFamilyFlags & 0x000000000000800000LL)
            {
                // $AP * min(0.06*$cp, 0.24)/6 [Yes, there is no difference, whether 4 or 5 CPs are being used]
                if (apply && !loading && caster && caster->GetTypeId() == TYPEID_PLAYER)
                {
                    uint8 cp = caster->ToPlayer()->GetComboPoints();

                    // Idol of Feral Shadows. Cant be handled as SpellMod in SpellAura:Dummy due its dependency from CPs
                    Unit::AuraList const& dummyAuras = caster->GetAurasByType(SPELL_AURA_DUMMY);
                    for (Unit::AuraList::const_iterator itr = dummyAuras.begin(); itr != dummyAuras.end(); ++itr)
                    {
                        if ((*itr)->GetId() == 34241)
                        {
                            m_modifier.m_amount += cp * (*itr)->GetModifier()->m_amount;
                            break;
                        }
                    }

                    if (cp > 4) cp = 4;
                    m_modifier.m_amount += int32(caster->GetTotalAttackPowerValue(BASE_ATTACK) * cp / 100);
                }
                return;
            }
            break;
        }
    case SPELLFAMILY_ROGUE:
        {
            // Deadly poison aura state
            if ((m_spellProto->SpellFamilyFlags & 0x10000) && m_spellProto->SpellVisual == 5100)
            {
                if (apply)
                    m_target->ModifyAuraState(AURA_STATE_DEADLY_POISON, true);
                else
                {
                    // current aura already removed, search present of another
                    bool found = false;
                    Unit::AuraList const& auras = m_target->GetAurasByType(SPELL_AURA_PERIODIC_DAMAGE);
                    for (Unit::AuraList::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
                    {
                        SpellEntry const* itr_spell = (*itr)->GetSpellProto();
                        if (itr_spell && itr_spell->SpellFamilyName == SPELLFAMILY_ROGUE && (itr_spell->SpellFamilyFlags & 0x10000) && itr_spell->SpellVisual == 5100)
                        {
                            found = true;
                            break;
                        }
                    }
                    // this has been last deadly poison aura
                    if (!found)
                        m_target->ModifyAuraState(AURA_STATE_DEADLY_POISON, false);
                }
                return;
            }
            // Rupture
            if (m_spellProto->SpellFamilyFlags & 0x000000000000100000LL)
            {
                // Dmg/tick = $AP*min(0.01*$cp, 0.03) [Like Rip: only the first three CP increase the contribution from AP]
                if (apply && !loading && caster && caster->GetTypeId() == TYPEID_PLAYER)
                {
                    uint8 cp = caster->ToPlayer()->GetComboPoints();
                    if (cp > 3) cp = 3;
                    m_modifier.m_amount += int32(caster->GetTotalAttackPowerValue(BASE_ATTACK) * cp / 100);
                }
                return;
            }
            // Garrote
            if (m_spellProto->SpellFamilyFlags & 0x000000000000000100LL)
            {
                // $AP*0.18/6 bonus per tick
                if (apply && !loading && caster)
                    m_modifier.m_amount += int32(caster->GetTotalAttackPowerValue(BASE_ATTACK) * 3 / 100);
                return;
            }
            break;
        }
    case SPELLFAMILY_HUNTER:
        {
            // Serpent Sting
            if (m_spellProto->SpellFamilyFlags & 0x0000000000004000LL)
            {
                // $RAP*0.1/5 bonus per tick
                if (apply && !loading && caster)
                    m_modifier.m_amount += int32(caster->GetTotalAttackPowerValue(RANGED_ATTACK) * 10 / 500);
                return;
            }
            // Immolation Trap
            if (m_spellProto->SpellFamilyFlags & 0x0000000000000004LL && m_spellProto->SpellIconID == 678)
            {
                // $RAP*0.1/5 bonus per tick
                if (apply && !loading && caster)
                    m_modifier.m_amount += int32(caster->GetTotalAttackPowerValue(RANGED_ATTACK) * 10 / 500);
                return;
            }
            break;
        }
    case SPELLFAMILY_PALADIN:
        {
            // Consecration
            if (m_spellProto->SpellFamilyFlags & 0x0000000000000020LL)
            {
                if (apply && !loading)
                {
                    if (Unit* caster = GetCaster())
                    {
                        Unit::AuraList const& classScripts = caster->GetAurasByType(SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
                        for (Unit::AuraList::const_iterator k = classScripts.begin(); k != classScripts.end(); ++k)
                        {
                            int32 tickcount = GetSpellDuration(m_spellProto) / m_spellProto->EffectAmplitude[m_effIndex];
                            switch ((*k)->GetModifier()->m_miscvalue)
                            {
                            case 5147:                  // Improved Consecration - Libram of the Eternal Rest
                                {
                                    m_modifier.m_amount += (*k)->GetModifier()->m_amount / tickcount;
                                    break;
                                }
                            }
                        }
                    }
                }
                return;
            }
            break;
        }
    default:
        break;
    }
}

void Aura::HandlePeriodicDamagePCT(bool apply, bool /*Real*/)
{
    if (m_periodicTimer <= 0)
        m_periodicTimer += m_modifier.periodictime;

    m_isPeriodic = apply;
}

void Aura::HandlePeriodicLeech(bool apply, bool /*Real*/)
{
    if (m_periodicTimer <= 0)
        m_periodicTimer += m_amplitude;

    m_isPeriodic = apply;
}

void Aura::HandlePeriodicManaLeech(bool apply, bool /*Real*/)
{
    if (m_periodicTimer <= 0)
        m_periodicTimer += m_amplitude;

    m_isPeriodic = apply;
}

/*********************************************************/
/***                 MODIFY STATS                     ***/
/*********************************************************/

/********************************/
/***       RESISTANCE        ***/
/********************************/

void Aura::HandleAuraModResistanceExclusive(bool apply, bool /*Real*/)
{
    for (int8 x = SPELL_SCHOOL_NORMAL; x < MAX_SPELL_SCHOOL; x++)
    {
        if (m_modifier.m_miscvalue & int32(1 << x))
        {
            m_target->HandleStatModifier(UnitMods(UNIT_MOD_RESISTANCE_START + x), BASE_VALUE, float(GetModifierValue()), apply);
            if (m_target->GetTypeId() == TYPEID_PLAYER)
                m_target->ApplyResistanceBuffModsMod(SpellSchools(x), m_positive, GetModifierValue(), apply);
        }
    }
}

void Aura::HandleAuraModResistance(bool apply, bool /*Real*/)
{
    for (int8 x = SPELL_SCHOOL_NORMAL; x < MAX_SPELL_SCHOOL; x++)
    {
        if (m_modifier.m_miscvalue & int32(1 << x))
        {
            m_target->HandleStatModifier(UnitMods(UNIT_MOD_RESISTANCE_START + x), TOTAL_VALUE, float(GetModifierValue()), apply);
            if (m_target->GetTypeId() == TYPEID_PLAYER || m_target->IsPet())
                m_target->ApplyResistanceBuffModsMod(SpellSchools(x), m_positive, GetModifierValue(), apply);
        }
    }

    // Faerie Fire (druid versions)
    if ((m_spellProto->SpellIconID == 109 &&
         m_spellProto->SpellFamilyName == SPELLFAMILY_DRUID &&
         m_spellProto->SpellFamilyFlags & 0x0000000000000400LL)
        || m_spellProto->Id == 35325)
        m_target->ModifyAuraState(AURA_STATE_FAERIE_FIRE, apply);
}

void Aura::HandleAuraModBaseResistancePCT(bool apply, bool /*Real*/)
{
    // only players have base stats
    if (m_target->GetTypeId() != TYPEID_PLAYER)
    {
        //pets only have base armor
        if (m_target->IsPet() && (m_modifier.m_miscvalue & SPELL_SCHOOL_MASK_NORMAL))
            m_target->HandleStatModifier(UNIT_MOD_ARMOR, BASE_PCT, float(GetModifierValue()), apply);
    }
    else
    {
        for (int8 x = SPELL_SCHOOL_NORMAL; x < MAX_SPELL_SCHOOL; x++)
        {
            if (m_modifier.m_miscvalue & int32(1 << x))
                m_target->HandleStatModifier(UnitMods(UNIT_MOD_RESISTANCE_START + x), BASE_PCT, float(GetModifierValue()), apply);
        }
    }
}

void Aura::HandleModResistancePercent(bool apply, bool /*Real*/)
{
    for (int8 i = SPELL_SCHOOL_NORMAL; i < MAX_SPELL_SCHOOL; i++)
    {
        if (m_modifier.m_miscvalue & int32(1 << i))
        {
            m_target->HandleStatModifier(UnitMods(UNIT_MOD_RESISTANCE_START + i), TOTAL_PCT, float(GetModifierValue()), apply);
            if (m_target->GetTypeId() == TYPEID_PLAYER || m_target->IsPet())
            {
                m_target->ApplyResistanceBuffModsPercentMod(SpellSchools(i), true, GetModifierValue(), apply);
                m_target->ApplyResistanceBuffModsPercentMod(SpellSchools(i), false, GetModifierValue(), apply);
            }
        }
    }
}

void Aura::HandleModBaseResistance(bool apply, bool /*Real*/)
{
    // only players have base stats
    if (m_target->GetTypeId() != TYPEID_PLAYER)
    {
        //only pets have base stats
        if (m_target->IsPet() && (m_modifier.m_miscvalue & SPELL_SCHOOL_MASK_NORMAL))
            m_target->HandleStatModifier(UNIT_MOD_ARMOR, TOTAL_VALUE, float(GetModifierValue()), apply);
    }
    else
    {
        for (int i = SPELL_SCHOOL_NORMAL; i < MAX_SPELL_SCHOOL; i++)
            if (m_modifier.m_miscvalue & (1 << i))
                m_target->HandleStatModifier(UnitMods(UNIT_MOD_RESISTANCE_START + i), TOTAL_VALUE, float(GetModifierValue()), apply);
    }
}

/********************************/
/***          STAT           ***/
/********************************/

void Aura::HandleAuraModStat(bool apply, bool /*Real*/)
{
    if (m_modifier.m_miscvalue < -2 || m_modifier.m_miscvalue > 4)
    {
        sLog.outError("WARNING: Spell %u effect %u has unsupported misc value (%i) for SPELL_AURA_MOD_STAT ", GetId(), GetEffIndex(), m_modifier.m_miscvalue);
        return;
    }

    if (apply && GetId() == 20007) // Crusader enchant proc: Holy Strength
    {
        uint32 lvldiff = m_target->getLevel() - 60;
        uint32 diff = lvldiff > 0 ? lvldiff * 4 : 0;
        m_modifier.m_amount = GetModifierValue() - diff;
    }

    for (int32 i = STAT_STRENGTH; i < MAX_STATS; i++)
    {
        // -1 or -2 is all stats (misc < -2 checked in function beginning)
        if (m_modifier.m_miscvalue < 0 || m_modifier.m_miscvalue == i)
        {
            //m_target->ApplyStatMod(Stats(i), m_modifier.m_amount,apply);
            m_target->HandleStatModifier(UnitMods(UNIT_MOD_STAT_START + i), TOTAL_VALUE, float(GetModifierValue()), apply);
            if (m_target->GetTypeId() == TYPEID_PLAYER || m_target->IsPet())
                m_target->ApplyStatBuffMod(Stats(i), (float)GetModifierValue(), apply);
        }
    }
}

void Aura::HandleModPercentStat(bool apply, bool /*Real*/)
{
    if (m_modifier.m_miscvalue < -1 || m_modifier.m_miscvalue > 4)
    {
        sLog.outError("WARNING: Misc Value for SPELL_AURA_MOD_PERCENT_STAT not valid");
        return;
    }

    // only players have base stats
    if (m_target->GetTypeId() != TYPEID_PLAYER)
        return;

    for (int32 i = STAT_STRENGTH; i < MAX_STATS; ++i)
    {
        if (m_modifier.m_miscvalue == i || m_modifier.m_miscvalue == -1)
            m_target->HandleStatModifier(UnitMods(UNIT_MOD_STAT_START + i), BASE_PCT, float(GetModifierValue()), apply);
    }
}

void Aura::HandleModSpellDamagePercentFromStat(bool /*apply*/, bool /*Real*/)
{
    if (m_target->GetTypeId() != TYPEID_PLAYER)
        return;

    // Magic damage modifiers implemented in Unit::SpellDamageBonus
    // This information for client side use only
    // Recalculate bonus
    m_target->ToPlayer()->UpdateSpellDamageAndHealingBonus();
}

void Aura::HandleModSpellHealingPercentFromStat(bool /*apply*/, bool /*Real*/)
{
    if (m_target->GetTypeId() != TYPEID_PLAYER)
        return;

    // Recalculate bonus
    m_target->ToPlayer()->UpdateSpellDamageAndHealingBonus();
}

void Aura::HandleAuraModDispelResist(bool apply, bool Real)
{
    if (!Real || !apply)
        return;

    if (GetId() == 33206)
        m_target->CastSpell(m_target, 44416, true, NULL, this, GetCasterGUID());
}

void Aura::HandleModSpellDamagePercentFromAttackPower(bool /*apply*/, bool /*Real*/)
{
    if (m_target->GetTypeId() != TYPEID_PLAYER)
        return;

    // Magic damage modifiers implemented in Unit::SpellDamageBonus
    // This information for client side use only
    // Recalculate bonus
    m_target->ToPlayer()->UpdateSpellDamageAndHealingBonus();
}

void Aura::HandleModSpellHealingPercentFromAttackPower(bool /*apply*/, bool /*Real*/)
{
    if (m_target->GetTypeId() != TYPEID_PLAYER)
        return;

    // Recalculate bonus
    m_target->ToPlayer()->UpdateSpellDamageAndHealingBonus();
}

void Aura::HandleModHealingDone(bool /*apply*/, bool /*Real*/)
{
    if (m_target->GetTypeId() != TYPEID_PLAYER)
        return;
    // implemented in Unit::SpellHealingBonus
    // this information is for client side only
    m_target->ToPlayer()->UpdateSpellDamageAndHealingBonus();
}

void Aura::HandleModTotalPercentStat(bool apply, bool /*Real*/)
{
    if (m_modifier.m_miscvalue < -1 || m_modifier.m_miscvalue > 4)
    {
        sLog.outError("WARNING: Misc Value for SPELL_AURA_MOD_PERCENT_STAT not valid");
        return;
    }

    // save current health state
    float healthPct = m_target->GetHealthPct();
    bool alive = m_target->IsAlive();

    for (int32 i = STAT_STRENGTH; i < MAX_STATS; ++i)
    {
        if (m_modifier.m_miscvalue == i || m_modifier.m_miscvalue == -1)
        {
            m_target->HandleStatModifier(UnitMods(UNIT_MOD_STAT_START + i), TOTAL_PCT, float(GetModifierValue()), apply);
            if (m_target->GetTypeId() == TYPEID_PLAYER || m_target->IsPet())
                m_target->ApplyStatPercentBuffMod(Stats(i), float(GetModifierValue()), apply);
        }
    }

    //recalculate current HP/MP after applying aura modifications (only for spells with 0x10 flag)
    if ((m_modifier.m_miscvalue == STAT_STAMINA) && (m_spellProto->Attributes & SPELL_ATTR0_ABILITY))
        m_target->SetHealth(std::max<uint32>(uint32(healthPct * m_target->GetMaxHealth() * 0.01f), (alive ? 1 : 0)));
}

void Aura::HandleAuraModResistenceOfStatPercent(bool /*apply*/, bool /*Real*/)
{
    if (m_target->GetTypeId() != TYPEID_PLAYER)
        return;

    if (m_modifier.m_miscvalue != SPELL_SCHOOL_MASK_NORMAL)
    {
        // support required adding replace UpdateArmor by loop by UpdateResistence at intellect update
        // and include in UpdateResistence same code as in UpdateArmor for aura mod apply.
        sLog.outError("Aura SPELL_AURA_MOD_RESISTANCE_OF_STAT_PERCENT(182) need adding support for non-armor resistances!");
        return;
    }

    // Recalculate Armor
    m_target->UpdateArmor();
}

/********************************/
/***     HEAL & ENERGIZE     ***/
/********************************/
void Aura::HandleAuraModTotalHealthPercentRegen(bool apply, bool /*Real*/)
{
    /*
    Need additional checking for auras who reduce or increase healing, magic effect like Dumpen Magic,
    so this aura not fully working.
    */
    if (apply)
    {
        if (!m_target->IsAlive())
            return;

        if ((GetSpellProto()->AuraInterruptFlags & AURA_INTERRUPT_FLAG_NOT_SEATED) && !m_target->IsSitState())
            m_target->SetStandState(UNIT_STAND_STATE_SIT);

        if (m_periodicTimer <= 0)
        {
            m_periodicTimer += m_amplitude;

            if (m_target->GetHealth() < m_target->GetMaxHealth())
            {
                // PeriodicTick can cast triggered spells with stats changes
                PeriodicTick();
            }
        }
    }

    m_isPeriodic = apply;
}

void Aura::HandleAuraModTotalManaPercentRegen(bool apply, bool /*Real*/)
{
    if ((GetSpellProto()->AuraInterruptFlags & AURA_INTERRUPT_FLAG_NOT_SEATED) && apply  && !m_target->IsSitState())
        m_target->SetStandState(UNIT_STAND_STATE_SIT);
    if (apply)
    {
        if (m_modifier.periodictime == 0)
            m_modifier.periodictime = 1000;
        if (m_periodicTimer <= 0 && m_target->getPowerType() == POWER_MANA)
        {
            m_periodicTimer += m_amplitude > 0 ? m_amplitude : 1000;

            if (m_target->GetPower(POWER_MANA) < m_target->GetMaxPower(POWER_MANA))
            {
                // PeriodicTick can cast triggered spells with stats changes
                PeriodicTick();
            }
        }
    }

    m_isPeriodic = apply;
}

void Aura::HandleModRegen(bool apply, bool /*Real*/)        // eating
{
    if (apply)
    {
        if (!m_target->IsAlive())
            return;

        if ((GetSpellProto()->AuraInterruptFlags & AURA_INTERRUPT_FLAG_NOT_SEATED)  && !m_target->IsSitState())
            m_target->SetStandState(UNIT_STAND_STATE_SIT);

        if (m_periodicTimer <= 0)
        {
            m_periodicTimer += 5000;
            int32 gain = m_target->ModifyHealth(GetModifierValue());
        }
    }

    m_isPeriodic = apply;
}

void Aura::HandleModPowerRegen(bool apply, bool Real)       // drinking
{
    if ((GetSpellProto()->AuraInterruptFlags & AURA_INTERRUPT_FLAG_NOT_SEATED) && apply && !m_target->IsSitState())
        m_target->SetStandState(UNIT_STAND_STATE_SIT);

    if (apply && m_periodicTimer <= 0)
    {
        m_periodicTimer = 2000;

        Powers pt = m_target->getPowerType();
        if (int32(pt) != m_modifier.m_miscvalue)
            return;

        if (GetId() == 20577)
        {
            // cannibalize anim
            m_target->HandleEmoteCommand(398);
        }

        // Warrior talent, gain 1 rage every 3 seconds while in combat
        // Anger Menagement
        // amount = 1+ 16 = 17 = 3,4*5 = 10,2*5/3
        // so 17 is rounded amount for 5 sec tick grow ~ 1 range grow in 3 sec
        if (pt == POWER_RAGE)
        {
            if (m_target->IsInCombat())
                m_target->ModifyPower(pt, m_modifier.m_amount * 3 / 5);

            m_periodicTimer += 1000;
        }
    }
    m_isPeriodic = apply;
    if (Real && m_target->GetTypeId() == TYPEID_PLAYER && m_modifier.m_miscvalue == POWER_MANA)
        m_target->ToPlayer()->UpdateManaRegen();
}

void Aura::HandleModPowerRegenPCT(bool /*apply*/, bool Real)
{
    // spells required only Real aura add/remove
    if (!Real)
        return;

    if (m_target->GetTypeId() != TYPEID_PLAYER)
        return;

    // Update manaregen value
    if (m_modifier.m_miscvalue == POWER_MANA)
        m_target->ToPlayer()->UpdateManaRegen();
}

void Aura::HandleModManaRegen(bool /*apply*/, bool Real)
{
    // spells required only Real aura add/remove
    if (!Real)
        return;

    if (m_target->GetTypeId() != TYPEID_PLAYER)
        return;

    //Note: an increase in regen does NOT cause threat.
    m_target->ToPlayer()->UpdateManaRegen();
}

void Aura::HandleComprehendLanguage(bool apply, bool /*Real*/)
{
    if (apply)
        m_target->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_COMPREHEND_LANG);
    else
    {
        if (m_target->HasAuraType(GetAuraType()))
            return;

        m_target->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_COMPREHEND_LANG);
    }
}

void Aura::HandleAuraModIncreaseHealth(bool apply, bool Real)
{
    if (!Real)
        return;

    if (apply)
    {
        m_target->HandleStatModifier(UNIT_MOD_HEALTH, TOTAL_VALUE, float(GetModifierValue()), apply);
        m_target->ModifyHealth(m_modifier.m_amount);
    }
    else
    {
        if (m_target->GetHealth() > 0)
        {
            int32 value = std::min<int32>(m_target->GetHealth() - 1, GetAmount());
            m_target->ModifyHealth(-value);
        }
        m_target->HandleStatModifier(UNIT_MOD_HEALTH, TOTAL_VALUE, float(GetModifierValue()), apply);
    }
}

void  Aura::HandleAuraModIncreaseMaxHealth(bool apply, bool /*Real*/)
{
    float percent = m_target->GetHealthPct();

    m_target->HandleStatModifier(UNIT_MOD_HEALTH, TOTAL_VALUE, float(m_modifier.m_amount), apply);

    // refresh percentage
    if (m_target->GetHealth() > 0)
    {
        uint32 newHealth = std::max<uint32>(m_target->CountPctFromMaxHealth(int32(percent)), 1);
        m_target->SetHealth(newHealth);
    }
}

void Aura::HandleAuraModIncreaseEnergy(bool apply, bool /*Real*/)
{
    Powers powerType = Powers(GetMiscValue());
    UnitMods unitMod = UnitMods(UNIT_MOD_POWER_START + powerType);

    m_target->HandleStatModifier(unitMod, TOTAL_VALUE, float(GetModifierValue()), apply);
}

void Aura::HandleAuraModIncreaseEnergyPercent(bool apply, bool /*Real*/)
{
    Powers powerType = Powers(GetMiscValue());
    UnitMods unitMod = UnitMods(UNIT_MOD_POWER_START + powerType);

    m_target->HandleStatModifier(unitMod, TOTAL_PCT, float(GetModifierValue()), apply);
}

void Aura::HandleAuraModIncreaseHealthPercent(bool apply, bool /*Real*/)
{
    // Unit will keep hp% after MaxHealth being modified if unit is alive.
    float percent = m_target->GetHealthPct();
    m_target->HandleStatModifier(UNIT_MOD_HEALTH, TOTAL_PCT, float(GetModifierValue()), apply);

    if (m_target->GetHealth() > 0)
    {
        uint32 newHealth = std::max<uint32>(m_target->CountPctFromMaxHealth(int32(percent)), 1);
        m_target->SetHealth(newHealth);
    }
}

/********************************/
/***         FIGHT           ***/
/********************************/

void Aura::HandleAuraModParryPercent(bool /*apply*/, bool /*Real*/)
{
    if (m_target->GetTypeId() != TYPEID_PLAYER)
        return;

    m_target->ToPlayer()->UpdateParryPercentage();
}

void Aura::HandleAuraModDodgePercent(bool /*apply*/, bool /*Real*/)
{
    if (m_target->GetTypeId() != TYPEID_PLAYER)
        return;

    m_target->ToPlayer()->UpdateDodgePercentage();
}

void Aura::HandleAuraModBlockPercent(bool /*apply*/, bool /*Real*/)
{
    if (m_target->GetTypeId() != TYPEID_PLAYER)
        return;

    m_target->ToPlayer()->UpdateBlockPercentage();
}

void Aura::HandleAuraModRegenInterrupt(bool /*apply*/, bool Real)
{
    // spells required only Real aura add/remove
    if (!Real)
        return;

    if (m_target->GetTypeId() != TYPEID_PLAYER)
        return;

    m_target->ToPlayer()->UpdateManaRegen();
}

void Aura::HandleAuraModCritPercent(bool apply, bool Real)
{
    if (m_target->GetTypeId() != TYPEID_PLAYER)
        return;

    // apply item specific bonuses for already equipped weapon
    if (Real)
    {
        for (int i = 0; i < MAX_ATTACK; ++i)
            if (Item* pItem = m_target->ToPlayer()->GetWeaponForAttack(WeaponAttackType(i), true))
                m_target->ToPlayer()->_ApplyWeaponDependentAuraCritMod(pItem, WeaponAttackType(i), this, apply);
    }

    // mods must be applied base at equipped weapon class and subclass comparison
    // with spell->EquippedItemClass and  EquippedItemSubClassMask and EquippedItemInventoryTypeMask
    // m_modifier.m_miscvalue comparison with item generated damage types

    if (GetSpellProto()->EquippedItemClass == -1)
    {
        m_target->ToPlayer()->HandleBaseModValue(CRIT_PERCENTAGE,         FLAT_MOD, float (GetModifierValue()), apply);
        m_target->ToPlayer()->HandleBaseModValue(OFFHAND_CRIT_PERCENTAGE, FLAT_MOD, float (GetModifierValue()), apply);
        m_target->ToPlayer()->HandleBaseModValue(RANGED_CRIT_PERCENTAGE,  FLAT_MOD, float (GetModifierValue()), apply);
    }
    else
    {
        // done in Player::_ApplyWeaponDependentAuraMods
    }
}

void Aura::HandleModHitChance(bool apply, bool /*Real*/)
{
    if (m_target->GetTypeId() == TYPEID_PLAYER)
    {
        m_target->ToPlayer()->UpdateMeleeHitChances();
        m_target->ToPlayer()->UpdateRangedHitChances();
    }
    else
    {
        m_target->m_modMeleeHitChance += apply ? GetModifierValue() : -GetModifierValue();
        m_target->m_modRangedHitChance += apply ? GetModifierValue() : -GetModifierValue();
    }
}

void Aura::HandleModSpellHitChance(bool apply, bool /*Real*/)
{
    if (m_target->GetTypeId() == TYPEID_PLAYER)
        m_target->ToPlayer()->UpdateSpellHitChances();
    else
        m_target->m_modSpellHitChance += apply ? GetModifierValue() : -GetModifierValue();
}

void Aura::HandleModSpellCritChance(bool apply, bool Real)
{
    // spells required only Real aura add/remove
    if (!Real)
        return;

    if (m_target->GetTypeId() == TYPEID_PLAYER)
        m_target->ToPlayer()->UpdateAllSpellCritChances();
    else
        m_target->m_baseSpellCritChance += apply ? GetModifierValue() : -GetModifierValue();
}

void Aura::HandleModSpellCritChanceShool(bool /*apply*/, bool Real)
{
    // spells required only Real aura add/remove
    if (!Real)
        return;

    if (m_target->GetTypeId() != TYPEID_PLAYER)
        return;

    for (int school = SPELL_SCHOOL_NORMAL; school < MAX_SPELL_SCHOOL; ++school)
        if (m_modifier.m_miscvalue & (1 << school))
            m_target->ToPlayer()->UpdateSpellCritChance(school);
}

/********************************/
/***        ATTACK SPEED     ***/
/********************************/

void Aura::HandleModCastingSpeed(bool apply, bool /*Real*/)
{
    m_target->ApplyCastTimePercentMod((float)GetModifierValue(), apply);
}

void Aura::HandleModMeleeRangedSpeedPct(bool apply, bool /*Real*/)
{
    m_target->ApplyAttackTimePercentMod(BASE_ATTACK, (float)GetModifierValue(), apply);
    m_target->ApplyAttackTimePercentMod(OFF_ATTACK, (float)GetModifierValue(), apply);
    m_target->ApplyAttackTimePercentMod(RANGED_ATTACK, (float)GetModifierValue(), apply);
}

void Aura::HandleModCombatSpeedPct(bool apply, bool /*Real*/)
{
    m_target->ApplyCastTimePercentMod((float)GetModifierValue(), apply);
    m_target->ApplyAttackTimePercentMod(BASE_ATTACK, (float)GetModifierValue(), apply);
    m_target->ApplyAttackTimePercentMod(OFF_ATTACK, (float)GetModifierValue(), apply);
    m_target->ApplyAttackTimePercentMod(RANGED_ATTACK, (float)GetModifierValue(), apply);
}

void Aura::HandleModAttackSpeed(bool apply, bool /*Real*/)
{
    m_target->ApplyAttackTimePercentMod(BASE_ATTACK, (float)GetModifierValue(), apply);
    m_target->UpdateDamagePhysical(BASE_ATTACK);
}

void Aura::HandleHaste(bool apply, bool /*Real*/)
{
    m_target->ApplyAttackTimePercentMod(BASE_ATTACK,  (float)GetModifierValue(), apply);
    m_target->ApplyAttackTimePercentMod(OFF_ATTACK,   (float)GetModifierValue(), apply);
    m_target->ApplyAttackTimePercentMod(RANGED_ATTACK, (float)GetModifierValue(), apply);
}

void Aura::HandleAuraModRangedHaste(bool apply, bool /*Real*/)
{
    m_target->ApplyAttackTimePercentMod(RANGED_ATTACK, (float)GetModifierValue(), apply);
}

void Aura::HandleRangedAmmoHaste(bool apply, bool /*Real*/)
{
    if (m_target->GetTypeId() != TYPEID_PLAYER)
        return;

    m_target->ApplyAttackTimePercentMod(RANGED_ATTACK, (float)GetModifierValue(), apply);
}

/********************************/
/***       ATTACK POWER      ***/
/********************************/

void Aura::HandleAuraModAttackPower(bool apply, bool /*Real*/)
{
    m_target->HandleStatModifier(UNIT_MOD_ATTACK_POWER, TOTAL_VALUE, float(GetModifierValue()), apply);
    if (apply)
        switch (m_spellProto->Id)
        {
            // Warrior & Druid Demoshout should remove stealth
            case 1160:
            case 6190:
            case 11554:
            case 11555:
            case 11556:
            case 25202:
            case 25203:
            case 99:
            case 1735:
            case 9490:
            case 9747:
            case 9898:
            case 26998:
                m_target->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);
                break;
        }
}

void Aura::HandleAuraModRangedAttackPower(bool apply, bool /*Real*/)
{
    if ((m_target->getClassMask() & CLASSMASK_WAND_USERS) != 0)
        return;

    m_target->HandleStatModifier(UNIT_MOD_ATTACK_POWER_RANGED, TOTAL_VALUE, float(GetModifierValue()), apply);
}

void Aura::HandleAuraModAttackPowerPercent(bool apply, bool /*Real*/)
{
    //UNIT_FIELD_ATTACK_POWER_MULTIPLIER = multiplier - 1
    m_target->HandleStatModifier(UNIT_MOD_ATTACK_POWER, TOTAL_PCT, float(GetModifierValue()), apply);
}

void Aura::HandleAuraModRangedAttackPowerPercent(bool apply, bool /*Real*/)
{
    if ((m_target->getClassMask() & CLASSMASK_WAND_USERS) != 0)
        return;

    //UNIT_FIELD_RANGED_ATTACK_POWER_MULTIPLIER = multiplier - 1
    m_target->HandleStatModifier(UNIT_MOD_ATTACK_POWER_RANGED, TOTAL_PCT, float(GetModifierValue()), apply);
}

void Aura::HandleAuraModRangedAttackPowerOfStatPercent(bool /*apply*/, bool Real)
{
    // spells required only Real aura add/remove
    if (!Real)
        return;

    if (m_modifier.m_miscvalue != STAT_INTELLECT)
    {
        // support required adding UpdateAttackPowerAndDamage calls at stat update
        sLog.outError("Aura SPELL_AURA_MOD_RANGED_ATTACK_POWER_OF_STAT_PERCENT (212) need support non-intellect stats!");
        return;
    }

    // Recalculate bonus
    if (m_target->GetTypeId() == TYPEID_PLAYER && !(m_target->getClassMask() & CLASSMASK_WAND_USERS))
        m_target->ToPlayer()->UpdateAttackPowerAndDamage(true);
}

/********************************/
/***       DAMAGE BONUS      ***/
/********************************/
void Aura::HandleModDamageDone(bool apply, bool Real)
{
    // apply item specific bonuses for already equipped weapon
    if (Real && m_target->GetTypeId() == TYPEID_PLAYER)
    {
        for (int i = 0; i < MAX_ATTACK; ++i)
            if (Item* pItem = m_target->ToPlayer()->GetWeaponForAttack(WeaponAttackType(i), true))
                m_target->ToPlayer()->_ApplyWeaponDependentAuraDamageMod(pItem, WeaponAttackType(i), this, apply);
    }

    // m_modifier.m_miscvalue is bitmask of spell schools
    // 1 (0-bit) - normal school damage (SPELL_SCHOOL_MASK_NORMAL)
    // 126 - full bitmask all magic damages (SPELL_SCHOOL_MASK_MAGIC) including wands
    // 127 - full bitmask any damages
    //
    // mods must be applied base at equipped weapon class and subclass comparison
    // with spell->EquippedItemClass and  EquippedItemSubClassMask and EquippedItemInventoryTypeMask
    // m_modifier.m_miscvalue comparison with item generated damage types

    if ((m_modifier.m_miscvalue & SPELL_SCHOOL_MASK_NORMAL) != 0)
    {
        // apply generic physical damage bonuses including wand case
        if (GetSpellProto()->EquippedItemClass == -1 || m_target->GetTypeId() != TYPEID_PLAYER)
        {
            m_target->HandleStatModifier(UNIT_MOD_DAMAGE_MAINHAND, TOTAL_VALUE, float(GetModifierValue()), apply);
            m_target->HandleStatModifier(UNIT_MOD_DAMAGE_OFFHAND, TOTAL_VALUE, float(GetModifierValue()), apply);
            m_target->HandleStatModifier(UNIT_MOD_DAMAGE_RANGED, TOTAL_VALUE, float(GetModifierValue()), apply);

            if (m_target->GetTypeId() == TYPEID_PLAYER)
            {
                if (m_positive)
                    m_target->ApplyModUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS, GetModifierValue(), apply);
                else
                    m_target->ApplyModUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_NEG, GetModifierValue(), apply);
            }
        }
        else
        {
            // done in Player::_ApplyWeaponDependentAuraMods
        }
    }

    // Skip non magic case for speedup
    if ((m_modifier.m_miscvalue & SPELL_SCHOOL_MASK_MAGIC) == 0)
        return;

    if (GetSpellProto()->EquippedItemClass != -1 || GetSpellProto()->EquippedItemInventoryTypeMask != 0)
    {
        // wand magic case (skip generic to all item spell bonuses)
        // done in Player::_ApplyWeaponDependentAuraMods

        // Skip item specific requirements for not wand magic damage
        return;
    }

    // Magic damage modifiers implemented in Unit::SpellDamageBonus
    // This information for client side use only
    if (m_target->GetTypeId() == TYPEID_PLAYER)
    {
        if (m_positive)
        {
            for (int i = SPELL_SCHOOL_HOLY; i < MAX_SPELL_SCHOOL; i++)
            {
                if ((m_modifier.m_miscvalue & (1 << i)) != 0)
                    m_target->ApplyModUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + i, GetModifierValue(), apply);
            }
        }
        else
        {
            for (int i = SPELL_SCHOOL_HOLY; i < MAX_SPELL_SCHOOL; i++)
            {
                if ((m_modifier.m_miscvalue & (1 << i)) != 0)
                    m_target->ApplyModUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_NEG + i, GetModifierValue(), apply);
            }
        }
        Pet* pet = m_target->ToPlayer()->GetPet();
        if (pet)
            pet->UpdateAttackPowerAndDamage();
    }
}

void Aura::HandleModDamagePercentDone(bool apply, bool Real)
{
    DEBUG_LOG("AURA MOD DAMAGE type:%u negative:%u", m_modifier.m_miscvalue, m_positive ? 0 : 1);

    // apply item specific bonuses for already equipped weapon
    if (Real && m_target->GetTypeId() == TYPEID_PLAYER)
    {
        for (int i = 0; i < MAX_ATTACK; ++i)
            if (Item* pItem = m_target->ToPlayer()->GetWeaponForAttack(WeaponAttackType(i), true))
                m_target->ToPlayer()->_ApplyWeaponDependentAuraDamageMod(pItem, WeaponAttackType(i), this, apply);
    }

    // m_modifier.m_miscvalue is bitmask of spell schools
    // 1 (0-bit) - normal school damage (SPELL_SCHOOL_MASK_NORMAL)
    // 126 - full bitmask all magic damages (SPELL_SCHOOL_MASK_MAGIC) including wand
    // 127 - full bitmask any damages
    //
    // mods must be applied base at equipped weapon class and subclass comparison
    // with spell->EquippedItemClass and  EquippedItemSubClassMask and EquippedItemInventoryTypeMask
    // m_modifier.m_miscvalue comparison with item generated damage types

    if ((m_modifier.m_miscvalue & SPELL_SCHOOL_MASK_NORMAL) != 0)
    {
        // apply generic physical damage bonuses including wand case
        if (GetSpellProto()->EquippedItemClass == -1 || m_target->GetTypeId() != TYPEID_PLAYER)
        {
            m_target->HandleStatModifier(UNIT_MOD_DAMAGE_MAINHAND, TOTAL_PCT, float(GetModifierValue()), apply);
            m_target->HandleStatModifier(UNIT_MOD_DAMAGE_OFFHAND, TOTAL_PCT, float(GetModifierValue()), apply);
            m_target->HandleStatModifier(UNIT_MOD_DAMAGE_RANGED, TOTAL_PCT, float(GetModifierValue()), apply);

            // For show in client
            if (m_target->GetTypeId() == TYPEID_PLAYER)
                m_target->ApplyModSignedFloatValue(PLAYER_FIELD_MOD_DAMAGE_DONE_PCT, m_modifier.m_amount / 100.0f, apply);
        }
        else
        {
            // done in Player::_ApplyWeaponDependentAuraMods
        }
    }

    // Skip non magic case for speedup
    if ((m_modifier.m_miscvalue & SPELL_SCHOOL_MASK_MAGIC) == 0)
        return;

    if (GetSpellProto()->EquippedItemClass != -1 || GetSpellProto()->EquippedItemInventoryTypeMask != 0)
    {
        // wand magic case (skip generic to all item spell bonuses)
        // done in Player::_ApplyWeaponDependentAuraMods

        // Skip item specific requirements for not wand magic damage
        return;
    }

    // Magic damage percent modifiers implemented in Unit::SpellDamageBonus
    // Send info to client
    if (m_target->GetTypeId() == TYPEID_PLAYER)
        for (int i = SPELL_SCHOOL_HOLY; i < MAX_SPELL_SCHOOL; ++i)
            m_target->ApplyModSignedFloatValue(PLAYER_FIELD_MOD_DAMAGE_DONE_PCT + i, m_modifier.m_amount / 100.0f, apply);
}

void Aura::HandleModOffhandDamagePercent(bool apply, bool Real)
{
    // spells required only Real aura add/remove
    if (!Real)
        return;

    m_target->HandleStatModifier(UNIT_MOD_DAMAGE_OFFHAND, TOTAL_PCT, float(GetModifierValue()), apply);
}

/********************************/
/***       POWER COST        ***/
/********************************/

void Aura::HandleModPowerCostPCT(bool apply, bool Real)
{
    // spells required only Real aura add/remove
    if (!Real)
        return;

    float amount = CalculatePct(1.0f, GetModifierValue());
    for (int i = 0; i < MAX_SPELL_SCHOOL; ++i)
        if (m_modifier.m_miscvalue & (1 << i))
            m_target->ApplyModSignedFloatValue(UNIT_FIELD_POWER_COST_MULTIPLIER + i, amount, apply);
}

void Aura::HandleModPowerCost(bool apply, bool Real)
{
    // spells required only Real aura add/remove
    if (!Real)
        return;

    for (int i = 0; i < MAX_SPELL_SCHOOL; ++i)
        if (m_modifier.m_miscvalue & (1 << i))
            m_target->ApplyModInt32Value(UNIT_FIELD_POWER_COST_MODIFIER + i, GetModifierValue(), apply);
}

/*********************************************************/
/***                   OTHERS                         ***/
/*********************************************************/

void Aura::HandleShapeshiftBoosts(bool apply)
{
    uint32 spellId = 0;
    uint32 spellId2 = 0;
    uint32 HotWSpellId = 0;

    switch (GetModifier()->m_miscvalue)
    {
        case FORM_CAT:
            spellId = 3025;
            if (m_target->HasSpell(5225) && !m_target->GetUInt32Value(PLAYER_TRACK_CREATURES) && !m_target->GetUInt32Value(PLAYER_TRACK_RESOURCES))
                spellId2 = 5225; // Track Humanoids
            HotWSpellId = 24900;
            break;
        case FORM_TREE:
            spellId = 5420;
            break;
        case FORM_TRAVEL:
            spellId = 5419;
            break;
        case FORM_AQUA:
            spellId = 5421;
            break;
        case FORM_BEAR:
            spellId = 1178;
            spellId2 = 21178;
            HotWSpellId = 24899;
            break;
        case FORM_DIREBEAR:
            spellId = 9635;
            spellId2 = 21178;
            HotWSpellId = 24899;
            break;
        case FORM_BATTLESTANCE:
            spellId = 21156;
            break;
        case FORM_DEFENSIVESTANCE:
            spellId = 7376;
            break;
        case FORM_BERSERKERSTANCE:
            spellId = 7381;
            break;
        case FORM_MOONKIN:
            spellId = 24905;
            // aura from effect trigger spell
            spellId2 = 24907;
            break;
        case FORM_FLIGHT:
            spellId = 33948;
            spellId2 = 34764;
            break;
        case FORM_FLIGHT_EPIC:
            spellId  = 40122;
            spellId2 = 40121;
            break;
        case FORM_SPIRITOFREDEMPTION:
            spellId  = 27792;
            spellId2 = 27795;                               // must be second, this important at aura remove to prevent to early iterator invalidation.
            break;
        case FORM_GHOSTWOLF:
        case FORM_AMBIENT:
        case FORM_GHOUL:
        case FORM_SHADOW:
        case FORM_STEALTH:
        case FORM_CREATURECAT:
        case FORM_CREATUREBEAR:
            break;
        default:
            break;
    }

    uint32 form = GetModifier()->m_miscvalue - 1;

    if (apply)
    {
        if (m_target->GetTypeId() == TYPEID_PLAYER)
        {
            // Remove cooldown of spells triggered on stance change - they may share cooldown with stance spell
            if (spellId)
            {
                m_target->ToPlayer()->RemoveSpellCooldown(spellId);
                m_target->CastSpell(m_target, spellId, true, NULL, this);
            }

            if (spellId2)
            {
                m_target->ToPlayer()->RemoveSpellCooldown(spellId2);
                m_target->CastSpell(m_target, spellId2, true, NULL, this);
            }

            const PlayerSpellMap& sp_list = m_target->ToPlayer()->GetSpellMap();
            for (PlayerSpellMap::const_iterator itr = sp_list.begin(); itr != sp_list.end(); ++itr)
            {
                if (itr->second.state == PLAYERSPELL_REMOVED || itr->second.disabled)
                    continue;

                if (itr->first == spellId || itr->first == spellId2)
                    continue;

                SpellEntry const* spellInfo = sSpellStore.LookupEntry(itr->first);
                if (!spellInfo || !(spellInfo->Attributes & (SPELL_ATTR0_PASSIVE | SPELL_ATTR0_HIDDEN_CLIENTSIDE)))
                    continue;

                if (spellInfo->Stances & (1 << form))
                    m_target->CastSpell(m_target, itr->first, true, NULL, this);
            }

            // Leader of the Pack
            if (m_target->ToPlayer()->HasSpell(17007))
            {
                SpellEntry const* spellInfo = sSpellStore.LookupEntry(24932);
                if (spellInfo && spellInfo->Stances & (1 << form))
                    m_target->CastSpell(m_target, 24932, true, NULL, this);
            }
            // Heart of the Wild
            if (HotWSpellId)
            {
                Unit::AuraList const& mModTotalStatPct = m_target->GetAurasByType(SPELL_AURA_MOD_TOTAL_STAT_PERCENTAGE);
                for (Unit::AuraList::const_iterator i = mModTotalStatPct.begin(); i != mModTotalStatPct.end(); ++i)
                {
                    // Heart of the Wild
                    if ((*i)->GetSpellProto()->SpellIconID == 240 && (*i)->GetModifier()->m_miscvalue == 3)
                    {
                        int32 HotWMod = (*i)->GetModifier()->m_amount; // For each 2% Intelligence, you get 1% stamina and 2% attack power, in cat form.
                        if (GetModifier()->m_miscvalue == FORM_CAT)
                            HotWMod /= 2;

                        m_target->CastCustomSpell(m_target, HotWSpellId, &HotWMod, NULL, NULL, true, NULL, this);
                        break;
                    }
                }
            }
        }
    }
    else
    {
        m_target->RemoveAurasDueToSpell(spellId);
        m_target->RemoveAurasDueToSpell(spellId2);

        Unit::AuraMap& tAuras = m_target->GetAuras();
        for (Unit::AuraMap::iterator itr = tAuras.begin(); itr != tAuras.end();)
        {
            if (itr->second->IsRemovedOnShapeLost())
            {
                m_target->RemoveAurasDueToSpell(itr->second->GetId());
                itr = tAuras.begin();
            }
            else
                ++itr;
        }
    }
}

void Aura::HandleAuraEmpathy(bool apply, bool /*Real*/)
{
    if (m_target->GetTypeId() != TYPEID_UNIT)
        return;

    if (!apply)
    {
        // do not remove unit flag if there are more than this auraEffect of that kind on unit on unit
        if (m_target->HasAuraType(GetAuraType()))
            return;
    }

    if (m_target->GetCreatureType() == CREATURE_TYPE_BEAST)
        m_target->ApplyModUInt32Value(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_SPECIALINFO, apply);
}

void Aura::HandleAuraUntrackable(bool apply, bool /*Real*/)
{
    if (apply)
        m_target->SetByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_UNTRACKABLE);
    else
    {
        // do not remove unit flag if there are more than this auraEffect of that kind on unit
        if (m_target->HasAuraType(GetAuraType()))
            return;
        m_target->RemoveByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_UNTRACKABLE);
    }
}

void Aura::HandleAuraModPacify(bool apply, bool /*Real*/)
{
    if (apply)
    {
        m_target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
        m_target->AttackStop();
    }
    else
    {
        // do not remove unit flag if there are more than this auraEffect of that kind on unit
        if (m_target->HasAuraType(SPELL_AURA_MOD_PACIFY) || m_target->HasAuraType(SPELL_AURA_MOD_PACIFY_SILENCE))
            return;

        m_target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
    }
}

void Aura::HandleAuraModPacifyAndSilence(bool apply, bool Real)
{
    // Vengeance of the Blue Flight (@todo REMOVE THIS!)
    /// @workaround
    if (m_spellProto->Id == 45839)
    {
        if (apply)
            m_target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        else
            m_target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }
    if (!(apply))
    {
        // do not remove unit flag if there are more than this auraEffect of that kind on unit
        if (m_target->HasAuraType(SPELL_AURA_MOD_PACIFY_SILENCE))
            return;
    }

    HandleAuraModPacify(apply, Real);
    HandleAuraModSilence(apply, Real);
}

void Aura::HandleAuraGhost(bool apply, bool /*Real*/)
{
    if (m_target->GetTypeId() != TYPEID_PLAYER)
        return;

    if (apply)
    {
        m_target->SetFlag(PLAYER_FLAGS, PLAYER_FLAGS_GHOST);
        m_target->m_serverSideVisibility.SetValue(SERVERSIDE_VISIBILITY_GHOST, GHOST_VISIBILITY_GHOST);
        m_target->m_serverSideVisibilityDetect.SetValue(SERVERSIDE_VISIBILITY_GHOST, GHOST_VISIBILITY_GHOST);
    }
    else
    {
        if (m_target->HasAuraType(SPELL_AURA_GHOST))
            return;

        m_target->RemoveFlag(PLAYER_FLAGS, PLAYER_FLAGS_GHOST);
        m_target->m_serverSideVisibility.SetValue(SERVERSIDE_VISIBILITY_GHOST, GHOST_VISIBILITY_ALIVE);
        m_target->m_serverSideVisibilityDetect.SetValue(SERVERSIDE_VISIBILITY_GHOST, GHOST_VISIBILITY_ALIVE);
    }
}

void Aura::HandleAuraAllowFlight(bool apply, bool Real)
{
    // all applied/removed only at real aura add/remove
    if (!Real)
        return;

    if (!apply)
    {
        // do not remove unit flag if there are more than this auraEffect of that kind on unit
        if (m_target->HasAuraType(GetAuraType()) || m_target->HasAuraType(SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED))
            return;
    }

    m_target->SetCanFly(apply);

    if (!apply && !m_target->IsLevitating())
        m_target->GetMotionMaster()->MoveFall();
}

void Aura::HandleModRating(bool apply, bool Real)
{
    // spells required only Real aura add/remove
    if (!Real)
        return;

    if (m_target->GetTypeId() != TYPEID_PLAYER)
        return;

    for (uint32 rating = 0; rating < MAX_COMBAT_RATING; ++rating)
        if (m_modifier.m_miscvalue & (1 << rating))
            m_target->ToPlayer()->ApplyRatingMod(CombatRating(rating), GetModifierValue(), apply);
}

void Aura::HandleForceMoveForward(bool apply, bool Real)
{
    if (!Real || m_target->GetTypeId() != TYPEID_PLAYER)
        return;

    if (apply)
        m_target->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FORCE_MOVE);
    else
    {
        // do not remove unit flag if there are more than this auraEffect of that kind on unit
        if (m_target->HasAuraType(GetAuraType()))
            return;

        m_target->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FORCE_MOVE);
    }
}

void Aura::HandleAuraModExpertise(bool /*apply*/, bool /*Real*/)
{
    if (m_target->GetTypeId() != TYPEID_PLAYER)
        return;

    m_target->ToPlayer()->UpdateExpertise(BASE_ATTACK);
    m_target->ToPlayer()->UpdateExpertise(OFF_ATTACK);
}

void Aura::HandleModTargetResistance(bool apply, bool Real)
{
    // spells required only Real aura add/remove
    if (!Real)
        return;
    // applied to damage as HandleNoImmediateEffect in Unit::CalcAbsorbResist and Unit::CalcArmorReducedDamage

    // show armor penetration
    if (m_target->GetTypeId() == TYPEID_PLAYER && (m_modifier.m_miscvalue & SPELL_SCHOOL_MASK_NORMAL))
        m_target->ApplyModInt32Value(PLAYER_FIELD_MOD_TARGET_PHYSICAL_RESISTANCE, GetModifierValue(), apply);

    // show as spell penetration only full spell penetration bonuses (all resistances except armor and holy
    if (m_target->GetTypeId() == TYPEID_PLAYER && (m_modifier.m_miscvalue & SPELL_SCHOOL_MASK_SPELL) == SPELL_SCHOOL_MASK_SPELL)
        m_target->ApplyModInt32Value(PLAYER_FIELD_MOD_TARGET_RESISTANCE, GetModifierValue(), apply);
}

void Aura::HandleShieldBlockValue(bool apply, bool /*Real*/)
{
    BaseModType modType = FLAT_MOD;
    if (m_modifier.m_auraname == SPELL_AURA_MOD_SHIELD_BLOCKVALUE_PCT)
        modType = PCT_MOD;

    if (m_target->GetTypeId() == TYPEID_PLAYER)
        m_target->ToPlayer()->HandleBaseModValue(SHIELD_BLOCK_VALUE, modType, float(GetModifierValue()), apply);
}

void Aura::HandleAuraRetainComboPoints(bool apply, bool Real)
{
    // spells required only Real aura add/remove
    if (!Real)
        return;

    if (m_target->GetTypeId() != TYPEID_PLAYER)
        return;

    // combo points was added in SPELL_EFFECT_ADD_COMBO_POINTS handler
    // remove only if aura expire by time (in case combo points amount change aura removed without combo points lost)
    if (!apply && m_duration == 0 && m_target->ToPlayer()->GetComboTarget())
        if (Unit* unit = ObjectAccessor::GetUnit(*m_target, m_target->ToPlayer()->GetComboTarget()))
            m_target->ToPlayer()->AddComboPoints(unit, -GetModifierValue());
}

void Aura::HandleModUnattackable(bool Apply, bool Real)
{
    // do not remove unit flag if there are more than this auraEffect of that kind on unit on unit
    if (!Apply && m_target->HasAuraType(SPELL_AURA_MOD_UNATTACKABLE))
        return;

    m_target->ApplyModFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE, Apply);

    if (Real && Apply)
    {
        m_target->CombatStop();
        m_target->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_UNATTACKABLE);
    }
}

void Aura::HandleSpiritOfRedemption(bool apply, bool Real)
{
    // spells required only Real aura add/remove
    if (!Real)
        return;

    // prepare spirit state
    if (apply)
    {
        if (m_target->GetTypeId() == TYPEID_PLAYER)
        {
            // set stand state (expected in this form)
            if (!m_target->IsStandState())
                m_target->SetStandState(UNIT_STAND_STATE_STAND);

            // Interrupt currently casted spell
            m_target->InterruptNonMeleeSpells(true);
        }
    }
    // die at aura end
    else
        m_target->setDeathState(JUST_DIED);
}

void Aura::CleanupTriggeredSpells()
{
    if (sSpellMgr.IsSpellMemberOfSpellGroup(m_spellProto->Id, SPELL_GROUP_ELIXIR_SHATTRATH))
    {
        m_target->RemoveAurasDueToSpell(m_spellProto->EffectTriggerSpell[1]);  // remove triggered effect of shattrath flask, when removing it
        return;
    }

    uint32 tSpellId = m_spellProto->EffectTriggerSpell[GetEffIndex()];
    if (!tSpellId)
        return;

    SpellEntry const* tProto = sSpellStore.LookupEntry(tSpellId);
    if (!tProto)
        return;

    if (GetSpellDuration(tProto) != -1)
        return;

    // needed for spell 43680, maybe others
    // @todo is there a spell flag, which can solve this in a more sophisticated way?
    if (m_spellProto->EffectApplyAuraName[GetEffIndex()] == SPELL_AURA_PERIODIC_TRIGGER_SPELL &&
        uint32(GetSpellDuration(m_spellProto)) == m_spellProto->EffectAmplitude[GetEffIndex()])
        return;
    m_target->RemoveAurasDueToSpell(tSpellId);
}

void Aura::HandleAuraPowerBurn(bool apply, bool /*Real*/)
{
    if (m_periodicTimer <= 0)
        m_periodicTimer += m_modifier.periodictime;

    m_isPeriodic = apply;
}

void Aura::HandleSchoolAbsorb(bool apply, bool Real)
{
    if (!Real)
        return;

    // prevent double apply bonuses
    if (apply && (m_target->GetTypeId() != TYPEID_PLAYER || !m_target->ToPlayer()->GetSession()->PlayerLoading()))
    {
        if (Unit* caster = GetCaster())
        {
            float DoneActualBenefit = 0.0f;
            switch (m_spellProto->SpellFamilyName)
            {
            case SPELLFAMILY_PRIEST:
                if (m_spellProto->SpellFamilyFlags == 0x1) //PW:S
                {
                    //+30% from +healing bonus
                    DoneActualBenefit = caster->SpellBaseHealingBonus(GetSpellSchoolMask(m_spellProto)) * 0.3f;
                    break;
                }
                break;
            case SPELLFAMILY_MAGE:
                if (m_spellProto->SpellFamilyFlags == 0x80100 || m_spellProto->SpellFamilyFlags == 0x8 || m_spellProto->SpellFamilyFlags == 0x100000000LL)
                {
                    //frost ward, fire ward, ice barrier
                    //+10% from +spd bonus
                    DoneActualBenefit = caster->SpellBaseDamageBonus(GetSpellSchoolMask(m_spellProto)) * 0.1f;
                    break;
                }
                break;
            case SPELLFAMILY_WARLOCK:
                if (m_spellProto->SpellFamilyFlags == 0x00)
                {
                    //shadow ward
                    //+30% from +spd bonus
                    DoneActualBenefit = caster->SpellBaseDamageBonus(GetSpellSchoolMask(m_spellProto)) * 0.3f;
                    break;
                }
                break;
            default:
                break;
            }

            DoneActualBenefit *= caster->CalculateLevelPenalty(GetSpellProto());

            m_modifier.m_amount += (int32)DoneActualBenefit;
        }
    }
}

void Aura::PeriodicTick()
{
    if (!m_target->IsAlive())
        return;

    switch (m_modifier.m_auraname)
    {
    case SPELL_AURA_PERIODIC_DAMAGE:
    case SPELL_AURA_PERIODIC_DAMAGE_PERCENT:
        {
            Unit* pCaster = GetCaster();
            if (!pCaster)
                return;

            Unit* target = m_target;                        // aura can be deleted in DealDamage
                                                            // Needs to be moved to aura apply
            if (!pCaster->IsValidAttackTarget(target))
                return;

            if (GetSpellProto()->Effect[GetEffIndex()] == SPELL_EFFECT_PERSISTENT_AREA_AURA &&
                pCaster->SpellHitResult(m_target, GetSpellProto(), false) != SPELL_MISS_NONE)
                return;

            // Check for immune (not use charges)
            if (m_target->IsImmunedToDamage(GetSpellSchoolMask(GetSpellProto())))
                return;

            // some auras remove at specific health level or more
            if (m_modifier.m_auraname == SPELL_AURA_PERIODIC_DAMAGE)
            {
                switch (GetId())
                {
                case 43093:
                case 31956:
                case 38801:
                case 35321:
                case 38363:
                case 39215:
                    if (m_target->GetHealth() == m_target->GetMaxHealth())
                    {
                        m_target->RemoveAurasDueToSpell(GetId());
                        return;
                    }
                    break;
                case 38772:
                    {
                        uint32 percent =
                            GetEffIndex() < 2 && GetSpellProto()->Effect[GetEffIndex()] == SPELL_EFFECT_DUMMY ?
                            pCaster->CalculateSpellDamage(GetSpellProto(), GetEffIndex() + 1, GetSpellProto()->EffectBasePoints[GetEffIndex() + 1], m_target) :
                            100;
                        if (m_target->GetHealth() * 100 >= m_target->GetMaxHealth() * percent )
                        {
                            m_target->RemoveAurasDueToSpell(GetId());
                            return;
                        }
                        break;
                    }
                case 41337:// aura of anger
                    {
                        Unit::AuraList const& mMod = m_target->GetAurasByType(SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
                        for (Unit::AuraList::const_iterator i = mMod.begin(); i != mMod.end(); ++i)
                        {
                            if ((*i)->GetId() == 41337)
                            {
                                (*i)->ApplyModifier(false);
                                (*i)->GetModifier()->m_amount += 5;
                                (*i)->ApplyModifier(true);
                                break;
                            }
                        }
                        m_modifier.m_amount = 100 * m_tickNumber;
                    }
                    break;
                // Brutallus Burn
                case 46394:
                    {
                        if (m_tickNumber % 11 == 0)
                            m_modifier.m_amount *= 2;
                    }
                    break;
                // Curse of Agony - Sathrovarr
                case 45032:
                case 45034:
                    {
                        if ((m_tickNumber - 1) % 5 == 0 && (m_tickNumber - 1) > 0)
                            m_modifier.m_amount *= 2;
                    }
                    break;
                default:
                    break;
                }
            }

            uint32 absorb = 0;
            uint32 resist = 0;

            // ignore non positive values (can be result apply spellmods to aura damage
            uint32 amount = GetModifierValuePerStack() > 0 ? GetModifierValuePerStack() : 0;

            CleanDamage cleanDamage =  CleanDamage(0, BASE_ATTACK, MELEE_HIT_NORMAL);

            uint32 pdamage;

            if (m_modifier.m_auraname == SPELL_AURA_PERIODIC_DAMAGE)
            {
                pdamage = pCaster->SpellDamageBonus(m_target, GetSpellProto(), amount, DOT);

                // Calculate armor mitigation
                if (pCaster->IsDamageReducedByArmor(GetSpellSchoolMask(GetSpellProto()), GetSpellProto(), GetEffIndex()))
                {
                    uint32 damageReductedArmor = pCaster->CalcArmorReducedDamage(m_target, pdamage);
                    cleanDamage.damage += pdamage - damageReductedArmor;
                    pdamage = damageReductedArmor;
                }

                //pdamage = pCaster->SpellDamageBonus(m_target,GetSpellProto(),pdamage,DOT);

                // Curse of Agony damage-per-tick calculation
                if (GetSpellProto()->SpellFamilyName == SPELLFAMILY_WARLOCK && (GetSpellProto()->SpellFamilyFlags & 0x0000000000000400LL) && GetSpellProto()->SpellIconID == 544)
                {
                    uint32 totalTick = m_maxduration / m_modifier.periodictime;
                    // 1..4 ticks, 1/2 from normal tick damage
                    if (m_tickNumber <= totalTick / 3)
                        pdamage = pdamage / 2;
                    // 9..12 ticks, 3/2 from normal tick damage
                    else if (m_tickNumber > totalTick * 2 / 3)
                        pdamage += (pdamage + 1) / 2;       // +1 prevent 0.5 damage possible lost at 1..4 ticks
                    // 5..8 ticks have normal tick damage
                }
            }
            else
                pdamage = uint32(m_target->GetMaxHealth() * amount / 100);

            // As of 2.2 resilience reduces damage from DoT ticks as much as the chance to not be critically hit
            // Reduce dot damage from resilience for players
            if (m_target->GetTypeId() == TYPEID_PLAYER)
                pdamage -= m_target->ToPlayer()->GetDotDamageReduction(pdamage);

            pdamage *= GetStackAmount();

            if (!cleanDamage.damage)
                cleanDamage.damage = pdamage;

            pCaster->CalcAbsorbResist(m_target, GetSpellSchoolMask(GetSpellProto()), DOT, pdamage, &absorb, &resist);

            DEBUG_LOG("PeriodicTick: %u (TypeId: %u) attacked %u (TypeId: %u) for %u dmg inflicted by %u abs is %u",
                      GUID_LOPART(GetCasterGUID()), GuidHigh2TypeId(GUID_HIPART(GetCasterGUID())), m_target->GetGUIDLow(), m_target->GetTypeId(), pdamage, GetId(), absorb);


            SpellEntry const* spellProto = GetSpellProto();

            // Set trigger flag
            uint32 procAttacker = PROC_FLAG_DONE_PERIODIC;
            uint32 procVictim   = PROC_FLAG_TAKEN_PERIODIC;
            uint32 procEx = PROC_EX_INTERNAL_DOT | PROC_EX_NORMAL_HIT;
            pdamage = (pdamage <= absorb + resist) ? 0 : (pdamage - absorb - resist);

            WorldPacket data(SMSG_PERIODICAURALOG, (21 + 16)); // we guess size
            data << m_target->GetPackGUID();
            data.appendPackGUID(GetCasterGUID());
            data << uint32(GetId());
            data << uint32(1);
            data << uint32(m_modifier.m_auraname);
            data << (uint32)pdamage;
            data << (uint32)GetSpellSchoolMask(GetSpellProto()); // will be mask in 2.4.x
            data << (uint32)absorb;
            data << (uint32)resist;
            m_target->SendMessageToSet(&data, true);

            if (pdamage)
                procVictim |= PROC_FLAG_TAKEN_DAMAGE;

            pCaster->DealDamage(target, pdamage, &cleanDamage, DOT, GetSpellSchoolMask(spellProto), spellProto, true);
            pCaster->ProcDamageAndSpell(target, procAttacker, procVictim, procEx, pdamage, BASE_ATTACK, spellProto);
            break;
        }
    case SPELL_AURA_PERIODIC_LEECH:
        {
            Unit* pCaster = GetCaster();
            if (!pCaster)
                return;

            if (!pCaster->IsAlive())
                return;

            if (GetSpellProto()->Effect[GetEffIndex()] == SPELL_EFFECT_PERSISTENT_AREA_AURA &&
                pCaster->SpellHitResult(m_target, GetSpellProto(), false) != SPELL_MISS_NONE)
                return;

            // Check for immune (not use charges)
            if (m_target->IsImmunedToDamage(GetSpellSchoolMask(GetSpellProto())))
                return;

            uint32 absorb = 0;
            uint32 resist = 0;

            bool applyHealingThreat = true;

            uint32 pdamage = GetModifierValuePerStack() > 0 ? GetModifierValuePerStack() : 0;

            CleanDamage cleanDamage =  CleanDamage(pdamage, BASE_ATTACK, MELEE_HIT_NORMAL);

            pdamage = pCaster->SpellDamageBonus(m_target, GetSpellProto(), pdamage, DOT);

            //Calculate armor mitigation if it is a physical spell
            if (GetSpellSchoolMask(GetSpellProto()) & SPELL_SCHOOL_MASK_NORMAL)
            {
                uint32 pdamageReductedArmor = pCaster->CalcArmorReducedDamage(m_target, pdamage);
                cleanDamage.damage += pdamage - pdamageReductedArmor;
                pdamage = pdamageReductedArmor;
            }

            //pdamage = pCaster->SpellDamageBonus(m_target,GetSpellProto(),pdamage,DOT);

            // talent Soul Siphon add bonus to Drain Life spells
            if (GetSpellProto()->SpellFamilyName == SPELLFAMILY_WARLOCK && (GetSpellProto()->SpellFamilyFlags & 0x8))
            {
                // Spell is Drain Life

                // Drain Life healing should not generate threat (Issue #1232)
                applyHealingThreat = false;

                // find talent max bonus percentage
                Unit::AuraList const& mClassScriptAuras = pCaster->GetAurasByType(SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
                for (Unit::AuraList::const_iterator i = mClassScriptAuras.begin(); i != mClassScriptAuras.end(); ++i)
                {
                    if ((*i)->GetModifier()->m_miscvalue == 4992 || (*i)->GetModifier()->m_miscvalue == 4993)
                    {
                        if ((*i)->GetEffIndex() != 1)
                        {
                            sLog.outError("Expected spell %u structure change, need code update", (*i)->GetId());
                            break;
                        }

                        // effect 1 m_amount
                        int32 maxPercent = (*i)->GetModifier()->m_amount;
                        // effect 0 m_amount
                        int32 stepPercent = pCaster->CalculateSpellDamage((*i)->GetSpellProto(), 0, (*i)->GetSpellProto()->EffectBasePoints[0], pCaster);

                        // count affliction effects and calc additional damage in percentage
                        int32 modPercent = 0;
                        Unit::AuraMap const& victimAuras = m_target->GetAuras();
                        for (Unit::AuraMap::const_iterator itr = victimAuras.begin(); itr != victimAuras.end(); ++itr)
                        {
                            Aura* aura = itr->second;
                            if (aura->IsPositive())continue;
                            SpellEntry const* m_spell = aura->GetSpellProto();
                            if (m_spell->SpellFamilyName != SPELLFAMILY_WARLOCK)
                                continue;

                            SkillLineAbilityMap::const_iterator lower = sSpellMgr.GetBeginSkillLineAbilityMap(m_spell->Id);
                            SkillLineAbilityMap::const_iterator upper = sSpellMgr.GetEndSkillLineAbilityMap(m_spell->Id);

                            for (SkillLineAbilityMap::const_iterator _spell_idx = lower; _spell_idx != upper; ++_spell_idx)
                            {
                                if (_spell_idx->second->skillId == SKILL_AFFLICTION)
                                {
                                    modPercent += stepPercent;
                                    if (modPercent >= maxPercent)
                                    {
                                        modPercent = maxPercent;
                                        break;
                                    }
                                }
                            }
                        }
                        pdamage += (pdamage * modPercent / 100);
                        break;
                    }
                }
            }

            // As of 2.2 resilience reduces damage from DoT ticks as much as the chance to not be critically hit
            // Reduce dot damage from resilience for players
            if (m_target->GetTypeId() == TYPEID_PLAYER)
                pdamage -= m_target->ToPlayer()->GetDotDamageReduction(pdamage);

            pdamage *= GetStackAmount();

            pCaster->CalcAbsorbResist(m_target, GetSpellSchoolMask(GetSpellProto()), DOT, pdamage, &absorb, &resist);

            if (m_target->GetHealth() < pdamage)
                pdamage = uint32(m_target->GetHealth());

            sLog.outDetail("PeriodicTick: %u (TypeId: %u) health leech of %u (TypeId: %u) for %u dmg inflicted by %u abs is %u",
                           GUID_LOPART(GetCasterGUID()), GuidHigh2TypeId(GUID_HIPART(GetCasterGUID())), m_target->GetGUIDLow(), m_target->GetTypeId(), pdamage, GetId(), absorb);

            pCaster->SendSpellNonMeleeDamageLog(m_target, GetId(), pdamage, GetSpellSchoolMask(GetSpellProto()), absorb, resist, false, 0);


            Unit* target = m_target;                        // aura can be deleted in DealDamage
            SpellEntry const* spellProto = GetSpellProto();
            float multiplier = spellProto->EffectMultipleValue[GetEffIndex()] > 0 ? spellProto->EffectMultipleValue[GetEffIndex()] : 1;

            // Set trigger flag
            uint32 procAttacker = PROC_FLAG_DONE_PERIODIC;
            uint32 procVictim   = PROC_FLAG_TAKEN_PERIODIC;
            uint32 procEx = PROC_EX_INTERNAL_DOT | PROC_EX_NORMAL_HIT;
            pdamage = (pdamage <= absorb + resist) ? 0 : (pdamage - absorb - resist);
            if (pdamage)
                procVictim |= PROC_FLAG_TAKEN_DAMAGE;
            pCaster->ProcDamageAndSpell(target, procAttacker, procVictim, procEx, pdamage, BASE_ATTACK, spellProto);
            // Apply damage to target
            int32 new_damage = pCaster->DealDamage(target, pdamage, &cleanDamage, DOT, GetSpellSchoolMask(spellProto), spellProto, false);

            if (!target->IsAlive() && pCaster->IsNonMeleeSpellCast(false))
                for (uint32 i = CURRENT_FIRST_NON_MELEE_SPELL; i < CURRENT_MAX_SPELL; ++i)
                    if (Spell* spell = pCaster->GetCurrentSpell(i))
                        if (spell->m_spellInfo->Id == spellProto->Id)
                            spell->cancel();

            if (Player* modOwner = pCaster->GetSpellModOwner())
                modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_MULTIPLE_VALUE, multiplier);

            uint32 heal = pCaster->SpellHealingBonus(spellProto, uint32(new_damage * multiplier), DOT, pCaster);

            // Apply healing to caster
            int32 gain = pCaster->ModifyHealth(heal);
            // Apply threat to caster for the amount healed.
            if (applyHealingThreat)
                pCaster->getHostileRefManager().threatAssist(pCaster, gain * 0.5f, spellProto);

            pCaster->SendHealSpellLog(pCaster, spellProto->Id, heal);
            break;
        }
    case SPELL_AURA_PERIODIC_HEAL:
    case SPELL_AURA_OBS_MOD_HEALTH:
        {
            Unit* pCaster = GetCaster();
            if (!pCaster)
                return;

            // heal for caster damage (must be alive)
            if (m_target != pCaster && GetSpellProto()->SpellVisual == 163 && !pCaster->IsAlive())
                return;

            // ignore non positive values (can be result apply spellmods to aura damage
            uint32 amount = GetModifierValuePerStack() > 0 ? GetModifierValuePerStack() : 0;

            uint32 pdamage;

            if (m_modifier.m_auraname == SPELL_AURA_OBS_MOD_HEALTH)
            {
                float heal = float(uint32(m_target->GetMaxHealth() * amount / 100));

                // Only reduce auras with SPELL_AURA_OBS_MOD_HEALTH, don't increase them
                float minval = pCaster->GetMaxNegativeAuraModifier(SPELL_AURA_MOD_HEALING_PCT);
                if (minval)
                    heal *= (100.0f + minval) / 100.0f;

                if (heal < 0) heal = 0;
                pdamage = heal;
            }
            else
                pdamage = pCaster->SpellHealingBonus(GetSpellProto(), amount, DOT, m_target);

            pdamage *= GetStackAmount();

            sLog.outDetail("PeriodicTick: %u (TypeId: %u) heal of %u (TypeId: %u) for %u health inflicted by %u",
                           GUID_LOPART(GetCasterGUID()), GuidHigh2TypeId(GUID_HIPART(GetCasterGUID())), m_target->GetGUIDLow(), m_target->GetTypeId(), pdamage, GetId());

            if (m_target->IsAIEnabled)
                m_target->ToCreature()->AI()->HealReceived(pCaster, pdamage);

            WorldPacket data(SMSG_PERIODICAURALOG, (21 + 16)); // we guess size
            data << m_target->GetPackGUID();
            data.appendPackGUID(GetCasterGUID());
            data << uint32(GetId());
            data << uint32(1);
            data << uint32(m_modifier.m_auraname);
            data << (uint32)pdamage;
            m_target->SendMessageToSet(&data, true);

            //Do check before because m_modifier.auraName can be invalidate by DealDamage.
            bool procSpell = (m_modifier.m_auraname == SPELL_AURA_PERIODIC_HEAL && m_target != pCaster);
            Unit* target = m_target;                        // aura can be deleted in DealDamage
            SpellEntry const* spellProto = GetSpellProto();
            bool haveCastItem = GetCastItemGUID() != 0;

            uint32 procAttacker = PROC_FLAG_DONE_PERIODIC;
            uint32 procVictim   = PROC_FLAG_TAKEN_PERIODIC;
            uint32 procEx = PROC_EX_INTERNAL_HOT | PROC_EX_NORMAL_HIT;
            // ignore item heals
            if (procSpell && !haveCastItem)
                pCaster->ProcDamageAndSpell(target, procAttacker, procVictim, procEx, pdamage, BASE_ATTACK, spellProto);

            int32 gain = m_target->ModifyHealth(pdamage);

            // add HoTs to amount healed in bgs
            if (pCaster->GetTypeId() == TYPEID_PLAYER)
                if (Battleground* bg = pCaster->ToPlayer()->GetBattleground())
                    bg->UpdatePlayerScore(pCaster->ToPlayer(), SCORE_HEALING_DONE, gain);

            m_target->getHostileRefManager().threatAssist(pCaster, float(gain) * 0.5f, GetSpellProto());

            // heal for caster damage
            if (m_target != pCaster && spellProto->SpellVisual == 163)
            {
                uint32 dmg = spellProto->manaPerSecond;
                if (pCaster->GetHealth() <= dmg && pCaster->GetTypeId() == TYPEID_PLAYER)
                {
                    pCaster->RemoveAurasDueToSpell(GetId());

                    // finish current generic/channeling spells, don't affect autorepeat
                    pCaster->FinishSpell(CURRENT_GENERIC_SPELL);
                    pCaster->FinishSpell(CURRENT_CHANNELED_SPELL);
                }
                else
                {
                    pCaster->SendSpellNonMeleeDamageLog(pCaster, GetId(), gain, GetSpellSchoolMask(GetSpellProto()), 0, 0, false, 0, false);

                    CleanDamage cleanDamage =  CleanDamage(0, BASE_ATTACK, MELEE_HIT_NORMAL);
                    pCaster->DealDamage(pCaster, gain, &cleanDamage, NODAMAGE, GetSpellSchoolMask(GetSpellProto()), GetSpellProto(), true);
                }
            }

            break;
        }
    case SPELL_AURA_PERIODIC_MANA_LEECH:
        {
            Unit* pCaster = GetCaster();
            if (!pCaster)
                return;

            if (!pCaster->IsAlive())
                return;

            if (GetSpellProto()->Effect[GetEffIndex()] == SPELL_EFFECT_PERSISTENT_AREA_AURA &&
                pCaster->SpellHitResult(m_target, GetSpellProto(), false) != SPELL_MISS_NONE)
                return;

            // Check for immune (not use charges)
            if (m_target->IsImmunedToDamage(GetSpellSchoolMask(GetSpellProto())))
                return;

            // ignore non positive values (can be result apply spellmods to aura damage
            uint32 pdamage = GetModifierValue() > 0 ? GetModifierValue() : 0;

            sLog.outDetail("PeriodicTick: %u (TypeId: %u) power leech of %u (TypeId: %u) for %u dmg inflicted by %u",
                           GUID_LOPART(GetCasterGUID()), GuidHigh2TypeId(GUID_HIPART(GetCasterGUID())), m_target->GetGUIDLow(), m_target->GetTypeId(), pdamage, GetId());

            if (m_modifier.m_miscvalue < 0 || m_modifier.m_miscvalue > 4)
                break;

            Powers power = Powers(m_modifier.m_miscvalue);

            // power type might have changed between aura applying and tick (druid's shapeshift)
            if (m_target->getPowerType() != power)
                break;

            int32 drain_amount = m_target->GetPower(power) > pdamage ? pdamage : m_target->GetPower(power);

            // resilience reduce mana draining effect at spell crit damage reduction (added in 2.4)
            if (power == POWER_MANA && m_target->GetTypeId() == TYPEID_PLAYER)
                drain_amount -= m_target->ToPlayer()->GetSpellCritDamageReduction(drain_amount);

            m_target->ModifyPower(power, -drain_amount);

            float gain_multiplier = 0;

            if (pCaster->GetMaxPower(power) > 0)
            {
                gain_multiplier = GetSpellProto()->EffectMultipleValue[GetEffIndex()];

                if (Player* modOwner = pCaster->GetSpellModOwner())
                    modOwner->ApplySpellMod(GetId(), SPELLMOD_MULTIPLE_VALUE, gain_multiplier);
            }

            WorldPacket data(SMSG_PERIODICAURALOG, (21 + 16)); // we guess size
            data << m_target->GetPackGUID();
            data.appendPackGUID(GetCasterGUID());
            data << uint32(GetId());
            data << uint32(1);
            data << uint32(m_modifier.m_auraname);
            data << (uint32)power;                          // power type
            data << (uint32)drain_amount;
            data << (float)gain_multiplier;
            m_target->SendMessageToSet(&data, true);

            int32 gain_amount = int32(drain_amount * gain_multiplier);

            if (gain_amount)
            {
                int32 gain = pCaster->ModifyPower(power, gain_amount);
                m_target->AddThreat(pCaster, float(gain) * 0.5f, GetSpellSchoolMask(GetSpellProto()), GetSpellProto());
            }

            // Mark of Kaz'rogal
            if (GetId() == 31447 && m_target->GetPower(power) == 0)
            {
                m_target->CastSpell(m_target, 31463, true, 0, this);
                // Remove aura
                SetAuraDuration(0);
            }

            // Mark of Kazzak
            if (GetId() == 32960)
            {
                int32 modifier = (m_target->GetPower(power) * 0.05f);
                m_target->ModifyPower(power, -modifier);

                if (m_target->GetPower(power) == 0)
                {
                    m_target->CastSpell(m_target, 32961, true, 0, this);
                    // Remove aura
                    SetAuraDuration(0);
                }
            }

            m_target->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_DAMAGE, m_spellProto ? m_spellProto->Id : 0);
            break;
        }
    case SPELL_AURA_PERIODIC_ENERGIZE:
        {
            // ignore non positive values (can be result apply spellmods to aura damage
            uint32 pdamage = GetModifierValue() > 0 ? GetModifierValue() : 0;

            sLog.outDetail("PeriodicTick: %u (TypeId: %u) energize %u (TypeId: %u) for %u dmg inflicted by %u",
                           GUID_LOPART(GetCasterGUID()), GuidHigh2TypeId(GUID_HIPART(GetCasterGUID())), m_target->GetGUIDLow(), m_target->GetTypeId(), pdamage, GetId());

            if (m_modifier.m_miscvalue < 0 || m_modifier.m_miscvalue >= MAX_POWERS)
                break;

            Powers power = Powers(m_modifier.m_miscvalue);

            if (m_target->GetMaxPower(power) == 0)
                break;

            WorldPacket data(SMSG_PERIODICAURALOG, (21 + 16)); // we guess size
            data << m_target->GetPackGUID();
            data.appendPackGUID(GetCasterGUID());
            data << uint32(GetId());
            data << uint32(1);
            data << uint32(m_modifier.m_auraname);
            data << (uint32)power;                          // power type
            data << (uint32)pdamage;
            m_target->SendMessageToSet(&data, true);

            int32 gain = m_target->ModifyPower(power, pdamage);

            if (Unit* pCaster = GetCaster())
                m_target->getHostileRefManager().threatAssist(pCaster, float(gain) * 0.5f, GetSpellProto());
            break;
        }
    case SPELL_AURA_OBS_MOD_MANA:
        {
            // ignore non positive values (can be result apply spellmods to aura damage
            uint32 amount = GetModifierValue() > 0 ? GetModifierValue() : 0;

            uint32 pdamage = uint32(m_target->GetMaxPower(POWER_MANA) * amount / 100);

            sLog.outDetail("PeriodicTick: %u (TypeId: %u) energize %u (TypeId: %u) for %u mana inflicted by %u",
                           GUID_LOPART(GetCasterGUID()), GuidHigh2TypeId(GUID_HIPART(GetCasterGUID())), m_target->GetGUIDLow(), m_target->GetTypeId(), pdamage, GetId());

            if (m_target->GetMaxPower(POWER_MANA) == 0)
                break;

            WorldPacket data(SMSG_PERIODICAURALOG, (21 + 16)); // we guess size
            data << m_target->GetPackGUID();
            data.appendPackGUID(GetCasterGUID());
            data << uint32(GetId());
            data << uint32(1);
            data << uint32(m_modifier.m_auraname);
            data << (uint32)0;                              // ?
            data << (uint32)pdamage;
            m_target->SendMessageToSet(&data, true);

            int32 gain = m_target->ModifyPower(POWER_MANA, pdamage);

            if (Unit* pCaster = GetCaster())
                m_target->getHostileRefManager().threatAssist(pCaster, float(gain) * 0.5f, GetSpellProto());
            break;
        }
    case SPELL_AURA_POWER_BURN_MANA:
        {
            Unit* pCaster = GetCaster();
            if (!pCaster)
                return;

            // Check for immune (not use charges)
            if (m_target->IsImmunedToDamage(GetSpellSchoolMask(GetSpellProto())))
                return;

            int32 pdamage = GetModifierValue() > 0 ? GetModifierValue() : 0;

            Powers powerType = Powers(m_modifier.m_miscvalue);

            if (!m_target->IsAlive() || m_target->getPowerType() != powerType)
                return;

            // resilience reduce mana draining effect at spell crit damage reduction (added in 2.4)
            if (powerType == POWER_MANA && m_target->GetTypeId() == TYPEID_PLAYER)
                pdamage -= m_target->ToPlayer()->GetSpellCritDamageReduction(pdamage);

            uint32 gain = uint32(-m_target->ModifyPower(powerType, -pdamage));

            gain = uint32(gain * GetSpellProto()->EffectMultipleValue[GetEffIndex()]);

            SpellEntry const* spellProto = GetSpellProto();
            //maybe has to be sent different to client, but not by SMSG_PERIODICAURALOG
            SpellNonMeleeDamage damageInfo(pCaster, m_target, spellProto->Id, spellProto->SchoolMask);
            //no SpellDamageBonus for burn mana
            pCaster->CalculateSpellDamageTaken(&damageInfo, gain, spellProto);
            pCaster->SendSpellNonMeleeDamageLog(&damageInfo);

            // Set trigger flag
            uint32 procAttacker = PROC_FLAG_DONE_PERIODIC;
            uint32 procVictim   = PROC_FLAG_TAKEN_PERIODIC;
            uint32 procEx       = createProcExtendMask(&damageInfo, SPELL_MISS_NONE) | PROC_EX_INTERNAL_DOT;
            if (damageInfo.damage)
                procVictim |= PROC_FLAG_TAKEN_DAMAGE;

            pCaster->ProcDamageAndSpell(damageInfo.target, procAttacker, procVictim, procEx, damageInfo.damage, BASE_ATTACK, spellProto);

            pCaster->DealSpellDamage(&damageInfo, true);
            break;
        }
    // Here tick dummy auras
    case SPELL_AURA_DUMMY:                              // some spells have dummy aura
    case SPELL_AURA_PERIODIC_DUMMY:
        {
            PeriodicDummyTick();
            break;
        }
    case SPELL_AURA_PERIODIC_TRIGGER_SPELL:
        {
            TriggerSpell();
            break;
        }
    case SPELL_AURA_PERIODIC_TRIGGER_SPELL_WITH_VALUE:
        {
            TriggerSpellWithValue();
            break;
        }
    default:
        break;
    }
}

void Aura::PeriodicDummyTick()
{
    SpellEntry const* spell = GetSpellProto();
    switch (spell->Id)
    {
    // Drink
    case 430:
    case 431:
    case 432:
    case 1133:
    case 1135:
    case 1137:
    case 10250:
    case 22734:
    case 27089:
    case 34291:
    case 43706:
    case 46755:
        {
            if (m_target->GetTypeId() != TYPEID_PLAYER)
                return;
            // Search SPELL_AURA_MOD_POWER_REGEN aura for this spell and add bonus
            Unit::AuraList const& aura = m_target->GetAurasByType(SPELL_AURA_MOD_POWER_REGEN);
            for (Unit::AuraList::const_iterator i = aura.begin(); i != aura.end(); ++i)
            {
                if ((*i)->GetId() == GetId())
                {
                    Battleground* bg = m_target->ToPlayer()->GetBattleground();
                    if (!bg || !bg->isArena())
                    {
                        // default case - not in arena
                        m_isPeriodic = false;
                        if (m_tickNumber == 1)
                            (*i)->GetModifier()->m_amount = m_modifier.m_amount;
                        m_target->ToPlayer()->UpdateManaRegen();
                        return;
                    }
                    //**********************************************/
                    // This feature uses only in arenas
                    //**********************************************/
                    // Here need increase mana regen per tick (6 second rule)
                    // on 0 tick -   0  (handled in 2 second)
                    // on 1 tick - 166% (handled in 4 second)
                    // on 2 tick - 133% (handled in 6 second)
                    // Not need update after 4 tick
                    if (m_tickNumber > 4)
                        return;
                    // Apply bonus for 1 - 4 tick
                    switch (m_tickNumber)
                    {
                    case 1:   // 0%
                        (*i)->GetModifier()->m_amount = 0;
                        break;
                    case 2:   // 166%
                        (*i)->GetModifier()->m_amount = m_modifier.m_amount * 5 / 3;
                        break;
                    case 3:   // 133%
                        (*i)->GetModifier()->m_amount = m_modifier.m_amount * 4 / 3;
                        break;
                    default:  // 100% - normal regen
                        (*i)->GetModifier()->m_amount = m_modifier.m_amount;
                        break;
                    }
                    m_target->ToPlayer()->UpdateManaRegen();
                    return;
                }
            }
            return;
        }
    case 7057:                                  // Haunting Spirits
    {
        if (roll_chance_i(33))
        {
            m_target->CastSpell(m_target, m_modifier.m_amount, true, NULL, this);

            m_amplitude = 30000;
        }

        return;
    }
    //        // Panda
    //        case 19230: break;
    //        // Master of Subtlety
    //        case 31666: break;
    //        // Gossip NPC Periodic - Talk
    //        case 33208: break;
    //        // Gossip NPC Periodic - Despawn
    //        case 33209: break;
    //        // Force of Nature
    //        case 33831: break;
    // Aspect of the Viper
    case 34074:
        {
            if (m_target->GetTypeId() != TYPEID_PLAYER)
                return;
            // Should be manauser
            if (m_target->getPowerType() != POWER_MANA)
                return;
            Unit* caster = GetCaster();
            if (!caster)
                return;
            // Regen amount is max (100% from spell) on 21% or less mana and min on 92.5% or greater mana (20% from spell)
            int mana = m_target->GetPower(POWER_MANA);
            int max_mana = m_target->GetMaxPower(POWER_MANA);
            int32 base_regen = caster->CalculateSpellDamage(m_spellProto, m_effIndex, m_currentBasePoints, m_target);
            float regen_pct = 1.20f - 1.1f * mana / max_mana;
            if      (regen_pct > 1.0f) regen_pct = 1.0f;
            else if (regen_pct < 0.2f) regen_pct = 0.2f;
            m_modifier.m_amount = int32 (base_regen * regen_pct);
            m_target->ToPlayer()->UpdateManaRegen();
            return;
        }
    //        // Steal Weapon
    //        case 36207: break;
    //        // Simon Game START timer, (DND)
    //        case 39993: break;
    //        // Harpooner's Mark
    //        case 40084: break;
    //        // Knockdown Fel Cannon: break; The Aggro Burst
    //        case 40119: break;
    //        // Old Mount Spell
    //        case 40154: break;
    //        // Magnetic Pull
    //        case 40581: break;
    //        // Ethereal Ring: break; The Bolt Burst
    //        case 40801: break;
    //        // Crystal Prison
    //        case 40846: break;
    //        // Copy Weapon
    //        case 41054: break;
    //        // Ethereal Ring Visual, Lightning Aura
    //        case 41477: break;
    //        // Ethereal Ring Visual, Lightning Aura (Fork)
    //        case 41525: break;
    //        // Ethereal Ring Visual, Lightning Jumper Aura
    //        case 41567: break;
    //        // No Man's Land
    //        case 41955: break;
    //        // Headless Horseman - Fire
    //        case 42074: break;
    //        // Headless Horseman - Visual - Large Fire
    //        case 42075: break;
    //        // Headless Horseman - Start Fire, Periodic Aura
    //        case 42140: break;
    //        // Ram Speed Boost
    //        case 42152: break;
    //        // Headless Horseman - Fires Out Victory Aura
    //        case 42235: break;
    //        // Pumpkin Life Cycle
    //        case 42280: break;
    //        // Brewfest Request Chick Chuck Mug Aura
    //        case 42537: break;
    //        // Squashling
    //        case 42596: break;
    //        // Headless Horseman Climax, Head: Periodic
    //        case 42603: break;
    //        // Fire Bomb
    //        case 42621: break;
    //        // Headless Horseman - Conflagrate, Periodic Aura
    //        case 42637: break;
    //        // Headless Horseman - Create Pumpkin Treats Aura
    //        case 42774: break;
    //        // Headless Horseman Climax - Summoning Rhyme Aura
    //        case 42879: break;
    //        // Tricky Treat
    //        case 42919: break;
    //        // Giddyup!
    //        case 42924: break;
    //        // Ram - Trot
    //        case 42992: break;
    //        // Ram - Canter
    //        case 42993: break;
    //        // Ram - Gallop
    //        case 42994: break;
    //        // Ram Level - Neutral
    //        case 43310: break;
    //        // Headless Horseman - Maniacal Laugh, Maniacal, Delayed 17
    //        case 43884: break;
    //        // Headless Horseman - Maniacal Laugh, Maniacal, other, Delayed 17
    //        case 44000: break;
    //        // Energy Feedback
    //        case 44328: break;
    //        // Romantic Picnic
    //        case 45102: break;
    //        // Romantic Picnic
    //        case 45123: break;
    //        // Looking for Love
    //        case 45124: break;
    //        // Kite - Lightning Strike Kite Aura
    //        case 45197: break;
    //        // Rocket Chicken
    //        case 45202: break;
    //        // Copy Offhand Weapon
    //        case 45205: break;
    //        // Upper Deck - Kite - Lightning Periodic Aura
    //        case 45207: break;
    //        // Kite -Sky  Lightning Strike Kite Aura
    //        case 45251: break;
    //        // Ribbon Pole Dancer Check Aura
    //        case 45390: break;
    //        // Holiday - Midsummer, Ribbon Pole Periodic Visual
    //        case 45406: break;
    //        // Parachute
    //        case 45472: break;
    //        // Alliance Flag, Extra Damage Debuff
    //        case 45898: break;
    //        // Horde Flag, Extra Damage Debuff
    //        case 45899: break;
    //        // Ahune - Summoning Rhyme Aura
    //        case 45926: break;
    //        // Ahune - Slippery Floor
    //        case 45945: break;
    //        // Ahune's Shield
    //        case 45954: break;
    //        // Nether Vapor Lightning
    //        case 45960: break;
    //        // Darkness
    //        case 45996: break;
    // Summon Blood Elves Periodic
    case 46041:
        m_target->CastSpell(m_target, 46037, true, NULL, this);
        m_target->CastSpell(m_target, roll_chance_i(50) ? 46038 : 46039, true, NULL, this);
        m_target->CastSpell(m_target, 46040, true, NULL, this);
        return;
        break;
    //        // Transform Visual Missile Periodic
    //        case 46205: break;
    //        // Find Opening Beam End
    //        case 46333: break;
    //        // Ice Spear Control Aura
    //        case 46371: break;
    //        // Hailstone Chill
    //        case 46458: break;
    //        // Hailstone Chill, Internal
    //        case 46465: break;
    //        // Chill, Internal Shifter
    //        case 46549: break;
    //        // Summon Ice Spear Knockback Delayer
    //        case 46878: break;
    //        // Burninate Effect
    //        case 47214: break;
    //        // Fizzcrank Practice Parachute
    //        case 47228: break;
    //        // Send Mug Control Aura
    //        case 47369: break;
    //        // Direbrew's Disarm (precast)
    //        case 47407: break;
    //        // Mole Machine Port Schedule
    //        case 47489: break;
    //        // Mole Machine Portal Schedule
    //        case 49466: break;
    //        // Drink Coffee
    //        case 49472: break;
    //        // Listening to Music
    //        case 50493: break;
    //        // Love Rocket Barrage
    //        case 50530: break;
    default:
        break;
    }
}

void Aura::HandlePreventFleeing(bool apply, bool Real)
{
    if (!Real)
        return;

    if (apply)
        m_target->RemoveAurasByType(SPELL_AURA_MOD_FEAR);

    m_target->ApplySpellImmune(GetSpellProto()->Id, IMMUNITY_MECHANIC, MECHANIC_FEAR,   apply); // fear
    m_target->ApplySpellImmune(GetSpellProto()->Id, IMMUNITY_MECHANIC, MECHANIC_HORROR, apply); // horror effects

    /// @todo: find a way to cancel fleeing for assistance.
    /// Currently this will only stop creatures fleeing due to low health that could not find nearby allies to flee towards.
    m_target->SetControlled(false, UNIT_STATE_FLEEING);
}

void Aura::HandleManaShield(bool apply, bool Real)
{
    if (!Real)
        return;

    // prevent double apply bonuses
    if (apply && (m_target->GetTypeId() != TYPEID_PLAYER || !m_target->ToPlayer()->GetSession()->PlayerLoading()))
    {
        if (Unit* caster = GetCaster())
        {
            float DoneActualBenefit = 0.0f;
            switch (m_spellProto->SpellFamilyName)
            {
            case SPELLFAMILY_MAGE:
                if (m_spellProto->SpellFamilyFlags & 0x8000)
                {
                    // Mana Shield
                    // +50% from +spd bonus
                    DoneActualBenefit = caster->SpellBaseDamageBonus(GetSpellSchoolMask(m_spellProto)) * 0.5f;
                    break;
                }
                break;
            default:
                break;
            }

            DoneActualBenefit *= caster->CalculateLevelPenalty(GetSpellProto());

            m_modifier.m_amount += (int32)DoneActualBenefit;
        }
    }
}

void Aura::HandleArenaPreparation(bool apply, bool Real)
{
    if (!Real)
        return;

    if (apply)
        m_target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PREPARATION);
    else
        m_target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PREPARATION);
}

void Aura::HandleAuraReflectSpellSchool(bool apply, bool real)
{
    if (!real || !apply)
        return;

    if (Player* pTarget = m_target->ToPlayer())
    {
        if (GetSpellProto()->SpellFamilyName == SPELLFAMILY_MAGE)
        {
            // Fire Ward
            if (GetSpellProto()->SpellFamilyFlags & 0x8)
                GetModifier()->m_amount += pTarget->HasSpell(11094) ? 10 : pTarget->HasSpell(13043) ? 20 : 0;
            // Frost Ward
            else if (GetSpellProto()->SpellFamilyFlags & 0x80100)
                GetModifier()->m_amount += pTarget->HasSpell(11189) ? 10 : pTarget->HasSpell(28332) ? 20 : 0;
        }
    }
}

void Aura::HandleAuraCloneCaster(bool apply, bool Real)
{
    if (apply)
    {
        Unit* caster = GetCaster();
        if (!caster || caster == m_target)
            return;

        m_target->SetDisplayId(caster->GetDisplayId());
        m_target->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_MIRROR_IMAGE);
    }
    else
    {
        m_target->SetDisplayId(m_target->GetNativeDisplayId());
        m_target->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_MIRROR_IMAGE);
    }
}
void Aura::UnregisterSingleCastAura()
{
    if (IsSingleTarget())
    {
        if (Unit* caster = GetCaster())
            caster->GetSingleCastAuras().remove(this);
        else
        {
            sLog.outError("Couldn't find the caster of the single target aura, may crash later!");
            ASSERT(false);
        }
        m_isSingleTargetAura = false;
    }
}

void Aura::HandleIncreasePetOutdoorSpeed(bool apply, bool /*Real*/)
{
    if (Unit* caster = GetCaster())
    {
        if (Player* player = caster->ToPlayer())
        {
            if (Pet* pet = player->GetPet())
            {
                if (apply)
                    pet->CastSpell(pet, SPELL_INC_OUTDOOR_SPEED_30, false);
                else
                    pet->RemoveAurasDueToSpell(SPELL_INC_OUTDOOR_SPEED_30);
            }
        }
    }
}

