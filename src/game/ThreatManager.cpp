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

#include "ThreatManager.h"
#include "Unit.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "Map.h"
#include "MapManager.h"
#include "Player.h"
#include "ObjectAccessor.h"
#include "UnitEvents.h"
#include "SpellMgr.h"

//==============================================================
//================= ThreatCalcHelper ===========================
//==============================================================

// The hatingUnit is not used yet
float ThreatCalcHelper::calcThreat(Unit* hatedUnit, Unit* /*hatingUnit*/, float threat, SpellSchoolMask schoolMask, SpellEntry const* threatSpell)
{
    if (threatSpell)
    {
        if (threatSpell->AttributesEx & SPELL_ATTR_EX_NO_THREAT)
            return 0.0f;

        if (SpellThreatEntry const*  threatEntry = sSpellMgr.GetSpellThreatEntry(threatSpell->Id))
            if (threatEntry->pctMod != 1.0f)
                threat *= threatEntry->pctMod;

        // Energize is not affected by Mods
        for (uint8 i = 0; i < MAX_SPELL_EFFECTS; i++)
            if (threatSpell->Effect[i] == SPELL_EFFECT_ENERGIZE || threatSpell->EffectApplyAuraName[i] == SPELL_AURA_PERIODIC_ENERGIZE)
                return threat;

        if (Player* modOwner = hatedUnit->GetSpellModOwner())
            modOwner->ApplySpellMod(threatSpell->Id, SPELLMOD_THREAT, threat);
    }

    return hatedUnit->ApplyTotalThreatModifier(threat, schoolMask);
}

bool ThreatCalcHelper::isValidProcess(Unit* hatedUnit, Unit* hatingUnit, SpellEntry const* /*threatSpell*/)
{
    //function deals with adding threat and adding players and pets into ThreatList
    //mobs, NPCs, guards have ThreatList and HateOfflineList
    //players and pets have only InHateListOf
    //HateOfflineList is used co contain unattackable victims (in-flight, in-water, GM etc.)

    if (!hatedUnit || !hatingUnit)
        return false;

    // not to self
    if (hatedUnit == hatingUnit)
        return false;

    // not to GM
    if (hatedUnit->GetTypeId() == TYPEID_PLAYER && hatedUnit->ToPlayer()->isGameMaster())
        return false;

    // not to dead and not for dead
    if (!hatedUnit->IsAlive() || !hatingUnit->IsAlive())
        return false;

    ASSERT(hatingUnit->GetTypeId() == TYPEID_UNIT);

    return true;
}

//============================================================
//================= HostileReference ==========================
//============================================================

HostileReference::HostileReference(Unit* pUnit, ThreatManager* pThreatManager, float pThreat)
{
    iThreat = pThreat;
    iTempThreatModifier = 0.0f;
    link(pUnit, pThreatManager);
    iUnitGuid = pUnit->GetGUID();
    iOnline = true;
    iAccessible = true;
}

//============================================================
// Tell our refTo (target) object that we have a link
void HostileReference::targetObjectBuildLink()
{
    getTarget()->addHatedBy(this);
}

//============================================================
// Tell our refTo (taget) object, that the link is cut
void HostileReference::targetObjectDestroyLink()
{
    getTarget()->removeHatedBy(this);
}

//============================================================
// Tell our refFrom (source) object, that the link is cut (Target destroyed)

void HostileReference::sourceObjectDestroyLink()
{
    setOnlineOfflineState(false);
}

//============================================================
// Inform the source, that the status of the reference changed

void HostileReference::fireStatusChanged(ThreatRefStatusChangeEvent& pThreatRefStatusChangeEvent)
{
    if (getSource())
        getSource()->processThreatEvent(&pThreatRefStatusChangeEvent);
}

//============================================================

void HostileReference::addThreat(float pMod)
{
    iThreat += pMod;
    // the threat is changed. Source and target unit have to be availabe
    // if the link was cut before relink it again
    if (!isOnline())
        updateOnlineStatus();
    if (pMod != 0.0f)
    {
        ThreatRefStatusChangeEvent event(UEV_THREAT_REF_THREAT_CHANGE, this, pMod);
        fireStatusChanged(event);
    }

    if (isValid() && pMod >= 0)
    {
        Unit* victim_owner = getTarget()->GetCharmerOrOwner();
        if (victim_owner && victim_owner->IsAlive())
            getSource()->addThreat(victim_owner, 0.0f);     // create a threat to the owner of a pet, if the pet attacks
    }
}

