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
#include "World.h"
#include "ObjectMgr.h"
#include "SpellMgr.h"
#include "Creature.h"
#include "QuestDef.h"
#include "GossipDef.h"
#include "Player.h"
#include "PoolMgr.h"
#include "Opcodes.h"
#include "Log.h"
#include "LootMgr.h"
#include "MapManager.h"
#include "CreatureAI.h"
#include "CreatureAISelector.h"
#include "Formulas.h"
#include "SpellAuras.h"
#include "WaypointMovementGenerator.h"
#include "InstanceData.h"
#include "BattleGround.h"
#include "Util.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"
#include "OutdoorPvPMgr.h"
#include "GameEventMgr.h"
#include "CreatureGroups.h"
#include "MoveSpline.h"

void TrainerSpellData::Clear()
{
    for (TrainerSpellList::iterator itr = spellList.begin(); itr != spellList.end(); ++itr)
        delete (*itr);
    spellList.clear();
}

TrainerSpell const* TrainerSpellData::Find(uint32 spell_id) const
{
    for (TrainerSpellList::const_iterator itr = spellList.begin(); itr != spellList.end(); ++itr)
        if ((*itr)->spell == spell_id)
            return *itr;

    return NULL;
}

bool VendorItemData::RemoveItem(uint32 item_id)
{
    bool found = false;
    for (VendorItemList::iterator i = m_items.begin(); i != m_items.end();)
    {
        if ((*i)->item == item_id)
        {
            i = m_items.erase(i++);
            found = true;
        }
        else
            ++i;
    }
    return found;
}

size_t VendorItemData::FindItemSlot(uint32 item_id) const
{
    for (size_t i = 0; i < m_items.size(); ++i)
        if (m_items[i]->item == item_id)
            return i;
    return m_items.size();
}

VendorItem const* VendorItemData::FindItem(uint32 item_id) const
{
    for (VendorItemList::const_iterator i = m_items.begin(); i != m_items.end(); ++i)
        if ((*i)->item == item_id)
            return *i;
    return NULL;
}

uint32 CreatureInfo::GetRandomValidModelId() const
{
    uint32 c = 0;
    uint32 modelIDs[4];

    if (Modelid_A1) modelIDs[c++] = Modelid_A1;
    if (Modelid_A2) modelIDs[c++] = Modelid_A2;
    if (Modelid_H1) modelIDs[c++] = Modelid_H1;
    if (Modelid_H2) modelIDs[c++] = Modelid_H2;

    return ((c > 0) ? modelIDs[urand(0, c - 1)] : 0);
}

uint32 CreatureInfo::GetFirstValidModelId() const
{
    if (Modelid_A1) return Modelid_A1;
    if (Modelid_A2) return Modelid_A2;
    if (Modelid_H1) return Modelid_H1;
    if (Modelid_H2) return Modelid_H2;
    return 0;
}

bool AssistDelayEvent::Execute(uint64 /*e_time*/, uint32 /*p_time*/)
{
    if (Unit* victim = Unit::GetUnit(m_owner, m_victim))
    {
        while (!m_assistants.empty())
        {
            Creature* assistant = Unit::GetCreature(m_owner, *m_assistants.begin());
            m_assistants.pop_front();

            if (assistant && assistant->CanAssistTo(&m_owner, victim))
            {
                assistant->SetNoCallAssistance(true);
                assistant->CombatStart(victim);
                if (assistant->IsAIEnabled)
                    assistant->AI()->AttackStart(victim);
            }
        }
    }
    return true;
}

bool ForcedDespawnDelayEvent::Execute(uint64 /*e_time*/, uint32 /*p_time*/)
{
    m_owner.ForcedDespawn();
    return true;
}

Creature::Creature() :
    Unit(),
    _pickpocketLootRestore(0),
    _skinner(0),
    m_GlobalCooldown(0),
    m_PlayerDamageReq(0), m_SightDistance(sWorld.getConfig(CONFIG_SIGHT_MONSTER)),
    m_CombatDistance(MELEE_RANGE), m_lootMoney(0), m_lootRecipient(0), m_lootRecipientGroup(0), m_corpseRemoveTime(0), m_respawnTime(0), m_respawnDelay(25),
    m_corpseDelay(60),
    m_respawnradius(0.0f), m_emoteState(0), m_reactState(REACT_AGGRESSIVE), m_regenTimer(2000),
    m_defaultMovementType(IDLE_MOTION_TYPE), m_DBTableGuid(0), m_equipmentId(0),
    m_AlreadyCallAssistance(false), m_AlreadySearchedAssistance(false), m_regenHealth(true), m_AI_locked(false),
    m_meleeDamageSchoolMask(SPELL_SCHOOL_MASK_NORMAL), DisableReputationGain(false), m_creatureData(NULL),
    m_formation(NULL), m_creatureInfo(NULL)
{
    m_valuesCount = UNIT_END;

    for (uint8 i = 0; i < CREATURE_MAX_SPELLS; ++i)
        m_spells[i] = 0;

    m_CreatureSpellCooldowns.clear();
    m_CreatureCategoryCooldowns.clear();
    
    m_SightDistance = sWorld.getConfig(CONFIG_SIGHT_MONSTER);
    m_CombatDistance = 0; // MELEE_RANGE
}

Creature::~Creature()
{
    m_vendorItemCounts.clear();

    delete i_AI;
    i_AI = NULL;
}

void Creature::AddToWorld()
{
    // Register the creature for guid lookup
    if (!IsInWorld())
    {
        if (m_zoneScript)
            m_zoneScript->OnCreatureCreate(this, true);
        ObjectAccessor::Instance().AddObject(this);
        Unit::AddToWorld();
        SearchFormation();
        AIM_Initialize();
    }
}

void Creature::RemoveFromWorld()
{
    if (IsInWorld())
    {
        if (m_zoneScript)
            m_zoneScript->OnCreatureCreate(this, false);
        if (m_formation)
            sFormationMgr.RemoveCreatureFromGroup(m_formation, this);
        Unit::RemoveFromWorld();
        ObjectAccessor::Instance().RemoveObject(this);
    }
}

void Creature::DisappearAndDie()
{
    DestroyForNearbyPlayers();
    if (IsAlive())
        setDeathState(JUST_DIED);
    RemoveCorpse(false);
}

void Creature::SearchFormation()
{
    if (IsPet())
        return;

    uint32 lowguid = GetDBTableGUIDLow();
    if (!lowguid)
        return;

    CreatureGroupInfoType::iterator frmdata = sFormationMgr.CreatureGroupMap.find(lowguid);
    if (frmdata != sFormationMgr.CreatureGroupMap.end())
        sFormationMgr.AddCreatureToGroup(frmdata->second->leaderGUID, this);
}

void Creature::RemoveCorpse(bool setSpawnTime)
{
    if (getDeathState() != CORPSE)
        return;

    m_corpseRemoveTime = time(NULL);
    setDeathState(DEAD);
    UpdateObjectVisibility();
    loot.clear();

    // Unit will forget everyone who has ever attacked it
    if (Unit* creature_unit = Unit::GetUnit(*this, GetGUID()))
        creature_unit->clearPastEnemyList();

    // Should get removed later, just keep "compatibility" with scripts
    if (setSpawnTime)
        m_respawnTime = time(NULL) + m_respawnDelay;

    float x, y, z, o;
    GetRespawnCoord(x, y, z, &o);
    SetHomePosition(x, y, z, o);
    GetMap()->CreatureRelocation(this, x, y, z, o);
}

/**
 * change the entry of creature until respawn
 */
bool Creature::InitEntry(uint32 Entry, uint32 team, const CreatureData* data)
{
    CreatureInfo const* normalInfo = sObjectMgr.GetCreatureTemplate(Entry);
    if (!normalInfo)
    {
        sLog.outErrorDb("Creature::UpdateEntry creature entry %u does not exist.", Entry);
        return false;
    }

    // get heroic mode entry
    uint32 actualEntry = Entry;
    CreatureInfo const* cinfo = normalInfo;
    if (normalInfo->HeroicEntry)
    {
        //we already have valid Map pointer for current creature!
        if (GetMap()->IsHeroic())
        {
            cinfo = sObjectMgr.GetCreatureTemplate(normalInfo->HeroicEntry);
            if (!cinfo)
            {
                sLog.outErrorDb("Creature::UpdateEntry creature heroic entry %u does not exist.", actualEntry);
                return false;
            }
        }
    }

    SetEntry(Entry);                                        // normal entry always
    m_creatureInfo = cinfo;                                 // map mode related always

    // Cancel load if no model defined
    if (!(cinfo->GetFirstValidModelId()))
    {
        sLog.outErrorDb("Creature (Entry: %u) has no model defined in table creature_template, can't load. ", Entry);
        return false;
    }

    uint32 display_id = sObjectMgr.ChooseDisplayId(team, GetCreatureTemplate(), data);
    CreatureModelInfo const* minfo = sObjectMgr.GetCreatureModelRandomGender(display_id);
    if (!minfo)                                             // Cancel load if no model defined
    {
        sLog.outErrorDb("Creature (Entry: %u) has model %u not found in table creature_model_info, can't load. ", Entry, display_id);
        return false;
    }

    display_id = minfo->modelid;                            // it can be different (for another gender)

    SetDisplayId(display_id);
    SetNativeDisplayId(display_id);
    SetByteValue(UNIT_FIELD_BYTES_0, 2, minfo->gender);

    // Load creature equipment
    if (!data || data->equipmentId == 0)
    {
        // use default from the template
        LoadEquipment(cinfo->equipmentId);
    }
    else if (data && data->equipmentId != -1)
    {
        // override, -1 means no equipment
        LoadEquipment(data->equipmentId);
    }

    SetName(normalInfo->Name);                              // at normal entry always

    SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, minfo->bounding_radius);
    SetFloatValue(UNIT_FIELD_COMBATREACH, minfo->combat_reach);

    SetFloatValue(UNIT_MOD_CAST_SPEED, 1.0f);

    SetSpeed(MOVE_WALK,     cinfo->speed);
    SetSpeed(MOVE_RUN,      cinfo->speed);
    SetSpeed(MOVE_SWIM,     cinfo->speed);

    SetObjectScale(cinfo->scale);

    // checked at loading
    m_defaultMovementType = MovementGeneratorType(cinfo->MovementType);
    if (!m_respawnradius && m_defaultMovementType == RANDOM_MOTION_TYPE)
        m_defaultMovementType = IDLE_MOTION_TYPE;

    for (int i = 0; i < CREATURE_MAX_SPELLS; ++i)
        m_spells[i] = GetCreatureTemplate()->spells[i];

    return true;
}

