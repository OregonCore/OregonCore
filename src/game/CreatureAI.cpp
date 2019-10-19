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

#include "CreatureAI.h"
#include "CreatureAIImpl.h"
#include "Creature.h"
#include "World.h"
#include "SpellMgr.h"
#include "MapRefManager.h"
#include "Player.h"

//Disable CreatureAI when charmed
void CreatureAI::OnCharmed(bool apply)
{
    me->NeedChangeAI = true;

    if(!(me->ToCreature()->GetCreatureTemplate()->flags_extra & CREATURE_FLAG_EXTRA_CHARM_AI))
        me->IsAIEnabled = !apply;
}

AISpellInfoType* UnitAI::AISpellInfo;
AISpellInfoType* GetAISpellInfo(uint32 i)
{
    return &CreatureAI::AISpellInfo[i];
}

void CreatureAI::DoZoneInCombatWithPlayers(bool force)
{
    Map* map = me->GetMap();

    if (!map->IsDungeon())
    {
        sLog.outError("CreatureAI::DoZoneInCombatWithPlayers called on a map that is not an instance (creature entry = %u)", me->GetEntry());
        return;
    }

    if (!force)
    {
        if (!me->CanHaveThreatList() || me->getThreatManager().isThreatListEmpty())
        {
            error_log("CreatureAI::DoZoneInCombatWithPlayers called for a creature that either cannot have a threat list or has empty threat list (creature entry = %u)", me->GetEntry());
            return;
        }
    }

    Map::PlayerList const &PlayerList = map->GetPlayers();
    for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
    {
        if (Player* pPlayer = i->GetSource())
        {
            if (pPlayer->IsGameMaster())
                continue;

            if (pPlayer->IsAlive())
            {
                me->SetInCombatWith(pPlayer);
                pPlayer->SetInCombatWith(me);
                me->AddThreat(pPlayer, 0.0f);
            }
        }
    }
}

void CreatureAI::DoZoneInCombat(Creature* creature /*= NULL*/, float maxRangeToNearestTarget /* = 50.0f*/)
{
    if (!creature)
        creature = me;

    if (!creature->CanHaveThreatList())
        return;

    Map* map = creature->GetMap();
    if (!map->IsDungeon())                                  //use IsDungeon instead of Instanceable, in case battlegrounds will be instantiated
    {
        sLog.outError("DoZoneInCombat call for map that isn't an instance (creature entry = %d)", creature->GetTypeId() == TYPEID_UNIT ? creature->ToCreature()->GetEntry() : 0);
        return;
    }

    if (!creature->HasReactState(REACT_PASSIVE) && !creature->GetVictim())
    {
        if (Unit* nearTarget = creature->SelectNearestTarget(maxRangeToNearestTarget))
            creature->AI()->AttackStart(nearTarget);
        else if (creature->IsSummon())
        {
            if (Unit* summoner = ((TempSummon*)creature)->GetSummoner())
            {
                Unit* target = summoner->getAttackerForHelper();
                if (!target && summoner->CanHaveThreatList() && !summoner->getThreatManager().isThreatListEmpty())
                    target = summoner->getThreatManager().getHostileTarget();
                if (target && (creature->IsFriendlyTo(summoner) || creature->IsHostileTo(target)))
                    creature->AI()->AttackStart(target);
            }
        }
    }

    // Intended duplicated check, the code above this should select a victim
    // If it can't find a suitable attack target then we should error out.
    if (!creature->HasReactState(REACT_PASSIVE) && !creature->GetVictim())
    {
        sLog.outError("DoZoneInCombat called for creature that has empty threat list (creature entry = %u)", creature->GetEntry());
        return;
    }

    Map::PlayerList const& PlList = map->GetPlayers();

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
                creature->SetInCombatWith(pPlayer);
                pPlayer->SetInCombatWith(creature);
                creature->AddThreat(pPlayer, 0.0f);
            }

            /* Causes certain things to never leave the threat list (Priest Lightwell, etc):
            for (Unit::ControlList::const_iterator itr = pPlayer->m_Controlled.begin(); itr != pPlayer->m_Controlled.end(); ++itr)
            {
                creature->SetInCombatWith(*itr);
                (*itr)->SetInCombatWith(creature);
                creature->AddThreat(*itr, 0.0f);
            }*/
        }
    }
}

// scripts does not take care about MoveInLineOfSight loops
// MoveInLineOfSight can be called inside another MoveInLineOfSight and cause stack overflow
void CreatureAI::MoveInLineOfSight_Safe(Unit* who)
{
    if (m_MoveInLineOfSight_locked == true)
        return;
    m_MoveInLineOfSight_locked = true;
    MoveInLineOfSight(who);
    m_MoveInLineOfSight_locked = false;
}

void CreatureAI::MoveInLineOfSight(Unit* who)
{
    if (me->GetVictim())
        return;
    
    if (me->GetCreatureType() == CREATURE_TYPE_NON_COMBAT_PET) // non-combat pets should just stand there and look good;)
        return;

    if (me->canStartAttack(who, false))
        AttackStart(who);
}

