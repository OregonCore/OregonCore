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

#include "UnitAI.h"
#include "Player.h"
#include "Creature.h"
#include "SpellAuras.h"
#include "SpellMgr.h"
#include "CreatureAIImpl.h"

void UnitAI::AttackStart(Unit* victim)
{
    if (victim && me->Attack(victim, true))
    {
        // Clear distracted state on attacking
        if (me->HasUnitState(UNIT_STATE_DISTRACTED))
        {
            me->ClearUnitState(UNIT_STATE_DISTRACTED);
            me->GetMotionMaster()->Clear();
        }
        me->GetMotionMaster()->MoveChase(victim);
    }
}

void UnitAI::AttackStartCaster(Unit* victim, float dist)
{
    if (victim && me->Attack(victim, false))
        me->GetMotionMaster()->MoveChase(victim, dist);
}

void UnitAI::DoMeleeAttackIfReady()
{
    if (me->HasUnitState(UNIT_STATE_CASTING))
        return;

    //Make sure our attack is ready and we aren't currently casting before checking distance
    if (me->isAttackReady())
    {
        //Prevent base and off attack at the same time
        if (me->haveOffhandWeapon())
            if (me->getAttackTimer(OFF_ATTACK) < ATTACK_DISPLAY_DELAY)
                me->setAttackTimer(OFF_ATTACK, ATTACK_DISPLAY_DELAY);

        //If we are within range melee the target
        if (me->IsWithinMeleeRange(me->GetVictim()))
        {
            me->AttackerStateUpdate(me->GetVictim());
            me->resetAttackTimer();
        }
    }
    if (me->haveOffhandWeapon() && me->isAttackReady(OFF_ATTACK))
    {
        if (me->getAttackTimer(BASE_ATTACK) < ATTACK_DISPLAY_DELAY)
            me->setAttackTimer(BASE_ATTACK, ATTACK_DISPLAY_DELAY);

        //If we are within range melee the target
        if (me->IsWithinMeleeRange(me->GetVictim()))
        {
            me->AttackerStateUpdate(me->GetVictim(), OFF_ATTACK);
            me->resetAttackTimer(OFF_ATTACK);
        }
    }
}

bool UnitAI::DoSpellAttackIfReady(uint32 spell)
{
    if (me->HasUnitState(UNIT_STATE_CASTING))
        return true;

    if (me->isAttackReady())
    {
        const SpellEntry* spellInfo = GetSpellStore()->LookupEntry(spell);
        if (me->IsWithinCombatRange(me->GetVictim(), GetSpellMaxRange(spellInfo)))
        {
            me->CastSpell(me->GetVictim(), spell, false);
            me->resetAttackTimer();
        }
        else
            return false;
    }
    return true;
}

inline bool SelectTargetHelper(const Unit* me, const Unit* target, const bool& playerOnly, const float& dist, const int32& aura)
{
    if (playerOnly && (!target || target->GetTypeId() != TYPEID_PLAYER))
        return false;

    if (dist && (!me || !target || !me->IsWithinCombatRange(target, dist)))
        return false;

    if (aura)
    {
        if (aura > 0)
        {
            if (!target->HasAura(aura, 0))
                return false;
        }
        else
        {
            if (target->HasAura(aura, 0))
                return false;
        }
    }

    return true;
}

struct TargetDistanceOrder : public std::binary_function<const Unit*, const Unit*, bool>
{
    const Unit* me;
    TargetDistanceOrder(const Unit* Target) : me(Target) {};
    // functor for operator ">"
    bool operator()(const Unit* _Left, const Unit* _Right) const
    {
        return (me->GetExactDistSq(_Left) < me->GetExactDistSq(_Right));
    }
};

