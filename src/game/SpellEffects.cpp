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
#include "SharedDefines.h"
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
#include "SkillExtraItems.h"
#include "Unit.h"
#include "CreatureAI.h"
#include "Spell.h"
#include "DynamicObject.h"
#include "SpellAuras.h"
#include "Pet.h"
#include "GameObject.h"
#include "GossipDef.h"
#include "Creature.h"
#include "Totem.h"
#include "Battleground.h"
#include "BattlegroundEY.h"
#include "BattlegroundWS.h"
#include "OutdoorPvPMgr.h"
#include "Language.h"
#include "SocialMgr.h"
#include "Utilities/Util.h"
#include "TemporarySummon.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"
#include "ScriptMgr.h"
#include "GameObjectAI.h"
#include "InstanceData.h"
#include "MoveSplineInit.h"

pEffect SpellEffects[TOTAL_SPELL_EFFECTS] =
{
    &Spell::EffectNULL,                                     //  0
    &Spell::EffectInstaKill,                                //  1 SPELL_EFFECT_INSTAKILL
    &Spell::EffectSchoolDMG,                                //  2 SPELL_EFFECT_SCHOOL_DAMAGE
    &Spell::EffectDummy,                                    //  3 SPELL_EFFECT_DUMMY
    &Spell::EffectUnused,                                   //  4 SPELL_EFFECT_PORTAL_TELEPORT          unused
    &Spell::EffectTeleportUnits,                            //  5 SPELL_EFFECT_TELEPORT_UNITS
    &Spell::EffectApplyAura,                                //  6 SPELL_EFFECT_APPLY_AURA
    &Spell::EffectEnvironmentalDMG,                         //  7 SPELL_EFFECT_ENVIRONMENTAL_DAMAGE
    &Spell::EffectPowerDrain,                               //  8 SPELL_EFFECT_POWER_DRAIN
    &Spell::EffectHealthLeech,                              //  9 SPELL_EFFECT_HEALTH_LEECH
    &Spell::EffectHeal,                                     // 10 SPELL_EFFECT_HEAL
    &Spell::EffectBind,                                     // 11 SPELL_EFFECT_BIND
    &Spell::EffectNULL,                                     // 12 SPELL_EFFECT_PORTAL
    &Spell::EffectUnused,                                   // 13 SPELL_EFFECT_RITUAL_BASE              unused
    &Spell::EffectUnused,                                   // 14 SPELL_EFFECT_RITUAL_SPECIALIZE        unused
    &Spell::EffectUnused,                                   // 15 SPELL_EFFECT_RITUAL_ACTIVATE_PORTAL   unused
    &Spell::EffectQuestComplete,                            // 16 SPELL_EFFECT_QUEST_COMPLETE
    &Spell::EffectWeaponDmg,                                // 17 SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL
    &Spell::EffectResurrect,                                // 18 SPELL_EFFECT_RESURRECT
    &Spell::EffectAddExtraAttacks,                          // 19 SPELL_EFFECT_ADD_EXTRA_ATTACKS
    &Spell::EffectUnused,                                   // 20 SPELL_EFFECT_DODGE                    one spell: Dodge
    &Spell::EffectUnused,                                   // 21 SPELL_EFFECT_EVADE                    one spell: Evade (DND)
    &Spell::EffectParry,                                    // 22 SPELL_EFFECT_PARRY
    &Spell::EffectBlock,                                    // 23 SPELL_EFFECT_BLOCK                    one spell: Block
    &Spell::EffectCreateItem,                               // 24 SPELL_EFFECT_CREATE_ITEM
    &Spell::EffectUnused,                                   // 25 SPELL_EFFECT_WEAPON
    &Spell::EffectUnused,                                   // 26 SPELL_EFFECT_DEFENSE                  one spell: Defense
    &Spell::EffectPersistentAA,                             // 27 SPELL_EFFECT_PERSISTENT_AREA_AURA
    &Spell::EffectSummonType,                               // 28 SPELL_EFFECT_SUMMON
    &Spell::EffectMomentMove,                               // 29 SPELL_EFFECT_LEAP
    &Spell::EffectEnergize,                                 // 30 SPELL_EFFECT_ENERGIZE
    &Spell::EffectWeaponDmg,                                // 31 SPELL_EFFECT_WEAPON_PERCENT_DAMAGE
    &Spell::EffectTriggerMissileSpell,                      // 32 SPELL_EFFECT_TRIGGER_MISSILE
    &Spell::EffectOpenLock,                                 // 33 SPELL_EFFECT_OPEN_LOCK
    &Spell::EffectSummonChangeItem,                         // 34 SPELL_EFFECT_SUMMON_CHANGE_ITEM
    &Spell::EffectApplyAura,                                // 35 SPELL_EFFECT_APPLY_AREA_AURA_PARTY
    &Spell::EffectLearnSpell,                               // 36 SPELL_EFFECT_LEARN_SPELL
    &Spell::EffectUnused,                                   // 37 SPELL_EFFECT_SPELL_DEFENSE            one spell: SPELLDEFENSE (DND)
    &Spell::EffectDispel,                                   // 38 SPELL_EFFECT_DISPEL
    &Spell::EffectUnused,                                   // 39 SPELL_EFFECT_LANGUAGE
    &Spell::EffectDualWield,                                // 40 SPELL_EFFECT_DUAL_WIELD
    &Spell::EffectUnused,                                   // 41 SPELL_EFFECT_41 (old SPELL_EFFECT_SUMMON_WILD)
    &Spell::SummonClassPet,                                 // 42 SPELL_EFFECT_SUMMON_CLASS_PET
    &Spell::EffectTeleUnitsFaceCaster,                      // 43 SPELL_EFFECT_TELEPORT_UNITS_FACE_CASTER
    &Spell::EffectLearnSkill,                               // 44 SPELL_EFFECT_SKILL_STEP
    &Spell::EffectAddHonor,                                 // 45 SPELL_EFFECT_ADD_HONOR                honor/pvp related
    &Spell::EffectNULL,                                     // 46 SPELL_EFFECT_SPAWN                    spawn/login animation, expected by spawn unit cast, also base points store some dynflags
    &Spell::EffectTradeSkill,                               // 47 SPELL_EFFECT_TRADE_SKILL
    &Spell::EffectUnused,                                   // 48 SPELL_EFFECT_STEALTH                  one spell: Base Stealth
    &Spell::EffectUnused,                                   // 49 SPELL_EFFECT_DETECT                   one spell: Detect
    &Spell::EffectTransmitted,                              // 50 SPELL_EFFECT_TRANS_DOOR
    &Spell::EffectUnused,                                   // 51 SPELL_EFFECT_FORCE_CRITICAL_HIT       unused from pre-1.2.1
    &Spell::EffectUnused,                                   // 52 SPELL_EFFECT_GUARANTEE_HIT            unused from pre-1.2.1
    &Spell::EffectEnchantItemPerm,                          // 53 SPELL_EFFECT_ENCHANT_ITEM
    &Spell::EffectEnchantItemTmp,                           // 54 SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY
    &Spell::EffectTameCreature,                             // 55 SPELL_EFFECT_TAMECREATURE
    &Spell::EffectSummonPet,                                // 56 SPELL_EFFECT_SUMMON_PET
    &Spell::EffectLearnPetSpell,                            // 57 SPELL_EFFECT_LEARN_PET_SPELL
    &Spell::EffectWeaponDmg,                                // 58 SPELL_EFFECT_WEAPON_DAMAGE
    &Spell::EffectOpenSecretSafe,                           // 59 SPELL_EFFECT_OPEN_LOCK_ITEM
    &Spell::EffectProficiency,                              // 60 SPELL_EFFECT_PROFICIENCY
    &Spell::EffectSendEvent,                                // 61 SPELL_EFFECT_SEND_EVENT
    &Spell::EffectPowerBurn,                                // 62 SPELL_EFFECT_POWER_BURN
    &Spell::EffectThreat,                                   // 63 SPELL_EFFECT_THREAT
    &Spell::EffectTriggerSpell,                             // 64 SPELL_EFFECT_TRIGGER_SPELL
    &Spell::EffectUnused,                                   // 65 SPELL_EFFECT_HEALTH_FUNNEL            unused
    &Spell::EffectUnused,                                   // 66 SPELL_EFFECT_POWER_FUNNEL             unused from pre-1.2.1
    &Spell::EffectHealMaxHealth,                            // 67 SPELL_EFFECT_HEAL_MAX_HEALTH
    &Spell::EffectInterruptCast,                            // 68 SPELL_EFFECT_INTERRUPT_CAST
    &Spell::EffectDistract,                                 // 69 SPELL_EFFECT_DISTRACT
    &Spell::EffectPull,                                     // 70 SPELL_EFFECT_PULL                     one spell: Distract Move
    &Spell::EffectPickPocket,                               // 71 SPELL_EFFECT_PICKPOCKET
    &Spell::EffectAddFarsight,                              // 72 SPELL_EFFECT_ADD_FARSIGHT
    &Spell::EffectUnused,                                   // 73 SPELL_EFFECT_73 (old SPELL_EFFECT_SUMMON_POSSESSED
    &Spell::EffectUnused,                                   // 74 SPELL_EFFECT_74 (old SPELL_EFFECT_SUMMON_TOTEM)
    &Spell::EffectHealMechanical,                           // 75 SPELL_EFFECT_HEAL_MECHANICAL          one spell: Mechanical Patch Kit
    &Spell::EffectSummonObjectWild,                         // 76 SPELL_EFFECT_SUMMON_OBJECT_WILD
    &Spell::EffectScriptEffect,                             // 77 SPELL_EFFECT_SCRIPT_EFFECT
    &Spell::EffectUnused,                                   // 78 SPELL_EFFECT_ATTACK
    &Spell::EffectSanctuary,                                // 79 SPELL_EFFECT_SANCTUARY
    &Spell::EffectAddComboPoints,                           // 80 SPELL_EFFECT_ADD_COMBO_POINTS
    &Spell::EffectUnused,                                   // 81 SPELL_EFFECT_CREATE_HOUSE             one spell: Create House (TEST)
    &Spell::EffectNULL,                                     // 82 SPELL_EFFECT_BIND_SIGHT
    &Spell::EffectDuel,                                     // 83 SPELL_EFFECT_DUEL
    &Spell::EffectStuck,                                    // 84 SPELL_EFFECT_STUCK
    &Spell::EffectSummonPlayer,                             // 85 SPELL_EFFECT_SUMMON_PLAYER
    &Spell::EffectActivateObject,                           // 86 SPELL_EFFECT_ACTIVATE_OBJECT
    &Spell::EffectUnused,                                   // 87 SPELL_EFFECT_87 (old SPELL_EFFECT_SUMMON_TOTEM_SLOT1)
    &Spell::EffectUnused,                                   // 88 SPELL_EFFECT_88 (old SPELL_EFFECT_SUMMON_TOTEM_SLOT2)
    &Spell::EffectUnused,                                   // 89 SPELL_EFFECT_89 (old SPELL_EFFECT_SUMMON_TOTEM_SLOT3)
    &Spell::EffectUnused,                                   // 90 SPELL_EFFECT_90 (old SPELL_EFFECT_SUMMON_TOTEM_SLOT4)
    &Spell::EffectUnused,                                   // 91 SPELL_EFFECT_THREAT_ALL               one spell: zzOLDBrainwash
    &Spell::EffectEnchantHeldItem,                          // 92 SPELL_EFFECT_ENCHANT_HELD_ITEM
    &Spell::EffectUnused,                                   // 93 SPELL_EFFECT_93 (old SPELL_EFFECT_SUMMON_PHANTASM)
    &Spell::EffectSelfResurrect,                            // 94 SPELL_EFFECT_SELF_RESURRECT
    &Spell::EffectSkinning,                                 // 95 SPELL_EFFECT_SKINNING
    &Spell::EffectUnused,                                   // 96 SPELL_EFFECT_CHARGE
    &Spell::EffectUnused,                                   // 97 SPELL_EFFECT_97 (old SPELL_EFFECT_SUMMON_CRITTER)
    &Spell::EffectKnockBack,                                // 98 SPELL_EFFECT_KNOCK_BACK
    &Spell::EffectDisEnchant,                               // 99 SPELL_EFFECT_DISENCHANT
    &Spell::EffectInebriate,                                //100 SPELL_EFFECT_INEBRIATE
    &Spell::EffectFeedPet,                                  //101 SPELL_EFFECT_FEED_PET
    &Spell::EffectDismissPet,                               //102 SPELL_EFFECT_DISMISS_PET
    &Spell::EffectReputation,                               //103 SPELL_EFFECT_REPUTATION
    &Spell::EffectSummonObject,                             //104 SPELL_EFFECT_SUMMON_OBJECT_SLOT1
    &Spell::EffectSummonObject,                             //105 SPELL_EFFECT_SUMMON_OBJECT_SLOT2
    &Spell::EffectSummonObject,                             //106 SPELL_EFFECT_SUMMON_OBJECT_SLOT3
    &Spell::EffectSummonObject,                             //107 SPELL_EFFECT_SUMMON_OBJECT_SLOT4
    &Spell::EffectDispelMechanic,                           //108 SPELL_EFFECT_DISPEL_MECHANIC
    &Spell::EffectSummonDeadPet,                            //109 SPELL_EFFECT_SUMMON_DEAD_PET
    &Spell::EffectDestroyAllTotems,                         //110 SPELL_EFFECT_DESTROY_ALL_TOTEMS
    &Spell::EffectDurabilityDamage,                         //111 SPELL_EFFECT_DURABILITY_DAMAGE
    &Spell::EffectUnused,                                   //112 SPELL_EFFECT_112 (old SPELL_EFFECT_SUMMON_DEMON)
    &Spell::EffectResurrectNew,                             //113 SPELL_EFFECT_RESURRECT_NEW
    &Spell::EffectTaunt,                                    //114 SPELL_EFFECT_ATTACK_ME
    &Spell::EffectDurabilityDamagePCT,                      //115 SPELL_EFFECT_DURABILITY_DAMAGE_PCT
    &Spell::EffectSkinPlayerCorpse,                         //116 SPELL_EFFECT_SKIN_PLAYER_CORPSE       one spell: Remove Insignia, bg usage, required special corpse flags...
    &Spell::EffectSpiritHeal,                               //117 SPELL_EFFECT_SPIRIT_HEAL              one spell: Spirit Heal
    &Spell::EffectSkill,                                    //118 SPELL_EFFECT_SKILL                    professions and more
    &Spell::EffectApplyAura,                                //119 SPELL_EFFECT_APPLY_AREA_AURA_PET
    &Spell::EffectUnused,                                   //120 SPELL_EFFECT_TELEPORT_GRAVEYARD       one spell: Graveyard Teleport Test
    &Spell::EffectWeaponDmg,                                //121 SPELL_EFFECT_NORMALIZED_WEAPON_DMG
    &Spell::EffectUnused,                                   //122 SPELL_EFFECT_122                      unused
    &Spell::EffectSendTaxi,                                 //123 SPELL_EFFECT_SEND_TAXI                taxi/flight related (misc value is taxi path id)
    &Spell::EffectPlayerPull,                               //124 SPELL_EFFECT_PLAYER_PULL              opposite of knockback effect (pulls player twoard caster)
    &Spell::EffectModifyThreatPercent,                      //125 SPELL_EFFECT_MODIFY_THREAT_PERCENT
    &Spell::EffectStealBeneficialBuff,                      //126 SPELL_EFFECT_STEAL_BENEFICIAL_BUFF    spell steal effect?
    &Spell::EffectProspecting,                              //127 SPELL_EFFECT_PROSPECTING              Prospecting spell
    &Spell::EffectApplyAura,                                //128 SPELL_EFFECT_APPLY_AREA_AURA_FRIEND
    &Spell::EffectApplyAura,                                //129 SPELL_EFFECT_APPLY_AREA_AURA_ENEMY
    &Spell::EffectRedirectThreat,                           //130 SPELL_EFFECT_REDIRECT_THREAT
    &Spell::EffectUnused,                                   //131 SPELL_EFFECT_131                      used in some test spells
    &Spell::EffectNULL,                                     //132 SPELL_EFFECT_PLAY_MUSIC               sound id in misc value (SoundEntries.dbc)
    &Spell::EffectUnlearnSpecialization,                    //133 SPELL_EFFECT_UNLEARN_SPECIALIZATION   unlearn profession specialization
    &Spell::EffectKillCredit,                               //134 SPELL_EFFECT_KILL_CREDIT              misc value is creature entry
    &Spell::EffectNULL,                                     //135 SPELL_EFFECT_CALL_PET
    &Spell::EffectHealPct,                                  //136 SPELL_EFFECT_HEAL_PCT
    &Spell::EffectEnergizePct,                              //137 SPELL_EFFECT_ENERGIZE_PCT
    &Spell::EffectNULL,                                     //138 SPELL_EFFECT_LEAP_BACK
    &Spell::EffectUnused,                                   //139 SPELL_EFFECT_CLEAR_QUEST              (misc - is quest ID), unused
    &Spell::EffectForceCast,                                //140 SPELL_EFFECT_FORCE_CAST
    &Spell::EffectNULL,                                     //141 SPELL_EFFECT_141                      damage and reduce speed?
    &Spell::EffectTriggerSpellWithValue,                    //142 SPELL_EFFECT_TRIGGER_SPELL_WITH_VALUE
    &Spell::EffectApplyAura,                                //143 SPELL_EFFECT_APPLY_AREA_AURA_OWNER
    &Spell::EffectKnockBack,                                //144 SPELL_EFFECT_KNOCK_BACK_2             Spectral Blast
    &Spell::EffectPlayerPull,                               //145 SPELL_EFFECT_145                      Black Hole Effect
    &Spell::EffectUnused,                                   //146 SPELL_EFFECT_ACTIVATE_RUNE            unused
    &Spell::EffectQuestFail,                                //147 SPELL_EFFECT_QUEST_FAIL               quest fail
    &Spell::EffectUnused,                                   //148 SPELL_EFFECT_148                      unused
    &Spell::EffectChargeDest,                               //149 SPELL_EFFECT_CHARGE_DEST                      swoop
    &Spell::EffectUnused,                                   //150 SPELL_EFFECT_150                      unused
    &Spell::EffectTriggerRitualOfSummoning,                 //151 SPELL_EFFECT_TRIGGER_SPELL_2
    &Spell::EffectSummonFriend,                             //152 SPELL_EFFECT_SUMMON_FRIEND            summon Refer-a-Friend
    &Spell::EffectNULL,                                     //153 SPELL_EFFECT_CREATE_PET               misc value is creature entry
};

void Spell::EffectNULL(SpellEffIndex /*effIndex*/)
{
    sLog.outDebug("WORLD: Spell Effect DUMMY");
}

void Spell::EffectUnused(SpellEffIndex /*effIndex*/)
{
    // NOT USED BY ANY SPELL OR USELESS OR IMPLEMENTED IN DIFFERENT WAY IN MANGOS
}

void Spell::EffectResurrectNew(SpellEffIndex effIndex)
{
    if (!unitTarget || unitTarget->IsAlive())
        return;

    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    if (!unitTarget->IsInWorld())
        return;

    Player* pTarget = unitTarget->ToPlayer();

    if (pTarget->isRessurectRequested())       // already have one active request
        return;

    uint32 health = damage;
    uint32 mana = m_spellInfo->EffectMiscValue[effIndex];
    pTarget->setResurrectRequestData(m_caster->GetGUID(), m_caster->GetMapId(), m_caster->GetPositionX(), m_caster->GetPositionY(), m_caster->GetPositionZ(), health, mana);
    SendResurrectRequest(pTarget);
}

