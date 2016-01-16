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

#include "PetAI.h"
#include "Errors.h"
#include "Pet.h"
#include "Player.h"
#include "Spell.h"
#include "ObjectAccessor.h"
#include "SpellMgr.h"
#include "Creature.h"
#include "Util.h"

int PetAI::Permissible(const Creature* creature)
{
    if (creature->IsPet())
        return PERMIT_BASE_SPECIAL;

    return PERMIT_BASE_NO;
}

PetAI::PetAI(Creature* c) : CreatureAI(c), i_tracker(TIME_INTERVAL_LOOK)
{
    m_AllySet.clear();
    UpdateAllies();
}

void PetAI::EnterEvadeMode()
{
}

bool PetAI::_needToStop() const
{
    // This is needed for charmed creatures, as once their target was reset other effects can trigger threat
    if (me->isCharmed() && me->getVictim() == me->GetCharmer())
        return true;

    return !me->canAttack(me->getVictim());
}

void PetAI::_stopAttack()
{
    if (!me->IsAlive())
    {
        DEBUG_LOG("Creature stoped attacking because he is dead [guid=%u]", me->GetGUIDLow());
        me->GetMotionMaster()->Clear();
        me->GetMotionMaster()->MoveIdle();
        me->CombatStop();
        me->getHostileRefManager().deleteReferences();

        return;
    }

    me->AttackStop();
    me->InterruptNonMeleeSpells(false);
    me->SendMeleeAttackStop(); // Should stop pet's attack button from flashing
    me->GetCharmInfo()->SetIsCommandAttack(false);
    HandleReturnMovement();
}

