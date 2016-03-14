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

#include "GuardAI.h"
#include "Errors.h"
#include "Player.h"
#include "ObjectAccessor.h"

int GuardAI::Permissible(const Creature* creature)
{
    if (creature->isGuard())
        return PERMIT_BASE_SPECIAL;

    return PERMIT_BASE_NO;
}

GuardAI::GuardAI(Creature* c) : CreatureAI(c), i_victimGuid(0), i_state(STATE_NORMAL), i_tracker(TIME_INTERVAL_LOOK)
{
}

void GuardAI::MoveInLineOfSight(Unit* u)
{
    // Ignore Z for flying creatures
    if (!me->canFly() && me->GetDistanceZ(u) > CREATURE_Z_ATTACK_RANGE)
        return;

    if (!me->getVictim() && me->canAttack(u) &&
        (u->IsHostileToPlayers() || me->IsHostileTo(u) /*|| u->getVictim() && me->IsFriendlyTo(u->getVictim())*/) &&
        u->isInAccessiblePlaceFor(me))
    {
        float attackRadius = me->GetAttackDistance(u);
        if (me->IsWithinDistInMap(u, attackRadius))
        {
            //Need add code to let guard support player
            AttackStart(u);
            //u->RemoveAurasByType(SPELL_AURA_MOD_STEALTH);
        }
    }
}

void GuardAI::EnterEvadeMode()
{
    if (!me->IsAlive())
    {
        DEBUG_LOG("Creature stopped attacking because he's dead [guid=%u]", me->GetGUIDLow());
        me->GetMotionMaster()->MoveIdle();

        i_state = STATE_NORMAL;

        i_victimGuid = 0;
        me->CombatStop(true);
        me->DeleteThreatList();
        return;
    }

    Unit* victim = ObjectAccessor::GetUnit(*me, i_victimGuid);

    if (!victim)
        DEBUG_LOG("Creature stopped attacking because victim is non exist [guid=%u]", me->GetGUIDLow());
    else if (!victim ->IsAlive())
        DEBUG_LOG("Creature stopped attacking because victim is dead [guid=%u]", me->GetGUIDLow());
    else if (victim ->HasStealthAura())
        DEBUG_LOG("Creature stopped attacking because victim is using stealth [guid=%u]", me->GetGUIDLow());
    else if (victim ->isInFlight())
        DEBUG_LOG("Creature stopped attacking because victim is flying away [guid=%u]", me->GetGUIDLow());
    else
        DEBUG_LOG("Creature stopped attacking because victim outran him [guid=%u]", me->GetGUIDLow());

    me->RemoveAllAuras();
    me->DeleteThreatList();
    i_victimGuid = 0;
    me->CombatStop(true);
    i_state = STATE_NORMAL;

    // Remove TargetedMovementGenerator from MotionMaster stack list, and add HomeMovementGenerator instead
    if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() & TARGETED_MOTION_TYPE)
        me->GetMotionMaster()->MoveTargetedHome();
}

void GuardAI::UpdateAI(const uint32 /*diff*/)
{
    // update i_victimGuid if me->getVictim() != 0 and changed
    if (!UpdateVictim() || !me->getVictim())
        return;

    i_victimGuid = me->getVictim()->GetGUID();

    if (me->isAttackReady())
    {
        if (me->IsWithinMeleeRange(me->getVictim()))
        {
            me->AttackerStateUpdate(me->getVictim());
            me->resetAttackTimer();
        }
    }
}

bool GuardAI::IsVisible(Unit* pl) const
{
    return me->IsWithinDistInMap(pl, sWorld.getConfig(CONFIG_SIGHT_GUARDER))
        && me->CanSeeOrDetect(pl);
}

void GuardAI::JustDied(Unit* killer)
{
    if (Player* pkiller = killer->GetCharmerOrOwnerPlayerOrPlayerItself())
        me->SendZoneUnderAttackMessage(pkiller);
}