void Spell::EffectInstaKill(SpellEffIndex /*effIndex*/)
{
    if (!unitTarget || !unitTarget->IsAlive())
        return;

    if (unitTarget->GetTypeId() == TYPEID_PLAYER)
        if (unitTarget->ToPlayer()->GetCommandStatus(CHEAT_GOD))
            return;

    // Demonic Sacrifice
    if (m_spellInfo->Id == 18788 && unitTarget->GetTypeId() == TYPEID_UNIT)
    {
        uint32 entry = unitTarget->GetEntry();
        uint32 spellID;
        switch (entry)
        {
        case   416:
            spellID = 18789;
            break;               //imp
        case   417:
            spellID = 18792;
            break;               //fellhunter
        case  1860:
            spellID = 18790;
            break;               //void
        case  1863:
            spellID = 18791;
            break;               //succubus
        case 17252:
            spellID = 35701;
            break;               //fellguard
        default:
            sLog.outError("EffectInstaKill: Unhandled creature entry (%u) case.", entry);
            return;
        }

        m_caster->CastSpell(m_caster, spellID, true);
    }

    if (m_caster == unitTarget)                                // prevent interrupt message
        finish();

    WorldPacket data(SMSG_SPELLINSTAKILLLOG, 8 + 8 + 4);
    data << uint64(m_caster->GetGUID());
    data << uint64(unitTarget->GetGUID());
    data << uint32(m_spellInfo->Id);
    m_caster->SendMessageToSet(&data, true);

    m_caster->DealDamage(unitTarget, unitTarget->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
}

void Spell::EffectEnvironmentalDMG(SpellEffIndex effIndex)
{
    uint32 absorb = 0;
    uint32 resist = 0;

    if (!unitTarget || !unitTarget->IsAlive())
        return;

    // Note: this hack with damage replace required until GO casting not implemented
    // environment damage spells already have around enemies targeting but this not help in case not existed GO casting support
    // currently each enemy selected explicitly and self cast damage, we prevent apply self casted spell bonuses/etc
    damage = m_spellInfo->EffectBasePoints[effIndex] + m_spellInfo->EffectBaseDice[effIndex];

    m_caster->SendSpellNonMeleeDamageLog(m_caster, m_spellInfo->Id, damage, GetSpellSchoolMask(m_spellInfo), absorb, resist, false, 0, false);
    // CalcAbsorbResist already in Player::EnvironmentalDamage
    if (unitTarget->GetTypeId() == TYPEID_PLAYER)
        unitTarget->ToPlayer()->EnvironmentalDamage(DAMAGE_FIRE, damage);
}

void Spell::EffectSchoolDMG(SpellEffIndex /*effIndex*/)
{
}

void Spell::SpellDamageSchoolDmg(SpellEffIndex effIndex)
{
    if (unitTarget && unitTarget->IsAlive())
    {
        switch (m_spellInfo->SpellFamilyName)
        {
        case SPELLFAMILY_GENERIC:
            {
                //Gore
                if (m_spellInfo->SpellIconID == 2269)
                    damage += (uint32)rand32() % 2 ? damage : 0;

                // Meteor like spells (divided damage to targets)
                if (m_customAttr & SPELL_ATTR_CU_SHARE_DAMAGE)
                {
                    uint32 count = 0;
                    for (std::list<TargetInfo>::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
                    {
                        if (ihit->deleted)
                            continue;

                        if (ihit->effectMask & (1 << effIndex))
                            ++count;
                    }

                    damage /= count;                    // divide to all targets
                }

                switch (m_spellInfo->Id)                    // better way to check unknown
                {
                case 35354: //Hand of Death
                    {
                        if (unitTarget && unitTarget->HasAura(38528, 0)) //Protection of Elune
                            damage = 0;
                        break;
                    }
                // percent from health with min
                case 25599:                             // Thundercrash
                    {
                        damage = unitTarget->GetHealth() / 2;
                        if (damage < 200)
                            damage = 200;
                        break;
                    }
                // arcane charge. must only affect demons (also undead?)
                case 45072:
                    {
                        if (unitTarget->GetCreatureType() != CREATURE_TYPE_DEMON
                            && unitTarget->GetCreatureType() != CREATURE_TYPE_UNDEAD)
                            return;
                        break;
                    }
                // gruul's shatter
                case 33671:
                    {
                        // don't damage self and only players
                        if (unitTarget->GetGUID() == m_caster->GetGUID() || unitTarget->GetTypeId() != TYPEID_PLAYER)
                            return;

                        float radius = GetSpellRadius(m_spellInfo, 0, false);
                        if (!radius) return;
                        float distance = m_caster->GetDistance2d(unitTarget);
                        damage = (distance > radius) ? 0 : int32(m_spellInfo->EffectBasePoints[0] * ((radius - distance) / radius));
                        // Set the damage directly without spell bonus damage
                        m_damage += damage;
                        damage = 0;
                        break;
                    }
                // Cataclysmic Bolt
                case 38441:
                    damage = unitTarget->GetMaxHealth() / 2;
                    break;
                }
                break;
            }

        case SPELLFAMILY_MAGE:
            {
                // Arcane Blast
                if (m_spellInfo->SpellFamilyFlags & 0x20000000LL)
                    m_caster->CastSpell(m_caster, 36032, true);
                break;
            }
        case SPELLFAMILY_WARRIOR:
            {
                // Bloodthirst
                if (m_spellInfo->SpellFamilyFlags & 0x40000000000LL)
                    damage = uint32(damage * (m_caster->GetTotalAttackPowerValue(BASE_ATTACK)) / 100);
                // Shield Slam
                else if (m_spellInfo->SpellFamilyFlags & 0x100000000LL)
                    damage += int32(m_caster->GetShieldBlockValue());
                // Victory Rush
                else if (m_spellInfo->SpellFamilyFlags & 0x10000000000LL)
                {
                    damage = uint32(damage * m_caster->GetTotalAttackPowerValue(BASE_ATTACK) / 100);
                    m_caster->ModifyAuraState(AURA_STATE_WARRIOR_VICTORY_RUSH, false);
                }
                break;
            }
        case SPELLFAMILY_WARLOCK:
            {
                // Incinerate Rank 1 & 2
                if ((m_spellInfo->SpellFamilyFlags & 0x00004000000000LL) && m_spellInfo->SpellIconID == 2128)
                {
                    // Incinerate does more dmg (dmg*0.25) if the target is Immolated.
                    if (unitTarget->HasAuraState(AURA_STATE_IMMOLATE))
                        damage += int32(damage * 0.25);
                }

                // Conflagrate - consumes immolate
                if (m_spellInfo->TargetAuraState == AURA_STATE_IMMOLATE)
                {
                    // for caster applied auras only
                    Unit::AuraList const& mPeriodic = unitTarget->GetAurasByType(SPELL_AURA_PERIODIC_DAMAGE);
                    for (Unit::AuraList::const_iterator i = mPeriodic.begin(); i != mPeriodic.end(); ++i)
                    {
                        if ((*i)->GetSpellProto()->SpellFamilyName == SPELLFAMILY_WARLOCK &&
                            ((*i)->GetSpellProto()->SpellFamilyFlags & 4) &&
                            (*i)->GetCasterGUID() == m_caster->GetGUID())
                        {
                            unitTarget->RemoveAurasByCasterSpell((*i)->GetId(), m_caster->GetGUID());
                            break;
                        }
                    }
                }
                break;
            }
        case SPELLFAMILY_PRIEST:
            {
                // Shadow Word: Death - deals damage equal to damage done to caster
                if (m_spellInfo->SpellFamilyFlags & 0x0000000200000000LL)
                {
                    int32 back_damage = int32(m_caster->SpellDamageBonus(unitTarget, m_spellInfo, (uint32)damage, SPELL_DIRECT_DAMAGE));
                    if (back_damage < int64(unitTarget->GetHealth()))
                        m_caster->CastCustomSpell(m_caster, 32409, &back_damage, 0, 0, true);
                }
                break;
            }
        case SPELLFAMILY_DRUID:
            {
                // Ferocious Bite
                if ((m_spellInfo->SpellFamilyFlags & 0x000800000) && m_spellInfo->SpellVisual == 6587)
                {
                    // converts each extra point of energy into ($f1+$AP/630) additional damage
                    float multiple = m_caster->GetTotalAttackPowerValue(BASE_ATTACK) / 630 + m_spellInfo->DmgMultiplier[effIndex];
                    damage += int32((m_caster->GetPower(POWER_ENERGY) - GetPowerCost()) * multiple);
                    m_caster->SetPower(POWER_ENERGY, GetPowerCost());
                }
                // Rake
                else if (m_spellInfo->SpellFamilyFlags & 0x0000000000001000LL)
                    damage += int32(m_caster->GetTotalAttackPowerValue(BASE_ATTACK) / 100);
                // Swipe
                else if (m_spellInfo->SpellFamilyFlags & 0x0010000000000000LL)
                    damage += int32(m_caster->GetTotalAttackPowerValue(BASE_ATTACK) * 0.08f);
                // Starfire
                else if (m_spellInfo->SpellFamilyFlags & 0x0004LL)
                {
                    Unit::AuraList const& m_OverrideClassScript = m_caster->GetAurasByType(SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
                    for (Unit::AuraList::const_iterator i = m_OverrideClassScript.begin(); i != m_OverrideClassScript.end(); ++i)
                    {
                        // Starfire Bonus (caster)
                        switch ((*i)->GetModifier()->m_miscvalue)
                        {
                        case 5481:                      // Nordrassil Regalia - bonus
                            {
                                Unit::AuraList const& m_periodicDamageAuras = unitTarget->GetAurasByType(SPELL_AURA_PERIODIC_DAMAGE);
                                for (Unit::AuraList::const_iterator itr = m_periodicDamageAuras.begin(); itr != m_periodicDamageAuras.end(); ++itr)
                                {
                                    // Moonfire or Insect Swarm (target debuff from any casters)
                                    if ((*itr)->GetSpellProto()->SpellFamilyFlags & 0x00200002LL)
                                    {
                                        int32 mod = (*i)->GetModifier()->m_amount;
                                        damage += damage * mod / 100;
                                        break;
                                    }
                                }
                                break;
                            }
                        case 5148:                      //Improved Starfire - Ivory Idol of the Moongoddes Aura
                            {
                                damage += (*i)->GetModifier()->m_amount;
                                break;
                            }
                        }
                    }
                }
                //Mangle Bonus for the initial damage of Lacerate and Rake
                if ((m_spellInfo->SpellFamilyFlags == 0x0000000000001000LL && m_spellInfo->SpellIconID == 494) ||
                    (m_spellInfo->SpellFamilyFlags == 0x0000010000000000LL && m_spellInfo->SpellIconID == 2246))
                {
                    Unit::AuraList const& mDummyAuras = unitTarget->GetAurasByType(SPELL_AURA_DUMMY);
                    for (Unit::AuraList::const_iterator i = mDummyAuras.begin(); i != mDummyAuras.end(); ++i)
                        if ((*i)->GetSpellProto()->SpellFamilyFlags & 0x0000044000000000LL && (*i)->GetSpellProto()->SpellFamilyName == SPELLFAMILY_DRUID)
                        {
                            damage = int32(damage * (100.0f + (*i)->GetModifier()->m_amount) / 100.0f);
                            break;
                        }
                }
                break;
            }
        case SPELLFAMILY_ROGUE:
            {
                // Envenom
                if (m_caster->GetTypeId() == TYPEID_PLAYER && (m_spellInfo->SpellFamilyFlags & 0x800000000LL))
                {
                    // consume from stack dozes not more that have combo-points
                    if (uint32 combo = m_caster->ToPlayer()->GetComboPoints())
                    {
                        // count consumed deadly poison doses at target
                        uint32 doses = 0;

                        // remove consumed poison doses
                        Unit::AuraList const& auras = unitTarget->GetAurasByType(SPELL_AURA_PERIODIC_DAMAGE);
                        for (Unit::AuraList::const_iterator itr = auras.begin(); itr != auras.end() && combo;)
                        {
                            // Deadly poison (only attacker applied)
                            if ((*itr)->GetSpellProto()->SpellFamilyName == SPELLFAMILY_ROGUE && ((*itr)->GetSpellProto()->SpellFamilyFlags & 0x10000) &&
                                (*itr)->GetSpellProto()->SpellVisual == 5100 && (*itr)->GetCasterGUID() == m_caster->GetGUID())
                            {
                                --combo;
                                ++doses;

                                unitTarget->RemoveSingleAuraFromStack((*itr)->GetId(), (*itr)->GetEffIndex());

                                itr = auras.begin();
                            }
                            else
                                ++itr;
                        }

                        damage *= doses;
                        damage += int32(m_caster->ToPlayer()->GetTotalAttackPowerValue(BASE_ATTACK) * 0.03f * doses);

                        // Eviscerate and Envenom Bonus Damage (item set effect)
                        if (m_caster->GetDummyAura(37169))
                            damage += m_caster->ToPlayer()->GetComboPoints() * 40;
                    }
                }
                // Eviscerate
                else if ((m_spellInfo->SpellFamilyFlags & 0x00020000LL) && m_caster->GetTypeId() == TYPEID_PLAYER)
                {
                    if (uint32 combo = m_caster->ToPlayer()->GetComboPoints())
                    {
                        damage += int32(m_caster->GetTotalAttackPowerValue(BASE_ATTACK) * combo * 0.03f);

                        // Eviscerate and Envenom Bonus Damage (item set effect)
                        if (m_caster->GetDummyAura(37169))
                            damage += combo * 40;
                    }
                }
                break;
            }
        case SPELLFAMILY_HUNTER:
            {
                // Mongoose Bite
                if ((m_spellInfo->SpellFamilyFlags & 0x000000002) && m_spellInfo->SpellVisual == 342)
                    damage += int32(m_caster->GetTotalAttackPowerValue(BASE_ATTACK) * 0.2);
                // Arcane Shot
                else if ((m_spellInfo->SpellFamilyFlags & 0x00000800) && m_spellInfo->maxLevel > 0)
                    damage += int32(m_caster->GetTotalAttackPowerValue(RANGED_ATTACK) * 0.15);
                // Steady Shot
                else if (m_spellInfo->SpellFamilyFlags & 0x100000000LL)
                {
                    int32 base = irand((int32)m_caster->GetWeaponDamageRange(RANGED_ATTACK, MINDAMAGE), (int32)m_caster->GetWeaponDamageRange(RANGED_ATTACK, MAXDAMAGE));
                    damage += int32(float(base) / m_caster->GetAttackTime(RANGED_ATTACK) * 2800 + m_caster->GetTotalAttackPowerValue(RANGED_ATTACK) * 0.2f);

                    bool found = false;

                    // check dazed affect
                    Unit::AuraList const& decSpeedList = unitTarget->GetAurasByType(SPELL_AURA_MOD_DECREASE_SPEED);
                    for (Unit::AuraList::const_iterator iter = decSpeedList.begin(); iter != decSpeedList.end(); ++iter)
                    {
                        if ((*iter)->GetSpellProto()->SpellIconID == 15 && (*iter)->GetSpellProto()->Dispel == 0)
                        {
                            found = true;
                            break;
                        }
                    }

                    // @todo should this be put on taken but not done?
                    if (found)
                        damage += m_spellInfo->EffectBasePoints[1];
                }
                //Explosive Trap Effect
                else if (m_spellInfo->SpellFamilyFlags & 0x00000004)
                    damage += int32(m_caster->GetTotalAttackPowerValue(RANGED_ATTACK) * 0.1);
                break;
            }
        case SPELLFAMILY_PALADIN:
            {
                //Judgement of Vengeance
                if ((m_spellInfo->SpellFamilyFlags & 0x800000000LL) && m_spellInfo->SpellIconID == 2292)
                {
                    uint32 stacks = 0;
                    Unit::AuraList const& auras = unitTarget->GetAurasByType(SPELL_AURA_PERIODIC_DAMAGE);
                    for (Unit::AuraList::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
                    {
                        if ((*itr)->GetId() == 31803 && (*itr)->GetCasterGUID() == m_caster->GetGUID())
                        {
                            stacks = (*itr)->GetStackAmount();
                            break;
                        }
                    }

                    if (!stacks)
                        //No damage if the target isn't affected by this
                        damage = -1;
                    else
                        damage *= stacks;
                }
                break;
            }
        case SPELLFAMILY_SHAMAN:
            {
                // Lightning Bolt & Chain Lightning
                if (m_spellInfo->SpellFamilyFlags & 0x0003LL)
                {
                    Unit::AuraList const& auras = unitTarget->GetAurasByType(SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
                    for (Unit::AuraList::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
                    {
                        if ((*itr)->GetId() == 28857 || (*itr)->GetId() == 34230 || (*itr)->GetId() == 41040)
                        {
                            damage += (*itr)->GetModifierValue();
                            break;
                        }
                    }
                }
                break;
            }
        }

        if (m_originalCaster && damage > 0)
            damage = m_originalCaster->SpellDamageBonus(unitTarget, m_spellInfo, (uint32)damage, SPELL_DIRECT_DAMAGE);

        m_damage += damage;
    }
}

void Spell::EffectDummy(SpellEffIndex effIndex)
{
    if (!unitTarget && !gameObjTarget && !itemTarget)
        return;

    uint32 spell_id = 0;
    int32 bp = 0;

    // selection by spell family
    switch (m_spellInfo->SpellFamilyName)
    {
    case SPELLFAMILY_GENERIC:
        {
            switch (m_spellInfo->Id)
            {
            // Wrath of the Astromancer
            case 42784:
                {
                    uint32 count = 0;
                    for (std::list<TargetInfo>::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
                    {
                        if (ihit->deleted)
                            continue;

                        if (ihit->effectMask & (1 << effIndex))
                            ++count;
                    }

                    damage = 12000; // maybe wrong value
                    damage /= count;

                    SpellEntry const* spellInfo = sSpellStore.LookupEntry(42784);

                    // now deal the damage
                    for (std::list<TargetInfo>::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
                    {
                        if (ihit->deleted)
                            continue;

                        if (ihit->effectMask & (1 << effIndex))
                        {
                            if (Unit* casttarget = Unit::GetUnit((*unitTarget), ihit->targetGUID))
                                m_caster->DealDamage(casttarget, damage, NULL, SPELL_DIRECT_DAMAGE, SPELL_SCHOOL_MASK_ARCANE, spellInfo, false);
                        }
                    }

                    return;
                }
            case 7671:                                  // Transformation (human<->worgen)
            {
                if (!unitTarget)
                    return;

                // Transform Visual
                unitTarget->CastSpell(unitTarget, 24085, true);
                return;
            }
            case 8063:                                  // Deviate Fish
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    uint32 spell_id = 0;
                    switch (urand(1, 5))
                    {
                    case 1:
                        spell_id = 8064;
                        break;     // Sleepy
                    case 2:
                        spell_id = 8065;
                        break;     // Invigorate
                    case 3:
                        spell_id = 8066;
                        break;     // Shrink
                    case 4:
                        spell_id = 8067;
                        break;     // Party Time!
                    case 5:
                        spell_id = 8068;
                        break;     // Healthy Spirit
                    }
                    m_caster->CastSpell(m_caster, spell_id, true, NULL);
                    return;
                }
            case 8213:                                  // Savory Deviate Delight
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    uint32 spell_id = 0;
                    switch (urand(1, 2))
                    {
                    // Flip Out - ninja
                    case 1:
                        spell_id = (m_caster->getGender() == GENDER_MALE ? 8219 : 8220);
                        break;
                    // Yaaarrrr - pirate
                    case 2:
                        spell_id = (m_caster->getGender() == GENDER_MALE ? 8221 : 8222);
                        break;
                    }

                    m_caster->CastSpell(m_caster, spell_id, true, NULL);
                    return;
                }
            case 8593:                                  // Symbol of life (restore creature to life)
            case 31225:                                 // Shimmering Vessel (restore creature to life)
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;
                    unitTarget->ToCreature()->setDeathState(JUST_RESPAWNED);
                    return;
                }
            case 33655: // Q: Mission: Gateways Murketh and Shaadraz
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER || !((Player*)m_caster)->IsFlying())
                        return;

                    Player* player = m_caster->ToPlayer();

                    if (m_caster->GetDistance(-145.554f, 1511.28f, 34.3641f) < 25)
                        player->KilledMonsterCredit(19291, 0);
                    if (m_caster->GetDistance(-304.408f, 1524.45f, 37.9685f) < 25)
                        player->KilledMonsterCredit(19292, 0);
                    return;
                }
            case 12162:                                 // Deep wounds
            case 12850:                                 // (now good common check for this spells)
            case 12868:
                {
                    if (!unitTarget)
                        return;

                    float damage;
                    // DW should benefit of attack power, damage percent mods etc.
                    // @todo check if using offhand damage is correct and if it should be divided by 2
                    if (m_caster->haveOffhandWeapon() && m_caster->getAttackTimer(BASE_ATTACK) > m_caster->getAttackTimer(OFF_ATTACK))
                        damage = (m_caster->GetFloatValue(UNIT_FIELD_MINOFFHANDDAMAGE) + m_caster->GetFloatValue(UNIT_FIELD_MAXOFFHANDDAMAGE)) / 2;
                    else
                        damage = (m_caster->GetFloatValue(UNIT_FIELD_MINDAMAGE) + m_caster->GetFloatValue(UNIT_FIELD_MAXDAMAGE)) / 2;

                    switch (m_spellInfo->Id)
                    {
                    case 12162:
                        damage *= 0.2f;
                        break; // Rank 1
                    case 12850:
                        damage *= 0.4f;
                        break; // Rank 2
                    case 12868:
                        damage *= 0.6f;
                        break; // Rank 3
                    default:
                        sLog.outError("Spell::EffectDummy: Spell %u not handled in DW", m_spellInfo->Id);
                        return;
                    };

                    int32 deepWoundsDotBasePoints0 = int32(damage / 4);
                    m_caster->CastCustomSpell(unitTarget, 12721, &deepWoundsDotBasePoints0, NULL, NULL, true, NULL);
                    return;
                }
            case 12975:                                 //Last Stand
                {
                    int32 healthModSpellBasePoints0 = int32(m_caster->GetMaxHealth() * 0.3);
                    m_caster->CastCustomSpell(m_caster, 12976, &healthModSpellBasePoints0, NULL, NULL, true, NULL);
                    return;
                }
            case 13120:                                 // net-o-matic
                {
                    if (!unitTarget)
                        return;

                    uint32 spell_id = 0;

                    uint32 roll = urand(0, 99);

                    if (roll < 2)                           // 2% for 30 sec self root (off-like chance unknown)
                        spell_id = 16566;
                    else if (roll < 4)                      // 2% for 20 sec root, charge to target (off-like chance unknown)
                        spell_id = 13119;
                    else                                    // normal root
                        spell_id = 13099;

                    m_caster->CastSpell(unitTarget, spell_id, true, NULL);
                    return;
                }
            case 13278:                                 // Gnomish Death Ray (First Dummy)
                m_caster->CastSpell(m_caster, 13493, true, NULL);
                break;
            case 13280:                                 // Gnomish Death Ray (Second Dummy)
                {
                    if (!unitTarget)
                        return;

                    if (urand(0, 100) < 15)
                        m_caster->CastSpell(m_caster, 13279, true, NULL); // fail - shitty gnomish technology
                    else
                        m_caster->CastSpell(unitTarget, 13279, true, NULL); // Boom!

                    return;
                }
            case 13567:                                 // Dummy Trigger
                {
                    // can be used for different aura triggering, so select by aura
                    if (!m_triggeredByAuraSpell || !unitTarget)
                        return;

                    switch (m_triggeredByAuraSpell->Id)
                    {
                    case 26467:                         // Persistent Shield
                        m_caster->CastCustomSpell(unitTarget, 26470, &damage, NULL, NULL, true);
                        break;
                    default:
                        sLog.outError("EffectDummy: Non-handled case for spell 13567 for triggered aura %u", m_triggeredByAuraSpell->Id);
                        break;
                    }
                    return;
                }
            case 14537:                                 // Six Demon Bag
                {
                    if (!unitTarget || !unitTarget->IsAlive())
                        return;

                    uint32 ClearSpellId[6] =
                    {
                        15662,   // Fireball
                        11538,   // Frostball
                        21179,   // Chain Lightning
                        14621,   // Polymorph
                        25189,   // Enveloping Winds
                        14642    // Summon Felhund minion
                    };

                    uint32 rand = urand(0, 100);

                    if (rand < 25)                      // Fireball (25% chance)
                        spell_id = ClearSpellId[0];
                    else if (rand >= 25 && rand < 50)   // Frostball (25% chance)
                        spell_id = ClearSpellId[1];
                    else if (rand >= 50 && rand < 70)   // Chain Lighting (25% chance)
                        spell_id = ClearSpellId[2];
                    else if (rand >= 70 && rand < 80)   // Polymorph (10% chance)
                    {
                        spell_id = ClearSpellId[3];
                        if (urand(0, 100) <= 30)        // 30% chance to self-cast
                            unitTarget = m_caster;
                    }
                    else if (rand >= 80 && rand < 95)   // Enveloping Winds (15% chance)
                        spell_id = ClearSpellId[4];
                    else                                // Summon Felhund minion (5% chance)
                    {
                        spell_id = ClearSpellId[5];
                        unitTarget = m_caster;
                    }

                    m_caster->CastSpell(unitTarget, spell_id, true, NULL);
                    return;
                }
            case 14185:                                 // Preparation Rogue
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    //immediately finishes the cooldown on certain Rogue abilities
                    const PlayerSpellMap& sp_list = m_caster->ToPlayer()->GetSpellMap();
                    for (PlayerSpellMap::const_iterator itr = sp_list.begin(); itr != sp_list.end(); ++itr)
                    {
                        uint32 classspell = itr->first;
                        SpellEntry const* spellInfo = sSpellStore.LookupEntry(classspell);

                        if (spellInfo->SpellFamilyName == SPELLFAMILY_ROGUE && (spellInfo->SpellFamilyFlags & 0x26000000860LL))
                            m_caster->ToPlayer()->RemoveSpellCooldown(classspell, true);
                    }
                    return;
                }
            case 15998:                                 // Capture Worg Pup
            case 29435:                                 // Capture Female Kaliri Hatchling
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    unitTarget->ToCreature()->ForcedDespawn();
                    return;
                }
            case 16589:                                 // Noggenfogger Elixir
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    uint32 spell_id = 0;
                    switch (urand(1, 3))
                    {
                    case 1:
                        spell_id = 16595;
                        break;
                    case 2:
                        spell_id = 16593;
                        break;
                    default:
                        spell_id = 16591;
                        break;
                    }

                    m_caster->CastSpell(m_caster, spell_id, true, NULL);
                    return;
                }
            case 17251:                                 // Spirit Healer Res
                {
                    if (!unitTarget || !m_originalCaster)
                        return;

                    if (m_originalCaster->GetTypeId() == TYPEID_PLAYER)
                    {
                        WorldPacket data(SMSG_SPIRIT_HEALER_CONFIRM, 8);
                        data << uint64(unitTarget->GetGUID());
                        m_originalCaster->ToPlayer()->GetSession()->SendPacket(&data);
                    }
                    return;
                }
            case 17271:                                 // Test Fetid Skull
                {
                    if (!itemTarget && m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    uint32 spell_id = roll_chance_i(50)
                                      ? 17269                             // Create Resonating Skull
                                      : 17270;                            // Create Bone Dust

                    m_caster->CastSpell(m_caster, spell_id, true, NULL);
                    return;
                }
            case 20577:                                 // Cannibalize
                {
                    if (unitTarget)
                        m_caster->CastSpell(m_caster, 20578, false, NULL);

                    return;
                }
            case 23019:                                 // Crystal Prison Dummy DND
                {
                    if (!unitTarget || !unitTarget->IsAlive() || unitTarget->GetTypeId() != TYPEID_UNIT || unitTarget->IsPet())
                        return;

                    Creature* creatureTarget = unitTarget->ToCreature();

                    GameObject* Crystal_Prison = m_caster->SummonGameObject(179644, creatureTarget->GetPositionX(), creatureTarget->GetPositionY(), creatureTarget->GetPositionZ(), creatureTarget->GetOrientation(), 0, 0, 0, 0, creatureTarget->GetRespawnTime() - time(NULL));
                    sLog.outDebug("SummonGameObject at SpellEfects.cpp EffectDummy for Spell 23019");

                    creatureTarget->ForcedDespawn();

                    WorldPacket data(SMSG_GAMEOBJECT_SPAWN_ANIM_OBSOLETE, 8);
                    data << uint64(Crystal_Prison->GetGUID());
                    m_caster->SendMessageToSet(&data, true);

                    return;
                }
            case 23074:                                 // Arcanite Dragonling
                {
                    if (!m_CastItem)
                        return;

                    m_caster->CastSpell(m_caster, 19804, true, m_CastItem);
                    return;
                }
            case 23075:                                 // Mithril Mechanical Dragonling
                {
                    if (!m_CastItem)
                        return;

                    m_caster->CastSpell(m_caster, 12749, true, m_CastItem);
                    return;
                }
            case 23076:                                 // Mechanical Dragonling
                {
                    if (!m_CastItem)
                        return;

                    m_caster->CastSpell(m_caster, 4073, true, m_CastItem);
                    return;
                }
            case 23133:                                 // Gnomish Battle Chicken
                {
                    if (!m_CastItem)
                        return;

                    m_caster->CastSpell(m_caster, 13166, true, m_CastItem);
                    return;
                }
            case 23448:                                 // Transporter Arrival - Ultrasafe Transporter: Gadgetzan - backfires
                {
                    int32 r = irand(0, 119);
                    if (r < 20)                             // Transporter Malfunction - 1/6 polymorph
                        m_caster->CastSpell(m_caster, 23444, true);
                    else if (r < 100)                       // Evil Twin               - 4/6 evil twin
                        m_caster->CastSpell(m_caster, 23445, true);
                    else                                    // 1/6 miss the target
                        m_caster->CastSpell(m_caster, 36902, true);

                    return;
                }
            case 23453:                                 // Gnomish Transporter - Ultrasafe Transporter: Gadgetzan
                {
                    if (roll_chance_i(50))                  // Gadgetzan Transporter         - success
                        m_caster->CastSpell(m_caster, 23441, true);
                    else                                    // failure
                        m_caster->CastSpell(m_caster, 23446, true);

                    return;
                }
            case 23645:                                 // Hourglass Sand
                m_caster->RemoveAurasDueToSpell(23170); // Brood Affliction: Bronze
                return;
            case 23725:                                 // Gift of Life (warrior bwl trinket)
                m_caster->CastSpell(m_caster, 23782, true);
                m_caster->CastSpell(m_caster, 23783, true);
                return;
            case 24930:                                 // Hallow's End Candy
                if (m_caster->GetTypeId() != TYPEID_PLAYER)
                    return;

                switch (irand(0, 3))
                {
                case 0:
                    m_caster->CastSpell(m_caster, 24927, true); // Ghost
                    break;
                case 1:
                    m_caster->CastSpell(m_caster, 24926, true); // Pirate
                    if (m_caster->getGender() == GENDER_MALE)
                        m_caster->CastSpell(m_caster, 44743, true);
                    else
                        m_caster->CastSpell(m_caster, 44742, true);
                    break;
                case 2:
                    m_caster->CastSpell(m_caster, 24925, true); // Skeleton
                    break;
                case 3:
                    m_caster->CastSpell(m_caster, 24924, true); // Huge and Orange
                    break;
                }
                return;
            case 25860:                                 // Reindeer Transformation
                {
                    if (!m_caster->HasAuraType(SPELL_AURA_MOUNTED))
                        return;

                    float flyspeed = m_caster->GetSpeedRate(MOVE_FLIGHT);
                    float speed = m_caster->GetSpeedRate(MOVE_RUN);

                    m_caster->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);

                    //5 different spells used depending on mounted speed and if mount can fly or not
                    if (flyspeed >= 4.1f)
                        // Flying Reindeer
                        m_caster->CastSpell(m_caster, 44827, true); //310% flying Reindeer
                    else if (flyspeed >= 3.8f)
                        // Flying Reindeer
                        m_caster->CastSpell(m_caster, 44825, true); //280% flying Reindeer
                    else if (flyspeed >= 1.6f)
                        // Flying Reindeer
                        m_caster->CastSpell(m_caster, 44824, true); //60% flying Reindeer
                    else if (speed >= 2.0f)
                        // Reindeer
                        m_caster->CastSpell(m_caster, 25859, true); //100% ground Reindeer
                    else
                        // Reindeer
                        m_caster->CastSpell(m_caster, 25858, true); //60% ground Reindeer

                    return;
                }
            //case 26074:                               // Holiday Cheer
            //    return; -- implemented at client side
            case 28006:                                 // Arcane Cloaking
                {
                    if (unitTarget && unitTarget->GetTypeId() == TYPEID_PLAYER)
                        m_caster->CastSpell(unitTarget, 29294, true);

                    return;
                }
            // Polarity Shift
            case 28089:
                if (unitTarget)
                    unitTarget->CastSpell(unitTarget, roll_chance_i(50) ? 28059 : 28084, true, NULL, NULL, m_caster->GetGUID());
                break;
            // Polarity Shift
            case 39096:
                if (unitTarget)
                    unitTarget->CastSpell(unitTarget, roll_chance_i(50) ? 39088 : 39091, true, NULL, NULL, m_caster->GetGUID());
                break;
            case 29200:                                 // Purify Helboar Meat
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    uint32 spell_id = roll_chance_i(50)
                                      ? 29277                             // Summon Purified Helboar Meat
                                      : 29278;                            // Summon Toxic Helboar Meat

                    m_caster->CastSpell(m_caster, spell_id, true, NULL);
                    return;
                }
            case 29858:                                 // Soulshatter
                {
                    if (unitTarget && unitTarget->CanHaveThreatList()
                        && unitTarget->getThreatManager().getThreat(m_caster) > 0.0f)
                        m_caster->CastSpell(unitTarget, 32835, true);

                    return;
                }
            case 29970:                                 // Deactivate Blizzard (Naxxramas: Sapphiron)
                {
                    if (!unitTarget)
                        return;

                    unitTarget->RemoveAurasDueToSpell(29952);
                    return;
                }
            case 30004:                                 // Flame Wreath
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    m_caster->CastSpell(unitTarget, 29946, true);
                    return;
                }
            case 30458:                                 // Nigh Invulnerability
                {
                    if (!m_CastItem)
                        return;

                    if (roll_chance_i(86))                  // Nigh-Invulnerability   - success
                        m_caster->CastSpell(m_caster, 30456, true, m_CastItem);
                    else                                    // backfire in 14% casts
                        m_caster->CastSpell(m_caster, 30457, true, m_CastItem);

                    return;
                }
            case 30507:                                 // Poultryizer
                {
                    if (!m_CastItem)
                        return;

                    if (roll_chance_i(80))                  // Poultryized! - success
                        m_caster->CastSpell(unitTarget, 30501, true, m_CastItem);
                    else                                    // backfire 20%
                        m_caster->CastSpell(unitTarget, 30504, true, m_CastItem);

                    return;
                }
            case 33060:                                 // Make a Wish
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    uint32 spell_id = 0;

                    switch (urand(1, 5))
                    {
                    case 1:
                        spell_id = 33053;
                        break;    // Mr Pinchy's Blessing
                    case 2:
                        spell_id = 33057;
                        break;    // Summon Mighty Mr. Pinchy
                    case 3:
                        spell_id = 33059;
                        break;    // Summon Furious Mr. Pinchy
                    case 4:
                        spell_id = 33062;
                        break;    // Tiny Magical Crawdad
                    case 5:
                        spell_id = 33064;
                        break;    // Mr. Pinchy's Gift
                    }

                    m_caster->CastSpell(m_caster, spell_id, true, NULL);
                    return;
                }
            case 35745:                                 // Socrethar's Stone
                {
                    uint32 spell_id;
                    switch (m_caster->GetAreaId())
                    {
                    case 3900:
                        spell_id = 35743;
                        break; // Socrethar Portal
                    case 3742:
                        spell_id = 35744;
                        break; // Socrethar Portal
                    default:
                        return;
                    }

                    m_caster->CastSpell(m_caster, spell_id, true);
                    return;
                }
            case 37674:                                 // Chaos Blast
                {
                    if (!unitTarget)
                        return;

                    int32 basepoints0 = 100;
                    m_caster->CastCustomSpell(unitTarget, 37675, &basepoints0, NULL, NULL, true);
                    return;
                }
            case 40109:                                 // Knockdown Fel Cannon: The Bolt
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, 40075, true);
                    return;
                }
            case 40802:                                 // Mingo's Fortune Generator (Mingo's Fortune Giblets)
                {
                    // selecting one from Bloodstained Fortune item
                    uint32 newitemid;
                    switch (urand(1, 20))
                    {
                    case 1:
                        newitemid = 32688;
                        break;
                    case 2:
                        newitemid = 32689;
                        break;
                    case 3:
                        newitemid = 32690;
                        break;
                    case 4:
                        newitemid = 32691;
                        break;
                    case 5:
                        newitemid = 32692;
                        break;
                    case 6:
                        newitemid = 32693;
                        break;
                    case 7:
                        newitemid = 32700;
                        break;
                    case 8:
                        newitemid = 32701;
                        break;
                    case 9:
                        newitemid = 32702;
                        break;
                    case 10:
                        newitemid = 32703;
                        break;
                    case 11:
                        newitemid = 32704;
                        break;
                    case 12:
                        newitemid = 32705;
                        break;
                    case 13:
                        newitemid = 32706;
                        break;
                    case 14:
                        newitemid = 32707;
                        break;
                    case 15:
                        newitemid = 32708;
                        break;
                    case 16:
                        newitemid = 32709;
                        break;
                    case 17:
                        newitemid = 32710;
                        break;
                    case 18:
                        newitemid = 32711;
                        break;
                    case 19:
                        newitemid = 32712;
                        break;
                    case 20:
                        newitemid = 32713;
                        break;
                    default:
                        return;
                    }

                    DoCreateItem(effIndex, newitemid);
                    return;
                }
            case 40834: // Agonizing Flames
                {
                    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    m_caster->CastSpell(unitTarget, 40932, true);
                    break;
                }
            // Salvage Wreckage
            case 42287:
            {
                if (!gameObjTarget && m_caster->GetTypeId() != TYPEID_PLAYER)
                    return;

                if (roll_chance_i(66))
                    m_caster->CastSpell(m_caster, 42289, true, m_CastItem); // Summon Mirefin Burrower
                else
                    m_caster->CastSpell(m_caster, 42288, true); // Summon Salvaged Lockbox

                gameObjTarget->DestroyForNearbyPlayers();

                return;
            }
            // Demon Broiled Surprise
            case 43723:
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    Player* player = m_caster->ToPlayer();

                    if (player && player->GetQuestStatus(11379) == QUEST_STATUS_INCOMPLETE)
                    {
                        Creature* creature = player->FindNearestCreature(19973, 10, false);
                        if (!creature)
                        {
                            SendCastResult(SPELL_FAILED_NOT_HERE);
                            return;
                        }

                        player->CastSpell(player, 43753, false);
                    }
                    return;
                }
            case 44875:                                 // Complete Raptor Capture
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    unitTarget->ToCreature()->ForcedDespawn();

                    //cast spell Raptor Capture Credit
                    m_caster->CastSpell(m_caster, 42337, true, NULL);
                    return;
                }
            case 37473:                                 // Detect Whispers (related to quest 10607 - Whispers of the Raven God_Whispers of the Raven God)
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, damage, true);
                    break;
                }
            case 37573:                                 //Temporal Phase Modulator
                {
                    if (!unitTarget)
                        return;

                    TempSummon* tempSummon = dynamic_cast<TempSummon*>(unitTarget);
                    if (!tempSummon)
                        return;

                    uint32 health = tempSummon->GetHealth();
                    const uint32 entry_list[6] = {21821, 21820, 21817};

                    float x = tempSummon->GetPositionX();
                    float y = tempSummon->GetPositionY();
                    float z = tempSummon->GetPositionZ();
                    float o = tempSummon->GetOrientation();

                    tempSummon->UnSummon();

                    Creature* pCreature = m_caster->SummonCreature(entry_list[urand(0, 2)], x, y, z, o, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 180000);
                    if (!pCreature)
                        return;

                    pCreature->SetHealth(health);

                    if (pCreature->IsAIEnabled)
                        pCreature->AI()->AttackStart(m_caster);

                    return;
                }
            case 34665:                                 //Administer Antidote
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT
                        || unitTarget->GetEntry() != 16880 || unitTarget->IsPet())
                        return;

                    unitTarget->ToCreature()->UpdateEntry(16992);
                    m_caster->ToPlayer()->RewardPlayerAndGroupAtEvent(16992, unitTarget);

                    if (unitTarget->IsAIEnabled)
                        unitTarget->ToCreature()->AI()->AttackStart(m_caster);

                    return;
                }
            case 44997: // Converting Sentry
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    Creature* creatureTarget = (Creature*)unitTarget;

                    creatureTarget->ForcedDespawn();

                    //Converted Sentry Credit
                    m_caster->CastSpell(m_caster, 45009, true);
                    return;
                }
            case 45030:                                 // Impale Emissary
                {
                    // Emissary of Hate Credit
                    m_caster->CastSpell(m_caster, 45088, true);
                    return;
                }
            case 50243:                                 // Teach Language
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    // spell has a 1/3 chance to trigger one of the below
                    if (roll_chance_i(66))
                        return;
                    if (m_caster->ToPlayer()->GetTeam() == ALLIANCE)
                    {
                        // 1000001 - gnomish binary
                        m_caster->CastSpell(m_caster, 50242, true);
                    }
                    else
                    {
                        // 01001000 - goblin binary
                        m_caster->CastSpell(m_caster, 50246, true);
                    }

                    return;
                }
            case 51582:                                 //Rocket Boots Engaged (Rocket Boots Xtreme and Rocket Boots Xtreme Lite)
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    if (Battleground* bg = m_caster->ToPlayer()->GetBattleground())
                        bg->EventPlayerDroppedFlag(m_caster->ToPlayer());

                    m_caster->CastSpell(m_caster, 30452, true, NULL);
                    return;
                }

             case 35372: // Protectorate Igniter
                {
                    if (Creature* victim = unitTarget->ToCreature())
                    {
                        victim->setDeathState(JUST_DIED);
                        victim->RemoveCorpse(true);
                    }
                }
                break;
            case 45226: // banging the gong
                if (InstanceData* pInstance = m_caster->GetInstanceData())
                    pInstance->ProcessEvent(unitTarget, 1);
                break;
            // Arcane Torrent (Mana)
            case 28730:
                {
                    Unit* caster = GetCaster();
                    if (!caster)
                        break;

                    Aura* dummy = caster->GetDummyAura(28734);
                    if (dummy)
                    {
                        int32 bp = m_spellInfo->EffectBasePoints[effIndex] * dummy->GetStackAmount();
                        caster->CastCustomSpell(caster, 28733, &bp, NULL, NULL, true);
                        caster->RemoveAurasDueToSpell(28734);
                    }
                    break;
                }
            // Arcane Torrent (Energy)
            case 25046:
                {
                    Unit* caster = GetCaster();
                    if (!caster)
                        break;

                    // Search Mana Tap auras on caster
                    Aura* dummy = caster->GetDummyAura(28734);
                    if (dummy)
                    {
                        int32 bp = dummy->GetStackAmount() * 10;
                        caster->CastCustomSpell(caster, 25048, &bp, NULL, NULL, true);
                        caster->RemoveAurasDueToSpell(28734);
                    }
                    break;
                }
            case 39105: //Activate Nether-wraith Beacon
                {
                    Player* player = m_caster->ToPlayer();
                    if (!player)
                        break;
                    for (int i = 0; i < 2; i++)
                    {
                        float x, y, z;
                        player->GetClosePoint(x, y, z, player->GetObjectBoundingRadius(), 20.0f, i * static_cast<float>(M_PI));
                        Creature* nether = player->SummonCreature(22408, x, y, z, 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 180000);
                        if (nether)
                            nether->AI()->AttackStart(player);
                    }
                    break;
                }
            }

            //All IconID Check in there
            switch (m_spellInfo->SpellIconID)
            {
            // Berserking (troll racial traits)
            case 1661:
                {
                    uint32 healthPerc = uint32((float(m_caster->GetHealth()) / m_caster->GetMaxHealth()) * 100);
                    int32 melee_mod = 10;
                    if (healthPerc <= 40)
                        melee_mod = 30;
                    if (healthPerc < 100 && healthPerc > 40)
                        melee_mod = 10 + (100 - healthPerc) / 3;

                    int32 hasteModBasePoints0 = melee_mod;          // (EffectBasePoints[0]+1)-1+(5-melee_mod) = (melee_mod-1+1)-1+5-melee_mod = 5-1
                    int32 hasteModBasePoints1 = (5 - melee_mod);
                    int32 hasteModBasePoints2 = 5;

                    // FIXME: custom spell required this aura state by some unknown reason, we not need remove it anyway
                    m_caster->ModifyAuraState(AURA_STATE_BERSERKING, true);
                    m_caster->CastCustomSpell(m_caster, 26635, &hasteModBasePoints0, &hasteModBasePoints1, &hasteModBasePoints2, true, NULL);
                    return;
                }
            case 30543: //Turn Cooldown : chess event karazhan dummy
                {
                    return;
                }
            case 32225: //Chess Event: Take Action Melee
                {
                    switch (m_caster->GetEntry())
                    {
                        case 21684:
                            m_caster->CastSpell(m_caster, 37150, true);
                            break;//King Liane Melee Attack
                        case 21683:
                            m_caster->CastSpell(m_caster, 37149, true);
                            break;//Human Conjur Melee Attack
                        case 21682:
                            m_caster->CastSpell(m_caster, 37147, true);
                            break;//Human Cleric Melee Attack
                        case 21664:
                            m_caster->CastSpell(m_caster, 37143, true);
                            break;//Human Charger Melee Attack
                        case 21160:
                            m_caster->CastSpell(m_caster, 37142, true);
                            break; //Water Elemental melee Attack
                        case 17211:
                            m_caster->CastSpell(m_caster, 32227, true);
                            break; //Human Footan Melee Attack
                        case 21752:
                            m_caster->CastSpell(m_caster, 37348, true);
                            break; //Warchief Blackhand Melee Attack
                        case 21750:
                            m_caster->CastSpell(m_caster, 37345, true);
                            break; //Orc Warlock Melee Attack
                        case 21747:
                            m_caster->CastSpell(m_caster, 37337, true);
                            break; //Orc Necrolyte Melee Attack
                        case 21748:
                            m_caster->CastSpell(m_caster, 37339, true);
                            break; //Orc Wolf Melee Attack
                        case 21726:
                            m_caster->CastSpell(m_caster, 37220, true);
                            break; //Summoned Deamon
                        case 17469:
                            m_caster->CastSpell(m_caster, 32228, true);
                            break; //Orc Gunt Melee Attack
                    }
                    return;
                }
            }
        }
    case SPELLFAMILY_MAGE:
        switch (m_spellInfo->Id)
        {
        case 11958:                                 // Cold Snap
            {
                if (m_caster->GetTypeId() != TYPEID_PLAYER)
                    return;

                // immediately finishes the cooldown on Frost spells
                const PlayerSpellMap& sp_list = m_caster->ToPlayer()->GetSpellMap();
                for (PlayerSpellMap::const_iterator itr = sp_list.begin(); itr != sp_list.end(); ++itr)
                {
                    if (itr->second.state == PLAYERSPELL_REMOVED)
                        continue;

                    uint32 classspell = itr->first;
                    SpellEntry const* spellInfo = sSpellStore.LookupEntry(classspell);

                    if (spellInfo->SpellFamilyName == SPELLFAMILY_MAGE &&
                        (GetSpellSchoolMask(spellInfo) & SPELL_SCHOOL_MASK_FROST) &&
                        spellInfo->Id != 11958 && GetSpellRecoveryTime(spellInfo) > 0)
                        m_caster->ToPlayer()->RemoveSpellCooldown(classspell, true);
                }
                return;
            }
        case 32826:
            {
                if (unitTarget && unitTarget->GetTypeId() == TYPEID_UNIT)
                {
                    //Polymorph Cast Visual Rank 1
                    const uint32 spell_list[6] = {32813, 32816, 32817, 32818, 32819, 32820};
                    unitTarget->CastSpell(unitTarget, spell_list[urand(0, 5)], true);
                }
                return;
            }
        }
        break;
    case SPELLFAMILY_WARRIOR:
        // Charge
        if (m_spellInfo->SpellFamilyFlags & 0x1 && m_spellInfo->SpellVisual == 867)
        {
            int32 chargeBasePoints0 = damage;
            m_caster->CastCustomSpell(m_caster, 34846, &chargeBasePoints0, NULL, NULL, true);
            return;
        }
        // Execute
        if (m_spellInfo->SpellFamilyFlags & 0x20000000)
        {
            if (!unitTarget)
                return;

            spell_id = 20647;
            bp = damage + int32(m_caster->GetPower(POWER_RAGE) * m_spellInfo->DmgMultiplier[effIndex]);
            m_caster->SetPower(POWER_RAGE, 0);
            break;
        }
        if (m_spellInfo->Id == 21977)                      //Warrior's Wrath
        {
            if (!unitTarget)
                return;

            m_caster->CastSpell(unitTarget, 21887, true); // spell mod
            return;
        }
        if (m_spellInfo->Id == 30284)  //Chess: Change Facing
        {
            m_caster->SetInFront(unitTarget);

            // and client
            WorldPacket data;
            m_caster->BuildHeartBeatMsg(&data);
            m_caster->SendMessageToSet(&data, false);

            return;
        }
        if (m_spellInfo->Id == 37151 || m_spellInfo->Id == 37148 || m_spellInfo->Id == 37144) //MoveSpells for Chess_Event - dunno why has spellfamilyname for Warrior Spells
        {
            if (m_caster->GetDistance(unitTarget) <= (GetSpellMaxRange(m_spellInfo) - GetSpellRadius(m_spellInfo, effIndex, true)))
            {
                if (unitTarget->GetTypeId() == TYPEID_UNIT && !(unitTarget->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_REMOVE_CLIENT_CONTROL))) //if UNIT_FLAG_NOT_SELECTABLE than field is occuped
                {
                    //cast transform Field
                    if (m_caster)
                    {
                        unitTarget->CastSpell(unitTarget, 32745, true); //implemented
                        m_caster->CastSpell(m_caster, 30543, true); //not implemented -- handled in this function
                        m_caster->HandleEmoteCommand(EMOTE_ONESHOT_POINT_NOSHEATHE);

                        //create new Move Marker
                        m_caster->CastSpell(unitTarget, 32261, true);

                        //Register Scripts
                        //prepare Move
                        float speed = m_caster->GetSpeed(MOVE_WALK);
                        speed *= 0.001f;
                        ScriptInfo *si = new ScriptInfo;
                        si->command = SCRIPT_COMMAND_MOVE_TO;
                        si->MoveTo.DestX = unitTarget->GetPositionX();
                        si->MoveTo.DestY = unitTarget->GetPositionY();
                        si->MoveTo.DestZ = unitTarget->GetPositionZ();
                        si->MoveTo.TravelTime = m_caster->GetExactDist(unitTarget->GetPositionX(), unitTarget->GetPositionY(), unitTarget->GetPositionZ()) / speed;

                        //prepare change facing
                        ScriptInfo *si2 = new ScriptInfo;
                        si2->command = SCRIPT_COMMAND_CAST_SPELL;
                        si2->CastSpell.SpellID = 38011;

                        //insert scripts
                        m_caster->GetMap()->ScriptCommandStart((*si2), 2 + 1 + si->MoveTo.TravelTime / IN_MILLISECONDS, m_caster, m_caster);
                        m_caster->GetMap()->ScriptCommandStart((*si), 2, m_caster, m_caster);

                        //set cooldowns
                        uint32 cooldown = 12000;
                        uint32 SpellTurn = 30284;
                        time_t cd = time(NULL) + cooldown / IN_MILLISECONDS;
                        m_caster->ToCreature()->_AddCreatureSpellCooldown(m_spellInfo->Id, cd);
                        m_caster->ToCreature()->_AddCreatureSpellCooldown(SpellTurn, cd);
                        m_caster->ToCreature()->_AddCreatureCategoryCooldown(30284, cd);

                        //send cooldowns to player
                        if (m_caster->isCharmed())
                        {
                            Unit* charmer = m_caster->GetCharmer();
                            if (charmer && charmer->GetTypeId() == TYPEID_PLAYER)
                            {
                                WorldPacket data(SMSG_SPELL_COOLDOWN, (8 + 1 + 16));
                                data << m_caster->GetGUID();
                                data << uint8(0x0);
                                data << m_spellInfo->Id;
                                data << cooldown;
                                data << SpellTurn;
                                data << cooldown;
                                charmer->ToPlayer()->GetSession()->SendPacket(&data);
                            }
                        }
                    }
                }
            }
            return;
        }
        break;
    case SPELLFAMILY_WARLOCK:
        //Life Tap (only it have this with dummy effect)
        if (m_spellInfo->SpellFamilyFlags == 0x40000)
        {
            float cost = damage;

            if (Player* modOwner = m_caster->GetSpellModOwner())
                modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_COST, cost, this);

            int32 dmg = m_caster->SpellDamageBonus(m_caster, m_spellInfo, uint32(cost > 0 ? cost : 0), SPELL_DIRECT_DAMAGE);

            if (int32(m_caster->GetHealth()) > dmg)
            {
                // Shouldn't Appear in Combat Log
                m_caster->ModifyHealth(-dmg);

                int32 mana = dmg;

                Unit::AuraList const& auraDummy = m_caster->GetAurasByType(SPELL_AURA_DUMMY);
                for (Unit::AuraList::const_iterator itr = auraDummy.begin(); itr != auraDummy.end(); ++itr)
                {
                    // only Imp. Life Tap have this in combination with dummy aura
                    if ((*itr)->GetSpellProto()->SpellFamilyName == SPELLFAMILY_WARLOCK && (*itr)->GetSpellProto()->SpellIconID == 208)
                        mana = ((*itr)->GetModifier()->m_amount + 100) * mana / 100;
                }

                m_caster->CastCustomSpell(m_caster, 31818, &mana, NULL, NULL, true, NULL);

                // Mana Feed
                int32 manaFeedVal = m_caster->CalculateSpellDamage(m_spellInfo, 1, m_spellInfo->EffectBasePoints[1], m_caster);
                manaFeedVal = manaFeedVal * mana / 100;
                if (manaFeedVal > 0 && m_caster->IsPlayer() && m_caster->ToPlayer()->GetPet())
                    m_caster->CastCustomSpell(m_caster, 32553, &manaFeedVal, NULL, NULL, true, NULL);
            }
            else
                SendCastResult(SPELL_FAILED_FIZZLE);
            return;
        }
        break;
    case SPELLFAMILY_PRIEST:
        switch (m_spellInfo->Id)
        {
        case 28598:                                 // Touch of Weakness triggered spell
            {
                if (!unitTarget || !m_triggeredByAuraSpell)
                    return;

                uint32 spellid = 0;
                switch (m_triggeredByAuraSpell->Id)
                {
                case 2652:
                    spellid =  2943;
                    break; // Rank 1
                case 19261:
                    spellid = 19249;
                    break; // Rank 2
                case 19262:
                    spellid = 19251;
                    break; // Rank 3
                case 19264:
                    spellid = 19252;
                    break; // Rank 4
                case 19265:
                    spellid = 19253;
                    break; // Rank 5
                case 19266:
                    spellid = 19254;
                    break; // Rank 6
                case 25461:
                    spellid = 25460;
                    break; // Rank 7
                default:
                    sLog.outError("Spell::EffectDummy: Spell 28598 triggered by unhandled spell %u", m_triggeredByAuraSpell->Id);
                    return;
                }
                m_caster->CastSpell(unitTarget, spellid, true, NULL);
                return;
            }
        }
        break;
    case SPELLFAMILY_DRUID:
        switch (m_spellInfo->Id)
        {
        case 5420:                                  // Tree of Life passive
            {
                // Tree of Life area effect
                int32 health_mod = int32(m_caster->GetStat(STAT_SPIRIT) / 4);
                m_caster->CastCustomSpell(m_caster, 34123, &health_mod, NULL, NULL, true, NULL);
                return;
            }
        }
        break;
    case SPELLFAMILY_ROGUE:
        switch (m_spellInfo->Id)
        {
        case 31231:                                 // Cheat Death
            {
                m_caster->CastSpell(m_caster, 45182, true);
                return;
            }
        case 5938:                                  // Shiv
            {
                if (m_caster->GetTypeId() != TYPEID_PLAYER)
                    return;

                Player* pCaster = m_caster->ToPlayer();

                Item* item = pCaster->GetWeaponForAttack(OFF_ATTACK);
                if (!item)
                    return;

                ((Player*)m_caster)->AddComboPoints(unitTarget, 1);

                // all poison enchantments is temporary
                if (uint32 enchant_id = item->GetEnchantmentId(TEMP_ENCHANTMENT_SLOT))
                {
                    if (SpellItemEnchantmentEntry const* pEnchant = sSpellItemEnchantmentStore.LookupEntry(enchant_id))
                    {
                        for (int s = 0; s < 3; s++)
                        {
                            if (pEnchant->type[s] != ITEM_ENCHANTMENT_TYPE_COMBAT_SPELL)
                                continue;

                            SpellEntry const* combatEntry = sSpellStore.LookupEntry(pEnchant->spellid[s]);
                            if (!combatEntry || combatEntry->Dispel != DISPEL_POISON)
                                continue;

                            m_caster->CastSpell(unitTarget, combatEntry, true, item);
                        }
                    }
                }

                m_caster->CastSpell(unitTarget, 5940, true);
                return;
            }
        }
        break;
    case SPELLFAMILY_HUNTER:
        // Kill command
        if (m_spellInfo->SpellFamilyFlags & 0x00080000000000LL)
        {
            if (m_caster->getClass() != CLASS_HUNTER)
                return;

            // clear hunter crit aura state
            m_caster->ModifyAuraState(AURA_STATE_HUNTER_CRIT_STRIKE, false);

            // additional damage from pet to pet target
            Unit* pet = m_caster->GetGuardianPet();
            if (!pet || !pet->GetVictim())
                return;

            uint32 spell_id = 0;
            switch (m_spellInfo->Id)
            {
            case 34026:
                spell_id = 34027;
                break;    // rank 1
            default:
                sLog.outError("Spell::EffectDummy: Spell %u not handled in KC", m_spellInfo->Id);
                return;
            }

            pet->CastSpell(pet->GetVictim(), spell_id, true);
            return;
        }

        switch (m_spellInfo->Id)
        {
        case 23989:                                 //Readiness talent
            {
                if (m_caster->GetTypeId() != TYPEID_PLAYER)
                    return;

                //immediately finishes the cooldown for hunter abilities
                const PlayerSpellMap& sp_list = m_caster->ToPlayer()->GetSpellMap();
                for (PlayerSpellMap::const_iterator itr = sp_list.begin(); itr != sp_list.end(); ++itr)
                {
                    uint32 classspell = itr->first;
                    SpellEntry const* spellInfo = sSpellStore.LookupEntry(classspell);

                    if (spellInfo->SpellFamilyName == SPELLFAMILY_HUNTER && spellInfo->Id != 23989 && GetSpellRecoveryTime(spellInfo) > 0)
                        m_caster->ToPlayer()->RemoveSpellCooldown(classspell, true);
                }
                return;
            }
        case 37506:                                 // Scatter Shot
            {
                if (m_caster->GetTypeId() != TYPEID_PLAYER)
                    return;

                // break Auto Shot and autohit
                m_caster->InterruptSpell(CURRENT_AUTOREPEAT_SPELL);
                m_caster->AttackStop();
                m_caster->ToPlayer()->SendAttackSwingCancelAttack();
                return;
            }
        }
        break;
    case SPELLFAMILY_PALADIN:
        switch (m_spellInfo->SpellIconID)
        {
        case 156:                                   // Holy Shock
            {
                if (!unitTarget)
                    return;

                int hurt = 0;
                int heal = 0;

                switch (m_spellInfo->Id)
                {
                case 20473:
                    hurt = 25912;
                    heal = 25914;
                    break;
                case 20929:
                    hurt = 25911;
                    heal = 25913;
                    break;
                case 20930:
                    hurt = 25902;
                    heal = 25903;
                    break;
                case 27174:
                    hurt = 27176;
                    heal = 27175;
                    break;
                case 33072:
                    hurt = 33073;
                    heal = 33074;
                    break;
                default:
                    sLog.outError("Spell::EffectDummy: Spell %u not handled in HS", m_spellInfo->Id);
                    return;
                }

                if (m_caster->IsFriendlyTo(unitTarget))
                    m_caster->CastSpell(unitTarget, heal, true, 0);
                else
                    m_caster->CastSpell(unitTarget, hurt, true, 0);

                return;
            }
        case 561:                                   // Judgement of command
            {
                if (!unitTarget)
                    return;

                uint32 spell_id = m_spellInfo->EffectBasePoints[effIndex] + 1; // m_currentBasePoints[effIndex]+1;
                SpellEntry const* spell_proto = sSpellStore.LookupEntry(spell_id);
                if (!spell_proto)
                    return;

                if (!unitTarget->HasUnitState(UNIT_STATE_STUNNED) && m_caster->GetTypeId() == TYPEID_PLAYER)
                {
                    // decreased damage (/2) for non-stunned target.
                    SpellModifier* mod = new SpellModifier;
                    mod->op = SPELLMOD_DAMAGE;
                    mod->value = -50;
                    mod->type = SPELLMOD_PCT;
                    mod->spellId = m_spellInfo->Id;
                    mod->effectId = effIndex;
                    mod->lastAffected = NULL;
                    mod->mask = 0x0000020000000000LL;
                    mod->charges = 0;

                    m_caster->ToPlayer()->AddSpellMod(mod, true);
                    m_caster->CastSpell(unitTarget, spell_proto, true, NULL);
                    // mod deleted
                    m_caster->ToPlayer()->AddSpellMod(mod, false);
                }
                else
                    m_caster->CastSpell(unitTarget, spell_proto, true, NULL);

                return;
            }
        }

        switch (m_spellInfo->Id)
        {
        case 31789:                                 // Righteous Defense (step 1)
            { 
            if (!unitTarget)
                return;

            Unit::AttackerSet attackers = unitTarget->getAttackers();

            for (Unit::AttackerSet::iterator aItr = attackers.begin(); aItr != attackers.end();)
                if (!(*aItr)->IsValidAttackTarget(m_caster))
                    aItr = attackers.erase(aItr);
                else
                    ++aItr;

            uint32 maxTargets = std::min<uint32>(3, attackers.size());

            for (uint32 i = 0; i < maxTargets; ++i)
            {
                Unit::AttackerSet::iterator aItr = attackers.begin();
                std::advance(aItr, urand(0, attackers.size() - 1));
                m_caster->CastSpell((*aItr), 31790, true);
                attackers.erase(aItr);
            }
                return;
            }
        case 37877:                                 // Blessing of Faith
            {
                if (!unitTarget)
                    return;

                uint32 spell_id = 0;
                switch (unitTarget->getClass())
                {
                case CLASS_DRUID:
                    spell_id = 37878;
                    break;
                case CLASS_PALADIN:
                    spell_id = 37879;
                    break;
                case CLASS_PRIEST:
                    spell_id = 37880;
                    break;
                case CLASS_SHAMAN:
                    spell_id = 37881;
                    break;
                default:
                    return;                    // ignore for not healing classes
                }

                m_caster->CastSpell(m_caster, spell_id, true);
                return;
            }
        }
        break;
    case SPELLFAMILY_SHAMAN:

        // Flametongue Totem & Weapon Proc
        if (m_spellInfo->SpellFamilyFlags & 0x400200000LL)
        {
            // damage is based on the speed of the weapon...
            // but we need to know which weapon was used main or off
            uint8 slot = (m_CastItem) ? m_CastItem->GetSlot() : uint8(EQUIPMENT_SLOT_MAINHAND);
            bp = m_caster->GetAttackTime( (slot == EQUIPMENT_SLOT_MAINHAND) ? BASE_ATTACK : OFF_ATTACK) * (m_spellInfo->EffectBasePoints[0] + 1) / 100000;
            spell_id = 16368;
            break;
        }

        //Shaman Rockbiter Weapon
        if (m_spellInfo->SpellFamilyFlags == 0x400000)
        {
            uint32 spell_id = 0;
            switch (m_spellInfo->Id)
            {
            case  8017:
                spell_id = 36494;
                break;    // Rank 1
            case  8018:
                spell_id = 36750;
                break;    // Rank 2
            case  8019:
                spell_id = 36755;
                break;    // Rank 3
            case 10399:
                spell_id = 36759;
                break;    // Rank 4
            case 16314:
                spell_id = 36763;
                break;    // Rank 5
            case 16315:
                spell_id = 36766;
                break;    // Rank 6
            case 16316:
                spell_id = 36771;
                break;    // Rank 7
            case 25479:
                spell_id = 36775;
                break;    // Rank 8
            case 25485:
                spell_id = 36499;
                break;    // Rank 9
            default:
                sLog.outError("Spell::EffectDummy: Spell %u not handled in RW", m_spellInfo->Id);
                return;
            }

            SpellEntry const* spellInfo = sSpellStore.LookupEntry(spell_id);

            if (!spellInfo)
            {
                sLog.outError("WORLD: unknown spell id %i\n", spell_id);
                return;
            }

            if (m_caster->GetTypeId() != TYPEID_PLAYER)
                return;

            for (int i = BASE_ATTACK; i <= OFF_ATTACK; ++i)
            {
                if (Item* item = m_caster->ToPlayer()->GetWeaponForAttack(WeaponAttackType(i)))
                {
                    if (item->IsFitToSpellRequirements(m_spellInfo))
                    {
                        Spell* spell = new Spell(m_caster, spellInfo, true);

                        // enchanting spell selected by calculated damage-per-sec in enchanting effect
                        // at calculation applied affect from Elemental Weapons talent
                        // real enchantment damage-1
                        spell->m_currentBasePoints[1] = damage - 1;

                        SpellCastTargets targets;
                        targets.setItemTarget(item);
                        spell->prepare(&targets);
                    }
                }
            }
            return;
        }

        if (m_spellInfo->Id == 39610)                   // Mana-Tide Totem effect
        {
            if (!unitTarget || unitTarget->getPowerType() != POWER_MANA)
                return;

            // Regenerate 6% of Total Mana Every 3 secs
            int32 EffectBasePoints0 = unitTarget->GetMaxPower(POWER_MANA)  * damage / 100;
            m_caster->CastCustomSpell(unitTarget, 39609, &EffectBasePoints0, NULL, NULL, true, NULL, NULL, m_originalCasterGUID);
            return;
        }

        break;
    }

    //spells triggered by dummy effect should not miss
    if (spell_id)
    {
        SpellEntry const* spellInfo = sSpellStore.LookupEntry(spell_id);

        if (!spellInfo)
        {
            sLog.outError("EffectDummy of spell %u: triggering unknown spell id %i\n", m_spellInfo->Id, spell_id);
            return;
        }

        Spell* spell = new Spell(m_caster, spellInfo, true, m_originalCasterGUID, NULL, true);
        if (bp) spell->m_currentBasePoints[0] = bp;
        SpellCastTargets targets;
        targets.setUnitTarget(unitTarget);
        spell->prepare(&targets);
    }

    // pet auras
    if (PetAura const* petSpell = sSpellMgr.GetPetAura(m_spellInfo->Id))
    {
        m_caster->AddPetAura(petSpell);
        return;
    }

    if (unitTarget && unitTarget->GetTypeId() == TYPEID_UNIT)
        sScriptMgr.EffectDummyCreature(m_caster, m_spellInfo->Id, effIndex, unitTarget->ToCreature());
}

