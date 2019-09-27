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
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Opcodes.h"
#include "Log.h"
#include "UpdateMask.h"
#include "World.h"
#include "ObjectMgr.h"
#include "SpellMgr.h"
#include "Player.h"
#include "Pet.h"
#include "Unit.h"
#include "Spell.h"
#include "DynamicObject.h"
#include "SpellAuras.h"
#include "UpdateData.h"
#include "ObjectAccessor.h"
#include "CellImpl.h"
#include "SharedDefines.h"
#include "LootMgr.h"
#include "VMapFactory.h"
#include "Battleground.h"
#include "Utilities/Util.h"
#include "TemporarySummon.h"
#include "GameEventMgr.h"
#include "DisableMgr.h"
#include "ConditionMgr.h"
#include "MoveMap.h"

#define SPELL_CHANNEL_UPDATE_INTERVAL (1*IN_MILLISECONDS)

extern pEffect SpellEffects[TOTAL_SPELL_EFFECTS];

bool IsQuestTameSpell(uint32 spellId)
{
    SpellEntry const* spellproto = sSpellStore.LookupEntry(spellId);
    if (!spellproto) return false;

    return spellproto->Effect[0] == SPELL_EFFECT_THREAT
           && spellproto->Effect[1] == SPELL_EFFECT_APPLY_AURA && spellproto->EffectApplyAuraName[1] == SPELL_AURA_DUMMY;
}

SpellCastTargets::SpellCastTargets()
{
    m_unitTarget = NULL;
    m_itemTarget = NULL;
    m_GOTarget   = NULL;

    m_unitTargetGUID   = 0;
    m_GOTargetGUID     = 0;
    m_CorpseTargetGUID = 0;
    m_itemTargetGUID   = 0;
    m_itemTargetEntry  = 0;

    m_srcPos.Relocate(0, 0, 0, 0);
    m_strTarget = "";
    m_targetMask = 0;
}

SpellCastTargets::~SpellCastTargets()
{
}

void SpellCastTargets::setUnitTarget(Unit* target)
{
    if (!target)
        return;

    m_unitTarget = target;
    m_unitTargetGUID = target->GetGUID();
    m_targetMask |= TARGET_FLAG_UNIT;
}

void SpellCastTargets::setSrc(float x, float y, float z)
{
    m_srcPos.Relocate(x, y, z);
    m_targetMask |= TARGET_FLAG_SOURCE_LOCATION;
}

void SpellCastTargets::setSrc(Position* pos)
{
    if (pos)
    {
        m_srcPos.Relocate(pos);
        m_targetMask |= TARGET_FLAG_SOURCE_LOCATION;
    }
}

void SpellCastTargets::setDst(float x, float y, float z, float orientation, uint32 mapId)
{
    m_dstPos.Relocate(x, y, z, orientation);
    m_targetMask |= TARGET_FLAG_DEST_LOCATION;
    if (mapId != MAPID_INVALID)
        m_dstPos.m_mapId = mapId;
}

void SpellCastTargets::setDst(Position* pos)
{
    if (pos)
    {
        m_dstPos.Relocate(pos);
        m_targetMask |= TARGET_FLAG_DEST_LOCATION;
    }
}

void SpellCastTargets::setGOTarget(GameObject* target)
{
    m_GOTarget = target;
    m_GOTargetGUID = target->GetGUID();
    //    m_targetMask |= TARGET_FLAG_OBJECT;
}

void SpellCastTargets::setItemTarget(Item* item)
{
    if (!item)
        return;

    m_itemTarget = item;
    m_itemTargetGUID = item->GetGUID();
    m_itemTargetEntry = item->GetEntry();
    m_targetMask |= TARGET_FLAG_ITEM;
}

void SpellCastTargets::setCorpseTarget(Corpse* corpse)
{
    m_CorpseTargetGUID = corpse->GetGUID();
}

void SpellCastTargets::Update(Unit* caster)
{
    m_GOTarget   = !m_GOTargetGUID.IsEmpty() ? caster->GetMap()->GetGameObject(m_GOTargetGUID.GetRawValue()) : NULL;
    m_unitTarget = !m_unitTargetGUID.IsEmpty() ?
                   (m_unitTargetGUID.GetRawValue() == caster->GetGUID() ? caster : ObjectAccessor::GetUnit(*caster, m_unitTargetGUID.GetRawValue())) :
                   NULL;

    m_itemTarget = NULL;
    if (caster->GetTypeId() == TYPEID_PLAYER)
    {
        if (m_targetMask & TARGET_FLAG_ITEM)
            m_itemTarget = caster->ToPlayer()->GetItemByGuid(m_itemTargetGUID.GetRawValue());
        else if (m_targetMask & TARGET_FLAG_TRADE_ITEM)
        {
            // here it is not guid but slot. Also prevent hacking slots
            Player* pTrader = caster->ToPlayer()->GetTrader();
            if (pTrader && m_itemTargetGUID.GetRawValue() < TRADE_SLOT_COUNT)
                m_itemTarget = pTrader->GetItemByTradeSlot(m_itemTargetGUID.GetRawValue());
        }
        if (m_itemTarget)
            m_itemTargetEntry = m_itemTarget->GetEntry();
    }
}

void SpellCastTargets::read(ByteBuffer& data, Unit* caster)
{
    data >> m_targetMask;

    if (m_targetMask == TARGET_FLAG_SELF)
        return;

    // TARGET_FLAG_UNK2 is used for non-combat pets, maybe other?
    if (m_targetMask & (TARGET_FLAG_UNIT | TARGET_FLAG_UNK2))
        data >> m_unitTargetGUID.ReadAsPacked();

    if (m_targetMask & (TARGET_FLAG_OBJECT | TARGET_FLAG_OBJECT_UNK | TARGET_FLAG_GAMEOBJECT_ITEM))
        data >> m_GOTargetGUID.ReadAsPacked();

    if ((m_targetMask & (TARGET_FLAG_ITEM | TARGET_FLAG_TRADE_ITEM)) && caster->GetTypeId() == TYPEID_PLAYER)
        data >> m_itemTargetGUID.ReadAsPacked();

    if (m_targetMask & TARGET_FLAG_SOURCE_LOCATION)
    {
        data >> m_srcPos.m_positionX >> m_srcPos.m_positionY >> m_srcPos.m_positionZ;
        if (!m_srcPos.IsPositionValid())
            throw ByteBufferException(false, data.rpos(), 0, data.size());
    }

    if (m_targetMask & TARGET_FLAG_DEST_LOCATION)
    {
        data >> m_dstPos.m_positionX >> m_dstPos.m_positionY >> m_dstPos.m_positionZ;
        if (!m_dstPos.IsPositionValid())
            throw ByteBufferException(false, data.rpos(), 0, data.size());
    }

    if (m_targetMask & TARGET_FLAG_STRING)
        data >> m_strTarget;

    if (m_targetMask & (TARGET_FLAG_CORPSE | TARGET_FLAG_PVP_CORPSE))
        data >> m_CorpseTargetGUID.ReadAsPacked();

    // find real units/GOs
    Update(caster);
}

void SpellCastTargets::write (ByteBuffer& data) const
{
    data << uint32(m_targetMask);

    if (m_targetMask & (TARGET_FLAG_UNIT | TARGET_FLAG_PVP_CORPSE | TARGET_FLAG_OBJECT | TARGET_FLAG_CORPSE | TARGET_FLAG_UNK2))
    {
        if (m_targetMask & TARGET_FLAG_UNIT)
        {
            if (m_unitTarget)
                data << m_unitTarget->GetPackGUID();
            else
                data << uint8(0);
        }
        else if (m_targetMask & (TARGET_FLAG_OBJECT | TARGET_FLAG_OBJECT_UNK))
        {
            if (m_GOTarget)
                data << m_GOTarget->GetPackGUID();
            else
                data << uint8(0);
        }
        else if (m_targetMask & (TARGET_FLAG_CORPSE | TARGET_FLAG_PVP_CORPSE))
            data << m_CorpseTargetGUID.WriteAsPacked();
        else
            data << uint8(0);
    }

    if (m_targetMask & (TARGET_FLAG_ITEM | TARGET_FLAG_TRADE_ITEM))
    {
        if (m_itemTarget)
            data << m_itemTarget->GetPackGUID();
        else
            data << uint8(0);
    }

    if (m_targetMask & TARGET_FLAG_SOURCE_LOCATION)
        data << m_srcPos.m_positionX << m_srcPos.m_positionY << m_srcPos.m_positionZ;

    if (m_targetMask & TARGET_FLAG_DEST_LOCATION)
        data << m_dstPos.m_positionX << m_dstPos.m_positionY << m_dstPos.m_positionZ;

    if (m_targetMask & TARGET_FLAG_STRING)
        data << m_strTarget;
}

Spell::Spell(Unit* Caster, SpellEntry const* info, bool triggered, uint64 originalCasterGUID, Spell** triggeringContainer, bool skipCheck)
    : m_spellInfo(info)
    , m_caster(Caster)
    , m_spellValue(new SpellValue(m_spellInfo))
{
    m_customAttr = sSpellMgr.GetSpellCustomAttr(m_spellInfo->Id);
    m_skipCheck = skipCheck;
    m_selfContainer = NULL;
    m_triggeringContainer = triggeringContainer;
    m_referencedFromCurrentSpell = false;
    m_executedCurrently = false;
    m_needComboPoints = !triggered && NeedsComboPoints(m_spellInfo);
    m_delayStart = 0;
    m_delayAtDamageCount = 0;
    m_destroyed = false;

    m_applyMultiplierMask = 0;

    // Get data for type of attack
    switch (m_spellInfo->DmgClass)
    {
    case SPELL_DAMAGE_CLASS_MELEE:
        if (m_spellInfo->AttributesEx3 & SPELL_ATTR3_REQ_OFFHAND)
            m_attackType = OFF_ATTACK;
        else
            m_attackType = BASE_ATTACK;
        break;
    case SPELL_DAMAGE_CLASS_RANGED:
        m_attackType = RANGED_ATTACK;
        break;
    default:
        // Wands
        if (m_spellInfo->AttributesEx2 & SPELL_ATTR2_AUTOREPEAT_FLAG)
            m_attackType = RANGED_ATTACK;
        else
            m_attackType = BASE_ATTACK;
        break;
    }

    m_spellSchoolMask = GetSpellSchoolMask(info);           // Can be override for some spell (wand shoot for example)

    if (m_attackType == RANGED_ATTACK)
        // wand case
        if ((m_caster->getClassMask() & CLASSMASK_WAND_USERS) != 0 && m_caster->GetTypeId() == TYPEID_PLAYER)
            if (Item* pItem = m_caster->ToPlayer()->GetWeaponForAttack(RANGED_ATTACK))
                m_spellSchoolMask = SpellSchoolMask(1 << pItem->GetProto()->Damage->DamageType);

    // Set health leech amount to zero
    m_healthLeech = 0;

    if (originalCasterGUID)
        m_originalCasterGUID = originalCasterGUID;
    else
        m_originalCasterGUID = m_caster->GetGUID();

    if (m_originalCasterGUID == m_caster->GetGUID())
        m_originalCaster = m_caster;
    else
    {
        m_originalCaster = ObjectAccessor::GetUnit(*m_caster, m_originalCasterGUID);
        if (m_originalCaster && !m_originalCaster->IsInWorld())
            m_originalCaster = NULL;
    }

    for (int i = 0; i < 3; ++i)
        m_currentBasePoints[i] = m_spellValue->EffectBasePoints[i];

    m_spellState = SPELL_STATE_NULL;

    m_castPositionX = m_castPositionY = m_castPositionZ = 0;
    m_TriggerSpells.clear();
    m_IsTriggeredSpell = bool(triggered || info->AttributesEx4 & SPELL_ATTR4_TRIGGERED);
    //m_AreaAura = false;
    m_CastItem = NULL;
    m_castItemGUID = 0;

    unitTarget = NULL;
    itemTarget = NULL;
    gameObjTarget = NULL;
    focusObject = NULL;
    m_cast_count = 0;
    m_triggeredByAuraSpell  = NULL;
    m_pathFinder = NULL;

    //Auto Shot & Shoot
    m_autoRepeat = IsAutoRepeatRangedSpell(m_spellInfo);

    m_powerCost = 0;                                        // setup to correct value in Spell::prepare, don't must be used before.
    m_casttime = 0;                                         // setup to correct value in Spell::prepare, don't must be used before.
    m_timer = 0;                                            // will set to castime in prepare

    m_needAliveTargetMask = 0;

    // Determine if spell can be reflected back to the caster
    // Patch 1.2 notes: Spell Reflection no longer reflects abilities
    m_canReflect = m_spellInfo->DmgClass == SPELL_DAMAGE_CLASS_MAGIC && !(m_spellInfo->Attributes & SPELL_ATTR0_ABILITY)
        && !(m_spellInfo->AttributesEx & SPELL_ATTR1_CANT_BE_REFLECTED) && !(m_spellInfo->Attributes & SPELL_ATTR0_UNAFFECTED_BY_INVULNERABILITY)
        && !IsPassiveSpell(m_spellInfo) && !IsPositiveSpell(m_spellInfo->Id);

    m_isNeedSendToClient = m_spellInfo->SpellVisual != 0 || IsChanneledSpell(m_spellInfo) ||
                           m_spellInfo->speed > 0.0f || (!m_triggeredByAuraSpell && !m_IsTriggeredSpell);

    CleanupTargetList();
}

Spell::~Spell()
{
    m_destroyed = true;

    delete m_spellValue;
    delete m_pathFinder;
}

void ResizeUnitListByDistance(std::list<Unit*> &_list, WorldObject* source, uint32 _size, bool _keepnearest)
{   
    float d;
    std::list<Unit*>::iterator _i;
    ASSERT(_size >= 0);
    while(_list.size() > _size)
    {
        d = source->GetDistance((*_list.begin()));
        _i = _list.begin();
        for(std::list<Unit*>::iterator itr = _list.begin(); itr != _list.end(); itr++)
        {
            if((_keepnearest && source->GetDistance(*itr) > d) || (!_keepnearest && source->GetDistance(*itr) < d))
            {
                d = source->GetDistance(*itr);
                _i = itr;
            }
        }
        _list.erase(_i);
    }
}

void Spell::FillTargetMap()
{
    for (uint32 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        // not call for empty effect.
        // Also some spells use not used effect targets for store targets for dummy effect in triggered spells
        if (!m_spellInfo->Effect[i])
            continue;

        uint32 effectTargetType = sSpellMgr.EffectTargetType[m_spellInfo->Effect[i]];

        // is it possible that areaaura is not applied to caster?
        if (effectTargetType == SPELL_REQUIRE_NONE)
            continue;

        uint32 targetA = m_spellInfo->EffectImplicitTargetA[i];
        uint32 targetB = m_spellInfo->EffectImplicitTargetB[i];

        if (targetA)
            SetTargetMap(i, targetA);
        if (targetB) // In very rare case !A && B
            SetTargetMap(i, targetB);

        if (effectTargetType != SPELL_REQUIRE_UNIT)
        {
            if (effectTargetType == SPELL_REQUIRE_CASTER)
                AddUnitTarget(m_caster, i);
            else if (effectTargetType == SPELL_REQUIRE_ITEM)
            {
                if (m_targets.getItemTarget())
                    AddItemTarget(m_targets.getItemTarget(), i);
            }
            continue;
        }

        if (!targetA && !targetB)
        {
            // add here custom effects that need default target.
            // FOR EVERY TARGET TYPE THERE IS A DIFFERENT FILL!!
            switch (m_spellInfo->Effect[i])
            {
            case SPELL_EFFECT_DUMMY:
                {
                    switch (m_spellInfo->Id)
                    {
                    case 20577:                         // Cannibalize
                        {
                            // non-standard target selection
                            SpellRangeEntry const* srange = sSpellRangeStore.LookupEntry(m_spellInfo->rangeIndex);
                            float max_range = GetSpellMaxRange(srange);
                            WorldObject* result = NULL;

                            Oregon::CannibalizeObjectCheck u_check(m_caster, max_range);
                            Oregon::WorldObjectSearcher<Oregon::CannibalizeObjectCheck > searcher(result, u_check);
                            m_caster->VisitNearbyGridObject(max_range, searcher);
                            if (!result)
                                m_caster->VisitNearbyWorldObject(max_range, searcher);


                            if (result)
                            {
                                switch (result->GetTypeId())
                                {
                                case TYPEID_UNIT:
                                case TYPEID_PLAYER:
                                    AddUnitTarget((Unit*)result, i);
                                    break;
                                case TYPEID_CORPSE:
                                    m_targets.setCorpseTarget((Corpse*)result);
                                    if (Player* owner = ObjectAccessor::FindPlayer(((Corpse*)result)->GetOwnerGUID()))
                                        AddUnitTarget(owner, i);
                                    break;
                                }
                            }
                            else
                            {
                                // clear cooldown at fail
                                if (m_caster->GetTypeId() == TYPEID_PLAYER)
                                    m_caster->ToPlayer()->RemoveSpellCooldown(m_spellInfo->Id, true);
                                SendCastResult(SPELL_FAILED_NO_EDIBLE_CORPSES);
                                finish(false);
                            }
                            break;
                        }
                    default:
                        if (m_targets.getUnitTarget())
                            AddUnitTarget(m_targets.getUnitTarget(), i);
                        break;
                    }
                    break;
                }
            case SPELL_EFFECT_BIND:
            case SPELL_EFFECT_RESURRECT:
            case SPELL_EFFECT_CREATE_ITEM:
            case SPELL_EFFECT_TRIGGER_SPELL:
            case SPELL_EFFECT_SKILL_STEP:
            case SPELL_EFFECT_SELF_RESURRECT:
            case SPELL_EFFECT_REPUTATION:
            case SPELL_EFFECT_LEARN_SPELL:
                if (m_targets.getUnitTarget())
                    AddUnitTarget(m_targets.getUnitTarget(), i);
                else
                    AddUnitTarget(m_caster, i);
                break;
            case SPELL_EFFECT_SUMMON_PLAYER:
                if (m_caster->GetTypeId() == TYPEID_PLAYER && m_caster->ToPlayer()->GetSelection())
                {
                    Player* target = sObjectMgr.GetPlayer(m_caster->ToPlayer()->GetSelection());
                    if (target)
                        AddUnitTarget(target, i);
                }
                break;
            case SPELL_EFFECT_RESURRECT_NEW:
                if (m_targets.getUnitTarget())
                    AddUnitTarget(m_targets.getUnitTarget(), i);
                if (m_targets.getCorpseTargetGUID())
                {
                    Corpse* corpse = ObjectAccessor::GetCorpse(*m_caster, m_targets.getCorpseTargetGUID());
                    if (corpse)
                    {
                        Player* owner = ObjectAccessor::FindPlayer(corpse->GetOwnerGUID());
                        if (owner)
                            AddUnitTarget(owner, i);
                    }
                }
                break;
            case SPELL_EFFECT_SUMMON_CHANGE_ITEM:
            case SPELL_EFFECT_ADD_FARSIGHT:
            case SPELL_EFFECT_STUCK:
            case SPELL_EFFECT_DESTROY_ALL_TOTEMS:
                AddUnitTarget(m_caster, i);
                break;
            case SPELL_EFFECT_LEARN_PET_SPELL:
                if (Guardian* pet = m_caster->GetGuardianPet())
                    AddUnitTarget(pet, i);
                break;
            /*case SPELL_EFFECT_ENCHANT_ITEM:
            case SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY:
            case SPELL_EFFECT_DISENCHANT:
            case SPELL_EFFECT_FEED_PET:
            case SPELL_EFFECT_PROSPECTING:
                if (m_targets.getItemTarget())
                    AddItemTarget(m_targets.getItemTarget(), i);
                break;*/
            case SPELL_EFFECT_APPLY_AURA:
                AddUnitTarget(m_caster, i);
                break;
            case SPELL_EFFECT_APPLY_AREA_AURA_PARTY:
                // AreaAura
                if (m_spellInfo->Attributes == 0x9050000 || m_spellInfo->Attributes == 0x10000)
                    SetTargetMap(i, TARGET_UNIT_PARTY_TARGET);
                break;
            case SPELL_EFFECT_SKIN_PLAYER_CORPSE:
                if (m_targets.getUnitTarget())
                    AddUnitTarget(m_targets.getUnitTarget(), i);
                else if (m_targets.getCorpseTargetGUID())
                {
                    Corpse* corpse = ObjectAccessor::GetCorpse(*m_caster, m_targets.getCorpseTargetGUID());
                    if (corpse)
                    {
                        Player* owner = ObjectAccessor::FindPlayer(corpse->GetOwnerGUID());
                        if (owner)
                            AddUnitTarget(owner, i);
                    }
                }
                break;
            case SPELL_EFFECT_SUMMON_FRIEND:
                if (!m_caster->ToPlayer())
                    break;

                // Special message for this Spell (if not ready)
                if (m_caster->ToPlayer()->HasSpellCooldown(m_spellInfo->Id))
                {
                    m_caster->ToPlayer()->SendReferFriendError(RAF_ERR_SUMMON_NOT_READY);
                    finish(false);
                    return;
                }

                if (sObjectMgr.GetRAFLinkedBuddyForPlayer(m_caster->ToPlayer()))
                    AddUnitTarget(m_caster, i);
                break;
            default:
                break;
            }
        }
    }

    if (m_targets.HasDst())
    {
        if (m_spellInfo->speed > 0.0f)
        {
            float dist = m_caster->GetDistance(m_targets.m_dstPos);
            if (dist < 5.0f) dist = 5.0f;
            m_delayMoment = (uint64) floor(dist / m_spellInfo->speed * 1000.0f);
        }
    }

    // m_targets is originally sent by client and its values are used
    // in single target casts. It may occur that we have set implicit
    // targets but we need to change m_targets too for those checks

    if (!m_targets.getUnitTarget() && m_UniqueTargetInfo.size())
        if (uint64 targetGUID = (*m_UniqueTargetInfo.begin()).targetGUID)
            if (Unit* target = m_caster->GetGUID() == targetGUID ? m_caster : ObjectAccessor::GetUnit(*m_caster, targetGUID))
                m_targets.setUnitTarget(target);

    if (!m_targets.getGOTarget() && m_UniqueGOTargetInfo.size())
        if (uint64 targetGUID = (*m_UniqueGOTargetInfo.begin()).targetGUID)
            if (GameObject* target = m_caster->GetMap()->GetGameObject(targetGUID))
                m_targets.setGOTarget(target);

    if (!m_targets.getItemTarget() && m_UniqueItemInfo.size())
        if (Item* target = (*m_UniqueItemInfo.begin()).item)
            m_targets.setItemTarget(target);
}

void Spell::prepareDataForTriggerSystem()
{
    //==========================================================================================
    // Now fill data for trigger system, need know:
    // can spell trigger another or not (m_canTrigger)
    // Create base triggers flags for Attacker and Victim (m_procAttacker and m_procVictim)
    //==========================================================================================

    // Fill flag can spell trigger or not
    if (!m_IsTriggeredSpell)
        m_canTrigger = true;          // Normal cast - can trigger
    else if (!m_triggeredByAuraSpell)
        m_canTrigger = true;          // Triggered from SPELL_EFFECT_TRIGGER_SPELL - can trigger
    else                              // Exceptions (some periodic triggers)
    {
        m_canTrigger = false;         // Triggered spells can`t trigger another
        switch (m_spellInfo->SpellFamilyName)
        {
        case SPELLFAMILY_MAGE:    // Arcane Missles / Blizzard / Molten Armor triggers need do it
            if (m_spellInfo->SpellFamilyFlags & 0x0000000800200080LL) m_canTrigger = true;
            break;
        case SPELLFAMILY_WARLOCK: // For Hellfire Effect / Rain of Fire / Soul Fire triggers need do it
            if (m_spellInfo->SpellFamilyFlags & 0x0000800000000060LL) m_canTrigger = true;
            if (m_spellInfo->SpellFamilyFlags & 0x0000008000000060LL)
            {
                m_procAttacker = PROC_FLAG_DONE_PERIODIC;
                m_procVictim   = PROC_FLAG_TAKEN_PERIODIC;
                m_canTrigger = true;
            }
            break;
        case SPELLFAMILY_HUNTER:  // Hunter Explosive Trap Effect/Immolation Trap Effect/Frost Trap Aura/Snake Trap Effect
            if (m_spellInfo->SpellFamilyFlags & 0x0000200000000014LL) m_canTrigger = true;
            break;
        case SPELLFAMILY_PALADIN: // For Holy Shock, SoC and SoB triggers need to do it
            if (m_spellInfo->SpellFamilyFlags & 0x0001040002200000LL) m_canTrigger = true;
            break;
        case SPELLFAMILY_ROGUE: // mutilate mainhand + offhand
            if (m_spellInfo->SpellFamilyFlags & 0x600000000LL) m_canTrigger = true;
            break;
        case SPELLFAMILY_SHAMAN:
        // Lightning Shield
            if (m_spellInfo->SpellFamilyFlags & 0x0000000000000400LL) m_canTrigger = true;
            break;
        }
    }

    if (m_CastItem && m_spellInfo->SpellFamilyName != SPELLFAMILY_POTION)
        m_canTrigger = false;         // Do not trigger from item cast spell(except potions)

    m_procVictim = m_procAttacker = 0;
    // Get data for type of attack and fill base info for trigger
    switch (m_spellInfo->DmgClass)
    {
        case SPELL_DAMAGE_CLASS_MELEE:
            m_procAttacker = PROC_FLAG_DONE_SPELL_MELEE_DMG_CLASS;
            if (m_attackType == OFF_ATTACK)
                m_procAttacker |= PROC_FLAG_DONE_OFFHAND_ATTACK;
            else
                m_procAttacker |= PROC_FLAG_DONE_MAINHAND_ATTACK;
            m_procVictim   = PROC_FLAG_TAKEN_SPELL_MELEE_DMG_CLASS;
            break;
        case SPELL_DAMAGE_CLASS_RANGED:
            // Auto attack
            if (m_spellInfo->AttributesEx2 & SPELL_ATTR2_AUTOREPEAT_FLAG)
            {
                m_procAttacker = PROC_FLAG_DONE_RANGED_AUTO_ATTACK;
                m_procVictim   = PROC_FLAG_TAKEN_RANGED_AUTO_ATTACK;
            }
            else // Ranged spell attack
            {
                m_procAttacker = PROC_FLAG_DONE_SPELL_RANGED_DMG_CLASS;
                m_procVictim   = PROC_FLAG_TAKEN_SPELL_RANGED_DMG_CLASS;
            }
            break;
        default:
            if (IsPositiveSpell(m_spellInfo->Id))                                 // Check for positive spell
            {
                m_procAttacker = PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_POS;
                m_procVictim   = PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_POS;
            }
            else if (m_spellInfo->AttributesEx2 & SPELL_ATTR2_AUTOREPEAT_FLAG) // Wands auto attack
            {
                m_procAttacker = PROC_FLAG_DONE_RANGED_AUTO_ATTACK;
                m_procVictim   = PROC_FLAG_TAKEN_RANGED_AUTO_ATTACK;
            }
            else                                           // Negative spell
            {
                m_procAttacker = PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_NEG;
                m_procVictim   = PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_NEG;
            }
            break;
    }

    if (m_spellInfo->AttributesEx2 & SPELL_ATTR2_TRIGGERED_CAN_TRIGGER_PROC ||
        m_spellInfo->AttributesEx3 & SPELL_ATTR3_TRIGGERED_CAN_TRIGGER_PROC_2)
        m_canTrigger = true;

    if (m_spellInfo->AttributesEx3 & SPELL_ATTR3_CANT_TRIGGER_PROC)
        m_canTrigger = false;

    // Hunter traps spells (for Entrapment trigger)
    // Gives your Immolation Trap, Frost Trap, Explosive Trap, and Snake Trap ....
    if (m_spellInfo->SpellFamilyName == SPELLFAMILY_HUNTER && m_spellInfo->SpellFamilyFlags & 0x0000200000000014LL)
        m_procAttacker |= PROC_FLAG_DONE_TRAP_ACTIVATION;
}

