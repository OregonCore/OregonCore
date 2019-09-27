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

#ifndef _PLAYER_H
#define _PLAYER_H

#include "Common.h"
#include "ItemPrototype.h"
#include "Unit.h"
#include "Item.h"

#include "Database/DatabaseEnv.h"
#include "DBCStores.h"
#include "NPCHandler.h"
#include "QuestDef.h"
#include "Group.h"
#include "Bag.h"
#include "WorldSession.h"
#include "CinematicMgr.h"
#include "Pet.h"
#include "MapReference.h"
#include "Utilities/Util.h"                                           // for Tokens typedef
#include "ReputationMgr.h"

#include<string>
#include<vector>

struct Mail;
class Channel;
class DynamicObject;
class Creature;
class Pet;
class PlayerMenu;
class Transport;
class UpdateMask;
class PlayerSocial;
class OutdoorPvP;

typedef std::deque<Mail*> PlayerMails;

#define PLAYER_MAX_SKILLS       127
#define PLAYER_MAX_DAILY_QUESTS 25

// Note: SPELLMOD_* values is aura types in fact
enum SpellModType
{
    SPELLMOD_FLAT         = 107,                            // SPELL_AURA_ADD_FLAT_MODIFIER
    SPELLMOD_PCT          = 108                             // SPELL_AURA_ADD_PCT_MODIFIER
};

// 2^n values, Player::m_isunderwater is a bitmask. These are internal values, they are never send to any client
enum PlayerUnderwaterState
{
    UNDERWATER_NONE                     = 0x00,
    UNDERWATER_INWATER                  = 0x01,             // terrain type is water and player is afflicted by it
    UNDERWATER_INLAVA                   = 0x02,             // terrain type is lava and player is afflicted by it
    UNDERWATER_INSLIME                  = 0x04,             // terrain type is lava and player is afflicted by it
    UNDERWATER_INDARKWATER              = 0x08,             // terrain type is dark water and player is afflicted by it

    UNDERWATER_EXIST_TIMERS             = 0x10
};

enum BuyBankSlotResult
{
    ERR_BANKSLOT_FAILED_TOO_MANY    = 0,
    ERR_BANKSLOT_INSUFFICIENT_FUNDS = 1,
    ERR_BANKSLOT_NOTBANKER          = 2,
    ERR_BANKSLOT_OK                 = 3
};

enum PlayerSpellState
{
    PLAYERSPELL_UNCHANGED = 0,
    PLAYERSPELL_CHANGED   = 1,
    PLAYERSPELL_NEW       = 2,
    PLAYERSPELL_REMOVED   = 3
};

struct PlayerSpell
{
    PlayerSpellState state : 8;
    bool active            : 1;
    bool disabled          : 1;
};

// Spell modifier (used for modify other spells)
struct SpellModifier
{
    SpellModOp   op   : 8;
    SpellModType type : 8;
    int16 charges     : 16;
    int32 value;
    uint64 mask;
    uint32 spellId;
    uint32 effectId;
    Spell const* lastAffected;
};

typedef UNORDERED_MAP<uint16, PlayerSpell> PlayerSpellMap;
typedef std::list<SpellModifier*> SpellModList;

struct SpellCooldown
{
    time_t end;
    uint16 itemid;
};

typedef std::map<uint32, SpellCooldown> SpellCooldowns;

enum TrainerSpellState
{
    TRAINER_SPELL_GREEN = 0,
    TRAINER_SPELL_RED   = 1,
    TRAINER_SPELL_GRAY  = 2
};

enum ActionButtonUpdateState
{
    ACTIONBUTTON_UNCHANGED = 0,
    ACTIONBUTTON_CHANGED   = 1,
    ACTIONBUTTON_NEW       = 2,
    ACTIONBUTTON_DELETED   = 3
};

struct ActionButton
{
    ActionButton() : action(0), type(0), misc(0), uState(ACTIONBUTTON_NEW) {}
    ActionButton(uint16 _action, uint8 _type, uint8 _misc) : action(_action), type(_type), misc(_misc), uState(ACTIONBUTTON_NEW) {}

    uint16 action;
    uint8 type;
    uint8 misc;
    ActionButtonUpdateState uState;
};

enum ActionButtonType
{
    ACTION_BUTTON_SPELL = 0,
    ACTION_BUTTON_MACRO = 64,
    ACTION_BUTTON_CMACRO = 65,
    ACTION_BUTTON_ITEM  = 128
};

#define  MAX_ACTION_BUTTONS 132                             //checked in 2.3.0

typedef std::map<uint8, ActionButton> ActionButtonList;

typedef std::pair<uint16, uint8> CreateSpellPair;

struct PlayerCreateInfoItem
{
    PlayerCreateInfoItem(uint32 id, uint32 amount) : item_id(id), item_amount(amount) {}

    uint32 item_id;
    uint32 item_amount;
};

typedef std::list<PlayerCreateInfoItem> PlayerCreateInfoItems;

struct PlayerClassLevelInfo
{
    PlayerClassLevelInfo() : basehealth(0), basemana(0) {}
    uint16 basehealth;
    uint16 basemana;
};

struct PlayerClassInfo
{
    PlayerClassInfo() : levelInfo(NULL) { }

    PlayerClassLevelInfo* levelInfo;                        //[level-1] 0..MaxPlayerLevel-1
};

struct PlayerLevelInfo
{
    PlayerLevelInfo()
    {
        for (int i = 0; i < MAX_STATS; ++i) stats[i] = 0;
    }

    uint8 stats[MAX_STATS];
};

struct PlayerInfo
{
    // existence checked by displayId != 0             // existence checked by displayId != 0
    PlayerInfo() : mapId(0), areaId(0), positionX(0.0f), positionY(0.0f), positionZ(0.0f), orientation(0.0f), displayId_m(0), displayId_f(0), levelInfo(nullptr) { }

    uint32 mapId;
    uint32 areaId;
    float positionX;
    float positionY;
    float positionZ;
    float orientation;
    uint16 displayId_m;
    uint16 displayId_f;
    PlayerCreateInfoItems item;
    std::list<CreateSpellPair> spell;
    std::list<uint16> action[4];

    PlayerLevelInfo* levelInfo;                             //[level-1] 0..MaxPlayerLevel-1
};

struct PvPInfo
{
    PvPInfo() : inHostileArea(false), inNoPvPArea(false), inFFAPvPArea(false), endTimer(0) {}

    bool inHostileArea;
    bool inNoPvPArea;
    bool inFFAPvPArea;
    time_t endTimer;
};

struct DuelInfo
{
    DuelInfo() : initiator(NULL), opponent(NULL), startTimer(0), startTime(0), outOfBound(0) {}

    Player* initiator;
    Player* opponent;
    time_t startTimer;
    time_t startTime;
    time_t outOfBound;
};

struct Areas
{
    uint32 areaID;
    uint32 areaFlag;
    float x1;
    float x2;
    float y1;
    float y2;
};

struct EnchantDuration
{
    EnchantDuration() : item(NULL), slot(MAX_ENCHANTMENT_SLOT), leftduration(0) {};
    EnchantDuration(Item* _item, EnchantmentSlot _slot, uint32 _leftduration) : item(_item), slot(_slot), leftduration(_leftduration)
    {
        assert(item);
    };

    Item* item;
    EnchantmentSlot slot;
    uint32 leftduration;
};

typedef std::list<EnchantDuration> EnchantDurationList;
typedef std::list<Item*> ItemDurationList;

struct LookingForGroupSlot
{
    LookingForGroupSlot() : entry(0), type(0) {}
    bool Empty() const
    {
        return !entry && !type;
    }
    void Clear()
    {
        entry = 0;
        type = 0;
    }
    void Set(uint32 _entry, uint32 _type)
    {
        entry = _entry;
        type = _type;
    }
    bool Is(uint32 _entry, uint32 _type) const
    {
        return entry == _entry && type == _type;
    }
    bool canAutoJoin() const
    {
        return entry && (type == 1 || type == 5);
    }

    uint32 entry;
    uint32 type;
};

#define MAX_LOOKING_FOR_GROUP_SLOT 3

struct LookingForGroup
{
    LookingForGroup() {}
    bool HaveInSlot(LookingForGroupSlot const& slot) const
    {
        return HaveInSlot(slot.entry, slot.type);
    }
    bool HaveInSlot(uint32 _entry, uint32 _type) const
    {
        for (int i = 0; i < MAX_LOOKING_FOR_GROUP_SLOT; ++i)
            if (slots[i].Is(_entry, _type))
                return true;
        return false;
    }

    bool canAutoJoin() const
    {
        for (int i = 0; i < MAX_LOOKING_FOR_GROUP_SLOT; ++i)
            if (slots[i].canAutoJoin())
                return true;
        return false;
    }

    bool Empty() const
    {
        for (int i = 0; i < MAX_LOOKING_FOR_GROUP_SLOT; ++i)
            if (!slots[i].Empty())
                return false;
        return more.Empty();
    }

    LookingForGroupSlot slots[MAX_LOOKING_FOR_GROUP_SLOT];
    LookingForGroupSlot more;
    std::string comment;
};

enum PlayerMovementType
{
    MOVE_ROOT       = 1,
    MOVE_UNROOT     = 2,
    MOVE_WATER_WALK = 3,
    MOVE_LAND_WALK  = 4
};

enum DrunkenState
{
    DRUNKEN_SOBER   = 0,
    DRUNKEN_TIPSY   = 1,
    DRUNKEN_DRUNK   = 2,
    DRUNKEN_SMASHED = 3
};

enum PlayerFlags
{
    PLAYER_FLAGS_GROUP_LEADER   = 0x00000001,
    PLAYER_FLAGS_AFK            = 0x00000002,
    PLAYER_FLAGS_DND            = 0x00000004,
    PLAYER_FLAGS_GM             = 0x00000008,
    PLAYER_FLAGS_GHOST          = 0x00000010,
    PLAYER_FLAGS_RESTING        = 0x00000020,
    PLAYER_FLAGS_FFA_PVP        = 0x00000080,
    PLAYER_FLAGS_CONTESTED_PVP  = 0x00000100,               // Player has been involved in a PvP combat and will be attacked by contested guards
    PLAYER_FLAGS_IN_PVP         = 0x00000200,
    PLAYER_FLAGS_HIDE_HELM      = 0x00000400,
    PLAYER_FLAGS_HIDE_CLOAK     = 0x00000800,
    PLAYER_FLAGS_UNK1           = 0x00001000,               // played long time
    PLAYER_FLAGS_UNK2           = 0x00002000,               // played too long time
    PLAYER_FLAGS_UNK3           = 0x00008000,               // strange visual effect (2.0.1), looks like PLAYER_FLAGS_GHOST flag
    PLAYER_FLAGS_SANCTUARY      = 0x00010000,               // player entered sanctuary
    PLAYER_FLAGS_TAXI_BENCHMARK = 0x00020000,               // taxi benchmark mode (on/off) (2.0.1)
    PLAYER_FLAGS_PVP_TIMER      = 0x00040000,               // 2.0.8...
};

// used for PLAYER__FIELD_KNOWN_TITLES field (uint64), (1<<bit_index) without (-1)
// can't use enum for uint64 values
#define PLAYER_TITLE_DISABLED              UI64LIT(0x0000000000000000)
#define PLAYER_TITLE_NONE                  UI64LIT(0x0000000000000001)
#define PLAYER_TITLE_PRIVATE               UI64LIT(0x0000000000000002) // 1
#define PLAYER_TITLE_CORPORAL              UI64LIT(0x0000000000000004) // 2
#define PLAYER_TITLE_SERGEANT_A            UI64LIT(0x0000000000000008) // 3
#define PLAYER_TITLE_MASTER_SERGEANT       UI64LIT(0x0000000000000010) // 4
#define PLAYER_TITLE_SERGEANT_MAJOR        UI64LIT(0x0000000000000020) // 5
#define PLAYER_TITLE_KNIGHT                UI64LIT(0x0000000000000040) // 6
#define PLAYER_TITLE_KNIGHT_LIEUTENANT     UI64LIT(0x0000000000000080) // 7
#define PLAYER_TITLE_KNIGHT_CAPTAIN        UI64LIT(0x0000000000000100) // 8
#define PLAYER_TITLE_KNIGHT_CHAMPION       UI64LIT(0x0000000000000200) // 9
#define PLAYER_TITLE_LIEUTENANT_COMMANDER  UI64LIT(0x0000000000000400) // 10
#define PLAYER_TITLE_COMMANDER             UI64LIT(0x0000000000000800) // 11
#define PLAYER_TITLE_MARSHAL               UI64LIT(0x0000000000001000) // 12
#define PLAYER_TITLE_FIELD_MARSHAL         UI64LIT(0x0000000000002000) // 13
#define PLAYER_TITLE_GRAND_MARSHAL         UI64LIT(0x0000000000004000) // 14
#define PLAYER_TITLE_SCOUT                 UI64LIT(0x0000000000008000) // 15
#define PLAYER_TITLE_GRUNT                 UI64LIT(0x0000000000010000) // 16
#define PLAYER_TITLE_SERGEANT_H            UI64LIT(0x0000000000020000) // 17
#define PLAYER_TITLE_SENIOR_SERGEANT       UI64LIT(0x0000000000040000) // 18
#define PLAYER_TITLE_FIRST_SERGEANT        UI64LIT(0x0000000000080000) // 19
#define PLAYER_TITLE_STONE_GUARD           UI64LIT(0x0000000000100000) // 20
#define PLAYER_TITLE_BLOOD_GUARD           UI64LIT(0x0000000000200000) // 21
#define PLAYER_TITLE_LEGIONNAIRE           UI64LIT(0x0000000000400000) // 22
#define PLAYER_TITLE_CENTURION             UI64LIT(0x0000000000800000) // 23
#define PLAYER_TITLE_CHAMPION              UI64LIT(0x0000000001000000) // 24
#define PLAYER_TITLE_LIEUTENANT_GENERAL    UI64LIT(0x0000000002000000) // 25
#define PLAYER_TITLE_GENERAL               UI64LIT(0x0000000004000000) // 26
#define PLAYER_TITLE_WARLORD               UI64LIT(0x0000000008000000) // 27
#define PLAYER_TITLE_HIGH_WARLORD          UI64LIT(0x0000000010000000) // 28
#define PLAYER_TITLE_GLADIATOR             UI64LIT(0x0000000020000000) // 29
#define PLAYER_TITLE_DUELIST               UI64LIT(0x0000000040000000) // 30
#define PLAYER_TITLE_RIVAL                 UI64LIT(0x0000000080000000) // 31
#define PLAYER_TITLE_CHALLENGER            UI64LIT(0x0000000100000000) // 32
#define PLAYER_TITLE_SCARAB_LORD           UI64LIT(0x0000000200000000) // 33
#define PLAYER_TITLE_CONQUEROR             UI64LIT(0x0000000400000000) // 34
#define PLAYER_TITLE_JUSTICAR              UI64LIT(0x0000000800000000) // 35
#define PLAYER_TITLE_CHAMPION_OF_THE_NAARU UI64LIT(0x0000001000000000) // 36
#define PLAYER_TITLE_MERCILESS_GLADIATOR   UI64LIT(0x0000002000000000) // 37
#define PLAYER_TITLE_OF_THE_SHATTERED_SUN  UI64LIT(0x0000004000000000) // 38
#define PLAYER_TITLE_HAND_OF_ADAL          UI64LIT(0x0000008000000000) // 39
#define PLAYER_TITLE_VENGEFUL_GLADIATOR    UI64LIT(0x0000010000000000) // 40