bool Creature::UpdateEntry(uint32 Entry, uint32 team, const CreatureData* data)
{
    if (!InitEntry(Entry, team, data))
        return false;

    CreatureInfo const* cInfo = GetCreatureTemplate();

    m_regenHealth = cInfo->RegenHealth;

    // creatures always have melee weapon ready if any unless specified otherwise
    if (!GetCreatureAddon())
        SetSheath(SHEATH_STATE_MELEE);

    SetByteValue(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_AURAS);

    SelectLevel(cInfo);
    if (team == HORDE)
        SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, cInfo->faction_H);
    else
        SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, cInfo->faction_A);

    if (cInfo->flags_extra & CREATURE_FLAG_EXTRA_WORLDEVENT)
        SetUInt32Value(UNIT_NPC_FLAGS, cInfo->npcflag | sGameEventMgr.GetNPCFlag(this));
    else
        SetUInt32Value(UNIT_NPC_FLAGS, cInfo->npcflag);

    SetAttackTime(BASE_ATTACK,  cInfo->baseattacktime);
    SetAttackTime(OFF_ATTACK,   cInfo->baseattacktime);
    SetAttackTime(RANGED_ATTACK, cInfo->rangeattacktime);

    SetUInt32Value(UNIT_FIELD_FLAGS, cInfo->unit_flags);
    SetUInt32Value(UNIT_DYNAMIC_FLAGS, cInfo->dynamicflags);

    RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);

    SetMeleeDamageSchool(SpellSchools(cInfo->dmgschool));
    SetModifierValue(UNIT_MOD_ARMOR, BASE_VALUE, float(cInfo->armor));

    if (cInfo->resistance1 < 0)
    {
        ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_HOLY, true);
        SetModifierValue(UNIT_MOD_RESISTANCE_HOLY, BASE_VALUE, 0);
    }
    else
        SetModifierValue(UNIT_MOD_RESISTANCE_HOLY, BASE_VALUE, float(cInfo->resistance1));

    if (cInfo->resistance2 < 0)
    {
        ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FIRE, true);
        SetModifierValue(UNIT_MOD_RESISTANCE_FIRE, BASE_VALUE, 0);
    }
    else
        SetModifierValue(UNIT_MOD_RESISTANCE_FIRE, BASE_VALUE, float(cInfo->resistance2));

    if (cInfo->resistance3 < 0)
    {
        ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_NATURE, true);
        SetModifierValue(UNIT_MOD_RESISTANCE_NATURE, BASE_VALUE, 0);
    }
    else
        SetModifierValue(UNIT_MOD_RESISTANCE_NATURE, BASE_VALUE, float(cInfo->resistance3));

    if (cInfo->resistance4 < 0)
    {
        ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FROST, true);
        SetModifierValue(UNIT_MOD_RESISTANCE_FROST, BASE_VALUE, 0);
    }
    else
        SetModifierValue(UNIT_MOD_RESISTANCE_FROST, BASE_VALUE, float(cInfo->resistance4));

    if (cInfo->resistance5 < 0)
    {
        ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_SHADOW, true);
        SetModifierValue(UNIT_MOD_RESISTANCE_SHADOW, BASE_VALUE, 0);
    }
    else
        SetModifierValue(UNIT_MOD_RESISTANCE_SHADOW, BASE_VALUE, float(cInfo->resistance5));

    if (cInfo->resistance6 < 0)
    {
        ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_ARCANE, true);
        SetModifierValue(UNIT_MOD_RESISTANCE_ARCANE, BASE_VALUE, 0);
    }
    else
        SetModifierValue(UNIT_MOD_RESISTANCE_ARCANE, BASE_VALUE, float(cInfo->resistance6));


    SetCanModifyStats(true);
    UpdateAllStats();

    // checked and error show at loading templates
    if (FactionTemplateEntry const* factionTemplate = sFactionTemplateStore.LookupEntry(cInfo->faction_A))
    {
        FactionEntry const* factionEntry = sFactionStore.LookupEntry(factionTemplate->faction);
        if (factionEntry)
            if (!(cInfo->flags_extra & CREATURE_FLAG_EXTRA_CIVILIAN) &&
                (factionEntry->team == ALLIANCE || factionEntry->team == HORDE))
                SetPvP(true);
    }

    // trigger creature is always not selectable and can not be attacked
    if (isTrigger())
        SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

    if (IsTotem() || isTrigger()
        || GetCreatureType() == CREATURE_TYPE_CRITTER)
        SetReactState(REACT_PASSIVE);
    /*else if (isCivilian())
        SetReactState(REACT_DEFENSIVE);*/
    else
        SetReactState(REACT_AGGRESSIVE);

    if (cInfo->flags_extra & CREATURE_FLAG_EXTRA_NO_TAUNT)
    {
        ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
        ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
    }

    UpdateMovementFlags();
    return true;
}

void Creature::UpdateMovementFlags(bool packetOnly)
{
    if (IsControlledByPlayer())
        return;

    CreatureInfo const* cInfo = GetCreatureTemplate();

    // Set the movement flags if the creature is in that mode. (Only fly if actually in air, only swim if in water, etc)
    float ground = GetMap()->GetHeight(GetPositionX(), GetPositionY(), GetPositionZ(), true, MAX_FALL_DISTANCE);
    if (ground < INVALID_HEIGHT)
    {
        sLog.outDebug("FallGround: creature %u at map %u (x: %f, y: %f, z: %f), not able to retrive a proper GetHeight (z: %f).",
            GetEntry(), GetMap()->GetId(), GetPositionX(), GetPositionX(), GetPositionZ(), ground);
    }

    bool isInAir = (G3D::fuzzyGt(GetPositionZ(), ground + 0.05f) || G3D::fuzzyLt(GetPositionZ(), ground - 0.05f)); // Can be underground too, prevent the falling

    if (GetCreatureTemplate()->InhabitType & INHABIT_AIR && isInAir && !IsFalling())
    {
        if (GetCreatureTemplate()->InhabitType & INHABIT_GROUND)
            SetCanFly(true, packetOnly);
        else
            SetLevitate(true, packetOnly);
    }
    else
    {
        SetCanFly(false);
        SetLevitate(false);
    }

    if (!isInAir)
        RemoveUnitMovementFlag(MOVEFLAG_FALLING);

    if (cInfo->InhabitType & INHABIT_WATER && IsInWater())
        SetSwim(true, packetOnly);
    else
        SetSwim(false);
}

void Creature::Update(uint32 diff)
{
    if (m_GlobalCooldown <= diff)
        m_GlobalCooldown = 0;
    else
        m_GlobalCooldown -= diff;

    UpdateMovementFlags();

    switch (m_deathState)
    {
    case JUST_RESPAWNED:
        // Must not be called, see Creature::setDeathState JUST_ALIVED -> ALIVE promoting.
        sLog.outError("Creature (GUIDLow: %u Entry: %u) in wrong state: JUST_ALIVED (4)", GetGUIDLow(), GetEntry());
        break;
    case JUST_DIED:
        // Must not be called, see Creature::setDeathState JUST_DIED -> CORPSE promoting.
        sLog.outError("Creature (GUIDLow: %u Entry: %u) in wrong state: JUST_DEAD (1)", GetGUIDLow(), GetEntry());
        break;
    case DEAD:
        {
            if (m_respawnTime <= time(NULL))
            {
                if (!GetLinkedCreatureRespawnTime()) // Can respawn
                    Respawn();
                else // the master is dead
                {
                    if (uint32 targetGuid = sObjectMgr.GetLinkedRespawnGuid(m_DBTableGuid))
                    {
                        if (targetGuid == m_DBTableGuid) // if linking self, never respawn (check delayed to next day)
                            SetRespawnTime(DAY);
                        else
                            m_respawnTime = (time(NULL) > GetLinkedCreatureRespawnTime() ? time(NULL) : GetLinkedCreatureRespawnTime()) + urand(5, MINUTE); // else copy time from master and add a little
                        SaveRespawnTime(); // also save to DB immediately
                    }
                    else
                        Respawn();
                }
            }
            break;
        }
    case CORPSE:
        {
            Unit::Update(diff);
            // deathstate changed on spells update, prevent problems
            if (m_deathState != CORPSE)
                break;

            if (m_corpseRemoveTime <= uint32(time(NULL)))
            {
                RemoveCorpse(false);
                DEBUG_LOG("Removing corpse... %u ", GetUInt32Value(OBJECT_FIELD_ENTRY));
            }
            else
            {
                if (m_groupLootTimer && lootingGroupLeaderGUID)
                {
                    if (diff <= m_groupLootTimer)
                        m_groupLootTimer -= diff;
                    else
                    {
                        Group* group = sObjectMgr.GetGroupByLeader(lootingGroupLeaderGUID);
                        if (group)
                            group->EndRoll();
                        m_groupLootTimer = 0;
                        lootingGroupLeaderGUID = 0;
                    }
                }
            }

            break;
        }
    case ALIVE:
        {
            Unit::Update(diff);

            // creature can be dead after Unit::Update call
            // CORPSE/DEAD state will processed at next tick (in other case death timer will be updated unexpectedly)
            if (!IsAlive())
                break;

            // if creature is charmed, switch to charmed AI
            if (NeedChangeAI)
            {
                UpdateCharmAI();
                NeedChangeAI = false;
                IsAIEnabled = true;
                if (!IsInEvadeMode() && LastCharmerGUID)
                    if (Unit* charmer = ObjectAccessor::GetUnit(*this, LastCharmerGUID))
                        i_AI->AttackStart(charmer);

                LastCharmerGUID = 0;
            }

            if (!IsInEvadeMode() && IsAIEnabled)
            {
                // do not allow the AI to be changed during update
                m_AI_locked = true;
                i_AI->UpdateAI(diff);
                m_AI_locked = false;
            }

            // creature can be dead after UpdateAI call
            // CORPSE/DEAD state will processed at next tick (in other case death timer will be updated unexpectedly)
            if (!IsAlive())
                break;

            if (m_regenTimer > 0)
            {
                if (diff >= m_regenTimer)
                    m_regenTimer = 0;
                else
                    m_regenTimer -= diff;
            }
            if (m_regenTimer != 0)
                break;

            if (!IsInEvadeMode() && (!IsInCombat() || IsPolymorphed())) // regenerate health if not in combat or if polymorphed
            {
                if (HasFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_OTHER_TAGGER))
                    SetUInt32Value(UNIT_DYNAMIC_FLAGS, GetCreatureTemplate()->dynamicflags);
                RegenerateHealth();
            }

            RegenerateMana();

            m_regenTimer = 2000;
            break;
        }
    case DEAD_FALLING:
        setDeathState(CORPSE);
        break;
    default:
        break;
    }
}

void Creature::RegenerateMana()
{
    uint32 curValue = GetPower(POWER_MANA);
    uint32 maxValue = GetMaxPower(POWER_MANA);

    if (curValue >= maxValue)
        return;

    uint32 addvalue = 0;

    // Combat and any controlled creature
    if (IsInCombat() || GetCharmerOrOwnerGUID())
    {
        if (!IsUnderLastManaUseEffect())
        {
            float ManaIncreaseRate = sWorld.getRate(RATE_POWER_MANA);
            float Spirit = GetStat(STAT_SPIRIT);

            addvalue = uint32((Spirit / 5.0f + 17.0f) * ManaIncreaseRate);
        }
    }
    else
        addvalue = maxValue / 3;

    ModifyPower(POWER_MANA, addvalue);
}

void Creature::RegenerateHealth()
{
    if (!isRegeneratingHealth())
        return;

    uint32 curValue = GetHealth();
    uint32 maxValue = GetMaxHealth();

    if (curValue >= maxValue)
        return;

    uint32 addvalue = 0;

    // Not only pet, but any controlled creature
    if (GetCharmerOrOwnerGUID())
    {
        float HealthIncreaseRate = sWorld.getRate(RATE_HEALTH);
        float Spirit = GetStat(STAT_SPIRIT);

        if (GetPower(POWER_MANA) > 0)
            addvalue = uint32(Spirit * 0.25 * HealthIncreaseRate);
        else
            addvalue = uint32(Spirit * 0.80 * HealthIncreaseRate);
    }
    else
        addvalue = maxValue / 3;

    ModifyHealth(addvalue);
}