void Spell::EffectTriggerSpellWithValue(SpellEffIndex effIndex)
{
    uint32 triggered_spell_id = m_spellInfo->EffectTriggerSpell[effIndex];

    // normal case
    SpellEntry const* spellInfo = sSpellStore.LookupEntry(triggered_spell_id);

    if (!spellInfo)
    {
        sLog.outError("EffectTriggerSpellWithValue of spell %u: triggering unknown spell id %i\n", m_spellInfo->Id, triggered_spell_id);
        return;
    }

    int32 bp = damage;
    m_caster->CastCustomSpell(unitTarget, triggered_spell_id, &bp, &bp, &bp, true, NULL, NULL, m_originalCasterGUID);
}

void Spell::EffectTriggerRitualOfSummoning(SpellEffIndex effIndex)
{
    uint32 triggered_spell_id = m_spellInfo->EffectTriggerSpell[effIndex];
    SpellEntry const* spellInfo = sSpellStore.LookupEntry(triggered_spell_id);

    if (!spellInfo)
    {
        sLog.outError("EffectTriggerRitualOfSummoning of spell %u: triggering unknown spell id %i", m_spellInfo->Id, triggered_spell_id);
        return;
    }

    finish();

    Spell* spell = new Spell(m_caster, spellInfo, true);
    SpellCastTargets targets;
    targets.setUnitTarget(unitTarget);
    spell->prepare(&targets);

    m_caster->SetCurrentCastedSpell(spell);
    Spell* curr_spell = m_caster->GetCurrentSpell(spell->GetCurrentContainer());
    spell->m_selfContainer = &curr_spell;
}

void Spell::EffectForceCast(SpellEffIndex effIndex)
{
    if (!unitTarget)
        return;

    uint32 triggered_spell_id = m_spellInfo->EffectTriggerSpell[effIndex];

    // normal case
    SpellEntry const* spellInfo = sSpellStore.LookupEntry(triggered_spell_id);

    if (!spellInfo)
    {
        sLog.outError("EffectForceCast of spell %u: triggering unknown spell id %i", m_spellInfo->Id, triggered_spell_id);
        return;
    }

    switch (m_spellInfo->Id)
    {
        case 45391:
            unitTarget->CastSpell((Unit*)NULL, triggered_spell_id, true, NULL, NULL, m_originalCasterGUID);
            break;
        case 45388:
            unitTarget->CastSpell(m_caster, triggered_spell_id, true, NULL, NULL, m_originalCasterGUID);
            break;
        case 45782:
            unitTarget->CastSpell((Unit*)NULL, triggered_spell_id, true, NULL, NULL, m_originalCasterGUID);
            break;
        default:
            unitTarget->CastSpell(unitTarget, spellInfo, true, NULL, NULL, m_originalCasterGUID);
            break;
    }
}

void Spell::EffectTriggerSpell(SpellEffIndex effIndex)
{
    uint32 triggered_spell_id = m_spellInfo->EffectTriggerSpell[effIndex];

    // special cases
    switch (triggered_spell_id)
    {
    // Vanish
    case 18461:
        {
            m_caster->RemoveMovementImpairingAuras();
            m_caster->RemoveSpellsCausingAura(SPELL_AURA_MOD_STALKED);

            // if this spell is given to NPC it must handle rest by it's own AI
            if (m_caster->GetTypeId() != TYPEID_PLAYER)
                return;

            // get highest rank of the Stealth spell
            bool found = false;
            SpellEntry const* spellInfo;
            const PlayerSpellMap& sp_list = m_caster->ToPlayer()->GetSpellMap();
            for (PlayerSpellMap::const_iterator itr = sp_list.begin(); itr != sp_list.end(); ++itr)
            {
                // only highest rank is shown in spell book, so simply check if shown in spell book
                if (!itr->second.active || itr->second.disabled || itr->second.state == PLAYERSPELL_REMOVED)
                    continue;

                spellInfo = sSpellStore.LookupEntry(itr->first);
                if (!spellInfo)
                    continue;

                if (spellInfo->SpellFamilyName == SPELLFAMILY_ROGUE && spellInfo->SpellFamilyFlags & SPELLFAMILYFLAG_ROGUE_STEALTH)
                {
                    found = true;
                    break;
                }
            }

            // no Stealth spell found
            if (!found)
                return;

            // reset cooldown on it if needed
            if (m_caster->ToPlayer()->HasSpellCooldown(spellInfo->Id))
                m_caster->ToPlayer()->RemoveSpellCooldown(spellInfo->Id);

           // Remove from combat
           m_caster->CombatStop();

            m_TriggerSpells.push_back(spellInfo);
            return;
        }
    // just skip
    case 23770:                                         // Sayge's Dark Fortune of *
        // not exist, common cooldown can be implemented in scripts if need.
        return;
    // Brittle Armor - (need add max stack of 24575 Brittle Armor)
    case 29284:
        {
            const SpellEntry* spell = sSpellStore.LookupEntry(24575);
            if (!spell)
                return;

            for (uint32 i = 0; i < spell->StackAmount; ++i)
                m_caster->CastSpell(unitTarget, spell->Id, true, m_CastItem, NULL, m_originalCasterGUID);
            return;
        }
    // Mercurial Shield - (need add max stack of 26464 Mercurial Shield)
    case 29286:
        {
            const SpellEntry* spell = sSpellStore.LookupEntry(26464);
            if (!spell)
                return;

            for (uint32 i = 0; i < spell->StackAmount; ++i)
                m_caster->CastSpell(unitTarget, spell->Id, true, m_CastItem, NULL, m_originalCasterGUID);
            return;
        }
    // Righteous Defense
    case 31980:
        {
            m_caster->CastSpell(unitTarget, 31790, true, m_CastItem, NULL, m_originalCasterGUID);
            return;
        }
    // Cloak of Shadows
    case 35729 :
        {
            uint32 dispelMask = GetDispelMask(DISPEL_ALL);
            Unit::AuraMap& Auras = m_caster->GetAuras();
            for (Unit::AuraMap::iterator iter = Auras.begin(); iter != Auras.end(); ++iter)
            {
                // remove all harmful spells on you...
                SpellEntry const* spell = iter->second->GetSpellProto();
                if ((spell->DmgClass == SPELL_DAMAGE_CLASS_MAGIC // only affect magic spells
                     || ((1 << spell->Dispel) & dispelMask))
                    // ignore positive and passive auras
                    && !iter->second->IsPositive() && !iter->second->IsPassive())
                {
                    m_caster->RemoveAurasDueToSpell(spell->Id);
                    iter = Auras.begin();
                }
            }
            return;
        }
    // Priest Shadowfiend (34433) need apply mana gain trigger aura on pet
    case 41967:
        {
            if (Unit* pet = m_caster->GetGuardianPet())
                pet->CastSpell(pet, 28305, true);
            return;
        }
    }

    // normal case
    SpellEntry const* spellInfo = sSpellStore.LookupEntry(triggered_spell_id);

    if (!spellInfo)
    {
        sLog.outError("EffectTriggerSpell of spell %u: triggering unknown spell id %i", m_spellInfo->Id, triggered_spell_id);
        return;
    }

    // some triggered spells require specific equipment
    if (spellInfo->EquippedItemClass >= 0 && m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        // main hand weapon required
        if (spellInfo->AttributesEx3 & SPELL_ATTR3_MAIN_HAND)
        {
            Item* item = m_caster->ToPlayer()->GetWeaponForAttack(BASE_ATTACK);

            // skip spell if no weapon in slot or broken
            if (!item || item->IsBroken())
                return;

            // skip spell if weapon not fit to triggered spell
            if (!item->IsFitToSpellRequirements(spellInfo))
                return;
        }

        // offhand hand weapon required
        if (spellInfo->AttributesEx3 & SPELL_ATTR3_REQ_OFFHAND)
        {
            Item* item = m_caster->ToPlayer()->GetWeaponForAttack(OFF_ATTACK);

            // skip spell if no weapon in slot or broken
            if (!item || item->IsBroken())
                return;

            // skip spell if weapon not fit to triggered spell
            if (!item->IsFitToSpellRequirements(spellInfo))
                return;
        }
    }

    if (unitTarget)
        m_caster->CastSpell(unitTarget, spellInfo, true, m_CastItem, NULL, m_originalCasterGUID);
}

void Spell::EffectTriggerMissileSpell(SpellEffIndex effIndex)
{
    uint32 triggered_spell_id = m_spellInfo->EffectTriggerSpell[effIndex];

    // normal case
    SpellEntry const* spellInfo = sSpellStore.LookupEntry(triggered_spell_id);

    if (!spellInfo)
    {
        sLog.outError("EffectTriggerMissileSpell of spell %u (eff: %u): triggering unknown spell id %u",
                      m_spellInfo->Id, effIndex, triggered_spell_id);
        return;
    }

    #ifdef OREGON_DEBUG
    if (m_CastItem)
        DEBUG_LOG("WORLD: cast Item spellId - %i", spellInfo->Id);
    #endif

    Spell* spell = new Spell(m_caster, spellInfo, true, m_originalCasterGUID);

    SpellCastTargets targets;
    targets.setDst(&m_targets.m_dstPos);
    spell->m_CastItem = m_CastItem;
    spell->prepare(&targets, NULL);
}

void Spell::EffectTeleportUnits(SpellEffIndex /*effIndex*/)
{
    if (!unitTarget || unitTarget->IsInFlight())
        return;

    // If not exist data for dest location - return
    if (!m_targets.HasDst())
    {
        sLog.outError("Spell::EffectTeleportUnits - does not have destination for spell ID %u\n", m_spellInfo->Id);
        return;
    }

    // Init dest coordinates
    uint32 mapid = m_targets.m_dstPos.GetMapId();
    if (mapid == MAPID_INVALID)
        mapid = unitTarget->GetMapId();
    float x, y, z, orientation;
    m_targets.m_dstPos.GetPosition(x, y, z, orientation);
    if (!orientation && m_targets.getUnitTarget())
        orientation = m_targets.getUnitTarget()->GetOrientation();
    DEBUG_LOG("Spell::EffectTeleportUnits - teleport unit to %u %f %f %f %f\n", mapid, x, y, z, orientation);

    if (unitTarget->GetTypeId() == TYPEID_PLAYER)
        unitTarget->ToPlayer()->TeleportTo(mapid, x, y, z, orientation, unitTarget == m_caster ? TELE_TO_SPELL | TELE_TO_NOT_LEAVE_COMBAT : 0);
    else if (mapid == unitTarget->GetMapId())
        unitTarget->NearTeleportTo(x, y, z, orientation, unitTarget == m_caster);
    else
    {
        sLog.outError("Spell::EffectTeleportUnits - spellId %u attempted to teleport creature to a different map.", m_spellInfo->Id);
        return;
    }

    // post effects for TARGET_DST_DB
    switch (m_spellInfo->Id)
    {
        // Dimensional Ripper - Everlook
        case 23442:
            {
                int32 r = irand(0, 119);
                if (r >= 70)                                  // 7/12 success
                {
                    if (r < 100)                              // 4/12 evil twin
                        m_caster->CastSpell(m_caster, 23445, true);
                    else                                        // 1/12 fire
                        m_caster->CastSpell(m_caster, 23449, true);
                }
                return;
            }
        // Ultrasafe Transporter: Toshley's Station
        case 36941:
            {
                if (roll_chance_i(50))                        // 50% success
                {
                    int32 rand_eff = urand(1, 7);
                    switch (rand_eff)
                    {
                    case 1:
                        // soul split - evil
                        m_caster->CastSpell(m_caster, 36900, true);
                        break;
                    case 2:
                        // soul split - good
                        m_caster->CastSpell(m_caster, 36901, true);
                        break;
                    case 3:
                        // Increase the size
                        m_caster->CastSpell(m_caster, 36895, true);
                        break;
                    case 4:
                        // Decrease the size
                        m_caster->CastSpell(m_caster, 36893, true);
                        break;
                    case 5:
                        // Transform
                        {
                            if (m_caster->ToPlayer() && m_caster->ToPlayer()->GetTeam() == ALLIANCE)
                                m_caster->CastSpell(m_caster, 36897, true);
                            else
                                m_caster->CastSpell(m_caster, 36899, true);
                            break;
                        }
                    case 6:
                        // chicken
                        m_caster->CastSpell(m_caster, 36940, true);
                        break;
                    case 7:
                        // evil twin
                        m_caster->CastSpell(m_caster, 23445, true);
                        break;
                    }
                }
                return;
            }
        // Dimensional Ripper - Area 52
        case 36890:
            {
                if (roll_chance_i(50))                        // 50% success
                {
                    int32 rand_eff = urand(1, 4);
                    switch (rand_eff)
                    {
                    case 1:
                        // soul split - evil
                        m_caster->CastSpell(m_caster, 36900, true);
                        break;
                    case 2:
                        // soul split - good
                        m_caster->CastSpell(m_caster, 36901, true);
                        break;
                    case 3:
                        // Increase the size
                        m_caster->CastSpell(m_caster, 36895, true);
                        break;
                    case 4:
                        // Transform
                        {
                            if (m_caster->ToPlayer()->GetTeam() == ALLIANCE)
                                m_caster->CastSpell(m_caster, 36897, true);
                            else
                                m_caster->CastSpell(m_caster, 36899, true);
                            break;
                        }
                    }
                }
                return;
        }
    }
}