#define KNOWN_TITLES_SIZE   2
#define MAX_TITLE_INDEX     (KNOWN_TITLES_SIZE*64)          // 2 uint64 fields

// used in PLAYER_FIELD_BYTES values
enum PlayerFieldByteFlags
{
    PLAYER_FIELD_BYTE_TRACK_STEALTHED   = 0x00000002,
    PLAYER_FIELD_BYTE_RELEASE_TIMER     = 0x00000008,       // Display time till auto release spirit
    PLAYER_FIELD_BYTE_NO_RELEASE_WINDOW = 0x00000010        // Display no "release spirit" window at all
};

// used in PLAYER_FIELD_BYTES2 values
enum PlayerFieldByte2Flags
{
    PLAYER_FIELD_BYTE2_NONE              = 0x00,
    PLAYER_FIELD_BYTE2_STEALTH           = 0x2000,
    PLAYER_FIELD_BYTE2_INVISIBILITY_GLOW = 0x4000
};

enum ActivateTaxiReplies
{
    ERR_TAXIOK                      = 0,
    ERR_TAXIUNSPECIFIEDSERVERERROR  = 1,
    ERR_TAXINOSUCHPATH              = 2,
    ERR_TAXINOTENOUGHMONEY          = 3,
    ERR_TAXITOOFARAWAY              = 4,
    ERR_TAXINOVENDORNEARBY          = 5,
    ERR_TAXINOTVISITED              = 6,
    ERR_TAXIPLAYERBUSY              = 7,
    ERR_TAXIPLAYERALREADYMOUNTED    = 8,
    ERR_TAXIPLAYERSHAPESHIFTED      = 9,
    ERR_TAXIPLAYERMOVING            = 10,
    ERR_TAXISAMENODE                = 11,
    ERR_TAXINOTSTANDING             = 12
};

enum MirrorTimerType
{
    FATIGUE_TIMER      = 0,
    BREATH_TIMER       = 1,
    FIRE_TIMER         = 2
};
#define MAX_TIMERS      3
#define DISABLED_MIRROR_TIMER   -1

// 2^n values
enum PlayerExtraFlags
{
    // gm abilities
    PLAYER_EXTRA_GM_ON              = 0x0001,
    PLAYER_EXTRA_ACCEPT_WHISPERS    = 0x0004,
    PLAYER_EXTRA_TAXICHEAT          = 0x0008,
    PLAYER_EXTRA_GM_INVISIBLE       = 0x0010,
    PLAYER_EXTRA_GM_CHAT            = 0x0020,               // Show GM badge in chat messages

    // other states
    PLAYER_EXTRA_PVP_DEATH          = 0x0100                // store PvP death status until corpse creating.
};

// 2^n values
enum AtLoginFlags
{
    AT_LOGIN_NONE          = 0,
    AT_LOGIN_RENAME        = 1,
    AT_LOGIN_RESET_SPELLS  = 2,
    AT_LOGIN_RESET_TALENTS = 4
};

typedef std::map<uint32, QuestStatusData> QuestStatusMap;

enum QuestSlotOffsets
{
    QUEST_ID_OFFSET     = 0,
    QUEST_STATE_OFFSET  = 1,
    QUEST_COUNTS_OFFSET = 2,
    QUEST_TIME_OFFSET   = 3
};

#define MAX_QUEST_OFFSET 4

enum QuestSlotStateMask
{
    QUEST_STATE_NONE     = 0x0000,
    QUEST_STATE_COMPLETE = 0x0001,
    QUEST_STATE_FAIL     = 0x0002
};

enum SkillUpdateState
{
    SKILL_UNCHANGED     = 0,
    SKILL_CHANGED       = 1,
    SKILL_NEW           = 2,
    SKILL_DELETED       = 3
};

struct SkillStatusData
{
    SkillStatusData(uint8 _pos, SkillUpdateState _uState) : pos(_pos), uState(_uState)
    {
    }
    uint8 pos;
    SkillUpdateState uState;
};

typedef UNORDERED_MAP<uint32, SkillStatusData> SkillStatusMap;

class Quest;
class Spell;
class Item;
class WorldSession;

enum PlayerSlots
{
    // first slot for item stored (in any way in player m_items data)
    PLAYER_SLOT_START           = 0,
    // last+1 slot for item stored (in any way in player m_items data)
    PLAYER_SLOT_END             = 118,
    PLAYER_SLOTS_COUNT          = (PLAYER_SLOT_END - PLAYER_SLOT_START)
};

enum EquipmentSlots
{
    EQUIPMENT_SLOT_START        = 0,
    EQUIPMENT_SLOT_HEAD         = 0,
    EQUIPMENT_SLOT_NECK         = 1,
    EQUIPMENT_SLOT_SHOULDERS    = 2,
    EQUIPMENT_SLOT_BODY         = 3,
    EQUIPMENT_SLOT_CHEST        = 4,
    EQUIPMENT_SLOT_WAIST        = 5,
    EQUIPMENT_SLOT_LEGS         = 6,
    EQUIPMENT_SLOT_FEET         = 7,
    EQUIPMENT_SLOT_WRISTS       = 8,
    EQUIPMENT_SLOT_HANDS        = 9,
    EQUIPMENT_SLOT_FINGER1      = 10,
    EQUIPMENT_SLOT_FINGER2      = 11,
    EQUIPMENT_SLOT_TRINKET1     = 12,
    EQUIPMENT_SLOT_TRINKET2     = 13,
    EQUIPMENT_SLOT_BACK         = 14,
    EQUIPMENT_SLOT_MAINHAND     = 15,
    EQUIPMENT_SLOT_OFFHAND      = 16,
    EQUIPMENT_SLOT_RANGED       = 17,
    EQUIPMENT_SLOT_TABARD       = 18,
    EQUIPMENT_SLOT_END          = 19
};

enum InventorySlots                                         // 4 slots
{
    INVENTORY_SLOT_BAG_0        = 255,
    INVENTORY_SLOT_BAG_START    = 19,
    INVENTORY_SLOT_BAG_END      = 23,
};

enum InventoryPackSlots                                     // 16 slots
{
    INVENTORY_SLOT_ITEM_START   = 23,
    INVENTORY_SLOT_ITEM_END     = 39
};

enum BankItemSlots                                          // 28 slots
{
    BANK_SLOT_ITEM_START        = 39,
    BANK_SLOT_ITEM_END          = 67
};

enum BankBagSlots                                           // 7 slots
{
    BANK_SLOT_BAG_START         = 67,
    BANK_SLOT_BAG_END           = 74
};

enum BuyBackSlots                                           // 12 slots
{
    // stored in m_buybackitems
    BUYBACK_SLOT_START          = 74,
    BUYBACK_SLOT_END            = 86
};

enum KeyRingSlots                                           // 32 slots
{
    KEYRING_SLOT_START          = 86,
    KEYRING_SLOT_END            = 118
};

struct ItemPosCount
{
    ItemPosCount(uint16 _pos, uint8 _count) : pos(_pos), count(_count) {}
    bool isContainedIn(std::vector<ItemPosCount> const& vec) const;
    uint16 pos;
    uint8 count;
};
typedef std::vector<ItemPosCount> ItemPosCountVec;

enum TradeSlots
{
    TRADE_SLOT_COUNT            = 7,
    TRADE_SLOT_TRADED_COUNT     = 6,
    TRADE_SLOT_NONTRADED        = 6
};

enum TransferAbortReason
{
    TRANSFER_ABORT_NONE                 = 0x0000,
    TRANSFER_ABORT_MAX_PLAYERS          = 0x0001,           // Transfer Aborted: instance is full
    TRANSFER_ABORT_NOT_FOUND            = 0x0002,           // Transfer Aborted: instance not found
    TRANSFER_ABORT_TOO_MANY_INSTANCES   = 0x0003,           // You have entered too many instances recently.
    TRANSFER_ABORT_ZONE_IN_COMBAT       = 0x0005,           // Unable to zone in while an encounter is in progress.
    TRANSFER_ABORT_INSUF_EXPAN_LVL1     = 0x0106,           // You must have TBC expansion installed to access this area.
    TRANSFER_ABORT_DIFFICULTY1          = 0x0007,           // Normal difficulty mode is not available for %s.
    TRANSFER_ABORT_DIFFICULTY2          = 0x0107,           // Heroic difficulty mode is not available for %s.
};

enum InstanceResetWarningType
{
    RAID_INSTANCE_WARNING_HOURS     = 1,                    // WARNING! %s is scheduled to reset in %d hour(s).
    RAID_INSTANCE_WARNING_MIN       = 2,                    // WARNING! %s is scheduled to reset in %d minute(s)!
    RAID_INSTANCE_WARNING_MIN_SOON  = 3,                    // WARNING! %s is scheduled to reset in %d minute(s). Please exit the zone or you will be returned to your bind location!
    RAID_INSTANCE_WELCOME           = 4                     // Welcome to %s. This raid instance is scheduled to reset in %s.
};

class InstanceSave;

// PLAYER_FIELD_ARENA_TEAM_INFO_1_1 offsets
enum ArenaTeamInfoType
{
    ARENA_TEAM_ID               = 0,
    ARENA_TEAM_MEMBER           = 1,                        // 0 - captain, 1 - member
    ARENA_TEAM_GAMES_WEEK       = 2,
    ARENA_TEAM_GAMES_SEASON     = 3,
    ARENA_TEAM_WINS_SEASON      = 4,
    ARENA_TEAM_PERSONAL_RATING  = 5,
    ARENA_TEAM_END              = 6
};

enum RestFlag
{
    REST_FLAG_IN_TAVERN         = 0x1,
    REST_FLAG_IN_CITY           = 0x2,
    REST_FLAG_IN_FACTION_AREA   = 0x4, // used with AREA_FLAG_REST_ZONE_*
};

enum DuelCompleteType
{
    DUEL_INTERUPTED = 0,
    DUEL_WON        = 1,
    DUEL_FLED       = 2
};

enum TeleportToOptions
{
    TELE_TO_GM_MODE             = 0x01,
    TELE_TO_NOT_LEAVE_TRANSPORT = 0x02,
    TELE_TO_NOT_LEAVE_COMBAT    = 0x04,
    TELE_TO_NOT_UNSUMMON_PET    = 0x08,
    TELE_TO_SPELL               = 0x10,
};

// Type of environmental damages
enum EnviromentalDamage
{
    DAMAGE_EXHAUSTED = 0,
    DAMAGE_DROWNING  = 1,
    DAMAGE_FALL      = 2,
    DAMAGE_LAVA      = 3,
    DAMAGE_SLIME     = 4,
    DAMAGE_FIRE      = 5,
    DAMAGE_FALL_TO_VOID = 6                                 // custom case for fall without durability loss
};

enum PlayerChatTag
{
    CHAT_TAG_NONE       = 0x00,
    CHAT_TAG_AFK        = 0x01,
    CHAT_TAG_DND        = 0x02,
    CHAT_TAG_GM         = 0x04,
    CHAT_TAG_COM        = 0x08, // [Unused WotLK] Commentator
    CHAT_TAG_DEV        = 0x10 //  [Unused WotLK] 
};

enum PlayedTimeIndex
{
    PLAYED_TIME_TOTAL = 0,
    PLAYED_TIME_LEVEL = 1
};

#define MAX_PLAYED_TIME_INDEX 2

// used at player loading query list preparing, and later result selection
enum PlayerLoginQueryIndex
{
    PLAYER_LOGIN_QUERY_LOADFROM                 = 0,
    PLAYER_LOGIN_QUERY_LOADGROUP                = 1,
    PLAYER_LOGIN_QUERY_LOADBOUNDINSTANCES       = 2,
    PLAYER_LOGIN_QUERY_LOADAURAS                = 3,
    PLAYER_LOGIN_QUERY_LOADSPELLS               = 4,
    PLAYER_LOGIN_QUERY_LOADQUESTSTATUS          = 5,
    PLAYER_LOGIN_QUERY_LOADDAILYQUESTSTATUS     = 6,
    PLAYER_LOGIN_QUERY_LOADTUTORIALS            = 7,        // common for all characters for some account at specific realm
    PLAYER_LOGIN_QUERY_LOADREPUTATION           = 8,
    PLAYER_LOGIN_QUERY_LOADINVENTORY            = 9,
    PLAYER_LOGIN_QUERY_LOADACTIONS              = 10,
    PLAYER_LOGIN_QUERY_LOADMAILCOUNT            = 11,
    PLAYER_LOGIN_QUERY_LOADMAILDATE             = 12,
    PLAYER_LOGIN_QUERY_LOADSOCIALLIST           = 13,
    PLAYER_LOGIN_QUERY_LOADHOMEBIND             = 14,
    PLAYER_LOGIN_QUERY_LOADSPELLCOOLDOWNS       = 15,
    PLAYER_LOGIN_QUERY_LOADDECLINEDNAMES        = 16,
    PLAYER_LOGIN_QUERY_LOADGUILD                = 17,
    PLAYER_LOGIN_QUERY_LOADARENAINFO            = 18,
    PLAYER_LOGIN_QUERY_LOADBGDATA               = 19,
    PLAYER_LOGIN_QUERY_LOADSKILLS               = 20,

    MAX_PLAYER_LOGIN_QUERY
};

enum PlayerDelayedOperations
{
    DELAYED_SAVE_PLAYER         = 0x01,
    DELAYED_RESURRECT_PLAYER    = 0x02,
    DELAYED_SPELL_CAST_DESERTER = 0x04,
    DELAYED_BG_MOUNT_RESTORE    = 0x08,                     // Flag to restore mount state after teleport from BG
    DELAYED_BG_TAXI_RESTORE     = 0x10,                     // Flag to restore taxi state after teleport from BG
    DELAYED_GROUP_RESTORE       = 0x20,                     // Flag to tell player he's in a group (client would crash if this is sent at loading screen)
    DELAYED_END
};

// Player summoning auto-decline time (in secs)
#define MAX_PLAYER_SUMMON_DELAY                   (2*MINUTE)
#define MAX_MONEY_AMOUNT                       (0x7FFFFFFF-1)

struct InstancePlayerBind
{
    InstanceSave* save;
    bool perm;
    /* permanent PlayerInstanceBinds are created in Raid/Heroic instances for players
       that aren't already permanently bound when they are inside when a boss is killed
       or when they enter an instance that the group leader is permanently bound to. */
    InstancePlayerBind() : save(NULL), perm(false) {}
};

struct AccessRequirement
{
    uint8  levelMin;
    uint8  levelMax;
    uint32 item;
    uint32 item2;
    uint32 heroicKey;
    uint32 heroicKey2;
    uint32 quest;
    std::string questFailedText;
    uint32 heroicQuest;
    std::string heroicQuestFailedText;
};