bool Creature::AIM_Initialize(CreatureAI* ai)
{
    // make sure nothing can change the AI during AI update
    if (m_AI_locked)
    {
        sLog.outDebug("AIM_Initialize: failed to init, locked.");
        return false;
    }

    UnitAI* oldAI = i_AI;
    i_motionMaster.Initialize();
    i_AI = ai ? ai : FactorySelector::selectAI(this);
    delete oldAI;
    IsAIEnabled = true;
    i_AI->InitializeAI();
    return true;
}

bool Creature::Create(uint32 guidlow, Map* map, uint32 Entry, uint32 team, float x, float y, float z, float ang, const CreatureData* data)
{
    ASSERT(map);
    SetMap(map);

    Relocate(x, y, z, ang);

    if (!IsPositionValid())
    {
        sLog.outError("Creature (guidlow %d, entry %d) not loaded. Suggested coordinates isn't valid (X: %f Y: %f)", guidlow, Entry, x, y);
        return false;
    }

    //oX = x;     oY = y;    dX = x;    dY = y;    m_moveTime = 0;    m_startMove = 0;
    const bool bResult = CreateFromProto(guidlow, Entry, team, data);

    if (bResult)
    {
        switch (GetCreatureTemplate()->rank)
        {
        case CREATURE_ELITE_RARE:
            m_corpseDelay = sWorld.getConfig(CONFIG_CORPSE_DECAY_RARE);
            break;
        case CREATURE_ELITE_ELITE:
            m_corpseDelay = sWorld.getConfig(CONFIG_CORPSE_DECAY_ELITE);
            break;
        case CREATURE_ELITE_RAREELITE:
            m_corpseDelay = sWorld.getConfig(CONFIG_CORPSE_DECAY_RAREELITE);
            break;
        case CREATURE_ELITE_WORLDBOSS:
            m_corpseDelay = sWorld.getConfig(CONFIG_CORPSE_DECAY_WORLDBOSS);
            break;
        default:
            m_corpseDelay = sWorld.getConfig(CONFIG_CORPSE_DECAY_NORMAL);
            break;
        }
        LoadCreaturesAddon();
    }

    return bResult;
}

bool Creature::IsTrainerOf(Player* pPlayer, bool msg) const
{
    if (!isTrainer())
        return false;

    TrainerSpellData const* trainer_spells = GetTrainerSpells();

    if (!trainer_spells || trainer_spells->spellList.empty())
    {
        sLog.outErrorDb("Creature %u (Entry: %u) has UNIT_NPC_FLAG_TRAINER but trainer spell list is empty.",
                        GetGUIDLow(), GetEntry());
        return false;
    }

    switch (GetCreatureTemplate()->trainer_type)
    {
    case TRAINER_TYPE_CLASS:
        if (pPlayer->getClass() != GetCreatureTemplate()->classNum)
        {
            if (msg)
            {
                pPlayer->PlayerTalkClass->ClearMenus();
                switch (GetCreatureTemplate()->classNum)
                {
                case CLASS_DRUID:
                    pPlayer->PlayerTalkClass->SendGossipMenu(4913, GetGUID());
                    break;
                case CLASS_HUNTER:
                    pPlayer->PlayerTalkClass->SendGossipMenu(10090, GetGUID());
                    break;
                case CLASS_MAGE:
                    pPlayer->PlayerTalkClass->SendGossipMenu( 328, GetGUID());
                    break;
                case CLASS_PALADIN:
                    pPlayer->PlayerTalkClass->SendGossipMenu(1635, GetGUID());
                    break;
                case CLASS_PRIEST:
                    pPlayer->PlayerTalkClass->SendGossipMenu(4436, GetGUID());
                    break;
                case CLASS_ROGUE:
                    pPlayer->PlayerTalkClass->SendGossipMenu(4797, GetGUID());
                    break;
                case CLASS_SHAMAN:
                    pPlayer->PlayerTalkClass->SendGossipMenu(5003, GetGUID());
                    break;
                case CLASS_WARLOCK:
                    pPlayer->PlayerTalkClass->SendGossipMenu(5836, GetGUID());
                    break;
                case CLASS_WARRIOR:
                    pPlayer->PlayerTalkClass->SendGossipMenu(4985, GetGUID());
                    break;
                }
            }
            return false;
        }
        break;
    case TRAINER_TYPE_PETS:
        if (pPlayer->getClass() != CLASS_HUNTER)
        {
            pPlayer->PlayerTalkClass->ClearMenus();
            pPlayer->PlayerTalkClass->SendGossipMenu(3620, GetGUID());
            return false;
        }
        break;
    case TRAINER_TYPE_MOUNTS:
        if (GetCreatureTemplate()->race && pPlayer->getRace() != GetCreatureTemplate()->race)
        {
            // Allowed to train if exalted
            if (FactionTemplateEntry const* faction_template = getFactionTemplateEntry())
            {
                if (pPlayer->GetReputationRank(faction_template->faction) == REP_EXALTED)
                    return true;
            }

            if (msg)
            {
                pPlayer->PlayerTalkClass->ClearMenus();
                switch (GetCreatureTemplate()->classNum)
                {
                case RACE_DWARF:
                    pPlayer->PlayerTalkClass->SendGossipMenu(5865, GetGUID());
                    break;
                case RACE_GNOME:
                    pPlayer->PlayerTalkClass->SendGossipMenu(4881, GetGUID());
                    break;
                case RACE_HUMAN:
                    pPlayer->PlayerTalkClass->SendGossipMenu(5861, GetGUID());
                    break;
                case RACE_NIGHTELF:
                    pPlayer->PlayerTalkClass->SendGossipMenu(5862, GetGUID());
                    break;
                case RACE_ORC:
                    pPlayer->PlayerTalkClass->SendGossipMenu(5863, GetGUID());
                    break;
                case RACE_TAUREN:
                    pPlayer->PlayerTalkClass->SendGossipMenu(5864, GetGUID());
                    break;
                case RACE_TROLL:
                    pPlayer->PlayerTalkClass->SendGossipMenu(5816, GetGUID());
                    break;
                case RACE_UNDEAD_PLAYER:
                    pPlayer->PlayerTalkClass->SendGossipMenu(624, GetGUID());
                    break;
                case RACE_BLOODELF:
                    pPlayer->PlayerTalkClass->SendGossipMenu(5862, GetGUID());
                    break;
                case RACE_DRAENEI:
                    pPlayer->PlayerTalkClass->SendGossipMenu(5864, GetGUID());
                    break;
                }
            }
            return false;
        }
        break;
    case TRAINER_TYPE_TRADESKILLS:
        if (GetCreatureTemplate()->trainer_spell && !pPlayer->HasSpell(GetCreatureTemplate()->trainer_spell))
        {
            if (msg)
            {
                pPlayer->PlayerTalkClass->ClearMenus();
                pPlayer->PlayerTalkClass->SendGossipMenu(11031, GetGUID());
            }
            return false;
        }
        break;
    default:
        return false;                                   // checked and error output at creature_template loading
    }
    return true;
}

bool Creature::CanInteractWithBattleMaster(Player* pPlayer, bool msg) const
{
    if (!isBattleMaster())
        return false;

    uint32 bgTypeId = sObjectMgr.GetBattleMasterBG(GetEntry());
    if (!msg)
        return pPlayer->GetBGAccessByLevel(bgTypeId);

    if (!pPlayer->GetBGAccessByLevel(bgTypeId))
    {
        pPlayer->PlayerTalkClass->ClearMenus();
        switch (bgTypeId)
        {
        case BATTLEGROUND_AV:
            pPlayer->PlayerTalkClass->SendGossipMenu(7616, GetGUID());
            break;
        case BATTLEGROUND_WS:
            pPlayer->PlayerTalkClass->SendGossipMenu(7599, GetGUID());
            break;
        case BATTLEGROUND_AB:
            pPlayer->PlayerTalkClass->SendGossipMenu(7642, GetGUID());
            break;
        case BATTLEGROUND_EY:
        case BATTLEGROUND_NA:
        case BATTLEGROUND_BE:
        case BATTLEGROUND_AA:
        case BATTLEGROUND_RL:
            pPlayer->PlayerTalkClass->SendGossipMenu(10024, GetGUID());
            break;
            break;
        }
        return false;
    }
    return true;
}

bool Creature::CanTrainAndResetTalentsOf(Player* pPlayer) const
{
    return pPlayer->getLevel() >= 10
           && GetCreatureTemplate()->trainer_type == TRAINER_TYPE_CLASS
           && pPlayer->getClass() == GetCreatureTemplate()->classNum;
}

Player* Creature::GetLootRecipient() const
{
    if (!m_lootRecipient)
        return NULL;
    return ObjectAccessor::FindPlayer(m_lootRecipient);
}

Group *Creature::GetLootRecipientGroup() const
{
    if (!m_lootRecipientGroup)
        return NULL;
    return sObjectMgr.GetGroupByLeader(m_lootRecipientGroup);
}

void Creature::SetLootRecipient(Unit* unit)
{
    // set the player whose group should receive the right
    // to loot the creature after it dies
    // should be set to NULL after the loot disappears

    if (!unit)
    {
        m_lootRecipient = 0;
        m_lootRecipientGroup = 0;
        RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
        RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_OTHER_TAGGER);
        return;
    }

    Player* player = unit->GetCharmerOrOwnerPlayerOrPlayerItself();
    if (!player)                                             // normal creature, no player involved
        return;

    m_lootRecipient = player->GetGUID();
    if (Group *group = player->GetGroup())
        m_lootRecipientGroup = group->GetLeaderGUID();

    SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_OTHER_TAGGER);
}

// return true if this creature is tapped by the player or by a member of his group.
bool Creature::isTappedBy(Player const* player) const
{
    if (player->GetGUID() == m_lootRecipient)
        return true;

    Group const* playerGroup = player->GetGroup();
    if (!playerGroup || playerGroup != GetLootRecipientGroup()) // if we dont have a group we arent the recipient
        return false;                                           // if creature doesnt have group bound it means it was solo killed by someone else

    return true;
}

void Creature::SaveToDB()
{
    // this should only be used when the creature has already been loaded
    // preferably after adding to map, because mapid may not be valid otherwise
    CreatureData const* data = sObjectMgr.GetCreatureData(m_DBTableGuid);
    if (!data)
    {
        sLog.outError("Creature::SaveToDB failed, cannot get creature data!");
        return;
    }

    SaveToDB(GetMapId(), data->spawnMask);
}