void Spell::EffectApplyAura(SpellEffIndex effIndex)
{
    if (!unitTarget)
        return;

    // ghost spell check, allow apply any auras at player loading in ghost mode (will be cleanup after load)
    if (!unitTarget->IsAlive() && m_spellInfo->Id != 20584 && m_spellInfo->Id != 8326 &&
        (unitTarget->GetTypeId() != TYPEID_PLAYER || !unitTarget->ToPlayer()->GetSession()->PlayerLoading()))
        return;

    Unit* caster = m_originalCasterGUID ? m_originalCaster : m_caster;
    if (!caster)
        return;

    DEBUG_LOG("Spell: Aura is: %u", m_spellInfo->EffectApplyAuraName[effIndex]);

    Aura* Aur = CreateAura(m_spellInfo, effIndex, &damage, unitTarget, caster, m_CastItem);

    // Now Reduce spell duration using data received at spell hit
    int32 duration = Aur->GetAuraMaxDuration();
    if (m_spellValue->CustomDuration)
        duration = m_spellValue->Duration;
    if (!IsPositiveSpell(m_spellInfo->Id))
    {
        unitTarget->ApplyDiminishingToDuration(m_diminishGroup, duration, caster, m_diminishLevel);
        Aur->setDiminishGroup(m_diminishGroup);
    }

    //mod duration of channeled aura by spell haste
    if (IsChanneledSpell(m_spellInfo))
        caster->ModSpellCastTime(m_spellInfo, duration, this);

    // if Aura removed and deleted, do not continue.
    if (Aur->IsExpired())
    {
        delete Aur;
        return;
    }

    if (duration != Aur->GetAuraMaxDuration())
    {
        Aur->SetAuraMaxDuration(duration);
        Aur->SetAuraDuration(duration);
    }

    bool added = unitTarget->AddAura(Aur);

    // Aura not added and deleted in AddAura call;
    if (!added)
        return;

    // found crash at character loading, broken pointer to Aur...
    // Aur was deleted in AddAura()...
    if (!Aur)
        return;

    // @todo Make a way so it works for every related spell!
    if (unitTarget->GetTypeId() == TYPEID_PLAYER || (unitTarget->GetTypeId() == TYPEID_UNIT && unitTarget->IsPet()))             // Negative buff should only be applied on players
    {
        uint32 spellId = 0;
        if (m_spellInfo->CasterAuraStateNot == AURA_STATE_WEAKENED_SOUL || m_spellInfo->TargetAuraStateNot == AURA_STATE_WEAKENED_SOUL)
            spellId = 6788;                                 // Weakened Soul
        else if (m_spellInfo->CasterAuraStateNot == AURA_STATE_FORBEARANCE || m_spellInfo->TargetAuraStateNot == AURA_STATE_FORBEARANCE)
            spellId = 25771;                                // Forbearance
        else if (m_spellInfo->CasterAuraStateNot == AURA_STATE_HYPOTHERMIA)
            spellId = 41425;                                // Hypothermia
        else if (m_spellInfo->Mechanic == MECHANIC_BANDAGE) // Bandages
            spellId = 11196;                                // Recently Bandaged
        else if ((m_spellInfo->AttributesEx & 0x20) && (m_spellInfo->AttributesEx2 & 0x20000))
            spellId = 23230;                                // Blood Fury - Healing Reduction

        SpellEntry const* AdditionalSpellInfo = sSpellStore.LookupEntry(spellId);
        if (AdditionalSpellInfo)
        {
            // applied at target by target
            Aura* AdditionalAura = CreateAura(AdditionalSpellInfo, 0, NULL, unitTarget, unitTarget, 0);
            unitTarget->AddAura(AdditionalAura);
            DEBUG_LOG("Spell: Additional Aura is: %u", AdditionalSpellInfo->EffectApplyAuraName[0]);
        }
    }

    // Prayer of Mending (jump animation), we need formal caster instead original for correct animation
    if (m_spellInfo->SpellFamilyName == SPELLFAMILY_PRIEST && (m_spellInfo->SpellFamilyFlags & 0x00002000000000LL))
        m_caster->CastSpell(unitTarget, 41637, true, NULL, Aur, m_originalCasterGUID);
}

void Spell::EffectUnlearnSpecialization(SpellEffIndex effIndex)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* _player = unitTarget->ToPlayer();
    uint32 spellToUnlearn = m_spellInfo->EffectTriggerSpell[effIndex];

    _player->RemoveSpell(spellToUnlearn);

    DEBUG_LOG("Spell: Player %u has unlearned spell %u from NpcGUID: %u", _player->GetGUIDLow(), spellToUnlearn, m_caster->GetGUIDLow());
}

void Spell::EffectPowerDrain(SpellEffIndex effIndex)
{
    if (m_spellInfo->EffectMiscValue[effIndex] < 0 || m_spellInfo->EffectMiscValue[effIndex] >= MAX_POWERS)
        return;

    Powers drain_power = Powers(m_spellInfo->EffectMiscValue[effIndex]);

    if (!unitTarget)
        return;
    if (!unitTarget->IsAlive())
        return;
    if (unitTarget->getPowerType() != drain_power)
        return;
    if (damage < 0)
        return;

    uint32 curPower = unitTarget->GetPower(drain_power);

    //add spell damage bonus
    damage = m_caster->SpellDamageBonus(unitTarget, m_spellInfo, uint32(damage), SPELL_DIRECT_DAMAGE);

    // resilience reduce mana draining effect at spell crit damage reduction (added in 2.4)
    uint32 power = damage;
    if (drain_power == POWER_MANA && unitTarget->GetTypeId() == TYPEID_PLAYER)
        power -= unitTarget->ToPlayer()->GetSpellCritDamageReduction(power);

    int32 new_damage;
    if (curPower < power)
        new_damage = curPower;
    else
        new_damage = power;

    unitTarget->ModifyPower(drain_power, -new_damage);

    if (drain_power == POWER_MANA)
    {
        float manaMultiplier = m_spellInfo->EffectMultipleValue[effIndex];

        if (Player* modOwner = m_caster->GetSpellModOwner())
            modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_MULTIPLE_VALUE, manaMultiplier);

        int32 gain = int32(new_damage * manaMultiplier);

        m_caster->EnergizeBySpell(m_caster, m_spellInfo->Id, gain, POWER_MANA);
    }
}

void Spell::EffectSendEvent(SpellEffIndex effIndex)
{
    if (m_caster->GetTypeId() == TYPEID_PLAYER && m_caster->ToPlayer()->InBattleground())
    {
        Battleground* bg = m_caster->ToPlayer()->GetBattleground();
        if (bg && bg->GetStatus() == STATUS_IN_PROGRESS)
        {
            switch (m_spellInfo->Id)
            {
            case 23333:                                 // Pickup Horde Flag
            /*do not uncomment .
            if (bg->GetTypeID() == BATTLEGROUND_WS)
                bg->EventPlayerClickedOnFlag((Player*)m_caster, gameObjTarget);
            sLog.outDebug("Send Event Horde Flag Picked Up");
            break;
            not used :
            case 23334:                                 // Drop Horde Flag
                if (bg->GetTypeID() == BATTLEGROUND_WS)
                    bg->EventPlayerDroppedFlag((Player*)m_caster);
                sLog.outDebug("Drop Horde Flag");
                break;
            */
            case 23335:                                 // Pickup Alliance Flag
            /*do not uncomment ... (it will cause crash, because of null targetobject!) anyway this is a bad way to call that event, because it would cause recursion
            if (bg->GetTypeID() == BATTLEGROUND_WS)
                bg->EventPlayerClickedOnFlag((Player*)m_caster, gameObjTarget);
            sLog.outDebug("Send Event Alliance Flag Picked Up");
            break;
            not used :
            case 23336:                                 // Drop Alliance Flag
                if (bg->GetTypeID() == BATTLEGROUND_WS)
                    bg->EventPlayerDroppedFlag((Player*)m_caster);
                sLog.outDebug("Drop Alliance Flag");
                break;
            case 23385:                                 // Alliance Flag Returns
                if (bg->GetTypeID() == BATTLEGROUND_WS)
                    bg->EventPlayerClickedOnFlag((Player*)m_caster, gameObjTarget);
                sLog.outDebug("Alliance Flag Returned");
                break;
            case 23386:                                   // Horde Flag Returns
                if (bg->GetTypeID() == BATTLEGROUND_WS)
                    bg->EventPlayerClickedOnFlag((Player*)m_caster, gameObjTarget);
                sLog.outDebug("Horde Flag Returned");
                break;*/
            case 34976:
                /*
                if (bg->GetTypeID() == BATTLEGROUND_EY)
                    bg->EventPlayerClickedOnFlag((Player*)m_caster, gameObjTarget);
                */
                break;
            default:
                sLog.outDebug("Unknown spellid %u in BG event", m_spellInfo->Id);
                break;
            }
        }
    }
    sLog.outDebug("Spell ScriptStart %u for spellid %u in EffectSendEvent ", m_spellInfo->EffectMiscValue[effIndex], m_spellInfo->Id);

    Object* pTarget;
    if (focusObject)
        pTarget = focusObject;
    else if (unitTarget)
        pTarget = unitTarget;
    else if (gameObjTarget)
        pTarget = gameObjTarget;
    else
        pTarget = NULL;

    m_caster->GetMap()->ScriptsStart(sEventScripts, m_spellInfo->EffectMiscValue[effIndex], m_caster, pTarget);
}

void Spell::EffectPowerBurn(SpellEffIndex effIndex)
{
    if (m_spellInfo->EffectMiscValue[effIndex] < 0 || m_spellInfo->EffectMiscValue[effIndex] >= MAX_POWERS)
        return;

    Powers powerType = Powers(m_spellInfo->EffectMiscValue[effIndex]);

    if (!unitTarget || !unitTarget->IsAlive() || unitTarget->getPowerType() != powerType || damage < 0)
        return;

    int32 curPower = int32(unitTarget->GetPower(powerType));

    // resilience reduce mana draining effect at spell crit damage reduction (added in 2.4)
    uint32 power = damage;
    if (powerType == POWER_MANA && unitTarget->GetTypeId() == TYPEID_PLAYER)
        power -= unitTarget->ToPlayer()->GetSpellCritDamageReduction(power);

    int32 new_damage = (curPower < int32(power)) ? curPower : int32(power);

    unitTarget->ModifyPower(powerType, -new_damage);
    float multiplier = m_spellInfo->EffectMultipleValue[effIndex];

    if (Player* modOwner = m_caster->GetSpellModOwner())
        modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_MULTIPLE_VALUE, multiplier);

    new_damage = int32(new_damage * multiplier);
    //m_damage+=new_damage; should not apply spell bonus
    //@todo no log
    //unitTarget->ModifyHealth(-new_damage);
    if (m_originalCaster)
        m_originalCaster->DealDamage(unitTarget, new_damage);
}

void Spell::EffectHeal(SpellEffIndex /*effIndex*/)
{
}

void Spell::SpellDamageHeal(SpellEffIndex /*effIndex*/)
{
    if (unitTarget && unitTarget->IsAlive() && damage >= 0)
    {
        // Try to get original caster
        Unit* caster = m_originalCasterGUID ? m_originalCaster : m_caster;

        // Skip if m_originalCaster not available
        if (!caster)
            return;

        int32 addhealth = damage;

        // Vessel of the Naaru (Vial of the Sunwell trinket)
        if (m_spellInfo->Id == 45064)
        {
            // Amount of heal - depends from stacked Holy Energy
            int damageAmount = 0;
            Unit::AuraList const& mDummyAuras = m_caster->GetAurasByType(SPELL_AURA_DUMMY);
            for (Unit::AuraList::const_iterator i = mDummyAuras.begin(); i != mDummyAuras.end(); ++i)
                if ((*i)->GetId() == 45062)
                    damageAmount += (*i)->GetModifierValue();
            if (damageAmount)
                m_caster->RemoveAurasDueToSpell(45062);

            addhealth += damageAmount;
        }
        // Swiftmend - consumes Regrowth or Rejuvenation
        else if (m_spellInfo->TargetAuraState == AURA_STATE_SWIFTMEND && unitTarget->HasAuraState(AURA_STATE_SWIFTMEND))
        {
            Unit::AuraList const& RejorRegr = unitTarget->GetAurasByType(SPELL_AURA_PERIODIC_HEAL);
            // find most short by duration
            Aura* targetAura = NULL;
            for (Unit::AuraList::const_iterator i = RejorRegr.begin(); i != RejorRegr.end(); ++i)
            {
                if ((*i)->GetSpellProto()->SpellFamilyName == SPELLFAMILY_DRUID
                    && ((*i)->GetSpellProto()->SpellFamilyFlags == 0x40 || (*i)->GetSpellProto()->SpellFamilyFlags == 0x10))
                {
                    if (!targetAura || (*i)->GetAuraDuration() < targetAura->GetAuraDuration())
                        targetAura = *i;
                }
            }

            if (!targetAura)
            {
                sLog.outError("Target(GUID:" UI64FMTD ") has aurastate AURA_STATE_SWIFTMEND but no matching aura.", unitTarget->GetGUID());
                return;
            }

            int32 tickheal = targetAura->GetModifierValuePerStack();
            if (Unit* auraCaster = targetAura->GetCaster())
                tickheal = auraCaster->SpellHealingBonus(targetAura->GetSpellProto(), tickheal, DOT, unitTarget);
            //int32 tickheal = targetAura->GetSpellProto()->EffectBasePoints[idx] + 1;
            //It is said that talent bonus should not be included

            int32 tickcount = 0;
            // Rejuvenation
            if (targetAura->GetSpellProto()->SpellFamilyFlags & 0x10)
                tickcount = 4;
            // Regrowth
            else // if (targetAura->GetSpellInfo()->SpellFamilyFlags[0] & 0x40)
                tickcount = 6;

            addhealth += tickheal * tickcount;

            unitTarget->RemoveAurasByCasterSpell(targetAura->GetId(), targetAura->GetCasterGUID());

            //addhealth += tickheal * tickcount;
            //addhealth = caster->SpellHealingBonus(m_spellInfo, addhealth,HEAL, unitTarget);
        }
        else
            addhealth = caster->SpellHealingBonus(m_spellInfo, addhealth, HEAL, unitTarget);

        m_damage -= addhealth;
    }
}

void Spell::EffectHealPct(SpellEffIndex /*effIndex*/)
{
    if (unitTarget && unitTarget->IsAlive() && damage >= 0)
    {
        // Try to get original caster
        Unit* caster = m_originalCasterGUID ? m_originalCaster : m_caster;

        // Skip if m_originalCaster not available
        if (!caster)
            return;

        uint32 addhealth = unitTarget->GetMaxHealth() * damage / 100;

        caster->SendHealSpellLog(unitTarget, m_spellInfo->Id, addhealth, false);
        int32 gain = unitTarget->ModifyHealth(int32(addhealth));
        unitTarget->getHostileRefManager().threatAssist(m_caster, float(gain) * 0.5f, m_spellInfo);

        if (caster->GetTypeId() == TYPEID_PLAYER)
            if (Battleground* bg = caster->ToPlayer()->GetBattleground())
                bg->UpdatePlayerScore(caster->ToPlayer(), SCORE_HEALING_DONE, gain);
    }
}

void Spell::EffectHealMechanical(SpellEffIndex /*effIndex*/)
{
    // Mechanic creature type should be correctly checked by targetCreatureType field
    if (unitTarget && unitTarget->IsAlive() && damage >= 0)
    {
        // Try to get original caster
        Unit* caster = m_originalCasterGUID ? m_originalCaster : m_caster;

        // Skip if m_originalCaster not available
        if (!caster)
            return;

        uint32 addhealth = caster->SpellHealingBonus(m_spellInfo, uint32(damage), HEAL, unitTarget);
        caster->SendHealSpellLog(unitTarget, m_spellInfo->Id, addhealth, false);
        unitTarget->ModifyHealth(int32(damage));
    }
}

void Spell::EffectHealthLeech(SpellEffIndex effIndex)
{
    if (!unitTarget)
        return;
    if (!unitTarget->IsAlive())
        return;

    if (damage < 0)
        return;

    DEBUG_LOG("HealthLeech :%i", damage);

    float multiplier = m_spellInfo->EffectMultipleValue[effIndex];

    if (Player* modOwner = m_caster->GetSpellModOwner())
        modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_MULTIPLE_VALUE, multiplier);

    int32 new_damage = int32(damage * multiplier);
    uint32 curHealth = unitTarget->GetHealth();
    new_damage = m_caster->SpellNonMeleeDamageLog(unitTarget, m_spellInfo->Id, new_damage, m_IsTriggeredSpell, true);
    if (int64(curHealth) < new_damage)
        new_damage = curHealth;

    if (m_caster->IsAlive())
    {
        new_damage = m_caster->SpellHealingBonus(m_spellInfo, new_damage, HEAL, m_caster);

        m_caster->ModifyHealth(new_damage);

        if (m_caster->GetTypeId() == TYPEID_PLAYER)
            m_caster->SendHealSpellLog(m_caster, m_spellInfo->Id, uint32(new_damage));
    }
    //    m_healthLeech+=tmpvalue;
    //    m_damage+=new_damage;
}

void Spell::DoCreateItem(uint32 /*i*/, uint32 itemtype)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* player = unitTarget->ToPlayer();

    uint32 newitemid = itemtype;
    ItemTemplate const* pProto = sObjectMgr.GetItemTemplate(newitemid);
    if (!pProto)
    {
        player->SendEquipError(EQUIP_ERR_ITEM_NOT_FOUND, NULL, NULL);
        return;
    }

    uint32 num_to_add;

    // @todo maybe all this can be replaced by using correct calculated `damage` value
    if (pProto->Class != ITEM_CLASS_CONSUMABLE || m_spellInfo->SpellFamilyName != SPELLFAMILY_MAGE)
    {
        num_to_add = damage;
        /*int32 basePoints = m_currentBasePoints[i];
        int32 randomPoints = m_spellInfo->EffectDieSides[i];
        if (randomPoints)
            num_to_add = basePoints + irand(1, randomPoints);
        else
            num_to_add = basePoints + 1;*/
    }
    else if (pProto->MaxCount == 1)
        num_to_add = 1;
    else if (player->getLevel() >= m_spellInfo->spellLevel)
    {
        num_to_add = damage;
        /*int32 basePoints = m_currentBasePoints[i];
        float pointPerLevel = m_spellInfo->EffectRealPointsPerLevel[i];
        num_to_add = basePoints + 1 + uint32((player->getLevel() - m_spellInfo->spellLevel)*pointPerLevel);*/
    }
    else
        num_to_add = 2;

    if (num_to_add < 1)
        num_to_add = 1;
    if (num_to_add > pProto->Stackable)
        num_to_add = pProto->Stackable;

    // init items_count to 1, since 1 item will be created regardless of specialization
    int items_count = 1;
    // the chance to create additional items
    float additionalCreateChance = 0.0f;
    // the maximum number of created additional items
    uint8 additionalMaxNum = 0;
    // get the chance and maximum number for creating extra items
    if (canCreateExtraItems(player, m_spellInfo->Id, additionalCreateChance, additionalMaxNum))
    {
        // roll with this chance till we roll not to create or we create the max num
        while (roll_chance_f(additionalCreateChance) && items_count <= additionalMaxNum)
            ++items_count;
    }

    // really will be created more items
    num_to_add *= items_count;

    // can the player store the new item?
    ItemPosCountVec dest;
    uint32 no_space = 0;
    uint8 msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, newitemid, num_to_add, &no_space);
    if (msg != EQUIP_ERR_OK)
    {
        // convert to possible store amount
        if (msg == EQUIP_ERR_INVENTORY_FULL || msg == EQUIP_ERR_CANT_CARRY_MORE_OF_THIS)
            num_to_add -= no_space;
        else
        {
            // if not created by another reason from full inventory or unique items amount limitation
            player->SendEquipError(msg, NULL, NULL);
            return;
        }
    }

    if (num_to_add)
    {
        // create the new item and store it
        Item* pItem = player->StoreNewItem(dest, newitemid, true, Item::GenerateItemRandomPropertyId(newitemid));

        // was it successful? return error if not
        if (!pItem)
        {
            player->SendEquipError(EQUIP_ERR_ITEM_NOT_FOUND, NULL, NULL);
            return;
        }

        // set the "Crafted by ..." property of the item
        if (pItem->GetProto()->Class != ITEM_CLASS_CONSUMABLE && pItem->GetProto()->Class != ITEM_CLASS_QUEST && newitemid != 6265 /*Soul Shard*/ && newitemid != 6948 /*Hearthstone*/)
            pItem->SetUInt32Value(ITEM_FIELD_CREATOR, player->GetGUIDLow());

        // send info to the client
        player->SendNewItem(pItem, num_to_add, true, true);

        // we succeeded in creating at least one item, so a levelup is possible
        player->UpdateCraftSkill(m_spellInfo->Id);
    }
}

void Spell::EffectCreateItem(SpellEffIndex effIndex)
{
    DoCreateItem(effIndex, m_spellInfo->EffectItemType[effIndex]);
}

void Spell::EffectPersistentAA(SpellEffIndex effIndex)
{
    Unit* caster = m_caster->GetEntry() == WORLD_TRIGGER ? m_originalCaster : m_caster;
    float radius = GetSpellRadius(m_spellInfo, effIndex, false);
    int32 duration = GetSpellDuration(m_spellInfo);

    // Caster not in world, might be spell triggered from aura removal
    if (!caster->IsInWorld())
        return;

    // Apply spell mods to radius
    if (Player* modOwner = m_originalCaster->GetSpellModOwner())
        modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_RADIUS, radius);

    // Apply spell mods to duration
    if (Player* modOwner = m_originalCaster->GetSpellModOwner())
        modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_DURATION, duration);

    DynamicObject* dynObj = new DynamicObject(false);
    if (!dynObj->CreateDynamicObject(sObjectMgr.GenerateLowGuid(HIGHGUID_DYNAMICOBJECT), caster, m_spellInfo->Id, effIndex, m_targets.m_dstPos, duration, radius))
    {
        delete dynObj;
        return;
    }

    dynObj->SetUInt32Value(OBJECT_FIELD_TYPE, 65);
    caster->AddDynObject(dynObj);
    dynObj->GetMap()->AddToMap(dynObj);
}

void Spell::EffectEnergize(SpellEffIndex effIndex)
{
    if (!unitTarget)
        return;
    if (!unitTarget->IsAlive())
        return;

    if (m_spellInfo->EffectMiscValue[effIndex] < 0 || m_spellInfo->EffectMiscValue[effIndex] >= MAX_POWERS)
        return;

    // Some level depends spells
    int multiplier = 0;
    int level_diff = 0;
    switch (m_spellInfo->Id)
    {
    case 9512:                                          // Restore Energy
        level_diff = m_caster->getLevel() - 40;
        multiplier = 2;
        break;
    case 24571:                                         // Blood Fury
        level_diff = m_caster->getLevel() - 60;
        multiplier = 10;
        break;
    case 24532:                                         // Burst of Energy
        level_diff = m_caster->getLevel() - 60;
        multiplier = 4;
        break;
    default:
        break;
    }

    if (level_diff > 0)
        damage -= multiplier * level_diff;

    if (damage < 0)
        return;

    // Handle Mana Gems / Serpent-Coil Braid
    if (m_spellInfo->SpellFamilyName == SPELLFAMILY_MAGE && m_spellInfo->SpellFamilyFlags == 0x10000000000LL)
        if (unitTarget->HasAura(37447, 0))
            unitTarget->CastSpell(unitTarget, 37445, true);

    Powers power = Powers(m_spellInfo->EffectMiscValue[effIndex]);

    if (unitTarget->GetMaxPower(power) == 0)
        return;

    unitTarget->ModifyPower(power, damage);
    m_caster->SendEnergizeSpellLog(unitTarget, m_spellInfo->Id, damage, power);

    // Mad Alchemist's Potion
    if (m_spellInfo->Id == 45051)
    {
        // find elixirs on target
        bool guardianFound = false;
        bool battleFound = false;
        Unit::AuraMap& Auras = unitTarget->GetAuras();
        for (Unit::AuraMap::iterator itr = Auras.begin(); itr != Auras.end(); ++itr)
        {
            uint32 spell_id = itr->second->GetId();
            if (!guardianFound)
                if (sSpellMgr.IsSpellMemberOfSpellGroup(spell_id, SPELL_GROUP_ELIXIR_GUARDIAN))
                    guardianFound = true;
            if (!battleFound)
                if (sSpellMgr.IsSpellMemberOfSpellGroup(spell_id, SPELL_GROUP_ELIXIR_BATTLE))
                    battleFound = true;
            if (battleFound && guardianFound)
                break;
        }

        // get all available elixirs by mask and spell level
        std::set<uint32> avalibleElixirs;
        if (!guardianFound)
            sSpellMgr.GetSetOfSpellsInSpellGroup(SPELL_GROUP_ELIXIR_GUARDIAN, avalibleElixirs);
        if (!battleFound)
            sSpellMgr.GetSetOfSpellsInSpellGroup(SPELL_GROUP_ELIXIR_BATTLE, avalibleElixirs);
        for (std::set<uint32>::iterator itr = avalibleElixirs.begin(); itr != avalibleElixirs.end() ;)
        {
            SpellEntry const* spellInfo = sSpellStore.LookupEntry(*itr);
            if (spellInfo->spellLevel < m_spellInfo->spellLevel || spellInfo->spellLevel > unitTarget->getLevel())
                avalibleElixirs.erase(itr++);
            else if (sSpellMgr.IsSpellMemberOfSpellGroup(*itr, SPELL_GROUP_ELIXIR_SHATTRATH))
                avalibleElixirs.erase(itr++);
            else if (sSpellMgr.IsSpellMemberOfSpellGroup(*itr, SPELL_GROUP_ELIXIR_UNSTABLE))
                avalibleElixirs.erase(itr++);
            else
                ++itr;
        }

        if (!avalibleElixirs.empty())
        {
            // cast random elixir on target
            uint32 rand_spell = urand(0, avalibleElixirs.size() - 1);
            std::set<uint32>::iterator itr = avalibleElixirs.begin();
            std::advance(itr, rand_spell);
            m_caster->CastSpell(unitTarget, *itr, true, m_CastItem);
        }
    }
}

void Spell::EffectEnergizePct(SpellEffIndex effIndex)
{
    if (!unitTarget)
        return;
    if (!unitTarget->IsAlive())
        return;

    if (m_spellInfo->EffectMiscValue[effIndex] < 0 || m_spellInfo->EffectMiscValue[effIndex] >= MAX_POWERS)
        return;

    Powers power = Powers(m_spellInfo->EffectMiscValue[effIndex]);

    uint32 maxPower = unitTarget->GetMaxPower(power);
    if (maxPower == 0)
        return;

    uint32 gain = damage * maxPower / 100;
    unitTarget->ModifyPower(power, gain);
    m_caster->SendEnergizeSpellLog(unitTarget, m_spellInfo->Id, gain, power);
}

void Spell::SendLoot(uint64 guid, LootType loottype)
{
    Player* player = m_caster->ToPlayer();
    if (!player)
        return;

    if (gameObjTarget)
    {
        if (sScriptMgr.GOHello(player, gameObjTarget))
            return;

        if (gameObjTarget->AI()->GossipHello(player))
            return;

        switch (gameObjTarget->GetGoType())
        {
        case GAMEOBJECT_TYPE_DOOR:
        case GAMEOBJECT_TYPE_BUTTON:
            gameObjTarget->UseDoorOrButton(0, false, player);
            player->GetMap()->ScriptsStart(sGameObjectScripts, gameObjTarget->GetDBTableGUIDLow(), player, gameObjTarget);
            return;

        case GAMEOBJECT_TYPE_QUESTGIVER:
            // start or end quest
            player->PrepareQuestMenu(guid);
            player->SendPreparedQuest(guid);
            return;

        case GAMEOBJECT_TYPE_SPELL_FOCUS:
            // triggering linked GO
            if (uint32 trapEntry = gameObjTarget->GetGOInfo()->spellFocus.linkedTrapId)
                gameObjTarget->TriggeringLinkedGameObject(trapEntry, m_caster);
            return;

        case GAMEOBJECT_TYPE_GOOBER:
            // goober_scripts can be triggered if the player don't have the quest
            if (gameObjTarget->GetGOInfo()->goober.eventId)
            {
                DEBUG_LOG("Goober ScriptStart id %u for GO %u", gameObjTarget->GetGOInfo()->goober.eventId, gameObjTarget->GetDBTableGUIDLow());
                player->GetMap()->ScriptsStart(sEventScripts, gameObjTarget->GetGOInfo()->goober.eventId, player, gameObjTarget);
            }

            // cast goober spell
            if (gameObjTarget->GetGOInfo()->goober.questId)
                // Quest require to be active for GO using
                if (player->GetQuestStatus(gameObjTarget->GetGOInfo()->goober.questId) != QUEST_STATUS_INCOMPLETE)
                    return;

            player->GetMap()->ScriptsStart(sGameObjectScripts, gameObjTarget->GetDBTableGUIDLow(), player, gameObjTarget);

            gameObjTarget->AddUniqueUse(player);
            gameObjTarget->SetLootState(GO_JUST_DEACTIVATED);

            //TODO? Objective counting called without spell check but with quest objective check
            // if send spell id then this line will duplicate to spell casting call (double counting)
            // So we or have this line and not required in quest_template have reqSpellIdN
            // or must remove this line and required in DB have data in quest_template have reqSpellIdN for all quest using cases.
            player->CastedCreatureOrGO(gameObjTarget->GetEntry(), gameObjTarget->GetGUID(), 0);

            // triggering linked GO
            if (uint32 trapEntry = gameObjTarget->GetGOInfo()->goober.linkedTrapId)
                gameObjTarget->TriggeringLinkedGameObject(trapEntry, m_caster);

            return;

        case GAMEOBJECT_TYPE_CHEST:
            // @todo possible must be moved to loot release (in different from linked triggering)
            if (gameObjTarget->GetGOInfo()->chest.eventId)
            {
                DEBUG_LOG("Chest ScriptStart id %u for GO %u", gameObjTarget->GetGOInfo()->chest.eventId, gameObjTarget->GetDBTableGUIDLow());
                player->GetMap()->ScriptsStart(sEventScripts, gameObjTarget->GetGOInfo()->chest.eventId, player, gameObjTarget);
            }

            // triggering linked GO
            if (uint32 trapEntry = gameObjTarget->GetGOInfo()->chest.linkedTrapId)
                gameObjTarget->TriggeringLinkedGameObject(trapEntry, m_caster);

        // Don't return, let loots been taken
        default:
            break;
        }
    }

    // Send loot
    player->SendLoot(guid, loottype);
}

void Spell::EffectOpenLock(SpellEffIndex effIndex)
{
    if (!m_caster)
        return;

    if (m_caster->GetTypeId() == TYPEID_UNIT && gameObjTarget)
    {
        gameObjTarget->UseDoorOrButton();
        return;
    }

    Player* player = m_caster->ToPlayer();
    if (!player)
        return;

    LootType loottype = LOOT_CORPSE;
    uint32 lockId = 0;
    uint64 guid = 0;

    // Get lockId
    if (gameObjTarget)
    {
        GameObjectInfo const* goInfo = gameObjTarget->GetGOInfo();
        // Arathi Basin banner opening !
        if ((goInfo->type == GAMEOBJECT_TYPE_BUTTON && goInfo->button.noDamageImmune) ||
            (goInfo->type == GAMEOBJECT_TYPE_GOOBER && goInfo->goober.losOK) ||
            (goInfo->type == GAMEOBJECT_TYPE_FLAGSTAND))
        {
            // CanUseBattlegroundObject() already called in CanCast()
            // in battleground check
            if (Battleground* bg = player->GetBattleground())
                bg->EventPlayerClickedOnFlag(player, gameObjTarget);
                return;
        }
        // handle outdoor pvp object opening, return true if go was registered for handling
        // these objects must have been spawned by outdoorpvp!
        else if (gameObjTarget->GetGOInfo()->type == GAMEOBJECT_TYPE_GOOBER && sOutdoorPvPMgr.HandleOpenGo(player, gameObjTarget->GetGUID()))
            return;

        lockId = gameObjTarget->GetGOInfo()->GetLockId();
        guid = gameObjTarget->GetGUID();
    }
    else if (itemTarget)
    {
        lockId = itemTarget->GetProto()->LockID;
        guid = itemTarget->GetGUID();
    }
    else
    {
        sLog.outError("WORLD: Open Lock - No GameObject/Item Target! SpellId; %u", m_spellInfo->Id);
        return;
    }

    if (!lockId)                                             // possible case for GO and maybe for items.
    {
        SendLoot(guid, loottype);
        return;
    }

    // Get LockInfo
    LockEntry const* lockInfo = sLockStore.LookupEntry(lockId);

    if (!lockInfo)
    {
        sLog.outError("Spell::EffectOpenLock: %s [guid = %u] has an unknown lockId: %u!",
                      (gameObjTarget ? "gameobject" : "item"), GUID_LOPART(guid), lockId);
        return;
    }

    // check key
    for (int i = 0; i < 5; ++i)
    {
        // type == 1 This means lockInfo->key[i] is an item
        if (lockInfo->keytype[i] == LOCK_KEY_ITEM && lockInfo->key[i] && m_CastItem && m_CastItem->GetEntry() == lockInfo->key[i])
        {
            SendLoot(guid, loottype);
            return;
        }
    }

    uint32 SkillId = 0;
    // Check and skill-up skill
    if (m_spellInfo->Effect[EFFECT_1] == SPELL_EFFECT_SKILL)
        SkillId = m_spellInfo->EffectMiscValue[EFFECT_1];
    // pickpocketing spells
    else if (m_spellInfo->EffectMiscValue[EFFECT_0] == LOCKTYPE_PICKLOCK)
        SkillId = SKILL_LOCKPICKING;

    uint32 reqSkillValue = lockInfo->requiredminingskill;
    if (m_spellInfo->EffectMiscValue[effIndex] == LOCKTYPE_PICKLOCK)
        reqSkillValue = lockInfo->requiredlockskill;

    if (SkillId)
    {
        loottype = LOOT_SKINNING;

        // update skill if really known
        uint32 SkillValue = player->GetPureSkillValue(SkillId);
        if (SkillValue)                                      // non only item base skill
        {
            if (gameObjTarget)
            {
                // Allow one skill-up until respawned
                if (!gameObjTarget->IsInSkillupList(player->GetGUIDLow()) && player->UpdateGatherSkill(SkillId, SkillValue, reqSkillValue))
                    gameObjTarget->AddToSkillupList(player->GetGUIDLow());
            }
            else if (itemTarget)
            {
                // Do one skill-up
                uint32 SkillValue = player->GetPureSkillValue(SkillId);
                player->UpdateGatherSkill(SkillId, SkillValue, reqSkillValue);
            }
        }
    }

    SendLoot(guid, loottype);
}