Unit* UnitAI::SelectTarget(SelectAggroTarget targetType, uint32 position, float dist, bool playerOnly, int32 aura)
{
    const std::list<HostileReference*>& threatlist = me->getThreatManager().getThreatList();
    std::list<Unit*> targetList;

    for (std::list<HostileReference*>::const_iterator itr = threatlist.begin(); itr != threatlist.end(); ++itr)
        if (SelectTargetHelper(me, (*itr)->getTarget(), playerOnly, dist, aura))
            targetList.push_back((*itr)->getTarget());

    if (position >= targetList.size())
        return NULL;

    if (targetType == SELECT_TARGET_NEAREST || targetType == SELECT_TARGET_FARTHEST)
        targetList.sort(TargetDistanceOrder(me));

    switch (targetType)
    {
    case SELECT_TARGET_NEAREST:
    case SELECT_TARGET_TOPAGGRO:
        {
            std::list<Unit*>::iterator itr = targetList.begin();
            advance(itr, position);
            return *itr;
        }
        break;

    case SELECT_TARGET_FARTHEST:
    case SELECT_TARGET_BOTTOMAGGRO:
        {
            std::list<Unit*>::reverse_iterator ritr = targetList.rbegin();
            advance(ritr, position);
            return *ritr;
        }
        break;

    case SELECT_TARGET_RANDOM:
        return SelectRandomContainerElement(targetList);
        break;
    }

    return NULL;
}

void UnitAI::SelectTargetList(std::list<Unit*>& targetList, uint32 num, SelectAggroTarget targetType, float dist, bool playerOnly, int32 aura)
{
    const std::list<HostileReference*>& threatlist = me->getThreatManager().getThreatList();
    if (threatlist.empty())
        return;

    for (std::list<HostileReference*>::const_iterator itr = threatlist.begin(); itr != threatlist.end(); ++itr)
        if (SelectTargetHelper(me, (*itr)->getTarget(), playerOnly, dist, aura))
            targetList.push_back((*itr)->getTarget());

    if (targetType == SELECT_TARGET_NEAREST || targetType == SELECT_TARGET_FARTHEST)
        targetList.sort(TargetDistanceOrder(me));

    if (targetType == SELECT_TARGET_FARTHEST || targetType == SELECT_TARGET_BOTTOMAGGRO)
        targetList.reverse();

    if (targetType == SELECT_TARGET_RANDOM)
    {
        while (num < targetList.size())
        {
            std::list<Unit*>::iterator itr = targetList.begin();
            advance(itr, urand(0, targetList.size() - 1));
            targetList.erase(itr);
        }
    }
    else
        targetList.resize(num);
}

float UnitAI::DoGetSpellMaxRange(uint32 spellId, bool /*positive*/)
{
    return GetSpellMaxRange(spellId);
}

void UnitAI::DoCast(uint32 spellId)
{
    Unit* target = NULL;
    //sLog.outError("aggre %u %u", spellId, (uint32)AISpellInfo[spellId].target);
    switch (AISpellInfo[spellId].target)
    {
    default:
    case AITARGET_SELF:
        target = me;
        break;
    case AITARGET_VICTIM:
        target = me->GetVictim();
        break;
    case AITARGET_ENEMY:
        {
            const SpellEntry* spellInfo = GetSpellStore()->LookupEntry(spellId);
            bool playerOnly = spellInfo->AttributesEx3 & SPELL_ATTR3_PLAYERS_ONLY;
            target = SelectTarget(SELECT_TARGET_RANDOM, 0, GetSpellMaxRange(spellInfo), playerOnly);
            break;
        }
    case AITARGET_ALLY:
        target = me;
        break;
    case AITARGET_BUFF:
        target = me;
        break;
    case AITARGET_DEBUFF:
        {
            const SpellEntry* spellInfo = GetSpellStore()->LookupEntry(spellId);
            bool playerOnly = spellInfo->AttributesEx3 & SPELL_ATTR3_PLAYERS_ONLY;
            float range = GetSpellMaxRange(spellInfo);
            if (!(spellInfo->Attributes & SPELL_ATTR0_HEARTBEAT_RESIST_CHECK)
                && !(spellInfo->AuraInterruptFlags & AURA_INTERRUPT_FLAG_NOT_VICTIM)
                && SelectTargetHelper(me, me->GetVictim(), playerOnly, range, -(int32)spellId))
                target = me->GetVictim();
            else
                target = SelectTarget(SELECT_TARGET_RANDOM, 0, range, playerOnly, -(int32)spellId);
            break;
        }
    }

    if (target)
        me->CastSpell(target, spellId, false);
}

void UnitAI::DoCast(Unit* victim, uint32 spellId, bool triggered)
{
    if (!victim || (me->HasUnitState(UNIT_STATE_CASTING) && !triggered))
        return;

    me->CastSpell(victim, spellId, triggered);
}

