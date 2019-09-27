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

#ifndef __SPELL_H
#define __SPELL_H

#include "GridDefines.h"
#include "SpellMgr.h"
#include "SharedDefines.h"
#include "PathFinder.h"

static const uint32 MAX_SPELL_ID = 53085;

class Unit;
class Player;
class GameObject;
class Aura;

enum SpellCastFlags
{
    CAST_FLAG_PENDING            = 0x00000001,              // Pending == 1
    CAST_FLAG_UNKNOWN1           = 0x00000002,
    CAST_FLAG_UNKNOWN2           = 0x00000010,
    CAST_FLAG_AMMO               = 0x00000020,
    CAST_FLAG_UNKNOWN3           = 0x00000100
};

enum SpellRangeFlag
{
    SPELL_RANGE_DEFAULT             = 0,
    SPELL_RANGE_MELEE               = 1,     //melee
    SPELL_RANGE_RANGED              = 2,     //hunter range and ranged weapon
};

enum SpellNotifyPushType
{
    PUSH_NONE           = 0,
    PUSH_IN_FRONT,
    PUSH_IN_BACK,
    PUSH_IN_LINE,
    PUSH_SRC_CENTER,
    PUSH_DST_CENTER,
    PUSH_CASTER_CENTER, //this is never used in grid search
    PUSH_CHAIN,
};

bool IsQuestTameSpell(uint32 spellId);

namespace Oregon
{
struct SpellNotifierCreatureAndPlayer;
}

class SpellCastTargets;

struct SpellCastTargetsReader
{
    explicit SpellCastTargetsReader(SpellCastTargets& _targets, Unit* _caster) : targets(_targets), caster(_caster) {}

    SpellCastTargets& targets;
    Unit* caster;
};

class SpellCastTargets
{
    public:
        SpellCastTargets();
        ~SpellCastTargets();

        void read(ByteBuffer& data, Unit* caster);
        void write(ByteBuffer& data) const;

        SpellCastTargetsReader ReadForCaster(Unit* caster)
        {
            return SpellCastTargetsReader(*this, caster);
        }

        SpellCastTargets& operator=(const SpellCastTargets& target)
        {
            m_unitTarget = target.m_unitTarget;
            m_itemTarget = target.m_itemTarget;
            m_GOTarget   = target.m_GOTarget;

            m_unitTargetGUID   = target.m_unitTargetGUID;
            m_GOTargetGUID     = target.m_GOTargetGUID;
            m_CorpseTargetGUID = target.m_CorpseTargetGUID;
            m_itemTargetGUID   = target.m_itemTargetGUID;

            m_itemTargetEntry  = target.m_itemTargetEntry;

            m_srcPos = target.m_srcPos;
            m_dstPos.Relocate(target.m_dstPos);

            m_strTarget = target.m_strTarget;

            m_targetMask = target.m_targetMask;

            return *this;
        }

        uint64 getUnitTargetGUID() const
        {
            return m_unitTargetGUID.GetRawValue();
        }
        Unit* getUnitTarget() const
        {
            return m_unitTarget;
        }
        void setUnitTarget(Unit* target);
        void setSrc(float x, float y, float z);
        void setSrc(Position* pos);
        void setDst(float x, float y, float z, float orientation, uint32 mapId = MAPID_INVALID);
        void setDst(Position* pos);

        uint64 getGOTargetGUID() const
        {
            return m_GOTargetGUID.GetRawValue();
        }
        GameObject* getGOTarget() const
        {
            return m_GOTarget;
        }
        void setGOTarget(GameObject* target);

        uint64 getCorpseTargetGUID() const
        {
            return m_CorpseTargetGUID.GetRawValue();
        }
        void setCorpseTarget(Corpse* corpse);
        uint64 getItemTargetGUID() const
        {
            return m_itemTargetGUID.GetRawValue();
        }
        Item* getItemTarget() const
        {
            return m_itemTarget;
        }
        uint32 getItemTargetEntry() const
        {
            return m_itemTargetEntry;
        }
        void setItemTarget(Item* item);
        void updateTradeSlotItem()
        {
            if (m_itemTarget && (m_targetMask & TARGET_FLAG_TRADE_ITEM))
            {
                m_itemTargetGUID = m_itemTarget->GetGUID();
                m_itemTargetEntry = m_itemTarget->GetEntry();
            }
        }