void Spell::CleanupTargetList()
{
    // m_UniqueTargetInfo.clear();
    // m_UniqueGOTargetInfo.clear();

    for (std::list<TargetInfo>::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
        ihit->deleted = true;

    for (std::list<GOTargetInfo>::iterator ihit = m_UniqueGOTargetInfo.begin(); ihit != m_UniqueGOTargetInfo.end(); ++ihit)
        ihit->deleted = true;

    m_UniqueItemInfo.clear();
    m_countOfHit = 0;
    m_countOfMiss = 0;
    m_delayMoment = 0;
}

void Spell::AddUnitTarget(Unit* pVictim, uint32 effIndex)
{
    if (m_spellInfo->Effect[effIndex] == 0)
        return;

    /*if (!CheckTarget(pVictim, effIndex))
        return;*/

    // Lookup target in already in list
    for (std::list<TargetInfo>::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
    {
        if (ihit->deleted)
            continue;

        if (pVictim->GetGUID() == ihit->targetGUID)           // Found in list
        {
            ihit->effectMask |= 1 << effIndex;               // Add only effect
            return;
        }
    }

    // Get spell hit result on target
    TargetInfo target;
    target.targetGUID = pVictim->GetGUID();                 // Store target GUID
    target.effectMask = 1 << effIndex;
    target.processed  = false;                              // Effects not apply on target
    target.damage     = 0;
    target.crit       = false;
    target.deleted    = false;

    // Add target to list
    m_UniqueTargetInfo.push_back(target);
}

void Spell::CalculateHitResults()
{
    for (std::list<TargetInfo>::iterator it = m_UniqueTargetInfo.begin(); it != m_UniqueTargetInfo.end(); ++it)
    {
        TargetInfo& target = *it;

        Unit* pVictim = m_caster->GetGUID() == target.targetGUID ? m_caster : ObjectAccessor::GetUnit(*m_caster, target.targetGUID);

        if (!pVictim)
            continue;

        // Remove effects the target is immune to
        for (uint32 effIndex = 0; effIndex < 3; ++effIndex)
        {
            if (target.effectMask & (1 << effIndex))
                if (pVictim->IsImmuneToSpellEffect(m_spellInfo, effIndex, false))
                    target.effectMask &= ~(1 << effIndex);

            if (IsChanneledSpell(m_spellInfo))
                if (target.effectMask & (1 << effIndex))
                    m_needAliveTargetMask |= (1 << effIndex);
        }

        // Calculate hit result
        if (m_originalCaster)
        {
            bool canMiss = (m_triggeredByAuraSpell || !m_IsTriggeredSpell);
            target.missCondition = m_originalCaster->SpellHitResult(pVictim, m_spellInfo, m_canReflect, canMiss);
            if (m_skipCheck && target.missCondition != SPELL_MISS_IMMUNE)
                target.missCondition = SPELL_MISS_NONE;
        }
        else
            target.missCondition = SPELL_MISS_EVADE; //SPELL_MISS_NONE;

        if (target.missCondition == SPELL_MISS_NONE)
            ++m_countOfHit;
        else
            ++m_countOfMiss;

        // Spell have speed - need calculate incoming time
        if (m_spellInfo->speed > 0.0f)
        {
            // calculate spell incoming interval
            float dist = m_caster->GetDistance(pVictim->GetPositionX(), pVictim->GetPositionY(), pVictim->GetPositionZ());

            if (dist < 5.0f)
                dist = 5.0f;
            target.timeDelay = (uint64)std::floor(dist / m_spellInfo->speed * 1000.0f);

            // Calculate minimum incoming time
            if (m_delayMoment == 0 || m_delayMoment > target.timeDelay)
                m_delayMoment = target.timeDelay;
        }
        else
            target.timeDelay = 0LL;

        // If target reflect spell back to caster
        if (target.missCondition == SPELL_MISS_REFLECT)
        {
            // Calculate reflected spell result on caster
            target.reflectResult =  m_caster->SpellHitResult(m_caster, m_spellInfo, m_canReflect);

            if (target.reflectResult == SPELL_MISS_REFLECT)     // Impossible reflect again, so simply deflect spell
                target.reflectResult = SPELL_MISS_PARRY;

            // Increase time interval for reflected spells by 1.5
            target.timeDelay += target.timeDelay >> 1;
        }
        else
            target.reflectResult = SPELL_MISS_NONE;
    }
}

void Spell::AddUnitTarget(uint64 unitGUID, uint32 effIndex)
{
    Unit* unit = m_caster->GetGUID() == unitGUID ? m_caster : ObjectAccessor::GetUnit(*m_caster, unitGUID);
    if (unit)
        AddUnitTarget(unit, effIndex);
}

void Spell::AddGOTarget(GameObject* pVictim, uint32 effIndex)
{
    if (m_spellInfo->Effect[effIndex] == 0)
        return;

    uint64 targetGUID = pVictim->GetGUID();

    // Lookup target in already in list
    for (std::list<GOTargetInfo>::iterator ihit = m_UniqueGOTargetInfo.begin(); ihit != m_UniqueGOTargetInfo.end(); ++ihit)
    {
        if (ihit->deleted)
            continue;

        if (targetGUID == ihit->targetGUID)                 // Found in list
        {
            ihit->effectMask |= (1 << effIndex);            // Add only effect mask
            return;
        }
    }

    // This is new target calculate data for him

    GOTargetInfo target;
    target.targetGUID = targetGUID;
    target.effectMask = (1 << effIndex);
    target.processed  = false;                              // Effects not apply on target
    target.deleted    = false;

    // Spell have speed - need calculate incoming time
    if (m_spellInfo->speed > 0.0f)
    {
        // calculate spell incoming interval
        float dist = m_caster->GetDistance(pVictim->GetPositionX(), pVictim->GetPositionY(), pVictim->GetPositionZ());
        if (dist < 5.0f)
            dist = 5.0f;
        target.timeDelay = (uint64) floor(dist / m_spellInfo->speed * 1000.0f);
        if (m_delayMoment == 0 || m_delayMoment > target.timeDelay)
            m_delayMoment = target.timeDelay;
    }
    else
        target.timeDelay = 0LL;

    ++m_countOfHit;

    // Add target to list
    m_UniqueGOTargetInfo.push_back(target);
}

void Spell::AddGOTarget(uint64 goGUID, uint32 effIndex)
{
    GameObject* go = m_caster->GetMap()->GetGameObject(goGUID);
    if (go)
        AddGOTarget(go, effIndex);
}

void Spell::AddItemTarget(Item* pitem, uint32 effIndex)
{
    if (m_spellInfo->Effect[effIndex] == 0)
        return;

    // Lookup target in already in list
    for (std::list<ItemTargetInfo>::iterator ihit = m_UniqueItemInfo.begin(); ihit != m_UniqueItemInfo.end(); ++ihit)
    {
        if (pitem == ihit->item)                            // Found in list
        {
            ihit->effectMask |= (1 << effIndex);            // Add only effect mask
            return;
        }
    }

    // This is new target add data

    ItemTargetInfo target;
    target.item       = pitem;
    target.effectMask = (1 << effIndex);
    m_UniqueItemInfo.push_back(target);
}

void Spell::DoAllEffectOnTarget(TargetInfo* target)
{
    if (!target || target == (TargetInfo*)0x10 || target->processed || m_spellInfo->Id > MAX_SPELL_ID)    // Check target
        return;

    target->processed = true;                               // Target checked in apply effects procedure

    // Get mask of effects for target
    uint8 mask = target->effectMask;

    Unit* unit = m_caster->GetGUID() == target->targetGUID ? m_caster : ObjectAccessor::GetUnit(*m_caster, target->targetGUID);

    if (!unit)
    {
        uint8 farMask = 0;
        // create far target mask
        for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
            if (IsFarUnitTargetEffect(m_spellInfo->Effect[i]))
                if ((1 << i) & mask)
                    farMask |= (1 << i);

        if (!farMask)
            return;
        // find unit in world
        unit = ObjectAccessor::FindUnit(target->targetGUID);
        if (!unit)
            return;

        // do far effects on the unit
        // can't use default call because of threading, do stuff as fast as possible
        for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
            if (farMask & (1 << i))
                HandleEffects(unit, NULL, NULL, i);
        return;
    }

    if (getState() == SPELL_STATE_DELAYED && !IsPositiveSpell(m_spellInfo->Id) && (getMSTime() - target->timeDelay) <= unit->m_lastSanctuaryTime)
        return;                                             // No missinfo in that case

    // Get original caster (if exist) and calculate damage/healing from him data
    Unit* caster = m_originalCasterGUID ? m_originalCaster : m_caster;

    // Skip if m_originalCaster not avaiable
    if (!caster)
        return;

    SpellMissInfo missInfo = target->missCondition;

    // Need init unitTarget by default unit (can changed in code on reflect)
    // Or on missInfo != SPELL_MISS_NONE unitTarget undefined (but need in trigger subsystem)
    unitTarget = unit;

    // Reset damage/healing counter
    m_damage = target->damage;
    m_healing = -target->damage;

    // Fill base trigger info
    uint32 procAttacker = m_procAttacker;
    uint32 procVictim   = m_procVictim;
    uint32 procEx       = PROC_EX_NONE;

    if (missInfo == SPELL_MISS_NONE)                        // In case spell hit target, do all effect on that target
        DoSpellHitOnUnit(unit, mask);
    else if (missInfo == SPELL_MISS_REFLECT)                // In case spell reflect from target, do all effect on caster (if hit)
    {
        if (target->reflectResult == SPELL_MISS_NONE)       // If reflected spell hit caster -> do all effect on him
        {
            DoSpellHitOnUnit(m_caster, mask);
            caster = unit;
        }
    }

    // Do not take combo points on dodge and miss
    if (m_needComboPoints && m_targets.getUnitTargetGUID() == target->targetGUID)
        if (missInfo != SPELL_MISS_NONE)
            m_needComboPoints = false;

    // All calculated do it!
    // Do healing and triggers
    if (m_healing > 0)
    {
        bool crit = caster->isSpellCrit(NULL, m_spellInfo, m_spellSchoolMask);
        uint32 addhealth = m_healing;
        if (crit)
        {
            procEx |= PROC_EX_CRITICAL_HIT;
            addhealth = caster->SpellCriticalBonus(m_spellInfo, addhealth, NULL);
        }
        else
            procEx |= PROC_EX_NORMAL_HIT;

        caster->SendHealSpellLog(unitTarget, m_spellInfo->Id, addhealth, crit);

        // Do triggers for unit (reflect triggers passed on hit phase for correct drop charge)
        if (missInfo != SPELL_MISS_REFLECT)
            caster->ProcDamageAndSpell(unitTarget, procAttacker, procVictim, procEx, addhealth, m_attackType, m_spellInfo, m_canTrigger);

        int32 gain = unitTarget->ModifyHealth(int32(addhealth));
        if (m_IsTriggeredSpell)
            /* @todo: Test me
               spells like earth  shield and prayer of mending
               should give threat to the target they were casted on */
            unitTarget->getHostileRefManager().threatAssist(caster, float(gain) * 0.5f, m_spellInfo);
        else
            caster->getHostileRefManager().threatAssist(caster, float(gain) * 0.5f, m_spellInfo);

        if (caster->GetTypeId() == TYPEID_PLAYER)
            if (Battleground* bg = caster->ToPlayer()->GetBattleground())
                bg->UpdatePlayerScore(caster->ToPlayer(), SCORE_HEALING_DONE, gain);

    }
    // Do damage and triggers
    else if (m_damage > 0)
    {
        // Fill base damage struct (unitTarget - is real spell target)
        SpellNonMeleeDamage damageInfo(caster, unitTarget, m_spellInfo->Id, m_spellSchoolMask);

        // Add bonuses and fill damageInfo struct
        caster->CalculateSpellDamageTaken(&damageInfo, m_damage, m_spellInfo, m_attackType,  target->crit);

        // Send log damage message to client
        caster->SendSpellNonMeleeDamageLog(&damageInfo);

        procEx = createProcExtendMask(&damageInfo, missInfo);
        procVictim |= PROC_FLAG_TAKEN_DAMAGE;

        caster->DealSpellDamage(&damageInfo, true);

        // Do triggers for unit (reflect triggers passed on hit phase for correct drop charge)
        if (missInfo != SPELL_MISS_REFLECT)
        {
            caster->ProcDamageAndSpell(unitTarget, procAttacker, procVictim, procEx, damageInfo.damage, m_attackType, m_spellInfo, m_canTrigger);
            if (caster->GetTypeId() == TYPEID_PLAYER && !(m_spellInfo->AttributesEx4 & SPELL_ATTR4_CANT_TRIGGER_ITEM_SPELLS))
                caster->ToPlayer()->CastItemCombatSpell(unitTarget, m_attackType, procVictim, procEx, m_spellInfo);
        }

        // Judgement of Blood
        if (m_spellInfo->SpellFamilyName == SPELLFAMILY_PALADIN && m_spellInfo->SpellFamilyFlags & 0x0000000800000000LL && m_spellInfo->SpellIconID == 153)
        {
            int32 damagePoint  = damageInfo.damage * 33 / 100;
            m_caster->CastCustomSpell(m_caster, 32220, &damagePoint, NULL, NULL, true);
        }
        // Bloodthirst
        else if (m_spellInfo->SpellFamilyName == SPELLFAMILY_WARRIOR && m_spellInfo->SpellFamilyFlags & 0x40000000000LL)
        {
            uint32 BTAura = 0;
            switch (m_spellInfo->Id)
            {
            case 23881:
                BTAura = 23885;
                break;
            case 23892:
                BTAura = 23886;
                break;
            case 23893:
                BTAura = 23887;
                break;
            case 23894:
                BTAura = 23888;
                break;
            case 25251:
                BTAura = 25252;
                break;
            case 30335:
                BTAura = 30339;
                break;
            default:
                sLog.outError("Spell::EffectSchoolDMG: Spell %u not handled in BTAura", m_spellInfo->Id);
                break;
            }
            if (BTAura)
                m_caster->CastSpell(m_caster, BTAura, true);
        }
    }
    // Passive spell hits/misses or active spells only misses (only triggers)
    else
    {
        // Fill base damage struct (unitTarget - is real spell target)
        SpellNonMeleeDamage damageInfo(caster, unitTarget, m_spellInfo->Id, m_spellSchoolMask);
        procEx = createProcExtendMask(&damageInfo, missInfo);
        // Do triggers for unit (reflect triggers passed on hit phase for correct drop charge)
        if (missInfo != SPELL_MISS_REFLECT)
            caster->ProcDamageAndSpell(unit, procAttacker, procVictim, procEx, 0, m_attackType, m_spellInfo, m_canTrigger);

        // Failed Pickpocket, reveal rogue
        if (missInfo == SPELL_MISS_RESIST && m_customAttr & SPELL_ATTR_CU_PICKPOCKET && unitTarget->GetTypeId() == TYPEID_UNIT)
        {
            m_caster->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_TALK);
            if (unitTarget->ToCreature()->IsAIEnabled)
                unitTarget->ToCreature()->AI()->AttackStart(m_caster);
        }
    }

    // Call scripted function for AI if this spell is casted upon a creature (except pets)
    if (IS_CREATURE_GUID(target->targetGUID))
    {
        // cast at creature (or GO) quest objectives update at successful cast finished (+channel finished)
        // ignore autorepeat/melee casts for speed (not exist quest for spells (hm...)
        if (m_caster->GetTypeId() == TYPEID_PLAYER && !IsAutoRepeat() && !IsNextMeleeSwingSpell() && !IsChannelActive())
            m_caster->ToPlayer()->CastedCreatureOrGO(unit->GetEntry(), unit->GetGUID(), m_spellInfo->Id);
    }

    if (missInfo != SPELL_MISS_EVADE && missInfo != SPELL_MISS_REFLECT
        && m_caster && !m_caster->IsFriendlyTo(unit) &&
        (!IsPositiveSpell(m_spellInfo->Id) || m_spellInfo->HasEffect(SPELL_EFFECT_DISPEL)))
    {
        m_caster->CombatStart(unit, !(m_spellInfo->AttributesEx3 & SPELL_ATTR3_NO_INITIAL_AGGRO));

        if (m_customAttr & SPELL_ATTR_CU_AURA_CC)
            if (!unit->IsStandState())
                unit->SetStandState(UNIT_STAND_STATE_STAND);
    }

    if (unit->IsPvP() && m_caster->GetTypeId() == TYPEID_PLAYER && (unit->GetTypeId() == TYPEID_PLAYER || unit->IsInCombat()))
        m_caster->ToPlayer()->UpdatePvP(true);
}

void Spell::DoSpellHitOnUnit(Unit* unit, const uint32 effectMask)
{
    if (!unit || !effectMask)
        return;

    // Recheck immune (only for delayed spells)
    if (m_spellInfo->speed &&
        !(m_spellInfo->Attributes & SPELL_ATTR0_UNAFFECTED_BY_INVULNERABILITY)
        && (unit->IsImmunedToDamage(GetSpellSchoolMask(m_spellInfo), true) ||
            unit->IsImmuneToSpell(m_spellInfo, true)))
    {
        m_caster->SendSpellMiss(unit, m_spellInfo->Id, SPELL_MISS_IMMUNE);
        m_damage = 0;
        return;
    }

    if (m_caster != unit)
    {
        if (unit->GetCharmerOrOwnerGUID() != m_caster->GetGUID())
        {
            if (unit->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            {
                if (! (sSpellMgr.GetSpellCustomAttr(m_spellInfo->Id) & SPELL_ATTR_CU_ANY_TARGET))
                {
                    m_caster->SendSpellMiss(unit, m_spellInfo->Id, SPELL_MISS_EVADE);
                    m_damage = 0;
                    return;
                }
            }
        }
        if (m_caster->IsValidAttackTarget(unit))
        {
            unit->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_HITBYSPELL);
            if (m_customAttr & SPELL_ATTR_CU_AURA_CC)
                unit->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_CC);
        }
        else if (m_caster->IsFriendlyTo(unit))
        {
            // for delayed spells ignore negative spells (after duel end) for friendly targets
            // @todo this cause soul transfer bugged
            if (m_spellInfo->speed > 0.0f && unit->GetTypeId() == TYPEID_PLAYER && !IsPositiveSpell(m_spellInfo->Id))
            {
                m_caster->SendSpellMiss(unit, m_spellInfo->Id, SPELL_MISS_EVADE);
                m_damage = 0;
                return;
            }

            // assisting case, healing and resurrection
            if (unit->HasUnitState(UNIT_STATE_ATTACK_PLAYER))
            {
                m_caster->SetContestedPvP();
                if (m_caster->GetTypeId() == TYPEID_PLAYER)
                    m_caster->ToPlayer()->UpdatePvP(true);
            }
            if (unit->IsInCombat() && !(m_spellInfo->AttributesEx3 & SPELL_ATTR3_NO_INITIAL_AGGRO))
            {
                m_caster->SetInCombatState(unit->IsInPvPCombat(), unit);
                unit->getHostileRefManager().threatAssist(m_caster, 0.0f);
            }
        }
    }

    uint8 aura_effmask = 0;
    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        if (effectMask & (1 << i) && m_spellInfo->Effect[i] == SPELL_EFFECT_APPLY_AURA)
            aura_effmask |= 1 << i;

    // Get Data Needed for Diminishing Returns, some effects may have multiple auras, so this must be done on spell hit, not aura add
    m_diminishGroup = GetDiminishingReturnsGroupForSpell(m_spellInfo, m_triggeredByAuraSpell);
    if (m_diminishGroup && aura_effmask)
    {
        m_diminishLevel = unit->GetDiminishing(m_diminishGroup);
        // send immunity message if target is immune
        if (m_diminishLevel == DIMINISHING_LEVEL_IMMUNE)
        {
            m_caster->SendSpellMiss(unitTarget, m_spellInfo->Id, SPELL_MISS_IMMUNE);
            return;
        }

        DiminishingReturnsType type = GetDiminishingReturnsGroupType(m_diminishGroup);
        // Increase Diminishing on unit, current informations for actually casts will use values above
        if ((type == DRTYPE_PLAYER && (unit->GetTypeId() == TYPEID_PLAYER || unit->IsPet() || unit->ToCreature()->isPossessedByPlayer())) || type == DRTYPE_ALL)
            unit->IncrDiminishing(m_diminishGroup);
    }

    for (uint32 effectNumber = 0; effectNumber < 3; effectNumber++)
        if (effectMask & (1 << effectNumber))
            HandleEffects(unit, NULL, NULL, effectNumber);

    if (unit->GetTypeId() == TYPEID_UNIT && unit->ToCreature()->IsAIEnabled)
        unit->ToCreature()->AI()->SpellHit(m_caster, m_spellInfo);

    if (m_caster->GetTypeId() == TYPEID_UNIT && m_caster->ToCreature()->IsAIEnabled)
        m_caster->ToCreature()->AI()->SpellHitTarget(unit, m_spellInfo);

    // trigger only for first effect targets
    if (m_ChanceTriggerSpells.size() && (effectMask & 0x1))
    {
        int _duration = 0;
        for (ChanceTriggerSpells::const_iterator i = m_ChanceTriggerSpells.begin(); i != m_ChanceTriggerSpells.end(); ++i)
        {
            if (roll_chance_i(i->second))
            {
                m_caster->CastSpell(unit, i->first, true);
                // SPELL_AURA_ADD_TARGET_TRIGGER auras shouldn't trigger auras without duration
                // set duration equal to triggering spell
                if (GetSpellDuration(i->first) == -1)
                {
                    // get duration from aura-only once
                    if (!_duration)
                    {
                        Aura* aur = unit->GetAuraByCasterSpell(m_spellInfo->Id, m_caster->GetGUID());
                        _duration = aur ? aur->GetAuraDuration() : -1;
                    }
                    unit->SetAurasDurationByCasterSpell(i->first->Id, m_caster->GetGUID(), _duration);
                }
            }
        }
    }

    if (m_customAttr & SPELL_ATTR_CU_LINK_HIT)
    {
        if (const std::vector<int32>* spell_triggered = sSpellMgr.GetSpellLinked(m_spellInfo->Id + SPELL_LINK_HIT))
        {
            for (std::vector<int32>::const_iterator i = spell_triggered->begin(); i != spell_triggered->end(); ++i)
                if (*i < 0)
                    unit->RemoveAurasDueToSpell(-(*i));
                else
                    unit->CastSpell(unit, *i, true, 0, 0, m_caster->GetGUID());
        }
    }
}

void Spell::DoAllEffectOnTarget(GOTargetInfo* target)
{
    if (!target || target == (GOTargetInfo*)0x10 || target->processed || m_spellInfo->Id > MAX_SPELL_ID)    // Check target
        return;

    target->processed = true;                               // Target checked in apply effects procedure

    uint32 effectMask = target->effectMask;
    if (!effectMask)
        return;

    GameObject* go = m_caster->GetMap()->GetGameObject(target->targetGUID);
    if (!go)
        return;

    for (uint32 effectNumber = 0; effectNumber < 3; effectNumber++)
        if (effectMask & (1 << effectNumber))
            HandleEffects(NULL, NULL, go, effectNumber);

    sLog.outString("CASTED %u on GUID: " UI64FMTD ", entry: %u", m_spellInfo->Id, go->GetGUID(), go->GetGOInfo()->id);

    // cast at creature (or GO) quest objectives update at successful cast finished (+channel finished)
    // ignore autorepeat/melee casts for speed (not exist quest for spells (hm...)
    if (m_caster->GetTypeId() == TYPEID_PLAYER && !IsAutoRepeat() && !IsNextMeleeSwingSpell() && !IsChannelActive())
        m_caster->ToPlayer()->CastedCreatureOrGO(go->GetEntry(), go->GetGUID(), m_spellInfo->Id);
}

void Spell::DoAllEffectOnTarget(ItemTargetInfo* target)
{
    uint32 effectMask = target->effectMask;
    if (!target->item || !effectMask)
        return;

    for (uint32 effectNumber = 0; effectNumber < 3; effectNumber++)
        if (effectMask & (1 << effectNumber))
            HandleEffects(NULL, target->item, NULL, effectNumber);
}