void PetAI::UpdateAI(const uint32 diff)
{
    Unit* owner = me->GetCharmerOrOwner();

    if (m_updateAlliesTimer <= diff)
        // UpdateAllies self set update timer
        UpdateAllies();
    else
        m_updateAlliesTimer -= diff;

    // Must also check if victim is alive
    if (inCombat && (!me->getVictim() || (me->ToPet() && me->ToPet()->GetModeFlags() & PET_MODE_DISABLE_ACTIONS)))
        _stopAttack();

    // i_pet.getVictim() can't be used for check in case stop fighting, i_pet.getVictim() clear at Unit death etc.
    if (me->getVictim())
    {
        if (_needToStop())
        {
            DEBUG_LOG("Pet AI stoped attacking [guid=%u]", me->GetGUIDLow());
            _stopAttack();
            return;
        }

        DoMeleeAttackIfReady();
    }
    else if (owner && me->GetCharmInfo()) //no victim
    {
        if (Unit* nextTarget = SelectNextTarget())
            AttackStart(nextTarget);
        else
        {
            // Cancels the attack command so the pet stops chasing the
            // target it is attacking, and returns the pet to his owner
            me->GetCharmInfo()->SetIsCommandAttack(false);
            HandleReturnMovement();
        }
    }
    else if (owner && !me->HasUnitState(UNIT_STATE_FOLLOW)) // no charm info and no victim
        me->GetMotionMaster()->MoveFollow(owner, PET_FOLLOW_DIST, me->GetFollowAngle());

    if (!me->GetCharmInfo())
        return;

    // Autocast (casted only in combat or persistent spells in any state)
    if (me->GetGlobalCooldown() == 0 && !me->HasUnitState(UNIT_STATE_CASTING))
    {
        typedef std::vector<std::pair<Unit*, Spell*> > TargetSpellList;
        TargetSpellList targetSpellStore;

        for (uint8 i = 0; i < me->GetPetAutoSpellSize(); ++i)
        {
            uint32 spellID = me->GetPetAutoSpellOnPos(i);
            if (!spellID)
                continue;

            SpellEntry const* spellInfo = sSpellStore.LookupEntry(spellID);
            if (!spellInfo)
                continue;

            // ignore some combinations of combat state and combat/noncombat spells
            if (!me->getVictim())
            {
                // ignore attacking spells, and allow only self/around spells
                if (!IsPositiveSpell(spellInfo->Id))
                    continue;

                // non combat spells allowed
                // only pet spells have IsNonCombatSpell and not fit this reqs:
                // Consume Shadows, Lesser Invisibility, so ignore checks for its
                if (!IsNonCombatSpell(spellInfo))
                {
                    // allow only spell without spell cost or with spell cost but not duration limit
                    int32 duration = GetSpellDuration(spellInfo);
                    if ((spellInfo->manaCost || spellInfo->ManaCostPercentage || spellInfo->manaPerSecond) && duration > 0)
                        continue;

                    // allow only spell without cooldown > duration
                    int32 cooldown = GetSpellRecoveryTime(spellInfo);
                    if (cooldown >= 0 && duration >= 0 && cooldown > duration)
                        continue;
                }
            }
            else
            {
                // just ignore non-combat spells
                if (IsNonCombatSpell(spellInfo))
                    continue;
            }

            Spell* spell = new Spell(me, spellInfo, false, 0);

            // Fix to allow pets on STAY to autocast
            if (me->getVictim() && _CanAttack(me->getVictim()) && spell->CanAutoCast(me->getVictim()))
            {
                targetSpellStore.push_back(std::pair<Unit*, Spell*>(me->getVictim(), spell));
                continue;
            }
            else
            {
                bool spellUsed = false;
                for (std::set<uint64>::const_iterator tar = m_AllySet.begin(); tar != m_AllySet.end(); ++tar)
                {
                    Unit* Target = ObjectAccessor::GetUnit(*me, *tar);

                    //only buff targets that are in combat, unless the spell can only be cast while out of combat
                    if (!Target)
                        continue;

                    if (spell->CanAutoCast(Target))
                    {
                        targetSpellStore.push_back(std::pair<Unit*, Spell*>(Target, spell));
                        spellUsed = true;
                        break;
                    }
                }
                if (!spellUsed)
                    delete spell;
            }
        }

        //found units to cast on to
        if (!targetSpellStore.empty())
        {
            uint32 index = urand(0, targetSpellStore.size() - 1);

            Spell* spell  = targetSpellStore[index].second;
            Unit* target = targetSpellStore[index].first;

            targetSpellStore.erase(targetSpellStore.begin() + index);

            SpellCastTargets targets;
            targets.setUnitTarget(target);

            if (!me->HasInArc(M_PI, target))
            {
                me->SetInFront(target);
                if (target->GetTypeId() == TYPEID_PLAYER)
                    me->SendUpdateToPlayer(target->ToPlayer());

                if (owner && owner->GetTypeId() == TYPEID_PLAYER)
                    me->SendUpdateToPlayer(owner->ToPlayer());
            }

            me->AddCreatureSpellCooldown(spell->m_spellInfo->Id);
            if (me->IsPet())
                ((Pet*)me)->CheckLearning(spell->m_spellInfo->Id);

            spell->prepare(&targets);
        }

        // deleted cached Spell objects
        for (TargetSpellList::const_iterator itr = targetSpellStore.begin(); itr != targetSpellStore.end(); ++itr)
            delete itr->second;
    }
}

void PetAI::UpdateAllies()
{
    m_updateAlliesTimer = 10 * IN_MILLISECONDS;              //update friendly targets every 10 seconds, lesser checks increase performance

    Unit* owner = me->GetCharmerOrOwner();
    if (!owner)
        return;

    Group* group = NULL;
    if (Player* player = owner->ToPlayer())
        group = player->GetGroup();

    //only pet and owner/not in group->ok
    if (m_AllySet.size() == 2 && !group)
        return;

    //owner is in group; group members filled in already (no raid -> subgroupcount = whole count)
    if (group && !group->isRaidGroup() && m_AllySet.size() == (group->GetMembersCount() + 2))
        return;

    m_AllySet.clear();
    m_AllySet.insert(me->GetGUID());
    if (group)                                              //add group
    {
        for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
        {
            Player* Target = itr->getSource();
            if (!Target || !group->SameSubGroup(owner->ToPlayer(), Target))
                continue;

            if (Target->GetGUID() == owner->GetGUID())
                continue;

            m_AllySet.insert(Target->GetGUID());
        }
    }
    else                                                    //remove group
        m_AllySet.insert(owner->GetGUID());
}

