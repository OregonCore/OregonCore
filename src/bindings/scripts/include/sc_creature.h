/* Copyright (C) 2008-2009 Oregon <http://www.oregoncore.com/>
 *
 * Thanks to the original authors: ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 *
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef SC_CREATURE_H
#define SC_CREATURE_H

#include "Creature.h"
#include "CreatureAI.h"
#include "CreatureAIImpl.h"
#include "InstanceData.h"

#define SCRIPT_CAST_TYPE dynamic_cast
//#define SCRIPT_CAST_TYPE static_cast

#define CAST_PLR(a)     (SCRIPT_CAST_TYPE<Player*>(a))
#define CAST_CRE(a)     (SCRIPT_CAST_TYPE<Creature*>(a))
#define CAST_SUM(a)     (SCRIPT_CAST_TYPE<TempSummon*>(a))
#define CAST_PET(a)     (SCRIPT_CAST_TYPE<Pet*>(a))
#define CAST_AI(a,b)    (SCRIPT_CAST_TYPE<a*>(b))

#define GET_SPELL(a)    (const_cast<SpellEntry*>(GetSpellStore()->LookupEntry(a)))

class ScriptedInstance;

class SummonList : public std::list<uint64>
{
    public:
        explicit SummonList(Creature* creature) : m_creature(creature) {}
        void Summon(Creature *summon) { push_back(summon->GetGUID()); }
        void Despawn(Creature *summon) { remove(summon->GetGUID()); }
        void DespawnEntry(uint32 entry);
        void DespawnAll();
        void DoAction(uint32 entry, uint32 info);
        void DoZoneInCombat(uint32 entry = 0);
    private:
        Creature *m_creature;
};

//Get a single creature of given entry
Unit* FindCreature(uint32 entry, float range, Unit* Finder);

//Get a single gameobject of given entry
GameObject* FindGameObject(uint32 entry, float range, Unit* Finder);

struct OREGON_DLL_DECL ScriptedAI : public CreatureAI
{
    explicit ScriptedAI(Creature* pCreature);
    ~ScriptedAI() {}

    //*************
    //CreatureAI Functions
    //*************

    void AttackStartNoMove(Unit *pTarget);
    void AttackStart(Unit *);
    void AttackStart(Unit *, bool melee);

    // Called at any Damage from any attacker (before damage apply)
    void DamageTaken(Unit* pDone_by, uint32& uiDamage) {}

    //Called at World update tick
    void UpdateAI(const uint32);

    //Called at creature death
    void JustDied(Unit* who){}

    //Called at creature killing another unit
    void KilledUnit(Unit* who){}

    // Called when the creature summon successfully other creature
    void JustSummoned(Creature*) {}

    // Called when a summoned creature is despawned
    void SummonedCreatureDespawn(Creature*) {}

    // Called when hit by a spell
    void SpellHit(Unit* caster, const SpellEntry *spell) {}

    // Called when spell hits a target
    void SpellHitTarget(Unit *pTarget, const SpellEntry *spell) {}

    //Called at waypoint reached or PointMovement end
    void MovementInform(uint32 type, uint32 id){}

    // Called when AI is temporarily replaced or put back when possess is applied or removed
    void OnPossess(bool apply) {}

    //*************
    // Variables
    //*************

    //Pointer to creature we are manipulating
    Creature* m_creature;

    //For fleeing
    bool IsFleeing;

    bool HeroicMode;

    //*************
    //Pure virtual functions
    //*************

    //Called at creature reset either by death or evade
    void Reset() {}

    //Called at creature aggro either by MoveInLOS or Attack Start
    void EnterCombat(Unit* who) {}

    //*************
    //AI Helper Functions
    //*************

    //Start movement toward victim
    void DoStartMovement(Unit* pVictim, float fDistance = 0, float fAngle = 0);

    //Start no movement on victim
    void DoStartNoMovement(Unit* pVictim);

    //Stop attack of current victim
    void DoStopAttack();

    //Cast spell by spell info
    void DoCastSpell(Unit* pTarget, SpellEntry const* pSpellInfo, bool bTriggered = false);

    //Creature say
    void DoSay(const char* text, uint32 language, Unit* target, bool SayEmote = false);

    //Creature Yell
    void DoYell(const char* text, uint32 language, Unit* target);

    //Creature Text emote, optional bool for boss emote text
    void DoTextEmote(const char* text, Unit* target, bool IsBossEmote = false);

    //Creature whisper, optional bool for boss whisper
    void DoWhisper(const char* text, Unit* reciever, bool IsBossWhisper = false);

    //Plays a sound to all nearby players
    void DoPlaySoundToSet(WorldObject* pSource, uint32 sound);

    //Drops all threat to 0%. Does not remove players from the threat list
    void DoResetThreat();

    float DoGetThreat(Unit* u);
    void DoModifyThreatPercent(Unit* pUnit, int32 pct);

    void DoTeleportTo(float fX, float fY, float fZ, uint32 uiTime = 0);
    void DoTeleportTo(const float pos[4]);

    void DoAction(const int32 param) {}

    //Teleports a player without dropping threat (only teleports to same map)
    void DoTeleportPlayer(Unit* pUnit, float fX, float fY, float fZ, float fO);
    void DoTeleportAll(float fX, float fY, float fZ, float fO);

    //Returns friendly unit with the most amount of hp missing from max hp
    Unit* DoSelectLowestHpFriendly(float fRange, uint32 uiMinHPDiff = 1);

    //Returns a list of friendly CC'd units within range
    std::list<Creature*> DoFindFriendlyCC(float fRange);

    //Returns a list of all friendly units missing a specific buff within range
    std::list<Creature*> DoFindFriendlyMissingBuff(float fRange, uint32 uiSpellId);

    //Return a player with at least minimumRange from m_creature
    Player* GetPlayerAtMinimumRange(float fMinimumRange);

    //Spawns a creature relative to m_creature
    Creature* DoSpawnCreature(uint32 uiId, float fX, float fY, float fZ, float fAngle, uint32 uiType, uint32 uiDespawntime);

    //Selects a unit from the creature's current aggro list
    Unit* SelectUnit(SelectAggroTarget pTarget, uint32 uiPosition);

    bool HealthBelowPct(uint32 pct) const { return me->GetHealth() * 100 < m_creature->GetMaxHealth() * pct; }

    //Returns spells that meet the specified criteria from the creatures spell list
    SpellEntry const* SelectSpell(Unit* Target, int32 School, int32 Mechanic, SelectTargetType Targets,  uint32 PowerCostMin, uint32 PowerCostMax, float RangeMin, float RangeMax, SelectEffect Effect);

    //Checks if you can cast the specified spell
    bool CanCast(Unit* pTarget, SpellEntry const* pSpell, bool bTriggered = false);

    void SetEquipmentSlots(bool bLoadDefault, int32 uiMainHand = EQUIP_NO_CHANGE, int32 uiOffHand = EQUIP_NO_CHANGE, int32 uiRanged = EQUIP_NO_CHANGE);

    //Generally used to control if MoveChase() is to be used or not in AttackStart(). Some creatures does not chase victims
    void SetCombatMovement(bool CombatMove);
    bool IsCombatMovement() { return m_bCombatMovement; }

    bool EnterEvadeIfOutOfCombatArea(const uint32 uiDiff);

    private:
        bool m_bCombatMovement;
        uint32 m_uiEvadeCheckCooldown;
};

struct OREGON_DLL_DECL Scripted_NoMovementAI : public ScriptedAI
{
    Scripted_NoMovementAI(Creature* creature) : ScriptedAI(creature) {}

    //Called at each attack of m_creature by any victim
    void AttackStart(Unit* who);
};

struct OREGON_DLL_DECL BossAI : public ScriptedAI
{
    BossAI(Creature *c, uint32 id);

    uint32 bossId;
    EventMap events;
    SummonList summons;
    InstanceData *instance;

    void JustSummoned(Creature *summon);
    void SummonedCreatureDespawn(Creature *summon);

    void UpdateAI(const uint32 diff) = 0;

    void Reset() { _Reset(); }
    void EnterCombat(Unit *who) { _EnterCombat(); }
    void JustDied(Unit *killer) { _JustDied(); }

    protected:
        void _Reset();
        void _EnterCombat();
        void _JustDied();
};

// SD2 grid searchers
//return closest creature alive in grid, with range from pSource
Creature* GetClosestCreatureWithEntry(WorldObject* pSource, uint32 Entry, float MaxSearchRange);

#endif

