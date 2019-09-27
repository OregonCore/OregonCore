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

#ifndef OREGONCORE_PET_H
#define OREGONCORE_PET_H

#include "ObjectGuid.h"
#include "Unit.h"
#include "TemporarySummon.h"

enum PetType
{
    SUMMON_PET              = 0,
    HUNTER_PET              = 1,
    CLASS_PET               = 2,
    MAX_PET_TYPE            = 4
};

enum PetSaveMode
{
    PET_SAVE_AS_DELETED       = -1,
    PET_SAVE_AS_CURRENT       = 0,
    PET_SAVE_IN_STABLE_SLOT_1 = 1,
    PET_SAVE_IN_STABLE_SLOT_2 = 2,
    PET_SAVE_NOT_IN_SLOT      = 3
};

// Every pet has to have a designated status at all times
// The status is stored in 'Player.h'
enum PetStatus
{
    PET_STATUS_NONE                  = 0,   // No pet available
    PET_STATUS_CURRENT               = 1,   // Pet is currently active and alive
    PET_STATUS_DEAD                  = 2,   // Pet is dead, but active in the world
    PET_STATUS_DISMISSED             = 3,   // Pet has been dismissed
    PET_STATUS_DEAD_AND_REMOVED      = 4    // Pet is dead and it's corpse removed from the world
};

enum PetModeFlags
{
    PET_MODE_UNKNOWN_0         = 0x0000001,
    PET_MODE_UNKNOWN_2         = 0x0000100,
    PET_MODE_DISABLE_ACTIONS   = 0x8000000,

    // autoset in client at summon
    PET_MODE_DEFAULT           = PET_MODE_UNKNOWN_0 | PET_MODE_UNKNOWN_2,
};

enum HappinessState
{
    UNHAPPY = 1,
    CONTENT = 2,
    HAPPY   = 3
};

enum LoyaltyLevel
{
    REBELLIOUS  = 1,
    UNRULY      = 2,
    SUBMISSIVE  = 3,
    DEPENDABLE  = 4,
    FAITHFUL    = 5,
    BEST_FRIEND = 6
};

enum PetSpellState
{
    PETSPELL_UNCHANGED = 0,
    PETSPELL_CHANGED   = 1,
    PETSPELL_NEW       = 2,
    PETSPELL_REMOVED   = 3
};

enum PetSpellType
{
    PETSPELL_NORMAL = 0,
    PETSPELL_FAMILY = 1,
};

struct PetSpell
{
    uint16 active;

    PetSpellState state : 8;
    PetSpellType type   : 8;
};

enum ActionFeedback
{
    FEEDBACK_NONE            = 0,
    FEEDBACK_PET_DEAD        = 1,
    FEEDBACK_NOTHING_TO_ATT  = 2,
    FEEDBACK_CANT_ATT_TARGET = 3
};

enum PetTalk
{
    PET_TALK_SPECIAL_SPELL  = 0,
    PET_TALK_ATTACK         = 1
};

enum PetNameInvalidReason
{
    PET_NAME_INVALID                                        = 1,
    PET_NAME_NO_NAME                                        = 2,
    PET_NAME_TOO_SHORT                                      = 3,
    PET_NAME_TOO_LONG                                       = 4,
    PET_NAME_MIXED_LANGUAGES                                = 6,
    PET_NAME_PROFANE                                        = 7,
    PET_NAME_RESERVED                                       = 8,
    PET_NAME_THREE_CONSECUTIVE                              = 11,
    PET_NAME_INVALID_SPACE                                  = 12,
    PET_NAME_CONSECUTIVE_SPACES                             = 13,
    PET_NAME_RUSSIAN_CONSECUTIVE_SILENT_CHARACTERS          = 14,
    PET_NAME_RUSSIAN_SILENT_CHARACTER_AT_BEGINNING_OR_END   = 15,
    PET_NAME_DECLENSION_DOESNT_MATCH_BASE_NAME              = 16
};

typedef UNORDERED_MAP<uint16, PetSpell> PetSpellMap;
typedef std::map<uint32, uint32> TeachSpellMap;
typedef std::vector<uint32> AutoSpellList;

#define HAPPINESS_LEVEL_SIZE        333000

extern const uint32 LevelUpLoyalty[6];
extern const uint32 LevelStartLoyalty[6];

#define ACTIVE_SPELLS_MAX           4

class Player;

class Pet : public Guardian
{
    public:
        explicit Pet(Player* owner, PetType type = MAX_PET_TYPE);
        ~Pet() override;

        void AddToWorld() override;
        void RemoveFromWorld() override;

        PetType getPetType() const
        {
            return m_petType;
        }
        void setPetType(PetType type)
        {
            m_petType = type;
        }
        bool isControlled() const
        {
            return getPetType() == SUMMON_PET || getPetType() == HUNTER_PET;
        }
        bool isTemporarySummoned() const
        {
            return m_duration > 0;
        }

        bool IsPermanentPetFor(Player* owner);              // pet have tab in character windows and set UNIT_FIELD_PETNUMBER

