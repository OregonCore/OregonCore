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

#include "ScriptedCreature.h"
#include "Spell.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Cell.h"
#include "CellImpl.h"
#include "ObjectMgr.h"
#include "MoveSplineInit.h"

// Spell summary for ScriptedAI::SelectSpell
struct TSpellSummary
{
    uint8 Targets;                                          // set of enum SelectTarget
    uint8 Effects;                                          // set of enum SelectEffect
}* SpellSummary;

Creature* SummonList::GetCreatureWithEntry(uint32 entry) const
{
    for (auto i = begin(); i != end();)
    {
        if (Creature* summon = ObjectAccessor::GetCreature(*me, *i))
            if (summon->GetEntry() == entry)
                return summon;
    }

    return NULL;
}

bool SummonList::HasEntry(uint32 entry) const
{
    for (auto i = begin(); i != end(); ++i)
    {
        Creature* summon = ObjectAccessor::GetCreature(*me, *i);
        if (summon && summon->GetEntry() == entry)
            return true;
    }
    return false;
}

void SummonList::DoZoneInCombat(uint32 entry)
{
    for (iterator i = begin(); i != end();)
    {
        Creature* summon = Unit::GetCreature(*me, *i);
        ++i;
        if (summon && summon->IsAIEnabled
            && (!entry || summon->GetEntry() == entry))
            summon->AI()->DoZoneInCombat();
    }
}

void SummonList::DoAction(uint32 entry, uint32 info)
{
    for (iterator i = begin(); i != end();)
    {
        Creature* summon = Unit::GetCreature(*me, *i);
        ++i;
        if (summon && summon->IsAIEnabled
            && (!entry || summon->GetEntry() == entry))
            summon->AI()->DoAction(info);
    }
}

void SummonList::DespawnEntry(uint32 entry)
{
    for (iterator i = begin(); i != end();)
    {
        Creature* summon = Unit::GetCreature(*me, *i);
        if (!summon)
            erase(i++);
        else if (summon->GetEntry() == entry)
        {
            erase(i++);
            summon->setDeathState(JUST_DIED);
            summon->RemoveCorpse();
        }
        else
            ++i;
    }
}

void SummonList::DespawnAll()
{
    if (empty())
        return;

    while (!empty())
    {
        Creature* summon = Unit::GetCreature(*me, *begin());
        if (!summon)
            erase(begin());
        else
        {
            erase(begin());
            if (summon->IsSummon())
            {
                summon->DestroyForNearbyPlayers();
                CAST_SUM(summon)->UnSummon();
            }
            else
                summon->DisappearAndDie(false);
        }
    }
}

ScriptedAI::ScriptedAI(Creature* pCreature) : CreatureAI(pCreature),
    me(pCreature),
    IsFleeing(false),
    m_bCombatMovement(true),
    m_uiEvadeCheckCooldown(2500)
{
    HeroicMode = me->GetMap()->IsHeroic();
}

void ScriptedAI::AttackStartNoMove(Unit* pWho)
{
    if (!pWho)
        return;

    if (me->Attack(pWho, false))
        DoStartNoMovement(pWho);
}

void ScriptedAI::UpdateAI(const uint32 /*uiDiff*/)
{
    //Check if we have a current target
    if (!UpdateVictim())
        return;

    DoMeleeAttackIfReady();
}

void ScriptedAI::DoStartMovement(Unit* pVictim, float fDistance, float fAngle)
{
    if (pVictim)
        me->GetMotionMaster()->MoveChase(pVictim, fDistance, fAngle);
}

void ScriptedAI::DoStartNoMovement(Unit* pVictim)
{
    if (!pVictim)
        return;

    me->GetMotionMaster()->MoveIdle();
}

void ScriptedAI::DoStopAttack()
{
    if (me->GetVictim())
        me->AttackStop();
}

void ScriptedAI::DoCastSpell(Unit* pTarget, SpellEntry const* pSpellInfo, bool bTriggered)
{
    if (!pTarget || me->IsNonMeleeSpellCast(false))
        return;

    me->StopMoving();
    me->CastSpell(pTarget, pSpellInfo, bTriggered);
}