void HostileReference::addThreatPercent(int32 percent)
{
    float tmpThreat = iThreat;
    AddPct(tmpThreat, percent);
    addThreat(tmpThreat - iThreat);
}

//============================================================
// check, if source can reach target and set the status

void HostileReference::updateOnlineStatus()
{
    bool online = false;
    bool accessible = false;

    if (!isValid())
    {
        Unit* target = ObjectAccessor::GetUnit(*getSourceUnit(), getUnitGuid());
        if (target)
            link(target, getSource());
    }
    // only check for online status if
    // ref is valid
    // target is no player or not gamemaster
    // target is not in flight
    if (isValid() &&
        ((getTarget()->GetTypeId() != TYPEID_PLAYER || !getTarget()->ToPlayer()->isGameMaster()) ||
         !getTarget()->HasUnitState(UNIT_STATE_IN_FLIGHT)))
    {
        Creature* creature = (Creature*) getSourceUnit();
        online = getTarget()->isInAccessiblePlaceFor(creature);
        if (!online)
        {
            if (creature->IsWithinCombatRange(getTarget(), MELEE_RANGE))
                online = true;                              // not accessible but stays online
        }
        else
            accessible = true;

    }
    setAccessibleState(accessible);
    setOnlineOfflineState(online);
}

//============================================================
// set the status and fire the event on status change

void HostileReference::setOnlineOfflineState(bool pIsOnline)
{
    if (iOnline != pIsOnline)
    {
        iOnline = pIsOnline;
        if (!iOnline)
            setAccessibleState(false);                      // if not online that not accessible as well

        ThreatRefStatusChangeEvent event(UEV_THREAT_REF_ONLINE_STATUS, this);
        fireStatusChanged(event);
    }
}

//============================================================

void HostileReference::setAccessibleState(bool pIsAccessible)
{
    if (iAccessible != pIsAccessible)
    {
        iAccessible = pIsAccessible;

        ThreatRefStatusChangeEvent event(UEV_THREAT_REF_ASSECCIBLE_STATUS, this);
        fireStatusChanged(event);
    }
}

//============================================================
// prepare the reference for deleting
// this is called be the target

void HostileReference::removeReference()
{
    invalidate();

    ThreatRefStatusChangeEvent event(UEV_THREAT_REF_REMOVE_FROM_LIST, this);
    fireStatusChanged(event);
}

//============================================================

Unit* HostileReference::getSourceUnit()
{
    return (getSource()->getOwner());
}

//============================================================
//================ ThreatContainer ===========================
//============================================================

void ThreatContainer::clearReferences()
{
    for (std::list<HostileReference*>::iterator i = iThreatList.begin(); i != iThreatList.end(); ++i)
    {
        (*i)->unlink();
        delete (*i);
    }
    iThreatList.clear();
}

//============================================================
// Return the HostileReference of NULL, if not found
HostileReference* ThreatContainer::getReferenceByTarget(Unit* pVictim)
{
    HostileReference* result = NULL;
    uint64 guid = pVictim->GetGUID();
    for (std::list<HostileReference*>::iterator i = iThreatList.begin(); i != iThreatList.end(); ++i)
    {
        if ((*i)->getUnitGuid() == guid)
        {
            result = (*i);
            break;
        }
    }

    return result;
}

//============================================================
// Add the threat, if we find the reference

HostileReference* ThreatContainer::addThreat(Unit* pVictim, float pThreat)
{
    HostileReference* ref = getReferenceByTarget(pVictim);
    if (ref)
        ref->addThreat(pThreat);
    return ref;
}

//============================================================

void ThreatContainer::modifyThreatPercent(Unit* pVictim, int32 pPercent)
{
    if (HostileReference* ref = getReferenceByTarget(pVictim))
        ref->addThreatPercent(pPercent);
}

//============================================================

bool HostileReferenceSortPredicate(const HostileReference* lhs, const HostileReference* rhs)
{
    // std::list::sort ordering predicate must be: (Pred(x,y)&&Pred(y,x)) == false
    return lhs->getThreat() > rhs->getThreat();             // reverse sorting
}

//============================================================
// Check if the list is dirty and sort if necessary