        bool Create (uint32 guidlow, Map* map, uint32 Entry, uint32 pet_number);
        bool CreateBaseAtCreature(Creature* creature);
        bool LoadPetFromDB(Player* owner, uint32 petentry = 0, uint32 petnumber = 0, bool current = false);
        void SavePetToDB(PetSaveMode mode);
        void Remove(PetSaveMode mode, bool returnreagent = false);
        static void DeleteFromDB(uint32 guidlow);

        void setDeathState(DeathState s) override;                   // overwrite virtual Creature::setDeathState and Unit::setDeathState
        void Update(uint32 diff) override;                           // overwrite virtual Creature::Update and Unit::Update

        uint8 GetPetAutoSpellSize() const override
        {
            return m_autospells.size();
        }
        uint32 GetPetAutoSpellOnPos(uint8 pos) const override
        {
            if (pos >= m_autospells.size())
                return 0;
            else
                return m_autospells[pos];
        }

        void RegenerateFocus();
        void LoseHappiness();
        void TickLoyaltyChange();
        void ModifyLoyalty(int32 addvalue);
        HappinessState GetHappinessState();
        uint32 GetMaxLoyaltyPoints(uint32 level);
        uint32 GetStartLoyaltyPoints(uint32 level);
        void KillLoyaltyBonus(uint32 level);
        uint32 GetLoyaltyLevel()
        {
            return GetByteValue(UNIT_FIELD_BYTES_1, 1);
        }
        void SetLoyaltyLevel(LoyaltyLevel level);
        void GivePetXP(uint32 xp);
        void GivePetLevel(uint32 level);
        void InitPetAuras(const uint32 Entry);
        bool HaveInDiet(ItemTemplate const* item) const;
        uint32 GetCurrentFoodBenefitLevel(uint32 itemlevel);
        void SetDuration(int32 dur) { m_duration = dur; }

        bool CanSwim() const { return true; }

        bool   CanTakeMoreActiveSpells(uint32 SpellIconID);
        void   ToggleAutocast(uint32 spellid, bool apply);
        bool   HasTPForSpell(uint32 spellid);
        int32  GetTPForSpell(uint32 spellid);

        void ApplyModeFlags(PetModeFlags mode, bool apply);
        PetModeFlags GetModeFlags() const { return m_petModeFlags; }

        bool HasSpell(uint32 spell) const override;
        void AddTeachSpell(uint32 learned_id, uint32 source_id)
        {
            m_teachspells[learned_id] = source_id;
        }

        void LearnPetPassives();
        void CastPetAuras(bool current);
        void CastPetAura(PetAura const* aura);

        void _LoadSpellCooldowns();
        void _SaveSpellCooldowns();
        void _LoadAuras(uint32 timediff);
        void _SaveAuras();
        void _LoadSpells();
        void _SaveSpells();

        bool addSpell(uint16 spell_id, uint16 active = ACT_DECIDE, PetSpellState state = PETSPELL_NEW, PetSpellType type = PETSPELL_NORMAL);
        bool learnSpell(uint16 spell_id);
        void removeSpell(uint16 spell_id);

        PetSpellMap     m_spells;
        TeachSpellMap   m_teachspells;
        AutoSpellList   m_autospells;

        void InitPetCreateSpells();
        void CheckLearning(uint32 spellid);
        uint32 resetTalentsCost() const;

        void  SetTP(int32 TP);
        int32 GetDispTP();

        int32   m_TrainingPoints;
        uint32  m_resetTalentsCost;
        time_t  m_resetTalentsTime;

        uint64 GetAuraUpdateMask()
        {
            return m_auraUpdateMask;
        }
        void SetAuraUpdateMask(uint8 slot)
        {
            m_auraUpdateMask |= (uint64(1) << slot);
        }
        void UnsetAuraUpdateMask(uint8 slot)
        {
            m_auraUpdateMask &= ~(uint64(1) << slot);
        }
        void ResetAuraUpdateMask()
        {
            m_auraUpdateMask = 0;
        }

        DeclinedName const* GetDeclinedNames() const
        {
            return m_declinedname;
        }

        bool    m_removed;                                  // prevent overwrite pet state in DB at next Pet::Update if pet already removed(saved)

        Player* GetOwner()
        {
            return m_owner;
        }
    protected:
        Player* m_owner;
        uint32  m_regenTimer;
        uint32  m_happinessTimer;
        uint32  m_loyaltyTimer;
        PetType m_petType;
        int32   m_duration;                                 // time until unsummon (used mostly for summoned guardians and not used for controlled pets)
        int32   m_loyaltyPoints;
        uint64  m_auraUpdateMask;

        DeclinedName* m_declinedname;

        bool m_wasOutdoors;
        uint32 m_outdoorBonusCheckTimer;

        PetModeFlags m_petModeFlags;
    private:

        // silence hidden overloaded functions warnings below
        using Creature::SaveToDB;
        using Creature::DeleteFromDB;
        
        void SaveToDB(uint32, uint8) override                        // overwrited of Creature::SaveToDB     - don't must be called
        {
            assert(false);
        }
        void DeleteFromDB() override                                 // overwrited of Creature::DeleteFromDB - don't must be called
        {
            assert(false);
        }
};
#endif

