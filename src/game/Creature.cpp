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
#include "MapManager.h"
#include "SpellMgr.h"
#include "Creature.h"
#include "QuestDef.h"
#include "GossipDef.h"
#include "Player.h"
#include "PoolMgr.h"
#include "Opcodes.h"
#include "Log.h"
#include "LootMgr.h"
#include "CreatureAI.h"
#include "CreatureAISelector.h"
#include "Formulas.h"
#include "SpellAuras.h"
#include "InstanceData.h"
#include "Battleground.h"
#include "Utilities/Util.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"
#include "GameEventMgr.h"
#include "CreatureGroups.h"
#include "MoveSpline.h"
#include <fstream>

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

    if (modelid1) modelIDs[c++] = modelid1;
    if (modelid2) modelIDs[c++] = modelid2;
    if (modelid3) modelIDs[c++] = modelid3;
    if (modelid4) modelIDs[c++] = modelid4;

    return ((c > 0) ? modelIDs[urand(0, c - 1)] : 0);
}

uint32 CreatureInfo::GetFirstValidModelId() const
{
    if (modelid1) return modelid1;
    if (modelid2) return modelid2;
    if (modelid3) return modelid3;
    if (modelid4) return modelid4;
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
    m_owner.DespawnOrUnsummon();
    return true;
}

Creature::Creature(bool isWorldObject): Unit(isWorldObject),
    _pickpocketLootRestore(0),
    _skinner(0),
    m_GlobalCooldown(0),
    hasPlayerDamaged(false), m_SightDistance(sWorld.getConfig(CONFIG_SIGHT_MONSTER)),
    m_CombatDistance(MELEE_RANGE), m_lootMoney(0), m_lootRecipient(0), m_lootRecipientGroup(0), m_corpseRemoveTime(0), m_respawnTime(0), m_respawnDelay(25),
    m_corpseDelay(60), m_respawnradius(0.0f), m_combatPulseTime(0), m_combatPulseDelay(0), m_emoteState(0), m_reactState(REACT_AGGRESSIVE), m_regenTimer(2000),
    m_defaultMovementType(IDLE_MOTION_TYPE), m_DBTableGuid(0), m_equipmentId(0),
    m_AlreadyCallAssistance(false), m_AlreadySearchedAssistance(false), m_regenHealth(true), m_AI_locked(false),
    m_meleeDamageSchoolMask(SPELL_SCHOOL_MASK_NORMAL), DisableReputationGain(false), m_creatureData(NULL),
    m_formation(NULL), m_creatureInfo(NULL)
{
    m_regenTimer = CREATURE_REGEN_INTERVAL;
    m_valuesCount = UNIT_END;

    for (uint8 i = 0; i < CREATURE_MAX_SPELLS; ++i)
        m_spells[i] = 0;

    m_CreatureSpellCooldowns.clear();
    m_CreatureCategoryCooldowns.clear();

    m_SightDistance = sWorld.getConfig(CONFIG_SIGHT_MONSTER);
    m_CombatDistance = 0; // MELEE_RANGE

    TriggerJustRespawned = false;
    m_isTempWorldObject = false;
}

Creature::~Creature()
{
    m_vendorItemCounts.clear();

    if (i_AI)
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

void Creature::DisappearAndDie(bool CorpseRemove)
{
    DestroyForNearbyPlayers();
    if (IsAlive())
        setDeathState(JUST_DIED);

    RemoveCorpse(CorpseRemove);
}

void Creature::SearchFormation()
{
    if (IsSummon())
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
    uint32 respawnDelay = m_respawnDelay;
    if (IsAIEnabled)
        AI()->CorpseRemoved(respawnDelay);

    // Should get removed later, just keep "compatibility" with scripts
    if (setSpawnTime)
        m_respawnTime = time(NULL) + respawnDelay;

    float x, y, z, o;
    GetRespawnPosition(x, y, z, &o);
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

    // equal to player Race field, but creature does not have race
    SetByteValue(UNIT_FIELD_BYTES_0, 0, 0);

    // known valid are: CLASS_WARRIOR, CLASS_PALADIN, CLASS_ROGUE, CLASS_MAGE
    SetByteValue(UNIT_FIELD_BYTES_0, 1, uint8(cinfo->unit_class));

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

    SetFaction(cInfo->faction);

    uint32 npcflag, unit_flags, dynamicflags;
    ObjectMgr::ChooseCreatureFlags(cInfo, npcflag, unit_flags, dynamicflags, data);

    if (cInfo->flags_extra & CREATURE_FLAG_EXTRA_WORLDEVENT)
        SetUInt32Value(UNIT_NPC_FLAGS, npcflag | sGameEventMgr.GetNPCFlag(this));
    else
        SetUInt32Value(UNIT_NPC_FLAGS, npcflag);

    SetUInt32Value(UNIT_FIELD_FLAGS, unit_flags);
    SetUInt32Value(UNIT_DYNAMIC_FLAGS, dynamicflags);

    RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);

    SetAttackTime(BASE_ATTACK, cInfo->BaseAttackTime);
    SetAttackTime(OFF_ATTACK, cInfo->BaseAttackTime);
    SetAttackTime(RANGED_ATTACK, cInfo->RangeAttackTime);

    SelectLevel();

    SetMeleeDamageSchool(SpellSchools(cInfo->dmgschool));
    CreatureBaseStats const* cCLS = sObjectMgr.GetCreatureClassLvlStats(getLevel(), cInfo->unit_class, cInfo->exp);
    float armor = cCLS->BaseArmor * cInfo->ModArmor;
    SetModifierValue(UNIT_MOD_ARMOR, BASE_VALUE, armor);

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
    if (FactionTemplateEntry const* factionTemplate = sFactionTemplateStore.LookupEntry(cInfo->faction))
    {
        if (factionTemplate->factionFlags & FACTION_TEMPLATE_FLAG_PVP || IsPvP()) // PvP state may be set in UnitFlags.. Prevent overwrite
            SetPvP(true);
        else
            SetPvP(false);
    }

    // trigger creature is always not selectable and can not be attacked
    if (IsTrigger())
        SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

    InitializeReactState();

    if (cInfo->flags_extra & CREATURE_FLAG_EXTRA_NO_TAUNT)
    {
        ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
        ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
    }

    UpdateMovementFlags();
    LoadCreaturesAddon(true);
    return true;
}