void ThreatContainer::update()
{
    if (iDirty && iThreatList.size() > 1)
        iThreatList.sort(HostileReferenceSortPredicate);
    iDirty = false;
}

//============================================================
// return the next best victim
// could be the current victim

HostileReference* ThreatContainer::selectNextVictim(Creature* pAttacker, HostileReference* pCurrentVictim)
{
    HostileReference* currentRef = NULL;
    bool found = false;

    std::list<HostileReference*>::iterator lastRef = iThreatList.end();
    lastRef--;

    for (std::list<HostileReference*>::iterator iter = iThreatList.begin(); iter != iThreatList.end() && !found; ++iter)
    {
        currentRef = (*iter);

        Unit* target = currentRef->getTarget();
        ASSERT(target);                                     // if the ref has status online the target must be there !

        // some units are preferred in comparison to others
        if (iter != lastRef && (target->IsImmunedToDamage(pAttacker->GetMeleeDamageSchoolMask(), false) ||
                                target->HasUnitState(UNIT_STATE_CONFUSED)
                               ))
        {
            // current victim is a second choice target, so don't compare threat with it below
            if (currentRef == pCurrentVictim)
                pCurrentVictim = NULL;
            continue;
        }

        if (!pAttacker->IsOutOfThreatArea(target))           // skip non attackable currently targets
        {
            if (pCurrentVictim)                              // select 1.3/1.1 better target in comparison current target
            {
                // list sorted and and we check current target, then this is best case
                if (pCurrentVictim == currentRef || currentRef->getThreat() <= 1.1f * pCurrentVictim->getThreat())
                {
                    currentRef = pCurrentVictim;            // for second case
                    found = true;
                    break;
                }

                if ( currentRef->getThreat() > 1.3f * pCurrentVictim->getThreat() ||
                     (currentRef->getThreat() > 1.1f * pCurrentVictim->getThreat() && pAttacker->IsWithinMeleeRange(target)))
                {
                    //implement 110% threat rule for targets in melee range
                    found = true;                           //and 130% rule for targets in ranged distances
                    break;                                  //for selecting alive targets
                }
            }
            else                                            // select any
            {
                found = true;
                break;
            }
        }
    }
    if (!found)
        currentRef = NULL;

    return currentRef;
}

//============================================================
//=================== ThreatManager ==========================
//============================================================

ThreatManager::ThreatManager(Unit* owner) : iCurrentVictim(NULL), iOwner(owner)
{
}

//============================================================

void ThreatManager::clearReferences()
{
    iThreatContainer.clearReferences();
    iThreatOfflineContainer.clearReferences();
    iCurrentVictim = NULL;
}

//============================================================

void ThreatManager::addThreat(Unit* victim, float threat, SpellSchoolMask schoolMask, SpellEntry const *threatSpell)
{
    if (!ThreatCalcHelper::isValidProcess(victim, getOwner(), threatSpell))
        return;

    doAddThreat(victim, ThreatCalcHelper::calcThreat(victim, iOwner, threat, schoolMask, threatSpell));
}

void ThreatManager::doAddThreat(Unit* victim, float threat)
{
    uint32 reducedThreadPercent = victim->GetReducedThreatPercent();

    // must check > 0.0f, otherwise dead loop
    if (threat > 0.0f && reducedThreadPercent)
    {
        Unit* redirectTarget = victim->GetMisdirectionTarget();

        float reducedThreat = threat * reducedThreadPercent / 100.0f;
        threat -= reducedThreat;
        if (redirectTarget)
            _addThreat(redirectTarget, reducedThreat);
    }

    _addThreat(victim, threat);
}

void ThreatManager::_addThreat(Unit* victim, float threat)
{
    HostileReference* ref = iThreatContainer.addThreat(victim, threat);
    // Ref is not in the online refs, search the offline refs next
    if (!ref)
        ref = iThreatOfflineContainer.addThreat(victim, threat);

    if (!ref) // there was no ref => create a new one
    {
                                                            // threat has to be 0 here
        HostileReference* hostileRef = new HostileReference(victim, this, 0);
        iThreatContainer.addReference(hostileRef);
        hostileRef->addThreat(threat); // now we add the real threat
        if (victim->GetTypeId() == TYPEID_PLAYER && victim->ToPlayer()->isGameMaster())
            hostileRef->setOnlineOfflineState(false); // GM is always offline
    }
}

//============================================================