void PetAI::KilledUnit(Unit* victim)
{
    // Called from Unit::Kill() in case where pet or owner kills something
    // if owner killed this victim, pet may still be attacking something else
    if (me == victim || (me->getVictim() && me->getVictim() != victim))
        return;

    // Clear target just in case. May help problem where health / focus / mana
    // regen gets stuck. Also resets attack command.
    // Can't use _stopAttack() because that activates movement handlers and ignores
    // next target selection
    me->AttackStop();
    me->InterruptNonMeleeSpells(false);
    me->SendMeleeAttackStop();  // Stops the pet's 'Attack' button from flashing
 
    // Before returning to owner, see if there are more things to attack
    if (Unit* nextTarget = SelectNextTarget())
        AttackStart(nextTarget);
    else
    {
        // Cancels the attack command so the pet stops chasing the
        // target it is attacking, and returns the pet to his owner
        me->GetCharmInfo()->SetIsCommandAttack(false);
        HandleReturnMovement();
    }
}

void PetAI::AttackStart(Unit* target)
{
    // Overrides Unit::AttackStart to correctly evaluate Pet states

    // Check all pet states to decide if we can attack this target
    if (!_CanAttack(target))
        return;

    // Only chase if not commanded to stay or if stay but commanded to attack
    DoAttack(target, (!me->GetCharmInfo()->HasCommandState(COMMAND_STAY) || me->GetCharmInfo()->IsCommandAttack()));
}

Unit* PetAI::SelectNextTarget()
{
    // Provides next target selection after current target death

    // Passive pets don't do next target selection
    if (me->HasReactState(REACT_PASSIVE))
        return NULL;

    // Check pet attackers first so we don't drag a bunch of targets to the owner
    if (Unit* myAttacker = me->getAttackerForHelper())
        return myAttacker;

    // Not sure why we wouldn't have an owner but just in case...
    if (!me->GetCharmerOrOwner())
        return NULL;

    // Check owner attackers
    if (Unit* ownerAttacker = me->GetCharmerOrOwner()->getAttackerForHelper())
        return ownerAttacker;

    // Default - no valid targets
    return NULL;
}

void PetAI::HandleReturnMovement()
{
    // Handles moving the pet back to stay or owner

    // Prevent activating movement when under control of spells
    // such as "Eyes of the Beast"
    if (me->isCharmed())
        return;

    if (me->GetCharmInfo()->HasCommandState(COMMAND_STAY))
    {
        if (!me->GetCharmInfo()->IsAtStay() && !me->GetCharmInfo()->IsReturning())
        {
            float x, y, z;

            me->GetCharmInfo()->GetStayPosition(x, y, z);
            ClearCharmInfoFlags();
            me->GetCharmInfo()->SetIsReturning(true);
            me->GetMotionMaster()->Clear();
            me->GetMotionMaster()->MovePoint(me->GetGUIDLow(), x, y, z);
        }
    }
    else // COMMAND_FOLLOW
    {
        if (!me->GetCharmInfo()->IsFollowing() && !me->GetCharmInfo()->IsReturning())
        {
            ClearCharmInfoFlags();
            me->GetCharmInfo()->SetIsReturning(true);
            me->GetMotionMaster()->Clear();
            me->GetMotionMaster()->MoveFollow(me->GetCharmerOrOwner(), PET_FOLLOW_DIST, me->GetFollowAngle());
        }
    }

}