bool Spell::IsAliveUnitPresentInTargetList()
{
    // Not need check return true
    if (m_needAliveTargetMask == 0)
        return true;

    uint8 needAliveTargetMask = m_needAliveTargetMask;

    for (std::list<TargetInfo>::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
    {
        if (ihit->deleted)
            continue;

        if (ihit->missCondition == SPELL_MISS_NONE && (needAliveTargetMask & ihit->effectMask))
        {
            Unit* unit = m_caster->GetGUID() == ihit->targetGUID ? m_caster : ObjectAccessor::GetUnit(*m_caster, ihit->targetGUID);

            if (unit && unit->IsAlive())
                needAliveTargetMask &= ~ihit->effectMask;   // remove from need alive mask effect that have alive target
        }
    }

    // is all effects from m_needAliveTargetMask have alive targets
    return needAliveTargetMask == 0;
}

// Helper for Chain Healing
// Spell target first
// Raidmates then descending by injury suffered (MaxHealth - Health)
// Other players/mobs then descending by injury suffered (MaxHealth - Health)
struct ChainHealingOrder : public std::binary_function<const Unit*, const Unit*, bool>
{
    const Unit* MainTarget;
    ChainHealingOrder(Unit const* Target) : MainTarget(Target) {};
    // functor for operator ">"
    bool operator()(Unit const* _Left, Unit const* _Right) const
    {
        return (ChainHealingHash(_Left) < ChainHealingHash(_Right));
    }

    int32 ChainHealingHash(Unit const* Target) const
    {
        /*if (Target == MainTarget)
            return 0;
        else*/ if (Target->GetTypeId() == TYPEID_PLAYER && MainTarget->GetTypeId() == TYPEID_PLAYER &&
                   Target->ToPlayer()->IsInSameRaidWith((Player const*)MainTarget))
        {
            if (Target->GetHealth() == Target->GetMaxHealth())
                return 40000;
            else
                return 20000 - Target->GetMaxHealth() + Target->GetHealth();
        }
        else
            return 40000 - Target->GetMaxHealth() + Target->GetHealth();
    }
};

// Helper for targets nearest to the spell target
// The spell target is always first unless there is a target at _completely_ the same position (unbelievable case)
struct TargetDistanceOrder : public std::binary_function<const Unit, const Unit, bool>
{
    const Unit* MainTarget;
    TargetDistanceOrder(const Unit* Target) : MainTarget(Target) {};
    // functor for operator ">"
    bool operator()(const Unit* _Left, const Unit* _Right) const
    {
        return (MainTarget->GetDistance(_Left) < MainTarget->GetDistance(_Right));
    }
};

void Spell::SearchChainTarget(std::list<Unit*>& TagUnitMap, float max_range, uint32 num, SpellTargets TargetType)
{
    Unit* cur = m_targets.getUnitTarget();
    if (!cur)
        return;

    //FIXME: This very like horrible hack and wrong for most spells
    if (m_spellInfo->DmgClass != SPELL_DAMAGE_CLASS_MELEE)
        max_range += num * CHAIN_SPELL_JUMP_RADIUS;

    std::list<Unit*> tempUnitMap;
    if (TargetType == SPELL_TARGETS_CHAINHEAL)
    {
        SearchAreaTarget(tempUnitMap, max_range, PUSH_CHAIN, SPELL_TARGETS_ALLY);
        tempUnitMap.sort(ChainHealingOrder(m_caster));
        //if (cur->GetHealth() == cur->GetMaxHealth() && tempUnitMap.size())
        //    cur = tempUnitMap.front();
    }
    else
        SearchAreaTarget(tempUnitMap, max_range, PUSH_CHAIN, TargetType);
    tempUnitMap.remove(cur);

    while (num)
    {
        TagUnitMap.push_back(cur);
        --num;

        if (tempUnitMap.empty())
            break;

        std::list<Unit*>::iterator next;

        if (TargetType == SPELL_TARGETS_CHAINHEAL)
        {
            next = tempUnitMap.begin();
            while (cur->GetDistance(*next) > CHAIN_SPELL_JUMP_RADIUS
                   || !cur->IsWithinLOSInMap(*next))
            {
                ++next;
                if (next == tempUnitMap.end())
                    return;
            }
        }
        else
        {
            tempUnitMap.sort(TargetDistanceOrder(cur));
            next = tempUnitMap.begin();

            if (cur->GetDistance(*next) > CHAIN_SPELL_JUMP_RADIUS)
                break;
            while ((m_spellInfo->DmgClass == SPELL_DAMAGE_CLASS_MELEE
                    && !m_caster->isInFrontInMap(*next, max_range))
                   || !m_caster->CanSeeOrDetect(*next)
                   || !cur->IsWithinLOSInMap(*next))
            {
                ++next;
                if (next == tempUnitMap.end() || cur->GetDistance(*next) > CHAIN_SPELL_JUMP_RADIUS)
                    return;
            }
        }

        cur = *next;
        tempUnitMap.erase(next);
    }
}

void Spell::SearchAreaTarget(std::list<Unit*>& TagUnitMap, float radius, const uint32 type, SpellTargets TargetType, uint32 entry)
{
    Position* pos;
    switch (type)
    {
    case PUSH_DST_CENTER:
        CheckDst();
        pos = &m_targets.m_dstPos;
        break;
    case PUSH_SRC_CENTER:
        CheckSrc();
        pos = &m_targets.m_srcPos;
        break;
    case PUSH_CHAIN:
        {
            Unit* target = m_targets.getUnitTarget();
            if (!target)
            {
                sLog.outError("SPELL: cannot find unit target for spell ID %u\n", m_spellInfo->Id);
                return;
            }
            pos = target;
            break;
        }
    default:
        pos = m_caster;
        break;
    }

    Oregon::SpellNotifierCreatureAndPlayer notifier(*this, TagUnitMap, radius, type, TargetType, pos, entry);
    if ((m_spellInfo->AttributesEx3 & SPELL_ATTR3_PLAYERS_ONLY) || (TargetType == SPELL_TARGETS_ENTRY && !entry))
    {
        m_caster->GetMap()->VisitWorld(pos->m_positionX, pos->m_positionY, radius, notifier);
        TagUnitMap.remove_if(Oregon::ObjectTypeIdCheck(TYPEID_PLAYER, false)); // above line will select also pets and totems, remove them
    }
    else
        m_caster->GetMap()->VisitAll(pos->m_positionX, pos->m_positionY, radius, notifier);
}

WorldObject* Spell::SearchNearbyTarget(float range, SpellTargets TargetType)
{
    switch (TargetType)
    {
    case SPELL_TARGETS_ENTRY:
        {
            ConditionList conditions = sConditionMgr.GetConditionsForNotGroupedEntry(CONDITION_SOURCE_TYPE_SPELL_SCRIPT_TARGET, m_spellInfo->Id);
            if (conditions.empty())
            {
                sLog.outDebug("Spell (ID: %u) (caster Entry: %u) does not have record in `conditions` for spell script target (ConditionSourceType 13)", m_spellInfo->Id, m_caster->GetEntry());
                if (IsPositiveSpell(m_spellInfo->Id))
                    return SearchNearbyTarget(range, SPELL_TARGETS_ALLY);
                else
                    return SearchNearbyTarget(range, SPELL_TARGETS_ENEMY);
            }

            Creature* creatureScriptTarget = NULL;
            GameObject* goScriptTarget = NULL;

            for (ConditionList::const_iterator i_spellST = conditions.begin(); i_spellST != conditions.end(); ++i_spellST)
            {
                if ((*i_spellST)->Type != CONDITION_SPELL_SCRIPT_TARGET)
                    continue;
                switch((*i_spellST)->ConditionValue1)
                {
                case SPELL_TARGET_TYPE_GAMEOBJECT:
                    if ((*i_spellST)->ConditionValue2)
                    {
                        if (GameObject *go = m_caster->FindNearestGameObject((*i_spellST)->ConditionValue2, range))
                        {
                            // remember found target and range, next attempt will find more near target with another entry
                            goScriptTarget = go;
                            creatureScriptTarget = NULL;
                            range = m_caster->GetDistance(goScriptTarget);
                        }
                    }
                    else if (focusObject)          //Focus Object
                    {
                        float frange = m_caster->GetDistance(focusObject);
                        if (range >= frange)
                        {
                            creatureScriptTarget = NULL;
                            goScriptTarget = focusObject;
                            range = frange;
                        }
                    }
                    break;
                case SPELL_TARGET_TYPE_CREATURE:
                case SPELL_TARGET_TYPE_DEAD:
                default:
                    if (Creature *cre = m_caster->FindNearestCreature((*i_spellST)->ConditionValue2, range, (*i_spellST)->ConditionValue1 != SPELL_TARGET_TYPE_DEAD))
                    {
                        creatureScriptTarget = cre;
                        goScriptTarget = NULL;
                        range = m_caster->GetDistance(creatureScriptTarget);
                    }
                    break;
                }
            }

            if (creatureScriptTarget)
                return creatureScriptTarget;
            else
                return goScriptTarget;
        }
    default:
    case SPELL_TARGETS_ENEMY:
        {
            Unit* target = NULL;
            Oregon::AnyUnfriendlyUnitInObjectRangeCheck u_check(m_caster, m_caster, range);
            Oregon::UnitLastSearcher<Oregon::AnyUnfriendlyUnitInObjectRangeCheck> searcher(target, u_check);
            m_caster->VisitNearbyObject(range, searcher);
            return target;
        }
    case SPELL_TARGETS_ALLY:
        {
            Unit* target = NULL;
            Oregon::AnyFriendlyUnitInObjectRangeCheck u_check(m_caster, m_caster, range);
            Oregon::UnitLastSearcher<Oregon::AnyFriendlyUnitInObjectRangeCheck> searcher(target, u_check);
            m_caster->VisitNearbyObject(range, searcher);
            return target;
        }
    }
}

void Spell::SetTargetMap(uint32 i, uint32 cur)
{
    SpellNotifyPushType pushType = PUSH_NONE;
    Player* modOwner = NULL;
    if (m_originalCaster)
        modOwner = m_originalCaster->GetSpellModOwner();

    switch (sSpellMgr.SpellTargetType[cur])
    {
    case TARGET_TYPE_UNIT_CASTER:
        {
            switch (cur)
            {
            case TARGET_NONE:
            case TARGET_UNIT_CASTER:
                AddUnitTarget(m_caster, i);
                break;
            case TARGET_UNIT_CASTER_FISHING:
                {
                    float min_dis = GetSpellMinRange(sSpellRangeStore.LookupEntry(m_spellInfo->rangeIndex));
                    float max_dis = GetSpellMaxRange(sSpellRangeStore.LookupEntry(m_spellInfo->rangeIndex));
                    float dis = rand_norm() * (max_dis - min_dis) + min_dis;
                    float x, y, z;
                    float angle = float(rand_norm()) * static_cast<float>(M_PI * 35.0f / 180.0f) - static_cast<float>(M_PI * 17.5f / 180.0f);
                    m_caster->GetClosePoint(x, y, z, DEFAULT_WORLD_OBJECT_SIZE, dis, angle);

                    float ground = z;
                    float liquidLevel = m_caster->GetMap()->GetWaterOrGroundLevel(x, y, z, &ground);
                    if (liquidLevel <= ground) // When there is no liquid Map::GetWaterOrGroundLevel returns ground level
                    {
                        SendCastResult(SPELL_FAILED_NOT_HERE);
                        SendChannelUpdate(0);
                        finish(false);
                        return;
                    }

                    if (ground + 0.75 > liquidLevel)
                    {
                        SendCastResult(SPELL_FAILED_TOO_SHALLOW);
                        SendChannelUpdate(0);
                        finish(false);
                        return;
                    }

                    m_targets.setDst(x, y, liquidLevel, m_caster->GetOrientation());

                    break;
                }
            case TARGET_UNIT_MASTER:
                if (Unit* owner = m_caster->GetCharmerOrOwner())
                    AddUnitTarget(owner, i);
                break;
            case TARGET_UNIT_PET:
                if (Player* plr = m_caster->ToPlayer())
                {
                    if (Pet* pet = plr->GetPet())
                    {
                        AddUnitTarget(pet, i);
                        break;
                    }
                }

                if (Guardian* pet = m_caster->GetGuardianPet())
                    AddUnitTarget(pet, i);
                break;
            case TARGET_UNIT_PARTY_CASTER:
            case TARGET_UNIT_RAID_CASTER:
                pushType = PUSH_CASTER_CENTER;
                break;
            }
            break;
        }

    case TARGET_TYPE_UNIT_TARGET:
        {
            Unit* target = m_targets.getUnitTarget();
            if (!target)
            {
                if (Player* player = m_caster->ToPlayer())
                    target = ObjectAccessor::GetUnit(*player, player->GetSelection());
                else
                    target = m_caster->GetVictim();

                if (target)
                    m_targets.setUnitTarget(target);
                else
                {
                    sLog.outError("SPELL: no unit target for spell ID %u", m_spellInfo->Id);
                    break;
                }
            }

            switch (cur)
            {
            case TARGET_UNIT_TARGET_ENEMY:
                if ((m_spellInfo->AttributesEx & (0x8 | 0x80)) == 0)
                {
                    // try to select magnet target first
                    if (SelectMagnetTarget() == m_targets.getUnitTarget())
                        // if not found (target is not changed) search for SPELL_AURA_ADD_CASTER_HIT_TRIGGER
                        HandleHitTriggerAura();
                }
            case TARGET_UNIT_CHAINHEAL:
                pushType = PUSH_CHAIN;
                break;
            case TARGET_UNIT_TARGET_ALLY:
                AddUnitTarget(target, i);
                break;
            case TARGET_UNIT_TARGET_RAID:
            case TARGET_UNIT_TARGET_ANY: // SelectMagnetTarget()?
            case TARGET_UNIT_TARGET_PARTY:
            case TARGET_UNIT_TARGET_MINIPET:
                if (IsValidSingleTargetSpell(target))
                    AddUnitTarget(target, i);
                break;
            case TARGET_UNIT_PARTY_TARGET:
            case TARGET_UNIT_CLASS_TARGET:
                pushType = PUSH_CASTER_CENTER; // not real
                break;
            }
            break;
        }

    case TARGET_TYPE_UNIT_NEARBY:
        {
            float range = GetSpellMaxRange(sSpellRangeStore.LookupEntry(m_spellInfo->rangeIndex));
            if (modOwner)
                modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_RANGE, range, this);

            WorldObject* target = NULL;

            switch (cur)
            {
            case TARGET_UNIT_NEARBY_ENEMY:
                target = SearchNearbyTarget(range, SPELL_TARGETS_ENEMY);
                break;
            case TARGET_UNIT_NEARBY_ALLY:
            case TARGET_UNIT_NEARBY_PARTY: // TODO: fix party/raid targets
            case TARGET_UNIT_NEARBY_RAID:
                target = SearchNearbyTarget(range, SPELL_TARGETS_ALLY);
                break;
            case TARGET_UNIT_NEARBY_ENTRY:
                target = SearchNearbyTarget(range, SPELL_TARGETS_ENTRY);
                break;
            }

            if (!target)
                return;
            else if (target->GetTypeId() == TYPEID_UNIT || target->GetTypeId() == TYPEID_PLAYER)
            {
                pushType = PUSH_CHAIN;
                m_targets.setUnitTarget((Unit*)target);
            }
            else if (target->GetTypeId() == TYPEID_GAMEOBJECT)
                AddGOTarget((GameObject*)target, i);

            break;
        }

    case TARGET_TYPE_AREA_SRC:
        pushType = PUSH_SRC_CENTER;
        break;

    case TARGET_TYPE_AREA_DST:
        pushType = PUSH_DST_CENTER;
        break;

    case TARGET_TYPE_AREA_CONE:
        if (m_customAttr & SPELL_ATTR_CU_CONE_BACK)
            pushType = PUSH_IN_BACK;
        else if (m_customAttr & SPELL_ATTR_CU_CONE_LINE)
            pushType = PUSH_IN_LINE;
        else
            pushType = PUSH_IN_FRONT;
        break;

    case TARGET_TYPE_DEST_CASTER: //4+8+2
        {
            if (cur == TARGET_SRC_CASTER)
            {
                m_targets.setSrc(m_caster);
                break;
            }
            else if (cur == TARGET_DST_CASTER)
            {
                m_targets.setDst(m_caster);
                break;
            }

            float angle, dist;

            float objSize = m_caster->GetObjectSize();
            dist = GetSpellRadius(m_spellInfo, i, true);
            if (dist < objSize)
                dist = objSize;
            else if (cur == TARGET_DEST_CASTER_RANDOM)
                dist = objSize + (dist - objSize) * rand_norm();

            switch (cur)
            {
            case TARGET_DEST_CASTER_FRONT_LEFT:
                angle = static_cast<float>(-M_PI / 4);
                break;
            case TARGET_DEST_CASTER_BACK_LEFT:
                angle = static_cast<float>(-3 * M_PI / 4);
                break;
            case TARGET_DEST_CASTER_BACK_RIGHT:
                angle = static_cast<float>(3 * M_PI / 4);
                break;
            case TARGET_DEST_CASTER_FRONT_RIGHT:
                angle = static_cast<float>(M_PI / 4);
                break;
            case TARGET_MINION:
            case TARGET_DEST_CASTER_FRONT_LEAP:
            case TARGET_DEST_CASTER_FRONT:
                angle = 0.0f;
                break;
            case TARGET_DEST_CASTER_BACK:
                angle = static_cast<float>(M_PI);
                break;
            case TARGET_DEST_CASTER_RIGHT:
                angle = static_cast<float>(-M_PI / 2);
                break;
            case TARGET_DEST_CASTER_LEFT:
                angle = static_cast<float>(M_PI / 2);
                break;
            default:
                angle = (float)rand_norm() * static_cast<float>(2 * M_PI);
                break;
            }

            Position pos;
            switch (cur)
            {
            case TARGET_DEST_CASTER_FRONT_LEAP:
            case TARGET_DEST_CASTER_FRONT_LEFT:
            case TARGET_DEST_CASTER_BACK_LEFT:
            case TARGET_DEST_CASTER_BACK_RIGHT:
            case TARGET_DEST_CASTER_FRONT_RIGHT:
                pos = m_caster->GetFirstCollisionPosition(dist, angle);
                break;
            default:
                pos = m_caster->GetNearPosition(dist, angle);
                break;
            }
            m_targets.setDst(&pos); // also flag
            break;
        }

    case TARGET_TYPE_DEST_TARGET: //2+8+2
        {
            Unit* target = m_targets.getUnitTarget();
            if (!target)
            {
                sLog.outError("SPELL: no unit target for spell ID %u\n", m_spellInfo->Id);
                break;
            }

            if (cur == TARGET_DST_TARGET_ENEMY || cur == TARGET_DEST_TARGET_ANY)
            {
                m_targets.setDst(target);
                break;
            }

            float angle, dist;

            float objSize = target->GetObjectSize();
            dist = GetSpellRadius(m_spellInfo, i, false);
            if (dist < objSize)
                dist = objSize;
            else if (cur == TARGET_DEST_CASTER_RANDOM)
                dist = objSize + (dist - objSize) * rand_norm();

            switch (cur)
            {
            case TARGET_DEST_TARGET_FRONT:
                angle = 0.0f;
                break;
            case TARGET_DEST_TARGET_BACK:
                angle = M_PI;
                break;
            case TARGET_DEST_TARGET_RIGHT:
                angle = M_PI / 2;
                break;
            case TARGET_DEST_TARGET_LEFT:
                angle = -M_PI / 2;
                break;
            case TARGET_DEST_TARGET_FRONT_LEFT:
                angle = -M_PI / 4;
                break;
            case TARGET_DEST_TARGET_BACK_LEFT:
                angle = -3 * M_PI / 4;
                break;
            case TARGET_DEST_TARGET_BACK_RIGHT:
                angle = 3 * M_PI / 4;
                break;
            case TARGET_DEST_TARGET_FRONT_RIGHT:
                angle = M_PI / 4;
                break;
            default:
                angle = rand_norm() * 2 * M_PI;
                break;
            }

            Position pos;
            switch (cur)
            {
            case TARGET_DEST_TARGET_FRONT:
            case TARGET_DEST_TARGET_BACK:
            case TARGET_DEST_CASTER_BACK_LEFT:
            case TARGET_DEST_TARGET_LEFT:
            case TARGET_DEST_TARGET_FRONT_LEFT:
            case TARGET_DEST_TARGET_BACK_LEFT:
            case TARGET_DEST_TARGET_BACK_RIGHT:
            case TARGET_DEST_TARGET_FRONT_RIGHT:
                {
                    target->GetContactPoint(m_caster, pos.m_positionX, pos.m_positionY, pos.m_positionZ, dist);
                    pos = target->GetFirstCollisionPosition(dist, angle);
                }
                break;
            default:
                pos = target->GetNearPosition(dist, angle);
                break;
            }
            m_targets.setDst(&pos);
            break;
        }

    case TARGET_TYPE_DEST_DEST: //5+8+1
        {
            if (!m_targets.HasDst())
            {
                sLog.outError("SPELL: no destination for spell ID %u\n", m_spellInfo->Id);
                break;
            }

            float angle;
            switch (cur)
            {
            case TARGET_DEST_DYNOBJ_ENEMY:
            case TARGET_DEST_DYNOBJ_ALLY:
            case TARGET_DEST_DYNOBJ_NONE:
            case TARGET_DEST_DEST:
            case TARGET_DEST_TRAJ:
                return;
            case TARGET_DEST_DEST_FRONT:
                angle = 0.0f;
                break;
            case TARGET_DEST_DEST_BACK:
                angle = M_PI;
                break;
            case TARGET_DEST_DEST_RIGHT:
                angle = M_PI / 2;
                break;
            case TARGET_DEST_DEST_LEFT:
                angle = -M_PI / 2;
                break;
            case TARGET_DEST_DEST_FRONT_LEFT:
                angle = -M_PI / 4;
                break;
            case TARGET_DEST_DEST_BACK_LEFT:
                angle = -3 * M_PI / 4;
                break;
            case TARGET_DEST_DEST_BACK_RIGHT:
                angle = 3 * M_PI / 4;
                break;
            case TARGET_DEST_DEST_FRONT_RIGHT:
                angle = float(M_PI) / 4;
                break;
            default:
                angle = rand_norm() * 2 * M_PI;
                break;
            }

            float dist;
            dist = GetSpellRadius(m_spellInfo, i, true);
            if (cur == TARGET_DEST_DEST_RANDOM)
                dist *= rand_norm();

            // must has dst, no need to set flag
            m_caster->MovePositionToFirstCollision(m_targets.m_dstPos, dist, angle);
            break;
        }

    case TARGET_TYPE_DEST_SPECIAL:
        {
            switch (cur)
            {
            case TARGET_DST_DB:
                if (SpellTargetPosition const* st = sSpellMgr.GetSpellTargetPosition(m_spellInfo->Id))
                {
                    if (IsSpellHaveEffect(m_spellInfo, SPELL_EFFECT_TELEPORT_UNITS))
                        m_targets.setDst(st->target_X, st->target_Y, st->target_Z, st->target_Orientation, (int32)st->target_mapId);
                    else if (st->target_mapId == m_caster->GetMapId())
                        m_targets.setDst(st->target_X, st->target_Y, st->target_Z, st->target_Orientation);
                }
                else
                    sLog.outError("SPELL: unknown target coordinates for spell ID %u\n", m_spellInfo->Id);
                break;
            case TARGET_DST_HOME:
                if (m_caster->GetTypeId() == TYPEID_PLAYER)
                    m_targets.setDst(m_caster->ToPlayer()->m_homebindX, m_caster->ToPlayer()->m_homebindY, m_caster->ToPlayer()->m_homebindZ, m_caster->ToPlayer()->GetOrientation(), m_caster->ToPlayer()->m_homebindMapId);
                break;
            case TARGET_DST_NEARBY_ENTRY:
                {
                    float range = GetSpellMaxRange(sSpellRangeStore.LookupEntry(m_spellInfo->rangeIndex));
                    if (modOwner)
                        modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_RANGE, range, this);

                    if (WorldObject* target = SearchNearbyTarget(range, SPELL_TARGETS_ENTRY))
                        m_targets.setDst(target);
                    break;
                }
            }
            break;
        }

    case TARGET_TYPE_CHANNEL:
        {
            if (!m_originalCaster || !m_originalCaster->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
            {
                sLog.outDebug("SPELL: no current channeled spell for spell ID %u - spell triggering this spell was interrupted.", m_spellInfo->Id);
                break;
            }

            switch (cur)
            {
            case TARGET_UNIT_CHANNEL:
                if (Unit* target = m_originalCaster->GetCurrentSpell(CURRENT_CHANNELED_SPELL)->m_targets.getUnitTarget())
                    AddUnitTarget(target, i);
                else
                    sLog.outError("SPELL: cannot find channel spell target for spell ID %u", m_spellInfo->Id);
                break;
            case TARGET_DEST_CHANNEL:
                if (m_originalCaster->GetCurrentSpell(CURRENT_CHANNELED_SPELL)->m_targets.HasDst())
                    m_targets = m_originalCaster->GetCurrentSpell(CURRENT_CHANNELED_SPELL)->m_targets;
                else
                    sLog.outError("SPELL: cannot find channel spell destination for spell ID %u", m_spellInfo->Id);
                break;
            }
            break;
        }

    default:
        {
            switch (cur)
            {
            case TARGET_GAMEOBJECT:
            case TARGET_GAMEOBJECT_NEARBY_ENTRY:
                if (m_targets.getGOTarget())
                    AddGOTarget(m_targets.getGOTarget(), i);
                else
                {
                    if (WorldObject* obj = SearchNearbyTarget(GetSpellMaxRange(m_spellInfo), SPELL_TARGETS_ENTRY))
                    {
                        if (obj->GetTypeId() != TYPEID_GAMEOBJECT)
                            break;
                        AddGOTarget(obj->ToGameObject(), i);
                    }
                }
                break;
            case TARGET_GAMEOBJECT_ITEM:
                if (m_targets.getGOTargetGUID())
                    AddGOTarget(m_targets.getGOTarget(), i);
                else if (m_targets.getItemTarget())
                    AddItemTarget(m_targets.getItemTarget(), i);
                break;
            default:
                sLog.outError("SPELL (caster[type: %u; guidlow: %u], spell: %u): unhandled spell target (%u)",
                              m_caster->GetTypeId(), m_caster->GetGUIDLow(), m_spellInfo->Id, cur);
                break;
            }
            break;
        }
    }

    if (pushType == PUSH_CHAIN) // Chain
    {
        Unit* target = m_targets.getUnitTarget();
        if (!target)
        {
            sLog.outError("SPELL: no chain unit target for spell ID %u", m_spellInfo->Id);
            return;
        }

        //Chain: 2, 6, 22, 25, 45, 77
        uint32 maxTargets = m_spellInfo->EffectChainTarget[i];
        if (modOwner)
            modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_JUMP_TARGETS, maxTargets, this);

        if (maxTargets > 1)
        {
            //otherwise, this multiplier is used for something else
            m_damageMultipliers[i] = 1.0f;
            m_applyMultiplierMask |= 1 << i;

            float range = GetSpellMaxRange(sSpellRangeStore.LookupEntry(m_spellInfo->rangeIndex));
            if (modOwner)
                modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_RANGE, range, this);

            std::list<Unit*> unitList;

            switch (cur)
            {
            case TARGET_UNIT_NEARBY_ENEMY:
            case TARGET_UNIT_TARGET_ENEMY:
            case TARGET_UNIT_NEARBY_ENTRY: // fix me
                SearchChainTarget(unitList, range, maxTargets, SPELL_TARGETS_ENEMY);
                break;
            case TARGET_UNIT_CHAINHEAL:
            case TARGET_UNIT_NEARBY_ALLY:  // fix me
            case TARGET_UNIT_NEARBY_PARTY:
            case TARGET_UNIT_NEARBY_RAID:
                SearchChainTarget(unitList, range, maxTargets, SPELL_TARGETS_CHAINHEAL);
                break;
            }

            for (std::list<Unit*>::iterator itr = unitList.begin(); itr != unitList.end(); ++itr)
                AddUnitTarget(*itr, i);
        }
        else
            AddUnitTarget(target, i);
    }
    else if (pushType)
    {
        // Dummy, just for client
        if (sSpellMgr.EffectTargetType[m_spellInfo->Effect[i]] == SPELL_REQUIRE_DEST)
            return;

        float radius = GetSpellRadius(m_spellInfo, i, true);
        if (modOwner)
            modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_RADIUS, radius, this);

        std::list<Unit*> unitList;

        switch (cur)
        {
        case TARGET_UNIT_AREA_ENEMY_SRC:
        case TARGET_UNIT_AREA_ENEMY_DST:
        case TARGET_UNIT_CONE_ENEMY:
        case TARGET_UNIT_CONE_ENEMY_UNKNOWN:
            SearchAreaTarget(unitList, radius, pushType, SPELL_TARGETS_ENEMY);
            radius = GetSpellRadius(m_spellInfo, i, false);
            break;
        case TARGET_UNIT_AREA_ALLY_SRC:
        case TARGET_UNIT_AREA_ALLY_DST:
        case TARGET_UNIT_CONE_ALLY:
            SearchAreaTarget(unitList, radius, pushType, SPELL_TARGETS_ALLY);
            break;
        case TARGET_UNIT_AREA_PARTY_SRC:
        case TARGET_UNIT_AREA_PARTY_DST:
            m_caster->GetPartyMember(unitList, radius); //fix me
            break;
        case TARGET_GAMEOBJECT_SRC_AREA: // fix me
        case TARGET_GAMEOBJECT_DEST_AREA:
            break;
        case TARGET_UNIT_AREA_ENTRY_SRC:
        case TARGET_UNIT_AREA_ENTRY_DST:
        case TARGET_UNIT_CONE_ENTRY: // fix me
            {
                ConditionList conditions = sConditionMgr.GetConditionsForNotGroupedEntry(CONDITION_SOURCE_TYPE_SPELL_SCRIPT_TARGET, m_spellInfo->Id);
                radius = GetSpellRadius(m_spellInfo, i, IsPositiveSpell(m_spellInfo->Id));
                if (conditions.empty())
                {
                    sLog.outErrorDb("Spell (ID: %u) (caster Entry: %u) does not have record in spell_script_target", m_spellInfo->Id, m_caster->GetEntry());

                    if (IsPositiveEffect(m_spellInfo->Id, i))
                        SearchAreaTarget(unitList, radius, pushType, SPELL_TARGETS_ALLY);
                    else
                        SearchAreaTarget(unitList, radius, pushType, SPELL_TARGETS_ENEMY);
                }
                // let it be done in one check?
                else
                {
                    for (ConditionList::const_iterator i_spellST = conditions.begin(); i_spellST != conditions.end(); ++i_spellST)
                    {
                        if ((*i_spellST)->Type != CONDITION_SPELL_SCRIPT_TARGET)
                            continue;
                        if ((*i_spellST)->ConditionValue1 == SPELL_TARGET_TYPE_CREATURE)
                            SearchAreaTarget(unitList, radius, pushType, SPELL_TARGETS_ENTRY, (*i_spellST)->ConditionValue2);
                    }
                }
                break;
            }
        case TARGET_UNIT_PARTY_TARGET:
            m_targets.getUnitTarget()->GetPartyMember(unitList, radius);
            break;
        case TARGET_UNIT_PARTY_CASTER:
            m_caster->GetPartyMember(unitList, radius);
            break;
        case TARGET_UNIT_RAID_CASTER:
            m_caster->GetRaidMember(unitList, radius);
            break;
        case TARGET_UNIT_CLASS_TARGET:
            {
                Player* targetPlayer = m_targets.getUnitTarget() && m_targets.getUnitTarget()->GetTypeId() == TYPEID_PLAYER
                                       ? m_targets.getUnitTarget()->ToPlayer() : NULL;

                Group* pGroup = targetPlayer ? targetPlayer->GetGroup() : NULL;
                if (pGroup)
                {
                    for (GroupReference* itr = pGroup->GetFirstMember(); itr != NULL; itr = itr->next())
                    {
                        Player* Target = itr->GetSource();

                        // IsHostileTo check duel and controlled by enemy
                        if (Target && targetPlayer->IsWithinDistInMap(Target, radius) &&
                            targetPlayer->getClass() == Target->getClass() &&
                            !m_caster->IsHostileTo(Target))
                            AddUnitTarget(Target, i);
                    }
                }
                else if (m_targets.getUnitTarget())
                    AddUnitTarget(m_targets.getUnitTarget(), i);
                break;
            }
        }

        if (!unitList.empty())
        {
            if (m_spellValue->MaxAffectedTargets)
            {
                if (m_spellInfo->Id == 5246) //Intimidating Shout
                    unitList.remove(m_targets.getUnitTarget());

                if(sSpellMgr.SpellTargetType[cur] == TARGET_TYPE_AREA_CONE)
                    ResizeUnitListByDistance(unitList,m_caster,m_spellValue->MaxAffectedTargets,true);
                else
                    Oregon::RandomResizeList(unitList, m_spellValue->MaxAffectedTargets);
            }
            else if (m_spellInfo->Id == 27285)  // Seed of Corruption proc spell
                unitList.remove(m_targets.getUnitTarget());

            bool massbuff = IsPositiveSpell(m_spellInfo->Id) && sSpellMgr.IsNoStackSpellDueToSpell(m_spellInfo->Id, m_spellInfo->Id, false);

            for (std::list<Unit*>::iterator itr = unitList.begin(); itr != unitList.end(); )
            {
                // for mass-buffs skip targets that have higher ranks already applied.on them
                if (*itr != m_caster && massbuff)
                {
                    if ((*itr)->HasHigherRankOfAura(m_spellInfo->Id, i))
                    {
                        itr = unitList.erase(itr);
                        continue;
                    }
                }

                AddUnitTarget(*itr, i);
                ++itr;
            }
        }
    }
}

void Spell::prepare(SpellCastTargets* targets, Aura* triggeredByAura) 
{
    if (m_CastItem)
        m_castItemGUID = m_CastItem->GetGUID();
    else
        m_castItemGUID = 0;

    m_targets = *targets;

    m_spellState = SPELL_STATE_PREPARING;

    m_caster->GetPosition(m_castPositionX, m_castPositionY, m_castPositionZ);
    m_castOrientation = m_caster->GetOrientation();

    if (triggeredByAura)
        m_triggeredByAuraSpell  = triggeredByAura->GetSpellProto();

    // create and add update event for this spell
    SpellEvent* Event = new SpellEvent(this);
    m_caster->m_Events.AddEvent(Event, m_caster->m_Events.CalculateTime(1));

    //Prevent casting at cast another spell (ServerSide check)
    if (!m_IsTriggeredSpell && m_caster->IsNonMeleeSpellCast(false, true, true) && m_cast_count)
    {
        SendCastResult(SPELL_FAILED_SPELL_IN_PROGRESS);
        finish(false);
        return;
    }

    if (sDisableMgr.IsDisabledFor(DISABLE_TYPE_SPELL, m_spellInfo->Id, m_caster))
    {
        SendCastResult(SPELL_FAILED_SPELL_UNAVAILABLE);
        finish(false);
        return;
    }

    // Fill cost data (not use power for item casts)
    m_powerCost = m_CastItem ? 0 : CalculatePowerCost(m_spellInfo, m_caster, m_spellSchoolMask);

    FillTargetMap();

    SpellCastResult result = CheckCast(true);
    if (result != SPELL_CAST_OK && !IsAutoRepeat())                      //always cast autorepeat dummy for triggering
    {
        if (triggeredByAura && !triggeredByAura->IsPassive() && !IsPassiveSpell(m_spellInfo))
        {
            SendChannelUpdate(0);
            triggeredByAura->SetAuraDuration(0);
        }

        SendCastResult(result);

        finish(false);
        return;
    }

    // Prepare data for triggers
    prepareDataForTriggerSystem();

    // Set combo point requirement
    if (m_IsTriggeredSpell || m_CastItem || m_caster->GetTypeId() != TYPEID_PLAYER)
        m_needComboPoints = false;

    // calculate cast time (calculated after first CanCast check to prevent charge counting for first CanCast fail)
    m_casttime = GetSpellCastTime(m_spellInfo, this);

    // don't allow channeled spells / spells with cast time to be casted while moving
    // (even if they are interrupted on moving, spells with almost immediate effect get to have their effect processed before movement interrupter kicks in)
    if (((IsChanneledSpell(m_spellInfo) && !(m_spellInfo->AttributesEx5 & SPELL_ATTR5_CAN_CHANNEL_WHEN_MOVING)) || m_casttime) && m_caster->GetTypeId() == TYPEID_PLAYER && m_caster->isMoving() && m_spellInfo->InterruptFlags & SPELL_INTERRUPT_FLAG_MOVEMENT)
    {
        SendCastResult(SPELL_FAILED_MOVING);
        finish(false);
        return;
    }
           
    // Set cast time to 0 if .cheat casttime is enabled.
    if (m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        if (m_caster->ToPlayer()->GetCommandStatus(CHEAT_CASTTIME))
            m_casttime = 0;
    }

    // set timer base at cast time
    ReSetTimer();

    if (m_IsTriggeredSpell)
        cast(true);
    else
    {
        // stealth must be removed at cast starting (at show channel bar)
        // skip triggered spell (item equip spell casting and other not explicit character casts/item uses)
        if (isSpellBreakStealth(m_spellInfo))
            m_caster->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_CAST);

        m_caster->SetCurrentCastedSpell(this);
        SendSpellStart();

        if (m_caster->GetTypeId() == TYPEID_PLAYER)
            m_caster->ToPlayer()->AddGlobalCooldown(m_spellInfo, this);

        // item: first cast may destroy item and second cast causes crash
        if (!m_casttime && !m_spellInfo->StartRecoveryTime && !m_castItemGUID && GetCurrentContainer() == CURRENT_GENERIC_SPELL)
            cast(true);
    }
}

void Spell::cancel(bool sendInterrupt)
{
    if (m_spellState == SPELL_STATE_FINISHED)
        return;

    uint32 oldState = m_spellState;
    m_spellState = SPELL_STATE_FINISHED;

    m_autoRepeat = false;
    switch (oldState)
    {
    case SPELL_STATE_PREPARING:
    case SPELL_STATE_DELAYED:
        {
            SendInterrupted(0);
            if (sendInterrupt)
                SendCastResult(SPELL_FAILED_INTERRUPTED);
        }
        break;

    case SPELL_STATE_CASTING:
        {
            for (std::list<TargetInfo>::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
            {
                if (ihit->deleted)
                    continue;

                if (ihit->missCondition == SPELL_MISS_NONE)
                {
                    Unit* unit = m_caster->GetGUID() == (*ihit).targetGUID ? m_caster : ObjectAccessor::GetUnit(*m_caster, ihit->targetGUID);
                    if (unit && unit->IsAlive())
                        unit->RemoveAurasByCasterSpell(m_spellInfo->Id, m_caster->GetGUID());
                }
            }

            m_caster->RemoveAurasByCasterSpell(m_spellInfo->Id, m_caster->GetGUID());
            SendChannelUpdate(0);

            // Do not send interrupt for channeling spells to avoid a visual bug
            // that happens when a caster recasts the spell before the channeling ended
            if (!IsChanneledSpell(m_spellInfo))
            {
                SendInterrupted(0);
                if (sendInterrupt)
                    SendCastResult(SPELL_FAILED_INTERRUPTED);
            }
        }
        break;
    default:
        {
        } break;
    }

    SetReferencedFromCurrent(false);
    if (m_selfContainer && *m_selfContainer == this)
        *m_selfContainer = nullptr;

    m_caster->RemoveDynObject(m_spellInfo->Id);
    if (IsChanneledSpell(m_spellInfo)) // if not channeled then the object for the current cast wasn't summoned yet
    {
        m_caster->RemoveGameObject(m_spellInfo->Id, true);

        // reset cooldown state for disabled while active spells
        if (m_caster->GetTypeId() == TYPEID_PLAYER)
        {
            if (m_spellInfo->Attributes & SPELL_ATTR0_DISABLED_WHILE_ACTIVE)
                m_caster->ToPlayer()->RemoveSpellCooldown(m_spellInfo->Id, true);
        }
    }

    //set state back so finish will be processed
    m_spellState = oldState;

    finish(false);
}

void Spell::cast(bool skipCheck)
{
    if (m_spellInfo->Id > MAX_SPELL_ID)
        return;

    // update pointers base at GUIDs to prevent access to non-existed already object
    UpdatePointers();

    // cancel at lost main target unit
    if (!m_targets.getUnitTarget() && m_targets.getUnitTargetGUID())
    {
        cancel();
        return;
    }

    SetExecutedCurrently(true);
    SpellCastResult castResult = SPELL_CAST_OK;

    if (m_caster->GetTypeId() != TYPEID_PLAYER && m_targets.getUnitTarget() && m_targets.getUnitTarget() != m_caster)
        m_caster->SetInFront(m_targets.getUnitTarget());

    if (!m_IsTriggeredSpell)
    {
        castResult = CheckPower();
        if (castResult != SPELL_CAST_OK)
        {
            SendCastResult(castResult);
            finish(false);
            SetExecutedCurrently(false);
            return;
        }
    }

    // triggered cast called from Spell::prepare where it was already checked
    if (!skipCheck)
    {
        FillTargetMap();

        castResult = CheckCast(false);
        if (castResult != SPELL_CAST_OK)
        {
            SendCastResult(castResult);
            SendInterrupted(0);
            finish(false);
            SetExecutedCurrently(false);
            return;
        }
    }
    // Check if some auras need to be interrupted when casting combat auto-repeating spells
    if (IsAutoRepeat() && !IsNonCombatSpell(m_spellInfo))
        m_caster->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_ATTACK);

    if (m_spellState == SPELL_STATE_FINISHED)                // stop cast if spell marked as finish somewhere in Take*/FillTargetMap
    {
        SendInterrupted(0);
        finish(false);
        SetExecutedCurrently(false);
        return;
    }

    if (m_spellInfo->AttributesEx & SPELL_ATTR1_DISMISS_PET)
        if (Creature* pet = ObjectAccessor::GetCreature(*m_caster, m_caster->GetPetGUID()))
            pet->DespawnOrUnsummon();

    // traded items have trade slot instead of guid in m_itemTargetGUID
    // set to real guid to be sent later to the client
    m_targets.updateTradeSlotItem();

    if (!m_IsTriggeredSpell)
        TakeReagents();                                         // we must remove reagents before HandleEffects to allow place crafted item in same slot

    // CAST SPELL
    SendSpellCooldown();

    // calc miss, reflect, parry, etc.
    CalculateHitResults();

    SendSpellGo();                                          // we must send smsg_spell_go packet before m_castItem delete in TakeCastItem()...

    //handle SPELL_AURA_ADD_TARGET_TRIGGER auras
    //are there any spells need to be triggered after hit?
    Unit::AuraList const& targetTriggers = m_caster->GetAurasByType(SPELL_AURA_ADD_TARGET_TRIGGER);
    for (Unit::AuraList::const_iterator i = targetTriggers.begin(); i != targetTriggers.end(); ++i)
    {
        SpellEntry const* auraSpellInfo = (*i)->GetSpellProto();
        uint32 auraSpellIdx = (*i)->GetEffIndex();
        if (IsAffectedBy(auraSpellInfo, auraSpellIdx))
        {
            if (SpellEntry const* spellInfo = sSpellStore.LookupEntry(auraSpellInfo->EffectTriggerSpell[auraSpellIdx]))
            {
                // Calculate chance at that moment (can be depend for example from combo points)
                int32 chance = m_caster->CalculateSpellDamage(auraSpellInfo, auraSpellIdx, (*i)->GetBasePoints(), NULL);
                m_ChanceTriggerSpells.push_back(std::make_pair(spellInfo, chance * (*i)->GetStackAmount()));
            }
        }
    }

    if (m_customAttr & SPELL_ATTR_CU_CHARGE)
    {
        if (m_spellInfo->Effect[0] == SPELL_EFFECT_CHARGE_DEST) //swoop is always first effect
            EffectChargeDest((SpellEffIndex)0);
        else
            EffectCharge((SpellEffIndex)0);
    }

    // Okay, everything is prepared. Now we need to distinguish between immediate and evented delayed spells
    // @TODO: Find similarities for spells such as Ruthlessness and run the proper check here
    if (((m_spellInfo->speed > 0.0f ||GetCCDelay(m_spellInfo->Id) > 0) && !IsChanneledSpell(m_spellInfo)) || m_spellInfo->Id == 14157)
    {
        // Okay, maps created, now prepare flags
        m_immediateHandled = false;
        m_spellState = SPELL_STATE_DELAYED;
        SetDelayStart(0);

        if (m_caster->HasUnitState(UNIT_STATE_CASTING) && !m_caster->IsNonMeleeSpellCast(false, false, true))
            m_caster->ClearUnitState(UNIT_STATE_CASTING);
    }
    else
    {
        // Immediate spell, no big deal
        handle_immediate();
    }

    // combo points should not be taken before SPELL_AURA_ADD_TARGET_TRIGGER auras are handled
    if (!m_IsTriggeredSpell)
        TakePower();

    if (m_customAttr & SPELL_ATTR_CU_LINK_CAST)
    {
        if (const std::vector<int32>* spell_triggered = sSpellMgr.GetSpellLinked(m_spellInfo->Id))
        {
            for (std::vector<int32>::const_iterator i = spell_triggered->begin(); i != spell_triggered->end(); ++i)
                if (*i < 0)
                    m_caster->RemoveAurasDueToSpell(-(*i));
                else
                    m_caster->CastSpell(m_targets.getUnitTarget() ? m_targets.getUnitTarget() : m_caster, *i, true);
        }
    }

    // Clear spell cooldowns after every spell is cast if .cheat cooldown is enabled.
    if (m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        if (m_caster->ToPlayer()->GetCommandStatus(CHEAT_COOLDOWN))
            m_caster->ToPlayer()->RemoveSpellCooldown(m_spellInfo->Id, true);
    }

    SetExecutedCurrently(false);
}