void ThreatManager::modifyThreatPercent(Unit* pVictim, int32 pPercent)
{
    iThreatContainer.modifyThreatPercent(pVictim, pPercent);
}

//============================================================

Unit* ThreatManager::getHostileTarget()
{
    iThreatContainer.update();
    HostileReference* nextVictim = iThreatContainer.selectNextVictim(getOwner()->ToCreature(), getCurrentVictim());
    setCurrentVictim(nextVictim);
    return getCurrentVictim() != NULL ? getCurrentVictim()->getTarget() : NULL;
}

//============================================================

float ThreatManager::getThreat(Unit* pVictim, bool pAlsoSearchOfflineList)
{
    float threat = 0.0f;
    HostileReference* ref = iThreatContainer.getReferenceByTarget(pVictim);
    if (!ref && pAlsoSearchOfflineList)
        ref = iThreatOfflineContainer.getReferenceByTarget(pVictim);
    if (ref)
        threat = ref->getThreat();
    return threat;
}

//============================================================

// Check if the unit was a threat before (is registered in pastThreatList)
bool ThreatManager::wasUnitThreat(Unit const* unit) const
{
    if (unit && !iThreatContainer.iPastEnemyList.empty())
    {
        std::vector<Unit*>::const_iterator it = iThreatContainer.iPastEnemyList.begin();
        for (; it != iThreatContainer.iPastEnemyList.end(); ++it)
            if ((*it) && unit == (*it))
                return true;
    }
    return false;
}

// Push new threat in pastEnemyList
void ThreatManager::pushThreatInMemory(Unit* unit)
{
    // Add the entry only if no duplicate found
    if (!wasUnitThreat(unit))
        iThreatContainer.iPastEnemyList.push_back(unit);
}

//============================================================

void ThreatManager::tauntApply(Unit* pTaunter)
{
    HostileReference* ref = iThreatContainer.getReferenceByTarget(pTaunter);
    if (getCurrentVictim() && ref && (ref->getThreat() < getCurrentVictim()->getThreat()))
    {
        if (ref->getTempThreatModifyer() == 0.0f)
            // Ok, temp threat is unused
            ref->setTempThreat(getCurrentVictim()->getThreat());
    }
}

//============================================================

void ThreatManager::tauntFadeOut(Unit* pTaunter)
{
    HostileReference* ref = iThreatContainer.getReferenceByTarget(pTaunter);
    if (ref)
        ref->resetTempThreat();
}

//============================================================

void ThreatManager::setCurrentVictim(HostileReference* pHostileReference)
{
    iCurrentVictim = pHostileReference;
}

//============================================================
// The hated unit is gone, dead or deleted
// return true, if the event is consumed

void ThreatManager::processThreatEvent(ThreatRefStatusChangeEvent* threatRefStatusChangeEvent)
{
    threatRefStatusChangeEvent->setThreatManager(this);     // now we can set the threat manager

    HostileReference* hostileReference = threatRefStatusChangeEvent->getReference();

    switch (threatRefStatusChangeEvent->getType())
    {
    case UEV_THREAT_REF_THREAT_CHANGE:
        if ((getCurrentVictim() == hostileReference && threatRefStatusChangeEvent->getFValue() < 0.0f) ||
            (getCurrentVictim() != hostileReference && threatRefStatusChangeEvent->getFValue() > 0.0f))
            setDirty(true);                             // the order in the threat list might have changed
        break;
    case UEV_THREAT_REF_ONLINE_STATUS:
        if (!hostileReference->isOnline())
        {
            if (hostileReference == getCurrentVictim())
            {
                setCurrentVictim(NULL);
                setDirty(true);
            }
            iThreatContainer.remove(hostileReference);
            iThreatOfflineContainer.addReference(hostileReference);
        }
        else
        {
            if (getCurrentVictim() && hostileReference->getThreat() > (1.1f * getCurrentVictim()->getThreat()))
                setDirty(true);
            iThreatContainer.addReference(hostileReference);
            iThreatOfflineContainer.remove(hostileReference);
        }
        break;
    case UEV_THREAT_REF_REMOVE_FROM_LIST:
        if (hostileReference == getCurrentVictim())
        {
            setCurrentVictim(NULL);
            setDirty(true);
        }
        if (hostileReference->isOnline())
            iThreatContainer.remove(hostileReference);
        else
            iThreatOfflineContainer.remove(hostileReference);
        break;
    }
}