enum CharDeleteMethod
{
    CHAR_DELETE_REMOVE = 0,                      // Completely remove from the database
    CHAR_DELETE_UNLINK = 1                       // The character gets unlinked from the account,
                         // the name gets freed up and appears as deleted ingame
};

enum PlayerRestState
{
    REST_STATE_RESTED       = 1, // 200% xp
    REST_STATE_NORMAL       = 2, // 100% xp
    REST_STATE_TIRED_1      = 3, // 100% xp
    REST_STATE_TIRED_2      = 4, //  50% xp
    REST_STATE_EXHAUSTED    = 5, //  25% xp
    REST_STATE_RAF_LINKED   = 6  // 200% xp
};

enum ReferFriendError
{
    RAF_ERR_OK                = 0x00,
    RAF_ERR_BAD_REFERRER      = 0x01, // You were not referred by that player
    RAF_ERR_LVL_TOO_HIGH      = 0x02, // That player's level is too high
    RAF_ERR_NO_GRANTABLE_LVLS = 0x03, // You have not earned enough levels to grant any more levels
    RAF_ERR_TOO_FAR_AWAY      = 0x04, // That player is too far away
    RAF_ERR_OPPOSITE_FACTION  = 0x05, // No cannot grant level to a character of the opposite faction
    RAF_ERR_NOT_RIGHT_NOW     = 0x06, // You cannot grant that player a level right now
    RAF_ERR_ONLY_UNTIL_60     = 0x07, // You cannot grant levels to players level 60 or higher
    RAF_ERR_NO_TARGET         = 0x08, // You have no target
    RAF_ERR_NOT_IN_PARTY      = 0x09, // %s is not in your party
    RAF_ERR_ONLY_UNTIL_60_2   = 0x0A, // You cannot summon players above level 60
    RAF_ERR_SUMMON_NOT_READY  = 0x0B, // You can only summon your friend once per hour
    RAF_ERR_BUDDY_OFFLINE     = 0x0C  // %s is offline and cannot be summoned
};

enum RAFLinkStatus
{
    RAF_LINK_NONE       = 0,
    RAF_LINK_REFERRER   = 1,
    RAF_LINK_REFERRED   = 2
};

enum PlayerCommandStates
{
    CHEAT_NONE      = 0x00,
    CHEAT_GOD       = 0x01,
    CHEAT_CASTTIME  = 0x02,
    CHEAT_COOLDOWN  = 0x04,
    CHEAT_POWER     = 0x08,
    CHEAT_WATERWALK = 0x10
};

class KillRewarder
{
public:
    KillRewarder(Player* killer, Unit* victim, bool isBattleGround);

    void Reward();

private:
    void _InitXP(Player* player);
    void _InitGroupData();

    void _RewardHonor(Player* player);
    void _RewardXP(Player* player, float rate);
    void _RewardReputation(Player* player, float rate);
    void _RewardKillCredit(Player* player);
    void _RewardPlayer(Player* player, bool isDungeon);
    void _RewardGroup();

    Player* _killer;
    Unit* _victim;
    Group* _group;
    float _groupRate;
    Player* _maxNotGrayMember;
    uint32 _count;
    uint32 _sumLevel;
    uint32 _xp;
    bool _isFullXP;
    uint8 _maxLevel;
    bool _isBattleGround;
    bool _isPvP;
};

class PlayerTaxi
{
    public:
        PlayerTaxi();
        ~PlayerTaxi() {}
        // Nodes
        void InitTaxiNodesForLevel(uint32 race, uint32 level);
        void LoadTaxiMask(const char* data);
        void SaveTaxiMask(const char* data);

        uint32 GetTaximask(uint8 index) const
        {
            return m_taximask[index];
        }
        bool IsTaximaskNodeKnown(uint32 nodeidx) const
        {
            uint8  field   = uint8((nodeidx - 1) / 32);
            uint32 submask = 1 << ((nodeidx - 1) % 32);
            return (m_taximask[field] & submask) == submask;
        }
        bool SetTaximaskNode(uint32 nodeidx)
        {
            uint8  field   = uint8((nodeidx - 1) / 32);
            uint32 submask = 1 << ((nodeidx - 1) % 32);
            if ((m_taximask[field] & submask) != submask)
            {
                m_taximask[field] |= submask;
                return true;
            }
            else
                return false;
        }
        void AppendTaximaskTo(ByteBuffer& data, bool all);

        // Destinations
        bool LoadTaxiDestinationsFromString(const std::string& values);
        std::string SaveTaxiDestinationsToString();

        void ClearTaxiDestinations()
        {
            m_TaxiDestinations.clear();
        }
        void AddTaxiDestination(uint32 dest)
        {
            m_TaxiDestinations.push_back(dest);
        }
        uint32 GetTaxiSource() const
        {
            return m_TaxiDestinations.empty() ? 0 : m_TaxiDestinations.front();
        }
        uint32 GetTaxiDestination() const
        {
            return m_TaxiDestinations.size() < 2 ? 0 : m_TaxiDestinations[1];
        }
        uint32 GetCurrentTaxiPath() const;
        uint32 NextTaxiDestination()
        {
            m_TaxiDestinations.pop_front();
            return GetTaxiDestination();
        }

        bool empty() const { return m_TaxiDestinations.empty(); }
    private:
        TaxiMask m_taximask;
        std::deque<uint32> m_TaxiDestinations;
};

class Player;

// Holder for Battleground data
struct BGData
{
    BGData() : bgInstanceID(0), bgTypeID(0), bgAfkReportedCount(0), bgAfkReportedTimer(0),
        bgTeam(0), mountSpell(0)
    {
        ClearTaxiPath();
    }

    uint32 bgInstanceID;                    // This variable is set to bg->m_InstanceID,
    //  when player is teleported to BG - (it is battleground's GUID)
    uint32 bgTypeID;

    std::set<uint32>   bgAfkReporter;
    uint8              bgAfkReportedCount;
    time_t             bgAfkReportedTimer;

    uint32 bgTeam;                          // What side the player will be added to

    uint32 mountSpell;
    uint32 taxiPath[2];

    WorldLocation joinPos;                  // From where player entered BG

    void ClearTaxiPath()
    {
        taxiPath[0] = taxiPath[1] = 0;
    }
    bool HasTaxiPath() const
    {
        return taxiPath[0] && taxiPath[1];
    }
};

class Player : public Unit, public GridObject<Player>
{
        friend class WorldSession;
        friend class CinematicMgr;
        friend class RegressionTestSuite;

        friend void Item::AddToUpdateQueueOf(Player* player);
        friend void Item::RemoveFromUpdateQueueOf(Player* player);
    public:
        explicit Player(WorldSession* session);
        ~Player() override;

        void CleanupsBeforeDelete() override;

        static UpdateMask updateVisualBits;
        static void InitVisibleBits();

        void AddToWorld() override;
        void RemoveFromWorld() override;