// Distract creature, if player gets too close while stealthed/prowling
void CreatureAI::TriggerAlert(Unit const* who) const
{
    // If there's no target, or target isn't a player do nothing
    if (!who || who->GetTypeId() != TYPEID_PLAYER)
        return;

    // If this unit isn't an NPC, is already distracted, is in combat, is confused, stunned or fleeing, do nothing
    if (me->GetTypeId() != TYPEID_UNIT || me->IsInCombat() || me->HasUnitState(UNIT_STATE_CONFUSED | UNIT_STATE_STUNNED | UNIT_STATE_FLEEING | UNIT_STATE_DISTRACTED))
        return;

    // Only alert for hostiles!
    if (me->IsCivilian() || me->HasReactState(REACT_PASSIVE) || !me->IsHostileTo(who))
        return;

    // Send alert sound (if any) for this creature
    me->SendAIReaction(AI_REACTION_ALERT);

    // Face the unit (stealthed player) and set distracted state for 5 seconds
    me->SetFacingTo(me->GetAngle(who->GetPositionX(), who->GetPositionY()));
    me->StopMoving();
    me->GetMotionMaster()->MoveDistract(5 * IN_MILLISECONDS);
}

bool CreatureAI::UpdateVictimByReact()
{
    if (!me->IsInCombat())
        return false;

    if (me->HasReactState(REACT_AGGRESSIVE))
    {
        if (Unit* victim = me->SelectVictim())
            AttackStart(victim);
        return me->GetVictim();
    }
    else if (me->getThreatManager().isThreatListEmpty())
    {
        EnterEvadeMode();
        return false;
    }

    return true;
}

void CreatureAI::EnterEvadeMode()
{
    if (!_EnterEvadeMode())
        return;

    sLog.outDebug("Creature %u enters evade mode.", me->GetEntry());

    if (Unit* owner = me->GetCharmerOrOwner())
    {
        me->GetMotionMaster()->Clear(false);
        me->GetMotionMaster()->MoveFollow(owner, PET_FOLLOW_DIST, me->GetFollowAngle(), MOTION_SLOT_ACTIVE);
    }
    else
    {
        // Required to prevent attacking creatures that are evading and cause them to reenter combat
        // Does not apply to MoveFollow
        me->AddUnitState(UNIT_STATE_EVADE);
        me->GetMotionMaster()->MoveTargetedHome();
    }

    Reset();
}

/*void CreatureAI::AttackedBy(Unit* attacker)
{
    if (!me->GetVictim())
        AttackStart(attacker);
}*/


void CreatureAI::SetGazeOn(Unit* target)
{
    if (me->IsValidAttackTarget(target))
    {
        AttackStart(target);
        me->SetReactState(REACT_PASSIVE);
    }
}

bool CreatureAI::UpdateVictimWithGaze()
{
    if (!me->IsInCombat())
        return false;

    if (me->HasReactState(REACT_PASSIVE))
    {
        if (me->GetVictim())
            return true;
        else
            me->SetReactState(REACT_AGGRESSIVE);
    }

    if (Unit* victim = me->SelectVictim())
        AttackStart(victim);
    return me->GetVictim();
}

bool CreatureAI::UpdateVictim()
{
    if (!me->IsInCombat())
        return false;

    if (!me->HasReactState(REACT_PASSIVE))
    {
        if (Unit* victim = me->SelectVictim())
            AttackStart(victim);
        return me->GetVictim();
    }
    else if (me->getThreatManager().isThreatListEmpty())
    {
        EnterEvadeMode();
        return false;
    }

    return true;
}

bool CreatureAI::_EnterEvadeMode()
{
    if (!me->IsAlive())
        return false;

    // sometimes bosses stuck in combat?
    me->DeleteThreatList();
    me->CombatStop(true);
    me->CombatStopWithPets(true);
    me->SetLootRecipient(nullptr);
    me->SetPlayerDamaged(false);
    me->SetLastDamagedTime(0);

    if (me->IsInEvadeMode())
        return false;

    me->RemoveAllAuras();
    me->LoadCreaturesAddon();

    return true;
}

void CreatureAI::SetCombatMovement(bool enable)
{
    if (CombatMovementEnabled == enable)
        return;

    CombatMovementEnabled = enable;

    if (enable && me->GetVictim())
    {
        if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() == IDLE_MOTION_TYPE || ASSISTANCE_MOTION_TYPE)
        {
            me->GetMotionMaster()->MoveChase(me->GetVictim());
            me->CastStop();
        }
    }
}

Creature* CreatureAI::DoSummon(uint32 entry, const Position& pos, uint32 despawnTime, TempSummonType summonType)
{
    return me->SummonCreature(entry, pos, summonType, despawnTime);
}

Creature* CreatureAI::DoSummon(uint32 entry, WorldObject* obj, float radius, uint32 despawnTime, TempSummonType summonType)
{
    Position pos = obj->GetRandomNearPosition(radius);
    return me->SummonCreature(entry, pos, summonType, despawnTime);
}

Creature* CreatureAI::DoSummonFlyer(uint32 entry, WorldObject* obj, float flightZ, float radius, uint32 despawnTime, TempSummonType summonType)
{
    Position pos = obj->GetRandomNearPosition(radius);
    pos.m_positionZ += flightZ;
    return me->SummonCreature(entry, pos, summonType, despawnTime);
}