void Spell::EffectSummonChangeItem(SpellEffIndex effIndex)
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* player = m_caster->ToPlayer();

    // applied only to using item
    if (!m_CastItem)
        return;

    // ... only to item in own inventory/bank/equip_slot
    if (m_CastItem->GetOwnerGUID() != player->GetGUID())
        return;

    uint32 newitemid = m_spellInfo->EffectItemType[effIndex];
    if (!newitemid)
        return;

    uint16 pos = m_CastItem->GetPos();

    Item* pNewItem = Item::CreateItem(newitemid, 1, player);
    if (!pNewItem)
        return;

    for (uint8 j = PERM_ENCHANTMENT_SLOT; j <= TEMP_ENCHANTMENT_SLOT; ++j)
    {
        if (m_CastItem->GetEnchantmentId(EnchantmentSlot(j)))
            pNewItem->SetEnchantment(EnchantmentSlot(j), m_CastItem->GetEnchantmentId(EnchantmentSlot(j)), m_CastItem->GetEnchantmentDuration(EnchantmentSlot(j)), m_CastItem->GetEnchantmentCharges(EnchantmentSlot(j)));
    }

    if (m_CastItem->GetUInt32Value(ITEM_FIELD_DURABILITY) < m_CastItem->GetUInt32Value(ITEM_FIELD_MAXDURABILITY))
    {
        double loosePercent = 1 - m_CastItem->GetUInt32Value(ITEM_FIELD_DURABILITY) / double(m_CastItem->GetUInt32Value(ITEM_FIELD_MAXDURABILITY));
        player->DurabilityLoss(pNewItem, loosePercent);
    }

    if (player->IsInventoryPos(pos))
    {
        ItemPosCountVec dest;
        uint8 msg = player->CanStoreItem(m_CastItem->GetBagSlot(), m_CastItem->GetSlot(), dest, pNewItem, true);
        if (msg == EQUIP_ERR_OK)
        {
            player->DestroyItem(m_CastItem->GetBagSlot(), m_CastItem->GetSlot(), true);

            // prevent crash at access and unexpected charges counting with item update queue corrupt
            if (m_CastItem == m_targets.getItemTarget())
                m_targets.setItemTarget(NULL);

            m_CastItem = NULL;

            player->StoreItem(dest, pNewItem, true);
            player->SendNewItem(pNewItem, 1, true, false);
            player->ItemAddedQuestCheck(newitemid, 1);
            return;
        }
    }
    else if (player->IsBankPos (pos))
    {
        ItemPosCountVec dest;
        uint8 msg = player->CanBankItem(m_CastItem->GetBagSlot(), m_CastItem->GetSlot(), dest, pNewItem, true);
        if (msg == EQUIP_ERR_OK)
        {
            player->DestroyItem(m_CastItem->GetBagSlot(), m_CastItem->GetSlot(), true);

            // prevent crash at access and unexpected charges counting with item update queue corrupt
            if (m_CastItem == m_targets.getItemTarget())
                m_targets.setItemTarget(NULL);

            m_CastItem = NULL;

            player->BankItem(dest, pNewItem, true);
            return;
        }
    }
    else if (player->IsEquipmentPos (pos))
    {
        uint16 dest;
        uint8 msg = player->CanEquipItem(m_CastItem->GetSlot(), dest, pNewItem, true);
        if (msg == EQUIP_ERR_OK)
        {
            player->DestroyItem(m_CastItem->GetBagSlot(), m_CastItem->GetSlot(), true);

            // prevent crash at access and unexpected charges counting with item update queue corrupt
            if (m_CastItem == m_targets.getItemTarget())
                m_targets.setItemTarget(NULL);

            m_CastItem = NULL;

            player->EquipItem(dest, pNewItem, true);
            player->AutoUnequipOffhandIfNeed();
            player->SendNewItem(pNewItem, 1, true, false);
            player->ItemAddedQuestCheck(newitemid, 1);
            return;
        }
    }

    // fail
    delete pNewItem;
}

void Spell::EffectOpenSecretSafe(SpellEffIndex effIndex)
{
    EffectOpenLock(effIndex);                                      //no difference for now
}

void Spell::EffectProficiency(SpellEffIndex /*effIndex*/)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;
    Player* p_target = unitTarget->ToPlayer();

    uint32 subClassMask = m_spellInfo->EquippedItemSubClassMask;
    if (m_spellInfo->EquippedItemClass == 2 && !(p_target->GetWeaponProficiency() & subClassMask))
    {
        p_target->AddWeaponProficiency(subClassMask);
        p_target->SendProficiency(ITEM_CLASS_WEAPON, p_target->GetWeaponProficiency());
    }
    if (m_spellInfo->EquippedItemClass == 4 && !(p_target->GetArmorProficiency() & subClassMask))
    {
        p_target->AddArmorProficiency(subClassMask);
        p_target->SendProficiency(ITEM_CLASS_ARMOR, p_target->GetArmorProficiency());
    }
}

void Spell::EffectSummonType(SpellEffIndex effIndex)
{
    uint32 entry = m_spellInfo->EffectMiscValue[effIndex];
    if (!entry)
        return;

    uint32 prop_id = m_spellInfo->EffectMiscValueB[effIndex];
    SummonPropertiesEntry const* properties = sSummonPropertiesStore.LookupEntry(prop_id);
    if (!properties)
    {
        sLog.outError("EffectSummonType: Unhandled summon type %u", prop_id);
        return;
    }

    if (!m_originalCaster)
        return;

    int32 duration = GetSpellDuration(m_spellInfo);
    if (Player* modOwner = m_originalCaster->GetSpellModOwner())
        modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_DURATION, duration);

    Position pos;
    GetSummonPosition(effIndex, pos);

    TempSummon* summon = NULL;

    switch (properties->Category)
    {
    case SUMMON_CATEGORY_WILD:
    case SUMMON_CATEGORY_ALLY:
        if (properties->Flags & 512)
        {
            SummonGuardian(effIndex, entry, properties);
            break;
        }
        switch (properties->Type)
        {
        case SUMMON_TYPE_PET:
        case SUMMON_TYPE_GUARDIAN:
        case SUMMON_TYPE_GUARDIAN2:
        case SUMMON_TYPE_MINION:
            SummonGuardian(effIndex, entry, properties);
            break;
        case SUMMON_TYPE_TOTEM:
            {
                summon = m_caster->GetMap()->SummonCreature(entry, pos, properties, duration, m_originalCaster, m_spellInfo->Id);
                if (!summon || !summon->IsTotem())
                    return;

                if (damage)                                            // if not spell info, DB values used
                {
                    summon->SetMaxHealth(damage);
                    summon->SetHealth(damage);
                }
                break;
            }
        case SUMMON_TYPE_MINIPET:
            {
                // If "resummoning" the same minipet, despawn only.
                TempSummon* oldCritter = dynamic_cast<TempSummon*>(ObjectAccessor::GetCreatureOrPet(*m_caster, m_caster->GetCritterGUID()));

                if (oldCritter)
                {
                    oldCritter->UnSummon();

                    if (oldCritter->GetEntry() == entry)
                        return;
                }

                summon = m_caster->GetMap()->SummonCreature(entry, pos, properties, duration, m_originalCaster);
                if (!summon || !summon->HasUnitTypeMask(UNIT_MASK_MINION))
                    return;

                m_caster->SetCritterGUID(summon->GetGUID());

                summon->SelectLevel();       // some summoned creaters have different from 1 DB data for level/hp
                summon->SetUInt32Value(UNIT_NPC_FLAGS, summon->GetCreatureTemplate()->npcflag);

                summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);

                summon->AI()->EnterEvadeMode();
                break;
            }
        default:
            {
                TempSummonType summonType = (duration == 0) ? TEMPSUMMON_DEAD_DESPAWN : TEMPSUMMON_TIMED_DESPAWN;
                summon = m_originalCaster->SummonCreature(entry, pos, summonType, duration);
                if (!summon)
                    return;

                if (properties->Category == SUMMON_CATEGORY_ALLY)
                {
                    summon->SetUInt64Value(UNIT_FIELD_SUMMONEDBY, m_originalCaster->GetGUID());
                    summon->SetFaction(m_originalCaster->GetFaction());
                    summon->SetLevel(m_originalCaster->getLevel());
                    summon->SetUInt32Value(UNIT_CREATED_BY_SPELL, m_spellInfo->Id);
                }

                switch (m_spellInfo->Id)
                {
                    case 45392:
                        if (summon->IsAIEnabled)
                            summon->AI()->AttackStart(m_caster);
                        break;
                    default:
                        if (summon->IsAIEnabled && m_originalCaster->IsInCombat())
                            summon->AI()->EnterCombat(nullptr);
                        break;
                }
            }
            return;
        }
        break;
    case SUMMON_CATEGORY_PET:
        SummonClassPet(effIndex);
        break;
    case SUMMON_CATEGORY_PUPPET:
        // only player may have a puppet, or else
        // there will be a failed assertion in Puppet::Puppet
        if (m_caster->GetTypeId() != TYPEID_PLAYER)
            break;

        summon = m_caster->GetMap()->SummonCreature(entry, pos, properties, duration, m_originalCaster);

        uint32 faction = properties->Faction;
        if (!faction)
            faction = m_originalCaster->GetFaction();

        summon->SetFaction(faction);

        if (prop_id == 65) // Eye of Kilrogg
            summon->CastSpell(summon, 2585, true);
        break;
    }

    if (summon)
    {
        summon->SetCreatorGUID(m_originalCaster->GetGUID());
        summon->SetUInt32Value(UNIT_CREATED_BY_SPELL, m_spellInfo->Id);
    }
}

void Spell::EffectLearnSpell(SpellEffIndex effIndex)
{
    if (!unitTarget)
        return;

    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
    {
        if (m_caster->GetTypeId() == TYPEID_PLAYER)
            EffectLearnPetSpell(effIndex);

        return;
    }

    Player* player = unitTarget->ToPlayer();

    uint32 spellToLearn = (m_spellInfo->Id == SPELL_GENERIC_LEARN) ? damage : m_spellInfo->EffectTriggerSpell[effIndex];
    player->LearnSpell(spellToLearn);

    DEBUG_LOG("Spell: Player %u has learned spell %u from NpcGUID=%u", player->GetGUIDLow(), spellToLearn, m_caster->GetGUIDLow());
}

void Spell::EffectDispel(SpellEffIndex effIndex)
{
    if (!unitTarget)
        return;

    // Fill possible dispell list
    std::vector <Aura*> dispel_list;

    if (unitTarget->IsHostileTo(m_caster))
    {
        m_caster->SetInCombatWith(unitTarget);
        unitTarget->SetInCombatWith(m_caster);
    }

    // Create dispel mask by dispel type
    uint32 dispel_type = m_spellInfo->EffectMiscValue[effIndex];
    uint32 dispelMask  = GetDispelMask(DispelType(dispel_type));
    Unit::AuraMap const& auras = unitTarget->GetAuras();
    for (Unit::AuraMap::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
    {
        Aura* aur = (*itr).second;
        if (aur && (1 << aur->GetSpellProto()->Dispel) & dispelMask)
        {
            if (aur->GetSpellProto()->Dispel == DISPEL_MAGIC)
            {
                // do not remove positive auras if friendly target
                //               negative auras if non-friendly target
                if (aur->IsPositive() == unitTarget->IsFriendlyTo(m_caster))
                    continue;
            }
            // Add every aura stack to dispel list
            for (int stack_amount = 0; stack_amount < aur->GetStackAmount(); ++stack_amount)
                dispel_list.push_back(aur);
        }
    }
    // Ok if exist some buffs for dispel try dispel it
    if (!dispel_list.empty())
    {
        std::list < std::pair<uint32, uint64> > success_list; // (spell_id,casterGuid)
        std::list < uint32 > fail_list;                     // spell_id

        int32 list_size = dispel_list.size();
        // dispel N = damage buffs (or while exist buffs for dispel)
        for (int32 count = 0; count < damage && list_size > 0; ++count)
        {
            // Random select buff for dispel
            Aura* aur = dispel_list[urand(0, list_size - 1)];

            SpellEntry const* spellInfo = aur->GetSpellProto();
            // Base dispel chance
            // @todo possible chance depend from spell level??
            int32 miss_chance = 0;
            // Apply dispel mod from aura caster
            if (Unit* caster = aur->GetCaster())
            {
                if (Player* modOwner = caster->GetSpellModOwner())
                    modOwner->ApplySpellMod(spellInfo->Id, SPELLMOD_RESIST_DISPEL_CHANCE, miss_chance, this);
            }
            // Try dispel
            if (roll_chance_i(miss_chance))
                fail_list.push_back(aur->GetId());
            else
                success_list.push_back(std::pair<uint32, uint64>(aur->GetId(), aur->GetCasterGUID()));
            // Remove buff from list for prevent doubles
            for (std::vector<Aura*>::iterator j = dispel_list.begin(); j != dispel_list.end();)
            {
                Aura* dispelled = *j;
                if (dispelled->GetId() == aur->GetId() && dispelled->GetCasterGUID() == aur->GetCasterGUID())
                {
                    j = dispel_list.erase(j);
                    --list_size;
                    break;
                }
                else
                    ++j;
            }
        }
        // Send success log and really remove auras
        if (!success_list.empty())
        {
            int32 count = success_list.size();
            WorldPacket data(SMSG_SPELLDISPELLOG, 8 + 8 + 4 + 1 + 4 + count * 5);
            data << unitTarget->GetPackGUID();              // Victim GUID
            data << m_caster->GetPackGUID();                // Caster GUID
            data << uint32(m_spellInfo->Id);                // dispel spell id
            data << uint8(0);                               // not used
            data << uint32(count);                          // count
            for (std::list<std::pair<uint32, uint64> >::iterator j = success_list.begin(); j != success_list.end(); ++j)
            {
                SpellEntry const* spellInfo = sSpellStore.LookupEntry(j->first);
                data << uint32(spellInfo->Id);              // Spell Id
                data << uint8(0);                           // 0 - dispelled != 0 cleansed
                if (spellInfo->StackAmount != 0)
                {
                    //Why are Aura's Removed by EffIndex? Auras should be removed as a whole.....
                    unitTarget->RemoveSingleAuraFromStackByDispel(spellInfo->Id);
                }
                else
                    unitTarget->RemoveAurasDueToSpellByDispel(spellInfo->Id, j->second, m_caster);
            }
            m_caster->SendMessageToSet(&data, true);

            // On succes dispel
            // Devour Magic
            if (m_spellInfo->SpellFamilyName == SPELLFAMILY_WARLOCK && m_spellInfo->Category == 12)
            {
                uint32 heal_spell = 0;
                switch (m_spellInfo->Id)
                {
                case 19505:
                    heal_spell = 19658;
                    break;
                case 19731:
                    heal_spell = 19732;
                    break;
                case 19734:
                    heal_spell = 19733;
                    break;
                case 19736:
                    heal_spell = 19735;
                    break;
                case 27276:
                    heal_spell = 27278;
                    break;
                case 27277:
                    heal_spell = 27279;
                    break;
                default:
                    sLog.outDebug("Spell for Devour Magic %d not handled in Spell::EffectDispel", m_spellInfo->Id);
                    break;
                }
                if (heal_spell)
                    m_caster->CastSpell(m_caster, heal_spell, true);
            }
        }
        // Send fail log to client
        if (!fail_list.empty())
        {
            // Failed to dispell
            WorldPacket data(SMSG_DISPEL_FAILED, 8 + 8 + 4 + 4 * fail_list.size());
            data << uint64(m_caster->GetGUID());            // Caster GUID
            data << uint64(unitTarget->GetGUID());          // Victim GUID
            data << uint32(m_spellInfo->Id);                // dispel spell id
            for (std::list< uint32 >::iterator j = fail_list.begin(); j != fail_list.end(); ++j)
                data << uint32(*j);                         // Spell Id
            m_caster->SendMessageToSet(&data, true);
        }
    }
}

void Spell::EffectDualWield(SpellEffIndex /*effIndex*/)
{
    unitTarget->SetCanDualWield(true);
}

void Spell::EffectPull(SpellEffIndex /*effIndex*/)
{
    // @todo create a proper pull towards distract spell center for distract
    DEBUG_LOG("WORLD: Spell Effect DUMMY");
}

void Spell::EffectDistract(SpellEffIndex /*effIndex*/)
{
    // Check for possible target
    if (!unitTarget || unitTarget->IsInCombat())
        return;

    // target must be OK to do this
    if (unitTarget->HasUnitState(UNIT_STATE_CONFUSED | UNIT_STATE_STUNNED | UNIT_STATE_FLEEING))
        return;

    float angle = unitTarget->GetAngle(&m_targets.m_dstPos);
    unitTarget->SetFacingTo(angle);

    unitTarget->ClearUnitState(UNIT_STATE_MOVING);

    if (unitTarget->GetTypeId() == TYPEID_UNIT)
        unitTarget->GetMotionMaster()->MoveDistract(damage * IN_MILLISECONDS);
}

void Spell::EffectPickPocket(SpellEffIndex /*effIndex*/)
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    // victim must be creature and attackable
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT || m_caster->IsFriendlyTo(unitTarget))
        return;

    // victim have to be alive and humanoid or undead
    if (unitTarget->IsAlive() && (unitTarget->GetCreatureTypeMask() &CREATURE_TYPEMASK_HUMANOID_OR_UNDEAD) != 0)
        m_caster->ToPlayer()->SendLoot(unitTarget->GetGUID(), LOOT_PICKPOCKETING);
}

void Spell::EffectAddFarsight(SpellEffIndex effIndex)
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    float radius = GetSpellRadius(m_spellInfo, effIndex, false);
    int32 duration = GetSpellDuration(m_spellInfo);
    // Caster not in world, might be spell triggered from aura removal
    if (!m_caster->IsInWorld())
        return;

    DynamicObject* dynObj = new DynamicObject(true);
    if (!dynObj->CreateDynamicObject(sObjectMgr.GenerateLowGuid(HIGHGUID_DYNAMICOBJECT), m_caster, m_spellInfo->Id, 4, m_targets.m_dstPos, duration, radius))
    {
        delete dynObj;
        return;
    }
    dynObj->SetUInt32Value(OBJECT_FIELD_TYPE, 65);
    dynObj->SetUInt32Value(DYNAMICOBJECT_BYTES, 0x80000002);
    m_caster->AddDynObject(dynObj);

    dynObj->setActive(true);    //must before add to map to be put in world container
    dynObj->GetMap()->AddToMap(dynObj); //grid will also be loaded

    // Need to update visibility of object for client to accept farsight guid
    ((Player*)m_caster)->SetViewpoint(dynObj, true);
    //((Player*)m_caster)->UpdateVisibilityOf(dynObj);
}

void Spell::EffectTeleUnitsFaceCaster(SpellEffIndex effIndex)
{
    if (!unitTarget)
        return;

    if (unitTarget->IsInFlight())
        return;

    float dis = GetSpellRadius(m_spellInfo, effIndex, false);

    float fx, fy, fz;
    m_caster->GetClosePoint(fx, fy, fz, unitTarget->GetObjectSize(), dis);

    unitTarget->NearTeleportTo(fx, fy, fz, -m_caster->GetOrientation(), unitTarget == m_caster);
}

void Spell::EffectLearnSkill(SpellEffIndex effIndex)
{
    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    if (damage < 0)
        return;

    uint32 skillid =  m_spellInfo->EffectMiscValue[effIndex];
    uint16 skillval = unitTarget->ToPlayer()->GetPureSkillValue(skillid);
    unitTarget->ToPlayer()->SetSkill(skillid, skillval ? skillval : 1, damage * 75);
}

void Spell::EffectAddHonor(SpellEffIndex /*effIndex*/)
{
    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    // 2.4.3 honor-spells don't scale with level and won't be casted by an item
    // also we must use damage+1 (spelldescription says +25 honor but damage is only 24)
    unitTarget->ToPlayer()->RewardHonor(NULL, 1, damage + 1);
    DEBUG_LOG("SpellEffect::AddHonor (spell_id %u) rewards %u honor points (non scale) for player: %u", m_spellInfo->Id, damage, unitTarget->ToPlayer()->GetGUIDLow());
}

void Spell::EffectTradeSkill(SpellEffIndex /*effIndex*/)
{
    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;
    // uint32 skillid =  m_spellInfo->EffectMiscValue[i];
    // uint16 skillmax = unitTarget->ToPlayer()->(skillid);
    // unitTarget->ToPlayer()->SetSkill(skillid,skillval?skillval:1,skillmax+75);
}

void Spell::EffectEnchantItemPerm(SpellEffIndex effIndex)
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;
    if (!itemTarget)
        return;

    Player* p_caster = m_caster->ToPlayer();

    p_caster->UpdateCraftSkill(m_spellInfo->Id);

    uint32 enchant_id = m_spellInfo->EffectMiscValue[effIndex];
    if (!enchant_id)
        return;

    SpellItemEnchantmentEntry const* pEnchant = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
    if (!pEnchant)
        return;

    // item can be in trade slot and have owner diff. from caster
    Player* item_owner = itemTarget->GetOwner();
    if (!item_owner)
        return;

    if (item_owner != p_caster && p_caster->GetSession()->GetSecurity() > SEC_PLAYER && sWorld.getConfig(CONFIG_GM_LOG_TRADE))
    {
        sLog.outCommand(p_caster->GetSession()->GetAccountId(), "GM %s (Account: %u) enchanting(perm): %s (Entry: %d) for player: %s (Account: %u)",
                        p_caster->GetName(), p_caster->GetSession()->GetAccountId(),
                        itemTarget->GetProto()->Name1, itemTarget->GetEntry(),
                        item_owner->GetName(), item_owner->GetSession()->GetAccountId());
    }

    // remove old enchanting before applying new if equipped
    item_owner->ApplyEnchantment(itemTarget, PERM_ENCHANTMENT_SLOT, false);

    itemTarget->SetEnchantment(PERM_ENCHANTMENT_SLOT, enchant_id, 0, 0);

    // add new enchanting if equipped
    item_owner->ApplyEnchantment(itemTarget, PERM_ENCHANTMENT_SLOT, true);

    // update trade window for show enchantment for caster in trade window
    if (m_targets.m_targetMask & TARGET_FLAG_TRADE_ITEM)
        p_caster->GetSession()->SendUpdateTrade();
}

void Spell::EffectEnchantItemTmp(SpellEffIndex effIndex)
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* p_caster = m_caster->ToPlayer();

    if (!itemTarget)
        return;

    uint32 enchant_id = m_spellInfo->EffectMiscValue[effIndex];

    // Shaman Rockbiter Weapon
    if (effIndex == 0 && m_spellInfo->Effect[1] == SPELL_EFFECT_DUMMY)
    {
        int32 enchnting_damage = CalculateDamage(1, NULL);//+1;

        // enchanting id selected by calculated damage-per-sec stored in Effect[1] base value
        // with already applied percent bonus from Elemental Weapons talent
        // Note: damage calculated (correctly) with rounding int32(float(v)) but
        // RW enchantments applied damage int32(float(v)+0.5), this create  0..1 difference sometime
        switch (enchnting_damage)
        {
        // Rank 1
        case  2:
            enchant_id =   29;
            break;              //  0% [ 7% == 2, 14% == 2, 20% == 2]
        // Rank 2
        case  4:
            enchant_id =    6;
            break;              //  0% [ 7% == 4, 14% == 4]
        case  5:
            enchant_id = 3025;
            break;              // 20%
        // Rank 3
        case  6:
            enchant_id =    1;
            break;              //  0% [ 7% == 6, 14% == 6]
        case  7:
            enchant_id = 3027;
            break;              // 20%
        // Rank 4
        case  9:
            enchant_id = 3032;
            break;              //  0% [ 7% == 6]
        case 10:
            enchant_id =  503;
            break;              // 14%
        case 11:
            enchant_id = 3031;
            break;              // 20%
        // Rank 5
        case 15:
            enchant_id = 3035;
            break;              // 0%
        case 16:
            enchant_id = 1663;
            break;              // 7%
        case 17:
            enchant_id = 3033;
            break;              // 14%
        case 18:
            enchant_id = 3034;
            break;              // 20%
        // Rank 6
        case 28:
            enchant_id = 3038;
            break;              // 0%
        case 29:
            enchant_id =  683;
            break;              // 7%
        case 31:
            enchant_id = 3036;
            break;              // 14%
        case 33:
            enchant_id = 3037;
            break;              // 20%
        // Rank 7
        case 40:
            enchant_id = 3041;
            break;              // 0%
        case 42:
            enchant_id = 1664;
            break;              // 7%
        case 45:
            enchant_id = 3039;
            break;              // 14%
        case 48:
            enchant_id = 3040;
            break;              // 20%
        // Rank 8
        case 49:
            enchant_id = 3044;
            break;              // 0%
        case 52:
            enchant_id = 2632;
            break;              // 7%
        case 55:
            enchant_id = 3042;
            break;              // 14%
        case 58:
            enchant_id = 3043;
            break;              // 20%
        // Rank 9
        case 62:
            enchant_id = 2633;
            break;              // 0%
        case 66:
            enchant_id = 3018;
            break;              // 7%
        case 70:
            enchant_id = 3019;
            break;              // 14%
        case 74:
            enchant_id = 3020;
            break;              // 20%
        default:
            sLog.outError("Spell::EffectEnchantItemTmp: Damage %u not handled in S'RW", enchnting_damage);
            return;
        }
    }

    if (!enchant_id)
    {
        sLog.outError("Spell %u Effect %u (SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY) has 0 as enchanting id", m_spellInfo->Id, effIndex);
        return;
    }

    SpellItemEnchantmentEntry const* pEnchant = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
    if (!pEnchant)
    {
        sLog.outError("Spell %u Effect %u (SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY) has invalid enchanting id %u ", m_spellInfo->Id, effIndex, enchant_id);
        return;
    }

    // select enchantment duration
    uint32 duration;

    // rogue family enchantments exception by duration
    if (m_spellInfo->Id == 38615)
        duration = 1800;                                    // 30 mins
    // other rogue family enchantments always 1 hour (some have spell damage=0, but some have wrong data in EffBasePoints)
    else if (m_spellInfo->SpellFamilyName == SPELLFAMILY_ROGUE)
        duration = 3600;                                    // 1 hour
    // shaman family enchantments
    else if (m_spellInfo->SpellFamilyName == SPELLFAMILY_SHAMAN)
        duration = 1800;                                    // 30 mins
    // other cases with this SpellVisual already selected
    else if (m_spellInfo->SpellVisual == 215)
        duration = 1800;                                    // 30 mins
    // some fishing pole bonuses
    else if (m_spellInfo->SpellVisual == 563)
        duration = 600;                                     // 10 mins
    // shaman rockbiter enchantments
    else if (m_spellInfo->SpellVisual == 0)
        duration = 1800;                                    // 30 mins
    else if (m_spellInfo->Id == 29702)
        duration = 300;                                     // 5 mins
    else if (m_spellInfo->Id == 37360)
        duration = 300;                                     // 5 mins
    // default case
    else
        duration = 3600;                                    // 1 hour

    // item can be in trade slot and have owner diff. from caster
    Player* item_owner = itemTarget->GetOwner();
    if (!item_owner)
        return;

    if (item_owner != p_caster && p_caster->GetSession()->GetSecurity() > SEC_PLAYER && sWorld.getConfig(CONFIG_GM_LOG_TRADE))
    {
        sLog.outCommand(p_caster->GetSession()->GetAccountId(), "GM %s (Account: %u) enchanting(temp): %s (Entry: %d) for player: %s (Account: %u)",
                        p_caster->GetName(), p_caster->GetSession()->GetAccountId(),
                        itemTarget->GetProto()->Name1, itemTarget->GetEntry(),
                        item_owner->GetName(), item_owner->GetSession()->GetAccountId());
    }

    // remove old enchanting before applying new if equipped
    item_owner->ApplyEnchantment(itemTarget, TEMP_ENCHANTMENT_SLOT, false);

    itemTarget->SetEnchantment(TEMP_ENCHANTMENT_SLOT, enchant_id, duration * IN_MILLISECONDS, 0);

    // add new enchanting if equipped
    item_owner->ApplyEnchantment(itemTarget, TEMP_ENCHANTMENT_SLOT, true);

    // update trade window for show enchantment for caster in trade window
    if (m_targets.m_targetMask & TARGET_FLAG_TRADE_ITEM)
        p_caster->GetSession()->SendUpdateTrade();
}

void Spell::EffectTameCreature(SpellEffIndex /*effIndex*/)
{
    if (m_caster->GetPetGUID())
        return;

    if (!unitTarget)
        return;

    if (unitTarget->GetTypeId() == TYPEID_PLAYER)
        return;

    Creature* creatureTarget = unitTarget->ToCreature();

    if (creatureTarget->IsPet())
        return;

    if (m_caster->getClass() != CLASS_HUNTER)
        return;

    // cast finish successfully
    //SendChannelUpdate(0);
    finish();

    Pet* pet = m_caster->CreateTamedPetFrom(creatureTarget, m_spellInfo->Id);
    if (!pet)                                               // in very specific state like near world end/etc.
        return;

    // "kill" original creature
    creatureTarget->ForcedDespawn();

    // prepare visual effect for levelup
    pet->SetUInt32Value(UNIT_FIELD_LEVEL, creatureTarget->getLevel() - 1);

    // add to world
    pet->GetMap()->AddToMap(pet->ToCreature());

    // visual effect for levelup
    pet->SetUInt32Value(UNIT_FIELD_LEVEL, creatureTarget->getLevel());

    // caster have pet now
    m_caster->SetMinion(pet, true);

    if (m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        pet->SavePetToDB(PET_SAVE_AS_CURRENT);
        m_caster->ToPlayer()->PetSpellInitialize();
    }
}

void Spell::EffectSummonPet(SpellEffIndex effIndex)
{
    Player* owner = NULL;
    if (m_originalCaster)
    {
        if (m_originalCaster->GetTypeId() == TYPEID_PLAYER)
            owner = m_originalCaster->ToPlayer();
        else if (m_originalCaster->IsTotem())
            owner = m_originalCaster->GetCharmerOrOwnerPlayerOrPlayerItself();
    }

    uint32 petentry = m_spellInfo->EffectMiscValue[effIndex];

    if (!owner)
    {
        SummonPropertiesEntry const* properties = sSummonPropertiesStore.LookupEntry(67);
        if (properties)
            SummonGuardian(effIndex, petentry, properties);
        return;
    }

    Pet* OldSummon = owner->GetPet();

    // if pet requested type already exist
    if (OldSummon)
    {
        if (petentry == 0 || OldSummon->GetEntry() == petentry)
        {
            // pet in corpse state can't be summoned
            if (OldSummon->isDead())
                return;

            ASSERT(OldSummon->GetMap() == owner->GetMap());

            //OldSummon->GetMap()->RemoveFromMap(OldSummon->ToCreature(),false);

            float px, py, pz;
            owner->GetClosePoint(px, py, pz, OldSummon->GetObjectSize());

            OldSummon->NearTeleportTo(px, py, pz, OldSummon->GetOrientation());
            //OldSummon->Relocate(px, py, pz, OldSummon->GetOrientation());
            //OldSummon->SetMap(owner->GetMap());
            //owner->GetMap()->AddToMap(OldSummon->ToCreature());

            if (owner->GetTypeId() == TYPEID_PLAYER && OldSummon->isControlled())
                owner->ToPlayer()->PetSpellInitialize();

            return;
        }

        if (owner->GetTypeId() == TYPEID_PLAYER)
            owner->ToPlayer()->RemovePet(OldSummon, (OldSummon->getPetType() == HUNTER_PET ? PET_SAVE_AS_DELETED : PET_SAVE_NOT_IN_SLOT), false);
        else
            return;
    }

    float x, y, z;
    owner->GetClosePoint(x, y, z, owner->GetObjectSize());
    Pet* pet = owner->SummonPet(petentry, x, y, z, owner->GetOrientation(), SUMMON_PET, 0);

    if (!pet)
        return;

    if (m_caster->GetTypeId() == TYPEID_UNIT)
    {
        if (m_caster->IsTotem())
            pet->SetReactState(REACT_AGGRESSIVE);
        else
            pet->SetReactState(REACT_DEFENSIVE);
    }

    pet->SetUInt32Value(UNIT_CREATED_BY_SPELL, m_spellInfo->Id);

    // generate new name for summon pet
    std::string new_name = sObjectMgr.GeneratePetName(petentry);

    if (!new_name.empty())
        pet->SetName(new_name);
}