void PetAI::DoAttack(Unit* target, bool chase)
{
    // Handles attack with or without chase and also resets flags
    // for next update / creature kill

    if (chase)
    {
        if (me->Attack(target, true))
        {
            bool oldCmdAttack = me->GetCharmInfo()->IsCommandAttack(); // This needs to be reset after other flags are cleared
            ClearCharmInfoFlags();
            me->GetCharmInfo()->SetIsCommandAttack(oldCmdAttack); // For passive pets commanded to attack so they will use spells
            me->GetMotionMaster()->Clear();
            me->GetMotionMaster()->MoveChase(target);
        }
    }
    else // (Stay && ((Aggressive || Defensive) && In Melee Range)))
    {
        ClearCharmInfoFlags();
        me->GetCharmInfo()->SetIsAtStay(true);
        me->GetMotionMaster()->Clear();
        me->GetMotionMaster()->MoveIdle();
    }
}

void PetAI::MovementInform(uint32 moveType, uint32 data)
{
    // Receives notification when pet reaches stay or follow owner
    switch (moveType)
    {
        case POINT_MOTION_TYPE:
        {
            // Pet is returning to where stay was clicked. data should be
            // pet's GUIDLow since we set that as the waypoint ID
            if (data == me->GetGUIDLow() && me->GetCharmInfo()->IsReturning())
            {
                ClearCharmInfoFlags();
                me->GetCharmInfo()->SetIsAtStay(true);
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveIdle();
            }
            break;
        }
        case FOLLOW_MOTION_TYPE:
        {
            // If data is owner's GUIDLow then we've reached follow point
            if (me->GetCharmerOrOwner() && me->GetCharmInfo() && data == me->GetCharmerOrOwner()->GetGUIDLow() && me->GetCharmInfo()->IsReturning())
            {
                ClearCharmInfoFlags();
                me->GetCharmInfo()->SetIsFollowing(true);
                me->AddUnitState(UNIT_STATE_FOLLOW);
            }
            break;
        }
        default:
            break;
    }
}

bool PetAI::_CanAttack(Unit* target)
{
    // Evaluates wether a pet can attack a specific target based on CommandState, ReactState and other flags

    if (!target)
        return false;

    if (!target->IsAlive())
    {
        // Clear target to prevent getting stuck on dead targets
        me->AttackStop();
        me->InterruptNonMeleeSpells(false);
        me->SendMeleeAttackStop();
        return false;
    }

    // Passive - passive pets can attack if told to
    if (me->HasReactState(REACT_PASSIVE))
        return me->GetCharmInfo()->IsCommandAttack();

    // Returning - pets ignore attacks only if owner clicked follow
    if (me->GetCharmInfo()->IsReturning())
        return false;

    // Is owner on a mount
    if (me->IsPet() && me->ToPet()->GetModeFlags() & PET_MODE_DISABLE_ACTIONS)
        return false;

    // Stay - can attack if target is within range or commanded to
    if (me->GetCharmInfo()->HasCommandState(COMMAND_STAY))
        return (me->IsWithinMeleeRange(target, MELEE_RANGE) || me->GetCharmInfo()->IsCommandAttack());

    //  Pets attacking something (or chasing) should only switch targets if owner tells them to
    if (me->getVictim() && me->getVictim() != target)
    {
        // Check if our owner selected this target and clicked "attack"
        Unit* ownerTarget = NULL;
        if (Player* owner = me->GetCharmerOrOwner()->ToPlayer())
            ownerTarget = owner->GetSelectedUnit();
        else
            ownerTarget = me->GetCharmerOrOwner()->getVictim();

        if (ownerTarget && me->GetCharmInfo()->IsCommandAttack())
            return (target->GetGUID() == ownerTarget->GetGUID());
    }

    // Follow
    if (me->GetCharmInfo()->HasCommandState(COMMAND_FOLLOW))
        return true;

    // default, though we shouldn't ever get here
    return false;
}

void PetAI::ClearCharmInfoFlags()
{
    // Quick access to set all flags to FALSE

    CharmInfo* ci = me->GetCharmInfo();

    if (ci)
    {
        ci->SetIsAtStay(false);
        ci->SetIsCommandAttack(false);
        ci->SetIsFollowing(false);
        ci->SetIsReturning(false);
    }
}
