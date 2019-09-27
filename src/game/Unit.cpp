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
#include "Log.h"
#include "Opcodes.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "World.h"
#include "ObjectMgr.h"
#include "SpellMgr.h"
#include "Unit.h"
#include "QuestDef.h"
#include "Player.h"
#include "Creature.h"
#include "Spell.h"
#include "Group.h"
#include "SpellAuras.h"
#include "ObjectAccessor.h"
#include "CreatureAI.h"
#include "Formulas.h"
#include "Pet.h"
#include "Utilities/Util.h"
#include "Totem.h"
#include "Battleground.h"
#include "OutdoorPvP.h"
#include "InstanceSaveMgr.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"
#include "CreatureGroups.h"
#include "PetAI.h"
#include "PassiveAI.h"
#include "TemporarySummon.h"
#include "PathFinder.h"
#include "ScriptMgr.h"
#include "Object.h"
#include "MovementGenerator.h"
#include "MoveSplineInit.h"
#include "MoveSpline.h"

#include <math.h>
#include <array>

float baseMoveSpeed[MAX_MOVE_TYPE] =
{
    2.5f,                  // MOVE_WALK
    7.0f,                  // MOVE_RUN
    4.5f,                  // MOVE_RUN_BACK
    4.722222f,             // MOVE_SWIM
    2.5f,                  // MOVE_SWIM_BACK
    3.141594f,             // MOVE_TURN_RATE
    7.0f,                  // MOVE_FLIGHT
    4.5f,                  // MOVE_FLIGHT_BACK
};

// Used for preapre can/can't trigger aura
void InitTriggerAuraData();

// auraTypes contains attacker auras capable of proc'ing cast auras
static Unit::AuraTypeSet GenerateAttakerProcCastAuraTypes()
{
    static Unit::AuraTypeSet auraTypes;
    auraTypes.insert(SPELL_AURA_DUMMY);
    auraTypes.insert(SPELL_AURA_PROC_TRIGGER_SPELL);
    auraTypes.insert(SPELL_AURA_MOD_HASTE);
    auraTypes.insert(SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
    return auraTypes;
}

// auraTypes contains victim auras capable of proc'ing cast auras
static Unit::AuraTypeSet GenerateVictimProcCastAuraTypes()
{
    static Unit::AuraTypeSet auraTypes;
    auraTypes.insert(SPELL_AURA_DUMMY);
    auraTypes.insert(SPELL_AURA_PRAYER_OF_MENDING);
    auraTypes.insert(SPELL_AURA_PROC_TRIGGER_SPELL);
    return auraTypes;
}

// auraTypes contains auras capable of proc effect/damage (but not cast) for attacker
static Unit::AuraTypeSet GenerateAttakerProcEffectAuraTypes()
{
    static Unit::AuraTypeSet auraTypes;
    auraTypes.insert(SPELL_AURA_MOD_DAMAGE_DONE);
    auraTypes.insert(SPELL_AURA_PROC_TRIGGER_DAMAGE);
    auraTypes.insert(SPELL_AURA_MOD_CASTING_SPEED);
    auraTypes.insert(SPELL_AURA_MOD_RATING);
    return auraTypes;
}

// auraTypes contains auras capable of proc effect/damage (but not cast) for victim
static Unit::AuraTypeSet GenerateVictimProcEffectAuraTypes()
{
    static Unit::AuraTypeSet auraTypes;
    auraTypes.insert(SPELL_AURA_MOD_RESISTANCE);
    auraTypes.insert(SPELL_AURA_PROC_TRIGGER_DAMAGE);
    auraTypes.insert(SPELL_AURA_MOD_PARRY_PERCENT);
    auraTypes.insert(SPELL_AURA_MOD_BLOCK_PERCENT);
    auraTypes.insert(SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN);
    return auraTypes;
}

static Unit::AuraTypeSet attackerProcCastAuraTypes = GenerateAttakerProcCastAuraTypes();
static Unit::AuraTypeSet attackerProcEffectAuraTypes = GenerateAttakerProcEffectAuraTypes();

static Unit::AuraTypeSet victimProcCastAuraTypes = GenerateVictimProcCastAuraTypes();
static Unit::AuraTypeSet victimProcEffectAuraTypes   = GenerateVictimProcEffectAuraTypes();

// auraTypes contains auras capable of proc'ing for attacker and victim
static Unit::AuraTypeSet GenerateProcAuraTypes()
{
    InitTriggerAuraData();

    Unit::AuraTypeSet auraTypes;
    auraTypes.insert(attackerProcCastAuraTypes.begin(), attackerProcCastAuraTypes.end());
    auraTypes.insert(attackerProcEffectAuraTypes.begin(), attackerProcEffectAuraTypes.end());
    auraTypes.insert(victimProcCastAuraTypes.begin(), victimProcCastAuraTypes.end());
    auraTypes.insert(victimProcEffectAuraTypes.begin(), victimProcEffectAuraTypes.end());
    return auraTypes;
}

static Unit::AuraTypeSet procAuraTypes = GenerateProcAuraTypes();

bool IsPassiveStackableSpell(uint32 spellId)
{
    if (!IsPassiveSpell(spellId))
        return false;

    SpellEntry const* spellProto = sSpellStore.LookupEntry(spellId);
    if (!spellProto)
        return false;

    for (int j = 0; j < MAX_SPELL_EFFECTS; ++j)
    {
        if (std::find(procAuraTypes.begin(), procAuraTypes.end(), spellProto->EffectApplyAuraName[j]) != procAuraTypes.end())
            return false;
    }

    return true;
}

typedef std::array<uint32, NUM_SPELL_PARTIAL_RESISTS> SpellPartialResistChanceEntry;
typedef std::vector<SpellPartialResistChanceEntry> SpellPartialResistDistribution;
static inline SpellPartialResistDistribution InitSpellPartialResistDistribution()
{
    // Precalculated chances for 0-100% mitigation
    // We use integer random instead of floats, so each chance is premultiplied by 100 (100.00 becomes 10000)
    const SpellPartialResistDistribution precalculated = {
        {{10000,0,0,0,0}},
        {{9700,200,100,0,0}},
        {{9400,400,200,0,0}},
        {{9000,800,200,0,0}},
        {{8700,1000,300,0,0}},
        {{8400,1200,400,0,0}},
        {{8200,1300,400,100,0}},
        {{7900,1500,500,100,0}},
        {{7600,1700,600,100,0}},
        {{7300,1900,700,100,0}},
        {{6900,2300,700,100,0}},
        {{6600,2500,800,100,0}},
        {{6300,2700,900,100,0}},
        {{6000,2900,1000,100,0}},
        {{5800,3000,1000,200,0}},
        {{5400,3300,1100,200,0}},
        {{5100,3600,1100,200,0}},
        {{4800,3800,1200,200,0}},
        {{4400,4200,1200,200,0}},
        {{4100,4400,1300,200,0}},
        {{3700,4800,1300,200,0}},
        {{3400,5000,1400,200,0}},
        {{3100,5200,1500,200,0}},
        {{3000,5200,1500,200,100}},
        {{2800,5300,1500,300,100}},
        {{2500,5500,1600,300,100}},
        {{2400,5400,1700,400,100}},
        {{2300,5300,1800,500,100}},
        {{2200,5100,2100,500,100}},
        {{2100,5000,2200,600,100}},
        {{2000,4900,2400,600,100}},
        {{1900,4700,2600,700,100}},
        {{1800,4600,2700,800,100}},
        {{1700,4400,3000,800,100}},
        {{1600,4300,3100,900,100}},
        {{1500,4200,3200,1000,100}},
        {{1400,4100,3300,1100,100}},
        {{1300,3900,3600,1100,100}},
        {{1300,3600,3800,1200,100}},
        {{1200,3500,3900,1300,100}},
        {{1100,3400,4000,1400,100}},
        {{1000,3300,4100,1500,100}},
        {{900,3100,4400,1500,100}},
        {{800,3000,4500,1600,100}},
        {{800,2700,4700,1700,100}},
        {{700,2600,4800,1800,100}},
        {{600,2500,4900,1900,100}},
        {{600,2300,5000,1900,200}},
        {{500,2200,5100,2000,200}},
        {{300,2200,5300,2000,200}},
        {{200,2100,5400,2100,200}},
        {{200,2000,5300,2200,300}},
        {{200,2000,5100,2200,500}},
        {{200,1900,5000,2300,600}},
        {{100,1900,4900,2500,600}},
        {{100,1800,4800,2600,700}},
        {{100,1700,4700,2700,800}},
        {{100,1600,4500,3000,800}},
        {{100,1500,4400,3100,900}},
        {{100,1500,4100,3300,1000}},
        {{100,1400,4000,3400,1100}},
        {{100,1300,3900,3500,1200}},
        {{100,1200,3800,3600,1300}},
        {{100,1100,3600,3900,1300}},
        {{100,1100,3300,4100,1400}},
        {{100,1000,3200,4200,1500}},
        {{100,900,3100,4300,1600}},
        {{100,800,3000,4400,1700}},
        {{100,800,2700,4600,1800}},
        {{100,700,2600,4700,1900}},
        {{100,600,2400,4900,2000}},
        {{100,600,2200,5000,2100}},
        {{100,500,2100,5100,2200}},
        {{100,500,1800,5300,2300}},
        {{100,400,1700,5400,2400}},
        {{100,300,1600,5500,2500}},
        {{100,300,1500,5300,2800}},
        {{100,200,1500,5200,3000}},
        {{0,200,1500,5200,3100}},
        {{0,200,1400,5000,3400}},
        {{0,200,1300,4800,3700}},
        {{0,200,1300,4400,4100}},
        {{0,200,1200,4200,4400}},
        {{0,200,1200,3800,4800}},
        {{0,200,1100,3600,5100}},
        {{0,200,1100,3300,5400}},
        {{0,200,1000,3000,5800}},
        {{0,100,1000,2900,6000}},
        {{0,100,900,2700,6300}},
        {{0,100,800,2500,6600}},
        {{0,100,700,2300,6900}},
        {{0,100,700,1900,7300}},
        {{0,100,600,1700,7600}},
        {{0,100,500,1500,7900}},
        {{0,100,400,1300,8200}},
        {{0,0,400,1200,8400}},
        {{0,0,300,1000,8700}},
        {{0,0,200,800,9000}},
        {{0,0,200,400,9400}},
        {{0,0,100,200,9700}},
        {{0,0,0,0,10000}},
    };
    // Inflate up to two decimal places of chance %: add intermediate values
    SpellPartialResistDistribution inflated;
    for (size_t index = 0; index < precalculated.size(); ++index)
    {
        for (uint8 intermediate = 0; intermediate < 100; ++intermediate)
        {
            // Check if this is the last one first
            if ((index + 1) == precalculated.size())
            {
                inflated.push_back(precalculated[index]);
                break;
            }
            SpellPartialResistChanceEntry values;
            for (uint8 column = SPELL_PARTIAL_RESIST_NONE; column < NUM_SPELL_PARTIAL_RESISTS; ++column)
            {
                const uint32 base = precalculated.at(index).at(column);
                const uint32 next = precalculated.at(index + 1).at(column);
                values[column] = base + ((next - base) * intermediate / 100);
            }
            inflated.push_back(values);
        }
    }
    return inflated;
}

static const SpellPartialResistDistribution SPELL_PARTIAL_RESIST_DISTRIBUTION = InitSpellPartialResistDistribution();

void MovementInfo::Read(ByteBuffer& data)
{
    data >> moveFlags;
    data >> moveFlags2;
    data >> time;
    data >> pos.m_positionX;
    data >> pos.m_positionY;
    data >> pos.m_positionZ;
    pos.SetOrientation(data.read<float>());

    if (HasMovementFlag(MOVEMENTFLAG_ONTRANSPORT))
    {
        data >> t_guid;
        data >> t_pos.m_positionX;
        data >> t_pos.m_positionY;
        data >> t_pos.m_positionZ;
        t_pos.SetOrientation(data.read<float>());
        data >> t_time;
    }
    if (HasMovementFlag(MovementFlags(MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_FLYING2)))
        data >> s_pitch;

    data >> fallTime;

    if (HasMovementFlag(MOVEMENTFLAG_FALLING))
    {
        data >> j_velocity;
        data >> j_sinAngle;
        data >> j_cosAngle;
        data >> j_xyspeed;
    }

    if (HasMovementFlag(MOVEMENTFLAG_SPLINE_ELEVATION))
        data >> u_unk1;
}

void MovementInfo::Write(ByteBuffer& data) const
{
    data << moveFlags;
    data << moveFlags2;
    data << time;
    data << pos.GetPositionX();
    data << pos.GetPositionY();
    data << pos.GetPositionZ();
    data << pos.GetOrientation();

    if (HasMovementFlag(MOVEMENTFLAG_ONTRANSPORT))
    {
        data << t_guid;
        data << t_pos.GetPositionX();
        data << t_pos.GetPositionY();
        data << t_pos.GetPositionZ();
        data << t_pos.GetOrientation();
        data << t_time;
    }
    if (HasMovementFlag(MovementFlags(MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_FLYING2)))
        data << s_pitch;

    data << fallTime;

    if (HasMovementFlag(MOVEMENTFLAG_FALLING))
    {
        data << j_velocity;
        data << j_sinAngle;
        data << j_cosAngle;
        data << j_xyspeed;
    }

    if (HasMovementFlag(MOVEMENTFLAG_SPLINE_ELEVATION))
        data << u_unk1;
}

Unit::Unit(bool isWorldObject):
      WorldObject(isWorldObject), IsAIEnabled(false),
      NeedChangeAI(false),
      LastCharmerGUID(0),
      m_ControlledByPlayer(false),
      i_AI(NULL),
      i_disabledAI(NULL),
      m_removedAurasCount(0),
      i_motionMaster(this),
      m_ThreatManager(this),
      m_HostileRefManager(this),
      m_lastSanctuaryTime(0),
      m_procDeep(0),
      movespline(new Movement::MoveSpline()),
      m_movesplineTimer(POSITION_UPDATE_DELAY),
      _lastDamagedTime(0)
{
    m_objectType |= TYPEMASK_UNIT;
    m_objectTypeId = TYPEID_UNIT;

    m_updateFlag = (UPDATEFLAG_HIGHGUID | UPDATEFLAG_LIVING | UPDATEFLAG_HAS_POSITION);

    m_attackTimer[BASE_ATTACK] = 0;
    m_attackTimer[OFF_ATTACK] = 0;
    m_attackTimer[RANGED_ATTACK] = 0;
    m_modAttackSpeedPct[BASE_ATTACK] = 1.0f;
    m_modAttackSpeedPct[OFF_ATTACK] = 1.0f;
    m_modAttackSpeedPct[RANGED_ATTACK] = 1.0f;

    m_extraAttacks = 0;
    m_canDualWield = false;

    m_state = 0;
    m_form = FORM_NONE;
    m_deathState = ALIVE;

    for (uint8 i = 0; i < CURRENT_MAX_SPELL; ++i)
        m_currentSpells[i] = NULL;

    for (uint8 i = 0; i < MAX_SUMMON_SLOT; ++i)
        m_SummonSlot[i] = 0;

    m_ObjectSlot[0] = m_ObjectSlot[1] = m_ObjectSlot[2] = m_ObjectSlot[3] = 0;

    m_AurasUpdateIterator = m_Auras.end();
    m_AuraFlags = 0;

    m_interruptMask = 0;
    m_transform = 0;
    m_ShapeShiftFormSpellId = 0;
    m_canModifyStats = false;

    for (uint8 i = 0; i < MAX_SPELL_IMMUNITY; ++i)
        m_spellImmune[i].clear();

    for (uint8 i = 0; i < UNIT_MOD_END; ++i)
    {
        m_auraModifiersGroup[i][BASE_VALUE] = 0.0f;
        m_auraModifiersGroup[i][BASE_PCT] = 1.0f;
        m_auraModifiersGroup[i][TOTAL_VALUE] = 0.0f;
        m_auraModifiersGroup[i][TOTAL_PCT] = 1.0f;
    }
    // implement 50% base damage from offhand
    m_auraModifiersGroup[UNIT_MOD_DAMAGE_OFFHAND][TOTAL_PCT] = 0.5f;

    for (uint8 i = 0; i < MAX_ATTACK; ++i)
    {
        m_weaponDamage[i][MINDAMAGE] = BASE_MINDAMAGE;
        m_weaponDamage[i][MAXDAMAGE] = BASE_MAXDAMAGE;
    }

    for (uint8 i = 0; i < MAX_STATS; ++i)
        m_createStats[i] = 0.0f;

    m_attacking = NULL;
    m_modMeleeHitChance = 0.0f;
    m_modRangedHitChance = 0.0f;
    m_modSpellHitChance = 0.0f;
    m_baseSpellCritChance = 5;

    m_initiatingCombat = false;

    m_CombatTimer = 0;
    m_lastManaUse = 0;

    for (uint8 i = 0; i < MAX_SPELL_SCHOOL; ++i)
        m_threatModifier[i] = 1.0f;

    m_isSorted = true;

    for (uint8 i = 0; i < MAX_MOVE_TYPE; ++i)
        m_speed_rate[i] = 1.0f;

    m_unitTypeMask = UNIT_MASK_NONE;

    m_charmInfo = NULL;
    m_reducedThreatPercent = 0;
    m_misdirectionTargetGUID = 0;

    // remove aurastates allowing special moves
    for (uint8 i = 0; i < MAX_REACTIVE; ++i)
        m_reactiveTimer[i] = 0;

    m_duringRemoveFromWorld = false;

    m_baseSpeedWalk = 0.f;
    m_baseSpeedRun = 0.f;

    _oldFactionId = 0;

    m_serverSideVisibility.SetValue(SERVERSIDE_VISIBILITY_GHOST, GHOST_VISIBILITY_ALIVE);
}

Unit::~Unit()
{
    // set current spells as deletable
    for (uint8 i = 0; i < CURRENT_MAX_SPELL; ++i)
        if (m_currentSpells[i])
        {
            m_currentSpells[i]->SetReferencedFromCurrent(false);
            m_currentSpells[i] = NULL;
        }
    
    RemoveAllGameObjects();
    RemoveAllDynObjects();
    _DeleteAuras();

    delete m_charmInfo;
    delete movespline;

    ASSERT(!m_duringRemoveFromWorld);
    ASSERT(!m_attacking);
    ASSERT(m_attackers.empty());
    ASSERT(m_sharedVision.empty());
}

// Check if unit in combat with specific unit
bool Unit::IsInCombatWith(Unit const* who) const
{
    // Check target exists
    if (!who)
        return false;

    // Search in threat list
    uint64 guid = who->GetGUID();
    ThreatContainer::StorageType threatList = m_ThreatManager.getThreatList();
    for (ThreatContainer::StorageType::const_iterator i = threatList.begin(); i != threatList.end(); ++i)
    {
        HostileReference* ref = (*i);

        // Return true if the unit matches
        if (ref && ref->getUnitGuid() == guid)
            return true;
    }

    // Nothing found, false.
    return false;
}

void Unit::Update(uint32 p_time)
{
    // WARNING! Order of execution here is important, do not change.
    // Spells must be processed with event system BEFORE they go to _UpdateSpells.
    // Or else we may have some SPELL_STATE_FINISHED spells stalled in pointers, that is bad.
    m_Events.Update(p_time);

    if (!IsInWorld())
        return;

    _UpdateSpells(p_time);

    if (uint32 base_att = getAttackTimer(BASE_ATTACK))
        setAttackTimer(BASE_ATTACK, (p_time >= base_att ? 0 : base_att - p_time));

    if (uint32 base_att = getAttackTimer(OFF_ATTACK))
        setAttackTimer(OFF_ATTACK, (p_time >= base_att ? 0 : base_att - p_time));

    if (uint32 ranged_att = getAttackTimer(RANGED_ATTACK))
        setAttackTimer(RANGED_ATTACK, (p_time >= ranged_att ? 0 : ranged_att - p_time));

    // update abilities available only for fraction of time
    UpdateReactives(p_time);

    if (IsAlive())
    {
        ModifyAuraState(AURA_STATE_HEALTHLESS_20_PERCENT, HealthBelowPct(20));
        ModifyAuraState(AURA_STATE_HEALTHLESS_35_PERCENT, HealthBelowPct(35));
    }

    UpdateSplineMovement(p_time);
    i_motionMaster.UpdateMotion(p_time);

    if (IsInCombat() && (GetTypeId() == TYPEID_PLAYER || (IsPet() && IsControlledByPlayer())))
    {
        if (m_HostileRefManager.isEmpty())
            if (m_CombatTimer <= p_time)
                ClearInCombat();
            else
                m_CombatTimer -= p_time;
    }

}

bool Unit::UpdateMeleeAttackingState()
{
	Unit *victim = GetVictim();
	if (!victim || IsNonMeleeSpellCast(false))
		return false;

	if (!isAttackReady(BASE_ATTACK) && !(isAttackReady(OFF_ATTACK) && haveOffhandWeapon()))
		return false;

	uint8 swingError = 0;
	if (!IsWithinMeleeRange(victim))
	{
		setAttackTimer(BASE_ATTACK, 100);
		setAttackTimer(OFF_ATTACK, 100);
		swingError = 1;
	}
	//120 degrees of radiant range
	else if (!HasInArc(2 * M_PI / 3, victim))
	{
		setAttackTimer(BASE_ATTACK, 100);
		setAttackTimer(OFF_ATTACK, 100);
		swingError = 2;
	}
	else
	{
		if (isAttackReady(BASE_ATTACK))
		{
			// prevent base and off attack in same time, delay attack at 0.2 sec
			if (haveOffhandWeapon())
			{
				if (getAttackTimer(OFF_ATTACK) < ATTACK_DISPLAY_DELAY)
					setAttackTimer(OFF_ATTACK, ATTACK_DISPLAY_DELAY);
			}
			AttackerStateUpdate(victim, BASE_ATTACK);
			resetAttackTimer(BASE_ATTACK);
		}
		if (haveOffhandWeapon() && isAttackReady(OFF_ATTACK))
		{
			// prevent base and off attack in same time, delay attack at 0.2 sec
			uint32 base_att = getAttackTimer(BASE_ATTACK);
			if (base_att < ATTACK_DISPLAY_DELAY)
				setAttackTimer(BASE_ATTACK, ATTACK_DISPLAY_DELAY);
			// do attack
			AttackerStateUpdate(victim, OFF_ATTACK);
			resetAttackTimer(OFF_ATTACK);
		}
	}

	Player* player = (GetTypeId() == TYPEID_PLAYER ? (Player*)this : NULL);
	if (player && swingError != player->LastSwingErrorMsg())
	{
		if (swingError == 1)
			player->SendAttackSwingNotInRange();
		else if (swingError == 2)
			player->SendAttackSwingBadFacingAttack();
		player->SwingErrorMsg(swingError);
	}

	return swingError == 0;
}

bool Unit::haveOffhandWeapon() const
{
	if (GetTypeId() == TYPEID_PLAYER)
		return ((Player*)this)->GetWeaponForAttack(OFF_ATTACK, true);
	else
	{
		uint8 itemClass = GetByteValue(UNIT_VIRTUAL_ITEM_INFO + (1 * 2) + 0, VIRTUAL_ITEM_INFO_0_OFFSET_CLASS);
		if (itemClass == ITEM_CLASS_WEAPON)
			return true;

		return false;
	}

    return CanDualWield();
}

void Unit::resetAttackTimer(WeaponAttackType type)
{
    m_attackTimer[type] = uint32(GetAttackTime(type) * m_modAttackSpeedPct[type]);
}

bool Unit::IsWithinCombatRange(const Unit* obj, float dist2compare) const
{
    if (!obj || !IsInMap(obj))
        return false;

    float dx = GetPositionX() - obj->GetPositionX();
    float dy = GetPositionY() - obj->GetPositionY();
    float dz = GetPositionZ() - obj->GetPositionZ();
    float distsq = dx * dx + dy * dy + dz * dz;

    float sizefactor = GetCombatReach() + obj->GetCombatReach();
    float maxdist = dist2compare + sizefactor;

    return distsq < maxdist * maxdist;
}

bool Unit::IsWithinMeleeRange(Unit* obj, float dist) const
{
    if (!obj || !IsInMap(obj))
        return false;

    float dx = GetPositionX() - obj->GetPositionX();
    float dy = GetPositionY() - obj->GetPositionY();
    float dz = GetPositionZ() - obj->GetPositionZ();
    float distsq = dx * dx + dy * dy + dz * dz;

    float sizefactor = GetMeleeReach() + obj->GetMeleeReach();
    float maxdist = dist + sizefactor;

    return distsq < maxdist * maxdist;
}

bool Unit::HasBreakableByDamageAuraType(AuraType type, uint32 excludeAura) const
{
    AuraList const& auras = GetAurasByType(type);
    for (AuraList::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
        if ((!excludeAura || excludeAura != (*itr)->GetSpellProto()->Id) &&
            ((*itr)->GetSpellProto()->AuraInterruptFlags & AURA_INTERRUPT_FLAG_DAMAGE))
            return true;
    return false;
}

bool Unit::HasBreakableByDamageCrowdControlAura(Unit* excludeCasterChannel) const
{
    uint32 excludeAura = 0;
    if (Spell* currentChanneledSpell = excludeCasterChannel ? excludeCasterChannel->GetCurrentSpell(CURRENT_CHANNELED_SPELL) : NULL)
        excludeAura = currentChanneledSpell->m_spellInfo->Id;

    return (HasBreakableByDamageAuraType(SPELL_AURA_MOD_CONFUSE, excludeAura)
        || HasBreakableByDamageAuraType(SPELL_AURA_MOD_FEAR, excludeAura)
        || HasBreakableByDamageAuraType(SPELL_AURA_MOD_STUN, excludeAura)
        || HasBreakableByDamageAuraType(SPELL_AURA_MOD_ROOT, excludeAura)
        || HasBreakableByDamageAuraType(SPELL_AURA_TRANSFORM, excludeAura));
}


void Unit::GetRandomContactPoint(const Unit* obj, float& x, float& y, float& z, float distance2dMin, float distance2dMax) const
{
    float combat_reach = GetCombatReach();
    if (combat_reach < 0.1) // sometimes bugged for players
    {
        //sLog.outError("Unit %u (Type: %u) has invalid combat_reach %f",GetGUIDLow(),GetTypeId(),combat_reach);
        //if (GetTypeId() == TYPEID_UNIT)
        //    sLog.outError("Creature entry %u has invalid combat_reach", ToCreature()->GetEntry());
        combat_reach = DEFAULT_COMBAT_REACH;
    }
    uint32 attacker_number = getAttackers().size();
    if (attacker_number > 0)
        --attacker_number;
    GetNearPoint(obj, x, y, z, obj->GetCombatReach(), distance2dMin + (distance2dMax - distance2dMin) * (float)rand_norm()
        , GetAngle(obj) + (attacker_number ? (static_cast<float>(M_PI/2) - static_cast<float>(M_PI) * (float)rand_norm()) * float(attacker_number) / combat_reach * 0.3f : 0));
}

void Unit::RemoveMovementImpairingAuras()
{
    for (AuraMap::iterator iter = m_Auras.begin(); iter != m_Auras.end();)
    {
        if (sSpellMgr.GetSpellCustomAttr(iter->second->GetId()) & SPELL_ATTR_CU_MOVEMENT_IMPAIR)
            RemoveAura(iter);
        else
            ++iter;
    }
}

void Unit::RemoveSpellsCausingAura(AuraType auraType)
{
    if (auraType >= TOTAL_AURAS)
        return;

    AuraList::iterator iter, next;
    for (iter = m_modAuras[auraType].begin(); iter != m_modAuras[auraType].end(); iter = next)
    {
        next = iter;
        ++next;

        if (*iter)
        {
            RemoveAurasDueToSpell((*iter)->GetId());
            if (!m_modAuras[auraType].empty())
                next = m_modAuras[auraType].begin();
            else
                return;
        }
    }
}

void Unit::RemoveAuraTypeByCaster(AuraType auraType, uint64 casterGUID)
{
    if (auraType >= TOTAL_AURAS) return;

    for (AuraList::iterator iter = m_modAuras[auraType].begin(); iter != m_modAuras[auraType].end();)
    {
        Aura* aur = *iter;
        ++iter;

        if (aur)
        {
            uint32 removedAuras = m_removedAurasCount;
            RemoveAurasByCasterSpell(aur->GetId(), casterGUID);
            if (m_removedAurasCount > removedAuras + 1)
                iter = m_modAuras[auraType].begin();
        }
    }
}

void Unit::RemoveAurasWithInterruptFlags(uint32 flag, uint32 except)
{
    if (!(m_interruptMask & flag))
        return;

    // interrupt auras
    AuraList::iterator iter;
    for (iter = m_interruptableAuras.begin(); iter != m_interruptableAuras.end();)
    {
        Aura* aur = *iter;
        ++iter;

        //sLog.outDetail("auraflag:%u flag:%u = %u", aur->GetSpellProto()->AuraInterruptFlags,flag, aur->GetSpellProto()->AuraInterruptFlags & flag);

        if (aur && (aur->GetSpellProto()->AuraInterruptFlags & flag))
        {
            if (aur->IsInUse())
                sLog.outError("Aura %u is trying to remove itself! Flag %u. May cause crash!", aur->GetId(), flag);

            else if (!except || aur->GetId() != except)
            {
                uint32 removedAuras = m_removedAurasCount;

                RemoveAurasDueToSpell(aur->GetId());
                if (m_removedAurasCount > removedAuras + 1)
                    iter = m_interruptableAuras.begin();

            }
        }
    }

    // interrupt channeled spell
    if (Spell* spell = m_currentSpells[CURRENT_CHANNELED_SPELL])
        if (spell->getState() == SPELL_STATE_CASTING
            && (spell->m_spellInfo->ChannelInterruptFlags & flag)
            && spell->m_spellInfo->Id != except)
            InterruptNonMeleeSpells(false);

    UpdateInterruptMask();
}

void Unit::UpdateInterruptMask()
{
    m_interruptMask = 0;
    for (AuraList::iterator i = m_interruptableAuras.begin(); i != m_interruptableAuras.end(); ++i)
    {
        if (*i)
            m_interruptMask |= (*i)->GetSpellProto()->AuraInterruptFlags;
    }
    if (Spell* spell = m_currentSpells[CURRENT_CHANNELED_SPELL])
        if (spell->getState() == SPELL_STATE_CASTING)
            m_interruptMask |= spell->m_spellInfo->ChannelInterruptFlags;
}

uint32 Unit::GetAuraCount(uint32 spellId) const
{
    uint32 count = 0;
    for (AuraMap::const_iterator itr = m_Auras.lower_bound(spellEffectPair(spellId, 0)); itr != m_Auras.upper_bound(spellEffectPair(spellId, 0)); ++itr)
    {
        if (!itr->second->GetStackAmount())
            count++;
        else
            count += (uint32)itr->second->GetStackAmount();
    }

    return count;
}

/**
  * Checks whether a higher rank of aura is present on the target.
  * @param spellid Spell to be checked
  * @param effIndex Spell Effect Index
  * @returns true if target has a higher rank, false otherwise
  */
bool Unit::HasHigherRankOfAura(uint32 spellid, uint8 effIndex) const
{
    if (SpellChainNode const* curr = sSpellMgr.GetSpellChainNode(spellid))
    {
        SpellChainNode const* node = sSpellMgr.GetSpellChainNode(curr->first);
        uint32 spell = curr->first;

        while (spell)
        {
            AuraMap::const_iterator aura = m_Auras.find(spellEffectPair(spell, effIndex));
            if (aura != m_Auras.end())
                if (node->rank > curr->rank)
                    return true;

            spell = node->next;
            node = sSpellMgr.GetSpellChainNode(node->next);
        }
    }

    return false;
}

bool Unit::HasAuraType(AuraType auraType) const
{
    return (!m_modAuras[auraType].empty());
}

bool Unit::HasAuraTypeWithCaster(AuraType auratype, uint64 caster) const
{
    Unit::AuraList const& mTotalAuraList = GetAurasByType(auratype);
    for (Unit::AuraList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
        if (caster == (*i)->GetCasterGUID())
            return true;
    return false;
}

bool Unit::HasAuraTypeWithMiscvalue(AuraType auratype, int32 miscvalue) const
{
    Unit::AuraList const& mTotalAuraList = GetAurasByType(auratype);
    for (Unit::AuraList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
        if (miscvalue == (*i)->GetMiscValue())
            return true;
    return false;
}

bool Unit::HasAuraTypeWithFamilyFlags(AuraType auraType, uint32 familyName, uint64 familyFlags) const
{
    if (!HasAuraType(auraType))
        return false;
    AuraList const& auras = GetAurasByType(auraType);
    for (AuraList::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
        if (SpellEntry const* iterSpellProto = (*itr)->GetSpellProto())
            if (iterSpellProto->SpellFamilyName == familyName && iterSpellProto->SpellFamilyFlags & familyFlags)
                return true;
    return false;
}

bool Unit::HasNegativeAuraWithInterruptFlag(uint32 flag)
{
    if (!(m_interruptMask & flag))
        return false;

    // interrupt auras
    AuraList::iterator iter;
    for (iter = m_interruptableAuras.begin(); iter != m_interruptableAuras.end();)
    {
        Aura* aur = *iter;
        ++iter;
        if (!aur->IsPositive() && aur->GetSpellProto()->AuraInterruptFlags & flag)
            return true;
    }

    return false;
}


/* Called by DealDamage for auras that have a chance to be dispelled on damage taken. */
void Unit::RemoveSpellbyDamageTaken(uint32 damage, uint32 spell)
{
    // The chance to dispel an aura depends on the damage taken with respect to the casters level.
    uint32 max_dmg = getLevel() > 8 ? 30 * getLevel() - 100 : 50;
    float chance = float(damage) / max_dmg * 100.0f;

    AuraList::iterator i, next;
    for (i = m_ccAuras.begin(); i != m_ccAuras.end(); i = next)
    {
        next = i;
        ++next;

        if (*i && (!spell || (*i)->GetId() != spell) && roll_chance_f(chance))
        {
            RemoveAurasDueToSpell((*i)->GetId());
            if (!m_ccAuras.empty())
                next = m_ccAuras.begin();
            else
                return;
        }
    }
}

uint32 Unit::DealDamage(Unit* victim, uint32 damage, CleanDamage const* cleanDamage, DamageEffectType damagetype, SpellSchoolMask damageSchoolMask, SpellEntry const* spellProto, bool durabilityLoss)
{

    if ((!victim->IsAlive() || victim->IsInFlight()) || (victim->GetTypeId() == TYPEID_UNIT && victim->ToCreature()->IsInEvadeMode()))
        return 0;

    //You don't lose health from damage taken from another player while in a sanctuary
    //You still see it in the combat log though
    if (victim != this && GetTypeId() == TYPEID_PLAYER && victim->GetTypeId() == TYPEID_PLAYER)
    {
        const AreaTableEntry* area = GetAreaEntryByAreaID(victim->GetAreaId());
        if (area && area->flags & AREA_FLAG_SANCTUARY)       //sanctuary
            return 0;
    }

    //Script Event damage taken
    if (victim->IsAIEnabled)
        victim->ToCreature()->AI()->DamageTaken(this, damage);

    // Signal to pet that it dealt damage
    if (IsPet() && this != victim && victim->IsAlive())
        ToPet()->AI()->DamageDealt(victim, damage, damagetype);
    else if (IsAIEnabled)
        ToCreature()->AI()->DamageDealt(victim, damage, damagetype);

    if (victim->GetTypeId() == TYPEID_PLAYER && this != victim)
    {
        // Signal to pets that their owner was attacked - except when DOT.
        Pet* pet = victim->ToPlayer()->GetPet();

        if (pet && pet->IsAlive())
            pet->AI()->OwnerAttackedBy(this);

        if (victim->ToPlayer()->GetCommandStatus(CHEAT_GOD))
            return 0;
    }

    // Signal the pet it was attacked so the AI can respond if needed
    if (victim->GetTypeId() == TYPEID_UNIT && this != victim && victim->IsPet() && victim->IsAlive())
        victim->ToPet()->AI()->AttackedBy(this);

    if (damage || (cleanDamage && cleanDamage->damage))
    {
        if (spellProto)
        {
            if (!(spellProto->AttributesEx4 & SPELL_ATTR4_DAMAGE_DOESNT_BREAK_AURAS))
                victim->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_DAMAGE, spellProto->Id);
        }
        else
            victim->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_DAMAGE, 0);

        victim->RemoveSpellbyDamageTaken(damage, spellProto ? spellProto->Id : 0);
        // Rage from physical damage received
        if (!damage)
        {
            if ((damageSchoolMask & SPELL_SCHOOL_MASK_NORMAL) && victim->GetTypeId() == TYPEID_PLAYER && (victim->getPowerType() == POWER_RAGE))
                victim->ToPlayer()->RewardRage(cleanDamage->damage, 0, false);
            return 0;
        }
    }

    DEBUG_LOG("DealDamageStart");

    uint32 health = victim->GetHealth();
    sLog.outDetail("deal dmg:%d to health:%d ", damage, health);

    // duel ends when player has 1 or less hp
    bool duel_hasEnded = false;
    if (victim->GetTypeId() == TYPEID_PLAYER && victim->ToPlayer()->duel && damage >= (health - 1))
    {
        // prevent kill only if killed in duel and killed by opponent or opponent controlled creature
        if (victim->ToPlayer()->duel->opponent == this || victim->ToPlayer()->duel->opponent->GetGUID() == GetOwnerGUID())
            damage = health - 1;

        duel_hasEnded = true;
    }

    // Rage from Damage made (only from direct weapon damage)
    if (cleanDamage && damagetype == DIRECT_DAMAGE && this != victim && GetTypeId() == TYPEID_PLAYER && (getPowerType() == POWER_RAGE))
    {
        uint32 weaponSpeedHitFactor;

        switch (cleanDamage->attackType)
        {
        case BASE_ATTACK:
            {
                if (cleanDamage->hitOutCome == MELEE_HIT_CRIT)
                    weaponSpeedHitFactor = uint32(GetAttackTime(cleanDamage->attackType) / 1000.0f * 7);
                else
                    weaponSpeedHitFactor = uint32(GetAttackTime(cleanDamage->attackType) / 1000.0f * 3.5f);

                // If attack is evaded/parried/dodged DON'T add rage
                if (cleanDamage->hitOutCome != MELEE_HIT_EVADE && cleanDamage->hitOutCome != MELEE_HIT_PARRY && cleanDamage->hitOutCome != MELEE_HIT_DODGE)
                    ToPlayer()->RewardRage(damage, weaponSpeedHitFactor, true);

                break;
            }
        case OFF_ATTACK:
            {
                if (cleanDamage->hitOutCome == MELEE_HIT_CRIT)
                    weaponSpeedHitFactor = uint32(GetAttackTime(cleanDamage->attackType) / 1000.0f * 3.5f);
                else
                    weaponSpeedHitFactor = uint32(GetAttackTime(cleanDamage->attackType) / 1000.0f * 1.75f);

                ToPlayer()->RewardRage(damage, weaponSpeedHitFactor, true);

                break;
            }
        case RANGED_ATTACK:
            break;
        }
    }

    if (victim->GetTypeId() == TYPEID_PLAYER && GetTypeId() == TYPEID_PLAYER)
    {
        if (victim->ToPlayer()->InBattleground())
        {
            Player* killer = ToPlayer();
            if (killer != victim->ToPlayer())
                if (Battleground* bg = killer->GetBattleground())
                    bg->UpdatePlayerScore(killer, SCORE_DAMAGE_DONE, damage);
        }
    }

    if (victim->GetTypeId() == TYPEID_UNIT && !victim->IsPet())
    {
        if (!victim->ToCreature()->hasLootRecipient())
            victim->ToCreature()->SetLootRecipient(this);

        if (!victim->ToCreature()->IsDamageEnoughForLootingAndReward() && (GetCharmerOrOwnerPlayerOrPlayerItself() || (ToTempSummon() && ToTempSummon()->GetSummoner() && ToTempSummon()->GetSummoner()->GetTypeId() == TYPEID_PLAYER)))
            victim->ToCreature()->SetPlayerDamaged(true);
    }

    if (health <= damage)
    {
        DEBUG_LOG("DealDamage: victim just died");
        Kill(victim, durabilityLoss);

        //Hook for OnPVPKill Event
        if (this->GetTypeId() == TYPEID_PLAYER)
        {
            if (victim->GetTypeId() == TYPEID_PLAYER)
            {
                Player *killer = this->ToPlayer();
                Player *killed = victim->ToPlayer();
                sScriptMgr.OnPVPKill(killer, killed);
            }
            else if (victim->GetTypeId() == TYPEID_UNIT)
            {
                Player *killer = this->ToPlayer();
                Creature *killed = victim->ToCreature();
                sScriptMgr.OnCreatureKill(killer, killed);
            }
        }
        else if (this->GetTypeId() == TYPEID_UNIT)
        {
            if (victim->GetTypeId() == TYPEID_PLAYER)
            {
                Creature *killer = this->ToCreature();
                Player *killed = victim->ToPlayer();
                sScriptMgr.OnPlayerKilledByCreature(killer, killed);
            }
        }
    }
    else                                                    // if (health <= damage)
    {
        DEBUG_LOG("DealDamageAlive");

        victim->ModifyHealth(-(int32)damage);

        if (damagetype == DIRECT_DAMAGE || damagetype == SPELL_DIRECT_DAMAGE)
        {
            victim->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_DIRECT_DAMAGE, spellProto ? spellProto->Id : 0);
        }

        if (victim->GetTypeId() != TYPEID_PLAYER)
        {
            if (damagetype != DOT && damage > 0 && !IS_PLAYER_GUID(victim->GetOwnerGUID()) && (!spellProto || !spellProto->HasEffect(SPELL_AURA_DAMAGE_SHIELD)))
                victim->ToCreature()->SetLastDamagedTime(sWorld.GetGameTime() + MAX_AGGRO_RESET_TIME);

            if (this)
                victim->AddThreat(this, float(damage), damageSchoolMask, spellProto);
        }
        else                                                // victim is a player
        {
            // random durability for items (HIT TAKEN)
            if (roll_chance_f(sWorld.getRate(RATE_DURABILITY_LOSS_DAMAGE)))
            {
                EquipmentSlots slot = EquipmentSlots(urand(0, EQUIPMENT_SLOT_END - 1));
                victim->ToPlayer()->DurabilityPointLossForEquipSlot(slot);
            }

            // Rage from damage received
            if (this != victim && victim->getPowerType() == POWER_RAGE)
            {
                uint32 rage_damage = damage + (cleanDamage ? cleanDamage->damage : 0);
                victim->ToPlayer()->RewardRage(rage_damage, 0, false);
            }
        }

        if (this && this->GetTypeId() == TYPEID_PLAYER)
        {
            // random durability for items (HIT DONE)
            if (roll_chance_f(sWorld.getRate(RATE_DURABILITY_LOSS_DAMAGE)))
            {
                EquipmentSlots slot = EquipmentSlots(urand(0, EQUIPMENT_SLOT_END - 1));
                this->ToPlayer()->DurabilityPointLossForEquipSlot(slot);
            }
        }

        if (damagetype != NODAMAGE && damage)
        {
            if (victim != this && victim->GetTypeId() == TYPEID_PLAYER) // does not support creature push_back
            {
                if (damagetype != DOT)
                {
                    if (Spell* spell = victim->m_currentSpells[CURRENT_GENERIC_SPELL])
                    {
                        if (spell->getState() == SPELL_STATE_PREPARING)
                        {
                            uint32 interruptFlags = spell->m_spellInfo->InterruptFlags;
                            if (interruptFlags & SPELL_INTERRUPT_FLAG_DAMAGE)
                                victim->InterruptNonMeleeSpells(false);
                            else if (interruptFlags & SPELL_INTERRUPT_FLAG_PUSH_BACK)
                                spell->Delayed();
                        }
                    }
                }

                if (Spell* spell = victim->m_currentSpells[CURRENT_CHANNELED_SPELL])
                {
                    if (spell->getState() == SPELL_STATE_CASTING)
                    {
                        uint32 channelInterruptFlags = spell->m_spellInfo->ChannelInterruptFlags;
                        if (((channelInterruptFlags & CHANNEL_FLAG_DELAY) != 0) && (damagetype != DOT))
                            spell->DelayedChannel();
                    }
                }
            }
        }

        // last damage from duel opponent
        if (duel_hasEnded)
        {
            ASSERT(victim->GetTypeId() == TYPEID_PLAYER);
            Player* he = victim->ToPlayer();

            ASSERT(he->duel);

            he->SetHealth(1);

            he->duel->opponent->CombatStopWithPets(true);
            he->CombatStopWithPets(true);

            he->CastSpell(he, 7267, true);                  // beg
            he->DuelComplete(DUEL_WON);
        }
    }

    DEBUG_LOG("DealDamageEnd returned %d damage", damage);

    return damage;
}

void Unit::CastStop(uint32 except_spellid)
{
    for (uint32 i = CURRENT_FIRST_NON_MELEE_SPELL; i < CURRENT_MAX_SPELL; i++)
        if (m_currentSpells[i] && m_currentSpells[i]->m_spellInfo->Id != except_spellid)
            InterruptSpell(CurrentSpellTypes(i), false, false);
}

void Unit::CastSpell(Unit* Victim, uint32 spellId, bool triggered, Item* castItem, Aura* triggeredByAura, uint64 originalCaster)
{
    SpellEntry const* spellInfo = sSpellStore.LookupEntry(spellId);

    if (!spellInfo)
    {
        sLog.outError("CastSpell: unknown spell id %i by caster: %s %u)", spellId, (GetTypeId() == TYPEID_PLAYER ? "player (GUID:" : "creature (Entry:"), (GetTypeId() == TYPEID_PLAYER ? GetGUIDLow() : GetEntry()));
        return;
    }

    CastSpell(Victim, spellInfo, triggered, castItem, triggeredByAura, originalCaster);
}

void Unit::CastSpell(Unit* Victim, SpellEntry const* spellInfo, bool triggered, Item* castItem, Aura* triggeredByAura, uint64 originalCaster)
{
    if (!spellInfo)
    {
        sLog.outError("CastSpell: unknown spell by caster: %s %u)", (GetTypeId() == TYPEID_PLAYER ? "player (GUID:" : "creature (Entry:"), (GetTypeId() == TYPEID_PLAYER ? GetGUIDLow() : GetEntry()));
        return;
    }

    SpellCastTargets targets;
    uint32 targetMask = spellInfo->Targets;
    //if (targetMask & (TARGET_FLAG_UNIT|TARGET_FLAG_UNK2))
    for (int i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        if (sSpellMgr.SpellTargetType[spellInfo->EffectImplicitTargetA[i]] == TARGET_TYPE_UNIT_TARGET)
        {
            /*SpellRangeEntry const* srange = sSpellRangeStore.LookupEntry(spellInfo->rangeIndex);
            if (srange && GetSpellMaxRange(srange) == 0.0f)
            {
                Victim = this;
                break;
            }
            else */if (!Victim)
            {
                sLog.outError("CastSpell: spell id %i by caster: %s %u) does not have unit target", spellInfo->Id, (GetTypeId() == TYPEID_PLAYER ? "player (GUID:" : "creature (Entry:"), (GetTypeId() == TYPEID_PLAYER ? GetGUIDLow() : GetEntry()));
                return;
            }
            else
                break;
        }
    }
    targets.setUnitTarget(Victim);

    if (targetMask & (TARGET_FLAG_SOURCE_LOCATION | TARGET_FLAG_DEST_LOCATION))
    {
        if (!Victim)
        {
            sLog.outError("CastSpell: spell id %i by caster: %s %u) does not have destination", spellInfo->Id, (GetTypeId() == TYPEID_PLAYER ? "player (GUID:" : "creature (Entry:"), (GetTypeId() == TYPEID_PLAYER ? GetGUIDLow() : GetEntry()));
            return;
        }
        targets.setDst(Victim);
    }

    #ifdef OREGON_DEBUG
    if (castItem)
        DEBUG_LOG("WORLD: cast Item spellId - %i", spellInfo->Id);
    #endif

    if (!originalCaster && triggeredByAura)
        originalCaster = triggeredByAura->GetCasterGUID();

    Spell* spell = new Spell(this, spellInfo, triggered, originalCaster);

    // When casting a combat spell the unit has to be flagged as initiating combat
    // Check for self-cast case here for this may have been called by a command
    if (Victim && spell->GetCaster() != Victim && !IsNonCombatSpell(spellInfo))
        spell->GetCaster()->SetInitiatingCombat(true);

    spell->m_CastItem = castItem;
    spell->prepare(&targets, triggeredByAura);
}

void Unit::CastCustomSpell(Unit* target, uint32 spellId, int32 const* bp0, int32 const* bp1, int32 const* bp2, bool triggered, Item* castItem, Aura* triggeredByAura, uint64 originalCaster)
{
    CustomSpellValues values;
    if (bp0)
        values.AddSpellMod(SPELLVALUE_BASE_POINT0, *bp0);
    if (bp1)
        values.AddSpellMod(SPELLVALUE_BASE_POINT1, *bp1);
    if (bp2)
        values.AddSpellMod(SPELLVALUE_BASE_POINT2, *bp2);
    CastCustomSpell(spellId, values, target, triggered, castItem, triggeredByAura, originalCaster);
}

void Unit::CastCustomSpell(uint32 spellId, SpellValueMod mod, uint32 value, Unit* target, bool triggered, Item* castItem, Aura* triggeredByAura, uint64 originalCaster)
{
    CustomSpellValues values;
    values.AddSpellMod(mod, value);
    CastCustomSpell(spellId, values, target, triggered, castItem, triggeredByAura, originalCaster);
}

void Unit::CastCustomSpell(uint32 spellId, CustomSpellValues const& value, Unit* Victim, bool triggered, Item* castItem, Aura* triggeredByAura, uint64 originalCaster)
{
    SpellEntry const* spellInfo = sSpellStore.LookupEntry(spellId);
    if (!spellInfo)
    {
        sLog.outError("CastSpell: unknown spell id %i by caster: %s %u)", spellId, (GetTypeId() == TYPEID_PLAYER ? "player (GUID:" : "creature (Entry:"), (GetTypeId() == TYPEID_PLAYER ? GetGUIDLow() : GetEntry()));
        return;
    }

    SpellCastTargets targets;
    uint32 targetMask = spellInfo->Targets;

    //check unit target
    for (int i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        if (sSpellMgr.SpellTargetType[spellInfo->EffectImplicitTargetA[i]] == TARGET_TYPE_UNIT_TARGET)
        {
            if (!Victim)
            {
                sLog.outError("CastSpell: spell id %i by caster: %s %u) does not have unit target", spellInfo->Id, (GetTypeId() == TYPEID_PLAYER ? "player (GUID:" : "creature (Entry:"), (GetTypeId() == TYPEID_PLAYER ? GetGUIDLow() : GetEntry()));
                return;
            }
            else
                break;
        }
    }
    targets.setUnitTarget(Victim);

    //check destination
    if (targetMask & (TARGET_FLAG_SOURCE_LOCATION | TARGET_FLAG_DEST_LOCATION))
    {
        if (!Victim)
        {
            sLog.outError("CastSpell: spell id %i by caster: %s %u) does not have destination", spellInfo->Id, (GetTypeId() == TYPEID_PLAYER ? "player (GUID:" : "creature (Entry:"), (GetTypeId() == TYPEID_PLAYER ? GetGUIDLow() : GetEntry()));
            return;
        }
        targets.setDst(Victim);
    }

    if (!originalCaster && triggeredByAura)
        originalCaster = triggeredByAura->GetCasterGUID();

    Spell* spell = new Spell(this, spellInfo, triggered, originalCaster);

    if (castItem)
    {
        DEBUG_LOG("WORLD: cast Item spellId - %i", spellInfo->Id);
        spell->m_CastItem = castItem;
    }

    for (CustomSpellValues::const_iterator itr = value.begin(); itr != value.end(); ++itr)
        spell->SetSpellValue(itr->first, itr->second);

    spell->prepare(&targets, triggeredByAura);
}

// used for scripting
void Unit::CastSpell(float x, float y, float z, uint32 spellId, bool triggered, Item* castItem, Aura* triggeredByAura, uint64 originalCaster)
{
    SpellEntry const* spellInfo = sSpellStore.LookupEntry(spellId);
    if (!spellInfo)
    {
        sLog.outError("CastSpell(x,y,z): unknown spell id %i by caster: %s %u)", spellId, (GetTypeId() == TYPEID_PLAYER ? "player (GUID:" : "creature (Entry:"), (GetTypeId() == TYPEID_PLAYER ? GetGUIDLow() : GetEntry()));
        return;
    }

    #ifdef OREGON_DEBUG
    if (castItem)
        DEBUG_LOG("WORLD: cast Item spellId - %i", spellInfo->Id);
    #endif

    if (!originalCaster && triggeredByAura)
        originalCaster = triggeredByAura->GetCasterGUID();

    Spell* spell = new Spell(this, spellInfo, triggered, originalCaster);

    SpellCastTargets targets;
    targets.setDst(x, y, z, GetOrientation());
    spell->m_CastItem = castItem;
    spell->prepare(&targets, triggeredByAura);
}

// used for scripting
void Unit::CastSpell(GameObject* go, uint32 spellId, bool triggered, Item* castItem, Aura* triggeredByAura, uint64 originalCaster)
{
    if (!go)
        return;

    SpellEntry const* spellInfo = sSpellStore.LookupEntry(spellId);

    if (!spellInfo)
    {
        sLog.outError("CastSpell(x,y,z): unknown spell id %i by caster: %s %u)", spellId, (GetTypeId() == TYPEID_PLAYER ? "player (GUID:" : "creature (Entry:"), (GetTypeId() == TYPEID_PLAYER ? GetGUIDLow() : GetEntry()));
        return;
    }

    if (!(spellInfo->Targets & (TARGET_FLAG_OBJECT | TARGET_FLAG_OBJECT_UNK)))
    {
        sLog.outError("CastSpell: spell id %i by caster: %s %u) is not gameobject spell", spellId, (GetTypeId() == TYPEID_PLAYER ? "player (GUID:" : "creature (Entry:"), (GetTypeId() == TYPEID_PLAYER ? GetGUIDLow() : GetEntry()));
        return;
    }

    #ifdef OREGON_DEBUG
    if (castItem)
        DEBUG_LOG("WORLD: cast Item spellId - %i", spellInfo->Id);
    #endif

    if (!originalCaster && triggeredByAura)
        originalCaster = triggeredByAura->GetCasterGUID();

    Spell* spell = new Spell(this, spellInfo, triggered, originalCaster);

    SpellCastTargets targets;
    targets.setGOTarget(go);
    spell->m_CastItem = castItem;
    spell->prepare(&targets, triggeredByAura);
}

// Obsolete func need remove, here only for comotability vs another patches
uint32 Unit::SpellNonMeleeDamageLog(Unit* victim, uint32 spellID, uint32 damage, bool /*isTriggeredSpell*/, bool /*useSpellDamage*/)
{
    SpellEntry const* spellInfo = sSpellStore.LookupEntry(spellID);
    SpellNonMeleeDamage damageInfo(this, victim, spellInfo->Id, spellInfo->SchoolMask);
    damage = SpellDamageBonus(victim, spellInfo, damage, SPELL_DIRECT_DAMAGE);
    CalculateSpellDamageTaken(&damageInfo, damage, spellInfo);
    SendSpellNonMeleeDamageLog(&damageInfo);
    DealSpellDamage(&damageInfo, true);
    return damageInfo.damage;
}

void Unit::CalculateSpellDamageTaken(SpellNonMeleeDamage* damageInfo, int32 damage, SpellEntry const* spellInfo, WeaponAttackType attackType, bool crit)
{
    if (damage < 0)
        return;

    Unit* victim = damageInfo->target;
    if (!victim || !victim->IsAlive())
        return;

    SpellSchoolMask damageSchoolMask = SpellSchoolMask(damageInfo->schoolMask);
    uint32 crTypeMask = victim->GetCreatureTypeMask();

    bool blocked = false;
    // Per-school calc
    switch (spellInfo->DmgClass)
    {
        // Melee and Ranged Spells
        case SPELL_DAMAGE_CLASS_RANGED:
        case SPELL_DAMAGE_CLASS_MELEE:
        {
            // Physical Damage
            if (damageSchoolMask & SPELL_SCHOOL_MASK_NORMAL)
            {
                // Get blocked status
                blocked = isSpellBlocked(victim, spellInfo, attackType);
            }

            if (crit)
            {
                damageInfo->HitInfo |= SPELL_HIT_TYPE_CRIT;

                // Calculate crit bonus
                uint32 crit_bonus = damage;
                // Apply crit_damage bonus for melee spells
                if (Player* modOwner = GetSpellModOwner())
                    modOwner->ApplySpellMod(spellInfo->Id, SPELLMOD_CRIT_DAMAGE_BONUS, crit_bonus);
                damage += crit_bonus;

                // Apply SPELL_AURA_MOD_ATTACKER_RANGED_CRIT_DAMAGE or SPELL_AURA_MOD_ATTACKER_MELEE_CRIT_DAMAGE
                int32 critPctDamageMod = 0;
                if (attackType == RANGED_ATTACK)
                    critPctDamageMod += victim->GetTotalAuraModifier(SPELL_AURA_MOD_ATTACKER_RANGED_CRIT_DAMAGE);
                else
                {
                    critPctDamageMod += victim->GetTotalAuraModifier(SPELL_AURA_MOD_ATTACKER_MELEE_CRIT_DAMAGE);
                    critPctDamageMod += GetTotalAuraModifier(SPELL_AURA_MOD_CRIT_DAMAGE_BONUS_MELEE);
                }

                // Increase crit damage from SPELL_AURA_MOD_CRIT_PERCENT_VERSUS
                critPctDamageMod += GetTotalAuraModifierByMiscMask(SPELL_AURA_MOD_CRIT_PERCENT_VERSUS, crTypeMask);

                if (critPctDamageMod != 0)
                    AddPct(damage, critPctDamageMod);

                // Resilience - reduce crit damage
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    damage -= victim->ToPlayer()->GetMeleeCritDamageReduction(damage);
            }

            // Spell weapon based damage CAN BE crit & blocked at same time
            if (blocked)
            {
                damageInfo->blocked = uint32(victim->GetShieldBlockValue());
                if (damage < int32(damageInfo->blocked))
                    damageInfo->blocked = uint32(damage);
                damage -= damageInfo->blocked;
            }
        }
        break;
    // Magical Attacks
    case SPELL_DAMAGE_CLASS_NONE:
    case SPELL_DAMAGE_CLASS_MAGIC:
        {
            // If crit add critical bonus
            if (crit)
            {
                damageInfo->HitInfo |= SPELL_HIT_TYPE_CRIT;
                damage = SpellCriticalBonus(spellInfo, damage, victim);

                // Resilience - reduce crit damage
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    damage -= victim->ToPlayer()->GetSpellCritDamageReduction(damage);
            }
        }
        break;
    default:
        break;
    }

    // damage before absorb/resist calculation
    damageInfo->cleanDamage = damage;
    if (IsDamageReducedByArmor(damageSchoolMask, spellInfo))
        damage = CalcArmorReducedDamage(victim, damage);

    // Calculate absorb resist
    if (damage > 0)
    {
        CalcAbsorbResist(victim, damageSchoolMask, SPELL_DIRECT_DAMAGE, damage, &damageInfo->absorb, &damageInfo->resist, spellInfo, IsBinarySpell(spellInfo));
        damage -= damageInfo->absorb + damageInfo->resist;
    }
    else
        damage = 0;

    damageInfo->damage = damage;
}

void Unit::DealSpellDamage(SpellNonMeleeDamage* damageInfo, bool durabilityLoss)
{
    if (damageInfo == 0)
        return;

    Unit* victim = damageInfo->target;

    if (!victim)
        return;

    if (!victim->IsAlive() || victim->HasUnitState(UNIT_STATE_IN_FLIGHT) || (victim->GetTypeId() == TYPEID_UNIT && victim->ToCreature()->IsInEvadeMode()))
        return;

    SpellEntry const* spellProto = sSpellStore.LookupEntry(damageInfo->SpellID);
    if (spellProto == NULL)
    {
        DEBUG_LOG("Unit::DealSpellDamage has invalid damageInfo->SpellID: %u", damageInfo->SpellID);
        return;
    }

    //You don't lose health from damage taken from another player while in a sanctuary
    //You still see it in the combat log though
    if (victim != this && GetTypeId() == TYPEID_PLAYER && victim->GetTypeId() == TYPEID_PLAYER)
    {
        const AreaTableEntry* area = GetAreaEntryByAreaID(victim->GetAreaId());
        if (area && area->flags & 0x800)                     //sanctuary
            return;
    }

    // update at damage Judgement aura duration that applied by attacker at victim
    if (damageInfo->damage && spellProto->Id == 35395)
    {
        AuraMap& vAuras = victim->GetAuras();
        for (AuraMap::iterator itr = vAuras.begin(); itr != vAuras.end(); ++itr)
        {
            SpellEntry const* spellInfo = (*itr).second->GetSpellProto();
            if (spellInfo->SpellFamilyName == SPELLFAMILY_PALADIN && spellInfo->AttributesEx3 & 0x40000)
            {
                (*itr).second->SetAuraDuration((*itr).second->GetAuraMaxDuration());
                (*itr).second->UpdateAuraDuration();
            }
        }
    }
    // Call default DealDamage
    CleanDamage cleanDamage(damageInfo->cleanDamage, BASE_ATTACK, MELEE_HIT_NORMAL);
    DealDamage(victim, damageInfo->damage, &cleanDamage, SPELL_DIRECT_DAMAGE, SpellSchoolMask(damageInfo->schoolMask), spellProto, durabilityLoss);
}

/// @todo for melee need create structure as in
void Unit::CalculateMeleeDamage(Unit* victim, uint32 damage, CalcDamageInfo* damageInfo, WeaponAttackType attackType)
{
    damageInfo->attacker         = this;
    damageInfo->target           = victim;
    damageInfo->damageSchoolMask = GetMeleeDamageSchoolMask();
    damageInfo->attackType       = attackType;
    damageInfo->damage           = 0;
    damageInfo->cleanDamage      = 0;
    damageInfo->absorb           = 0;
    damageInfo->resist           = 0;
    damageInfo->blocked_amount   = 0;

    damageInfo->TargetState      = 0;
    damageInfo->HitInfo          = 0;
    damageInfo->procAttacker     = PROC_FLAG_NONE;
    damageInfo->procVictim       = PROC_FLAG_NONE;
    damageInfo->procEx           = PROC_EX_NONE;
    damageInfo->hitOutCome       = MELEE_HIT_EVADE;

    if (!victim)
        return;

    if (!IsAlive() || !victim->IsAlive())
        return;

    // Select HitInfo/procAttacker/procVictim flag based on attack type
    switch (attackType)
    {
        case BASE_ATTACK:
            damageInfo->procAttacker = PROC_FLAG_DONE_MELEE_AUTO_ATTACK | PROC_FLAG_DONE_MAINHAND_ATTACK;
            damageInfo->procVictim   = PROC_FLAG_TAKEN_MELEE_AUTO_ATTACK;
            damageInfo->HitInfo      = HITINFO_NORMALSWING2;
            break;
        case OFF_ATTACK:
            damageInfo->procAttacker = PROC_FLAG_DONE_MELEE_AUTO_ATTACK | PROC_FLAG_DONE_OFFHAND_ATTACK;
            damageInfo->procVictim   = PROC_FLAG_TAKEN_MELEE_AUTO_ATTACK;
            damageInfo->HitInfo = HITINFO_LEFTSWING;
            break;
        case RANGED_ATTACK:
            damageInfo->procAttacker = PROC_FLAG_DONE_RANGED_AUTO_ATTACK;
            damageInfo->procVictim   = PROC_FLAG_TAKEN_RANGED_AUTO_ATTACK;
            damageInfo->HitInfo = HITINFO_UNK3; // HitInfo flag not confirmed.
            break;
        default:
            break;
        }

    // Physical Immune check
    if (damageInfo->target->IsImmunedToDamage(SpellSchoolMask(damageInfo->damageSchoolMask), true))
    {
        damageInfo->HitInfo       |= HITINFO_NORMALSWING;
        damageInfo->TargetState    = VICTIMSTATE_IS_IMMUNE;

        damageInfo->procEx        |= PROC_EX_IMMUNE;
        damageInfo->damage         = 0;
        damageInfo->cleanDamage    = 0;
        return;
    }

    damage += CalculateDamage(damageInfo->attackType, false, true);
    // Add melee damage bonus
    MeleeDamageBonus(damageInfo->target, &damage, damageInfo->attackType);

    // Calculate armor reduction
    if (IsDamageReducedByArmor((SpellSchoolMask)(damageInfo->damageSchoolMask)))
    {
        damageInfo->damage = CalcArmorReducedDamage(damageInfo->target, damage);
        damageInfo->cleanDamage += damage - damageInfo->damage;
    }
    else
        damageInfo->damage = damage;

    damageInfo->hitOutCome = RollMeleeOutcomeAgainst(damageInfo->target, damageInfo->attackType);

    // Disable parry or dodge for ranged attack
    if (damageInfo->attackType == RANGED_ATTACK)
    {
        if (damageInfo->hitOutCome == MELEE_HIT_PARRY) damageInfo->hitOutCome = MELEE_HIT_NORMAL;
        if (damageInfo->hitOutCome == MELEE_HIT_DODGE) damageInfo->hitOutCome = MELEE_HIT_MISS;
    }

    switch (damageInfo->hitOutCome)
    {
        case MELEE_HIT_EVADE:
            damageInfo->HitInfo    |= HITINFO_MISS | HITINFO_SWINGNOHITSOUND;
            damageInfo->TargetState = VICTIMSTATE_EVADES;
            damageInfo->procEx |= PROC_EX_EVADE;
            damageInfo->damage = 0;
            damageInfo->cleanDamage = 0;
            return;
        case MELEE_HIT_MISS:
            damageInfo->HitInfo    |= HITINFO_MISS;
            damageInfo->TargetState = VICTIMSTATE_NORMAL;
            damageInfo->procEx |= PROC_EX_MISS;
            damageInfo->damage  = 0;
            damageInfo->cleanDamage = 0;
            break;
        case MELEE_HIT_NORMAL:
            damageInfo->TargetState = VICTIMSTATE_NORMAL;
            damageInfo->procEx |= PROC_EX_NORMAL_HIT;
            break;
        case MELEE_HIT_CRIT:
        {
            damageInfo->HitInfo     |= HITINFO_CRITICALHIT;
            damageInfo->TargetState  = VICTIMSTATE_NORMAL;

            damageInfo->procEx      |= PROC_EX_CRITICAL_HIT;
            // Crit bonus calc
            damageInfo->damage += damageInfo->damage;
            int32 mod = 0;
            // Apply SPELL_AURA_MOD_ATTACKER_RANGED_CRIT_DAMAGE or SPELL_AURA_MOD_ATTACKER_MELEE_CRIT_DAMAGE
            if (damageInfo->attackType == RANGED_ATTACK)
                mod += damageInfo->target->GetTotalAuraModifier(SPELL_AURA_MOD_ATTACKER_RANGED_CRIT_DAMAGE);
            else
            {
                mod += damageInfo->target->GetTotalAuraModifier(SPELL_AURA_MOD_ATTACKER_MELEE_CRIT_DAMAGE);
                mod += GetTotalAuraModifier(SPELL_AURA_MOD_CRIT_DAMAGE_BONUS_MELEE);
            }

            uint32 crTypeMask = damageInfo->target->GetCreatureTypeMask();

            // Increase crit damage from SPELL_AURA_MOD_CRIT_PERCENT_VERSUS
            mod += GetTotalAuraModifierByMiscMask(SPELL_AURA_MOD_CRIT_PERCENT_VERSUS, crTypeMask);
            if (mod != 0)
                AddPct(damageInfo->damage, mod);

            // Resilience - reduce crit damage
            if (victim->GetTypeId() == TYPEID_PLAYER)
            {
                uint32 resilienceReduction = victim->ToPlayer()->GetMeleeCritDamageReduction(damageInfo->damage);
                damageInfo->damage      -= resilienceReduction;
                damageInfo->cleanDamage += resilienceReduction;
            }
            break;
        }
    case MELEE_HIT_PARRY:
        damageInfo->TargetState  = VICTIMSTATE_PARRY;
        damageInfo->procEx      |= PROC_EX_PARRY;
        damageInfo->cleanDamage += damageInfo->damage;
        damageInfo->damage = 0;
        break;

    case MELEE_HIT_DODGE:
        damageInfo->TargetState  = VICTIMSTATE_DODGE;
        damageInfo->procEx      |= PROC_EX_DODGE;
        damageInfo->cleanDamage += damageInfo->damage;
        damageInfo->damage = 0;
        break;
    case MELEE_HIT_BLOCK:
        damageInfo->TargetState = VICTIMSTATE_NORMAL;
        damageInfo->procEx     |= PROC_EX_BLOCK;
        damageInfo->blocked_amount = damageInfo->target->GetShieldBlockValue();
        if (damageInfo->blocked_amount >= damageInfo->damage)
        {
            damageInfo->TargetState = VICTIMSTATE_BLOCKS;
            damageInfo->blocked_amount = damageInfo->damage;
        }
        else
            damageInfo->procEx |= PROC_EX_NORMAL_HIT;
        damageInfo->damage      -= damageInfo->blocked_amount;
        damageInfo->cleanDamage += damageInfo->blocked_amount;
        break;
    case MELEE_HIT_GLANCING:
        {
            damageInfo->HitInfo     |= HITINFO_GLANCING;
            damageInfo->TargetState  = VICTIMSTATE_NORMAL;
            damageInfo->procEx      |= PROC_EX_NORMAL_HIT;
            int32 leveldif = int32(victim->getLevel()) - int32(getLevel());
            if (leveldif > 3)
                leveldif = 3;
            float reducePercent = 1 - leveldif * 0.1f;
            damageInfo->cleanDamage += damageInfo->damage - uint32(reducePercent * damageInfo->damage);
            damageInfo->damage   = uint32(reducePercent * damageInfo->damage);
            break;
        }
    case MELEE_HIT_CRUSHING:
            damageInfo->HitInfo     |= HITINFO_CRUSHING;
            damageInfo->TargetState  = VICTIMSTATE_NORMAL;
            damageInfo->procEx      |= PROC_EX_NORMAL_HIT;
            // 150% normal damage
            damageInfo->damage += (damageInfo->damage / 2);
            break;
    default:
        break;
    }

    // Calculate absorb resist
    if (int32(damageInfo->damage) > 0)
    {
        damageInfo->procVictim |= PROC_FLAG_TAKEN_DAMAGE;
        // Calculate absorb & resists
        CalcAbsorbResist(damageInfo->target, SpellSchoolMask(damageInfo->damageSchoolMask), DIRECT_DAMAGE, damageInfo->damage, &damageInfo->absorb, &damageInfo->resist);


        if (damageInfo->absorb)
        {
            damageInfo->HitInfo |= HITINFO_ABSORB;
            damageInfo->procEx  |= PROC_EX_ABSORB;
        }

        if (damageInfo->resist)
            damageInfo->HitInfo |= HITINFO_RESIST;

        damageInfo->damage -= damageInfo->absorb + damageInfo->resist;
    }
    else // Impossible get negative result but....
        damageInfo->damage = 0;
}

void Unit::DealMeleeDamage(CalcDamageInfo* damageInfo, bool durabilityLoss)
{
    Unit* victim = damageInfo->target;

    if (!victim)
        return;

    if (!victim->IsAlive() || victim->HasUnitState(UNIT_STATE_IN_FLIGHT) || (victim->GetTypeId() == TYPEID_UNIT && victim->ToCreature()->IsInEvadeMode()))
        return;

    //You don't lose health from damage taken from another player while in a sanctuary
    //You still see it in the combat log though
    if (victim != this && GetTypeId() == TYPEID_PLAYER && victim->GetTypeId() == TYPEID_PLAYER)
    {
        const AreaTableEntry* area = GetAreaEntryByAreaID(victim->GetAreaId());
        if (area && area->flags & 0x800)                     //sanctuary
            return;
    }

    // Hmmmm dont like this emotes client must by self do all animations
    if (damageInfo->HitInfo & HITINFO_CRITICALHIT)
        victim->HandleEmoteCommand(EMOTE_ONESHOT_WOUNDCRITICAL);
    if (damageInfo->blocked_amount && damageInfo->TargetState != VICTIMSTATE_BLOCKS)
        victim->HandleEmoteCommand(EMOTE_ONESHOT_PARRYSHIELD);

    if (damageInfo->TargetState == VICTIMSTATE_PARRY)
    {
        // Get attack timers
        float offtime  = float(victim->getAttackTimer(OFF_ATTACK));
        float basetime = float(victim->getAttackTimer(BASE_ATTACK));
        // Reduce attack time
        if (victim->haveOffhandWeapon() && offtime < basetime)
        {
            float percent20 = victim->GetAttackTime(OFF_ATTACK) * 0.20f;
            float percent60 = 3.0f * percent20;
            if (offtime > percent20 && offtime <= percent60)
                victim->setAttackTimer(OFF_ATTACK, uint32(percent20));
            else if (offtime > percent60)
            {
                offtime -= 2.0f * percent20;
                victim->setAttackTimer(OFF_ATTACK, uint32(offtime));
            }
        }
        else
        {
            float percent20 = victim->GetAttackTime(BASE_ATTACK) * 0.20;
            float percent60 = 3.0f * percent20;
            if (basetime > percent20 && basetime <= percent60)
                victim->setAttackTimer(BASE_ATTACK, uint32(percent20));
            else if (basetime > percent60)
            {
                basetime -= 2.0f * percent20;
                victim->setAttackTimer(BASE_ATTACK, uint32(basetime));
            }
        }
    }

    // Call default DealDamage
    CleanDamage cleanDamage(damageInfo->cleanDamage, damageInfo->attackType, damageInfo->hitOutCome);
    DealDamage(victim, damageInfo->damage, &cleanDamage, DIRECT_DAMAGE, SpellSchoolMask(damageInfo->damageSchoolMask), NULL, durabilityLoss);

    // If this is a creature and it attacks from behind it has a probability to daze it's victim
    if ((damageInfo->hitOutCome == MELEE_HIT_CRIT || damageInfo->hitOutCome == MELEE_HIT_CRUSHING || damageInfo->hitOutCome == MELEE_HIT_NORMAL || damageInfo->hitOutCome == MELEE_HIT_GLANCING) &&
        GetTypeId() != TYPEID_PLAYER && !ToCreature()->IsControlledByPlayer() && !victim->HasInArc(float(M_PI), this) && damageInfo->damage
        && (victim->GetTypeId() == TYPEID_PLAYER || !victim->ToCreature()->isWorldBoss()))
    {
        // -probability is between 0% and 40%
        // 20% base chance
        float Probability = 20.0f;

        //there is a newbie protection, at level 10 just 7% base chance; assuming linear function
        if (victim->getLevel() < 30)
            Probability = 0.65f * victim->getLevel() + 0.5f;

        uint32 VictimDefense = victim->GetDefenseSkillValue();
        uint32 AttackerMeleeSkill = GetUnitMeleeSkill();

        Probability *= AttackerMeleeSkill / (float)VictimDefense*0.16f;

        if (Probability < 0)
            Probability = 0;

        if (Probability > 40.0f)
            Probability = 40.0f;

        if (roll_chance_f(Probability))
            CastSpell(victim, 1604, true);
    }

    // update at damage Judgement aura duration that applied by attacker at victim
    if (damageInfo->damage)
    {
        AuraMap& vAuras = victim->GetAuras();
        for (AuraMap::iterator itr = vAuras.begin(); itr != vAuras.end(); ++itr)
        {
            SpellEntry const* spellInfo = (*itr).second->GetSpellProto();
            if (spellInfo->AttributesEx3 & 0x40000 && spellInfo->SpellFamilyName == SPELLFAMILY_PALADIN && ((*itr).second->GetCasterGUID() == GetGUID()))
            {
                (*itr).second->SetAuraDuration((*itr).second->GetAuraMaxDuration());
                (*itr).second->UpdateAuraDuration();
            }
        }
    }

    if (GetTypeId() == TYPEID_PLAYER)
        ToPlayer()->CastItemCombatSpell(victim, damageInfo->attackType, damageInfo->procVictim, damageInfo->procEx);

    // Do effect if any damage done to target
    if (damageInfo->procVictim & PROC_FLAG_TAKEN_DAMAGE)
    {
        // victim's damage shield
        std::set<Aura*> alreadyDone;
        uint32 removedAuras = victim->m_removedAurasCount;
        AuraList const& vDamageShields = victim->GetAurasByType(SPELL_AURA_DAMAGE_SHIELD);
        for (AuraList::const_iterator i = vDamageShields.begin(), next = vDamageShields.begin(); i != vDamageShields.end(); i = next)
        {
            ++next;
            if (alreadyDone.find(*i) == alreadyDone.end())
            {
                SpellEntry const* spellProto = sSpellStore.LookupEntry((*i)->GetId());
                if (!spellProto)
                    continue;

                // Damage shield can be resisted...
                if (SpellMissInfo missInfo = victim->SpellHitResult(this, spellProto, false))
                {
                    victim->SendSpellMiss(this, spellProto->Id, missInfo);
                    continue;
                }

                // ...or immuned
                if (IsImmunedToDamage(spellProto))
                {
                    victim->SendSpellDamageImmune(this, spellProto->Id);
                    continue;
                }

                alreadyDone.insert(*i);
                uint32 damage = (*i)->GetModifier()->m_amount;

                WorldPacket data(SMSG_SPELLDAMAGESHIELD, (8 + 8 + 4 + 4 + 4));
                data << uint64(victim->GetGUID());
                data << uint64(GetGUID());
                data << uint32(spellProto->Id);
                data << uint32(damage);                  // Damage
                data << uint32(spellProto->SchoolMask);
                victim->SendMessageToSet(&data, true);

                victim->DealDamage(this, damage, 0, SPELL_DIRECT_DAMAGE, GetSpellSchoolMask(spellProto), spellProto, true);

                if (victim->m_removedAurasCount > removedAuras)
                {
                    removedAuras = victim->m_removedAurasCount;
                    next = vDamageShields.begin();
                }
            }
        }
    }
}

void Unit::HandleEmoteCommand(uint32 anim_id)
{
    WorldPacket data(SMSG_EMOTE, 4 + 8);
    data << uint32(anim_id);
    data << uint64(GetGUID());
    SendMessageToSet(&data, true);
}

bool Unit::IsDamageReducedByArmor(SpellSchoolMask schoolMask, SpellEntry const* spellInfo, uint8 effIndex)
{
    // only physical spells damage gets reduced by armor
    if ((schoolMask & SPELL_SCHOOL_MASK_NORMAL) == 0)
        return false;
    if (spellInfo)
    {
        // there are spells with no specific attribute but they have "ignores armor" in tooltip
        
        if (sSpellMgr.GetSpellCustomAttr(spellInfo->Id) & SPELL_ATTR_CU_IGNORE_ARMOR)
            return false;

        // bleeding effects are not reduced by armor
        // @todo Get this to work with Rake.
        if (effIndex != MAX_SPELL_EFFECTS)
        {
            if (spellInfo->EffectApplyAuraName[effIndex] == SPELL_AURA_PERIODIC_DAMAGE ||
                spellInfo->Effect[effIndex] == SPELL_EFFECT_SCHOOL_DAMAGE)
                if (spellInfo->GetEffectMechanicMask(effIndex) & (1<<MECHANIC_BLEED))
                    return false;
        }
    }
    return true;
}

uint32 Unit::CalculateEffectiveMagicResistance(Unit* attacker, SpellSchoolMask schoolMask) const
{
    if (!(schoolMask & SPELL_SCHOOL_MASK_MAGIC))
        return 0;

    uint32 resistance = 0;
    // Select a resistance value matching spell school mask, prefer mininal for multischool spells
    uint32 schools = uint32(schoolMask);
    for (uint32 school = 0; schools; ++school)
    {
        if ((schools & 1) && school != SPELL_SCHOOL_NORMAL && school != SPELL_SCHOOL_HOLY)
        {
            // Base victim resistance
            uint32 amount = GetResistance(SpellSchools(school));
            // Add SPELL_AURA_MOD_TARGET_RESISTANCE aura value at caster
            // Negative value is spell penetration, but effective resistance can't be negative since betas
            if (const int32 casterMod = attacker->GetTotalAuraModifierByMiscMask(SPELL_AURA_MOD_TARGET_RESISTANCE, (1 << school)))
                amount = uint32(std::max((int32(amount) + casterMod), 0));
            if (!amount)
            {
                resistance = 0; // No resistance for this school: use it!
                break;
            }
            else if (!resistance || amount < resistance)
                resistance = amount;    // First school encountered or more vulnerable: memorize and continue
        }
        schools >>= 1;
    }

    return resistance;
}

float Unit::CalculateMagicResistanceMitigation(Unit* attacker, uint32 resistance, bool binary) const
{
    // Total resistance mitigation: final ratio of resistance effectiveness
    float ratio = float(float(resistance) / (attacker->getLevelForTarget(this) * 5)) * 0.75f;
    // Add bonus resistance mitigation to victim based on level difference for non-binary spells
    if (!binary)
        ratio += std::max(int32(getLevelForTarget(attacker) - attacker->getLevelForTarget(this)), 0) * 0.02f;
    // Magic resistance mitigation is capped at 0.75
    return std::min(ratio, 0.75f);
}

uint32 Unit::CalcArmorReducedDamage(Unit* victim, const uint32 damage)
{
    uint32 newdamage = 0;
    float armor = victim->GetArmor();

    // Ignore enemy armor by SPELL_AURA_MOD_TARGET_RESISTANCE aura
    armor += GetTotalAuraModifierByMiscMask(SPELL_AURA_MOD_TARGET_RESISTANCE, SPELL_SCHOOL_MASK_NORMAL);

    if (armor < 0.0f) armor = 0.0f;

    float tmpvalue = 0.0f;
    if (getLevel() <= 59)                                    //Level 1-59
        tmpvalue = armor / (armor + 400.0f + 85.0f * getLevel());
    else if (getLevel() < 70)                                //Level 60-69
        tmpvalue = armor / (armor - 22167.5f + 467.5f * getLevel());
    else                                                    //Level 70+
        tmpvalue = armor / (armor + 10557.5f);

    if (tmpvalue < 0.0f)
        tmpvalue = 0.0f;
    if (tmpvalue > 0.75f)
        tmpvalue = 0.75f;

    newdamage = uint32(damage - (damage * tmpvalue));

    return (newdamage > 1) ? newdamage : 1;
}

void Unit::CalcAbsorbResist(Unit* victim, SpellSchoolMask schoolMask, DamageEffectType damagetype, const uint32 damage, uint32* absorb, uint32* resist, SpellEntry const* spellInfo /*= NULL*/, bool binary)
{
    if (!victim || !victim->IsAlive() || !damage)
        return;

    // Magic damage, check for resists
    if (!spellInfo || (spellInfo->AttributesEx4 & SPELL_ATTR4_IGNORE_RESISTANCES) == 0 && (schoolMask & SPELL_SCHOOL_MASK_MAGIC) && (!binary || damagetype == DOT))
    {
        const float mitigation = victim->CalculateMagicResistanceMitigation(victim, victim->CalculateEffectiveMagicResistance(victim, schoolMask), false);
        const SpellPartialResistChanceEntry &chances = SPELL_PARTIAL_RESIST_DISTRIBUTION.at(uint32(mitigation * 10000));
        // We choose which portion of damage is resisted below, none by default
        uint8 portion = SPELL_PARTIAL_RESIST_NONE;
        // If we got to this point, we already rolled for full resist on hit
        // We do a roll between remaining chances
        const uint8 outcomes = (NUM_SPELL_PARTIAL_RESISTS - 1);
        const uint32 roll = urand(1, (10000 - chances.at(SPELL_PARTIAL_RESIST_PCT_100)));
        uint32 sum = 0;
        for (uint8 outcome = SPELL_PARTIAL_RESIST_NONE; outcome < outcomes; ++outcome)
        {
            if (const uint32 chance = chances.at(outcome))
            {
                sum += chance;
                if (roll <= sum)
                {
                    portion = outcome;
                    break;
                }
            }
        }
        const uint32 amount = uint32(damage * (portion * (1.0f / float(outcomes))));
        // We already rolled for full resist on hit, so we need to deal at least *some* amount of damage...
        *resist = (amount >= damage) ? (damage - 1) : amount;
    }
    else
        *resist = 0;

    int32 RemainingDamage = damage - *resist;

    // Paladin Blessed Life 4/7/10% chance to cause 50% dmg
    AuraList const& blessedLife = victim->GetAurasByType(SPELL_AURA_REUSED_BLESSED_LIFE);
    AuraList::const_iterator blessedAura = blessedLife.begin();
    if (blessedAura != blessedLife.end() && *blessedAura)
    {
        if (urand(0, 100) <= (*blessedAura)->GetSpellProto()->procChance)
            RemainingDamage /= 2;
    }

    // Need to remove expired auras after
    bool expiredExists = false;

    // absorb without mana cost
    int32 reflectDamage = 0;
    Aura* reflectAura = NULL;
    AuraList const& vSchoolAbsorb = victim->GetAurasByType(SPELL_AURA_SCHOOL_ABSORB);
    for (AuraList::const_iterator i = vSchoolAbsorb.begin(); i != vSchoolAbsorb.end() && RemainingDamage > 0; ++i)
    {
        int32* p_absorbAmount = &(*i)->GetModifier()->m_amount;

        // should not happen....
        if (*p_absorbAmount <= 0)
        {
            expiredExists = true;
            continue;
        }

        if (((*i)->GetModifier()->m_miscvalue & schoolMask) == 0)
            continue;

        // Cheat Death
        if ((*i)->GetSpellProto()->SpellFamilyName == SPELLFAMILY_ROGUE && (*i)->GetSpellProto()->SpellIconID == 2109)
        {
            if (victim->ToPlayer()->HasSpellCooldown(31231))
                continue;
            if (int32(victim->GetHealth()) <= RemainingDamage)
            {
                int32 chance = *p_absorbAmount;
                if (roll_chance_i(chance))
                {
                    victim->CastSpell(victim, 31231, true);
                    victim->ToPlayer()->AddSpellCooldown(31231, 0, time(NULL) + 60);

                    // with health > 10% lost health until health == 10%, in other case no losses
                    uint32 health10 = victim->CountPctFromMaxHealth(10);
                    RemainingDamage = victim->GetHealth() > health10 ? victim->GetHealth() - health10 : 0;
                }
            }
            continue;
        }

        int32 currentAbsorb;

        //Reflective Shield
        if ((victim != this) && (*i)->GetSpellProto()->SpellFamilyName == SPELLFAMILY_PRIEST && (*i)->GetSpellProto()->SpellFamilyFlags == 0x1)
        {
            if (Unit* caster = (*i)->GetCaster())
            {
                AuraList const& vOverRideCS = caster->GetAurasByType(SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
                for (AuraList::const_iterator k = vOverRideCS.begin(); k != vOverRideCS.end(); ++k)
                {
                    switch ((*k)->GetModifier()->m_miscvalue)
                    {
                    case 5065:                          // Rank 1
                    case 5064:                          // Rank 2
                    case 5063:                          // Rank 3
                    case 5062:                          // Rank 4
                    case 5061:                          // Rank 5
                        {
                            if (RemainingDamage >= *p_absorbAmount)
                                reflectDamage = *p_absorbAmount * (*k)->GetModifier()->m_amount / 100;
                            else
                                reflectDamage = (*k)->GetModifier()->m_amount * RemainingDamage / 100;
                            reflectAura = *i;

                        }
                        break;
                    default:
                        break;
                    }

                    if (reflectDamage)
                        break;
                }
            }
        }

        if (RemainingDamage >= *p_absorbAmount)
        {
            currentAbsorb = *p_absorbAmount;
            expiredExists = true;
        }
        else
            currentAbsorb = RemainingDamage;

        *p_absorbAmount -= currentAbsorb;
        RemainingDamage -= currentAbsorb;
    }
    // do not cast spells while looping auras; auras can get invalid otherwise
    if (reflectDamage)
        victim->CastCustomSpell(this, 33619, &reflectDamage, NULL, NULL, true, NULL, reflectAura);

    // Remove all expired absorb auras
    if (expiredExists)
    {
        for (AuraList::const_iterator i = vSchoolAbsorb.begin(); i != vSchoolAbsorb.end();)
        {
            Aura* aur = (*i);
            ++i;
            if (aur->GetModifier()->m_amount <= 0)
            {
                uint32 removedAuras = victim->m_removedAurasCount;
                victim->RemoveAurasDueToSpell(aur->GetId());
                if (removedAuras + 1 < victim->m_removedAurasCount)
                    i = vSchoolAbsorb.begin();
            }
        }
    }

    // absorb by mana cost
    AuraList const& vManaShield = victim->GetAurasByType(SPELL_AURA_MANA_SHIELD);
    for (AuraList::const_iterator i = vManaShield.begin(), next; i != vManaShield.end() && RemainingDamage > 0; i = next)
    {
        next = i;
        ++next;
        int32* p_absorbAmount = &(*i)->GetModifier()->m_amount;

        // check damage school mask
        if (((*i)->GetModifier()->m_miscvalue & schoolMask) == 0)
            continue;

        int32 currentAbsorb;
        if (RemainingDamage >= *p_absorbAmount)
            currentAbsorb = *p_absorbAmount;
        else
            currentAbsorb = RemainingDamage;

        float manaMultiplier = (*i)->GetSpellProto()->EffectMultipleValue[(*i)->GetEffIndex()];
        if (Player* modOwner = GetSpellModOwner())
            modOwner->ApplySpellMod((*i)->GetId(), SPELLMOD_MULTIPLE_VALUE, manaMultiplier);

        if (manaMultiplier)
        {
            int32 maxAbsorb = int32(victim->GetPower(POWER_MANA) / manaMultiplier);
            if (currentAbsorb > maxAbsorb)
                currentAbsorb = maxAbsorb;
        }

        *p_absorbAmount -= currentAbsorb;
        if (*p_absorbAmount <= 0)
        {
            victim->RemoveAurasDueToSpell((*i)->GetId());
            next = vManaShield.begin();
        }

        int32 manaReduction = int32(currentAbsorb * manaMultiplier);
        victim->ApplyPowerMod(POWER_MANA, manaReduction, false);

        RemainingDamage -= currentAbsorb;
    }

    // only split damage if not damaging yourself
    if (victim != this)
    {
        AuraList const& vSplitDamageFlat = victim->GetAurasByType(SPELL_AURA_SPLIT_DAMAGE_FLAT);
        for (AuraList::const_iterator i = vSplitDamageFlat.begin(), next; i != vSplitDamageFlat.end() && RemainingDamage >= 0; i = next)
        {
            next = i;
            ++next;

            // check damage school mask
            if (((*i)->GetModifier()->m_miscvalue & schoolMask) == 0)
                continue;

            // Damage can be splitted only if aura has an alive caster
            Unit* caster = (*i)->GetCaster();
            if (!caster || caster == victim || !caster->IsInWorld() || !caster->IsAlive() || caster->IsImmunedToDamage((SpellSchoolMask)(*i)->GetSpellProto()->SchoolMask))
                continue;

            int32 currentAbsorb;
            if (RemainingDamage >= (*i)->GetModifier()->m_amount)
                currentAbsorb = (*i)->GetModifier()->m_amount;
            else
                currentAbsorb = RemainingDamage;

            RemainingDamage -= currentAbsorb;

            SendSpellNonMeleeDamageLog(caster, (*i)->GetSpellProto()->Id, currentAbsorb, schoolMask, 0, 0, false, 0, false);

            CleanDamage cleanDamage = CleanDamage(currentAbsorb, BASE_ATTACK, MELEE_HIT_NORMAL);
            DealDamage(caster, currentAbsorb, &cleanDamage, DOT, schoolMask, (*i)->GetSpellProto(), false);
        }

        AuraList const& vSplitDamagePct = victim->GetAurasByType(SPELL_AURA_SPLIT_DAMAGE_PCT);
        for (AuraList::const_iterator i = vSplitDamagePct.begin(), next; i != vSplitDamagePct.end() && RemainingDamage >= 0; i = next)
        {
            next = i;
            ++next;

            // check damage school mask
            if (!((*i)->GetModifier()->m_miscvalue & schoolMask))
                continue;

            // Damage can only be split if the aura has an alive caster linked
            Unit* caster = (*i)->GetCaster();
            if (!caster || caster == victim || !caster->IsInWorld() || !caster->IsAlive() || caster->IsImmunedToDamage((SpellSchoolMask)(*i)->GetSpellProto()->SchoolMask))
                continue;

            uint32 splitted = CalculatePct(RemainingDamage, (*i)->GetModifier()->m_amount);

            RemainingDamage -= int32(splitted);

            SendSpellNonMeleeDamageLog(caster, (*i)->GetSpellProto()->Id, splitted, schoolMask, 0, 0, false, 0, false);

            CleanDamage cleanDamage = CleanDamage(splitted, BASE_ATTACK, MELEE_HIT_NORMAL);
            DealDamage(caster, splitted, &cleanDamage, DOT, schoolMask, (*i)->GetSpellProto(), false);
            // break 'Fear' and similar auras
            caster->ProcDamageAndSpellFor(true, this, PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_NEG, PROC_EX_NORMAL_HIT, BASE_ATTACK, (*i)->GetSpellProto(), splitted);
        }
    }

    *absorb = damage - RemainingDamage - *resist;
}

void Unit::AttackerStateUpdate (Unit* victim, WeaponAttackType attType, bool extra)
{
    if ((!extra && HasUnitState(UNIT_STATE_CANNOT_AUTOATTACK)) || HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED))
        return;

    if (!victim->IsAlive())
        return;

    if ((attType == BASE_ATTACK || attType == OFF_ATTACK) && !IsWithinLOSInMap(victim))
        return;

    RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_ATTACK);

    if (attType != BASE_ATTACK && attType != OFF_ATTACK)
        return;                                             // ignore ranged case

    // melee attack spell casted at main hand attack only
    if (attType == BASE_ATTACK && m_currentSpells[CURRENT_MELEE_SPELL] && !extra)
    {
        m_currentSpells[CURRENT_MELEE_SPELL]->cast();
        return;
    }

    CalcDamageInfo damageInfo;
    CalculateMeleeDamage(victim, 0, &damageInfo, attType);
    // Send log damage message to client
    SendAttackStateUpdate(&damageInfo);

    ProcDamageAndSpell(damageInfo.target, damageInfo.procAttacker, damageInfo.procVictim, damageInfo.procEx, damageInfo.damage, damageInfo.attackType);

    DealMeleeDamage(&damageInfo, true);

    // We call this last because DealMeleeDamage has to check for sit state to
    // allow critical hits, but we set STANDING in CombatStart, therefore always
    // nullifying the check.
    CombatStart(victim);

    #ifdef OREGON_DEBUG
    if (GetTypeId() == TYPEID_PLAYER)
        DEBUG_LOG("AttackerStateUpdate: (Player) %u attacked %u (TypeId: %u) for %u dmg, absorbed %u, blocked %u, resisted %u.",
                  GetGUIDLow(), victim->GetGUIDLow(), victim->GetTypeId(), damageInfo.damage, damageInfo.absorb, damageInfo.blocked_amount, damageInfo.resist);
    else
        DEBUG_LOG("AttackerStateUpdate: (NPC)    %u attacked %u (TypeId: %u) for %u dmg, absorbed %u, blocked %u, resisted %u.",
                  GetGUIDLow(), victim->GetGUIDLow(), victim->GetTypeId(), damageInfo.damage, damageInfo.absorb, damageInfo.blocked_amount, damageInfo.resist);
    #endif

}

MeleeHitOutcome Unit::RollMeleeOutcomeAgainst(const Unit* victim, WeaponAttackType attType) const
{
    // This is only wrapper

    // Miss chance based on melee
    //float miss_chance = MeleeMissChanceCalc(victim, attType);
    float miss_chance = MeleeSpellMissChance(victim, attType, int32(GetWeaponSkillValue(attType, victim)) - int32(victim->GetDefenseSkillValue(this)), 0);

    // Critical hit chance
    float crit_chance = GetUnitCriticalChance(attType, victim);

    // stunned target cannot dodge and this is check in GetUnitDodgeChance() (returned 0 in this case)
    float dodge_chance = victim->GetUnitDodgeChance();
    float block_chance = victim->GetUnitBlockChance();
    float parry_chance = victim->GetUnitParryChance();

    // Useful if want to specify crit & miss chances for melee, else it could be removed
    DEBUG_LOG("MELEE OUTCOME: miss %f crit %f dodge %f parry %f block %f", miss_chance, crit_chance, dodge_chance, parry_chance, block_chance);

    return RollMeleeOutcomeAgainst(victim, attType, int32(crit_chance * 100), int32(miss_chance * 100), int32(dodge_chance * 100), int32(parry_chance * 100), int32(block_chance * 100), false);
}

MeleeHitOutcome Unit::RollMeleeOutcomeAgainst (const Unit* victim, WeaponAttackType attType, int32 crit_chance, int32 miss_chance, int32 dodge_chance, int32 parry_chance, int32 block_chance, bool SpellCasted) const
{
    if (victim->GetTypeId() == TYPEID_UNIT && victim->ToCreature()->IsInEvadeMode())
        return MELEE_HIT_EVADE;

    int32 attackerMaxSkillValueForLevel = GetMaxSkillValueForLevel(victim);
    int32 victimMaxSkillValueForLevel = victim->GetMaxSkillValueForLevel(this);

    int32 attackerWeaponSkill = GetWeaponSkillValue(attType, victim);
    int32 victimDefenseSkill = victim->GetDefenseSkillValue(this);

    // bonus from skills is 0.04%
    int32    skillBonus  = 4 * (attackerWeaponSkill - victimMaxSkillValueForLevel);
    int32    skillBonus2 = 4 * (attackerMaxSkillValueForLevel - victimDefenseSkill);
    int32    sum = 0, tmp = 0;
    int32    roll = urand (0, 10000);

    DEBUG_LOG ("RollMeleeOutcomeAgainst: skill bonus of %d for attacker", skillBonus);
    DEBUG_LOG ("RollMeleeOutcomeAgainst: rolled %d, miss %d, dodge %d, parry %d, block %d, crit %d",
               roll, miss_chance, dodge_chance, parry_chance, block_chance, crit_chance);

    tmp = miss_chance;

    if (tmp > 0 && roll < (sum += tmp))
    {
        DEBUG_LOG ("RollMeleeOutcomeAgainst: MISS");
        return MELEE_HIT_MISS;
    }

    // always crit against a sitting target (except 0 crit chance)
    if (victim->GetTypeId() == TYPEID_PLAYER && crit_chance > 0 && !victim->IsStandState())
    {
        DEBUG_LOG ("RollMeleeOutcomeAgainst: CRIT (sitting victim)");
        return MELEE_HIT_CRIT;
    }

    // Dodge chance

    // only players can't dodge if attacker is behind
    if (victim->GetTypeId() == TYPEID_PLAYER && !victim->HasInArc(float(M_PI), this))
        DEBUG_LOG ("RollMeleeOutcomeAgainst: attack came from behind and victim was a player.");
    else
    {
        // Reduce dodge chance by attacker expertise rating
        if (GetTypeId() == TYPEID_PLAYER)
            dodge_chance -= int32(ToPlayer()->GetExpertiseDodgeOrParryReduction(attType) * 100);
        else
            dodge_chance -= GetTotalAuraModifier(SPELL_AURA_MOD_EXPERTISE) * 25;

        // Modify dodge chance by attacker SPELL_AURA_MOD_COMBAT_RESULT_CHANCE
        dodge_chance += GetTotalAuraModifierByMiscValue(SPELL_AURA_MOD_COMBAT_RESULT_CHANCE, VICTIMSTATE_DODGE) * 100;
        dodge_chance = int32 (float (dodge_chance) * GetTotalAuraMultiplier(SPELL_AURA_MOD_ENEMY_DODGE));

        tmp = dodge_chance;
        if ((tmp > 0)                                        // check if unit _can_ dodge
            && ((tmp -= skillBonus) > 0)
            && roll < (sum += tmp))
        {
            DEBUG_LOG ("RollMeleeOutcomeAgainst: DODGE <%d, %d)", sum - tmp, sum);
            return MELEE_HIT_DODGE;
        }
    }

    // parry & block chances

    // check if attack comes from behind, nobody can parry or block if attacker is behind
    if (!victim->HasInArc(float(M_PI), this))
        DEBUG_LOG ("RollMeleeOutcomeAgainst: attack came from behind.");
    else
    {
        // Reduce parry chance by attacker expertise rating
        if (GetTypeId() == TYPEID_PLAYER)
            parry_chance -= int32(ToPlayer()->GetExpertiseDodgeOrParryReduction(attType) * 100);
        else
            parry_chance -= GetTotalAuraModifier(SPELL_AURA_MOD_EXPERTISE) * 25;

        if (victim->GetTypeId() == TYPEID_PLAYER || !(victim->ToCreature()->GetCreatureTemplate()->flags_extra & CREATURE_FLAG_EXTRA_NO_PARRY))
        {
            int32 tmp2 = int32(parry_chance);
            if (tmp2 > 0                                         // check if unit _can_ parry
                && (tmp2 -= skillBonus) > 0
                && roll < (sum += tmp2))
            {
                DEBUG_LOG ("RollMeleeOutcomeAgainst: PARRY <%d, %d)", sum - tmp2, sum);
                return MELEE_HIT_PARRY;
            }
        }

        if (victim->GetTypeId() == TYPEID_PLAYER || !(victim->ToCreature()->GetCreatureTemplate()->flags_extra & CREATURE_FLAG_EXTRA_NO_BLOCK))
        {
            tmp = block_chance;
            if (tmp > 0                                          // check if unit _can_ block
                && (tmp -= skillBonus) > 0
                && roll < (sum += tmp))
            {
                // Critical chance
                tmp = crit_chance + skillBonus2;
                if (GetTypeId() == TYPEID_PLAYER && SpellCasted && tmp > 0)
                {
                    if (roll_chance_i(tmp / 100))
                    {
                        DEBUG_LOG ("RollMeleeOutcomeAgainst: BLOCKED CRIT");
                        return MELEE_HIT_BLOCK_CRIT;
                    }
                }
                DEBUG_LOG ("RollMeleeOutcomeAgainst: BLOCK <%d, %d)", sum - tmp, sum);
                return MELEE_HIT_BLOCK;
            }
        }
    }

    // Critical chance
    tmp = crit_chance + skillBonus2;

    if (tmp > 0 && roll < (sum += tmp))
    {
        DEBUG_LOG ("RollMeleeOutcomeAgainst: CRIT <%d, %d)", sum - tmp, sum);
        if (GetTypeId() == TYPEID_UNIT && (ToCreature()->GetCreatureTemplate()->flags_extra & CREATURE_FLAG_EXTRA_NO_CRIT))
            DEBUG_LOG ("RollMeleeOutcomeAgainst: CRIT DISABLED)");
        else
            return MELEE_HIT_CRIT;
    }

    // Max 40% chance to score a glancing blow against mobs that are higher level (can do only players and pets and not with ranged weapon)
    if (attType != RANGED_ATTACK && !SpellCasted &&
        (GetTypeId() == TYPEID_PLAYER || IsPet()) &&
        victim->GetTypeId() != TYPEID_PLAYER && !victim->IsPet() &&
        getLevel() < victim->getLevelForTarget(this))
    {
        // cap possible value (with bonuses > max skill)
        int32 skill = attackerWeaponSkill;
        int32 maxskill = attackerMaxSkillValueForLevel;
        skill = (skill > maxskill) ? maxskill : skill;

        tmp = (10 + (victimDefenseSkill - skill)) * 100;
        tmp = tmp > 4000 ? 4000 : tmp;
        if (roll < (sum += tmp))
        {
            DEBUG_LOG ("RollMeleeOutcomeAgainst: GLANCING <%d, %d)", sum - 4000, sum);
            return MELEE_HIT_GLANCING;
        }
    }

    // mobs can only score crushing blows with autoattack
    if (!SpellCasted && !IsControlledByPlayer() &&
        !(GetTypeId() == TYPEID_UNIT && ToCreature()->GetCreatureTemplate()->flags_extra & CREATURE_FLAG_EXTRA_NO_CRUSH))
    {
        // when their weapon skill is 15 or more above victim's defense skill
        tmp = victimDefenseSkill;
        int32 tmpmax = victimMaxSkillValueForLevel;
        // having defense above your maximum (from items, talents etc.) has no effect
        tmp = tmp > tmpmax ? tmpmax : tmp;
        // tmp = mob's level * 5 - player's current defense skill
        tmp = attackerMaxSkillValueForLevel - tmp;
        if (tmp >= 15)
        {
            // add 2% chance per lacking skill point, min. is 15%
            tmp = tmp * 200 - 1500;
            if (roll < (sum += tmp))
            {
                DEBUG_LOG ("RollMeleeOutcomeAgainst: CRUSHING <%d, %d)", sum - tmp, sum);
                return MELEE_HIT_CRUSHING;
            }
        }
    }

    DEBUG_LOG ("RollMeleeOutcomeAgainst: NORMAL");
    return MELEE_HIT_NORMAL;
}

uint32 Unit::CalculateDamage(WeaponAttackType attType, bool normalized, bool addTotalPct)
{
    float minDamage = 0.0f;
    float maxDamage = 0.0f;

    if (normalized && GetTypeId() == TYPEID_PLAYER)
        ToPlayer()->CalculateMinMaxDamage(attType, normalized, addTotalPct, minDamage, maxDamage);
    else
    {
        switch (attType)
        {
            case RANGED_ATTACK:
                minDamage = GetFloatValue(UNIT_FIELD_MINRANGEDDAMAGE);
                maxDamage = GetFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE);
                break;
            case BASE_ATTACK:
                minDamage = GetFloatValue(UNIT_FIELD_MINDAMAGE);
                maxDamage = GetFloatValue(UNIT_FIELD_MAXDAMAGE);
                break;
            case OFF_ATTACK:
                minDamage = GetFloatValue(UNIT_FIELD_MINOFFHANDDAMAGE);
                maxDamage = GetFloatValue(UNIT_FIELD_MAXOFFHANDDAMAGE);
                break;
            default:
                break;
        }
    }

    minDamage = std::max(0.f, minDamage);
    maxDamage = std::max(0.f, maxDamage);

    if (minDamage > maxDamage)
        std::swap(minDamage, maxDamage);

    if (maxDamage == 0.0f)
        maxDamage = 5.0f;

    return urand(uint32(minDamage), uint32(maxDamage));
}

float Unit::CalculateLevelPenalty(SpellEntry const* spellProto) const
{
    if (spellProto->spellLevel <= 0 || spellProto->spellLevel >= spellProto->maxLevel)
        return 1.0f;

    float LvlPenalty = 0.0f;

    if (spellProto->spellLevel < 20)
        LvlPenalty = (20.0f - spellProto->spellLevel) * 3.75f;

    float LvlFactor = (float(spellProto->spellLevel) + 6.0f) / float(getLevel());
    if (LvlFactor > 1.0f)
        LvlFactor = 1.0f;

    return (100.0f - LvlPenalty) * LvlFactor / 100.0f;
}

void Unit::SendMeleeAttackStart(Unit* victim)
{
    WorldPacket data(SMSG_ATTACKSTART, 8 + 8);
    data << uint64(GetGUID());
    data << uint64(victim->GetGUID());
    SendMessageToSet(&data, true);
    DEBUG_LOG("WORLD: Sent SMSG_ATTACKSTART");
}

void Unit::SendMeleeAttackStop(Unit* victim)
{
    WorldPacket data(SMSG_ATTACKSTOP, (8+8+4));
    data << GetPackGUID();
    if (victim)
        data << victim->GetPackGUID();
    else
        data << uint8(0);

    data << uint32(0);                                         //! Can also take the value 0x01, which seems related to updating rotation
    SendMessageToSet(&data, true);
    sLog.outDebug("WORLD: Sent SMSG_ATTACKSTOP");

    if (victim)
        sLog.outDetail("%s %u stopped attacking %s %u", (GetTypeId() == TYPEID_PLAYER ? "player" : "creature"), GetGUIDLow(), (victim->GetTypeId() == TYPEID_PLAYER ? "player" : "creature"), victim->GetGUIDLow());
    else
        sLog.outDetail("%s %u stopped attacking", (GetTypeId() == TYPEID_PLAYER ? "Player" : "Creature"), GetGUIDLow());
}

bool Unit::isSpellBlocked(Unit* victim, SpellEntry const* spellProto, WeaponAttackType attackType)
{
    // These spells can't be blocked
    if (spellProto && spellProto->Attributes & SPELL_ATTR0_IMPOSSIBLE_DODGE_PARRY_BLOCK)
        return false;

    if (victim->HasInArc(float(M_PI), this))
    {
        // Check creatures flags_extra for disable block
        if (victim->GetTypeId() == TYPEID_UNIT &&
            victim->ToCreature()->GetCreatureTemplate()->flags_extra & CREATURE_FLAG_EXTRA_NO_BLOCK)
                return false;

        float blockChance = victim->GetUnitBlockChance();

        float fAttackerSkill = GetWeaponSkillValue(attackType, victim) * 0.04f;
        float fDefenserSkill = victim->GetDefenseSkillValue(this) * 0.04f;

        blockChance += (fDefenserSkill - fAttackerSkill);
        if (blockChance < 0.0)
            blockChance = 0.0;

        if (roll_chance_f(blockChance))
            return true;
    }
    return false;
}

// Melee based spells can be miss, parry or dodge on this step
// Crit or block - determined on damage calculation phase! (and can be both in some time)
float Unit::MeleeSpellMissChance(const Unit* victim, WeaponAttackType attType, int32 skillDiff, uint32 spellId) const
{
    // Calculate hit chance (more correct for chance mod)
    float HitChance = 0.0f;

    // PvP - PvE melee chances
    if (victim->GetTypeId() == TYPEID_PLAYER)
        HitChance = 95.0f + skillDiff * 0.04f;
    else if (skillDiff < -10)
        HitChance = 93.0f + (skillDiff + 10) * 0.4f;        // 7% base chance to miss for big skill diff (%6 in 3.x)
    else
        HitChance = 95.0f + skillDiff * 0.1f;

    // Hit chance depends from victim auras
    if (attType == RANGED_ATTACK)
        HitChance += victim->GetTotalAuraModifier(SPELL_AURA_MOD_ATTACKER_RANGED_HIT_CHANCE);
    else
        HitChance += victim->GetTotalAuraModifier(SPELL_AURA_MOD_ATTACKER_MELEE_HIT_CHANCE);

    // Spellmod from SPELLMOD_RESIST_MISS_CHANCE
    if (spellId)
    {
        if (Player* modOwner = GetSpellModOwner())
            modOwner->ApplySpellMod(spellId, SPELLMOD_RESIST_MISS_CHANCE, HitChance);
    }

    // Miss = 100 - hit
    float miss_chance = 100.0f - HitChance;

    // Bonuses from attacker aura and ratings
    if (attType == RANGED_ATTACK)
        miss_chance -= m_modRangedHitChance;
    else
        miss_chance -= m_modMeleeHitChance;

    // bonus from skills is 0.04%
    //miss_chance -= skillDiff * 0.04f;
    int32 diff = -skillDiff;
    if (victim->GetTypeId() == TYPEID_PLAYER)
        miss_chance += diff > 0 ? diff * 0.04 : diff * 0.02;
    else
        miss_chance += diff > 10 ? 2 + (diff - 10) * 0.4 : diff * 0.1;

    // Limit miss chance from 0 to 60%
    if (miss_chance < 0.0f)
        return 0.0f;
    if (miss_chance > 60.0f)
        return 60.0f;
    return miss_chance;
}

int32 Unit::GetMechanicResistChance(const SpellEntry* spellInfo)
{
    if (!spellInfo)
        return 0;

    int32 resistMech = 0;
    for (uint8 eff = 0; eff < MAX_SPELL_EFFECTS; ++eff)
    {
        if (spellInfo->Effect[eff] == 0)
            break;

        int32 effectMech = GetEffectMechanic(spellInfo, eff);
        if (effectMech)
        {
            int32 temp = GetTotalAuraModifierByMiscValue(SPELL_AURA_MOD_MECHANIC_RESISTANCE, effectMech);
            if (resistMech < temp)
                resistMech = temp;
        }
    }
    return resistMech;
}

bool Unit::CanUseAttackType(uint8 attacktype) const
{
    switch (attacktype)
    {
        case BASE_ATTACK:
            return !HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISARMED);
        /* @todo Implement unit flags 2 properly
        case OFF_ATTACK:
            return !HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISARM_OFFHAND);
        case RANGED_ATTACK:
            return !HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISARM_RANGED);*/
        default:
            return true;
    }
}

// Melee based spells hit result calculations
SpellMissInfo Unit::MeleeSpellHitResult(Unit* victim, SpellEntry const* spellInfo, bool cMiss)
{
    // Spells with SPELL_ATTR3_IGNORE_HIT_RESULT will additionally fully ignore
    // resist and deflect chances
    if (spellInfo->AttributesEx3 & SPELL_ATTR3_CANT_MISS)
        return SPELL_MISS_NONE;

    WeaponAttackType attType = BASE_ATTACK;

    // Check damage class instead of attack type to correctly handle judgements
    // - they are meele, but can't be dodged/parried/deflected because of ranged dmg class
    if (spellInfo->DmgClass == SPELL_DAMAGE_CLASS_RANGED)
        attType = RANGED_ATTACK;

    int32 attackerWeaponSkill;
    // skill value for these spells (for example judgements) is 5* level
    if (spellInfo->DmgClass == SPELL_DAMAGE_CLASS_RANGED && !(spellInfo->Attributes & SPELL_ATTR0_RANGED))
        attackerWeaponSkill = getLevel() * 5;
    // bonus from skills is 0.04% per skill Diff
    else
        attackerWeaponSkill = int32(GetWeaponSkillValue(attType, victim));

    int32 skillDiff = attackerWeaponSkill - int32(victim->GetMaxSkillValueForLevel(this));
    int32 fullSkillDiff = attackerWeaponSkill - int32(victim->GetDefenseSkillValue(this));

    uint32 roll = urand (0, 10000);
    uint32 tmp = 0;

    bool canDodge = true;
    bool canParry = true;
    bool canBlock = (spellInfo->AttributesEx3 & SPELL_ATTR3_CAN_BE_BLOCKED) != 0;
    bool canMiss = !(spellInfo->AttributesEx3 & SPELL_ATTR3_CANT_MISS) && cMiss;

    if (canMiss)
    {
        uint32 missChance = uint32(MeleeSpellMissChance(victim, attType, fullSkillDiff, spellInfo->Id) * 100.0f);
        // Roll miss
        tmp += missChance;
        if (roll < tmp)
            return SPELL_MISS_MISS;
    }

    // Chance resist mechanic
    int32 resist_chance = victim->GetMechanicResistChance(spellInfo) * 100;
    tmp += resist_chance;
    if (roll < tmp)
        return SPELL_MISS_RESIST;

    // @XXX: Tidal Charm: Increased chance to be resisted when used against targets over level 60.
    if (spellInfo->Id == 835)
    {
        if (victim->getLevel() > 60)
        {
            uint32 tmp = victim->getLevel();
            uint32 rand = urand(60, 160);

            if (rand > tmp)
                return SPELL_MISS_RESIST;
        }
    }

    // Ranged attack can`t miss too
    if (attType == RANGED_ATTACK)
        return SPELL_MISS_NONE;

    // Some spells cannot be parry/dodge
    if (spellInfo->Attributes & SPELL_ATTR0_IMPOSSIBLE_DODGE_PARRY_BLOCK)
        return SPELL_MISS_NONE;

    // Check for attack from behind
    // This is the only Dodge/Block check that can't be done from behind
    // based on unit type.
    if (!victim->HasInArc(float(M_PI), this))
    {
        // Can`t dodge from behind in PvP (but its possible in PvE)
        if (victim->GetTypeId() == TYPEID_PLAYER)
            canDodge = false;
        // Can`t parry
        canParry = false;
        // Can't block from behind in PvE (but its possible in PvP)
        if (victim->GetTypeId() == TYPEID_UNIT)
            canBlock = false;
    }

    // Check creatures flags_extra for disable parry
    if (victim->GetTypeId() == TYPEID_UNIT)
    {
        uint32 flagEx = victim->ToCreature()->GetCreatureTemplate()->flags_extra;
        if (flagEx & CREATURE_FLAG_EXTRA_NO_PARRY)
            canParry = false;
        // Check creatures flags_extra for disable block
        if (flagEx & CREATURE_FLAG_EXTRA_NO_BLOCK)
            canBlock = false;
    }

    // Ignore combat result aura
    AuraList const& mCanNotBeDodge = GetAurasByType(SPELL_AURA_IGNORE_COMBAT_RESULT);
    for (AuraList::const_iterator i = mCanNotBeDodge.begin(); i != mCanNotBeDodge.end(); ++i)
    {
        switch ((*i)->GetMiscValue())
        {
            case MELEE_HIT_DODGE:
                canDodge = false;
                break;
            case MELEE_HIT_BLOCK:
                canBlock = false;
                break;
            case MELEE_HIT_PARRY:
                canParry = false;
                break;
            default:
                DEBUG_LOG("Spell %u SPELL_AURA_IGNORE_COMBAT_RESULT has unhandled state %d", (*i)->GetId(), (*i)->GetMiscValue());
                break;
        }
    }

    if (canDodge)
    {
        // Roll dodge
        int32 dodgeChance = int32(victim->GetUnitDodgeChance() * 100.0f) - skillDiff * 4;
        // Reduce enemy dodge chance by SPELL_AURA_MOD_COMBAT_RESULT_CHANCE
        dodgeChance += GetTotalAuraModifierByMiscValue(SPELL_AURA_MOD_COMBAT_RESULT_CHANCE, VICTIMSTATE_DODGE) * 100;
        dodgeChance = int32(float(dodgeChance) * GetTotalAuraMultiplier(SPELL_AURA_MOD_ENEMY_DODGE));
        // Reduce dodge chance by attacker expertise rating
        if (GetTypeId() == TYPEID_PLAYER)
            dodgeChance -= int32(ToPlayer()->GetExpertiseDodgeOrParryReduction(attType) * 100.0f);
        else
            dodgeChance -= GetTotalAuraModifier(SPELL_AURA_MOD_EXPERTISE) * 25;
        if (dodgeChance < 0)
            dodgeChance = 0;

        if (roll < (tmp += dodgeChance))
            return SPELL_MISS_DODGE;
    }

    if (canParry)
    {
        // Roll parry
        int32 parryChance = int32(victim->GetUnitParryChance() * 100.0f)  - skillDiff * 4;
        // Reduce parry chance by attacker expertise rating
        if (GetTypeId() == TYPEID_PLAYER)
            parryChance -= int32(ToPlayer()->GetExpertiseDodgeOrParryReduction(attType) * 100.0f);
        else
            parryChance -= GetTotalAuraModifier(SPELL_AURA_MOD_EXPERTISE) * 25;
        if (parryChance < 0)
            parryChance = 0;

        tmp += parryChance;
        if (roll < tmp)
            return SPELL_MISS_PARRY;
    }

    if (canBlock)
    {
        int32 blockChance = int32(victim->GetUnitBlockChance() * 100.0f)  - skillDiff * 4;
        if (blockChance < 0)
            blockChance = 0;
        tmp += blockChance;

        if (roll < tmp)
            return SPELL_MISS_BLOCK;
    }

    return SPELL_MISS_NONE;
}

/// @todo need use unit spell resistances in calculations
SpellMissInfo Unit::MagicSpellHitResult(Unit* victim, SpellEntry const* spellInfo)
{
    // Can`t miss on dead target (on skinning for example)
    if ((!victim->IsAlive() && victim->GetTypeId() != TYPEID_PLAYER) || spellInfo->AttributesEx3 & SPELL_ATTR3_CANT_MISS)
        return SPELL_MISS_NONE;

    SpellSchoolMask schoolMask = GetSpellSchoolMask(spellInfo);
    // PvP - PvE spell misschances per leveldif > 2
    int32 lchance = victim->GetTypeId() == TYPEID_PLAYER ? 7 : 11;
    int32 thisLevel = getLevelForTarget(victim);
    if (GetTypeId() == TYPEID_UNIT && ToCreature()->IsTrigger())
        thisLevel = std::max<int32>(thisLevel, spellInfo->spellLevel);
    int32 leveldif = int32(victim->getLevelForTarget(this)) - thisLevel;

    // Base hit chance from attacker and victim levels
    int32 modHitChance;
    if (leveldif < 3)
        modHitChance = 96 - leveldif;
    else
        modHitChance = 94 - (leveldif - 2) * lchance;

    // Spellmod from SPELLMOD_RESIST_MISS_CHANCE
    if (Player* modOwner = GetSpellModOwner())
        modOwner->ApplySpellMod(spellInfo->Id, SPELLMOD_RESIST_MISS_CHANCE, modHitChance);

    // Increase from attacker SPELL_AURA_MOD_INCREASES_SPELL_PCT_TO_HIT auras
    modHitChance += GetTotalAuraModifierByMiscMask(SPELL_AURA_MOD_INCREASES_SPELL_PCT_TO_HIT, schoolMask);

    // Chance hit from victim SPELL_AURA_MOD_ATTACKER_SPELL_HIT_CHANCE auras
    modHitChance += victim->GetTotalAuraModifierByMiscMask(SPELL_AURA_MOD_ATTACKER_SPELL_HIT_CHANCE, schoolMask);
    // Reduce spell hit chance for Area of effect spells from victim SPELL_AURA_MOD_AOE_AVOIDANCE aura
    if (IsAreaOfEffectSpell(spellInfo))
        modHitChance -= victim->GetTotalAuraModifier(SPELL_AURA_MOD_AOE_AVOIDANCE);

    // Decrease hit chance from victim rating bonus
    if (victim->GetTypeId() == TYPEID_PLAYER)
        modHitChance -= int32(victim->ToPlayer()->GetRatingBonusValue(CR_HIT_TAKEN_SPELL));

    // Reduce spell hit chance for dispel mechanic spells from victim SPELL_AURA_MOD_DISPEL_RESIST
    if (IsDispelSpell(spellInfo))
        modHitChance -= victim->GetTotalAuraModifier(SPELL_AURA_MOD_DISPEL_RESIST);

    // Chance resist mechanic (select max value from every mechanic spell effect)
    int32 resist_chance = victim->GetMechanicResistChance(spellInfo);
    // Apply mod
    modHitChance -= resist_chance;

    // Chance resist debuff
    modHitChance -= victim->GetTotalAuraModifierByMiscValue(SPELL_AURA_MOD_DEBUFF_RESISTANCE, int32(spellInfo->Dispel));

    int32 HitChance = modHitChance * 100;
    // Increase hit chance from attacker SPELL_AURA_MOD_SPELL_HIT_CHANCE and attacker ratings
    HitChance += int32(m_modSpellHitChance * 100.0f);

    if (HitChance <  100) HitChance =  100;
    if (HitChance > 9900) HitChance = 9900;

    int32 rand = irand(0, 10000);
    if (rand > HitChance)
        return SPELL_MISS_RESIST;

    if (!spellInfo->HasAttribute(SPELL_ATTR4_IGNORE_RESISTANCES))
    {
        const bool binary = IsBinarySpell(spellInfo);
        const float mitigation = victim->CalculateMagicResistanceMitigation(this, victim->CalculateEffectiveMagicResistance(this, schoolMask), binary);
        // Calculate full resist chance
        const uint32 chance = binary ? uint32(mitigation * 10000) : SPELL_PARTIAL_RESIST_DISTRIBUTION.at(uint32(mitigation * 10000)).at(SPELL_PARTIAL_RESIST_PCT_100);
        // Do a roll for full resist chance
        if (urand(0,9999) < chance)
            return SPELL_MISS_RESIST;
    }

    return SPELL_MISS_NONE;
}

// Calculate spell hit result can be:
// Every spell can: Evade/Immune/Reflect/Sucesful hit
// For melee based spells:
//   Miss
//   Dodge
//   Parry
// For spells
//   Resist
SpellMissInfo Unit::SpellHitResult(Unit* victim, SpellEntry const* spell, bool CanReflect, bool canMiss)
{
    // Check for immune (use charges)
    if (victim->IsImmuneToSpell(spell, true))
        return SPELL_MISS_IMMUNE;

    // All positive spells can`t miss
    // @todo client not show miss log for this spells - so need find info for this in dbc and use it!
    if (IsPositiveSpell(spell->Id) && !IsHostileTo(victim)) //prevent from affecting enemy by "positive" spell
        return SPELL_MISS_NONE;

    // Check for immune
    if (victim->IsImmunedToDamage(spell))
        return SPELL_MISS_IMMUNE;

    if (this == victim)
        return SPELL_MISS_NONE;

    // Return evade for units in evade mode
    if (victim->GetTypeId() == TYPEID_UNIT && victim->ToCreature()->IsInEvadeMode() && this != victim)
        return SPELL_MISS_EVADE;

    // Try victim reflect spell
    if (CanReflect)
    {
        int32 reflectchance = victim->GetTotalAuraModifier(SPELL_AURA_REFLECT_SPELLS);
        Unit::AuraList const& mReflectSpellsSchool = victim->GetAurasByType(SPELL_AURA_REFLECT_SPELLS_SCHOOL);
        for (Unit::AuraList::const_iterator i = mReflectSpellsSchool.begin(); i != mReflectSpellsSchool.end(); ++i)
            if ((*i)->GetModifier()->m_miscvalue & GetSpellSchoolMask(spell))
                reflectchance = (*i)->GetModifierValue();
        if (reflectchance > 0 && roll_chance_i(reflectchance))
        {
            // Start triggers for remove charges if need (trigger only for victim, and mark as active spell)
            ProcDamageAndSpell(victim, PROC_FLAG_NONE, PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_NEG, PROC_EX_REFLECT, 1, BASE_ATTACK, spell);
            return SPELL_MISS_REFLECT;
        }
    }

    switch (spell->DmgClass)
    {
    case SPELL_DAMAGE_CLASS_RANGED:
    case SPELL_DAMAGE_CLASS_MELEE:
        return MeleeSpellHitResult(victim, spell, canMiss);
    case SPELL_DAMAGE_CLASS_NONE:
        return SPELL_MISS_NONE;
    case SPELL_DAMAGE_CLASS_MAGIC:
        return MagicSpellHitResult(victim, spell);
    }
    return SPELL_MISS_NONE;
}

float Unit::MeleeMissChanceCalc(const Unit* victim, WeaponAttackType attType) const
{
    if (!victim)
        return 0.0f;

    // Base misschance 5%
    float missChance = 5.0f;

    // DualWield - white damage has additional 19% miss penalty
    if (haveOffhandWeapon() && attType != RANGED_ATTACK)
    {
        bool isNormal = false;
        for (uint32 i = CURRENT_FIRST_NON_MELEE_SPELL; i < CURRENT_MAX_SPELL; ++i)
        {
            if (m_currentSpells[i] && (GetSpellSchoolMask(m_currentSpells[i]->m_spellInfo) & SPELL_SCHOOL_MASK_NORMAL))
            {
                isNormal = true;
                break;
            }
        }
        if (!isNormal && !m_currentSpells[CURRENT_MELEE_SPELL])
            missChance += 19.0f;
    }

    int32 skillDiff = int32(GetWeaponSkillValue(attType, victim)) - int32(victim->GetDefenseSkillValue(this));

    // PvP - PvE melee chances
    if ( victim->GetTypeId() == TYPEID_PLAYER )
        missChance -= skillDiff * 0.04f;
    else if ( skillDiff < -10 )
        missChance -= (skillDiff + 10) * 0.4f - 2.0f;       // 7% base chance to miss for big skill diff (%6 in 3.x)
    else
        missChance -=  skillDiff * 0.1f;

    // Hit chance bonus from attacker based on ratings and auras
    if (attType == RANGED_ATTACK)
        missChance -= m_modRangedHitChance;
    else
        missChance -= m_modMeleeHitChance;

    // Hit chance for victim based on ratings
    if (victim->GetTypeId() == TYPEID_PLAYER)
    {
        if (attType == RANGED_ATTACK)
            missChance += ((Player*)victim)->GetRatingBonusValue(CR_HIT_TAKEN_RANGED);
        else
            missChance += ((Player*)victim)->GetRatingBonusValue(CR_HIT_TAKEN_MELEE);
    }

    // Modify miss chance by victim auras
    if (attType == RANGED_ATTACK)
        missChance -= victim->GetTotalAuraModifier(SPELL_AURA_MOD_ATTACKER_RANGED_HIT_CHANCE);
    else
        missChance -= victim->GetTotalAuraModifier(SPELL_AURA_MOD_ATTACKER_MELEE_HIT_CHANCE);

    // Limit miss chance from 0 to 60%
    if (missChance < 0.0f)
        return 0.0f;
    if (missChance > 60.0f)
        return 60.0f;

    return missChance;
}

uint32 Unit::GetDefenseSkillValue(Unit const* target) const
{
    if (GetTypeId() == TYPEID_PLAYER)
    {
        // in PvP use full skill instead current skill value
        uint32 value = (target && target->GetTypeId() == TYPEID_PLAYER)
                       ? ToPlayer()->GetMaxSkillValue(SKILL_DEFENSE)
                       : ToPlayer()->GetSkillValue(SKILL_DEFENSE);
        value += uint32(ToPlayer()->GetRatingBonusValue(CR_DEFENSE_SKILL));
        return value;
    }
    else
        return GetUnitMeleeSkill(target);
}

float Unit::GetUnitDodgeChance() const
{
    if (IsNonMeleeSpellCast(false) || HasUnitState(UNIT_STATE_LOST_CONTROL))
        return 0.0f;

    if (GetTypeId() == TYPEID_PLAYER)
        return GetFloatValue(PLAYER_DODGE_PERCENTAGE);
    else
    {
        if (IsTotem())
            return 0.0f;
        else
        {
            float dodge = 5.0f;
            dodge += GetTotalAuraModifier(SPELL_AURA_MOD_DODGE_PERCENT);
            return dodge > 0.0f ? dodge : 0.0f;
        }
    }
}

float Unit::GetUnitParryChance() const
{
    if (IsNonMeleeSpellCast(false) || HasUnitState(UNIT_STATE_LOST_CONTROL))
        return 0.0f;

    float chance = 0.0f;

    if (GetTypeId() == TYPEID_PLAYER)
    {
        Player const* player = (Player const*)this;
        if (player->CanParry())
        {
            Item* tmpitem = player->GetWeaponForAttack(BASE_ATTACK, true);
            if (!tmpitem)
                tmpitem = player->GetWeaponForAttack(OFF_ATTACK, true);

            if (tmpitem)
                chance = GetFloatValue(PLAYER_PARRY_PERCENTAGE);
        }
    }
    else if (GetTypeId() == TYPEID_UNIT)
    {
        if (GetCreatureType() == CREATURE_TYPE_HUMANOID)
        {
            chance = 5.0f;
            chance += GetTotalAuraModifier(SPELL_AURA_MOD_PARRY_PERCENT);
        }
    }

    return chance > 0.0f ? chance : 0.0f;
}

float Unit::GetUnitBlockChance() const
{
    if (IsNonMeleeSpellCast(false) || HasUnitState(UNIT_STATE_STUNNED))
        return 0.0f;

    if (GetTypeId() == TYPEID_PLAYER)
    {
        Player const* player = (Player const*)this;
        if (player->CanBlock())
        {
            Item* tmpitem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
            if (tmpitem && !tmpitem->IsBroken() && tmpitem->GetProto()->Block)
                return GetFloatValue(PLAYER_BLOCK_PERCENTAGE);
        }
        // is player but has no block ability or no not broken shield equipped
        return 0.0f;
    }
    else
    {
        if (IsTotem())
            return 0.0f;
        else
        {
            float block = 5.0f;
            block += GetTotalAuraModifier(SPELL_AURA_MOD_BLOCK_PERCENT);
            return block > 0.0f ? block : 0.0f;
        }
    }
}

float Unit::GetUnitCriticalChance(WeaponAttackType attackType, const Unit* victim) const
{
    float crit;

    if (GetTypeId() == TYPEID_PLAYER)
    {
        switch (attackType)
        {
        case BASE_ATTACK:
            crit = GetFloatValue(PLAYER_CRIT_PERCENTAGE);
            break;
        case OFF_ATTACK:
            crit = GetFloatValue(PLAYER_OFFHAND_CRIT_PERCENTAGE);
            break;
        case RANGED_ATTACK:
            crit = GetFloatValue(PLAYER_RANGED_CRIT_PERCENTAGE);
            break;
        // Just for good manner
        default:
            crit = 0.0f;
            break;
        }
    }
    else
    {
        crit = 5.0f;
        crit += GetTotalAuraModifier(SPELL_AURA_MOD_CRIT_PERCENT);
    }

    // flat aura mods
    if (attackType == RANGED_ATTACK)
        crit += victim->GetTotalAuraModifier(SPELL_AURA_MOD_ATTACKER_RANGED_CRIT_CHANCE);
    else
        crit += victim->GetTotalAuraModifier(SPELL_AURA_MOD_ATTACKER_MELEE_CRIT_CHANCE);

    crit += victim->GetTotalAuraModifier(SPELL_AURA_MOD_ATTACKER_SPELL_AND_WEAPON_CRIT_CHANCE);

    // reduce crit chance from Rating for players
    if (victim->GetTypeId() == TYPEID_PLAYER)
    {
        if (attackType == RANGED_ATTACK)
            crit -= victim->ToPlayer()->GetRatingBonusValue(CR_CRIT_TAKEN_RANGED);
        else
            crit -= victim->ToPlayer()->GetRatingBonusValue(CR_CRIT_TAKEN_MELEE);
    }

    if (crit < 0.0f)
        crit = 0.0f;
    return crit;
}

uint32 Unit::GetWeaponSkillValue (WeaponAttackType attType, Unit const* target) const
{
    uint32 value = 0;
    if (GetTypeId() == TYPEID_PLAYER)
    {
        Item* item = ToPlayer()->GetWeaponForAttack(attType, true);

        // feral or unarmed skill only for base attack
        if (attType != BASE_ATTACK && !item)
        {
            if (attType == RANGED_ATTACK && getClass() == CLASS_PALADIN) //hammer
                return GetMaxSkillValueForLevel();
            return 0;
        }

        if (IsInFeralForm())
            return GetMaxSkillValueForLevel();              // always maximized SKILL_FERAL_COMBAT in fact

        // weapon skill or (unarmed for base attack and fist weapons)
        uint32  skill = item && item->GetSkill() != SKILL_FIST_WEAPONS ? item->GetSkill() : uint32(SKILL_UNARMED);

        // in PvP use full skill instead current skill value
        value = (target && target->IsCharmedOwnedByPlayerOrPlayer())
                ? ToPlayer()->GetMaxSkillValue(skill)
                : ToPlayer()->GetSkillValue(skill);
        // Modify value from ratings
        value += uint32(ToPlayer()->GetRatingBonusValue(CR_WEAPON_SKILL));
        switch (attType)
        {
            case BASE_ATTACK:   value += uint32(ToPlayer()->GetRatingBonusValue(CR_WEAPON_SKILL_MAINHAND)); break;
            case OFF_ATTACK:    value += uint32(ToPlayer()->GetRatingBonusValue(CR_WEAPON_SKILL_OFFHAND));  break;
            case RANGED_ATTACK: value += uint32(ToPlayer()->GetRatingBonusValue(CR_WEAPON_SKILL_RANGED));   break;
            default: break;
        }
    }
    else
        value = GetUnitMeleeSkill(target);
    return value;
}

void Unit::_DeleteAuras()
{
    while (!m_removedAuras.empty())
    {
        delete m_removedAuras.front();
        m_removedAuras.pop_front();
    }
}

void Unit::_UpdateSpells(uint32 time)
{
    if (m_currentSpells[CURRENT_AUTOREPEAT_SPELL])
        _UpdateAutoRepeatSpell();

    // remove finished spells from current pointers
    for (uint32 i = 0; i < CURRENT_MAX_SPELL; ++i)
    {
        if (m_currentSpells[i] && m_currentSpells[i]->getState() == SPELL_STATE_FINISHED)
        {
            m_currentSpells[i]->SetReferencedFromCurrent(false);
            m_currentSpells[i] = NULL;                      // remove pointer
        }
    }

    // m_AurasUpdateIterator can be updated in inderect called code at aura remove to skip next planned to update but removed auras
    for (m_AurasUpdateIterator = m_Auras.begin(); m_AurasUpdateIterator != m_Auras.end();)
    {
        Aura* i_aura = m_AurasUpdateIterator->second;
        ++m_AurasUpdateIterator;                            // need shift to next for allow update if need into aura update
        i_aura->Update(time);
    }

    // remove expired auras
    for (AuraMap::iterator i = m_Auras.begin(); i != m_Auras.end();)
    {
        if (i->second->IsExpired())
            RemoveAura(i, AURA_REMOVE_BY_EXPIRE);
        else
            ++i;
    }

    _DeleteAuras();

    if (!m_gameObj.empty())
    {
        std::list<GameObject*>::iterator itr;
        for (itr = m_gameObj.begin(); itr != m_gameObj.end();)
        {
            if (!(*itr)->isSpawned())
            {
                (*itr)->SetOwnerGUID(0);
                (*itr)->SetRespawnTime(0);
                (*itr)->Delete();
                m_gameObj.erase(itr++);
            }
            else
                ++itr;
        }
    }
}

void Unit::_UpdateAutoRepeatSpell()
{
    //check "realtime" interrupts
    if ((GetTypeId() == TYPEID_PLAYER && isMoving()) || IsNonMeleeSpellCast(false, false, true))
    {
        // cancel wand shoot
        if (m_currentSpells[CURRENT_AUTOREPEAT_SPELL]->m_spellInfo->Category == 351)
            InterruptSpell(CURRENT_AUTOREPEAT_SPELL);
        m_AutoRepeatFirstCast = true;
        return;
    }

    // apply delay (Auto Shot (spellID 75) not affected)
    if (m_AutoRepeatFirstCast && getAttackTimer(RANGED_ATTACK) < 500 && m_currentSpells[CURRENT_AUTOREPEAT_SPELL]->m_spellInfo->Id != 75)
        setAttackTimer(RANGED_ATTACK, 500);
    m_AutoRepeatFirstCast = false;

    //castroutine
    if (isAttackReady(RANGED_ATTACK))
    {
        // Check if able to cast
        if (m_currentSpells[CURRENT_AUTOREPEAT_SPELL]->CheckCast(true) != SPELL_CAST_OK)
        {
            InterruptSpell(CURRENT_AUTOREPEAT_SPELL);
            return;
        }

        // we want to shoot
        Spell* spell = new Spell(this, m_currentSpells[CURRENT_AUTOREPEAT_SPELL]->m_spellInfo, true, 0);
        spell->prepare(&(m_currentSpells[CURRENT_AUTOREPEAT_SPELL]->m_targets));

        // all went good, reset attack
        resetAttackTimer(RANGED_ATTACK);
    }
}

void Unit::SetCurrentCastedSpell(Spell* pSpell)
{
    ASSERT(pSpell);                                         // NULL may be never passed here, use InterruptSpell or InterruptNonMeleeSpells

    CurrentSpellTypes CSpellType = pSpell->GetCurrentContainer();

    if (pSpell == m_currentSpells[CSpellType])      // avoid breaking self
        return;

    // break same type spell if it is not delayed
    InterruptSpell(CSpellType, false);

    // special breakage effects:
    switch (CSpellType)
    {
        case CURRENT_GENERIC_SPELL:
        {
            // generic spells always break channeled not delayed spells
            InterruptSpell(CURRENT_CHANNELED_SPELL, false);

            // autorepeat breaking
            if (m_currentSpells[CURRENT_AUTOREPEAT_SPELL])
            {
                // break autorepeat if not Auto Shot
                if (m_currentSpells[CURRENT_AUTOREPEAT_SPELL]->m_spellInfo->Category == 351)
                    InterruptSpell(CURRENT_AUTOREPEAT_SPELL);
                m_AutoRepeatFirstCast = true;
            }
            if (pSpell->GetCastTime() > 0)
                AddUnitState(UNIT_STATE_CASTING);

            break;
        }
        case CURRENT_CHANNELED_SPELL:
        {
            // channel spells always break generic non-delayed and any channeled spells
            InterruptSpell(CURRENT_GENERIC_SPELL, false);
            InterruptSpell(CURRENT_CHANNELED_SPELL);

            // it also does break autorepeat if not Auto Shot
            if (m_currentSpells[CURRENT_AUTOREPEAT_SPELL] &&
                m_currentSpells[CURRENT_AUTOREPEAT_SPELL]->m_spellInfo->Category == 351)
                InterruptSpell(CURRENT_AUTOREPEAT_SPELL);
            AddUnitState(UNIT_STATE_CASTING);

            break;
        }
        case CURRENT_AUTOREPEAT_SPELL:
        {
            // only Auto Shoot does not break anything
            if (pSpell->m_spellInfo->Category == 351)
            {
                // generic autorepeats break generic non-delayed and channeled non-delayed spells
                InterruptSpell(CURRENT_GENERIC_SPELL, false);
                InterruptSpell(CURRENT_CHANNELED_SPELL, false);
            }
            // special action: set first cast flag
            m_AutoRepeatFirstCast = true;

            break;
        }
    default:
        break;  // other spell types don't break anything now
    }

    // current spell (if it is still here) may be safely deleted now
    if (m_currentSpells[CSpellType])
        m_currentSpells[CSpellType]->SetReferencedFromCurrent(false);

    // set new current spell
    m_currentSpells[CSpellType] = pSpell;
    pSpell->SetReferencedFromCurrent(true);

    pSpell->m_selfContainer = &(m_currentSpells[pSpell->GetCurrentContainer()]);
}

void Unit::InterruptSpell(CurrentSpellTypes spellType, bool withDelayed, bool withInstant)
{
    Spell* spell = m_currentSpells[spellType];
    if (spell
        && (withDelayed || spell->getState() != SPELL_STATE_DELAYED)
        && (withInstant || spell->GetCastTime() > 0 || spell->getState() == SPELL_STATE_CASTING))
    {
        // for example, do not let self-stun aura interrupt itself
        if (!spell->IsInterruptable())
            return;

        m_currentSpells[spellType] = NULL;

        // send autorepeat cancel message for autorepeat spells
        if (spellType == CURRENT_AUTOREPEAT_SPELL)
            if (GetTypeId() == TYPEID_PLAYER)
                ToPlayer()->SendAutoRepeatCancel();

        if (spell->getState() != SPELL_STATE_FINISHED)
            spell->cancel(true);
        else
        {
            m_currentSpells[spellType] = nullptr;
            spell->SetReferencedFromCurrent(false);
        }
    }
}

void Unit::FinishSpell(CurrentSpellTypes spellType, bool ok /*= true*/)
{
    Spell* spell = m_currentSpells[spellType];
    if (!spell)
        return;

    if (spellType == CURRENT_CHANNELED_SPELL)
        spell->SendChannelUpdate(0);

    spell->finish(ok);
}

bool Unit::IsNonMeleeSpellCast(bool withDelayed, bool skipChanneled, bool skipAutorepeat) const
{
    // We don't do loop here to explicitly show that melee spell is excluded.
    // Maybe later some special spells will be excluded too.

    // generic spells are casted when they are not finished and not delayed
    if (m_currentSpells[CURRENT_GENERIC_SPELL] &&
        (m_currentSpells[CURRENT_GENERIC_SPELL]->getState() != SPELL_STATE_FINISHED) &&
        (withDelayed || m_currentSpells[CURRENT_GENERIC_SPELL]->getState() != SPELL_STATE_DELAYED))
    {
        if (!(m_currentSpells[CURRENT_GENERIC_SPELL]->m_spellInfo->AttributesEx2 & SPELL_ATTR2_NOT_RESET_AUTO_ACTIONS))
            return (true);
    }

    // channeled spells may be delayed, but they are still considered casted
    else if (!skipChanneled && m_currentSpells[CURRENT_CHANNELED_SPELL] &&
             (m_currentSpells[CURRENT_CHANNELED_SPELL]->getState() != SPELL_STATE_FINISHED))
    {
        if (!(m_currentSpells[CURRENT_CHANNELED_SPELL]->m_spellInfo->AttributesEx2 & SPELL_ATTR2_NOT_RESET_AUTO_ACTIONS))
            return (true);
    }

    // autorepeat spells may be finished or delayed, but they are still considered casted
    else if (!skipAutorepeat && m_currentSpells[CURRENT_AUTOREPEAT_SPELL])
        return (true);

    return (false);
}

void Unit::InterruptNonMeleeSpells(bool withDelayed, uint32 spell_id, bool withInstant)
{
    // generic spells are interrupted if they are not finished or delayed
    if (m_currentSpells[CURRENT_GENERIC_SPELL] && (!spell_id || m_currentSpells[CURRENT_GENERIC_SPELL]->m_spellInfo->Id == spell_id))
        InterruptSpell(CURRENT_GENERIC_SPELL, withDelayed, withInstant);

    // autorepeat spells are interrupted if they are not finished or delayed
    if (m_currentSpells[CURRENT_AUTOREPEAT_SPELL] && (!spell_id || m_currentSpells[CURRENT_AUTOREPEAT_SPELL]->m_spellInfo->Id == spell_id))
        InterruptSpell(CURRENT_AUTOREPEAT_SPELL, withDelayed, withInstant);

    // channeled spells are interrupted if they are not finished, even if they are delayed
    if (m_currentSpells[CURRENT_CHANNELED_SPELL] && (!spell_id || m_currentSpells[CURRENT_CHANNELED_SPELL]->m_spellInfo->Id == spell_id))
        InterruptSpell(CURRENT_CHANNELED_SPELL, true, true);
}

Spell* Unit::FindCurrentSpellBySpellId(uint32 spell_id) const
{
    for (uint32 i = 0; i < CURRENT_MAX_SPELL; i++)
        if (m_currentSpells[i] && m_currentSpells[i]->m_spellInfo->Id == spell_id)
            return m_currentSpells[i];
    return NULL;
}

int32 Unit::GetCurrentSpellCastTime(uint32 spell_id) const
{
    if (Spell const* spell = FindCurrentSpellBySpellId(spell_id))
        return spell->GetCastTime();
    return 0;
}

bool Unit::CanMoveDuringChannel() const
{
    if (Spell* spell = m_currentSpells[CURRENT_CHANNELED_SPELL])
        if (spell->getState() != SPELL_STATE_FINISHED)
            return spell->m_spellInfo->AttributesEx5 & SPELL_ATTR5_CAN_CHANNEL_WHEN_MOVING && spell->IsChannelActive();

    return false;
}

bool Unit::isInFrontInMap(Unit const* target, float distance,  float arc) const
{
    return IsWithinDistInMap(target, distance) && HasInArc(arc, target);
}

bool Unit::isInBackInMap(Unit const* target, float distance, float arc) const
{
    return IsWithinDistInMap(target, distance) && !HasInArc(2 * M_PI - arc, target);
}

void Unit::SetFacingTo(float ori)
{
    Movement::MoveSplineInit init(*this);
    init.MoveTo(GetPositionX(), GetPositionY(), GetPositionZ(), false);
    init.SetFacing(ori);
    init.Launch();
}

void Unit::SetFacingToObject(WorldObject* pObject)
{
    // never face when already moving
    if (!IsStopped())
        return;

    /// @todo figure out under what conditions creature will move towards object instead of facing it where it currently is.
    Movement::MoveSplineInit init(*this);
    init.MoveTo(GetPositionX(), GetPositionY(), GetPositionZ());
    init.SetFacing(GetAngle(pObject));   // when on transport, GetAngle will still return global coordinates (and angle) that needs transforming
    init.Launch();
}

bool Unit::isInAccessiblePlaceFor(Creature const* c) const
{
    if (IsInWater())
        return c->CanSwim();
    else
        return c->CanWalk() || c->CanFly();
}

bool Unit::IsInWater() const
{
    return GetBaseMap()->IsInWater(GetPositionX(), GetPositionY(), GetPositionZ());
}

bool Unit::IsUnderWater() const
{
    return GetBaseMap()->IsUnderWater(GetPositionX(), GetPositionY(), GetPositionZ());
}

void Unit::DeMorph()
{
    SetDisplayId(GetNativeDisplayId());
}

int32 Unit::GetTotalAuraModifier(AuraType auratype) const
{
    int32 modifier = 0;

    AuraList const& mTotalAuraList = GetAurasByType(auratype);
    for (AuraList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
        modifier += (*i)->GetModifierValue();

    return modifier;
}

float Unit::GetTotalAuraMultiplier(AuraType auratype) const
{
    float multiplier = 1.0f;

    AuraList const& mTotalAuraList = GetAurasByType(auratype);
    for (AuraList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
        multiplier *= (100.0f + (*i)->GetModifierValue()) / 100.0f;

    return multiplier;
}

int32 Unit::GetMaxPositiveAuraModifier(AuraType auratype) const
{
    int32 modifier = 0;

    AuraList const& mTotalAuraList = GetAurasByType(auratype);
    for (AuraList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
    {
        int32 amount = (*i)->GetModifierValue();
        if (amount > modifier)
            modifier = amount;
    }

    return modifier;
}

int32 Unit::GetMaxNegativeAuraModifier(AuraType auratype) const
{
    int32 modifier = 0;

    AuraList const& mTotalAuraList = GetAurasByType(auratype);
    for (AuraList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
    {
        int32 amount = (*i)->GetModifierValue();
        if (amount < modifier)
            modifier = amount;
    }

    return modifier;
}

int32 Unit::GetTotalAuraModifierByMiscMask(AuraType auratype, uint32 misc_mask) const
{
    int32 modifier = 0;

    AuraList const& mTotalAuraList = GetAurasByType(auratype);
    for (AuraList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
    {
        Modifier* mod = (*i)->GetModifier();
        if ((mod->m_miscvalue & misc_mask) != 0)
            modifier += (*i)->GetModifierValue();
    }
    return modifier;
}

float Unit::GetTotalAuraMultiplierByMiscMask(AuraType auratype, uint32 misc_mask) const
{
    float multiplier = 1.0f;

    AuraList const& mTotalAuraList = GetAurasByType(auratype);
    for (AuraList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
    {
        Modifier* mod = (*i)->GetModifier();
        if (mod->m_miscvalue & misc_mask)
            multiplier *= (100.0f + (*i)->GetModifierValue()) / 100.0f;
    }
    return multiplier;
}

int32 Unit::GetMaxPositiveAuraModifierByMiscMask(AuraType auratype, uint32 misc_mask) const
{
    int32 modifier = 0;

    AuraList const& mTotalAuraList = GetAurasByType(auratype);
    for (AuraList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
    {
        Modifier* mod = (*i)->GetModifier();
        int32 amount = (*i)->GetModifierValue();
        if (mod->m_miscvalue & misc_mask && amount > modifier)
            modifier = amount;
    }

    return modifier;
}

int32 Unit::GetMaxNegativeAuraModifierByMiscMask(AuraType auratype, uint32 misc_mask) const
{
    int32 modifier = 0;

    AuraList const& mTotalAuraList = GetAurasByType(auratype);
    for (AuraList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
    {
        Modifier* mod = (*i)->GetModifier();
        int32 amount = (*i)->GetModifierValue();
        if (mod->m_miscvalue & misc_mask && amount < modifier)
            modifier = amount;
    }

    return modifier;
}

int32 Unit::GetTotalAuraModifierByMiscValue(AuraType auratype, int32 misc_value) const
{
    int32 modifier = 0;

    AuraList const& mTotalAuraList = GetAurasByType(auratype);
    for (AuraList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
    {
        Modifier* mod = (*i)->GetModifier();
        if (mod->m_miscvalue == misc_value)
            modifier += (*i)->GetModifierValue();
    }
    return modifier;
}

float Unit::GetTotalAuraMultiplierByMiscValue(AuraType auratype, int32 misc_value) const
{
    float multiplier = 1.0f;

    AuraList const& mTotalAuraList = GetAurasByType(auratype);
    for (AuraList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
    {
        Modifier* mod = (*i)->GetModifier();
        if (mod->m_miscvalue == misc_value)
            multiplier *= (100.0f + (*i)->GetModifierValue()) / 100.0f;
    }
    return multiplier;
}

int32 Unit::GetMaxPositiveAuraModifierByMiscValue(AuraType auratype, int32 misc_value) const
{
    int32 modifier = 0;

    AuraList const& mTotalAuraList = GetAurasByType(auratype);
    for (AuraList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
    {
        Modifier* mod = (*i)->GetModifier();
        int32 amount = (*i)->GetModifierValue();
        if (mod->m_miscvalue == misc_value && amount > modifier)
            modifier = amount;
    }

    return modifier;
}

int32 Unit::GetMaxNegativeAuraModifierByMiscValue(AuraType auratype, int32 misc_value) const
{
    int32 modifier = 0;

    AuraList const& mTotalAuraList = GetAurasByType(auratype);
    for (AuraList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
    {
        Modifier* mod = (*i)->GetModifier();
        int32 amount = (*i)->GetModifierValue();
        if (mod->m_miscvalue == misc_value && amount < modifier)
            modifier = amount;
    }

    return modifier;
}

bool Unit::AddAura(Aura* Aur)
{
    // ghost spell check, allow apply any auras at player loading in ghost mode (will be cleanup after load)
    if (!IsAlive() && Aur->GetId() != 20584 && Aur->GetId() != 8326 && Aur->GetId() != 2584 &&
        (GetTypeId() != TYPEID_PLAYER || !ToPlayer()->GetSession()->PlayerLoading()))
    {
        delete Aur;
        return false;
    }

    if (Aur->GetTarget() != this)
    {
        sLog.outError("Aura (spell %u eff %u) add to aura list of %s (lowguid: %u) but Aura target is %s (lowguid: %u)",
                      Aur->GetId(), Aur->GetEffIndex(), (GetTypeId() == TYPEID_PLAYER ? "player" : "creature"), GetGUIDLow(),
                      (Aur->GetTarget()->GetTypeId() == TYPEID_PLAYER ? "player" : "creature"), Aur->GetTarget()->GetGUIDLow());
        delete Aur;
        return false;
    }

    SpellEntry const* aurSpellInfo = Aur->GetSpellProto();

    spellEffectPair spair = spellEffectPair(Aur->GetId(), Aur->GetEffIndex());
    bool stackModified = false;

    // Handle aura stacking down below...
    if (!Aur->IsPassive() && !Aur->IsPersistent())
    {
        for (AuraMap::iterator i2 = m_Auras.lower_bound(spair); i2 != m_Auras.upper_bound(spair);)
        {
            Aura* aur2 = i2->second;

            if (aur2 && !stackModified)
            {
                ASSERT(aur2->GetId() == Aur->GetId());

                // @todo: fix this hack
                // Allow mongoose proc from different weapons... this should be corrected to allow multiple
                // auras triggered by different enchanted items this is not possible currently since we only have
                // GUID and not even information if the Aura was "triggered" see m_IsTriggeredSpell in Spell.h
                if (Aur->GetCastItemGUID() && Aur->GetId() == 28093)
                {
                    bool allow = true;

                    for (AuraMap::const_iterator i3 = m_Auras.lower_bound(spair); i3 != m_Auras.upper_bound(spair); i3++)
                        if (i3->second->GetCastItemGUID() == Aur->GetCastItemGUID())
                            allow = false;

                    if (allow)
                        break;
                }

                if (sSpellMgr.IsNoStackSpellDueToSpell(Aur->GetId(), aur2->GetId(), Aur->GetCasterGUID() == aur2->GetCasterGUID()))
                {
                    // Non stackable and capped auras do not allow stacking
                    if (!(aurSpellInfo->StackAmount && uint32(aur2->GetStackAmount()) < aurSpellInfo->StackAmount))
                    {
                        // Do not let the stack size exceed the maximum stack limit
                        // Instead of adding a new stack, just set the duration time
                        // we need to use time from Aur because of diminishing effects
                        aur2->SetAuraDuration(Aur->GetAuraMaxDuration());
                        aur2->SetAuraProcCharges(Aur->m_procCharges);
                        aur2->UpdateAuraDuration();
                        aur2->UpdateAuraCharges();
                        *aur2->GetModifier() = *Aur->GetModifier();
                        delete Aur;
                        return true;
                    }

                    // Increment aura's stack, one stack per one call
                    Aur->SetStackAmount(aur2->GetStackAmount() + 1);
                    stackModified = true;

                    // Existing aura will be replaced with the newly created one
                    RemoveAura(i2, AURA_REMOVE_BY_STACK);
                    i2 = m_Auras.lower_bound(spair);
                    continue;
                }
            }

            ++i2;
        }
    }

    // passive auras stack with all (except passive spell proc auras)
    if ((!Aur->IsPassive() || !IsPassiveStackableSpell(Aur->GetId())) &&
        !(Aur->GetId() == 20584 || Aur->GetId() == 8326))
    {
        if (!RemoveNoStackAurasDueToAura(Aur))
        {
            delete Aur;
            return false;                                   // couldn't remove conflicting aura with higher rank
        }
    }

    // update single target auras list (before aura add to aura list, to prevent unexpected remove recently added aura)
    if (Aur->IsSingleTarget() && Aur->GetTarget())
    {
        // caster pointer can be deleted in time aura remove, find it by guid at each iteration
        for (;;)
        {
            Unit* caster = Aur->GetCaster();
            if (!caster)                                     // caster deleted and not required adding scAura
                break;

            bool restart = false;
            AuraList& scAuras = caster->GetSingleCastAuras();
            for (AuraList::iterator itr = scAuras.begin(); itr != scAuras.end(); ++itr)
            {
                if ((*itr)->GetTarget() != Aur->GetTarget() &&
                    IsSingleTargetSpells((*itr)->GetSpellProto(), aurSpellInfo))
                {
                    if ((*itr)->IsInUse())
                    {
                        sLog.outError("Aura (Spell %u Effect %u) is in process but attempt removed at aura (Spell %u Effect %u) adding, need add stack rule for IsSingleTargetSpell", (*itr)->GetId(), (*itr)->GetEffIndex(), Aur->GetId(), Aur->GetEffIndex());
                        continue;
                    }
                    (*itr)->GetTarget()->RemoveAura((*itr)->GetId(), (*itr)->GetEffIndex());
                    restart = true;
                    break;
                }
            }

            if (!restart)
            {
                // done
                scAuras.push_back(Aur);
                break;
            }
        }
    }

    // add aura, register in lists and arrays
    Aur->_AddAura();
    m_Auras.insert(AuraMap::value_type(spellEffectPair(Aur->GetId(), Aur->GetEffIndex()), Aur));
    if (Aur->GetModifier()->m_auraname < TOTAL_AURAS)
    {
        m_modAuras[Aur->GetModifier()->m_auraname].push_back(Aur);
        if (Aur->GetSpellProto()->AuraInterruptFlags)
        {
            m_interruptableAuras.push_back(Aur);
            AddInterruptMask(Aur->GetSpellProto()->AuraInterruptFlags);
        }
        if ((Aur->GetSpellProto()->Attributes & SPELL_ATTR0_HEARTBEAT_RESIST_CHECK)
            && (Aur->GetModifier()->m_auraname != SPELL_AURA_MOD_POSSESS)) //only dummy aura is breakable
            m_ccAuras.push_back(Aur);
    }

    Aur->ApplyModifier(true, true);

    uint32 id = Aur->GetId();
    if (sSpellMgr.GetSpellCustomAttr(id) & SPELL_ATTR_CU_LINK_AURA)
    {
        if (const std::vector<int32>* spell_triggered = sSpellMgr.GetSpellLinked(id + SPELL_LINK_AURA))
        {
            for (std::vector<int32>::const_iterator itr = spell_triggered->begin(); itr != spell_triggered->end(); ++itr)
                if (*itr < 0)
                    ApplySpellImmune(id, IMMUNITY_ID, -(*itr), true);
                else if (Unit* caster = Aur->GetCaster())
                    caster->AddAura(*itr, this);
        }
    }

    DEBUG_LOG("Aura %u now is in use", Aur->GetModifier()->m_auraname);
    return true;
}

void Unit::RemoveRankAurasDueToSpell(uint32 spellId)
{
    SpellEntry const* spellInfo = sSpellStore.LookupEntry(spellId);
    if (!spellInfo)
        return;
    AuraMap::iterator i, next;
    for (i = m_Auras.begin(); i != m_Auras.end(); i = next)
    {
        next = i;
        ++next;
        uint32 i_spellId = (*i).second->GetId();
        if ((*i).second && i_spellId && i_spellId != spellId)
        {
            if (sSpellMgr.IsRankSpellDueToSpell(spellInfo, i_spellId))
            {
                RemoveAurasDueToSpell(i_spellId);

                if (m_Auras.empty())
                    break;
                else
                    next =  m_Auras.begin();
            }
        }
    }
}

bool Unit::RemoveNoStackAurasDueToAura(Aura* Aur)
{
    if (!Aur) return false;

    SpellEntry const* spellProto = Aur->GetSpellProto();
    if (!spellProto) return false;

    uint32 spellId = Aur->GetId();
    uint32 effIndex = Aur->GetEffIndex();

    AuraMap::iterator i, next;
    for (i = m_Auras.begin(); i != m_Auras.end(); i = next)
    {
        next = i;
        ++next;
        if (!(*i).second) continue;

        SpellEntry const* i_spellProto = (*i).second->GetSpellProto();
        if (!i_spellProto) continue;

        uint32 i_spellId = i_spellProto->Id;

        if (spellId == i_spellId)
            continue;

        if (IsPassiveSpell(i_spellId))
        {
            // passive non-stackable spells not stackable only with another rank of same spell
            if (IsPassiveStackableSpell(i_spellId) || !sSpellMgr.IsRankSpellDueToSpell(spellProto, i_spellId))
                continue;
        }
        uint32 i_effIndex = (*i).second->GetEffIndex();

        bool is_triggered_by_spell = false;
        // prevent triggered aura of removing aura that triggered it
        for (int j = 0; j < MAX_SPELL_EFFECTS; ++j)
        {
            if (i_spellProto->EffectTriggerSpell[j] == spellProto->Id)
            {
                is_triggered_by_spell = true;
                break;
            }
        }
        if (is_triggered_by_spell) continue;

        for (int j = 0; j < MAX_SPELL_EFFECTS; ++j)
        {
            // prevent remove dummy triggered spells at next effect aura add
            if (spellProto->Effect[j] == SPELL_EFFECT_DUMMY && spellId == 5420 && i_spellId == 34123)
                is_triggered_by_spell = true;

            // main aura added before triggered spell
            // prevent remove form main spell by triggered passive spells
            else if (i_spellProto->EffectApplyAuraName[j] == SPELL_AURA_MOD_SHAPESHIFT)
            {
                if ((i_spellId == 24858 && spellId == 24905) || (i_spellId == 34551 && spellId == 22688))
                    is_triggered_by_spell = true;

                else if (i_spellId == 33891 && (spellId == 5420 || spellId == 34123))
                    is_triggered_by_spell = true;
            }
            else continue;
            break;
        }

        if (!is_triggered_by_spell)
        {
            bool sameCaster = Aur->GetCasterGUID() == (*i).second->GetCasterGUID();

            if (!sSpellMgr.IsNoStackSpellDueToSpell(spellId, i_spellId, sameCaster))
                continue;

            int32 aur1Rank = Aur->GetModifierValue();
            int32 aur2Rank = (*i).second->GetModifierValue();
            // Check if effect is "better"
            if (!sameCaster && (abs(aur1Rank)-abs(aur2Rank)) < 0)
                return false;
            // Some spells should be not removed by lower rank of them (totem, paladin aura)
            if (!sameCaster && spellProto->DurationIndex == 21 && sSpellMgr.IsRankSpellDueToSpell(spellProto, i_spellId))
                if (spellProto->Effect[effIndex] == SPELL_EFFECT_APPLY_AREA_AURA_PARTY)
                    if (CompareAuraRanks(spellId, effIndex, i_spellId, i_effIndex) < 0)
                        return false;

            // Its a parent aura (create this aura in ApplyModifier)
            if ((*i).second->IsInUse())
            {
                sLog.outError("Aura (Spell %u Effect %u) is in process but attempt removed at aura (Spell %u Effect %u) adding, need add stack rule for Unit::RemoveNoStackAurasDueToAura", i->second->GetId(), i->second->GetEffIndex(), Aur->GetId(), Aur->GetEffIndex());
                continue;
            }
            uint64 caster = (*i).second->GetCasterGUID();
            // Remove all auras by aura caster
            for (uint8 a = 0; a < MAX_SPELL_EFFECTS; ++a)
            {
                spellEffectPair spair = spellEffectPair(i_spellId, a);
                for (AuraMap::iterator iter = m_Auras.lower_bound(spair); iter != m_Auras.upper_bound(spair);)
                {
                    if (iter->second->GetCasterGUID() == caster)
                    {
                        RemoveAura(iter, AURA_REMOVE_BY_STACK);
                        iter = m_Auras.lower_bound(spair);
                    }
                    else ++iter;
                }
            }
            if (m_Auras.empty()) break;
            else next = m_Auras.begin();
        }
    }
    return true;
}

void Unit::RemoveAura(uint32 spellId, uint32 effindex, Aura* except)
{
    spellEffectPair spair = spellEffectPair(spellId, effindex);
    for (AuraMap::iterator iter = m_Auras.lower_bound(spair); iter != m_Auras.upper_bound(spair);)
    {
        if (iter->second != except)
        {
            RemoveAura(iter);
            iter = m_Auras.lower_bound(spair);
        }
        else
            ++iter;
    }
}

void Unit::RemoveAurasByCasterSpell(uint32 spellId, uint64 casterGUID)
{
    for (int k = 0; k < MAX_SPELL_EFFECTS; ++k)
    {
        spellEffectPair spair = spellEffectPair(spellId, k);
        for (AuraMap::iterator iter = m_Auras.lower_bound(spair); iter != m_Auras.upper_bound(spair);)
        {
            if (iter->second->GetCasterGUID() == casterGUID)
            {
                RemoveAura(iter);
                iter = m_Auras.upper_bound(spair);          // overwrite by more appropriate
            }
            else
                ++iter;
        }
    }
}

void Unit::SetAurasDurationByCasterSpell(uint32 spellId, uint64 casterGUID, int32 duration)
{
    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        spellEffectPair spair = spellEffectPair(spellId, i);
        for (AuraMap::const_iterator itr = GetAuras().lower_bound(spair); itr != GetAuras().upper_bound(spair); ++itr)
        {
            if (itr->second->GetCasterGUID() == casterGUID)
            {
                itr->second->SetAuraDuration(duration);
                break;
            }
        }
    }
}

Aura* Unit::GetAuraByCasterSpell(uint32 spellId, uint64 casterGUID)
{
    // Returns first found aura from spell-use only in cases where effindex of spell doesn't matter!
    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        spellEffectPair spair = spellEffectPair(spellId, i);
        for (AuraMap::const_iterator itr = GetAuras().lower_bound(spair); itr != GetAuras().upper_bound(spair); ++itr)
        {
            if (itr->second->GetCasterGUID() == casterGUID)
                return itr->second;
        }
    }
    return NULL;
}

void Unit::RemoveAurasDueToSpellByDispel(uint32 spellId, uint64 casterGUID, Unit* dispeler)
{
    for (AuraMap::iterator iter = m_Auras.begin(); iter != m_Auras.end();)
    {
        Aura* aur = iter->second;
        if (aur->GetId() == spellId && aur->GetCasterGUID() == casterGUID)
        {
            // Custom dispel case
            // Unstable Affliction
            if (aur->GetSpellProto()->SpellFamilyName == SPELLFAMILY_WARLOCK && (aur->GetSpellProto()->SpellFamilyFlags & 0x010000000000LL))
            {
                int32 damage = aur->GetModifier()->m_amount * 9;
                uint64 caster_guid = aur->GetCasterGUID();

                // Remove aura
                RemoveAura(iter, AURA_REMOVE_BY_DISPEL);

                // backfire damage and silence
                dispeler->CastCustomSpell(dispeler, 31117, &damage, NULL, NULL, true, NULL, NULL, caster_guid);

                iter = m_Auras.begin();                     // iterator can be invalidate at cast if self-dispel
            }
            else
                RemoveAura(iter, AURA_REMOVE_BY_DISPEL);
        }
        else
            ++iter;
    }
}

void Unit::RemoveAurasDueToSpellBySteal(uint32 spellId, uint64 casterGUID, Unit* stealer)
{
    for (AuraMap::iterator iter = m_Auras.begin(); iter != m_Auras.end();)
    {
        Aura* aur = iter->second;
        if (aur->GetId() == spellId && aur->GetCasterGUID() == casterGUID)
        {
            //int32 basePoints = aur->GetBasePoints();
            // construct the new aura for the attacker
            Aura* new_aur = CreateAura(aur->GetSpellProto(), aur->GetEffIndex(), NULL/*&basePoints*/, stealer);
            if (!new_aur)
                continue;

            // set its duration and maximum duration
            // max duration 2 minutes (in msecs)
            int32 dur = aur->GetAuraDuration();
            const int32 max_dur = 2 * MINUTE * IN_MILLISECONDS;
            new_aur->SetAuraMaxDuration(max_dur > dur ? dur : max_dur);
            new_aur->SetAuraDuration(max_dur > dur ? dur : max_dur);
            new_aur->SetAuraProcCharges(aur->m_procCharges);

            // Unregister _before_ adding to stealer
            aur->UnregisterSingleCastAura();
            // strange but intended behaviour: Stolen single target auras won't be treated as single targeted
            new_aur->SetIsSingleTarget(false);
            // add the new aura to stealer
            stealer->AddAura(new_aur);
            // Remove aura as dispel
            if (aur->GetStackAmount() > 1)
            {
                // reapply modifier with reduced stack amount
                aur->ApplyModifier(false, true);
                aur->SetStackAmount(aur->GetStackAmount() - 1);
                aur->ApplyModifier(true, true);

                aur->UpdateSlotCounterAndDuration();
                ++iter;
            }
            else
                RemoveAura(iter, AURA_REMOVE_BY_DISPEL);
        }
        else
            ++iter;
    }
}

void Unit::RemoveAurasDueToSpellByCancel(uint32 spellId)
{
    for (AuraMap::iterator iter = m_Auras.begin(); iter != m_Auras.end();)
    {
        if (iter->second->GetId() == spellId)
            RemoveAura(iter, AURA_REMOVE_BY_CANCEL);
        else
            ++iter;
    }
}

void Unit::RemoveAurasWithAttribute(uint32 flags)
{
    for (AuraMap::iterator iter = m_Auras.begin(); iter != m_Auras.end();)
    {
        SpellEntry const* spell = iter->second->GetSpellProto();
        if (spell->Attributes & flags)
            RemoveAura(iter);
        else
            ++iter;
    }
}

void Unit::RemoveAurasWithDispelType(DispelType type)
{
    // Create dispel mask by dispel type
    uint32 dispelMask = GetDispelMask(type);
    // Dispel all existing auras vs current dispel type
    AuraMap& auras = GetAuras();
    for (AuraMap::iterator itr = auras.begin(); itr != auras.end();)
    {
        SpellEntry const* spell = itr->second->GetSpellProto();
        if ((1 << spell->Dispel) & dispelMask && !itr->second->IsInUse())
        {
            // Dispel aura
            RemoveAurasDueToSpell(spell->Id);
            itr = auras.begin();
        }
        else
            ++itr;
    }
}

void Unit::RemoveSingleAuraFromStackByDispel(uint32 spellId)
{
    for (AuraMap::iterator iter = m_Auras.begin(); iter != m_Auras.end();)
    {
        Aura* aur = iter->second;
        if (aur->GetId() == spellId)
        {
            if (iter->second->GetStackAmount() > 1)
            {
                // reapply modifier with reduced stack amount
                iter->second->ApplyModifier(false, true);
                iter->second->SetStackAmount(iter->second->GetStackAmount() - 1);
                iter->second->ApplyModifier(true, true);

                iter->second->UpdateSlotCounterAndDuration();
                return; // not remove aura if stack amount > 1
            }
            else
                RemoveAura(iter, AURA_REMOVE_BY_DISPEL);
        }
        else
            ++iter;
    }
}

void Unit::RemoveSingleAuraFromStack(uint32 spellId, uint32 effindex)
{
    AuraMap::iterator iter = m_Auras.find(spellEffectPair(spellId, effindex));
    if (iter != m_Auras.end())
    {
        if (iter->second->GetStackAmount() > 1)
        {
            // reapply modifier with reduced stack amount
            iter->second->ApplyModifier(false, true);
            iter->second->SetStackAmount(iter->second->GetStackAmount() - 1);
            iter->second->ApplyModifier(true, true);

            iter->second->UpdateSlotCounterAndDuration();
            return; // not remove aura if stack amount > 1
        }
        RemoveAura(iter);
    }
}

void Unit::RemoveAurasDueToSpell(uint32 spellId, Aura* except)
{
    for (int i = 0; i < MAX_SPELL_EFFECTS; ++i)
        RemoveAura(spellId, i, except);
}

void Unit::RemoveAurasDueToItemSpell(Item* castItem, uint32 spellId)
{
    for (int k = 0; k < MAX_SPELL_EFFECTS; ++k)
    {
        spellEffectPair spair = spellEffectPair(spellId, k);
        for (AuraMap::iterator iter = m_Auras.lower_bound(spair); iter != m_Auras.upper_bound(spair);)
        {
            if (iter->second->GetCastItemGUID() == castItem->GetGUID())
            {
                RemoveAura(iter);
                iter = m_Auras.upper_bound(spair);          // overwrite by more appropriate
            }
            else
                ++iter;
        }
    }
}

void Unit::RemoveAurasByType(AuraType auraType, uint64 casterGUID, Aura* except, bool negative, bool positive)
{
    if (auraType >= TOTAL_AURAS)
        return;

    for (AuraList::iterator iter = m_modAuras[auraType].begin(); iter != m_modAuras[auraType].end();)
    {
        Aura* aura = *iter;

        ++iter;
        if (aura != except && (!casterGUID || aura->GetCasterGUID() == casterGUID)
            && ((negative && !aura->IsPositive()) || (positive && aura->IsPositive())))
        {
            uint32 removedAuras = m_removedAurasCount;
            RemoveAurasDueToSpell(aura->GetId());
            if (m_removedAurasCount > removedAuras + 1)
                iter = m_modAuras[auraType].begin();
        }
    }
}

void Unit::RemoveNotOwnSingleTargetAuras()
{
    // single target auras from other casters
    for (AuraMap::iterator iter = m_Auras.begin(); iter != m_Auras.end();)
    {
        Aura const* aura = iter->second;

        if (aura->GetCasterGUID() != GetGUID() && aura->IsSingleTarget()  || IsSingleTargetSpell(iter->second->GetSpellProto()))
            RemoveAura(iter);
        else
            ++iter;
    }

    // single target auras at other targets
    AuraList& scAuras = GetSingleCastAuras();
    for (AuraList::iterator iter = scAuras.begin(); iter != scAuras.end();)
    {
        Aura* aur = *iter;
        ++iter;
        if (aur->GetTarget() != this)
        {
            uint32 removedAuras = m_removedAurasCount;
            aur->GetTarget()->RemoveAura(aur->GetId(), aur->GetEffIndex());
            if (m_removedAurasCount > removedAuras + 1)
                iter = scAuras.begin();
        }
    }

}

void Unit::RemoveAura(AuraMap::iterator& i, AuraRemoveMode mode)
{
    Aura* Aur = i->second;

    // if unit currently update aura list then make safe update iterator shift to next
    if (m_AurasUpdateIterator == i)
        ++m_AurasUpdateIterator;

    // some ShapeshiftBoosts at remove trigger removing other auras including parent Shapeshift aura
    // remove aura from list before to prevent deleting it before
    m_Auras.erase(i);
    ++m_removedAurasCount;

    SpellEntry const* AurSpellInfo = Aur->GetSpellProto();
    Unit* caster = NULL;
    Aur->UnregisterSingleCastAura();

    // remove from list before mods removing (prevent cyclic calls, mods added before including to aura list - use reverse order)
    if (Aur->GetModifier()->m_auraname < TOTAL_AURAS)
    {
        m_modAuras[Aur->GetModifier()->m_auraname].remove(Aur);

        if (Aur->GetSpellProto()->AuraInterruptFlags)
        {
            m_interruptableAuras.remove(Aur);
            UpdateInterruptMask();
        }

        if ((Aur->GetSpellProto()->Attributes & SPELL_ATTR0_HEARTBEAT_RESIST_CHECK)
            && (Aur->GetModifier()->m_auraname != SPELL_AURA_MOD_POSSESS)) //only dummy aura is breakable
            m_ccAuras.remove(Aur);
    }

    // Set remove mode
    Aur->SetRemoveMode(mode);

    // Statue unsummoned at aura remove
    //Totem* statue = NULL;
    if (Aur->GetAuraDuration() && !Aur->IsPersistent() && IsChanneledSpell(AurSpellInfo))
    {
        if (!caster)                                         // can be already located for IsSingleTargetSpell case
            caster = Aur->GetCaster();

        if (caster && caster->IsAlive())
        {
            // stop caster chanelling state
            if (caster->m_currentSpells[CURRENT_CHANNELED_SPELL]
                //prevent recurential call
                && caster->m_currentSpells[CURRENT_CHANNELED_SPELL]->getState() != SPELL_STATE_FINISHED)
            {
                if (caster == this || !IsAreaOfEffectSpell(AurSpellInfo))
                {
                    // remove auras only for non-aoe spells or when chanelled aura is removed
                    // because aoe spells don't require aura on target to continue
                    if (AurSpellInfo->EffectApplyAuraName[Aur->GetEffIndex()] != SPELL_AURA_PERIODIC_DUMMY
                        && AurSpellInfo->EffectApplyAuraName[Aur->GetEffIndex()] != SPELL_AURA_DUMMY)
                        //don't stop channeling of scripted spells (this is actually a hack)
                    {
                        caster->m_currentSpells[CURRENT_CHANNELED_SPELL]->cancel();
                        caster->m_currentSpells[CURRENT_CHANNELED_SPELL] = NULL;

                    }
                }

                //if (caster->GetTypeId() == TYPEID_UNIT && caster->IsTotem() && ((Totem*)caster)->GetTotemType() == TOTEM_STATUE)
                //    statue = ((Totem*)caster);
            }

            // Unsummon summon as possessed creatures on spell cancel
            if (caster->GetTypeId() == TYPEID_PLAYER)
                if (Unit* charm = caster->GetCharm())
                    if (charm->GetTypeId() == TYPEID_UNIT
                        && charm->ToCreature()->HasUnitTypeMask(UNIT_MASK_PUPPET)
                        && charm->GetUInt32Value(UNIT_CREATED_BY_SPELL) == AurSpellInfo->Id)
                        ((Puppet*)charm)->UnSummon();
        }
    }

    DEBUG_LOG("Aura %u (%u) now is remove mode %d", Aur->GetId(), Aur->GetModifier()->m_auraname, mode);
    ASSERT(!Aur->IsInUse());
    Aur->ApplyModifier(false, true);

    Aur->SetStackAmount(0);

    // set aura to be removed during unit::_updatespells
    m_removedAuras.push_back(Aur);

    Aur->_RemoveAura();

    bool stack = false;
    spellEffectPair spair = spellEffectPair(Aur->GetId(), Aur->GetEffIndex());
    for (AuraMap::const_iterator itr = GetAuras().lower_bound(spair); itr != GetAuras().upper_bound(spair); ++itr)
    {
        if (itr->second->GetCasterGUID() == GetGUID())
            stack = true;
    }
    if (!stack)
    {
        // Remove all triggered by aura spells vs unlimited duration
        Aur->CleanupTriggeredSpells();

        // Remove Linked Auras
        uint32 id = Aur->GetId();
        if (sSpellMgr.GetSpellCustomAttr(id) & SPELL_ATTR_CU_LINK_REMOVE)
        {
            if (const std::vector<int32>* spell_triggered = sSpellMgr.GetSpellLinked(-(int32)id))
            {
                for (std::vector<int32>::const_iterator itr = spell_triggered->begin(); itr != spell_triggered->end(); ++itr)
                    if (*itr < 0)
                        RemoveAurasDueToSpell(-(*itr));
                    else if (Unit* caster = Aur->GetCaster())
                        CastSpell(this, *itr, true, 0, 0, caster->GetGUID());
            }
        }
        if (sSpellMgr.GetSpellCustomAttr(id) & SPELL_ATTR_CU_LINK_AURA)
        {
            if (const std::vector<int32>* spell_triggered = sSpellMgr.GetSpellLinked(id + SPELL_LINK_AURA))
            {
                for (std::vector<int32>::const_iterator itr = spell_triggered->begin(); itr != spell_triggered->end(); ++itr)
                    if (*itr < 0)
                        ApplySpellImmune(id, IMMUNITY_ID, -(*itr), false);
                    else
                        RemoveAurasDueToSpell(*itr);
            }
        }
    }

    //if (statue)
    //    statue->UnSummon();

    i = m_Auras.begin();
}

void Unit::RemoveAllAuras()
{
    while (!m_Auras.empty())
    {
        AuraMap::iterator iter = m_Auras.begin();
        RemoveAura(iter);
    }
}

void Unit::RemoveArenaAuras()
{
    // in join, remove positive buffs, on end, remove negative
    // used to remove positive visible auras in arenas
    for (AuraMap::iterator iter = m_Auras.begin(); iter != m_Auras.end();)
    {
        Aura* aura = iter->second;
        if (!(aura->GetSpellProto()->AttributesEx4 & SPELL_ATTR4_STANCES)                                    // don't remove stances, shadowform, pally/hunter auras
            && !aura->IsPassive()                                                                               // don't remove passive auras
            && (aura->IsPositive() || !(aura->GetSpellProto()->AttributesEx3 & SPELL_ATTR3_DEATH_PERSISTENT))) // not negative death persistent auras
            RemoveAura(iter);
        else
            ++iter;
    }
}

void Unit::RemoveAllAurasOnDeath()
{
    // used just after dieing to remove all visible auras
    // and disable the mods for the passive ones
    for (AuraMap::iterator iter = m_Auras.begin(); iter != m_Auras.end();)
    {
        if (!iter->second->IsPassive() && !iter->second->IsDeathPersistent())
            RemoveAura(iter, AURA_REMOVE_BY_DEATH);
        else
            ++iter;
    }
}

void Unit::DelayAura(uint32 spellId, uint32 effindex, int32 delaytime)
{
    AuraMap::iterator iter = m_Auras.find(spellEffectPair(spellId, effindex));
    if (iter != m_Auras.end())
    {
        if (iter->second->GetAuraDuration() < delaytime)
            iter->second->SetAuraDuration(0);
        else
            iter->second->SetAuraDuration(iter->second->GetAuraDuration() - delaytime);
        iter->second->UpdateAuraDuration();
        DEBUG_LOG("Aura %u partially interrupted on unit %u, new duration: %u ms", iter->second->GetModifier()->m_auraname, GetGUIDLow(), iter->second->GetAuraDuration());
    }
}

void Unit::_RemoveAllAuraMods()
{
    for (AuraMap::iterator i = m_Auras.begin(); i != m_Auras.end(); ++i)
        (*i).second->ApplyModifier(false);
}

void Unit::_ApplyAllAuraMods()
{
    for (AuraMap::iterator i = m_Auras.begin(); i != m_Auras.end(); ++i)
        (*i).second->ApplyModifier(true);
}

Aura* Unit::GetAura(uint32 spellId, uint32 effindex)
{
    AuraMap::iterator iter = m_Auras.find(spellEffectPair(spellId, effindex));
    if (iter != m_Auras.end())
        return iter->second;
    return NULL;
}

void Unit::AddDynObject(DynamicObject* dynObj)
{
    m_dynObjGUIDs.push_back(dynObj->GetGUID());
}

void Unit::RemoveDynObject(uint32 spellid)
{
    if (m_dynObjGUIDs.empty())
        return;
    for (DynObjectGUIDs::iterator i = m_dynObjGUIDs.begin(); i != m_dynObjGUIDs.end();)
    {
        DynamicObject* dynObj = GetMap()->GetDynamicObject(*i);
        if (!dynObj) // may happen if a dynobj is removed when grid unload
            i = m_dynObjGUIDs.erase(i);
        else if (spellid == 0 || dynObj->GetSpellId() == spellid)
        {
            dynObj->Remove();
            i = m_dynObjGUIDs.erase(i);
        }
        else
            ++i;
    }
}

void Unit::RemoveAllDynObjects()
{
    while (!m_dynObjGUIDs.empty())
    {
        DynamicObject* dynObj = GetMap()->GetDynamicObject(*m_dynObjGUIDs.begin());
        if (dynObj)
            dynObj->Remove();
        m_dynObjGUIDs.erase(m_dynObjGUIDs.begin());
    }
}

DynamicObject* Unit::GetDynObject(uint32 spellId, uint32 effIndex)
{
    for (DynObjectGUIDs::iterator i = m_dynObjGUIDs.begin(); i != m_dynObjGUIDs.end();)
    {
        DynamicObject* dynObj = GetMap()->GetDynamicObject(*i);
        if (!dynObj)
        {
            i = m_dynObjGUIDs.erase(i);
            continue;
        }

        if (dynObj->GetSpellId() == spellId && dynObj->GetEffIndex() == effIndex)
            return dynObj;
        ++i;
    }
    return NULL;
}

DynamicObject* Unit::GetDynObject(uint32 spellId)
{
    for (DynObjectGUIDs::iterator i = m_dynObjGUIDs.begin(); i != m_dynObjGUIDs.end();)
    {
        DynamicObject* dynObj = GetMap()->GetDynamicObject(*i);
        if (!dynObj)
        {
            i = m_dynObjGUIDs.erase(i);
            continue;
        }

        if (dynObj->GetSpellId() == spellId)
            return dynObj;
        ++i;
    }
    return NULL;
}

void Unit::AddGameObject(GameObject* gameObj)
{
    if (!gameObj || gameObj->GetOwnerGUID() != 0)
        return;

    m_gameObj.push_back(gameObj);
    gameObj->SetOwnerGUID(GetGUID());
}

void Unit::RemoveGameObject(GameObject* gameObj, bool del)
{
    if (!gameObj || gameObj->GetOwnerGUID() != GetGUID())
        return;

    gameObj->SetOwnerGUID(0);

    for (uint8 i = 0; i < 3; ++i)
    {
        if (m_ObjectSlot[i] == gameObj->GetGUID())
        {
            m_ObjectSlot[i] = 0;
            break;
        }
    }

    // GO created by some spell
    if (uint32 spellid = gameObj->GetSpellId())
    {
        RemoveAurasDueToSpell(spellid);

        if (GetTypeId() == TYPEID_PLAYER)
        {
            SpellEntry const* createBySpell = sSpellStore.LookupEntry(spellid);
            // Need activate spell use for owner
            if (createBySpell && createBySpell->Attributes & SPELL_ATTR0_DISABLED_WHILE_ACTIVE)
                // note: item based cooldowns and cooldown spell mods with charges ignored (unknown existed cases)
                ToPlayer()->SendCooldownEvent(createBySpell);
        }
    }

    m_gameObj.remove(gameObj);

    if (del)
    {
        gameObj->SetRespawnTime(0);
        gameObj->Delete();
    }
}

void Unit::RemoveGameObject(uint32 spellid, bool del)
{
    if (m_gameObj.empty())
        return;
    std::list<GameObject*>::iterator i, next;
    for (i = m_gameObj.begin(); i != m_gameObj.end(); i = next)
    {
        next = i;
        if (spellid == 0 || (*i)->GetSpellId() == spellid)
        {
            (*i)->SetOwnerGUID(0);
            if (del)
            {
                (*i)->SetRespawnTime(0);
                (*i)->Delete();
            }

            next = m_gameObj.erase(i);
        }
        else
            ++next;
    }
}

void Unit::RemoveAllGameObjects()
{
    // remove references to unit
    for (std::list<GameObject*>::iterator i = m_gameObj.begin(); i != m_gameObj.end();)
    {
        (*i)->SetOwnerGUID(0);
        (*i)->SetRespawnTime(0);
        (*i)->Delete();
        i = m_gameObj.erase(i);
    }
}

void Unit::SendSpellNonMeleeDamageLog(SpellNonMeleeDamage* log)
{
    WorldPacket data(SMSG_SPELLNONMELEEDAMAGELOG, (16 + 4 + 4 + 1 + 4 + 4 + 1 + 1 + 4 + 4 + 1)); // we guess size
    data << log->target->GetPackGUID();
    data << log->attacker->GetPackGUID();
    data << uint32(log->SpellID);
    data << uint32(log->damage);                            // damage amount
    data << uint8 (log->schoolMask);                        // damage school
    data << uint32(log->absorb);                            // AbsorbedDamage
    data << uint32(log->resist);                            // resist
    data << uint8 (log->physicalLog);                       // damsge type? flag
    data << uint8 (log->unused);                            // unused
    data << uint32(log->blocked);                           // blocked
    data << uint32(log->HitInfo);
    data << uint8 (0);                                      // flag to use extend data
    SendMessageToSet(&data, true);
}

void Unit::SendSpellNonMeleeDamageLog(Unit* target, uint32 SpellID, uint32 Damage, SpellSchoolMask damageSchoolMask, uint32 AbsorbedDamage, uint32 Resist, bool PhysicalDamage, uint32 Blocked, bool CriticalHit)
{
    DEBUG_LOG("Sending: SMSG_SPELLNONMELEEDAMAGELOG");
    WorldPacket data(SMSG_SPELLNONMELEEDAMAGELOG, (16 + 4 + 4 + 1 + 4 + 4 + 1 + 1 + 4 + 4 + 1)); // we guess size
    data << target->GetPackGUID();
    data << GetPackGUID();
    data << uint32(SpellID);
    data << uint32(Damage - AbsorbedDamage - Resist - Blocked);
    data << uint8(damageSchoolMask);                        // spell school
    data << uint32(AbsorbedDamage);                         // AbsorbedDamage
    data << uint32(Resist);                                 // resist
    data << uint8(PhysicalDamage);                          // if 1, then client show spell name (example: %s's ranged shot hit %s for %u school or %s suffers %u school damage from %s's spell_name
    data << uint8(0);                                       // unk isFromAura
    data << uint32(Blocked);                                // blocked
    data << uint32(CriticalHit ? 0x27 : 0x25);              // hitType, flags: 0x2 - SPELL_HIT_TYPE_CRIT, 0x10 - replace caster?
    data << uint8(0);                                       // isDebug?
    SendMessageToSet(&data, true);
}

void Unit::ProcDamageAndSpell(Unit* victim, uint32 procAttacker, uint32 procVictim, uint32 procExtra, uint32 amount, WeaponAttackType attType, SpellEntry const* procSpell, bool canTrigger)
{
    // Proc all effects (auras/talents/whatever) on on victim first, then on caster

    // Now go on with a victim's events'n'auras
    // Not much to do if no flags are set or there is no victim
    if (victim && victim->IsAlive() && procVictim)
        victim->ProcDamageAndSpellFor(true, this, procVictim, procExtra, attType, procSpell, amount);

    // Not much to do if no flags are set.
    if (procAttacker && canTrigger)
        ProcDamageAndSpellFor(false, victim, procAttacker, procExtra, attType, procSpell, amount);
}

void Unit::SendSpellMiss(Unit* target, uint32 spellID, SpellMissInfo missInfo)
{
    WorldPacket data(SMSG_SPELLLOGMISS, (4+8+1+4+8+1));
    data << uint32(spellID);
    data << uint64(GetGUID());
    data << uint8(0);                                       // can be 0 or 1
    data << uint32(1);                                      // target count
    // for (i = 0; i < target count; ++i)
    data << uint64(target->GetGUID());                      // target GUID
    data << uint8(missInfo);
    // end loop
    SendMessageToSet(&data, true);
}

void Unit::SendSpellDamageImmune(Unit* target, uint32 spellId)
{
    WorldPacket data(SMSG_SPELLORDAMAGE_IMMUNE, 8 + 8 + 4 + 1);
    data << uint64(GetGUID());
    data << uint64(target->GetGUID());
    data << uint32(spellId);
    data << uint8(0); // bool - log format: 0-default, 1-debug
    SendMessageToSet(&data, true);
}

void Unit::SendAttackStateUpdate(CalcDamageInfo* damageInfo)
{
    WorldPacket data(SMSG_ATTACKERSTATEUPDATE, (16 + 84));  // we guess size
    data << (uint32)damageInfo->HitInfo;
    data << GetPackGUID();
    data << damageInfo->target->GetPackGUID();
    data << (uint32)(damageInfo->damage);     // Full damage

    data << (uint8)1;                         // Sub damage count
    //=== Sub damage description
    data << (uint32)(damageInfo->damageSchoolMask); // School of sub damage
    data << (float)damageInfo->damage;        // sub damage
    data << (uint32)damageInfo->damage;       // Sub Damage
    data << (uint32)damageInfo->absorb;       // Absorb
    data << (uint32)damageInfo->resist;       // Resist
    //=================================================
    data << (uint32)damageInfo->TargetState;
    data << (uint32)0;
    data << (uint32)0;
    data << (uint32)damageInfo->blocked_amount;
    SendMessageToSet(&data, true);/**/
}

void Unit::SendAttackStateUpdate(uint32 HitInfo, Unit* target, uint8 SwingType, SpellSchoolMask damageSchoolMask, uint32 Damage, uint32 AbsorbDamage, uint32 Resist, VictimState TargetState, uint32 BlockedAmount)
{
    DEBUG_LOG("WORLD: Sending SMSG_ATTACKERSTATEUPDATE");

    WorldPacket data(SMSG_ATTACKERSTATEUPDATE, (16 + 45));  // we guess size
    data << (uint32)HitInfo;
    data << GetPackGUID();
    data << target->GetPackGUID();
    data << (uint32)(Damage - AbsorbDamage - Resist - BlockedAmount);

    data << (uint8)SwingType;                               // count?

    // for (i = 0; i < SwingType; ++i)
    data << (uint32)damageSchoolMask;
    data << (float)(Damage - AbsorbDamage - Resist - BlockedAmount);
    // still need to double check damage
    data << (uint32)(Damage - AbsorbDamage - Resist - BlockedAmount);
    data << (uint32)AbsorbDamage;
    data << (uint32)Resist;
    // end loop

    data << (uint32)TargetState;

    if (AbsorbDamage == 0)                                 //also 0x3E8 = 0x3E8, check when that happens
        data << (uint32)0;
    else
        data << (uint32) - 1;

    data << (uint32)0;
    data << (uint32)BlockedAmount;

    SendMessageToSet(&data, true);
}

bool Unit::HandleHasteAuraProc(Unit* victim, uint32 damage, Aura* triggeredByAura, SpellEntry const* /*procSpell*/, uint32 /*procFlag*/, uint32 /*procEx*/, uint32 cooldown)
{
    SpellEntry const* hasteSpell = triggeredByAura->GetSpellProto();

    Item* castItem = triggeredByAura->GetCastItemGUID() && GetTypeId() == TYPEID_PLAYER
                     ? ToPlayer()->GetItemByGuid(triggeredByAura->GetCastItemGUID()) : NULL;

    uint32 triggered_spell_id = 0;
    Unit* target = victim;
    int32 basepoints0 = 0;

    switch (hasteSpell->SpellFamilyName)
    {
    case SPELLFAMILY_ROGUE:
        {
            switch (hasteSpell->Id)
            {
            // Blade Flurry
            case 13877:
            case 33735:
                {
                    target = SelectNearbyTarget(victim);

                    if (!target || target->IsTotem() || target->IsFriendlyTo(this))
                        return false;

                    basepoints0 = damage;
                    triggered_spell_id = 22482;
                    break;
                }
            }
            break;
        }
    }

    // processed charge only counting case
    if (!triggered_spell_id)
        return true;

    SpellEntry const* triggerEntry = sSpellStore.LookupEntry(triggered_spell_id);

    if (!triggerEntry)
    {
        sLog.outError("Unit::HandleHasteAuraProc: Spell %u has invalid triggered spell %u", hasteSpell->Id, triggered_spell_id);
        return false;
    }

    // default case
    if (!target || (target != this && !target->IsAlive()))
        return false;

    if (cooldown && GetTypeId() == TYPEID_PLAYER && ToPlayer()->HasSpellCooldown(triggered_spell_id))
        return false;

    if (basepoints0)
        CastCustomSpell(target, triggered_spell_id, &basepoints0, NULL, NULL, true, castItem, triggeredByAura);
    else
        CastSpell(target, triggered_spell_id, true, castItem, triggeredByAura);

    if (cooldown && GetTypeId() == TYPEID_PLAYER)
        ToPlayer()->AddSpellCooldown(triggered_spell_id, 0, time(NULL) + cooldown);

    return true;
}

bool Unit::HandleDummyAuraProc(Unit* victim, uint32 damage, Aura* triggeredByAura, SpellEntry const* procSpell, uint32 /*procFlag*/, uint32 procEx, uint32 cooldown)
{
    SpellEntry const* dummySpell = triggeredByAura->GetSpellProto();
    uint32 effIndex = triggeredByAura->GetEffIndex();

    Item* castItem = triggeredByAura->GetCastItemGUID() && GetTypeId() == TYPEID_PLAYER
                     ? ToPlayer()->GetItemByGuid(triggeredByAura->GetCastItemGUID()) : NULL;

    uint32 triggered_spell_id = 0;
    Unit* target = victim;
    int32 basepoints0 = 0;
    uint64 originalCaster = 0;

    switch (dummySpell->SpellFamilyName)
    {
        case SPELLFAMILY_GENERIC:
        {
            switch (dummySpell->Id)
            {
            // Eye for an Eye
            case 9799:
            case 25988:
                {
                    // prevent damage back from weapon special attacks
                    if (!procSpell || procSpell->DmgClass != SPELL_DAMAGE_CLASS_MAGIC)
                        return false;

                    // return damage % to attacker but < 50% own total health
                    basepoints0 = triggeredByAura->GetModifier()->m_amount * int32(damage) / 100;
                    if (uint32(basepoints0) > GetMaxHealth() / 2)
                        basepoints0 = GetMaxHealth() / 2;

                    triggered_spell_id = 25997;
                    break;
                }
            // Sweeping Strikes
            case 12328:
            case 18765:
            case 35429:
                {
                    // prevent chain of triggered spell from same triggered spell
                    if (procSpell && procSpell->Id == 12723)
                        return false;

                    target = SelectNearbyTarget(victim);

                    if (!target || target->IsTotem() || target->IsFriendlyTo(this))
                        return false;

                    if (procSpell && procSpell->SpellFamilyFlags == 536870912 && procSpell->SpellIconID == 1648)        // Prevent Execute proc on targets with > 20% health
                        if (target && target->GetHealth() > target->GetMaxHealth() * 0.2)
                            return false;

                    if (procSpell && procSpell->SpellIconID == 83)      // Prevent Whirlwind proc 4 times. It should proc 1 time.
                        cooldown = 1;

                    triggered_spell_id = 12723;
                    basepoints0 = CalcArmorReducedDamage(target, damage);
                    break;
                }
            // Unstable Power
            case 24658:
                {
                    if (!procSpell || procSpell->Id == 24659)
                        return false;
                    // Need remove one 24659 aura
                    RemoveSingleAuraFromStack(24659, 0);
                    RemoveSingleAuraFromStack(24659, 1);
                    return true;
                }
            // Restless Strength
            case 24661:
                {
                    // Need remove one 24662 aura
                    RemoveSingleAuraFromStack(24662, 0);
                    return true;
                }
            // Adaptive Warding (Frostfire Regalia set)
            case 28764:
                {
                    if (!procSpell)
                        return false;

                    // find Mage Armor
                    bool found = false;
                    AuraList const& mRegenInterupt = GetAurasByType(SPELL_AURA_MOD_MANA_REGEN_INTERRUPT);
                    for (AuraList::const_iterator iter = mRegenInterupt.begin(); iter != mRegenInterupt.end(); ++iter)
                    {
                        if (SpellEntry const* iterSpellProto = (*iter)->GetSpellProto())
                        {
                            if (iterSpellProto->SpellFamilyName == SPELLFAMILY_MAGE && (iterSpellProto->SpellFamilyFlags & 0x10000000))
                            {
                                found = true;
                                break;
                            }
                        }
                    }
                    if (!found)
                        return false;

                    switch (GetFirstSchoolInMask(GetSpellSchoolMask(procSpell)))
                    {
                    case SPELL_SCHOOL_NORMAL:
                    case SPELL_SCHOOL_HOLY:
                        return false;                   // ignored
                    case SPELL_SCHOOL_FIRE:
                        triggered_spell_id = 28765;
                        break;
                    case SPELL_SCHOOL_NATURE:
                        triggered_spell_id = 28768;
                        break;
                    case SPELL_SCHOOL_FROST:
                        triggered_spell_id = 28766;
                        break;
                    case SPELL_SCHOOL_SHADOW:
                        triggered_spell_id = 28769;
                        break;
                    case SPELL_SCHOOL_ARCANE:
                        triggered_spell_id = 28770;
                        break;
                    default:
                        return false;
                    }

                    target = this;
                    break;
                }
            // Obsidian Armor (Justice Bearer`s Pauldrons shoulder)
            case 27539:
                {
                    if (!procSpell)
                        return false;

                    switch (GetFirstSchoolInMask(GetSpellSchoolMask(procSpell)))
                    {
                    case SPELL_SCHOOL_NORMAL:
                        return false;                   // ignore
                    case SPELL_SCHOOL_HOLY:
                        triggered_spell_id = 27536;
                        break;
                    case SPELL_SCHOOL_FIRE:
                        triggered_spell_id = 27533;
                        break;
                    case SPELL_SCHOOL_NATURE:
                        triggered_spell_id = 27538;
                        break;
                    case SPELL_SCHOOL_FROST:
                        triggered_spell_id = 27534;
                        break;
                    case SPELL_SCHOOL_SHADOW:
                        triggered_spell_id = 27535;
                        break;
                    case SPELL_SCHOOL_ARCANE:
                        triggered_spell_id = 27540;
                        break;
                    default:
                        return false;
                    }

                    target = this;
                    break;
                }
            // Mana Leech (Passive) (Priest Pet Aura)
            case 28305:
                {
                    // Cast on owner
                    target = GetOwner();
                    if (!target)
                        return false;

                    basepoints0 = int32(damage * 2.5f);     // manaregen
                    triggered_spell_id = 34650;
                    break;
                }
            // Mark of Malice
            case 33493:
                {
                    // Cast finish spell at last charge
                    if (triggeredByAura->m_procCharges > 1)
                        return false;

                    target = this;
                    triggered_spell_id = 33494;
                    break;
                }
            // Twisted Reflection (boss spell)
            case 21063:
                triggered_spell_id = 21064;
                break;
            // Vampiric Aura (boss spell)
            case 38196:
                {
                    basepoints0 = 3 * damage;               // 300%
                    if (basepoints0 < 0)
                        return false;

                    triggered_spell_id = 31285;
                    target = this;
                    break;
                }
            // Aura of Madness (Darkmoon Card: Madness trinket)
            //=====================================================
            // 39511 Sociopath: +35 strength (Paladin, Rogue, Druid, Warrior)
            // 40997 Delusional: +70 attack power (Rogue, Hunter, Paladin, Warrior, Druid)
            // 40998 Kleptomania: +35 agility (Warrior, Rogue, Paladin, Hunter, Druid)
            // 40999 Megalomania: +41 damage/healing (Druid, Shaman, Priest, Warlock, Mage, Paladin)
            // 41002 Paranoia: +35 spell/melee/ranged crit strike rating (All classes)
            // 41005 Manic: +35 haste (spell, melee and ranged) (All classes)
            // 41009 Narcissism: +35 intellect (Druid, Shaman, Priest, Warlock, Mage, Paladin, Hunter)
            // 41011 Martyr Complex: +35 stamina (All classes)
            // 41406 Dementia: Every 5 seconds either gives you +5% damage/healing. (Druid, Shaman, Priest, Warlock, Mage, Paladin)
            // 41409 Dementia: Every 5 seconds either gives you -5% damage/healing. (Druid, Shaman, Priest, Warlock, Mage, Paladin)
            case 39446:
                {
                    if (GetTypeId() != TYPEID_PLAYER || !IsAlive())
                        return false;

                    // Select class defined buff
                    switch (getClass())
                    {
                    case CLASS_PALADIN:                 // 39511,40997,40998,40999,41002,41005,41009,41011,41409
                    case CLASS_DRUID:                   // 39511,40997,40998,40999,41002,41005,41009,41011,41409
                        {
                            uint32 RandomSpell[] = {39511, 40997, 40998, 40999, 41002, 41005, 41009, 41011, 41409};
                            triggered_spell_id = RandomSpell[ irand(0, sizeof(RandomSpell) / sizeof(uint32) - 1) ];
                            break;
                        }
                    case CLASS_ROGUE:                   // 39511,40997,40998,41002,41005,41011
                    case CLASS_WARRIOR:                 // 39511,40997,40998,41002,41005,41011
                        {
                            uint32 RandomSpell[] = {39511, 40997, 40998, 41002, 41005, 41011};
                            triggered_spell_id = RandomSpell[ irand(0, sizeof(RandomSpell) / sizeof(uint32) - 1) ];
                            break;
                        }
                    case CLASS_PRIEST:                  // 40999,41002,41005,41009,41011,41406,41409
                    case CLASS_SHAMAN:                  // 40999,41002,41005,41009,41011,41406,41409
                    case CLASS_MAGE:                    // 40999,41002,41005,41009,41011,41406,41409
                    case CLASS_WARLOCK:                 // 40999,41002,41005,41009,41011,41406,41409
                        {
                            uint32 RandomSpell[] = {40999, 41002, 41005, 41009, 41011, 41406, 41409};
                            triggered_spell_id = RandomSpell[ irand(0, sizeof(RandomSpell) / sizeof(uint32) - 1) ];
                            break;
                        }
                    case CLASS_HUNTER:                  // 40997,40999,41002,41005,41009,41011,41406,41409
                        {
                            uint32 RandomSpell[] = {40997, 40999, 41002, 41005, 41009, 41011, 41406, 41409};
                            triggered_spell_id = RandomSpell[ irand(0, sizeof(RandomSpell) / sizeof(uint32) - 1) ];
                            break;
                        }
                    default:
                        return false;
                    }

                    target = this;
                    if (roll_chance_i(10))
                        ToPlayer()->Say("This is Madness!", LANG_UNIVERSAL);
                    break;
                }
            /*
            // @todo need find item for aura and triggered spells
            // Sunwell Exalted Caster Neck (??? neck)
            // cast ??? Light's Wrath if Exalted by Aldor
            // cast ??? Arcane Bolt if Exalted by Scryers*/
            case 46569:
                return false;                           // disable for while
            // Sunwell Exalted Caster Neck (Shattered Sun Pendant of Acumen neck)
            // cast 45479 Light's Wrath if Exalted by Aldor
            // cast 45429 Arcane Bolt if Exalted by Scryers
            case 45481:
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        return false;

                    // Get Aldor reputation rank
                    if (ToPlayer()->GetReputationRank(932) == REP_EXALTED)
                    {
                        target = this;
                        triggered_spell_id = 45479;
                        break;
                    }
                    // Get Scryers reputation rank
                    if (ToPlayer()->GetReputationRank(934) == REP_EXALTED)
                    {
                        if (this->IsFriendlyTo(target))
                            return false;

                        triggered_spell_id = 45429;
                        break;
                    }
                    return false;
                }
            // Sunwell Exalted Melee Neck (Shattered Sun Pendant of Might neck)
            // cast 45480 Light's Strength if Exalted by Aldor
            // cast 45428 Arcane Strike if Exalted by Scryers
            case 45482:
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        return false;

                    // Get Aldor reputation rank
                    if (ToPlayer()->GetReputationRank(932) == REP_EXALTED)
                    {
                        target = this;
                        triggered_spell_id = 45480;
                        break;
                    }
                    // Get Scryers reputation rank
                    if (ToPlayer()->GetReputationRank(934) == REP_EXALTED)
                    {
                        triggered_spell_id = 45428;
                        break;
                    }
                    return false;
                }
            // Sunwell Exalted Tank Neck (Shattered Sun Pendant of Resolve neck)
            // cast 45431 Arcane Insight if Exalted by Aldor
            // cast 45432 Light's Ward if Exalted by Scryers
            case 45483:
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        return false;

                    // Get Aldor reputation rank
                    if (ToPlayer()->GetReputationRank(932) == REP_EXALTED)
                    {
                        target = this;
                        triggered_spell_id = 45432;
                        break;
                    }
                    // Get Scryers reputation rank
                    if (ToPlayer()->GetReputationRank(934) == REP_EXALTED)
                    {
                        target = this;
                        triggered_spell_id = 45431;
                        break;
                    }
                    return false;
                }
            // Sunwell Exalted Healer Neck (Shattered Sun Pendant of Restoration neck)
            // cast 45478 Light's Salvation if Exalted by Aldor
            // cast 45430 Arcane Surge if Exalted by Scryers
            case 45484:
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        return false;

                    // Get Aldor reputation rank
                    if (ToPlayer()->GetReputationRank(932) == REP_EXALTED)
                    {
                        target = this;
                        triggered_spell_id = 45478;
                        break;
                    }
                    // Get Scryers reputation rank
                    if (ToPlayer()->GetReputationRank(934) == REP_EXALTED)
                    {
                        triggered_spell_id = 45430;
                        break;
                    }
                    return false;
                }
            }
            break;
        }
        case SPELLFAMILY_MAGE:
        {
            // Magic Absorption
            if (dummySpell->SpellIconID == 459)             // only this spell have SpellIconID == 459 and dummy aura
            {
                if (getPowerType() != POWER_MANA)
                    return false;

                // mana reward
                basepoints0 = (triggeredByAura->GetModifier()->m_amount * GetMaxPower(POWER_MANA) / 100);
                target = this;
                triggered_spell_id = 29442;
                break;
            }
            // Master of Elements
            if (dummySpell->SpellIconID == 1920)
            {
                if (!procSpell)
                    return false;

                // mana cost save
                basepoints0 = procSpell->manaCost * triggeredByAura->GetModifier()->m_amount / 100;
                if (basepoints0 <= 0)
                    return false;

                target = this;
                triggered_spell_id = 29077;
                break;
            }
            // Arcane Potency
            if (dummySpell->SpellIconID == 2120)
            {
                if (!procSpell)
                    return false;

                target = this;

                switch (dummySpell->Id)
                {
                case 31571:
                    triggered_spell_id = 33421;
                    break;
                case 31572:
                    triggered_spell_id = 33713;
                    break;
                case 31573:
                    triggered_spell_id = 33714;
                    break;
                default:
                    sLog.outError("Unit::HandleDummyAuraProc: non handled spell id: %u", dummySpell->Id);
                    return false;
                }
                break;
            }
            // Incanter's Regalia set (add trigger chance to Mana Shield)
            if (dummySpell->SpellFamilyFlags & 0x0000000000008000LL)
            {
                if (GetTypeId() != TYPEID_PLAYER)
                    return false;

                target = this;
                triggered_spell_id = 37436;
                break;
            }
            switch (dummySpell->Id)
            {
            // Ignite
            case 11119:
            case 11120:
            case 12846:
            case 12847:
            case 12848:
                {
                    switch (dummySpell->Id)
                    {
                    case 11119:
                        basepoints0 = int32(0.04f * damage);
                        break;
                    case 11120:
                        basepoints0 = int32(0.08f * damage);
                        break;
                    case 12846:
                        basepoints0 = int32(0.12f * damage);
                        break;
                    case 12847:
                        basepoints0 = int32(0.16f * damage);
                        break;
                    case 12848:
                        basepoints0 = int32(0.20f * damage);
                        break;
                    default:
                        sLog.outError("Unit::HandleDummyAuraProc: non handled spell id: %u (IG)", dummySpell->Id);
                        return false;
                    }

                    AuraList const& DoT = victim->GetAurasByType(SPELL_AURA_PERIODIC_DAMAGE);
                    for (AuraList::const_iterator itr = DoT.begin(); itr != DoT.end(); ++itr)
                        if ((*itr)->GetId() == 12654 && (*itr)->GetCaster() == this)
                            if ((*itr)->GetBasePoints() > 0)
                                basepoints0 += int((*itr)->GetBasePoints() / ((*itr)->GetTickNumber() + 1));

                    triggered_spell_id = 12654;
                    break;
                }
            // Combustion
            case 11129:
                {
                    //last charge and crit
                    if (triggeredByAura->m_procCharges <= 1 && (procEx & PROC_EX_CRITICAL_HIT))
                    {
                        RemoveAurasDueToSpell(28682);       //-> remove Combustion auras
                        return true;                        // charge counting (will removed)
                    }

                    CastSpell(this, 28682, true, castItem, triggeredByAura);
                    return (procEx & PROC_EX_CRITICAL_HIT);// charge update only at crit hits, no hidden cooldowns
                }
            }
            break;
        }
    case SPELLFAMILY_WARRIOR:
        {
            // Retaliation
            if (dummySpell->SpellFamilyFlags == 0x0000000800000000LL)
            {
                // check attack comes not from behind
                if (!HasInArc(float(M_PI), victim))
                    return false;

                triggered_spell_id = 22858;
                break;
            }
            // Second Wind
            else if (dummySpell->SpellIconID == 1697)
            {
                // only for spells and hit/crit (trigger start always) and not start from self casted spells (5530 Mace Stun Effect for example)
                if (procSpell == 0 || !(procEx & (PROC_EX_NORMAL_HIT | PROC_EX_CRITICAL_HIT)) || this == victim)
                    return false;
                // Need stun or root mechanic
                if (procSpell->Mechanic != MECHANIC_ROOT && procSpell->Mechanic != MECHANIC_STUN)
                {
                    int32 i;
                    for (i = 0; i < 3; i++)
                        if (procSpell->EffectMechanic[i] == MECHANIC_ROOT || procSpell->EffectMechanic[i] == MECHANIC_STUN)
                            break;
                    if (i == 3)
                        return false;
                }

                switch (dummySpell->Id)
                {
                case 29838:
                    triggered_spell_id = 29842;
                    break;
                case 29834:
                    triggered_spell_id = 29841;
                    break;
                default:
                    sLog.outError("Unit::HandleDummyAuraProc: non handled spell id: %u (SW)", dummySpell->Id);
                    return false;
                }

                target = this;
                break;
            }
            break;
        }
    case SPELLFAMILY_WARLOCK:
        {
            // Seed of Corruption
            if (dummySpell->SpellFamilyFlags & 0x0000001000000000LL)
            {
                if (procSpell && procSpell->Id == 27285)
                    return false;
                Modifier* mod = triggeredByAura->GetModifier();
                // if damage is more than need or target die from damage deal finish spell
                if (mod->m_amount <= int64(damage) || GetHealth() <= damage)
                {
                    // remember guid before aura delete
                    uint64 casterGuid = triggeredByAura->GetCasterGUID();

                    // Remove aura (before cast for prevent infinite loop handlers)
                    RemoveAurasDueToSpell(triggeredByAura->GetId());

                    // Cast finish spell (triggeredByAura already not exist!)
                    if (Unit* caster = GetUnit(*this, casterGuid))
                        caster->CastSpell(this, 27285, true, castItem);
                    return true;                            // no hidden cooldown
                }

                // Damage counting
                mod->m_amount -= damage;
                return true;
            }
            // Seed of Corruption (Mobs cast) - no die req
            if (dummySpell->SpellFamilyFlags == 0x00LL && dummySpell->SpellIconID == 1932)
            {
                Modifier* mod = triggeredByAura->GetModifier();
                // if damage is more than need deal finish spell
                if (mod->m_amount <= int64(damage))
                {
                    // remember guid before aura delete
                    uint64 casterGuid = triggeredByAura->GetCasterGUID();

                    // Remove aura (before cast for prevent infinite loop handlers)
                    RemoveAurasDueToSpell(triggeredByAura->GetId());

                    // Cast finish spell (triggeredByAura already not exist!)
                    if (Unit* caster = GetUnit(*this, casterGuid))
                        caster->CastSpell(this, 32865, true, castItem);
                    return true;                            // no hidden cooldown
                }
                // Damage counting
                mod->m_amount -= damage;
                return true;
            }
            switch (dummySpell->Id)
            {
            // Nightfall
            case 18094:
            case 18095:
                {
                    target = this;
                    triggered_spell_id = 17941;
                    break;
                }
            //Soul Leech
            case 30293:
            case 30295:
            case 30296:
                {
                    // health
                    basepoints0 = int32(damage * triggeredByAura->GetModifier()->m_amount / 100);
                    target = this;
                    triggered_spell_id = 30294;
                    break;
                }
            // Shadowflame (Voidheart Raiment set bonus)
            case 37377:
                {
                    triggered_spell_id = 37379;
                    break;
                }
            // Pet Healing (Corruptor Raiment or Rift Stalker Armor)
            case 37381:
                {
                    target = GetGuardianPet();
                    if (!target)
                        return false;

                    // heal amount
                    basepoints0 = damage * triggeredByAura->GetModifier()->m_amount / 100;
                    triggered_spell_id = 37382;
                    break;
                }
            // Shadowflame Hellfire (Voidheart Raiment set bonus)
            case 39437:
                {
                    triggered_spell_id = 37378;
                    break;
                }
            }
            break;
        }
    case SPELLFAMILY_PRIEST:
        {
            // Vampiric Touch
            if (dummySpell->SpellFamilyFlags & 0x0000040000000000LL)
            {
                if (!victim || !victim->IsAlive())
                    return false;

                // victim is caster of aura
                if (triggeredByAura->GetCasterGUID() != victim->GetGUID())
                    return false;

                // energize amount
                basepoints0 = triggeredByAura->GetModifier()->m_amount * damage / 100;
                victim->CastCustomSpell(victim, 34919, &basepoints0, NULL, NULL, true, castItem, triggeredByAura);
                return true;                                // no hidden cooldown
            }
            switch (dummySpell->Id)
            {
            // Vampiric Embrace
            case 15286:
                {
                    if (!victim || !victim->IsAlive())
                        return false;

                    // victim is caster of aura
                    if (triggeredByAura->GetCasterGUID() != victim->GetGUID())
                        return false;

                    // heal amount
                    basepoints0 = triggeredByAura->GetModifier()->m_amount * damage / 100;
                    victim->CastCustomSpell(victim, 15290, &basepoints0, NULL, NULL, true, castItem, triggeredByAura);
                    return true;                                // no hidden cooldown
                }
            // Priest Tier 6 Trinket (Ashtongue Talisman of Acumen)
            case 40438:
                {
                    // Shadow Word: Pain
                    if (procSpell->SpellFamilyFlags & 0x0000000000008000LL)
                        triggered_spell_id = 40441;
                    // Renew
                    else if (procSpell->SpellFamilyFlags & 0x0000000000000040LL)
                        triggered_spell_id = 40440;
                    else
                        return false;

                    target = this;
                    break;
                }
            // Oracle Healing Bonus ("Garments of the Oracle" set)
            case 26169:
                {
                    // heal amount
                    basepoints0 = int32(damage * 10 / 100);
                    target = this;
                    triggered_spell_id = 26170;
                    break;
                }
            // Frozen Shadoweave (Shadow's Embrace set) warning! its not only priest set
            case 39372:
                {
                    if (!procSpell || (GetSpellSchoolMask(procSpell) & (SPELL_SCHOOL_MASK_FROST | SPELL_SCHOOL_MASK_SHADOW)) == 0)
                        return false;

                    // heal amount
                    basepoints0 = int32(damage * 2 / 100);
                    target = this;
                    triggered_spell_id = 39373;
                    break;
                }
            // Greater Heal (Vestments of Faith (Priest Tier 3) - 4 pieces bonus)
            case 28809:
                {
                    triggered_spell_id = 28810;
                    break;
                }
            }
            break;
        }
    case SPELLFAMILY_DRUID:
        {
            switch (dummySpell->Id)
            {
            // Healing Touch (Dreamwalker Raiment set)
            case 28719:
                {
                    // mana back
                    basepoints0 = int32(procSpell->manaCost * 30 / 100);
                    target = this;
                    triggered_spell_id = 28742;
                    break;
                }
            // Healing Touch Refund (Idol of Longevity trinket)
            case 28847:
                {
                    target = this;
                    triggered_spell_id = 28848;
                    break;
                }
            // Mana Restore (Malorne Raiment set / Malorne Regalia set)
            case 37288:
            case 37295:
                {
                    target = this;
                    triggered_spell_id = 37238;
                    break;
                }
            // Druid Tier 6 Trinket
            case 40442:
                {
                    float  chance;

                    // Starfire
                    if (procSpell->SpellFamilyFlags & 0x0000000000000004LL)
                    {
                        triggered_spell_id = 40445;
                        chance = 25.0f;
                    }
                    // Rejuvenation
                    else if (procSpell->SpellFamilyFlags & 0x0000000000000010LL)
                    {
                        triggered_spell_id = 40446;
                        chance = 25.0f;
                    }
                    // Mangle (Bear) and Mangle (Cat)
                    else if (procSpell->SpellFamilyFlags & 0x0000044000000000LL)
                    {
                        triggered_spell_id = 40452;
                        chance = 40.0f;
                    }
                    else
                        return false;

                    if (!roll_chance_f(chance))
                        return false;

                    target = this;
                    break;
                }
            // Maim Interrupt
            case 44835:
                {
                    // Deadly Interrupt Effect
                    triggered_spell_id = 32747;
                    break;
                }
            }
            break;
        }
    case SPELLFAMILY_ROGUE:
        {
            switch (dummySpell->Id)
            {
            // Deadly Throw Interrupt
            case 32748:
                {
                    // Prevent cast Deadly Throw Interrupt on self from last effect (apply dummy) of Deadly Throw
                    if (this == victim)
                        return false;

                    triggered_spell_id = 32747;
                    break;
                }
            }
            /*switch (dummySpell->SpellIconID) // @todo Should this be here?
            {*/ 
            // Quick Recovery
            if (dummySpell->SpellIconID == 2116)
            {
                if (!procSpell)
                    return false;

                // only rogue's finishing moves (maybe need additional checks)
                if (procSpell->SpellFamilyName != SPELLFAMILY_ROGUE ||
                    (procSpell->SpellFamilyFlags & SPELLFAMILYFLAG_ROGUE__FINISHING_MOVE) == 0)
                    return false;

                // energy cost save
                basepoints0 = procSpell->manaCost * triggeredByAura->GetModifier()->m_amount / 100;
                if (basepoints0 <= 0)
                    return false;

                target = this;
                triggered_spell_id = 31663;
                break;
            }
            break;
        }
    case SPELLFAMILY_HUNTER:
        {
            // Thrill of the Hunt
            if (dummySpell->SpellIconID == 2236)
            {
                if (!procSpell)
                    return false;

                // mana cost save
                basepoints0 = procSpell->manaCost * 40 / 100;
                if (basepoints0 <= 0)
                    return false;

                target = this;
                triggered_spell_id = 34720;
                break;
            }
            break;
        }
    case SPELLFAMILY_PALADIN:
        {
            // Seal of Righteousness - melee proc dummy
            if (dummySpell->SpellFamilyFlags & 0x000000008000000LL && triggeredByAura->GetEffIndex() == 0)
            {
                if (GetTypeId() != TYPEID_PLAYER)
                    return false;

                uint32 spellId;
                switch (triggeredByAura->GetId())
                {
                case 21084:
                    spellId = 25742;
                    break;     // Rank 1
                case 20287:
                    spellId = 25740;
                    break;     // Rank 2
                case 20288:
                    spellId = 25739;
                    break;     // Rank 3
                case 20289:
                    spellId = 25738;
                    break;     // Rank 4
                case 20290:
                    spellId = 25737;
                    break;     // Rank 5
                case 20291:
                    spellId = 25736;
                    break;     // Rank 6
                case 20292:
                    spellId = 25735;
                    break;     // Rank 7
                case 20293:
                    spellId = 25713;
                    break;     // Rank 8
                case 27155:
                    spellId = 27156;
                    break;     // Rank 9
                default:
                    sLog.outError("Unit::HandleDummyAuraProc: non handled possibly SoR (Id = %u)", triggeredByAura->GetId());
                    return false;
                }
                Item* item = ToPlayer()->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
                float speed = (item ? item->GetProto()->Delay : BASE_ATTACK_TIME) / 1000.0f;

                float damageBasePoints;
                if (item && item->GetProto()->InventoryType == INVTYPE_2HWEAPON)
                    // two hand weapon
                    damageBasePoints = 1.20f * triggeredByAura->GetModifier()->m_amount * 1.2f * 1.03f * speed / 100.0f + 1;
                else
                    // one hand weapon/no weapon
                    damageBasePoints = 0.85f * ceil(triggeredByAura->GetModifier()->m_amount * 1.2f * 1.03f * speed / 100.0f) - 1;

                int32 damagePoint = int32(damageBasePoints + 0.03f * (GetWeaponDamageRange(BASE_ATTACK, MINDAMAGE) + GetWeaponDamageRange(BASE_ATTACK, MAXDAMAGE)) / 2.0f) + 1;

                // apply damage bonuses manually
                if (damagePoint >= 0)
                    damagePoint = SpellDamageBonus(victim, dummySpell, damagePoint, SPELL_DIRECT_DAMAGE);

                CastCustomSpell(victim, spellId, &damagePoint, NULL, NULL, true, NULL, triggeredByAura);
                return true;                                // no hidden cooldown
            }
            // Seal of Blood do damage trigger
            if (dummySpell->SpellFamilyFlags & 0x0000040000000000LL)
            {
                switch (triggeredByAura->GetEffIndex())
                {
                case 0:
                    triggered_spell_id = 31893;
                    break;
                case 1:
                    {
                        // damage
                        damage += CalculateDamage(BASE_ATTACK, false, true) * 35 / 100; // add spell damage from prev effect (35%)
                        basepoints0 =  triggeredByAura->GetModifier()->m_amount * damage / 100;

                        target = this;

                        triggered_spell_id = 32221;
                        break;
                    }
                }
            }

            switch (dummySpell->Id)
            {
            // Holy Power (Redemption Armor set)
            case 28789:
                {
                    if (!victim)
                        return false;

                    // Set class defined buff
                    switch (victim->getClass())
                    {
                    case CLASS_PALADIN:
                    case CLASS_PRIEST:
                    case CLASS_SHAMAN:
                    case CLASS_DRUID:
                        triggered_spell_id = 28795;     // Increases the friendly target's mana regeneration by $s1 per 5 sec. for $d.
                        break;
                    case CLASS_MAGE:
                    case CLASS_WARLOCK:
                        triggered_spell_id = 28793;     // Increases the friendly target's spell damage and healing by up to $s1 for $d.
                        break;
                    case CLASS_HUNTER:
                    case CLASS_ROGUE:
                        triggered_spell_id = 28791;     // Increases the friendly target's attack power by $s1 for $d.
                        break;
                    case CLASS_WARRIOR:
                        triggered_spell_id = 28790;     // Increases the friendly target's armor
                        break;
                    default:
                        return false;
                    }
                    break;
                }
            //Seal of Vengeance
            case 31801:
            {
                if  (triggeredByAura->GetEffIndex() == 0)
                {
                    uint32 stacks = 0;

                    triggered_spell_id = 31803;

                    // Since Patch 2.2.0 Seal of Vengeance does additional damage against fully stacked targets
                    // Add 5-stack effect from Holy Vengeance

                    AuraList const& auras = target->GetAurasByType(SPELL_AURA_PERIODIC_DAMAGE);
                    for (AuraList::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
                    {
                        if (((*itr)->GetId() == 31803) && (*itr)->GetCasterGUID() == GetGUID())
                        {
                            stacks = (*itr)->GetStackAmount();
                            break;
                        }
                    }

                    if (stacks >= 5)
                        CastSpell(target, 42463, true, NULL, triggeredByAura);
                }
                break;
            }
            // Spiritual Att.
            case 31785:
            case 33776:
                {
                    // if healed by another unit (victim)
                    if (this == victim)
                        return false;

                    // heal amount
                    basepoints0 = triggeredByAura->GetModifierValue() * std::min(damage, GetMaxHealth() - GetHealth()) / 100;
                    target = this;

                    if (basepoints0)
                        triggered_spell_id = 31786;
                    break;
                }
            // Paladin Tier 6 Trinket (Ashtongue Talisman of Zeal)
            case 40470:
                {
                    if (!procSpell)
                        return false;

                    float  chance;

                    // Flash of light/Holy light
                    if (procSpell->SpellFamilyFlags & 0x00000000C0000000LL)
                    {
                        triggered_spell_id = 40471;
                        chance = 15.0f;
                    }
                    // Judgement
                    else if (procSpell->SpellFamilyFlags & 0x0000000000800000LL)
                    {
                        triggered_spell_id = 40472;
                        chance = 50.0f;
                    }
                    else
                        return false;

                    if (!roll_chance_f(chance))
                        return false;

                    break;
                }
            }
            break;
        }
    case SPELLFAMILY_SHAMAN:
        {
            switch (dummySpell->Id)
            {
            // Totemic Power (The Earthshatterer set)
            case 28823:
                {
                    if (!victim)
                        return false;

                    // Set class defined buff
                    switch (victim->getClass())
                    {
                    case CLASS_PALADIN:
                    case CLASS_PRIEST:
                    case CLASS_SHAMAN:
                    case CLASS_DRUID:
                        triggered_spell_id = 28824;     // Increases the friendly target's mana regeneration by $s1 per 5 sec. for $d.
                        break;
                    case CLASS_MAGE:
                    case CLASS_WARLOCK:
                        triggered_spell_id = 28825;     // Increases the friendly target's spell damage and healing by up to $s1 for $d.
                        break;
                    case CLASS_HUNTER:
                    case CLASS_ROGUE:
                        triggered_spell_id = 28826;     // Increases the friendly target's attack power by $s1 for $d.
                        break;
                    case CLASS_WARRIOR:
                        triggered_spell_id = 28827;     // Increases the friendly target's armor
                        break;
                    default:
                        return false;
                    }
                    break;
                }
            // Lesser Healing Wave (Totem of Flowing Water Relic)
            case 28849:
                {
                    target = this;
                    triggered_spell_id = 28850;
                    break;
                }
            // Windfury Weapon (Passive) 1-5 Ranks
            case 33757:
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        return false;

                    if (!castItem || !castItem->IsEquipped())
                        return false;

                    if (triggeredByAura && castItem->GetGUID() != triggeredByAura->GetCastItemGUID())
                        return false;

                    // custom cooldown processing case
                    if (cooldown && ToPlayer()->HasSpellCooldown(dummySpell->Id))
                        return false;

                    uint32 spellId;
                    switch (castItem->GetEnchantmentId(EnchantmentSlot(TEMP_ENCHANTMENT_SLOT)))
                    {
                    case 283:
                        spellId = 33757;
                        break;   //1 Rank
                    case 284:
                        spellId = 33756;
                        break;   //2 Rank
                    case 525:
                        spellId = 33755;
                        break;   //3 Rank
                    case 1669:
                        spellId = 33754;
                        break;   //4 Rank
                    case 2636:
                        spellId = 33727;
                        break;   //5 Rank
                    default:
                        {
                            sLog.outError("Unit::HandleDummyAuraProc: non handled item enchantment (rank?) %u for spell id: %u (Windfury)",
                                          castItem->GetEnchantmentId(EnchantmentSlot(TEMP_ENCHANTMENT_SLOT)), dummySpell->Id);
                            return false;
                        }
                    }

                    SpellEntry const* windfurySpellEntry = sSpellStore.LookupEntry(spellId);
                    if (!windfurySpellEntry)
                    {
                        sLog.outError("Unit::HandleDummyAuraProc: invalid spell id: %u (Windfury)", spellId);
                        return false;
                    }

                    int32 extra_attack_power = CalculateSpellDamage(windfurySpellEntry, 0, windfurySpellEntry->EffectBasePoints[0], victim);

                    // Off-Hand case
                    if (castItem->GetSlot() == EQUIPMENT_SLOT_OFFHAND)
                    {
                        // Value gained from additional AP
                        basepoints0 = int32(extra_attack_power / 14.0f * GetAttackTime(OFF_ATTACK) / 1000 / 2);
                        triggered_spell_id = 33750;
                    }
                    // Main-Hand case
                    else
                    {
                        // Value gained from additional AP
                        basepoints0 = int32(extra_attack_power / 14.0f * GetAttackTime(BASE_ATTACK) / 1000);
                        triggered_spell_id = 25504;
                    }

                    // apply cooldown before cast to prevent processing itself
                    if (cooldown)
                        ToPlayer()->AddSpellCooldown(dummySpell->Id, 0, time(NULL) + cooldown);

                    // Attack Twice
                    for (uint32 i = 0; i < 2; ++i)
                        CastCustomSpell(victim, triggered_spell_id, &basepoints0, NULL, NULL, true, castItem, triggeredByAura);

                    return true;
                }
            // Shaman Tier 6 Trinket
            case 40463:
                {
                    if (!procSpell)
                        return false;

                    float chance;
                    if (procSpell->SpellFamilyFlags & 0x0000000000000001LL)
                    {
                        triggered_spell_id = 40465;         // Lightning Bolt
                        chance = 15.0f;
                    }
                    else if (procSpell->SpellFamilyFlags & 0x0000000000000080LL)
                    {
                        triggered_spell_id = 40465;         // Lesser Healing Wave
                        chance = 10.0f;
                    }
                    else if (procSpell->SpellFamilyFlags & 0x0000001000000000LL)
                    {
                        triggered_spell_id = 40466;         // Stormstrike
                        chance = 50.0f;
                    }
                    else
                        return false;

                    if (!roll_chance_f(chance))
                        return false;

                    target = this;
                    break;
                }
            }

            // Earth Shield
            if (dummySpell->SpellFamilyFlags == 0x40000000000LL)
            {
                // Now correctly uses the Shaman's own spell critical strike chance to determine the chance of a critical heal.
                originalCaster = triggeredByAura->GetCasterGUID();
                target = this;
                basepoints0 = triggeredByAura->GetModifier()->m_amount;
                triggered_spell_id = 379;
                break;
            }
            // Lightning Overload
            if (dummySpell->SpellIconID == 2018)            // only this spell have SpellFamily Shaman SpellIconID == 2018 and dummy aura
            {
                if (!procSpell || GetTypeId() != TYPEID_PLAYER || !victim)
                    return false;

                // custom cooldown processing case
                if (cooldown && GetTypeId() == TYPEID_PLAYER && ToPlayer()->HasSpellCooldown(dummySpell->Id))
                    return false;

                uint32 spellId = 0;
                // Every Lightning Bolt and Chain Lightning spell have duplicate vs half damage and zero cost
                switch (procSpell->Id)
                {
                // Lightning Bolt
                case   403:
                    spellId = 45284;
                    break;     // Rank  1
                case   529:
                    spellId = 45286;
                    break;     // Rank  2
                case   548:
                    spellId = 45287;
                    break;     // Rank  3
                case   915:
                    spellId = 45288;
                    break;     // Rank  4
                case   943:
                    spellId = 45289;
                    break;     // Rank  5
                case  6041:
                    spellId = 45290;
                    break;     // Rank  6
                case 10391:
                    spellId = 45291;
                    break;     // Rank  7
                case 10392:
                    spellId = 45292;
                    break;     // Rank  8
                case 15207:
                    spellId = 45293;
                    break;     // Rank  9
                case 15208:
                    spellId = 45294;
                    break;     // Rank 10
                case 25448:
                    spellId = 45295;
                    break;     // Rank 11
                case 25449:
                    spellId = 45296;
                    break;     // Rank 12
                // Chain Lightning
                case   421:
                    spellId = 45297;
                    break;     // Rank  1
                case   930:
                    spellId = 45298;
                    break;     // Rank  2
                case  2860:
                    spellId = 45299;
                    break;     // Rank  3
                case 10605:
                    spellId = 45300;
                    break;     // Rank  4
                case 25439:
                    spellId = 45301;
                    break;     // Rank  5
                case 25442:
                    spellId = 45302;
                    break;     // Rank  6
                default:
                    sLog.outError("Unit::HandleDummyAuraProc: non handled spell id: %u (LO)", procSpell->Id);
                    return false;
                }
                // No thread generated mod
                SpellModifier* mod = new SpellModifier;
                mod->op = SPELLMOD_THREAT;
                mod->value = -100;
                mod->type = SPELLMOD_PCT;
                mod->spellId = dummySpell->Id;
                mod->effectId = 0;
                mod->lastAffected = NULL;
                mod->mask = 0x0000000000000003LL;
                mod->charges = 0;
                ToPlayer()->AddSpellMod(mod, true);

                // Remove cooldown (Chain Lightning - have Category Recovery time)
                if (procSpell->SpellFamilyFlags & 0x0000000000000002LL)
                    ToPlayer()->RemoveSpellCooldown(spellId);

                // Hmmm.. in most case spells already set half basepoints but...
                // Lightning Bolt (2-10 rank) have full basepoint and half bonus from level
                // As on wiki:
                // BUG: Rank 2 to 10 (and maybe 11) of Lightning Bolt will proc another Bolt with FULL damage (not halved). This bug is known and will probably be fixed soon.
                // So - no add changes :)
                CastSpell(victim, spellId, true, castItem, triggeredByAura);

                ToPlayer()->AddSpellMod(mod, false);

                if (cooldown && GetTypeId() == TYPEID_PLAYER)
                    ToPlayer()->AddSpellCooldown(dummySpell->Id, 0, time(NULL) + cooldown);

                return true;
            }
            break;
        }
    case SPELLFAMILY_POTION:
        {
            // alchemist's stone
            if (dummySpell->Id == 17619)
            {
                if (procSpell->SpellFamilyName == SPELLFAMILY_POTION)
                {
                    for (uint8 i = 0; i < 3; i++)
                    {
                        if (procSpell->Effect[i] == SPELL_EFFECT_HEAL)
                            triggered_spell_id = 21399;
                        else if (procSpell->Effect[i] == SPELL_EFFECT_ENERGIZE)
                            triggered_spell_id = 21400;
                        else
                            continue;

                        basepoints0 = CalculateSpellDamage(procSpell, i, procSpell->EffectBasePoints[i], this) * 0.4f;
                        CastCustomSpell(this, triggered_spell_id, &basepoints0, NULL, NULL, true, castItem, triggeredByAura);
                    }
                    return true;
                }
            }
        }
    default:
        break;
    }

    // processed charge only counting case
    if (!triggered_spell_id)
        return true;

    SpellEntry const* triggerEntry = sSpellStore.LookupEntry(triggered_spell_id);

    if (!triggerEntry)
    {
        sLog.outError("Unit::HandleDummyAuraProc: Spell %u has invalid triggered spell %u", dummySpell->Id, triggered_spell_id);
        return false;
    }

    // default case
    if (!target || (target != this && !target->IsAlive()))
        return false;

    if (cooldown && GetTypeId() == TYPEID_PLAYER && ToPlayer()->HasSpellCooldown(triggered_spell_id))
        return false;

    if (basepoints0)
        CastCustomSpell(target, triggered_spell_id, &basepoints0, NULL, NULL, true, castItem, triggeredByAura, originalCaster);
    else
        CastSpell(target, triggered_spell_id, true, castItem, triggeredByAura, originalCaster);

    if (cooldown && GetTypeId() == TYPEID_PLAYER)
        ToPlayer()->AddSpellCooldown(triggered_spell_id, 0, time(NULL) + cooldown);

    return true;
}

bool Unit::HandleProcTriggerSpell(Unit* victim, uint32 damage, Aura* triggeredByAura, SpellEntry const* procSpell, uint32 procFlags, uint32 /*procEx*/, uint32 cooldown)
{
    // Get triggered aura spell info
    SpellEntry const* auraSpellInfo = triggeredByAura->GetSpellProto();

    // Basepoints of trigger aura
    int32 triggerAmount = triggeredByAura->GetModifier()->m_amount;

    // Set trigger spell id, target, custom basepoints
    uint32 trigger_spell_id = auraSpellInfo->EffectTriggerSpell[triggeredByAura->GetEffIndex()];

    Unit* target = NULL;
    int32  basepoints0 = 0;

    // Need to be able to adjust the default behaviour for if the proc makes it all the way through this method.
    // This will most often be true, telling the server to remove a charge.
    // However, cases exist where it needs to be false instead to preserve the charge.
    bool removeCharge = true;

    if (triggeredByAura->GetModifier()->m_auraname == SPELL_AURA_PROC_TRIGGER_SPELL_WITH_VALUE)
        basepoints0 = triggerAmount;

    Item* castItem = triggeredByAura->GetCastItemGUID() && GetTypeId() == TYPEID_PLAYER
                     ? ToPlayer()->GetItemByGuid(triggeredByAura->GetCastItemGUID()) : NULL;

    // Try handle unknown trigger spells
    if (sSpellStore.LookupEntry(trigger_spell_id) == NULL)
    {
        switch (auraSpellInfo->SpellFamilyName)
        {
        case SPELLFAMILY_GENERIC:
            switch (auraSpellInfo->Id)
            {
            case 23780:             // Aegis of Preservation (Aegis of Preservation trinket)
                trigger_spell_id = 23781;
                break;
            case 33896:             // Desperate Defense (Stonescythe Whelp, Stonescythe Alpha, Stonescythe Ambusher)
                trigger_spell_id = 33898;
                break;
            case 43820:             // Charm of the Witch Doctor (Amani Charm of the Witch Doctor trinket)
                // Pct value stored in dummy
                basepoints0 = victim->GetCreateHealth() * auraSpellInfo->EffectBasePoints[1] / 100;
                target = victim;
                break;
            case 27522:             // Mana Drain Trigger of Black Grasp of the Destroyer
            case 46939:             // Mana Drain Trigger of Black Bow of the Betrayer
                {
                    // On successful melee or ranged attack gain 8 mana and if possible drain 8 mana from the target.
                    if (IsAlive())
                        CastSpell(this, 29471, true, castItem, triggeredByAura);
                    if (victim && victim->IsAlive() && victim->getPowerType() == POWER_MANA && victim->GetPower(POWER_MANA) >= 8)
                    {
                        CastSpell(victim, 27526, true, castItem, triggeredByAura);
                        CastSpell(this, 29471, true, castItem, triggeredByAura);
                    }
                    return true;
                }
            // Moonkin Form (Passive)
            case 24905:
                {
                    // Elune's Touch (instead non-existed triggered spell) 30% from AP
                    trigger_spell_id = 33926;
                    basepoints0 = GetTotalAttackPowerValue(BASE_ATTACK) * 30 / 100;
                    target = this;
                }
            }
            break;
        case SPELLFAMILY_MAGE:
            if (auraSpellInfo->SpellIconID == 2127)     // Blazing Speed
            {
                switch (auraSpellInfo->Id)
                {
                case 31641:  // Rank 1
                case 31642:  // Rank 2
                    trigger_spell_id = 31643;
                    break;
                default:
                    sLog.outError("Unit::HandleProcTriggerSpell: Spell %u miss possibly Blazing Speed", auraSpellInfo->Id);
                    return false;
                }
            }
            break;
        case SPELLFAMILY_WARRIOR:
            // Rampage
            if (auraSpellInfo->SpellIconID == 2006 && auraSpellInfo->SpellFamilyFlags == 0x100000)
            {
                switch (auraSpellInfo->Id)
                {
                case 29801:
                    trigger_spell_id = 30029;
                    break;       // Rank 1
                case 30030:
                    trigger_spell_id = 30031;
                    break;       // Rank 2
                case 30033:
                    trigger_spell_id = 30032;
                    break;       // Rank 3
                default:
                    sLog.outError("Unit::HandleProcTriggerSpell: Spell %u not handled in Rampage", auraSpellInfo->Id);
                    return false;
                }
            }
            break;
        case SPELLFAMILY_WARLOCK:
            {
                // Pyroclasm
                if (auraSpellInfo->SpellIconID == 1137)
                {
                    if (!victim || !victim->IsAlive() || victim == this || procSpell == NULL)
                        return false;
                    // Calculate spell tick count for spells
                    uint32 tick = 1; // Default tick = 1

                    // Hellfire have 15 tick
                    if (procSpell->SpellFamilyFlags & 0x0000000000000040LL)
                        tick = 15;
                    // Rain of Fire have 4 tick
                    else if (procSpell->SpellFamilyFlags & 0x0000000000000020LL)
                        tick = 4;

                    // Calculate chance = baseChance / tick
                    float chance = 0;
                    switch (auraSpellInfo->Id)
                    {
                    case 18096:
                        chance = 13.0f / tick;
                        break;
                    case 18073:
                        chance = 26.0f / tick;
                        break;
                    }
                    // Roll chance
                    if (!roll_chance_f(chance))
                        return false;

                    trigger_spell_id = 18093;
                }
                // Drain Soul
                else if (auraSpellInfo->SpellFamilyFlags & 0x4000)
                {
                    // Improved Drain Soul
                    Unit::AuraList const& mAddFlatModifier = GetAurasByType(SPELL_AURA_ADD_FLAT_MODIFIER);
                    for (Unit::AuraList::const_iterator i = mAddFlatModifier.begin(); i != mAddFlatModifier.end(); ++i)
                    {

                        // If the current aura being examined is not a rank of Improved Drain Soul,
                        //  skip the current aura and keep looking.
                        if((*i)->GetSpellProto()->SpellIconID != 113)
                            continue;

                        int32 value2 = CalculateSpellDamage((*i)->GetSpellProto(), 2, (*i)->GetSpellProto()->EffectBasePoints[2], this);
                        basepoints0 = int32(CalculatePct(GetMaxPower(POWER_MANA), value2));
                        DEBUG_LOG("Granting %d mana from Improved Drain Soul",basepoints0);

                        break;

                    }
                    trigger_spell_id = 18371;

                    // Not remove charge (aura removed on death in any cases)
                    // Need for correct work Drain Soul SPELL_AURA_CHANNEL_DEATH_ITEM aura
                    removeCharge = false;
                }
                break;
            }
        case SPELLFAMILY_PRIEST:
            {
                // Greater Heal Refund
                if (auraSpellInfo->Id == 37594)
                    trigger_spell_id = 37595;
                // Shadowguard
                else if (auraSpellInfo->SpellFamilyFlags == 0x100080000000LL && auraSpellInfo->SpellVisual == 7958)
                {
                    switch (auraSpellInfo->Id)
                    {
                    case 18137:
                        trigger_spell_id = 28377;
                        break;  // Rank 1
                    case 19308:
                        trigger_spell_id = 28378;
                        break;  // Rank 2
                    case 19309:
                        trigger_spell_id = 28379;
                        break;  // Rank 3
                    case 19310:
                        trigger_spell_id = 28380;
                        break;  // Rank 4
                    case 19311:
                        trigger_spell_id = 28381;
                        break;  // Rank 5
                    case 19312:
                        trigger_spell_id = 28382;
                        break;  // Rank 6
                    case 25477:
                        trigger_spell_id = 28385;
                        break;  // Rank 7
                    default:
                        sLog.outError("Unit::HandleProcTriggerSpell: Spell %u not handled in SG", auraSpellInfo->Id);
                        return false;
                    }
                }
                // Blessed Recovery
                else if (auraSpellInfo->SpellIconID == 1875)
                {
                    switch (auraSpellInfo->Id)
                    {
                    case 27811:
                        trigger_spell_id = 27813;
                        break;
                    case 27815:
                        trigger_spell_id = 27817;
                        break;
                    case 27816:
                        trigger_spell_id = 27818;
                        break;
                    default:
                        sLog.outError("Unit::HandleProcTriggerSpell: Spell %u not handled in BR", auraSpellInfo->Id);
                        return false;
                    }
                    basepoints0 = damage * triggerAmount / 100 / 3;
                    target = this;
                }
                break;
            }
        case SPELLFAMILY_DRUID:
            {
                // Druid Forms Trinket
                if (auraSpellInfo->Id == 37336)
                {
                    switch (m_form)
                    {
                    case 0:
                        trigger_spell_id = 37344;
                        break;
                    case FORM_CAT:
                        trigger_spell_id = 37341;
                        break;
                    case FORM_BEAR:
                    case FORM_DIREBEAR:
                        trigger_spell_id = 37340;
                        break;
                    case FORM_TREE:
                        trigger_spell_id = 37342;
                        break;
                    case FORM_MOONKIN:
                        trigger_spell_id = 37343;
                        break;
                    default:
                        return false;
                    }
                }
                // Leader of the Pack
                else if (auraSpellInfo->Id == 24932)
                {
                    if (triggerAmount == 0)
                        return false;
                    basepoints0 = int32(CountPctFromMaxHealth(triggerAmount));
                    trigger_spell_id = 34299;
                }
                break;
            }
        case SPELLFAMILY_PALADIN:
            {
                switch (auraSpellInfo->Id)
                {
                // Healing Discount
                case 37705:
                    {
                        trigger_spell_id = 37706;
                        target = this;
                        break;
                    }
                // Lightning Capacitor
                case 37657:
                    {
                        if (!victim || !victim->IsAlive())
                            return false;
                        // stacking
                        CastSpell(this, 37658, true, NULL, triggeredByAura);

                        Aura* dummy = GetDummyAura(37658);
                        // release at 3 aura in stack (cont contain in basepoint of trigger aura)
                        if (!dummy || dummy->GetStackAmount() <= 2)
                            return false;

                        RemoveAurasDueToSpell(37658);
                        trigger_spell_id = 37661;
                        target = victim;
                        break;
                    }
                default:
                    // Judgement of Light and Judgement of Wisdom
                    if (auraSpellInfo->SpellFamilyFlags & 0x0000000000080000LL)
                    {
                        switch (auraSpellInfo->Id)
                        {
                        // Judgement of Light
                        case 20185:
                            trigger_spell_id = 20267;
                            break;  // Rank 1
                        case 20344:
                            trigger_spell_id = 20341;
                            break;  // Rank 2
                        case 20345:
                            trigger_spell_id = 20342;
                            break;  // Rank 3
                        case 20346:
                            trigger_spell_id = 20343;
                            break;  // Rank 4
                        case 27162:
                            trigger_spell_id = 27163;
                            break;  // Rank 5
                        // Judgement of Wisdom
                        case 20186:
                            trigger_spell_id = 20268;
                            break;  // Rank 1
                        case 20354:
                            trigger_spell_id = 20352;
                            break;  // Rank 2
                        case 20355:
                            trigger_spell_id = 20353;
                            break;  // Rank 3
                        case 27164:
                            trigger_spell_id = 27165;
                            break;  // Rank 4
                        default:
                            sLog.outError("Unit::HandleProcTriggerSpell: Spell %u miss possibly Judgement of Light/Wisdom", auraSpellInfo->Id);
                            return false;
                        }

                        SpellEntry const *triggerSpell = sSpellStore.LookupEntry(trigger_spell_id);
                        int32 healAmount = triggerSpell->EffectBasePoints[0] + 1;

                        if (Unit* auraCaster = triggeredByAura->GetCaster())
                        {
                            // Look through the caster's dummy auras for auras that affect Judgement of Light.
                            Unit::AuraList const& casterDummyAuras = auraCaster->GetAurasByType(SPELL_AURA_DUMMY);
                            for (Unit::AuraList::const_iterator i = casterDummyAuras.begin(); i != casterDummyAuras.end(); ++i)
                            {
                                // Justicar 2-piece bonus (Holy)
                                if ((*i)->GetSpellProto()->Id == 37182)
                                {
                                    healAmount += (*i)->GetModifier()->m_amount;
                                    break;
                                }
                            }
                        }

                        victim->CastCustomSpell(victim, trigger_spell_id, &healAmount, NULL, NULL, true, castItem, triggeredByAura);
                        return false;
                    }
                    // Illumination
                    if (auraSpellInfo->SpellIconID == 241)
                    {
                        if (!procSpell)
                            return false;
                        // procspell is triggered spell but we need mana cost of original casted spell
                        uint32 originalSpellId = procSpell->Id;
                        // Holy Shock heal
                        if (procSpell->SpellFamilyFlags & 0x1000000000000LL)
                        {
                            switch (procSpell->Id)
                            {
                            case 25914:
                                originalSpellId = 20473;
                                break;
                            case 25913:
                                originalSpellId = 20929;
                                break;
                            case 25903:
                                originalSpellId = 20930;
                                break;
                            case 27175:
                                originalSpellId = 27174;
                                break;
                            case 33074:
                                originalSpellId = 33072;
                                break;
                            default:
                                sLog.outError("Unit::HandleProcTriggerSpell: Spell %u not handled in HShock", procSpell->Id);
                                return false;
                            }
                        }
                        SpellEntry const* originalSpell = sSpellStore.LookupEntry(originalSpellId);
                        if (!originalSpell)
                        {
                            sLog.outError("Unit::HandleProcTriggerSpell: Spell %u unknown but selected as original in Illu", originalSpellId);
                            return false;
                        }
                        // percent stored in effect 1 (class scripts) base points
                        int32 manaRestore = originalSpell->manaCost * (auraSpellInfo->EffectBasePoints[1] + 1) / 100;
                        // Reduce mana restore when having Divine Illumination
                        basepoints0 = HasAura(31842, 0) ? (manaRestore / 2) : manaRestore;

                        trigger_spell_id = 20272;
                        target = this;
                    }
                    break;
                }
                break;
            }
        case SPELLFAMILY_SHAMAN:
            {
                switch (auraSpellInfo->Id)
                {
                // Lightning Shield (The Ten Storms set)
                case 23551:
                    {
                        trigger_spell_id = 23552;
                        target = victim;
                        break;
                    }
                // Damage from Lightning Shield (The Ten Storms set)
                case 23552:
                    {
                        trigger_spell_id = 27635;
                        break;
                    }
                // Mana Surge (The Earthfury set)
                case 23572:
                    {
                        if (!procSpell)
                            return false;
                        basepoints0 = procSpell->manaCost * 35 / 100;
                        trigger_spell_id = 23571;
                        target = this;
                        break;
                    }
                default:
                    {
                        // Lightning Shield (overwrite non existing triggered spell call in spell.dbc
                        if (auraSpellInfo->SpellFamilyFlags == 0x00000400 && auraSpellInfo->SpellVisual == 37)
                        {
                            switch (auraSpellInfo->Id)
                            {
                            case   324:
                                trigger_spell_id = 26364;
                                break;  // Rank 1
                            case   325:
                                trigger_spell_id = 26365;
                                break;  // Rank 2
                            case   905:
                                trigger_spell_id = 26366;
                                break;  // Rank 3
                            case   945:
                                trigger_spell_id = 26367;
                                break;  // Rank 4
                            case  8134:
                                trigger_spell_id = 26369;
                                break;  // Rank 5
                            case 10431:
                                trigger_spell_id = 26370;
                                break;  // Rank 6
                            case 10432:
                                trigger_spell_id = 26363;
                                break;  // Rank 7
                            case 25469:
                                trigger_spell_id = 26371;
                                break;  // Rank 8
                            case 25472:
                                trigger_spell_id = 26372;
                                break;  // Rank 9
                            default:
                                sLog.outError("Unit::HandleProcTriggerSpell: Spell %u not handled in LShield", auraSpellInfo->Id);
                                return false;
                            }
                        }
                        // Nature's Guardian
                        else if (auraSpellInfo->SpellIconID == 2013)
                        {
                            // Check health condition - should drop to less 30% (damage deal after this!)
                            if (uint32((10 * (int32(GetHealth() - damage)))) >= 3 * GetMaxHealth())
                                return false;

                            if (victim && victim->IsAlive())
                                victim->getThreatManager().modifyThreatPercent(this, -10);

                            basepoints0 = triggerAmount * GetMaxHealth() / 100;
                            trigger_spell_id = 31616;
                            target = this;
                        }
                    }
                }
                break;
            }
        default:
            break;
        }
    }

    // All ok. Check current trigger spell
    SpellEntry const* triggerEntry = sSpellStore.LookupEntry(trigger_spell_id);
    if (triggerEntry == NULL)
    {
        // Not cast unknown spell
        // sLog.outError("Unit::HandleProcTriggerSpell: Spell %u has 0 in EffectTriggered[%d], unhandled custom case?",auraSpellInfo->Id,triggeredByAura->GetEffIndex());
        return false;
    }

    // check if triggering spell can stack with current target's auras (if not - don't proc)
    // don't check if
    // aura is passive (talent's aura)
    // trigger_spell_id's aura is already active (allow to refresh triggered auras)
    // trigger_spell_id's triggeredByAura is already active (for example shaman's shields)
    AuraMap::iterator i, next;
    uint32 aura_id = 0;
    for (i = m_Auras.begin(); i != m_Auras.end(); i = next)
    {
        next = i;
        ++next;
        if (!(*i).second) continue;
        aura_id = (*i).second->GetSpellProto()->Id;
        if (IsPassiveSpell(aura_id) || aura_id == trigger_spell_id || aura_id == triggeredByAura->GetSpellProto()->Id) continue;
        if (sSpellMgr.IsNoStackSpellDueToSpell(trigger_spell_id, (*i).second->GetSpellProto()->Id, ((*i).second->GetCasterGUID() == GetGUID())))
            return false;
    }

    // Custom requirements (not listed in procEx) Warning! damage dealing after this
    // Custom triggered spells
    switch (auraSpellInfo->Id)
    {
     // Primal Fury - Druid Talent
    case 16961:
    {
        if (GetTypeId() != TYPEID_PLAYER) // Check see if unit is player
            return false;

        if (!procSpell) 
            return false;

        if (!(PROC_EX_CRITICAL_HIT)) // if we are not critical hit return
            return false;

        switch (m_form)
        {
        case FORM_CAT:
            
            if (!procSpell->HasEffect(SPELL_EFFECT_ADD_COMBO_POINTS)) // if the current spell does not add combo points return.
                return false;

            trigger_spell_id = 16953;
            target = victim;
            break;
        case FORM_BEAR:
            trigger_spell_id = 16959;
            target = this;
            break;
        } 
        break;
    }
    // Persistent Shield (Scarab Brooch trinket)
    // This spell originally trigger 13567 - Dummy Trigger (vs dummy efect)
    case 26467:
        {
            basepoints0 = damage * 15 / 100;
            target = victim;
            trigger_spell_id = 26470;
            break;
        }
    // Unyielding Knights (item exploit 29108\29109)
    case 38164:
        {
            if (!victim || victim->GetEntry() != 19457)  // Proc only if your target is Grillok
                return false;
            break;
        }
    // Cheat Death
    case 28845:
        {
            // When your health drops below 20%
            if (GetHealth() - damage > GetMaxHealth() / 5 || GetHealth() < GetMaxHealth() / 5)
                return false;
            break;
        }
    // Deadly Swiftness (Rank 1)
    case 31255:
        {
            // whenever you deal damage to a target who is below 20% health.
            if (!victim || !victim->IsAlive() || victim->HealthAbovePct(20))
                return false;

            target = this;
            trigger_spell_id = 22588;
        }
    // Greater Heal Refund (Avatar Raiment set)
    case 37594:
        {
            if (!victim || !victim->IsAlive())
                return false;

            // Not give if target already have full health
            if (victim->IsFullHealth())
                return false;
            // If your Greater Heal brings the target to full health, you gain $37595s1 mana.
            if (victim->GetHealth() + damage < victim->GetMaxHealth())
                return false;
            break;
        }
    // Bonus Healing (Crystal Spire of Karabor mace)
    case 40971:
        {
            // If your target is below $s1% health
            if (!victim || !victim->IsAlive() || victim->HealthAbovePct(triggerAmount))
                return false;
            break;
        }
    // Evasive Maneuvers (Commendation of Kael`thas trinket)
    case 45057:
        {
            // reduce you below $s1% health
            if (!HealthBelowPctDamaged(triggerAmount, damage))
                return false;
            break;
        }
        // Molten Shields
        case 30482:
        {
            if (procFlags & (PROC_FLAG_TAKEN_RANGED_AUTO_ATTACK | PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_NEG))
            {
                float chance = HasSpell(11094) ? 50.0f : HasSpell(13043) ? 100.0f : 0.0f;
                if (!roll_chance_f(chance))
                    return false;
            }
            else if (!(procFlags & (PROC_FLAG_TAKEN_MELEE_AUTO_ATTACK | PROC_FLAG_TAKEN_SPELL_MELEE_DMG_CLASS)))
                return false;
        }
    }

    // Costum basepoints/target for exist spell
    // dummy basepoints or other customs
    switch (trigger_spell_id)
    {
    // Cast positive spell on enemy target
    case 7099:  // Curse of Mending
    case 39647: // Curse of Mending
    case 29494: // Temptation
    case 20233: // Improved Lay on Hands (cast on target)
        {
            target = victim;
            break;
        }
    // Combo points add triggers (need add combopoint only for main target, and after possible combopoints reset)
    case 15250: // Rogue Setup
        {
            if (!victim || (ToPlayer() && victim != ToPlayer()->GetSelectedUnit())) // applied only for main target
                return false;
            break;                                   // continue normal case
        }
    // Finish movies that add combo
    case 14189: // Seal Fate (Netherblade set)
    case 14157: // Ruthlessness
        {
            if (!victim || victim == this)
                return false;

            if (procSpell->Id == 26679)
                return false;

            // Need add combopoint AFTER finish movie (or they dropped in finish phase)
            break;
        }
    // Shamanistic Rage triggered spell
    case 30824:
        {
            basepoints0 = int32(GetTotalAttackPowerValue(BASE_ATTACK) * triggerAmount / 100);
            trigger_spell_id = 30824;
            break;
        }
    // Hunter: Expose Weakness
    case 34501:
        {
            basepoints0 = int32(GetStat(STAT_AGILITY) * 0.25);
            int32 basepoints1 = int32(GetStat(STAT_AGILITY) * 0.25);

            CastCustomSpell(victim, trigger_spell_id, &basepoints0, &basepoints1, NULL, true, castItem, triggeredByAura);
            return true;
        }
    case 12536:         // Clearcasting (trigger only from mana cost spells)
    case 35095:         // Enlightenment (trigger only from mana cost spells)
        {
            if (!procSpell || procSpell->powerType != POWER_MANA || (procSpell->manaCost == 0 && procSpell->ManaCostPercentage == 0 && procSpell->manaCostPerlevel == 0))
                return false;
            break;
        }
    // Blackout
    case 15269:
        {
            // Should not proc on self
            if (!victim || victim == this)
                return false;

            // Should not proc from periodic ticks of Shadow Word: Pain or Mind Flay, only the initial cast. Should also proc from Shadowguard trigger.
            if (!procSpell || (procSpell->EffectApplyAuraName[0] == SPELL_AURA_PERIODIC_DAMAGE && procSpell->manaCost == 0 && procSpell->ManaCostPercentage == 0 && procSpell->manaCostPerlevel == 0))
                return false;

            // Should not proc from spells that don't deal damage.
            // Silence               // Mind Control
            if (procSpell->Id == 15487 || procSpell->Id == 605)
                return false;

            break;
        }
    }

    if (cooldown && GetTypeId() == TYPEID_PLAYER && ToPlayer()->HasSpellCooldown(trigger_spell_id))
        return false;

    // try detect target manually if not set
    if (target == NULL)
        target = !(procFlags & PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_POS) && IsPositiveSpell(trigger_spell_id) ? this : victim;

    // default case
    if (!target || (target != this && !target->IsAlive()))
        return false;

    // apply spell cooldown before casting to prevent triggering spells with SPELL_EFFECT_ADD_EXTRA_ATTACKS if spell has hidden cooldown
    if (cooldown && GetTypeId() == TYPEID_PLAYER)
        ToPlayer()->AddSpellCooldown(trigger_spell_id, 0, time(NULL) + cooldown);

    if (basepoints0)
        CastCustomSpell(target, trigger_spell_id, &basepoints0, NULL, NULL, true, castItem, triggeredByAura);
    else
        CastSpell(target, trigger_spell_id, true, castItem, triggeredByAura);

    return removeCharge;
}

bool Unit::HandleOverrideClassScriptAuraProc(Unit* victim, Aura* triggeredByAura, SpellEntry const* procSpell, uint32 cooldown)
{
    int32 scriptId = triggeredByAura->GetModifier()->m_miscvalue;

    if (!victim || !victim->IsAlive())
        return false;

    Item* castItem = triggeredByAura->GetCastItemGUID() && GetTypeId() == TYPEID_PLAYER
                     ? ToPlayer()->GetItemByGuid(triggeredByAura->GetCastItemGUID()) : NULL;

    uint32 triggered_spell_id = 0;
    CustomSpellValues values;

    switch (scriptId)
    {
    case 836:                                           // Improved Blizzard (Rank 1)
        {
            if (!procSpell || procSpell->SpellVisual != 9487)
                return false;
            triggered_spell_id = 12484;
            break;
        }
    case 988:                                           // Improved Blizzard (Rank 2)
        {
            if (!procSpell || procSpell->SpellVisual != 9487)
                return false;
            triggered_spell_id = 12485;
            break;
        }
    case 989:                                           // Improved Blizzard (Rank 3)
        {
            if (!procSpell || procSpell->SpellVisual != 9487)
                return false;
            triggered_spell_id = 12486;
            break;
        }
    case 4086:                                          // Improved Mend Pet (Rank 1)
    case 4087:                                          // Improved Mend Pet (Rank 2)
        {
            int32 chance = triggeredByAura->GetSpellProto()->EffectBasePoints[triggeredByAura->GetEffIndex()];
            if (!roll_chance_i(chance))
                return false;

            triggered_spell_id = 24406;
            break;
        }
    case 4533:                                          // Dreamwalker Raiment 2 pieces bonus
        {
            // Chance 50%
            if (!roll_chance_i(50))
                return false;

            switch (victim->getPowerType())
            {
            case POWER_MANA:
                triggered_spell_id = 28722;
                break;
            case POWER_RAGE:
                triggered_spell_id = 28723;
                break;
            case POWER_ENERGY:
                triggered_spell_id = 28724;
                break;
            default:
                return false;
            }
            break;
        }
    case 4537:                                          // Dreamwalker Raiment 6 pieces bonus
        triggered_spell_id = 28750;                     // Blessing of the Claw
        break;
    case 5497:                                          // Improved Mana Gems
        triggered_spell_id = 37445;                     // Mana Surge
        break;
    }

    // not processed
    if (!triggered_spell_id)
        return false;

    // standard non-dummy case
    SpellEntry const* triggerEntry = sSpellStore.LookupEntry(triggered_spell_id);

    if (!triggerEntry)
    {
        sLog.outError("Unit::HandleOverrideClassScriptAuraProc: Spell %u triggering for class script id %u", triggered_spell_id, scriptId);
        return false;
    }

    if (cooldown && GetTypeId() == TYPEID_PLAYER && ToPlayer()->HasSpellCooldown(triggered_spell_id))
        return false;

    CastCustomSpell(triggered_spell_id, values, victim, true, castItem, triggeredByAura);

    if (cooldown && GetTypeId() == TYPEID_PLAYER)
        ToPlayer()->AddSpellCooldown(triggered_spell_id, 0, time(NULL) + cooldown);

    return true;
}

void Unit::setPowerType(Powers new_powertype)
{
    SetByteValue(UNIT_FIELD_BYTES_0, 3, new_powertype);

    if (GetTypeId() == TYPEID_PLAYER)
    {
        if (ToPlayer()->GetGroup())
            ToPlayer()->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_POWER_TYPE);
    }
    else if (IsPet())
    {
        Pet* pet = ((Pet*)this);
        if (pet->isControlled())
        {
            Unit* owner = GetOwner();
            if (owner && (owner->GetTypeId() == TYPEID_PLAYER) && owner->ToPlayer()->GetGroup())
                owner->ToPlayer()->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_PET_POWER_TYPE);
        }
    }

    switch (new_powertype)
    {
    default:
    case POWER_MANA:
        break;
    case POWER_RAGE:
        SetMaxPower(POWER_RAGE, GetCreatePowers(POWER_RAGE));
        SetPower(POWER_RAGE, 0);
        break;
    case POWER_FOCUS:
        SetMaxPower(POWER_FOCUS, GetCreatePowers(POWER_FOCUS));
        SetPower(POWER_FOCUS, GetCreatePowers(POWER_FOCUS));
        break;
    case POWER_ENERGY:
        SetMaxPower(POWER_ENERGY, GetCreatePowers(POWER_ENERGY));
        SetPower(POWER_ENERGY, 0);
        break;
    case POWER_HAPPINESS:
        SetMaxPower(POWER_HAPPINESS, GetCreatePowers(POWER_HAPPINESS));
        SetPower(POWER_HAPPINESS, GetCreatePowers(POWER_HAPPINESS));
        break;
    }
}
FactionTemplateEntry const* Unit::GetFactionTemplateEntry() const
{
    FactionTemplateEntry const* entry = sFactionTemplateStore.LookupEntry(GetFaction());
    if (!entry)
    {
        if (Player const* player = ToPlayer())
            sLog.outError("Player %s has invalid faction (faction template id) #%u", player->GetName(), GetFaction());
        else if (Creature const* creature = ToCreature())
            sLog.outError("Creature (template id: %u) has invalid faction (faction template id) #%u", creature->GetCreatureTemplate()->Entry, GetFaction());
        else
            sLog.outError("Unit (name=%s, type=%u) has invalid faction (faction template id) #%u", GetName(), uint32(GetTypeId()), GetFaction());
    }
    return entry;
}

ReputationRank Unit::GetReactionTo(Unit const* target) const
{
    // always friendly to self
    if (this == target)
        return REP_FRIENDLY;

    // always friendly to charmer or owner
    if (GetCharmerOrOwnerOrSelf() == target->GetCharmerOrOwnerOrSelf())
        return REP_FRIENDLY;

    Player const* selfPlayerOwner = GetAffectingPlayer();
    Player const* targetPlayerOwner = target->GetAffectingPlayer();

    // check forced reputation to support SPELL_AURA_FORCE_REACTION
    if (selfPlayerOwner)
    {
        if (FactionTemplateEntry const* targetFactionTemplateEntry = target->GetFactionTemplateEntry())
            if (ReputationRank const* repRank = selfPlayerOwner->GetReputationMgr().GetForcedRankIfAny(targetFactionTemplateEntry))
                return *repRank;
    }
    else if (targetPlayerOwner)
    {
        if (FactionTemplateEntry const* selfFactionTemplateEntry = GetFactionTemplateEntry())
            if (ReputationRank const* repRank = targetPlayerOwner->GetReputationMgr().GetForcedRankIfAny(selfFactionTemplateEntry))
                return *repRank;
    }


    if (HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE))
    {
        if (target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE))
        {
            if (selfPlayerOwner && targetPlayerOwner)
            {
                // always friendly to other unit controlled by player, or to the player himself
                if (selfPlayerOwner == targetPlayerOwner)
                    return REP_FRIENDLY;

                // duel - always hostile to opponent
                if (selfPlayerOwner->duel && selfPlayerOwner->duel->opponent == targetPlayerOwner && selfPlayerOwner->duel->startTime != 0)
                    return REP_HOSTILE;

                // same group - checks dependant only on our faction - skip FFA_PVP for example
                if (selfPlayerOwner->IsInRaidWith(targetPlayerOwner))
                    return REP_FRIENDLY; // return true to allow config option AllowTwoSide.Interaction.Group to work
                // however client seems to allow mixed group parties, because in 13850 client it works like:
                // return GetFactionReactionTo(GetFactionTemplateEntry(), target);

                // PvP FFA state
                if (selfPlayerOwner->IsFFAPvP() && targetPlayerOwner->IsFFAPvP())
                    return REP_HOSTILE;
            }

            if (selfPlayerOwner)
            {
                if (FactionTemplateEntry const* targetFactionTemplateEntry = target->GetFactionTemplateEntry())
                {
                    if (ReputationRank const* repRank = selfPlayerOwner->GetReputationMgr().GetForcedRankIfAny(targetFactionTemplateEntry))
                        return *repRank;
                    if (!selfPlayerOwner->HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_IGNORE_REPUTATION))
                    {
                        if (FactionEntry const* targetFactionEntry = sFactionStore.LookupEntry(targetFactionTemplateEntry->faction))
                        {
                            if (targetFactionEntry->CanHaveReputation())
                            {
                                // check contested flags
                                if (targetFactionTemplateEntry->factionFlags & FACTION_TEMPLATE_FLAG_CONTESTED_GUARD
                                    && selfPlayerOwner->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_CONTESTED_PVP))
                                    return REP_HOSTILE;

                                // if faction has reputation, hostile state depends only from AtWar state
                                if (selfPlayerOwner->GetReputationMgr().IsAtWar(targetFactionEntry))
                                    return REP_HOSTILE;
                                return REP_FRIENDLY;
                            }
                        }
                    }
                }
            }
        }
    }
    // do checks dependant only on our faction
    return GetFactionReactionTo(GetFactionTemplateEntry(), target);
}

ReputationRank Unit::GetFactionReactionTo(FactionTemplateEntry const* factionTemplateEntry, Unit const* target)
{
    // always neutral when no template entry found
    if (!factionTemplateEntry)
        return REP_NEUTRAL;

    FactionTemplateEntry const* targetFactionTemplateEntry = target->GetFactionTemplateEntry();
    if (!targetFactionTemplateEntry)
        return REP_NEUTRAL;

    if (Player const* targetPlayerOwner = target->GetAffectingPlayer())
    {
        // check contested flags
        if (factionTemplateEntry->factionFlags & FACTION_TEMPLATE_FLAG_CONTESTED_GUARD
            && targetPlayerOwner->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_CONTESTED_PVP))
            return REP_HOSTILE;
        if (ReputationRank const* repRank = targetPlayerOwner->GetReputationMgr().GetForcedRankIfAny(factionTemplateEntry))
            return *repRank;
        if (!target->HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_IGNORE_REPUTATION))
        {
            if (FactionEntry const* factionEntry = sFactionStore.LookupEntry(factionTemplateEntry->faction))
            {
                if (factionEntry->CanHaveReputation())
                {
                    // CvP case - check reputation, don't allow state higher than neutral when at war
                    ReputationRank repRank = targetPlayerOwner->GetReputationMgr().GetRank(factionEntry);
                    if (targetPlayerOwner->GetReputationMgr().IsAtWar(factionEntry))
                        repRank = std::min(REP_NEUTRAL, repRank);
                    return repRank;
                }
            }
        }
    }

    // common faction based check
    if (factionTemplateEntry->IsHostileTo(*targetFactionTemplateEntry))
        return REP_HOSTILE;
    if (factionTemplateEntry->IsFriendlyTo(*targetFactionTemplateEntry))
        return REP_FRIENDLY;
    if (targetFactionTemplateEntry->IsFriendlyTo(*factionTemplateEntry))
        return REP_FRIENDLY;
    if (factionTemplateEntry->factionFlags & FACTION_TEMPLATE_FLAG_HOSTILE_BY_DEFAULT)
        return REP_HOSTILE;
    // neutral by default
    return REP_NEUTRAL;
}

bool Unit::IsHostileTo(Unit const* unit) const
{
    return GetReactionTo(unit) <= REP_HOSTILE;
}

bool Unit::IsFriendlyTo(Unit const* unit) const
{
    return GetReactionTo(unit) >= REP_FRIENDLY;
}

bool Unit::IsHostileToPlayers() const
{
    FactionTemplateEntry const* my_faction = GetFactionTemplateEntry();
    if (!my_faction || !my_faction->faction)
        return false;

    FactionEntry const* raw_faction = sFactionStore.LookupEntry(my_faction->faction);
    if (raw_faction && raw_faction->reputationListID >= 0)
        return false;

    return my_faction->IsHostileToPlayers();
}

bool Unit::IsNeutralToAll() const
{
    FactionTemplateEntry const* my_faction = GetFactionTemplateEntry();
    if (!my_faction || !my_faction->faction)
        return true;

    FactionEntry const* raw_faction = sFactionStore.LookupEntry(my_faction->faction);
    if (raw_faction && raw_faction->reputationListID >= 0)
        return false;

    return my_faction->IsNeutralToAll();
}

Unit* Unit::getAttackerForHelper() const                 // If someone wants to help, who to give them
{
    if (Unit* victim = GetVictim())
        if (!IsControlledByPlayer() || IsInCombatWith(victim) || victim->IsInCombatWith(this))
            return victim;

    if (!m_attackers.empty())
        return *(m_attackers.begin());

    if (Player* owner = GetCharmerOrOwnerPlayerOrPlayerItself())
    {
        HostileRefManager& refs = owner->getHostileRefManager();
        for (Reference<Unit, ThreatManager> const& ref : refs)
            if (Unit* hostile = ref.GetSource()->getOwner())
                return hostile;

        
    }

    return nullptr;
}

bool Unit::Attack(Unit* victim, bool meleeAttack)
{
    if (!victim || victim == this)
        return false;

    // dead units can neither attack nor be attacked
    if (!IsAlive() || !victim->IsInWorld() || !victim->IsAlive())
        return false;

    // player cannot attack in mount state
    if (GetTypeId() == TYPEID_PLAYER && IsMounted())
        return false;
    
    Creature* creature = ToCreature();
    // creatures cannot attack while evading
    if (creature && creature->IsInEvadeMode())
        return false;

    if (HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED))
        return false;

    // nobody can attack GM in GM-mode
    if (victim->GetTypeId() == TYPEID_PLAYER)
    {
        if (victim->ToPlayer()->IsGameMaster())
            return false;
    }
    else
    {
        if (victim->ToCreature()->IsInEvadeMode())
            return false;
    }

    // remove SPELL_AURA_MOD_UNATTACKABLE at attack (in case non-interruptible spells stun aura applied also that not let attack)
    if (HasAuraType(SPELL_AURA_MOD_UNATTACKABLE))
        RemoveSpellsCausingAura(SPELL_AURA_MOD_UNATTACKABLE);

    if (m_attacking)
    {
        if (m_attacking == victim)
        {
            // switch to melee attack from ranged/magic
            if (meleeAttack && !HasUnitState(UNIT_STATE_MELEE_ATTACKING))
            {
                AddUnitState(UNIT_STATE_MELEE_ATTACKING);
                SendMeleeAttackStart(victim);
                return true;
            }
            return false;
        }
        AttackStop();
    }

    // Set our target
    SetTarget(victim->GetGUID());

    if (meleeAttack)
        AddUnitState(UNIT_STATE_MELEE_ATTACKING);

    m_attacking = victim;
    m_attacking->_addAttacker(this);

    if (creature && !IsPet())
    {
        // should not let player enter combat by right clicking target
        SetInCombatWith(victim);
        if (victim->GetTypeId() == TYPEID_PLAYER)
            victim->SetInCombatWith(this);
        AddThreat(victim, 0.0f);

        creature->SendAIReaction(AI_REACTION_HOSTILE);
        creature->CallAssistance();
    }

    // delay offhand weapon attack to next attack time
    if (haveOffhandWeapon())
        resetAttackTimer(OFF_ATTACK);

    if (meleeAttack)
        SendMeleeAttackStart(victim);

    // Let the pet know we've started attacking someting. Handles melee attacks only
    // Spells such as auto-shot and others handled in WorldSession::HandleCastSpellOpcode
    if (GetTypeId() == TYPEID_PLAYER)
    {
        Pet* playerPet = this->ToPlayer()->GetPet();

        if (playerPet && playerPet->IsAlive())
            playerPet->AI()->OwnerAttacked(victim);
    }

    return true;
}

bool Unit::AttackStop()
{
    if (!m_attacking)
        return false;

    Unit* victim = m_attacking;

    m_attacking->_removeAttacker(this);
    m_attacking = NULL;

    // Clear our target
    SetTarget(0);

    ClearUnitState(UNIT_STATE_MELEE_ATTACKING);

    InterruptSpell(CURRENT_MELEE_SPELL);

    // reset only at real combat stop
    if (Creature* creature = ToCreature())
    {
        creature->SetNoCallAssistance(false);

        if (creature->HasSearchedAssistance())
        {
            creature->SetNoSearchAssistance(false);
            UpdateSpeed(MOVE_RUN, false);
        }
    }

    SendMeleeAttackStop(victim);

    return true;
}

void Unit::CombatStop(bool includingCast)
{
    if (includingCast && IsNonMeleeSpellCast(false))
        InterruptNonMeleeSpells(false);

    AttackStop();
    RemoveAllAttackers();
    m_HostileRefManager.deleteReferences();
    if (GetTypeId() == TYPEID_PLAYER)
        ToPlayer()->SendAttackSwingCancelAttack();     // melee and ranged forced attack cancel
    ClearInCombat();

    // just in case
    if (IsPetInCombat() && GetTypeId() != TYPEID_PLAYER)
        ClearInPetCombat();
}

void Unit::CombatStopWithPets(bool includingCast)
{
    CombatStop(includingCast);

    for (ControlList::const_iterator itr = m_Controlled.begin(); itr != m_Controlled.end(); ++itr)
        (*itr)->CombatStop(includingCast);
}

bool Unit::isAttackingPlayer() const
{
    if (HasUnitState(UNIT_STATE_ATTACK_PLAYER))
        return true;

    for (ControlList::const_iterator itr = m_Controlled.begin(); itr != m_Controlled.end(); ++itr)
        if ((*itr)->isAttackingPlayer())
            return true;

    for (uint8 i = 0; i < MAX_SUMMON_SLOT; ++i)
        if (m_SummonSlot[i])
            if (Creature* summon = GetMap()->GetCreature(m_SummonSlot[i]))
                if (summon->isAttackingPlayer())
                    return true;

    return false;
}

void Unit::RemoveAllAttackers()
{
    while (!m_attackers.empty())
    {
        AttackerSet::iterator iter = m_attackers.begin();
        if (!(*iter)->AttackStop())
        {
            sLog.outError("WORLD: Unit has an attacker that isn't attacking it!");
            m_attackers.erase(iter);
        }
    }
}

void Unit::ModifyAuraState(AuraState flag, bool apply)
{
    if (apply)
    {
        if (!HasFlag(UNIT_FIELD_AURASTATE, 1 << (flag - 1)))
        {
            SetFlag(UNIT_FIELD_AURASTATE, 1 << (flag - 1));
            if (GetTypeId() == TYPEID_PLAYER)
            {
                const PlayerSpellMap& sp_list = ToPlayer()->GetSpellMap();
                for (PlayerSpellMap::const_iterator itr = sp_list.begin(); itr != sp_list.end(); ++itr)
                {
                    if (itr->second.state == PLAYERSPELL_REMOVED)
                        continue;
                    SpellEntry const* spellInfo = sSpellStore.LookupEntry(itr->first);
                    if (!spellInfo || !IsPassiveSpell(itr->first))
                        continue;
                    if (spellInfo->CasterAuraState == flag)
                        CastSpell(this, itr->first, true, NULL);
                }
            }
        }
    }
    else
    {
        if (HasFlag(UNIT_FIELD_AURASTATE, 1 << (flag - 1)))
        {
            RemoveFlag(UNIT_FIELD_AURASTATE, 1 << (flag - 1));
            Unit::AuraMap& tAuras = GetAuras();
            for (Unit::AuraMap::iterator itr = tAuras.begin(); itr != tAuras.end();)
            {
                SpellEntry const* spellProto = (*itr).second->GetSpellProto();
                if (spellProto->CasterAuraState == flag)
                {
                    // exceptions (applied at state but not removed at state change)
                    // Rampage
                    if (spellProto->SpellIconID == 2006 && spellProto->SpellFamilyName == SPELLFAMILY_WARRIOR && spellProto->SpellFamilyFlags == 0x100000)
                    {
                        ++itr;
                        continue;
                    }

                    RemoveAura(itr);
                }
                else
                    ++itr;
            }
        }
    }
}

Unit* Unit::GetOwner() const
{
    if (uint64 ownerGUID = GetOwnerGUID())
        return ObjectAccessor::GetUnit(*this, ownerGUID);

    return NULL;
}

Unit* Unit::GetCharmer() const
{
    if (uint64 charmerGUID = GetCharmerGUID())
        return ObjectAccessor::GetUnit(*this, charmerGUID);

    return NULL;
}

Player* Unit::GetCharmerOrOwnerPlayerOrPlayerItself() const
{
    uint64 guid = GetCharmerOrOwnerGUID();
    if (IS_PLAYER_GUID(guid))
        return ObjectAccessor::GetPlayer(*this, guid);

    return GetTypeId() == TYPEID_PLAYER ? (Player*)this : NULL;
}

Player* Unit::GetAffectingPlayer() const
{
    if (!GetCharmerOrOwnerGUID())
        return const_cast<Unit*>(this)->ToPlayer();

    if (Unit* owner = GetCharmerOrOwner())
        return owner->GetCharmerOrOwnerPlayerOrPlayerItself();

    return NULL;
}

Minion* Unit::GetFirstMinion() const
{
    if (uint64 pet_guid = GetMinionGUID())
    {
        if (Creature* pet = ObjectAccessor::GetCreatureOrPet(*this, pet_guid))
            if (pet->HasUnitTypeMask(UNIT_MASK_MINION))
                return (Minion*)pet;

        sLog.outError("Unit::GetFirstMinion: Minion %u not exist.", GUID_LOPART(pet_guid));
        const_cast<Unit*>(this)->SetMinionGUID(0);
    }

    return NULL;
}

Guardian* Unit::GetGuardianPet() const
{
    if (uint64 pet_guid = GetPetGUID())
    {
        if (Creature* pet = ObjectAccessor::GetCreatureOrPet(*this, pet_guid))
            if (pet->HasUnitTypeMask(UNIT_MASK_GUARDIAN))
                return (Guardian*)pet;

        sLog.outError("Unit::GetGuardianPet: Guardian %u not exist.", GUID_LOPART(pet_guid));
        const_cast<Unit*>(this)->SetPetGUID(0);
    }

    return NULL;
}

Unit* Unit::GetCharm() const
{
    if (uint64 charm_guid = GetCharmGUID())
    {
        if (Unit* pet = ObjectAccessor::GetUnit(*this, charm_guid))
            return pet;

        sLog.outError("Unit::GetCharm: Charmed creature %u not exist.", GUID_LOPART(charm_guid));
        const_cast<Unit*>(this)->SetUInt64Value(UNIT_FIELD_CHARM, 0);
    }

    return NULL;
}

void Unit::SetMinion(Minion* minion, bool apply)
{
    sLog.outDebug("SetMinion %u for %u, apply %u", minion->GetEntry(), GetEntry(), apply);

    if (apply)
    {
        if (!minion->AddUInt64Value(UNIT_FIELD_SUMMONEDBY, GetGUID()))
        {
            sLog.outError("Crash alert! SetMinion: Minion %u is not the minion of owner %u", minion->GetEntry(), GetEntry());
            return;
        }

        m_Controlled.insert(minion);

        if (GetTypeId() == TYPEID_PLAYER)
        {
            minion->m_ControlledByPlayer = true;
            minion->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);
        }

        if (minion->m_Properties && minion->m_Properties->Type == SUMMON_TYPE_MINIPET)
            SetCritterGUID(minion->GetGUID());

        // Can only have one pet. If a new one is summoned, dismiss the old one.
        if (minion->IsPet() || (minion->m_Properties && minion->m_Properties->Category == SUMMON_CATEGORY_PET))
        {
            if (Guardian* oldPet = GetGuardianPet())
            {
                if (oldPet != minion && (oldPet->IsPet() || minion->IsPet() || oldPet->GetEntry() != minion->GetEntry()))
                {
                    // remove existing minion pet
                    if (oldPet->IsPet())
                        ((Pet*)oldPet)->Remove(PET_SAVE_AS_CURRENT);
                    else
                        oldPet->UnSummon();
                    SetPetGUID(minion->GetGUID());
                    SetMinionGUID(0);
                }
            }
            else
            {
                SetPetGUID(minion->GetGUID());
                SetMinionGUID(0);
            }
        }

        if (minion->HasUnitTypeMask(UNIT_MASK_CONTROLABLE_GUARDIAN))
            AddUInt64Value(UNIT_FIELD_SUMMON, minion->GetGUID());

        // PvP, FFAPvP
        minion->SetByteValue(UNIT_FIELD_BYTES_2, 1, GetByteValue(UNIT_FIELD_BYTES_2, 1));
    }
    else
    {
        if (minion->GetOwnerGUID() != GetGUID())
        {
            sLog.outError("Crash alert! SetMinion: Minion %u is not the minion of owner %u", minion->GetEntry(), GetEntry());
            return;
        }

        m_Controlled.erase(minion);

        if (minion->m_Properties && minion->m_Properties->Type == SUMMON_TYPE_MINIPET)
            if (GetCritterGUID() == minion->GetGUID())
                SetCritterGUID(0);

        if (minion->IsPet() || (minion->m_Properties && minion->m_Properties->Category == SUMMON_CATEGORY_PET))
        {
            if (GetPetGUID() == minion->GetGUID())
                SetPetGUID(0);
        }
        else if (minion->IsTotem())
        {
            // All summoned by totem minions must disappear when it is removed.
            if (const SpellEntry* spInfo = sSpellStore.LookupEntry(((Totem*)minion)->GetSpell()))
                for (int i = 0; i < MAX_SPELL_EFFECTS; ++i)
                {
                    if (spInfo->Effect[i] != SPELL_EFFECT_SUMMON)
                        continue;

                    this->RemoveAllMinionsByEntry(spInfo->EffectMiscValue[i]);
                }
        }

        if (RemoveUInt64Value(UNIT_FIELD_SUMMON, minion->GetGUID()))
        {
            //Check if there is another minion
            for (ControlList::iterator itr = m_Controlled.begin(); itr != m_Controlled.end(); ++itr)
            {
                // do not use this check, creature do not have charm guid
                //if (GetCharmGUID() == (*itr)->GetGUID())
                if (GetGUID() == (*itr)->GetCharmerGUID())
                    continue;

                ASSERT((*itr)->GetOwnerGUID() == GetGUID());
                ASSERT((*itr)->GetTypeId() == TYPEID_UNIT);

                if (!(*itr)->ToCreature()->HasUnitTypeMask(UNIT_MASK_CONTROLABLE_GUARDIAN))
                    continue;

                if (AddUInt64Value(UNIT_FIELD_SUMMON, (*itr)->GetGUID()))
                {
                    //show another pet bar if there is no charm bar
                    if (GetTypeId() == TYPEID_PLAYER && !GetCharmGUID())
                    {
                        if ((*itr)->IsPet())
                            ToPlayer()->PetSpellInitialize();
                        else
                            ToPlayer()->CharmSpellInitialize();
                    }
                }
                break;
            }
        }
    }
}

void Unit::RemoveAllMinionsByEntry(uint32 entry)
{
    for (Unit::ControlList::iterator itr = m_Controlled.begin(); itr != m_Controlled.end();)
    {
        Unit* unit = *itr;
        ++itr;
        if (unit->GetEntry() == entry && unit->GetTypeId() == TYPEID_UNIT
            && unit->IsSummon()) // minion, actually
            ((TempSummon*)unit)->UnSummon();
        // i think this is safe because i have never heard that a despawned minion will trigger a same minion
    }
}

void Unit::SetCharm(Unit* charm, bool apply)
{
    if (apply)
    {
        if (GetTypeId() == TYPEID_PLAYER)
        {
            if (!AddUInt64Value(UNIT_FIELD_CHARM, charm->GetGUID()))
                sLog.outError("Crash alert! Player %s is trying to charm unit %u, but it already has a charmed unit " UI64FMTD "", GetName(), charm->GetEntry(), GetCharmGUID());

            charm->m_ControlledByPlayer = true;
            // @todo maybe we can use this flag to check if controlled by player
            charm->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);
        }
        else
            charm->m_ControlledByPlayer = false;

        // PvP, FFAPvP
        charm->SetByteValue(UNIT_FIELD_BYTES_2, 1, GetByteValue(UNIT_FIELD_BYTES_2, 1));

        if (!charm->AddUInt64Value(UNIT_FIELD_CHARMEDBY, GetGUID()))
            sLog.outError("Crash alert! Unit %u is being charmed, but it already has a charmer " UI64FMTD "", charm->GetEntry(), charm->GetCharmerGUID());

        if (charm->HasUnitMovementFlag(MOVEMENTFLAG_WALK_MODE))
        {
            charm->RemoveUnitMovementFlag(MOVEMENTFLAG_WALK_MODE);
            charm->SendMovementFlagUpdate();
        }

        m_Controlled.insert(charm);
    }
    else
    {
        if (GetTypeId() == TYPEID_PLAYER)
        {
            if (!RemoveUInt64Value(UNIT_FIELD_CHARM, charm->GetGUID()))
                sLog.outError("Crash alert! Player %s is trying to uncharm unit %u, but it has another charmed unit " UI64FMTD "", GetName(), charm->GetEntry(), GetCharmGUID());
        }

        if (!charm->RemoveUInt64Value(UNIT_FIELD_CHARMEDBY, GetGUID()))
            sLog.outError("Crash alert! Unit %u is being uncharmed, but it has another charmer " UI64FMTD "", charm->GetEntry(), charm->GetCharmerGUID());

        if (charm->GetTypeId() == TYPEID_PLAYER)
        {
            charm->m_ControlledByPlayer = true;
            charm->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);
            charm->ToPlayer()->UpdatePvPState();
        }
        else if (Player* player = charm->GetCharmerOrOwnerPlayerOrPlayerItself())
        {
            charm->m_ControlledByPlayer = true;
            charm->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);
            charm->SetByteValue(UNIT_FIELD_BYTES_2, 1, player->GetByteValue(UNIT_FIELD_BYTES_2, 1));
        }
        else
        {
            charm->m_ControlledByPlayer = false;
            charm->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);
            charm->SetByteValue(UNIT_FIELD_BYTES_2, 1, 0);
        }

        if (charm->GetTypeId() == TYPEID_PLAYER
            || !charm->ToCreature()->HasUnitTypeMask(UNIT_MASK_MINION)
            || charm->GetOwnerGUID() != GetGUID())
            m_Controlled.erase(charm);
    }
}

Unit* Unit::GetFirstControlled() const
{
    Unit* unit = GetCharm();
    if (!unit)
    {
        if (uint64 guid = GetUInt64Value(UNIT_FIELD_SUMMON))
            unit = ObjectAccessor::GetUnit(*this, guid);
    }
    return unit;
}

void Unit::RemoveAllControlled()
{
    while (!m_Controlled.empty())
    {
        Unit* target = *m_Controlled.begin();
        m_Controlled.erase(m_Controlled.begin());
        if (target->GetCharmerGUID() == GetGUID())
            target->RemoveCharmAuras();
        else if (target->GetOwnerGUID() == GetGUID()
                 && target->GetTypeId() == TYPEID_UNIT
                 && ((Creature*)target)->HasUnitTypeMask(UNIT_MASK_SUMMON))
            ((TempSummon*)target)->UnSummon();
        else
            sLog.outError("Unit %u is trying to release unit %u which is neither charmed nor owned by it", GetEntry(), target->GetEntry());
    }
    if (GetPetGUID() != GetUInt64Value(UNIT_FIELD_SUMMON))
        sLog.outError("Crash alert! Unit %u is not able to release its summon " UI64FMTD "", GetEntry(), GetPetGUID());
    if (GetCharmGUID())
        sLog.outError("Crash alert! Unit %u is not able to release its charm " UI64FMTD "", GetEntry(), GetCharmGUID());
}

void Unit::AddPlayerToVision(Player* plr)
{
    if (m_sharedVision.empty())
    {
        setActive(true);
        SetWorldObject(true);
    }
    m_sharedVision.push_back(plr);
}

void Unit::RemovePlayerFromVision(Player* plr)
{
    m_sharedVision.remove(plr);
    if (m_sharedVision.empty())
    {
        setActive(false);
        SetWorldObject(false);
    }
}

void Unit::RemoveBindSightAuras()
{
    RemoveSpellsCausingAura(SPELL_AURA_BIND_SIGHT);
}

void Unit::RemoveCharmAuras()
{
    RemoveSpellsCausingAura(SPELL_AURA_MOD_CHARM);
    RemoveSpellsCausingAura(SPELL_AURA_MOD_POSSESS_PET);
    RemoveSpellsCausingAura(SPELL_AURA_MOD_POSSESS);
}

void Unit::UnsummonAllTotems()
{
    for (uint8 i = 0; i < MAX_SUMMON_SLOT; ++i)
    {
        if (!m_SummonSlot[i])
            continue;

        if (Creature* OldTotem = GetMap()->GetCreature(m_SummonSlot[i]))
            if (OldTotem->IsSummon())
                ((TempSummon*)OldTotem)->UnSummon();
    }
}

void Unit::SendHealSpellLog(Unit* victim, uint32 SpellID, uint32 Damage, bool critical)
{
    // we guess size
    WorldPacket data(SMSG_SPELLHEALLOG, (8 + 8 + 4 + 4 + 1));
    data << victim->GetPackGUID();
    data << GetPackGUID();
    data << uint32(SpellID);
    data << uint32(Damage);
    data << uint8(critical ? 1 : 0);
    data << uint8(0);                                       // unused in client?
    SendMessageToSet(&data, true);
}

void Unit::SendEnergizeSpellLog(Unit* victim, uint32 SpellID, uint32 Damage, Powers powertype)
{
    WorldPacket data(SMSG_SPELLENERGIZELOG, (8 + 8 + 4 + 4 + 4 + 1));
    data << victim->GetPackGUID();
    data << GetPackGUID();
    data << uint32(SpellID);
    data << uint32(powertype);
    data << uint32(Damage);
    SendMessageToSet(&data, true);
}

void Unit::EnergizeBySpell(Unit* victim, uint32 SpellID, uint32 damage, Powers powertype)
{
    SendEnergizeSpellLog(victim, SpellID, damage, powertype);
    // needs to be called after sending spell log
    victim->ModifyPower(powertype, damage);

    SpellEntry const* spellInfo = sSpellStore.LookupEntry(SpellID);
    victim->getHostileRefManager().threatAssist(this, float(damage) * 0.5f, spellInfo);
}

uint32 Unit::SpellDamageBonus(Unit* victim, SpellEntry const* spellProto, uint32 pdamage, DamageEffectType damagetype)
{
    if (!spellProto || !victim || damagetype == DIRECT_DAMAGE || spellProto->AttributesEx6 & SPELL_ATTR6_NO_DMG_PERCENT_MODS)
        return pdamage;

    int32 BonusDamage = 0;
    if (GetTypeId() == TYPEID_UNIT)
    {
        // Pets just add their bonus damage to their spell damage
        // note that their spell damage is just gain of their own auras
        if (ToCreature()->HasUnitTypeMask(UNIT_MASK_GUARDIAN))
        {
            if (spellProto->DmgClass == SPELL_DAMAGE_CLASS_MAGIC)
                BonusDamage = ((Guardian*)this)->IsHunterPet() ? ((Guardian*)this)->GetBonusDamage() * 0.33 : ((Guardian*)this)->GetBonusDamage();
            else if (spellProto->DmgClass == SPELL_DAMAGE_CLASS_MELEE && ((Guardian*)this)->IsHunterPet())
                BonusDamage = ((Guardian*)this)->GetTotalAttackPowerValue(BASE_ATTACK) * 0.07;
        }
        // For totems get damage bonus from owner (statue isn't totem in fact)
        else if (IsTotem())
        {
            if (Unit* owner = GetOwner())
                return owner->SpellDamageBonus(victim, spellProto, pdamage, damagetype);
        }
    }

    // Damage Done
    uint32 CastingTime = !IsChanneledSpell(spellProto) ? GetSpellCastTime(spellProto) : GetSpellDuration(spellProto);

    // Taken/Done fixed damage bonus auras
    int32 DoneAdvertisedBenefit  = SpellBaseDamageBonus(GetSpellSchoolMask(spellProto)) + BonusDamage;
    int32 TakenAdvertisedBenefit = SpellBaseDamageBonusForVictim(GetSpellSchoolMask(spellProto), victim);

    // Damage over Time spells bonus calculation
    float DotFactor = 1.0f;
    if (damagetype == DOT)
    {
        int32 DotDuration = GetSpellDuration(spellProto);
        // 200% limit
        if (DotDuration > 0)
        {
            if (DotDuration > 30000) DotDuration = 30000;
            if (!IsChanneledSpell(spellProto)) DotFactor = DotDuration / 15000.0f;
            int x = 0;
            for (int j = 0; j < MAX_SPELL_EFFECTS; j++)
            {
                if (spellProto->Effect[j] == SPELL_EFFECT_APPLY_AURA && (
                        spellProto->EffectApplyAuraName[j] == SPELL_AURA_PERIODIC_DAMAGE ||
                        spellProto->EffectApplyAuraName[j] == SPELL_AURA_PERIODIC_LEECH))
                {
                    x = j;
                    break;
                }
            }
            int DotTicks = 6;
            if (spellProto->EffectAmplitude[x] != 0)
                DotTicks = DotDuration / spellProto->EffectAmplitude[x];
            if (DotTicks)
            {
                DoneAdvertisedBenefit /= DotTicks;
                TakenAdvertisedBenefit /= DotTicks;
            }
        }
    }

    // Taken/Done total percent damage auras
    float DoneTotalMod = 1.0f;
    float TakenTotalMod = 1.0f;

    // ..done
    AuraList const& mModDamagePercentDone = GetAurasByType(SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
    for (AuraList::const_iterator i = mModDamagePercentDone.begin(); i != mModDamagePercentDone.end(); ++i)
    {
        if (spellProto->EquippedItemClass == -1 && (*i)->GetSpellProto()->EquippedItemClass != -1 && (*i)->GetMiscValue() == SPELL_SCHOOL_MASK_NORMAL)    //prevent apply mods from weapon specific case to non weapon specific spells (Example: thunder clap and two-handed weapon specialization)
            continue;

        if (((*i)->GetModifier()->m_miscvalue & GetSpellSchoolMask(spellProto)) &&
            (*i)->GetSpellProto()->EquippedItemClass == -1 &&
            // -1 == any item class (not wand then)
            (*i)->GetSpellProto()->EquippedItemInventoryTypeMask == 0 )
            // 0 == any inventory type (not wand then)
            DoneTotalMod *= ((*i)->GetModifierValue() + 100.0f) / 100.0f;
    }

    uint32 creatureTypeMask = victim->GetCreatureTypeMask();
    AuraList const& mDamageDoneVersus = GetAurasByType(SPELL_AURA_MOD_DAMAGE_DONE_VERSUS);
    for (AuraList::const_iterator i = mDamageDoneVersus.begin(); i != mDamageDoneVersus.end(); ++i)
        if (creatureTypeMask & uint32((*i)->GetModifier()->m_miscvalue))
            DoneTotalMod *= ((*i)->GetModifierValue() + 100.0f) / 100.0f;

    // ..taken
    AuraList const& mModDamagePercentTaken = victim->GetAurasByType(SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN);
    for (AuraList::const_iterator i = mModDamagePercentTaken.begin(); i != mModDamagePercentTaken.end(); ++i)
        if ((*i)->GetModifier()->m_miscvalue & GetSpellSchoolMask(spellProto))
            TakenTotalMod *= ((*i)->GetModifierValue() + 100.0f) / 100.0f;

    // .. taken pct: scripted (increases damage of * against targets *)
    AuraList const& mOverrideClassScript = GetAurasByType(SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
    for (AuraList::const_iterator i = mOverrideClassScript.begin(); i != mOverrideClassScript.end(); ++i)
    {
        switch ((*i)->GetModifier()->m_miscvalue)
        {
            //Molten Fury
            case 4920:
            case 4919:
                if (victim->HasAuraState(AURA_STATE_HEALTHLESS_20_PERCENT))
                    TakenTotalMod *= (100.0f + (*i)->GetModifier()->m_amount) / 100.0f;
                break;
            // Increased Lightning Damage
            case 4554:
            case 5142:
            case 6008:
                if (spellProto->SpellFamilyName == SPELLFAMILY_SHAMAN && spellProto->SpellFamilyFlags & 0x03)
                    pdamage += (*i)->GetSpellProto()->EffectBasePoints[(*i)->GetEffIndex()] + 1;
                break;
        }
    }

    bool hasmangle = false;
    // .. taken pct: dummy auras
    AuraList const& mDummyAuras = victim->GetAurasByType(SPELL_AURA_DUMMY);
    for (AuraList::const_iterator i = mDummyAuras.begin(); i != mDummyAuras.end(); ++i)
    {
        switch ((*i)->GetSpellProto()->SpellIconID)
        {
            // Cheat Death
            case 2109:
                if ((*i)->GetMiscValue() & SPELL_SCHOOL_MASK_NORMAL)
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        continue;
                    // Patch 2.4.3: The resilience required to reach the 90% damage reduction cap
                    // is 22.5% critical strike damage reduction, or 444 resilience.
                    // To calculate for 90%, we multiply the 100% by 4 (22.5% * 4 = 90%)
                    float mod = -1.0f * ToPlayer()->GetMeleeCritDamageReduction(400);
                    AddPct(TakenTotalMod, std::max(mod, float((*i)->GetAmount())));
                }
                break;
            //This is changed in WLK, using aura 255
            //Mangle
            case 2312:
            case 44955:
                // don't apply mod twice
                if (hasmangle)
                    break;
                hasmangle = true;
                for (int j = 0; j < 3; j++)
                {
                    if (GetEffectMechanic(spellProto, j) == MECHANIC_BLEED)
                    {
                        TakenTotalMod *= (100.0f + (*i)->GetModifier()->m_amount) / 100.0f;
                        break;
                    }
                }
                break;
        }
    }

    // Distribute Damage over multiple effects, reduce by AoE
    CastingTime = GetCastingTimeForBonus(spellProto, damagetype, CastingTime);

    // 50% for damage and healing spells for leech spells from damage bonus and 0% from healing
    for (int j = 0; j < MAX_SPELL_EFFECTS; ++j)
    {
        if ((spellProto->Effect[j] == SPELL_EFFECT_HEALTH_LEECH) ||
            (spellProto->Effect[j] == SPELL_EFFECT_APPLY_AURA && spellProto->EffectApplyAuraName[j] == SPELL_AURA_PERIODIC_LEECH))
        {
            CastingTime /= 2;
            break;
        }
    }

    switch (spellProto->SpellFamilyName)
    {
        case SPELLFAMILY_GENERIC:
            // Siphon Essence - 0%
            if (spellProto->AttributesEx == 268435456 && spellProto->SpellIconID == 2027)
                CastingTime = 0;
            // Goblin Rocket Launcher - 0%
            else if (spellProto->SpellIconID == 184 && spellProto->Attributes == 4259840)
                CastingTime = 0;
            // Darkmoon Card: Vengeance - 0.1%
            else if (spellProto->SpellVisual == 9850 && spellProto->SpellIconID == 2230)
                CastingTime = 3500;
            // This will disable the spell power coefficient for other generic family spells
            else CastingTime = 0;

        case SPELLFAMILY_MAGE:
            // Ignite - do not modify, it is (8*Rank)% damage of procing Spell
            if (spellProto->Id == 12654)
                return pdamage;
            // Ice Lance
            else if ((spellProto->SpellFamilyFlags & 0x20000LL) && spellProto->SpellIconID == 186)
            {
                CastingTime /= 3;                           // applied 1/3 bonuses in case generic target
                if (victim->isFrozen())                     // and compensate this for frozen target.
                    TakenTotalMod *= 3.0f;
            }
            // Pyroblast - 115% of Fire Damage, DoT - 20% of Fire Damage
            else if ((spellProto->SpellFamilyFlags & 0x400000LL) && spellProto->SpellIconID == 184)
            {
                DotFactor = damagetype == DOT ? 0.2f : 1.0f;
                CastingTime = damagetype == DOT ? 3500 : 4025;
            }
            // Fireball - 100% of Fire Damage, DoT - 0% of Fire Damage
            else if ((spellProto->SpellFamilyFlags & 0x1LL) && spellProto->SpellIconID == 185)
            {
                CastingTime = 3500;
                DotFactor = damagetype == DOT ? 0.0f : 1.0f;
            }
            // Molten armor
            else if (spellProto->SpellFamilyFlags & 0x0000000800000000LL)
                CastingTime = 0;
            // Arcane Missiles triggered spell
            else if ((spellProto->SpellFamilyFlags & 0x200000LL) && spellProto->SpellIconID == 225)
                CastingTime = 1000;
            // Blizzard triggered spell
            else if ((spellProto->SpellFamilyFlags & 0x80080LL) && spellProto->SpellIconID == 285)
                CastingTime = 500;
            break;
        case SPELLFAMILY_WARLOCK:
            // Life Tap
            if ((spellProto->SpellFamilyFlags & 0x40000LL) && spellProto->SpellIconID == 208)
            {
                CastingTime = 2800;                         // 80% from +shadow damage
                DoneTotalMod = 1.0f;
                TakenTotalMod = 1.0f;
            }
            // Dark Pact
            else if ((spellProto->SpellFamilyFlags & 0x80000000LL) && spellProto->SpellIconID == 154 && GetPetGUID())
            {
                CastingTime = 3360;                         // 96% from +shadow damage
                DoneTotalMod = 1.0f;
                TakenTotalMod = 1.0f;
            }
            // Soul Fire - 115% of Fire Damage
            else if ((spellProto->SpellFamilyFlags & 0x8000000000LL) && spellProto->SpellIconID == 184)
                CastingTime = 4025;
            // Curse of Agony - 120% of Shadow Damage
            else if ((spellProto->SpellFamilyFlags & 0x0000000400LL) && spellProto->SpellIconID == 544)
                DotFactor = 1.2f;
            // Drain Mana - 0% of Shadow Damage
            else if ((spellProto->SpellFamilyFlags & 0x10LL) && spellProto->SpellIconID == 548)
                CastingTime = 0;
            // Drain Soul 214.3%
            else if ((spellProto->SpellFamilyFlags & 0x4000LL) && spellProto->SpellIconID == 113)
                CastingTime = 7500;
            // Hellfire
            else if ((spellProto->SpellFamilyFlags & 0x40LL) && spellProto->SpellIconID == 937)
            {
                CastingTime = damagetype == DOT ? 5000 : 500; // self damage seems to be so
            }
            // Unstable Affliction - 180%
            else if (spellProto->Id == 31117 && spellProto->SpellIconID == 232)
                CastingTime = 6300;
            // Corruption 93.6%
            else if ((spellProto->SpellFamilyFlags & 0x2LL) && spellProto->SpellIconID == 313)
                DotFactor = 0.936f;
            break;
        case SPELLFAMILY_PALADIN:
            // Consecration - 95.24% of Holy Damage
            if ((spellProto->SpellFamilyFlags & 0x20LL) && spellProto->SpellIconID == 51)
            {
                DotFactor = 0.9524f;
                CastingTime = 3500;
            }
            // Seal of Righteousness - 10.2%/9.8% (based on weapon type) of Holy Damage, multiplied by weapon speed
            else if ((spellProto->SpellFamilyFlags & 0x8000000LL) && spellProto->SpellIconID == 25)
            {
                Item* item = ToPlayer()->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
                float wspeed = GetAttackTime(BASE_ATTACK) / 1000.0f;

                if (item && item->GetProto()->InventoryType == INVTYPE_2HWEAPON)
                    CastingTime = uint32(wspeed * 3500 * 0.102f);
                else
                    CastingTime = uint32(wspeed * 3500 * 0.098f);
            }
            // Judgement of Righteousness - 71.43%
            else if ((spellProto->SpellFamilyFlags & 1024) && spellProto->SpellIconID == 25)
                CastingTime = 2500;
            // Seal of Vengeance - 17% per Fully Stacked Tick - 5 Applications
            else if ((spellProto->SpellFamilyFlags & 0x80000000000LL) && spellProto->SpellIconID == 2292)
            {
                DotFactor = 0.17f;
                CastingTime = 3500;
            }
            // Holy shield - 5% of Holy Damage
            else if ((spellProto->SpellFamilyFlags & 0x4000000000LL) && spellProto->SpellIconID == 453)
                CastingTime = 175;
            // Blessing of Sanctuary - 0%
            else if ((spellProto->SpellFamilyFlags & 0x10000000LL) && spellProto->SpellIconID == 29)
                CastingTime = 0;
            // Seal of Righteousness trigger - already computed for parent spell
            else if (spellProto->SpellFamilyName == SPELLFAMILY_PALADIN && spellProto->SpellIconID == 25 && spellProto->AttributesEx4 & 0x00800000LL)
                return pdamage;
            break;
        case  SPELLFAMILY_SHAMAN:
            // totem attack
            if (spellProto->SpellFamilyFlags & 0x000040000000LL)
            {
                if (spellProto->SpellIconID == 33)          // Fire Nova totem attack must be 21.4%(untested)
                    CastingTime = 749;                      // ignore CastingTime and use as modifier
                else if (spellProto->SpellIconID == 680)    // Searing Totem attack 8%
                    CastingTime = 280;                      // ignore CastingTime and use as modifier
                else if (spellProto->SpellIconID == 37)     // Magma totem attack must be 6.67%(untested)
                    CastingTime = 234;                      // ignore CastingTimePenalty and use as modifier
            }
            // Lightning Shield (and proc shield from T2 8 pieces bonus) 33% per charge
            else if ((spellProto->SpellFamilyFlags & 0x00000000400LL) || spellProto->Id == 23552)
                CastingTime = 1155;                         // ignore CastingTimePenalty and use as modifier
            break;
        case SPELLFAMILY_PRIEST:
            // Mana Burn - 0% of Shadow Damage
            if ((spellProto->SpellFamilyFlags & 0x10LL) && spellProto->SpellIconID == 212)
                CastingTime = 0;
            // Mind Flay - 57.1% of Shadow Damage
            else if ((spellProto->SpellFamilyFlags & 0x800000LL) && spellProto->SpellIconID == 548)
                CastingTime = 2000;
            // Holy Fire - 85.71%, DoT - 16.5%
            else if ((spellProto->SpellFamilyFlags & 0x100000LL) && spellProto->SpellIconID == 156)
            {
                DotFactor = damagetype == DOT ? 0.165f : 1.0f;
                CastingTime = damagetype == DOT ? 3500 : 3000;
            }
            // Shadowguard - 28% per charge
            else if ((spellProto->SpellFamilyFlags & 0x2000000LL) && spellProto->SpellIconID == 19)
                CastingTime = 980;
            // Touch of Weakeness - 10%
            else if ((spellProto->SpellFamilyFlags & 0x80000LL) && spellProto->SpellIconID == 1591)
                CastingTime = 350;
            // Reflective Shield (back damage) - 0% (other spells fit to check not have damage effects/auras)
            else if (spellProto->SpellFamilyFlags == 0 && spellProto->SpellIconID == 566)
                CastingTime = 0;
            // Holy Nova - 16%
            else if ((spellProto->SpellFamilyFlags & 0x400000LL) && spellProto->SpellIconID == 1874)
                CastingTime = 560;
            break;
        case SPELLFAMILY_DRUID:
            // Hurricane triggered spell
            if ((spellProto->SpellFamilyFlags & 0x400000LL) && spellProto->SpellIconID == 220)
                CastingTime = 500;
            break;
        case SPELLFAMILY_WARRIOR:
        case SPELLFAMILY_HUNTER:
        case SPELLFAMILY_ROGUE:
            CastingTime = 0;
            break;
        default:
            break;
    }

    float LvlPenalty = CalculateLevelPenalty(spellProto);

    // Spellmod SpellDamage
    //float SpellModSpellDamage = 100.0f;
    float CoefficientPtc = DotFactor * 100.0f;
    if (spellProto->SchoolMask != SPELL_SCHOOL_MASK_NORMAL)
        CoefficientPtc *= ((float)CastingTime / 3500.0f);

    if (Player* modOwner = GetSpellModOwner())
        //modOwner->ApplySpellMod(spellProto->Id,SPELLMOD_SPELL_BONUS_DAMAGE,SpellModSpellDamage);
        modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_SPELL_BONUS_DAMAGE, CoefficientPtc);

    //SpellModSpellDamage /= 100.0f;
    CoefficientPtc /= 100.0f;

    //float DoneActualBenefit = DoneAdvertisedBenefit * (CastingTime / 3500.0f) * DotFactor * SpellModSpellDamage * LvlPenalty;

    float DoneActualBenefit = DoneAdvertisedBenefit * CoefficientPtc * LvlPenalty;
    float TakenActualBenefit = TakenAdvertisedBenefit * DotFactor * LvlPenalty;
    if (spellProto->SpellFamilyName && spellProto->SchoolMask != SPELL_SCHOOL_MASK_NORMAL)
        TakenActualBenefit *= ((float)CastingTime / 3500.0f);

    float tmpDamage = (float(pdamage) + DoneActualBenefit) * DoneTotalMod;

    // Add flat bonus from spell damage versus
    tmpDamage += GetTotalAuraModifierByMiscMask(SPELL_AURA_MOD_FLAT_SPELL_DAMAGE_VERSUS, creatureTypeMask);

    // apply spellmod to Done damage
    if (Player* modOwner = GetSpellModOwner())
        modOwner->ApplySpellMod(spellProto->Id, damagetype == DOT ? SPELLMOD_DOT : SPELLMOD_DAMAGE, tmpDamage);

    tmpDamage = (tmpDamage + TakenActualBenefit) * TakenTotalMod;

    if (GetTypeId() == TYPEID_UNIT && !IsPet())
        tmpDamage *= ToCreature()->GetSpellDamageMod(ToCreature()->GetCreatureTemplate()->rank);

    return tmpDamage > 0 ? uint32(tmpDamage) : 0;
}

int32 Unit::SpellBaseDamageBonus(SpellSchoolMask schoolMask)
{
    int32 DoneAdvertisedBenefit = 0;

    // ..done
    AuraList const& mDamageDone = GetAurasByType(SPELL_AURA_MOD_DAMAGE_DONE);
    for (AuraList::const_iterator i = mDamageDone.begin(); i != mDamageDone.end(); ++i)
        if (((*i)->GetModifier()->m_miscvalue & schoolMask) != 0 &&
            (*i)->GetSpellProto()->EquippedItemClass == -1 &&
            // -1 == any item class (not wand then)
            (*i)->GetSpellProto()->EquippedItemInventoryTypeMask == 0)
            // 0 == any inventory type (not wand then)
            DoneAdvertisedBenefit += (*i)->GetModifierValue();

    if (GetTypeId() == TYPEID_PLAYER)
    {
        // Damage bonus from stats
        AuraList const& mDamageDoneOfStatPercent = GetAurasByType(SPELL_AURA_MOD_SPELL_DAMAGE_OF_STAT_PERCENT);
        for (AuraList::const_iterator i = mDamageDoneOfStatPercent.begin(); i != mDamageDoneOfStatPercent.end(); ++i)
        {
            if ((*i)->GetModifier()->m_miscvalue & schoolMask)
            {
                SpellEntry const* iSpellProto = (*i)->GetSpellProto();
                uint8 eff = (*i)->GetEffIndex();

                // stat used dependent from next effect aura SPELL_AURA_MOD_SPELL_HEALING presence and misc value (stat index)
                Stats usedStat = STAT_INTELLECT;
                if (eff < 2 && iSpellProto->EffectApplyAuraName[eff + 1] == SPELL_AURA_MOD_SPELL_HEALING_OF_STAT_PERCENT)
                    usedStat = Stats(iSpellProto->EffectMiscValue[eff + 1]);

                DoneAdvertisedBenefit += int32(GetStat(usedStat) * (*i)->GetModifierValue() / 100.0f);
            }
        }
        // ... and attack power
        AuraList const& mDamageDonebyAP = GetAurasByType(SPELL_AURA_MOD_SPELL_DAMAGE_OF_ATTACK_POWER);
        for (AuraList::const_iterator i = mDamageDonebyAP.begin(); i != mDamageDonebyAP.end(); ++i)
            if ((*i)->GetModifier()->m_miscvalue & schoolMask)
                DoneAdvertisedBenefit += int32(GetTotalAttackPowerValue(BASE_ATTACK) * (*i)->GetModifierValue() / 100.0f);

    }
    return DoneAdvertisedBenefit;
}

int32 Unit::SpellBaseDamageBonusForVictim(SpellSchoolMask schoolMask, Unit* victim)
{
    uint32 creatureTypeMask = victim->GetCreatureTypeMask();

    int32 TakenAdvertisedBenefit = 0;
    // ..done (for creature type by mask) in taken
    AuraList const& mDamageDoneCreature = GetAurasByType(SPELL_AURA_MOD_DAMAGE_DONE_CREATURE);
    for (AuraList::const_iterator i = mDamageDoneCreature.begin(); i != mDamageDoneCreature.end(); ++i)
        if (creatureTypeMask & uint32((*i)->GetModifier()->m_miscvalue))
            TakenAdvertisedBenefit += (*i)->GetModifierValue();

    // ..taken
    AuraList const& mDamageTaken = victim->GetAurasByType(SPELL_AURA_MOD_DAMAGE_TAKEN);
    for (AuraList::const_iterator i = mDamageTaken.begin(); i != mDamageTaken.end(); ++i)
        if (((*i)->GetModifier()->m_miscvalue & schoolMask) != 0)
            TakenAdvertisedBenefit += (*i)->GetModifierValue();

    return TakenAdvertisedBenefit;
}

bool Unit::isSpellCrit(Unit* victim, SpellEntry const* spellProto, SpellSchoolMask schoolMask, WeaponAttackType attackType)
{
    // Mobs can't crit with spells
    if (IS_CREATURE_GUID(GetGUID()))
        return false;

    // not critting spell
    if (!IsSpellAbleToCrit(spellProto))
        return false;

    // Creatures do not crit with their spells or abilities, unless it is owned by a player (pet, totem, etc)
    if (GetTypeId() != TYPEID_PLAYER)
    {
        Unit* owner = GetOwner();
        if (!owner || owner->GetTypeId() != TYPEID_PLAYER)
            return false;
    }

    float crit_chance = 0.0f;
    switch (spellProto->DmgClass)
    {
        case SPELL_DAMAGE_CLASS_NONE:
            // We need more spells to find a general way (if there is any)
            switch (spellProto->Id)
            {
                case 379:   // Earth Shield
                case 33778: // Lifebloom Final Bloom
                case 45064: // Item - Vial of the Sunwell
                    break;
                default:
                    return false;
            }
        // Do not add a break here, case fallthrough is intentional! Adding a break will make above spells unable to crit.
        case SPELL_DAMAGE_CLASS_MAGIC:
            {
                if (schoolMask & SPELL_SCHOOL_MASK_NORMAL)
                    crit_chance = (float)m_baseSpellCritChance;
                // For other schools
                else if (GetTypeId() == TYPEID_PLAYER)
                    crit_chance = GetFloatValue(PLAYER_SPELL_CRIT_PERCENTAGE1 + GetFirstSchoolInMask(schoolMask));
                else
                {
                    crit_chance = (float)m_baseSpellCritChance;
                    crit_chance += GetTotalAuraModifierByMiscMask(SPELL_AURA_MOD_SPELL_CRIT_CHANCE_SCHOOL, schoolMask);
                }
                // taken
                if (victim && !IsPositiveSpell(spellProto->Id))
                {
                    // Modify critical chance by victim SPELL_AURA_MOD_ATTACKER_SPELL_CRIT_CHANCE
                    crit_chance += victim->GetTotalAuraModifierByMiscMask(SPELL_AURA_MOD_ATTACKER_SPELL_CRIT_CHANCE, schoolMask);
                    // Modify critical chance by victim SPELL_AURA_MOD_ATTACKER_SPELL_AND_WEAPON_CRIT_CHANCE
                    crit_chance += victim->GetTotalAuraModifier(SPELL_AURA_MOD_ATTACKER_SPELL_AND_WEAPON_CRIT_CHANCE);
                    // Modify by player victim resilience
                    if (victim->GetTypeId() == TYPEID_PLAYER)
                        crit_chance -= victim->ToPlayer()->GetRatingBonusValue(CR_CRIT_TAKEN_SPELL);
                    // scripted (increase crit chance ... against ... target by x%
                    if (victim->isFrozen()) // Shatter
                    {
                        AuraList const& mOverrideClassScript = GetAurasByType(SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
                        for (AuraList::const_iterator i = mOverrideClassScript.begin(); i != mOverrideClassScript.end(); ++i)
                        {
                            switch ((*i)->GetModifier()->m_miscvalue)
                            {
                            case 849:
                                crit_chance += 10.0f;
                                break; //Shatter Rank 1
                            case 910:
                                crit_chance += 20.0f;
                                break; //Shatter Rank 2
                            case 911:
                                crit_chance += 30.0f;
                                break; //Shatter Rank 3
                            case 912:
                                crit_chance += 40.0f;
                                break; //Shatter Rank 4
                            case 913:
                                crit_chance += 50.0f;
                                break; //Shatter Rank 5
                            }
                        }
                    }
                }
                break;
            }
        case SPELL_DAMAGE_CLASS_MELEE:
        case SPELL_DAMAGE_CLASS_RANGED:
            {
                if (victim)
                {
                    crit_chance = GetUnitCriticalChance(attackType, victim);
                    crit_chance += (int32(GetMaxSkillValueForLevel(victim)) - int32(victim->GetDefenseSkillValue(this))) * 0.04f;
                    crit_chance += GetTotalAuraModifierByMiscMask(SPELL_AURA_MOD_SPELL_CRIT_CHANCE_SCHOOL, schoolMask);

                    // Autoshot must crit if target is sitting down and crit chance not 0
                    if ((spellProto->AttributesEx4 & SPELL_ATTR4_AUTOSHOT) && victim->GetTypeId() == TYPEID_PLAYER && crit_chance > 0 && !victim->IsStandState())
                        return true;
                }
                break;
            }
        default:
            return false;
    }
    // percent done
    // only players use intelligence for critical chance computations
    if (Player* modOwner = GetSpellModOwner())
        modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_CRITICAL_CHANCE, crit_chance);

    crit_chance = crit_chance > 0.0f ? crit_chance : 0.0f;
    if (roll_chance_f(crit_chance))
        return true;
    return false;
}

uint32 Unit::SpellCriticalBonus(SpellEntry const* spellProto, uint32 damage, Unit* victim)
{
    // Calculate critical bonus
    int32 crit_bonus = damage;
    float crit_mod = 0.0f;

    switch (spellProto->DmgClass)
    {
        case SPELL_DAMAGE_CLASS_MELEE:                      // for melee based spells is 100%
        case SPELL_DAMAGE_CLASS_RANGED:
            // @todo write here full calculation for melee/ranged spells
            crit_bonus += damage;
            break;
        default:
            crit_bonus += damage / 2;                        // for spells is 50%
            break;
    }

    if (victim)
        crit_mod += GetTotalAuraMultiplierByMiscMask(SPELL_AURA_MOD_CRIT_PERCENT_VERSUS, victim->GetCreatureTypeMask());

    if (crit_bonus != 0)
        AddPct(crit_bonus, crit_mod);

    crit_bonus -= damage;

    if (damage > uint32(crit_bonus))
    {
        // adds additional damage to crit_bonus (from talents)
        if (Player* modOwner = GetSpellModOwner())
            modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_CRIT_DAMAGE_BONUS, crit_bonus);
    }

    crit_bonus += damage;

    return crit_bonus;
}

uint32 Unit::SpellHealingBonus(SpellEntry const* spellProto, uint32 healamount, DamageEffectType damagetype, Unit* victim)
{
    // For totems get healing bonus from owner (statue isn't totem in fact)
    if (GetTypeId() == TYPEID_UNIT && IsTotem())
        if (Unit* owner = GetOwner())
            return owner->SpellHealingBonus(spellProto, healamount, damagetype, victim);

    // No bonus healing for potion spells
    if (spellProto->SpellFamilyName == SPELLFAMILY_POTION)
        return healamount;

    // Healing Done
    float TotalHealPct = 1.0f;
    float minval = victim->GetMaxNegativeAuraModifier(SPELL_AURA_MOD_HEALING_PCT);
    if (minval)
        TotalHealPct *= (100.0f + minval) / 100.0f;

    float maxval = victim->GetMaxPositiveAuraModifier(SPELL_AURA_MOD_HEALING_PCT);
    if (maxval)
        TotalHealPct *= (100.0f + maxval) / 100.0f;

    const uint32 spellId = spellProto->Id;

    // These spells are triggers, and should trigger with their full amount of healing.
    // So, to prevent double dipping of reduced heals we need to remove these from effects of SPELL_AURA_MOD_HEALING_PCT.
    // @todo Find a less hacky method of handling these spells.
    if ((spellProto->Id == 33763 && damagetype != DOT) ||                                                        // Lifeblooms Final Tick
        (spellProto->SpellFamilyName == SPELLFAMILY_SHAMAN && spellProto->SpellFamilyFlags == 0x40000000000l) || // Earth Shield
        spellProto->Id == 41635)                                                                                 // Prayer of Mending
        TotalHealPct = 1.0f;

    // These Spells are doing fixed amount of healing (@todo find a less hack-like check)
    // Generic family spells are not handled here, these are exceptions to the rule
    if (spellId == 33778 || spellId == 379 || spellId == 38395 || spellId == 34299 ||  // Lifebloom (rank 1), Earth Shield, Siphon Essence, Flame Quills
        spellId == 27813 || spellId == 27817 || spellId == 27818 || spellId == 28880)   // Blessed Recovery (rank 1, rank 2 & rank 3), Gift of the Naaru
        return healamount * TotalHealPct;

    int32 AdvertisedBenefit = SpellBaseHealingBonus(GetSpellSchoolMask(spellProto));
    uint32 CastingTime = GetSpellCastTime(spellProto);

    // Healing Taken
    AdvertisedBenefit += SpellBaseHealingBonusForVictim(GetSpellSchoolMask(spellProto), victim);

    // Blessing of Light dummy effects healing taken from Holy Light and Flash of Light
    if (spellProto->SpellFamilyName == SPELLFAMILY_PALADIN && (spellProto->SpellFamilyFlags & 0x00000000C0000000LL))
    {
        AuraList const& mDummyAuras = victim->GetAurasByType(SPELL_AURA_DUMMY);
        for (AuraList::const_iterator i = mDummyAuras.begin(); i != mDummyAuras.end(); ++i)
        {
            if ((*i)->GetSpellProto()->SpellVisual == 9180) // Blessing of Light
            {
                // Flash of Light
                if ((spellProto->SpellFamilyFlags & 0x0000000040000000LL) && (*i)->GetEffIndex() == 1)
                    AdvertisedBenefit += (*i)->GetModifier()->m_amount;
                // Holy Light
                else if ((spellProto->SpellFamilyFlags & 0x0000000080000000LL) && (*i)->GetEffIndex() == 0)
                    AdvertisedBenefit += (*i)->GetModifier()->m_amount;
            }
            else if ((*i)->GetSpellProto()->Id == 38320) // Libram of Souls Redeemed
            {
                // Flash of Light
                if ((spellProto->SpellFamilyFlags & UI64LIT(0x0000000040000000)) && (*i)->GetEffIndex() == 0)
                    AdvertisedBenefit += (*i)->GetModifier()->m_amount / 2;
                // Holy Light
                else if ((spellProto->SpellFamilyFlags & UI64LIT(0x0000000080000000)) && (*i)->GetEffIndex() == 0)
                    AdvertisedBenefit += (*i)->GetModifier()->m_amount;
            }
            else if ((*i)->GetSpellProto()->Id == 34231) // Libram of the Lightbringer
            {
                // Holy Light
                if ((spellProto->SpellFamilyFlags & UI64LIT(0x0000000080000000)) && (*i)->GetEffIndex() == 0)
                    AdvertisedBenefit += (*i)->GetModifier()->m_amount;
            }
            else if ((*i)->GetSpellProto()->Id == 28851) // Libram of Light
            {
                // Flash of Light
                if ((spellProto->SpellFamilyFlags & UI64LIT(0x0000000040000000)) && (*i)->GetEffIndex() == 0)
                    AdvertisedBenefit += (*i)->GetModifier()->m_amount;
            }
            else if ((*i)->GetSpellProto()->Id == 28853) // Libram of Divinity
            {
                // Flash of Light
                if ((spellProto->SpellFamilyFlags & UI64LIT(0x0000000040000000)) && (*i)->GetEffIndex() == 0)
                    AdvertisedBenefit += (*i)->GetModifier()->m_amount;
            }
            else if ((*i)->GetSpellProto()->Id == 32403) // Blessed book of Nagrand
            {
                // Flash of Light
                if ((spellProto->SpellFamilyFlags & UI64LIT(0x0000000040000000)) && (*i)->GetEffIndex() == 0)
                    AdvertisedBenefit += (*i)->GetModifier()->m_amount;
            }
        }
    }

    float ActualBenefit = 0.0f;

    if (AdvertisedBenefit != 0)
    {
        // Healing over Time spells
        float DotFactor = 1.0f;
        if (damagetype == DOT)
        {
            int32 DotDuration = GetSpellDuration(spellProto);
            if (DotDuration > 0)
            {
                // 200% limit
                if (DotDuration > 30000) DotDuration = 30000;
                if (!IsChanneledSpell(spellProto)) DotFactor = DotDuration / 15000.0f;
                int x = 0;
                for (int j = 0; j < MAX_SPELL_EFFECTS; j++)
                {
                    if (spellProto->Effect[j] == SPELL_EFFECT_APPLY_AURA && (
                            spellProto->EffectApplyAuraName[j] == SPELL_AURA_PERIODIC_HEAL ||
                            spellProto->EffectApplyAuraName[j] == SPELL_AURA_PERIODIC_LEECH))
                    {
                        x = j;
                        break;
                    }
                }
                int DotTicks = 6;
                if (spellProto->EffectAmplitude[x] != 0)
                    DotTicks = DotDuration / spellProto->EffectAmplitude[x];
                if (DotTicks)
                    AdvertisedBenefit /= DotTicks;
            }
        }

        // distribute healing to all effects, reduce AoE damage
        CastingTime = GetCastingTimeForBonus(spellProto, damagetype, CastingTime);

        // 0% bonus for damage and healing spells for leech spells from healing bonus
        for (int j = 0; j < MAX_SPELL_EFFECTS; ++j)
        {
            if ((spellProto->Effect[j] == SPELL_EFFECT_HEALTH_LEECH) ||
                (spellProto->Effect[j] == SPELL_EFFECT_APPLY_AURA && spellProto->EffectApplyAuraName[j] == SPELL_AURA_PERIODIC_LEECH))
            {
                CastingTime = 0;
                break;
            }
        }

        // Exception
        switch (spellProto->SpellFamilyName)
        {
        case  SPELLFAMILY_SHAMAN:
            // Healing stream from totem (add 6% per tick from hill bonus owner)
            if (spellProto->SpellFamilyFlags & 0x000000002000LL)
                CastingTime = 210;
            // Earth Shield 30% per charge
            else if (spellProto->SpellFamilyFlags & 0x40000000000LL)
                CastingTime = 1050;
            break;
        case  SPELLFAMILY_DRUID:
            // Lifebloom
            if (spellProto->SpellFamilyFlags & 0x1000000000LL)
            {
                CastingTime = damagetype == DOT ? 3500 : 1200;
                DotFactor = damagetype == DOT ? 0.519f : 1.0f;
            }
            // Tranquility triggered spell
            else if (spellProto->SpellFamilyFlags & 0x80LL)
                CastingTime = 667;
            // Rejuvenation
            else if (spellProto->SpellFamilyFlags & 0x10LL)
                DotFactor = 0.845f;
            // Regrowth
            else if (spellProto->SpellFamilyFlags & 0x40LL)
            {
                DotFactor = damagetype == DOT ? 0.705f : 1.0f;
                CastingTime = damagetype == DOT ? 3500 : 1010;
            }
            // Improved Leader of the Pack
            else if (spellProto->AttributesEx2 == 536870912 && spellProto->SpellIconID == 312
                     && spellProto->AttributesEx3 == 33554432)
                CastingTime = 0;
            break;
        case SPELLFAMILY_PRIEST:
            // Holy Nova - 14%
            if ((spellProto->SpellFamilyFlags & 0x8000000LL) && spellProto->SpellIconID == 1874)
                CastingTime = 500;
            break;
        case SPELLFAMILY_PALADIN:
            // Seal and Judgement of Light
            if (spellProto->SpellFamilyFlags & 0x100040000LL)
                CastingTime = 0;
            break;
        case SPELLFAMILY_WARRIOR:
        case SPELLFAMILY_ROGUE:
        case SPELLFAMILY_HUNTER:
        case SPELLFAMILY_POTION:
        case SPELLFAMILY_GENERIC:
            CastingTime = 0;
            break;
        }

        float LvlPenalty = CalculateLevelPenalty(spellProto);

        // Spellmod SpellDamage
        //float SpellModSpellDamage = 100.0f;
        float CoefficientPtc = ((float)CastingTime / 3500.0f) * DotFactor * 100.0f;

        if (Player* modOwner = GetSpellModOwner())
            //modOwner->ApplySpellMod(spellProto->Id,SPELLMOD_SPELL_BONUS_DAMAGE,SpellModSpellDamage);
            modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_SPELL_BONUS_DAMAGE, CoefficientPtc);

        //SpellModSpellDamage /= 100.0f;
        CoefficientPtc /= 100.0f;

        //ActualBenefit = (float)AdvertisedBenefit * ((float)CastingTime / 3500.0f) * DotFactor * SpellModSpellDamage * LvlPenalty;
        ActualBenefit = (float)AdvertisedBenefit * CoefficientPtc * LvlPenalty;
    }

    // use float as more appropriate for negative values and percent applying
    float heal = healamount + ActualBenefit;

    // @todo check for ALL/SPELLS type
    // Healing done percent
    AuraList const& mHealingDonePct = GetAurasByType(SPELL_AURA_MOD_HEALING_DONE_PERCENT);
    for (AuraList::const_iterator i = mHealingDonePct.begin(); i != mHealingDonePct.end(); ++i)
        heal *= (100.0f + (*i)->GetModifierValue()) / 100.0f;

    // apply spellmod to Done amount
    if (Player* modOwner = GetSpellModOwner())
        modOwner->ApplySpellMod(spellProto->Id, damagetype == DOT ? SPELLMOD_DOT : SPELLMOD_DAMAGE, heal);

    // Healing Wave cast
    if (spellProto->SpellFamilyName == SPELLFAMILY_SHAMAN && spellProto->SpellFamilyFlags & 0x0000000000000040LL)
    {
        // Search for Healing Way on Victim (stack up to 3 time)
        int32 pctMod = 0;
        Unit::AuraList const& auraDummy = victim->GetAurasByType(SPELL_AURA_DUMMY);
        for (Unit::AuraList::const_iterator itr = auraDummy.begin(); itr != auraDummy.end(); ++itr)
        {
            if ((*itr)->GetId() == 29203)
            {
                pctMod = (*itr)->GetModifier()->m_amount * (*itr)->GetStackAmount();
                break;
            }
        }
        // Apply bonus
        if (pctMod)
            heal = heal * (100 + pctMod) / 100;
    }

    heal *= TotalHealPct;

    return heal < 0 ? 0 : uint32(heal);
}

int32 Unit::SpellBaseHealingBonus(SpellSchoolMask schoolMask)
{
    int32 AdvertisedBenefit = 0;

    AuraList const& mHealingDone = GetAurasByType(SPELL_AURA_MOD_HEALING_DONE);
    for (AuraList::const_iterator i = mHealingDone.begin(); i != mHealingDone.end(); ++i)
        if (((*i)->GetModifier()->m_miscvalue & schoolMask) != 0)
            AdvertisedBenefit += (*i)->GetModifierValue();

    // Healing bonus of spirit, intellect and strength
    if (GetTypeId() == TYPEID_PLAYER)
    {
        // Healing bonus from stats
        AuraList const& mHealingDoneOfStatPercent = GetAurasByType(SPELL_AURA_MOD_SPELL_HEALING_OF_STAT_PERCENT);
        for (AuraList::const_iterator i = mHealingDoneOfStatPercent.begin(); i != mHealingDoneOfStatPercent.end(); ++i)
        {
            // stat used dependent from misc value (stat index)
            Stats usedStat = Stats((*i)->GetSpellProto()->EffectMiscValue[(*i)->GetEffIndex()]);
            AdvertisedBenefit += int32(GetStat(usedStat) * (*i)->GetModifierValue() / 100.0f);
        }

        // ... and attack power
        AuraList const& mHealingDonebyAP = GetAurasByType(SPELL_AURA_MOD_SPELL_HEALING_OF_ATTACK_POWER);
        for (AuraList::const_iterator i = mHealingDonebyAP.begin(); i != mHealingDonebyAP.end(); ++i)
            if ((*i)->GetModifier()->m_miscvalue & schoolMask)
                AdvertisedBenefit += int32(GetTotalAttackPowerValue(BASE_ATTACK) * (*i)->GetModifierValue() / 100.0f);
    }
    return AdvertisedBenefit;
}

int32 Unit::SpellBaseHealingBonusForVictim(SpellSchoolMask schoolMask, Unit* victim)
{
    int32 AdvertisedBenefit = 0;

    AuraList const& mDamageTaken = victim->GetAurasByType(SPELL_AURA_MOD_HEALING);
    for (AuraList::const_iterator i = mDamageTaken.begin(); i != mDamageTaken.end(); ++i)
        if (((*i)->GetModifier()->m_miscvalue & schoolMask) != 0)
            AdvertisedBenefit += (*i)->GetModifierValue();
    return AdvertisedBenefit;
}

bool Unit::IsImmunedToDamage(SpellSchoolMask shoolMask, bool /*useCharges*/)
{
    //If m_immuneToSchool type contain this school type, IMMUNE damage.
    SpellImmuneList const& schoolList = m_spellImmune[IMMUNITY_SCHOOL];
    for (SpellImmuneList::const_iterator itr = schoolList.begin(); itr != schoolList.end(); ++itr)
        if (itr->type & shoolMask)
            return true;

    //If m_immuneToDamage type contain magic, IMMUNE damage.
    SpellImmuneList const& damageList = m_spellImmune[IMMUNITY_DAMAGE];
    for (SpellImmuneList::const_iterator itr = damageList.begin(); itr != damageList.end(); ++itr)
        if (itr->type & shoolMask)
            return true;

    return false;
}

bool Unit::IsImmunedToDamage(SpellEntry const* spellInfo) const
{
    if (spellInfo->Attributes & SPELL_ATTR0_UNAFFECTED_BY_INVULNERABILITY)
        return false;

    uint32 shoolMask = GetSpellSchoolMask(spellInfo);
    if (spellInfo->Id != 42292 && spellInfo->Id != 59752)
    {
        // If m_immuneToSchool type contain this school type, IMMUNE damage.
        SpellImmuneList const& schoolList = m_spellImmune[IMMUNITY_SCHOOL];
        for (SpellImmuneList::const_iterator itr = schoolList.begin(); itr != schoolList.end(); ++itr)
            if (itr->type & shoolMask)
                return true;
    }

    // If m_immuneToDamage type contain magic, IMMUNE damage.
    SpellImmuneList const& damageList = m_spellImmune[IMMUNITY_DAMAGE];
    for (SpellImmuneList::const_iterator itr = damageList.begin(); itr != damageList.end(); ++itr)
        if (itr->type & shoolMask)
            return true;

    return false;
}

bool Unit::IsImmuneToSpell(SpellEntry const* spellInfo, bool /*useCharges*/)
{
    if (!spellInfo)
        return false;

    // Single spell immunity.
    SpellImmuneList const& idList = m_spellImmune[IMMUNITY_ID];
    for (SpellImmuneList::const_iterator itr = idList.begin(); itr != idList.end(); ++itr)
        if (itr->type == spellInfo->Id)
            return true;

    // If Spell has this flag cannot be resisted/immuned/etc
    if (spellInfo->Attributes & SPELL_ATTR0_UNAFFECTED_BY_INVULNERABILITY)
        return false;

    if (spellInfo->Dispel)
    {
        SpellImmuneList const& dispelList = m_spellImmune[IMMUNITY_DISPEL];
        for (SpellImmuneList::const_iterator itr = dispelList.begin(); itr != dispelList.end(); ++itr)
            if (itr->type == spellInfo->Dispel)
                return true;
    }

    // Spells that don't have effectMechanics.
    if (spellInfo->Mechanic)
    {
        SpellImmuneList const& mechanicList = m_spellImmune[IMMUNITY_MECHANIC];
        for (SpellImmuneList::const_iterator itr = mechanicList.begin(); itr != mechanicList.end(); ++itr)
            if (itr->type == spellInfo->Mechanic)
                return true;
    }

    bool immuneToAllEffects = true;
    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        // State/effect immunities applied by aura expect full spell immunity
        // Ignore effects with mechanic, they are supposed to be checked separately
        if (!spellInfo->IsEffect(i))
            continue;
        if (!IsImmuneToSpellEffect(spellInfo, i, false))
        {
            immuneToAllEffects = false;
            break;
        }
    }

    if (immuneToAllEffects) //Return immune only if the target is immune to all spell effects.
        return true;

    if (!(spellInfo->AttributesEx & SPELL_ATTR1_UNAFFECTED_BY_SCHOOL_IMMUNE) &&         // unaffected by school immunity
        !(spellInfo->AttributesEx & SPELL_ATTR1_DISPEL_AURAS_ON_IMMUNITY) &&            // can remove immune (by dispell or immune it)
        (spellInfo->Id != 42292))
    {
        SpellImmuneList const& schoolList = m_spellImmune[IMMUNITY_SCHOOL];
        for (SpellImmuneList::const_iterator itr = schoolList.begin(); itr != schoolList.end(); ++itr)
            if (!(IsPositiveSpell(itr->spellId) && IsPositiveSpell(spellInfo->Id)) &&
                (itr->type & GetSpellSchoolMask(spellInfo)))
                return true;
    }

    return false;
}

bool Unit::IsImmuneToSpellEffect(SpellEntry const* spellInfo, uint32 index, bool /*castOnSelf*/) const
{
    if (!spellInfo || !spellInfo->IsEffect(index))
        return false;

    if (spellInfo->Attributes & SPELL_ATTR0_UNAFFECTED_BY_INVULNERABILITY)
        return false;

    //If m_immuneToEffect type contain this effect type, IMMUNE effect.
    uint32 effect = spellInfo->Effect[index];
    SpellImmuneList const& effectList = m_spellImmune[IMMUNITY_EFFECT];
    for (SpellImmuneList::const_iterator itr = effectList.begin(); itr != effectList.end(); ++itr)
        if (itr->type == effect)
            return true;

    if (uint32 mechanic = spellInfo->EffectMechanic[index])
    {
        SpellImmuneList const& mechanicList = m_spellImmune[IMMUNITY_MECHANIC];
        for (SpellImmuneList::const_iterator itr = mechanicList.begin(); itr != mechanicList.end(); ++itr)
            if (itr->type == mechanic)
                return true;
    }

    if (uint32 aura = spellInfo->EffectApplyAuraName[index])
    {
        SpellImmuneList const& list = m_spellImmune[IMMUNITY_STATE];
        for (SpellImmuneList::const_iterator itr = list.begin(); itr != list.end(); ++itr)
            if (itr->type == aura)
                if (!(spellInfo->AttributesEx3 & SPELL_ATTR3_CANT_MISS))
                    return true;
    }

    return false;
}

void Unit::MeleeDamageBonus(Unit* victim, uint32* pdamage, WeaponAttackType attType, SpellEntry const* spellProto)
{
    if (!victim || pdamage == 0 || (spellProto && spellProto->AttributesEx6 & SPELL_ATTR6_NO_DMG_PERCENT_MODS))
        return;

    uint32 creatureTypeMask = victim->GetCreatureTypeMask();

    // Taken/Done fixed damage bonus auras
    int32 DoneFlatBenefit = 0;
    int32 TakenFlatBenefit = 0;

    // ..done (for creature type by mask) in taken
    AuraList const& mDamageDoneCreature = GetAurasByType(SPELL_AURA_MOD_DAMAGE_DONE_CREATURE);
    for (AuraList::const_iterator i = mDamageDoneCreature.begin(); i != mDamageDoneCreature.end(); ++i)
        if (creatureTypeMask & uint32((*i)->GetModifier()->m_miscvalue))
            DoneFlatBenefit += (*i)->GetModifierValue();

    // ..done
    // SPELL_AURA_MOD_DAMAGE_DONE included in weapon damage

    // ..done (base at attack power for marked target and base at attack power for creature type)
    int32 APbonus = 0;

    if (attType == RANGED_ATTACK)
    {
        APbonus += victim->GetTotalAuraModifier(SPELL_AURA_RANGED_ATTACK_POWER_ATTACKER_BONUS);

        // ..done (base at attack power and creature type)
        AuraList const& mCreatureAttackPower = GetAurasByType(SPELL_AURA_MOD_RANGED_ATTACK_POWER_VERSUS);
        for (AuraList::const_iterator i = mCreatureAttackPower.begin(); i != mCreatureAttackPower.end(); ++i)
            if (creatureTypeMask & uint32((*i)->GetModifier()->m_miscvalue))
                APbonus += (*i)->GetModifierValue();
    }
    else
    {
        APbonus += victim->GetTotalAuraModifier(SPELL_AURA_MELEE_ATTACK_POWER_ATTACKER_BONUS);

        // ..done (base at attack power and creature type)
        AuraList const& mCreatureAttackPower = GetAurasByType(SPELL_AURA_MOD_MELEE_ATTACK_POWER_VERSUS);
        for (AuraList::const_iterator i = mCreatureAttackPower.begin(); i != mCreatureAttackPower.end(); ++i)
            if (creatureTypeMask & uint32((*i)->GetModifier()->m_miscvalue))
                APbonus += (*i)->GetModifierValue();
    }

    if (APbonus != 0)                                       // Can be negative
    {
        bool normalized = false;
        if (spellProto)
            for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
                if (spellProto->Effect[i] == SPELL_EFFECT_NORMALIZED_WEAPON_DMG)
                {
                    normalized = true;
                    break;
                }
        DoneFlatBenefit += int32(APbonus / 14.0f * GetAPMultiplier(attType, normalized));
    }

    // ..taken
    AuraList const& mDamageTaken = victim->GetAurasByType(SPELL_AURA_MOD_DAMAGE_TAKEN);
    for (AuraList::const_iterator i = mDamageTaken.begin(); i != mDamageTaken.end(); ++i)
        if ((*i)->GetModifier()->m_miscvalue & GetMeleeDamageSchoolMask())
            TakenFlatBenefit += (*i)->GetModifierValue();

    if (attType != RANGED_ATTACK)
        TakenFlatBenefit += victim->GetTotalAuraModifier(SPELL_AURA_MOD_MELEE_DAMAGE_TAKEN);
    else
        TakenFlatBenefit += victim->GetTotalAuraModifier(SPELL_AURA_MOD_RANGED_DAMAGE_TAKEN);

    // Done/Taken total percent damage auras
    float DoneTotalMod = 1.0f;
    float TakenTotalMod = 1.0f;

    // ..done
    // SPELL_AURA_MOD_DAMAGE_PERCENT_DONE included in weapon damage
    // SPELL_AURA_MOD_OFFHAND_DAMAGE_PCT  included in weapon damage

    AuraList const& mDamageDoneVersus = GetAurasByType(SPELL_AURA_MOD_DAMAGE_DONE_VERSUS);
    for (AuraList::const_iterator i = mDamageDoneVersus.begin(); i != mDamageDoneVersus.end(); ++i)
        if (creatureTypeMask & uint32((*i)->GetModifier()->m_miscvalue))
            DoneTotalMod *= ((*i)->GetModifierValue() + 100.0f) / 100.0f;

    // ..taken
    AuraList const& mModDamagePercentTaken = victim->GetAurasByType(SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN);
    for (AuraList::const_iterator i = mModDamagePercentTaken.begin(); i != mModDamagePercentTaken.end(); ++i)
        if ((*i)->GetModifier()->m_miscvalue & GetMeleeDamageSchoolMask())
            TakenTotalMod *= ((*i)->GetModifierValue() + 100.0f) / 100.0f;

    // .. taken pct: dummy auras
    AuraList const& mDummyAuras = victim->GetAurasByType(SPELL_AURA_DUMMY);
    for (AuraList::const_iterator i = mDummyAuras.begin(); i != mDummyAuras.end(); ++i)
    {
        switch ((*i)->GetSpellProto()->SpellIconID)
        {
            //Cheat Death
            case 2109:
                if ((*i)->GetModifier()->m_miscvalue & SPELL_SCHOOL_MASK_NORMAL)
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        continue;
                    // Patch 2.4.3: The resilience required to reach the 90% damage reduction cap
                    // is 22.5% critical strike damage reduction, or 444 resilience.
                    // To calculate for 90%, we multiply the 100% by 4 (22.5% * 4 = 90%)
                    float mod = -1.0f * ToPlayer()->GetMeleeCritDamageReduction(400);
                    AddPct(TakenTotalMod, std::max(mod, float((*i)->GetAmount())));
                }
                break;
            //Mangle
            case 2312:
                if (spellProto == NULL)
                    break;
                // Should increase Shred (initial Damage of Lacerate and Rake handled in Spell::EffectSchoolDMG)
                if (spellProto->SpellFamilyName == SPELLFAMILY_DRUID && (spellProto->SpellFamilyFlags == 0x00008000LL))
                    TakenTotalMod *= (100.0f + (*i)->GetModifier()->m_amount) / 100.0f;
                break;
        }
    }

    // .. taken pct: class scripts
    AuraList const& mclassScritAuras = GetAurasByType(SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
    for (AuraList::const_iterator i = mclassScritAuras.begin(); i != mclassScritAuras.end(); ++i)
    {
        switch ((*i)->GetMiscValue())
        {
            case 6427:
            case 6428:                           // Dirty Deeds
                if (victim->HasAuraState(AURA_STATE_HEALTHLESS_35_PERCENT))
                {
                    Aura* eff0 = GetAura((*i)->GetId(), 0);
                    if (!eff0 || (*i)->GetEffIndex() != 1)
                    {
                        sLog.outError("Spell structure of DD (%u) changed.", (*i)->GetId());
                        continue;
                    }

                    // effect 0 have expected value but in negative state
                    TakenTotalMod *= (-eff0->GetModifier()->m_amount + 100.0f) / 100.0f;
                }
            break;
        }
    }

    if (attType != RANGED_ATTACK)
    {
        AuraList const& mModMeleeDamageTakenPercent = victim->GetAurasByType(SPELL_AURA_MOD_MELEE_DAMAGE_TAKEN_PCT);
        for (AuraList::const_iterator i = mModMeleeDamageTakenPercent.begin(); i != mModMeleeDamageTakenPercent.end(); ++i)
            TakenTotalMod *= ((*i)->GetModifierValue() + 100.0f) / 100.0f;
    }
    else
    {
        AuraList const& mModRangedDamageTakenPercent = victim->GetAurasByType(SPELL_AURA_MOD_RANGED_DAMAGE_TAKEN_PCT);
        for (AuraList::const_iterator i = mModRangedDamageTakenPercent.begin(); i != mModRangedDamageTakenPercent.end(); ++i)
            TakenTotalMod *= ((*i)->GetModifierValue() + 100.0f) / 100.0f;
    }

    float tmpDamage = float(int32(*pdamage) + DoneFlatBenefit) * DoneTotalMod;

    // apply spellmod to Done damage
    if (spellProto)
        if (Player* modOwner = GetSpellModOwner())
            modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_DAMAGE, tmpDamage);

    tmpDamage = (tmpDamage + TakenFlatBenefit) * TakenTotalMod;

    // bonus result can be negative
    *pdamage =  tmpDamage > 0 ? uint32(tmpDamage) : 0;
}

void Unit::ApplySpellImmune(uint32 spellId, uint32 op, uint32 type, bool apply)
{
    if (apply)
    {
        for (SpellImmuneList::iterator itr = m_spellImmune[op].begin(), next; itr != m_spellImmune[op].end(); itr = next)
        {
            next = itr; ++next;
            if (itr->type == type)
            {
                m_spellImmune[op].erase(itr);
                next = m_spellImmune[op].begin();
            }
        }
        SpellImmune Immune;
        Immune.spellId = spellId;
        Immune.type = type;
        m_spellImmune[op].push_back(Immune);
    }
    else
    {
        for (SpellImmuneList::iterator itr = m_spellImmune[op].begin(); itr != m_spellImmune[op].end(); ++itr)
        {
            if (itr->spellId == spellId && itr->type == type)
            {
                m_spellImmune[op].erase(itr);
                break;
            }
        }
    }

}

void Unit::ApplySpellDispelImmunity(const SpellEntry* spellProto, DispelType type, bool apply)
{
    ApplySpellImmune(spellProto->Id, IMMUNITY_DISPEL, type, apply);

    if (apply && spellProto->AttributesEx & SPELL_ATTR1_DISPEL_AURAS_ON_IMMUNITY)
        RemoveAurasWithDispelType(type);
}

float Unit::GetWeaponProcChance() const
{
    // normalized proc chance for weapon attack speed
    // (odd formula...)
    if (isAttackReady(BASE_ATTACK))
        return (GetAttackTime(BASE_ATTACK) * 1.8f / 1000.0f);
    else if (haveOffhandWeapon() && isAttackReady(OFF_ATTACK))
        return (GetAttackTime(OFF_ATTACK) * 1.6f / 1000.0f);
    return 0;
}

float Unit::GetPPMProcChance(uint32 WeaponSpeed, float PPM) const
{
    // proc per minute chance calculation
    if (PPM <= 0)
        return 0.0f;

    uint32 result = uint32((WeaponSpeed * PPM) / 600.0f);   // result is chance in percents (probability = Speed_in_sec * (PPM / 60))
    return result;
}

void Unit::Mount(uint32 mount, uint32 spellId)
{
    if (mount)
        SetUInt32Value(UNIT_FIELD_MOUNTDISPLAYID, mount);

    SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_MOUNT);

    // unsummon pet
    if (GetTypeId() == TYPEID_PLAYER)
    {
        // Called by taxi
        if (!spellId)
            ToPlayer()->UnsummonPetTemporaryIfAny();
        // Called by mount aura
        else if (SpellEntry const* spellInfo = sSpellStore.LookupEntry(spellId))
        {
            // Flying case (Unsummon any pet)
            if (!GetAurasByType(SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED).empty())
                ToPlayer()->UnsummonPetTemporaryIfAny();
            // Normal case (Unsummon only permanent pet)
            else if (Pet* pet = ToPlayer()->GetPet())
            {
                if (pet->IsPermanentPetFor((Player*)this) && !ToPlayer()->InArena())
                    ToPlayer()->UnsummonPetTemporaryIfAny();
                else
                    pet->ApplyModeFlags(PET_MODE_DISABLE_ACTIONS, true);
            }
        }
    }

    RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_MOUNT);
}

void Unit::Dismount()
{
    if (!IsMounted())
        return;

    SetUInt32Value(UNIT_FIELD_MOUNTDISPLAYID, 0);
    RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_MOUNT);

    WorldPacket data(SMSG_DISMOUNT, 8);
    data << GetPackGUID();
    SendMessageToSet(&data, true);

    RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_NOT_MOUNTED);

    // only resummon old pet if the player is already added to a map
    // this prevents adding a pet to a not created map which would otherwise cause a crash
    // (it could probably happen when logging in after a previous crash)
    if (GetTypeId() == TYPEID_PLAYER && IsInWorld() && IsAlive())
    {
        if(Pet* pet = ToPlayer()->GetPet())
            pet->ApplyModeFlags(PET_MODE_DISABLE_ACTIONS, false);
        else
            ToPlayer()->ResummonTemporaryUnsummonedPetIfAny();
    }
}

void Unit::SetInCombatWith(Unit* enemy)
{
    Unit* eOwner = enemy->GetCharmerOrOwnerOrSelf();
    if (eOwner->IsPvP())
    {
        SetInCombatState(true, enemy);
        return;
    }

    //check for duel
    if (eOwner->GetTypeId() == TYPEID_PLAYER && eOwner->ToPlayer()->duel)
    {
        Unit const* myOwner = GetCharmerOrOwnerOrSelf();
        if (eOwner->ToPlayer()->duel->opponent == myOwner)
        {
            SetInCombatState(true, enemy);
            return;
        }
    }
    SetInCombatState(false, enemy);
}

void Unit::CombatStart(Unit* target, bool initialAggro)
{
    if (target->ToCreature() && target->ToCreature()->IsInEvadeMode())
        return;

    if (initialAggro)
    {
        if (!target->IsStandState())
            target->SetStandState(UNIT_STAND_STATE_STAND);

        if (!target->IsInCombat() && target->GetTypeId() != TYPEID_PLAYER
            && !target->ToCreature()->HasReactState(REACT_PASSIVE) && target->ToCreature()->IsAIEnabled)
        {
            if (target->IsPet())
                target->ToCreature()->AI()->AttackedBy(this); // PetAI has special handler before AttackStart()
            else
                target->ToCreature()->AI()->AttackStart(this);

            if (((Creature*)target)->GetFormation())
            {
                ((Creature*)target)->GetFormation()->MemberAttackStart((Creature*)target, this);
                sLog.outDebug("Unit::CombatStart() calls CreatureGroups::MemberHasAttacked(this);");
            }
        }

        SetInCombatWith(target);
        target->SetInCombatWith(this);
    }
    Unit* who = target->GetCharmerOrOwnerOrSelf();
    if (who->GetTypeId() == TYPEID_PLAYER)
        SetContestedPvP(who->ToPlayer());

    Player* me = GetCharmerOrOwnerPlayerOrPlayerItself();
    if (me && who->IsPvP()
        && (who->GetTypeId() != TYPEID_PLAYER
            || !me->duel || me->duel->opponent != who))
    {
        me->UpdatePvP(true);
        me->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_ENTER_PVP_COMBAT);
    }
}

void Unit::SetInCombatState(bool PvP, Unit* enemy)
{
    // only alive units can be in combat
    if (!IsAlive())
        return;

    if (PvP)
    {
        m_CombatTimer = 5500;
    }

    if (IsInCombat() || HasUnitState(UNIT_STATE_EVADE))
        return;

    // Combat is no longer in initiation phase
    SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);
    SetInitiatingCombat(false);

    if (GetTypeId() == TYPEID_PLAYER)
    {
        if (m_currentSpells[CURRENT_GENERIC_SPELL] && m_currentSpells[CURRENT_GENERIC_SPELL]->getState() != SPELL_STATE_FINISHED)
        {
            if (m_currentSpells[CURRENT_GENERIC_SPELL]->m_spellInfo->Attributes & SPELL_ATTR0_CANT_USED_IN_COMBAT)
                InterruptSpell(CURRENT_GENERIC_SPELL);
        }
    }
    else
    {
        // Set home position at place of engaging combat for escorted creatures
        if ((IsAIEnabled && ToCreature()->AI()->IsEscorted()) ||
            GetMotionMaster()->GetCurrentMovementGeneratorType() == WAYPOINT_MOTION_TYPE ||
            GetMotionMaster()->GetCurrentMovementGeneratorType() == POINT_MOTION_TYPE)
            ToCreature()->SetHomePosition(GetPositionX(), GetPositionY(), GetPositionZ(), GetOrientation());

        if (enemy)
        {
            if (IsAIEnabled)
            {
                ToCreature()->AI()->EnterCombat(enemy);
                RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC); // unit has engaged in combat, remove immunity so players can fight back
            }

            if (ToCreature()->GetFormation())
                ToCreature()->GetFormation()->MemberAttackStart((Creature*)this, enemy);
        }

        if (IsPet())
        {
            UpdateSpeed(MOVE_RUN, true);
            UpdateSpeed(MOVE_SWIM, true);
            UpdateSpeed(MOVE_FLIGHT, true);
        }

        if (!(ToCreature()->GetCreatureTemplate()->type_flags & CREATURE_TYPE_FLAG_MOUNTED_COMBAT_ALLOWED))
            Dismount();
    }

    for (Unit::ControlList::iterator itr = m_Controlled.begin(); itr != m_Controlled.end();)
    {
        Unit* controlled = *itr;
        ++itr;

        controlled->SetInCombatState(PvP, enemy);
    }
}

void Unit::ClearInCombat()
{
    m_CombatTimer = 0;
    RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);

    // Player's state will be cleared in Player::UpdateContestedPvP
    if (Creature* creature = ToCreature())
    {
        if (creature->GetCreatureTemplate() && creature->GetCreatureTemplate()->unit_flags & UNIT_FLAG_IMMUNE_TO_PC)
            SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);

        ClearUnitState(UNIT_STATE_ATTACK_PLAYER);
        if (HasFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_OTHER_TAGGER))
            SetUInt32Value(UNIT_DYNAMIC_FLAGS, creature->GetCreatureTemplate()->dynamicflags);
            //RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_OTHER_TAGGER);
    
        if (creature->IsPet() || creature->IsGuardian())
        {
            // creatures' pets only
            if (Creature* owner = dynamic_cast<Creature*>(GetOwner()))
                for (uint8 i = 0; i < MAX_MOVE_TYPE; ++i)
                    if (owner->GetSpeedRate(UnitMoveType(i)) > GetSpeedRate(UnitMoveType(i)))
                        SetSpeed(UnitMoveType(i), owner->GetSpeedRate(UnitMoveType(i)), true);
        }
        else if (!isCharmed())
            return;
    }
}

void Unit::ClearInPetCombat()
{
    RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PET_IN_COMBAT);
    if (Unit* owner = GetOwner())
        owner->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PET_IN_COMBAT);
}

bool Unit::isTargetableForAttack(bool checkFakeDeath) const
{
    if (!IsAlive())
        return false;

    if (HasFlag(UNIT_FIELD_FLAGS,
        UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC))
        return false;

    if (GetTypeId() == TYPEID_PLAYER && ToPlayer()->IsGameMaster())
        return false;

    return !HasUnitState(UNIT_STATE_UNATTACKABLE) && (!checkFakeDeath || !HasUnitState(UNIT_STATE_DIED));
}

bool Unit::canAttack(Unit const* target, bool force) const
{
    ASSERT(target);

    if (force)
    {
        if (IsFriendlyTo(target))
            return false;
    }
    else if (!IsHostileTo(target))
        return false;

    if (!target->isAttackableByAOE())
        return false;

    // feign dead case
    if (target->HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH))
    {
        if ((GetTypeId() != TYPEID_PLAYER && !GetOwner()) || (GetOwner() && GetOwner()->GetTypeId() != TYPEID_PLAYER))
            return false;
        // if this == player or owner == player check other conditions
    }
    // real dead case ~UNIT_FLAG2_FEIGN_DEATH && UNIT_STATE_DIED
    else if (target->HasUnitState(UNIT_STATE_DIED))
        return false;

    return true;
}

bool Unit::IsValidAttackTarget(Unit const* target) const
{
    return _IsValidAttackTarget(target, NULL);
}

bool Unit::IsInSanctuary() const
{
    const AreaTableEntry* area = GetAreaEntryByAreaID(GetAreaId());
    if (area && area->flags & AREA_FLAG_SANCTUARY)       //sanctuary
        return true;

    return false;
}

bool Unit::_IsValidAttackTarget(Unit const* target, SpellEntry const* bySpell, WorldObject const* obj) const
{
    ASSERT(target);

    // can't attack self
    if (this == target)
        return false;

    // can't attack unattackable units or GMs
    if (target->HasUnitState(UNIT_STATE_UNATTACKABLE)
        || target->GetTypeId() == TYPEID_PLAYER && target->ToPlayer()->IsGameMaster())
        return false;

    // visibility checks
    // skip visibility check for GO casts, needs removal when go cast is implemented. Also ignore for gameobject and dynauras
    if (GetEntry() != WORLD_TRIGGER && (!obj || !obj->isType(TYPEMASK_GAMEOBJECT | TYPEMASK_DYNAMICOBJECT)))
    {
        // can't attack invisible
        if (!bySpell)
        {
            if (obj && !obj->CanSeeOrDetect(target))
                return false;
            else if (!obj)
            {
                // ignore stealth for aoe spells. Ignore stealth if target is player and unit in combat with same player
                bool const ignoreStealthCheck = target->GetTypeId() == TYPEID_PLAYER && target->HasStealthAura() && target->IsInCombat() && IsInCombatWith(target);

                if (!CanSeeOrDetect(target, ignoreStealthCheck))
                    return false;
            }
        }
    }

    // can't attack dead
    if (!target->IsAlive())
        return false;

    // can't attack untargetable
    if (!bySpell && target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
        return false;

    // check flags
    if (target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_TAXI_FLIGHT | UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_NOT_PL_SPELL_TARGET)
        || (!HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE) && target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC))
        || (!target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE) && HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC))
        || (HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE) && target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC))
        // check if this is a world trigger cast - GOs are using world triggers to cast their spells, so we need to ignore their immunity flag here, this is a temp workaround, needs removal when go cast is implemented properly
        || ((GetEntry() != WORLD_TRIGGER && (!obj || !obj->isType(TYPEMASK_GAMEOBJECT | TYPEMASK_DYNAMICOBJECT))) && target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE) && HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC)))
        return false;

    // CvC case - can attack each other only when one of them is hostile
    if (!HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE) && !target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE))
        return GetReactionTo(target) <= REP_HOSTILE || target->GetReactionTo(this) <= REP_HOSTILE;

    // PvP, PvC, CvP case
    // can't attack friendly targets
    if (GetReactionTo(target) > REP_NEUTRAL
        || target->GetReactionTo(this) > REP_NEUTRAL)
        return false;

    Player const* playerAffectingAttacker = HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE) ? GetAffectingPlayer() : nullptr;
    Player const* playerAffectingTarget = target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE) ? target->GetAffectingPlayer() : nullptr;

    // Not all neutral creatures can be attacked (even some unfriendly faction does not react aggresive to you, like Sporaggar)
    if ((playerAffectingAttacker && !playerAffectingTarget) ||
        (!playerAffectingAttacker && playerAffectingTarget))
    {
        Player const* player = playerAffectingAttacker ? playerAffectingAttacker : playerAffectingTarget;
        Unit const* creature = playerAffectingAttacker ? target : this;

        if (creature->IsContestedGuard() && player->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_CONTESTED_PVP))
            return true;

        if (FactionTemplateEntry const* factionTemplate = creature->GetFactionTemplateEntry())
        {
            if (!(player->GetReputationMgr().GetForcedRankIfAny(factionTemplate)))
                if (FactionEntry const* factionEntry = sFactionStore.LookupEntry(factionTemplate->faction))
                    if (FactionState const* repState = player->GetReputationMgr().GetState(factionEntry))
                        if (!(repState->Flags & FACTION_FLAG_AT_WAR) && (!IsContestedGuard() && !player->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_CONTESTED_PVP)))
                            return false;

        }
    }

    // check duel - before sanctuary checks
    if (playerAffectingAttacker && playerAffectingTarget)
        if (playerAffectingAttacker->duel && playerAffectingAttacker->duel->opponent == playerAffectingTarget && playerAffectingAttacker->duel->startTime != 0)
            return true;

    // PvP case - can't attack when attacker or target are in sanctuary
    if (target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE) && HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE) && (target->IsInSanctuary() || IsInSanctuary()))
        return false;

    // additional checks - only PvP case
    if (playerAffectingAttacker && playerAffectingTarget)
    {
        if (target->IsPvP())
            return true;

        if (playerAffectingAttacker->IsFFAPvP() && playerAffectingTarget->IsFFAPvP())
            return true;

        return HasFlag(UNIT_FIELD_BYTES_2, UNIT_BYTE2_FLAG_UNK1)
            || target->HasFlag(UNIT_FIELD_BYTES_2, UNIT_BYTE2_FLAG_UNK1);
    }
    return true;
}

bool Unit::IsValidAssistTarget(Unit const* target) const
{
    return _IsValidAssistTarget(target, NULL);
}

bool Unit::_IsValidAssistTarget(Unit const* target, SpellEntry const* bySpell) const
{
    ASSERT(target);

    // can assist to self
    if (this == target)
        return true;

    // can't assist unattackable units or GMs
    if (target->HasUnitState(UNIT_STATE_UNATTACKABLE)
        || target->GetTypeId() == TYPEID_PLAYER && target->ToPlayer()->IsGameMaster())
        return false;

    // can't assist invisible
    if (!CanSeeOrDetect(target))
        return false;

    // can't assist dead
    if (!target->IsAlive())
        return false;

    // can't assist untargetable
    if (target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
        return false;

    if (!bySpell || !(bySpell->AttributesEx6 & SPELL_ATTR6_ASSIST_IGNORE_IMMUNE_FLAG))
    {
        if (HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE))
        {
            if (target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC))
                return false;
        }
        else
        {
            if (target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC))
                return false;
        }
    }

    // can't assist non-friendly targets
    if (GetReactionTo(target) < REP_NEUTRAL
        && target->GetReactionTo(this) < REP_NEUTRAL)
        return false;

    // PvP case
    if (target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE))
    {
        Player const* targetPlayerOwner = target->GetAffectingPlayer();
        if (HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE))
        {
            Player const* selfPlayerOwner = GetAffectingPlayer();
            if (selfPlayerOwner && targetPlayerOwner)
            {
                // can't assist player which is dueling someone
                if (selfPlayerOwner != targetPlayerOwner
                    && targetPlayerOwner->duel)
                    return false;
            }
            // can't assist player in ffa_pvp zone from outside
            if ((target->GetByteValue(UNIT_FIELD_BYTES_2, 1) & UNIT_BYTE2_FLAG_FFA_PVP)
                && !(GetByteValue(UNIT_FIELD_BYTES_2, 1) & UNIT_BYTE2_FLAG_FFA_PVP))
                return false;
            // can't assist player out of sanctuary from sanctuary if has pvp enabled
            if (target->GetByteValue(UNIT_FIELD_BYTES_2, 1) & UNIT_BYTE2_FLAG_UNK0)
                if (HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_SANCTUARY) && !target->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_SANCTUARY))
                    return false;
        }
    }
    // PvC case - player can assist creature only if has specific type flags
    // !target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE) &&
    else if (HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE)
        && (!bySpell || !(bySpell->AttributesEx6 & SPELL_ATTR6_ASSIST_IGNORE_IMMUNE_FLAG))
        && !((target->GetByteValue(UNIT_FIELD_BYTES_2, 1) & UNIT_BYTE2_FLAG_UNK0)))
    {
        if (Creature const* creatureTarget = target->ToCreature())
            return creatureTarget->GetCreatureTemplate()->type_flags & CREATURE_TYPE_FLAG_CAN_ASSIST;
    }
    return true;
}

bool Unit::isAttackableByAOE(float x, float y, float z, bool LosCheck) const
{
    if (!IsAlive())
        return false;

    if (HasFlag(UNIT_FIELD_FLAGS,
                UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC))
        return false;

    if (GetTypeId() == TYPEID_PLAYER && ToPlayer()->IsGameMaster())
        return false;

    if (LosCheck)
        if (!IsWithinLOS(x, y, z))
            return false;

    return !IsInFlight();
}

int32 Unit::ModifyHealth(int32 dVal)
{
    int32 gain = 0;

    if (dVal == 0)
        return 0;

    int32 curHealth = (int32)GetHealth();

    int32 val = dVal + curHealth;
    if (val <= 0)
    {
        SetHealth(0);
        return -curHealth;
    }

    int32 maxHealth = (int32)GetMaxHealth();

    if (val < maxHealth)
    {
        SetHealth(val);
        gain = val - curHealth;
    }
    else if (curHealth != maxHealth)
    {
        SetHealth(maxHealth);
        gain = maxHealth - curHealth;
    }

    return gain;
}

// used only to calculate channeling time
void Unit::ModSpellCastTime(SpellEntry const* spellProto, int32& castTime, Spell* spell)
{
    if (!spellProto || castTime < 0)
        return;

    //called from caster
    if (Player* modOwner = GetSpellModOwner())
        modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_CASTING_TIME, castTime, spell);

    if (!((spellProto->Attributes & (SPELL_ATTR0_ABILITY | SPELL_ATTR0_TRADESPELL)) || (spellProto->AttributesEx3 & SPELL_ATTR3_NO_DONE_BONUS)) &&
        ((GetTypeId() == TYPEID_PLAYER && spellProto->SpellFamilyName) || GetTypeId() == TYPEID_UNIT))
        castTime = int32(float(castTime) * GetFloatValue(UNIT_MOD_CAST_SPEED));
    else if (spellProto->Attributes & SPELL_ATTR0_RANGED && !(spellProto->AttributesEx2 & SPELL_ATTR2_AUTOREPEAT_FLAG))
        castTime = int32 (float(castTime) * m_modAttackSpeedPct[RANGED_ATTACK]);
}

void Unit::ModSpellDurationTime(SpellEntry const* spellProto, int32& duration, Spell* spell)
{
    if (!spellProto || duration < 0)
        return;

    if (IsChanneledSpell(spellProto) && !(spellProto->AttributesEx5 & SPELL_ATTR5_HASTE_AFFECT_DURATION))
        return;

    // called from caster
    if (Player* modOwner = GetSpellModOwner())
        modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_CASTING_TIME, duration, spell);

    if (!((spellProto->Attributes & (SPELL_ATTR0_ABILITY | SPELL_ATTR0_TRADESPELL)) || (spellProto->AttributesEx3 & SPELL_ATTR3_NO_DONE_BONUS)) &&
        ((GetTypeId() == TYPEID_PLAYER && spellProto->SpellFamilyName) || GetTypeId() == TYPEID_UNIT))
        duration = int32(float(duration) * GetFloatValue(UNIT_MOD_CAST_SPEED));
    else if (spellProto->Attributes & SPELL_ATTR0_RANGED && !(spellProto->AttributesEx2 & SPELL_ATTR2_AUTOREPEAT_FLAG))
        duration = int32 (float(duration) * m_modAttackSpeedPct[RANGED_ATTACK]);
}

int32 Unit::ModifyPower(Powers power, int32 dVal)
{
    int32 gain = 0;

    if (dVal == 0)
        return 0;

    int32 curPower = (int32)GetPower(power);

    int32 val = dVal + curPower;
    if (val <= 0)
    {
        SetPower(power, 0);
        return -curPower;
    }

    int32 maxPower = (int32)GetMaxPower(power);

    if (val < maxPower)
    {
        SetPower(power, val);
        gain = val - curPower;
    }
    else if (curPower != maxPower)
    {
        SetPower(power, maxPower);
        gain = maxPower - curPower;
    }

    return gain;
}

bool Unit::IsAlwaysVisibleFor(WorldObject const* seer) const
{
    if (WorldObject::IsAlwaysVisibleFor(seer))
        return true;

    // Always seen by owner
    if (uint64 guid = GetCharmerOrOwnerGUID())
        if (seer->GetGUID() == guid)
            return true;

    if (Player const* seerPlayer = seer->ToPlayer())
        if (Unit* owner =  GetOwner())
            if (Player* ownerPlayer = owner->ToPlayer())
                if (ownerPlayer->IsGroupVisibleFor(seerPlayer))
                    return true;

    return false;
}

/// returns true if creature can't be seen by alive units
bool Unit::isInvisibleForAlive() const
{
    if (m_AuraFlags & UNIT_AURAFLAG_ALIVE_INVISIBLE)
        return true;
    // @todo maybe spiritservices also have just an aura
    return IsSpiritService();
}

bool Unit::IsAlwaysDetectableFor(WorldObject const* seer) const
{
    if (WorldObject::IsAlwaysDetectableFor(seer))
        return true;

    if (HasAuraTypeWithCaster(SPELL_AURA_MOD_STALKED, seer->GetGUID()))
        return true;

    return false;
}

void Unit::SetVisible(bool x)
{
    if (!x)
        m_serverSideVisibility.SetValue(SERVERSIDE_VISIBILITY_GM, SEC_GAMEMASTER);
    else
        m_serverSideVisibility.SetValue(SERVERSIDE_VISIBILITY_GM, SEC_PLAYER);

    UpdateObjectVisibility();
}

void Unit::UpdateSpeed(UnitMoveType mtype, bool forced)
{
    int32 main_speed_mod  = 0;
    float stack_bonus     = 1.0f;
    float non_stack_bonus = 1.0f;

    switch (mtype)
    {
        case MOVE_WALK:
        case MOVE_RUN_BACK:
        case MOVE_SWIM_BACK:
        case MOVE_FLIGHT_BACK:
            break;
        case MOVE_RUN:
        {
            if (IsMounted()) // Use on mount auras
            {
                main_speed_mod  = GetMaxPositiveAuraModifier(SPELL_AURA_MOD_INCREASE_MOUNTED_SPEED);
                stack_bonus     = GetTotalAuraMultiplier(SPELL_AURA_MOD_MOUNTED_SPEED_ALWAYS);
                non_stack_bonus = (100.0f + GetMaxPositiveAuraModifier(SPELL_AURA_MOD_MOUNTED_SPEED_NOT_STACK)) / 100.0f;
            }
            else
            {
                main_speed_mod  = GetMaxPositiveAuraModifier(SPELL_AURA_MOD_INCREASE_SPEED);
                stack_bonus     = GetTotalAuraMultiplier(SPELL_AURA_MOD_SPEED_ALWAYS);
                non_stack_bonus = (100.0f + GetMaxPositiveAuraModifier(SPELL_AURA_MOD_SPEED_NOT_STACK)) / 100.0f;
            }
            break;
        }
        case MOVE_SWIM:
        {
            main_speed_mod  = GetMaxPositiveAuraModifier(SPELL_AURA_MOD_INCREASE_SWIM_SPEED);
            break;
        }
        case MOVE_FLIGHT:
        {
            if (IsMounted()) // Use on mount auras
            {
                main_speed_mod  = GetMaxPositiveAuraModifier(SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED);
                stack_bonus     = GetTotalAuraMultiplier(SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED_STACKING);
                non_stack_bonus = (100.0f + GetMaxPositiveAuraModifier(SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED_NOT_STACKING)) / 100.0f;
            }
            else             // Use not mount (shapeshift for example) auras (should stack)
            {
                main_speed_mod  = GetTotalAuraModifier(SPELL_AURA_MOD_FLIGHT_SPEED);
                stack_bonus     = GetTotalAuraMultiplier(SPELL_AURA_MOD_FLIGHT_SPEED_STACKING);
                non_stack_bonus = (100.0f + GetMaxPositiveAuraModifier(SPELL_AURA_MOD_FLIGHT_SPEED_NOT_STACKING)) / 100.0f;
            }
            break;
        }
        default:
            sLog.outError("Unit::UpdateSpeed: Unsupported move type (%d)", mtype);
            return;
    }

    // now we ready for speed calculation
    float speed = std::max(non_stack_bonus, stack_bonus);
    if (main_speed_mod)
        AddPct(speed, main_speed_mod);

    switch (mtype)
    {
        case MOVE_RUN:
        case MOVE_SWIM:
        case MOVE_FLIGHT:
        {
            // Normalize speed by 191 aura SPELL_AURA_USE_NORMAL_MOVEMENT_SPEED if need
            // @todo possible affect only on MOVE_RUN
            if (int32 normalization = GetMaxPositiveAuraModifier(SPELL_AURA_USE_NORMAL_MOVEMENT_SPEED))
            {
                if (Creature* creature = ToCreature())
                {
                    uint32 immuneMask = creature->GetCreatureTemplate()->MechanicImmuneMask;
                    if (immuneMask & (1 << MECHANIC_SNARE) || immuneMask & (1 << MECHANIC_DAZE))
                        break;
                }

                // Use speed from aura
                float max_speed = normalization / baseMoveSpeed[mtype];
                if (speed > max_speed)
                    speed = max_speed;
            }
            break;
        }
        default:
            break;
    }

    if (GetTypeId() == TYPEID_UNIT)
    {
        if (mtype == MOVE_RUN)
            speed *= ToCreature()->GetCreatureTemplate()->speed_run;

        if (Unit* owner = GetCharmerOrOwner())
        {
            if (ToCreature()->HasUnitTypeMask(UNIT_MASK_MINION) &&
                ToCreature()->HasUnitState(UNIT_STATE_FOLLOW) && !ToCreature()->IsInCombat())
            {
                // Sync speed with owner when near or slower
                float distance = GetDistance(owner);
                float owner_speed = owner->GetSpeedRate(mtype);
                if (distance < (GetCombatReach() + sWorld.getRate(RATE_TARGET_POS_RECALCULATION_RANGE)) || speed < owner_speed)
                    speed = owner_speed;

                // Increase speed when away to help prevent falling behind
                speed *= std::min(1.0f + (distance / 100.0f), 1.2f);
            }
        }

        // Check if mob searched for assistance
        if (ToCreature()->HasSearchedAssistance())
            speed *= 0.66f;                                 // best guessed value, so this will be 33% reduction. Based off initial speed, mob can then "run", "walk fast" or "walk".
    }

    // Apply strongest slow aura mod to speed
    int32 slow = GetMaxNegativeAuraModifier(SPELL_AURA_MOD_DECREASE_SPEED);
    int32 slow_non_stack = GetMaxNegativeAuraModifier(SPELL_AURA_MOD_SPEED_NOT_STACK);
    slow = slow < slow_non_stack ? slow : slow_non_stack;
    if (slow)
        AddPct(speed, slow);

    SetSpeed(mtype, speed, forced);
}

float Unit::GetSpeed(UnitMoveType mtype) const
{
    return m_speed_rate[mtype] * baseMoveSpeed[mtype];
}

void Unit::SetSpeed(UnitMoveType mtype, float rate, bool forced)
{
    if (rate < 0)
        rate = 0.0f;

    // Update speed only on change
    if (m_speed_rate[mtype] == rate)
        return;

    m_speed_rate[mtype] = rate;

    propagateSpeedChange();

    WorldPacket data;
    if (!forced)
    {
        switch (mtype)
        {
            case MOVE_WALK:
                data.Initialize(MSG_MOVE_SET_WALK_SPEED, 8 + 4 + 1 + 4 + 4 + 4 + 4 + 4 + 4 + 4);
                break;
            case MOVE_RUN:
                data.Initialize(MSG_MOVE_SET_RUN_SPEED, 8 + 4 + 1 + 4 + 4 + 4 + 4 + 4 + 4 + 4);
                break;
            case MOVE_RUN_BACK:
                data.Initialize(MSG_MOVE_SET_RUN_BACK_SPEED, 8 + 4 + 1 + 4 + 4 + 4 + 4 + 4 + 4 + 4);
                break;
            case MOVE_SWIM:
                data.Initialize(MSG_MOVE_SET_SWIM_SPEED, 8 + 4 + 1 + 4 + 4 + 4 + 4 + 4 + 4 + 4);
                break;
            case MOVE_SWIM_BACK:
                data.Initialize(MSG_MOVE_SET_SWIM_BACK_SPEED, 8 + 4 + 1 + 4 + 4 + 4 + 4 + 4 + 4 + 4);
                break;
            case MOVE_TURN_RATE:
                data.Initialize(MSG_MOVE_SET_TURN_RATE, 8 + 4 + 1 + 4 + 4 + 4 + 4 + 4 + 4 + 4);
                break;
            case MOVE_FLIGHT:
                data.Initialize(MSG_MOVE_SET_FLIGHT_SPEED, 8 + 4 + 1 + 4 + 4 + 4 + 4 + 4 + 4 + 4);
                break;
            case MOVE_FLIGHT_BACK:
                data.Initialize(MSG_MOVE_SET_FLIGHT_BACK_SPEED, 8 + 4 + 1 + 4 + 4 + 4 + 4 + 4 + 4 + 4);
                break;
            default:
                sLog.outError("Unit::SetSpeed: Unsupported move type (%d), data not sent to client.", mtype);
                return;
        }

        data << GetPackGUID();
        BuildMovementPacket(&data);
        data << float(GetSpeed(mtype));
        SendMessageToSet(&data, true);
    }
    else
    {
        if (GetTypeId() == TYPEID_PLAYER)
        {
            // register forced speed changes for WorldSession::HandleForceSpeedChangeAck
            // and do it only for real sent packets and use run for run/mounted as client expected
            ++ToPlayer()->m_forced_speed_changes[mtype];
        }

        switch (mtype)
        {
            case MOVE_WALK:
                data.Initialize(SMSG_FORCE_WALK_SPEED_CHANGE, 16);
                break;
            case MOVE_RUN:
                data.Initialize(SMSG_FORCE_RUN_SPEED_CHANGE, 17);
                break;
            case MOVE_RUN_BACK:
                data.Initialize(SMSG_FORCE_RUN_BACK_SPEED_CHANGE, 16);
                break;
            case MOVE_SWIM:
                data.Initialize(SMSG_FORCE_SWIM_SPEED_CHANGE, 16);
                break;
            case MOVE_SWIM_BACK:
                data.Initialize(SMSG_FORCE_SWIM_BACK_SPEED_CHANGE, 16);
                break;
            case MOVE_TURN_RATE:
                data.Initialize(SMSG_FORCE_TURN_RATE_CHANGE, 16);
                break;
            case MOVE_FLIGHT:
                data.Initialize(SMSG_FORCE_FLIGHT_SPEED_CHANGE, 16);
                break;
            case MOVE_FLIGHT_BACK:
                data.Initialize(SMSG_FORCE_FLIGHT_BACK_SPEED_CHANGE, 16);
                break;
            default:
                sLog.outError("Unit::SetSpeed: Unsupported move type (%d), data not sent to client.", mtype);
                return;
        }
        data << GetPackGUID();
        data << (uint32)0;                                  // moveEvent, NUM_PMOVE_EVTS = 0x39
        if (mtype == MOVE_RUN)
            data << uint8(0);                               // new 2.1.0
        data << float(GetSpeed(mtype));
        SendMessageToSet(&data, true);
    }
}

void Unit::setDeathState(DeathState s)
{
    // Death state needs to be updated before RemoveAllAurasOnDeath() is called, to prevent entering combat
    m_deathState = s;

    if (s != ALIVE && s != JUST_RESPAWNED)
    {
        CombatStop();
        DeleteThreatList();
        getHostileRefManager().deleteReferences();
        ClearComboPointHolders();                           // any combo points pointed to unit lost at it death

        if (IsNonMeleeSpellCast(false))
            InterruptNonMeleeSpells(false);

        UnsummonAllTotems();
        RemoveAllControlled();
        RemoveAllAurasOnDeath();
    }

    if (s == JUST_DIED)
    {
        ModifyAuraState(AURA_STATE_HEALTHLESS_20_PERCENT, false);
        ModifyAuraState(AURA_STATE_HEALTHLESS_35_PERCENT, false);
        // remove aurastates allowing special moves
        ClearAllReactives();
        ClearDiminishings();
        if (IsInWorld())
        {
            // Only clear MotionMaster for entities that exists in world
            // Avoids crashes in the following conditions :
            //  * Using 'call pet' on dead pets
            //  * Using 'call stabled pet'
            //  * Logging in with dead pets
            GetMotionMaster()->Clear(false);
            GetMotionMaster()->MoveIdle();
        }

        StopMoving();
        DisableSpline();
        //without this when removing IncreaseMaxHealth aura player may stuck with 1 hp
        //do not why since in IncreaseMaxHealth currenthealth is checked
        SetHealth(0);
        SetPower(getPowerType(), 0);
        SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);
    }
    else if (s == JUST_RESPAWNED)
        RemoveFlag (UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE); // clear skinnable for creature and player (at Battleground)
}

/*########################################
########                          ########
########       AGGRO SYSTEM       ########
########                          ########
########################################*/
bool Unit::CanHaveThreatList() const
{
    // only creatures can have threat list
    if (GetTypeId() != TYPEID_UNIT)
        return false;

    // only alive units can have threat list
    if (!IsAlive() || isDying())
        return false;

    // totems can not have threat list
    if (IsTotem())
        return false;

    // summons can not have a threat list, unless they are controlled by a creature
    if (ToCreature()->HasUnitTypeMask(UNIT_MASK_MINION | UNIT_MASK_GUARDIAN | UNIT_MASK_CONTROLABLE_GUARDIAN) && IS_PLAYER_GUID(((Pet*)this)->GetOwnerGUID()))
        return false;

    // charmed units can not have a threat list if charmed by player
    if (ToCreature()->isCharmed() && IS_PLAYER_GUID(ToCreature()->GetCharmerGUID()))
        return false;

    return true;
}

//======================================================================

float Unit::ApplyTotalThreatModifier(float fThreat, SpellSchoolMask schoolMask)
{
    if (!HasAuraType(SPELL_AURA_MOD_THREAT) || fThreat < 0)
        return fThreat;

    SpellSchools school = GetFirstSchoolInMask(schoolMask);

    return fThreat * m_threatModifier[school];
}

//======================================================================

void Unit::AddThreat(Unit* victim, float fThreat, SpellSchoolMask schoolMask, SpellEntry const* threatSpell)
{
    // Only mobs can manage threat lists
    if (CanHaveThreatList() && !HasUnitState(UNIT_STATE_EVADE))
        m_ThreatManager.addThreat(victim, fThreat, schoolMask, threatSpell);
}

//======================================================================

void Unit::AddAssistThreat(Unit* victim, float threatToAdd, SpellEntry const* threatSpell)
{
    // Only mobs can manage threat lists
    if (CanHaveThreatList())
        getHostileRefManager().threatAssist(victim, threatToAdd, threatSpell);
}

//======================================================================

void Unit::DeleteThreatList()
{
    m_ThreatManager.clearReferences();
}

//======================================================================

void Unit::TauntApply(Unit* taunter)
{
    ASSERT(GetTypeId() == TYPEID_UNIT);

    if (!taunter || (taunter->GetTypeId() == TYPEID_PLAYER && taunter->ToPlayer()->IsGameMaster()))
        return;

    if (!CanHaveThreatList())
        return;

    Creature* creature = ToCreature();

    if (creature->HasReactState(REACT_PASSIVE))
        return;

    Unit* target = GetVictim();
    if (target && target == taunter)
        return;

    SetInFront(taunter);
    if (creature->IsAIEnabled)
        creature->AI()->AttackStart(taunter);

    //m_ThreatManager.tauntApply(taunter);
}

//======================================================================

void Unit::TauntFadeOut(Unit* taunter)
{
    ASSERT(GetTypeId() == TYPEID_UNIT);

    if (!taunter || (taunter->GetTypeId() == TYPEID_PLAYER && taunter->ToPlayer()->IsGameMaster()))
        return;

    if (!CanHaveThreatList())
        return;

    Creature* creature = ToCreature();

    if (creature->HasReactState(REACT_PASSIVE))
        return;

    Unit* target = GetVictim();
    if (!target || target != taunter)
        return;

    if (m_ThreatManager.isThreatListEmpty())
    {
        if (creature->IsAIEnabled)
            creature->AI()->EnterEvadeMode();
        return;
    }

    target = creature->SelectVictim();  // might have more taunt auras remaining

    if (target && target != taunter)
    {
        SetInFront(target);
        if (creature->IsAIEnabled)
            creature->AI()->AttackStart(target);
    }
}

//======================================================================

Unit* Creature::SelectVictim()
{
    //function provides main threat functionality
    //next-victim-selection algorithm and evade mode are called
    //threat list sorting etc.

    //This should not be called by unit who does not have a threatlist
    //or who does not have threat (totem/pet/critter)
    //otherwise enterevademode every update

    Unit* target = nullptr;
    // First checking if we have some taunt on us
    AuraList const& tauntAuras = GetAurasByType(SPELL_AURA_MOD_TAUNT);
    if (!tauntAuras.empty())
    {
        Unit* caster = tauntAuras.back()->GetCaster();

        // The last taunt aura caster is alive an we are happy to attack him
        if (caster && caster->IsAlive())
            return GetVictim();
        else if (tauntAuras.size() > 1)
        {
            // We do not have last taunt aura caster but we have more taunt auras,
            // so find first available target

            // Auras are pushed_back, last caster will be on the end
            AuraList::const_iterator aura = --tauntAuras.end();
            do
            {
                --aura;
                caster = (*aura)->GetCaster();
                if (caster && CanSeeOrDetect(caster, true) && IsValidAttackTarget(caster) && caster->isInAccessiblePlaceFor(ToCreature()))
                {
                    target = caster;
                    break;
                }
            } while (aura != tauntAuras.begin());
        }
        else
            target = GetVictim();
    }

    if (!m_ThreatManager.isThreatListEmpty())
    {
        if (!HasAuraType(SPELL_AURA_MOD_TAUNT))
            target = m_ThreatManager.getHostileTarget();
        else
            target = GetVictim();
    }

    if (target)
    {
        if (!HasUnitState(UNIT_STATE_STUNNED))
        {
            SetInFront(target);

            // check if currently selected target is reachable
            // NOTE: path already generated from AttackStart()
            if (!GetMotionMaster()->isReachable())
            {
                // remove all taunts
                RemoveSpellsCausingAura(SPELL_AURA_MOD_TAUNT);

                if (m_ThreatManager.getThreatList().size() < 2)
                {
                    // only one target in list, we have to evade after timer
                    // TODO: make timer - inside Creature class
                    ((Creature*)this)->AI()->EnterEvadeMode();
                }
                else
                {
                    // remove unreachable target from our threat list
                    // next iteration we will select next possible target
                    getHostileRefManager().deleteReference(target);
                    m_ThreatManager.modifyThreatPercent(target, -101);

                    _removeAttacker(target);
                }
                return NULL;
            }
        }
        return target;
    }

    // Case where mob is being kited.
    // Mob may not be in range to attack or may have dropped target. In any case,
    //  don't evade if damage received within the last 10 seconds
    // Does not apply to world bosses to prevent kiting to cities
    /* @todo: Fix this
    if (!isWorldBoss() && !GetInstanceId())
        if (time(NULL) - GetLastDamagedTime() <= MAX_AGGRO_RESET_TIME)
            return target;*/

    // last case when creature don't must go to evade mode:
    // it in combat but attacker not make any damage and not enter to aggro radius to have record in threat list
    // for example at owner command to pet attack some far away creature
    // Note: creature not have targeted movement generator but have attacker in this case
    /*if (GetMotionMaster()->GetCurrentMovementGeneratorType() != TARGETED_MOTION_TYPE)
    {
        for (AttackerSet::const_iterator itr = m_attackers.begin(); itr != m_attackers.end(); ++itr)
        {
            if ((*itr)->IsInMap(this) && canAttack(*itr) && (*itr)->isInAccessiblePlaceFor(ToCreature()))
                return NULL;
        }
    }*/

    // search nearby enemy before enter evade mode
    if (HasReactState(REACT_AGGRESSIVE))
    {
        target = SelectNearestTargetInAttackDistance(m_CombatDistance ? m_CombatDistance : ATTACK_DISTANCE);

        if (target && _IsTargetAcceptable(target) && CanCreatureAttack(target))
            return target;
    }

    Unit::AuraList const& iAuras = GetAurasByType(SPELL_AURA_MOD_INVISIBILITY);
    if (!iAuras.empty())
    {
        for (Unit::AuraList::const_iterator itr = iAuras.begin(); itr != iAuras.end(); ++itr)
        {
            if ((*itr)->IsPermanent())
            {
                if (m_attackers.size())
                    return NULL;

                AI()->EnterEvadeMode();
                break;
            }
        }
        return NULL;
    }

    // enter in evade mode in other case
    AI()->EnterEvadeMode();

    return NULL;
}

//======================================================================
//======================================================================
//======================================================================

int32 Unit::CalculateSpellDamage(SpellEntry const* spellProto, uint8 effect_index, int32 effBasePoints, Unit const* /*target*/)
{
    Player* unitPlayer = (GetTypeId() == TYPEID_PLAYER) ? ToPlayer() : NULL;

    uint8 comboPoints = unitPlayer ? unitPlayer->GetComboPoints() : 0;

    int32 level = int32(getLevel());
    if (level > (int32)spellProto->maxLevel && spellProto->maxLevel > 0)
        level = (int32)spellProto->maxLevel;
    else if (level < (int32)spellProto->baseLevel)
        level = (int32)spellProto->baseLevel;
    if (!IsPassiveSpell(spellProto))
        level -= int32(spellProto->spellLevel);

    float basePointsPerLevel = spellProto->EffectRealPointsPerLevel[effect_index];
    float randomPointsPerLevel = spellProto->EffectDicePerLevel[effect_index];
    int32 basePoints = int32(effBasePoints + level * basePointsPerLevel);
    int32 randomPoints = int32(spellProto->EffectDieSides[effect_index] + level * randomPointsPerLevel);
    float comboDamage = spellProto->EffectPointsPerComboPoint[effect_index];

    // prevent random generator from getting confused by spells casted with Unit::CastCustomSpell
    int32 randvalue = spellProto->EffectBaseDice[effect_index] >= randomPoints ? spellProto->EffectBaseDice[effect_index] : irand(spellProto->EffectBaseDice[effect_index], randomPoints);
    int32 value = basePoints + randvalue;
    //random damage
    if (comboDamage != 0 && unitPlayer /*&& target && (target->GetGUID() == unitPlayer->GetComboTarget())*/)
        value += (int32)(comboDamage * comboPoints);

    if (Player* modOwner = GetSpellModOwner())
    {
        modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_ALL_EFFECTS, value);
        switch (effect_index)
        {
        case 0:
            modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_EFFECT1, value);
            break;
        case 1:
            modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_EFFECT2, value);
            break;
        case 2:
            modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_EFFECT3, value);
            break;
        }
    }

    if (spellProto->Attributes & SPELL_ATTR0_LEVEL_DAMAGE_CALCULATION && spellProto->spellLevel)
    {
        if (spellProto->EffectApplyAuraName[effect_index])
            value += int32(std::max(0, int32(getLevel() - spellProto->maxLevel)) * basePointsPerLevel);
        else if (spellProto->Effect[effect_index] != SPELL_EFFECT_WEAPON_PERCENT_DAMAGE &&
                 spellProto->Effect[effect_index] != SPELL_EFFECT_KNOCK_BACK &&
                 !IsControlledByPlayer())
        {
            GtNPCManaCostScalerEntry const* spellScaler = sGtNPCManaCostScalerStore.LookupEntry(spellProto->spellLevel - 1);
            GtNPCManaCostScalerEntry const* casterScaler = sGtNPCManaCostScalerStore.LookupEntry(this->getLevel() - 1);
            if (spellScaler && casterScaler)
                value *= casterScaler->ratio / spellScaler->ratio;
        }
    }

    return value;
}

int32 Unit::CalculateSpellDuration(SpellEntry const* spellProto, uint8 effect_index, Unit const* target)
{
    Player* unitPlayer = (GetTypeId() == TYPEID_PLAYER) ? ToPlayer() : NULL;

    uint8 comboPoints = unitPlayer ? unitPlayer->GetComboPoints() : 0;

    int32 minduration = GetSpellDuration(spellProto);
    int32 maxduration = GetSpellMaxDuration(spellProto);

    int32 duration;

    if (comboPoints && minduration != -1 && minduration != maxduration)
        duration = minduration + int32((maxduration - minduration) * comboPoints / 5);
    else
        duration = minduration;

    if (duration > 0)
    {
        int32 mechanic = GetEffectMechanic(spellProto, effect_index);
        // Find total mod value (negative bonus)
        int32 durationMod_always = target->GetTotalAuraModifierByMiscValue(SPELL_AURA_MECHANIC_DURATION_MOD, mechanic);
        // Find max mod (negative bonus)
        int32 durationMod_not_stack = target->GetMaxNegativeAuraModifierByMiscValue(SPELL_AURA_MECHANIC_DURATION_MOD_NOT_STACK, mechanic);

        int32 durationMod = 0;
        // Select strongest negative mod
        if (durationMod_always > durationMod_not_stack)
            durationMod = durationMod_not_stack;
        else
            durationMod = durationMod_always;

        if (durationMod != 0)
            duration = int32(int64(duration) * (100 + durationMod) / 100);

        if (duration < 0) duration = 0;
    }

    return duration;
}

DiminishingLevels Unit::GetDiminishing(DiminishingGroup group)
{
    for (Diminishing::iterator i = m_Diminishing.begin(); i != m_Diminishing.end(); ++i)
    {
        if (i->DRGroup != group)
            continue;

        if (!i->hitCount)
            return DIMINISHING_LEVEL_1;

        if (!i->hitTime)
            return DIMINISHING_LEVEL_1;

        // If last spell was casted more than 15 seconds ago - reset the count.
        if (i->stack == 0 && getMSTimeDiff(i->hitTime, getMSTime()) > 15000)
        {
            i->hitCount = 0;
            i->hitTime = 0;
            return DIMINISHING_LEVEL_1;
        }
        // or else increase the count.
        else
            return DiminishingLevels(i->hitCount);
    }
    return DIMINISHING_LEVEL_1;
}

void Unit::IncrDiminishing(DiminishingGroup group)
{
    // Checking for existing in the table
    bool IsExist = false;
    for (Diminishing::iterator i = m_Diminishing.begin(); i != m_Diminishing.end(); ++i)
    {
        if (i->DRGroup != group)
            continue;

        IsExist = true;
        if (i->hitCount < DIMINISHING_LEVEL_IMMUNE)
            i->hitCount += 1;

        break;
    }

    if (!IsExist)
        m_Diminishing.push_back(DiminishingReturn(group, getMSTime(), DIMINISHING_LEVEL_2));
}

void Unit::ApplyDiminishingToDuration(DiminishingGroup group, int32& duration, Unit* caster, DiminishingLevels Level)
{
    if (duration == -1 || group == DIMINISHING_NONE)
        return;

    // Creatures other then pets do not suffer from Diminishing Returns
    if (caster->GetTypeId() == TYPEID_UNIT && !caster->GetOwner())
        return;

    // Test pet/charm masters instead pets/charmedsz
    Unit const* targetOwner = GetCharmerOrOwner();
    Unit const* casterOwner = caster->GetCharmerOrOwner();

    // Duration of crowd control abilities on pvp target is limited by 10 sec. (2.2.0)
    if (duration > 10000 && IsDiminishingReturnsGroupDurationLimited(group))
    {
        Unit const* target = targetOwner ? targetOwner : this;
        Unit const* source = casterOwner ? casterOwner : caster;

        if (target->GetTypeId() == TYPEID_PLAYER && source->GetTypeId() == TYPEID_PLAYER)
            if (source != target || GetTypeId() != TYPEID_UNIT)
                duration = 10000;
    }

    float mod = 1.0f;

    // Some diminishings applies to mobs too (for example, Stun)
    if ((GetDiminishingReturnsGroupType(group) == DRTYPE_PLAYER && (targetOwner ? targetOwner->GetTypeId() : GetTypeId()) == TYPEID_PLAYER) || GetDiminishingReturnsGroupType(group) == DRTYPE_ALL)
    {
        DiminishingLevels diminish = Level;
        switch (diminish)
        {
        case DIMINISHING_LEVEL_1:
            break;
        case DIMINISHING_LEVEL_2:
            mod = 0.5f;
            break;
        case DIMINISHING_LEVEL_3:
            mod = 0.25f;
            break;
        case DIMINISHING_LEVEL_IMMUNE:
            mod = 0.0f;
            break;
        default:
            break;
        }
    }

    duration = int32(duration * mod);
}

void Unit::ApplyDiminishingAura(DiminishingGroup group, bool apply)
{
    // Checking for existing in the table
    for (Diminishing::iterator i = m_Diminishing.begin(); i != m_Diminishing.end(); ++i)
    {
        if (i->DRGroup != group)
            continue;

        i->hitTime = getMSTime();

        if (apply)
            i->stack += 1;
        else if (i->stack)
            i->stack -= 1;

        break;
    }
}

uint32 Unit::GetSpellRadiusForTarget(Unit* target, const SpellRadiusEntry* radiusEntry)
{
    if (!radiusEntry)
        return 0;
    if (radiusEntry->radiusHostile == radiusEntry->radiusFriend)
        return radiusEntry->radiusFriend;
    if (IsHostileTo(target))
        return radiusEntry->radiusHostile;
    return radiusEntry->radiusFriend;
};

Unit* Unit::GetUnit(WorldObject& object, uint64 guid)
{
    return ObjectAccessor::GetUnit(object, guid);
}

Player* Unit::GetPlayer(WorldObject& object, uint64 guid)
{
    return ObjectAccessor::GetPlayer(object, guid);
}

Creature* Unit::GetCreature(WorldObject& object, uint64 guid)
{
    return object.GetMap()->GetCreature(guid);
}

uint32 Unit::GetCreatureType() const
{
    if (GetTypeId() == TYPEID_PLAYER)
    {
        SpellShapeshiftEntry const* ssEntry = sSpellShapeshiftStore.LookupEntry(ToPlayer()->m_form);
        if (ssEntry && ssEntry->creatureType > 0)
            return ssEntry->creatureType;
        else
            return CREATURE_TYPE_HUMANOID;
    }
    else
        return ToCreature()->GetCreatureTemplate()->type;
}

/*#######################################
########                         ########
########       STAT SYSTEM       ########
########                         ########
#######################################*/

bool Unit::HandleStatModifier(UnitMods unitMod, UnitModifierType modifierType, float amount, bool apply)
{
    if (unitMod >= UNIT_MOD_END || modifierType >= MODIFIER_TYPE_END)
    {
        sLog.outError("ERROR in HandleStatModifier(): Invalid UnitMods or invalid UnitModifierType!");
        return false;
    }

    switch (modifierType)
    {
        case BASE_VALUE:
        case TOTAL_VALUE:
            m_auraModifiersGroup[unitMod][modifierType] += apply ? amount : -amount;
            break;
        case BASE_PCT:
        case TOTAL_PCT:
            ApplyPercentModFloatVar(m_auraModifiersGroup[unitMod][modifierType], amount, apply);
            break;
        default:
            break;
    }

    if (!CanModifyStats())
        return false;

    switch (unitMod)
    {
        case UNIT_MOD_STAT_STRENGTH:
        case UNIT_MOD_STAT_AGILITY:
        case UNIT_MOD_STAT_STAMINA:
        case UNIT_MOD_STAT_INTELLECT:
        case UNIT_MOD_STAT_SPIRIT:         UpdateStats(GetStatByAuraGroup(unitMod));  break;

        case UNIT_MOD_ARMOR:               UpdateArmor();           break;
        case UNIT_MOD_HEALTH:              UpdateMaxHealth();       break;

        case UNIT_MOD_MANA:
        case UNIT_MOD_RAGE:
        case UNIT_MOD_FOCUS:
        case UNIT_MOD_ENERGY:
        case UNIT_MOD_HAPPINESS:           UpdateMaxPower(GetPowerTypeByAuraGroup(unitMod));          break;

        case UNIT_MOD_RESISTANCE_HOLY:
        case UNIT_MOD_RESISTANCE_FIRE:
        case UNIT_MOD_RESISTANCE_NATURE:
        case UNIT_MOD_RESISTANCE_FROST:
        case UNIT_MOD_RESISTANCE_SHADOW:
        case UNIT_MOD_RESISTANCE_ARCANE:   UpdateResistances(GetSpellSchoolByAuraGroup(unitMod));      break;

        case UNIT_MOD_ATTACK_POWER:        UpdateAttackPowerAndDamage();         break;
        case UNIT_MOD_ATTACK_POWER_RANGED: UpdateAttackPowerAndDamage(true);     break;

        case UNIT_MOD_DAMAGE_MAINHAND:     UpdateDamagePhysical(BASE_ATTACK);    break;
        case UNIT_MOD_DAMAGE_OFFHAND:      UpdateDamagePhysical(OFF_ATTACK);     break;
        case UNIT_MOD_DAMAGE_RANGED:       UpdateDamagePhysical(RANGED_ATTACK);  break;

        default:
            break;
    }

    return true;
}

float Unit::GetModifierValue(UnitMods unitMod, UnitModifierType modifierType) const
{
    if (unitMod >= UNIT_MOD_END || modifierType >= MODIFIER_TYPE_END)
    {
        sLog.outError("GetModifierValue: Invalid modifier value from UnitMods!");
        return 0.0f;
    }

    if (modifierType == TOTAL_PCT && m_auraModifiersGroup[unitMod][modifierType] <= 0.0f)
        return 0.0f;

    return m_auraModifiersGroup[unitMod][modifierType];
}

float Unit::GetTotalStatValue(Stats stat) const
{
    UnitMods unitMod = UnitMods(UNIT_MOD_STAT_START + stat);

    if (m_auraModifiersGroup[unitMod][TOTAL_PCT] <= 0.0f)
        return 0.0f;

    // value = ((base_value * base_pct) + total_value) * total_pct
    float value  = m_auraModifiersGroup[unitMod][BASE_VALUE] + GetCreateStat(stat);
    value *= m_auraModifiersGroup[unitMod][BASE_PCT];
    value += m_auraModifiersGroup[unitMod][TOTAL_VALUE];
    value *= m_auraModifiersGroup[unitMod][TOTAL_PCT];

    return value;
}

float Unit::GetTotalAuraModValue(UnitMods unitMod) const
{
    if (unitMod >= UNIT_MOD_END)
    {
        sLog.outError("Invalid UnitMods in GetTotalAuraModValue()!");
        return 0.0f;
    }

    if (m_auraModifiersGroup[unitMod][TOTAL_PCT] <= 0.0f)
        return 0.0f;

    float value = m_auraModifiersGroup[unitMod][BASE_VALUE];
    value *= m_auraModifiersGroup[unitMod][BASE_PCT];
    value += m_auraModifiersGroup[unitMod][TOTAL_VALUE];
    value *= m_auraModifiersGroup[unitMod][TOTAL_PCT];

    return value;
}

SpellSchools Unit::GetSpellSchoolByAuraGroup(UnitMods unitMod) const
{
    SpellSchools school = SPELL_SCHOOL_NORMAL;

    switch (unitMod)
    {
        case UNIT_MOD_RESISTANCE_HOLY:     school = SPELL_SCHOOL_HOLY;          break;
        case UNIT_MOD_RESISTANCE_FIRE:     school = SPELL_SCHOOL_FIRE;          break;
        case UNIT_MOD_RESISTANCE_NATURE:   school = SPELL_SCHOOL_NATURE;        break;
        case UNIT_MOD_RESISTANCE_FROST:    school = SPELL_SCHOOL_FROST;         break;
        case UNIT_MOD_RESISTANCE_SHADOW:   school = SPELL_SCHOOL_SHADOW;        break;
        case UNIT_MOD_RESISTANCE_ARCANE:   school = SPELL_SCHOOL_ARCANE;        break;

        default:
            break;
    }

    return school;
}

Stats Unit::GetStatByAuraGroup(UnitMods unitMod) const
{
    Stats stat = STAT_STRENGTH;

    switch (unitMod)
    {
        case UNIT_MOD_STAT_STRENGTH:    stat = STAT_STRENGTH;      break;
        case UNIT_MOD_STAT_AGILITY:     stat = STAT_AGILITY;       break;
        case UNIT_MOD_STAT_STAMINA:     stat = STAT_STAMINA;       break;
        case UNIT_MOD_STAT_INTELLECT:   stat = STAT_INTELLECT;     break;
        case UNIT_MOD_STAT_SPIRIT:      stat = STAT_SPIRIT;        break;

        default:
            break;
    }

    return stat;
}

Powers Unit::GetPowerTypeByAuraGroup(UnitMods unitMod) const
{
    switch (unitMod)
    {
        case UNIT_MOD_RAGE:        return POWER_RAGE;
        case UNIT_MOD_FOCUS:       return POWER_FOCUS;
        case UNIT_MOD_ENERGY:      return POWER_ENERGY;
        case UNIT_MOD_HAPPINESS:   return POWER_HAPPINESS;
        default:
        case UNIT_MOD_MANA:        return POWER_MANA;
    }
}

float Unit::GetTotalAttackPowerValue(WeaponAttackType attType) const
{
    if (attType == RANGED_ATTACK)
    {
        int32 ap = GetInt32Value(UNIT_FIELD_RANGED_ATTACK_POWER) + GetInt32Value(UNIT_FIELD_RANGED_ATTACK_POWER_MODS);
        if (ap < 0)
            return 0.0f;
        return ap * (1.0f + GetFloatValue(UNIT_FIELD_RANGED_ATTACK_POWER_MULTIPLIER));
    }
    else
    {
        int32 ap = GetInt32Value(UNIT_FIELD_ATTACK_POWER) + GetInt32Value(UNIT_FIELD_ATTACK_POWER_MODS);
        if (ap < 0)
            return 0.0f;
        return ap * (1.0f + GetFloatValue(UNIT_FIELD_ATTACK_POWER_MULTIPLIER));
    }
}

float Unit::GetWeaponDamageRange(WeaponAttackType attType , WeaponDamageRange type) const
{
    if (attType == OFF_ATTACK && !haveOffhandWeapon())
        return 0.0f;

    return m_weaponDamage[attType][type];
}

void Unit::SetLevel(uint32 lvl)
{
    SetUInt32Value(UNIT_FIELD_LEVEL, lvl);

    // group update
    if ((GetTypeId() == TYPEID_PLAYER) && ToPlayer()->GetGroup())
        ToPlayer()->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_LEVEL);
}

void Unit::SetHealth(uint32 val)
{
    if (getDeathState() == JUST_DIED)
        val = 0;
    else if (GetTypeId() == TYPEID_PLAYER && getDeathState() == DEAD)
        val = 1;
    else
    {
        uint32 maxHealth = GetMaxHealth();
        if (maxHealth < val)
            val = maxHealth;
    }

    SetUInt32Value(UNIT_FIELD_HEALTH, val);

    // group update
    if (GetTypeId() == TYPEID_PLAYER)
    {
        if (ToPlayer()->GetGroup())
            ToPlayer()->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_CUR_HP);
    }
    else if (IsPet())
    {
        Pet* pet = ((Pet*)this);
        if (pet->isControlled())
        {
            Unit* owner = GetOwner();
            if (owner && (owner->GetTypeId() == TYPEID_PLAYER) && owner->ToPlayer()->GetGroup())
                owner->ToPlayer()->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_PET_CUR_HP);
        }
    }
}

void Unit::SetMaxHealth(uint32 val)
{
    float healthPct = GetMaxHealth();
    SetUInt32Value(UNIT_FIELD_MAXHEALTH, val);

    // group update
    if (GetTypeId() == TYPEID_PLAYER)
    {
        if (ToPlayer()->GetGroup())
            ToPlayer()->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_MAX_HP);
    }
    else if (IsPet())
    {
        Pet* pet = ((Pet*)this);
        if (pet->isControlled())
        {
            Unit* owner = GetOwner();
            if (owner && (owner->GetTypeId() == TYPEID_PLAYER) && owner->ToPlayer()->GetGroup())
                owner->ToPlayer()->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_PET_MAX_HP);
        }
    }

    if (healthPct) // prevent dividing by zero
        SetHealth(val * (float(GetHealth()) / healthPct));
    else
        SetHealth(0);
}

bool Unit::ShouldRevealHealthTo(Player* player) const
{
    if (!sWorld.getConfig(CONFIG_HEALTH_IN_PERCENTS))
        return true;

    if (player == this || player->IsGameMaster())
        return true;

    if (player->IsInRaidWith(this))
        return true;

    return false;
}

/**
 * Handler for charm/uncharm events,
 * properly converts health from values to percentages
 * and vice versa, and notifies all seers about it.
 */
void Unit::SendHealthUpdateDueToCharm(Player* charmer)
{
    ForceValuesUpdateAtIndex(UNIT_FIELD_HEALTH);
    ForceValuesUpdateAtIndex(UNIT_FIELD_MAXHEALTH);

    if (Group* group = charmer->GetGroup())
    {
        charmer->SetGroupUpdateFlag(GROUP_UPDATE_PET);
        group->UpdatePlayerOutOfRange(charmer);
    }
}

void Unit::SetPower(Powers power, uint32 val)
{
    if (GetPower(power) == val)
        return;

    uint32 maxPower = GetMaxPower(power);
    if (maxPower < val)
        val = maxPower;

    SetStatInt32Value(UNIT_FIELD_POWER1 + power, val);

    // group update
    if (GetTypeId() == TYPEID_PLAYER)
    {
        if (ToPlayer()->GetGroup())
            ToPlayer()->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_CUR_POWER);
    }
    else if (IsPet())
    {
        Pet* pet = ((Pet*)this);
        if (pet->isControlled())
        {
            Unit* owner = GetOwner();
            if (owner && (owner->GetTypeId() == TYPEID_PLAYER) && owner->ToPlayer()->GetGroup())
                owner->ToPlayer()->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_PET_CUR_POWER);
        }

        // Update the pet's character sheet with happiness damage bonus
        if (pet->getPetType() == HUNTER_PET && power == POWER_HAPPINESS)
            pet->UpdateDamagePhysical(BASE_ATTACK);
    }
}

void Unit::SetMaxPower(Powers power, uint32 val)
{
    float powerPct = GetMaxPower(power);
    SetStatInt32Value(UNIT_FIELD_MAXPOWER1 + power, val);

    // group update
    if (GetTypeId() == TYPEID_PLAYER)
    {
        if (ToPlayer()->GetGroup())
            ToPlayer()->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_MAX_POWER);
    }
    else if (IsPet())
    {
        Pet* pet = ((Pet*)this);
        if (pet->isControlled())
        {
            Unit* owner = GetOwner();
            if (owner && (owner->GetTypeId() == TYPEID_PLAYER) && owner->ToPlayer()->GetGroup())
                owner->ToPlayer()->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_PET_MAX_POWER);
        }
    }

    if (powerPct)
        SetPower(power, val * (float(GetPower(power)) / powerPct));
    else
        SetPower(power, 0);
}

void Unit::ApplyPowerMod(Powers power, uint32 val, bool apply)
{
    ApplyModUInt32Value(UNIT_FIELD_POWER1 + power, val, apply);

    // group update
    if (GetTypeId() == TYPEID_PLAYER)
    {
        if (ToPlayer()->GetGroup())
            ToPlayer()->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_CUR_POWER);
    }
    else if (IsPet())
    {
        Pet* pet = ((Pet*)this);
        if (pet->isControlled())
        {
            Unit* owner = GetOwner();
            if (owner && (owner->GetTypeId() == TYPEID_PLAYER) && owner->ToPlayer()->GetGroup())
                owner->ToPlayer()->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_PET_CUR_POWER);
        }
    }
}

void Unit::ApplyMaxPowerMod(Powers power, uint32 val, bool apply)
{
    ApplyModUInt32Value(UNIT_FIELD_MAXPOWER1 + power, val, apply);

    // group update
    if (GetTypeId() == TYPEID_PLAYER)
    {
        if (ToPlayer()->GetGroup())
            ToPlayer()->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_MAX_POWER);
    }
    else if (IsPet())
    {
        Pet* pet = ((Pet*)this);
        if (pet->isControlled())
        {
            Unit* owner = GetOwner();
            if (owner && (owner->GetTypeId() == TYPEID_PLAYER) && owner->ToPlayer()->GetGroup())
                owner->ToPlayer()->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_PET_MAX_POWER);
        }
    }
}

void Unit::ApplyAuraProcTriggerDamage(Aura* aura, bool apply)
{
    AuraList& tAuraProcTriggerDamage = m_modAuras[SPELL_AURA_PROC_TRIGGER_DAMAGE];
    if (apply)
        tAuraProcTriggerDamage.push_back(aura);
    else
        tAuraProcTriggerDamage.remove(aura);
}

uint32 Unit::GetCreatePowers(Powers power) const
{
    // POWER_FOCUS and POWER_HAPPINESS only have hunter pet
    switch (power)
    {
        case POWER_MANA:      return GetCreateMana();
        case POWER_RAGE:      return 1000;
        case POWER_FOCUS:     return (GetTypeId() == TYPEID_PLAYER || !((Creature const*)this)->IsPet() || ((Pet const*)this)->getPetType() != HUNTER_PET ? 0 : 100);
        case POWER_ENERGY:    return 100;
        case POWER_HAPPINESS: return (GetTypeId() == TYPEID_PLAYER || !((Creature const*)this)->IsPet() || ((Pet const*)this)->getPetType() != HUNTER_PET ? 0 : 1050000);
        case POWER_HEALTH:    return 0;
        default:
            break;
    }

    return 0;
}

void Unit::AddToWorld()
{
    if (!IsInWorld())
        WorldObject::AddToWorld();
}

void Unit::RemoveFromWorld()
{
    // cleanup
    ASSERT(GetGUID());

    if (IsInWorld())
    {
        m_duringRemoveFromWorld = true;

        RemoveCharmAuras();
        RemoveBindSightAuras();
        RemoveNotOwnSingleTargetAuras();

        RemoveAllGameObjects();
        RemoveAllDynObjects();

        UnsummonAllTotems();
        RemoveAllControlled();

        if (isCharmed())
            RemoveCharmedBy(nullptr);

        if (GetCharmerGUID())
            sLog.outFatal("Crash alert! Unit %u has charmer guid when removed from world", GetEntry());

        WorldObject::RemoveFromWorld();
        m_duringRemoveFromWorld = false;
    }
}

void Unit::CleanupsBeforeDelete()
{
    if (!IsInWorld())
    return;

    // This needs to be before RemoveFromWorld to make GetCaster() return a valid pointer on aura removal
    InterruptNonMeleeSpells(true);

    RemoveFromWorld();

    ASSERT(GetGUID());

    // A unit may be in removelist and not in world, but it is still in grid
    // and may have some references during delete
    RemoveAllAuras();
    RemoveAllGameObjects();

    m_Events.KillAllEvents(false);                      // non-delatable (currently casted spells) will not deleted now but it will deleted at call in Map::RemoveAllObjectsInRemoveList
    CombatStop();
    ClearComboPointHolders();
    if (CanHaveThreatList())
         DeleteThreatList();
    if (GetTypeId() == TYPEID_PLAYER)
        getHostileRefManager().setOnlineOfflineState(false);
    else
        getHostileRefManager().deleteReferences();
    GetMotionMaster()->Clear(false);                    // remove different non-standard movement generators.
}

void Unit::UpdateCharmAI()
{
    if (GetTypeId() == TYPEID_PLAYER)
        return;

    if (i_disabledAI) // disabled AI must be primary AI
    {
        if (!isCharmed())
        {
            delete i_AI;
            i_AI = i_disabledAI;
            i_disabledAI = NULL;
        }
    }
    else
    {
        if (isCharmed())
        {
            i_disabledAI = i_AI;
            if (isPossessed())
                i_AI = new PossessedAI(ToCreature());
            else
                i_AI = new PetAI(ToCreature());
        }
    }
}

CharmInfo* Unit::InitCharmInfo()
{
    if (!m_charmInfo)
        m_charmInfo = new CharmInfo(this);

    return m_charmInfo;
}

void Unit::DeleteCharmInfo()
{
    delete m_charmInfo;
    m_charmInfo = NULL;
}

CharmInfo::CharmInfo(Unit* unit)
    : m_unit(unit), m_CommandState(COMMAND_FOLLOW), m_petnumber(0), m_barInit(false),
    m_isCommandAttack(false), _isCommandFollow(false), m_isAtStay(false), m_isFollowing(false), m_isReturning(false),
    m_stayX(0.0f), m_stayY(0.0f), m_stayZ(0.0f)
{
    for (int i = 0; i < MAX_SPELL_CHARM; ++i)
    {
        m_charmspells[i].spellId = 0;
        m_charmspells[i].active = ACT_DISABLED;
    }

    if (m_unit->GetTypeId() == TYPEID_UNIT)
    {
        m_oldReactState = m_unit->ToCreature()->GetReactState();
        m_unit->ToCreature()->SetReactState(REACT_PASSIVE);
    }
}

CharmInfo::~CharmInfo()
{
    if (Creature* pCreature = m_unit->ToCreature())
        pCreature->SetReactState(m_oldReactState);
}

void CharmInfo::InitPetActionBar()
{
    if (m_barInit)
        return;

    // the first 3 SpellOrActions are attack, follow and stay
    for (uint32 i = 0; i < 3; i++)
    {
        PetActionBar[i].Type = ACT_COMMAND;
        PetActionBar[i].SpellOrAction = COMMAND_ATTACK - i;

        PetActionBar[i + 7].Type = ACT_REACTION;
        PetActionBar[i + 7].SpellOrAction = COMMAND_ATTACK - i;
    }
    for (uint32 i = 0; i < 4; i++)
    {
        PetActionBar[i + 3].Type = ACT_DISABLED;
        PetActionBar[i + 3].SpellOrAction = 0;
    }
    m_barInit = true;
}

void CharmInfo::InitEmptyActionBar(bool withAttack)
{
    if (m_barInit)
        return;

    for (uint32 x = 0; x < 10; ++x)
    {
        PetActionBar[x].Type = ACT_CAST;
        PetActionBar[x].SpellOrAction = 0;
    }
    if (withAttack)
    {
        PetActionBar[0].Type = ACT_COMMAND;
        PetActionBar[0].SpellOrAction = COMMAND_ATTACK;
    }
    m_barInit = true;
}

void CharmInfo::InitPossessCreateSpells(bool withAttack)
{
    InitEmptyActionBar(withAttack);

    if (m_unit->GetTypeId() == TYPEID_UNIT)
    {
        for (uint32 i = 0; i < CREATURE_MAX_SPELLS; ++i)
        {
            uint32 spellid = m_unit->ToCreature()->m_spells[i];
            if (IsPassiveSpell(spellid))
                m_unit->CastSpell(m_unit, spellid, true);
            else
                AddSpellToActionBar(0, spellid, ACT_CAST);
        }
    }
}

void CharmInfo::InitCharmCreateSpells()
{
    if (m_unit->GetTypeId() == TYPEID_PLAYER)                //charmed players don't have spells
    {
        InitEmptyActionBar();
        return;
    }

    InitPetActionBar();

    for (uint32 x = 0; x < MAX_SPELL_CHARM; ++x)
    {
        uint32 spellId = m_unit->ToCreature()->m_spells[x];
        m_charmspells[x].spellId = spellId;

        if (!spellId)
            continue;

        if (IsPassiveSpell(spellId))
        {
            m_unit->CastSpell(m_unit, spellId, true);
            m_charmspells[x].active = ACT_PASSIVE;
        }
        else
        {
            ActiveStates newstate;
            bool onlyselfcast = true;
            SpellEntry const* spellInfo = sSpellStore.LookupEntry(spellId);

            if (!spellInfo) onlyselfcast = false;
            for (uint32 i = 0; i < 3 && onlyselfcast; ++i)   //non existent spell will not make any problems as onlyselfcast would be false -> break right away
            {
                if (spellInfo->EffectImplicitTargetA[i] != TARGET_UNIT_CASTER && spellInfo->EffectImplicitTargetA[i] != 0)
                    onlyselfcast = false;
            }

            if (onlyselfcast || !IsPositiveSpell(spellId))   //only self cast and spells versus enemies are autocastable
                newstate = ACT_DISABLED;
            else
                newstate = ACT_CAST;

            AddSpellToActionBar(0, spellId, newstate);
        }
    }
}

bool CharmInfo::AddSpellToActionBar(uint32 oldid, uint32 newid, ActiveStates newstate)
{
    for (uint8 i = 0; i < 10; i++)
    {
        if ((PetActionBar[i].Type == ACT_DISABLED || PetActionBar[i].Type == ACT_ENABLED || PetActionBar[i].Type == ACT_CAST) && PetActionBar[i].SpellOrAction == oldid)
        {
            PetActionBar[i].SpellOrAction = newid;
            if (!oldid)
            {
                if (newstate == ACT_DECIDE)
                    PetActionBar[i].Type = ACT_DISABLED;
                else
                    PetActionBar[i].Type = newstate;
            }

            return true;
        }
    }
    return false;
}

void CharmInfo::ToggleCreatureAutocast(uint32 spellid, bool apply)
{
    if (IsPassiveSpell(spellid))
        return;

    for (uint32 x = 0; x < MAX_SPELL_CHARM; ++x)
        if (spellid == m_charmspells[x].spellId)
            m_charmspells[x].active = apply ? ACT_ENABLED : ACT_DISABLED;
}

void CharmInfo::SetPetNumber(uint32 petnumber, bool statwindow)
{
    m_petnumber = petnumber;
    if (statwindow)
        m_unit->SetUInt32Value(UNIT_FIELD_PETNUMBER, m_petnumber);
    else
        m_unit->SetUInt32Value(UNIT_FIELD_PETNUMBER, 0);
}

bool Unit::isFrozen() const
{
    AuraList const& mRoot = GetAurasByType(SPELL_AURA_MOD_ROOT);
    for (AuraList::const_iterator i = mRoot.begin(); i != mRoot.end(); ++i)
        if (GetSpellSchoolMask((*i)->GetSpellProto()) & SPELL_SCHOOL_MASK_FROST)
            return true;
    return false;
}

struct ProcTriggeredData
{
    ProcTriggeredData(SpellProcEventEntry const* _spellProcEvent, Aura* _triggeredByAura)
        : spellProcEvent(_spellProcEvent), triggeredByAura(_triggeredByAura),
          triggeredByAura_SpellPair(Unit::spellEffectPair(triggeredByAura->GetId(), triggeredByAura->GetEffIndex()))
    {}
    SpellProcEventEntry const* spellProcEvent;
    Aura* triggeredByAura;
    Unit::spellEffectPair triggeredByAura_SpellPair;
};

typedef std::list< ProcTriggeredData > ProcTriggeredList;
typedef std::list< uint32> RemoveSpellList;

// List of auras that CAN be trigger but may not exist in spell_proc_event
// in most case need for drop charges
// in some types of aura need do additional check
// for example SPELL_AURA_MECHANIC_IMMUNITY - need check for mechanic
static bool isTriggerAura[TOTAL_AURAS];
static bool isNonTriggerAura[TOTAL_AURAS];
void InitTriggerAuraData()
{
    for (uint16 i = 0; i < TOTAL_AURAS; ++i)
    {
        isTriggerAura[i] = false;
        isNonTriggerAura[i] = false;
    }
    isTriggerAura[SPELL_AURA_DUMMY] = true;
    isTriggerAura[SPELL_AURA_MOD_CONFUSE] = true;
    isTriggerAura[SPELL_AURA_MOD_THREAT] = true;
    isTriggerAura[SPELL_AURA_MOD_STUN] = true; // Aura not have charges but need remove him on trigger
    isTriggerAura[SPELL_AURA_MOD_DAMAGE_DONE] = true;
    isTriggerAura[SPELL_AURA_MOD_DAMAGE_TAKEN] = true;
    isTriggerAura[SPELL_AURA_MOD_RESISTANCE] = true;
    isTriggerAura[SPELL_AURA_MOD_STEALTH] = true; // Aura not have charges but need remove him on trigger
    isTriggerAura[SPELL_AURA_MOD_ROOT] = true;
    isTriggerAura[SPELL_AURA_REFLECT_SPELLS] = true;
    isTriggerAura[SPELL_AURA_DAMAGE_IMMUNITY] = true;
    isTriggerAura[SPELL_AURA_PROC_TRIGGER_SPELL] = true;
    isTriggerAura[SPELL_AURA_PROC_TRIGGER_DAMAGE] = true;
    isTriggerAura[SPELL_AURA_MOD_CASTING_SPEED] = true;
    isTriggerAura[SPELL_AURA_MOD_POWER_COST_SCHOOL_PCT] = true;
    isTriggerAura[SPELL_AURA_MOD_POWER_COST_SCHOOL] = true;
    isTriggerAura[SPELL_AURA_REFLECT_SPELLS_SCHOOL] = true;
    isTriggerAura[SPELL_AURA_MECHANIC_IMMUNITY] = true;
    isTriggerAura[SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN] = true;
    isTriggerAura[SPELL_AURA_SPELL_MAGNET] = true;
    isTriggerAura[SPELL_AURA_MOD_ATTACK_POWER] = true;
    isTriggerAura[SPELL_AURA_ADD_CASTER_HIT_TRIGGER] = true;
    isTriggerAura[SPELL_AURA_OVERRIDE_CLASS_SCRIPTS] = true;
    isTriggerAura[SPELL_AURA_MOD_MECHANIC_RESISTANCE] = true;
    isTriggerAura[SPELL_AURA_RANGED_ATTACK_POWER_ATTACKER_BONUS] = true;
    isTriggerAura[SPELL_AURA_MOD_HASTE] = true;
    isTriggerAura[SPELL_AURA_MOD_ATTACKER_MELEE_HIT_CHANCE] = true;
    isTriggerAura[SPELL_AURA_PRAYER_OF_MENDING] = true;
    isTriggerAura[SPELL_AURA_PROC_TRIGGER_SPELL_WITH_VALUE] = true;

    isNonTriggerAura[SPELL_AURA_MOD_POWER_REGEN] = true;
    isNonTriggerAura[SPELL_AURA_RESIST_PUSHBACK] = true;
}

uint32 createProcExtendMask(SpellNonMeleeDamage* damageInfo, SpellMissInfo missCondition)
{
    uint32 procEx = PROC_EX_NONE;
    // Check victim state
    if (missCondition != SPELL_MISS_NONE)
        switch (missCondition)
        {
        case SPELL_MISS_MISS:
            procEx |= PROC_EX_MISS;
            break;
        case SPELL_MISS_RESIST:
            procEx |= PROC_EX_RESIST;
            break;
        case SPELL_MISS_DODGE:
            procEx |= PROC_EX_DODGE;
            break;
        case SPELL_MISS_PARRY:
            procEx |= PROC_EX_PARRY;
            break;
        case SPELL_MISS_BLOCK:
            procEx |= PROC_EX_BLOCK;
            break;
        case SPELL_MISS_EVADE:
            procEx |= PROC_EX_EVADE;
            break;
        case SPELL_MISS_IMMUNE:
            procEx |= PROC_EX_IMMUNE;
            break;
        case SPELL_MISS_IMMUNE2:
            procEx |= PROC_EX_IMMUNE;
            break;
        case SPELL_MISS_DEFLECT:
            procEx |= PROC_EX_DEFLECT;
            break;
        case SPELL_MISS_ABSORB:
            procEx |= PROC_EX_ABSORB;
            break;
        case SPELL_MISS_REFLECT:
            procEx |= PROC_EX_REFLECT;
            break;
        default:
            break;
        }
    else
    {
        // On block
        if (damageInfo->blocked)
            procEx |= PROC_EX_BLOCK;
        // On absorb
        if (damageInfo->absorb)
            procEx |= PROC_EX_ABSORB;
        // On crit
        if (damageInfo->HitInfo & SPELL_HIT_TYPE_CRIT)
            procEx |= PROC_EX_CRITICAL_HIT;
        else
            procEx |= PROC_EX_NORMAL_HIT;
    }
    return procEx;
}

void Unit::ProcDamageAndSpellFor(bool isVictim, Unit* pTarget, uint32 procFlag, uint32 procExtra, WeaponAttackType attType, SpellEntry const* procSpell, uint32 damage)
{
    ++m_procDeep;
    if (m_procDeep > 5)
    {
        sLog.outError("Prevent possible stack owerflow in Unit::ProcDamageAndSpellFor");
        if (procSpell)
            sLog.outError("  Spell %u", procSpell->Id);
        --m_procDeep;
        return;
    }
    // For melee/ranged based attack need update skills and set some Aura states
    if (procFlag & MELEE_BASED_TRIGGER_MASK && pTarget)
    {
        // Update skills here for players
        if (GetTypeId() == TYPEID_PLAYER)
        {
            // On melee based hit/miss/resist need update skill (for victim and attacker)
            if (procExtra & (PROC_EX_NORMAL_HIT | PROC_EX_MISS | PROC_EX_RESIST))
            {
                if (pTarget->GetTypeId() != TYPEID_PLAYER && !pTarget->IsCritter())
                    ToPlayer()->UpdateCombatSkills(pTarget, attType, MELEE_HIT_MISS, isVictim);
            }
            // Update defence if player is victim and parry/dodge/block
            else if (isVictim && procExtra & (PROC_EX_DODGE | PROC_EX_PARRY | PROC_EX_BLOCK))
                ToPlayer()->UpdateDefense();
        }
        // If exist crit/parry/dodge/block need update aura state (for victim and attacker)
        if (procExtra & (PROC_EX_CRITICAL_HIT | PROC_EX_PARRY | PROC_EX_DODGE | PROC_EX_BLOCK))
        {
            // for victim
            if (isVictim)
            {
                // if victim and dodge attack
                if (procExtra & PROC_EX_DODGE)
                {
                    //Update AURA_STATE on dodge
                    if (getClass() != CLASS_ROGUE) // skip Rogue Riposte
                    {
                        ModifyAuraState(AURA_STATE_DEFENSE, true);
                        StartReactiveTimer(REACTIVE_DEFENSE);
                    }
                }
                // if victim and parry attack
                if (procExtra & PROC_EX_PARRY)
                {
                    // For Hunters only Counterattack (skip Mongoose bite)
                    if (getClass() == CLASS_HUNTER)
                    {
                        ModifyAuraState(AURA_STATE_HUNTER_PARRY, true);
                        StartReactiveTimer(REACTIVE_HUNTER_PARRY);
                    }
                    else
                    {
                        ModifyAuraState(AURA_STATE_DEFENSE, true);
                        StartReactiveTimer(REACTIVE_DEFENSE);
                    }
                }
                // if and victim block attack
                if (procExtra & PROC_EX_BLOCK)
                {
                    ModifyAuraState(AURA_STATE_DEFENSE, true);
                    StartReactiveTimer(REACTIVE_DEFENSE);
                }
            }
            else //For attacker
            {
                // Overpower on victim dodge
                if (procExtra & PROC_EX_DODGE && GetTypeId() == TYPEID_PLAYER && getClass() == CLASS_WARRIOR)
                {
                    ToPlayer()->AddComboPoints(pTarget, 1);
                    StartReactiveTimer(REACTIVE_OVERPOWER);
                }
                // Enable AURA_STATE_CRIT on crit
                if (procExtra & PROC_EX_CRITICAL_HIT)
                {
                    ModifyAuraState(AURA_STATE_CRIT, true);
                    StartReactiveTimer(REACTIVE_CRIT);
                    if (getClass() == CLASS_HUNTER)
                    {
                        ModifyAuraState(AURA_STATE_HUNTER_CRIT_STRIKE, true);
                        StartReactiveTimer(REACTIVE_HUNTER_CRIT);
                    }
                }
            }
        }
    }

    RemoveSpellList removedSpells;
    ProcTriggeredList procTriggered;
    // Fill procTriggered list
    for (AuraMap::const_iterator itr = GetAuras().begin(); itr != GetAuras().end(); ++itr)
    {
        SpellProcEventEntry const* spellProcEvent = NULL;
        bool active = damage || (procExtra & PROC_EX_BLOCK && isVictim);

        SpellEntry const* spellproto = sSpellStore.LookupEntry(itr->second->GetId());

        for (uint32 i = 0; i < MAX_SPELL_EFFECTS; ++i)
            if (spellproto->Effect[i] == SPELL_EFFECT_TRIGGER_SPELL)
                active = true;

        if (!IsTriggeredAtSpellProcEvent(pTarget, itr->second, procSpell, procFlag, procExtra, attType, isVictim, active, spellProcEvent))
            continue;

        procTriggered.push_back(ProcTriggeredData(spellProcEvent, itr->second));
    }
    // Handle effects proceed this time
    for (ProcTriggeredList::iterator i = procTriggered.begin(); i != procTriggered.end(); ++i)
    {
        // Some auras can be deleted in function called in this loop (except first, ofc)
        // Until storing auars in std::multimap to hard check deleting by another way
        if (i != procTriggered.begin())
        {
            bool found = false;
            AuraMap::const_iterator lower = GetAuras().lower_bound(i->triggeredByAura_SpellPair);
            AuraMap::const_iterator upper = GetAuras().upper_bound(i->triggeredByAura_SpellPair);
            for (AuraMap::const_iterator itr = lower; itr != upper; ++itr)
            {
                if (itr->second == i->triggeredByAura)
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                //                sLog.outDebug("Spell aura %u (id:%u effect:%u) has been deleted before call spell proc event handler", i->triggeredByAura->GetModifier()->m_auraname, i->triggeredByAura_SpellPair.first, i->triggeredByAura_SpellPair.second);
                //                sLog.outDebug("It can be deleted one from early proccesed auras:");
                //                for (ProcTriggeredList::iterator i2 = procTriggered.begin(); i != i2; ++i2)
                //                    sLog.outDebug("     Spell aura %u (id:%u effect:%u)", i->triggeredByAura->GetModifier()->m_auraname,i2->triggeredByAura_SpellPair.first,i2->triggeredByAura_SpellPair.second);
                //                    sLog.outDebug("     <end of list>");
                continue;
            }
        }

        SpellProcEventEntry const* spellProcEvent = i->spellProcEvent;
        Aura* triggeredByAura = i->triggeredByAura;
        Modifier* auraModifier = triggeredByAura->GetModifier();
        SpellEntry const* spellInfo = triggeredByAura->GetSpellProto();
        bool useCharges = triggeredByAura->m_procCharges > 0;

        // For players set spell cooldown if need
        uint32 cooldown = 0;
        if (GetTypeId() == TYPEID_PLAYER && spellProcEvent && spellProcEvent->cooldown)
            cooldown = spellProcEvent->cooldown;

        if (spellInfo->AttributesEx3 & SPELL_ATTR3_DISABLE_PROC)
            ++m_procDeep;

        switch (auraModifier->m_auraname)
        {
        case SPELL_AURA_PROC_TRIGGER_SPELL:
            {
                DEBUG_LOG("ProcDamageAndSpell: casting spell %u (triggered by %s aura of spell %u)", spellInfo->Id, (isVictim ? "a victim's" : "an attacker's"), triggeredByAura->GetId());
                // Don`t drop charge or add cooldown for not started trigger
                if (!HandleProcTriggerSpell(pTarget, damage, triggeredByAura, procSpell, procFlag, procExtra, cooldown))
                    continue;
                break;
            }
        case SPELL_AURA_PROC_TRIGGER_DAMAGE:
            {
                DEBUG_LOG("ProcDamageAndSpell: doing %u damage from spell id %u (triggered by %s aura of spell %u)", auraModifier->m_amount, spellInfo->Id, (isVictim ? "a victim's" : "an attacker's"), triggeredByAura->GetId());
                SpellNonMeleeDamage damageInfo(this, pTarget, spellInfo->Id, spellInfo->SchoolMask);
                uint32 damage = SpellDamageBonus(pTarget, spellInfo, auraModifier->m_amount, SPELL_DIRECT_DAMAGE);
                CalculateSpellDamageTaken(&damageInfo, damage, spellInfo);
                SendSpellNonMeleeDamageLog(&damageInfo);
                DealSpellDamage(&damageInfo, true);
                break;
            }
        case SPELL_AURA_MANA_SHIELD:
        case SPELL_AURA_DUMMY:
            {
                DEBUG_LOG("ProcDamageAndSpell: casting spell id %u (triggered by %s dummy aura of spell %u)", spellInfo->Id, (isVictim ? "a victim's" : "an attacker's"), triggeredByAura->GetId());
                if (!HandleDummyAuraProc(pTarget, damage, triggeredByAura, procSpell, procFlag, procExtra, cooldown))
                    continue;
                break;
            }
        case SPELL_AURA_MOD_HASTE:
            {
                DEBUG_LOG("ProcDamageAndSpell: casting spell id %u (triggered by %s haste aura of spell %u)", spellInfo->Id, (isVictim ? "a victim's" : "an attacker's"), triggeredByAura->GetId());
                if (!HandleHasteAuraProc(pTarget, damage, triggeredByAura, procSpell, procFlag, procExtra, cooldown))
                    continue;
                break;
            }
        case SPELL_AURA_OVERRIDE_CLASS_SCRIPTS:
            {
                DEBUG_LOG("ProcDamageAndSpell: casting spell id %u (triggered by %s aura of spell %u)", spellInfo->Id, (isVictim ? "a victim's" : "an attacker's"), triggeredByAura->GetId());
                if (!HandleOverrideClassScriptAuraProc(pTarget, triggeredByAura, procSpell, cooldown))
                    continue;
                break;
            }
        case SPELL_AURA_PRAYER_OF_MENDING:
            {
                DEBUG_LOG("ProcDamageAndSpell: casting mending (triggered by %s dummy aura of spell %u)",
                          (isVictim ? "a victim's" : "an attacker's"), triggeredByAura->GetId());

                HandleMendingAuraProc(triggeredByAura);
                break;
            }
        case SPELL_AURA_PROC_TRIGGER_SPELL_WITH_VALUE:
            {
                DEBUG_LOG("ProcDamageAndSpell: casting spell %u (triggered with value by %s aura of spell %u)", spellInfo->Id, (isVictim ? "a victim's" : "an attacker's"), triggeredByAura->GetId());

                if (!HandleProcTriggerSpell(pTarget, damage, triggeredByAura, procSpell, procFlag, procExtra, cooldown))
                    continue;
                break;
            }
        case SPELL_AURA_MOD_STUN:
            // Remove by default, but if charge exist drop it
            if (triggeredByAura->m_procCharges == 0)
                removedSpells.push_back(triggeredByAura->GetId());
            break;
        case SPELL_AURA_MELEE_ATTACK_POWER_ATTACKER_BONUS:
        case SPELL_AURA_RANGED_ATTACK_POWER_ATTACKER_BONUS:
            // Hunter's Mark (1-4 Rangs)
            if (spellInfo->SpellFamilyName == SPELLFAMILY_HUNTER && (spellInfo->SpellFamilyFlags & 0x0000000000000400LL))
            {
                uint32 basevalue = triggeredByAura->GetBasePoints();
                auraModifier->m_amount += basevalue / 10;
                if (auraModifier->m_amount > int32(basevalue) * 4)
                    auraModifier->m_amount = basevalue * 4;
            }
            break;
        case SPELL_AURA_MOD_CASTING_SPEED:
            // Skip melee hits or instant cast spells
            if (procSpell == NULL || GetSpellCastTime(procSpell) == 0)
                continue;
            break;
        case SPELL_AURA_REFLECT_SPELLS_SCHOOL:
            // Skip Melee hits and spells ws wrong school
            if (procSpell == NULL || (auraModifier->m_miscvalue & procSpell->SchoolMask) == 0)
                continue;
            break;
        case SPELL_AURA_MOD_POWER_COST_SCHOOL_PCT:
        case SPELL_AURA_MOD_POWER_COST_SCHOOL:
            // Skip melee hits and spells ws wrong school or zero cost
            if (procSpell == NULL ||
                (procSpell->manaCost == 0 && procSpell->ManaCostPercentage == 0) || // Cost check
                (auraModifier->m_miscvalue & procSpell->SchoolMask) == 0)         // School check
                continue;
            break;
        case SPELL_AURA_MECHANIC_IMMUNITY:
            // Compare mechanic
            if (procSpell == NULL || procSpell->Mechanic != uint32(auraModifier->m_miscvalue))
                continue;
            break;
        case SPELL_AURA_MOD_MECHANIC_RESISTANCE:
            // Compare mechanic
            if (procSpell == NULL || procSpell->Mechanic != uint32(auraModifier->m_miscvalue))
                continue;
            break;
        default:
            // nothing do, just charges counter
            break;
        }
        // Remove charge (aura can be removed by triggers)
        if (useCharges)
        {
            // need found aura on drop (can be dropped by triggers)
            AuraMap::const_iterator lower = GetAuras().lower_bound(i->triggeredByAura_SpellPair);
            AuraMap::const_iterator upper = GetAuras().upper_bound(i->triggeredByAura_SpellPair);
            for (AuraMap::const_iterator itr = lower; itr != upper; ++itr)
            {
                if (itr->second == i->triggeredByAura)
                {
                    triggeredByAura->m_procCharges -= 1;
                    triggeredByAura->UpdateAuraCharges();
                    if (triggeredByAura->m_procCharges <= 0)
                        removedSpells.push_back(triggeredByAura->GetId());
                    break;
                }
            }
        }

        if (spellInfo->AttributesEx3 & SPELL_ATTR3_DISABLE_PROC)
            --m_procDeep;
    }
    if (removedSpells.size())
    {
        // Sort spells and remove dublicates
        removedSpells.sort();
        removedSpells.unique();
        // Remove auras from removedAuras
        for (RemoveSpellList::const_iterator i = removedSpells.begin(); i != removedSpells.end(); i++)
            RemoveAurasDueToSpell(*i);
    }
    --m_procDeep;
}

SpellSchoolMask Unit::GetMeleeDamageSchoolMask() const
{
    return SPELL_SCHOOL_MASK_NORMAL;
}

Player* Unit::GetSpellModOwner() const
{
    if (Player* player = const_cast<Unit*>(this)->ToPlayer())
        return player;

    if (IsPet() || IsTotem())
    {
        if (Unit* owner = GetOwner())
            if (Player* player = owner->ToPlayer())
                return player;
    }
    return NULL;
}

//----------Pet responses methods-----------------
void Unit::SendPetCastFail(uint32 spellid, SpellCastResult msg)
{
    Unit* owner = GetCharmerOrOwner();
    if (!owner || owner->GetTypeId() != TYPEID_PLAYER)
        return;

    WorldPacket data(SMSG_PET_CAST_FAILED, 4 + 1);
    data << uint32(spellid);
    data << uint8(msg);
    owner->ToPlayer()->GetSession()->SendPacket(&data);
}

void Unit::SendPetActionFeedback(uint8 msg)
{
    Unit* owner = GetOwner();
    if (!owner || owner->GetTypeId() != TYPEID_PLAYER)
        return;

    WorldPacket data(SMSG_PET_ACTION_FEEDBACK, 1);
    data << uint8(msg);
    owner->ToPlayer()->GetSession()->SendPacket(&data);
}

void Unit::SendPetTalk(uint32 pettalk)
{
    Unit* owner = GetOwner();
    if (!owner || owner->GetTypeId() != TYPEID_PLAYER)
        return;

    WorldPacket data(SMSG_PET_ACTION_SOUND, 8 + 4);
    data << uint64(GetGUID());
    data << uint32(pettalk);
    owner->ToPlayer()->GetSession()->SendPacket(&data);
}

void Unit::SendPetSpellCooldown (uint32 spellid, time_t cooltime)
{
    Unit* owner = GetOwner();
    if (!owner || owner->GetTypeId() != TYPEID_PLAYER)
        return;

    WorldPacket data(SMSG_SPELL_COOLDOWN, 8 + 1 + 4 + 4);
    data << uint64(GetGUID());
    data << uint8(0x0);                                     // flags (0x1, 0x2)
    data << uint32(spellid);
    data << uint32(cooltime);

    owner->ToPlayer()->GetSession()->SendPacket(&data);
}

void Unit::SendPetClearCooldown (uint32 spellid)
{
    Unit* owner = GetOwner();
    if (!owner || owner->GetTypeId() != TYPEID_PLAYER)
        return;

    WorldPacket data(SMSG_CLEAR_COOLDOWN, (4 + 8));
    data << uint32(spellid);
    data << uint64(GetGUID());
    owner->ToPlayer()->GetSession()->SendPacket(&data);
}

void Unit::SendPetAIReaction(uint64 guid)
{
    Unit* owner = GetOwner();
    if (!owner || owner->GetTypeId() != TYPEID_PLAYER)
        return;

    WorldPacket data(SMSG_AI_REACTION, 8 + 4);
    data << uint64(guid) << uint32(00000002);
    owner->ToPlayer()->GetSession()->SendPacket(&data);
}

//----------End of Pet responses methods----------

void Unit::StopMoving()
{
    ClearUnitState(UNIT_STATE_MOVING);

    // not need send any packets if not in world
    if (!IsInWorld() || movespline->Finalized())
        return;

    Movement::MoveSplineInit init(*this);
    init.Stop();
}

void Unit::SendMovementFlagUpdate(bool self /* = false */)
{
    WorldPacket data;
    BuildHeartBeatMsg(&data);
    SendMessageToSet(&data, self);
}

bool Unit::IsSitState() const
{
    uint8 s = GetStandState();
    return
        s == UNIT_STAND_STATE_SIT_CHAIR        || s == UNIT_STAND_STATE_SIT_LOW_CHAIR  ||
        s == UNIT_STAND_STATE_SIT_MEDIUM_CHAIR || s == UNIT_STAND_STATE_SIT_HIGH_CHAIR ||
        s == UNIT_STAND_STATE_SIT;
}

bool Unit::IsStandState() const
{
    uint8 s = GetStandState();
    return !IsSitState() && s != UNIT_STAND_STATE_SLEEP && s != UNIT_STAND_STATE_KNEEL;
}

void Unit::SetStandState(uint8 state)
{
    SetByteValue(UNIT_FIELD_BYTES_1, 0, state);

    if (IsStandState())
        RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_NOT_SEATED);

    if (GetTypeId() == TYPEID_PLAYER)
    {
        WorldPacket data(SMSG_STANDSTATE_UPDATE, 1);
        data << (uint8)state;
        ToPlayer()->GetSession()->SendPacket(&data);
    }
}

bool Unit::IsPolymorphed() const
{
    uint32 transformId = getTransForm();
    if (!transformId)
        return false;

    return GetSpellSpecific(getTransForm()) == SPELL_SPECIFIC_MAGE_POLYMORPH;
}

void Unit::SetDisplayId(uint32 modelId)
{
    SetUInt32Value(UNIT_FIELD_DISPLAYID, modelId);

    UpdateModelData();

    if (GetTypeId() == TYPEID_UNIT && IsPet())
    {
        Pet* pet = ((Pet*)this);
        if (!pet->isControlled())
            return;
        Unit* owner = GetOwner();
        if (owner && (owner->GetTypeId() == TYPEID_PLAYER) && owner->ToPlayer()->GetGroup())
            owner->ToPlayer()->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_PET_MODEL_ID);
    }
}

void Unit::UpdateModelData()
{
    if (CreatureModelInfo const* modelInfo = sObjectMgr.GetCreatureModelInfo(GetDisplayId()))
    {
        // we expect values in database to be relative to scale = 1.0
        SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, GetObjectScale() * modelInfo->bounding_radius);

        // never actually update combat_reach for player, it's always the same. Below player case is for initialization
        if (GetTypeId() == TYPEID_PLAYER)
            SetFloatValue(UNIT_FIELD_COMBATREACH, 1.5f);
        else
            SetFloatValue(UNIT_FIELD_COMBATREACH, GetObjectScale() * modelInfo->combat_reach);

        SetBaseWalkSpeed(modelInfo->SpeedWalk);
        SetBaseRunSpeed(modelInfo->SpeedRun);
    }
}

void Unit::ClearComboPointHolders()
{
    while (!m_ComboPointHolders.empty())
    {
        uint32 lowguid = *m_ComboPointHolders.begin();

        Player* plr = sObjectMgr.GetPlayer(MAKE_NEW_GUID(lowguid, 0, HIGHGUID_PLAYER));
        if (plr && plr->GetComboTarget() == GetGUID())         // recheck for safe
            plr->ClearComboPoints();                        // remove also guid from m_ComboPointHolders;
        else
            m_ComboPointHolders.erase(lowguid);             // or remove manually
    }
}

void Unit::ClearAllReactives()
{
    for (uint8 i = 0; i < MAX_REACTIVE; ++i)
        m_reactiveTimer[i] = 0;

    if (HasAuraState(AURA_STATE_DEFENSE))
        ModifyAuraState(AURA_STATE_DEFENSE, false);
    if (getClass() == CLASS_HUNTER && HasAuraState(AURA_STATE_HUNTER_PARRY))
        ModifyAuraState(AURA_STATE_HUNTER_PARRY, false);
    if (HasAuraState(AURA_STATE_CRIT))
        ModifyAuraState(AURA_STATE_CRIT, false);
    if (getClass() == CLASS_HUNTER && HasAuraState(AURA_STATE_HUNTER_CRIT_STRIKE))
        ModifyAuraState(AURA_STATE_HUNTER_CRIT_STRIKE, false);

    if (getClass() == CLASS_WARRIOR && GetTypeId() == TYPEID_PLAYER)
        ToPlayer()->ClearComboPoints();
}

void Unit::UpdateReactives(uint32 p_time)
{
    for (uint8 i = 0; i < MAX_REACTIVE; ++i)
    {
        ReactiveType reactive = ReactiveType(i);

        if (!m_reactiveTimer[reactive])
            continue;

        if (m_reactiveTimer[reactive] <= p_time)
        {
            m_reactiveTimer[reactive] = 0;

            switch (reactive)
            {
                case REACTIVE_DEFENSE:
                    if (HasAuraState(AURA_STATE_DEFENSE))
                        ModifyAuraState(AURA_STATE_DEFENSE, false);
                    break;
                case REACTIVE_HUNTER_PARRY:
                    if (getClass() == CLASS_HUNTER && HasAuraState(AURA_STATE_HUNTER_PARRY))
                        ModifyAuraState(AURA_STATE_HUNTER_PARRY, false);
                    break;
                case REACTIVE_CRIT:
                    if (HasAuraState(AURA_STATE_CRIT))
                        ModifyAuraState(AURA_STATE_CRIT, false);
                    break;
                case REACTIVE_HUNTER_CRIT:
                    if (getClass() == CLASS_HUNTER && HasAuraState(AURA_STATE_HUNTER_CRIT_STRIKE))
                        ModifyAuraState(AURA_STATE_HUNTER_CRIT_STRIKE, false);
                    break;
                case REACTIVE_OVERPOWER:
                    if (getClass() == CLASS_WARRIOR && GetTypeId() == TYPEID_PLAYER)
                        ToPlayer()->ClearComboPoints();
                    break;
                default:
                    break;
            }
        }
        else
            m_reactiveTimer[reactive] -= p_time;
    }
}

Unit* Unit::SelectNearbyTarget(Unit* exclude, float dist) const
{
    std::list<Unit* > targets;
    Oregon::AnyUnfriendlyUnitInObjectRangeCheck u_check(this, this, dist);
    Oregon::UnitListSearcher<Oregon::AnyUnfriendlyUnitInObjectRangeCheck> searcher(targets, u_check);
    VisitNearbyObject(dist, searcher);

    // remove current target
    if (GetVictim())
        targets.remove(GetVictim());

    if (exclude)
        targets.remove(exclude);

    // remove not LoS targets
    for (std::list<Unit* >::iterator tIter = targets.begin(); tIter != targets.end();)
    {
        if (!IsWithinLOSInMap(*tIter))
        {
            std::list<Unit* >::iterator tIter2 = tIter;
            ++tIter;
            targets.erase(tIter2);
        }
        else
            ++tIter;
    }

    // no appropriate targets
    if (targets.empty())
        return NULL;

    // select random
    return SelectRandomContainerElement(targets);
}

Player* Unit::SelectNearestPlayer(float distance) const
{
    CellCoord pair(Oregon::ComputeCellCoord(GetPositionX(), GetPositionY()));
    Cell cell(pair);
    cell.SetNoCreate();

    Player* pPlayer = NULL;

    {
        Oregon::NearestPlayerInObjectRangeCheck creature_check(*this, distance);
        Oregon::PlayerSearcher<Oregon::NearestPlayerInObjectRangeCheck> searcher(pPlayer, creature_check);

        TypeContainerVisitor<Oregon::PlayerSearcher<Oregon::NearestPlayerInObjectRangeCheck>, WorldTypeMapContainer> world_player_searcher(searcher);
        TypeContainerVisitor<Oregon::PlayerSearcher<Oregon::NearestPlayerInObjectRangeCheck>, GridTypeMapContainer> grid_player_searcher(searcher);

        cell.Visit(pair, world_player_searcher, *GetMap(), *this, distance);
        cell.Visit(pair, grid_player_searcher, *GetMap(), *this, distance);
    }

    return pPlayer;
}

void Unit::ApplyAttackTimePercentMod(WeaponAttackType att, float val, bool apply)
{
    float remainingTimePct = (float)m_attackTimer[att] / (GetAttackTime(att) * m_modAttackSpeedPct[att]);
    if (val > 0)
    {
        ApplyPercentModFloatVar(m_modAttackSpeedPct[att], val, !apply);
        ApplyPercentModFloatValue(UNIT_FIELD_BASEATTACKTIME + att, val, !apply);
    }
    else
    {
        ApplyPercentModFloatVar(m_modAttackSpeedPct[att], -val, apply);
        ApplyPercentModFloatValue(UNIT_FIELD_BASEATTACKTIME + att, -val, apply);
    }
    m_attackTimer[att] = uint32(GetAttackTime(att) * m_modAttackSpeedPct[att] * remainingTimePct);
}

void Unit::ApplyCastTimePercentMod(float val, bool apply)
{
    if (val > 0)
        ApplyPercentModFloatValue(UNIT_MOD_CAST_SPEED, val, !apply);
    else
        ApplyPercentModFloatValue(UNIT_MOD_CAST_SPEED, -val, apply);
}

uint32 Unit::GetCastingTimeForBonus(SpellEntry const* spellProto, DamageEffectType damagetype, uint32 CastingTime)
{
    // Not apply this to creature casted spells with casttime == 0
    if (CastingTime == 0 && GetTypeId() == TYPEID_UNIT && !IsPet())
        return 3500;

    if (CastingTime > 7000) CastingTime = 7000;
    if (CastingTime < 1500) CastingTime = 1500;

    if (damagetype == DOT && !IsChanneledSpell(spellProto))
        CastingTime = 3500;

    int32 overTime    = 0;
    uint8 effects     = 0;
    bool DirectDamage = false;
    bool AreaEffect   = false;

    for (uint32 i = 0; i < MAX_SPELL_EFFECTS; i++)
    {
        switch (spellProto->Effect[i])
        {
            case SPELL_EFFECT_SCHOOL_DAMAGE:
            case SPELL_EFFECT_POWER_DRAIN:
            case SPELL_EFFECT_HEALTH_LEECH:
            case SPELL_EFFECT_ENVIRONMENTAL_DAMAGE:
            case SPELL_EFFECT_POWER_BURN:
            case SPELL_EFFECT_HEAL:
                DirectDamage = true;
                break;
            case SPELL_EFFECT_APPLY_AURA:
                switch (spellProto->EffectApplyAuraName[i])
                {
                    case SPELL_AURA_PERIODIC_DAMAGE:
                    case SPELL_AURA_PERIODIC_HEAL:
                    case SPELL_AURA_PERIODIC_LEECH:
                        if (GetSpellDuration(spellProto))
                            overTime = GetSpellDuration(spellProto);
                        break;
                    default:
                        // -5% per additional effect
                        ++effects;
                        break;
                }
            default:
                break;
        }

        if (IsAreaEffectTarget[spellProto->EffectImplicitTargetA[i]] || IsAreaEffectTarget[spellProto->EffectImplicitTargetB[i]])
            AreaEffect = true;
    }

    // Combined Spells with Both Over Time and Direct Damage
    if (overTime > 0 && CastingTime > 0 && DirectDamage)
    {
        // mainly for DoTs which are 3500 here otherwise
        uint32 OriginalCastTime = GetSpellCastTime(spellProto);
        if (OriginalCastTime > 7000) OriginalCastTime = 7000;
        if (OriginalCastTime < 1500) OriginalCastTime = 1500;
        // Portion to Over Time
        float PtOT = (overTime / 15000.0f) / ((overTime / 15000.0f) + (OriginalCastTime / 3500.0f));

        if (damagetype == DOT)
            CastingTime = uint32(CastingTime * PtOT);
        else if (PtOT < 1.0f)
            CastingTime  = uint32(CastingTime * (1 - PtOT));
        else
            CastingTime = 0;
    }

    // Area Effect Spells receive only half of bonus
    if (AreaEffect)
        CastingTime /= 2;

    // -5% of total per any additional effect
    for (uint8 i = 0; i < effects; ++i)
    {
        if (CastingTime > 175)
            CastingTime -= 175;
        else
        {
            CastingTime = 0;
            break;
        }
    }

    return CastingTime;
}

void Unit::UpdateAuraForGroup(uint8 slot)
{
    if (slot >= MAX_AURAS)                        // slot not found, return
        return;

    if (GetTypeId() == TYPEID_PLAYER)
    {
        Player* player = ToPlayer();
        if (player->GetGroup())
        {
            player->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_AURAS);
            player->SetAuraUpdateMask(slot);
        }
    }
    else if (GetTypeId() == TYPEID_UNIT && IsPet())
    {
        Pet* pet = ((Pet*)this);
        if (pet->isControlled())
        {
            Unit* owner = GetOwner();
            if (owner && (owner->GetTypeId() == TYPEID_PLAYER) && owner->ToPlayer()->GetGroup())
            {
                owner->ToPlayer()->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_PET_AURAS);
                pet->SetAuraUpdateMask(slot);
            }
        }
    }
}

float Unit::GetAPMultiplier(WeaponAttackType attType, bool normalized)
{
    if (!normalized || GetTypeId() != TYPEID_PLAYER || (IsInFeralForm() && !normalized))
        return float(GetAttackTime(attType)) / 1000.0f;

    Item* Weapon = ToPlayer()->GetWeaponForAttack(attType);
    if (!Weapon)
        return 2.4f;                                         // fist attack

    switch (Weapon->GetProto()->InventoryType)
    {
        case INVTYPE_2HWEAPON:
            return 3.3f;
        case INVTYPE_RANGED:
        case INVTYPE_RANGEDRIGHT:
        case INVTYPE_THROWN:
            return 2.8f;
        case INVTYPE_WEAPON:
        case INVTYPE_WEAPONMAINHAND:
        case INVTYPE_WEAPONOFFHAND:
        default:
            return Weapon->GetProto()->SubClass == ITEM_SUBCLASS_WEAPON_DAGGER ? 1.7f : 2.4f;
    }
}

Aura* Unit::GetDummyAura(uint32 spell_id) const
{
    Unit::AuraList const& mDummy = GetAurasByType(SPELL_AURA_DUMMY);
    for (Unit::AuraList::const_iterator itr = mDummy.begin(); itr != mDummy.end(); ++itr)
        if ((*itr)->GetId() == spell_id)
            return *itr;

    return NULL;
}

bool Unit::IsUnderLastManaUseEffect() const
{
    return  getMSTimeDiff(m_lastManaUse, getMSTime()) < 5000;
}

void Unit::SetContestedPvP(Player* attackedPlayer)
{
    Player* player = GetCharmerOrOwnerPlayerOrPlayerItself();

    if (!player || (attackedPlayer && attackedPlayer == player) || (player->duel && player->duel->opponent == attackedPlayer))
        return;

    player->SetContestedPvPTimer(30000);

    if (!player->HasUnitState(UNIT_STATE_ATTACK_PLAYER))
    {
        player->AddUnitState(UNIT_STATE_ATTACK_PLAYER);
        player->SetFlag(PLAYER_FLAGS, PLAYER_FLAGS_CONTESTED_PVP);
        // call MoveInLineOfSight for nearby contested guards
        UpdateObjectVisibility();
    }
    if (!HasUnitState(UNIT_STATE_ATTACK_PLAYER))
    {
        AddUnitState(UNIT_STATE_ATTACK_PLAYER);
        // call MoveInLineOfSight for nearby contested guards
        UpdateObjectVisibility();
    }
}

void Unit::AddPetAura(PetAura const* petSpell)
{
    if (GetTypeId() != TYPEID_PLAYER)
        return;

    m_petAuras.insert(petSpell);
    if (Pet* pet = ToPlayer()->GetPet())
        pet->CastPetAura(petSpell);
}

void Unit::RemovePetAura(PetAura const* petSpell)
{
    if (GetTypeId() != TYPEID_PLAYER)
        return;

    m_petAuras.erase(petSpell);
    if (Pet* pet = ToPlayer()->GetPet())
        pet->RemoveAurasDueToSpell(petSpell->GetAura(pet->GetEntry()));
}

Pet* Unit::CreateTamedPetFrom(Creature* creatureTarget, uint32 spell_id)
{
    if (GetTypeId() != TYPEID_PLAYER)
        return NULL;

    Pet* pet = new Pet(ToPlayer(), HUNTER_PET);

    if (!pet->CreateBaseAtCreature(creatureTarget))
    {
        delete pet;
        return NULL;
    }

    pet->SetCreatorGUID(GetGUID());
    pet->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, GetFaction());
    pet->SetUInt32Value(UNIT_CREATED_BY_SPELL, spell_id);

    if (!pet->InitStatsForLevel(creatureTarget->getLevel()))
    {
        sLog.outError("Pet::InitStatsForLevel() failed for creature (Entry: %u)!", creatureTarget->GetEntry());
        delete pet;
        return NULL;
    }

    pet->GetCharmInfo()->SetPetNumber(sObjectMgr.GeneratePetNumber(), true);
    // this enables pet details window (Shift+P)
    pet->InitPetCreateSpells();
    pet->SetFullHealth();

    return pet;
}

bool Unit::IsTriggeredAtSpellProcEvent(Unit* victim, Aura* aura, SpellEntry const* procSpell, uint32 procFlag, uint32 procExtra, WeaponAttackType attType, bool isVictim, bool active, SpellProcEventEntry const*& spellProcEvent )
{
    SpellEntry const* spellProto = aura->GetSpellProto();

    // Get proc Event Entry
    spellProcEvent = sSpellMgr.GetSpellProcEvent(spellProto->Id);

    // Aura info stored here
    Modifier* mod = aura->GetModifier();
    // Skip this auras
    if (isNonTriggerAura[mod->m_auraname])
        return false;
    // If not trigger by default and spellProcEvent == NULL - skip
    if (!isTriggerAura[mod->m_auraname] && spellProcEvent == NULL)
        return false;

    // Get EventProcFlag
    uint32 EventProcFlag;
    if (spellProcEvent && spellProcEvent->procFlags) // if exist get custom spellProcEvent->procFlags
        EventProcFlag = spellProcEvent->procFlags;
    else
        EventProcFlag = spellProto->procFlags;       // else get from spell proto
    // Continue if no trigger exist
    if (!EventProcFlag)
        return false;

    // Check spellProcEvent data requirements
    if (!SpellMgr::IsSpellProcEventCanTriggeredBy(spellProcEvent, EventProcFlag, procSpell, procFlag, procExtra, active))
        return false;

    // In most cases req get honor or XP from kill
    if (EventProcFlag & PROC_FLAG_KILL && GetTypeId() == TYPEID_PLAYER)
    {
        bool allow = ToPlayer()->isHonorOrXPTarget(victim);
        // Shadow Word: Death - can trigger from every kill
        if (aura->GetId() == 32409)
            allow = true;
        if (!allow)
            return false;
    }
    // Aura added by spell can`t trigger from self (prevent drop charges/do triggers)
    // But except periodic and kill triggers (can triggered from self)
    if (procSpell && procSpell->Id == spellProto->Id
        && !(spellProto->procFlags&(PROC_FLAG_TAKEN_PERIODIC | PROC_FLAG_KILL)))
        return false;

    // Check if current equipment allows aura to proc
    if (!isVictim && GetTypeId() == TYPEID_PLAYER)
    {
        Player* player = ToPlayer();
        if (spellProto->EquippedItemClass == ITEM_CLASS_WEAPON)
        {
            Item* item = NULL;
            if (attType == BASE_ATTACK)
                item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
            else if (attType == OFF_ATTACK)
                item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
            else
                item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_RANGED);

            if (!ToPlayer()->IsUseEquippedWeapon(attType == BASE_ATTACK))
                return false;

            if (!item || item->IsBroken() || item->GetProto()->Class != ITEM_CLASS_WEAPON || !((1 << item->GetProto()->SubClass) & spellProto->EquippedItemSubClassMask))
                return false;
        }
        else if (spellProto->EquippedItemClass == ITEM_CLASS_ARMOR)
        {
            // Check if player is wearing shield
            Item* item = ToPlayer()->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
            if (!item || item->IsBroken() || item->GetProto()->Class != ITEM_CLASS_ARMOR || !((1 << item->GetProto()->SubClass) & spellProto->EquippedItemSubClassMask))
                return false;
        }
    }
    // Get chance from spell
    float chance = float(spellProto->procChance);
    // If in spellProcEvent exist custom chance, chance = spellProcEvent->customChance;
    if (spellProcEvent && spellProcEvent->customChance)
        chance = spellProcEvent->customChance;
    // If PPM exist calculate chance from PPM
    if (!isVictim && spellProcEvent && spellProcEvent->ppmRate != 0)
    {
        uint32 WeaponSpeed = GetAttackTime(attType);
        chance = GetPPMProcChance(WeaponSpeed, spellProcEvent->ppmRate);
    }
    // Apply chance modifer aura
    if (Player* modOwner = GetSpellModOwner())
        modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_CHANCE_OF_SUCCESS, chance);

    return roll_chance_f(chance);
}

bool Unit::HandleMendingAuraProc(Aura* triggeredByAura)
{
    // aura can be deleted at casts
    SpellEntry const* spellProto = triggeredByAura->GetSpellProto();
    uint32 effIdx = triggeredByAura->GetEffIndex();
    int32 heal = triggeredByAura->GetModifier()->m_amount;
    uint64 caster_guid = triggeredByAura->GetCasterGUID();

    // jumps
    int32 jumps = triggeredByAura->m_procCharges - 1;

    // current aura expire
    triggeredByAura->m_procCharges = 1;             // will removed at next charges decrease

    // next target selection
    if (jumps > 0 && GetTypeId() == TYPEID_PLAYER && IS_PLAYER_GUID(caster_guid))
    {
        float radius;
        if (spellProto->EffectRadiusIndex[effIdx])
            radius = GetSpellRadius(spellProto, effIdx, false);
        else
            radius = GetSpellMaxRange(sSpellRangeStore.LookupEntry(spellProto->rangeIndex));

        if (Player* caster = ((Player*)triggeredByAura->GetCaster()))
        {
            caster->ApplySpellMod(spellProto->Id, SPELLMOD_RADIUS, radius, NULL);

            if (Player* target = ToPlayer()->GetNextRandomRaidMember(radius))
            {
                // aura will applied from caster, but spell casted from current aura holder
                SpellModifier* mod = new SpellModifier;
                mod->op = SPELLMOD_CHARGES;
                mod->value = jumps - 5;             // negative
                mod->type = SPELLMOD_FLAT;
                mod->spellId = spellProto->Id;
                mod->effectId = effIdx;
                mod->lastAffected = NULL;
                mod->mask = spellProto->SpellFamilyFlags;
                mod->charges = 0;

                caster->AddSpellMod(mod, true);
                CastCustomSpell(target, spellProto->Id, &heal, NULL, NULL, true, NULL, triggeredByAura, caster->GetGUID());
                caster->AddSpellMod(mod, false);
            }
            heal = caster->SpellHealingBonus(spellProto, heal, HEAL, this);
        }
    }

    // heal
    CastCustomSpell(this, 33110, &heal, NULL, NULL, true, NULL, NULL, caster_guid);
    return true;
}

void Unit::RemoveAurasAtChanneledTarget(SpellEntry const* spellInfo, Unit* caster)
{
    /*   uint64 target_guid = GetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT);
        if (target_guid == GetGUID())
            return;

        if (!IS_UNIT_GUID(target_guid))
            return;

        Unit* target = ObjectAccessor::GetUnit(*this, target_guid);*/
    if (!caster)
        return;

    for (AuraMap::iterator iter = GetAuras().begin(); iter != GetAuras().end();)
    {
        if (iter->second->GetId() == spellInfo->Id && iter->second->GetCasterGUID() == caster->GetGUID())
            RemoveAura(iter);
        else
            ++iter;
    }
}

bool Unit::SetPosition(float x, float y, float z, float orientation, bool teleport)
{
    // prevent crash when a bad coord is sent by the client
    if (!Oregon::IsValidMapCoord(x, y, z, orientation))
    {
        sLog.outDebug("Unit::SetPosition(%f, %f, %f) .. bad coordinates!", x, y, z);
        return false;
    }

    bool const turn = (GetOrientation() != orientation);

    // G3D::fuzzyEq won't help here, in some cases magnitudes differ by a little more than G3D::eps, but should be considered equal
    bool const relocated = (teleport || 
        std::fabs(GetPositionX() - x) > 0.001f ||
        std::fabs(GetPositionY() - y) > 0.001f ||
        std::fabs(GetPositionZ() - z) > 0.001f);

    if (turn)
        RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_TURNING);

    if (relocated)
    {
        RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_MOVE);

        // move and update visible state if need
        if (GetTypeId() == TYPEID_PLAYER)
            GetMap()->PlayerRelocation(ToPlayer(), x, y, z, orientation);
        else
            GetMap()->CreatureRelocation(ToCreature(), x, y, z, orientation);
    }
    else if (turn)
        SetOrientation(orientation);

    return (relocated || turn);
}

void Unit::NearTeleportTo(float x, float y, float z, float orientation, bool casting /*= false*/)
{
    DisableSpline();

    if (GetTypeId() == TYPEID_PLAYER)
        ToPlayer()->TeleportTo(GetMapId(), x, y, z, orientation, TELE_TO_NOT_LEAVE_TRANSPORT | TELE_TO_NOT_LEAVE_COMBAT | TELE_TO_NOT_UNSUMMON_PET | (casting ? TELE_TO_SPELL : 0));
    else
    {
        Position pos = {x, y, z, orientation};
        SendTeleportPacket(pos);
        SetPosition(x, y, z, orientation, true);
        UpdateObjectVisibility();
    }
}

void Unit::SendTeleportPacket(Position& pos)
{
    Position oldPos = { GetPositionX(), GetPositionY(), GetPositionZ(), GetOrientation() };
    if (GetTypeId() == TYPEID_UNIT)
        Relocate(&pos);

    WorldPacket data(MSG_MOVE_TELEPORT, 38);
    data << GetPackGUID();
    BuildMovementPacket(&data);
    if (GetTypeId() == TYPEID_UNIT)
        Relocate(&oldPos);
    if (GetTypeId() == TYPEID_PLAYER)
        Relocate(&pos);
    SendMessageToSet(&data, false);
}


/*-----------------------Oregon-----------------------------*/

void Unit::Kill(Unit* victim, bool durabilityLoss)
{
    // Prevent killing unit twice (and giving reward from kill twice)
    if (!victim->GetHealth())
        return;

    // find player: owner of controlled `this` or `this` itself maybe
    Player* player = GetCharmerOrOwnerPlayerOrPlayerItself();
    Creature* creature = victim->ToCreature();

    bool bRewardIsAllowed = true;
    if (creature)
    {
        bRewardIsAllowed = creature->IsDamageEnoughForLootingAndReward();
        if (!bRewardIsAllowed)
            creature->SetLootRecipient(NULL);
    }

    if (bRewardIsAllowed && creature && creature->GetLootRecipient())
        player = creature->GetLootRecipient();

    // Reward player, his pets, and group/raid members
    // call kill spell proc event (before real die and combat stop to triggering auras removed at death/combat stop)
    if (bRewardIsAllowed && player && player != victim)
    {
        WorldPacket data(SMSG_PARTYKILLLOG, (8+8)); // send event PARTY_KILL
        data << uint64(player->GetGUID()); // player with killing blow
        data << uint64(victim->GetGUID()); // victim

        Player* looter = player;
        Group* group = player->GetGroup();
        bool hasLooterGuid = false;

        if (group)
        {
            group->BroadcastPacket(&data, group->GetMemberGroup(player->GetGUID()) != 0);

            if (creature)
            {
                group->UpdateLooterGuid(creature, true);
                if (group->GetLooterGuid())
                {
                    looter = ObjectAccessor::FindPlayer(group->GetLooterGuid());
                    if (looter)
                    {
                        hasLooterGuid = true;
                        creature->SetLootRecipient(looter);   // update creature loot recipient to the allowed looter.
                    }
                }
            }
        }
        else
        {
            player->SendDirectMessage(&data);

            if (creature)
            {
                WorldPacket data2(SMSG_LOOT_LIST, 8 + 1 + 1);
                data2 << uint64(creature->GetGUID());
                data2 << uint8(0); // unk1
                data2 << uint8(0); // no group looter
                player->SendMessageToSet(&data2, true);
            }
        }

        // Generate loot before updating looter
        if (creature)
        {
            Loot* loot = &creature->loot;

            loot->clear();
            if (uint32 lootid = creature->GetCreatureTemplate()->lootid)
                loot->FillLoot(lootid, LootTemplates_Creature, looter, false);

            loot->generateMoneyLoot(creature->GetCreatureTemplate()->mingold, creature->GetCreatureTemplate()->maxgold);

            if (group)
            {
                if (hasLooterGuid)
                    group->SendLooter(creature, looter);
                else
                    group->SendLooter(creature, NULL);

                // Update round robin looter only if the creature had loot
                if (!loot->empty())
                    group->UpdateLooterGuid(creature);
            }
        }

        player->RewardPlayerAndGroupAtKill(victim, false);
    }

    // Do KILL and KILLED procs. KILL proc is called only for the unit who landed the killing blow (and its owner - for pets and totems) regardless of who tapped the victim
    if (IsPet() || IsTotem())
        if (Unit* owner = GetOwner())
            owner->ProcDamageAndSpell(victim, PROC_FLAG_KILL, PROC_FLAG_NONE, PROC_EX_NONE, 0);

    if (!victim->IsCritter())
        ProcDamageAndSpell(victim, PROC_FLAG_KILL, PROC_FLAG_KILLED, PROC_EX_NONE, 0);

    // Proc auras on death - must be before aura/combat remove
    victim->ProcDamageAndSpell(NULL, PROC_FLAG_DEATH, PROC_FLAG_NONE, PROC_EX_NONE, 0, BASE_ATTACK, 0);

    // if talent known but not triggered (check priest class for speedup check)
    bool SpiritOfRedemption = false;
    if (victim->GetTypeId() == TYPEID_PLAYER && victim->getClass() == CLASS_PRIEST)
    {
        AuraList const& vDummyAuras = victim->GetAurasByType(SPELL_AURA_DUMMY);
        for (AuraList::const_iterator itr = vDummyAuras.begin(); itr != vDummyAuras.end(); ++itr)
        {
            if ((*itr)->GetSpellProto()->SpellIconID == 1654)
            {
                // Save value before aura remove
                uint32 ressSpellId = victim->GetUInt32Value(PLAYER_SELF_RES_SPELL);
                if (!ressSpellId)
                    ressSpellId = victim->ToPlayer()->GetResurrectionSpellId();

                //Remove all expected to remove at death auras (most important negative case like DoT or periodic triggers)
                victim->RemoveAllAurasOnDeath();

                // Restore for use at real death
                victim->SetUInt32Value(PLAYER_SELF_RES_SPELL, ressSpellId);

                // Set health to 0 to simulate death
                victim->SetHealth(0);

                // FORM_SPIRITOFREDEMPTION and related auras
                victim->CastSpell(victim, 27827, true, NULL, *itr);
                SpiritOfRedemption = true;
                break;
            }
        }
    }

    if (!SpiritOfRedemption)
    {
        DEBUG_LOG("SET JUST_DIED");
        victim->setDeathState(JUST_DIED);
    }

    // Inform pets (if any) when player kills target)
    // MUST come after victim->setDeathState(JUST_DIED); or pet next target
    // selection will get stuck on same target and break pet react state
    if (player)
    {
        Pet* pet = player->GetPet();
        if (pet && pet->IsAlive() && pet->isControlled())
            pet->AI()->KilledUnit(victim);
    }

    // 10% durability loss on death
    // clean InHateListOf
    if (victim->GetTypeId() == TYPEID_PLAYER)
    {
        // remember victim PvP death for corpse type and corpse reclaim delay
        // at original death (not at SpiritOfRedemtionTalent timeout)
        victim->ToPlayer()->SetPvPDeath(player != NULL);

        // only if not player and not controlled by player pet. And not at BG
        if (durabilityLoss && !player && !victim->ToPlayer()->InBattleground())
        {
            DEBUG_LOG("We are dead, loosing 10 percents durability");
            victim->ToPlayer()->DurabilityLossAll(0.10f, false);
            // durability lost message
            WorldPacket data(SMSG_DURABILITY_DAMAGE_DEATH, 0);
            victim->ToPlayer()->GetSession()->SendPacket(&data);
        }
        // Call KilledUnit for creatures
        if (GetTypeId() == TYPEID_UNIT && ToCreature()->IsAIEnabled)
            ToCreature()->AI()->KilledUnit(victim);
        else if (Guardian* pPet = GetGuardianPet())
            pPet->AI()->KilledUnit(victim);

        // last damage from non duel opponent or opponent controlled creature
        if (victim->ToPlayer()->duel)
        {
            victim->ToPlayer()->duel->opponent->CombatStopWithPets(true);
            victim->ToPlayer()->CombatStopWithPets(true);
            victim->ToPlayer()->DuelComplete(DUEL_INTERUPTED);
        }
    }
    else                                                // creature died
    {
        DEBUG_LOG("DealDamageNotPlayer");

        if (!creature->IsPet())
        {
            creature->DeleteThreatList();

            // must be after setDeathState which resets dynamic flags
            if (!creature->loot.isLooted())
                creature->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
            else
                creature->AllLootRemovedFromCorpse();
        }

        // Call KilledUnit for creatures, this needs to be called after the lootable flag is set
        if (GetTypeId() == TYPEID_UNIT && ToCreature()->IsAIEnabled)
            ToCreature()->AI()->KilledUnit(victim);
        else if (Guardian* pPet = GetGuardianPet())
            pPet->AI()->KilledUnit(victim);

        // Call creature just died function
        if (creature->IsAIEnabled)
            creature->AI()->JustDied(this);

        // Dungeon specific stuff, only applies to players killing creatures
        if (creature->GetInstanceId())
        {
            Map* m = creature->GetMap();
            Player* creditedPlayer = GetCharmerOrOwnerPlayerOrPlayerItself();
            // @todo do instance binding anyway if the charmer/owner is offline

            if (m->IsDungeon() && creditedPlayer)
            {
                if (m->IsRaid() || m->IsHeroic())
                {
                    if (creature->GetCreatureTemplate()->flags_extra & CREATURE_FLAG_EXTRA_INSTANCE_BIND)
                        ((InstanceMap*)m)->PermBindAllPlayers(creditedPlayer);
                }
                else
                {
                    // the reset time is set but not added to the scheduler
                    // until the players leave the instance
                    time_t resettime = creature->GetRespawnTimeEx() + 2 * HOUR;
                    if (InstanceSave* save = sInstanceSaveMgr.GetInstanceSave(creature->GetInstanceId()))
                        if (save->GetResetTime() < resettime) save->SetResetTime(resettime);
                }
            }
        }
    }

    // outdoor pvp things, do these after setting the death state, else the player activity notify won't work... doh...
    // handle player kill only if not suicide (spirit of redemption for example)
    if (player && this != victim)
        if (OutdoorPvP* pvp = player->GetOutdoorPvP())
            pvp->HandleKill(player, victim);

    //if (victim->GetTypeId() == TYPEID_PLAYER)
    //    if (OutdoorPvP * pvp = victim->ToPlayer()->GetOutdoorPvP())
    //        pvp->HandlePlayerActivityChanged(victim->ToPlayer());

    // battleground things (do this at the end, so the death state flag will be properly set to handle in the bg->handlekill)
    if (player && player->InBattleground())
    {
        if (Battleground* bg = player->GetBattleground())
        {
            if (victim->GetTypeId() == TYPEID_PLAYER)
                bg->HandleKillPlayer(victim->ToPlayer(), player);
            else
                bg->HandleKillUnit(victim->ToCreature(), player);
        }
    }
}

void Unit::SetControlled(bool apply, UnitState state)
{
    if (apply)
    {
        if (HasUnitState(state))
            return;

        AddUnitState(state);
        switch (state)
        {
            case UNIT_STATE_STUNNED:
                SetStunned(true);
                CastStop();
                break;
            case UNIT_STATE_ROOT:
                if (!HasUnitState(UNIT_STATE_STUNNED))
                    SetRooted(true);
                break;
            case UNIT_STATE_CONFUSED:
                if (!HasUnitState(UNIT_STATE_STUNNED))
                {
                    ClearUnitState(UNIT_STATE_MELEE_ATTACKING);
                    SendMeleeAttackStop();
                    SetConfused(true);
                    CastStop();
                }
                break;
            case UNIT_STATE_FLEEING:
                if (!HasUnitState(UNIT_STATE_STUNNED | UNIT_STATE_CONFUSED))
                {
                    ClearUnitState(UNIT_STATE_MELEE_ATTACKING);
                    SendMeleeAttackStop();
                    SetFeared(true);
                    CastStop();
                }
                break;
            default:
                break;
        }
    }
    else
    {
        switch (state)
        {
            case UNIT_STATE_STUNNED:
                if (HasAuraType(SPELL_AURA_MOD_STUN))
                    return;

                ClearUnitState(state);
                SetStunned(false);
                break;
            case UNIT_STATE_ROOT:
                if (HasAuraType(SPELL_AURA_MOD_ROOT))
                    return;

                ClearUnitState(state);
                SetRooted(false);
                break;
            case UNIT_STATE_CONFUSED:
                if (HasAuraType(SPELL_AURA_MOD_CONFUSE))
                    return;

                ClearUnitState(state);
                SetConfused(false);
                break;
            case UNIT_STATE_FLEEING:
                if (HasAuraType(SPELL_AURA_MOD_FEAR))
                    return;

                ClearUnitState(state);
                SetFeared(false);
                break;
            default:
                return;
        }

        // Unit States might have been already cleared but auras still present. I need to check with HasAuraType
        if (HasAuraType(SPELL_AURA_MOD_STUN))
            SetStunned(true);

        if (HasAuraType(SPELL_AURA_MOD_ROOT))
            SetRooted(true);

        if (HasAuraType(SPELL_AURA_MOD_CONFUSE))
            SetConfused(true);

        if (HasAuraType(SPELL_AURA_MOD_FEAR))
            SetFeared(true);
    }
}

void Unit::SetStunned(bool apply)
{
    if (apply)
    {
        SetTarget(0);
        SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);

        // MOVEMENTFLAG_ROOT cannot be used in conjunction with MOVEMENTFLAG_MASK_MOVING (tested 3.3.5a)
        // this will freeze clients. That's why we remove MOVEMENTFLAG_MASK_MOVING before
        // setting MOVEMENTFLAG_ROOT
        RemoveUnitMovementFlag(MOVEMENTFLAG_MOVING);
        AddUnitMovementFlag(MOVEMENTFLAG_ROOT);
        StopMoving();

        if (GetTypeId() == TYPEID_PLAYER)
            SetStandState(UNIT_STAND_STATE_STAND);

        WorldPacket data(SMSG_FORCE_MOVE_ROOT, 8);
        data << GetPackGUID();
        data << uint32(0);
        SendMessageToSet(&data, true);

        CastStop();
    }
    else
    {
        if (IsAlive() && GetVictim())
           SetTarget(GetVictim()->GetGUID());

        // don't remove UNIT_FLAG_STUNNED for pet when owner is mounted (disabled pet's interface)
        Unit* owner = GetOwner();
        if (!owner || (owner->GetTypeId() == TYPEID_PLAYER && !owner->ToPlayer()->IsMounted()))
            RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);

        if (!HasUnitState(UNIT_STATE_ROOT))         // prevent moving if it also has root effect
        {
            WorldPacket data(SMSG_FORCE_MOVE_UNROOT, 8 + 4);
            data << GetPackGUID();
            data << uint32(0);
            SendMessageToSet(&data, true);

            RemoveUnitMovementFlag(MOVEMENTFLAG_ROOT);
        }
    }
}

void Unit::SetRooted(bool apply)
{
    if (apply)
    {
        // MOVEMENTFLAG_ROOT cannot be used in conjunction with MOVEMENTFLAG_MASK_MOVING (tested 3.3.5a)
        // this will freeze clients. That's why we remove MOVEMENTFLAG_MASK_MOVING before
        // setting MOVEMENTFLAG_ROOT
        RemoveUnitMovementFlag(MOVEMENTFLAG_MOVING);
        m_movementInfo.AddMovementFlag(MOVEMENTFLAG_ROOT);
        StopMoving();

        if (GetTypeId() == TYPEID_PLAYER)
        {
            WorldPacket data(SMSG_FORCE_MOVE_ROOT, 10);
            data << GetPackGUID();
            data << (uint32)2; // @todo Identify this
            SendMessageToSet(&data, true);
        }
        else
        {
            WorldPacket data(SMSG_SPLINE_MOVE_ROOT, 8);
            data << GetPackGUID();
            SendMessageToSet(&data, true);
        }
    }
    else
    {
        if (!HasUnitState(UNIT_STATE_STUNNED))      // prevent moving if it also has stun effect
        {
            if (GetTypeId() == TYPEID_PLAYER)
            {
                WorldPacket data(SMSG_FORCE_MOVE_UNROOT, 10);
                data << GetPackGUID();
                data << (uint32)2; // @todo Identify this
                SendMessageToSet(&data, true);
            }
            else
            {
                WorldPacket data(SMSG_SPLINE_MOVE_UNROOT, 8);
                data << GetPackGUID();
                SendMessageToSet(&data, true);
            }

            m_movementInfo.RemoveMovementFlag(MOVEMENTFLAG_ROOT);
        }
    }
}

void Unit::SetFeared(bool apply)
{
    if (apply)
    {
        SetTarget(0);

        Unit* caster = NULL;
        Unit::AuraList const& fearAuras = GetAurasByType(SPELL_AURA_MOD_FEAR);
        if (!fearAuras.empty())
            caster = ObjectAccessor::GetUnit(*this, fearAuras.front()->GetCasterGUID());
        if (!caster)
            caster = getAttackerForHelper();
        GetMotionMaster()->MoveFleeing(caster, fearAuras.empty() ? sWorld.getConfig(CONFIG_CREATURE_FAMILY_FLEE_DELAY) : 0);             // caster == NULL processed in MoveFleeing
    }
    else
    {
        if (IsAlive())
        {
            if (GetMotionMaster()->GetCurrentMovementGeneratorType() == FLEEING_MOTION_TYPE)
                GetMotionMaster()->MovementExpired();
            if (GetVictim())
                SetTarget(GetVictim()->GetGUID());
        }
    }

    if (Player* player = ToPlayer())
        if(!player->HasUnitState(UNIT_STATE_POSSESSED))
            player->SetClientControl(this, !apply);
}

void Unit::SetConfused(bool apply)
{
    if (apply)
    {
        SetTarget(0);
        GetMotionMaster()->MoveConfused();
    }
    else
    {
        if (IsAlive())
        {
            if (GetMotionMaster()->GetCurrentMovementGeneratorType() == CONFUSED_MOTION_TYPE)
                GetMotionMaster()->MovementExpired();
            if (GetVictim())
                SetTarget(GetVictim()->GetGUID());
        }
    }

    if (Player* player = ToPlayer())
        if (!player->HasUnitState(UNIT_STATE_POSSESSED))
            player->SetClientControl(this, !apply);
}

void Unit::SetCharmedBy(Unit* charmer, CharmType type)
{
    if (!charmer)
        return;

    // dismount players when charmed
    if (GetTypeId() == TYPEID_PLAYER)
        RemoveAurasByType(SPELL_AURA_MOUNTED);

    if (charmer->GetTypeId() == TYPEID_PLAYER)
        charmer->RemoveAurasByType(SPELL_AURA_MOUNTED);

    ASSERT(type != CHARM_TYPE_POSSESS || charmer->GetTypeId() == TYPEID_PLAYER);

    sLog.outDebug("SetCharmedBy: charmer %u, charmed %u, type %u.", charmer->GetEntry(), GetEntry(), (uint32)type);

    if (this == charmer)
    {
        sLog.outError("Crash alert! Unit::SetCharmedBy: Unit %u is trying to charm itself!", GetEntry());
        return;
    }

    if (GetTypeId() == TYPEID_PLAYER && ToPlayer()->GetTransport())
    {
        sLog.outError("Crash alert! Unit::SetCharmedBy: Player on transport is trying to charm %u", GetEntry());
        return;
    }

    // Already charmed
    if (GetCharmerGUID() || (IsPet() && ToPet()->GetOwner() != charmer))
    {
        sLog.outDebug("Unit::SetCharmedBy: %u (GUID %u) has already been charmed but %u (GUID %u) is trying to charm it!", GetEntry(), GetGUIDLow(), charmer->GetEntry(), charmer->GetGUIDLow());
        return;
    }

    CastStop();
    CombatStop(); //@todo CombatStop(true) may cause crash (interrupt spells)
    DeleteThreatList();

    // StopCastingCharm may remove a possessed pet?
    if (!IsInWorld())
    {
        sLog.outError("Crash alert! Unit::SetCharmedBy: %u is not in world but %u is trying to charm it!", GetEntry(), charmer->GetEntry());
        return;
    }

    _oldFactionId = GetFaction();
    SetFaction(charmer->GetFaction());

    // Set charmed
    charmer->SetCharm(this, true);
    SetCharmerGUID(charmer->GetGUID());

    if (GetTypeId() == TYPEID_UNIT)
    {
        ToCreature()->AI()->OnCharmed(true);
        GetMotionMaster()->MoveIdle();
    }
    else
    {
        Player* player = ToPlayer();
        if (player->isAFK())
            player->ToggleAFK();
        player->SetClientControl(this, false);
    }

    // Pets already have a properly initialized CharmInfo, don't overwrite it.
    if (!GetCharmInfo())
    {
        InitCharmInfo();
        if (type == CHARM_TYPE_POSSESS)
            GetCharmInfo()->InitPossessCreateSpells();
        else
            GetCharmInfo()->InitCharmCreateSpells();
    }

    if (charmer->GetTypeId() == TYPEID_PLAYER)
    {
        switch (type)
        {
            case CHARM_TYPE_POSSESS:
                AddUnitState(UNIT_STATE_POSSESSED);
                SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED);
                charmer->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_REMOVE_CLIENT_CONTROL);
                charmer->ToPlayer()->SetClientControl(this, true);
                charmer->ToPlayer()->SetViewpoint(this, true);
                charmer->ToPlayer()->PossessSpellInitialize();
                break;
            case CHARM_TYPE_CHARM:
                if (GetTypeId() == TYPEID_UNIT && charmer->getClass() == CLASS_WARLOCK)
                {
                    CreatureInfo const* cinfo = ToCreature()->GetCreatureTemplate();
                    if (cinfo && cinfo->type == CREATURE_TYPE_DEMON)
                    {
                        // to prevent client crash
                        SetFlag(UNIT_FIELD_BYTES_0, 2048);

                        // just to enable stat window
                        if (GetCharmInfo())
                            GetCharmInfo()->SetPetNumber(sObjectMgr.GeneratePetNumber(), true);

                        // if charmed two demons the same session, the 2nd gets the 1st one's name
                        SetUInt32Value(UNIT_FIELD_PET_NAME_TIMESTAMP, uint32(time(NULL))); // cast can't be helped
                    }
                }
                charmer->ToPlayer()->CharmSpellInitialize();
                break;
            default:
                break;
        }
    }
}

void Unit::RemoveCharmedBy(Unit* charmer)
{
    if (!isCharmed())
        return;

    if (!charmer)
        charmer = GetCharmer();
    if (charmer != GetCharmer()) // one aura overrides another?
        return;

    CharmType type;
    if (HasUnitState(UNIT_STATE_POSSESSED))
        type = CHARM_TYPE_POSSESS;
    else
        type = CHARM_TYPE_CHARM;

    GetMotionMaster()->MovementExpired(true);

    CastStop();
    CombatStop(); //@todo CombatStop(true) may cause crash (interrupt spells)
    getHostileRefManager().deleteReferences();
    DeleteThreatList();

    if (_oldFactionId)
    {
        SetFaction(_oldFactionId);
        _oldFactionId = 0;
    }
    else
        RestoreFaction();

    // If charmer still exists
    if (!charmer)
        return;

    if (Creature* creature = ToCreature())
    {
        // Creature will restore its old AI on next update
        if (creature->AI())
            creature->AI()->OnCharmed(false);

        LastCharmerGUID = charmer->GetGUID();
    }

    ASSERT(type != CHARM_TYPE_POSSESS || charmer->GetTypeId() == TYPEID_PLAYER);

    charmer->SetCharm(this, false);
    SetCharmerGUID(0);

    if (charmer->GetTypeId() == TYPEID_PLAYER)
    {
        switch (type)
        {
            case CHARM_TYPE_POSSESS:
                charmer->ToPlayer()->SetClientControl(charmer, true);
                charmer->ToPlayer()->SetViewpoint(this, false);
                charmer->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_REMOVE_CLIENT_CONTROL);
                RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED);
                ClearUnitState(UNIT_STATE_POSSESSED);
                break;
            case CHARM_TYPE_CHARM:
                if (GetTypeId() == TYPEID_UNIT && charmer->getClass() == CLASS_WARLOCK)
                {
                    CreatureInfo const* cinfo = ToCreature()->GetCreatureTemplate();
                    if (cinfo && cinfo->type == CREATURE_TYPE_DEMON)
                    {
                        SetByteValue(UNIT_FIELD_BYTES_0, 1, uint8(cinfo->unit_class));

                        if (GetCharmInfo())
                            GetCharmInfo()->SetPetNumber(0, true);
                        else
                            sLog.outError("Aura::HandleModCharm: target=" UI64FMTD " with typeid=%d has a charm aura but no charm info!", GetGUID(), GetTypeId());
                    }
                }
                break;
            default:
                break;
        }
    }

    // if charmed target was player, give him controls back
    if (ToPlayer())
        ToPlayer()->SetClientControl(this, true);

    if (GetTypeId() == TYPEID_PLAYER || (GetTypeId() == TYPEID_UNIT && !IsGuardian()))
        DeleteCharmInfo();

    if (Player* playerCharmer = charmer->ToPlayer())
    {
        if (this != charmer->GetFirstControlled())
            charmer->ToPlayer()->SendRemoveControlBar();
        SendHealthUpdateDueToCharm(playerCharmer);
    }
}

void Unit::RestoreFaction()
{
    if (GetTypeId() == TYPEID_PLAYER)
        ToPlayer()->setFactionForRace(getRace());
    else
    {
        CreatureInfo const* cinfo = ToCreature()->GetCreatureTemplate();

        if (IsPet())
        {
            if (Unit* owner = GetOwner())
                SetFaction(owner->GetFaction());
            else if (cinfo)
                SetFaction(cinfo->faction);
        }
        else if (cinfo)  // normal creature
            SetFaction(cinfo->faction);
    }
}

bool Unit::IsInPartyWith(Unit const* unit) const
{
    if (this == unit)
        return true;

    const Unit* u1 = GetCharmerOrOwnerOrSelf();
    const Unit* u2 = unit->GetCharmerOrOwnerOrSelf();
    if (u1 == u2)
        return true;

    if (u1->GetTypeId() == TYPEID_PLAYER && u2->GetTypeId() == TYPEID_PLAYER)
        return u1->ToPlayer()->IsInSameGroupWith(u2->ToPlayer());
    else
        return false;
}

bool Unit::IsInRaidWith(Unit const* unit) const
{
    if (this == unit)
        return true;

    const Unit* u1 = GetCharmerOrOwnerOrSelf();
    const Unit* u2 = unit->GetCharmerOrOwnerOrSelf();
    if (u1 == u2)
        return true;

    if (u1->GetTypeId() == TYPEID_PLAYER && u2->GetTypeId() == TYPEID_PLAYER)
        return u1->ToPlayer()->IsInSameRaidWith(u2->ToPlayer());
    else
        return false;
}

void Unit::GetRaidMember(std::list<Unit*>& nearMembers, float radius)
{
    Player* owner = GetCharmerOrOwnerPlayerOrPlayerItself();
    if (!owner)
        return;

    Group* pGroup = owner->GetGroup();
    if (!pGroup)
        return;

    for (GroupReference* itr = pGroup->GetFirstMember(); itr != NULL; itr = itr->next())
    {
        if (Player* Target = itr->GetSource())
        {
            // IsHostileTo check duel and controlled by enemy
            if (Target != this && IsWithinDistInMap(Target, radius) && Target->IsAlive() && !IsHostileTo(Target))
                nearMembers.push_back(Target);
        }
    }
}

void Unit::GetPartyMember(std::list<Unit*>& TagUnitMap, float radius)
{
    Unit* owner = GetCharmerOrOwnerOrSelf();
    Group* pGroup = NULL;
    if (owner->GetTypeId() == TYPEID_PLAYER)
        pGroup = owner->ToPlayer()->GetGroup();

    if (pGroup)
    {
        uint8 subgroup = owner->ToPlayer()->GetSubGroup();

        for (GroupReference* itr = pGroup->GetFirstMember(); itr != NULL; itr = itr->next())
        {
            Player* Target = itr->GetSource();

            // IsHostileTo check duel and controlled by enemy
            if (Target && Target->IsInMap(owner) && Target->GetSubGroup() == subgroup && !IsHostileTo(Target))
            {
                if (Target->IsAlive() && IsWithinDist(Target, radius))
                    TagUnitMap.push_back(Target);

                if (Guardian* pet = Target->GetGuardianPet())
                    if (pet->IsAlive() &&  IsWithinDist(pet, radius))
                        TagUnitMap.push_back(pet);
            }
        }
    }
    else
    {
        if ((owner == this || IsInMap(owner)) && owner->IsAlive())
            TagUnitMap.push_back(owner);
        if (Guardian* pet = owner->GetGuardianPet())
            if ((pet == this || IsInMap(pet)) && pet->IsAlive())
                TagUnitMap.push_back(pet);
    }
}

bool Unit::HasShapeshiftChangingModel() const
{
    AuraList const& auras = GetAurasByType(SPELL_AURA_MOD_SHAPESHIFT);
    for (AuraList::const_iterator i = auras.begin(); i != auras.end(); i++)
    {
        switch ((*i)->GetModifier()->m_miscvalue)
        {
            case FORM_CAT:
            case FORM_TREE:
            case FORM_TRAVEL:
            case FORM_AQUA:
            case FORM_BEAR:
            case FORM_DIREBEAR:
            case FORM_MOONKIN:
            case FORM_FLIGHT:
            case FORM_FLIGHT_EPIC:
            case FORM_SPIRITOFREDEMPTION:
            case FORM_GHOSTWOLF:
            case FORM_GHOUL:
            case FORM_CREATURECAT:
            case FORM_CREATUREBEAR:
                return true;
            case FORM_BATTLESTANCE:
            case FORM_DEFENSIVESTANCE:
            case FORM_BERSERKERSTANCE:
            case FORM_AMBIENT:
            case FORM_SHADOW:
            case FORM_STEALTH:
                return false;
        }
    }

    return false;
}

void Unit::AddAura(uint32 spellId, Unit* target)
{
    if (!target)
        return;

    SpellEntry const* spellInfo = sSpellStore.LookupEntry(spellId);
    if (!spellInfo)
        return;

    if (!target->IsAlive() && !(spellInfo->AttributesEx & SPELL_ATTR0_PASSIVE) && !(spellInfo->AttributesEx2 & SPELL_ATTR2_CAN_TARGET_DEAD))
        return;

    if (target->IsImmuneToSpell(spellInfo))
        return;

    for (uint32 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        if (spellInfo->Effect[i] == SPELL_EFFECT_APPLY_AURA)
        {
            if (target->IsImmuneToSpellEffect(spellInfo, i, false))
                continue;

            if (spellInfo->EffectImplicitTargetA[i] == TARGET_UNIT_CASTER)
            {
                Aura* Aur = CreateAura(spellInfo, i, NULL, this, this);
                AddAura(Aur);
            }
            else
            {
                Aura* Aur = CreateAura(spellInfo, i, NULL, target, this);
                target->AddAura(Aur);
            }
        }
    }
}

void Unit::UpdateObjectVisibility(bool forced)
{
    if (!forced)
        AddToNotify(NOTIFY_VISIBILITY_CHANGED);
    else
    {
        WorldObject::UpdateObjectVisibility(true);
        // call MoveInLineOfSight for nearby creatures
        Oregon::AIRelocationNotifier notifier(*this);
        VisitNearbyObject(GetVisibilityRange(), notifier);
    }
}

void CharmInfo::SetIsCommandAttack(bool val)
{
    m_isCommandAttack = val;
}

bool CharmInfo::IsCommandAttack()
{
    return m_isCommandAttack;
}

void CharmInfo::SetIsCommandFollow(bool val)
{
    _isCommandFollow = val;
}

bool CharmInfo::IsCommandFollow()
{
    return _isCommandFollow;
}

void CharmInfo::SaveStayPosition()
{
    m_unit->GetPosition(m_stayX, m_stayY, m_stayZ);
}

void CharmInfo::GetStayPosition(float& x, float& y, float& z)
{
    x = m_stayX;
    y = m_stayY;
    z = m_stayZ;
}

void CharmInfo::SetIsAtStay(bool val)
{
    m_isAtStay = val;
}

bool CharmInfo::IsAtStay()
{
    return m_isAtStay;
}

void CharmInfo::SetIsFollowing(bool val)
{
    m_isFollowing = val;
}

bool CharmInfo::IsFollowing()
{
    return m_isFollowing;
}

void CharmInfo::SetIsReturning(bool val)
{
    m_isReturning = val;
}

bool CharmInfo::IsReturning()
{
    return m_isReturning;
}

void Unit::BuildMovementPacket(ByteBuffer *data) const
{
    *data << uint32(GetUnitMovementFlags());            // movement flags
    *data << uint8(0);                                  // 2.3.0 @todo: Implement extra movement flags
    *data << uint32(getMSTime());                       // time / counter
    *data << float(GetPositionX());
    *data << float(GetPositionY());
    *data << float(GetPositionZ());
    *data << float(GetOrientation());
    *data << uint32(0);
}

void Unit::UpdateSplineMovement(uint32 t_diff)
{
    if (movespline->Finalized())
        return;

    movespline->updateState(t_diff);
    bool arrived = movespline->Finalized();

    if (arrived)
        DisableSpline();

    m_movesplineTimer.Update(t_diff);
    if (m_movesplineTimer.Passed() || arrived)
    {
        m_movesplineTimer.Reset(POSITION_UPDATE_DELAY);
        Movement::Location loc = movespline->ComputePosition();

        if (HasUnitState(UNIT_STATE_CANNOT_TURN))
            loc.orientation = GetOrientation();
        
        SetPosition(loc.x, loc.y, loc.z, loc.orientation);
        
        if (GetTypeId() == TYPEID_UNIT)
            ToCreature()->UpdateMovementFlags(true);
    }
}

void Unit::DisableSpline()
{
    m_movementInfo.RemoveMovementFlag(MovementFlags(MOVEMENTFLAG_SPLINE_ENABLED | MOVEMENTFLAG_FORWARD));
    movespline->_Interrupt();
}

void Unit::MonsterMoveWithSpeed(float x, float y, float z, float speed, bool generatePath, bool forceDestination)
{
    Movement::MoveSplineInit init(*this);
    init.MoveTo(x, y, z, generatePath, forceDestination);
    init.SetVelocity(speed);
    init.Launch();
}

void Unit::SendMonsterMoveWithSpeedToCurrentDestination(float speed)
{
    float x, y, z;
    if (movespline->Finalized())
        return;

    Vector3 dest = movespline->FinalDestination();
    x = dest.x;
    y = dest.y;
    z = dest.z;

    MonsterMoveWithSpeed(x, y, z, speed, true);
}

bool Unit::IsFalling() const
{
    return m_movementInfo.HasMovementFlag((MovementFlags)(MOVEMENTFLAG_FALLING | MOVEMENTFLAG_FALLINGFAR)) || movespline->isFalling();
}

bool Unit::SetWalk(bool enable)
{
    if (enable == IsWalking())
        return false;

    if (enable)
        AddUnitMovementFlag(MOVEMENTFLAG_WALK_MODE);
    else
        RemoveUnitMovementFlag(MOVEMENTFLAG_WALK_MODE);

    return true;
}

bool Unit::SetLevitate(bool apply, bool /*packetOnly = false */)
{
    if (apply == IsLevitating())
        return false;

    if (apply)
        AddUnitMovementFlag(MOVEMENTFLAG_LEVITATING);
    else
        RemoveUnitMovementFlag(MOVEMENTFLAG_LEVITATING);

    return true;
}

bool Unit::SetSwim(bool enable)
{
    if (enable == HasUnitMovementFlag(MOVEMENTFLAG_SWIMMING))
        return false;

    if (enable)
        AddUnitMovementFlag(MOVEMENTFLAG_SWIMMING);
    else
        RemoveUnitMovementFlag(MOVEMENTFLAG_SWIMMING);

    return true;
}

bool Unit::SetCanFly(bool enable, bool /*packetOnly = false */)
{
    if (enable == HasUnitMovementFlag(MOVEMENTFLAG_FLYING))
        return false;

    if (enable)
    {
        AddUnitMovementFlag(MOVEMENTFLAG_CAN_FLY);
        RemoveUnitMovementFlag(MOVEMENTFLAG_FALLING);
    }
    else
        RemoveUnitMovementFlag(MOVEMENTFLAG_FLYING | MOVEMENTFLAG_MASK_MOVING_FLY);

    return true;
}

bool Unit::SetWaterWalking(bool enable, bool /*packetOnly = false */)
{
    if (enable == HasUnitMovementFlag(MOVEMENTFLAG_WATERWALKING))
        return false;

    if (enable)
        AddUnitMovementFlag(MOVEMENTFLAG_WATERWALKING);
    else
        RemoveUnitMovementFlag(MOVEMENTFLAG_WATERWALKING);

    return true;
}

bool Unit::SetFeatherFall(bool enable, bool /*packetOnly = false */)
{
    if (enable == HasUnitMovementFlag(MOVEMENTFLAG_SAFE_FALL))
        return false;

    if (enable)
        AddUnitMovementFlag(MOVEMENTFLAG_SAFE_FALL);
    else
        RemoveUnitMovementFlag(MOVEMENTFLAG_SAFE_FALL);

    return true;
}

bool Unit::SetHover(bool enable, bool /*packetOnly = false*/)
{
    if (enable == HasUnitMovementFlag(MOVEMENTFLAG_HOVER))
        return false;

    if (enable)
        AddUnitMovementFlag(MOVEMENTFLAG_HOVER);
    else
        RemoveUnitMovementFlag(MOVEMENTFLAG_HOVER);

    return true;
}