void Spell::SummonClassPet(SpellEffIndex effIndex)
{
    uint32 pet_entry = m_spellInfo->EffectMiscValue[effIndex];
    if (!pet_entry)
        return;

    Player* caster = NULL;
    if (m_originalCaster)
    {
        if (m_originalCaster->GetTypeId() == TYPEID_PLAYER)
            caster = m_originalCaster->ToPlayer();
        else if (m_originalCaster->IsTotem())
            caster = m_originalCaster->GetCharmerOrOwnerPlayerOrPlayerItself();
    }

    uint32 petentry = m_spellInfo->EffectMiscValue[effIndex];

    if (!caster)
    {
        SummonPropertiesEntry const* properties = sSummonPropertiesStore.LookupEntry(67);
        if (properties)
            SummonGuardian(effIndex, petentry, properties);
        return;
    }

    // set timer for unsummon
    int32 duration = GetSpellDuration(m_spellInfo);

    Pet* OldSummon = caster->GetPet();

    // if pet requested type already exist
    if (OldSummon)
    {
        if (petentry == 0 || OldSummon->GetEntry() == petentry)
        {
            // pet in corpse state can't be summoned
            if (OldSummon->isDead())
                return;

            ASSERT(OldSummon->GetMap() == caster->GetMap());

            float px, py, pz;
            caster->GetClosePoint(px, py, pz, OldSummon->GetObjectSize());

            OldSummon->NearTeleportTo(px, py, pz, OldSummon->GetOrientation());

            if (caster->GetTypeId() == TYPEID_PLAYER && OldSummon->isControlled())
                caster->ToPlayer()->PetSpellInitialize();

            return;
        }

        if (caster->GetTypeId() == TYPEID_PLAYER)
            caster->ToPlayer()->RemovePet(OldSummon, PET_SAVE_AS_DELETED, false);
        else
            return;
    }

    /*
    // in another case summon new
    uint32 level = caster->getLevel();

    // level of pet summoned using engineering item based at engineering skill level
    if (m_CastItem)
    {
        ItemTemplate const *proto = m_CastItem->GetProto();
        if (proto && proto->RequiredSkill == SKILL_ENGINEERING)
        {
            uint16 skill202 = caster->GetSkillValue(SKILL_ENGINEERING);
            if (skill202)
            {
                level = skill202 / 5;
            }
        }
    }
    */

    // select center of summon position
    WorldLocation center = m_targets.m_dstPos;

    float radius = GetSpellRadius(m_spellInfo, effIndex, false);

    int32 amount = damage > 0 ? damage : 1;

    for (int32 count = 0; count < amount; ++count)
    {
        float px, py, pz;
        // If dest location if present
        if (m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
        {
            // Summon 1 unit in dest location
            if (count == 0)
                m_targets.m_dstPos.GetPosition(px, py, pz);
            // Summon in random point all other units if location present
            else
                m_caster->GetRandomPoint(center, radius, px, py, pz);
        }
        // Summon if dest location not present near caster
        else
            m_caster->GetClosePoint(px, py, pz, m_caster->GetObjectSize());

        Pet* pet = caster->SummonPet(m_spellInfo->EffectMiscValue[effIndex], px, py, pz, m_caster->GetOrientation(), CLASS_PET, duration);
        if (!pet)
            return;

        pet->SetUInt32Value(UNIT_FIELD_PET_NAME_TIMESTAMP, 0);
        pet->SetUInt32Value(UNIT_CREATED_BY_SPELL, m_spellInfo->Id);
        pet->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);
    }
}

void Spell::EffectLearnPetSpell(SpellEffIndex effIndex)
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* _player = m_caster->ToPlayer();

    Pet* pet = _player->GetPet();
    if (!pet)
        return;
    if (!pet->IsAlive())
        return;

    SpellEntry const* learn_spellproto = sSpellStore.LookupEntry(m_spellInfo->EffectTriggerSpell[effIndex]);
    if (!learn_spellproto)
        return;

    pet->SetTP(pet->m_TrainingPoints - pet->GetTPForSpell(learn_spellproto->Id));
    pet->learnSpell(learn_spellproto->Id);

    pet->SavePetToDB(PET_SAVE_AS_CURRENT);
    _player->PetSpellInitialize();
}

void Spell::EffectTaunt(SpellEffIndex /*effIndex*/)
{
    // this effect use before aura Taunt apply for prevent taunt already attacking target
    // for spell as marked "non effective at already attacking target"
    if (!unitTarget || !unitTarget->CanHaveThreatList()
        || unitTarget->GetVictim() == m_caster)
    {
        SendCastResult(SPELL_FAILED_DONT_REPORT);
        return;
    }

    // Also use this effect to set the taunter's threat to the taunted creature's highest value
    if (unitTarget->getThreatManager().getCurrentVictim())
    {
        float myThreat = unitTarget->getThreatManager().getThreat(m_caster);
        float itsThreat = unitTarget->getThreatManager().getCurrentVictim()->getThreat();
        if (itsThreat > myThreat)
            unitTarget->getThreatManager().addThreat(m_caster, itsThreat - myThreat);
    }

    //Set aggro victim to caster
    if (!unitTarget->getThreatManager().getOnlineContainer().empty())
        if (HostileReference* forcedVictim = unitTarget->getThreatManager().getOnlineContainer().getReferenceByTarget(m_caster))
            unitTarget->getThreatManager().setCurrentVictim(forcedVictim);

    if (unitTarget->ToCreature()->IsAIEnabled)
        unitTarget->ToCreature()->AI()->AttackStart(m_caster);
}

void Spell::EffectWeaponDmg(SpellEffIndex /*effIndex*/)
{
}

void Spell::SpellDamageWeaponDmg(SpellEffIndex effIndex)
{
    if (!unitTarget)
        return;
    if (!unitTarget->IsAlive())
        return;

    // multiple weapon dmg effect workaround
    // execute only the last weapon damage
    // and handle all effects at once
    for (uint32 j = 0; j < MAX_SPELL_EFFECTS; j++)
    {
        switch (m_spellInfo->Effect[j])
        {
        case SPELL_EFFECT_WEAPON_DAMAGE:
        case SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL:
        case SPELL_EFFECT_NORMALIZED_WEAPON_DMG:
        case SPELL_EFFECT_WEAPON_PERCENT_DAMAGE:
            if (j < effIndex)                                  // we must calculate only at last weapon effect
                return;
            break;
        }
    }

    // some spell specific modifiers
    //float weaponDamagePercentMod = 1.0f;                    // applied to weapon damage (and to fixed effect damage bonus if customBonusDamagePercentMod not set
    float totalDamagePercentMod  = 1.0f;                    // applied to final bonus+weapon damage
    int32 fixed_bonus = 0;
    int32 spell_bonus = 0;                                  // bonus specific for spell

    switch (m_spellInfo->SpellFamilyName)
    {
    case SPELLFAMILY_WARRIOR:
        {
            // Devastate bonus and sunder armor refresh
            if (m_spellInfo->SpellVisual == 671 && m_spellInfo->SpellIconID == 1508)
            {
                uint32 stack = 0;

                Unit::AuraList const& list = unitTarget->GetAurasByType(SPELL_AURA_MOD_RESISTANCE);
                for (Unit::AuraList::const_iterator itr = list.begin(); itr != list.end(); ++itr)
                {
                    SpellEntry const* proto = (*itr)->GetSpellProto();
                    if (proto->SpellFamilyName == SPELLFAMILY_WARRIOR
                        && proto->SpellFamilyFlags == SPELLFAMILYFLAG_WARRIOR_SUNDERARMOR)
                    {
                        int32 duration = GetSpellDuration(proto);
                        (*itr)->SetAuraDuration(duration);
                        (*itr)->UpdateAuraDuration();
                        stack = (*itr)->GetStackAmount();
                        break;
                    }
                }

                for (int j = 0; j < MAX_SPELL_EFFECTS; j++)
                {
                    if (m_spellInfo->Effect[j] == SPELL_EFFECT_NORMALIZED_WEAPON_DMG)
                    {
                        fixed_bonus += (stack - 1) * CalculateDamage(j, unitTarget);
                        break;
                    }
                }

                if (stack < 5)
                {
                    if (Player* player = m_caster->ToPlayer())
                    {
                        // get highest rank of the Sunder Armor spell
                        const PlayerSpellMap& sp_list = player->GetSpellMap();
                        for (PlayerSpellMap::const_iterator itr = sp_list.begin(); itr != sp_list.end(); ++itr)
                        {
                            // only highest rank is shown in spell book, so simply check if shown in spell book
                            if (!itr->second.active || itr->second.disabled || itr->second.state == PLAYERSPELL_REMOVED)
                                continue;

                            SpellEntry const* spellInfo = sSpellStore.LookupEntry(itr->first);
                            if (!spellInfo)
                                continue;

                            if (spellInfo->SpellFamilyFlags == SPELLFAMILYFLAG_WARRIOR_SUNDERARMOR
                                && spellInfo->Id != m_spellInfo->Id
                                && spellInfo->SpellFamilyName == SPELLFAMILY_WARRIOR)
                            {
                                m_caster->CastSpell(unitTarget, spellInfo, true);
                                break;
                            }
                        }
                    }
                }
            }
            break;
        }
    case SPELLFAMILY_ROGUE:
        {
            // Hemorrhage
            if (m_spellInfo->SpellFamilyFlags & 0x2000000)
            {
                if (m_caster->GetTypeId() == TYPEID_PLAYER)
                    m_caster->ToPlayer()->AddComboPoints(unitTarget, 1);
            }
            // Mutilate (for each hand)
            else if (m_spellInfo->SpellFamilyFlags & 0x600000000LL)
            {
                Unit::AuraMap const& auras = unitTarget->GetAuras();
                for (Unit::AuraMap::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
                {
                    if (itr->second->GetSpellProto()->Dispel == DISPEL_POISON)
                    {
                        totalDamagePercentMod *= 1.5f;          // 150% if poisoned
                        break;
                    }
                }
            }
            break;
        }
    case SPELLFAMILY_PALADIN:
        {
            // Seal of Command - receive benefit from Spell Damage and Healing
            if (m_spellInfo->SpellFamilyFlags & 0x00000002000000LL)
            {
                spell_bonus += int32(0.20f * m_caster->SpellBaseDamageBonus(GetSpellSchoolMask(m_spellInfo)));
                spell_bonus += int32(0.29f * m_caster->SpellBaseDamageBonusForVictim(GetSpellSchoolMask(m_spellInfo), unitTarget));
            }
            break;
        }
    case SPELLFAMILY_SHAMAN:
        {
            // Skyshatter Harness item set bonus
            // Stormstrike
            if (m_spellInfo->SpellFamilyFlags & 0x001000000000LL)
            {
                Unit::AuraList const& m_OverrideClassScript = m_caster->GetAurasByType(SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
                for (Unit::AuraList::const_iterator i = m_OverrideClassScript.begin(); i != m_OverrideClassScript.end(); ++i)
                {
                    // Stormstrike AP Buff
                    if ((*i)->GetModifier()->m_miscvalue == 5634)
                    {
                        m_caster->CastSpell(m_caster, 38430, true, NULL, *i);
                        break;
                    }
                }
            }
            break;
        }
    case SPELLFAMILY_DRUID:
        {
            // Mangle (Cat): CP
            if (m_spellInfo->SpellFamilyFlags == 0x0000040000000000LL)
            {
                if (m_caster->GetTypeId() == TYPEID_PLAYER)
                    m_caster->ToPlayer()->AddComboPoints(unitTarget, 1);
            }
            break;
        }
    }

    bool normalized = false;
    float weaponDamagePercentMod = 1.0;
    for (int j = 0; j < MAX_SPELL_EFFECTS; ++j)
    {
        switch (m_spellInfo->Effect[j])
        {
        case SPELL_EFFECT_WEAPON_DAMAGE:
        case SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL:
            fixed_bonus += CalculateDamage(j, unitTarget);
            break;
        case SPELL_EFFECT_NORMALIZED_WEAPON_DMG:
            fixed_bonus += CalculateDamage(j, unitTarget);
            normalized = true;
            break;
        case SPELL_EFFECT_WEAPON_PERCENT_DAMAGE:
            weaponDamagePercentMod *= float(CalculateDamage(j, unitTarget)) / 100.0f;
            break;
        default:
            break;                                      // not weapon damage effect, just skip
        }
    }

    // apply to non-weapon bonus weapon total pct effect, weapon total flat effect included in weapon damage
    if (fixed_bonus || spell_bonus)
    {
        UnitMods unitMod;
        switch (m_attackType)
        {
        default:
        case BASE_ATTACK:
            unitMod = UNIT_MOD_DAMAGE_MAINHAND;
            break;
        case OFF_ATTACK:
            unitMod = UNIT_MOD_DAMAGE_OFFHAND;
            break;
        case RANGED_ATTACK:
            unitMod = UNIT_MOD_DAMAGE_RANGED;
            break;
        }

        float weapon_total_pct = 1.0f;
        if (m_spellInfo->SchoolMask & SPELL_SCHOOL_MASK_NORMAL)
            weapon_total_pct = m_caster->GetModifierValue(unitMod, TOTAL_PCT);

        if (fixed_bonus)
            fixed_bonus = int32(fixed_bonus * weapon_total_pct);
        if (spell_bonus)
            spell_bonus = int32(spell_bonus * weapon_total_pct);
    }

    int32 weaponDamage = m_caster->CalculateDamage(m_attackType, normalized, true);

    // Sequence is important
    for (int j = 0; j < MAX_SPELL_EFFECTS; ++j)
    {
        // We assume that a spell have at most one fixed_bonus
        // and at most one weaponDamagePercentMod
        switch (m_spellInfo->Effect[j])
        {
        case SPELL_EFFECT_WEAPON_DAMAGE:
        case SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL:
        case SPELL_EFFECT_NORMALIZED_WEAPON_DMG:
            weaponDamage += fixed_bonus;
            break;
        case SPELL_EFFECT_WEAPON_PERCENT_DAMAGE:
            weaponDamage = int32(weaponDamage * weaponDamagePercentMod);
        default:
            break;                                      // not weapon damage effect, just skip
        }
    }

    // only for Seal of Command
    if (spell_bonus)
        weaponDamage += spell_bonus;

    // only for Mutilate
    if (totalDamagePercentMod != 1.0f)
        weaponDamage = int32(weaponDamage * totalDamagePercentMod);

    // prevent negative damage
    uint32 eff_damage = uint32(weaponDamage > 0 ? weaponDamage : 0);

    // Add melee damage bonuses (also check for negative)
    m_caster->MeleeDamageBonus(unitTarget, &eff_damage, m_attackType, m_spellInfo);
    m_damage += eff_damage;

    // take ammo
    if (m_attackType == RANGED_ATTACK && m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        Item* pItem = m_caster->ToPlayer()->GetWeaponForAttack(RANGED_ATTACK);

        // wands don't have ammo
        if (!pItem  || pItem->IsBroken() || pItem->GetProto()->SubClass == ITEM_SUBCLASS_WEAPON_WAND)
            return;

        if (pItem->GetProto()->InventoryType == INVTYPE_THROWN)
        {
            if (pItem->GetMaxStackCount() == 1)
            {
                // decrease durability for non-stackable throw weapon
                m_caster->ToPlayer()->DurabilityPointLossForEquipSlot(EQUIPMENT_SLOT_RANGED);
            }
            else
            {
                // decrease items amount for stackable throw weapon
                uint32 count = 1;
                m_caster->ToPlayer()->DestroyItemCount(pItem, count, true);
            }
        }
        else if (uint32 ammo = m_caster->ToPlayer()->GetUInt32Value(PLAYER_AMMO_ID))
            m_caster->ToPlayer()->DestroyItemCount(ammo, 1, true);
    }
}

void Spell::EffectThreat(SpellEffIndex /*effIndex*/)
{
    if (!unitTarget || !unitTarget->IsAlive() || !m_caster->IsAlive())
        return;

    if (!unitTarget->CanHaveThreatList())
        return;

    unitTarget->AddThreat(m_caster, float(damage));
}

void Spell::EffectHealMaxHealth(SpellEffIndex /*effIndex*/)
{
    if (!unitTarget || !unitTarget->IsAlive())
        return;

    if (unitTarget->GetMaxNegativeAuraModifier(SPELL_AURA_MOD_HEALING_PCT) <= -100)
        return;

    int32 addhealth = 0;

    // damage == 0 - heal for caster max health
    if (damage == 0)
        addhealth = m_caster->GetMaxHealth();
    else
        addhealth = unitTarget->GetMaxHealth() - unitTarget->GetHealth();

    unitTarget->SetHealth(addhealth);

    if (m_originalCaster)
        m_originalCaster->SendHealSpellLog(unitTarget, m_spellInfo->Id, addhealth, false);
}

void Spell::EffectInterruptCast(SpellEffIndex effIndex)
{
    if (!unitTarget)
        return;
    if (!unitTarget->IsAlive())
        return;

    // @todo not all spells that used this effect apply cooldown at school spells
    // also exist case: apply cooldown to interrupted cast only and to all spells
    for (uint32 i = CURRENT_FIRST_NON_MELEE_SPELL; i < CURRENT_MAX_SPELL; ++i)
    {
        if (Spell* spell = unitTarget->GetCurrentSpell(CurrentSpellTypes(i)))
        {
            SpellEntry const* curSpellInfo = spell->m_spellInfo;
            // check if we can interrupt spell
            if ((spell->getState() == SPELL_STATE_CASTING
                 || (spell->getState() == SPELL_STATE_PREPARING && spell->GetCastTime() > 0.0f))
                && curSpellInfo->PreventionType == SPELL_PREVENTION_TYPE_SILENCE
                && ((i == CURRENT_GENERIC_SPELL && curSpellInfo->InterruptFlags & SPELL_INTERRUPT_FLAG_INTERRUPT)
                    || (i == CURRENT_CHANNELED_SPELL && curSpellInfo->ChannelInterruptFlags & CHANNEL_FLAG_MOVEMENT)))
            {
                if (m_originalCaster)
                {
                    int32 duration = m_originalCaster->CalculateSpellDuration(m_spellInfo, effIndex, unitTarget);
                    unitTarget->ProhibitSpellSchool(GetSpellSchoolMask(curSpellInfo), duration/*GetSpellDuration(m_spellInfo)*/);
                }
                unitTarget->InterruptSpell(CurrentSpellTypes(i), false);
            }
        }
    }
}

void Spell::EffectSummonObjectWild(SpellEffIndex effIndex)
{
    uint32 gameobject_id = m_spellInfo->EffectMiscValue[effIndex];

    GameObject* pGameObj = new GameObject;

    WorldObject* target = focusObject;
    if (!target)
        target = m_caster;

    float x, y, z;
    if (m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
        m_targets.m_dstPos.GetPosition(x, y, z);
    else
        m_caster->GetClosePoint(x, y, z, DEFAULT_WORLD_OBJECT_SIZE);

    Map* map = target->GetMap();

    if (!pGameObj->Create(sObjectMgr.GenerateLowGuid(HIGHGUID_GAMEOBJECT), gameobject_id, map,
                          x, y, z, target->GetOrientation(), 0.0f, 0.0f, 0.0f, 0.0f, 100, GO_STATE_READY))
    {
        delete pGameObj;
        return;
    }

    int32 duration = GetSpellDuration(m_spellInfo);

    pGameObj->SetRespawnTime(duration > 0 ? duration / IN_MILLISECONDS : 0);
    pGameObj->SetSpellId(m_spellInfo->Id);

    // Wild object not have owner and check clickable by players
    map->AddToMap(pGameObj);

    if (pGameObj->GetGoType() == GAMEOBJECT_TYPE_FLAGDROP && m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        switch (pGameObj->GetMapId())
        {
        case 489:                                       //WS
            {
                Player* pl = m_caster->ToPlayer();
                Battleground* bg = m_caster->ToPlayer()->GetBattleground();
                if (bg && bg->GetTypeID() == BATTLEGROUND_WS && bg->GetStatus() == STATUS_IN_PROGRESS)
                {
                    uint32 team = ALLIANCE;

                    if (pl->GetTeam() == team)
                        team = HORDE;

                    ((BattlegroundWS*)bg)->SetDroppedFlagGUID(pGameObj->GetGUID(), team);
                }
                break;
            }
        case 566:                                       //EY
            {
                Battleground* bg = m_caster->ToPlayer()->GetBattleground();
                if (bg && bg->GetTypeID() == BATTLEGROUND_EY && bg->GetStatus() == STATUS_IN_PROGRESS)
                    ((BattlegroundEY*)bg)->SetDroppedFlagGUID(pGameObj->GetGUID());
                break;
            }
        }
    }

    if (uint32 linkedEntry = pGameObj->GetGOInfo()->GetLinkedGameObjectEntry())
    {
        GameObject* linkedGO = new GameObject;
        if (linkedGO->Create(sObjectMgr.GenerateLowGuid(HIGHGUID_GAMEOBJECT), linkedEntry, map,
                             x, y, z, target->GetOrientation(), 0.0f, 0.0f, 0.0f, 0.0f, 100, GO_STATE_READY))
        {
            linkedGO->SetRespawnTime(duration > 0 ? duration / IN_MILLISECONDS : 0);
            linkedGO->SetSpellId(m_spellInfo->Id);

            // Wild object not have owner and check clickable by players
            map->AddToMap(linkedGO);
        }
        else
        {
            delete linkedGO;
            linkedGO = NULL;
            return;
        }
    }
}

void Spell::EffectScriptEffect(SpellEffIndex effIndex)
{
    // @todo we must implement hunter pet summon at login there (spell 6962)

    switch (m_spellInfo->SpellFamilyName)
    {
    case SPELLFAMILY_GENERIC:
        {
            switch (m_spellInfo->Id)
            {
            // Pet Summoned
            case 6962:
            {
                Player* player = GetCaster()->ToPlayer();
                if (player->GetLastPetNumber())
                {
                    PetType newPetType = (player->getClass() == CLASS_HUNTER) ? HUNTER_PET : SUMMON_PET;
                    Pet* newPet = new Pet(player, newPetType);
                    if (newPet->LoadPetFromDB(player, 0, player->GetLastPetNumber(), true))
                    {
                        // revive the pet if it is dead
                        if (newPet->getDeathState() == DEAD)
                            newPet->setDeathState(ALIVE);

                        newPet->SetFullHealth();
                        newPet->SetPower(newPet->getPowerType(), newPet->GetMaxPower(newPet->getPowerType()));

                        // Exact numbers are unknown, but we should increase happiness state
                        newPet->SetPower(POWER_HAPPINESS, HAPPINESS_LEVEL_SIZE * 2);

                        switch (newPet->GetEntry())
                        {
                        case 11859:
                        case 89:
                            newPet->SetEntry(416);
                            break;
                        default:
                            break;
                        }
                    }
                    else
                        delete newPet;
                }
            }
            // Bending Shinbone
            case 8856:
                {
                    if (!itemTarget && m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    uint32 spell_id = 0;
                    switch (urand(1, 5))
                    {
                    case 1:
                        spell_id = 8854;
                        break;
                    default:
                        spell_id = 8855;
                        break;
                    }

                    m_caster->CastSpell(m_caster, spell_id, true, NULL);
                    return;
                }
            // Piccolo of the Flaming Fire
            case 17512:
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;
                    unitTarget->HandleEmoteCommand(EMOTE_STATE_DANCE);
                    return;
                }
            // Brittle Armor - need remove one 24575 Brittle Armor aura
            case 24590:
                unitTarget->RemoveSingleAuraFromStack(24575, 0);
                unitTarget->RemoveSingleAuraFromStack(24575, 1);
                return;
            // Pirate Costume
            case 24717:
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    // Pirate Costume (male or female)
                    m_caster->CastSpell(unitTarget, unitTarget->getGender() == GENDER_MALE ? 24708 : 24709, true);
                    return;
                }
            // Ninja Costume
            case 24718:
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    // Ninja Costume (male or female)
                    m_caster->CastSpell(unitTarget, unitTarget->getGender() == GENDER_MALE ? 24711 : 24710, true);
                    return;
                }
            // Leper Gnome Costume
            case 24719:
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    // Leper Gnome Costume (male or female)
                    m_caster->CastSpell(unitTarget, unitTarget->getGender() == GENDER_MALE ? 24712 : 24713, true);
                    return;
                }
            // Random Costume
            case 24720:
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    uint32 spellId = 0;

                    switch (urand(0, 6))
                    {
                    case 0:
                        spellId = unitTarget->getGender() == GENDER_MALE ? 24708 : 24709;
                        break;
                    case 1:
                        spellId = unitTarget->getGender() == GENDER_MALE ? 24711 : 24710;
                        break;
                    case 2:
                        spellId = unitTarget->getGender() == GENDER_MALE ? 24712 : 24713;
                        break;
                    case 3:
                        spellId = 24723;
                        break;
                    case 4:
                        spellId = 24732;
                        break;
                    case 5:
                        spellId = unitTarget->getGender() == GENDER_MALE ? 24735 : 24736;
                        break;
                    case 6:
                        spellId = 24740;
                        break;
                    }

                    m_caster->CastSpell(unitTarget, spellId, true);
                    return;
                }
            // Ghost Costume
            case 24737:
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    // Ghost Costume (male or female)
                    m_caster->CastSpell(unitTarget, unitTarget->getGender() == GENDER_MALE ? 24735 : 24736, true);
                    return;
                }
            // PX-238 Winter Wondervolt TRAP
            case 26275:
                {
                    uint32 spells[4] = { 26272, 26157, 26273, 26274 };

                    // check presence
                    for (uint8 j = 0; j < 4; ++j)
                        if (unitTarget->HasAura(spells[j], 0))
                            return;

                    // select spell
                    uint32 iTmpSpellId = spells[urand(0, 3)];

                    // cast
                    unitTarget->CastSpell(unitTarget, iTmpSpellId, true);
                    return;
                }
            // Mercurial Shield - need remove one 26464 Mercurial Shield aura
            case 26465:
                unitTarget->RemoveSingleAuraFromStack(26464, 0);
                return;
            // Orb teleport spells
            case 25140:
            case 25143:
            case 25650:
            case 25652:
            case 29128:
            case 29129:
            case 35376:
            case 35727:
                {
                    if (!unitTarget)
                        return;

                    uint32 spellid;
                    switch (m_spellInfo->Id)
                    {
                    case 25140:
                        spellid =  32568;
                        break;
                    case 25143:
                        spellid =  32572;
                        break;
                    case 25650:
                        spellid =  30140;
                        break;
                    case 25652:
                        spellid =  30141;
                        break;
                    case 29128:
                        spellid =  32571;
                        break;
                    case 29129:
                        spellid =  32569;
                        break;
                    case 35376:
                        spellid =  25649;
                        break;
                    case 35727:
                        spellid =  35730;
                        break;
                    default:
                        return;
                    }

                    unitTarget->CastSpell(unitTarget, spellid, false);
                    return;
                }
            // Shadow Flame (All script effects, not just end ones to prevent player from dodging the last triggered spell)
            case 22539:
            case 22972:
            case 22975:
            case 22976:
            case 22977:
            case 22978:
            case 22979:
            case 22980:
            case 22981:
            case 22982:
            case 22983:
            case 22984:
            case 22985:
                {
                    if (!unitTarget || !unitTarget->IsAlive())
                        return;

                    // Onyxia Scale Cloak
                    if (unitTarget->GetDummyAura(22683))
                        return;

                    // Shadow Flame
                    m_caster->CastSpell(unitTarget, 22682, true);
                    return;
                }
            // Mistletoe
            case 26218:
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    switch (urand(0, 2))
                    {
                    case 0:
                        m_caster->CastSpell(unitTarget, 26207, true);
                        break;
                    case 1:
                        m_caster->CastSpell(unitTarget, 26206, true);
                        break;
                    case 2:
                        m_caster->CastSpell(unitTarget, 45036, true);
                        break;
                    }

                    return;
                }
            // Summon Black Qiraji Battle Tank
            case 26656:
                {
                    if (!unitTarget)
                        return;

                    // Two separate mounts depending on area id (allows use both in and out of specific instance)
                    if (unitTarget->GetAreaId() == 3428)
                        unitTarget->CastSpell(unitTarget, 25863, false);
                    else
                        unitTarget->CastSpell(unitTarget, 26655, false);

                    return;
                }
            case 26678:                                 // Heart Candy
                {
                    uint32 item = 0;
                    switch (urand(0, 7))
                    {
                    case 0:
                        item = 21816;
                        break;
                    case 1:
                        item = 21817;
                        break;
                    case 2:
                        item = 21818;
                        break;
                    case 3:
                        item = 21819;
                        break;
                    case 4:
                        item = 21820;
                        break;
                    case 5:
                        item = 21821;
                        break;
                    case 6:
                        item = 21822;
                        break;
                    case 7:
                        item = 21823;
                        break;
                    }
                    if (item)
                        DoCreateItem(effIndex, item);
                    break;
                }
            // Mirren's Drinking Hat
            case 29830:
                {
                    uint32 item = 0;
                    switch (urand(1, 6))
                    {
                    case 1:
                    case 2:
                    case 3:
                        item = 23584;
                        break;            // Loch Modan Lager
                    case 4:
                    case 5:
                        item = 23585;
                        break;            // Stouthammer Lite
                    case 6:
                        item = 23586;
                        break;            // Aerie Peak Pale Ale
                    }
                    if (item)
                        DoCreateItem(effIndex, item);
                    break;
                }
            // Improved Sprint
            case 30918:
                {
                    // Removes snares and roots.
                    uint32 mechanic_mask = (1 << MECHANIC_ROOT) | (1 << MECHANIC_SNARE);
                    Unit::AuraMap& Auras = unitTarget->GetAuras();
                    for (Unit::AuraMap::iterator iter = Auras.begin(), next; iter != Auras.end(); iter = next)
                    {
                        next = iter;
                        ++next;
                        Aura* aur = iter->second;
                        if (!aur->IsPositive())             //only remove negative spells
                        {
                            // check for mechanic mask
                            if (GetSpellMechanicMask(aur->GetSpellProto(), aur->GetEffIndex()) & mechanic_mask)
                            {
                                unitTarget->RemoveAurasDueToSpell(aur->GetId());
                                if (Auras.empty())
                                    break;
                                else
                                    next = Auras.begin();
                            }
                        }
                    }
                    break;
                }
            // Plant Warmaul Ogre Banner
            case 32307:
                {
                    Player* p_caster = dynamic_cast<Player*>(m_caster);
                    if (!p_caster)
                        break;
                    p_caster->RewardPlayerAndGroupAtEvent(18388, unitTarget);
                    Creature* cTarget = dynamic_cast<Creature*>(unitTarget);
                    if (!cTarget)
                        break;
                    cTarget->setDeathState(CORPSE);
                    cTarget->RemoveCorpse();
                    break;
                }
            // Gruul's shatter
            case 33654:
                {
                    if (!unitTarget)
                        return;

                    //Remove Stoned
                    if (unitTarget->HasAura(33652, 0))
                        unitTarget->RemoveAurasDueToSpell(33652);

                    // Only player cast this
                    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    unitTarget->CastSpell(unitTarget, 33671, true, 0, 0, m_caster->GetGUID());
                    break;
                }
            /*// Needle Spine
            case 39835:
            {
                if (!unitTarget)
                    return;

                unitTarget->CastSpell(unitTarget, 39968, true);
                break;
            }*/
            // Tidal Surge
            case 37473:                                 // Detect Whispers (related to quest 10607 - Whispers of the Raven God_Whispers of the Raven God)
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, damage, true);
                    break;
                }
            case 38358:
                if (unitTarget)
                    m_caster->CastSpell(unitTarget, 38353, true);
                return;
            /*// Flame Crash
            case 41126:
            {
                if (!unitTarget)
                    return;

                unitTarget->CastSpell(unitTarget, 41131, true);
                break;
            }*/
            // Draw Soul
            case 40904:
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(m_caster, 40903, true);
                    break;
                }
            //this is not working correctly, handled by script now
            /*case 39395: //win for chessevent
                {
                    ScriptInfo* si = new ScriptInfo;
                    si->command = SCRIPT_COMMAND_DESPAWN_SELF;
                    si->DespawnSelf.DespawnDelay = urand(10, 20) * 1000;
                    unitTarget->GetMap()->ScriptCommandStart((*si), 0, unitTarget, unitTarget);
                    unitTarget->RemoveAurasDueToSpell(32226); //attack timer -- stop attacking;
                    break;
                }*/
            case 48025:                                     // Headless Horseman's Mount
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    // Prevent stacking of mounts and client crashes upon dismounting
                    unitTarget->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);

                    // Triggered spell id dependent on riding skill and zone
                    bool canFly = true;
                    uint32 v_map = GetVirtualMapForMapAndZone(unitTarget->GetMapId(), unitTarget->GetZoneId());
                    if (v_map != 530)
                        canFly = false;

                    switch (unitTarget->ToPlayer()->GetBaseSkillValue(SKILL_RIDING))
                    {
                    case 75:
                        unitTarget->CastSpell(unitTarget, 51621, true);
                        break;;
                    case 150:
                        unitTarget->CastSpell(unitTarget, 48024, true);
                        break;
                    case 225:
                        {
                            if (canFly)
                                unitTarget->CastSpell(unitTarget, 51617, true);
                            else
                                unitTarget->CastSpell(unitTarget, 48024, true);
                            break;
                        }
                    case 300:
                        {
                            if (canFly)
                                unitTarget->CastSpell(unitTarget, 48023, true);
                            else
                                unitTarget->CastSpell(unitTarget, 48024, true);
                            break;
                        }
                    }
                    return;
                }
            case 47977:                                     // Magic Broom
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    // Prevent stacking of mounts and client crashes upon dismounting
                    unitTarget->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);

                    // Triggered spell id dependent on riding skill and zone
                    bool canFly = true;
                    uint32 v_map = GetVirtualMapForMapAndZone(unitTarget->GetMapId(), unitTarget->GetZoneId());
                    if (v_map != 530)
                        canFly = false;

                    switch (unitTarget->ToPlayer()->GetBaseSkillValue(SKILL_RIDING))
                    {
                    case 75:
                        unitTarget->CastSpell(unitTarget, 42680, true);
                        break;
                    case 150:
                        unitTarget->CastSpell(unitTarget, 42683, true);
                        break;
                    case 225:
                        {
                            if (canFly)
                                unitTarget->CastSpell(unitTarget, 42667, true);
                            else
                                unitTarget->CastSpell(unitTarget, 42683, true);
                            break;
                        }
                    case 300:
                        {
                            if (canFly)
                                unitTarget->CastSpell(unitTarget, 42668, true);
                            else
                                unitTarget->CastSpell(unitTarget, 42683, true);
                            break;
                        }
                    }
                    return;
                }
            // Mug Transformation
            case 41931:
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    uint8 bag = 19;
                    uint8 slot = 0;
                    Item* item = NULL;

                    while (bag != 0)
                    {
                        item = m_caster->ToPlayer()->GetItemByPos(bag, slot);
                        if (item && item->GetEntry() == 38587) break;
                        ++slot;
                        if (slot == 39)
                        {
                            slot = 0;
                            ++bag;
                        }
                    }
                    if (bag != 0)
                    {
                        if (m_caster->ToPlayer()->GetItemByPos(bag, slot)->GetCount() == 1) m_caster->ToPlayer()->RemoveItem(bag, slot, true);
                        else m_caster->ToPlayer()->GetItemByPos(bag, slot)->SetCount(m_caster->ToPlayer()->GetItemByPos(bag, slot)->GetCount() - 1);
                        // Spell 42518 (Braufest - Gratisprobe des Braufest herstellen)
                        m_caster->CastSpell(m_caster, 42518, true);
                        return;
                    }
                    break;
                }
            // Force Cast - Portal Effect: Sunwell Isle
            case 44876:
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, 44870, true);
                    break;
                }
            // Brutallus - Burn
            case 45141:
            case 45151:
                {
                    //Workaround for Range ... should be global for every ScriptEffect
                    //float radius = GetSpellRadius(sSpellRadiusStore.LookupEntry(m_spellInfo->EffectRadiusIndex[effIndex]));
                    float radius = GetSpellRadius(m_spellInfo, effIndex, false);
                    if (unitTarget && unitTarget->GetTypeId() == TYPEID_PLAYER && unitTarget->GetDistance(m_caster) <= radius && !unitTarget->HasAura(46394, 0) && unitTarget != m_caster)
                        unitTarget->CastSpell(unitTarget, 46394, true);

                    break;
                }
            // spell of Brutallus - Stomp
            case 45185:
                {
                    if (!unitTarget)
                        return;

                    if (unitTarget->HasAura(46394, 0))
                        unitTarget->RemoveAurasDueToSpell(46394);
                    break;
                }
            // Sinister Reflection
            case 45892:
            {
                if (unitTarget)
                {
                    // Summon 4 clones of the same player
                    for (uint8 i = 0; i < 4; ++i)
                        unitTarget->CastSpell((Unit*)NULL, 45891, true, NULL, NULL, m_caster->GetGUID());

                    unitTarget->CastSpell((Unit*)NULL, 45785, true);
                }
                return;
            }
            // Copy Weapon
            case 45785:
            { 
                if (!unitTarget || unitTarget->GetEntry() != 25708)
                    return;

                Player* pCaster = m_caster->ToPlayer();
                if (!pCaster)
                    return;

                Item* mainItem = NULL;
                Item* offItem = NULL;
                Item* rangedItem = NULL;
                if (mainItem = pCaster->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND))
                    unitTarget->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY + 0, mainItem->GetProto()->DisplayInfoID);

                if (offItem = pCaster->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND))
                    unitTarget->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY + 1, offItem->GetProto()->DisplayInfoID);

                if (rangedItem = pCaster->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_RANGED))
                    unitTarget->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY + 2, rangedItem->GetProto()->DisplayInfoID);

                break;
            }
            // Negative Energy
            case 46289:
                {
                    if (!unitTarget)
                        return;

                    m_caster->CastSpell(unitTarget, 46285, true);
                    break;
                }
            // Goblin Weather Machine
            case 46203:
                {
                    if (!unitTarget)
                        return;

                    uint32 spellId = 0;
                    switch (rand() % 4)
                    {
                    case 0:
                        spellId = 46740;
                        break;
                    case 1:
                        spellId = 46739;
                        break;
                    case 2:
                        spellId = 46738;
                        break;
                    case 3:
                        spellId = 46736;
                        break;
                    }
                    unitTarget->CastSpell(unitTarget, spellId, true);
                    break;
                }
            // 5,000 Gold
            case 46642:
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    unitTarget->ToPlayer()->ModifyMoney(5000 * GOLD);

                    break;
                }
            case 38011: //After move Facing Chess Event karazhan
                {
                    float Orientation[4];
                    Orientation[0] = 0.615f;
                    Orientation[1] = 2.234f;
                    Orientation[2] = 3.829f;
                    Orientation[3] = 5.378f;

                    //get div to closest orientation
                    float div = abs(Orientation[0] - m_caster->GetOrientation());
                    for (int i = 1; i <= 3; i++)
                    if (abs(Orientation[i] - m_caster->GetOrientation()) < div)
                        div = abs(Orientation[i] - m_caster->GetOrientation());
                    if (div > 0.3) //correct orientation
                    {
                        //get next orientation
                        int id = 0;
                        for (int i = 0; i <= 3; i++)
                        if (Orientation[i] - m_caster->GetOrientation() > 0)
                        {
                            id = i;
                            break;
                        }
                        //don't need to check orientation > 5.378 => it's 0 
                        m_caster->SetOrientation(Orientation[id]);

                        // and client
                        WorldPacket data;
                        m_caster->BuildHeartBeatMsg(&data);
                        m_caster->SendMessageToSet(&data, false);
                    }
                    break;
                }
            //handle Melee Attack Spells for Chess Pieces
            //Alliance Melee Attack (all 1000dmg? need proof for now just use DMG Footman Spell (32247) -- no other related spell like this
            case 37150:
            case 37149:
            case 37147:
            case 37143:
            case 37142:
            case 32227:
            //Horde Spells
            case 37348:
            case 37345:
            case 37337:
            case 37339:
            case 37220:
            case 32228:
                {
                    //we should do extra check here, can't remember we can hit targets that are not on a field in front of us..
                    if (m_caster->HasInArc(float(M_PI) / 18, unitTarget) && m_caster->GetDistance(unitTarget) <= 4)//sould be in an angle within 10� and Fieldrange
                        m_caster->CastSpell(unitTarget, 32247, true); //Chess NPC Action: Melee Attack: DAMAGE (Footman)
                    return;
                }
            case 45714:
                unitTarget->CastSpell(m_caster, m_spellInfo->EffectBasePoints[0], true);
                return;
            case 45717:
                m_caster->CastSpell(unitTarget, 45726, true);
                return;
            }
            break;
        }
    case SPELLFAMILY_WARLOCK:
        {
            switch (m_spellInfo->Id)
            {
            // Healthstone creating spells
            case  6201:
            case  6202:
            case  5699:
            case 11729:
            case 11730:
            case 27230:
                {
                    uint32 itemtype;
                    uint32 rank = 0;

                    // Improved Healthstone
                    Unit::AuraList const& mDummyAuras = unitTarget->GetAurasByType(SPELL_AURA_DUMMY);
                    for (Unit::AuraList::const_iterator i = mDummyAuras.begin(); i != mDummyAuras.end(); ++i)
                    {
                        if ((*i)->GetId() == 18692)
                        {
                            rank = 1;
                            break;
                        }
                        else if ((*i)->GetId() == 18693)
                        {
                            rank = 2;
                            break;
                        }
                    }

                    static uint32 const itypes[6][3] =
                    {
                        { 5512, 19004, 19005},              // Minor Healthstone
                        { 5511, 19006, 19007},              // Lesser Healthstone
                        { 5509, 19008, 19009},              // Healthstone
                        { 5510, 19010, 19011},              // Greater Healthstone
                        { 9421, 19012, 19013},              // Major Healthstone
                        {22103, 22104, 22105}               // Master Healthstone
                    };

                    switch (m_spellInfo->Id)
                    {
                    case  6201:
                        itemtype = itypes[0][rank];
                        break; // Minor Healthstone
                    case  6202:
                        itemtype = itypes[1][rank];
                        break; // Lesser Healthstone
                    case  5699:
                        itemtype = itypes[2][rank];
                        break; // Healthstone
                    case 11729:
                        itemtype = itypes[3][rank];
                        break; // Greater Healthstone
                    case 11730:
                        itemtype = itypes[4][rank];
                        break; // Major Healthstone
                    case 27230:
                        itemtype = itypes[5][rank];
                        break; // Master Healthstone
                    default:
                        return;
                    }
                    DoCreateItem(effIndex, itemtype);
                    return;
                }
            }
            break;
        }
    case SPELLFAMILY_PALADIN:
        {
            // Judgement (seal trigger)
            if (m_spellInfo->SpellFamilyFlags == 0x800000LL)
            {
                if (!unitTarget || !unitTarget->IsAlive())
                    return;
                uint32 spellId2 = 0;

                // all seals have aura dummy
                Unit::AuraList const& m_dummyAuras = m_caster->GetAurasByType(SPELL_AURA_DUMMY);
                for (Unit::AuraList::const_iterator itr = m_dummyAuras.begin(); itr != m_dummyAuras.end(); ++itr)
                {
                    SpellEntry const* spellInfo = (*itr)->GetSpellProto();

                    // search seal (all seals have judgement's aura dummy spell id in 2 effect
                    if (!spellInfo || !IsSealSpell((*itr)->GetSpellProto()) || (*itr)->GetEffIndex() != 2)
                        continue;

                    // must be calculated base at raw base points in spell proto, GetModifier()->m_value for S.Righteousness modified by SPELLMOD_DAMAGE
                    spellId2 = (*itr)->GetSpellProto()->EffectBasePoints[2] + 1;

                    if (spellId2 <= 1)
                        continue;

                    // found, remove seal
                    m_caster->RemoveAurasDueToSpell((*itr)->GetId());

                    // Sanctified Judgement
                    Unit::AuraList const& m_auras = m_caster->GetAurasByType(SPELL_AURA_DUMMY);
                    for (Unit::AuraList::const_iterator i = m_auras.begin(); i != m_auras.end(); ++i)
                    {
                        if ((*i)->GetSpellProto()->SpellIconID == 205 && (*i)->GetSpellProto()->Attributes == 0x01D0LL)
                        {
                            int32 chance = (*i)->GetModifier()->m_amount;
                            if (roll_chance_i(chance))
                            {
                                int32 mana = spellInfo->manaCost;
                                if (Player* modOwner = m_caster->GetSpellModOwner())
                                    modOwner->ApplySpellMod(spellInfo->Id, SPELLMOD_COST, mana);
                                mana = int32(mana * 0.8f);
                                m_caster->CastCustomSpell(m_caster, 31930, &mana, NULL, NULL, true, NULL, *i);
                            }
                            break;
                        }
                    }

                    break;
                }
                m_caster->CastSpell(unitTarget, spellId2, false);
                return;
            }
        }
    case SPELLFAMILY_POTION:
        {
            switch (m_spellInfo->Id)
            {
            // Dreaming Glory
            case 28698:
                {
                    if (!unitTarget)
                        return;
                    unitTarget->CastSpell(unitTarget, 28694, true);
                    break;
                }
            // Netherbloom
            case 28702:
                {
                    if (!unitTarget)
                        return;
                    // 25% chance of casting a random buff
                    if (roll_chance_i(75))
                        return;

                    // triggered spells are 28703 to 28707
                    // Note: some sources say, that there was the possibility of
                    //       receiving a debuff. However, this seems to be removed by a patch.
                    const uint32 spellid = 28703;

                    // don't overwrite an existing aura
                    for (uint8 i = 0; i < 5; ++i)
                        if (unitTarget->HasAura(spellid + i, 0))
                            return;
                    unitTarget->CastSpell(unitTarget, spellid + urand(0, 4), true);
                    break;
                }
            // Nightmare Vine
            case 28720:
                {
                    if (!unitTarget)
                        return;
                    // 25% chance of casting Nightmare Pollen
                    if (roll_chance_i(75))
                        return;
                    unitTarget->CastSpell(unitTarget, 28721, true);
                    break;
                }
            }
            break;
        }
    }

    // normal DB scripted effect
    DEBUG_LOG("Spell ScriptStart spellid %u in EffectScriptEffect ", m_spellInfo->Id);
    m_caster->GetMap()->ScriptsStart(sSpellScripts, m_spellInfo->Id, m_caster, unitTarget);
}