        bool IsEmpty() const
        {
            return m_GOTargetGUID.IsEmpty() && m_unitTargetGUID.IsEmpty() && m_itemTarget == NULL && m_CorpseTargetGUID.IsEmpty();
        }

        bool HasSrc() const { return (m_targetMask & TARGET_FLAG_SOURCE_LOCATION) != 0; }
        bool HasDst() const { return (m_targetMask & TARGET_FLAG_DEST_LOCATION) != 0; }

        void Update(Unit* caster);

        Position m_srcPos;
        WorldLocation m_dstPos;
        std::string m_strTarget;

        uint32 m_targetMask;
    private:
        // objects (can be used at spell creating and after Update at casting
        Unit* m_unitTarget;
        GameObject* m_GOTarget;
        Item* m_itemTarget;

        // object GUID/etc, can be used always
        ObjectGuid m_unitTargetGUID;
        ObjectGuid m_GOTargetGUID;
        ObjectGuid m_CorpseTargetGUID;
        ObjectGuid m_itemTargetGUID;
        uint32 m_itemTargetEntry;
};

inline ByteBuffer& operator<< (ByteBuffer& buf, SpellCastTargets const& targets)
{
    targets.write(buf);
    return buf;
}

inline ByteBuffer& operator>> (ByteBuffer& buf, SpellCastTargetsReader const& targets)
{
    targets.targets.read(buf, targets.caster);
    return buf;
}


struct SpellValue
{
    explicit SpellValue(SpellEntry const* proto)
    {
        for (uint32 i = 0; i < 3; ++i)
            EffectBasePoints[i] = proto->EffectBasePoints[i];
        MaxAffectedTargets = proto->MaxAffectedTargets;
        Duration = GetSpellDuration(proto);
        CustomDuration = false;
    }
    int32     EffectBasePoints[3];
    uint32    MaxAffectedTargets;
    uint32    Duration;
    bool      CustomDuration;
};

enum SpellState
{
    SPELL_STATE_NULL      = 0,
    SPELL_STATE_PREPARING = 1,
    SPELL_STATE_CASTING   = 2,
    SPELL_STATE_FINISHED  = 3,
    SPELL_STATE_IDLE      = 4,
    SPELL_STATE_DELAYED   = 5
};

enum ReplenishType
{
    REPLENISH_UNDEFINED = 0,
    REPLENISH_HEALTH    = 20,
    REPLENISH_MANA      = 21,
    REPLENISH_RAGE      = 22
};

enum SpellTargets
{
    SPELL_TARGETS_ALLY,
    SPELL_TARGETS_ENEMY,
    SPELL_TARGETS_ENTRY,
    SPELL_TARGETS_CHAINHEAL,
};

class Spell
{
        friend struct Oregon::SpellNotifierCreatureAndPlayer;
        friend void Unit::SetCurrentCastedSpell(Spell* pSpell);
    public:

        void EffectNULL(SpellEffIndex effIndex);
        void EffectUnused(SpellEffIndex effIndex);
        void EffectDistract(SpellEffIndex effIndex);
        void EffectPull(SpellEffIndex effIndex);
        void EffectSchoolDMG(SpellEffIndex effIndex);
        void EffectEnvironmentalDMG(SpellEffIndex effIndex);
        void EffectInstaKill(SpellEffIndex effIndex);
        void EffectDummy(SpellEffIndex effIndex);
        void EffectTeleportUnits(SpellEffIndex effIndex);
        void EffectApplyAura(SpellEffIndex effIndex);
        void EffectSendEvent(SpellEffIndex effIndex);
        void EffectPowerBurn(SpellEffIndex effIndex);
        void EffectPowerDrain(SpellEffIndex effIndex);
        void EffectHeal(SpellEffIndex effIndex);
        void EffectBind(SpellEffIndex effIndex);
        void EffectHealthLeech(SpellEffIndex effIndex);
        void EffectQuestComplete(SpellEffIndex effIndex);
        void EffectCreateItem(SpellEffIndex effIndex);
        void EffectPersistentAA(SpellEffIndex effIndex);
        void EffectEnergize(SpellEffIndex effIndex);
        void EffectOpenLock(SpellEffIndex effIndex);
        void EffectSummonChangeItem(SpellEffIndex effIndex);
        void EffectOpenSecretSafe(SpellEffIndex effIndex);
        void EffectProficiency(SpellEffIndex effIndex);
        void EffectSummonType(SpellEffIndex effIndex);
        void EffectLearnSpell(SpellEffIndex effIndex);
        void EffectDispel(SpellEffIndex effIndex);
        void EffectDualWield(SpellEffIndex effIndex);
        void EffectPickPocket(SpellEffIndex effIndex);
        void EffectAddFarsight(SpellEffIndex effIndex);
        void EffectHealMechanical(SpellEffIndex effIndex);
        void EffectTeleUnitsFaceCaster(SpellEffIndex effIndex);
        void EffectLearnSkill(SpellEffIndex effIndex);
        void EffectAddHonor(SpellEffIndex effIndex);
        void EffectTradeSkill(SpellEffIndex effIndex);
        void EffectEnchantItemPerm(SpellEffIndex effIndex);
        void EffectEnchantItemTmp(SpellEffIndex effIndex);
        void EffectTameCreature(SpellEffIndex effIndex);
        void EffectSummonPet(SpellEffIndex effIndex);
        void SummonClassPet(SpellEffIndex effIndex);
        void EffectLearnPetSpell(SpellEffIndex effIndex);
        void EffectWeaponDmg(SpellEffIndex effIndex);
        void EffectForceCast(SpellEffIndex effIndex);
        void EffectTriggerSpell(SpellEffIndex effIndex);
        void EffectTriggerMissileSpell(SpellEffIndex effIndex);
        void EffectThreat(SpellEffIndex effIndex);
        void EffectHealMaxHealth(SpellEffIndex effIndex);
        void EffectInterruptCast(SpellEffIndex effIndex);
        void EffectSummonObjectWild(SpellEffIndex effIndex);
        void EffectScriptEffect(SpellEffIndex effIndex);
        void EffectSanctuary(SpellEffIndex effIndex);
        void EffectAddComboPoints(SpellEffIndex effIndex);
        void EffectDuel(SpellEffIndex effIndex);
        void EffectStuck(SpellEffIndex effIndex);
        void EffectSummonPlayer(SpellEffIndex effIndex);
        void EffectActivateObject(SpellEffIndex effIndex);
        void EffectEnchantHeldItem(SpellEffIndex effIndex);
        void EffectSummonObject(SpellEffIndex effIndex);
        void EffectResurrect(SpellEffIndex effIndex);
        void EffectParry(SpellEffIndex effIndex);
        void EffectBlock(SpellEffIndex effIndex);
        void EffectMomentMove(SpellEffIndex effIndex);
        void EffectTransmitted(SpellEffIndex effIndex);
        void EffectDisEnchant(SpellEffIndex effIndex);
        void EffectInebriate(SpellEffIndex effIndex);
        void EffectFeedPet(SpellEffIndex effIndex);
        void EffectDismissPet(SpellEffIndex effIndex);
        void EffectReputation(SpellEffIndex effIndex);
        void EffectSelfResurrect(SpellEffIndex effIndex);
        void EffectSkinning(SpellEffIndex effIndex);
        void EffectCharge(SpellEffIndex effIndex);
        void EffectChargeDest(SpellEffIndex effIndex);
        void EffectProspecting(SpellEffIndex effIndex);
        void EffectSendTaxi(SpellEffIndex effIndex);
        void EffectSummonCritter(SpellEffIndex effIndex);
        void EffectKnockBack(SpellEffIndex effIndex);
        void EffectPlayerPull(SpellEffIndex effIndex);
        void EffectDispelMechanic(SpellEffIndex effIndex);
        void EffectSummonDeadPet(SpellEffIndex effIndex);
        void EffectDestroyAllTotems(SpellEffIndex effIndex);
        void EffectDurabilityDamage(SpellEffIndex effIndex);
        void EffectSkill(SpellEffIndex effIndex);
        void EffectTaunt(SpellEffIndex effIndex);
        void EffectDurabilityDamagePCT(SpellEffIndex effIndex);
        void EffectModifyThreatPercent(SpellEffIndex effIndex);
        void EffectResurrectNew(SpellEffIndex effIndex);
        void EffectAddExtraAttacks(SpellEffIndex effIndex);
        void EffectSpiritHeal(SpellEffIndex effIndex);
        void EffectSkinPlayerCorpse(SpellEffIndex effIndex);
        void EffectStealBeneficialBuff(SpellEffIndex effIndex);
        void EffectUnlearnSpecialization(SpellEffIndex effIndex);
        void EffectHealPct(SpellEffIndex effIndex);
        void EffectEnergizePct(SpellEffIndex effIndex);
        void EffectTriggerSpellWithValue(SpellEffIndex effIndex);
        void EffectTriggerRitualOfSummoning(SpellEffIndex effIndex);
        void EffectKillCredit(SpellEffIndex effIndex);
        void EffectQuestFail(SpellEffIndex effIndex);
        void EffectRedirectThreat(SpellEffIndex effIndex);
        void EffectSummonFriend(SpellEffIndex effIndex);