void Creature::SaveToDB(uint32 mapid, uint8 spawnMask)
{
    // update in loaded data
    if (!m_DBTableGuid)
        m_DBTableGuid = GetGUIDLow();
    CreatureData& data = sObjectMgr.NewOrExistCreatureData(m_DBTableGuid);

    uint32 displayId = GetNativeDisplayId();

    // check if it's a custom model and if not, use 0 for displayId
    CreatureInfo const* cinfo = GetCreatureTemplate();
    if (cinfo)
    {
        if (displayId == cinfo->Modelid_A1 || displayId == cinfo->Modelid_A2 ||
            displayId == cinfo->Modelid_H1 || displayId == cinfo->Modelid_H2) displayId = 0;
    }

    // data->guid = guid must not be update at save
    data.id = GetEntry();
    data.mapid = mapid;
    data.displayid = displayId;
    data.equipmentId = GetEquipmentId();
    data.posX = GetPositionX();
    data.posY = GetPositionY();
    data.posZ = GetPositionZ();
    data.orientation = GetOrientation();
    data.spawntimesecs = m_respawnDelay;
    // prevent add data integrity problems
    data.spawndist = GetDefaultMovementType() == IDLE_MOTION_TYPE ? 0 : m_respawnradius;
    data.currentwaypoint = 0;
    data.curhealth = GetHealth();
    data.curmana = GetPower(POWER_MANA);
    // prevent add data integrity problems
    data.movementType = !m_respawnradius && GetDefaultMovementType() == RANDOM_MOTION_TYPE
                        ? IDLE_MOTION_TYPE : GetDefaultMovementType();
    data.spawnMask = spawnMask;

    // updated in DB
    WorldDatabase.BeginTransaction();

    WorldDatabase.PExecuteLog("DELETE FROM creature WHERE guid = '%u'", m_DBTableGuid);

    std::ostringstream ss;
    ss << "INSERT INTO creature VALUES ("
       << m_DBTableGuid << ","
       << GetEntry() << ","
       << mapid << ","
       << (uint32)spawnMask << ","
       << displayId << ","
       << GetEquipmentId() << ","
       << GetPositionX() << ","
       << GetPositionY() << ","
       << GetPositionZ() << ","
       << GetOrientation() << ","
       << m_respawnDelay << ","                            //respawn time
       << (float) m_respawnradius << ","                   //spawn distance (float)
       << (uint32) (0) << ","                              //currentwaypoint
       << GetHealth() << ","                               //curhealth
       << GetPower(POWER_MANA) << ","                      //curmana
       << GetDefaultMovementType() << ")";                 //default movement generator type

    WorldDatabase.PExecuteLog("%s", ss.str().c_str());

    WorldDatabase.CommitTransaction();
}

void Creature::SelectLevel(const CreatureInfo* cinfo)
{
    uint32 rank = IsPet() ? 0 : cinfo->rank;

    // level
    uint32 minlevel = std::min(cinfo->maxlevel, cinfo->minlevel);
    uint32 maxlevel = std::max(cinfo->maxlevel, cinfo->minlevel);
    uint32 level = minlevel == maxlevel ? minlevel : urand(minlevel, maxlevel);
    SetLevel(level);

    float rellevel = maxlevel == minlevel ? 0 : (float(level - minlevel)) / (maxlevel - minlevel);

    // health
    float healthmod = _GetHealthMod(rank);

    uint32 minhealth = std::min(cinfo->maxhealth, cinfo->minhealth);
    uint32 maxhealth = std::max(cinfo->maxhealth, cinfo->minhealth);
    uint32 health = uint32(healthmod * (minhealth + uint32(rellevel * (maxhealth - minhealth))));

    SetCreateHealth(health);
    SetMaxHealth(health);
    SetHealth(health);
    ResetPlayerDamageReq();

    // mana
    uint32 minmana = std::min(cinfo->maxmana, cinfo->minmana);
    uint32 maxmana = std::max(cinfo->maxmana, cinfo->minmana);
    uint32 mana = minmana + uint32(rellevel * (maxmana - minmana));

    SetCreateMana(mana);
    SetMaxPower(POWER_MANA, mana);                          //MAX Mana
    SetPower(POWER_MANA, mana);

    SetModifierValue(UNIT_MOD_HEALTH, BASE_VALUE, health);
    SetModifierValue(UNIT_MOD_MANA, BASE_VALUE, mana);

    // damage
    float damagemod = _GetDamageMod(rank);

    SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, cinfo->mindmg * damagemod);
    SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, cinfo->maxdmg * damagemod);
    SetBaseWeaponDamage(OFF_ATTACK, MINDAMAGE, cinfo->mindmg * damagemod);
    SetBaseWeaponDamage(OFF_ATTACK, MAXDAMAGE, cinfo->maxdmg * damagemod);
    SetBaseWeaponDamage(RANGED_ATTACK, MINDAMAGE, cinfo->minrangedmg * damagemod);
    SetBaseWeaponDamage(RANGED_ATTACK, MAXDAMAGE, cinfo->maxrangedmg * damagemod);

    // this value is not accurate, but should be close to the real value
    SetModifierValue(UNIT_MOD_ATTACK_POWER, BASE_VALUE, level * 5);
    SetModifierValue(UNIT_MOD_ATTACK_POWER_RANGED, BASE_VALUE, level * 5);
    //SetModifierValue(UNIT_MOD_ATTACK_POWER, BASE_VALUE, cinfo->attackpower * damagemod);
    //SetModifierValue(UNIT_MOD_ATTACK_POWER_RANGED, BASE_VALUE, cinfo->rangedattackpower * damagemod);
    UpdateAllStats();
}

float Creature::_GetHealthMod(int32 Rank)
{
    switch (Rank)                                           // define rates for each elite rank
    {
    case CREATURE_ELITE_NORMAL:
        return sWorld.getRate(RATE_CREATURE_NORMAL_HP);
    case CREATURE_ELITE_ELITE:
        return sWorld.getRate(RATE_CREATURE_ELITE_ELITE_HP);
    case CREATURE_ELITE_RAREELITE:
        return sWorld.getRate(RATE_CREATURE_ELITE_RAREELITE_HP);
    case CREATURE_ELITE_WORLDBOSS:
        return sWorld.getRate(RATE_CREATURE_ELITE_WORLDBOSS_HP);
    case CREATURE_ELITE_RARE:
        return sWorld.getRate(RATE_CREATURE_ELITE_RARE_HP);
    default:
        return sWorld.getRate(RATE_CREATURE_ELITE_ELITE_HP);
    }
}

float Creature::_GetDamageMod(int32 Rank)
{
    switch (Rank)                                           // define rates for each elite rank
    {
    case CREATURE_ELITE_NORMAL:
        return sWorld.getRate(RATE_CREATURE_NORMAL_DAMAGE);
    case CREATURE_ELITE_ELITE:
        return sWorld.getRate(RATE_CREATURE_ELITE_ELITE_DAMAGE);
    case CREATURE_ELITE_RAREELITE:
        return sWorld.getRate(RATE_CREATURE_ELITE_RAREELITE_DAMAGE);
    case CREATURE_ELITE_WORLDBOSS:
        return sWorld.getRate(RATE_CREATURE_ELITE_WORLDBOSS_DAMAGE);
    case CREATURE_ELITE_RARE:
        return sWorld.getRate(RATE_CREATURE_ELITE_RARE_DAMAGE);
    default:
        return sWorld.getRate(RATE_CREATURE_ELITE_ELITE_DAMAGE);
    }
}

float Creature::GetSpellDamageMod(int32 Rank)
{
    switch (Rank)                                           // define rates for each elite rank
    {
    case CREATURE_ELITE_NORMAL:
        return sWorld.getRate(RATE_CREATURE_NORMAL_SPELLDAMAGE);
    case CREATURE_ELITE_ELITE:
        return sWorld.getRate(RATE_CREATURE_ELITE_ELITE_SPELLDAMAGE);
    case CREATURE_ELITE_RAREELITE:
        return sWorld.getRate(RATE_CREATURE_ELITE_RAREELITE_SPELLDAMAGE);
    case CREATURE_ELITE_WORLDBOSS:
        return sWorld.getRate(RATE_CREATURE_ELITE_WORLDBOSS_SPELLDAMAGE);
    case CREATURE_ELITE_RARE:
        return sWorld.getRate(RATE_CREATURE_ELITE_RARE_SPELLDAMAGE);
    default:
        return sWorld.getRate(RATE_CREATURE_ELITE_ELITE_SPELLDAMAGE);
    }
}

bool Creature::CreateFromProto(uint32 guidlow, uint32 Entry, uint32 team, const CreatureData* data)
{
    SetZoneScript();
    if (m_zoneScript && data)
    {
        Entry = m_zoneScript->GetCreatureEntry(guidlow, data);
        if (!Entry)
            return false;
    }

    CreatureInfo const* cinfo = sObjectMgr.GetCreatureTemplate(Entry);
    if (!cinfo)
    {
        sLog.outErrorDb("Creature entry %u does not exist.", Entry);
        return false;
    }

    m_originalEntry = Entry;

    Object::_Create(guidlow, Entry, HIGHGUID_UNIT);

    if (!UpdateEntry(Entry, team, data))
        return false;

    return true;
}

bool Creature::LoadFromDB(uint32 guid, Map* map)
{
    CreatureData const* data = sObjectMgr.GetCreatureData(guid);

    if (!data)
    {
        sLog.outErrorDb("Creature (GUID: %u) not found in table creature, can't load. ", guid);
        return false;
    }

    m_DBTableGuid = guid;
    if (map->GetInstanceId() != 0)
        guid = sObjectMgr.GenerateLowGuid(HIGHGUID_UNIT);

    uint16 team = 0;
    if (!Create(guid, map, data->id, team, data->posX, data->posY, data->posZ, data->orientation, data))
        return false;

    //We should set first home position, because then AI calls home movement
    SetHomePosition(data->posX, data->posY, data->posZ, data->orientation);

    m_respawnradius = data->spawndist;

    m_respawnDelay = data->spawntimesecs;
    m_deathState = ALIVE;

    m_respawnTime  = sObjectMgr.GetCreatureRespawnTime(m_DBTableGuid, GetInstanceId());
    if (m_respawnTime)                          // respawn on Update
    {
        m_deathState = DEAD;
        if (canFly())
        {
            float tz = GetMap()->GetHeight(data->posX, data->posY, data->posZ, false);
            if (data->posZ - tz > 0.1)
                Relocate(data->posX, data->posY, tz);
        }
    }

    uint32 curhealth;
    if (!m_regenHealth)
    {
        curhealth = data->curhealth;
        if (curhealth)
        {
            curhealth = uint32(curhealth * _GetHealthMod(GetCreatureTemplate()->rank));
            if (curhealth < 1)
                curhealth = 1;
        }
        SetPower(POWER_MANA, data->curmana);
    }
    else
    {
        curhealth = GetMaxHealth();
        SetPower(POWER_MANA, GetMaxPower(POWER_MANA));
    }

    SetHealth(m_deathState == ALIVE ? curhealth : 0);

    // checked at creature_template loading
    m_defaultMovementType = MovementGeneratorType(data->movementType);

    m_creatureData = data;

    return true;
}