void ScriptedAI::DoPlaySoundToSet(WorldObject* pSource, uint32 uiSoundId)
{
    if (!pSource)
        return;

    if (!GetSoundEntriesStore()->LookupEntry(uiSoundId))
    {
        error_log("OSCR: Invalid soundId %u used in DoPlaySoundToSet (Source: TypeId %u, GUID %u)", uiSoundId, pSource->GetTypeId(), pSource->GetGUIDLow());
        return;
    }

    pSource->PlayDirectSound(uiSoundId);
}

Creature* ScriptedAI::DoSpawnCreature(uint32 uiId, float fX, float fY, float fZ, float fAngle, uint32 uiType, uint32 uiDespawntime)
{
    return me->SummonCreature(uiId, me->GetPositionX() + fX, me->GetPositionY() + fY, me->GetPositionZ() + fZ, fAngle, (TempSummonType)uiType, uiDespawntime);
}

Unit* ScriptedAI::SelectUnit(SelectAggroTarget pTarget, uint32 uiPosition)
{
    ThreatContainer::StorageType threatList = me->getThreatManager().getThreatList();
    ThreatContainer::StorageType::const_iterator itr = threatList.begin();
    ThreatContainer::StorageType::reverse_iterator ritr = threatList.rbegin();

    if (uiPosition >= threatList.size() || !threatList.size())
        return NULL;

    switch (pTarget)
    {
    case SELECT_TARGET_RANDOM:
        advance (itr , uiPosition +  (rand() % (threatList.size() - uiPosition)));
        return Unit::GetUnit((*me), (*itr)->getUnitGuid());
        break;

    case SELECT_TARGET_TOPAGGRO:
        advance (itr , uiPosition);
        return Unit::GetUnit((*me), (*itr)->getUnitGuid());
        break;

    case SELECT_TARGET_BOTTOMAGGRO:
        advance (ritr , uiPosition);
        return Unit::GetUnit((*me), (*ritr)->getUnitGuid());
        break;

    default:
        return UnitAI::SelectTarget(pTarget, uiPosition);
    }
}

SpellEntry const* ScriptedAI::SelectSpell(Unit* pTarget, uint32 uiSchool, uint32 uiMechanic, SelectTargetType selectTargets, uint32 uiPowerCostMin, uint32 uiPowerCostMax, float fRangeMin, float fRangeMax, SelectEffect selectEffects)
{
    //No target so we can't cast
    if (!pTarget)
        return NULL;

    //Silenced so we can't cast
    if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED))
        return NULL;

    //Using the extended script system we first create a list of viable spells
    SpellEntry const* apSpell[CREATURE_MAX_SPELLS];
    memset(apSpell, 0, sizeof(SpellEntry*)*CREATURE_MAX_SPELLS);

    uint32 uiSpellCount = 0;

    SpellEntry const* pTempSpell;
    SpellRangeEntry const* pTempRange;

    //Check if each spell is viable(set it to null if not)
    for (uint32 i = 0; i < CREATURE_MAX_SPELLS; i++)
    {
        pTempSpell = GetSpellStore()->LookupEntry(me->m_spells[i]);

        //This spell doesn't exist
        if (!pTempSpell)
            continue;

        // Targets and Effects checked first as most used restrictions
        //Check the spell targets if specified
        if (selectTargets && !(SpellSummary[me->m_spells[i]].Targets & (1 << (selectTargets - 1))))
            continue;

        //Check the type of spell if we are looking for a specific spell type
        if (selectEffects && !(SpellSummary[me->m_spells[i]].Effects & (1 << (selectEffects - 1))))
            continue;

        //Check for school if specified
        if (uiSchool && (pTempSpell->SchoolMask & uiSchool) == 0)
            continue;

        //Check for spell mechanic if specified
        if (uiMechanic && pTempSpell->Mechanic != uiMechanic)
            continue;

        //Make sure that the spell uses the requested amount of power
        if (uiPowerCostMin && pTempSpell->manaCost < uiPowerCostMin)
            continue;

        if (uiPowerCostMax && pTempSpell->manaCost > uiPowerCostMax)
            continue;

        //Continue if we don't have the mana to actually cast this spell
        if (pTempSpell->manaCost > me->GetPower((Powers)pTempSpell->powerType))
            continue;

        //Get the Range
        pTempRange = GetSpellRangeStore()->LookupEntry(pTempSpell->rangeIndex);

        //Spell has invalid range store so we can't use it
        if (!pTempRange)
            continue;

        //Check if the spell meets our range requirements
        if (fRangeMin && pTempRange->maxRange < fRangeMin)
            continue;
        if (fRangeMax && pTempRange->maxRange > fRangeMax)
            continue;

        //Check if our target is in range
        if (me->IsWithinDistInMap(pTarget, pTempRange->minRange) || !me->IsWithinDistInMap(pTarget, pTempRange->maxRange))
            continue;

        //All good so lets add it to the spell list
        apSpell[uiSpellCount] = pTempSpell;
        ++uiSpellCount;
    }

    //We got our usable spells so now lets randomly pick one
    if (!uiSpellCount)
        return NULL;

    return apSpell[rand() % uiSpellCount];
}