uint32 Spell::GetCCDelay(uint32 _spell)
{
    SpellEntry const* spellproto = sSpellStore.LookupEntry(_spell);

    const uint32 delay70 = 70;
    const uint32 dalay100 = 100;
    const uint32 delay130 = 130;

    for (uint8 j = 0; j < MAX_SPELL_EFFECTS; ++j)
    {
        switch (spellproto->EffectApplyAuraName[j])
        {
        case SPELL_AURA_MOD_STUN:
            return dalay100;
            break;
        case SPELL_AURA_MOD_CONFUSE:
        case SPELL_AURA_MOD_FEAR:
        case SPELL_AURA_MOD_SILENCE:
        case SPELL_AURA_MOD_POSSESS:
            return delay130;
            break;
        case SPELL_AURA_MOD_DISARM:
        case SPELL_AURA_MOD_ROOT:
            return delay70;
            break;
        }
    }

    return 0;
}

void Spell::handle_immediate()
{
    if (m_spellInfo->Id > MAX_SPELL_ID)
        return;

    // start channeling if applicable
    if (IsChanneledSpell(m_spellInfo))
    {
        int32 duration = GetSpellDuration(m_spellInfo);
        if (m_spellValue->CustomDuration)
            duration = m_spellValue->Duration;
        if (duration > 0)
        {
            if (m_targets.getUnitTarget())
            {
                DiminishingGroup DRgroup = GetDiminishingReturnsGroupForSpell(m_spellInfo, false);
                m_targets.getUnitTarget()->ApplyDiminishingToDuration(DRgroup, duration, m_caster, m_targets.getUnitTarget()->GetDiminishing(DRgroup));
            }

            // Apply duration mod
            if (Player* modOwner = m_caster->GetSpellModOwner())
                modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_DURATION, duration);

            // Apply haste mods
            m_caster->ModSpellDurationTime(m_spellInfo, duration, this);

            m_spellState = SPELL_STATE_CASTING;
            m_caster->AddInterruptMask(m_spellInfo->ChannelInterruptFlags);
            SendChannelStart(duration);
        }
        else if (duration == -1)
        {
            m_spellState = SPELL_STATE_CASTING;
            m_caster->AddInterruptMask(m_spellInfo->ChannelInterruptFlags);
            SendChannelStart(duration);
        }

        // Interrupt movement at channeled spells for creature case
        if (m_caster->GetTypeId() == TYPEID_UNIT && m_caster->isMoving() && !(m_spellInfo->AttributesEx5 & SPELL_ATTR5_CAN_CHANNEL_WHEN_MOVING))
            m_caster->StopMoving();
    }

    // process immediate effects (items, ground, etc.) also initialize some variables
    _handle_immediate_phase();

    for (std::list<TargetInfo>::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
    {
        if (m_destroyed || ihit == m_UniqueTargetInfo.end() || m_UniqueTargetInfo.size() == 0)
            break;

        if (ihit->deleted)
            continue;

        DoAllEffectOnTarget(&(*ihit));
    }

    for (std::list<GOTargetInfo>::iterator ihit = m_UniqueGOTargetInfo.begin(); ihit != m_UniqueGOTargetInfo.end(); ++ihit)
    {
        if (m_destroyed || ihit == m_UniqueGOTargetInfo.end() || m_UniqueGOTargetInfo.size() == 0)
            break;

        if (ihit->deleted)
            continue;

        DoAllEffectOnTarget(&(*ihit));
    }

    // spell is finished, perform some last features of the spell here
    _handle_finish_phase();

    if (m_spellState != SPELL_STATE_CASTING)
        finish(true);                                       // successfully finish spell cast (not last in case autorepeat or channel spell)
}

uint64 Spell::handle_delayed(uint64 t_offset)
{
    UpdatePointers();
    uint64 next_time = 0;

    if (!m_immediateHandled)
    {
        _handle_immediate_phase();
        m_immediateHandled = true;
    }

    bool single_missile = (m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION);

    // now recheck units targeting correctness (need before any effects apply to prevent adding immunity at first effect not allow apply second spell effect and similar cases)
    for (std::list<TargetInfo>::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
    {
        if (ihit->deleted)
            continue;

        if (ihit->processed == false)
        {
            if (single_missile || ihit->timeDelay <= t_offset)
            {
                ihit->timeDelay = t_offset;
                DoAllEffectOnTarget(&(*ihit));
            }
            else if (next_time == 0 || ihit->timeDelay < next_time)
                next_time = ihit->timeDelay;
        }
    }

    // now recheck gameobject targeting correctness
    for (std::list<GOTargetInfo>::iterator ighit = m_UniqueGOTargetInfo.begin(); ighit != m_UniqueGOTargetInfo.end(); ++ighit)
    {
        if (ighit->deleted)
            continue;

        if (!ighit->processed)
        {
            if (single_missile || ighit->timeDelay <= t_offset)
                DoAllEffectOnTarget(&(*ighit));
            else if (next_time == 0 || ighit->timeDelay < next_time)
                next_time = ighit->timeDelay;
        }
    }
    // All targets passed - need finish phase
    if (next_time == 0)
    {
        // spell is finished, perform some last features of the spell here
        _handle_finish_phase();

        finish(true);                                       // successfully finish spell cast

        // return zero, spell is finished now
        return 0;
    }
    else
    {
        // spell is unfinished, return next execution time
        return next_time;
    }
}

void Spell::_handle_immediate_phase()
{
    // handle some immediate features of the spell here
    if (m_customAttr & SPELL_ATTR_CU_DIRECT_DAMAGE)
        CalculateDamageDoneForAllTargets();

    HandleThreatSpells();
    TakeCastItem();

    m_needSpellLog = m_isNeedSendToClient;

    // initialize Diminishing Returns Data
    m_diminishLevel = DIMINISHING_LEVEL_1;
    m_diminishGroup = DIMINISHING_NONE;

    // process items
    for (std::list<ItemTargetInfo>::iterator ihit = m_UniqueItemInfo.begin(); ihit != m_UniqueItemInfo.end(); ++ihit)
        DoAllEffectOnTarget(&(*ihit));

    for (int i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        if (m_spellInfo->Effect[i] == 0)
            continue;

        if (m_spellInfo->Effect[i] == SPELL_EFFECT_SCHOOL_DAMAGE || m_spellInfo->Effect[i] == 0)
            m_needSpellLog = false;

        // apply Send Event effect to ground in case empty target lists
        if (m_spellInfo->Effect[i] == SPELL_EFFECT_SEND_EVENT || m_spellInfo->Effect[i] == SPELL_EFFECT_TRANS_DOOR && !HaveTargetsForEffect(i))
        {
            HandleEffects(NULL, NULL, NULL, i);
        }else if (sSpellMgr.EffectTargetType[m_spellInfo->Effect[i]] == SPELL_REQUIRE_DEST)
        {
            if (!m_targets.HasDst())
                m_targets.setDst(m_caster);
            HandleEffects(m_originalCaster, nullptr, nullptr, i);

        }else if (sSpellMgr.EffectTargetType[m_spellInfo->Effect[i]] == SPELL_REQUIRE_NONE)
        {
            HandleEffects(m_originalCaster, nullptr, nullptr, i);

        }
    }
}

void Spell::_handle_finish_phase()
{
    // Take for real after all targets are processed
    if (m_needComboPoints)
        if (Player* player = m_caster->ToPlayer())
            player->ClearComboPoints();

    // spell log
    if (m_needSpellLog)
        SendLogExecute();
}

void Spell::SendSpellCooldown()
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* _player = m_caster->ToPlayer();
    // Add cooldown for max (disable spell)
    // Cooldown started on SendCooldownEvent call
    if (m_spellInfo->Attributes & SPELL_ATTR0_DISABLED_WHILE_ACTIVE)
        return;
  
    // init cooldown values
    uint32 cat   = 0;
    int32 rec    = -1;
    int32 catrec = -1;

    // some special item spells without correct cooldown in SpellInfo
    // cooldown information stored in item prototype
    // This used in same way in WorldSession::HandleItemQuerySingleOpcode data sending to client.

    if (m_CastItem)
    {
        ItemTemplate const* proto = m_CastItem->GetProto();
        if (proto)
        {
            for (int idx = 0; idx < 5; ++idx)
            {
                if (proto->Spells[idx].SpellId == m_spellInfo->Id)
                {
                    cat    = proto->Spells[idx].SpellCategory;
                    rec    = proto->Spells[idx].SpellCooldown;
                    catrec = proto->Spells[idx].SpellCategoryCooldown;
                    break;
                }
            }
        }
    }

    // if no cooldown found above then base at DBC data
    if (rec < 0 && catrec < 0)
    {
        cat = m_spellInfo->Category;
        rec = m_spellInfo->RecoveryTime;
        catrec = m_spellInfo->CategoryRecoveryTime;
    }

    // shoot spells used equipped item cooldown values already assigned in GetAttackTime(RANGED_ATTACK)
    // prevent 0 cooldowns set by another way
    if (rec <= 0 && catrec <= 0 && (cat == 76 || cat == 351))
        rec = _player->GetAttackTime(RANGED_ATTACK);

    // Now we have cooldown data (if found any), time to apply mods
    if (rec > 0)
        _player->ApplySpellMod(m_spellInfo->Id, SPELLMOD_COOLDOWN, rec, this);

    if (catrec > 0)
        _player->ApplySpellMod(m_spellInfo->Id, SPELLMOD_COOLDOWN, catrec, this);

    // replace negative cooldowns by 0
    if (rec < 0) rec = 0;
    if (catrec < 0) catrec = 0;

    // no cooldown after applying spell mods
    if (rec == 0 && catrec == 0)
        return;

    time_t curTime = time(NULL);

    time_t catrecTime = catrec ? curTime + catrec / IN_MILLISECONDS : 0; // in secs
    time_t recTime    = rec ? curTime + rec / IN_MILLISECONDS : catrecTime; // in secs

    // self spell cooldown
    if (recTime > 0)
        _player->AddSpellCooldown(m_spellInfo->Id, m_CastItem ? m_CastItem->GetEntry() : 0, recTime);

    // category spells
    if (catrec > 0)
    {
        SpellCategoryStore::const_iterator i_scstore = sSpellCategoryStore.find(cat);
        if (i_scstore != sSpellCategoryStore.end())
        {
            for (SpellCategorySet::const_iterator i_scset = i_scstore->second.begin(); i_scset != i_scstore->second.end(); ++i_scset)
            {
                if (*i_scset == m_spellInfo->Id)             // skip main spell, already handled above
                    continue;

                _player->AddSpellCooldown(*i_scset, m_CastItem ? m_CastItem->GetEntry() : 0, catrecTime);
            }
        }
    }
}

void Spell::update(uint32 difftime)
{
    // update pointers based at it's GUIDs
    UpdatePointers();

    if (m_targets.getUnitTargetGUID() && !m_targets.getUnitTarget())
    {
        cancel();
        return;
    }

    // check if the player or unit caster has moved before the spell finished (exclude casting on vehicles)
    if (((m_caster->GetTypeId() == TYPEID_PLAYER || m_caster->GetTypeId() == TYPEID_UNIT) && m_timer != 0) &&
        (m_castPositionX != m_caster->GetPositionX() || m_castPositionY != m_caster->GetPositionY() || m_castPositionZ != m_caster->GetPositionZ()) &&
        (m_spellInfo->Effect[0] != SPELL_EFFECT_STUCK || !m_caster->HasUnitMovementFlag(MOVEMENTFLAG_FALLINGFAR)))
    {
         // always cancel for channeled spells
        if (m_spellState == SPELL_STATE_CASTING && !m_spellInfo->AttributesEx5 & SPELL_ATTR5_CAN_CHANNEL_WHEN_MOVING)
             cancel();
        // don't cancel for melee, autorepeat, triggered and instant spells
        else if (!IsNextMeleeSwingSpell() && !IsAutoRepeat() && !m_IsTriggeredSpell && (m_spellInfo->InterruptFlags & SPELL_INTERRUPT_FLAG_MOVEMENT))
            cancel();
    }

    switch (m_spellState)
    {
    case SPELL_STATE_PREPARING:
        {
            if (m_timer > 0)
            {
                if (difftime >= (uint32)m_timer)
                    m_timer = 0;
                else
                    m_timer -= difftime;
            }

            if (m_timer == 0 && !IsNextMeleeSwingSpell() && !IsAutoRepeat())
                cast(m_spellInfo->CastingTimeIndex == 1);
        }
        break;
    case SPELL_STATE_CASTING:
        {
            if (m_timer)
            {
                if (m_caster->GetTypeId() == TYPEID_PLAYER || m_caster->GetTypeId() == TYPEID_UNIT)
                {
                    // check if player has jumped before the channeling finished
                    if (m_caster->HasUnitMovementFlag(MOVEMENTFLAG_FALLING))
                        cancel();

                    // check for incapacitating player states
                    if (m_caster->HasUnitState(UNIT_STATE_STUNNED | UNIT_STATE_CONFUSED))
                        cancel();
                }

                // check if there are alive targets left
                if (!IsAliveUnitPresentInTargetList())
                {
                    SendChannelUpdate(0);
                    finish();
                }

                if (m_timer > 0)
                {
                    if (difftime >= (uint32)m_timer)
                        m_timer = 0;
                    else
                        m_timer -= difftime;
                }
            }

            if (m_timer == 0)
            {
                SendChannelUpdate(0);

                // channeled spell processed independently for quest targeting
                // cast at creature (or GO) quest objectives update at successful cast channel finished
                // ignore autorepeat/melee casts for speed (not exist quest for spells (hm...)
                if (m_caster->GetTypeId() == TYPEID_PLAYER && !IsAutoRepeat() && !IsNextMeleeSwingSpell())
                {
                    for (std::list<TargetInfo>::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
                    {
                        if (ihit->deleted)
                            continue;

                        TargetInfo* target = &*ihit;

                        if (!IS_CREATURE_GUID(target->targetGUID))
                            continue;

                        Unit* unit = m_caster->GetGUID() == target->targetGUID ? m_caster : ObjectAccessor::GetUnit(*m_caster, target->targetGUID);
                        if (unit == NULL)
                            continue;

                        m_caster->ToPlayer()->CastedCreatureOrGO(unit->GetEntry(), unit->GetGUID(), m_spellInfo->Id);
                    }

                    for (std::list<GOTargetInfo>::iterator ihit = m_UniqueGOTargetInfo.begin(); ihit != m_UniqueGOTargetInfo.end(); ++ihit)
                    {
                        if (ihit->deleted)
                            continue;

                        GOTargetInfo* target = &*ihit;

                        GameObject* go = m_caster->GetMap()->GetGameObject(target->targetGUID);
                        if (!go)
                            continue;

                        m_caster->ToPlayer()->CastedCreatureOrGO(go->GetEntry(), go->GetGUID(), m_spellInfo->Id);
                    }
                }

                finish();
            }
        }
        break;
    default:
        {
        } break;
    }
}

void Spell::finish(bool ok)
{
    if (!m_caster)
        return;

    if (m_spellState == SPELL_STATE_FINISHED)
        return;
    m_spellState = SPELL_STATE_FINISHED;

    if (IsChanneledSpell(m_spellInfo))
        m_caster->UpdateInterruptMask();

    if (m_caster->HasUnitState(UNIT_STATE_CASTING) && !m_caster->IsNonMeleeSpellCast(false, false, true))
        m_caster->ClearUnitState(UNIT_STATE_CASTING);

    if (!ok)
    {
        //restore spell mods
        if (m_caster->GetTypeId() == TYPEID_PLAYER)
            m_caster->ToPlayer()->RestoreSpellMods(this);

        // Spell has failed so the combat is no longer being initiated
        // Do this only if the unit is initiating combat
        if (m_caster->IsInitiatingCombat())
            m_caster->SetInitiatingCombat(false);

        return;
    }
    // other code related only to successfully finished spells

    //remove spell mods
    if (Player* modOwner = m_caster->GetSpellModOwner())
    {
        // Take mods after trigger spell
        // mods are taken only on succesfull cast and independantly from targets of the spell
        modOwner->RemoveSpellMods(this);
    }

    // Okay to remove extra attacks
    if (IsSpellHaveEffect(m_spellInfo, SPELL_EFFECT_ADD_EXTRA_ATTACKS))
        m_caster->m_extraAttacks = 0;

    // Heal caster for all health leech from all targets
    if (m_healthLeech)
    {
        m_caster->ModifyHealth(m_healthLeech);
        m_caster->SendHealSpellLog(m_caster, m_spellInfo->Id, uint32(m_healthLeech));
    }

    if (IsAutoActionResetSpell())
    {
        if (!(m_spellInfo->AttributesEx2 & SPELL_ATTR2_NOT_RESET_AUTO_ACTIONS))
        {
            m_caster->resetAttackTimer(BASE_ATTACK);
            m_caster->resetAttackTimer(RANGED_ATTACK);
            if (m_caster->haveOffhandWeapon())
                m_caster->resetAttackTimer(OFF_ATTACK);
        }
    }

    // call triggered spell only at successful cast (after clear combo points -> for add some if need)
    // I assume what he means is that some triggered spells may add combo points
    if (!m_TriggerSpells.empty())
        TriggerSpell();

    // Stop Attack for some spells
    if (m_spellInfo->Attributes & SPELL_ATTR0_STOP_ATTACK_TARGET)
        m_caster->AttackStop();
}

void Spell::SendCastResult(SpellCastResult result)
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    if (m_caster->ToPlayer()->GetSession()->PlayerLoading())  // don't send cast results at loading time
        return;

    if (result != SPELL_CAST_OK)
    {
        WorldPacket data(SMSG_CAST_FAILED, (4 + 1 + 1));
        data << uint32(m_spellInfo->Id);
        data << uint8(!IsPassiveSpell(m_spellInfo) ? result : SPELL_FAILED_DONT_REPORT); // do not report failed passive spells
        data << uint8(m_cast_count);                        // single cast or multi 2.3 (0/1)
        switch (result)
        {
        case SPELL_FAILED_REQUIRES_SPELL_FOCUS:
            data << uint32(m_spellInfo->RequiresSpellFocus);
            break;
        case SPELL_FAILED_REQUIRES_AREA:
            // hardcode areas limitation case
            switch (m_spellInfo->Id)
            {
            case 41617:                             // Cenarion Mana Salve
            case 41619:                             // Cenarion Healing Salve
                data << uint32(3905);
                break;
            case 41618:                             // Bottled Nethergon Energy
            case 41620:                             // Bottled Nethergon Vapor
                data << uint32(3842);
                break;
            case 45373:                             // Bloodberry Elixir
                data << uint32(4075);
                break;
            case 32307:                             // Warmaul Ogre Banner
                data << uint32(3637);
                break;
            default:                                // default case
                data << uint32(m_spellInfo->AreaId);
                break;
            }
            break;
        case SPELL_FAILED_TOTEMS:
            if (m_spellInfo->Totem[0])
                data << uint32(m_spellInfo->Totem[0]);
            if (m_spellInfo->Totem[1])
                data << uint32(m_spellInfo->Totem[1]);
            break;
        case SPELL_FAILED_TOTEM_CATEGORY:
            if (m_spellInfo->TotemCategory[0])
                data << uint32(m_spellInfo->TotemCategory[0]);
            if (m_spellInfo->TotemCategory[1])
                data << uint32(m_spellInfo->TotemCategory[1]);
            break;
        case SPELL_FAILED_EQUIPPED_ITEM_CLASS:
        case SPELL_FAILED_EQUIPPED_ITEM_CLASS_MAINHAND:
        case SPELL_FAILED_EQUIPPED_ITEM_CLASS_OFFHAND:
            data << uint32(m_spellInfo->EquippedItemClass);
            data << uint32(m_spellInfo->EquippedItemSubClassMask);
            data << uint32(m_spellInfo->EquippedItemInventoryTypeMask);
            break;
        default:
            break;
        }
        m_caster->ToPlayer()->GetSession()->SendPacket(&data);
    }
    else
    {
        WorldPacket data(SMSG_CLEAR_EXTRA_AURA_INFO, (8 + 4));
        data << m_caster->GetPackGUID();
        data << uint32(m_spellInfo->Id);
        m_caster->ToPlayer()->GetSession()->SendPacket(&data);
    }
}

void Spell::SendSpellStart()
{
    if (!m_isNeedSendToClient)
        return;

    DEBUG_LOG("Sending SMSG_SPELL_START id=%u", m_spellInfo->Id);

    uint32 castFlags = CAST_FLAG_UNKNOWN1;
    if (IsRangedSpell())
        castFlags |= CAST_FLAG_AMMO;

    //Unit* target = m_targets.getUnitTarget() ? m_targets.getUnitTarget() : m_caster;

    WorldPacket data(SMSG_SPELL_START, (8 + 8 + 4 + 4 + 2));
    if (m_CastItem)
        data << m_CastItem->GetPackGUID();
    else
        data << m_caster->GetPackGUID();

    data << m_caster->GetPackGUID();
    data << uint32(m_spellInfo->Id);;                       // spellId
    data << uint8(m_cast_count);                            // pending spell cast?
    data << uint16(castFlags);                              // cast flags
    data << int32(m_timer);                                 // delay?
    data << m_targets;

    if (castFlags & CAST_FLAG_AMMO)
        WriteAmmoToPacket(&data);

    m_caster->SendMessageToSet(&data, true);
}

void Spell::SendSpellGo()
{
    // not send invisible spell casting
    if (!m_isNeedSendToClient)
        return;

    DEBUG_LOG("Sending SMSG_SPELL_GO id=%u", m_spellInfo->Id);

    //Unit* target = m_targets.getUnitTarget() ? m_targets.getUnitTarget() : m_caster;

    uint32 castFlags = CAST_FLAG_UNKNOWN3;

    // triggered spells with spell visual != 0 and not auto shot
    if ((m_IsTriggeredSpell && !IsAutoRepeatRangedSpell(m_spellInfo)) || m_triggeredByAuraSpell)
        castFlags |= CAST_FLAG_PENDING;

    if (IsRangedSpell())
        castFlags |= CAST_FLAG_AMMO;                        // arrows/bullets visual

    WorldPacket data(SMSG_SPELL_GO, 50);                    // guess size

    if (m_CastItem)
        data << m_CastItem->GetPackGUID();
    else
        data << m_caster->GetPackGUID();

    data << m_caster->GetPackGUID();
    data << uint32(m_spellInfo->Id);                        // spellId
    data << uint16(castFlags);                              // cast flags
    data << uint32(getMSTime());                            // timestamp

    WriteSpellGoTargets(&data);

    data << m_targets;

    if (castFlags & CAST_FLAG_AMMO)
        WriteAmmoToPacket(&data);

    m_caster->SendMessageToSet(&data, true);
}

void Spell::WriteAmmoToPacket(WorldPacket* data)
{
    uint32 ammoInventoryType = 0;
    uint32 ammoDisplayID = 0;

    if (m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        Item* pItem = m_caster->ToPlayer()->GetWeaponForAttack(RANGED_ATTACK);
        if (pItem)
        {
            ammoInventoryType = pItem->GetProto()->InventoryType;
            if (ammoInventoryType == INVTYPE_THROWN)
                ammoDisplayID = pItem->GetProto()->DisplayInfoID;
            else
            {
                uint32 ammoID = m_caster->ToPlayer()->GetUInt32Value(PLAYER_AMMO_ID);
                if (ammoID)
                {
                    ItemTemplate const* pProto = sObjectMgr.GetItemTemplate(ammoID);
                    if (pProto)
                    {
                        ammoDisplayID = pProto->DisplayInfoID;
                        ammoInventoryType = pProto->InventoryType;
                    }
                }
                else if (m_caster->HasAura(46699))      // Requires No Ammo
                {
                    ammoDisplayID = 5996;                   // normal arrow
                    ammoInventoryType = INVTYPE_AMMO;
                }
            }
        }
    }
    else
    {
        for (uint8 i = 0; i < 3; ++i)
        {
            if (uint32 item_id = m_caster->GetUInt32Value(UNIT_VIRTUAL_ITEM_INFO + i))
            {
                if (ItemTemplate const* pProto = sObjectMgr.GetItemTemplate(item_id))
                {
                    if (pProto->Class == ITEM_CLASS_WEAPON)
                    {
                        switch (pProto->SubClass)
                        {
                            case ITEM_SUBCLASS_WEAPON_THROWN:
                                ammoDisplayID = pProto->DisplayInfoID;
                                ammoInventoryType = pProto->InventoryType;
                                break;
                            case ITEM_SUBCLASS_WEAPON_BOW:
                            case ITEM_SUBCLASS_WEAPON_CROSSBOW:
                                ammoDisplayID = 5996;       // is this need fixing?
                                ammoInventoryType = INVTYPE_AMMO;
                                break;
                            case ITEM_SUBCLASS_WEAPON_GUN:
                                ammoDisplayID = 5998;       // is this need fixing?
                                ammoInventoryType = INVTYPE_AMMO;
                                break;
                        }

                        if (ammoDisplayID)
                            break;
                    }
                }
            }
        }
    }

    *data << uint32(ammoDisplayID);
    *data << uint32(ammoInventoryType);
}