        Spell(Unit* Caster, SpellEntry const* info, bool triggered, uint64 originalCasterGUID = 0, Spell** triggeringContainer = NULL, bool skipCheck = false);
        ~Spell();

        void prepare(SpellCastTargets* targets, Aura* triggeredByAura = NULL);
        void cancel(bool sendInterrupt = true);
        void update(uint32 difftime);
        void cast(bool skipCheck = false);
        void finish(bool ok = true);
        void TakePower();
        void TakeReagents();
        void TakeCastItem();
        void TriggerSpell();
        SpellCastResult CheckCast(bool strict);
        SpellCastResult CheckPetCast(Unit* target);
        static uint32 GetCCDelay(uint32 _spell);
        SpellCastResult CheckDummyCast(uint32 effIndex);
        bool CanAutoCast(Unit* target);

        // handlers
        void handle_immediate();
        uint64 handle_delayed(uint64 t_offset);
        // handler helpers
        void _handle_immediate_phase();
        void _handle_finish_phase();

        SpellCastResult CheckItems();
        SpellCastResult CheckRange(bool strict);
        SpellCastResult CheckPower();
        SpellCastResult CheckCasterAuras() const;

        int32 CalculateDamage(uint8 i, Unit* target)
        {
            if (m_originalCaster)
                return m_originalCaster->CalculateSpellDamage(m_spellInfo, i, m_currentBasePoints[i], target);
            return m_caster->CalculateSpellDamage(m_spellInfo, i, m_currentBasePoints[i], target);
        }

        bool HaveTargetsForEffect(uint8 effect) const;
        void Delayed();
        void DelayedChannel();
        uint32 getState() const
        {
            return m_spellState;
        }
        void setState(uint32 state)
        {
            m_spellState = state;
        }

        void DoCreateItem(uint32 i, uint32 itemtype);

        void WriteSpellGoTargets(WorldPacket* data);
        void WriteAmmoToPacket(WorldPacket* data);
        void FillTargetMap();

        void SetTargetMap(uint32 i, uint32 cur);

        Unit* SelectMagnetTarget();
        void HandleHitTriggerAura();
        bool CheckTarget(Unit* target, uint32 eff);