void Spell::EffectSanctuary(SpellEffIndex /*effIndex*/)
{
    if (!unitTarget)
        return;

    unitTarget->getHostileRefManager().UpdateVisibility();

    Unit::AttackerSet const& attackers = unitTarget->getAttackers();
    for (Unit::AttackerSet::const_iterator itr = attackers.begin(); itr != attackers.end();)
    {
        if (!(*itr)->CanSeeOrDetect(unitTarget))
            (*(itr++))->AttackStop();
        else
            ++itr;
    }

    unitTarget->m_lastSanctuaryTime = getMSTime();

    // Vanish allows to remove all threat and cast regular stealth so other spells can be used
    if (m_caster->GetTypeId() == TYPEID_PLAYER
        && m_spellInfo->SpellFamilyName == SPELLFAMILY_ROGUE
        && (m_spellInfo->SpellFamilyFlags & SPELLFAMILYFLAG_ROGUE_VANISH))
        m_caster->ToPlayer()->RemoveSpellsCausingAura(SPELL_AURA_MOD_ROOT);
}

void Spell::EffectAddComboPoints(SpellEffIndex /*effIndex*/)
{
    if (!unitTarget)
        return;

    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    if (damage <= 0)
        return;

    m_caster->ToPlayer()->AddComboPoints(unitTarget, damage);
}

void Spell::EffectDuel(SpellEffIndex effIndex)
{
    if (!m_caster || !unitTarget || m_caster->GetTypeId() != TYPEID_PLAYER || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* caster = m_caster->ToPlayer();
    Player* target = unitTarget->ToPlayer();

    // caster or target already have requested duel
    if (caster->duel || target->duel || !target->GetSocial() || target->GetSocial()->HasIgnore(caster->GetGUIDLow()))
        return;

    // Players can only fight a duel in zones with this flag
    AreaTableEntry const* casterAreaEntry = GetAreaEntryByAreaID(caster->GetAreaId());
    if (casterAreaEntry && !(casterAreaEntry->flags & AREA_FLAG_ALLOW_DUELS))
    {
        SendCastResult(SPELL_FAILED_NO_DUELING);            // Dueling isn't allowed here
        return;
    }

    AreaTableEntry const* targetAreaEntry = GetAreaEntryByAreaID(target->GetAreaId());
    if (targetAreaEntry && !(targetAreaEntry->flags & AREA_FLAG_ALLOW_DUELS))
    {
        SendCastResult(SPELL_FAILED_NO_DUELING);            // Dueling isn't allowed here
        return;
    }

    //CREATE DUEL FLAG OBJECT
    GameObject* pGameObj = new GameObject;

    uint32 gameobject_id = m_spellInfo->EffectMiscValue[effIndex];

    Map* map = m_caster->GetMap();
    if (!pGameObj->Create(sObjectMgr.GenerateLowGuid(HIGHGUID_GAMEOBJECT), gameobject_id, map,
                          m_caster->GetPositionX() + (unitTarget->GetPositionX() - m_caster->GetPositionX()) / 2 ,
                          m_caster->GetPositionY() + (unitTarget->GetPositionY() - m_caster->GetPositionY()) / 2 ,
                          m_caster->GetPositionZ(),
                          m_caster->GetOrientation(), 0.0f, 0.0f, 0.0f, 0.0f, 0, GO_STATE_READY))
    {
        delete pGameObj;
        return;
    }

    pGameObj->SetUInt32Value(GAMEOBJECT_FACTION, m_caster->GetFaction());
    pGameObj->SetUInt32Value(GAMEOBJECT_LEVEL, m_caster->getLevel() + 1);
    int32 duration = GetSpellDuration(m_spellInfo);
    pGameObj->SetRespawnTime(duration > 0 ? duration / IN_MILLISECONDS : 0);
    pGameObj->SetSpellId(m_spellInfo->Id);

    m_caster->AddGameObject(pGameObj);
    map->AddToMap(pGameObj);
    //END

    // Send request
    WorldPacket data(SMSG_DUEL_REQUESTED, 8 + 8);
    data << uint64(pGameObj->GetGUID());
    data << uint64(caster->GetGUID());
    caster->GetSession()->SendPacket(&data);
    target->GetSession()->SendPacket(&data);

    // create duel-info
    DuelInfo* duel   = new DuelInfo;
    duel->initiator  = caster;
    duel->opponent   = target;
    duel->startTime  = 0;
    duel->startTimer = 0;
    caster->duel     = duel;

    DuelInfo* duel2   = new DuelInfo;
    duel2->initiator  = caster;
    duel2->opponent   = caster;
    duel2->startTime  = 0;
    duel2->startTimer = 0;
    target->duel      = duel2;

    caster->SetUInt64Value(PLAYER_DUEL_ARBITER, pGameObj->GetGUID());
    target->SetUInt64Value(PLAYER_DUEL_ARBITER, pGameObj->GetGUID());
}

void Spell::EffectStuck(SpellEffIndex /*effIndex*/)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    if (!sWorld.getConfig(CONFIG_CAST_UNSTUCK))
        return;

    Player* pTarget = unitTarget->ToPlayer();
    if (!pTarget)
        return;

    DEBUG_LOG("Spell Effect: Stuck");
    sLog.outDetail("Player %s (guid %u) used auto-unstuck feature at map %u (%f, %f, %f)", pTarget->GetName(), pTarget->GetGUIDLow(), m_caster->GetMapId(), m_caster->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ());

    // If the player is dead, it will return them to the graveyard closest to their corpse.
    if (pTarget->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_GHOST))
    {
        pTarget->RepopAtGraveyard();
        return;
    }
    
    // If the player is alive, and their hearthstone is in their inventory, and their hearthstone
    // is cooled down, it will activate their hearthstone. The 30 minute hearthstone cooldown is activated as usual.
    if (!pTarget->HasSpellCooldown(8690) && pTarget->HasItemCount(6948, 1, false))
    {
        pTarget->TeleportToHomebind(TELE_TO_SPELL);

        // Trigger cooldown
        SpellEntry const* spellInfo = sSpellStore.LookupEntry(8690);
        if (!spellInfo)
            return;
        Spell spell(pTarget, spellInfo, true, 0);
        spell.SendSpellCooldown();
    }
    else
    {
        // If the player is alive, but their hearthstone is either not in their inventory (e.g. in the bank) or 
        // their hearthstone is on cooldown, then the game will try to "nudge" the player in a seemingly random direction.
        // @todo This check could possibly more accurately find a safe position to port to, has the potential for porting underground.
        float x, y, z;
        pTarget->GetNearPoint(pTarget, x, y, z, DEFAULT_WORLD_OBJECT_SIZE, 10.0f, pTarget->GetOrientation());
        pTarget->NearTeleportTo(x, y, z, pTarget->GetOrientation());
    }
}

void Spell::EffectSummonPlayer(SpellEffIndex /*effIndex*/)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    // Evil Twin (ignore player summon, but hide this for summoner)
    if (unitTarget->GetDummyAura(23445))
        return;

    float x, y, z;
    m_caster->GetPosition(x, y, z);

    unitTarget->ToPlayer()->SetSummonPoint(m_caster->GetMapId(), x, y, z);

    WorldPacket data(SMSG_SUMMON_REQUEST, 8 + 4 + 4);
    data << uint64(m_caster->GetGUID());                    // summoner guid
    data << uint32(m_caster->GetZoneId());                  // summoner zone
    data << uint32(MAX_PLAYER_SUMMON_DELAY * IN_MILLISECONDS); // auto decline after msecs
    unitTarget->ToPlayer()->GetSession()->SendPacket(&data);
}

static ScriptInfo generateActivateCommand()
{
    ScriptInfo si;
    si.command = SCRIPT_COMMAND_ACTIVATE_OBJECT;
    return si;
}

void Spell::EffectActivateObject(SpellEffIndex effIndex)
{
    if (!gameObjTarget)
        return;

    static ScriptInfo activateCommand = generateActivateCommand();

    /* This is not always delay, need more reaseach on this */
    int32 delay_secs = m_spellInfo->EffectMiscValue[effIndex];

    switch (gameObjTarget->GetGoType())
    {
        case GAMEOBJECT_TYPE_DOOR:
        case GAMEOBJECT_TYPE_BUTTON:
        gameObjTarget->UseDoorOrButton(delay_secs, false);
        return;
        case GAMEOBJECT_TYPE_SUMMONING_RITUAL:
            gameObjTarget->SendObjectCustomAnim(gameObjTarget->GetGUID());
            return;
        default:
    gameObjTarget->GetMap()->ScriptCommandStart(activateCommand, delay_secs, m_caster, gameObjTarget);
            return;
    }

    return; // not reached
}

void Spell::EffectEnchantHeldItem(SpellEffIndex effIndex)
{
    // this is only item spell effect applied to main-hand weapon of target player (players in area)
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* item_owner = unitTarget->ToPlayer();
    Item* item = item_owner->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);

    if (!item)
        return;

    // must be equipped
    if (!item ->IsEquipped())
        return;

    if (m_spellInfo->EffectMiscValue[effIndex])
    {
        uint32 enchant_id = m_spellInfo->EffectMiscValue[effIndex];
        int32 duration = GetSpellDuration(m_spellInfo);          //Try duration index first ..
        if (!duration)
            duration = damage;//+1;            //Base points after ..
        if (!duration)
            duration = 10;                                  //10 seconds for enchants which don't have listed duration

        SpellItemEnchantmentEntry const* pEnchant = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
        if (!pEnchant)
            return;

        // Always go to temp enchantment slot
        EnchantmentSlot slot = TEMP_ENCHANTMENT_SLOT;

        // Enchantment will not be applied if a different one already exists
        if (item->GetEnchantmentId(slot) && item->GetEnchantmentId(slot) != enchant_id)
            return;

        // Apply the temporary enchantment
        item->SetEnchantment(slot, enchant_id, duration * IN_MILLISECONDS, 0);
        item_owner->ApplyEnchantment(item, slot, true);
    }
}

void Spell::EffectDisEnchant(SpellEffIndex /*effIndex*/)
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* p_caster = m_caster->ToPlayer();
    if (!itemTarget || !itemTarget->GetProto()->DisenchantID)
        return;

    p_caster->UpdateCraftSkill(m_spellInfo->Id);

    m_caster->ToPlayer()->SendLoot(itemTarget->GetGUID(), LOOT_DISENCHANTING);

    // item will be removed at disenchanting end
}

void Spell::EffectInebriate(SpellEffIndex /*effIndex*/)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* player = unitTarget->ToPlayer();
    uint16 currentDrunk = player->GetDrunkValue();
    uint16 drunkMod = damage * 256;
    if (currentDrunk + drunkMod > 0xFFFF)
        currentDrunk = 0xFFFF;
    else
        currentDrunk += drunkMod;
    player->SetDrunkValue(currentDrunk, m_CastItem ? m_CastItem->GetEntry() : 0);
}

void Spell::EffectFeedPet(SpellEffIndex effIndex)
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* _player = m_caster->ToPlayer();

    if (!itemTarget)
        return;

    Pet* pet = _player->GetPet();
    if (!pet)
        return;

    if (!pet->IsAlive())
        return;

    int32 benefit = pet->GetCurrentFoodBenefitLevel(itemTarget->GetProto()->ItemLevel);
    if (benefit <= 0)
        return;

    uint32 count = 1;
    _player->DestroyItemCount(itemTarget, count, true);
    // @todo fix crash when a spell has two effects, both pointed at the same item target

    m_caster->CastCustomSpell(m_caster, m_spellInfo->EffectTriggerSpell[effIndex], &benefit, NULL, NULL, true);
}

void Spell::EffectDismissPet(SpellEffIndex /*effIndex*/)
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Pet* pet = m_caster->ToPlayer()->GetPet();

    // not let dismiss dead pet
    if (!pet || !pet->IsAlive())
        return;

    m_caster->ToPlayer()->RemovePet(pet, PET_SAVE_NOT_IN_SLOT);
}

void Spell::EffectSummonObject(SpellEffIndex effIndex)
{
    uint32 go_id = m_spellInfo->EffectMiscValue[effIndex];

    uint8 slot = 0;
    switch (m_spellInfo->Effect[effIndex])
    {
    case SPELL_EFFECT_SUMMON_OBJECT_SLOT1:
        slot = 0;
        break;
    case SPELL_EFFECT_SUMMON_OBJECT_SLOT2:
        slot = 1;
        break;
    case SPELL_EFFECT_SUMMON_OBJECT_SLOT3:
        slot = 2;
        break;
    case SPELL_EFFECT_SUMMON_OBJECT_SLOT4:
        slot = 3;
        break;
    default:
        return;
    }

    if (uint64 guid = m_caster->m_ObjectSlot[slot])
    {
        GameObject* obj = NULL;
        obj = m_caster->GetMap()->GetGameObject(guid);

        if (obj) obj->Delete();
        m_caster->m_ObjectSlot[slot] = 0;
    }

    GameObject* pGameObj = new GameObject;

    float x, y, z;
    // If dest location if present
    if (m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
        m_targets.m_dstPos.GetPosition(x, y, z);
    // Summon in random point all other units if location present
    else
        m_caster->GetClosePoint(x, y, z, DEFAULT_WORLD_OBJECT_SIZE);

    Map* map = m_caster->GetMap();
    if (!pGameObj->Create(sObjectMgr.GenerateLowGuid(HIGHGUID_GAMEOBJECT), go_id, map,
                          x, y, z, m_caster->GetOrientation(), 0.0f, 0.0f, 0.0f, 0.0f, 0, GO_STATE_READY))
    {
        delete pGameObj;
        return;
    }

    //pGameObj->SetUInt32Value(GAMEOBJECT_LEVEL,m_caster->getLevel());
    int32 duration = GetSpellDuration(m_spellInfo);
    pGameObj->SetRespawnTime(duration > 0 ? duration / IN_MILLISECONDS : 0);
    pGameObj->SetSpellId(m_spellInfo->Id);
    m_caster->AddGameObject(pGameObj);

    map->AddToMap(pGameObj);
    WorldPacket data(SMSG_GAMEOBJECT_SPAWN_ANIM_OBSOLETE, 8);
    data << uint64(pGameObj->GetGUID());
    m_caster->SendMessageToSet(&data, true);

    m_caster->m_ObjectSlot[slot] = pGameObj->GetGUID();
}

void Spell::EffectResurrect(SpellEffIndex /*effIndex*/)
{
    if (!unitTarget)
        return;

    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    if (unitTarget->IsAlive())
        return;

    if (!unitTarget->IsInWorld())
        return;

    switch (m_spellInfo->Id)
    {
    // Defibrillate (Goblin Jumper Cables) have 33% chance on success
    case 8342:
        if (roll_chance_i(67))
        {
            m_caster->CastSpell(m_caster, 8338, true, m_CastItem);
            return;
        }
        break;
    // Defibrillate (Goblin Jumper Cables XL) have 50% chance on success
    case 22999:
        if (roll_chance_i(50))
        {
            m_caster->CastSpell(m_caster, 23055, true, m_CastItem);
            return;
        }
        break;
    default:
        break;
    }

    Player* pTarget = unitTarget->ToPlayer();

    if (pTarget->isRessurectRequested())       // already have one active request
        return;

    uint32 health = pTarget->GetMaxHealth() * damage / 100;
    uint32 mana   = pTarget->GetMaxPower(POWER_MANA) * damage / 100;

    pTarget->setResurrectRequestData(m_caster->GetGUID(), m_caster->GetMapId(), m_caster->GetPositionX(), m_caster->GetPositionY(), m_caster->GetPositionZ(), health, mana);
    SendResurrectRequest(pTarget);
}

void Spell::EffectAddExtraAttacks(SpellEffIndex /*effIndex*/)
{
    if (!unitTarget || !unitTarget->IsAlive())
        return;

    //if (unitTarget->m_extraAttacks)
    //    return;

    Unit* victim = unitTarget->GetVictim();

    // attack prevented
    // fixme, some attacks may not target current victim, this is right now not handled
    if (!victim || !unitTarget->IsWithinMeleeRange(victim) || !unitTarget->HasInArc(2 * float(M_PI) / 3, victim))
        return;

    // Only for proc/log informations
    unitTarget->m_extraAttacks = damage;
    // Need to send log before attack is made
    SendLogExecute();
    m_needSpellLog = false;

    unitTarget->AttackerStateUpdate(victim, BASE_ATTACK, true);
}

void Spell::EffectParry(SpellEffIndex /*effIndex*/)
{
    if (unitTarget && unitTarget->GetTypeId() == TYPEID_PLAYER)
        unitTarget->ToPlayer()->SetCanParry(true);
}

void Spell::EffectBlock(SpellEffIndex /*effIndex*/)
{
    if (unitTarget && unitTarget->GetTypeId() == TYPEID_PLAYER)
        unitTarget->ToPlayer()->SetCanBlock(true);
}

void Spell::EffectMomentMove(SpellEffIndex effIndex)
{
    if (unitTarget->IsInFlight())
        return;

    if (!m_targets.HasDst())
        return;

    float selfAddition = 1.5f;
    float distance = GetSpellRadius(m_spellInfo, effIndex, false) + selfAddition;
    Map* map = unitTarget->GetMap();
    float x, y, z;
    float destX, destY, destZ, ground;
    float orientation = unitTarget->GetOrientation();
    unitTarget->GetPosition(x, y, z);

    Position destPos;
    unitTarget->GetPosition(&destPos);
    destX = x + distance * cos(orientation);
    destY = y + distance * sin(orientation);

    ground = map->GetHeight(destX, destY, z + 5.0f);
    destZ = ground + 5.0f;

    if (!unitTarget->HasUnitMovementFlag(MOVEMENTFLAG_FALLING) || (z - ground < 25.0f))
    {
        if ((unitTarget->HasUnitMovementFlag(MOVEMENTFLAG_FALLING) && (z - ground > 3.0f)) && (!map->IsInWater(x, y, z)))
        {
            Position pos;
            pos.Relocate(destX, destY, destZ, orientation);
            pos = unitTarget->GetFirstCollisionPosition(unitTarget->GetDistance(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ()), 0.0f);
            destPos.Relocate(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation());
        }
        else
        {
            // recalculate, we need it if want can blink in different situations
            uint32 mapid = m_caster->GetMapId();
            float tstX, tstY, tstZ, prevX, prevY, prevZ, beforewaterz, travelDistZ, newdistance, totalpath;
            float tstZ1, tstZ2, tstZ3, destZ1, destZ2, destZ3, srange, srange1, srange2, srange3;
            float maxtravelDistZ = 2.65f;
            const float step = 2.0f;
            const uint8 numChecks = ceil(fabs(distance / step));
            const float DELTA_X = (destX - x) / numChecks;
            const float DELTA_Y = (destY - y) / numChecks;
            int j = 1;
            for (; j < (numChecks + 1); j++)
            {
                prevX = x + (float(j - 1)*DELTA_X);
                prevY = y + (float(j - 1)*DELTA_Y);
                tstX = x + (float(j)*DELTA_X);
                tstY = y + (float(j)*DELTA_Y);

                if (j < 2)
                {
                    prevZ = z;
                    newdistance = 0.0f;
                    totalpath = 0.0f;
                }
                else
                    prevZ = tstZ;

                travelDistZ = sqrt((tstY - prevY)*(tstY - prevY) + (tstX - prevX)*(tstX - prevX));
                tstZ = map->GetHeight(tstX, tstY, prevZ + travelDistZ, true);

                if (!map->IsInWater(x, y, z))
                {
                    if (map->IsInWater(tstX, tstY, tstZ) && !map->IsInWater(prevX, prevY, prevZ))// if first we start contact with water, we save coordinate Z before water and use her
                    {
                        beforewaterz = prevZ;
                        tstZ = beforewaterz;
                    }
                    else if (map->IsInWater(tstX, tstY, tstZ)) // it next step , where first contact was previos step, and we must recalculate prevZ to Z before water.
                    {
                        prevZ = beforewaterz;
                        tstZ = beforewaterz;
                    }
                }
                else if (map->IsInWater(tstX, tstY, tstZ))
                {
                    prevZ = z;
                    tstZ = z;
                }

                if (!map->IsInWater(tstX, tstY, tstZ))  // second safety check z for blink way if on the ground
                {
                    // highest available point
                    tstZ1 = map->GetHeight(tstX, tstY, prevZ);
                    // upper or floor
                    tstZ2 = map->GetHeight(tstX, tstY, prevZ); 
                    //lower than floor
                    tstZ3 = map->GetHeight(tstX, tstY, prevZ);

                    //distance of rays, will select the shortest in 3D
                    srange1 = sqrt((tstY - prevY)*(tstY - prevY) + (tstX - prevX)*(tstX - prevX) + (tstZ1 - prevZ)*(tstZ1 - prevZ));
                    srange2 = sqrt((tstY - prevY)*(tstY - prevY) + (tstX - prevX)*(tstX - prevX) + (tstZ2 - prevZ)*(tstZ2 - prevZ));
                    srange3 = sqrt((tstY - prevY)*(tstY - prevY) + (tstX - prevX)*(tstX - prevX) + (tstZ3 - prevZ)*(tstZ3 - prevZ));

                    if (srange1 < srange2)
                        tstZ = tstZ1; 
                    else if (srange3 < srange2)
                        tstZ = tstZ3; 
                    else
                        tstZ = tstZ2;  
                }

                destZ = tstZ;
                srange = sqrt((tstY - prevY)*(tstY - prevY) + (tstX - prevX)*(tstX - prevX) + (tstZ - prevZ)*(tstZ - prevZ));
                totalpath += srange;

                if (totalpath > distance)
                    newdistance = totalpath - distance;

                bool col = VMAP::VMapFactory::createOrGetVMapManager()->getObjectHitPos(mapid, prevX, prevY, prevZ + 0.5f, tstX, tstY, tstZ + 0.5f, tstX, tstY, tstZ, -0.5f);

                // collision occured
                if (col || (newdistance > 0.0f) || (fabs(prevZ - tstZ) > maxtravelDistZ))
                {
                    if ((newdistance > 0.0f) && (newdistance < step))
                    {
                        destX = prevX + newdistance * cos(orientation);
                        destY = prevY + newdistance * sin(orientation);
                    }
                    else
                    {
                        // move back a bit
                        destX = tstX - (0.6 * cos(orientation));
                        destY = tstY - (0.6 * sin(orientation));
                    }

                    travelDistZ = sqrt((destY - prevY)*(destY - prevY) + (destX - prevX)*(destX - prevX));
                    // highest available point
                    destZ1 = map->GetHeight(destX, destY, prevZ + travelDistZ, true);
                    // upper or floor
                    destZ2 = map->GetHeight(destX, destY, prevZ + travelDistZ, true);
                    //lower than floor
                    destZ3 = map->GetHeight(destX, destY, prevZ - travelDistZ, true);

                    //distance of rays, will select the shortest in 3D
                    srange1 = sqrt((destY - prevY)*(destY - prevY) + (destX - prevX)*(destX - prevX) + (destZ1 - prevZ)*(destZ1 - prevZ));
                    srange2 = sqrt((destY - prevY)*(destY - prevY) + (destX - prevX)*(destX - prevX) + (destZ2 - prevZ)*(destZ2 - prevZ));
                    srange3 = sqrt((destY - prevY)*(destY - prevY) + (destX - prevX)*(destX - prevX) + (destZ3 - prevZ)*(destZ3 - prevZ));

                    if (srange1 < srange2)
                        destZ = destZ1; 
                    else if (srange3 < srange2)
                        destZ = destZ3; 
                    else
                        destZ = destZ2;

                    if (map->IsInWater(destX, destY, destZ)) // recheck collide on top water 
                        destZ = prevZ;

                    break;
                }
                // we have correct destZ now
            }

            destPos.Relocate(destX, destY, destZ, orientation);
        }
    }

    if (unitTarget->GetDistance(destPos) <= distance)
        unitTarget->NearTeleportTo(destPos.GetPositionX(), destPos.GetPositionY(), destPos.GetPositionZ(), destPos.GetOrientation(), true);

}

void Spell::EffectReputation(SpellEffIndex effIndex)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* _player = unitTarget->ToPlayer();

    int32  rep_change = damage;//+1;           // field store reputation change -1

    uint32 faction_id = m_spellInfo->EffectMiscValue[effIndex];

    FactionEntry const* factionEntry = sFactionStore.LookupEntry(faction_id);

    if (!factionEntry)
        return;

    _player->GetReputationMgr().ModifyReputation(factionEntry, rep_change);
}