void Spell::WriteSpellGoTargets(WorldPacket* data)
{
    *data << (uint8)m_countOfHit;
    for (std::list<TargetInfo>::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
    {
        if (ihit->deleted)
            continue;

        if ((*ihit).missCondition == SPELL_MISS_NONE)       // Add only hits
            *data << uint64(ihit->targetGUID);
    }

    for (std::list<GOTargetInfo>::iterator ighit = m_UniqueGOTargetInfo.begin(); ighit != m_UniqueGOTargetInfo.end(); ++ighit)
    {
        if (ighit->deleted)
            continue;

        *data << uint64(ighit->targetGUID);                 // Always hits
    }

    *data << (uint8)m_countOfMiss;
    for (std::list<TargetInfo>::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
    {
        if (ihit->deleted)
            continue;

        if (ihit->missCondition != SPELL_MISS_NONE)        // Add only miss
        {
            *data << uint64(ihit->targetGUID);
            *data << uint8(ihit->missCondition);
            if (ihit->missCondition == SPELL_MISS_REFLECT)
                *data << uint8(ihit->reflectResult);
        }
    }
}

void Spell::SendLogExecute()
{
    Unit* target = m_targets.getUnitTarget() ? m_targets.getUnitTarget() : m_caster;

    WorldPacket data(SMSG_SPELLLOGEXECUTE, (8 + 4 + 4 + 4 + 4 + 8));

    if (m_caster->GetTypeId() == TYPEID_PLAYER)
        data << m_caster->GetPackGUID();
    else
        data << target->GetPackGUID();

    data << uint32(m_spellInfo->Id);
    uint32 count1 = 1;
    data << uint32(count1);                                 // count1 (effect count?)
    for (uint32 i = 0; i < count1; ++i)
    {
        data << uint32(m_spellInfo->Effect[0]);             // spell effect
        uint32 count2 = 1;
        data << uint32(count2);                             // count2 (target count?)
        for (uint32 j = 0; j < count2; ++j)
        {
            switch (m_spellInfo->Effect[0])
            {
            case SPELL_EFFECT_POWER_DRAIN:
                if (Unit* unit = m_targets.getUnitTarget())
                    data << unit->GetPackGUID();
                else
                    data << uint8(0);
                data << uint32(0);
                data << uint32(0);
                data << float(0);
                break;
            case SPELL_EFFECT_ADD_EXTRA_ATTACKS:
                if (Unit* unit = m_targets.getUnitTarget())
                    data << unit->GetPackGUID();
                else
                    data << uint8(0);
                data << uint32(m_caster->m_extraAttacks);
                break;
            case SPELL_EFFECT_INTERRUPT_CAST:
                if (Unit* unit = m_targets.getUnitTarget())
                    data << unit->GetPackGUID();
                else
                    data << uint8(0);
                data << uint32(0);                      // spellid
                break;
            case SPELL_EFFECT_DURABILITY_DAMAGE:
                if (Unit* unit = m_targets.getUnitTarget())
                    data << unit->GetPackGUID();
                else
                    data << uint8(0);
                data << uint32(0);
                data << uint32(0);
                break;
            case SPELL_EFFECT_OPEN_LOCK:
            case SPELL_EFFECT_OPEN_LOCK_ITEM:
                if (Item* item = m_targets.getItemTarget())
                    data << item->GetPackGUID();
                else
                    data << uint8(0);
                break;
            case SPELL_EFFECT_CREATE_ITEM:
                data << uint32(m_spellInfo->EffectItemType[0]);
                break;
            case SPELL_EFFECT_SUMMON:
            case SPELL_EFFECT_TRANS_DOOR:
            case SPELL_EFFECT_SUMMON_PET:
            case SPELL_EFFECT_SUMMON_OBJECT_WILD:
            case SPELL_EFFECT_CREATE_HOUSE:
            case SPELL_EFFECT_DUEL:
            case SPELL_EFFECT_SUMMON_OBJECT_SLOT1:
            case SPELL_EFFECT_SUMMON_OBJECT_SLOT2:
            case SPELL_EFFECT_SUMMON_OBJECT_SLOT3:
            case SPELL_EFFECT_SUMMON_OBJECT_SLOT4:
                if (Unit* unit = m_targets.getUnitTarget())
                    data << unit->GetPackGUID();
                else if (m_targets.getItemTargetGUID())
                    data.appendPackGUID(m_targets.getItemTargetGUID());
                else if (GameObject* go = m_targets.getGOTarget())
                    data << go->GetPackGUID();
                else
                    data << uint8(0);                   // guid
                break;
            case SPELL_EFFECT_FEED_PET:
                data << uint32(m_targets.getItemTargetEntry());
                break;
            case SPELL_EFFECT_DISMISS_PET:
                if (Unit* unit = m_targets.getUnitTarget())
                    data << unit->GetPackGUID();
                else
                    data << uint8(0);
                break;
            default:
                return;
            }
        }
    }

    m_caster->SendMessageToSet(&data, true);
}

void Spell::SendInterrupted(uint8 result)
{
    WorldPacket data(SMSG_SPELL_FAILURE, (8 + 4 + 1));
    data << m_caster->GetPackGUID();
    data << m_spellInfo->Id;
    data << result;
    m_caster->SendMessageToSet(&data, true);

    data.Initialize(SMSG_SPELL_FAILED_OTHER, (8 + 4));
    data << m_caster->GetPackGUID();
    data << m_spellInfo->Id;
    m_caster->SendMessageToSet(&data, true);
}

void Spell::SendChannelUpdate(uint32 time)
{
    if (time == 0)
    {
        m_caster->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, 0);
        m_caster->SetUInt32Value(UNIT_CHANNEL_SPELL, 0);
    }

    WorldPacket data(MSG_CHANNEL_UPDATE, 8 + 4);
    data << m_caster->GetPackGUID();
    data << uint32(time);

    m_caster->SendMessageToSet(&data, true);
}

void Spell::SendChannelStart(uint32 duration)
{
    uint64 channelTarget = 0;
    if (m_targets.getUnitTargetGUID())
        channelTarget = m_targets.getUnitTargetGUID();
    else if (m_targets.getGOTargetGUID())
        channelTarget = m_targets.getGOTargetGUID();
    else if (m_targets.getCorpseTargetGUID())
        channelTarget = m_targets.getCorpseTargetGUID();


    m_timer = duration;
    if (channelTarget)
        m_caster->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, channelTarget);

    m_caster->SetUInt32Value(UNIT_CHANNEL_SPELL, m_spellInfo->Id);
    
    if (!m_isNeedSendToClient)
        return;

    WorldPacket data(MSG_CHANNEL_START, (8 + 4 + 4));
    data << m_caster->GetPackGUID();
    data << uint32(m_spellInfo->Id);
    data << uint32(duration);
    m_caster->SendMessageToSet(&data, true);
}

void Spell::SendResurrectRequest(Player* target)
{
    WorldPacket data(SMSG_RESURRECT_REQUEST, (8 + 4 + 2 + 4));
    data << m_caster->GetGUID();
    data << uint32(1) << uint16(0) << uint32(1);

    target->GetSession()->SendPacket(&data);
}

void Spell::SendPlaySpellVisual(uint32 SpellID)
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    WorldPacket data(SMSG_PLAY_SPELL_VISUAL, 12);
    data << uint64(m_caster->GetGUID());
    data << uint32(SpellID);                                // spell visual id?
    m_caster->ToPlayer()->GetSession()->SendPacket(&data);
}