        void CheckSrc()
        {
            if (!m_targets.HasSrc()) m_targets.setSrc(m_caster);
        }
        void CheckDst()
        {
            if (!m_targets.HasDst()) m_targets.setDst(m_caster);
        }

        void SendCastResult(SpellCastResult result);
        void SendSpellStart();
        void SendSpellGo();
        void SendSpellCooldown();
        void SendLogExecute();
        void SendInterrupted(uint8 result);
        void SendChannelUpdate(uint32 time);
        void SendChannelStart(uint32 duration);
        void SendResurrectRequest(Player* target);
        void SendPlaySpellVisual(uint32 SpellID);

        void HandleEffects(Unit* pUnitTarget, Item* pItemTarget, GameObject* pGOTarget, uint32 i, float DamageMultiplier = 1.0);
        void HandleThreatSpells();
        //void HandleAddAura(Unit* Target);

        const SpellEntry* const m_spellInfo;
        int32 m_currentBasePoints[3];                       // cache SpellEntry::EffectBasePoints and use for set custom base points
        Item* m_CastItem;
        uint64 m_castItemGUID;
        uint8 m_cast_count;
        SpellCastTargets m_targets;


        int32 GetCastTime() const
        {
            return m_casttime;
        }
        bool IsAutoRepeat() const
        {
            return m_autoRepeat;
        }
        void SetAutoRepeat(bool rep)
        {
            m_autoRepeat = rep;
        }
        void ReSetTimer()
        {
            m_timer = m_casttime > 0 ? m_casttime : 0;
        }
        bool IsNextMeleeSwingSpell() const
        {
            return m_spellInfo->Attributes & (SPELL_ATTR0_ON_NEXT_SWING_1 | SPELL_ATTR0_ON_NEXT_SWING_2);
        }
        bool IsRangedSpell() const
        {
            return  m_spellInfo->Attributes & SPELL_ATTR0_RANGED;
        }
        bool IsChannelActive() const
        {
            return m_caster->GetUInt32Value(UNIT_CHANNEL_SPELL) != 0;
        }
        bool IsAutoActionResetSpell() const
        {
            return !m_IsTriggeredSpell && (m_spellInfo->InterruptFlags & SPELL_INTERRUPT_FLAG_AUTOATTACK);
        }

        bool IsDeletable() const
        {
            return !m_referencedFromCurrentSpell && !m_executedCurrently;
        }
        void SetReferencedFromCurrent(bool yes)
        {
            m_referencedFromCurrentSpell = yes;
        }
        bool IsInterruptable() const
        {
            return !m_executedCurrently;
        }
        void SetExecutedCurrently(bool yes)
        {
            m_executedCurrently = yes;
        }
        uint64 GetDelayStart() const
        {
            return m_delayStart;
        }
        void SetDelayStart(uint64 m_time)
        {
            m_delayStart = m_time;
        }
        uint64 GetDelayMoment() const
        {
            return m_delayMoment;
        }

        CurrentSpellTypes GetCurrentContainer();

        Unit* GetCaster() const
        {
            return m_caster;
        }
        Unit* GetOriginalCaster() const
        {
            return m_originalCaster;
        }
        int32 GetPowerCost() const
        {
            return m_powerCost;
        }

        void UpdatePointers();                              // must be used at call Spell code after time delay (non triggered spell cast/update spell call/etc)
        void CalculateHitResults();

        bool IsAffectedBy(SpellEntry const* spellInfo, uint32 effectId);

        bool CheckTargetCreatureType(Unit* target) const;

        void AddTriggeredSpell(SpellEntry const* spell)
        {
            m_TriggerSpells.push_back(spell);
        }

        void CleanupTargetList();

        void SetSpellValue(SpellValueMod mod, int32 value);
    protected:

        void SendLoot(uint64 guid, LootType loottype);

        Unit* const m_caster;

        SpellValue* const m_spellValue;
        PathInfo* m_pathFinder;