        void SetObjectScale(float scale)
        {
            Unit::SetObjectScale(scale);
            SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, scale * DEFAULT_WORLD_OBJECT_SIZE);
            SetFloatValue(UNIT_FIELD_COMBATREACH, scale * DEFAULT_COMBAT_REACH);
        }

        bool TeleportTo(uint32 mapid, float x, float y, float z, float orientation, uint32 options = 0);
        bool TeleportTo(WorldLocation const &loc, uint32 options = 0);
        bool TeleportToBGEntryPoint();

        void SetSummonPoint(uint32 mapid, float x, float y, float z);
        void SummonIfPossible(bool agree);

        bool Create(uint32 guidlow, const std::string& name, uint8 race, uint8 class_, uint8 gender, uint8 skin, uint8 face, uint8 hairStyle, uint8 hairColor, uint8 facialHair, uint8 outfitId);

        void Update(uint32 time) override;

        static bool BuildEnumData(QueryResult_AutoPtr result, WorldPacket* data);

        void SetInWater(bool apply);

        bool IsInWater() const override { return m_isInWater; }
        bool IsUnderWater() const override;
        bool IsFalling() { return GetPositionZ() < m_lastFallZ; }
        bool IsInAreaTriggerRadius(const AreaTriggerEntry* trigger) const;

        void SendInitialPacketsBeforeAddToMap();
        void SendInitialPacketsAfterAddToMap();
        void SendTransferAborted(uint32 mapid, uint16 reason);
        void SendInstanceResetWarning(uint32 mapid, uint32 time);

        bool CanInteractWithQuestGiver(Object* questGiver);
        Creature* GetNPCIfCanInteractWith(uint64 guid, uint32 npcflagmask);
        GameObject* GetGameObjectIfCanInteractWith(uint64 guid, GameobjectTypes type) const;

        void ToggleAFK();
        void ToggleDND();
        bool isAFK() const { return HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_AFK); }
        bool isDND() const { return HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_DND); }
        uint8 GetChatTag() const;
        std::string autoReplyMsg;

        PlayerSocial* GetSocial()
        {
            return m_social;
        }

        PlayerTaxi m_taxi;
        void InitTaxiNodesForLevel()
        {
            m_taxi.InitTaxiNodesForLevel(getRace(), getLevel());
        }
        bool ActivateTaxiPathTo(std::vector<uint32> const& nodes, uint32 mount_id = 0 , Creature* npc = NULL);
        bool ActivateTaxiPathTo(uint32 taxi_path_id);
        void CleanupAfterTaxiFlight();
        void ContinueTaxiFlight();
        // mount_id can be used in scripting calls
        bool isAcceptWhispers() const
        {
            return m_ExtraFlags & PLAYER_EXTRA_ACCEPT_WHISPERS;
        }
        void SetAcceptWhispers(bool on)
        {
            if (on) m_ExtraFlags |= PLAYER_EXTRA_ACCEPT_WHISPERS;
            else m_ExtraFlags &= ~PLAYER_EXTRA_ACCEPT_WHISPERS;
        }
        bool IsGameMaster() const
        {
            return m_ExtraFlags & PLAYER_EXTRA_GM_ON;
        }
        void SetGameMaster(bool on);
        bool isGMChat() const
        {
            return GetSession()->GetSecurity() >= SEC_MODERATOR && (m_ExtraFlags & PLAYER_EXTRA_GM_CHAT);
        }
        void SetGMChat(bool on)
        {
            if (on) m_ExtraFlags |= PLAYER_EXTRA_GM_CHAT;
            else m_ExtraFlags &= ~PLAYER_EXTRA_GM_CHAT;
        }
        bool isTaxiCheater() const
        {
            return m_ExtraFlags & PLAYER_EXTRA_TAXICHEAT;
        }
        void SetTaxiCheater(bool on)
        {
            if (on) m_ExtraFlags |= PLAYER_EXTRA_TAXICHEAT;
            else m_ExtraFlags &= ~PLAYER_EXTRA_TAXICHEAT;
        }
        bool isGMVisible() const
        {
            return !(m_ExtraFlags & PLAYER_EXTRA_GM_INVISIBLE);
        }
        void SetGMVisible(bool on);
        void SetPvPDeath(bool on)
        {
            if (on) m_ExtraFlags |= PLAYER_EXTRA_PVP_DEATH;
            else m_ExtraFlags &= ~PLAYER_EXTRA_PVP_DEATH;
        }

        void GiveXP(uint32 xp, Unit* victim, bool disableRafBonus = false);
        void GiveLevel(uint32 level, bool ignoreRAF = false);
        void InitStatsForLevel(bool reapplyMods = false);

        // .cheat command related
        bool GetCommandStatus(uint32 command) const { return _activeCheats & command; }
        void SetCommandStatusOn(uint32 command) { _activeCheats |= command; }
        void SetCommandStatusOff(uint32 command) { _activeCheats &= ~command; }

        // Played Time Stuff
        time_t m_logintime;
        time_t m_Last_tick;
        uint32 m_Played_time[MAX_PLAYED_TIME_INDEX];
        uint32 GetTotalPlayedTime()
        {
            return m_Played_time[PLAYED_TIME_TOTAL];
        }
        uint32 GetLevelPlayedTime()
        {
            return m_Played_time[PLAYED_TIME_LEVEL];
        }

        void ResetTimeSync();
        void SendTimeSync();

        void setDeathState(DeathState s) override;                   // overwrite Unit::setDeathState

        float GetRestBonus() const { return m_rest_bonus; }
        void SetRestBonus(float rest_bonus_new);

        bool HasRestFlag(RestFlag restFlag) const { return (_restFlagMask & restFlag) != 0; }
        void SetRestFlag(RestFlag restFlag, uint32 triggerId = 0);
        void RemoveRestFlag(RestFlag restFlag);

        uint32 GetXPRestBonus(uint32 xp);
        uint32 GetInnTriggerId() const { return inn_triggerId; }

        Pet* GetPet() const;
        Pet* SummonPet(uint32 entry, float x, float y, float z, float ang, PetType petType, uint32 despwtime);
        void RemovePet(Pet* pet, PetSaveMode mode, bool returnreagent = false);

        void Say(const std::string& text, const uint32 language);
        void Yell(const std::string& text, const uint32 language);
        void TextEmote(const std::string& text);
        void Whisper(const std::string& text, const uint32 language, uint64 receiver);
        void Whisper(const std::string& text, const uint32 language, Player* receiver);
        void BuildPlayerChat(WorldPacket* data, uint8 msgtype, const std::string& text, uint32 language) const;

        /*********************************************************/
        /***                   STORAGE SYSTEM                 ***/
        /*********************************************************/

        void SetVirtualItemSlot(uint8 i, Item* item);
        void SetSheath(SheathState sheathed) override;             // overwrite Unit version
        uint8 FindEquipSlot(ItemTemplate const* proto, uint32 slot, bool swap) const;
        uint32 GetItemCount(uint32 item, bool inBankAlso = false, Item* skipItem = NULL) const;
        Item* GetItemByGuid(uint64 guid) const;
        Item* GetItemByPos(uint16 pos) const;
        Item* GetItemByPos(uint8 bag, uint8 slot) const;
        Item* GetWeaponForAttack(WeaponAttackType attackType, bool useable = false) const;
        Item* GetShield(bool useable = false) const;
        static uint32 GetAttackBySlot(uint8 slot);        // MAX_ATTACK if not weapon slot
        std::vector<Item* >& GetItemUpdateQueue()
        {
            return m_itemUpdateQueue;
        }
        static bool IsInventoryPos(uint16 pos)
        {
            return IsInventoryPos(pos >> 8, pos & 255);
        }
        static bool IsInventoryPos(uint8 bag, uint8 slot);
        static bool IsEquipmentPos(uint16 pos)
        {
            return IsEquipmentPos(pos >> 8, pos & 255);
        }
        static bool IsEquipmentPos(uint8 bag, uint8 slot);
        static bool IsBagPos(uint16 pos);
        static bool IsBankPos(uint16 pos)
        {
            return IsBankPos(pos >> 8, pos & 255);
        }
        static bool IsBankPos(uint8 bag, uint8 slot);
        bool IsValidPos(uint16 pos)
        {
            return IsBankPos(pos >> 8, pos & 255);
        }
        bool IsValidPos(uint8 bag, uint8 slot);
        uint8 GetBankBagSlotCount() const
        {
            return GetByteValue(PLAYER_BYTES_2, 2);
        }
        bool HasBankBagSlot(uint8 slot) const;
        bool HasItemCount(uint32 item, uint32 count, bool inBankAlso = false) const;
        bool HasItemFitToSpellReqirements(SpellEntry const* spellInfo, Item const* ignoreItem = NULL);
        bool CanCastNoReagents(SpellEntry const* spellInfo) const;
        Item* GetItemOrItemWithGemEquipped(uint32 item) const;
        uint8 CanTakeMoreSimilarItems(Item* pItem) const
        {
            return _CanTakeMoreSimilarItems(pItem->GetEntry(), pItem->GetCount(), pItem);
        }
        uint8 CanTakeMoreSimilarItems(uint32 entry, uint32 count) const
        {
            return _CanTakeMoreSimilarItems(entry, count, NULL);
        }
        uint8 CanStoreNewItem(uint8 bag, uint8 slot, ItemPosCountVec& dest, uint32 item, uint32 count, uint32* no_space_count = NULL) const
        {
            return _CanStoreItem(bag, slot, dest, item, count, NULL, false, no_space_count);
        }
        uint8 CanStoreItem(uint8 bag, uint8 slot, ItemPosCountVec& dest, Item* pItem, bool swap = false) const
        {
            if (!pItem)
                return EQUIP_ERR_ITEM_NOT_FOUND;
            uint32 count = pItem->GetCount();
            return _CanStoreItem(bag, slot, dest, pItem->GetEntry(), count, pItem, swap, NULL);

        }
        uint8 CanStoreItems(Item** pItem, int count) const;
        uint8 CanEquipNewItem(uint8 slot, uint16& dest, uint32 item, bool swap) const;
        uint8 CanEquipItem(uint8 slot, uint16& dest, Item* pItem, bool swap, bool not_loading = true) const;
        uint8 CanUnequipItems(uint32 item, uint32 count) const;
        uint8 CanUnequipItem(uint16 src, bool swap) const;
        uint8 CanBankItem(uint8 bag, uint8 slot, ItemPosCountVec& dest, Item* pItem, bool swap, bool not_loading = true) const;
        uint8 CanUseItem(Item* pItem, bool not_loading = true) const;
        bool HasItemTotemCategory(uint32 TotemCategory) const;
        bool CanUseItem(ItemTemplate const* pItem);
        uint8 CanUseAmmo(uint32 item) const;
        Item* StoreNewItem(ItemPosCountVec const& pos, uint32 item, bool update, int32 randomPropertyId = 0);
        Item* StoreItem(ItemPosCountVec const& pos, Item* pItem, bool update);
        Item* EquipNewItem(uint16 pos, uint32 item, bool update);
        Item* EquipItem(uint16 pos, Item* pItem, bool update);
        void AutoUnequipOffhandIfNeed();
        bool StoreNewItemInBestSlots(uint32 item_id, uint32 item_count);

        uint8 _CanTakeMoreSimilarItems(uint32 entry, uint32 count, Item* pItem, uint32* no_space_count = NULL) const;
        uint8 _CanStoreItem(uint8 bag, uint8 slot, ItemPosCountVec& dest, uint32 entry, uint32 count, Item* pItem = NULL, bool swap = false, uint32* no_space_count = NULL) const;

        void ApplyEquipCooldown(Item* pItem);
        void SetAmmo(uint32 item);
        void RemoveAmmo();
        float GetAmmoDPS() const
        {
            return m_ammoDPS;
        }
        bool CheckAmmoCompatibility(const ItemTemplate* ammo_proto) const;
        void QuickEquipItem(uint16 pos, Item* pItem);
        void VisualizeItem(uint8 slot, Item* pItem);
        void SetVisibleItemSlot(uint8 slot, Item* pItem);
        Item* BankItem(ItemPosCountVec const& dest, Item* pItem, bool update)
        {
            return StoreItem(dest, pItem, update);
        }
        Item* BankItem(uint16 pos, Item* pItem, bool update);
        void RemoveItem(uint8 bag, uint8 slot, bool update);
        void MoveItemFromInventory(uint8 bag, uint8 slot, bool update);
        // in trade, auction, guild bank, mail....
        void MoveItemToInventory(ItemPosCountVec const& dest, Item* pItem, bool update, bool in_characterInventoryDB = false);
        // in trade, guild bank, mail....
        void RemoveItemDependentAurasAndCasts(Item* pItem);
        void DestroyItem(uint8 bag, uint8 slot, bool update);
        void DestroyItemCount(uint32 item, uint32 count, bool update, bool unequip_check = false);
        void DestroyItemCount(Item* item, uint32& count, bool update);
        void DestroyConjuredItems(bool update);
        void DestroyZoneLimitedItem(bool update, uint32 new_zone);
        void SplitItem(uint16 src, uint16 dst, uint32 count);
        void SwapItem(uint16 src, uint16 dst);
        void AddItemToBuyBackSlot(Item* pItem);
        Item* GetItemFromBuyBackSlot(uint32 slot);
        void RemoveItemFromBuyBackSlot(uint32 slot, bool del);
        uint32 GetMaxKeyringSize() const
        {
            return KEYRING_SLOT_END - KEYRING_SLOT_START;
        }
        void SendEquipError(uint8 msg, Item* pItem, Item* pItem2);
        void SendBuyError(uint8 msg, Creature* pCreature, uint32 item, uint32 param);
        void SendSellError(uint8 msg, Creature* pCreature, uint64 guid, uint32 param);
        void SendReferFriendError(ReferFriendError err, const char* name = 0);
        void AddWeaponProficiency(uint32 newflag)
        {
            m_WeaponProficiency |= newflag;
        }
        void AddArmorProficiency(uint32 newflag)
        {
            m_ArmorProficiency |= newflag;
        }
        uint32 GetWeaponProficiency() const
        {
            return m_WeaponProficiency;
        }
        uint32 GetArmorProficiency() const
        {
            return m_ArmorProficiency;
        }
        bool IsInFeralForm() const
        {
            return m_form == FORM_CAT || m_form == FORM_BEAR || m_form == FORM_DIREBEAR;
        }
        bool IsUseEquippedWeapon(bool mainhand) const
        {
            // disarm applied only to mainhand weapon
            return !IsInFeralForm() && m_form != FORM_GHOSTWOLF && (!mainhand || !HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISARMED));
        }
        void SendNewItem(Item* item, uint32 count, bool received, bool created, bool broadcast = false);
        bool BuyItemFromVendor(uint64 vendorguid, uint32 item, uint8 count, uint8 bag, uint8 slot);

        float GetReputationPriceDiscount(Creature const* pCreature) const;
        Player* GetTrader() const
        {
            return pTrader;
        }
        void ClearTrade();
        void TradeCancel(bool sendback);

        CinematicMgr* GetCinematicMgr() const { return _cinematicMgr; }

        Item* GetItemByTradeSlot(uint8 slot) const
        {
            if (slot < TRADE_SLOT_COUNT && tradeItems[slot])
                return GetItemByGuid(tradeItems[slot]);
            return NULL;
        }

        void UpdateEnchantTime(uint32 time);
        void UpdateItemDuration(uint32 time, bool realtimeonly = false);
        void AddEnchantmentDurations(Item* item);
        void RemoveEnchantmentDurations(Item* item);
        void RemoveEnchantmentDurationsReferences(Item* item);
        void RemoveAllEnchantments(EnchantmentSlot slot, bool arena);
        void AddEnchantmentDuration(Item* item, EnchantmentSlot slot, uint32 duration);
        void ApplyEnchantment(Item* item, EnchantmentSlot slot, bool apply, bool apply_dur = true, bool ignore_condition = false);
        void ApplyEnchantment(Item* item, bool apply);
        void SendEnchantmentDurations();
        void AddItemDurations(Item* item);
        void RemoveItemDurations(Item* item);
        void SendItemDurations();
        void LoadCorpse();
        void LoadPet();

        uint32 GetFullGrantableLevels() const
        {
            return uint32(m_GrantableLevels);
        }
        float GetGrantableLevels() const
        {
            return m_GrantableLevels;
        }
        void  SetGrantableLevels(float amount)
        {
            if (uint32(m_GrantableLevels = amount) > 0)
                SetByteFlag(PLAYER_FIELD_BYTES, 1, 1);
            else
                RemoveByteFlag(PLAYER_FIELD_BYTES, 1, 1);
        }
        float GetReferFriendXPMultiplier() const;

        bool AddItem(uint32 itemId, uint32 count);

        uint32 m_stableSlots;

        /*********************************************************/
        /***                   GOSSIP SYSTEM                  ***/
        /*********************************************************/

        void PrepareGossipMenu(WorldObject* pSource, uint32 menuId = 0);
        void SendPreparedGossip(WorldObject* pSource);
        void OnGossipSelect(WorldObject* pSource, uint32 gossipListId, uint32 menuId);

        uint32 GetGossipTextId(uint32 menuId, WorldObject* source);
        uint32 GetGossipTextId(WorldObject* pSource);
        uint32 GetDefaultGossipMenuForSource(WorldObject* pSource);

        /*********************************************************/
        /***                   QUEST SYSTEM                   ***/
        /*********************************************************/

        // Return player level when QuestLevel is dynamic (-1)
        uint32 GetQuestLevelForPlayer(Quest const* pQuest) const
        {
            return pQuest && (pQuest->GetQuestLevel() > 0) ? pQuest->GetQuestLevel() : getLevel();
        }

        void PrepareQuestMenu(uint64 guid);
        void SendPreparedQuest(uint64 guid);
        bool IsActiveQuest(uint32 quest_id) const;
        Quest const* GetNextQuest(uint64 guid, Quest const* pQuest);
        bool CanSeeStartQuest(Quest const* pQuest);
        bool CanTakeQuest(Quest const* pQuest, bool msg);
        bool CanAddQuest(Quest const* pQuest, bool msg);
        bool CanCompleteQuest(uint32 quest_id);
        bool CanCompleteRepeatableQuest(Quest const* pQuest);
        bool CanRewardQuest(Quest const* pQuest, bool msg);
        bool CanRewardQuest(Quest const* pQuest, uint32 reward, bool msg);
        void AddQuest(Quest const* pQuest, Object* questGiver);
        void CompleteQuest(uint32 quest_id);
        void IncompleteQuest(uint32 quest_id);
        void AbandonQuest(uint32 questId);
        void RewardQuest(Quest const* pQuest, uint32 reward, Object* questGiver, bool announce = true);

        void FailQuest(uint32 questId);
        bool SatisfyQuestSkill(Quest const* qInfo, bool msg) const;
        bool SatisfyQuestLevel(Quest const* qInfo, bool msg);
        bool SatisfyQuestLog(bool msg);
        bool SatisfyQuestPreviousQuest(Quest const* qInfo, bool msg);
        bool SatisfyQuestClass(Quest const* qInfo, bool msg) const;
        bool SatisfyQuestRace(Quest const* qInfo, bool msg);
        bool SatisfyQuestReputation(Quest const* qInfo, bool msg);
        bool SatisfyQuestStatus(Quest const* qInfo, bool msg);
        bool SatisfyQuestConditions(Quest const* qInfo, bool msg);
        bool SatisfyQuestTimed(Quest const* qInfo, bool msg);
        bool SatisfyQuestExclusiveGroup(Quest const* qInfo, bool msg);
        bool SatisfyQuestNextChain(Quest const* qInfo, bool msg);
        bool SatisfyQuestPrevChain(Quest const* qInfo, bool msg);
        bool SatisfyQuestDay(Quest const* qInfo, bool msg);
        bool GiveQuestSourceItem(Quest const* pQuest);
        bool TakeQuestSourceItem(uint32 quest_id, bool msg);
        bool GetQuestRewardStatus(uint32 quest_id) const;
        QuestStatus GetQuestStatus(uint32 quest_id) const;
        void SetQuestStatus(uint32 quest_id, QuestStatus status);

        void SetDailyQuestStatus(uint32 quest_id);
        void ResetDailyQuestStatus();

        uint16 FindQuestSlot(uint32 quest_id) const;
        uint32 GetQuestSlotQuestId(uint16 slot) const
        {
            return GetUInt32Value(PLAYER_QUEST_LOG_1_1 + slot * MAX_QUEST_OFFSET + QUEST_ID_OFFSET);
        }
        uint32 GetQuestSlotState(uint16 slot)   const
        {
            return GetUInt32Value(PLAYER_QUEST_LOG_1_1 + slot * MAX_QUEST_OFFSET + QUEST_STATE_OFFSET);
        }
        uint32 GetQuestSlotCounters(uint16 slot)const
        {
            return GetUInt32Value(PLAYER_QUEST_LOG_1_1 + slot * MAX_QUEST_OFFSET + QUEST_COUNTS_OFFSET);
        }
        uint8 GetQuestSlotCounter(uint16 slot, uint8 counter) const
        {
            return GetByteValue(PLAYER_QUEST_LOG_1_1 + slot * MAX_QUEST_OFFSET + QUEST_COUNTS_OFFSET, counter);
        }
        uint32 GetQuestSlotTime(uint16 slot)    const
        {
            return GetUInt32Value(PLAYER_QUEST_LOG_1_1 + slot * MAX_QUEST_OFFSET + QUEST_TIME_OFFSET);
        }
        void SetQuestSlot(uint16 slot, uint32 quest_id, uint32 timer = 0)
        {
            SetUInt32Value(PLAYER_QUEST_LOG_1_1 + slot * MAX_QUEST_OFFSET + QUEST_ID_OFFSET, quest_id);
            SetUInt32Value(PLAYER_QUEST_LOG_1_1 + slot * MAX_QUEST_OFFSET + QUEST_STATE_OFFSET, 0);
            SetUInt32Value(PLAYER_QUEST_LOG_1_1 + slot * MAX_QUEST_OFFSET + QUEST_COUNTS_OFFSET, 0);
            SetUInt32Value(PLAYER_QUEST_LOG_1_1 + slot * MAX_QUEST_OFFSET + QUEST_TIME_OFFSET, timer);
        }
        void SetQuestSlotCounter(uint16 slot, uint8 counter, uint8 count)
        {
            SetByteValue(PLAYER_QUEST_LOG_1_1 + slot * MAX_QUEST_OFFSET + QUEST_COUNTS_OFFSET, counter, count);
        }
        void SetQuestSlotState(uint16 slot, uint32 state)
        {
            SetFlag(PLAYER_QUEST_LOG_1_1 + slot * MAX_QUEST_OFFSET + QUEST_STATE_OFFSET, state);
        }
        void RemoveQuestSlotState(uint16 slot, uint32 state)
        {
            RemoveFlag(PLAYER_QUEST_LOG_1_1 + slot * MAX_QUEST_OFFSET + QUEST_STATE_OFFSET, state);
        }
        void SetQuestSlotTimer(uint16 slot, uint32 timer)
        {
            SetUInt32Value(PLAYER_QUEST_LOG_1_1 + slot * MAX_QUEST_OFFSET + QUEST_TIME_OFFSET, timer);
        }
        void SwapQuestSlot(uint16 slot1, uint16 slot2)
        {
            for (int i = 0; i < MAX_QUEST_OFFSET; ++i)
            {
                uint32 temp1 = GetUInt32Value(PLAYER_QUEST_LOG_1_1 + MAX_QUEST_OFFSET * slot1 + i);
                uint32 temp2 = GetUInt32Value(PLAYER_QUEST_LOG_1_1 + MAX_QUEST_OFFSET * slot2 + i);

                SetUInt32Value(PLAYER_QUEST_LOG_1_1 + MAX_QUEST_OFFSET * slot1 + i, temp2);
                SetUInt32Value(PLAYER_QUEST_LOG_1_1 + MAX_QUEST_OFFSET * slot2 + i, temp1);
            }
        }
        uint32 GetReqKillOrCastCurrentCount(uint32 quest_id, int32 entry);
        void AdjustQuestReqItemCount(Quest const* pQuest);
        void AreaExploredOrEventHappens(uint32 questId);
        void GroupEventHappens(uint32 questId, WorldObject const* pEventObject);
        void ItemAddedQuestCheck(uint32 entry, uint32 count);
        void ItemRemovedQuestCheck(uint32 entry, uint32 count);
        void KilledMonster(CreatureInfo const* cInfo, uint64 guid);
        void KilledMonsterCredit(uint32 entry, uint64 guid = 0);
        void CastedCreatureOrGO(uint32 entry, uint64 guid, uint32 spell_id);
        void TalkedToCreature(uint32 entry, uint64 guid);
        void MoneyChanged(uint32 value);
        bool HasQuestForItem(uint32 itemid) const;
        bool HasQuestForGO(int32 GOId);
        void UpdateForQuestsGO();
        bool CanShareQuest(uint32 quest_id) const;

        void SendQuestComplete(uint32 quest_id);
        void SendQuestReward(Quest const* pQuest, uint32 XP, Object* questGiver);
        void SendQuestFailed(uint32 quest_id);
        void SendQuestTimerFailed(uint32 quest_id);
        void SendCanTakeQuestResponse(uint32 msg) const;
        void SendQuestConfirmAccept(Quest const* pQuest, Player* pReceiver);
        void SendPushToPartyResponse(Player* pPlayer, uint32 msg);
        void SendQuestUpdateAddItem(Quest const* pQuest, uint32 item_idx, uint32 count);
        void SendQuestUpdateAddCreatureOrGo(Quest const* pQuest, uint64 guid, uint32 creatureOrGO_idx, uint32 old_count, uint32 add_count);

        uint64 GetDivider()
        {
            return m_divider;
        }
        void SetDivider(uint64 guid)
        {
            m_divider = guid;
        }

        uint32 GetInGameTime()
        {
            return m_ingametime;
        }

        void SetInGameTime(uint32 time)
        {
            m_ingametime = time;
        }

        void AddTimedQuest(uint32 quest_id)
        {
            m_timedquests.insert(quest_id);
        }
        void RemoveTimedQuest(uint32 quest_id)
        {
            m_timedquests.erase(quest_id);
        }

        /*********************************************************/
        /***                  LOAD SYSTEM                     ***/
        /*********************************************************/

        bool LoadFromDB(uint32 guid, SqlQueryHolder* holder);
        void Initialize(uint32 guid);
        static bool   LoadValuesArrayFromDB(Tokens& data, uint64 guid);
        static uint32 GetUInt32ValueFromArray(Tokens const& data, uint16 index);
        static float  GetFloatValueFromArray(Tokens const& data, uint16 index);
        static uint32 GetUInt32ValueFromDB(uint16 index, uint64 guid);
        static uint32 GetZoneIdFromDB(uint64 guid);
        static uint32 GetLevelFromDB(uint64 guid);
        static bool   LoadPositionFromDB(uint32& mapid, float& x, float& y, float& z, float& o, bool& in_flight, uint64 guid);

        /*********************************************************/
        /***                  SAVE SYSTEM                     ***/
        /*********************************************************/

        void SaveToDB();
        void SaveInventoryAndGoldToDB();                    // fast save function for item/money cheating preventing
        void SaveGoldToDB();
        void SaveDataFieldToDB();
        static bool SaveValuesArrayInDB(Tokens const& data, uint64 guid);
        static void SetUInt32ValueInArray(Tokens& data, uint16 index, uint32 value);
        static void SetFloatValueInArray(Tokens& data, uint16 index, float value);
        static void SetUInt32ValueInDB(uint16 index, uint32 value, uint64 guid);
        static void SavePositionInDB(uint32 mapid, float x, float y, float z, float o, uint32 zone, uint64 guid);

        static void DeleteFromDB(uint64 playerguid, uint32 accountId, bool updateRealmChars = true, bool deleteFinally = false);
        static void DeleteOldCharacters();
        static void DeleteOldCharacters(uint32 keepDays);

        bool m_mailsLoaded;
        bool m_mailsUpdated;

        void SetBindPoint(uint64 guid);
        void SendTalentWipeConfirm(uint64 guid);
        void RewardRage(uint32 damage, uint32 weaponSpeedHitFactor, bool attacker);
        void SendPetSkillWipeConfirm();
        void CalcRage(uint32 damage, bool attacker);
        void RegenerateAll();
        void Regenerate(Powers power);
        void RegenerateHealth();
        void setRegenTimer(uint32 time)
        {
            m_regenTimer = time;
        }
        void setWeaponChangeTimer(uint32 time)
        {
            m_weaponChangeTimer = time;
        }

        uint32 GetMoney()
        {
            return GetUInt32Value (PLAYER_FIELD_COINAGE);
        }
        void ModifyMoney(int32 d)
        {
            if (d < 0)
                SetMoney (GetMoney() > uint32(-d) ? GetMoney() + d : 0);
            else
                SetMoney (GetMoney() < uint32(MAX_MONEY_AMOUNT - d) ? GetMoney() + d : MAX_MONEY_AMOUNT);

            // "At Gold Limit"
            if (GetMoney() >= MAX_MONEY_AMOUNT)
                SendEquipError(EQUIP_ERR_TOO_MUCH_GOLD, NULL, NULL);
        }
        void SetMoney(uint32 value)
        {
            SetUInt32Value (PLAYER_FIELD_COINAGE, value);
            MoneyChanged(value);
        }

        uint32 GetTutorialInt(uint32 intId)
        {
            ASSERT((intId < 8));
            return m_Tutorials[intId];
        }

        void SetTutorialInt(uint32 intId, uint32 value)
        {
            ASSERT((intId < 8));
            if (m_Tutorials[intId] != value)
            {
                m_Tutorials[intId] = value;
                m_TutorialsChanged = true;
            }
        }

        QuestStatusMap& getQuestStatusMap()
        {
            return mQuestStatus;
        };
        Unit *GetSelectedUnit() const;
        Player *GetSelectedPlayer() const;
        const uint64& GetSelection() const
        {
            return m_curSelection;
        }
        void SetSelection(const uint64& guid)
        {
            m_curSelection = guid;
            SetUInt64Value(UNIT_FIELD_TARGET, guid);
        }

        uint8 GetComboPoints()
        {
            return m_comboPoints;
        }
        uint64 GetComboTarget()
        {
            return m_comboTarget;
        }

        void AddComboPoints(Unit* target, int8 count);
        void ClearComboPoints();
        void SendComboPoints();

        void SendMailResult(uint32 mailId, MailResponseType mailAction, MailResponseResult mailError, uint32 equipError = 0, uint32 item_guid = 0, uint32 item_count = 0);
        void SendNewMail();
        void UpdateNextMailTimeAndUnreads();
        void AddNewMailDeliverTime(time_t deliver_time);
        bool IsMailsLoaded() const
        {
            return m_mailsLoaded;
        }

        //void SetMail(Mail *m);
        void RemoveMail(uint32 id);

        void AddMail(Mail* mail)
        {
            m_mail.push_front(mail);   // for call from WorldSession::SendMailTo
        }
        uint32 GetMailSize()
        {
            return m_mail.size();
        }
        Mail* GetMail(uint32 id);

        PlayerMails::iterator GetmailBegin()
        {
            return m_mail.begin();
        };
        PlayerMails::iterator GetmailEnd()
        {
            return m_mail.end();
        };

        /*********************************************************/
        /*** MAILED ITEMS SYSTEM ***/
        /*********************************************************/

        uint8 unReadMails;
        time_t m_nextMailDelivereTime;

        typedef UNORDERED_MAP<uint32, Item*> ItemMap;

        ItemMap mMitems;                                    //template defined in objectmgr.cpp

        Item* GetMItem(uint32 id)
        {
            ItemMap::const_iterator itr = mMitems.find(id);
            return itr != mMitems.end() ? itr->second : NULL;
        }

        void AddMItem(Item* it)
        {
            ASSERT(it);
            //assert deleted, because items can be added before loading
            mMitems[it->GetGUIDLow()] = it;
        }

        bool RemoveMItem(uint32 id)
        {
            return mMitems.erase(id) ? true : false;
        }

        void PetSpellInitialize();
        void CharmSpellInitialize();
        void PossessSpellInitialize();
        void SendRemoveControlBar();
        bool HasSpell(uint32 spell) const override;
        bool HasActiveSpell(uint32 spell) const;            // shown in spellbook
        TrainerSpellState GetTrainerSpellState(TrainerSpell const* trainer_spell) const;
        bool IsSpellFitByClassAndRace(uint32 spell_id) const;

        void SendProficiency(ItemClass itemClass, uint32 itemSubclassMask);
        void SendInitialSpells();
        bool AddSpell(uint32 spell_id, bool active, bool learning = true, bool loading = false, bool disabled = false);
        void LearnSpell(uint32 spell_id);
        void RemoveSpell(uint32 spell_id, bool disabled = false);
        void ResetSpells();
        void LearnDefaultSpells(bool loading = false);
        void LearnQuestRewardedSpells();
        void LearnQuestRewardedSpells(Quest const* quest);
        void LearnSpellHighestRank(uint32 spellid);
        uint32 GetHighestLearnedRankOf(uint32 spellid) const override;

        uint32 GetFreeTalentPoints() const
        {
            return GetUInt32Value(PLAYER_CHARACTER_POINTS1);
        }
        void SetFreeTalentPoints(uint32 points)
        {
            SetUInt32Value(PLAYER_CHARACTER_POINTS1, points);
        }
        bool ResetTalents(bool no_cost = false);
        uint32 ResetTalentsCost() const;
        void InitTalentForLevel();

        uint32 GetFreePrimaryProfessionPoints() const
        {
            return GetUInt32Value(PLAYER_CHARACTER_POINTS2);
        }
        void SetFreePrimaryProfessions(uint16 profs)
        {
            SetUInt32Value(PLAYER_CHARACTER_POINTS2, profs);
        }
        void InitPrimaryProfessions();

        PlayerSpellMap const& GetSpellMap() const
        {
            return m_spells;
        }
        PlayerSpellMap&       GetSpellMap()
        {
            return m_spells;
        }

        void AddSpellMod(SpellModifier* mod, bool apply);
        int32 GetTotalFlatMods(uint32 spellId, SpellModOp op);
        int32 GetTotalPctMods(uint32 spellId, SpellModOp op);
        bool IsAffectedBySpellmod(SpellEntry const* spellInfo, SpellModifier* mod, Spell const* spell = NULL);
        template <class T> T ApplySpellMod(uint32 spellId, SpellModOp op, T& basevalue, Spell const* spell = NULL);
        void RemoveSpellMods(Spell const* spell);
        void RestoreSpellMods(Spell const* spell);

        bool HasSpellCooldown(uint32 spell_id) const
        {
            SpellCooldowns::const_iterator itr = m_spellCooldowns.find(spell_id);
            return itr != m_spellCooldowns.end() && itr->second.end > time(NULL);
        }
        uint32 GetSpellCooldownDelay(uint32 spell_id) const
        {
            SpellCooldowns::const_iterator itr = m_spellCooldowns.find(spell_id);
            time_t t = time(NULL);
            return itr != m_spellCooldowns.end() && itr->second.end > t ? itr->second.end - t : 0;
        }
        void AddSpellCooldown(uint32 spell_id, uint32 itemid, time_t end_time);
        void SendCooldownEvent(SpellEntry const* spellInfo);
        void ProhibitSpellSchool(SpellSchoolMask idSchoolMask, uint32 unTimeMs) override;
        void RemoveSpellCooldown(uint32 spell_id, bool update = false);
        void SendClearCooldown(uint32 spell_id, Unit* target);
        void RemoveArenaSpellCooldowns();
        void RemoveAllSpellCooldown();
        void _LoadSpellCooldowns(QueryResult_AutoPtr result);
        void _SaveSpellCooldowns();

        // global cooldown
        void AddGlobalCooldown(SpellEntry const* spellInfo, Spell* spell);
        bool HasGlobalCooldown(SpellEntry const* spellInfo) const;
        void RemoveGlobalCooldown(SpellEntry const* spellInfo);

        void setResurrectRequestData(uint64 guid, uint32 mapId, float X, float Y, float Z, uint32 health, uint32 mana)
        {
            m_resurrectGUID = guid;
            m_resurrectMap = mapId;
            m_resurrectX = X;
            m_resurrectY = Y;
            m_resurrectZ = Z;
            m_resurrectHealth = health;
            m_resurrectMana = mana;
        }
        void clearResurrectRequestData()
        {
            setResurrectRequestData(0, 0, 0.0f, 0.0f, 0.0f, 0, 0);
        }
        bool isRessurectRequestedBy(uint64 guid) const
        {
            return m_resurrectGUID == guid;
        }
        bool isRessurectRequested() const
        {
            return m_resurrectGUID != 0;
        }
        void ResurectUsingRequestData();

        int getCinematic()
        {
            return m_cinematic;
        }
        void setCinematic(int cine)
        {
            m_cinematic = cine;
        }

        void addActionButton(uint8 button, uint16 action, uint8 type, uint8 misc);
        void removeActionButton(uint8 button);
        void SendInitialActionButtons();

        PvPInfo pvpInfo;
        void UpdatePvPState(bool onlyFFA = false);
        void SetPvP(bool state)
        {
            Unit::SetPvP(state);
            for (ControlList::iterator itr = m_Controlled.begin(); itr != m_Controlled.end(); ++itr)
                (*itr)->SetPvP(state);
        }
        void UpdatePvP(bool state, bool override = false);
        bool IsFFAPvP() const
        {
            return HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_FFA_PVP);
        }
        void SetFFAPvP(bool state);

        void UpdateZone(uint32 newZone);
        void UpdateArea(uint32 newArea);

        void UpdateZoneDependentAuras(uint32 zone_id);    // zones
        void UpdateAreaDependentAuras(uint32 area_id);    // subzones

        void UpdateAfkReport(time_t currTime);
        void UpdatePvPFlag(time_t currTime);
        void UpdateContestedPvP(uint32 currTime);
        void SetContestedPvPTimer(uint32 newTime)
        {
            m_contestedPvPTimer = newTime;
        }
        void ResetContestedPvP()
        {
            ClearUnitState(UNIT_STATE_ATTACK_PLAYER);
            RemoveFlag(PLAYER_FLAGS, PLAYER_FLAGS_CONTESTED_PVP);
            m_contestedPvPTimer = 0;
        }

        /** @todo -maybe move UpdateDuelFlag+DuelComplete to independent DuelHandler.. **/
        DuelInfo* duel;
        void UpdateDuelFlag(time_t currTime);
        void CheckDuelDistance(time_t currTime);
        void DuelComplete(DuelCompleteType type);

        bool IsGroupVisibleFor(Player const* p) const;
        bool IsInSameGroupWith(Player const* p) const;
        bool IsInSameRaidWith(Player const* p) const
        {
            return p == this || (GetGroup() != NULL && GetGroup() == p->GetGroup());
        }
        void UninviteFromGroup();
        static void RemoveFromGroup(Group* group, uint64 guid, RemoveMethod method = GROUP_REMOVEMETHOD_DEFAULT, uint64 kicker = 0 , const char* reason = NULL);
        void RemoveFromGroup(RemoveMethod method = GROUP_REMOVEMETHOD_DEFAULT) { RemoveFromGroup(GetGroup(),GetGUID(), method); }
        void SendUpdateToOutOfRangeGroupMembers();

        void SetInGuild(uint32 GuildId)
        {
            SetUInt32Value(PLAYER_GUILDID, GuildId);
        }
        void SetRank(uint32 rankId)
        {
            SetUInt32Value(PLAYER_GUILDRANK, rankId);
        }
        void SetGuildIdInvited(uint32 GuildId)
        {
            m_GuildIdInvited = GuildId;
        }
        uint32 GetGuildId()
        {
            return GetUInt32Value(PLAYER_GUILDID);
        }
        static uint32 GetGuildIdFromDB(uint64 guid);
        uint32 GetRank()
        {
            return GetUInt32Value(PLAYER_GUILDRANK);
        }
        static uint32 GetRankFromDB(uint64 guid);
        int GetGuildIdInvited()
        {
            return m_GuildIdInvited;
        }
        static void RemovePetitionsAndSigns(uint64 guid, uint32 type);

        // Arena Team
        void SetInArenaTeam(uint32 ArenaTeamId, uint8 slot)
        {
            SetArenaTeamInfoField(slot, ARENA_TEAM_ID, ArenaTeamId);
        }
        void SetArenaTeamInfoField(uint8 slot, ArenaTeamInfoType type, uint32 value)
        {
            SetUInt32Value(PLAYER_FIELD_ARENA_TEAM_INFO_1_1 + (slot * ARENA_TEAM_END) + type, value);
        }
        uint32 GetArenaTeamId(uint8 slot)
        {
            return GetUInt32Value(PLAYER_FIELD_ARENA_TEAM_INFO_1_1 + (slot * ARENA_TEAM_END) + ARENA_TEAM_ID);
        }
        uint32 GetArenaPersonalRating(uint8 slot)
        {
            return GetUInt32Value(PLAYER_FIELD_ARENA_TEAM_INFO_1_1 + (slot * ARENA_TEAM_END) + ARENA_TEAM_PERSONAL_RATING);
        }
        static uint32 GetArenaTeamIdFromDB(uint64 guid, uint8 slot);
        void SetArenaTeamIdInvited(uint32 ArenaTeamId)
        {
            m_ArenaTeamIdInvited = ArenaTeamId;
        }
        uint32 GetArenaTeamIdInvited()
        {
            return m_ArenaTeamIdInvited;
        }

        void SetDifficulty(DungeonDifficulty dungeon_difficulty)
        {
            m_dungeonDifficulty = dungeon_difficulty;
        }
        DungeonDifficulty GetDifficulty()
        {
            return m_dungeonDifficulty;
        }

        bool UpdateSkill(uint32 skill_id, uint32 step);
        bool UpdateSkillPro(uint16 SkillId, int32 Chance, uint32 step);

        bool UpdateCraftSkill(uint32 spellid);
        bool UpdateGatherSkill(uint32 SkillId, uint32 SkillValue, uint32 RedLevel, uint32 Multiplicator = 1);
        bool UpdateFishingSkill();

        uint32 GetBaseDefenseSkillValue() const
        {
            return GetBaseSkillValue(SKILL_DEFENSE);
        }
        uint32 GetBaseWeaponSkillValue(WeaponAttackType attType) const;

        uint32 GetSpellByProto(ItemTemplate* proto);

        float GetHealthBonusFromStamina();
        float GetManaBonusFromIntellect();

        bool UpdateStats(Stats stat) override;
        bool UpdateAllStats() override;
        void UpdateResistances(uint32 school) override;
        void UpdateArmor() override;
        void UpdateMaxHealth() override;
        void UpdateMaxPower(Powers power) override;
        void UpdateAttackPowerAndDamage(bool ranged = false) override;
        void UpdateShieldBlockValue();
        void UpdateSpellDamageAndHealingBonus();

        void CalculateMinMaxDamage(WeaponAttackType attType, bool normalized, bool addTotalPct, float& minDamage, float& maxDamage) override;

        void UpdateDefenseBonusesMod();
        void ApplyRatingMod(CombatRating cr, int32 value, bool apply);
        float GetMeleeCritFromAgility();
        float GetDodgeFromAgility();
        float GetSpellCritFromIntellect();
        float OCTRegenHPPerSpirit();
        float OCTRegenMPPerSpirit();
        float GetRatingMultiplier(CombatRating cr) const;
        float GetRatingBonusValue(CombatRating cr) const;
        uint32 GetMeleeCritDamageReduction(uint32 damage) const;
        uint32 GetRangedCritDamageReduction(uint32 damage) const;
        uint32 GetSpellCritDamageReduction(uint32 damage) const;
        uint32 GetDotDamageReduction(uint32 damage) const;

        float GetExpertiseDodgeOrParryReduction(WeaponAttackType attType) const;
        void UpdateBlockPercentage();
        void UpdateCritPercentage(WeaponAttackType attType);
        void UpdateAllCritPercentages();
        void UpdateParryPercentage();
        void UpdateDodgePercentage();
        void UpdateMeleeHitChances();
        void UpdateRangedHitChances();
        void UpdateSpellHitChances();
        void UpdateAllSpellCritChances();
        void UpdateSpellCritChance(uint32 school);
        void UpdateExpertise(WeaponAttackType attType);
        void UpdateManaRegen();

        const uint64& GetLootGUID() const
        {
            return m_lootGuid;
        }
        void SetLootGUID(const uint64& guid)
        {
            m_lootGuid = guid;
        }

        void RemovedInsignia(Player* looterPlr);

        WorldSession* GetSession() const
        {
            return m_session;
        }
        void SetSession(WorldSession* s)
        {
            m_session = s;
        }

        void BuildCreateUpdateBlockForPlayer(UpdateData* data, Player* target) const override;
        void DestroyForPlayer(Player* target, bool onDeath = false) const override;
        void SendDelayResponse(const uint32);
        void SendLogXPGain(uint32 GivenXP, Unit* victim, uint32 RestXP, bool RafBonus = false);

		uint8 LastSwingErrorMsg() const { return m_swingErrorMsg; }
		void SwingErrorMsg(uint8 val) { m_swingErrorMsg = val; }

        //notifiers
        void SendAttackSwingCantAttack();
        void SendAttackSwingCancelAttack();
        void SendAttackSwingDeadTarget();
        void SendAttackSwingNotStanding();
        void SendAttackSwingNotInRange();
        void SendAttackSwingBadFacingAttack();
        void SendAutoRepeatCancel();
        void SendExplorationExperience(uint32 Area, uint32 Experience);

        void SendDungeonDifficulty(bool IsInGroup);
        void ResetInstances(uint8 method);
        void SendResetInstanceSuccess(uint32 MapId);
        void SendResetInstanceFailed(uint32 reason, uint32 MapId);
        void SendResetFailedNotify(uint32 mapid);

        bool SetPosition(float x, float y, float z, float orientation, bool teleport = false) override;
        bool SetPosition(const Position& pos, bool teleport = false)
        {
            return SetPosition(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation(), teleport);
        }

        void UpdateUnderwaterState(Map* m, float x, float y, float z);
        void SendMessageToSet(WorldPacket *data, bool self) override {SendMessageToSetInRange(data, GetVisibilityRange(), self); };// overwrite Object::SendMessageToSet
        void SendMessageToSetInRange(WorldPacket* data, float fist, bool self) override;// overwrite Object::SendMessageToSetInRange
        void SendMessageToSetInRange(WorldPacket* data, float dist, bool self, bool own_team_only);

        Corpse* GetCorpse() const;
        void SpawnCorpseBones();
        void CreateCorpse();
        void KillPlayer();
        uint32 GetResurrectionSpellId();
        void ResurrectPlayer(float restore_percent, bool applySickness = false);
        void BuildPlayerRepop();
        void RepopAtGraveyard();

        void DurabilityLossAll(double percent, bool inventory);
        void DurabilityLoss(Item* item, double percent);
        void DurabilityPointsLossAll(int32 points, bool inventory);
        void DurabilityPointsLoss(Item* item, int32 points);
        void DurabilityPointLossForEquipSlot(EquipmentSlots slot);
        uint32 DurabilityRepairAll(bool cost, float discountMod, bool guildBank);
        uint32 DurabilityRepair(uint16 pos, bool cost, float discountMod, bool guildBank);

        void UpdateMirrorTimers();
        void StopMirrorTimers()
        {
            StopMirrorTimer(FATIGUE_TIMER);
            StopMirrorTimer(BREATH_TIMER);
            StopMirrorTimer(FIRE_TIMER);
        }

        void JoinedChannel(Channel* c);
        void LeftChannel(Channel* c);
        void CleanupChannels();
        void UpdateLocalChannels(uint32 newZone);
        void LeaveLFGChannel();

        void UpdateDefense();
        void UpdateWeaponSkill (WeaponAttackType attType);
        void UpdateCombatSkills(Unit* victim, WeaponAttackType attType, MeleeHitOutcome outcome, bool defence);

        void SetSkill(uint32 id, uint16 currVal, uint16 maxVal);
        uint16 GetMaxSkillValue(uint32 skill) const;        // max + perm. bonus
        uint16 GetPureMaxSkillValue(uint32 skill) const;    // max
        uint16 GetSkillValue(uint32 skill) const;           // skill value + perm. bonus + temp bonus
        uint16 GetBaseSkillValue(uint32 skill) const;       // skill value + perm. bonus
        uint16 GetPureSkillValue(uint32 skill) const;       // skill value
        int16 GetSkillTempBonusValue(uint32 skill) const;
        bool HasSkill(uint32 skill) const;
        void LearnSkillRewardedSpells(uint32 id);
        void LearnSkillRewardedSpells();

        WorldLocation& GetTeleportDest()
        {
            return m_teleport_dest;
        }
        bool IsBeingTeleported() const
        {
            return mSemaphoreTeleport_Near || mSemaphoreTeleport_Far;
        }
        bool IsBeingTeleportedNear() const
        {
            return mSemaphoreTeleport_Near;
        }
        bool IsBeingTeleportedFar() const
        {
            return mSemaphoreTeleport_Far;
        }
        void SetSemaphoreTeleportNear(bool semphsetting)
        {
            mSemaphoreTeleport_Near = semphsetting;
        }
        void SetSemaphoreTeleportFar(bool semphsetting)
        {
            mSemaphoreTeleport_Far = semphsetting;
        }
        void ProcessDelayedOperations();

        void CheckAreaExploreAndOutdoor(void);

        static uint32 TeamForRace(uint8 race);
        uint32 GetTeam() const
        {
            return m_team;
        }
        TeamId GetTeamId() const
        {
            return m_team == ALLIANCE ? TEAM_ALLIANCE : TEAM_HORDE;
        }
        static uint32 getFactionForRace(uint8 race);
        void setFactionForRace(uint8 race);

        bool IsAtGroupRewardDistance(WorldObject const* pRewardSource) const;
        void RewardPlayerAndGroupAtKill(Unit* victim, bool isBattleGround);
        void RewardPlayerAndGroupAtEvent(uint32 creature_id, WorldObject* pRewardSource);
        bool isHonorOrXPTarget(Unit* victim) const;

        ReputationMgr&       GetReputationMgr()       { return m_reputationMgr; }
        ReputationMgr const& GetReputationMgr() const { return m_reputationMgr; }
        ReputationRank GetReputationRank(uint32 faction_id) const;
        void RewardReputation(Unit* victim, float rate);
        void RewardReputation(Quest const* pQuest);
 
        int32 CalculateReputationGain(uint32 creatureOrQuestLevel, int32 rep, bool for_quest);

        void UpdateSkillsForLevel();
        void UpdateSkillsToMaxSkillsForLevel();             // for .levelup
        void ModifySkillBonus(uint32 skillid, int32 val, bool talent);

        /*********************************************************/
        /***                 PVP SYSTEM                       ***/
        /*********************************************************/
        void UpdateHonorFields();
        bool RewardHonor(Unit* victim, uint32 groupsize, float honor = -1, bool pvptoken = false);
        uint32 GetHonorPoints()
        {
            return GetUInt32Value(PLAYER_FIELD_HONOR_CURRENCY);
        }
        uint32 GetArenaPoints()
        {
            return GetUInt32Value(PLAYER_FIELD_ARENA_CURRENCY);
        }
        void ModifyHonorPoints(int32 value);
        void ModifyArenaPoints(int32 value, bool update = true);
        uint8 GetHighestPvPRankIndex();
        uint32 GetMaxPersonalArenaRatingRequirement();

        //End of PvP System

        void SetDrunkValue(uint16 newDrunkValue, uint32 itemid = 0);
        uint16 GetDrunkValue() const
        {
            return m_drunk;
        }
        static DrunkenState GetDrunkenstateByValue(uint16 value);

        uint32 GetDeathTimer() const
        {
            return m_deathTimer;
        }
        uint32 GetCorpseReclaimDelay(bool pvp) const;
        void UpdateCorpseReclaimDelay();
        int32 CalculateCorpseReclaimDelay(bool load = false);
        void SendCorpseReclaimDelay(uint32 delay);

        uint32 GetShieldBlockValue() const override;                 // overwrite Unit version (virtual)
        bool CanParry() const
        {
            return m_canParry;
        }
        void SetCanParry(bool value);
        bool CanBlock() const
        {
            return m_canBlock;
        }
        void SetCanBlock(bool value);

        void SetRegularAttackTime();
        void SetBaseModValue(BaseModGroup modGroup, BaseModType modType, float value)
        {
            m_auraBaseMod[modGroup][modType] = value;
        }
        void HandleBaseModValue(BaseModGroup modGroup, BaseModType modType, float amount, bool apply);
        float GetBaseModValue(BaseModGroup modGroup, BaseModType modType) const;
        float GetTotalBaseModValue(BaseModGroup modGroup) const;
        float GetTotalPercentageModValue(BaseModGroup modGroup) const
        {
            return m_auraBaseMod[modGroup][FLAT_MOD] + m_auraBaseMod[modGroup][PCT_MOD];
        }
        void _ApplyAllStatBonuses();
        void _RemoveAllStatBonuses();

        void ResetAllPowers();

        void _ApplyWeaponDependentAuraMods(Item* item, WeaponAttackType attackType, bool apply);
        void _ApplyWeaponDependentAuraCritMod(Item* item, WeaponAttackType attackType, Aura* aura, bool apply);
        void _ApplyWeaponDependentAuraDamageMod(Item* item, WeaponAttackType attackType, Aura* aura, bool apply);

        void _ApplyItemMods(Item* item, uint8 slot, bool apply);
        void _RemoveAllItemMods();
        void _ApplyAllItemMods();
        void _ApplyItemBonuses(ItemTemplate const* proto, uint8 slot, bool apply);
        void _ApplyAmmoBonuses();
        bool EnchantmentFitsRequirements(uint32 enchantmentcondition, int8 slot);
        void ToggleMetaGemsActive(uint8 exceptslot, bool apply);
        void CorrectMetaGemEnchants(uint8 slot, bool apply);
        void InitDataForForm(bool reapplyMods = false);

        void ApplyItemEquipSpell(Item* item, bool apply, bool form_change = false);
        void ApplyEquipSpell(SpellEntry const* spellInfo, Item* item, bool apply, bool form_change = false);
        void UpdateEquipSpellsAtFormChange();
        void CastItemCombatSpell(Unit* target, WeaponAttackType attType, uint32 procVictim, uint32 procEx, SpellEntry const* spellInfo = NULL);
        void CastItemCombatSpell(Unit* target, WeaponAttackType attType, uint32 procVictim, uint32 procEx, Item* item, ItemTemplate const* proto, SpellEntry const* spell = NULL);

        void SendInitWorldStates(bool force = false, uint32 forceZoneId = 0);
        void SendUpdateWorldState(uint32 Field, uint32 Value);
        void SendDirectMessage(WorldPacket* data);

        void SendAuraDurationsForTarget(Unit* target);

        PlayerMenu* PlayerTalkClass;
        std::vector<ItemSetEffect*> ItemSetEff;

        void SendLoot(uint64 guid, LootType loot_type);
        void SendLootError(uint64 guid, LootError error);
        void SendLootRelease(uint64 guid);
        void SendNotifyLootItemRemoved(uint8 lootSlot);
        void SendNotifyLootMoneyRemoved();

        /*********************************************************/
        /***              BATTLEGROUND SYSTEM                 ***/
        /*********************************************************/

        bool InBattleground()       const
        {
            return m_bgData.bgInstanceID != 0;
        }
        bool InArena()              const;
        uint32 GetBattlegroundId()  const
        {
            return m_bgData.bgInstanceID;
        }
        Battleground* GetBattleground() const;

        static uint32 GetMinLevelForBattlegroundQueueId(uint32 queue_id);
        static uint32 GetMaxLevelForBattlegroundQueueId(uint32 queue_id);
        uint32 GetBattlegroundQueueIdFromLevel() const;

        bool InBattlegroundQueue() const
        {
            for (uint8 i = 0; i < PLAYER_MAX_BATTLEGROUND_QUEUES; ++i)
                if (m_bgBattlegroundQueueID[i].bgQueueType != 0)
                    return true;
            return false;
        }

        uint32 GetBattlegroundQueueId(uint32 index) const
        {
            return m_bgBattlegroundQueueID[index].bgQueueType;
        }
        uint32 GetBattlegroundQueueIndex(uint32 bgQueueType) const
        {
            for (uint8 i = 0; i < PLAYER_MAX_BATTLEGROUND_QUEUES; ++i)
                if (m_bgBattlegroundQueueID[i].bgQueueType == bgQueueType)
                    return i;
            return PLAYER_MAX_BATTLEGROUND_QUEUES;
        }
        bool IsInvitedForBattlegroundQueueType(uint32 bgQueueType) const
        {
            for (uint8 i = 0; i < PLAYER_MAX_BATTLEGROUND_QUEUES; ++i)
                if (m_bgBattlegroundQueueID[i].bgQueueType == bgQueueType)
                    return m_bgBattlegroundQueueID[i].invitedToInstance != 0;
            return false;
        }
        bool InBattlegroundQueueForBattlegroundQueueType(uint32 bgQueueType) const
        {
            return GetBattlegroundQueueIndex(bgQueueType) < PLAYER_MAX_BATTLEGROUND_QUEUES;
        }

        void SetBattlegroundId(uint32 val)
        {
            m_bgData.bgInstanceID = val;
        }
        uint32 AddBattlegroundQueueId(uint32 val)
        {
            for (uint8 i = 0; i < PLAYER_MAX_BATTLEGROUND_QUEUES; ++i)
            {
                if (m_bgBattlegroundQueueID[i].bgQueueType == 0 || m_bgBattlegroundQueueID[i].bgQueueType == val)
                {
                    m_bgBattlegroundQueueID[i].bgQueueType = val;
                    m_bgBattlegroundQueueID[i].invitedToInstance = 0;
                    return i;
                }
            }
            return PLAYER_MAX_BATTLEGROUND_QUEUES;
        }
        bool HasFreeBattlegroundQueueId()
        {
            for (uint8 i = 0; i < PLAYER_MAX_BATTLEGROUND_QUEUES; ++i)
                if (m_bgBattlegroundQueueID[i].bgQueueType == 0)
                    return true;
            return false;
        }
        void RemoveBattlegroundQueueId(uint32 val)
        {
            for (uint8 i = 0; i < PLAYER_MAX_BATTLEGROUND_QUEUES; ++i)
            {
                if (m_bgBattlegroundQueueID[i].bgQueueType == val)
                {
                    m_bgBattlegroundQueueID[i].bgQueueType = 0;
                    m_bgBattlegroundQueueID[i].invitedToInstance = 0;
                    return;
                }
            }
        }
        void SetInviteForBattlegroundQueueType(uint32 bgQueueType, uint32 instanceId)
        {
            for (uint8 i = 0; i < PLAYER_MAX_BATTLEGROUND_QUEUES; ++i)
                if (m_bgBattlegroundQueueID[i].bgQueueType == bgQueueType)
                    m_bgBattlegroundQueueID[i].invitedToInstance = instanceId;
        }
        bool IsInvitedForBattlegroundInstance(uint32 instanceId) const
        {
            for (uint8 i = 0; i < PLAYER_MAX_BATTLEGROUND_QUEUES; ++i)
                if (m_bgBattlegroundQueueID[i].invitedToInstance == instanceId)
                    return true;
            return false;
        }
        WorldLocation const& GetBattlegroundEntryPoint() const
        {
            return m_bgData.joinPos;
        }
        void SetBattlegroundEntryPoint();
        void SetBattlegroundEntryPoint(uint32 Map, float PosX, float PosY, float PosZ, float PosO)
        {
            m_bgData.joinPos = WorldLocation(Map, PosX, PosY, PosZ, PosO);
        }

        void SetBGTeam(uint32 team)
        {
            m_bgData.bgTeam = team;
        }
        uint32 GetBGTeam() const
        {
            return m_bgData.bgTeam ? m_bgData.bgTeam : GetTeam();
        }

        void LeaveBattleground(bool teleportToEntryPoint = true);
        bool CanJoinToBattleground() const;
        bool CanReportAfkDueToLimit();
        void ReportedAfkBy(Player* reporter);
        void ClearAfkReports()
        {
            m_bgData.bgAfkReporter.clear();
        }

        bool GetBGAccessByLevel(uint32 bgTypeId) const;
        bool isTotalImmunity();
        bool CanUseBattlegroundObject(GameObject* gameobject);
        bool CanCaptureTowerPoint();

        /*********************************************************/
        /***              OUTDOOR PVP SYSTEM                  ***/
        /*********************************************************/

        OutdoorPvP* GetOutdoorPvP() const;
        // returns true if the player is in active state for outdoor pvp objective capturing, false otherwise
        bool IsOutdoorPvPActive();

        /*********************************************************/
        /***             ENVIROMENTAL SYSTEM                  ***/
        /*********************************************************/

        void EnvironmentalDamage(EnviromentalDamage type, uint32 damage);

        /*********************************************************/
        /***              FLOOD FILTER SYSTEM                 ***/
        /*********************************************************/

        void UpdateSpeakTime();
        bool CanSpeak() const;
        void ChangeSpeakTime(int utime);

        /*********************************************************/
        /***                VARIOUS SYSTEMS                   ***/
        /*********************************************************/
        uint32 m_lastFallTime;
        float  m_lastFallZ;
        Unit* m_mover;
        WorldObject* m_seer;
        void SetFallInformation(uint32 time, float z)
        {
            m_lastFallTime = time;
            m_lastFallZ = z;
        }

        void BuildTeleportAckMsg( WorldPacket* data, float x, float y, float z, float ang) const;

        bool isMovingOrTurning() const
        {
            return HasUnitMovementFlag(MOVEMENTFLAG_TURNING);
        }

        bool CanFly() const
        {
            return HasUnitMovementFlag(MOVEMENTFLAG_CAN_FLY);
        }

        void HandleFallDamage(MovementInfo& movementInfo);
        void HandleFallUnderMap();

        void SetClientControl(Unit* target, bool allowMove);

        void SetMover(Unit* target)
        {
            //m_mover->m_movedPlayer = NULL;
            m_mover = target;
            //m_mover->m_movedPlayer = this;
        }
        void SetSeer(WorldObject* target)
        {
            m_seer = target;
        }
        void SetViewpoint(WorldObject* target, bool apply);
        WorldObject* GetViewpoint() const;
        void StopCastingCharm();
        void StopCastingBindSight();

        // Transports
        Transport* GetTransport() const
        {
            return m_transport;
        }
        void SetTransport(Transport* t)
        {
            m_transport = t;
        }

        float GetTransOffsetX() const
        {
            return m_movementInfo.GetTransportPos()->m_positionX;
        }
        float GetTransOffsetY() const
        {
            return m_movementInfo.GetTransportPos()->m_positionY;
        }
        float GetTransOffsetZ() const
        {
            return m_movementInfo.GetTransportPos()->m_positionZ;
        }
        float GetTransOffsetO() const
        {
            return m_movementInfo.GetTransportPos()->GetOrientation();
        }
        uint32 GetTransTime() const
        {
            return m_movementInfo.GetTransportTime();
        }

        uint32 GetSaveTimer() const
        {
            return m_nextSave;
        }
        void   SetSaveTimer(uint32 timer)
        {
            m_nextSave = timer;
        }

        // Recall position
        uint32 m_recallMap;
        float  m_recallX;
        float  m_recallY;
        float  m_recallZ;
        float  m_recallO;
        void   SaveRecallPosition();

        // Homebind coordinates
        uint32 m_homebindMapId;
        uint16 m_homebindAreaId;
        float m_homebindX;
        float m_homebindY;
        float m_homebindZ;
        void SetHomebindToLocation(WorldLocation const& loc, uint32 area_id);
        void RelocateToHomebind(uint32& newMap)
        {
            newMap = m_homebindMapId;
            Relocate(m_homebindX, m_homebindY, m_homebindZ);
        }
        bool TeleportToHomebind(uint32 options = 0)
        {
            return TeleportTo(m_homebindMapId, m_homebindX, m_homebindY, m_homebindZ, GetOrientation(), options);
        }

        WorldLocation GetStartPosition() const;

        // currently visible objects at player client
        typedef std::set<uint64> ClientGUIDs;
        ClientGUIDs m_clientGUIDs;

        bool HaveAtClient(WorldObject const* u) const;

        bool IsNeverVisible() const override;

        bool IsVisibleGloballyFor(Player* pl) const;

        void UpdateObjectVisibility(bool forced = true) override;
        void UpdateVisibilityForPlayer();
        void UpdateVisibilityOf(WorldObject* target);
        void UpdateTriggerVisibility();

        template<class T>
        void UpdateVisibilityOf(T* target, UpdateData& data, std::set<Unit*>& visibleNow);

        uint8 m_forced_speed_changes[MAX_MOVE_TYPE];

        bool HasAtLoginFlag(AtLoginFlags f) const
        {
            return m_atLoginFlags & f;
        }
        void SetAtLoginFlag(AtLoginFlags f)
        {
            m_atLoginFlags |= f;
        }

        LookingForGroup m_lookingForGroup;

        // Temporarily removed pet cache
        uint32 GetTemporaryUnsummonedPetNumber() const
        {
            return m_temporaryUnsummonedPetNumber;
        }
        void SetTemporaryUnsummonedPetNumber(uint32 petnumber)
        {
            m_temporaryUnsummonedPetNumber = petnumber;
        }
        uint32 GetOldPetSpell() const
        {
            return m_oldpetspell;
        }
        void SetOldPetSpell(uint32 petspell)
        {
            m_oldpetspell = petspell;
        }

        void UnsummonPetTemporaryIfAny();
        void ResummonTemporaryUnsummonedPetIfAny();

        // Handle pet status here
        PetStatus GetPetStatus() const
        {
            return m_petStatus;
        }
        void SetPetStatus(PetStatus status)
        {
            m_petStatus = status;
        }

        bool isPetDeadAndRemoved() const
        {
            return (m_petStatus == PET_STATUS_DEAD_AND_REMOVED);
        }
        bool isPetDismissed() const
        {
            return (m_petStatus == PET_STATUS_DISMISSED);
        }
        bool doesOwnPet() const
        {
            return (m_petStatus != PET_STATUS_NONE);
        }

        void SendCinematicStart(uint32 CinematicSequenceId);

        /*********************************************************/
        /***                INSTANCE SYSTEM                   ***/
        /*********************************************************/

        typedef UNORDERED_MAP< uint32 /*mapId*/, InstancePlayerBind > BoundInstancesMap;

        void UpdateHomebindTime(uint32 time);

        uint32 m_HomebindTimer;
        bool m_InstanceValid;
        // permanent binds and solo binds by difficulty
        BoundInstancesMap m_boundInstances[TOTAL_DIFFICULTIES];
        InstancePlayerBind* GetBoundInstance(uint32 mapid, uint8 difficulty);
        BoundInstancesMap& GetBoundInstances(uint8 difficulty)
        {
            return m_boundInstances[difficulty];
        }
        InstanceSave* GetInstanceSave(uint32 mapid);
        void UnbindInstance(uint32 mapid, uint8 difficulty, bool unload = false);
        void UnbindInstance(BoundInstancesMap::iterator& itr, uint8 difficulty, bool unload = false);
        InstancePlayerBind* BindToInstance(InstanceSave* save, bool permanent, bool load = false);
        void SendRaidInfo();
        void SendSavedInstances();
        bool CheckInstanceValidity(bool /*isLogin*/);
        static void ConvertInstancesToGroup(Player* player, Group* group = NULL, uint64 player_guid = 0);
        bool Satisfy(AccessRequirement const*, uint32 target_map, bool report = false);

        // last used pet number (for BG's)
        uint32 GetLastPetNumber() const { return m_lastpetnumber; }
        void SetLastPetNumber(uint32 petnumber) { m_lastpetnumber = petnumber; }

        /*********************************************************/
        /***                  GROUP SYSTEM                    ***/
        /*********************************************************/

        Group* GetGroupInvite()
        {
            return m_groupInvite;
        }
        void SetGroupInvite(Group* group)
        {
            m_groupInvite = group;
        }
        Group* GetGroup()
        {
            return m_group.getTarget();
        }
        const Group* GetGroup() const
        {
            return (const Group*)m_group.getTarget();
        }
        GroupReference& GetGroupRef()
        {
            return m_group;
        }
        void SetGroup(Group* group, int8 subgroup = -1);
        uint8 GetSubGroup() const
        {
            return m_group.getSubGroup();
        }
        uint32 GetGroupUpdateFlag()
        {
            return m_groupUpdateMask;
        }
        void SetGroupUpdateFlag(uint32 flag)
        {
            m_groupUpdateMask |= flag;
        }
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
        Player* GetNextRandomRaidMember(float radius);
        PartyResult CanUninviteFromGroup() const;

        // Battleground Group System
        void SetBattlegroundRaid(Group* group, int8 subgroup = -1);
        void RemoveFromBattlegroundRaid();
        Group* GetOriginalGroup()
        {
            return m_originalGroup.getTarget();
        }
        GroupReference& GetOriginalGroupRef()
        {
            return m_originalGroup;
        }
        uint8 GetOriginalSubGroup() const
        {
            return m_originalGroup.getSubGroup();
        }
        void SetOriginalGroup(Group* group, int8 subgroup = -1);

        void SetPassOnGroupLoot(bool bPassOnGroupLoot) { _passOnGroupLoot = bPassOnGroupLoot; }
        bool GetPassOnGroupLoot() const { return _passOnGroupLoot; }

        MapReference& GetMapRef()
        {
            return m_mapRef;
        }

        // Set map to player and add reference
        void SetMap(Map* map) override;
        void ResetMap() override;

        bool isAllowedToLoot(const Creature* creature);

        DeclinedName const* GetDeclinedNames() const
        {
            return m_declinedname;
        }
        bool HasTitle(uint32 bitIndex);
        bool HasTitle(CharTitlesEntry const* title)
        {
            return HasTitle(title->bit_index);
        }
        void SetTitle(CharTitlesEntry const* title, bool lost = false);

        bool IsLoading() const;

        RAFLinkStatus GetRAFStatus() const
        {
            return m_rafLink;
        }

        void SetRAFStatus(RAFLinkStatus status)
        {
            m_rafLink = status;
        }

        bool SetFeatherFall(bool apply, bool packetOnly = false) override;
        bool SetHover(bool apply, bool packetOnly = false) override;
        bool SetCanFly(bool apply, bool packetOnly = false) override;
        bool SetLevitate(bool apply, bool packetOnly = false) override;
        bool SetWaterWalking(bool enable, bool packetOnly = false) override;

        void ScheduleDelayedOperation(uint32 operation)
        {
            if (operation < DELAYED_END)
                m_DelayedOperations |= operation;
        }
    protected:

        uint32 m_contestedPvPTimer;

        /*********************************************************/
        /***              BATTLEGROUND SYSTEM                 ***/
        /*********************************************************/

        /*
        this is an array of BG queues (BgTypeIDs) in which is player
        */
        struct BgBattlegroundQueueID_Rec
        {
            uint32 bgQueueType;
            uint32 invitedToInstance;
        };

        BgBattlegroundQueueID_Rec m_bgBattlegroundQueueID[PLAYER_MAX_BATTLEGROUND_QUEUES];
        BGData                    m_bgData;

        /*********************************************************/
        /***                   QUEST SYSTEM                   ***/
        /*********************************************************/

        //We allow only one timed quest active at the same time. Below can then be simple value instead of set.
        std::set<uint32> m_timedquests;

        uint64 m_divider;
        uint32 m_ingametime;

        /*********************************************************/
        /***                  LOAD SYSTEM                     ***/
        /*********************************************************/

        void _LoadActions(QueryResult_AutoPtr result);
        void _LoadAuras(QueryResult_AutoPtr result, uint32 timediff);
        void _LoadBoundInstances(QueryResult_AutoPtr result);
        void _LoadInventory(QueryResult_AutoPtr result, uint32 timediff);
        void _LoadMailInit(QueryResult_AutoPtr resultUnread, QueryResult_AutoPtr resultDelivery);
        void _LoadMail();
        void _LoadMailedItems(Mail* mail);
        void _LoadQuestStatus(QueryResult_AutoPtr result);
        void _LoadDailyQuestStatus(QueryResult_AutoPtr result);
        void _LoadGroup(QueryResult_AutoPtr result);
        void _LoadSkills(QueryResult_AutoPtr result);
        void _LoadSpells(QueryResult_AutoPtr result);
        void _LoadTutorials(QueryResult_AutoPtr result);
        void _LoadFriendList(QueryResult_AutoPtr result);
        bool _LoadHomeBind(QueryResult_AutoPtr result);
        void _LoadDeclinedNames(QueryResult_AutoPtr result);
        void _LoadArenaTeamInfo(QueryResult_AutoPtr result);
        void _LoadBGData(QueryResult_AutoPtr result);

        /*********************************************************/
        /***                  SAVE SYSTEM                     ***/
        /*********************************************************/

        void _SaveActions();
        void _SaveAuras();
        void _SaveInventory();
        void _SaveMail();
        void _SaveQuestStatus();
        void _SaveDailyQuestStatus();
        void _SaveSkills();
        void _SaveSpells();
        void _SaveTutorials();
        void _SaveBGData();

        void _SetCreateBits(UpdateMask* updateMask, Player* target) const override;
        void _SetUpdateBits(UpdateMask* updateMask, Player* target) const override;

        /*********************************************************/
        /***             ENVIRONMENTAL SYSTEM                 ***/
        /*********************************************************/
        void HandleSobering();
        void SendMirrorTimer(MirrorTimerType Type, uint32 MaxValue, uint32 CurrentValue, int32 Regen);
        void StopMirrorTimer(MirrorTimerType Type);
        bool HasMirrorTimerFlag(uint32 flag) const { return m_MirrorTimerFlags & flag; }
        void HandleDrowning(uint32 time_diff);
        int32 getMaxTimer(MirrorTimerType timer);

        /*********************************************************/
        /***                 HONOR SYSTEM                     ***/
        /*********************************************************/
        time_t m_lastHonorUpdateTime;

        void outDebugValues() const;
        bool _removeSpell(uint16 spell_id);
        uint64 m_lootGuid;

        uint32 m_team;
        uint32 m_nextSave;
        time_t m_speakTime;
        uint32 m_speakCount;
        DungeonDifficulty m_dungeonDifficulty;

        uint32 m_atLoginFlags;

        Item* m_items[PLAYER_SLOTS_COUNT];
        uint32 m_currentBuybackSlot;

        std::vector<Item*> m_itemUpdateQueue;
        bool m_itemUpdateQueueBlocked;

        uint32 m_ExtraFlags;
        uint64 m_curSelection;

        uint64 m_comboTarget;
        int8 m_comboPoints;

        QuestStatusMap mQuestStatus;

        SkillStatusMap mSkillStatus;

        uint32 m_GuildIdInvited;
        uint32 m_ArenaTeamIdInvited;

        PlayerMails m_mail;
        PlayerSpellMap m_spells;
        SpellCooldowns m_spellCooldowns;
        std::map<uint32, uint32> m_globalCooldowns; // whole start recovery category stored in one

        ActionButtonList m_actionButtons;

        float m_auraBaseMod[BASEMOD_END][MOD_END];

        SpellModList m_spellMods[MAX_SPELLMOD];
        int32 m_SpellModRemoveCount;
        EnchantDurationList m_enchantDuration;
        ItemDurationList m_itemDuration;

        uint64 m_resurrectGUID;
        uint32 m_resurrectMap;
        float m_resurrectX, m_resurrectY, m_resurrectZ;
        uint32 m_resurrectHealth, m_resurrectMana;

        WorldSession* m_session;

        typedef std::list<Channel*> JoinedChannelsList;
        JoinedChannelsList m_channels;

        int m_cinematic;

        Player* pTrader;
        bool acceptTrade;
        uint64 tradeItems[TRADE_SLOT_COUNT];
        uint32 tradeGold;

        time_t m_nextThinkTime;

        uint32 m_Tutorials[8];
        bool   m_TutorialsChanged;

        bool   m_DailyQuestChanged;
        time_t m_lastDailyQuestTime;

        uint32 m_regenTimer;
        uint32 m_drunkTimer;
        uint16 m_drunk;
        uint32 m_weaponChangeTimer;

        uint32 m_zoneUpdateId;
        uint32 m_zoneUpdateTimer;
        uint32 m_areaUpdateId;

        uint32 m_deathTimer;
        time_t m_deathExpireTime;

        uint32 m_WeaponProficiency;
        uint32 m_ArmorProficiency;
        bool m_canParry;
        bool m_canBlock;
        uint8 m_swingErrorMsg;
        float m_ammoDPS;
        ////////////////////Rest System/////////////////////
        time_t _restTime;
        uint32 inn_triggerId;
        float m_rest_bonus;
        uint32 _restFlagMask;
        ////////////////////Rest System/////////////////////

        // Transports
        Transport* m_transport;

        uint32 m_resetTalentsCost;
        time_t m_resetTalentsTime;
        uint32 m_usedTalentCount;

        // Social
        PlayerSocial* m_social;

        // Groups
        GroupReference m_group;
        GroupReference m_originalGroup;
        Group* m_groupInvite;
        uint32 m_groupUpdateMask;
        uint64 m_auraUpdateMask;
        bool _passOnGroupLoot;

        // last used pet number (for BG's)
        uint32 m_lastpetnumber;

        // Player summoning
        time_t m_summon_expire;
        uint32 m_summon_mapid;
        float  m_summon_x;
        float  m_summon_y;
        float  m_summon_z;

        DeclinedName* m_declinedname;

        bool CanAlwaysSee(WorldObject const* obj) const override;

        bool IsAlwaysDetectableFor(WorldObject const* seer) const override;

    private:
        // internal common parts for CanStore/StoreItem functions
        uint8 _CanStoreItem_InSpecificSlot(uint8 bag, uint8 slot, ItemPosCountVec& dest, ItemTemplate const* pProto, uint32& count, bool swap, Item* pSrcItem) const;
        uint8 _CanStoreItem_InBag(uint8 bag, ItemPosCountVec& dest, ItemTemplate const* pProto, uint32& count, bool merge, bool non_specialized, Item* pSrcItem, uint8 skip_bag, uint8 skip_slot) const;
        uint8 _CanStoreItem_InInventorySlots(uint8 slot_begin, uint8 slot_end, ItemPosCountVec& dest, ItemTemplate const* pProto, uint32& count, bool merge, Item* pSrcItem, uint8 skip_bag, uint8 skip_slot) const;
        Item* _StoreItem(uint16 pos, Item* pItem, uint32 count, bool clone, bool update);

        CinematicMgr* _cinematicMgr;

        LiquidTypeEntry const* _lastLiquid;
        int32 m_MirrorTimer[MAX_TIMERS];
        uint8 m_MirrorTimerFlags;
        uint8 m_MirrorTimerFlagsLast;
        float m_GrantableLevels;
        bool m_isInWater;
        bool m_wasOutdoors;

        ReputationMgr  m_reputationMgr;

        void SetCanDelayTeleport(bool setting)
        {
            m_bCanDelayTeleport = setting;
        }
        bool IsHasDelayedTeleport() const
        {
            // we should not execute delayed teleports for now dead players but has been alive at teleport
            // because we don't want player's ghost teleported from graveyard
            return m_bHasDelayedTeleport && (IsAlive() || !m_bHasBeenAliveAtDelayedTeleport);
        }

        bool SetDelayedTeleportFlagIfCan()
        {
            m_bHasDelayedTeleport = m_bCanDelayTeleport;
            m_bHasBeenAliveAtDelayedTeleport = IsAlive();
            return m_bHasDelayedTeleport;
        }

        bool IsInstanceLoginGameMasterException() const;

        MapReference m_mapRef;

        void UpdateCharmedAI();
        UnitAI* i_AI;

        uint32 m_timeSyncCounter;
        uint32 m_timeSyncTimer;
        uint32 m_timeSyncClient;
        uint32 m_timeSyncServer;

        // Current teleport data
        WorldLocation m_teleport_dest;
        uint32 m_teleport_options;
        bool mSemaphoreTeleport_Near;
        bool mSemaphoreTeleport_Far;

        uint32 m_DelayedOperations;
        bool m_bCanDelayTeleport;
        bool m_bHasDelayedTeleport;
        bool m_bHasBeenAliveAtDelayedTeleport;

        RAFLinkStatus m_rafLink;

        // Temporary removed pet cache
        uint32 m_temporaryUnsummonedPetNumber;
        uint32 m_oldpetspell;

        // Status of your currently controlled pet
        PetStatus m_petStatus;

        uint32 _activeCheats;
};