bool ScriptedAI::CanCast(Unit* pTarget, SpellEntry const* pSpell, bool bTriggered)
{
    //No target so we can't cast
    if (!pTarget || !pSpell)
        return false;

    //Silenced so we can't cast
    if (!bTriggered && me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED))
        return false;

    //Check for power
    if (!bTriggered && me->GetPower((Powers)pSpell->powerType) < pSpell->manaCost)
        return false;

    SpellRangeEntry const* pTempRange = GetSpellRangeStore()->LookupEntry(pSpell->rangeIndex);

    //Spell has invalid range store so we can't use it
    if (!pTempRange)
        return false;

    //Unit is out of range of this spell
    if (me->IsInRange(pTarget, pTempRange->minRange, pTempRange->maxRange))
        return false;

    return true;
}

void FillSpellSummary()
{
    SpellSummary = new TSpellSummary[GetSpellStore()->GetNumRows()];

    SpellEntry const* pTempSpell;

    for (uint32 i = 0; i < GetSpellStore()->GetNumRows(); ++i)
    {
        SpellSummary[i].Effects = 0;
        SpellSummary[i].Targets = 0;

        pTempSpell = GetSpellStore()->LookupEntry(i);
        //This spell doesn't exist
        if (!pTempSpell)
            continue;

        for (uint32 j = 0; j < 3; ++j)
        {
            //Spell targets self
            if (pTempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_CASTER)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_SELF - 1);

            //Spell targets a single enemy
            if (pTempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_TARGET_ENEMY ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_DST_TARGET_ENEMY)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_SINGLE_ENEMY - 1);

            //Spell targets AoE at enemy
            if (pTempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_AREA_ENEMY_SRC ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_AREA_ENEMY_DST ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_SRC_CASTER ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_DEST_DYNOBJ_ENEMY)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_AOE_ENEMY - 1);

            //Spell targets an enemy
            if (pTempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_TARGET_ENEMY ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_DST_TARGET_ENEMY ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_AREA_ENEMY_SRC ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_AREA_ENEMY_DST ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_SRC_CASTER ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_DEST_DYNOBJ_ENEMY)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_ANY_ENEMY - 1);

            //Spell targets a single friend(or self)
            if (pTempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_CASTER ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_TARGET_ALLY ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_TARGET_PARTY)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_SINGLE_FRIEND - 1);

            //Spell targets aoe friends
            if (pTempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_PARTY_CASTER ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_PARTY_TARGET ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_SRC_CASTER)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_AOE_FRIEND - 1);

            //Spell targets any friend(or self)
            if (pTempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_CASTER ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_TARGET_ALLY ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_TARGET_PARTY ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_PARTY_CASTER ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_PARTY_TARGET ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_SRC_CASTER)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_ANY_FRIEND - 1);

            //Make sure that this spell includes a damage effect
            if (pTempSpell->Effect[j] == SPELL_EFFECT_SCHOOL_DAMAGE ||
                pTempSpell->Effect[j] == SPELL_EFFECT_INSTAKILL ||
                pTempSpell->Effect[j] == SPELL_EFFECT_ENVIRONMENTAL_DAMAGE ||
                pTempSpell->Effect[j] == SPELL_EFFECT_HEALTH_LEECH)
                SpellSummary[i].Effects |= 1 << (SELECT_EFFECT_DAMAGE - 1);

            //Make sure that this spell includes a healing effect (or an apply aura with a periodic heal)
            if (pTempSpell->Effect[j] == SPELL_EFFECT_HEAL ||
                pTempSpell->Effect[j] == SPELL_EFFECT_HEAL_MAX_HEALTH ||
                pTempSpell->Effect[j] == SPELL_EFFECT_HEAL_MECHANICAL ||
                (pTempSpell->Effect[j] == SPELL_EFFECT_APPLY_AURA  && pTempSpell->EffectApplyAuraName[j] == 8))
                SpellSummary[i].Effects |= 1 << (SELECT_EFFECT_HEALING - 1);

            //Make sure that this spell applies an aura
            if (pTempSpell->Effect[j] == SPELL_EFFECT_APPLY_AURA)
                SpellSummary[i].Effects |= 1 << (SELECT_EFFECT_AURA - 1);
        }
    }
}