        uint64 m_originalCasterGUID;                        // real source of cast (aura caster/etc), used for spell targets selection
        // e.g. damage around area spell trigered by victim aura and damage enemies of aura caster
        Unit* m_originalCaster;                             // cached pointer for m_originalCaster, updated at Spell::UpdatePointers()

        Spell** m_selfContainer;                            // pointer to our spell container (if applicable)
        Spell** m_triggeringContainer;                      // pointer to container with spell that has triggered us

        //Spell data
        SpellSchoolMask m_spellSchoolMask;                  // Spell school (can be overwrite for some spells (wand shoot for example)
        WeaponAttackType m_attackType;                      // For weapon based attack
        int32 m_powerCost;                                  // Calculated spell cost     initialized only in Spell::prepare
        int32 m_casttime;                                   // Calculated spell cast time initialized only in Spell::prepare
        bool m_canReflect;                                  // can reflect this spell?
        bool m_autoRepeat;
        bool m_isNeedSendToClient;
        bool m_isCastTimeHidden;


        uint8 m_delayAtDamageCount;
        int32 GetNextDelayAtDamageMsTime()
        {
            return m_delayAtDamageCount < 5 ? IN_MILLISECONDS - (m_delayAtDamageCount++) * 200 : 200;
        }

        // Delayed spells system
        uint64 m_delayStart;                                // time of spell delay start, filled by event handler, zero = just started
        uint64 m_delayMoment;                               // moment of next delay call, used internally
        bool m_immediateHandled;                            // were immediate actions handled? (used by delayed spells only)

        // These vars are used in both delayed spell system and modified immediate spell system
        bool m_referencedFromCurrentSpell;                  // mark as references to prevent deleted and access by dead pointers
        bool m_executedCurrently;                           // mark as executed to prevent deleted and access by dead pointers
        bool m_needSpellLog;                                // need to send spell log?
        bool m_needComboPoints;                             // Need combo points?
        uint8 m_applyMultiplierMask;                        // by effect: damage multiplier needed?
        float m_damageMultipliers[3];                       // by effect: damage multiplier

        // Current targets, to be used in SpellEffects (MUST BE USED ONLY IN SPELL EFFECTS)
        Unit* unitTarget;
        Item* itemTarget;
        GameObject* gameObjTarget;
        int32 damage;

        // this is set in Spell Hit, but used in Apply Aura handler
        DiminishingLevels m_diminishLevel;
        DiminishingGroup m_diminishGroup;

        // -------------------------------------------
        GameObject* focusObject;

        // Damage and healing in effects need just calculate
        int32 m_damage;           // Damge   in effects count here
        int32 m_healing;          // Healing in effects count here
        int32 m_healthLeech;      // Health leech in effects for all targets count here

        //******************************************
        // Spell trigger system
        //******************************************
        bool   m_canTrigger;                  // Can start trigger (m_IsTriggeredSpell can`t use for this)
        uint32 m_procAttacker;                // Attacker trigger flags
        uint32 m_procVictim;                  // Victim   trigger flags
        void   prepareDataForTriggerSystem();

        //*****************************************
        // Spell target subsystem
        //*****************************************
        // Targets store structures and data
        uint32 m_countOfHit;
        uint32 m_countOfMiss;
        struct TargetInfo
        {
            uint64 targetGUID;
            uint64 timeDelay;
            SpellMissInfo missCondition: 8;
            SpellMissInfo reflectResult: 8;
            uint8  effectMask: 8;
            bool   processed: 1;
            bool   deleted: 1;
            int32  damage;
            bool   crit;
        };
        std::list<TargetInfo> m_UniqueTargetInfo;
        uint8 m_needAliveTargetMask;                        // Mask req. alive targets
        bool m_destroyed;

        struct GOTargetInfo
        {
            uint64 targetGUID;
            uint64 timeDelay;
            uint8  effectMask: 8;
            bool   processed: 1;
            bool   deleted: 1;
        };
        std::list<GOTargetInfo> m_UniqueGOTargetInfo;