void UnitAI::DoCastVictim(uint32 spellId, bool triggered)
{
    // Why don't we check for casting unit_state and existing target as we do in DoCast(.. ?
    DoCast(me->GetVictim(), spellId, triggered);
}

void UnitAI::DoCastAOE(uint32 spellId, bool triggered)
{
    if (!triggered && me->HasUnitState(UNIT_STATE_CASTING))
        return;

    me->CastSpell((Unit*)NULL, spellId, triggered);
}

#define UPDATE_TARGET(a) {if (AIInfo->target<a) AIInfo->target=a;}

void UnitAI::FillAISpellInfo()
{
    AISpellInfo = new AISpellInfoType[GetSpellStore()->GetNumRows()];

    AISpellInfoType* AIInfo = AISpellInfo;
    const SpellEntry* spellInfo;

    for (uint32 i = 0; i < GetSpellStore()->GetNumRows(); ++i, ++AIInfo)
    {
        spellInfo = GetSpellStore()->LookupEntry(i);
        if (!spellInfo)
            continue;

        if (spellInfo->Attributes & SPELL_ATTR0_CASTABLE_WHILE_DEAD)
            AIInfo->condition = AICOND_DIE;
        else if (IsPassiveSpell(i) || GetSpellDuration(spellInfo) == -1)
            AIInfo->condition = AICOND_AGGRO;
        else
            AIInfo->condition = AICOND_COMBAT;

        if (AIInfo->cooldown < spellInfo->RecoveryTime)
            AIInfo->cooldown = spellInfo->RecoveryTime;

        if (!GetSpellMaxRange(spellInfo))
            UPDATE_TARGET(AITARGET_SELF)
            else
            {
                for (uint32 j = 0; j < 3; ++j)
                {
                    uint32 targetType = spellInfo->EffectImplicitTargetA[j];

                    if (targetType == TARGET_UNIT_TARGET_ENEMY
                        || targetType == TARGET_DST_TARGET_ENEMY)
                        UPDATE_TARGET(AITARGET_VICTIM)
                        else if (targetType == TARGET_UNIT_AREA_ENEMY_DST)
                            UPDATE_TARGET(AITARGET_ENEMY)

                            if (spellInfo->Effect[j] == SPELL_EFFECT_APPLY_AURA)
                            {
                                if (targetType == TARGET_UNIT_TARGET_ENEMY)
                                    UPDATE_TARGET(AITARGET_DEBUFF)
                                    else if (IsPositiveSpell(i))
                                        UPDATE_TARGET(AITARGET_BUFF)
                                    }
                }
            }
        AIInfo->realCooldown = spellInfo->RecoveryTime + spellInfo->StartRecoveryTime;
        SpellRangeEntry const* srange = sSpellRangeStore.LookupEntry(spellInfo->rangeIndex);
        if (srange)
            AIInfo->maxRange = srange->maxRange * 3 / 4;
    }
}

void UnitAI::ClearAISpellInfo()
{
    delete[] AISpellInfo;
    AISpellInfo = NULL;
}

//Enable PlayerAI when charmed
void PlayerAI::OnCharmed(bool apply)
{
    me->IsAIEnabled = apply;
}

void SimpleCharmedAI::UpdateAI(const uint32 /*diff*/)
{
    Creature* charmer = me->GetCharmer()->ToCreature();

    //kill self if charm aura has infinite duration
    if (charmer->IsInEvadeMode())
    {
        Unit::AuraList const& auras = me->GetAurasByType(SPELL_AURA_MOD_CHARM);
        for (Unit::AuraList::const_iterator iter = auras.begin(); iter != auras.end(); ++iter)
            if ((*iter)->GetCasterGUID() == charmer->GetGUID() && (*iter)->IsPermanent())
            {
                charmer->Kill(me);
                return;
            }
    }

    if (!charmer->IsInCombat())
        me->GetMotionMaster()->MoveFollow(charmer, PET_FOLLOW_DIST, me->GetFollowAngle());

    Unit* target = me->GetVictim();
    if (!target || !charmer->IsValidAttackTarget(target))
        AttackStart(charmer->SelectNearestTargetInAttackDistance());
}