void Spell::TakeCastItem()
{
    if (!m_CastItem || m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    // not remove cast item at triggered spell (equipping, weapon damage, etc)
    if (m_IsTriggeredSpell)
        return;

    ItemTemplate const* proto = m_CastItem->GetProto();

    if (!proto)
    {
        // This code is to avoid a crash
        // I'm not sure, if this is really an error, but I guess every item needs a prototype
        sLog.outError("Cast item has no item prototype highId=%d, lowId=%d", m_CastItem->GetGUIDHigh(), m_CastItem->GetGUIDLow());
        return;
    }

    bool expendable = false;
    bool withoutCharges = false;

    for (int i = 0; i < 5; i++)
    {
        if (proto->Spells[i].SpellId)
        {
            // item has limited charges
            if (proto->Spells[i].SpellCharges)
            {
                if (proto->Spells[i].SpellCharges < 0)
                    expendable = true;

                int32 charges = m_CastItem->GetSpellCharges(i);

                // item has charges left
                if (charges)
                {
                    (charges > 0) ? --charges : ++charges;  // abs(charges) less at 1 after use
                    if (proto->Stackable < 2)
                        m_CastItem->SetSpellCharges(i, charges);
                    m_CastItem->SetState(ITEM_CHANGED, m_caster->ToPlayer());
                }

                // all charges used
                withoutCharges = (charges == 0);
            }
        }
    }

    if (expendable && withoutCharges)
    {
        uint32 count = 1;
        m_caster->ToPlayer()->DestroyItemCount(m_CastItem, count, true);

        // prevent crash at access to deleted m_targets.getItemTarget
        if (m_CastItem == m_targets.getItemTarget())
            m_targets.setItemTarget(NULL);

        m_CastItem = NULL;
    }
}

void Spell::TakePower()
{
    if (m_CastItem || m_triggeredByAuraSpell)
        return;

    //Don't take power if the spell is cast while .cheat power is enabled.
    if (m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        if (m_caster->ToPlayer()->GetCommandStatus(CHEAT_POWER))
            return;
    }

    bool hit = true;
    if (m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        if (m_spellInfo->powerType == POWER_RAGE || m_spellInfo->powerType == POWER_ENERGY)
            if (uint64 targetGUID = m_targets.getUnitTargetGUID())
                for (std::list<TargetInfo>::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
                {
                    if (ihit->deleted)
                        continue;

                    if (ihit->targetGUID == targetGUID)
                    {
                        if (ihit->missCondition != SPELL_MISS_NONE && ihit->missCondition != SPELL_MISS_MISS/* && ihit->targetGUID != m_caster->GetGUID()*/)
                            hit = false;
                        break;
                    }
                }
    }

    if (!m_powerCost)
        return;

    // health as power used
    if (m_spellInfo->powerType == POWER_HEALTH)
    {
        m_caster->ModifyHealth(-(int32)m_powerCost);
        return;
    }

    if (m_spellInfo->powerType >= MAX_POWERS)
    {
        sLog.outError("Spell::TakePower: Unknown power type '%d'", m_spellInfo->powerType);
        return;
    }

    Powers powerType = Powers(m_spellInfo->powerType);

    m_caster->ModifyPower(powerType, -m_powerCost);

    // Set the five second timer
    if (powerType == POWER_MANA && m_powerCost > 0)
        m_caster->SetLastManaUse(getMSTime());
}

void Spell::TakeReagents()
{
    if (m_IsTriggeredSpell)                                  // reagents used in triggered spell removed by original spell or don't must be removed.
        return;

    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    ItemTemplate const* castItemTemplate = m_CastItem ? m_CastItem->GetProto() : NULL;

    // do not take reagents for these item casts
    if (castItemTemplate && castItemTemplate->Flags & ITEM_PROTO_FLAG_TRIGGERED_CAST)
        return;

    Player* p_caster = m_caster->ToPlayer();
    if (p_caster->CanCastNoReagents(m_spellInfo))
        return;

    for (uint32 x = 0; x < 8; x++)
    {
        if (m_spellInfo->Reagent[x] <= 0)
            continue;

        uint32 itemid = m_spellInfo->Reagent[x];
        uint32 itemcount = m_spellInfo->ReagentCount[x];

        // if CastItem is also spell reagent
        if (m_CastItem)
        {
            ItemTemplate const* proto = m_CastItem->GetProto();
            if (proto && proto->ItemId == itemid)
            {
                for (int s = 0; s < 5; s++)
                {
                    // CastItem will be used up and does not count as reagent
                    int32 charges = m_CastItem->GetSpellCharges(s);
                    if (proto->Spells[s].SpellCharges < 0 && abs(charges) < 2)
                    {
                        ++itemcount;
                        break;
                    }
                }

                m_CastItem = NULL;
            }
        }

        // if getItemTarget is also spell reagent
        if (m_targets.getItemTargetEntry() == itemid)
            m_targets.setItemTarget(NULL);

        p_caster->DestroyItemCount(itemid, itemcount, true);
    }
}

void Spell::HandleThreatSpells()
{
    if (m_UniqueTargetInfo.empty())
        return;

    if ((m_spellInfo->AttributesEx  & SPELL_ATTR1_NO_THREAT) ||
        (m_spellInfo->AttributesEx3 & SPELL_ATTR3_NO_INITIAL_AGGRO))
        return;

    float threat = 0.0f;
    if (SpellThreatEntry const* threatEntry = sSpellMgr.GetSpellThreatEntry(m_spellInfo->Id))
    {
        if (threatEntry->apPctMod != 0.0f)
            threat += threatEntry->apPctMod * m_caster->GetTotalAttackPowerValue(BASE_ATTACK);

        threat += threatEntry->flatMod;
    }
    else
        threat += m_spellInfo->spellLevel;

    if (threat == 0.0f)
        return;

    // since 2.0.1 threat from positive effects also is distributed among all targets, so the overall caused threat is at most the defined bonus
    threat /= m_UniqueTargetInfo.size();

    for (std::list<TargetInfo>::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
    {
        float threatToAdd = threat;
        if (ihit->missCondition != SPELL_MISS_NONE)
            threatToAdd = 0.0f;

        Unit* target = ObjectAccessor::GetUnit(*m_caster, ihit->targetGUID);
        if (!target)
            continue;

        // positive spells distribute threat among all units that are in combat with target, like healing
        if (IsPositiveSpell(m_spellInfo->Id))
            target->AddAssistThreat(m_caster, threatToAdd, m_spellInfo);
        // for negative spells threat gets distributed among affected targets
        else
            if (!target->IsFriendlyTo(m_caster))
                target->AddThreat(m_caster, threatToAdd, GetSpellSchoolMask(m_spellInfo), m_spellInfo);
    }

    sLog.outDebug("Spell %u, added an additional %f threat for %s %u target(s)", m_spellInfo->Id, threat, IsPositiveSpell(m_spellInfo->Id) ? "assisting" : "harming", uint32(m_UniqueTargetInfo.size()));
}

void Spell::HandleEffects(Unit* pUnitTarget, Item* pItemTarget, GameObject* pGOTarget, uint32 i, float /*DamageMultiplier*/)
{
    unitTarget = pUnitTarget;
    itemTarget = pItemTarget;
    gameObjTarget = pGOTarget;

    uint8 eff = m_spellInfo->Effect[i];

    DEBUG_LOG("Spell: Effect : %u", eff);

    //we do not need DamageMultiplier here.
    damage = CalculateDamage(i, NULL);

    if (eff < TOTAL_SPELL_EFFECTS)
    {
        //sLog.outDebug("WORLD: Spell FX %d < TOTAL_SPELL_EFFECTS ", eff);
        (*this.*SpellEffects[eff])((SpellEffIndex)i);
    }
    /*
    else
    {
        sLog.outDebug("WORLD: Spell FX %d > TOTAL_SPELL_EFFECTS ", eff);
        if (m_CastItem)
            EffectEnchantItemTmp(i);
        else
        {
            sLog.outError("SPELL: unknown effect %u spell id %u\n",
                eff, m_spellInfo->Id);
        }
    }
    */
}

void Spell::TriggerSpell()
{
    for (TriggerSpells::iterator si = m_TriggerSpells.begin(); si != m_TriggerSpells.end(); ++si)
    {
        Spell* spell = new Spell(m_caster, (*si), true, m_originalCasterGUID, m_selfContainer, true);
        spell->prepare(&m_targets);                         // use original spell original targets
    }
}

SpellCastResult Spell::CheckCast(bool strict)
{
    Unit* victim = m_targets.getUnitTarget();

    if (m_spellInfo->Id == 26656) // Black Qiraji Battle Tank
    {
        if (m_caster->IsMounted())
        {
            m_caster->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);
            return SPELL_FAILED_DONT_REPORT;
        }

        if (m_caster->IsInCombat())
            return SPELL_FAILED_AFFECTING_COMBAT;
    }


    if (victim)
        if (!(m_spellInfo->AttributesEx6 & SPELL_ATTR6_CAN_TARGET_INVISIBLE) && !m_caster->CanSeeOrDetect(victim))
            return SPELL_FAILED_BAD_TARGETS;

    // check death state
    if (!m_caster->IsAlive() && !(m_spellInfo->Attributes & SPELL_ATTR0_PASSIVE) && !(m_spellInfo->Attributes & SPELL_ATTR0_CASTABLE_WHILE_DEAD || (m_IsTriggeredSpell && !m_triggeredByAuraSpell)))
        return SPELL_FAILED_CASTER_DEAD;

    // check cooldowns to prevent cheating
    if (!m_IsTriggeredSpell)
        if (Player* plr = m_caster->ToPlayer())
            if (plr->HasSpellCooldown(m_spellInfo->Id) || (strict && plr->HasGlobalCooldown(m_spellInfo)))
                return SPELL_FAILED_NOT_READY;

    // only triggered spells can be processed an ended battleground
    if (!m_IsTriggeredSpell && m_caster->GetTypeId() == TYPEID_PLAYER)
        if (Battleground* bg = m_caster->ToPlayer()->GetBattleground())
            if (bg->GetStatus() == STATUS_WAIT_LEAVE)
                return SPELL_FAILED_DONT_REPORT;

    if (m_caster->GetTypeId() == TYPEID_PLAYER && VMAP::VMapFactory::createOrGetVMapManager()->isLineOfSightCalcEnabled())
    {
        if (m_spellInfo->Attributes & SPELL_ATTR0_OUTDOORS_ONLY &&
            !m_caster->GetMap()->IsOutdoors(m_caster->GetPositionX(), m_caster->GetPositionY(), m_caster->GetPositionZ()))
            return SPELL_FAILED_ONLY_OUTDOORS;

        if (m_spellInfo->Attributes & SPELL_ATTR0_INDOORS_ONLY &&
            m_caster->GetMap()->IsOutdoors(m_caster->GetPositionX(), m_caster->GetPositionY(), m_caster->GetPositionZ()))
            return SPELL_FAILED_ONLY_INDOORS;
    }

    // only check at first call, Stealth auras are already removed at second call
    // for now, ignore triggered spells
    if (strict && !m_IsTriggeredSpell)
    {
        // Cannot be used in this stance/form
        SpellCastResult shapeError = GetErrorAtShapeshiftedCast(m_spellInfo, m_caster->m_form);
        if (shapeError != SPELL_CAST_OK)
            return shapeError;

        if ((m_spellInfo->Attributes & SPELL_ATTR0_ONLY_STEALTHED) && !(m_caster->HasStealthAura()))
            return SPELL_FAILED_ONLY_STEALTHED;
    }

    // caster state requirements
    if (m_spellInfo->CasterAuraState && !m_caster->HasAuraState(AuraState(m_spellInfo->CasterAuraState)))
        return SPELL_FAILED_CASTER_AURASTATE;
    if (m_spellInfo->CasterAuraStateNot && m_caster->HasAuraState(AuraState(m_spellInfo->CasterAuraStateNot)))
        return SPELL_FAILED_CASTER_AURASTATE;

    // cancel autorepeat spells if cast start when moving
    // (not wand currently autorepeat cast delayed to moving stop anyway in spell update code)
    if (m_caster->GetTypeId() == TYPEID_PLAYER && m_caster->isMoving())
    {
        // skip stuck spell to allow use it in falling case and apply spell limitations at movement
        if ((!m_caster->HasUnitMovementFlag(MOVEMENTFLAG_FALLINGFAR) || m_spellInfo->Effect[0] != SPELL_EFFECT_STUCK) &&
            (IsAutoRepeat() || (m_spellInfo->AuraInterruptFlags & AURA_INTERRUPT_FLAG_NOT_SEATED) != 0))
            return SPELL_FAILED_MOVING;
    }

    // check spell cast conditions from database
    {
        ConditionSourceInfo condInfo = ConditionSourceInfo(m_caster);
        condInfo.mConditionTargets[1] = m_targets.getUnitTarget();
        ConditionList conditions = sConditionMgr.GetConditionsForNotGroupedEntry(CONDITION_SOURCE_TYPE_SPELL, m_spellInfo->Id);
        if (!conditions.empty() && !sConditionMgr.IsObjectMeetToConditions(condInfo, conditions))
        {
            // mLastFailedCondition can be NULL if there was an error processing the condition in Condition::Meets (i.e. wrong data for ConditionTarget or others)
            if (condInfo.mLastFailedCondition && condInfo.mLastFailedCondition->ErrorType)
                return SpellCastResult(condInfo.mLastFailedCondition->ErrorType);

            if (!condInfo.mLastFailedCondition || !condInfo.mLastFailedCondition->ConditionTarget)
                return SPELL_FAILED_CASTER_AURASTATE;

            return SPELL_FAILED_BAD_TARGETS;
        }
    }

    if (m_spellInfo->Attributes & SPELL_ATTR0_CANT_USED_IN_COMBAT && m_caster->IsInCombat())
        return SPELL_FAILED_AFFECTING_COMBAT;

    Unit* target = m_targets.getUnitTarget();

    if (target)
    {
        // target state requirements (not allowed state), apply to self also
        if (m_spellInfo->TargetAuraStateNot && target->HasAuraState(AuraState(m_spellInfo->TargetAuraStateNot)))
            return SPELL_FAILED_TARGET_AURASTATE;

        if (!m_IsTriggeredSpell && IsDeathOnlySpell(m_spellInfo) && target->IsAlive())
            return SPELL_FAILED_TARGET_NOT_DEAD;

        // @todo Find a way to check for form instead of checking byte flag, probably no problem either way
        // Not allow casting on Spirit of Redemption form, includes the priest IN redemption form, except for triggered spells  
        if (!m_IsTriggeredSpell && target->HasByteFlag(UNIT_FIELD_BYTES_2, 3, FORM_SPIRITOFREDEMPTION))
            return SPELL_FAILED_BAD_TARGETS;

        if (target != m_caster)
        {
            // target state requirements (apply to non-self only), to allow cast affects to self like Dirty Deeds
            if (m_spellInfo->TargetAuraState && !target->HasAuraState(AuraState(m_spellInfo->TargetAuraState)))
                return SPELL_FAILED_TARGET_AURASTATE;

            // Not allow casting on flying player
            if (target->IsInFlight())
                return SPELL_FAILED_BAD_TARGETS;

            // Must be behind the target.
            if (m_spellInfo->AttributesEx2 == 0x100000 && (m_spellInfo->AttributesEx & 0x200) == 0x200 && target->HasInArc(static_cast<float>(M_PI), m_caster)
                && (m_spellInfo->SpellFamilyName != SPELLFAMILY_DRUID || m_spellInfo->SpellFamilyFlags != 0x0000000000020000LL))
                return SPELL_FAILED_NOT_BEHIND;

            // Target must be facing you.
            if ((m_spellInfo->Attributes == 0x150010) && !target->HasInArc(static_cast<float>(M_PI), m_caster))
                return SPELL_FAILED_NOT_INFRONT;

            // Do not allow these spells to target creatures not tapped by us (Banish, Polymorph, many quest spells)
            if (m_spellInfo->AttributesEx2 & SPELL_ATTR2_REQUIRE_TAPPED_BY_CASTER)
                if (Creature const* targetCreature = target->ToCreature())
                    if (targetCreature->hasLootRecipient() && !targetCreature->isTappedBy(m_caster->ToPlayer()))
                        return SPELL_FAILED_CANT_CAST_ON_TAPPED;

            if (strict && m_spellInfo->AttributesEx3 & SPELL_ATTR3_PLAYERS_ONLY && target->GetTypeId() != TYPEID_PLAYER && !IsAreaOfEffectSpell(m_spellInfo))
                return SPELL_FAILED_BAD_TARGETS;

            bool isTrigger = (target->ToCreature() && target->ToCreature()->IsTrigger());
            if (!isTrigger)
            {
                WorldObject* losTarget = target;
                if (m_IsTriggeredSpell && m_triggeredByAuraSpell)
                    if (DynamicObject* dynObj = m_caster->GetDynObject(m_triggeredByAuraSpell->Id))
                        losTarget = dynObj;

                if (!IsSpellIgnoringLOS(m_spellInfo) && !m_caster->IsWithinLOSInMap(losTarget))
                    return SPELL_FAILED_LINE_OF_SIGHT;
            }

            // auto selection spell rank implemented in WorldSession::HandleCastSpellOpcode
            // this case can be triggered if rank not found (too low-level target for first rank)
            if (m_caster->GetTypeId() == TYPEID_PLAYER && !IsPassiveSpell(m_spellInfo->Id) && !m_CastItem)
            {
                for (int i = 0; i < 3; i++)
                {
                    if (IsPositiveEffect(m_spellInfo->Id, i) && m_spellInfo->Effect[i] == SPELL_EFFECT_APPLY_AURA)
                        if (target->getLevel() + 10 < m_spellInfo->spellLevel)
                            return SPELL_FAILED_LOWLEVEL;
                }
            }
        }

        // check pet presence
        for (int j = 0; j < MAX_SPELL_EFFECTS; ++j)
        {
            if (m_spellInfo->EffectImplicitTargetA[j] == TARGET_UNIT_PET)
            {
                target = m_caster->GetGuardianPet();
                if (!target)
                {
                    if (m_triggeredByAuraSpell)              // not report pet not existence for triggered spells
                        return SPELL_FAILED_DONT_REPORT;
                    else
                        return SPELL_FAILED_NO_PET;
                }
                else if (!target->IsAlive())
                    return SPELL_FAILED_TARGETS_DEAD;
                break;
            }
        }

        //check creature type
        //ignore self casts (including area casts when caster selected as target)
        if (target != m_caster)
        {
            if (!CheckTargetCreatureType(target))
            {
                if (target->GetTypeId() == TYPEID_PLAYER)
                    return SPELL_FAILED_TARGET_IS_PLAYER;
                else
                    return SPELL_FAILED_BAD_TARGETS;
            }
        }

        // @todo this check can be applied and for player to prevent cheating when IsPositiveSpell will return always correct result.
        // check target for pet/charmed casts (not self targeted), self targeted cast used for area effects and etc
        if (m_caster != target && m_caster->GetTypeId() == TYPEID_UNIT && m_caster->GetCharmerOrOwnerGUID())
        {
            // check correctness positive/negative cast target (pet cast real check and cheating check)
            if (IsPositiveSpell(m_spellInfo->Id))
            {
                //dispel positivity is dependant on target, don't check it
                if (m_caster->IsHostileTo(target) && !IsDispel(m_spellInfo))
                    return SPELL_FAILED_BAD_TARGETS;
            }
            else
            {
                if (m_caster->IsFriendlyTo(target))
                    return SPELL_FAILED_BAD_TARGETS;
            }
        }

        // spells cannot be cast if player is in fake combat also
        if (m_spellInfo->HasAttribute(SPELL_ATTR1_CANT_TARGET_IN_COMBAT) && (target->IsInCombat() || target->IsPetInCombat()))
            return SPELL_FAILED_TARGET_AFFECTING_COMBAT;
    }

    // Spell casted only in battleground
    if ((m_spellInfo->AttributesEx3 & SPELL_ATTR3_BATTLEGROUND) &&  m_caster->GetTypeId() == TYPEID_PLAYER)
        if (!m_caster->ToPlayer()->InBattleground())
            return SPELL_FAILED_ONLY_BATTLEGROUNDS;

    // do not allow spells to be cast in arenas
    // - with greater than 15 min CD without SPELL_ATTR4_USABLE_IN_ARENA flag
    // - with SPELL_ATTR4_NOT_USABLE_IN_ARENA flag
    if ((m_spellInfo->AttributesEx4 & SPELL_ATTR4_NOT_USABLE_IN_ARENA) ||
        (GetSpellRecoveryTime(m_spellInfo) > 15 * MINUTE * IN_MILLISECONDS && !(m_spellInfo->AttributesEx4 & SPELL_ATTR4_USABLE_IN_ARENA)))
        if (MapEntry const* mapEntry = sMapStore.LookupEntry(m_caster->GetMapId()))
            if (mapEntry->IsBattleArena())
                return SPELL_FAILED_NOT_IN_ARENA;

    // zone check
    if (m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        SpellCastResult locRes = IsSpellAllowedInLocation(m_spellInfo, m_caster->GetMapId(), m_caster->GetZoneId(), m_caster->GetAreaId());
        if (locRes != SPELL_CAST_OK)
            return locRes;
    }

    // not let players cast spells at mount (and let do it to creatures)
    if (m_caster->IsMounted() && m_caster->GetTypeId() == TYPEID_PLAYER && !m_IsTriggeredSpell &&
        !IsPassiveSpell(m_spellInfo->Id) && !(m_spellInfo->Attributes & SPELL_ATTR0_CASTABLE_WHILE_MOUNTED))
    {
        if (m_caster->IsInFlight())
            return SPELL_FAILED_NOT_FLYING;
        else
            return SPELL_FAILED_NOT_MOUNTED;
    }

    if (m_customAttr & SPELL_ATTR_CU_CAST_BY_ITEM_ONLY && !m_CastItem)
        return SPELL_FAILED_SPELL_UNAVAILABLE;

    SpellCastResult castResult = SPELL_CAST_OK;

    // always (except passive spells) check items (focus object can be required for any type casts)
    if (!IsPassiveSpell(m_spellInfo->Id))
    {
        castResult = CheckItems();
        if (castResult != SPELL_CAST_OK)
            return castResult;
    }

    if (!m_IsTriggeredSpell)
    {
        if (!m_triggeredByAuraSpell)
        {
            castResult = CheckRange(strict);
            if (castResult != SPELL_CAST_OK)
                return castResult;
        }
    }

    // ImpliciteTargetA-B = 38, If fact there is 0 Spell with  ImpliciteTargetB=38
    if (m_UniqueTargetInfo.empty())                          // skip second canCast apply (for delayed spells for example)
    {
        for (uint8 j = 0; j < MAX_SPELL_EFFECTS; j++)
        {
            if ((m_spellInfo->EffectImplicitTargetA[j] == TARGET_UNIT_NEARBY_ENTRY) ||
                (m_spellInfo->EffectImplicitTargetB[j] == TARGET_UNIT_NEARBY_ENTRY && m_spellInfo->EffectImplicitTargetA[j] != TARGET_UNIT_CASTER) ||
                (m_spellInfo->EffectImplicitTargetA[j] == TARGET_DST_NEARBY_ENTRY) ||
                (m_spellInfo->EffectImplicitTargetB[j] == TARGET_DST_NEARBY_ENTRY))
            {
                ConditionList conditions = sConditionMgr.GetConditionsForNotGroupedEntry(CONDITION_SOURCE_TYPE_SPELL_SCRIPT_TARGET, m_spellInfo->Id);
                if (conditions.empty())
                    sLog.outErrorDb("Spell (ID: %u) has effect EffectImplicitTargetA/EffectImplicitTargetB = TARGET_UNIT_NEARBY_ENTRY or TARGET_DST_NEARBY_ENTRY, but does not have record in conditions.", m_spellInfo->Id);

                SpellRangeEntry const* srange = sSpellRangeStore.LookupEntry(m_spellInfo->rangeIndex);
                float range = GetSpellMaxRange(srange);

                Creature* creatureScriptTarget = NULL;
                GameObject* goScriptTarget = NULL;

            for (ConditionList::const_iterator i_spellST = conditions.begin(); i_spellST != conditions.end(); ++i_spellST)
            {
                if ((*i_spellST)->Type != CONDITION_SPELL_SCRIPT_TARGET)
                    continue;
                    switch((*i_spellST)->ConditionValue1)
                    {
                    case SPELL_TARGET_TYPE_GAMEOBJECT:
                        {
                            GameObject* p_GameObject = NULL;

                            if ((*i_spellST)->ConditionValue2)
                            {
                                CellCoord p(Oregon::ComputeCellCoord(m_caster->GetPositionX(), m_caster->GetPositionY()));
                                Cell cell(p);

                                Oregon::NearestGameObjectEntryInObjectRangeCheck go_check(*m_caster, (*i_spellST)->ConditionValue2, range);
                                Oregon::GameObjectLastSearcher<Oregon::NearestGameObjectEntryInObjectRangeCheck> checker(p_GameObject, go_check);

                                TypeContainerVisitor<Oregon::GameObjectLastSearcher<Oregon::NearestGameObjectEntryInObjectRangeCheck>, GridTypeMapContainer > object_checker(checker);
                                cell.Visit(p, object_checker, *m_caster->GetMap(), *m_caster, range);

                                if (p_GameObject)
                                {
                                    // remember found target and range, next attempt will find more near target with another entry
                                    creatureScriptTarget = NULL;
                                    goScriptTarget = p_GameObject;
                                    range = go_check.GetLastRange();
                                }
                            }
                            else if (focusObject)          //Focus Object
                            {
                                float frange = m_caster->GetDistance(focusObject);
                                if (range >= frange)
                                {
                                    creatureScriptTarget = NULL;
                                    goScriptTarget = focusObject;
                                    range = frange;
                                }
                            }
                            break;
                        }
                    case SPELL_TARGET_TYPE_CREATURE:
                    case SPELL_TARGET_TYPE_DEAD:
                    default:
                        {
                            Creature* p_Creature = NULL;

                            CellCoord p(Oregon::ComputeCellCoord(m_caster->GetPositionX(), m_caster->GetPositionY()));
                            Cell cell(p);
                            cell.SetNoCreate();             // Really don't know what is that???

                            Oregon::NearestCreatureEntryWithLiveStateInObjectRangeCheck u_check(*m_caster, (*i_spellST)->ConditionValue2, (*i_spellST)->ConditionValue1 != SPELL_TARGET_TYPE_DEAD, range);
                            Oregon::CreatureLastSearcher<Oregon::NearestCreatureEntryWithLiveStateInObjectRangeCheck> searcher(p_Creature, u_check);

                            TypeContainerVisitor<Oregon::CreatureLastSearcher<Oregon::NearestCreatureEntryWithLiveStateInObjectRangeCheck>, GridTypeMapContainer >  grid_creature_searcher(searcher);

                            cell.Visit(p, grid_creature_searcher, *m_caster->GetMap(), *m_caster, range);

                            if (p_Creature)
                            {
                                creatureScriptTarget = p_Creature;
                                goScriptTarget = NULL;
                                range = u_check.GetLastRange();
                            }
                            break;
                        }
                    }
                }

                if (creatureScriptTarget)
                {
                    // store coordinates for TARGET_DST_NEARBY_ENTRY
                    if (m_spellInfo->EffectImplicitTargetA[j] == TARGET_DST_NEARBY_ENTRY ||
                        m_spellInfo->EffectImplicitTargetB[j] == TARGET_DST_NEARBY_ENTRY)
                    {
                        m_targets.setDst(creatureScriptTarget->GetPositionX(), creatureScriptTarget->GetPositionY(), creatureScriptTarget->GetPositionZ(), creatureScriptTarget->GetOrientation());

                        if (m_spellInfo->EffectImplicitTargetA[j] == TARGET_DST_NEARBY_ENTRY && m_spellInfo->EffectImplicitTargetB[j] == 0 && m_spellInfo->Effect[j] != SPELL_EFFECT_PERSISTENT_AREA_AURA)
                            AddUnitTarget(creatureScriptTarget, j);
                    }
                    // store explicit target for TARGET_UNIT_NEARBY_ENTRY
                    else
                        AddUnitTarget(creatureScriptTarget, j);
                }
                else if (goScriptTarget)
                {
                    // store coordinates for TARGET_DST_NEARBY_ENTRY
                    if (m_spellInfo->EffectImplicitTargetA[j] == TARGET_DST_NEARBY_ENTRY ||
                        m_spellInfo->EffectImplicitTargetB[j] == TARGET_DST_NEARBY_ENTRY)
                    {
                        m_targets.setDst(goScriptTarget->GetPositionX(), goScriptTarget->GetPositionY(), goScriptTarget->GetPositionZ(), goScriptTarget->GetOrientation());

                        if (m_spellInfo->EffectImplicitTargetA[j] == TARGET_DST_NEARBY_ENTRY && m_spellInfo->EffectImplicitTargetB[j] == 0 && m_spellInfo->Effect[j] != SPELL_EFFECT_PERSISTENT_AREA_AURA)
                            AddGOTarget(goScriptTarget, j);
                    }
                    // store explicit target for TARGET_UNIT_NEARBY_ENTRY
                    else
                        AddGOTarget(goScriptTarget, j);
                }
                //Missing DB Entry or targets for this spellEffect.
                else
                {
                    // not report target not existence for triggered spells
                    if (m_triggeredByAuraSpell || m_IsTriggeredSpell)
                        return SPELL_FAILED_DONT_REPORT;
                    else
                        return SPELL_FAILED_BAD_TARGETS;
                }
            }
        }
    }

    if (!m_IsTriggeredSpell)
    {
        SpellCastResult castResult;

        castResult = CheckRange(strict);
        if (castResult != SPELL_CAST_OK)
            return castResult;

        castResult = CheckPower();
        if (castResult != SPELL_CAST_OK)
            return castResult;

        castResult = CheckCasterAuras();
        if (castResult != SPELL_CAST_OK)
            return castResult;
    }

    // check for effect-specific restrictions
    for (uint32 i = 0; i < MAX_SPELL_EFFECTS; i++)
    {
        // for effects of spells that have only one target
        switch (m_spellInfo->Effect[i])
        {
        case SPELL_EFFECT_DUMMY:
            {
                if (SpellCastResult result = CheckDummyCast(i))
                    if (result != SPELL_CAST_OK)
                        return result;
                break;
            }
        case SPELL_EFFECT_SCHOOL_DAMAGE:
            {
                // Hammer of Wrath
                if (m_spellInfo->SpellVisual == 7250)
                {
                    if (!m_targets.getUnitTarget())
                        return SPELL_FAILED_BAD_IMPLICIT_TARGETS;

                    if (m_targets.getUnitTarget()->GetHealth() > m_targets.getUnitTarget()->GetMaxHealth() * 0.2)
                        return SPELL_FAILED_BAD_TARGETS;
                }
                break;
            }
        case SPELL_EFFECT_APPLY_AURA:
            {
                // report single-target "buff degrading" (dont report for triggered spells)
                // also works only for positive spells and the buffed is always able to degrade
                // his own buffs
                if (strict && target != m_caster && target)
                    if (!m_IsTriggeredSpell && !m_triggeredByAuraSpell)
                        if (IsPositiveSpell(m_spellInfo->Id) && !IsAreaOfEffectSpell(m_spellInfo))
                            if (sSpellMgr.IsNoStackSpellDueToSpell(m_spellInfo->Id, m_spellInfo->Id, false))
                                if (target->HasHigherRankOfAura(m_spellInfo->Id, i))
                                    return SPELL_FAILED_AURA_BOUNCED;

                switch(m_spellInfo->EffectApplyAuraName[i])
                {
                    case SPELL_AURA_BIND_SIGHT:
                        {
                            // Cannot bind sight across instances/continents.
                            // Does not affect the same instance/continent, no matter the range.
                            if(target == m_caster)
                                return SPELL_FAILED_BAD_TARGETS;
                            break;
                        }
                }
                break;
            }
        case SPELL_EFFECT_LEARN_SPELL:
            {
                if (m_caster->GetTypeId() != TYPEID_PLAYER)
                    return SPELL_FAILED_BAD_TARGETS;

                if (m_spellInfo->EffectImplicitTargetA[i] != TARGET_UNIT_PET)
                    break;

                Pet* pet = m_caster->ToPlayer()->GetPet();

                if (!pet)
                    return SPELL_FAILED_NO_PET;

                SpellEntry const* learn_spellproto = sSpellStore.LookupEntry(m_spellInfo->EffectTriggerSpell[i]);

                if (!learn_spellproto)
                    return SPELL_FAILED_NOT_KNOWN;

                if (!pet->CanTakeMoreActiveSpells(learn_spellproto->Id))
                    return SPELL_FAILED_TOO_MANY_SKILLS;

                if (m_spellInfo->spellLevel > pet->getLevel())
                    return SPELL_FAILED_LOWLEVEL;

                if (!pet->HasTPForSpell(learn_spellproto->Id))
                    return SPELL_FAILED_TRAINING_POINTS;

                break;
            }
        case SPELL_EFFECT_TRIGGER_SPELL_2:          // Only Ritual of Summoning
            {
                if (m_caster->ToPlayer() && m_caster->ToPlayer()->GetBattleground())
                    return SPELL_FAILED_NOT_IN_BATTLEGROUND;
                break;
            }
        case SPELL_EFFECT_LEARN_PET_SPELL:
            {
                if (m_caster->GetTypeId() != TYPEID_PLAYER)
                    return SPELL_FAILED_BAD_TARGETS;

                Pet* pet = m_caster->ToPlayer()->GetPet();
                if (!pet)
                    return SPELL_FAILED_NO_PET;

                SpellEntry const* learn_spellproto = sSpellStore.LookupEntry(m_spellInfo->EffectTriggerSpell[i]);

                if (!learn_spellproto)
                    return SPELL_FAILED_NOT_KNOWN;

                if (!pet->CanTakeMoreActiveSpells(learn_spellproto->Id))
                    return SPELL_FAILED_TOO_MANY_SKILLS;

                if (m_spellInfo->spellLevel > pet->getLevel())
                    return SPELL_FAILED_LOWLEVEL;

                if (!pet->HasTPForSpell(learn_spellproto->Id))
                    return SPELL_FAILED_TRAINING_POINTS;

                break;
            }
        case SPELL_EFFECT_FEED_PET:
            {
                if (m_caster->GetTypeId() != TYPEID_PLAYER || !m_targets.getItemTarget())
                    return SPELL_FAILED_BAD_TARGETS;

                Pet* pet = m_caster->ToPlayer()->GetPet();

                if (!pet)
                    return SPELL_FAILED_NO_PET;

                if (!pet->HaveInDiet(m_targets.getItemTarget()->GetProto()))
                    return SPELL_FAILED_WRONG_PET_FOOD;

                if (!pet->GetCurrentFoodBenefitLevel(m_targets.getItemTarget()->GetProto()->ItemLevel))
                    return SPELL_FAILED_FOOD_LOWLEVEL;

                if (m_caster->IsInCombat() || pet->IsInCombat())
                    return SPELL_FAILED_AFFECTING_COMBAT;

                break;
            }
        case SPELL_EFFECT_POWER_BURN:
        case SPELL_EFFECT_POWER_DRAIN:
            {
                // Can be area effect, Check only for players and not check if target - caster (spell can have multiply drain/burn effects)
                if (m_caster->GetTypeId() == TYPEID_PLAYER)
                    if (Unit* target = m_targets.getUnitTarget())
                        if (target != m_caster && target->getPowerType() != uint32(m_spellInfo->EffectMiscValue[i]))
                            if (!(target->ToCreature() && target->ToCreature()->ToPet() && m_spellInfo->EffectMiscValue[i] == POWER_HAPPINESS))
                                return SPELL_FAILED_BAD_TARGETS;
                break;
            }
        case SPELL_EFFECT_CHARGE:
            {
            if (Unit* target = m_targets.getUnitTarget())
                if (!target->IsAlive())
                    return SPELL_FAILED_BAD_TARGETS;

                if (m_caster->HasUnitState(UNIT_STATE_ROOT))
                    return SPELL_FAILED_ROOTED;

                if (m_caster->GetTypeId() == TYPEID_PLAYER)
                    if (Unit* target = m_targets.getUnitTarget())
                        if (!target->IsAlive())
                            return SPELL_FAILED_BAD_TARGETS;

                if (MMAP::MMapFactory::IsPathfindingEnabled(m_caster->GetMapId()))
                {
                    Unit* target = m_targets.getUnitTarget();

                    if (!target)
                        return SPELL_FAILED_BAD_TARGETS;

                    Position pos;
                    target->GetChargeContactPoint(m_caster, pos.m_positionX, pos.m_positionY, pos.m_positionZ);

                    float maxdist = MELEE_RANGE + m_caster->GetMeleeReach() + target->GetMeleeReach();
                    if (target->GetExactDistSq(&pos) > maxdist*maxdist)
                        return SPELL_FAILED_NOPATH;

                    if (m_caster->GetMapId() == 572) // pussywizard: 572 Ruins of Lordaeron
                    {
                        if (pos.GetPositionX() < 1275.0f || m_caster->GetPositionX() < 1275.0f) // special case (acid)
                            break; // can't force path because the way is around and the path is too long
                    }

                    if (m_caster->GetTypeId() == TYPEID_PLAYER && m_caster->ToPlayer()->GetTransport())
                        break;

                    m_pathFinder = new PathInfo(m_caster);
                    m_pathFinder->Update(pos.m_positionX, pos.m_positionY, pos.m_positionZ + 0.15f, false);
                    G3D::Vector3 endPos = m_pathFinder->getEndPosition(); // also check distance between target and the point calculated by mmaps
                    if (m_pathFinder->getPathType() & PATHFIND_NOPATH || target->GetExactDistSq(endPos.x, endPos.y, endPos.z) > maxdist*maxdist || m_pathFinder->getPathLength() > 45.0f)
                        return SPELL_FAILED_NOPATH;
                }
                break;
            }
        case SPELL_EFFECT_SKINNING:
            {
                if (m_caster->GetTypeId() != TYPEID_PLAYER || !m_targets.getUnitTarget() || m_targets.getUnitTarget()->GetTypeId() != TYPEID_UNIT)
                    return SPELL_FAILED_BAD_TARGETS;

                if (!m_targets.getUnitTarget()->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE))
                    return SPELL_FAILED_TARGET_UNSKINNABLE;

                Creature* creature = m_targets.getUnitTarget()->ToCreature();
                if (!creature->IsCritter() && !creature->loot.isLooted())
                    return SPELL_FAILED_TARGET_NOT_LOOTED;

                uint32 skill = creature->GetCreatureTemplate()->GetRequiredLootSkill();

                int32 skillValue = m_caster->ToPlayer()->GetSkillValue(skill);
                int32 TargetLevel = m_targets.getUnitTarget()->getLevel();
                int32 ReqValue = (skillValue < 100 ? (TargetLevel - 10) * 10 : TargetLevel * 5);
                if (ReqValue > skillValue)
                    return SPELL_FAILED_LOW_CASTLEVEL;

                // chance for fail at orange skinning attempt
                if ((m_selfContainer && (*m_selfContainer) == this) &&
                    skillValue < sWorld.GetConfigMaxSkillValue() &&
                    (ReqValue < 0 ? 0 : ReqValue) > irand(skillValue - 25, skillValue + 37))
                    return SPELL_FAILED_TRY_AGAIN;

                break;
            }
        case SPELL_EFFECT_OPEN_LOCK_ITEM:
        case SPELL_EFFECT_OPEN_LOCK:
            {
                if (m_spellInfo->EffectImplicitTargetA[i] != TARGET_GAMEOBJECT &&
                    m_spellInfo->EffectImplicitTargetA[i] != TARGET_GAMEOBJECT_ITEM)
                    break;

                // we need a go target in case of TARGET_GAMEOBJECT
                if (m_spellInfo->EffectImplicitTargetA[i] == TARGET_GAMEOBJECT && !m_targets.getGOTarget())
                    return SPELL_FAILED_BAD_TARGETS;

                Item* pTempItem = nullptr;
                if (m_targets.m_targetMask & TARGET_FLAG_ITEM)
                    pTempItem = m_caster->ToPlayer()->GetItemByGuid(m_targets.getItemTargetGUID());

                // we need a go target, or an openable item target in case of TARGET_GAMEOBJECT_ITEM_TARGET
                if (m_spellInfo->EffectImplicitTargetA[i] == TARGET_GAMEOBJECT_ITEM &&
                    !m_targets.getGOTarget() &&
                    (!pTempItem || !pTempItem->GetProto()->LockID || !pTempItem->IsLocked()))
                    return SPELL_FAILED_BAD_TARGETS;

                if (m_spellInfo->Id != 1842 || (m_targets.getGOTarget() &&
                    m_targets.getGOTarget()->GetGOInfo()->type != GAMEOBJECT_TYPE_TRAP))
                    if (m_caster->ToPlayer()->InBattleground() && // In Battleground players can use only flags and banners
                        !m_caster->ToPlayer()->CanUseBattlegroundObject(m_targets.getGOTarget()))
                        return SPELL_FAILED_TRY_AGAIN;

                // get the lock entry
                uint32 lockId = 0;
                if (GameObject* go = m_targets.getGOTarget())
                {
                    lockId = go->GetGOInfo()->GetLockId();
                    if (!lockId)
                        return SPELL_FAILED_BAD_TARGETS;
                }
                else if (Item* itm = m_targets.getItemTarget())
                    lockId = itm->GetProto()->LockID;

                SkillType skillId = SKILL_NONE;
                int32 reqSkillValue = 0;
                int32 skillValue = 0;

                // check lock compatibility
                SpellCastResult res = CanOpenLock(i, lockId, skillId, reqSkillValue, skillValue);
                if (res != SPELL_CAST_OK)
                    return res;

                // chance for fail at lockpicking attempt
                // second check prevent fail at rechecks
                if (skillId != SKILL_NONE && (!m_selfContainer || ((*m_selfContainer) != this)))
                {
                    bool canFailAtMax = skillId == SKILL_LOCKPICKING;

                    // chance for failure in orange lockpick
                    if ((canFailAtMax || skillValue < sWorld.GetConfigMaxSkillValue()) && reqSkillValue > irand(skillValue - 25, skillValue + 37))
                        return SPELL_FAILED_TRY_AGAIN;
                }
                break;
            }
        // Bring the pet back to the land of the living
        case SPELL_EFFECT_SUMMON_DEAD_PET:
            {
                Creature* pet = m_caster->GetGuardianPet();
                if (pet && pet->IsAlive())
                    return SPELL_FAILED_ALREADY_HAVE_SUMMON;

                // Do not revive dismissed pets, they are not dead
                if (m_caster->IsPlayer() && m_caster->ToPlayer()->isPetDismissed())
                    return SPELL_FAILED_TARGET_NOT_DEAD;

                // Attempting to revive a non existing pet?
                if (m_caster->IsPlayer() && !m_caster->ToPlayer()->doesOwnPet())
                    return SPELL_FAILED_NO_PET;

                break;
            }
        // Called when we start channeling 'Dismiss pet' spell
        case SPELL_EFFECT_DISMISS_PET:
            {
                // Don't start dismissing the pet if it's dead!
                Pet* pet = m_caster->ToPlayer()->GetPet();
                if (!pet || !pet->IsAlive())
                    return SPELL_FAILED_TARGETS_DEAD;

                break;
            }
        // This is generic summon effect
        case SPELL_EFFECT_SUMMON:
            {
                SummonPropertiesEntry const* SummonProperties = sSummonPropertiesStore.LookupEntry(m_spellInfo->EffectMiscValueB[i]);
                if (!SummonProperties)
                    break;
                switch (SummonProperties->Category)
                {
                case SUMMON_CATEGORY_PET:
                    if (!m_spellInfo->AttributesEx & SPELL_ATTR1_DISMISS_PET && m_caster->GetPetGUID())
                        return SPELL_FAILED_ALREADY_HAVE_SUMMON;
                    // intentional missing break, check both GetPetGUID() and GetCharmGUID for SUMMON_CATEGORY_PET
                case SUMMON_CATEGORY_PUPPET:
                    if (m_caster->GetCharmGUID())
                        return SPELL_FAILED_ALREADY_HAVE_CHARM;
                    break;
                }
                break;
            }
        case SPELL_EFFECT_SUMMON_PET:
            {
                // Check these things only for player hunters
                if (m_caster->IsPlayer() && m_caster->getClass() == CLASS_HUNTER)
                {
                    // Player should not be able to call the pet if he doesn't own one...
                    if (!m_caster->ToPlayer()->doesOwnPet())
                        return SPELL_FAILED_NO_PET;

                    // ... or the pet is dead ...
                    if (m_caster->ToPlayer()->isPetDeadAndRemoved())
                        return SPELL_FAILED_TARGETS_DEAD;
                }
                if (m_caster->GetPetGUID())  // let warlock do a replacement summon
                {
                    if (m_caster->IsPlayer())
                    {
                        if (strict)          // starting cast, trigger pet stun (cast by pet so it doesn't attack player)
                            if (Pet* pet = m_caster->ToPlayer()->GetPet())
                                pet->CastSpell(pet, 32752, true, NULL, NULL, pet->GetGUID());
                    }
                    else if (!m_spellInfo->AttributesEx & SPELL_ATTR1_DISMISS_PET)
                        return SPELL_FAILED_ALREADY_HAVE_SUMMON;
                }

                if (m_caster->GetCharmGUID())
                    return SPELL_FAILED_ALREADY_HAVE_CHARM;

                break;
            }
        case SPELL_EFFECT_SUMMON_PLAYER:
            {
                if (m_caster->GetTypeId() != TYPEID_PLAYER)
                    return SPELL_FAILED_BAD_TARGETS;
                if (!m_caster->ToPlayer()->GetSelection())
                    return SPELL_FAILED_BAD_TARGETS;

                Player* target = sObjectMgr.GetPlayer(m_caster->ToPlayer()->GetSelection());
                if (!target || m_caster->ToPlayer() == target || !target->IsInSameRaidWith(m_caster->ToPlayer()))
                    return SPELL_FAILED_BAD_TARGETS;

                // check if our map is dungeon
                MapEntry const* map = sMapStore.LookupEntry(m_caster->GetMapId());

                if (map->IsDungeon())
                {
                    uint32 mapId = m_caster->GetMap()->GetId();
                    DungeonDifficulty difficulty = m_caster->GetMap()->GetDifficulty();
                    if (map->IsRaid())
                        if (InstancePlayerBind* targetBind = target->GetBoundInstance(mapId, difficulty))
                            if (InstancePlayerBind* casterBind = m_caster->ToPlayer()->GetBoundInstance(mapId, difficulty))
                                if (targetBind->perm && targetBind->save != casterBind->save)
                                    return SPELL_FAILED_TARGET_LOCKED_TO_RAID_INSTANCE;

                    InstanceTemplate const* instance = ObjectMgr::GetInstanceTemplate(mapId);
                    if (!instance)
                        return SPELL_FAILED_TARGET_NOT_IN_INSTANCE;
                    if (!target->Satisfy(sObjectMgr.GetAccessRequirement(instance->access_id), m_caster->GetMapId()))
                        return SPELL_FAILED_BAD_TARGETS;
                }
                break;
            }
        case SPELL_EFFECT_SUMMON_FRIEND:
            {
                if (m_caster->GetTypeId() != TYPEID_PLAYER)
                    return SPELL_FAILED_BAD_TARGETS;

                Player* buddy = sObjectMgr.GetRAFLinkedBuddyForPlayer(m_caster->ToPlayer());

                if (!buddy || !buddy->IsInWorld())
                {
                    m_caster->ToPlayer()->SendReferFriendError(RAF_ERR_NO_TARGET);
                    return SPELL_FAILED_DONT_REPORT;
                }

                if (!buddy->IsAlive())
                    return SPELL_FAILED_TARGETS_DEAD;

                if (!m_caster->ToPlayer()->IsInPartyWith(buddy))
                {
                    m_caster->ToPlayer()->SendReferFriendError(RAF_ERR_NOT_IN_PARTY, buddy->GetName());
                    return SPELL_FAILED_DONT_REPORT;
                }

                if (buddy->getLevel() > sWorld.getConfig(CONFIG_RAF_LEVEL_LIMIT))
                {
                    m_caster->ToPlayer()->SendReferFriendError(RAF_ERR_ONLY_UNTIL_60_2);
                    return SPELL_FAILED_DONT_REPORT;
                }

                // check if our map is dungeon
                if (sMapStore.LookupEntry(m_caster->GetMapId())->IsDungeon())
                {
                    InstanceTemplate const* instance = ObjectMgr::GetInstanceTemplate(m_caster->GetMapId());
                    if (!instance)
                        return SPELL_FAILED_TARGET_NOT_IN_INSTANCE;
                    if (!buddy->Satisfy(sObjectMgr.GetAccessRequirement(instance->access_id), m_caster->GetMapId()))
                        return SPELL_FAILED_BAD_TARGETS;
                }
                break;
            }
        case SPELL_EFFECT_LEAP:
        case SPELL_EFFECT_TELEPORT_UNITS_FACE_CASTER:
            {
                //Do not allow to cast it before BG starts.
                if (m_caster->GetTypeId() == TYPEID_PLAYER)
                    if (Battleground const* bg = m_caster->ToPlayer()->GetBattleground())
                        if (bg->GetStatus() != STATUS_IN_PROGRESS)
                            return SPELL_FAILED_TRY_AGAIN;
                break;
            }
        case SPELL_EFFECT_TELEPORT_UNITS:
            {
                //Do not allow use of Trinket before BG starts
                if (m_caster->GetTypeId() == TYPEID_PLAYER)
                    if (m_spellInfo->Id == 22563 || m_spellInfo->Id == 22564)
                        if (Battleground const* bg = m_caster->ToPlayer()->GetBattleground())
                            if (bg->GetStatus() != STATUS_IN_PROGRESS)
                                return SPELL_FAILED_TRY_AGAIN;
                break;
            }
        case SPELL_EFFECT_STEAL_BENEFICIAL_BUFF:
            {
                if (m_targets.getUnitTarget() == m_caster)
                    return SPELL_FAILED_BAD_TARGETS;
                break;
            }
        case SPELL_EFFECT_ENERGIZE:
            {
                // Consume Magic
                if (m_spellInfo->Id == 32676)
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return SPELL_FAILED_UNKNOWN;

                    std::vector<uint32> priest_buffs;
                    priest_buffs.clear();
                    Unit::AuraMap& Auras = m_caster->GetAuras();
                    for (Unit::AuraMap::iterator i = Auras.begin(); i != Auras.end();  ++i)
                    {
                        // get only priests auras
                        SpellEntry const* spellInfo = i->second->GetSpellProto();
                        if (spellInfo->SpellFamilyName != SPELLFAMILY_PRIEST)
                            continue;
                        // do not select passive auras
                        if (i->second->IsPassive())
                            continue;
                        // only beneficial effects count
                        if (!IsPositiveSpell(spellInfo->Id))
                            continue;
                        priest_buffs.push_back(spellInfo->Id);
                    }
                    if (!priest_buffs.empty())
                    {
                        // remove random aura from caster
                        uint32 rand_buff = urand(0, priest_buffs.size() - 1);
                        m_caster->RemoveAurasDueToSpell(priest_buffs[rand_buff]);
                    }
                    else    // if nothing to dispell, send error and break a spell
                        return SPELL_FAILED_NOTHING_TO_DISPEL;
                }
                break;
            }
        default:
            break;
        }
    }

    for (int i = 0; i < MAX_SPELL_EFFECTS; i++)
    {
        switch (m_spellInfo->EffectApplyAuraName[i])
        {
        case SPELL_AURA_DUMMY:
            {
                if (SpellCastResult result = CheckDummyCast(i))
                    if (result != SPELL_CAST_OK)
                        return result;
                break;
            }
        case SPELL_AURA_MOD_POSSESS_PET:
            {
                if (m_caster->GetTypeId() != TYPEID_PLAYER)
                    return SPELL_FAILED_NO_PET;

                Pet* pet = m_caster->ToPlayer()->GetPet();
                if (!pet)
                    return SPELL_FAILED_NO_PET;

                if (pet->GetCharmerGUID())
                    return SPELL_FAILED_CHARMED;
                break;
            }
        case SPELL_AURA_MOD_POSSESS:
        case SPELL_AURA_MOD_CHARM:
        case SPELL_AURA_AOE_CHARM:
            {
                if (m_caster->GetCharmerGUID())
                    return SPELL_FAILED_CHARMED;

                if (m_spellInfo->EffectApplyAuraName[i] == SPELL_AURA_MOD_CHARM
                    || m_spellInfo->EffectApplyAuraName[i] == SPELL_AURA_MOD_POSSESS)
                {
                    if (!m_spellInfo->AttributesEx & SPELL_ATTR1_DISMISS_PET && m_caster->GetPetGUID())
                        return SPELL_FAILED_ALREADY_HAVE_SUMMON;

                    if (m_caster->GetCharmGUID())
                        return SPELL_FAILED_ALREADY_HAVE_CHARM;
                }

                if (!m_targets.getUnitTarget())
                    return SPELL_FAILED_BAD_IMPLICIT_TARGETS;

                if (m_targets.getUnitTarget()->GetCharmerGUID() ||
                    // dont allow charming/possessing of pet that caster doesn't own
                    (m_targets.getUnitTarget()->HasUnitTypeMask(UNIT_MASK_PET) && m_targets.getUnitTarget()->ToPet()->GetOwner() != m_caster))
                    return SPELL_FAILED_CHARMED;

                if (int32(m_targets.getUnitTarget()->getLevel()) > CalculateDamage(i, m_targets.getUnitTarget()))
                    return SPELL_FAILED_HIGHLEVEL;

                if (m_spellInfo->EffectApplyAuraName[i] == SPELL_AURA_MOD_POSSESS)
                {
                    int32 lvlBounds = (m_spellInfo->EffectBasePoints[i] + 1) - m_spellInfo->baseLevel;
                    if (int32(m_targets.getUnitTarget()->getLevel()) > int32(m_caster->getLevel()) + lvlBounds)
                        return SPELL_FAILED_HIGHLEVEL;
                }
                break;
            }
        case SPELL_AURA_MOUNTED:
            {
                if (m_caster->IsInWater())
                    return SPELL_FAILED_ONLY_ABOVEWATER;

                if (m_caster->GetTypeId() == TYPEID_PLAYER && m_caster->ToPlayer()->GetTransport())
                    return SPELL_FAILED_NO_MOUNTS_ALLOWED;

                // Ignore map check if spell have AreaId. AreaId already checked and this prevent special mount spells
                bool AllowMount = !m_caster->GetMap()->IsDungeon() || m_caster->GetMap()->IsBattlegroundOrArena();
                InstanceTemplate const* it = sObjectMgr.GetInstanceTemplate(m_caster->GetMapId());
                if (it)
                    AllowMount = it->allowMount;
                if (m_caster->GetTypeId() == TYPEID_PLAYER && !AllowMount && !m_IsTriggeredSpell && m_spellInfo->AreaId && m_spellInfo->AreaId != m_caster->GetAreaId())
                    return SPELL_FAILED_NO_MOUNTS_ALLOWED;

                ShapeshiftForm form = m_caster->m_form;
                if (form == FORM_CAT          || form == FORM_TREE      || form == FORM_TRAVEL   ||
                    form == FORM_AQUA         || form == FORM_BEAR      || form == FORM_DIREBEAR ||
                    form == FORM_CREATUREBEAR || form == FORM_GHOSTWOLF || form == FORM_FLIGHT   ||
                    form == FORM_FLIGHT_EPIC  || form == FORM_MOONKIN)
                    return SPELL_FAILED_NOT_SHAPESHIFT;

                break;
            }
        case SPELL_AURA_RANGED_ATTACK_POWER_ATTACKER_BONUS:
            {
                if (!m_targets.getUnitTarget())
                    return SPELL_FAILED_BAD_IMPLICIT_TARGETS;

                // can be casted at non-friendly unit or own pet/charm
                if (m_caster->IsFriendlyTo(m_targets.getUnitTarget()))
                    return SPELL_FAILED_TARGET_FRIENDLY;

                break;
            }
        case SPELL_AURA_FLY:
        case SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED:
            {
            if (m_caster->IsInWater())
                return SPELL_FAILED_ONLY_ABOVEWATER;

            if (m_originalCaster && m_originalCaster->GetTypeId() == TYPEID_PLAYER && m_originalCaster->IsAlive())
            {
                if (AreaTableEntry const* pArea = sAreaStore.LookupEntry(m_originalCaster->GetAreaId()))
                    if (pArea->flags & 0x20000000)
                        return SPELL_FAILED_NOT_HERE;
            }
                break;
            }
        case SPELL_AURA_PERIODIC_MANA_LEECH:
            {
                if (!m_targets.getUnitTarget())
                    return SPELL_FAILED_BAD_IMPLICIT_TARGETS;

                if (m_caster->GetTypeId() != TYPEID_PLAYER || m_CastItem)
                    break;

                if (m_targets.getUnitTarget()->getPowerType() != POWER_MANA ||
                    m_targets.getUnitTarget()->GetMaxPower(POWER_MANA) == 0)
                    return SPELL_FAILED_BAD_TARGETS;

                break;
            }
        case SPELL_AURA_WATER_WALK:
            {
                if (!m_targets.getUnitTarget())
                    return SPELL_FAILED_BAD_IMPLICIT_TARGETS;

                if (m_targets.getUnitTarget()->GetTypeId() == TYPEID_PLAYER)
                {
                    Player const* player = static_cast<Player const*>(m_targets.getUnitTarget());

                    // Player is not allowed to cast water walk on shapeshifted/mounted player 
                    if (player->HasShapeshiftChangingModel() || player->IsMounted())
                        return SPELL_FAILED_BAD_TARGETS;
                }

                break;
            }
        }
    }

    // check if caster has at least 1 combo point for spells that require combo points
    if (m_needComboPoints)
        if (Player* plrCaster = m_caster->ToPlayer())
            if (!plrCaster->GetComboPoints())
                return SPELL_FAILED_NO_COMBO_POINTS;

    // all ok
    return SPELL_CAST_OK;
}