        struct ItemTargetInfo
        {
            Item*  item;
            uint8 effectMask;
        };
        std::list<ItemTargetInfo> m_UniqueItemInfo;

        void AddUnitTarget(Unit* target, uint32 effIndex);
        void AddUnitTarget(uint64 unitGUID, uint32 effIndex);
        void AddGOTarget(GameObject* target, uint32 effIndex);
        void AddGOTarget(uint64 goGUID, uint32 effIndex);
        void AddItemTarget(Item* target, uint32 effIndex);
        void DoAllEffectOnTarget(TargetInfo* target);
        void DoSpellHitOnUnit(Unit* unit, uint32 effectMask);
        void DoAllEffectOnTarget(GOTargetInfo* target);
        void DoAllEffectOnTarget(ItemTargetInfo* target);
        bool IsAliveUnitPresentInTargetList();
        void SearchAreaTarget(std::list<Unit*>& unitList, float radius, const uint32 type, SpellTargets TargetType, uint32 entry = 0);
        void SearchChainTarget(std::list<Unit*>& unitList, float radius, uint32 unMaxTargets, SpellTargets TargetType);
        WorldObject* SearchNearbyTarget(float range, SpellTargets TargetType);
        bool IsValidSingleTargetEffect(Unit const* target, Targets type) const;
        bool IsValidSingleTargetSpell(Unit const* target) const;
        void CalculateDamageDoneForAllTargets();
        int32 CalculateDamageDone(Unit* unit, const uint32 effectMask, float* multiplier);
        void SpellDamageSchoolDmg(SpellEffIndex effIndex);
        void SpellDamageWeaponDmg(SpellEffIndex effIndex);
        void SpellDamageHeal(SpellEffIndex effIndex);

        void GetSummonPosition(uint32 i, Position& pos, float radius = 0.0f);
        void SummonGuardian(uint32 i, uint32 entry, SummonPropertiesEntry const* properties);

        SpellCastResult CanOpenLock(uint32 effIndex, uint32 lockid, SkillType& skillid, int32& reqSkillValue, int32& skillValue);
        // -------------------------------------------

        //List For Triggered Spells
        typedef std::vector<SpellEntry const*> TriggerSpells;
        TriggerSpells m_TriggerSpells;
        typedef std::vector< std::pair<SpellEntry const*, int32> > ChanceTriggerSpells;
        ChanceTriggerSpells m_ChanceTriggerSpells;

        uint32 m_spellState;
        int32 m_timer;

        float m_castPositionX;
        float m_castPositionY;
        float m_castPositionZ;
        float m_castOrientation;
        bool m_IsTriggeredSpell;

        // if need this can be replaced by Aura copy
        // we can't store original aura link to prevent access to deleted auras
        // and in same time need aura data and after aura deleting.
        SpellEntry const* m_triggeredByAuraSpell;

        uint32 m_customAttr;
        bool m_skipCheck;
};

namespace Oregon
{
struct SpellNotifierCreatureAndPlayer
{
    std::list<Unit*>* i_data;
    Spell& i_spell;
    const uint32& i_push_type;
    float i_radius, i_radiusSq;
    SpellTargets i_TargetType;
    Unit* i_caster;
    uint32 i_entry;
    const Position* const i_pos;
    SpellEntry const* i_spellProto;

    SpellNotifierCreatureAndPlayer(Spell& spell, std::list<Unit*>& data, float radius, const uint32& type,
                                   SpellTargets TargetType = SPELL_TARGETS_ENEMY, const Position* pos = NULL, uint32 entry = 0, SpellEntry const* spellProto = NULL)
        : i_data(&data), i_spell(spell), i_push_type(type), i_radius(radius), i_radiusSq(radius* radius),
          i_TargetType(TargetType), i_caster(spell.GetCaster()), i_entry(entry), i_pos(pos), i_spellProto(spellProto)
    {
    }