void ScriptedAI::DoResetThreat()
{
    if (!me->CanHaveThreatList() || me->getThreatManager().isThreatListEmpty())
    {
        error_log("OSCR: DoResetThreat called for creature that either cannot have threat list or has empty threat list (me entry = %d)", me->GetEntry());
        return;
    }

    ThreatContainer::StorageType threatlist = me->getThreatManager().getThreatList();

    for (ThreatContainer::StorageType::const_iterator itr = threatlist.begin(); itr != threatlist.end(); ++itr)
    {
        Unit* pUnit = Unit::GetUnit((*me), (*itr)->getUnitGuid());

        if (pUnit && DoGetThreat(pUnit))
            DoModifyThreatPercent(pUnit, -100);
    }
}

float ScriptedAI::DoGetThreat(Unit* pUnit)
{
    if (!pUnit) return 0.0f;
    return me->getThreatManager().getThreat(pUnit);
}

void ScriptedAI::DoModifyThreatPercent(Unit* pUnit, int32 pct)
{
    if (!pUnit) return;
    me->getThreatManager().modifyThreatPercent(pUnit, pct);
}

void ScriptedAI::DoTeleportTo(float fX, float fY, float fZ, uint32 uiTime)
{
    me->Relocate(fX, fY, fZ);
    Movement::MoveSplineInit init(*me);
    init.MoveTo(fX, fY, fZ, true);
    int32 duration = init.Launch();
    if (duration != uiTime)
        sLog.outError("Scripted Creature - DoTeleportTo : Travel time and duration from spline don't match (travel time : %u, duration : %i). Object (TypeId: %u, Entry: %u, GUID: %u)",
        uiTime, duration,
        me->GetTypeId(), me->GetEntry(), me->GetGUIDLow());
}

void ScriptedAI::DoTeleportTo(const float fPos[4])
{
    me->NearTeleportTo(fPos[0], fPos[1], fPos[2], fPos[3]);
}

void ScriptedAI::DoTeleportPlayer(Unit* pUnit, float fX, float fY, float fZ, float fO)
{
    if (!pUnit || pUnit->GetTypeId() != TYPEID_PLAYER)
    {
        if (pUnit)
            error_log("OSCR: Creature " UI64FMTD " (Entry: %u) Tried to teleport non-player unit (Type: %u GUID: " UI64FMTD ") to x: %f y:%f z: %f o: %f. Aborted.", me->GetGUID(), me->GetEntry(), pUnit->GetTypeId(), pUnit->GetGUID(), fX, fY, fZ, fO);
        return;
    }

    CAST_PLR(pUnit)->TeleportTo(pUnit->GetMapId(), fX, fY, fZ, fO, TELE_TO_NOT_LEAVE_COMBAT);
}

void ScriptedAI::DoTeleportAll(float fX, float fY, float fZ, float fO)
{
    Map* map = me->GetMap();
    if (!map->IsDungeon())
        return;

    Map::PlayerList const& PlayerList = map->GetPlayers();
    for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
        if (Player* i_pl = i->GetSource())
            if (i_pl->IsAlive())
                i_pl->TeleportTo(me->GetMapId(), fX, fY, fZ, fO, TELE_TO_NOT_LEAVE_COMBAT);
}