void Creature::Update(uint32 diff)
{
    if (m_GlobalCooldown <= diff)
        m_GlobalCooldown = 0;
    else
        m_GlobalCooldown -= diff;

    if (IsAIEnabled && TriggerJustRespawned)
    {
        TriggerJustRespawned = false;
        AI()->JustRespawned();
    }

    UpdateMovementFlags();

    switch (m_deathState)
    {
        case JUST_RESPAWNED:
            // Must not be called, see Creature::setDeathState JUST_RESPAWNED -> ALIVE promoting.
            sLog.outError("Creature (GUIDLow: %u Entry: %u) in wrong state: JUST_RESPAWNED (4)", GetGUIDLow(), GetEntry());
            break;
        case JUST_DIED:
            // Must not be called, see Creature::setDeathState JUST_DIED -> CORPSE promoting.
            sLog.outError("Creature (GUIDLow: %u Entry: %u) in wrong state: JUST_DEAD (1)", GetGUIDLow(), GetEntry());
            break;
        case DEAD:
        {
            time_t now = time(NULL);
            if (m_respawnTime <= time(NULL))
            {
                time_t linkedRespawntime = GetLinkedCreatureRespawnTime();
                if (!linkedRespawntime)     // Can respawn
                    Respawn();
                else                        // the master is dead
                {
                    uint32 targetGuid = sObjectMgr.GetLinkedRespawnGuid(m_DBTableGuid);
                    if (targetGuid == m_DBTableGuid) // if linking self, never respawn (check delayed to next day)
                        SetRespawnTime(DAY);
                    else
                        m_respawnTime = (now > linkedRespawntime ? now : linkedRespawntime) + urand(5, MINUTE); // else copy time from master and add a little
                    SaveRespawnTime(); // also save to DB immediately
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

            if (m_groupLootTimer && lootingGroupLeaderGUID)
            {
                if (m_groupLootTimer <= diff)
                {
                    Group* group = sObjectMgr.GetGroupByLeader(lootingGroupLeaderGUID);
                    if (group)
                        group->EndRoll();
                    m_groupLootTimer = 0;
                    lootingGroupLeaderGUID = 0;
                }
                else m_groupLootTimer -= diff;
            }
            else if (m_corpseRemoveTime <= time(NULL))
            {
                RemoveCorpse(false);
                DEBUG_LOG("Removing corpse... %u ", GetUInt32Value(OBJECT_FIELD_ENTRY));
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
                        if (canStartAttack(charmer, true))
                            i_AI->AttackStart(charmer);

                LastCharmerGUID = 0;
            }

            // if periodic combat pulse is enabled and we are both in combat and in a dungeon, do this now
            if (m_combatPulseDelay > 0 && IsInCombat() && GetMap()->IsDungeon())
            {
                if (diff > m_combatPulseTime)
                    m_combatPulseTime = 0;
                else
                    m_combatPulseTime -= diff;

                if (m_combatPulseTime == 0)
                {
                    Map::PlayerList const &players = GetMap()->GetPlayers();
                    if (!players.isEmpty())
                        for (Map::PlayerList::const_iterator it = players.begin(); it != players.end(); ++it)
                        {
                            if (Player* player = it->GetSource())
                            {
                                if (player->IsGameMaster())
                                    continue;

                                if (player->IsAlive() && this->IsHostileTo(player))
                                {
                                    if (CanHaveThreatList())
                                        AddThreat(player, 0.0f);
                                    this->SetInCombatWith(player);
                                    player->SetInCombatWith(this);
                                }
                            }
                        }

                    m_combatPulseTime = m_combatPulseDelay * IN_MILLISECONDS;
                }
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
                RegenerateHealth();

            RegenerateMana();

            m_regenTimer = CREATURE_REGEN_INTERVAL;
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

    // Apply modifiers (if any).
    AuraList const& ModPowerRegenPCTAuras = this->GetAurasByType(SPELL_AURA_MOD_POWER_REGEN_PERCENT);
    for (AuraList::const_iterator i = ModPowerRegenPCTAuras.begin(); i != ModPowerRegenPCTAuras.end(); ++i)
        if ((*i)->GetMiscValue() == POWER_MANA)
            AddPct(addvalue, (*i)->GetAmount());

    addvalue += GetTotalAuraModifierByMiscValue(SPELL_AURA_MOD_POWER_REGEN, POWER_MANA) * CREATURE_REGEN_INTERVAL / (5 * IN_MILLISECONDS);

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

    // Apply modifiers (if any).
    AuraList const& ModPowerRegenPCTAuras = GetAurasByType(SPELL_AURA_MOD_HEALTH_REGEN_PERCENT);
    for (AuraList::const_iterator i = ModPowerRegenPCTAuras.begin(); i != ModPowerRegenPCTAuras.end(); ++i)
        AddPct(addvalue, (*i)->GetAmount());

    addvalue += GetTotalAuraModifier(SPELL_AURA_MOD_REGEN) * CREATURE_REGEN_INTERVAL  / (5 * IN_MILLISECONDS);

    ModifyHealth(addvalue);
}

void Creature::DoFleeToGetAssistance()
{
    if (!GetVictim())
        return;

    if (HasAuraType(SPELL_AURA_PREVENTS_FLEEING))
        return;

    float radius = sWorld.getConfig(CONFIG_CREATURE_FAMILY_FLEE_ASSISTANCE_RADIUS);
    if (radius > 0)
    {
        Creature* creature = NULL;

        CellCoord p(Oregon::ComputeCellCoord(GetPositionX(), GetPositionY()));
        Cell cell(p);
        cell.SetNoCreate();
        Oregon::NearestAssistCreatureInCreatureRangeCheck u_check(this, GetVictim(), radius);
        Oregon::CreatureLastSearcher<Oregon::NearestAssistCreatureInCreatureRangeCheck> searcher(creature, u_check);

        TypeContainerVisitor<Oregon::CreatureLastSearcher<Oregon::NearestAssistCreatureInCreatureRangeCheck>, GridTypeMapContainer > grid_creature_searcher(searcher);

        cell.Visit(p, grid_creature_searcher, *GetMap(), *this, radius);

        SetNoSearchAssistance(true);
        UpdateSpeed(MOVE_RUN, false);

        if (!creature)
            SetControlled(true, UNIT_STATE_FLEEING);
        else
            GetMotionMaster()->MoveSeekAssistance(creature->GetPositionX(), creature->GetPositionY(), creature->GetPositionZ());
    }

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

bool Creature::Create(uint32 guidlow, Map* map, uint32 entry, uint32 team, float x, float y, float z, float ang, const CreatureData* data)
{
    ASSERT(map);
    SetMap(map);

    CreatureInfo const* cinfo = sObjectMgr.GetCreatureTemplate(entry);
    if (!cinfo)
    {
        sLog.outError("Creature::Create(): creature template (guidlow: %u, entry: %u) does not exist.", guidlow, entry);
        return false;
    }

    //! Relocate before CreateFromProto, to initialize coords and allow
    //! returning correct zone id for selecting OutdoorPvP/Battlefield script
    Relocate(x, y, z, ang);

    if (!IsPositionValid())
    {
        sLog.outError("Creature (guidlow %d, entry %d) not loaded. Suggested coordinates isn't valid (X: %f Y: %f)", guidlow, entry, x, y);
        return false;
    }

    // Allow players to see those units while dead, do it here (mayby altered by addon auras)
    if (cinfo->type_flags & CREATURE_TYPE_FLAG_GHOST_VISIBLE)
        m_serverSideVisibility.SetValue(SERVERSIDE_VISIBILITY_GHOST, GHOST_VISIBILITY_ALIVE | GHOST_VISIBILITY_GHOST);

    if (!CreateFromProto(guidlow, entry, team, data))
        return false;

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

    /// @todo Replace with spell, handle from DB
    if (IsSpiritHealer() || IsSpiritGuide())
    {
        m_serverSideVisibility.SetValue(SERVERSIDE_VISIBILITY_GHOST, GHOST_VISIBILITY_GHOST);
        m_serverSideVisibilityDetect.SetValue(SERVERSIDE_VISIBILITY_GHOST, GHOST_VISIBILITY_GHOST);
    }

    return true;
}

void Creature::InitializeReactState()
{
    if (IsTotem() || IsTrigger() || IsCritter() || IsSpiritService())
        SetReactState(REACT_PASSIVE);
    else
        SetReactState(REACT_AGGRESSIVE);
}

bool Creature::IsTrainerOf(Player* pPlayer, bool msg) const
{
    if (!IsTrainer())
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
            if (FactionTemplateEntry const* faction_template = GetFactionTemplateEntry())
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

bool Creature::CanInteractWithBattleMaster(Player* player, bool msg) const
{
    if (!IsBattleMaster())
        return false;

    uint32 bgTypeId = sObjectMgr.GetBattleMasterBG(GetEntry());
    if (!msg)
        return player->GetBGAccessByLevel(bgTypeId);

    if (!player->GetBGAccessByLevel(bgTypeId))
    {
        player->PlayerTalkClass->ClearMenus();
        switch (bgTypeId)
        {
            case BATTLEGROUND_AV:  player->PlayerTalkClass->SendGossipMenu(7616, GetGUID()); break;
            case BATTLEGROUND_WS:  player->PlayerTalkClass->SendGossipMenu(7599, GetGUID()); break;
            case BATTLEGROUND_AB:  player->PlayerTalkClass->SendGossipMenu(7642, GetGUID()); break;
            case BATTLEGROUND_EY:
            case BATTLEGROUND_NA:
            case BATTLEGROUND_BE:
            case BATTLEGROUND_AA:
            case BATTLEGROUND_RL: player->PlayerTalkClass->SendGossipMenu(10024, GetGUID()); break;
            default: break;
        }
        return false;
    }
    return true;
}

bool Creature::CanTrainAndResetTalentsOf(Player* player) const
{
    return player->getLevel() >= 10
        && GetCreatureTemplate()->trainer_type == TRAINER_TYPE_CLASS
        && player->getClass() == GetCreatureTemplate()->classNum;
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
        RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE|UNIT_DYNFLAG_OTHER_TAGGER);
        return;
    }

    Player* player = unit->GetCharmerOrOwnerPlayerOrPlayerItself();
    if (!player)                                             // normal creature, no player involved
        return;

    m_lootRecipient = player->GetGUID();
    if (Group* group = player->GetGroup())
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
    uint32 npcflag = GetUInt32Value(UNIT_NPC_FLAGS);
    uint32 unit_flags = GetUInt32Value(UNIT_FIELD_FLAGS);
    uint32 dynamicflags = GetUInt32Value(UNIT_DYNAMIC_FLAGS);

    // check if it's a custom model and if not, use 0 for displayId
    CreatureInfo const* cinfo = GetCreatureTemplate();
    if (cinfo)
    {
        if (displayId == cinfo->modelid1 || displayId == cinfo->modelid2 ||
            displayId == cinfo->modelid3 || displayId == cinfo->modelid4)
            displayId = 0;
    }

    if (npcflag == cinfo->npcflag)
        npcflag = 0;

    if (unit_flags == cinfo->unit_flags)
        unit_flags = 0;

    if (dynamicflags == cinfo->dynamicflags)
        dynamicflags = 0;

    // data->guid = guid must not be update at save
    data.id = GetEntry();
    data.mapid = mapid;
    data.displayid = displayId;
    data.equipmentId = GetCurrentEquipmentId();
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
    data.npcflag = npcflag;
    data.unit_flags = unit_flags;
    data.dynamicflags = dynamicflags;

    // updated in DB
    WorldDatabase.BeginTransaction();

    WorldDatabase.PExecuteLog("DELETE FROM creature WHERE guid = '%u'", m_DBTableGuid);

    std::ostringstream ss;
    ss << "INSERT INTO creature (guid, id , map, spawnMask, modelid, equipment_id, position_x, position_y, position_z, orientation, spawntimesecs, spawndist, currentwaypoint, curhealth, curmana, MovementType, npcflag, unit_flags, dynamicflags) VALUES ("
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
       << GetDefaultMovementType() << ","                  //default movement generator type
       << npcflag << ","
       << unit_flags << ","
       << dynamicflags << ");";

    WorldDatabase.PExecuteLog("%s", ss.str().c_str());

    WorldDatabase.CommitTransaction();

    if (sWorld.getConfig(CONFIG_CREATEUPDATE_FILE))
        sLog.CreateUpdateFile(ss.str().c_str());
}

void Creature::SelectLevel()
{
    CreatureInfo const* cInfo = GetCreatureTemplate();

    uint32 rank = IsPet() ? 0 : cInfo->rank;

    // level
    uint32 const minlevel = cInfo->minlevel;
    uint32 const maxlevel = cInfo->maxlevel;
    uint8 level = minlevel == maxlevel ? minlevel : urand(minlevel, maxlevel);
    SetLevel(level);

    uint32 health;
    uint32 mana;

    CreatureBaseStats const* cCLS = sObjectMgr.GetCreatureClassLvlStats(level, cInfo->unit_class, cInfo->exp);
    
    // health
    health = cCLS->BaseHealth * cInfo->ModHealth;

    health *= _GetHealthMod(rank); // Apply custom config settting
    if (health < 1)
        health = 1;

    SetCreateHealth(health);
    SetMaxHealth(health);
    SetHealth(health);
    SetPlayerDamaged(false);

    // mana
    mana = cCLS->BaseMana * cInfo->ModMana;
    SetCreateMana(mana);
    SetMaxPower(POWER_MANA, mana);                          //MAX Mana
    SetPower(POWER_MANA, mana);

    SetModifierValue(UNIT_MOD_HEALTH, BASE_VALUE, (float)health);
    SetModifierValue(UNIT_MOD_MANA, BASE_VALUE, (float)mana);

    // damage
    float basedamage = cCLS->BaseDamage;
    
    float weaponBaseMinDamage = basedamage;
    float weaponBaseMaxDamage = basedamage * 1.5;

    SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, weaponBaseMinDamage);
    SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, weaponBaseMaxDamage);
    
    SetBaseWeaponDamage(OFF_ATTACK, MINDAMAGE, weaponBaseMinDamage);
    SetBaseWeaponDamage(OFF_ATTACK, MAXDAMAGE, weaponBaseMaxDamage);
    
    SetBaseWeaponDamage(RANGED_ATTACK, MINDAMAGE, weaponBaseMinDamage);
    SetBaseWeaponDamage(RANGED_ATTACK, MAXDAMAGE, weaponBaseMaxDamage);
    
    SetModifierValue(UNIT_MOD_ATTACK_POWER, BASE_VALUE, cCLS->BaseMeleeAttackPower);
    SetModifierValue(UNIT_MOD_ATTACK_POWER_RANGED, BASE_VALUE, cCLS->BaseRangedAttackPower);

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

void Creature::SetPlayerDamaged(bool set)
{
    hasPlayerDamaged = set;
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

float Creature::GetSpellDamageMod(int32 Rank) const
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

bool Creature::CreateFromProto(uint32 guidlow, uint32 entry, uint32 team, const CreatureData* data)
{
    SetZoneScript();
    if (m_zoneScript && data)
    {
        entry = m_zoneScript->GetCreatureEntry(guidlow, data);
        if (!entry)
            return false;
    }

    CreatureInfo const* cinfo = sObjectMgr.GetCreatureTemplate(entry);
    if (!cinfo)
    {
        sLog.outErrorDb("Creature::CreateFromProto(): creature template (guidlow: %u, entry: %u) does not exist.", guidlow, entry);
        return false;
    }

    SetOriginalEntry(entry);

    Object::_Create(guidlow, entry, HIGHGUID_UNIT);

    if (!UpdateEntry(entry, team, data))
        return false;

    return true;
}

bool Creature::LoadCreatureFromDB(uint32 guid, Map* map, bool addToMap)
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
        if (CanFly())
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

    if (addToMap && !GetMap()->AddToMap(this))
        return false;
    return true;
}

void Creature::SetCanDualWield(bool value)
{
    Unit::SetCanDualWield(value);
    UpdateDamagePhysical(OFF_ATTACK);
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

    ItemTemplate const* proto = ObjectMgr::GetItemTemplate(item_id);
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
    QuestRelations qr = sObjectMgr.mCreatureQuestRelations;
    for (QuestRelations::const_iterator itr = qr.lower_bound(GetEntry()); itr != qr.upper_bound(GetEntry()); ++itr)
    {
        if (itr->second == quest_id)
            return true;
    }
    return false;
}

bool Creature::hasInvolvedQuest(uint32 quest_id) const
{
    QuestRelations qir = sObjectMgr.mCreatureQuestInvolvedRelations;
    for (QuestRelations::const_iterator itr = qir.lower_bound(GetEntry()); itr != qir.upper_bound(GetEntry()); ++itr)
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
    std::ostringstream statement;

    // This below will generate the update file 
    statement << "DELETE FROM creature WHERE guid = " << m_DBTableGuid << "; \n"
        "DELETE FROM creature_addon WHERE guid = " << m_DBTableGuid << "; \n"
        "DELETE FROM game_event_creature WHERE guid = " << m_DBTableGuid << "; \n"
        "DELETE FROM game_event_model_equip WHERE guid = " << m_DBTableGuid << ";";
    
    WorldDatabase.BeginTransaction();
    WorldDatabase.PExecuteLog("DELETE FROM creature WHERE guid = '%u'", m_DBTableGuid);
    WorldDatabase.PExecuteLog("DELETE FROM creature_addon WHERE guid = '%u'", m_DBTableGuid);
    WorldDatabase.PExecuteLog("DELETE FROM game_event_creature WHERE guid = '%u'", m_DBTableGuid);
    WorldDatabase.PExecuteLog("DELETE FROM game_event_model_equip WHERE guid = '%u'", m_DBTableGuid);
    WorldDatabase.CommitTransaction();

    if (sWorld.getConfig(CONFIG_CREATEUPDATE_FILE))
        sLog.CreateUpdateFile(statement.str().c_str());

    sObjectMgr.DeleteCreatureData(m_DBTableGuid);
}

bool Creature::IsInvisibleDueToDespawn() const
{
    if (Unit::IsInvisibleDueToDespawn())
        return true;

    if (IsAlive() || isDying() || m_corpseRemoveTime > time(NULL))
        return false;

    return true;
}

bool Creature::CanAlwaysSee(WorldObject const* obj) const
{
    if (IsAIEnabled && AI()->CanSeeAlways(obj))
        return true;

    return false;
}

bool Creature::canStartAttack(Unit const* who, bool force) const
{
    if (IsCivilian())
        return false;

    // This set of checks is should be done only for creatures
    if ((HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC) && who->GetTypeId() != TYPEID_PLAYER)                                   // flag is valid only for non player characters
        || (HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC) && who->GetTypeId() == TYPEID_PLAYER)                                 // immune to PC and target is a player, return false
        || (who->GetOwner() && who->GetOwner()->GetTypeId() == TYPEID_PLAYER && HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC))) // player pets are immune to pc as well
        return false;

    // Do not attack non-combat pets
    if (who->GetTypeId() == TYPEID_UNIT && who->GetCreatureType() == CREATURE_TYPE_NON_COMBAT_PET)
        return false;

    if (!CanFly() && (GetDistanceZ(who) > CREATURE_Z_ATTACK_RANGE + m_CombatDistance))
        return false;

    if (!force)
    {
        if (!_IsTargetAcceptable(who))
            return false;

        if (who->IsInCombat() && IsWithinDist(who, ATTACK_DISTANCE))
            if (Unit* victim = who->getAttackerForHelper())
                if (IsWithinDistInMap(victim, sWorld.getConfig(CONFIG_CREATURE_FAMILY_ASSISTANCE_RADIUS)))
                    force = true;

        if (!force && (IsNeutralToAll() || !IsWithinDistInMap(who, GetAttackDistance(who) + m_CombatDistance)))
            return false;
    }

    if (!CanCreatureAttack(who, force))
        return false;

    return IsWithinLOSInMap(who);
}

float Creature::GetAttackDistance(Unit const* player) const
{
    float aggroRate = sWorld.getRate(RATE_CREATURE_AGGRO);
    if (aggroRate == 0)
        return 0.0f;

    uint32 playerlevel   = player->getLevelForTarget(this);
    uint32 creaturelevel = getLevelForTarget(player);

    int32 leveldif       = int32(playerlevel) - int32(creaturelevel);

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
        RetDistance += player->GetTotalAuraModifier(SPELL_AURA_MOD_DETECTED_RANGE);
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

        SetTarget(0);                // remove target selection in any cases (can be set at aura remove in Unit::setDeathState)
        SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);

        SetUInt32Value(UNIT_FIELD_MOUNTDISPLAYID, 0); // if creature is mounted on a virtual mount, remove it at death

        setActive(false);

        if (HasSearchedAssistance())
        {
            SetNoSearchAssistance(false);
            UpdateSpeed(MOVE_RUN, false);
        }

        //Dismiss group if is leader
        if (m_formation && m_formation->getLeader() == this)
            m_formation->FormationReset(true);

        if ((CanFly() || IsFlying()))
            GetMotionMaster()->MoveFall();

        SetHealth(0);
        SetPower(getPowerType(), 0);

        if (m_zoneScript)
            m_zoneScript->OnCreatureDeath(this);

        Unit::setDeathState(CORPSE);
    }
    else if (s == JUST_RESPAWNED)
    {
        SetFullHealth();
        SetLootRecipient(nullptr);
        SetPlayerDamaged(false);

        UpdateMovementFlags();

        ClearUnitState(uint32(UNIT_STATE_ALL_STATE & ~UNIT_STATE_IGNORE_PATHFINDING));

        if (!IsPet())
        {
            CreatureInfo  const* cinfo = GetCreatureTemplate();

            SetUInt32Value(UNIT_NPC_FLAGS, cinfo->npcflag);

            RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);

            SetMeleeDamageSchool(SpellSchools(cinfo->dmgschool));
        }

        Unit::setDeathState(ALIVE);
        LoadCreaturesAddon(true);

        i_motionMaster.Initialize();

        // Prevents the creature from re-spawning at the location of it's death
        GetMap()->CreatureRespawnRelocation(this);
    }
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

        SelectLevel();

        setDeathState(JUST_RESPAWNED);

        GetMotionMaster()->InitDefault();

        //Call AI respawn virtual function
        if (IsAIEnabled)
        {
            //reset the AI to be sure no dirty or uninitialized values will be used till next tick
            AI()->Reset();
            TriggerJustRespawned = true;    //delay event to next tick so all creatures are created on the map before processing
        }

        uint32 poolid = sPoolMgr.IsPartOfAPool<Creature>(GetDBTableGUIDLow());
        if (poolid)
            sPoolMgr.UpdatePool<Creature>(poolid, GetDBTableGUIDLow());

        //Re-initialize reactstate that could be altered by movementgenerators
        InitializeReactState();
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