SpellCastResult Spell::CheckPetCast(Unit* target)
{
    if (!m_caster->IsAlive())
        return SPELL_FAILED_CASTER_DEAD;

    if (m_caster->IsNonMeleeSpellCast(false) && !m_IsTriggeredSpell)  //prevent spellcast interruption by another spellcast
        return SPELL_FAILED_SPELL_IN_PROGRESS;
    if (m_caster->IsInCombat() && IsNonCombatSpell(m_spellInfo))
        return SPELL_FAILED_AFFECTING_COMBAT;

    //dead owner (pets still alive when owners ressed?)
    if (Unit* owner = m_caster->GetCharmerOrOwner())
        if (!owner->IsAlive())
            return SPELL_FAILED_CASTER_DEAD;

    if (!target && m_targets.getUnitTarget())
        target = m_targets.getUnitTarget();

    for (uint32 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        if (sSpellMgr.SpellTargetType[m_spellInfo->EffectImplicitTargetA[i]] == TARGET_TYPE_UNIT_TARGET
            || sSpellMgr.SpellTargetType[m_spellInfo->EffectImplicitTargetA[i]] == TARGET_TYPE_DEST_TARGET)
        {
            if (!target)
                return SPELL_FAILED_BAD_IMPLICIT_TARGETS;
            m_targets.setUnitTarget(target);
            break;
        }
    }

    Unit* _target = m_targets.getUnitTarget();

    // for target dead/target not valid
    if (_target)
    {
        if (!_target->IsAlive())
            return SPELL_FAILED_BAD_TARGETS;

        if (!IsValidSingleTargetSpell(_target))
            return SPELL_FAILED_BAD_TARGETS;
    }

    //cooldown
    if (m_caster->ToCreature()->HasSpellCooldown(m_spellInfo->Id))
        return SPELL_FAILED_NOT_READY;

    return CheckCast(true);                                       //this allows to check spell fail 0, in combat
}

SpellCastResult Spell::CheckDummyCast(uint32 effIndex)
{
    SpellDummyConditionEntry const* sdcEntry = sSpellMgr.GetSpellDummyCondition(m_spellInfo->Id, effIndex);

    if (!sdcEntry)
        return SPELL_CAST_OK;

    Unit* unitTarget = m_targets.getUnitTarget();

    if (sdcEntry->bitMaskCondition)
    {
        if (sdcEntry->bitMaskCondition & SDC_BTM_CASTER_MUST_NOT_HAVE_PET)
            if (m_caster->GetPetGUID())
                return SPELL_FAILED_ALREADY_HAVE_SUMMON;

        if (sdcEntry->bitMaskCondition & SDC_BTM_CASTER_MUST_NOT_HAVE_CHARM)
            if (m_caster->GetCharmGUID())
                return SPELL_FAILED_ALREADY_HAVE_CHARM;

        bool creature = sdcEntry->bitMaskCondition & SDC_BTM_TARGET_MUST_BE_CREATURE;
        bool player   = sdcEntry->bitMaskCondition & SDC_BTM_TARGET_MUST_BE_PLAYER;

        //  creature && !player = TARGET MUST BE CREATURE
        // !creature &&  player = TARGET MUST BE PLAYER
        //  creature &&  player = TARGET MUST BE ANY UNIT
        // !creature && !player = TARGET is not required

        // operations with target, prevent crash if there's no target
        if (sdcEntry->bitMaskCondition & (SDC_BTM_TARGET_MUST_BE_DEAD     |
                                          SDC_BTM_TARGET_MUST_BE_PLAYER   |
                                          SDC_BTM_TARGET_MUST_BE_CREATURE |
                                          SDC_BTM_TARGET_MUST_BE_HOSTILE  |
                                          SDC_BTM_TARGET_MUST_BE_FRIENDLY |
                                          SDC_BTM_TARGET_MUST_NOT_BE_HIER_LEVEL) && !unitTarget)
            return SPELL_FAILED_BAD_IMPLICIT_TARGETS;

        if (creature)
        {
            if (!player && !unitTarget->ToCreature())
                return SPELL_FAILED_BAD_TARGETS;
        }
        else if (player && !unitTarget->ToPlayer())
            return SPELL_FAILED_BAD_TARGETS;

        bool friendly = (sdcEntry->bitMaskCondition & SDC_BTM_TARGET_MUST_BE_FRIENDLY);
        bool hostile  = (sdcEntry->bitMaskCondition & SDC_BTM_TARGET_MUST_BE_HOSTILE);

        // Note: IsHostileTo returns true even if target is neutral ( <= neutral)
        //       IsFriendlyTo returns true only if target is friendly ( > neutral)

        if (!friendly)
        {
            if (hostile)
            {
                if (!m_caster->IsHostileTo(unitTarget))
                    //    return SPELL_FAILED_BAD_TARGETS;
                    return SPELL_FAILED_TARGET_FRIENDLY;
            }
        }
        else if (!hostile && !m_caster->IsFriendlyTo(unitTarget))
            return SPELL_FAILED_TARGET_ENEMY;

        if (sdcEntry->bitMaskCondition & SDC_BTM_TARGET_MUST_BE_DEAD)
            if (!unitTarget->isDead())
                return SPELL_FAILED_TARGET_NOT_DEAD;

        if (sdcEntry->bitMaskCondition & SDC_BTM_TARGET_MUST_NOT_BE_HIER_LEVEL)
            if (unitTarget->getLevel() > m_caster->getLevel())
                return SPELL_FAILED_HIGHLEVEL;

        // 14 upper bits are for Creature Type
        if (sdcEntry->bitMaskCondition >> 18) // 32 - 14 = 18
        {
            // we have some SDC_BTM_TYPE set
            if (!unitTarget)
                return SPELL_FAILED_BAD_IMPLICIT_TARGETS;

            Creature* c = unitTarget->ToCreature();
            if (!c)
                return SPELL_FAILED_BAD_TARGETS;

            if (!(sdcEntry->bitMaskCondition & (1 << (c->GetCreatureTemplate()->type + 17))))
                return SPELL_FAILED_BAD_TARGETS;
        }

        if (sdcEntry->bitMaskCondition & SDC_BTM_NEEDS_SCRIPT_CHECK)
        {
            //switch (m_spellInfo->Id)
            //{
            //default:
                sLog.outError("Spell %u has SDC_BTM_NEEDS_SCRIPT_CHECK in bitMaskCondition (spell_dummy_condition), but hasn't been implemented, yet.", m_spellInfo->Id);
            //    break;
            //}
        }
    }

    for (uint32 i = 0; i < sizeof(sdcEntry->conditions) / sizeof(*sdcEntry->conditions); i++)
    {
        int32 data = sdcEntry->conditions[i].data;
        switch (sdcEntry->conditions[i].condition)
        {
        case SDC_NONE:
            break;
        case SDC_AURA_TARGET:
            if (!unitTarget)
                return SPELL_FAILED_BAD_TARGETS;

            if (data > 0)
            {
                if (!unitTarget->HasAura(data, EFFECT_0) &&
                    !unitTarget->HasAura(data, EFFECT_1) &&
                    !unitTarget->HasAura(data, EFFECT_2))
                    return SPELL_FAILED_BAD_TARGETS;
            }
            else
            {
                data = -data;
                if (unitTarget->HasAura(data, EFFECT_0) &&
                    unitTarget->HasAura(data, EFFECT_1) &&
                    unitTarget->HasAura(data, EFFECT_2))
                    return SPELL_FAILED_BAD_TARGETS;
            }
            break;
        case SDC_AURA_CASTER:
            if (data > 0)
            {
                if (!m_caster->HasAura(data, EFFECT_0) &&
                    !m_caster->HasAura(data, EFFECT_1) &&
                    !m_caster->HasAura(data, EFFECT_2))
                    return SPELL_FAILED_BAD_TARGETS;
            }
            else
            {
                data = -data;
                if (m_caster->HasAura(data, EFFECT_0) &&
                    m_caster->HasAura(data, EFFECT_1) &&
                    m_caster->HasAura(data, EFFECT_2))
                    return SPELL_FAILED_BAD_TARGETS;
            }
            break;
        case SDC_QUEST_TAKEN:
            if (!m_caster->ToPlayer() || m_caster->ToPlayer()->GetQuestStatus(data) != QUEST_STATUS_INCOMPLETE)
                return SPELL_FAILED_SPELL_UNAVAILABLE;
            break;
        case SDC_QUEST_REWARDED:
            if (!m_caster->ToPlayer() || !m_caster->ToPlayer()->GetQuestRewardStatus(data))
                return SPELL_FAILED_SPELL_UNAVAILABLE;
            break;
        case SDC_HAS_SPELL:
            if (!m_caster->ToPlayer() || !m_caster->ToPlayer()->HasSpell(data))
                return SPELL_FAILED_SPELL_UNAVAILABLE;
            break;
        case SDC_ACTIVE_EVENT:
            if (!sGameEventMgr.IsActiveEvent(data))
                return SPELL_FAILED_SPELL_UNAVAILABLE;
            break;
        case SDC_TEAM:
            if (!m_caster->ToPlayer() || m_caster->ToPlayer()->GetTeam() != (data ? HORDE : ALLIANCE))
                return SPELL_FAILED_SPELL_UNAVAILABLE;
            break;
        case SDC_CASTER_HP_PCT:
            {
                bool below = data < 0;
                float pct = float(labs(data)) / 100.f;

                if (below)
                {
                    if (m_caster->GetHealth() > m_caster->GetMaxHealth()*pct)
                        return SPELL_FAILED_BAD_TARGETS;
                }
                else
                {
                    if (m_caster->GetHealth() < m_caster->GetMaxHealth()*pct)
                        return SPELL_FAILED_BAD_TARGETS;
                }
                break;
            }
        case SDC_TARGET_HP_PCT:
            {
                if (!unitTarget)
                    return SPELL_FAILED_BAD_IMPLICIT_TARGETS;

                bool below = data < 0;

                if (below)
                {
                    if (unitTarget->GetHealthPct() <= data)
                        return SPELL_FAILED_BAD_TARGETS;
                }
                else
                {
                    if (unitTarget->GetHealthPct() >= data)
                        return SPELL_FAILED_BAD_TARGETS;
                }
                break;
            }
        case SDC_CASTER_MANA_PCT:
            {
                bool below = data < 0;
                float pct = float(abs(data)) / 100.f;

                if (below)
                {
                    if (m_caster->GetHealth() >= m_caster->GetMaxHealth()*pct)
                        return SPELL_FAILED_BAD_TARGETS;
                }
                else
                {
                    if (m_caster->GetHealth() <= m_caster->GetMaxHealth()*pct)
                        return SPELL_FAILED_BAD_TARGETS;
                }
                break;
            }
        case SDC_TARGET_MANA_PCT:
            {
                if (!unitTarget)
                    return SPELL_FAILED_BAD_IMPLICIT_TARGETS;

                bool below = data < 0;
                float pct = float(abs(data)) / 100.f;

                if (below)
                {
                    if (unitTarget->GetPower(POWER_MANA) >= m_caster->GetMaxPower(POWER_MANA)*pct)
                        return SPELL_FAILED_BAD_TARGETS;
                }
                else
                {
                    if (unitTarget->GetPower(POWER_MANA) <= m_caster->GetMaxPower(POWER_MANA)*pct)
                        return SPELL_FAILED_BAD_TARGETS;
                }
                break;
            }
        case SDC_TARGET_IN_ARC:
            if (!unitTarget)
                return SPELL_FAILED_BAD_IMPLICIT_TARGETS;

            if (data == 1 && !m_caster->IsHostileTo(unitTarget))
                break;

            if (!m_caster->HasInArc(float(M_PI), unitTarget))
                return SPELL_FAILED_NOT_INFRONT;
            break;
        case SDC_TARGET_EXACT_ENTRY:
            if (!unitTarget)
                return SPELL_FAILED_BAD_IMPLICIT_TARGETS;
            if (!unitTarget->ToCreature())
                return SPELL_FAILED_BAD_TARGETS;

            if (unitTarget->ToCreature()->GetEntry() != uint32(data))
                return SPELL_FAILED_BAD_TARGETS;
            break;
        case SDC_TARGET_TYPE_FLAGS:
            if (!unitTarget)
                return SPELL_FAILED_BAD_IMPLICIT_TARGETS;
            if (!(unitTarget->ToCreature() && unitTarget->ToCreature()->GetCreatureTemplate()->type_flags & uint32(data)))
                return SPELL_FAILED_BAD_TARGETS;
            break;
        case SDC_TARGET_FAMILY:
            if (!unitTarget)
                return SPELL_FAILED_BAD_IMPLICIT_TARGETS;
            if (!unitTarget->ToCreature())
                return SPELL_FAILED_BAD_TARGETS;
            if (data < 0)
            {
                if (unitTarget->ToCreature()->GetCreatureTemplate()->family == 0)
                    return SPELL_FAILED_BAD_TARGETS;
            }
            else if (unitTarget->ToCreature()->GetCreatureTemplate()->family != uint32(data))
                return SPELL_FAILED_BAD_TARGETS;
            break;
        }
    }

    return SPELL_CAST_OK;
}

SpellCastResult Spell::CheckCasterAuras() const
{
    // spells totally immuned to caster auras (wsg flag drop, give marks etc)
    if (m_spellInfo->AttributesEx6 & SPELL_ATTR6_IGNORE_CASTER_AURAS)
        return SPELL_CAST_OK;

    uint8 school_immune = 0;
    uint32 mechanic_immune = 0;
    uint32 dispel_immune = 0;

    //Check if the spell grants school or mechanic immunity.
    //We use bitmasks so the loop is done only once and not on every aura check below.
    if (m_spellInfo->AttributesEx & SPELL_ATTR1_DISPEL_AURAS_ON_IMMUNITY)
    {
        for (int i = 0; i < MAX_SPELL_EFFECTS; i ++)
        {
            if (m_spellInfo->EffectApplyAuraName[i] == SPELL_AURA_SCHOOL_IMMUNITY)
                school_immune |= uint32(m_spellInfo->EffectMiscValue[i]);
            else if (m_spellInfo->EffectApplyAuraName[i] == SPELL_AURA_MECHANIC_IMMUNITY)
                mechanic_immune |= 1 << uint32(m_spellInfo->EffectMiscValue[i]);
            else if (m_spellInfo->EffectApplyAuraName[i] == SPELL_AURA_DISPEL_IMMUNITY)
                dispel_immune |= GetDispelMask(DispelType(m_spellInfo->EffectMiscValue[i]));
        }

        // immune movement impairment and loss of control (spell data have special structure for mark this case)
        if (IsSpellRemoveAllMovementAndControlLossEffects(m_spellInfo))
            mechanic_immune = IMMUNE_TO_MOVEMENT_IMPAIRMENT_AND_LOSS_CONTROL_MASK;
    }

    //Check whether the cast should be prevented by any state you might have.
    SpellCastResult prevented_reason = SPELL_CAST_OK;
    // Have to check if there is a stun aura. Otherwise will have problems with ghost aura apply while logging out
    if (m_caster->HasAuraType(SPELL_AURA_MOD_STUN) && !(m_spellInfo->AttributesEx5 & SPELL_ATTR5_USABLE_WHILE_STUNNED))
        prevented_reason = SPELL_FAILED_STUNNED;
    else if (m_caster->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_CONFUSED) && !(m_spellInfo->AttributesEx5 & SPELL_ATTR5_USABLE_WHILE_CONFUSED))
        prevented_reason = SPELL_FAILED_CONFUSED;
    else if (m_caster->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_FLEEING) && !(m_spellInfo->AttributesEx5 & SPELL_ATTR5_USABLE_WHILE_FEARED))
        prevented_reason = SPELL_FAILED_FLEEING;
    else if (m_caster->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED) && m_spellInfo->PreventionType == SPELL_PREVENTION_TYPE_SILENCE)
        prevented_reason = SPELL_FAILED_SILENCED;
    else if (m_caster->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED) && m_spellInfo->PreventionType == SPELL_PREVENTION_TYPE_PACIFY)
        prevented_reason = SPELL_FAILED_PACIFIED;

    // Attr must make flag drop spell totally immune from all effects
    if (prevented_reason != SPELL_CAST_OK)
    {
        if (school_immune || mechanic_immune || dispel_immune)
        {
            //Checking auras is needed now, because you are prevented by some state but the spell grants immunity.
            Unit::AuraMap const& auras = m_caster->GetAuras();
            bool trapped;
            for (Unit::AuraMap::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
            {
                if (itr->second)
                {
                    trapped = false;

                    if (GetSpellMechanicMask(itr->second->GetSpellProto(), itr->second->GetEffIndex()) & mechanic_immune)
                        continue;
                    if (GetSpellSchoolMask(itr->second->GetSpellProto()) & school_immune &&                                    // we match school mask and
                        !(trapped = (itr->second->GetSpellProto()->AttributesEx & SPELL_ATTR1_UNAFFECTED_BY_SCHOOL_IMMUNE))) // the aura doesn't block us
                        continue;
                    if ((1 << (itr->second->GetSpellProto()->Dispel)) & dispel_immune)
                        continue;

                    //Make a second check for spell failed so the right SPELL_FAILED message is returned.
                    //That is needed when your casting is prevented by multiple states and you are only immune to some of them.
                    switch (itr->second->GetModifier()->m_auraname)
                    {
                    case SPELL_AURA_MOD_STUN:
                        if (trapped || !(m_spellInfo->AttributesEx5 & SPELL_ATTR5_USABLE_WHILE_STUNNED))
                            return SPELL_FAILED_STUNNED;
                    case SPELL_AURA_MOD_CONFUSE:
                        if (trapped || !(m_spellInfo->AttributesEx5 & SPELL_ATTR5_USABLE_WHILE_CONFUSED))
                            return SPELL_FAILED_CONFUSED;
                    case SPELL_AURA_MOD_FEAR:
                        if (trapped || !(m_spellInfo->AttributesEx5 & SPELL_ATTR5_USABLE_WHILE_FEARED))
                            return SPELL_FAILED_FLEEING;
                    case SPELL_AURA_MOD_SILENCE:
                    case SPELL_AURA_MOD_PACIFY:
                    case SPELL_AURA_MOD_PACIFY_SILENCE:
                        if (trapped || m_spellInfo->PreventionType == SPELL_PREVENTION_TYPE_PACIFY)
                            return SPELL_FAILED_PACIFIED;
                        else if (m_spellInfo->PreventionType == SPELL_PREVENTION_TYPE_SILENCE)
                            return SPELL_FAILED_SILENCED;
                    default:
                        break;
                    }
                }
            }
        }
        //You are prevented from casting and the spell casted does not grant immunity. Return a failed error.
        else
            return prevented_reason;
    }
    return SPELL_CAST_OK;                                               // all ok
}

bool Spell::CanAutoCast(Unit* target)
{
    uint64 targetguid = target->GetGUID();

    for (uint32 j = 0; j < 3; j++)
    {
        if (m_spellInfo->Effect[j] == SPELL_EFFECT_APPLY_AURA)
        {
            if (m_spellInfo->StackAmount <= 1)
            {
                if (target->HasAura(m_spellInfo->Id, j))
                    return false;
            }
            else
            {
                if (target->GetAuras().count(Unit::spellEffectPair(m_spellInfo->Id, j)) >= m_spellInfo->StackAmount)
                    return false;
            }
        }
        else if (IsAreaAuraEffect(m_spellInfo->Effect[j]))
        {
            if (target->HasAura(m_spellInfo->Id, j))
                return false;
        }
    }

    FillTargetMap();
    SpellCastResult result = CheckPetCast(target);

    if (result == SPELL_CAST_OK || result == SPELL_FAILED_UNIT_NOT_INFRONT)
    {
        //check if among target units, our WANTED target is as well (->only self cast spells return false)
        for (std::list<TargetInfo>::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
        {
            if (ihit->deleted)
                continue;

            if (ihit->targetGUID == targetguid)
                return true;
        }
    }
    return false;                                           //target invalid
}

SpellCastResult Spell::CheckRange(bool strict)
{
    Unit* target = m_targets.getUnitTarget();

    // special range cases
    switch (m_spellInfo->rangeIndex)
    {
    // self cast doesn't need range checking -- also for Starshards fix
    // spells that can be cast anywhere also need no check
    case SPELL_RANGE_IDX_SELF_ONLY:
    case SPELL_RANGE_IDX_ANYWHERE:
        return SPELL_CAST_OK;
    // combat range spells are treated differently
    case SPELL_RANGE_IDX_COMBAT:
        {
            if (target)
            {
                if (target == m_caster)
                    return SPELL_CAST_OK;

                if (m_caster->GetTypeId() == TYPEID_PLAYER &&
                    (m_spellInfo->FacingCasterFlags & SPELL_FACING_FLAG_INFRONT) && !m_caster->HasInArc(float(M_PI), target))
                    return !m_IsTriggeredSpell ? SPELL_FAILED_UNIT_NOT_INFRONT : SPELL_FAILED_DONT_REPORT;

                float base = ATTACK_DISTANCE;
                float range_mod = strict ? 0.0f : 5.0f;
                if (Player* modOwner = m_caster->GetSpellModOwner())
                    range_mod += modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_RANGE, base, this);

                // Not 100% sure about this one
                // but seems to work for now.
                range_mod += base;

                // with additional 5 dist for non stricted case (some melee spells have delay in apply
                return m_caster->IsWithinMeleeRange(target, range_mod) ? SPELL_CAST_OK : SPELL_FAILED_OUT_OF_RANGE;
            }
            break; // let continue in generic way for no target
        }
    }

    //add radius of caster and ~5 yds "give" for non stricred (landing) check
    float range_mod = strict ? 1.25f : 6.25;

    SpellRangeEntry const* srange = sSpellRangeStore.LookupEntry(m_spellInfo->rangeIndex);
    float max_range = GetSpellMaxRange(srange) + range_mod;
    float min_range = GetSpellMinRange(srange);
    uint32 range_type = GetSpellRangeType(srange);

    if (Player* modOwner = m_caster->GetSpellModOwner())
        modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_RANGE, max_range, this);

    if (target && target != m_caster)
    {
        if (range_type == SPELL_RANGE_MELEE)
        {
            // Because of lag, we can not check too strictly here.
            if (!m_caster->IsWithinMeleeRange(target, max_range/* - 2*MIN_MELEE_REACH*/))
                return !m_IsTriggeredSpell ? SPELL_FAILED_OUT_OF_RANGE : SPELL_FAILED_DONT_REPORT;
        }
        else if (!m_caster->IsWithinCombatRange(target, max_range))
            return !m_IsTriggeredSpell ? SPELL_FAILED_OUT_OF_RANGE : SPELL_FAILED_DONT_REPORT;

        if (range_type == SPELL_RANGE_RANGED)
        {
            if (m_caster->IsWithinMeleeRange(target))
                return !m_IsTriggeredSpell ? SPELL_FAILED_TOO_CLOSE : SPELL_FAILED_DONT_REPORT;
        }
        else if (min_range && m_caster->IsWithinCombatRange(target, min_range)) // skip this check if min_range = 0
            return SPELL_FAILED_TOO_CLOSE;

        if (m_caster->GetTypeId() == TYPEID_PLAYER &&
            (m_spellInfo->FacingCasterFlags & SPELL_FACING_FLAG_INFRONT) && !m_caster->HasInArc(float(M_PI), target))
            return !m_IsTriggeredSpell ? SPELL_FAILED_UNIT_NOT_INFRONT : SPELL_FAILED_DONT_REPORT;
    }

    if (m_targets.HasDst())
    {
        // allow far teleports (e.g. portals)
        if (IsSpellHaveEffect(m_spellInfo, SPELL_EFFECT_TELEPORT_UNITS))
            return SPELL_CAST_OK;

        WorldLocation destPos = m_targets.m_dstPos;
        if (!m_caster->IsWithinDist3d(destPos.m_positionX, destPos.m_positionY, destPos.m_positionZ, max_range))
            return SPELL_FAILED_OUT_OF_RANGE;
        if (min_range && m_caster->IsWithinDist3d(destPos.m_positionX, destPos.m_positionY, destPos.m_positionZ, min_range))
            return SPELL_FAILED_TOO_CLOSE;
    }

    return SPELL_CAST_OK;
}

SpellCastResult Spell::CheckPower()
{
    // item cast not used power
    if (m_CastItem)
        return SPELL_CAST_OK;

    // health as power used - need check health amount
    if (m_spellInfo->powerType == POWER_HEALTH)
    {
        if (int32(m_caster->GetHealth()) <= m_powerCost)
            return SPELL_FAILED_CASTER_AURASTATE;
        return SPELL_CAST_OK;
    }
    // Check valid power type
    if (m_spellInfo->powerType >= MAX_POWERS)
    {
        sLog.outError("Spell::CheckMana: Unknown power type '%d'", m_spellInfo->powerType);
        return SPELL_FAILED_UNKNOWN;
    }
    // Check power amount
    Powers powerType = Powers(m_spellInfo->powerType);
    if (int32(m_caster->GetPower(powerType)) < m_powerCost)
        return SPELL_FAILED_NO_POWER;
    else
        return SPELL_CAST_OK;
}