void Creature::LoadEquipment(uint32 equip_entry, bool force)
{
    if (equip_entry == 0)
    {
        if (force)
        {
            for (uint8 i = 0; i < 3; ++i)
            {
                SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY + i, 0);
                SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO + (i * 2), 0);
                SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO + (i * 2) + 1, 0);
            }
            m_equipmentId = 0;
        }
        return;
    }

    if (EquipmentInfo const* einfo = sObjectMgr.GetEquipmentInfo(equip_entry))
    {
        m_equipmentId = equip_entry;
        for (uint8 i = 0; i < MAX_VIRTUAL_ITEM_SLOT; ++i)
            SetVirtualItem(VirtualItemSlot(i), einfo->equipentry[i]);
    }
    else if (EquipmentInfoRaw const* einfo = sObjectMgr.GetEquipmentInfoRaw(equip_entry))
    {
        m_equipmentId = equip_entry;
        for (uint8 i = 0; i < MAX_VIRTUAL_ITEM_SLOT; ++i)
            SetVirtualItemRaw(VirtualItemSlot(i), einfo->equipmodel[i], einfo->equipinfo[i], einfo->equipslot[i]);
    }
}

void Creature::SetVirtualItem(VirtualItemSlot slot, uint32 item_id)
{
    if (item_id == 0)
    {
        SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY + slot, 0);
        SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO + (slot * 2) + 0, 0);
        SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO + (slot * 2) + 1, 0);
        return;
    }

    ItemPrototype const* proto = ObjectMgr::GetItemPrototype(item_id);
    if (!proto)
    {
        sLog.outError("Not listed in 'item_template' item (ID:%u) used as virtual item for " UI64FMTD "", item_id, GetGUID());
        return;
    }

    SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY + slot, proto->DisplayInfoID);
    SetByteValue(UNIT_VIRTUAL_ITEM_INFO + (slot * 2) + 0, VIRTUAL_ITEM_INFO_0_OFFSET_CLASS, proto->Class);
    SetByteValue(UNIT_VIRTUAL_ITEM_INFO + (slot * 2) + 0, VIRTUAL_ITEM_INFO_0_OFFSET_SUBCLASS, proto->SubClass);
    SetByteValue(UNIT_VIRTUAL_ITEM_INFO + (slot * 2) + 0, VIRTUAL_ITEM_INFO_0_OFFSET_UNK0, proto->SoundOverrideSubclass);
    SetByteValue(UNIT_VIRTUAL_ITEM_INFO + (slot * 2) + 0, VIRTUAL_ITEM_INFO_0_OFFSET_MATERIAL, proto->Material);
    SetByteValue(UNIT_VIRTUAL_ITEM_INFO + (slot * 2) + 1, VIRTUAL_ITEM_INFO_1_OFFSET_INVENTORYTYPE, proto->InventoryType);
    SetByteValue(UNIT_VIRTUAL_ITEM_INFO + (slot * 2) + 1, VIRTUAL_ITEM_INFO_1_OFFSET_SHEATH, proto->Sheath);
}

void Creature::SetVirtualItemRaw(VirtualItemSlot slot, uint32 display_id, uint32 info0, uint32 info1)
{
    SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY + slot, display_id);
    SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO + (slot * 2) + 0, info0);
    SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO + (slot * 2) + 1, info1);
}

bool Creature::hasQuest(uint32 quest_id) const
{
    QuestRelations const& qr = sObjectMgr.mCreatureQuestRelations;
    for (QuestRelations::const_iterator itr = qr.lower_bound(GetEntry()); itr != qr.upper_bound(GetEntry()); ++itr)
    {
        if (itr->second == quest_id)
            return true;
    }
    return false;
}

bool Creature::hasInvolvedQuest(uint32 quest_id) const
{
    QuestRelations const& qr = sObjectMgr.mCreatureQuestInvolvedRelations;
    for (QuestRelations::const_iterator itr = qr.lower_bound(GetEntry()); itr != qr.upper_bound(GetEntry()); ++itr)
    {
        if (itr->second == quest_id)
            return true;
    }
    return false;
}

void Creature::DeleteFromDB()
{
    if (!m_DBTableGuid)
    {
        sLog.outDebug("Trying to delete not saved creature!");
        return;
    }

    sObjectMgr.SaveCreatureRespawnTime(m_DBTableGuid, GetInstanceId(), 0);
    sObjectMgr.DeleteCreatureData(m_DBTableGuid);

    WorldDatabase.BeginTransaction();
    WorldDatabase.PExecuteLog("DELETE FROM creature WHERE guid = '%u'", m_DBTableGuid);
    WorldDatabase.PExecuteLog("DELETE FROM creature_addon WHERE guid = '%u'", m_DBTableGuid);
    WorldDatabase.PExecuteLog("DELETE FROM game_event_creature WHERE guid = '%u'", m_DBTableGuid);
    WorldDatabase.PExecuteLog("DELETE FROM game_event_model_equip WHERE guid = '%u'", m_DBTableGuid);
    WorldDatabase.CommitTransaction();
}

bool Creature::canSeeOrDetect(Unit const* u, bool detect, bool /*inVisibleList*/, bool /*is3dDistance*/) const
{
    // not in world
    if (!IsInWorld() || !u->IsInWorld())
        return false;

    // all dead creatures/players not visible for any creatures
    if (!u->IsAlive() || !IsAlive())
        return false;

    // Always can see self
    if (u == this)
        return true;

    // always seen by owner
    if (GetGUID() == u->GetCharmerOrOwnerGUID())
        return true;

    if (u->GetVisibility() == VISIBILITY_OFF) //GM
        return false;

    // invisible aura
    if ((m_invisibilityMask || u->m_invisibilityMask) && !canDetectInvisibilityOf(u))
        return false;

    // unit got in stealth in this moment and must ignore old detected state
    //if (m_Visibility == VISIBILITY_GROUP_NO_DETECT)
    //    return false;

    // GM invisibility checks early, invisibility if any detectable, so if not stealth then visible
    if (u->GetVisibility() == VISIBILITY_GROUP_STEALTH)
    {
        //do not know what is the use of this detect
        if (!detect || !canDetectStealthOf(u, GetDistance(u)))
            return false;
    }

    // Now check is target visible with LoS
    //return u->IsWithinLOS(GetPositionX(),GetPositionY(),GetPositionZ());
    return true;
}

bool Creature::canStartAttack(Unit const* who) const
{
    if (isCivilian())
        return false;

    if (HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE))
        return false;

    // Do not attack non-combat pets
    if (who->GetTypeId() == TYPEID_UNIT && who->GetCreatureType() == CREATURE_TYPE_NON_COMBAT_PET)
        return false;

    if (!canFly() && (GetDistanceZ(who) > CREATURE_Z_ATTACK_RANGE + m_CombatDistance))
        return false;

    if (!who->isInAccessiblePlaceFor(this))
        return false;

    if (!IsWithinDistInMap(who, GetAttackDistance(who)))
        return false;

    if (!canAttack(who, false))
        return false;

    return IsWithinLOSInMap(who);
}

float Creature::GetAttackDistance(Unit const* pl) const
{
    float aggroRate = sWorld.getRate(RATE_CREATURE_AGGRO);
    if (aggroRate == 0)
        return 0.0f;

    int32 playerlevel   = pl->getLevelForTarget(this);
    int32 creaturelevel = getLevelForTarget(pl);

    int32 leveldif       = playerlevel - creaturelevel;

    // "The maximum Aggro Radius has a cap of 25 levels under. Example: A level 30 char has the same Aggro Radius of a level 5 char on a level 60 mob."
    if (leveldif < - 25)
        leveldif = -25;

    // "The aggro radius of a mob having the same level as the player is roughly 20 yards"
    float RetDistance = 20;

    // "Aggro Radius varies with level difference at a rate of roughly 1 yard/level"
    // radius grow if playlevel < creaturelevel
    RetDistance -= (float)leveldif;

    if (creaturelevel + 5 <= int32(sWorld.getConfig(CONFIG_MAX_PLAYER_LEVEL)))
    {
        // detect range auras
        RetDistance += GetTotalAuraModifier(SPELL_AURA_MOD_DETECT_RANGE);

        // detected range auras
        RetDistance += pl->GetTotalAuraModifier(SPELL_AURA_MOD_DETECTED_RANGE);
    }

    // "Minimum Aggro Radius for a mob seems to be combat range (5 yards)"
    if (RetDistance < 5)
        RetDistance = 5;

    return (RetDistance * aggroRate);
}

void Creature::setDeathState(DeathState s)
{
    Unit::setDeathState(s);

    if (s == JUST_DIED)
    {
        m_corpseRemoveTime = time(NULL) + m_corpseDelay;
        m_respawnTime = time(NULL) + m_respawnDelay + m_corpseDelay;

        // always save boss respawn time at death to prevent crash cheating
        if (sWorld.getConfig(CONFIG_SAVE_RESPAWN_TIME_IMMEDIATELY) || isWorldBoss())
            SaveRespawnTime();

        SetNoSearchAssistance(false);

        //Dismiss group if is leader
        if (m_formation && m_formation->getLeader() == this)
            m_formation->FormationReset(true);

        SetHealth(0);
        SetPower(getPowerType(), 0);

        if (m_zoneScript)
            m_zoneScript->OnCreatureDeath(this);

        SetUInt64Value(UNIT_FIELD_TARGET, 0);               // remove target selection in any cases (can be set at aura remove in Unit::setDeathState)
        SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);

        SetUInt32Value(UNIT_FIELD_MOUNTDISPLAYID, 0);       // if creature is mounted on a virtual mount, remove it at death

        setActive(false);

        // return, since we promote to DEAD_FALLING. DEAD_FALLING is promoted to CORPSE at next update.
        if (canFly() && IsFlying())
            if (FallGround())
            return;

        Unit::setDeathState(CORPSE);
    }
    if (s == JUST_RESPAWNED)
    {
        SetHealth(GetMaxHealth());
        SetLootRecipient(NULL);
        ResetPlayerDamageReq();
        Unit::setDeathState(ALIVE);
        CreatureInfo const* cinfo = GetCreatureTemplate();
        RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE);

        UpdateMovementFlags();

        SetUInt32Value(UNIT_NPC_FLAGS, cinfo->npcflag);
        ClearUnitState(UNIT_STATE_ALL_STATE);
        i_motionMaster.Initialize();
        SetMeleeDamageSchool(SpellSchools(cinfo->dmgschool));
        LoadCreaturesAddon(true);

        // Prevents the creature from re-spawning at the location of it's death
        GetMap()->CreatureRespawnRelocation(this);
    }
}

bool Creature::FallGround()
{
    // Only if state is JUST_DIED. DEAD_FALLING is set below and promoted to CORPSE later
    if (getDeathState() != JUST_DIED)
        return false;

    // use larger distance for vmap height search than in most other cases
    float ground_Z = GetMap()->GetHeight(GetPositionX(), GetPositionY(), GetPositionZ(), true, MAX_FALL_DISTANCE);

    if (ground_Z < INVALID_HEIGHT)
    {
        sLog.outDebug("FallGround: creature %u at map %u (x: %f, y: %f, z: %f), not able to retrive a proper GetHeight (z: %f).",
                      GetEntry(), GetMap()->GetId(), GetPositionX(), GetPositionX(), GetPositionZ(), ground_Z);
    }

    // Abort too if the ground is very near
    if (fabs(GetPositionZ() - ground_Z) < 0.1f)
        return false;

    Unit::setDeathState(DEAD_FALLING);

    GetMotionMaster()->MoveFall();

    return true;
}