void Creature::DespawnOrUnsummon(uint32 msTimeToDespawn /*= 0*/)
{
    if (TempSummon* summon = this->ToTempSummon())
        summon->UnSummon(msTimeToDespawn);
    else
        ForcedDespawn(msTimeToDespawn);
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

    if (GetCreatureTemplate()->type == CREATURE_TYPE_MECHANICAL && spellInfo->Effect[index] == SPELL_EFFECT_HEAL)
        return true;

    return Unit::IsImmuneToSpellEffect(spellInfo, index, castOnSelf);
}

bool Creature::isElite() const
{
    if (IsPet())
        return false;

    uint32 rank = GetCreatureTemplate()->rank;
    return rank != CREATURE_ELITE_NORMAL && rank != CREATURE_ELITE_RARE;
}

bool Creature::isWorldBoss() const
{
    if (IsPet())
        return false;

    return (GetCreatureTemplate()->type_flags & CREATURE_TYPE_FLAG_BOSS_MOB) != 0;
}

SpellEntry const* Creature::reachWithSpellAttack(Unit* victim)
{
    if (!victim)
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
        if (bcontinue)
            continue;

        if (spellInfo->manaCost > GetPower(POWER_MANA))
            continue;

        SpellRangeEntry const* srange = sSpellRangeStore.LookupEntry(spellInfo->rangeIndex);
        float range = GetSpellMaxRange(srange);
        float minrange = GetSpellMinRange(srange);
        float dist = GetDistance(victim);
        if (dist > range || dist < minrange)
            continue;
        if (spellInfo->PreventionType == SPELL_PREVENTION_TYPE_SILENCE && HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED))
            continue;
        if (spellInfo->PreventionType == SPELL_PREVENTION_TYPE_PACIFY && HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED))
            continue;
        return spellInfo;
    }
    return NULL;
}