Unit* ScriptedAI::DoSelectLowestHpFriendly(float fRange, uint32 uiMinHPDiff)
{
    Unit* pUnit = NULL;
    Oregon::MostHPMissingInRange u_check(me, fRange, uiMinHPDiff);
    Oregon::UnitLastSearcher<Oregon::MostHPMissingInRange> searcher(pUnit, u_check);
    me->VisitNearbyObject(fRange, searcher);

    return pUnit;
}

std::list<Creature*> ScriptedAI::DoFindFriendlyCC(float fRange)
{
    std::list<Creature*> pList;
    Oregon::FriendlyCCedInRange u_check(me, fRange);
    Oregon::CreatureListSearcher<Oregon::FriendlyCCedInRange> searcher(pList, u_check);
    me->VisitNearbyObject(fRange, searcher);
    return pList;
}

std::list<Creature*> ScriptedAI::DoFindFriendlyMissingBuff(float fRange, uint32 uiSpellid)
{
    std::list<Creature*> pList;
    Oregon::FriendlyMissingBuffInRange u_check(me, fRange, uiSpellid);
    Oregon::CreatureListSearcher<Oregon::FriendlyMissingBuffInRange> searcher(pList, u_check);
    me->VisitNearbyObject(fRange, searcher);
    return pList;
}

Player* ScriptedAI::GetPlayerAtMinimumRange(float minimumRange)
{
    Player* pPlayer = NULL;

    CellCoord pair(Oregon::ComputeCellCoord(me->GetPositionX(), me->GetPositionY()));
    Cell cell(pair);
    cell.SetNoCreate();

    Oregon::PlayerAtMinimumRangeAway check(me, minimumRange);
    Oregon::PlayerSearcher<Oregon::PlayerAtMinimumRangeAway> searcher(pPlayer, check);
    TypeContainerVisitor<Oregon::PlayerSearcher<Oregon::PlayerAtMinimumRangeAway>, GridTypeMapContainer> visitor(searcher);

    cell.Visit(pair, visitor, *me->GetMap(), *me, minimumRange);

    return pPlayer;
}

void ScriptedAI::SetEquipmentSlots(bool bLoadDefault, int32 uiMainHand, int32 uiOffHand, int32 uiRanged)
{
    if (bLoadDefault)
    {
        if (CreatureInfo const* pInfo = GetCreatureTemplateStore(me->GetEntry()))
            me->LoadEquipment(pInfo->equipmentId, true);

        return;
    }

    if (uiMainHand >= 0)
        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY + 0, uint32(uiMainHand));

    if (uiOffHand >= 0)
        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY + 1, uint32(uiOffHand));

    if (uiRanged >= 0)
        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY + 2, uint32(uiRanged));
}

void ScriptedAI::SetCombatMovement(bool bCombatMove)
{
    m_bCombatMovement = bCombatMove;
}

enum eNPCs
{
    NPC_BROODLORD   = 12017,
    NPC_VOID_REAVER = 19516,
    NPC_JAN_ALAI    = 23578,
    NPC_SARTHARION  = 28860
};

// Hacklike storage used for misc creatures that are expected to evade of outside of a certain area.
// It is assumed the information is found elswehere and can be handled by the core. So far no luck finding such information/way to extract it.
bool ScriptedAI::EnterEvadeIfOutOfCombatArea(const uint32 uiDiff)
{
    if (m_uiEvadeCheckCooldown <= uiDiff)
        m_uiEvadeCheckCooldown = 2500;
    else
    {
        m_uiEvadeCheckCooldown -= uiDiff;
        return false;
    }

    if (me->IsInEvadeMode() || !me->GetVictim())
        return false;

    float fX = me->GetPositionX();
    float fY = me->GetPositionY();
    float fZ = me->GetPositionZ();

    switch (me->GetEntry())
    {
    case NPC_BROODLORD:                                         // broodlord (not move down stairs)
        if (fZ > 448.60f)
            return false;
        break;
    case NPC_VOID_REAVER:                                         // void reaver (calculate from center of room)
        if (me->GetDistance2d(432.59f, 371.93f) < 105.0f)
            return false;
        break;
    case NPC_JAN_ALAI:                                         // jan'alai (calculate by Z)
        if (fZ > 12.0f)
            return false;
        break;
    case NPC_SARTHARION:                                         // sartharion (calculate box)
        if (fX > 3218.86f && fX < 3275.69f && fY < 572.40f && fY > 484.68f)
            return false;
        break;
    default:
        error_log("OSCR: EnterEvadeIfOutOfCombatArea used for creature entry %u, but does not have any definition.", me->GetEntry());
        return false;
    }

    EnterEvadeMode();
    return true;
}