void Spell::EffectQuestComplete(SpellEffIndex effIndex)
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* pPlayer = m_caster->ToPlayer();

    uint32 quest_id = m_spellInfo->EffectMiscValue[effIndex];
    pPlayer->AreaExploredOrEventHappens(quest_id);
}

void Spell::EffectSelfResurrect(SpellEffIndex effIndex)
{
    if (!unitTarget || unitTarget->IsAlive())
        return;
    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;
    if (!unitTarget->IsInWorld())
        return;

    uint32 health = 0;
    uint32 mana = 0;

    // flat case
    if (damage < 0)
    {
        health = uint32(-damage);
        mana = m_spellInfo->EffectMiscValue[effIndex];
    }
    // percent case
    else
    {
        health = uint32(damage / 100.0f * unitTarget->GetMaxHealth());
        if (unitTarget->GetMaxPower(POWER_MANA) > 0)
            mana = uint32(damage / 100.0f * unitTarget->GetMaxPower(POWER_MANA));
    }

    Player* plr = unitTarget->ToPlayer();
    plr->ResurrectPlayer(0.0f);

    plr->SetHealth(health);
    plr->SetPower(POWER_MANA, mana);
    plr->SetPower(POWER_RAGE, 0);
    plr->SetPower(POWER_ENERGY, plr->GetMaxPower(POWER_ENERGY));

    plr->SpawnCorpseBones();
}

void Spell::EffectSkinning(SpellEffIndex /*effIndex*/)
{
    if (unitTarget->GetTypeId() != TYPEID_UNIT)
        return;
    if (!m_caster || m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Creature* creature = unitTarget->ToCreature();
    int32 targetLevel = creature->getLevel();

    uint32 skill = creature->GetCreatureTemplate()->GetRequiredLootSkill();

    m_caster->ToPlayer()->SendLoot(creature->GetGUID(), LOOT_SKINNING);
    creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE);
    creature->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

    int32 reqValue = targetLevel < 10 ? 0 : targetLevel < 20 ? (targetLevel - 10) * 10 : targetLevel * 5;

    int32 skillValue = m_caster->ToPlayer()->GetPureSkillValue(skill);

    // Double chances for elites
    m_caster->ToPlayer()->UpdateGatherSkill(skill, skillValue, reqValue, creature->isElite() ? 2 : 1);
}

void Spell::EffectCharge(SpellEffIndex /*effIndex*/)
{
    if (!m_caster)
        return;

    Unit* target = m_targets.getUnitTarget();
    if (!target)
        return;

    if (m_caster->GetTypeId() == TYPEID_PLAYER)
        m_caster->ToPlayer()->SetFallInformation(time(NULL), m_caster->GetPositionZ());

    if (m_pathFinder)
    {
        m_caster->GetMotionMaster()->MoveCharge(m_pathFinder->getEndPosition().x, m_pathFinder->getEndPosition().y, m_pathFinder->getEndPosition().z, 42.0f, EVENT_CHARGE, m_pathFinder->getPathType());
    }
    else
    {
        Position pos;
        target->GetContactPoint(m_caster, pos.m_positionX, pos.m_positionY, pos.m_positionZ);

        if (!m_caster->IsWithinLOS(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ()))
        {
            float angle = m_caster->GetRelativeAngle(&pos);
            float dist = m_caster->GetDistance(pos);
            m_caster->GetFirstCollisionPosition(dist, angle);
        }

        m_caster->GetMotionMaster()->MoveCharge(pos.m_positionX, pos.m_positionY, pos.m_positionZ + 0.5f);
    }

    if (!IsPositiveSpell(m_spellInfo->Id) && m_caster->GetTypeId() == TYPEID_PLAYER)
        m_caster->Attack(target, true);
}

void Spell::EffectChargeDest(SpellEffIndex /*effIndex*/)
{
    if (m_targets.HasDst())
    {
        Position pos;
        m_targets.m_dstPos.GetPosition(&pos);

        if (!m_caster->IsWithinLOS(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ()))
        {
            float angle = m_caster->GetRelativeAngle(&pos);
            float dist = m_caster->GetDistance(pos);
            pos = m_caster->GetFirstCollisionPosition(dist, angle);
        }

        m_caster->GetMotionMaster()->MoveCharge(pos.m_positionX, pos.m_positionY, pos.m_positionZ);
    }
}



void Spell::EffectKnockBack(SpellEffIndex effIndex)
{
    if (!unitTarget)
        return;

    // Effect only works on players
    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    // Spells with SPELL_EFFECT_KNOCK_BACK (like Thunderstorm) can't knockback target if target has ROOT/STUN
    if (unitTarget->HasUnitState(UNIT_STATE_ROOT | UNIT_STATE_STUNNED))
        return;

    // Instantly interrupt non melee spells being cast
    if (unitTarget->IsNonMeleeSpellCast(true))
        unitTarget->InterruptNonMeleeSpells(true);

    float ratio = 0.1f;
    float speedxy = float(m_spellInfo->EffectMiscValue[effIndex]) * ratio;
    float speedz = float(damage) * ratio;
    if (speedxy < 0.1f && speedz < 0.1f)
        return;

    float x, y;
    if (m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
    {
            m_targets.m_dstPos.GetPosition(x, y);
    }
    else //if (m_spellInfo->Effect[effIndex] == SPELL_EFFECT_KNOCK_BACK)
    {
        m_caster->GetPosition(x, y);
    }

    float vcos, vsin;
    unitTarget->GetSinCos(x, y, vsin, vcos);

    WorldPacket data(SMSG_MOVE_KNOCK_BACK, (8+4+4+4+4+4));
    data << unitTarget->GetPackGUID();
    data << uint32(0);                                      // Counter
    data << float(vcos);                                    // x direction
    data << float(vsin);                                    // y direction
    data << float(speedxy);                                 // Horizontal speed
    data << float(-speedz);                                 // Z Movement speed (vertical)

    unitTarget->ToPlayer()->GetSession()->SendPacket(&data);
}

void Spell::EffectSendTaxi(SpellEffIndex effIndex)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    TaxiPathEntry const* entry = sTaxiPathStore.LookupEntry(m_spellInfo->EffectMiscValue[effIndex]);
    if (!entry)
        return;

    std::vector<uint32> nodes;

    nodes.resize(2);
    nodes[0] = entry->from;
    nodes[1] = entry->to;

    uint32 mountid = 0;
    switch (m_spellInfo->Id)
    {
    case 31606:       //Stormcrow Amulet
        mountid = 17447;
        break;
    case 45071:      //Quest - Sunwell Daily - Dead Scar Bombing Run
    case 45113:      //Quest - Sunwell Daily - Ship Bombing Run
    case 45353:      //Quest - Sunwell Daily - Ship Bombing Run Return
        mountid = 22840;
        break;
    case 34905:      //Stealth Flight
        mountid = 6851;
        break;
    case 41533:      //Fly of the Netherwing
    case 41540:      //Fly of the Netherwing
        mountid = 23468;
        break;
    }

    unitTarget->ToPlayer()->ActivateTaxiPathTo(nodes, mountid);

}

void Spell::EffectPlayerPull(SpellEffIndex effIndex)
{
    if (!unitTarget || !m_caster)
        return;

    // Effect only works on players
    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    float vsin = sin(unitTarget->GetAngle(m_caster));
    float vcos = cos(unitTarget->GetAngle(m_caster));

    WorldPacket data(SMSG_MOVE_KNOCK_BACK, (8 + 4 + 4 + 4 + 4 + 4));
    data << unitTarget->GetPackGUID();
    data << uint32(0);                                      // Sequence
    data << float(vcos);                                    // x direction
    data << float(vsin);                                    // y direction
    // Horizontal speed
    data << float(damage ? damage : unitTarget->GetDistance2d(m_caster));
    data << float(m_spellInfo->EffectMiscValue[effIndex]) / -10;   // Z Movement speed

    unitTarget->ToPlayer()->GetSession()->SendPacket(&data);
}

void Spell::EffectDispelMechanic(SpellEffIndex effIndex)
{
    if (!unitTarget)
        return;

    uint32 mechanic = m_spellInfo->EffectMiscValue[effIndex];

    Unit::AuraMap& Auras = unitTarget->GetAuras();
    for (Unit::AuraMap::iterator iter = Auras.begin(), next; iter != Auras.end(); iter = next)
    {
        next = iter;
        ++next;
        SpellEntry const* spell = sSpellStore.LookupEntry(iter->second->GetSpellProto()->Id);
        if (spell->Mechanic == mechanic || spell->EffectMechanic[iter->second->GetEffIndex()] == mechanic)
        {
            unitTarget->RemoveAurasDueToSpell(spell->Id);
            if (Auras.empty())
                break;
            else
                next = Auras.begin();
        }
    }
}

void Spell::EffectSummonDeadPet(SpellEffIndex /*effIndex*/)
{
    if (!m_caster->IsPlayer())
        return;

    Player* _player = m_caster->ToPlayer();
    Pet* pet = _player->GetPet();

    // Only dead pets should be revived
    if (pet && pet->IsAlive())
        return;

    // Don't revive the pet if it doesn't exist, or has been dismissed
    if (!m_caster->ToPlayer()->doesOwnPet() || m_caster->ToPlayer()->isPetDismissed())
        return;

    float px, py, pz;

    // If the pet was not found in the world we have to summon it,
    // add it to the world and relocate for everything to work
    if (!pet)
    {
        _player->GetClosePoint(px, py, pz, _player->GetObjectSize(), PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
        _player->SummonPet(0, px, py, pz, _player->GetOrientation(), SUMMON_PET, 0);

        // Make sure the pet has been summoned and exists in the world
        if (Pet* currentPet = _player->GetPet())
        {
            pet = currentPet;
            pet->AddToWorld();
            pet->Relocate(px, py, pz, _player->GetOrientation());
        }
        else return; // Something has gone wrong and the pet was not summoned
    }
    else
    {
        _player->GetPosition(px, py, pz);
        _player->GetMap()->CreatureRelocation(pet, px, py, pz, _player->GetOrientation());
    }

    pet->SetUInt32Value(UNIT_DYNAMIC_FLAGS, 0);
    pet->RemoveFlag (UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE);
    pet->setDeathState(ALIVE);
    pet->ClearUnitState(UNIT_STATE_ALL_STATE);
    pet->SetHealth(uint32(pet->GetMaxHealth() * (float(damage) / 100)));

    pet->SavePetToDB(PET_SAVE_AS_CURRENT);
    pet->SendUpdateToPlayer(_player);
}

void Spell::EffectDestroyAllTotems(SpellEffIndex /*effIndex*/)
{
    float mana = 0;
    for (int slot = SUMMON_SLOT_TOTEM; slot < MAX_TOTEM_SLOT; ++slot)
    {
        if (!m_caster->m_SummonSlot[slot])
            continue;

        Creature* totem = m_caster->GetMap()->GetCreature(m_caster->m_SummonSlot[slot]);
        if (totem && totem->IsTotem())
        {
            uint32 spell_id = totem->GetUInt32Value(UNIT_CREATED_BY_SPELL);
            SpellEntry const* spellInfo = sSpellStore.LookupEntry(spell_id);
            if (spellInfo)
            {
                if (spellInfo->ManaCostPercentage)
                    mana += spellInfo->ManaCostPercentage * (m_caster->GetCreateMana() / 100.f) * (damage / 100.f);
                else
                    mana += spellInfo->manaCost * (damage / 100.f);
            }
            ((Totem*)totem)->UnSummon();
        }
    }

    m_caster->EnergizeBySpell(m_caster, m_spellInfo->Id, int32(mana), POWER_MANA);
}

void Spell::EffectDurabilityDamage(SpellEffIndex effIndex)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    int32 slot = m_spellInfo->EffectMiscValue[effIndex];

    // FIXME: some spells effects have value -1/-2
    // Possibly its mean -1 all player equipped items and -2 all items
    if (slot < 0)
    {
        unitTarget->ToPlayer()->DurabilityPointsLossAll(damage, (slot < -1));
        return;
    }

    // invalid slot value
    if (slot >= INVENTORY_SLOT_BAG_END)
        return;

    if (Item* item = unitTarget->ToPlayer()->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
        unitTarget->ToPlayer()->DurabilityPointsLoss(item, damage);
}

void Spell::EffectDurabilityDamagePCT(SpellEffIndex effIndex)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    int32 slot = m_spellInfo->EffectMiscValue[effIndex];

    // FIXME: some spells effects have value -1/-2
    // Possibly its mean -1 all player equipped items and -2 all items
    if (slot < 0)
    {
        unitTarget->ToPlayer()->DurabilityLossAll(double(damage) / 100.0f, (slot < -1));
        return;
    }

    // invalid slot value
    if (slot >= INVENTORY_SLOT_BAG_END)
        return;

    if (damage <= 0)
        return;

    if (Item* item = unitTarget->ToPlayer()->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
        unitTarget->ToPlayer()->DurabilityLoss(item, double(damage) / 100.0f);
}

void Spell::EffectModifyThreatPercent(SpellEffIndex /*effIndex*/)
{
    if (!unitTarget)
        return;

    unitTarget->getThreatManager().modifyThreatPercent(m_caster, damage);
}

void Spell::EffectTransmitted(SpellEffIndex effIndex)
{
    uint32 name_id = m_spellInfo->EffectMiscValue[effIndex];

    switch (m_spellInfo->Id)
    {
        case 29886: // Create Soulwell
            if (m_caster->HasAura(18692))
                name_id = 183510;
            else if (m_caster->HasAura(18693))
                name_id = 183511;
            break;
        default:
            break;
    }

    GameObjectInfo const* goinfo = sObjectMgr.GetGameObjectInfo(name_id);

    if (!goinfo)
    {
        sLog.outErrorDb("Gameobject (Entry: %u) not exist and not created at spell (ID: %u) cast", name_id, m_spellInfo->Id);
        return;
    }

    float fx, fy, fz;

    if (m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
        m_targets.m_dstPos.GetPosition(fx, fy, fz);
    //FIXME: this can be better check for most objects but still hack
    else if (m_spellInfo->EffectRadiusIndex[effIndex] && m_spellInfo->speed == 0)
    {
        float dis = GetSpellRadius(m_spellInfo, effIndex, false);
        m_caster->GetClosePoint(fx, fy, fz, DEFAULT_WORLD_OBJECT_SIZE, dis);
    }
    else
    {
        float min_dis = GetSpellMinRange(sSpellRangeStore.LookupEntry(m_spellInfo->rangeIndex));
        float max_dis = GetSpellMaxRange(sSpellRangeStore.LookupEntry(m_spellInfo->rangeIndex));
        float dis = rand_norm() * (max_dis - min_dis) + min_dis;

        m_caster->GetClosePoint(fx, fy, fz, DEFAULT_WORLD_OBJECT_SIZE, dis);
    }

    Map* cMap = m_caster->GetMap();
    // if gameobject is summoning object, it should be spawned right on caster's position
    if (goinfo->type == GAMEOBJECT_TYPE_SUMMONING_RITUAL)
        m_caster->GetPosition(fx, fy, fz);

    GameObject* pGameObj = new GameObject;

    if (!pGameObj->Create(sObjectMgr.GenerateLowGuid(HIGHGUID_GAMEOBJECT), name_id, cMap,
                          fx, fy, fz, m_caster->GetOrientation(), 0.0f, 0.0f, 0.0f, 0.0f, 100, GO_STATE_READY))
    {
        delete pGameObj;
        return;
    }

    int32 duration = GetSpellDuration(m_spellInfo);

    switch (goinfo->type)
    {
    case GAMEOBJECT_TYPE_FISHINGNODE:
        {
            m_caster->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, pGameObj->GetGUID());
            m_caster->AddGameObject(pGameObj);              // will removed at spell cancel

            // end time of range when possible catch fish (FISHING_BOBBER_READY_TIME..GetDuration(m_spellInfo))
            // start time == fish-FISHING_BOBBER_READY_TIME (0..GetDuration(m_spellInfo)-FISHING_BOBBER_READY_TIME)
            int32 lastSec = 0;
            switch (urand(0, 3))
            {
                case 0: lastSec =  3; break;
                case 1: lastSec =  7; break;
                case 2: lastSec = 13; break;
                case 3: lastSec = 17; break;
            }

            duration = duration - lastSec * IN_MILLISECONDS + FISHING_BOBBER_READY_TIME * IN_MILLISECONDS;
            break;
        }
    case GAMEOBJECT_TYPE_SUMMONING_RITUAL:
        {
            if (m_caster->GetTypeId() == TYPEID_PLAYER)
            {
                pGameObj->AddUniqueUse(m_caster->ToPlayer());
                m_caster->AddGameObject(pGameObj);          // will removed at spell cancel
            }
            break;
        }
    case GAMEOBJECT_TYPE_FISHINGHOLE:
    case GAMEOBJECT_TYPE_CHEST:
    default:
        break;
    }

    pGameObj->SetRespawnTime(duration > 0 ? duration / IN_MILLISECONDS : 0);

    pGameObj->SetOwnerGUID(m_caster->GetGUID());

    //pGameObj->SetUInt32Value(GAMEOBJECT_LEVEL, m_caster->getLevel());
    pGameObj->SetSpellId(m_spellInfo->Id);

    DEBUG_LOG("AddObject at SpellEfects.cpp EffectTransmitted");
    //m_caster->AddGameObject(pGameObj);
    //m_ObjToDel.push_back(pGameObj);

    cMap->AddToMap(pGameObj);

    WorldPacket data(SMSG_GAMEOBJECT_SPAWN_ANIM_OBSOLETE, 8);
    data << uint64(pGameObj->GetGUID());
    m_caster->SendMessageToSet(&data, true);

    if (uint32 linkedEntry = pGameObj->GetGOInfo()->GetLinkedGameObjectEntry())
    {
        GameObject* linkedGO = new GameObject;
        if (linkedGO->Create(sObjectMgr.GenerateLowGuid(HIGHGUID_GAMEOBJECT), linkedEntry, cMap,
                             fx, fy, fz, m_caster->GetOrientation(), 0, 0, 0, 0, 100, GO_STATE_READY))
        {
            linkedGO->SetRespawnTime(duration > 0 ? duration / IN_MILLISECONDS : 0);
            //linkedGO->SetUInt32Value(GAMEOBJECT_LEVEL, m_caster->getLevel());
            linkedGO->SetSpellId(m_spellInfo->Id);
            linkedGO->SetOwnerGUID(m_caster->GetGUID());

            linkedGO->GetMap()->AddToMap(linkedGO);
        }
        else
        {
            delete linkedGO;
            linkedGO = NULL;
            return;
        }
    }
}

void Spell::EffectProspecting(SpellEffIndex /*effIndex*/)
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* p_caster = m_caster->ToPlayer();
    if (!itemTarget || !(itemTarget->GetProto()->Flags & ITEM_PROTO_FLAG_PROSPECTABLE))
        return;

    if (itemTarget->GetCount() < 5)
        return;

    if (sWorld.getConfig(CONFIG_SKILL_PROSPECTING))
    {
        uint32 SkillValue = p_caster->GetPureSkillValue(SKILL_JEWELCRAFTING);
        uint32 reqSkillValue = itemTarget->GetProto()->RequiredSkillRank;
        p_caster->UpdateGatherSkill(SKILL_JEWELCRAFTING, SkillValue, reqSkillValue);
    }

    m_caster->ToPlayer()->SendLoot(itemTarget->GetGUID(), LOOT_PROSPECTING);
}

void Spell::EffectSkill(SpellEffIndex /*effIndex*/)
{
    DEBUG_LOG("WORLD: SkillEFFECT");
}

/* There is currently no need for this effect. We handle it in Battleground.cpp
   If we would handle the resurrection here, the spiritguide would instantly disappear as the
   player revives, and so we wouldn't see the spirit heal visual effect on the npc.
   This is why we use a half sec delay between the visual effect and the resurrection itself */
void Spell::EffectSpiritHeal(SpellEffIndex /*effIndex*/)
{
    /*
    if (!unitTarget || unitTarget->IsAlive())
        return;
    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;
    if (!unitTarget->IsInWorld())
        return;

    //m_spellInfo->EffectBasePoints[i]; == 99 (percent?)
    //unitTarget->ToPlayer()->setResurrect(m_caster->GetGUID(), unitTarget->GetPositionX(), unitTarget->GetPositionY(), unitTarget->GetPositionZ(), unitTarget->GetMaxHealth(), unitTarget->GetMaxPower(POWER_MANA));
    unitTarget->ToPlayer()->ResurrectPlayer(1.0f);
    unitTarget->ToPlayer()->SpawnCorpseBones();
    */
}

// remove insignia spell effect
void Spell::EffectSkinPlayerCorpse(SpellEffIndex /*effIndex*/)
{
    DEBUG_LOG("Effect: SkinPlayerCorpse");
    if ((m_caster->GetTypeId() != TYPEID_PLAYER) || (unitTarget->GetTypeId() != TYPEID_PLAYER) || (unitTarget->IsAlive()))
        return;

    unitTarget->ToPlayer()->RemovedInsignia(m_caster->ToPlayer());
}

void Spell::EffectStealBeneficialBuff(SpellEffIndex effIndex)
{
    DEBUG_LOG("Effect: StealBeneficialBuff");

    if (!unitTarget || unitTarget == m_caster)                 // can't steal from self
        return;

    std::vector <Aura*> steal_list;
    // Create dispel mask by dispel type
    uint32 dispelMask  = GetDispelMask(DispelType(m_spellInfo->EffectMiscValue[effIndex]));
    Unit::AuraMap const& auras = unitTarget->GetAuras();
    for (Unit::AuraMap::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
    {
        Aura* aur = (*itr).second;
        if (aur && (1 << aur->GetSpellProto()->Dispel) & dispelMask)
        {
            // Need check for passive? this
            if (aur->IsPositive() && !aur->IsPassive() && !(aur->GetSpellProto()->AttributesEx4 & SPELL_ATTR4_NOT_STEALABLE))
                steal_list.push_back(aur);
        }
    }
    // Ok if exist some buffs for dispel try dispel it
    if (!steal_list.empty())
    {
        std::list < std::pair<uint32, uint64> > success_list;   // (spellId, casterGUID)
        std::list < uint32 > fail_list;                         // spellId
        int32 list_size = steal_list.size();
        // dispel N = damage buffs (or while exist buffs for dispel)
        for (int32 count = 0; count < damage && list_size > 0; ++count)
        {
            // Random select buff for dispel
            Aura* aur = steal_list[urand(0, list_size - 1)];

            // Base dispel chance
            // @todo possible chance depend from spell level??
            int32 miss_chance = 0;
            // Apply dispel mod from aura caster
            if (Unit* caster = aur->GetCaster())
            {
                if ( Player* modOwner = caster->GetSpellModOwner())
                    modOwner->ApplySpellMod(aur->GetSpellProto()->Id, SPELLMOD_RESIST_DISPEL_CHANCE, miss_chance, this);
            }
            // Try dispel
            if (roll_chance_i(miss_chance))
                fail_list.push_back(aur->GetId());
            else
                success_list.push_back( std::pair<uint32, uint64>(aur->GetId(), aur->GetCasterGUID()));

            // Remove buff from list for prevent doubles
            for (std::vector<Aura*>::iterator j = steal_list.begin(); j != steal_list.end();)
            {
                Aura* stealed = *j;
                if (stealed->GetId() == aur->GetId() && stealed->GetCasterGUID() == aur->GetCasterGUID())
                {
                    j = steal_list.erase(j);
                    --list_size;
                }
                else
                    ++j;
            }
        }
        // Really try steal and send log
        if (!success_list.empty())
        {
            int32 count = success_list.size();
            WorldPacket data(SMSG_SPELLSTEALLOG, 8 + 8 + 4 + 1 + 4 + count * 5);
            data << unitTarget->GetPackGUID();       // Victim GUID
            data << m_caster->GetPackGUID();         // Caster GUID
            data << uint32(m_spellInfo->Id);         // dispel spell id
            data << uint8(0);                        // not used
            data << uint32(count);                   // count
            for (std::list<std::pair<uint32, uint64> >::iterator j = success_list.begin(); j != success_list.end(); ++j)
            {
                SpellEntry const* spellInfo = sSpellStore.LookupEntry(j->first);
                data << uint32(spellInfo->Id);       // Spell Id
                data << uint8(0);                    // 0 - steals != 0 transfers
                unitTarget->RemoveAurasDueToSpellBySteal(spellInfo->Id, j->second, m_caster);
            }
            m_caster->SendMessageToSet(&data, true);
        }

        // Is there other way to send spellsteal resists?
        // Send fail log to client
        if (!fail_list.empty())
        {
            // Failed to dispell
            WorldPacket data(SMSG_DISPEL_FAILED, 8 + 8 + 4 + 4 * fail_list.size());
            data << uint64(m_caster->GetGUID());            // Caster GUID
            data << uint64(unitTarget->GetGUID());          // Victim GUID
            data << uint32(m_spellInfo->Id);                // dispel spell id
            for (std::list< uint32 >::iterator j = fail_list.begin(); j != fail_list.end(); ++j)
                data << uint32(*j);                         // Spell Id
            m_caster->SendMessageToSet(&data, true);
        }
    }
}

void Spell::EffectKillCredit(SpellEffIndex effIndex)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    unitTarget->ToPlayer()->RewardPlayerAndGroupAtEvent(m_spellInfo->EffectMiscValue[effIndex], unitTarget);
}

void Spell::EffectQuestFail(SpellEffIndex effIndex)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    unitTarget->ToPlayer()->FailQuest(m_spellInfo->EffectMiscValue[effIndex]);
}

void Spell::EffectBind(SpellEffIndex /*effIndex*/)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* player = (Player*)unitTarget;

    WorldLocation loc = WorldLocation(player->GetMapId(), player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation());;
    uint32 area_id = player->GetAreaId();

    player->SetHomebindToLocation(loc, area_id);

    // binding
    WorldPacket data(SMSG_BINDPOINTUPDATE, (4 + 4 + 4 + 4 + 4));
    data << float(loc.GetPositionX());
    data << float(loc.GetPositionY());
    data << float(loc.GetPositionZ());
    data << uint32(loc.GetMapId());
    data << uint32(area_id);
    player->SendDirectMessage(&data);

    DEBUG_LOG("New Home Position X is %f", loc.GetPositionX());
    DEBUG_LOG("New Home Position Y is %f", loc.GetPositionY());
    DEBUG_LOG("New Home Position Z is %f", loc.GetPositionZ());
    DEBUG_LOG("New Home MapId is %u", loc.GetMapId());
    DEBUG_LOG("New Home AreaId is %u", area_id);

    // zone update
    data.Initialize(SMSG_PLAYERBOUND, 8 + 4);
    data << uint64(m_caster->GetGUID());
    data << uint32(area_id);
    player->SendDirectMessage(&data);
}

void Spell::SummonGuardian(uint32 i, uint32 entry, SummonPropertiesEntry const* properties)
{
    Unit* caster = m_originalCaster;
    if (caster && caster->GetTypeId() == TYPEID_UNIT && caster->IsTotem())
        caster = caster->GetOwner();
    if (!caster)
        return;

    // in another case summon new
    uint8 level = caster->getLevel();

    // level of pet summoned using engineering item based at engineering skill level
    if (m_CastItem && caster->GetTypeId() == TYPEID_PLAYER)
    {
        ItemTemplate const* proto = m_CastItem->GetProto();
        if (proto && proto->RequiredSkill == SKILL_ENGINEERING)
        {
            uint16 skill202 = caster->ToPlayer()->GetSkillValue(SKILL_ENGINEERING);
            if (skill202)
                level = skill202 / 5;
        }
    }

    //float radius = GetSpellRadiusForFriend(sSpellRadiusStore.LookupEntry(m_spellInfo->EffectRadiusIndex[i]));
    float radius = 5.0f;
    uint32 amount = damage > 0 ? damage : 1;
    int32 duration = GetSpellDuration(m_spellInfo);
    switch (m_spellInfo->Id)
    {
    case 1122: // Inferno
        amount = 1;
        break;
    }
    if (Player* modOwner = m_originalCaster->GetSpellModOwner())
        modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_DURATION, duration);

    //TempSummonType summonType = (duration == 0) ? TEMPSUMMON_DEAD_DESPAWN : TEMPSUMMON_TIMED_DESPAWN;
    Map* map = caster->GetMap();

    for (uint32 count = 0; count < amount; ++count)
    {
        Position pos;
        GetSummonPosition(i, pos, radius);

        TempSummon* summon = map->SummonCreature(entry, pos, properties, duration, caster);
        if (!summon)
            return;
        if (summon->HasUnitTypeMask(UNIT_MASK_GUARDIAN))
            ((Guardian*)summon)->InitStatsForLevel(level);

        if (properties && properties->Category == SUMMON_CATEGORY_ALLY)
            summon->SetFaction(caster->GetFaction());

        if (summon->HasUnitTypeMask(UNIT_MASK_MINION) && m_targets.HasDst())
            ((Minion*)summon)->SetFollowAngle(m_caster->GetAngle(summon));

        summon->SetUInt32Value(UNIT_CREATED_BY_SPELL, m_spellInfo->Id);
        summon->AI()->EnterEvadeMode();
    }
}

void Spell::GetSummonPosition(uint32 i, Position& pos, float radius)
{
    pos.SetOrientation(m_caster->GetOrientation());

    if (m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
    {
        switch (m_spellInfo->EffectImplicitTargetA[i])
        {
        case TARGET_MINION:
        case TARGET_DEST_CASTER_RANDOM:
            radius = std::max<float>(radius, m_caster->GetObjectSize());
            pos = m_caster->GetNearPosition(radius * rand_norm(), rand_norm() * 2 * M_PI);
            break;
        case TARGET_DEST_DEST_RANDOM:
        case TARGET_DEST_TARGET_RANDOM:
            m_caster->GetRandomPoint(m_targets.m_dstPos, radius, pos);
            break;
        default:
            pos.Relocate(m_targets.m_dstPos);
            break;
        }
    }
    // Summon if dest location not present near caster
    else
    {
        float x, y, z;
        m_caster->GetClosePoint(x, y, z, 3.0f);
        pos.Relocate(x, y, z);
    }
}

void Spell::EffectRedirectThreat(SpellEffIndex /*effIndex*/)
{
    if (unitTarget)
        m_caster->SetReducedThreatPercent(100, unitTarget->GetGUID());
}

void Spell::EffectSummonFriend(SpellEffIndex /*effIndex*/)
{
    if (!m_caster->ToPlayer())
        return;

    /* This is a workeround. Normally this should trigger EffectTriggerSpell[i],
       but it won't work because we will lost unit target in UpdatePointes -> SpellCastTargets::Update */

    if (Player* buddy = sObjectMgr.GetRAFLinkedBuddyForPlayer(m_caster->ToPlayer()))
    {
        float x, y, z;
        m_caster->GetClosePoint(x, y, z, buddy->GetObjectSize());

        buddy->SetSummonPoint(m_caster->GetMapId(), x, y, z);

        WorldPacket data(SMSG_SUMMON_REQUEST, 8 + 4 + 4);
        data << uint64(m_caster->GetGUID());                     // summoner guid
        data << uint32(m_caster->GetZoneId());                   // summoner zone
        data << uint32(MAX_PLAYER_SUMMON_DELAY * IN_MILLISECONDS); // auto decline after msecs
        buddy->GetSession()->SendPacket(&data);
    }
}