void AddItemsSetItem(Player* player, Item* item);
void RemoveItemsSetItem(Player* player, ItemTemplate const* proto);

// "the bodies of template functions must be made available in a header file"
template <class T> T Player::ApplySpellMod(uint32 spellId, SpellModOp op, T& basevalue, Spell const* spell)
{
    SpellEntry const* spellInfo = sSpellStore.LookupEntry(spellId);
    if (!spellInfo)
        return 0;
    float totalpct = 1.0f;
    int32 totalflat = 0;

    for (SpellModList::iterator itr = m_spellMods[op].begin(); itr != m_spellMods[op].end(); ++itr)
    {
        SpellModifier* mod = *itr;

        if (!IsAffectedBySpellmod(spellInfo, mod, spell))
            continue;

        if (mod->type == SPELLMOD_FLAT)
            totalflat += mod->value;
        else if (mod->type == SPELLMOD_PCT)
        {
            // skip percent mods for null basevalue (most important for spell mods with charges)
            if (basevalue == T(0))
                continue;

            // special case (skip >10sec spell casts for instant cast setting)
            if (mod->op == SPELLMOD_CASTING_TIME  && basevalue >= T(10 * IN_MILLISECONDS) && mod->value <= -100)
                continue;

            totalpct += CalculatePct(1.0f, (float)mod->value);
        }

        if (mod->charges > 0)
        {
            if (!(spellInfo->SpellFamilyName == 8 && spellInfo->SpellFamilyFlags & 0x200000000LL))
                --mod->charges;
            if (mod->charges == 0)
            {
                mod->charges = -1;
                mod->lastAffected = spell;
                if (!mod->lastAffected)
                    mod->lastAffected = FindCurrentSpellBySpellId(spellId);
                ++m_SpellModRemoveCount;
            }
        }
    }

    float diff = (float)basevalue * (totalpct - 1.0f) + (float)totalflat;
    basevalue = T((float)basevalue + diff);
    return T(diff);
}
#endif