void Scripted_NoMovementAI::AttackStart(Unit* pWho)
{
    if (!pWho)
        return;

    if (me->Attack(pWho, true))
        DoStartNoMovement(pWho);
}

BossAI::BossAI(Creature* c, uint32 id) : ScriptedAI(c)
    , bossId(id), summons(me), instance(c->GetInstanceData())
{
}

void BossAI::_Reset()
{
    if (!me->IsAlive())
        return;

    me->SetCombatPulseDelay(0);
    events.Reset();
    summons.DespawnAll();
    if (instance)
        instance->SetBossState(bossId, NOT_STARTED);
}

void BossAI::_JustDied()
{
    events.Reset();
    summons.DespawnAll();
    if (instance)
    {
        instance->SetBossState(bossId, DONE);
        instance->SaveToDB();
    }
}

void BossAI::_EnterCombat()
{
    me->SetCombatPulseDelay(5);
    me->setActive(true);
    DoZoneInCombat();
    if (instance)
        instance->SetBossState(bossId, IN_PROGRESS);
}

void BossAI::JustSummoned(Creature* summon)
{
    summons.Summon(summon);
    if (me->IsInCombat())
        DoZoneInCombat(summon);
}

void BossAI::SummonedCreatureDespawn(Creature* summon)
{
    summons.Despawn(summon);
}

#define GOBJECT(x) (const_cast<GameObjectInfo*>(GetGameObjectInfo(x)))

void LoadOverridenSQLData()
{
    GameObjectInfo* goInfo;

    // Sunwell Plateau : Kalecgos : Spectral Rift
    goInfo = GOBJECT(187055);
    if (goInfo)
        if (goInfo->type == GAMEOBJECT_TYPE_GOOBER)
            goInfo->type = GAMEOBJECT_TYPE_SPELLCASTER;

    // Naxxramas : Sapphiron Birth
    goInfo = GOBJECT(181356);
    if (goInfo)
        if (goInfo->type == GAMEOBJECT_TYPE_TRAP)
            goInfo->trap.radius = 50;
}

Player* ScriptedAI::SelectTargetFromPlayerList(float maxdist, uint32 excludeAura, bool mustBeInLOS) const
{
    Map::PlayerList const& pList = me->GetMap()->GetPlayers();
    std::vector<Player*> tList;
    for (Map::PlayerList::const_iterator itr = pList.begin(); itr != pList.end(); ++itr)
    {
        if (me->GetDistance(itr->GetSource()) > maxdist || !itr->GetSource()->IsAlive() || itr->GetSource()->IsGameMaster())
            continue;
        if (excludeAura && itr->GetSource()->HasAura(excludeAura))
            continue;
        if (mustBeInLOS && !me->IsWithinLOSInMap(itr->GetSource()))
            continue;
        tList.push_back(itr->GetSource());
    }
    if (!tList.empty())
        return tList[urand(0, tList.size() - 1)];
    else
        return nullptr;
}

// SD2 grid searchers.
Creature* GetClosestCreatureWithEntry(WorldObject* pSource, uint32 uiEntry, float fMaxSearchRange, bool bAlive)
{
    return pSource->FindNearestCreature(uiEntry, fMaxSearchRange, bAlive);
}
GameObject* GetClosestGameObjectWithEntry(WorldObject* pSource, uint32 uiEntry, float fMaxSearchRange)
{
    return pSource->FindNearestGameObject(uiEntry, fMaxSearchRange);
}
void GetCreatureListWithEntryInGrid(std::list<Creature*>& lList, WorldObject* pSource, uint32 uiEntry, float fMaxSearchRange)
{
    return pSource->GetCreatureListWithEntryInGrid(lList, uiEntry, fMaxSearchRange);
}
void GetGameObjectListWithEntryInGrid(std::list<GameObject*>& lList, WorldObject* pSource, uint32 uiEntry, float fMaxSearchRange)
{
    return pSource->GetGameObjectListWithEntryInGrid(lList, uiEntry, fMaxSearchRange);
}