void Creature::Respawn(bool force)
{
    DestroyForNearbyPlayers();

    if (force)
    {
        if (IsAlive())
            setDeathState(JUST_DIED);
        else if (getDeathState() != CORPSE)
            setDeathState(CORPSE);
    }

    RemoveCorpse(false);

    if (getDeathState() == DEAD)
    {
        if (m_DBTableGuid)
            sObjectMgr.SaveCreatureRespawnTime(m_DBTableGuid, GetInstanceId(), 0);

        DEBUG_LOG("Respawning creature %s (GuidLow: %u, Full GUID: " UI64FMTD " Entry: %u)", GetName(), GetGUIDLow(), GetGUID(), GetEntry());
        m_respawnTime = 0;
        ResetPickPocketRefillTimer();
        loot.clear();

        if (m_originalEntry != GetEntry())
            UpdateEntry(m_originalEntry);

        CreatureInfo const* cinfo = GetCreatureTemplate();
        SelectLevel(cinfo);

        setDeathState(JUST_RESPAWNED);

        GetMotionMaster()->InitDefault();

        //Call AI respawn virtual function
        if (IsAIEnabled)
            AI()->JustRespawned();

        uint32 poolid = sPoolMgr.IsPartOfAPool<Creature>(GetDBTableGUIDLow());
        if (poolid)
            sPoolMgr.UpdatePool<Creature>(poolid, GetDBTableGUIDLow());
    }

    UpdateObjectVisibility();
}

void Creature::ForcedDespawn(uint32 timeMSToDespawn)
{
    if (timeMSToDespawn)
    {
        ForcedDespawnDelayEvent* pEvent = new ForcedDespawnDelayEvent(*this);

        m_Events.AddEvent(pEvent, m_Events.CalculateTime(timeMSToDespawn));
        return;
    }

    if (IsAlive())
        setDeathState(JUST_DIED);

    RemoveCorpse(false);
}

bool Creature::IsImmuneToSpell(SpellEntry const* spellInfo, bool useCharges)
{
    if (!spellInfo)
        return false;

    if (GetCreatureTemplate()->MechanicImmuneMask & (1 << (spellInfo->Mechanic - 1)))
        return true;

    return Unit::IsImmuneToSpell(spellInfo, useCharges);
}

bool Creature::IsImmuneToSpellEffect(SpellEntry const* spellInfo, uint32 index, bool castOnSelf) const
{
    if (!castOnSelf && GetCreatureTemplate()->MechanicImmuneMask & (1 << (spellInfo->EffectMechanic[index] - 1)))
        return true;

    // Taunt immunity special flag check
    if (GetCreatureTemplate()->flags_extra & CREATURE_FLAG_EXTRA_NO_TAUNT)
    {
        // Taunt aura apply check
        if (spellInfo->Effect[index] == SPELL_EFFECT_APPLY_AURA)
        {
            if (spellInfo->EffectApplyAuraName[index] == SPELL_AURA_MOD_TAUNT)
                return true;
        }
        // Spell effect taunt check
        else if (spellInfo->Effect[index] == SPELL_EFFECT_ATTACK_ME)
            return true;
    }

    return Unit::IsImmuneToSpellEffect(spellInfo, index, castOnSelf);
}

SpellEntry const* Creature::reachWithSpellAttack(Unit* pVictim)
{
    if (!pVictim)
        return NULL;

    for (uint32 i = 0; i < CREATURE_MAX_SPELLS; i++)
    {
        if (!m_spells[i])
            continue;
        SpellEntry const* spellInfo = sSpellStore.LookupEntry(m_spells[i]);
        if (!spellInfo)
        {
            sLog.outError("WORLD: unknown spell id %i\n", m_spells[i]);
            continue;
        }

        bool bcontinue = true;
        for (uint32 j = 0; j < 3; j++)
        {
            if ((spellInfo->Effect[j] == SPELL_EFFECT_SCHOOL_DAMAGE)       ||
                (spellInfo->Effect[j] == SPELL_EFFECT_INSTAKILL)            ||
                (spellInfo->Effect[j] == SPELL_EFFECT_ENVIRONMENTAL_DAMAGE) ||
                (spellInfo->Effect[j] == SPELL_EFFECT_HEALTH_LEECH)
               )
            {
                bcontinue = false;
                break;
            }
        }
        if (bcontinue) continue;

        if (spellInfo->manaCost > GetPower(POWER_MANA))
            continue;
        SpellRangeEntry const* srange = sSpellRangeStore.LookupEntry(spellInfo->rangeIndex);
        float range = GetSpellMaxRange(srange);
        float minrange = GetSpellMinRange(srange);
        float dist = GetDistance(pVictim);
        //if (!isInFront(pVictim, range) && spellInfo->AttributesEx)
        //    continue;
        if (dist > range || dist < minrange)
            continue;
        if (HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED))
            continue;
        return spellInfo;
    }
    return NULL;
}

SpellEntry const* Creature::reachWithSpellCure(Unit* pVictim)
{
    if (!pVictim)
        return NULL;

    for (uint32 i = 0; i < CREATURE_MAX_SPELLS; i++)
    {
        if (!m_spells[i])
            continue;
        SpellEntry const* spellInfo = sSpellStore.LookupEntry(m_spells[i]);
        if (!spellInfo)
        {
            sLog.outError("WORLD: unknown spell id %i\n", m_spells[i]);
            continue;
        }

        bool bcontinue = true;
        for (uint32 j = 0; j < 3; j++)
        {
            if ((spellInfo->Effect[j] == SPELL_EFFECT_HEAL))
            {
                bcontinue = false;
                break;
            }
        }
        if (bcontinue) continue;

        if (spellInfo->manaCost > GetPower(POWER_MANA))
            continue;
        SpellRangeEntry const* srange = sSpellRangeStore.LookupEntry(spellInfo->rangeIndex);
        float range = GetSpellMaxRange(srange);
        float minrange = GetSpellMinRange(srange);
        float dist = GetDistance(pVictim);
        //if (!isInFront(pVictim, range) && spellInfo->AttributesEx)
        //    continue;
        if (dist > range || dist < minrange)
            continue;
        if (HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED))
            continue;
        return spellInfo;
    }
    return NULL;
}

bool Creature::IsVisibleInGridForPlayer(Player const* pl) const
{
    // gamemaster in GM mode see all, including ghosts
    if (pl->isGameMaster())
        return true;

    // Live player (or with not release body see live creatures or death creatures with corpse disappearing time > 0
    if (pl->IsAlive() || pl->GetDeathTimer() > 0)
    {
        if (GetEntry() == VISUAL_WAYPOINT && !pl->isGameMaster())
            return false;
        return (IsAlive() || m_corpseRemoveTime > uint32(time(NULL)));
    }

    // Dead player see creatures near own corpse
    Corpse* corpse = pl->GetCorpse();
    if (corpse)
    {
        // 20 - aggro distance for same level, 25 - max additional distance if player level less that creature level
        if (corpse->IsWithinDistInMap(this, (20 + 25) * sWorld.getRate(RATE_CREATURE_AGGRO)))
            return true;
    }

    // Dead player can see ghosts
    if (GetCreatureTemplate()->type_flags & CREATURE_TYPEFLAGS_GHOST_VISIBLE)
        return true;

    // and not see any other
    return false;
}

void Creature::DoFleeToGetAssistance()
{
    if (!getVictim())
        return;

    if (HasAuraType(SPELL_AURA_PREVENTS_FLEEING))
        return;

    float radius = sWorld.getConfig(CONFIG_CREATURE_FAMILY_FLEE_ASSISTANCE_RADIUS);
    if (radius > 0)
    {
        Creature* pCreature = NULL;

        CellPair p(Oregon::ComputeCellPair(GetPositionX(), GetPositionY()));
        Cell cell(p);
        cell.data.Part.reserved = ALL_DISTRICT;
        cell.SetNoCreate();
        Oregon::NearestAssistCreatureInCreatureRangeCheck u_check(this, getVictim(), radius);
        Oregon::CreatureLastSearcher<Oregon::NearestAssistCreatureInCreatureRangeCheck> searcher(pCreature, u_check);

        TypeContainerVisitor<Oregon::CreatureLastSearcher<Oregon::NearestAssistCreatureInCreatureRangeCheck>, GridTypeMapContainer > grid_creature_searcher(searcher);

        cell.Visit(p, grid_creature_searcher, *GetMap(), *this, radius);

        SetNoSearchAssistance(true);
        if (!pCreature)
            SetControlled(true, UNIT_STATE_FLEEING);
        else
            GetMotionMaster()->MoveSeekAssistance(pCreature->GetPositionX(), pCreature->GetPositionY(), pCreature->GetPositionZ());
    }

}

// select nearest hostile unit within the given distance (regardless of threat list).
Unit* Creature::SelectNearestTarget(float dist) const
{
    CellPair p(Oregon::ComputeCellPair(GetPositionX(), GetPositionY()));
    Cell cell(p);
    cell.data.Part.reserved = ALL_DISTRICT;
    cell.SetNoCreate();

    Unit* target = NULL;

    {
        Oregon::NearestHostileUnitCheck u_check(this, dist);
        Oregon::UnitLastSearcher<Oregon::NearestHostileUnitCheck> searcher(target, u_check);

        TypeContainerVisitor<Oregon::UnitLastSearcher<Oregon::NearestHostileUnitCheck>, WorldTypeMapContainer > world_unit_searcher(searcher);
        TypeContainerVisitor<Oregon::UnitLastSearcher<Oregon::NearestHostileUnitCheck>, GridTypeMapContainer >  grid_unit_searcher(searcher);

        cell.Visit(p, world_unit_searcher, *GetMap(), *this, dist);
        cell.Visit(p, grid_unit_searcher, *GetMap(), *this, dist);
    }

    return target;
}

// select nearest hostile unit within the given attack distance (i.e. distance is ignored if > than ATTACK_DISTANCE), regardless of threat list.
Unit* Creature::SelectNearestTargetInAttackDistance(float dist) const
{
    CellPair p(Oregon::ComputeCellPair(GetPositionX(), GetPositionY()));
    Cell cell(p);
    cell.data.Part.reserved = ALL_DISTRICT;
    cell.SetNoCreate();

    Unit *target = NULL;

    if (dist > ATTACK_DISTANCE)
        sLog.outError("Creature (GUID: %u Entry: %u) SelectNearestTargetInAttackDistance called with dist > ATTACK_DISTANCE. Extra distance ignored.",GetGUIDLow(),GetEntry());

    {
        Oregon::NearestHostileUnitInAttackDistanceCheck u_check(this, dist);
        Oregon::UnitLastSearcher<Oregon::NearestHostileUnitInAttackDistanceCheck> searcher(target, u_check);

        TypeContainerVisitor<Oregon::UnitLastSearcher<Oregon::NearestHostileUnitInAttackDistanceCheck>, WorldTypeMapContainer > world_unit_searcher(searcher);
        TypeContainerVisitor<Oregon::UnitLastSearcher<Oregon::NearestHostileUnitInAttackDistanceCheck>, GridTypeMapContainer >  grid_unit_searcher(searcher);

        cell.Visit(p, world_unit_searcher, *GetMap(), *this, ATTACK_DISTANCE);
        cell.Visit(p, grid_unit_searcher, *GetMap(), *this, ATTACK_DISTANCE);
    }

    return target;
}

