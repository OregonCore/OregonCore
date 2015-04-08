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

#ifndef _THREATMANAGER
#define _THREATMANAGER

#include "Common.h"
#include "SharedDefines.h"
#include "Utilities/LinkedReference/Reference.h"
#include "UnitEvents.h"

#include <list>

//==============================================================

class Unit;
class Creature;
class ThreatManager;
struct SpellEntry;

//==============================================================
// Class to calculate the real threat based

struct ThreatCalcHelper
{
    static float calcThreat(Unit* hatedUnit, Unit* hatingUnit, float threat, SpellSchoolMask schoolMask = SPELL_SCHOOL_MASK_NORMAL, SpellEntry const* threatSpell = NULL);
    static bool isValidProcess(Unit* hatedUnit, Unit* hatingUnit, SpellEntry const* threatSpell = NULL);
};

//==============================================================
class HostileReference : public Reference<Unit, ThreatManager>
{
    public:
        HostileReference(Unit* refUnit, ThreatManager* threatManager, float threat);

        //=================================================
        void addThreat(float modThreat);

        void setThreat(float threat) { addThreat(threat - getThreat()); }

        void addThreatPercent(int32 percent);

        float getThreat() const { return iThreat; }

        bool isOnline() const
        {
            return iOnline;
        }

        // The Unit might be in water and the creature can not enter the water, but has range attack
        // in this case online = true, but accessible = false
        bool isAccessible() const
        {
            return iAccessible;
        }

        // used for temporary setting a threat and reducting it later again.
        // the threat modification is stored
        void setTempThreat(float threat)
        {
            addTempThreat(threat - getThreat());
        }

        void addTempThreat(float threat)
        {
            iTempThreatModifier = threat;
            if (iTempThreatModifier != 0.0f)
                addThreat(iTempThreatModifier);
        }

        void resetTempThreat()
        {
            if (iTempThreatModifier != 0.0f)
            {
                addThreat(-iTempThreatModifier);
                iTempThreatModifier = 0.0f;
            }
        }

        float getTempThreatModifyer()
        {
            return iTempThreatModifier;
        }

        //=================================================
        // check, if source can reach target and set the status
        void updateOnlineStatus();

        void setOnlineOfflineState(bool isOnline);

        void setAccessibleState(bool isAccessible);
        //=================================================

        bool operator == (const HostileReference& hostileRef) const { return hostileRef.getUnitGuid() == getUnitGuid(); }

        //=================================================

        uint64 getUnitGuid() const
        {
            return iUnitGuid;
        }

        //=================================================
        // reference is not needed anymore. realy delete it !

        void removeReference();

        //=================================================

        HostileReference* next()
        {
            return ((HostileReference*) Reference<Unit, ThreatManager>::next());
        }

        //=================================================

        // Tell our refTo (target) object that we have a link
        void targetObjectBuildLink();

        // Tell our refTo (taget) object, that the link is cut
        void targetObjectDestroyLink();

        // Tell our refFrom (source) object, that the link is cut (Target destroyed)
        void sourceObjectDestroyLink();
    private:
        // Inform the source, that the status of that reference was changed
        void fireStatusChanged(ThreatRefStatusChangeEvent& threatRefStatusChangeEvent);

        Unit* getSourceUnit();
    private:
        float iThreat;
        float iTempThreatModifier;                          // used for taunt
        uint64 iUnitGuid;
        bool iOnline;
        bool iAccessible;
};

//==============================================================
class ThreatManager;

class ThreatContainer
{
    private:
        std::list<HostileReference*> iThreatList;
        std::vector<Unit*> iPastEnemyList;
        bool iDirty;
    protected:
        friend class ThreatManager;

        void remove(HostileReference* hostileRef) { iThreatList.remove(hostileRef); }
        void addReference(HostileReference* hostileRef) { iThreatList.push_back(hostileRef); }
        void clearReferences();

        // Sort the list if necessary
        void update();
    public:
        ThreatContainer()
        {
            iDirty = false;
        }
        ~ThreatContainer()
        {
            clearReferences();
        }

        HostileReference* addThreat(Unit* victim, float threat);

        void modifyThreatPercent(Unit* victim, int32 percent);

        HostileReference* selectNextVictim(Creature* attacker, HostileReference* currentVictim);

        void setDirty(bool isDirty) { iDirty = isDirty; }

        bool isDirty() const { return iDirty; }

        bool empty() const { return iThreatList.empty(); }

        HostileReference* getMostHated() { return iThreatList.empty() ? NULL : iThreatList.front(); }

        HostileReference* getReferenceByTarget(Unit* victim);

        std::list<HostileReference*>& getThreatList()
        {
            return iThreatList;
        }
};

//=================================================

class ThreatManager
{
    public:
        friend class HostileReference;

        explicit ThreatManager(Unit *owner);

        ~ThreatManager()
        {
            clearReferences();
        }

        void clearReferences();

        void addThreat(Unit* victim, float threat, SpellSchoolMask schoolMask = SPELL_SCHOOL_MASK_NORMAL, SpellEntry const* threatSpell = NULL);

        void doAddThreat(Unit* victim, float threat);

        void modifyThreatPercent(Unit* victim, int32 percent);

        float getThreat(Unit* victim, bool alsoSearchOfflineList = false);

        //-----------------------------------------------------

        bool wasUnitThreat(Unit const* unit) const;
        void pushThreatInMemory(Unit* unit);

        void clearPastEnemyList()
        {
            iThreatContainer.iPastEnemyList.clear();
        }

        //-----------------------------------------------------

        bool isThreatListEmpty()
        {
            return iThreatContainer.empty();
        }

        void processThreatEvent(ThreatRefStatusChangeEvent* threatRefStatusChangeEvent);

        HostileReference* getCurrentVictim()
        {
            return iCurrentVictim;
        }

        Unit* getOwner()
        {
            return iOwner;
        }

        Unit* getHostileTarget();

        void tauntApply(Unit* taunter);
        void tauntFadeOut(Unit* taunter);

        void setCurrentVictim(HostileReference* hostileRef);

        void setDirty(bool isDirty) { iThreatContainer.setDirty(isDirty); }

        // methods to access the lists from the outside to do sume dirty manipulation (scriping and such)
        // I hope they are used as little as possible.
        inline std::list<HostileReference*>& getThreatList()
        {
            return iThreatContainer.getThreatList();
        }
        inline std::list<HostileReference*>& getOfflieThreatList()
        {
            return iThreatOfflineContainer.getThreatList();
        }
        inline ThreatContainer& getOnlineContainer()
        {
            return iThreatContainer;
        }
        inline ThreatContainer& getOfflineContainer()
        {
            return iThreatOfflineContainer;
        }

        void _addThreat(Unit* victim, float threat);
    private:
        HostileReference* iCurrentVictim;
        Unit* iOwner;
        ThreatContainer iThreatContainer;
        ThreatContainer iThreatOfflineContainer;
};

//=================================================
#endif