SpellCastResult Spell::CheckItems()
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return SPELL_CAST_OK;

    uint32 itemid, itemcount;
    Player* p_caster = m_caster->ToPlayer();

    if (!m_CastItem)
    {
        if (m_castItemGUID)
            return SPELL_FAILED_ITEM_NOT_READY;
    }
    else
    {
        itemid = m_CastItem->GetEntry();
        if (!p_caster->HasItemCount(itemid, 1))
            return SPELL_FAILED_ITEM_NOT_READY;
        else
        {
            ItemTemplate const* proto = m_CastItem->GetProto();
            if (!proto)
                return SPELL_FAILED_ITEM_NOT_READY;

            for (int i = 0; i < 5; i++)
            {
                if (proto->Spells[i].SpellCharges)
                {
                    if (m_CastItem->GetSpellCharges(i) == 0)
                        return SPELL_FAILED_NO_CHARGES_REMAIN;
                }
            }

            // Consumable cast item checks
            if (proto->Class == ITEM_CLASS_CONSUMABLE && m_targets.getUnitTarget())
            {
                // such items should only fail if there is no suitable effect at all - see Rejuvenation Potions for example
                SpellCastResult failReason = SPELL_CAST_OK;
                for (int i = 0; i < MAX_SPELL_EFFECTS; i++)
                {
                    // skip check, pet not required like checks, and for TARGET_UNIT_PET m_targets.getUnitTarget() is not the real target but the caster
                    if (m_spellInfo->EffectImplicitTargetA[i] == TARGET_UNIT_PET)
                        continue;

                    if (m_spellInfo->Effect[i] == SPELL_EFFECT_HEAL)
                    {
                        if (m_targets.getUnitTarget()->GetHealth() == m_targets.getUnitTarget()->GetMaxHealth())
                        {
                            failReason = SPELL_FAILED_ALREADY_AT_FULL_HEALTH;
                            continue;
                        }
                        else
                        {
                            failReason = SPELL_CAST_OK;
                            break;
                        }
                    }

                    // Mana Potion, Rage Potion, Thistle Tea(Rogue), ...
                    if (m_spellInfo->Effect[i] == SPELL_EFFECT_ENERGIZE)
                    {
                        if (m_spellInfo->EffectMiscValue[i] < 0 || m_spellInfo->EffectMiscValue[i] >= MAX_POWERS)
                        {
                            failReason = SPELL_FAILED_ALREADY_AT_FULL_POWER;
                            continue;
                        }

                        Powers power = Powers(m_spellInfo->EffectMiscValue[i]);
                        if (m_targets.getUnitTarget()->GetPower(power) == m_targets.getUnitTarget()->GetMaxPower(power))
                        {
                            failReason = SPELL_FAILED_ALREADY_AT_FULL_POWER;
                            continue;
                        }
                        else
                        {
                            failReason = SPELL_CAST_OK;
                            break;
                        }
                    }
                }
                if (failReason != SPELL_CAST_OK)
                    return failReason;
            }
        }
    }

    if (m_targets.getItemTargetGUID())
    {
        if (m_caster->GetTypeId() != TYPEID_PLAYER)
            return SPELL_FAILED_BAD_TARGETS;

        if (!m_targets.getItemTarget())
            return SPELL_FAILED_ITEM_GONE;

        if (!m_targets.getItemTarget()->IsFitToSpellRequirements(m_spellInfo))
            return SPELL_FAILED_EQUIPPED_ITEM_CLASS;
    }
    // if not item target then required item must be equipped
    else
    {
        if (m_caster->GetTypeId() == TYPEID_PLAYER && !m_caster->ToPlayer()->HasItemFitToSpellReqirements(m_spellInfo))
            return SPELL_FAILED_EQUIPPED_ITEM_CLASS;
    }

    // Check spell focus object
    if (m_spellInfo->RequiresSpellFocus)
    {
        CellCoord p(Oregon::ComputeCellCoord(m_caster->GetPositionX(), m_caster->GetPositionY()));
        Cell cell(p);

        GameObject* ok = NULL;
        Oregon::GameObjectFocusCheck go_check(m_caster, m_spellInfo->RequiresSpellFocus);
        Oregon::GameObjectSearcher<Oregon::GameObjectFocusCheck> checker(ok, go_check);

        TypeContainerVisitor<Oregon::GameObjectSearcher<Oregon::GameObjectFocusCheck>, GridTypeMapContainer > object_checker(checker);
        Map& map = *m_caster->GetMap();
        cell.Visit(p, object_checker, map, *m_caster, map.GetVisibilityRange());

        if (!ok)
            return SPELL_FAILED_REQUIRES_SPELL_FOCUS;

        focusObject = ok;                                   // game object found in range
    }

    if (!m_IsTriggeredSpell && !p_caster->CanCastNoReagents(m_spellInfo))
    {
        for (uint32 i = 0; i < 8; i++)
        {
            if (m_spellInfo->Reagent[i] <= 0)
                continue;

            itemid    = m_spellInfo->Reagent[i];
            itemcount = m_spellInfo->ReagentCount[i];

            // if CastItem is also spell reagent
            if (m_CastItem && m_CastItem->GetEntry() == itemid)
            {
                ItemTemplate const* proto = m_CastItem->GetProto();
                if (!proto)
                    return SPELL_FAILED_ITEM_NOT_READY;
                for (int s = 0; s < 5; s++)
                {
                    // CastItem will be used up and does not count as reagent
                    int32 charges = m_CastItem->GetSpellCharges(s);
                    if (proto->Spells[s].SpellCharges < 0 && abs(charges) < 2)
                    {
                        ++itemcount;
                        break;
                    }
                }
            }
            if (!p_caster->HasItemCount(itemid, itemcount))
                return SPELL_FAILED_ITEM_NOT_READY;      //0x54
        }
    }

    // check totem-item requirements (items presence in inventory)
    uint32 totems = 2;
    for (int i = 0; i < 2; ++i)
    {
        if (m_spellInfo->Totem[i] != 0)
        {
            if (p_caster->HasItemCount(m_spellInfo->Totem[i], 1))
            {
                totems -= 1;
                continue;
            }
        }
        else
            totems -= 1;
    }
    if (totems != 0)
        return SPELL_FAILED_TOTEMS;                  //0x7C

    //Check items for TotemCategory (items presence in inventory)
    uint32 TotemCategory = 2;
    for (int i = 0; i < 2; ++i)
    {
        if (m_spellInfo->TotemCategory[i] != 0)
        {
            if (p_caster->HasItemTotemCategory(m_spellInfo->TotemCategory[i]))
            {
                TotemCategory -= 1;
                continue;
            }
        }
        else
            TotemCategory -= 1;
    }
    if (TotemCategory != 0)
        return SPELL_FAILED_TOTEM_CATEGORY;          //0x7B

    // Special check for spell effects
    for (int i = 0; i < MAX_SPELL_EFFECTS; i++)
    {
        switch (m_spellInfo->Effect[i])
        {
        case SPELL_EFFECT_CREATE_ITEM:
            {
                if (!m_IsTriggeredSpell && m_spellInfo->EffectItemType[i])
                {
                    ItemPosCountVec dest;
                    uint8 msg = p_caster->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, m_spellInfo->EffectItemType[i], 1);
                    if (msg != EQUIP_ERR_OK)
                    {
                        p_caster->SendEquipError(msg, NULL, NULL);
                        return SPELL_FAILED_DONT_REPORT;
                    }
                }
                break;
            }
        case SPELL_EFFECT_ENCHANT_ITEM:
            {
                Item* targetItem = m_targets.getItemTarget();
                if (!targetItem)
                    return SPELL_FAILED_ITEM_NOT_FOUND;

                if (targetItem->GetProto()->ItemLevel < m_spellInfo->baseLevel)
                    return SPELL_FAILED_LOWLEVEL;
                // Not allow enchant in trade slot for some enchant type
                if (targetItem->GetOwner() != m_caster)
                {
                    uint32 enchant_id = m_spellInfo->EffectMiscValue[i];
                    SpellItemEnchantmentEntry const* pEnchant = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
                    if (!pEnchant)
                        return SPELL_FAILED_ERROR;
                    if (pEnchant->slot & ENCHANTMENT_CAN_SOULBOUND)
                        return SPELL_FAILED_NOT_TRADEABLE;
                }
                break;
            }
        case SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY:
            {
                Item* item = m_targets.getItemTarget();
                if (!item)
                    return SPELL_FAILED_ITEM_NOT_FOUND;
                // Not allow enchant in trade slot for some enchant type
                if (item->GetOwner() != m_caster)
                {
                    uint32 enchant_id = m_spellInfo->EffectMiscValue[i];
                    SpellItemEnchantmentEntry const* pEnchant = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
                    if (!pEnchant)
                        return SPELL_FAILED_ERROR;
                    if (pEnchant->slot & ENCHANTMENT_CAN_SOULBOUND)
                        return SPELL_FAILED_NOT_TRADEABLE;
                }
                break;
            }
        case SPELL_EFFECT_ENCHANT_HELD_ITEM:
            // check item existence in effect code (not output errors at offhand hold item effect to main hand for example
            break;
        case SPELL_EFFECT_DISENCHANT:
            {
                if (!m_targets.getItemTarget())
                    return SPELL_FAILED_CANT_BE_DISENCHANTED;

                // prevent disenchanting in trade slot
                if (m_targets.getItemTarget()->GetOwnerGUID() != m_caster->GetGUID())
                    return SPELL_FAILED_CANT_BE_DISENCHANTED;

                ItemTemplate const* itemProto = m_targets.getItemTarget()->GetProto();
                if (!itemProto)
                    return SPELL_FAILED_CANT_BE_DISENCHANTED;

                uint32 item_quality = itemProto->Quality;
                // 2.0.x addon: Check player enchanting level against the item disenchanting requirements
                uint32 item_disenchantskilllevel = itemProto->RequiredDisenchantSkill;
                if (item_disenchantskilllevel == uint32(-1))
                    return SPELL_FAILED_CANT_BE_DISENCHANTED;
                if (item_disenchantskilllevel > p_caster->GetSkillValue(SKILL_ENCHANTING))
                    return SPELL_FAILED_LOW_CASTLEVEL;
                if (item_quality > 4 || item_quality < 2)
                    return SPELL_FAILED_CANT_BE_DISENCHANTED;
                if (itemProto->Class != ITEM_CLASS_WEAPON && itemProto->Class != ITEM_CLASS_ARMOR)
                    return SPELL_FAILED_CANT_BE_DISENCHANTED;
                if (!itemProto->DisenchantID)
                    return SPELL_FAILED_CANT_BE_DISENCHANTED;
                break;
            }
        case SPELL_EFFECT_PROSPECTING:
            {
                if (!m_targets.getItemTarget())
                    return SPELL_FAILED_CANT_BE_PROSPECTED;
                //ensure item is a prospectable ore
                if (!(m_targets.getItemTarget()->GetProto()->Flags & ITEM_PROTO_FLAG_PROSPECTABLE))
                    return SPELL_FAILED_CANT_BE_PROSPECTED;
                //prevent prospecting in trade slot
                if (m_targets.getItemTarget()->GetOwnerGUID() != m_caster->GetGUID())
                    return SPELL_FAILED_CANT_BE_PROSPECTED;
                //Check for enough skill in jewelcrafting
                uint32 item_prospectingskilllevel = m_targets.getItemTarget()->GetProto()->RequiredSkillRank;
                if (item_prospectingskilllevel > p_caster->GetSkillValue(SKILL_JEWELCRAFTING))
                    return SPELL_FAILED_LOW_CASTLEVEL;
                //make sure the player has the required ores in inventory
                if (m_targets.getItemTarget()->GetCount() < 5)
                    return SPELL_FAILED_PROSPECT_NEED_MORE;

                if (!LootTemplates_Prospecting.HaveLootFor(m_targets.getItemTargetEntry()))
                    return SPELL_FAILED_CANT_BE_PROSPECTED;

                break;
            }
        case SPELL_EFFECT_WEAPON_DAMAGE:
        case SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL:
            {
                if (m_caster->GetTypeId() != TYPEID_PLAYER) return SPELL_FAILED_TARGET_NOT_PLAYER;
                if (m_attackType != RANGED_ATTACK)
                    break;
                Item* pItem = m_caster->ToPlayer()->GetWeaponForAttack(m_attackType);
                if (!pItem || pItem->IsBroken())
                    return SPELL_FAILED_EQUIPPED_ITEM;

                switch (pItem->GetProto()->SubClass)
                {
                case ITEM_SUBCLASS_WEAPON_THROWN:
                    {
                        uint32 ammo = pItem->GetEntry();
                        if (!m_caster->ToPlayer()->HasItemCount(ammo, 1))
                            return SPELL_FAILED_NO_AMMO;
                    };
                    break;
                case ITEM_SUBCLASS_WEAPON_GUN:
                case ITEM_SUBCLASS_WEAPON_BOW:
                case ITEM_SUBCLASS_WEAPON_CROSSBOW:
                    {
                        uint32 ammo = m_caster->ToPlayer()->GetUInt32Value(PLAYER_AMMO_ID);
                        if (!ammo)
                        {
                            // Requires No Ammo
                            if (m_caster->GetDummyAura(46699))
                                break;                      // skip other checks

                            return SPELL_FAILED_NO_AMMO;
                        }

                        ItemTemplate const* ammoProto = sObjectMgr.GetItemTemplate(ammo);
                        if (!ammoProto)
                            return SPELL_FAILED_NO_AMMO;

                        if (ammoProto->Class != ITEM_CLASS_PROJECTILE)
                            return SPELL_FAILED_NO_AMMO;

                        // check ammo ws. weapon compatibility
                        switch (pItem->GetProto()->SubClass)
                        {
                        case ITEM_SUBCLASS_WEAPON_BOW:
                        case ITEM_SUBCLASS_WEAPON_CROSSBOW:
                            if (ammoProto->SubClass != ITEM_SUBCLASS_ARROW)
                                return SPELL_FAILED_NO_AMMO;
                            break;
                        case ITEM_SUBCLASS_WEAPON_GUN:
                            if (ammoProto->SubClass != ITEM_SUBCLASS_BULLET)
                                return SPELL_FAILED_NO_AMMO;
                            break;
                        default:
                            return SPELL_FAILED_NO_AMMO;
                        }

                        if (!m_caster->ToPlayer()->HasItemCount(ammo, 1))
                            return SPELL_FAILED_NO_AMMO;
                    };
                    break;
                case ITEM_SUBCLASS_WEAPON_WAND:
                default:
                    break;
                }
                break;
            }
        default:
            break;
        }
    }

    return SPELL_CAST_OK;
}

void Spell::Delayed() // only called in DealDamage()
{
    if (!m_caster)// || m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    //if (m_spellState == SPELL_STATE_DELAYED)
    //    return;                                             // spell is active and can't be time-backed

    // spells not loosing casting time (slam, dynamites, bombs..)
    //if (!(m_spellInfo->InterruptFlags & SPELL_INTERRUPT_FLAG_DAMAGE))
    //    return;

    //check resist chance
    int32 resistChance = 100;                               //must be initialized to 100 for percent modifiers
    m_caster->ToPlayer()->ApplySpellMod(m_spellInfo->Id, SPELLMOD_NOT_LOSE_CASTING_TIME, resistChance, this);
    resistChance += m_caster->GetTotalAuraModifier(SPELL_AURA_RESIST_PUSHBACK) - 100;
    if (roll_chance_i(resistChance))
        return;

    int32 delaytime = GetNextDelayAtDamageMsTime();

    if (m_timer + delaytime > m_casttime)
    {
        delaytime = m_casttime - m_timer;
        m_timer = m_casttime;
    }
    else
        m_timer += delaytime;

    sLog.outDetail("Spell %u partially interrupted for (%d) ms at damage", m_spellInfo->Id, delaytime);

    WorldPacket data(SMSG_SPELL_DELAYED, 8 + 4);
    data << m_caster->GetPackGUID();
    data << uint32(delaytime);

    m_caster->SendMessageToSet(&data, true);
}

void Spell::DelayedChannel()
{
    if (!m_caster || m_caster->GetTypeId() != TYPEID_PLAYER || getState() != SPELL_STATE_CASTING)
        return;

    //check resist chance
    int32 resistChance = 100;                               //must be initialized to 100 for percent modifiers
    m_caster->ToPlayer()->ApplySpellMod(m_spellInfo->Id, SPELLMOD_NOT_LOSE_CASTING_TIME, resistChance, this);
    resistChance += m_caster->GetTotalAuraModifier(SPELL_AURA_RESIST_PUSHBACK) - 100;
    if (roll_chance_i(resistChance))
        return;

    int32 delaytime = GetNextDelayAtDamageMsTime();

    if (m_timer < delaytime)
    {
        delaytime = m_timer;
        m_timer = 0;
    }
    else
        m_timer -= delaytime;

    DEBUG_LOG("Spell %u partially interrupted for %i ms, new duration: %u ms", m_spellInfo->Id, delaytime, m_timer);

    for (std::list<TargetInfo>::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
    {
        if (ihit->deleted)
            continue;

        if ((*ihit).missCondition == SPELL_MISS_NONE)
        {
            Unit* unit = m_caster->GetGUID() == ihit->targetGUID ? m_caster : ObjectAccessor::GetUnit(*m_caster, ihit->targetGUID);
            if (unit)
            {
                for (int j = 0; j < 3; j++)
                    if (ihit->effectMask & (1 << j))
                        unit->DelayAura(m_spellInfo->Id, j, delaytime);
            }

        }
    }

    for (int j = 0; j < MAX_SPELL_EFFECTS; j++)
    {
        // partially interrupt persistent area auras
        DynamicObject* dynObj = m_caster->GetDynObject(m_spellInfo->Id, j);
        if (dynObj)
            dynObj->Delay(delaytime);
    }

    SendChannelUpdate(m_timer);
}

void Spell::UpdatePointers()
{
    if (m_originalCasterGUID == m_caster->GetGUID())
        m_originalCaster = m_caster;
    else
    {
        m_originalCaster = ObjectAccessor::GetUnit(*m_caster, m_originalCasterGUID);
        if (m_originalCaster && !m_originalCaster->IsInWorld()) m_originalCaster = NULL;
    }

    if (m_castItemGUID && m_caster->GetTypeId() == TYPEID_PLAYER)
        m_CastItem = m_caster->ToPlayer()->GetItemByGuid(m_castItemGUID);

    m_targets.Update(m_caster);
}

bool Spell::IsAffectedBy(SpellEntry const* spellInfo, uint32 effectId)
{
    return sSpellMgr.IsAffectedBySpell(m_spellInfo, spellInfo->Id, effectId, spellInfo->EffectItemType[effectId]);
}

bool Spell::CheckTargetCreatureType(Unit* target) const
{
    uint32 spellCreatureTargetMask = m_spellInfo->TargetCreatureType;

    // Curse of Doom : not find another way to fix spell target check :/
    if (m_spellInfo->SpellFamilyName == SPELLFAMILY_WARLOCK && m_spellInfo->SpellFamilyFlags == 0x0200000000LL)
    {
        // not allow cast at player
        if (target->GetTypeId() == TYPEID_PLAYER)
            return false;

        spellCreatureTargetMask = 0x7FF;
    }

    // Dismiss Pet and Taming Lesson skipped
    if (m_spellInfo->Id == 2641 || m_spellInfo->Id == 23356)
        spellCreatureTargetMask =  0;

    if (spellCreatureTargetMask)
    {
        uint32 TargetCreatureType = target->GetCreatureTypeMask();

        return !TargetCreatureType || (spellCreatureTargetMask & TargetCreatureType);
    }
    return true;
}

CurrentSpellTypes Spell::GetCurrentContainer()
{
    if (IsNextMeleeSwingSpell())
        return (CURRENT_MELEE_SPELL);
    else if (IsAutoRepeat())
        return (CURRENT_AUTOREPEAT_SPELL);
    else if (IsChanneledSpell(m_spellInfo))
        return (CURRENT_CHANNELED_SPELL);
    else
        return (CURRENT_GENERIC_SPELL);
}

bool Spell::CheckTarget(Unit* target, uint32 eff)
{
    if (sSpellMgr.GetSpellCustomAttr(m_spellInfo->Id) & SPELL_ATTR_CU_ANY_TARGET)
        return true;

    // Check targets for creature type mask and remove not appropriate (skip explicit self target case, maybe need other explicit targets)
    if (m_spellInfo->EffectImplicitTargetA[eff] != TARGET_UNIT_CASTER)
    {
        if (!CheckTargetCreatureType(target))
            return false;
    }

    // Check targets for not_selectable unit flag and remove
    // A player can cast spells on his pet (or other controlled unit) though in any state
    if (target != m_caster && target->GetCharmerOrOwnerGUID() != m_caster->GetGUID())
    {
        // any unattackable target skipped
        if (target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return false;

        // unselectable targets skipped in all cases except TARGET_UNIT_NEARBY_ENTRY targeting
        // in case TARGET_UNIT_NEARBY_ENTRY target selected by server always and can't be cheated
        /*if (target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE) &&
            m_spellInfo->EffectImplicitTargetA[eff] != TARGET_UNIT_NEARBY_ENTRY &&
            m_spellInfo->EffectImplicitTargetB[eff] != TARGET_UNIT_NEARBY_ENTRY)
            return false;*/
    }

    //Check player targets and remove if in GM mode or GM invisibility (for not self casting case)
    if (target != m_caster && target->GetTypeId() == TYPEID_PLAYER)
    {
        if (!target->ToPlayer()->IsVisible())
            return false;

        if (target->ToPlayer()->IsGameMaster() && !IsPositiveSpell(m_spellInfo->Id))
            return false;
    }

    //Do not check LOS for triggered spells
    if (m_IsTriggeredSpell || IsSpellIgnoringLOS(m_spellInfo))
        return true;

    switch (m_spellInfo->EffectApplyAuraName[eff])
    {
        case SPELL_AURA_BIND_SIGHT:
            if (target == m_caster)
                return false;

            if (target->GetMapId() != m_caster->GetMapId())
                return false;
            break;
        default:
            break;
    };

    //Check targets for LOS visibility (except spells without range limitations)
    switch (m_spellInfo->Effect[eff])
    {
    case SPELL_EFFECT_SUMMON_PLAYER:                    // from anywhere
        break;
    case SPELL_EFFECT_DUMMY:
        if (m_spellInfo->Id != 20577)                      // Cannibalize
            break;
    //fall through
    case SPELL_EFFECT_RESURRECT_NEW:
        // player far away, maybe his corpse near?
        if (target != m_caster && !target->IsWithinLOSInMap(m_caster))
        {
            if (!m_targets.getCorpseTargetGUID())
                return false;

            Corpse* corpse = ObjectAccessor::GetCorpse(*m_caster, m_targets.getCorpseTargetGUID());
            if (!corpse)
                return false;

            if (target->GetGUID() != corpse->GetOwnerGUID())
                return false;

            if (!corpse->IsWithinLOSInMap(m_caster))
                return false;
        }

        // all ok by some way or another, skip normal check
        break;
    default:                                            // normal case
        if (target != m_caster && !target->IsWithinLOSInMap(m_caster))
            return false;
        break;
    }

    return true;
}

Unit* Spell::SelectMagnetTarget()
{
    Unit* target = m_targets.getUnitTarget();

    if (target && m_spellInfo->DmgClass == SPELL_DAMAGE_CLASS_MAGIC && target->HasAuraType(SPELL_AURA_SPELL_MAGNET)) //Attributes & 0x10 what is this?
    {
        Unit::AuraList const& magnetAuras = target->GetAurasByType(SPELL_AURA_SPELL_MAGNET);
        for (Unit::AuraList::const_iterator itr = magnetAuras.begin(); itr != magnetAuras.end(); ++itr)
        {
            if (Unit* magnet = (*itr)->GetCaster())
            {
                if ((*itr)->m_procCharges > 0)
                {
                    (*itr)->SetAuraProcCharges((*itr)->m_procCharges - 1);

                    uint64 targetGUID = target->GetGUID();
                    uint64 magnetGUID = magnet->GetGUID();

                    AddUnitTarget(magnet, 0);

                    for (std::list<TargetInfo>::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
                    {
                        if (ihit->deleted)
                            continue;

                        // remove original target, since the
                        // magnet target should redirect the cast
                        // to itself
                        if (targetGUID == ihit->targetGUID)
                        {
                            ihit->deleted = true;
                            --m_countOfHit;
                            if (ihit->missCondition != SPELL_MISS_NONE)
                                --m_countOfMiss;
                            continue;
                        }

                        if (magnetGUID == ihit->targetGUID)                 // Found in list
                        {
                            (*ihit).damage = target->GetHealth();
                            break; // should be the last cycle anyway
                        }
                    }

                    m_targets.setUnitTarget(magnet);
                    break;
                }
            }
        }
    }

    return target;
}

void Spell::HandleHitTriggerAura()
{
    Unit* target = m_targets.getUnitTarget();

    if (target && m_spellInfo->DmgClass != SPELL_DAMAGE_CLASS_MAGIC && target->HasAuraType(SPELL_AURA_ADD_CASTER_HIT_TRIGGER))
    {
        Unit::AuraList const& hitTriggerAuras = target->GetAurasByType(SPELL_AURA_ADD_CASTER_HIT_TRIGGER);
        for (Unit::AuraList::const_iterator itr = hitTriggerAuras.begin(); itr != hitTriggerAuras.end(); ++itr)
        {
            if (Unit* hitTarget = (*itr)->GetCaster())
            {
                if ((*itr)->m_procCharges > 0)
                {
                    (*itr)->SetAuraProcCharges((*itr)->m_procCharges - 1);
                    target = hitTarget;
                    m_targets.setUnitTarget(target);
                    AddUnitTarget(target, 0);
                    //uint64 targetGUID = target->GetGUID();
                    return;
                }
            }
        }
    }
}

bool Spell::HaveTargetsForEffect(uint8 effect) const
{
    for (std::list<TargetInfo>::const_iterator itr = m_UniqueTargetInfo.begin(); itr != m_UniqueTargetInfo.end(); ++itr)
    {
        if (itr->deleted)
            continue;

        if (itr->effectMask & (1 << effect))
            return true;
    }

    for (std::list<GOTargetInfo>::const_iterator itr = m_UniqueGOTargetInfo.begin(); itr != m_UniqueGOTargetInfo.end(); ++itr)
    {
        if (itr->deleted)
            continue;

        if (itr->effectMask & (1 << effect))
            return true;
    }

    for (std::list<ItemTargetInfo>::const_iterator itr = m_UniqueItemInfo.begin(); itr != m_UniqueItemInfo.end(); ++itr)
        if (itr->effectMask & (1 << effect))
            return true;

    return false;
}

SpellEvent::SpellEvent(Spell* spell) : BasicEvent()
{
    m_Spell = spell;
}

SpellEvent::~SpellEvent()
{
    if (m_Spell->getState() != SPELL_STATE_FINISHED)
        m_Spell->cancel();

    if (m_Spell->IsDeletable())
        delete m_Spell;
    else
    {
        sLog.outError("~SpellEvent: %s %u tried to delete non-deletable spell %u. Was not deleted, causes memory leak.",
                      (m_Spell->GetCaster()->GetTypeId() == TYPEID_PLAYER ? "Player" : "Creature"), m_Spell->GetCaster()->GetGUIDLow(), m_Spell->m_spellInfo->Id);
    }
}

bool SpellEvent::Execute(uint64 e_time, uint32 p_time)
{
    // update spell if it is not finished
    if (m_Spell->getState() != SPELL_STATE_FINISHED)
        m_Spell->update(p_time);

    // check spell state to process
    switch (m_Spell->getState())
    {
    case SPELL_STATE_FINISHED:
        {
            // spell was finished, check deletable state
            if (m_Spell->IsDeletable())
            {
                // check, if we do have unfinished triggered spells

                return (true);                              // spell is deletable, finish event
            }
            // event will be re-added automatically at the end of routine)
        }
        break;

    case SPELL_STATE_DELAYED:
        {
            // first, check, if we have just started
            if (m_Spell->GetDelayStart() != 0)
            {
                // no, we aren't, do the typical update
                // check, if we have channeled spell on our hands
                if (IsChanneledSpell(m_Spell->m_spellInfo))
                {
                    // evented channeled spell is processed separately, casted once after delay, and not destroyed till finish
                    // check, if we have casting anything else except this channeled spell and autorepeat
                    if (m_Spell->GetCaster()->IsNonMeleeSpellCast(false, true, true))
                    {
                        // another non-melee non-delayed spell is casted now, abort
                        m_Spell->cancel();
                    }
                    // Check if target of channeled spell still in range
                    else if (m_Spell->CheckRange(false))
                        m_Spell->cancel();
                    else
                    {
                        // do the action (pass spell to channeling state)
                        m_Spell->handle_immediate();
                    }
                    // event will be re-added automatically at the end of routine)
                }
                else
                {
                    // run the spell handler and think about what we can do next
                    uint64 t_offset = e_time - m_Spell->GetDelayStart();
                    uint64 n_offset = m_Spell->handle_delayed(t_offset);
                    if (n_offset)
                    {
                        // re-add us to the queue
                        m_Spell->GetCaster()->m_Events.AddEvent(this, m_Spell->GetDelayStart() + n_offset, false);
                        return (false);                     // event not complete
                    }
                    // event complete
                    // finish update event will be re-added automatically at the end of routine)
                }
            }
            else
            {
                // delaying had just started, record the moment
                m_Spell->SetDelayStart(e_time);
                // re-plan the event for the delay moment
                m_Spell->GetCaster()->m_Events.AddEvent(this, e_time + m_Spell->GetDelayMoment(), false);
                return (false);                             // event not complete
            }
        }
        break;

    default:
        {
            // all other states
            // event will be re-added automatically at the end of routine)
        } break;
    }

    // spell processing not complete, plan event on the next update interval
    m_Spell->GetCaster()->m_Events.AddEvent(this, e_time + 1, false);
    return (false);                                         // event not complete
}

void SpellEvent::Abort(uint64 /*e_time*/)
{
    // oops, the spell we try to do is aborted
    if (m_Spell->getState() != SPELL_STATE_FINISHED)
        m_Spell->cancel();
}

bool SpellEvent::IsDeletable() const
{
    return m_Spell->IsDeletable();
}

bool Spell::IsValidSingleTargetEffect(Unit const* target, Targets type) const
{
    switch (type)
    {
    case TARGET_UNIT_TARGET_ENEMY:
        return !m_caster->IsFriendlyTo(target);
    case TARGET_UNIT_TARGET_ALLY:
    case TARGET_UNIT_PARTY_TARGET:
        return m_caster->IsFriendlyTo(target);
    case TARGET_UNIT_TARGET_PARTY:
        return m_caster != target && m_caster->IsInPartyWith(target);
    case TARGET_UNIT_TARGET_RAID:
        return m_caster->IsInRaidWith(target);
    default:
        break;
    }
    return true;
}

bool Spell::IsValidSingleTargetSpell(Unit const* target) const
{
    for (int i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        if (!IsValidSingleTargetEffect(target, Targets(m_spellInfo->EffectImplicitTargetA[i])))
            return false;
        // Need to check B?
        //if (!IsValidSingleTargetEffect(m_spellInfo->EffectImplicitTargetB[i], target)
        //    return false;
    }
    return true;
}

void Spell::CalculateDamageDoneForAllTargets()
{
    float multiplier[3];
    for (int i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        if (m_applyMultiplierMask & (1 << i))
        {
            // Get multiplier
            multiplier[i] = m_spellInfo->DmgMultiplier[i];
            // Apply multiplier mods
            if (m_originalCaster)
                if (Player* modOwner = m_originalCaster->GetSpellModOwner())
                    modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_EFFECT_PAST_FIRST, multiplier[i], this);
        }
    }

    for (std::list<TargetInfo>::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
    {
        if (ihit->deleted)
            continue;

        TargetInfo& target = *ihit;

        uint32 mask = target.effectMask;
        if (!mask)
            continue;

        Unit* unit = m_caster->GetGUID() == target.targetGUID ? m_caster : ObjectAccessor::GetUnit(*m_caster, target.targetGUID);
        if (!unit)
            continue;

        if (target.missCondition == SPELL_MISS_NONE)                          // In case spell hit target, do all effect on that target
        {
            target.damage += CalculateDamageDone(unit, mask, multiplier);
            target.crit = m_caster->isSpellCrit(unit, m_spellInfo, m_spellSchoolMask, m_attackType);
        }
        else if (target.missCondition == SPELL_MISS_REFLECT)                // In case spell reflect from target, do all effect on caster (if hit)
        {
            if (target.reflectResult == SPELL_MISS_NONE)       // If reflected spell hit caster -> do all effect on him
            {
                target.damage += CalculateDamageDone(m_caster, mask, multiplier);
                target.crit = m_caster->isSpellCrit(m_caster, m_spellInfo, m_spellSchoolMask, m_attackType);
            }
        }
    }
}

int32 Spell::CalculateDamageDone(Unit* unit, const uint32 effectMask, float* multiplier)
{
    int32 damageDone = 0;
    unitTarget = unit;
    for (uint32 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        if (effectMask & (1 << i))
        {
            m_damage = 0;
            damage = CalculateDamage(i, NULL);

            switch (m_spellInfo->Effect[i])
            {
            case SPELL_EFFECT_SCHOOL_DAMAGE:
                SpellDamageSchoolDmg((SpellEffIndex)i);
                break;
            case SPELL_EFFECT_WEAPON_DAMAGE:
            case SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL:
            case SPELL_EFFECT_NORMALIZED_WEAPON_DMG:
            case SPELL_EFFECT_WEAPON_PERCENT_DAMAGE:
                SpellDamageWeaponDmg((SpellEffIndex)i);
                break;
            case SPELL_EFFECT_HEAL:
                SpellDamageHeal((SpellEffIndex)i);
                break;
            }

            if (m_damage > 0)
            {
                if (IsAreaEffectTarget[m_spellInfo->EffectImplicitTargetA[i]] || IsAreaEffectTarget[m_spellInfo->EffectImplicitTargetB[i]])
                {
                    if (int32 reducedPct = unit->GetMaxNegativeAuraModifier(SPELL_AURA_MOD_AOE_DAMAGE_AVOIDANCE))
                        m_damage = m_damage * (100 + reducedPct) / 100;
                }
            }
            if (m_applyMultiplierMask & (1 << i))
            {
                m_damage *= m_damageMultipliers[i];
                m_damageMultipliers[i] *= multiplier[i];
            }

            damageDone += m_damage;
        }
    }

    return damageDone;
}

SpellCastResult Spell::CanOpenLock(uint32 effIndex, uint32 lockId, SkillType& skillId, int32& reqSkillValue, int32& skillValue)
{
    if (!lockId)                                             // possible case for GO and maybe for items.
        return SPELL_CAST_OK;

    // Get LockInfo
    LockEntry const* lockInfo = sLockStore.LookupEntry(lockId);

    if (!lockInfo)
        return SPELL_FAILED_BAD_TARGETS;

    bool reqKey = false;                                    // some locks not have reqs

    for (int j = 0; j < 5; ++j)
    {
        switch (lockInfo->keytype[j])
        {
            // check key item (many fit cases can be)
            case LOCK_KEY_ITEM:
                if (lockInfo->key[j] && m_CastItem && m_CastItem->GetEntry() == lockInfo->key[j])
                    return SPELL_CAST_OK;
                reqKey = true;
                break;
                // check key skill (only single first fit case can be)
            case LOCK_KEY_SKILL:
            {
                reqKey = true;

                // wrong locktype, skip
                if (uint32(m_spellInfo->EffectMiscValue[effIndex]) != lockInfo->key[j])
                    continue;

                skillId = SkillByLockType(LockType(lockInfo->key[j]));

                if (skillId != SKILL_NONE)
                {
                    reqSkillValue = lockInfo->requiredlockskill;

                    // castitem check: rogue using skeleton keys. the skill values should not be added in this case.
                    skillValue = m_CastItem || m_caster->GetTypeId() != TYPEID_PLAYER ?
                        0 : m_caster->ToPlayer()->GetSkillValue(skillId);

                    if (skillValue < reqSkillValue)
                        return SPELL_FAILED_LOW_CASTLEVEL;
                }

                return SPELL_CAST_OK;
            }
        }
    }

    if (reqKey)
        return SPELL_FAILED_BAD_TARGETS;

    return SPELL_CAST_OK;
}

void Spell::SetSpellValue(SpellValueMod mod, int32 value)
{
    switch (mod)
    {
    case SPELLVALUE_BASE_POINT0:
        m_spellValue->EffectBasePoints[0] = value - int32(m_spellInfo->EffectBaseDice[0]);
        m_currentBasePoints[0] = m_spellValue->EffectBasePoints[0]; //this should be removed in the future
        break;
    case SPELLVALUE_BASE_POINT1:
        m_spellValue->EffectBasePoints[1] = value - int32(m_spellInfo->EffectBaseDice[1]);
        m_currentBasePoints[1] = m_spellValue->EffectBasePoints[1];
        break;
    case SPELLVALUE_BASE_POINT2:
        m_spellValue->EffectBasePoints[2] = value - int32(m_spellInfo->EffectBaseDice[2]);
        m_currentBasePoints[2] = m_spellValue->EffectBasePoints[2];
        break;
    case SPELLVALUE_MAX_TARGETS:
        m_spellValue->MaxAffectedTargets = (uint32)value;
        break;
    case SPELLVALUE_DURATION:
        m_spellValue->Duration = (uint32) value;
        m_spellValue->CustomDuration = true;
        break;
    }
}