SpellEntry const* Creature::reachWithSpellCure(Unit* victim)
{
    if (!victim)
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
        if (bcontinue)
            continue;

        if (spellInfo->manaCost > GetPower(POWER_MANA))
            continue;

        SpellRangeEntry const* srange = sSpellRangeStore.LookupEntry(spellInfo->rangeIndex);
        float range = GetSpellMaxRange(srange);
        float minrange = GetSpellMinRange(srange);
        float dist = GetDistance(victim);
        //if (!isInFront(victim, range) && spellInfo->AttributesEx)
        //    continue;
        if (dist > range || dist < minrange)
            continue;
        if (spellInfo->PreventionType == SPELL_PREVENTION_TYPE_SILENCE && HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED))
            continue;
        if (spellInfo->PreventionType == SPELL_PREVENTION_TYPE_PACIFY && HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED))
            continue;
        return spellInfo;
    }
    return NULL;
}

// select nearest hostile unit within the given distance (regardless of threat list).
Unit* Creature::SelectNearestTarget(float dist, bool playerOnly /* = false */) const
{
    CellCoord p(Oregon::ComputeCellCoord(GetPositionX(), GetPositionY()));
    Cell cell(p);
    cell.SetNoCreate();

    Unit* target = NULL;

    {
        Oregon::NearestHostileUnitCheck u_check(this, dist, playerOnly);
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
    CellCoord p(Oregon::ComputeCellCoord(GetPositionX(), GetPositionY()));
    Cell cell(p);
    cell.SetNoCreate();

    Unit *target = nullptr;

    if (dist > MAX_VISIBILITY_DISTANCE)
    {
        sLog.outError("Creature (GUID: %u Entry: %u) SelectNearestTargetInAttackDistance called with dist > MAX_VISIBILITY_DISTANCE. Distance set to ATTACK_DISTANCE.", GetGUIDLow(), GetEntry());
        dist = ATTACK_DISTANCE;
    }

    Oregon::NearestHostileUnitInAttackDistanceCheck u_check(this, dist);
    Oregon::UnitLastSearcher<Oregon::NearestHostileUnitInAttackDistanceCheck> searcher(target, u_check);

    TypeContainerVisitor<Oregon::UnitLastSearcher<Oregon::NearestHostileUnitInAttackDistanceCheck>, WorldTypeMapContainer > world_unit_searcher(searcher);
    TypeContainerVisitor<Oregon::UnitLastSearcher<Oregon::NearestHostileUnitInAttackDistanceCheck>, GridTypeMapContainer >  grid_unit_searcher(searcher);

    cell.Visit(p, world_unit_searcher, *GetMap(), *this, ATTACK_DISTANCE > dist ? ATTACK_DISTANCE : dist);
    cell.Visit(p, grid_unit_searcher, *GetMap(), *this, ATTACK_DISTANCE > dist ? ATTACK_DISTANCE : dist);

    return target;
}

void Creature::SendAIReaction(AiReaction reactionType)
{
    WorldPacket data(SMSG_AI_REACTION, 12);

    data << uint64(GetGUID());
    data << uint32(reactionType);

    ((WorldObject*)this)->SendMessageToSet(&data, true);

    sLog.outDebug("WORLD: Sent SMSG_AI_REACTION, type %u.", reactionType);
}

void Creature::CallAssistance()
{
    if (!m_AlreadyCallAssistance && GetVictim() && !IsPet() && !isCharmed())
    {
        SetNoCallAssistance(true);

        float radius = sWorld.getConfig(CONFIG_CREATURE_FAMILY_ASSISTANCE_RADIUS);

        if (radius > 0)
        {
            std::list<Creature*> assistList;

            {
                CellCoord p(Oregon::ComputeCellCoord(GetPositionX(), GetPositionY()));
                Cell cell(p);
                cell.SetNoCreate();

                Oregon::AnyAssistCreatureInRangeCheck u_check(this, GetVictim(), radius);
                Oregon::CreatureListSearcher<Oregon::AnyAssistCreatureInRangeCheck> searcher(assistList, u_check);

                TypeContainerVisitor<Oregon::CreatureListSearcher<Oregon::AnyAssistCreatureInRangeCheck>, GridTypeMapContainer >  grid_creature_searcher(searcher);

                cell.Visit(p, grid_creature_searcher, *GetMap(), *this, radius);
            }

            if (!assistList.empty())
            {
                AssistDelayEvent* e = new AssistDelayEvent(GetVictim()->GetGUID(), *this);
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

void Creature::CallForHelp(float radius)
{
    if (radius <= 0.0f || !GetVictim() || IsPet() || isCharmed())
        return;

    CellCoord p(Oregon::ComputeCellCoord(GetPositionX(), GetPositionY()));
    Cell cell(p);
    cell.SetNoCreate();

    Oregon::CallOfHelpCreatureInRangeDo u_do(this, GetVictim(), radius);
    Oregon::CreatureWorker<Oregon::CallOfHelpCreatureInRangeDo> worker(u_do);

    TypeContainerVisitor<Oregon::CreatureWorker<Oregon::CallOfHelpCreatureInRangeDo>, GridTypeMapContainer >  grid_creature_searcher(worker);

    cell.Visit(p, grid_creature_searcher, *GetMap(), *this, radius);
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

    // we don't need help from non-combatant ;)
    if (IsCivilian())
        return false;

    if (HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_NPC))
        return false;

    // skip fighting creature
    if (IsInCombat())
        return false;

    // only free creature
    if (GetCharmerOrOwnerGUID())
        return false;

    // only from same creature faction
    if (checkfaction)
    {
        if (GetFaction() != u->GetFaction())
            return false;
    }
    else
    {
        if (!IsFriendlyTo(u))
            return false;
    }

    // skip non hostile to caster enemy creatures
    if (!IsHostileTo(enemy))
        return false;

    return true;
}

// use this function to avoid having hostile creatures attack
// friendlies and other mobs they shouldn't attack
bool Creature::_IsTargetAcceptable(Unit const* target) const
{
    ASSERT(target);

    // if the target cannot be attacked, the target is not acceptable
    if (IsFriendlyTo(target)
        || !target->isTargetableForAttack(false))
        return false;

    if (target->HasUnitState(UNIT_STATE_DIED))
    {
        // guards can detect fake death
        if (IsGuard() && target->HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH))
            return true;
        else
            return false;
    }

    Unit const* targetVictim = target->getAttackerForHelper();

    // if I'm already fighting target, or I'm hostile towards the target, the target is acceptable
    if (IsInCombatWith(target) || IsHostileTo(target))
        return true;

    // if the target's victim is friendly, and the target is neutral, the target is acceptable
    if (targetVictim && IsFriendlyTo(targetVictim))
        return true;

    // if the target's victim is not friendly, or the target is friendly, the target is not acceptable
    return false;
}

void Creature::SaveRespawnTime()
{
    if (IsSummon() || !m_DBTableGuid || (m_creatureData && !m_creatureData->dbData))
        return;

    sObjectMgr.SaveCreatureRespawnTime(m_DBTableGuid, GetInstanceId(), m_respawnTime);
}

bool Creature::CanCreatureAttack(Unit const* victim, bool /*force*/) const
{
    if (!victim)
        return false;

    if (!victim->IsInMap(this))
        return false;

    if (!IsValidAttackTarget(victim))
        return false;

    if (!victim->isInAccessiblePlaceFor(this))
        return false;

    if (IsAIEnabled && !AI()->CanAIAttack(victim))
        return false;

    // we cannot attack in evade mode
    if (IsInEvadeMode())
        return false;

    // or if enemy is in evade mode
    if (victim->GetTypeId() == TYPEID_UNIT && victim->ToCreature()->IsInEvadeMode())
        return false;

    if (GetMap()->IsDungeon())
        return true;

    if (Unit* u = GetCharmerOrOwner())
    {
        if (u->GetTypeId() != TYPEID_PLAYER)
        {
            // don't check distance to home position if recently damaged, this should include taunt auras
            if (!isWorldBoss() && (GetLastDamagedTime() > getMSTime() || HasAuraType(SPELL_AURA_MOD_TAUNT)))
                return true;
        }
    }

    // Get the correct threat range from the config.
    float dist = std::max<float>(GetAttackDistance(victim), sWorld.getConfig(CONFIG_THREAT_RADIUS) + m_CombatDistance);

    if (Unit* unit = GetCharmerOrOwner())
        return victim->IsWithinDist(unit, dist);
    else
    {
        // include sizes for huge npcs
        dist += GetObjectSize() + victim->GetObjectSize();

        // to prevent creatures in air ignore attacks because distance is already too high...
        if (GetCreatureTemplate()->InhabitType & INHABIT_AIR)
            return victim->IsInDist2d(&m_homePosition, dist);
        else
            return victim->IsInDist(&m_homePosition, dist);
    }
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

    if (cainfo->bytes1 != 0)
        SetUInt32Value(UNIT_FIELD_BYTES_1, cainfo->bytes1);

    // Sheath State
    SetUInt32Value(UNIT_FIELD_BYTES_2, cainfo->bytes2);

    if (cainfo->emote != 0)
        SetUInt32Value(UNIT_NPC_EMOTESTATE, cainfo->emote);

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
        if (Player* pPlayer = i->GetSource())
        {
            if (pPlayer->IsGameMaster())
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
    return HasUnitState(UNIT_STATE_EVADE);
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

void Creature::GetRespawnPosition(float& x, float& y, float& z, float* ori, float* dist) const
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

uint8 Creature::getLevelForTarget(Unit const* target) const
{
    if (!isWorldBoss() || !target->ToUnit())
        return Unit::getLevelForTarget(target);

    uint16 level = target->ToUnit()->getLevel() + sWorld.getConfig(CONFIG_WORLD_BOSS_LEVEL_DIFF);
    if (level < 1)
        return 1;
    if (level > 255)
        return 255;
    return uint8(level);
}

std::string Creature::GetAIName() const
{
    return sObjectMgr.GetCreatureTemplate(GetEntry())->AIName;
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
        if (ItemTemplate const* pProto = sObjectMgr.GetItemTemplate(vItem->item))
        {
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
        if (ItemTemplate const* pProto = sObjectMgr.GetItemTemplate(vItem->item))
        {
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

bool Creature::SetWalk(bool enable)
{
    if (!Unit::SetWalk(enable))
        return false;

    WorldPacket data(enable ? SMSG_SPLINE_MOVE_SET_WALK_MODE : SMSG_SPLINE_MOVE_SET_RUN_MODE, 9);
    data << GetPackGUID();
    SendMessageToSet(&data, false);
    return true;
}

bool Creature::SetSwim(bool enable)
{
    if (!Unit::SetSwim(enable))
        return false;

    if (!movespline->Initialized())
        return true;

    WorldPacket data(enable ? SMSG_SPLINE_MOVE_START_SWIM : SMSG_SPLINE_MOVE_STOP_SWIM);
    data << GetPackGUID();
    SendMessageToSet(&data, true);
    return true;
}

bool Creature::SetCanFly(bool enable, bool /*packetOnly = false */)
{
    if (!Unit::SetCanFly(enable))
        return false;

    if (!movespline->Initialized())
        return true;

    WorldPacket data(enable ? SMSG_SPLINE_MOVE_SET_FLYING : SMSG_SPLINE_MOVE_UNSET_FLYING, 9);
    data << GetPackGUID();
    SendMessageToSet(&data, false);
    return true;
}

bool Creature::SetWaterWalking(bool enable, bool packetOnly /* = false */)
{
    if (!packetOnly && !Unit::SetWaterWalking(enable))
        return false;

    if (!movespline->Initialized())
        return true;

    WorldPacket data(enable ? SMSG_SPLINE_MOVE_WATER_WALK : SMSG_SPLINE_MOVE_LAND_WALK, 9);
    data << GetPackGUID();
    SendMessageToSet(&data, true);
    return true;
}

bool Creature::SetFeatherFall(bool enable, bool packetOnly /* = false */)
{
    if (!packetOnly && !Unit::SetFeatherFall(enable))
        return false;

    if (!movespline->Initialized())
        return true;

    WorldPacket data(enable ? SMSG_SPLINE_MOVE_FEATHER_FALL : SMSG_SPLINE_MOVE_NORMAL_FALL);
    data << GetPackGUID();
    SendMessageToSet(&data, true);
    return true;
}

bool Creature::SetHover(bool enable, bool packetOnly /*= false*/)
{
    if (!packetOnly && !Unit::SetHover(enable))
        return false;

    if (!movespline->Initialized())
        return true;

    WorldPacket data(enable ? SMSG_SPLINE_MOVE_SET_HOVER : SMSG_SPLINE_MOVE_UNSET_HOVER, 9);
    data << GetPackGUID();
    SendMessageToSet(&data, false);
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

void Creature::UpdateMovementFlags(bool packetOnly)
{
    if (GetCreatureTemplate()->flags_extra & CREATURE_FLAG_EXTRA_NO_DYNAMIC_MOVEMENT_FLAG)
        return;

    // Set the movement flags if the creature is in that mode. (Only fly if actually in air, only swim if in water, etc)
    float ground = GetMap()->GetHeight(GetPositionX(), GetPositionY(), GetPositionZ());

    if (ground < INVALID_HEIGHT)
    {
        sLog.outDebug("FallGround: creature %u at map %u (x: %f, y: %f, z: %f), not able to retrive a proper GetHeight (z: %f).",
            GetEntry(), GetMap()->GetId(), GetPositionX(), GetPositionX(), GetPositionZ(), ground);
        return;
    }

    bool isInAir = (G3D::fuzzyGt(GetPositionZ(), ground + 0.05f) || G3D::fuzzyLt(GetPositionZ(), ground - 0.05f)); // Can be underground too, prevent the falling

    if (GetCreatureTemplate()->InhabitType & INHABIT_AIR && isInAir && !IsFalling())
        SetLevitate(true, packetOnly);
    else
        SetLevitate(false);

    if (!isInAir)
        RemoveUnitMovementFlag(MOVEMENTFLAG_FALLING);

    SetSwim(CanSwim() && GetBaseMap()->IsSwimmable(GetPositionX(), GetPositionY(), GetPositionZ()));
}

void Creature::StartPickPocketRefillTimer()
{
    _pickpocketLootRestore = time(NULL) + sWorld.getConfig(CONFIG_CREATURE_PICKPOCKET_REFILL);
}

void Creature::SetTextRepeatId(uint8 textGroup, uint8 id)
{
    CreatureTextRepeatIds& repeats = m_textRepeat[textGroup];
    if (std::find(repeats.begin(), repeats.end(), id) == repeats.end())
        repeats.push_back(id);
    else
        sLog.outError("CreatureTextMgr: TextGroup %u for Creature(%s) GuidLow %u Entry %u, id %u already added", uint32(textGroup), GetName(), GetGUIDLow(), GetEntry(), uint32(id));
}

CreatureTextRepeatIds Creature::GetTextRepeatGroup(uint8 textGroup)
{
    CreatureTextRepeatIds ids;

    CreatureTextRepeatGroup::const_iterator groupItr = m_textRepeat.find(textGroup);
    if (groupItr != m_textRepeat.end())
        ids = groupItr->second;

    return ids;
}

void Creature::ClearTextRepeatGroup(uint8 textGroup)
{
    CreatureTextRepeatGroup::iterator groupItr = m_textRepeat.find(textGroup);
    if (groupItr != m_textRepeat.end())
        groupItr->second.clear();
}


float Creature::GetAggroRange(Unit const* target) const
{
    // Determines the aggro range for creatures (usually pets), used mainly for aggressive pet target selection.
    // Based on data from wowwiki due to lack of 3.3.5a data

    if (target && this->IsPet())
    {
        uint32 targetLevel = 0;

        if (target->GetTypeId() == TYPEID_PLAYER)
            targetLevel = target->getLevelForTarget(this);
        else if (target->GetTypeId() == TYPEID_UNIT)
            targetLevel = target->ToCreature()->getLevelForTarget(this);

        uint32 myLevel = getLevelForTarget(target);
        int32 levelDiff = int32(targetLevel) - int32(myLevel);

        // The maximum Aggro Radius is capped at 45 yards (25 level difference)
        if (levelDiff < -25)
            levelDiff = -25;

        // The base aggro radius for mob of same level
        float aggroRadius = 20;

        // Aggro Radius varies with level difference at a rate of roughly 1 yard/level
        aggroRadius -= (float)levelDiff;

        // detect range auras
        aggroRadius += GetTotalAuraModifier(SPELL_AURA_MOD_DETECT_RANGE);

        // detected range auras
        aggroRadius += target->GetTotalAuraModifier(SPELL_AURA_MOD_DETECTED_RANGE);

        // Just in case, we don't want pets running all over the map
        if (aggroRadius > MAX_AGGRO_RADIUS)
            aggroRadius = MAX_AGGRO_RADIUS;

        // Minimum Aggro Radius for a mob seems to be combat range (5 yards)
        //  hunter pets seem to ignore minimum aggro radius so we'll default it a little higher
        if (aggroRadius < 10)
            aggroRadius = 10;

        return (aggroRadius);
    }

    // Default
    return 0.0f;
}

Unit* Creature::SelectNearestHostileUnitInAggroRange(bool useLOS) const
{
    // Selects nearest hostile target within creature's aggro range. Used primarily by
    //  pets set to aggressive. Will not return neutral or friendly targets.

    Unit* target = NULL;

    {
        Oregon::NearestHostileUnitInAggroRangeCheck u_check(this, useLOS);
        Oregon::UnitSearcher<Oregon::NearestHostileUnitInAggroRangeCheck> searcher(target, u_check);

        VisitNearbyGridObject(MAX_AGGRO_RADIUS, searcher);
    }

    return target;
}
void Creature::SetBaseWalkSpeed(float speed)
{
    float newSpeed = speed;
    if (m_creatureInfo->speed_walk) // Creature template should still override
        newSpeed = m_creatureInfo->speed_walk;

    if (newSpeed != m_baseSpeedWalk)
    {
        m_baseSpeedWalk = newSpeed;
        UpdateSpeed(MOVE_WALK, false);
    }
}

void Creature::SetBaseRunSpeed(float speed)
{
    float newSpeed = speed;
    if (m_creatureInfo->speed_run) // Creature template should still override
        newSpeed = m_creatureInfo->speed_run;

    if (newSpeed != m_baseSpeedWalk)
    {
        m_baseSpeedRun = newSpeed;
        UpdateSpeed(MOVE_RUN, false);
    }
}