    template<class T> inline void Visit(GridRefManager<T>&  m)
    {
        assert(i_data);

        if (!i_caster)
            return;

        for (typename GridRefManager<T>::iterator itr = m.begin(); itr != m.end(); ++itr)
        {
            if (!itr->GetSource()->IsAlive() || (itr->GetSource()->GetTypeId() == TYPEID_PLAYER && ((Player*)itr->GetSource())->IsInFlight()))
                continue;

            switch (i_TargetType)
            {
            case SPELL_TARGETS_ALLY:
                if (!i_caster->_IsValidAssistTarget(itr->GetSource(), i_spellProto))
                    continue;
                if (itr->GetSource()->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
                    continue;
                if (itr->GetSource()->GetTypeId() == TYPEID_PLAYER && itr->GetSource()->ToPlayer()->IsGameMaster())
                    continue;
                break;
            case SPELL_TARGETS_ENEMY:
                {
                    if (itr->GetSource()->GetTypeId() == TYPEID_UNIT && ((Creature*)itr->GetSource())->IsTotem())
                        continue;

                    if (i_caster->GetCreatureType() == CREATURE_TYPE_TOTEM)
                    {
                        if (!itr->GetSource()->isAttackableByAOE(i_pos->GetPositionX(), i_pos->GetPositionY(), i_pos->GetPositionZ(), true))
                            continue;
                    }
                    else
                    {
                        if (!itr->GetSource()->isAttackableByAOE())
                            continue;
                    }

                    if (!i_caster->_IsValidAttackTarget(itr->GetSource(), i_spellProto))
                        continue;

                }
                break;
            case SPELL_TARGETS_ENTRY:
                {
                    if (itr->GetSource()->GetEntry() != i_entry)
                        continue;
                }
                break;
            default:
                continue;
            }

            switch (i_push_type)
            {
            case PUSH_IN_FRONT:
                if (i_caster->isInFrontInMap((Unit*)(itr->GetSource()), i_radius, float(M_PI) / 3))
                    i_data->push_back(itr->GetSource());
                break;
            case PUSH_IN_BACK:
                if (i_caster->isInBackInMap((Unit*)(itr->GetSource()), i_radius, float(M_PI) / 3))
                    i_data->push_back(itr->GetSource());
                break;
            case PUSH_IN_LINE:
                if (i_caster->HasInLine((Unit*)(itr->GetSource()), i_caster->GetObjectSize()))
                    i_data->push_back(itr->GetSource());
                break;
            default:
                if (i_TargetType != SPELL_TARGETS_ENTRY && i_push_type == PUSH_SRC_CENTER && i_caster) // if caster then check distance from caster to target (because of model collision)
                {
                    if (i_caster->IsWithinDistInMap(itr->GetSource(), i_radius))
                        i_data->push_back(itr->GetSource());
                }
                else
                {
                    if ((itr->GetSource()->GetExactDistSq(i_pos) < i_radiusSq))
                        i_data->push_back(itr->GetSource());
                }
                break;
            }
        }
    }

    #ifdef WIN32
    template<> inline void Visit(CorpseMapType&) {}
    template<> inline void Visit(GameObjectMapType&) {}
    template<> inline void Visit(DynamicObjectMapType&) {}
    #endif
};

#ifndef WIN32
template<> inline void SpellNotifierCreatureAndPlayer::Visit(CorpseMapType&) {}
template<> inline void SpellNotifierCreatureAndPlayer::Visit(GameObjectMapType&) {}
template<> inline void SpellNotifierCreatureAndPlayer::Visit(DynamicObjectMapType&) {}
#endif
}

typedef void(Spell::*pEffect)(SpellEffIndex effIndex);

class SpellEvent : public BasicEvent
{
    public:
        SpellEvent(Spell* spell);
        virtual ~SpellEvent();

        virtual bool Execute(uint64 e_time, uint32 p_time);
        virtual void Abort(uint64 e_time);
        virtual bool IsDeletable() const;
    protected:
        Spell* m_Spell;
};
#endif