void Creature::CallAssistance()
{
    if (!m_AlreadyCallAssistance && getVictim() && !IsPet() && !isCharmed())
    {
        SetNoCallAssistance(true);

        float radius = sWorld.getConfig(CONFIG_CREATURE_FAMILY_ASSISTANCE_RADIUS);
        if (radius > 0)
        {
            std::list<Creature*> assistList;

            {
                CellPair p(Oregon::ComputeCellPair(GetPositionX(), GetPositionY()));
                Cell cell(p);
                cell.data.Part.reserved = ALL_DISTRICT;
                cell.SetNoCreate();

                Oregon::AnyAssistCreatureInRangeCheck u_check(this, getVictim(), radius);
                Oregon::CreatureListSearcher<Oregon::AnyAssistCreatureInRangeCheck> searcher(assistList, u_check);

                TypeContainerVisitor<Oregon::CreatureListSearcher<Oregon::AnyAssistCreatureInRangeCheck>, GridTypeMapContainer >  grid_creature_searcher(searcher);

                cell.Visit(p, grid_creature_searcher, *GetMap(), *this, radius);
            }

            if (!assistList.empty())
            {
                AssistDelayEvent* e = new AssistDelayEvent(getVictim()->GetGUID(), *this);
                while (!assistList.empty())
                {
                    // Pushing guids because in delay can happen some creature gets despawned => invalid pointer
                    e->AddAssistant((*assistList.begin())->GetGUID());
                    assistList.pop_front();
                }
                m_Events.AddEvent(e, m_Events.CalculateTime(sWorld.getConfig(CONFIG_CREATURE_FAMILY_ASSISTANCE_DELAY)));
            }
        }
    }
}

void Creature::CallForHelp(float fRadius)
{
    if (fRadius <= 0.0f || !getVictim() || IsPet() || isCharmed())
        return;

    CellPair p(Oregon::ComputeCellPair(GetPositionX(), GetPositionY()));
    Cell cell(p);
    cell.data.Part.reserved = ALL_DISTRICT;
    cell.SetNoCreate();

    Oregon::CallOfHelpCreatureInRangeDo u_do(this, getVictim(), fRadius);
    Oregon::CreatureWorker<Oregon::CallOfHelpCreatureInRangeDo> worker(u_do);

    TypeContainerVisitor<Oregon::CreatureWorker<Oregon::CallOfHelpCreatureInRangeDo>, GridTypeMapContainer >  grid_creature_searcher(worker);

    cell.Visit(p, grid_creature_searcher, *GetMap(), *this, fRadius);
}

bool Creature::CanAssistTo(const Unit* u, const Unit* enemy, bool checkfaction /*= true*/) const
{
    if (IsInEvadeMode())
        return false;

    // is it true?
    if (!HasReactState(REACT_AGGRESSIVE))
        return false;

    // we don't need help from zombies :)
    if (!IsAlive())
        return false;

    if (isCivilian())
        return false;

    if (HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PASSIVE))
        return false;

    // skip fighting creature
    if (IsInCombat())
        return false;

    // only from same creature faction
    if (checkfaction)
    {
        if (getFaction() != u->getFaction())
            return false;
    }
    else
    {
        if (!IsFriendlyTo(u))
            return false;
    }

    // only free creature
    if (GetCharmerOrOwnerGUID())
        return false;

    // skip non hostile to caster enemy creatures
    if (!IsHostileTo(enemy))
        return false;

    return true;
}

void Creature::SaveRespawnTime()
{
    if (IsPet() || !m_DBTableGuid || (m_creatureData && !m_creatureData->dbData))
        return;

    sObjectMgr.SaveCreatureRespawnTime(m_DBTableGuid, GetInstanceId(), m_respawnTime);
}

bool Creature::IsOutOfThreatArea(Unit* pVictim) const
{
    if (!pVictim)
        return true;

    if (!pVictim->IsInMap(this))
        return true;

    if (!pVictim->isTargetableForAttack())
        return true;

    if (!pVictim->isInAccessiblePlaceFor(this))
        return true;

    if (!pVictim->isVisibleForOrDetect(this, true, false))
        return true;

    if (sMapStore.LookupEntry(GetMapId())->IsDungeon())
        return false;

    float length = pVictim->GetDistance(m_homePosition);
    float AttackDist = GetAttackDistance(pVictim);
    uint32 ThreatRadius = sWorld.getConfig(CONFIG_THREAT_RADIUS);

    //Use AttackDistance in distance check if threat radius is lower. This prevents creature bounce in and out of combat every update tick.
    return (length > (ThreatRadius > AttackDist ? ThreatRadius : AttackDist));
}

CreatureDataAddon const* Creature::GetCreatureAddon() const
{
    if (m_DBTableGuid)
    {
        if (CreatureDataAddon const* addon = ObjectMgr::GetCreatureAddon(m_DBTableGuid))
            return addon;
    }

    // dependent from heroic mode entry
    return ObjectMgr::GetCreatureTemplateAddon(GetCreatureTemplate()->Entry);
}

//creature_addon table
bool Creature::LoadCreaturesAddon(bool reload)
{
    CreatureDataAddon const* cainfo = GetCreatureAddon();
    if (!cainfo)
        return false;

    if (cainfo->mount != 0)
        Mount(cainfo->mount);

    if (cainfo->bytes0 != 0)
        SetUInt32Value(UNIT_FIELD_BYTES_0, cainfo->bytes0);

    if (cainfo->bytes1 != 0)
        SetUInt32Value(UNIT_FIELD_BYTES_1, cainfo->bytes1);

    // Sheath State
    SetUInt32Value(UNIT_FIELD_BYTES_2, cainfo->bytes2);

    if (cainfo->emote != 0)
        SetUInt32Value(UNIT_NPC_EMOTESTATE, cainfo->emote);

    if (cainfo->move_flags != 0)
        SetUnitMovementFlags(cainfo->move_flags);

    //Load Path
    if (cainfo->path_id != 0)
        m_path_id = cainfo->path_id;

    if (cainfo->auras)
    {
        for (CreatureDataAddonAura const* cAura = cainfo->auras; cAura->spell_id; ++cAura)
        {
            SpellEntry const* AdditionalSpellInfo = sSpellStore.LookupEntry(cAura->spell_id);
            if (!AdditionalSpellInfo)
            {
                sLog.outErrorDb("Creature (GUIDLow: %u Entry: %u) has wrong spell %u defined in auras field.", GetGUIDLow(), GetEntry(), cAura->spell_id);
                continue;
            }

            // skip already applied aura
            if (HasAura(cAura->spell_id, cAura->effect_idx))
            {
                if (!reload)
                    sLog.outErrorDb("Creature (GUIDLow: %u Entry: %u) has duplicate aura (spell %u effect %u) in auras field.", GetGUIDLow(), GetEntry(), cAura->spell_id, cAura->effect_idx);

                continue;
            }

            Aura* AdditionalAura = CreateAura(AdditionalSpellInfo, cAura->effect_idx, NULL, this, this, 0);
            AddAura(AdditionalAura);
            sLog.outDebug("Spell: %u with Aura %u added to creature (GUIDLow: %u Entry: %u)", cAura->spell_id, AdditionalSpellInfo->EffectApplyAuraName[0], GetGUIDLow(), GetEntry());
        }
    }
    return true;
}

// Send a message to LocalDefense channel for players opposition team in the zone
void Creature::SendZoneUnderAttackMessage(Player* attacker)
{
    uint32 enemy_team = attacker->GetTeam();

    WorldPacket data(SMSG_ZONE_UNDER_ATTACK, 4);
    data << (uint32)GetZoneId();
    sWorld.SendGlobalMessage(&data, NULL, (enemy_team == ALLIANCE ? HORDE : ALLIANCE));
}

void Creature::SetInCombatWithZone()
{
    if (!CanHaveThreatList())
    {
        sLog.outError("Creature entry %u call SetInCombatWithZone but creature cannot have threat list.", GetEntry());
        return;
    }

    Map* pMap = GetMap();

    if (!pMap->IsDungeon())
    {
        sLog.outError("Creature entry %u call SetInCombatWithZone for map (id: %u) that isn't an instance.", GetEntry(), pMap->GetId());
        return;
    }

    Map::PlayerList const& PlList = pMap->GetPlayers();

    if (PlList.isEmpty())
        return;

    for (Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
    {
        if (Player* pPlayer = i->getSource())
        {
            if (pPlayer->isGameMaster())
                continue;

            if (pPlayer->IsAlive())
            {
                pPlayer->SetInCombatWith(this);
                AddThreat(pPlayer, 0.0f);
            }
        }
    }
}

void Creature::_AddCreatureSpellCooldown(uint32 spell_id, time_t end_time)
{
    m_CreatureSpellCooldowns[spell_id] = end_time;
}

void Creature::_AddCreatureCategoryCooldown(uint32 category, time_t end_time)
{
    m_CreatureCategoryCooldowns[category] = end_time;
}

void Creature::AddCreatureSpellCooldown(uint32 spellid)
{
    SpellEntry const* spellInfo = sSpellStore.LookupEntry(spellid);
    if (!spellInfo)
        return;

    uint32 cooldown = GetSpellRecoveryTime(spellInfo);
    if (Player* modOwner = GetSpellModOwner())
        modOwner->ApplySpellMod(spellid, SPELLMOD_COOLDOWN, cooldown);

    if (cooldown)
        _AddCreatureSpellCooldown(spellid, time(NULL) + cooldown / IN_MILLISECONDS);

    if (spellInfo->Category)
        _AddCreatureCategoryCooldown(spellInfo->Category, time(NULL));

    m_GlobalCooldown = spellInfo->StartRecoveryTime;

    if (Unit* owner = GetCharmerOrOwner())
        if (owner->GetTypeId() == TYPEID_PLAYER)
        {
            WorldPacket data(SMSG_SPELL_COOLDOWN, (4 + 8));
            data << uint64(GetGUID());
            data << uint8(0x0);                                     // flags (0x1, 0x2)
            data << uint32(spellid);
            data << uint32(cooldown);                               // in m.secs

            owner->ToPlayer()->GetSession()->SendPacket(&data);
        }
}

bool Creature::HasCategoryCooldown(uint32 spell_id) const
{
    SpellEntry const* spellInfo = sSpellStore.LookupEntry(spell_id);
    if (!spellInfo)
        return false;

    // check global cooldown if spell affected by it
    if (spellInfo->StartRecoveryCategory > 0 && m_GlobalCooldown > 0)
        return true;

    CreatureSpellCooldowns::const_iterator itr = m_CreatureCategoryCooldowns.find(spellInfo->Category);
    return (itr != m_CreatureCategoryCooldowns.end() && time_t(itr->second + (spellInfo->CategoryRecoveryTime / IN_MILLISECONDS)) > time(NULL));
}

bool Creature::HasSpellCooldown(uint32 spell_id) const
{
    CreatureSpellCooldowns::const_iterator itr = m_CreatureSpellCooldowns.find(spell_id);
    return (itr != m_CreatureSpellCooldowns.end() && itr->second > time(NULL)) || HasCategoryCooldown(spell_id);
}

bool Creature::IsInEvadeMode() const
{
    return /*!i_motionMaster.empty() &&*/ i_motionMaster.GetCurrentMovementGeneratorType() == HOME_MOTION_TYPE;
}

bool Creature::HasSpell(uint32 spellID) const
{
    uint8 i;
    for (i = 0; i < CREATURE_MAX_SPELLS; ++i)
        if (spellID == m_spells[i])
            break;
    return i < CREATURE_MAX_SPELLS;                         //broke before end of iteration of known spells
}

time_t Creature::GetRespawnTimeEx() const
{
    time_t now = time(NULL);
    if (m_respawnTime > now)
        return m_respawnTime;
    else
        return now;
}

void Creature::GetRespawnCoord(float& x, float& y, float& z, float* ori, float* dist) const
{
    if (m_DBTableGuid)
    {
        if (CreatureData const* data = sObjectMgr.GetCreatureData(GetDBTableGUIDLow()))
        {
            x = data->posX;
            y = data->posY;
            z = data->posZ;
            if (ori)
                *ori = data->orientation;
            if (dist)
                *dist = data->spawndist;

            return;
        }
    }

    x = GetPositionX();
    y = GetPositionY();
    z = GetPositionZ();
    if (ori)
        *ori = GetOrientation();
    if (dist)
        *dist = 0;
}

void Creature::AllLootRemovedFromCorpse()
{
    if (loot.loot_type != LOOT_SKINNING && !IsPet() && GetCreatureTemplate()->SkinLootId && hasLootRecipient())
        if (LootTemplates_Skinning.HaveLootFor(GetCreatureTemplate()->SkinLootId))
            SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE);

    time_t now = time(NULL);
    // Do not reset corpse remove time if corpse is already removed
    if (m_corpseRemoveTime <= now)
        return;

    float decayRate = sWorld.getRate(RATE_CORPSE_DECAY_LOOTED);

    // corpse skinnable, but without skinning flag, and then skinned, corpse will despawn next update
    if (loot.loot_type == LOOT_SKINNING)
        m_corpseRemoveTime = now;
    else
        m_corpseRemoveTime = now + uint32(m_corpseDelay * decayRate);

    m_respawnTime = m_corpseRemoveTime + m_respawnDelay;
}

uint32 Creature::getLevelForTarget(Unit const* target) const
{
    if (!isWorldBoss())
        return Unit::getLevelForTarget(target);

    uint32 level = target->getLevel() + sWorld.getConfig(CONFIG_WORLD_BOSS_LEVEL_DIFF);
    if (level < 1)
        return 1;
    if (level > 255)
        return 255;
    return level;
}

std::string Creature::GetAIName() const
{
    return ObjectMgr::GetCreatureTemplate(GetEntry())->AIName;
}

std::string Creature::GetScriptName()
{
    return sObjectMgr.GetScriptName(GetScriptId());
}

uint32 Creature::GetScriptId()
{
    return ObjectMgr::GetCreatureTemplate(GetEntry())->ScriptID;
}

VendorItemData const* Creature::GetVendorItems() const
{
    return sObjectMgr.GetNpcVendorItemList(GetEntry());
}

uint32 Creature::GetVendorItemCurrentCount(VendorItem const* vItem)
{
    if (!vItem->maxcount)
        return vItem->maxcount;

    VendorItemCounts::iterator itr = m_vendorItemCounts.begin();
    for (; itr != m_vendorItemCounts.end(); ++itr)
        if (itr->itemId == vItem->item)
            break;

    if (itr == m_vendorItemCounts.end())
        return vItem->maxcount;

    VendorItemCount* vCount = &*itr;

    time_t ptime = time(NULL);

    if (vCount->lastIncrementTime + vItem->incrtime <= uint32(ptime))
    {
        ItemPrototype const* pProto = sObjectMgr.GetItemPrototype(vItem->item);

        uint32 diff = uint32((ptime - vCount->lastIncrementTime) / vItem->incrtime);
        if ((vCount->count + diff * pProto->BuyCount) >= vItem->maxcount)
        {
            m_vendorItemCounts.erase(itr);
            return vItem->maxcount;
        }

        vCount->count += diff * pProto->BuyCount;
        vCount->lastIncrementTime = ptime;
    }

    return vCount->count;
}

uint32 Creature::UpdateVendorItemCurrentCount(VendorItem const* vItem, uint32 used_count)
{
    if (!vItem->maxcount)
        return 0;

    VendorItemCounts::iterator itr = m_vendorItemCounts.begin();
    for (; itr != m_vendorItemCounts.end(); ++itr)
        if (itr->itemId == vItem->item)
            break;

    if (itr == m_vendorItemCounts.end())
    {
        uint32 new_count = vItem->maxcount > used_count ? vItem->maxcount - used_count : 0;
        m_vendorItemCounts.push_back(VendorItemCount(vItem->item, new_count));
        return new_count;
    }

    VendorItemCount* vCount = &*itr;

    time_t ptime = time(NULL);

    if (vCount->lastIncrementTime + vItem->incrtime <= uint32(ptime))
    {
        ItemPrototype const* pProto = sObjectMgr.GetItemPrototype(vItem->item);

        uint32 diff = uint32((ptime - vCount->lastIncrementTime) / vItem->incrtime);
        if ((vCount->count + diff * pProto->BuyCount) < vItem->maxcount)
            vCount->count += diff * pProto->BuyCount;
        else
            vCount->count = vItem->maxcount;
    }

    vCount->count = vCount->count > used_count ? vCount->count - used_count : 0;
    vCount->lastIncrementTime = ptime;
    return vCount->count;
}

TrainerSpellData const* Creature::GetTrainerSpells() const
{
    return sObjectMgr.GetNpcTrainerSpells(GetEntry());
}

// overwrite WorldObject function for proper name localization
const char* Creature::GetNameForLocaleIdx(int32 loc_idx) const
{
    if (loc_idx >= 0)
    {
        CreatureLocale const* cl = sObjectMgr.GetCreatureLocale(GetEntry());
        if (cl)
        {
            if (cl->Name.size() > uint32(loc_idx) && !cl->Name[loc_idx].empty())
                return cl->Name[loc_idx].c_str();
        }
    }

    return GetName();
}

const CreatureData* Creature::GetLinkedRespawnCreatureData() const
{
    if (!m_DBTableGuid) // only hard-spawned creatures from DB can have a linked master
        return NULL;

    if (uint32 targetGuid = sObjectMgr.GetLinkedRespawnGuid(m_DBTableGuid))
        return sObjectMgr.GetCreatureData(targetGuid);

    return NULL;
}

// returns master's remaining respawn time if any
time_t Creature::GetLinkedCreatureRespawnTime() const
{
    if (!m_DBTableGuid) // only hard-spawned creatures from DB can have a linked master
        return 0;

    if (uint32 targetGuid = sObjectMgr.GetLinkedRespawnGuid(m_DBTableGuid))
    {
        Map* targetMap = NULL;
        if (const CreatureData* data = sObjectMgr.GetCreatureData(targetGuid))
        {
            if (data->mapid == GetMapId())   // look up on the same map
                targetMap = GetMap();
            else                            // it shouldn't be instanceable map here
                targetMap = MapManager::Instance().FindMap(data->mapid);
        }
        if (targetMap)
            return sObjectMgr.GetCreatureRespawnTime(targetGuid, targetMap->GetInstanceId());
    }

    return 0;
}

void Creature::StartPickPocketRefillTimer()
{
    _pickpocketLootRestore = time(NULL) + sWorld.getConfig(CONFIG_CREATURE_PICKPOCKET_REFILL);
}

void Creature::SetRooted(bool apply)
{
    if (apply)
    {
        // MOVEMENTFLAG_ROOT cannot be used in conjunction with MOVEMENTFLAG_MASK_MOVING (tested 3.3.5a)
        // this will freeze clients. That's why we remove MOVEMENTFLAG_MASK_MOVING before
        // setting MOVEMENTFLAG_ROOT
        RemoveUnitMovementFlag(MOVEFLAG_MOVING);
        m_movementInfo.AddMovementFlag(MOVEFLAG_ROOT);

        WorldPacket data(SMSG_SPLINE_MOVE_ROOT, 9);
        data << GetPackGUID();
        SendMessageToSet(&data, true);
    }
    else
    {
        if (!HasUnitState(UNIT_STATE_STUNNED))      // prevent moving if it also has stun effect
        {
            m_movementInfo.RemoveMovementFlag(MOVEFLAG_ROOT);

            WorldPacket data(SMSG_SPLINE_MOVE_UNROOT, 9);
            data << GetPackGUID();
            SendMessageToSet(&data, true);
        }
    }
}

bool Creature::SetFeatherFall(bool apply)
{
    if (!Unit::SetFeatherFall(apply))
        return false;

    if (!movespline->Initialized())
        return true;

    WorldPacket data(apply ? SMSG_SPLINE_MOVE_FEATHER_FALL : SMSG_SPLINE_MOVE_NORMAL_FALL);
    data << GetPackGUID();
    SendMessageToSet(&data, true);
    return true;
}

bool Creature::SetHover(bool apply)
{
    if (!Unit::SetHover(apply))
        return false;

    if (!movespline->Initialized())
        return true;

    WorldPacket data(apply ? SMSG_SPLINE_MOVE_SET_HOVER : SMSG_SPLINE_MOVE_UNSET_HOVER, 9);
    data << GetPackGUID();
    SendMessageToSet(&data, false);
    return true;
}

bool Creature::SetCanFly(bool apply, bool packetOnly)
{
    if (!packetOnly && !Unit::SetCanFly(apply))
        return false;

    if (!movespline->Initialized())
        return true;

    WorldPacket data(apply ? SMSG_SPLINE_MOVE_SET_FLYING : SMSG_SPLINE_MOVE_UNSET_FLYING, 9);
    data << GetPackGUID();
    SendMessageToSet(&data, true);
    return true;
}

bool Creature::SetSwim(bool apply, bool packetOnly)
{
    if (!packetOnly && !Unit::SetSwim(apply))
        return false;

    if (!movespline->Initialized())
        return true;

    WorldPacket data(apply ? SMSG_SPLINE_MOVE_START_SWIM : SMSG_SPLINE_MOVE_STOP_SWIM);
    data << GetPackGUID();
    SendMessageToSet(&data, true);
    return true;
}

bool Creature::SetWalk(bool apply)
{
    if (!Unit::SetWalk(apply))
        return false;

    WorldPacket data(apply ? SMSG_SPLINE_MOVE_SET_WALK_MODE : SMSG_SPLINE_MOVE_SET_RUN_MODE, 9);
    data << GetPackGUID();
    SendMessageToSet(&data, true);
    return true;
}

bool Creature::SetLevitate(bool apply, bool packetOnly)
{
    if (!packetOnly && !Unit::SetLevitate(apply))
        return false;

    if (!movespline->Initialized())
        return true;

    WorldPacket data(apply ? SMSG_SPLINE_MOVE_SET_FLYING : SMSG_SPLINE_MOVE_UNSET_FLYING, 9);
    data << GetPackGUID();
    SendMessageToSet(&data, true);
    return true;
}

bool Creature::SetWaterWalk(bool apply)
{
    if (!Unit::SetWaterWalk(apply))
        return false;

    if (!movespline->Initialized())
        return true;

    WorldPacket data(apply ? SMSG_SPLINE_MOVE_WATER_WALK : SMSG_SPLINE_MOVE_LAND_WALK, 9);
    data << GetPackGUID();
    SendMessageToSet(&data, true);
    return true;
}
