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

#ifndef _OBJECTMGR_H
#define _OBJECTMGR_H

#include "Log.h"
#include "Object.h"
#include "Bag.h"
#include "Creature.h"
#include "Player.h"
#include "DynamicObject.h"
#include "GameObject.h"
#include "Corpse.h"
#include "QuestDef.h"
#include "ItemPrototype.h"
#include "NPCHandler.h"
#include "Database/DatabaseEnv.h"
#include "Mail.h"
#include "Map.h"
#include "ObjectAccessor.h"
#include "ObjectGuid.h"
#include "Policies/Singleton.h"
#include "Database/SQLStorage.h"
#include "Path.h"
#include "ConditionMgr.h"

#include <string>
#include <map>
#include <limits>
#include <tuple>

extern SQLStorage sCreatureStorage;
extern SQLStorage sCreatureDataAddonStorage;
extern SQLStorage sCreatureInfoAddonStorage;
extern SQLStorage sCreatureModelStorage;
extern SQLStorage sEquipmentStorage;
extern SQLStorage sEquipmentStorageRaw;
extern SQLStorage sGOStorage;
extern SQLStorage sPageTextStore;
extern SQLStorage sItemStorage;
extern SQLStorage sInstanceTemplate;

class Group;
class Guild;
class ArenaTeam;
class Item;

struct GameTele
{
    float  position_x;
    float  position_y;
    float  position_z;
    float  orientation;
    uint32 mapId;
    std::string name;
    std::wstring wnameLow;
};

typedef UNORDERED_MAP<uint32, GameTele > GameTeleMap;

/// Key for storing temp summon data in TempSummonDataContainer
struct TempSummonGroupKey
{
    TempSummonGroupKey(uint32 summonerEntry, SummonerType summonerType, uint8 group)
        : _summonerEntry(summonerEntry), _summonerType(summonerType), _summonGroup(group)
    {
    }

    bool operator<(TempSummonGroupKey const& rhs) const
    {
        return std::tie(_summonerEntry, _summonerType, _summonGroup) <
            std::tie(rhs._summonerEntry, rhs._summonerType, rhs._summonGroup);
    }

private:
    uint32 _summonerEntry;      ///< Summoner's entry
    SummonerType _summonerType; ///< Summoner's type, see SummonerType for available types
    uint8 _summonGroup;         ///< Summon's group id
};

enum ScriptsType
{
    SCRIPTS_FIRST = 1,

    SCRIPTS_QUEST_END = SCRIPTS_FIRST,
    SCRIPTS_QUEST_START,
    SCRIPTS_SPELL,
    SCRIPTS_GAMEOBJECT,
    SCRIPTS_EVENT,
    SCRIPTS_WAYPOINT,
    SCRIPTS_GOSSIP,

    SCRIPTS_LAST
};

enum eScriptFlags
{
    // Talk Flags
    SF_TALK_USE_PLAYER          = 0x1,

    // Emote flags
    SF_EMOTE_USE_STATE          = 0x1,

    // TeleportTo flags
    SF_TELEPORT_USE_CREATURE    = 0x1,

    // KillCredit flags
    SF_KILLCREDIT_REWARD_GROUP  = 0x1,

    // RemoveAura flags
    SF_REMOVEAURA_REVERSE       = 0x1,

    // CastSpell flags
    SF_CASTSPELL_SOURCE_TO_TARGET = 0,
    SF_CASTSPELL_SOURCE_TO_SOURCE = 1,
    SF_CASTSPELL_TARGET_TO_TARGET = 2,
    SF_CASTSPELL_TARGET_TO_SOURCE = 3,
    SF_CASTSPELL_SEARCH_CREATURE  = 4,
    SF_CASTSPELL_TRIGGERED      = 0x1,

    // PlaySound flags
    SF_PLAYSOUND_TARGET_PLAYER  = 0x1,
    SF_PLAYSOUND_DISTANCE_SOUND = 0x2,

    // Orientation flags
    SF_ORIENTATION_FACE_TARGET  = 0x1,
};

struct ScriptInfo
{
    ScriptsType type;
    uint32 id;
    uint32 delay;
    ScriptCommands command;

    union
    {
        struct
        {
            uint32 nData[3];
            float  fData[4];
        } Raw;

        struct                      // SCRIPT_COMMAND_TALK (0)
        {
            uint32 ChatType;        // datalong
            uint32 Flags;           // datalong2
            int32  TextID;          // dataint
        } Talk;

        struct                      // SCRIPT_COMMAND_EMOTE (1)
        {
            uint32 EmoteID;         // datalong
            uint32 Flags;           // datalong2
        } Emote;

        struct                      // SCRIPT_COMMAND_FIELD_SET (2)
        {
            uint32 FieldID;         // datalong
            uint32 FieldValue;      // datalong2
        } FieldSet;

        struct                      // SCRIPT_COMMAND_MOVE_TO (3)
        {
            uint32 Unused1;         // datalong
            uint32 TravelTime;      // datalong2
            int32  Unused2;         // dataint

            float DestX;
            float DestY;
            float DestZ;
        } MoveTo;

        struct                      // SCRIPT_COMMAND_FLAG_SET (4)
                // SCRIPT_COMMAND_FLAG_REMOVE (5)
        {
            uint32 FieldID;         // datalong
            uint32 FieldValue;      // datalong2
        } FlagToggle;

        struct                      // SCRIPT_COMMAND_TELEPORT_TO (6)
        {
            uint32 MapID;           // datalong
            uint32 Flags;           // datalong2
            int32  Unused1;         // dataint

            float DestX;
            float DestY;
            float DestZ;
            float Orientation;
        } TeleportTo;

        struct                      // SCRIPT_COMMAND_QUEST_EXPLORED (7)
        {
            uint32 QuestID;         // datalong
            uint32 Distance;        // datalong2
        } QuestExplored;

        struct                      // SCRIPT_COMMAND_KILL_CREDIT (8)
        {
            uint32 CreatureEntry;   // datalong
            uint32 Flags;           // datalong2
        } KillCredit;

        struct                      // SCRIPT_COMMAND_RESPAWN_GAMEOBJECT (9)
        {
            uint32 GOGuid;          // datalong
            uint32 DespawnDelay;    // datalong2
        } RespawnGameobject;

        struct                      // SCRIPT_COMMAND_TEMP_SUMMON_CREATURE (10)
        {
            uint32 CreatureEntry;   // datalong
            uint32 DespawnDelay;    // datalong2
            int32  Unused1;         // dataint

            float PosX;
            float PosY;
            float PosZ;
            float Orientation;
        } TempSummonCreature;

        struct                      // SCRIPT_COMMAND_CLOSE_DOOR (12)
                // SCRIPT_COMMAND_OPEN_DOOR (11)
        {
            uint32 GOGuid;          // datalong
            uint32 ResetDelay;      // datalong2
        } ToggleDoor;

        // SCRIPT_COMMAND_ACTIVATE_OBJECT (13)

        struct                      // SCRIPT_COMMAND_REMOVE_AURA (14)
        {
            uint32 SpellID;         // datalong
            uint32 Flags;           // datalong2
        } RemoveAura;

        struct                      // SCRIPT_COMMAND_CAST_SPELL (15)
        {
            uint32 SpellID;         // datalong
            uint32 Flags;           // datalong2
            int32  CreatureEntry;   // dataint

            float SearchRadius;
        } CastSpell;

        struct                      // SCRIPT_COMMAND_PLAY_SOUND (16)
        {
            uint32 SoundID;         // datalong
            uint32 Flags;           // datalong2
        } PlaySound;

        struct                      // SCRIPT_COMMAND_CREATE_ITEM (17)
        {
            uint32 ItemEntry;       // datalong
            uint32 Amount;          // datalong2
        } CreateItem;

        struct                      // SCRIPT_COMMAND_DESPAWN_SELF (18)
        {
            uint32 DespawnDelay;    // datalong
        } DespawnSelf;

        struct                      // SCRIPT_COMMAND_LOAD_PATH (20)
        {
            uint32 PathID;          // datalong
            uint32 IsRepeatable;    // datalong2
        } LoadPath;

        struct                      // SCRIPT_COMMAND_CALLSCRIPT_TO_UNIT (21)
        {
            uint32 CreatureEntry;   // datalong
            uint32 ScriptID;        // datalong2
            uint32 ScriptType;      // dataint
        } CallScript;

        struct                      // SCRIPT_COMMAND_KILL (22)
        {
            uint32 Unused1;         // datalong
            uint32 Unused2;         // datalong2
            int32  RemoveCorpse;    // dataint
        } Kill;

        struct                      // SCRIPT_COMMAND_ORIENTATION (30)
        {
            uint32 Flags;           // datalong
            uint32 Unused1;         // datalong2
            int32  Unused2;         // dataint

            float Unused3;
            float Unused4;
            float Unused5;
            float Orientation;
        } Orientation;

        struct                      // SCRIPT_COMMAND_EQUIP (31)
        {
            uint32 EquipmentID;     // datalong
        } Equip;

        struct                      // SCRIPT_COMMAND_MODEL (32)
        {
            uint32 ModelID;         // datalong
        } Model;

        // SCRIPT_COMMAND_CLOSE_GOSSIP (33)
    };

    std::string GetDebugInfo() const;
};

typedef std::multimap<uint32, ScriptInfo> ScriptMap;
typedef std::map<uint32, ScriptMap > ScriptMapMap;
extern ScriptMapMap sQuestEndScripts;
extern ScriptMapMap sQuestStartScripts;
extern ScriptMapMap sSpellScripts;
extern ScriptMapMap sGameObjectScripts;
extern ScriptMapMap sEventScripts;
extern ScriptMapMap sGossipScripts;
extern ScriptMapMap sWaypointScripts;

std::string GetScriptsTableNameByType(ScriptsType type);
ScriptMapMap* GetScriptsMapByType(ScriptsType type);
std::string GetScriptCommandName(ScriptCommands command);

struct AreaTrigger
{
    uint32 access_id;
    uint32 target_mapId;
    float  target_X;
    float  target_Y;
    float  target_Z;
    float  target_Orientation;
};

typedef std::set<uint32> CellGuidSet;
typedef std::map<uint32/*player guid*/, uint32/*instance*/> CellCorpseSet;
struct CellObjectGuids
{
    CellGuidSet creatures;
    CellGuidSet gameobjects;
    CellCorpseSet corpses;
};
typedef UNORDERED_MAP<uint32/*cell_id*/, CellObjectGuids> CellObjectGuidsMap;
typedef UNORDERED_MAP<uint32/*(mapid,spawnMode) pair*/, CellObjectGuidsMap> MapObjectGuids;

typedef UNORDERED_MAP<uint64/*(instance,guid) pair*/, time_t> RespawnTimes;


// Oregon string ranges
#define MIN_OREGON_STRING_ID           1                    // 'mangos_string'
#define MAX_OREGON_STRING_ID           2000000000
#define MIN_DB_SCRIPT_STRING_ID        MAX_OREGON_STRING_ID // 'db_script_string'
#define MAX_DB_SCRIPT_STRING_ID        2000010000
#define MIN_CREATURE_AI_TEXT_STRING_ID (-1)                 // 'creature_ai_texts'
#define MAX_CREATURE_AI_TEXT_STRING_ID (-1000000)

struct OregonStringLocale
{
    std::vector<std::string> Content;                       // 0 -> default, i -> i-1 locale index
};

typedef std::map<uint32, uint32> CreatureLinkedRespawnMap;
typedef UNORDERED_MAP<uint32, CreatureData> CreatureDataMap;
typedef UNORDERED_MAP<uint32, GameObjectData> GameObjectDataMap;
typedef UNORDERED_MAP<uint32, CreatureLocale> CreatureLocaleMap;
typedef UNORDERED_MAP<uint32, GameObjectLocale> GameObjectLocaleMap;
typedef UNORDERED_MAP<uint32, ItemLocale> ItemLocaleMap;
typedef UNORDERED_MAP<uint32, QuestLocale> QuestLocaleMap;
typedef UNORDERED_MAP<uint32, NpcTextLocale> NpcTextLocaleMap;
typedef UNORDERED_MAP<uint32, PageTextLocale> PageTextLocaleMap;
typedef UNORDERED_MAP<uint32, OregonStringLocale> OregonStringLocaleMap;
typedef UNORDERED_MAP<uint32, GossipMenuItemsLocale> GossipMenuItemsLocaleMap;

typedef std::multimap<uint32, uint32> QuestRelations;

struct PetLevelInfo
{
    PetLevelInfo() : health(0), mana(0)
    {
        for (int i = 0; i < MAX_STATS; ++i) stats[i] = 0;
    }

    uint16 stats[MAX_STATS];
    uint16 health;
    uint16 mana;
    uint16 armor;
};

struct ReputationOnKillEntry
{
    uint32 repfaction1;
    uint32 repfaction2;
    bool is_teamaward1;
    uint32 reputation_max_cap1;
    int32 repvalue1;
    bool is_teamaward2;
    uint32 reputation_max_cap2;
    int32 repvalue2;
    bool team_dependent;
};

struct GossipMenuItems
{
    uint32          menu_id;
    uint32          id;
    uint8           option_icon;
    std::string     option_text;
    uint32          option_id;
    uint32          npc_option_npcflag;
    uint32          action_menu_id;
    uint32          action_poi_id;
    uint32          action_script_id;
    bool            box_coded;
    uint32          box_money;
    std::string     box_text;
    ConditionList   conditions;
};

struct GossipMenus
{
    uint32          entry;
    uint32          text_id;
    ConditionList   conditions;
};

typedef std::multimap<uint32, GossipMenus> GossipMenusMap;
typedef std::pair<GossipMenusMap::const_iterator, GossipMenusMap::const_iterator> GossipMenusMapBounds;
typedef std::pair<GossipMenusMap::iterator, GossipMenusMap::iterator> GossipMenusMapBoundsNonConst;
typedef std::multimap<uint32, GossipMenuItems> GossipMenuItemsMap;
typedef std::pair<GossipMenuItemsMap::const_iterator, GossipMenuItemsMap::const_iterator> GossipMenuItemsMapBounds;
typedef std::pair<GossipMenuItemsMap::iterator, GossipMenuItemsMap::iterator> GossipMenuItemsMapBoundsNonConst;

typedef std::map<TempSummonGroupKey, std::vector<TempSummonData> > TempSummonDataContainer;

struct PetCreateSpellEntry
{
    uint32 spellid[4];
};

#define WEATHER_SEASONS 4
struct WeatherSeasonChances
{
    uint32 rainChance;
    uint32 snowChance;
    uint32 stormChance;
};

struct WeatherZoneChances
{
    WeatherSeasonChances data[WEATHER_SEASONS];
};

struct GraveYardData
{
    uint32 safeLocId;
    uint32 team;
};
typedef std::multimap<uint32, GraveYardData> GraveYardMap;

// NPC gossip text id
typedef UNORDERED_MAP<uint32, uint32> CacheNpcTextIdMap;

typedef UNORDERED_MAP<uint32, VendorItemData> CacheVendorItemMap;
typedef UNORDERED_MAP<uint32, TrainerSpellData> CacheTrainerSpellMap;

enum SkillRangeType
{
    SKILL_RANGE_LANGUAGE,                                   // 300..300
    SKILL_RANGE_LEVEL,                                      // 1..max skill for level
    SKILL_RANGE_MONO,                                       // 1..1, grey monolite bar
    SKILL_RANGE_RANK,                                       // 1..skill for known rank
    SKILL_RANGE_NONE,                                       // 0..0 always
};

SkillRangeType GetSkillRangeType(SkillLineEntry const* pSkill, bool racial);

#define MAX_PLAYER_NAME 12                                  // max allowed by client name length
#define MAX_INTERNAL_PLAYER_NAME 15                         // max server internal player name length (> MAX_PLAYER_NAME for support declined names)

bool normalizePlayerName(std::string& name);

struct LanguageDesc
{
    Language lang_id;
    uint32   spell_id;
    uint32   skill_id;
};

extern LanguageDesc lang_description[LANGUAGES_COUNT];
LanguageDesc const* GetLanguageDescByID(uint32 lang);

class PlayerDumpReader;

class ObjectMgr
{
        friend class PlayerDumpReader;

    public:
        ObjectMgr();
        ~ObjectMgr();

        typedef UNORDERED_MAP<uint32, Item*> ItemMap;

        typedef std::set< Group* > GroupSet;

        typedef UNORDERED_MAP<uint32, Guild* > GuildMap;

        typedef UNORDERED_MAP<uint32, ArenaTeam*> ArenaTeamMap;

        typedef UNORDERED_MAP<uint32, Quest*> QuestMap;

        typedef UNORDERED_MAP<uint32, AreaTrigger> AreaTriggerMap;

        typedef UNORDERED_MAP<uint32, uint32> AreaTriggerScriptMap;

        typedef UNORDERED_MAP<uint32, AccessRequirement> AccessRequirementMap;

        typedef UNORDERED_MAP<uint32, ReputationOnKillEntry> RepOnKillMap;

        typedef UNORDERED_MAP<uint32, WeatherZoneChances> WeatherZoneMap;

        typedef UNORDERED_MAP<uint32, PetCreateSpellEntry> PetCreateSpellMap;

        typedef std::vector<std::string> ScriptNameMap;

        typedef UNORDERED_MAP<uint64, uint64> ReferFriendMap;

        UNORDERED_MAP<uint32, uint32> TransportEventMap;

        Player* GetPlayer(const char* name, bool force = false) const
        {
            return ObjectAccessor::Instance().FindPlayerByName(name, force);
        }
        Player* GetPlayer(uint64 guid, bool force = false) const
        {
            return ObjectAccessor::FindPlayer(guid, force);
        }

        // Refer-a-Friend

        // Gets RAFLinkStatus and puts the friend's acc into 'linked'
        RAFLinkStatus GetRAFLinkStatus (uint64 account, uint64* linked = NULL) const;
        RAFLinkStatus GetRAFLinkStatus (const Player* plr) const
        {
            return GetRAFLinkStatus(plr->GetSession()->GetAccountId());
        }
        RAFLinkStatus GetRAFLinkStatus (uint64 AccOne, uint64 AccTwo) const;
        RAFLinkStatus GetRAFLinkStatus (const Player* plr1, const Player* plr2) const
        {
            return GetRAFLinkStatus(plr1->GetSession()->GetAccountId(), plr2->GetSession()->GetAccountId());
        }
        void LinkIntoRAF      (uint64 AccReferrer, uint64 AccReferred);
        void UnlinkFromRAF    (uint64 account);
        Player* GetRAFLinkedBuddyForPlayer(const Player* plr1) const;

        static GameObjectInfo const* GetGameObjectInfo(uint32 id)
        {
            return sGOStorage.LookupEntry<GameObjectInfo>(id);
        }

        void LoadGameobjectInfo();
        void AddGameobjectInfo(GameObjectInfo* goinfo);

        Group* GetGroupByLeader(const uint64& guid) const;
        void AddGroup(Group* group)
        {
            mGroupSet.insert(group);
        }
        void RemoveGroup(Group* group)
        {
            mGroupSet.erase(group);
        }

        Guild* GetGuildByLeader(uint64 const& guid) const;
        Guild* GetGuildById(const uint32 GuildId) const;
        Guild* GetGuildByName(const std::string& guildname) const;
        std::string GetGuildNameById(const uint32 GuildId) const;
        void AddGuild(Guild* guild);
        void RemoveGuild(uint32 Id);

        ArenaTeam* GetArenaTeamById(const uint32 arenateamid) const;
        ArenaTeam* GetArenaTeamByName(const std::string& arenateamname) const;
        ArenaTeam* GetArenaTeamByCaptain(uint64 const& guid) const;
        void AddArenaTeam(ArenaTeam* arenaTeam);
        void RemoveArenaTeam(uint32 Id);
        ArenaTeamMap::iterator GetArenaTeamMapBegin()
        {
            return mArenaTeamMap.begin();
        }
        ArenaTeamMap::iterator GetArenaTeamMapEnd()
        {
            return mArenaTeamMap.end();
        }

        static CreatureInfo const* GetCreatureTemplate(uint32 id);
        CreatureModelInfo const* GetCreatureModelInfo(uint32 modelid);
        CreatureModelInfo const* GetCreatureModelRandomGender(uint32 display_id);
        uint32 ChooseDisplayId(uint32 team, const CreatureInfo* cinfo, const CreatureData* data = NULL);
        EquipmentInfo const* GetEquipmentInfo(uint32 entry);
        EquipmentInfoRaw const* GetEquipmentInfoRaw( uint32 entry );
        static CreatureDataAddon const* GetCreatureAddon(uint32 lowguid)
        {
            return sCreatureDataAddonStorage.LookupEntry<CreatureDataAddon>(lowguid);
        }

        static CreatureDataAddon const* GetCreatureTemplateAddon(uint32 entry)
        {
            return sCreatureInfoAddonStorage.LookupEntry<CreatureDataAddon>(entry);
        }

        static ItemTemplate const* GetItemTemplate(uint32 id)
        {
            return sItemStorage.LookupEntry<ItemTemplate>(id);
        }

        static InstanceTemplate const* GetInstanceTemplate(uint32 map)
        {
            return sInstanceTemplate.LookupEntry<InstanceTemplate>(map);
        }

        PetLevelInfo const* GetPetLevelInfo(uint32 creature_id, uint32 level) const;

        PlayerClassInfo const* GetPlayerClassInfo(uint32 class_) const
        {
            if (class_ >= MAX_CLASSES) return NULL;
            return &playerClassInfo[class_];
        }
        void GetPlayerClassLevelInfo(uint32 class_, uint32 level, PlayerClassLevelInfo* info) const;

        PlayerInfo const* GetPlayerInfo(uint32 race, uint32 class_) const
        {
            if (race   >= MAX_RACES)   return NULL;
            if (class_ >= MAX_CLASSES) return NULL;
            PlayerInfo const* info = &playerInfo[race][class_];
            if (info->displayId_m == 0 || info->displayId_f == 0) return NULL;
            return info;
        }
        void GetPlayerLevelInfo(uint32 race, uint32 class_, uint32 level, PlayerLevelInfo* info) const;

        uint64 GetPlayerGUIDByName(std::string name) const;
        bool GetPlayerNameByGUID(const uint64& guid, std::string& name) const;
        uint32 GetPlayerTeamByGUID(const uint64& guid) const;
        uint32 GetPlayerAccountIdByGUID(const uint64& guid) const;
        uint32 GetPlayerAccountIdByPlayerName(const std::string& name) const;

        uint32 GetNearestTaxiNode(float x, float y, float z, uint32 mapid);
        void GetTaxiPath(uint32 source, uint32 destination, uint32& path, uint32& cost);
        uint16 GetTaxiMount(uint32 id, uint32 team);

        Quest const* GetQuestTemplate(uint32 quest_id) const
        {
            QuestMap::const_iterator itr = mQuestTemplates.find(quest_id);
            return itr != mQuestTemplates.end() ? itr->second : NULL;
        }
        QuestMap const& GetQuestTemplates() const
        {
            return mQuestTemplates;
        }

        uint32 GetQuestForAreaTrigger(uint32 Trigger_ID) const
        {
            QuestAreaTriggerMap::const_iterator itr = mQuestAreaTriggerMap.find(Trigger_ID);
            if (itr != mQuestAreaTriggerMap.end())
                return itr->second;
            return 0;
        }
        bool IsTavernAreaTrigger(uint32 Trigger_ID) const
        {
            return mTavernAreaTriggerSet.count(Trigger_ID) != 0;
        }
        bool IsGameObjectForQuests(uint32 entry) const
        {
            return mGameObjectForQuestSet.count(entry) != 0;
        }

        uint32 GetBattleMasterBG(uint32 entry) const
        {
            BattleMastersMap::const_iterator itr = mBattleMastersMap.find(entry);
            if (itr != mBattleMastersMap.end())
                return itr->second;
            return 2;                                       //BATTLEGROUND_WS - i will not add include only for constant usage!
        }

        void AddGossipText(GossipText* pGText);
        GossipText* GetGossipText(uint32 Text_ID);

        WorldSafeLocsEntry const* GetClosestGraveYard(float x, float y, float z, uint32 MapId, uint32 team);
        bool AddGraveYardLink(uint32 id, uint32 zone, uint32 team, bool inDB = true);
        void RemoveGraveYardLink(uint32 id, uint32 zone, uint32 team, bool inDB = false);
        void LoadGraveyardZones();
        GraveYardData const* FindGraveYardData(uint32 id, uint32 zone);

        AreaTrigger const* GetAreaTrigger(uint32 trigger) const
        {
            AreaTriggerMap::const_iterator itr = mAreaTriggers.find(trigger);
            if (itr != mAreaTriggers.end())
                return &itr->second;
            return NULL;
        }

        AccessRequirement const* GetAccessRequirement(uint32 requirement) const
        {
            AccessRequirementMap::const_iterator itr = mAccessRequirements.find(requirement);
            if (itr != mAccessRequirements.end())
                return &itr->second;
            return NULL;
        }

        AreaTrigger const* GetGoBackTrigger(uint32 Map) const;
        AreaTrigger const* GetMapEntranceTrigger(uint32 Map) const;

        uint32 GetAreaTriggerScriptId(uint32 trigger_id);

        ReputationOnKillEntry const* GetReputationOnKilEntry(uint32 id) const
        {
            RepOnKillMap::const_iterator itr = mRepOnKill.find(id);
            if (itr != mRepOnKill.end())
                return &itr->second;
            return NULL;
        }

        PetCreateSpellEntry const* GetPetCreateSpellEntry(uint32 id) const
        {
            PetCreateSpellMap::const_iterator itr = mPetCreateSpell.find(id);
            if (itr != mPetCreateSpell.end())
                return &itr->second;
            return NULL;
        }

        void LoadGuilds();
        void LoadArenaTeams();
        void LoadGroups();
        void LoadQuests();
        void LoadQuestRelations()
        {
            LoadGameobjectQuestRelations();
            LoadGameobjectInvolvedRelations();
            LoadCreatureQuestRelations();
            LoadCreatureInvolvedRelations();
        }

        QuestRelations* GetGOQuestRelationMap()
        {
            return &mGOQuestRelations;
        }

        QuestRelations* GetCreatureQuestRelationMap()
        {
            return &mCreatureQuestRelations;
        }

        void LoadGameobjectQuestRelations();
        void LoadGameobjectInvolvedRelations();
        void LoadCreatureQuestRelations();
        void LoadCreatureInvolvedRelations();

        QuestRelations mGOQuestRelations;
        QuestRelations mGOQuestInvolvedRelations;
        QuestRelations mCreatureQuestRelations;
        QuestRelations mCreatureQuestInvolvedRelations;

        void LoadGameObjectScripts();
        void LoadQuestEndScripts();
        void LoadQuestStartScripts();
        void LoadEventScripts();
        void LoadSpellScripts();
        void LoadGossipScripts();
        void LoadWaypointScripts();
        void LoadReferredFriends();

        void LoadTransportEvents();

        bool LoadOregonStrings(DatabaseType& db, char const* table, int32 min_value, int32 max_value);
        bool LoadOregonStrings()
        {
            return LoadOregonStrings(WorldDatabase, "oregon_string", MIN_OREGON_STRING_ID, MAX_OREGON_STRING_ID);
        }
        void LoadDbScriptStrings();
        void LoadPetCreateSpells();
        void LoadCreatureLocales();
        void LoadCreatureTemplates();
        void LoadTempSummons();
        void LoadCreatures();
        void LoadCreatureLinkedRespawn();
        bool CheckCreatureLinkedRespawn(uint32 guid, uint32 linkedGuid) const;
        bool SetCreatureLinkedRespawn(uint32 guid, uint32 linkedGuid);
        void LoadCreatureRespawnTimes();
        void LoadCreatureAddons();
        void LoadCreatureModelInfo();
        void LoadEquipmentTemplates();
        void LoadGameObjectLocales();
        void LoadGameobjects();
        void LoadGameobjectRespawnTimes();
        void LoadItemPrototypes();
        void LoadItemLocales();
        void LoadQuestLocales();
        void LoadNpcTextLocales();
        void LoadPageTextLocales();
        void LoadGossipMenuItemsLocales();
        void LoadInstanceTemplate();

        void LoadGossipText();

        void LoadAreaTriggerTeleports();
        void LoadAccessRequirements();
        void LoadQuestAreaTriggers();
        void LoadAreaTriggerScripts();
        void LoadTavernAreaTriggers();
        void LoadBattleMastersEntry();
        void LoadGameObjectForQuests();

        void LoadItemTexts();
        void LoadPageTexts();

        void LoadPlayerInfo();
        void LoadPetLevelInfo();
        void LoadExplorationBaseXP();
        void LoadPetNames();
        void LoadPetNumber();
        void LoadCorpses();
        void LoadFishingBaseSkillLevel();

        void LoadReputationOnKill();

        void LoadWeatherZoneChances();
        void LoadGameTele();

        void LoadNpcTextId();

        void LoadGossipMenu();
        void LoadGossipMenuItems();

        void LoadVendors();
        void LoadTrainerSpell();

        std::string GeneratePetName(uint32 entry);
        uint32 GetBaseXP(uint32 level);

        int32 GetFishingBaseSkillLevel(uint32 entry) const
        {
            FishingBaseSkillMap::const_iterator itr = mFishingBaseForArea.find(entry);
            return itr != mFishingBaseForArea.end() ? itr->second : 0;
        }

        void ReturnOrDeleteOldMails(bool serverUp);

        void SetHighestGuids();
        uint32 GenerateLowGuid(HighGuid guidhigh);
        uint32 GenerateAuctionID();
        uint32 GenerateMailID();
        uint32 GenerateItemTextID();
        uint32 GeneratePetNumber();
        uint32 GenerateArenaTeamId();
        uint32 GenerateGuildId();

        uint32 CreateItemText(std::string text);
        std::string GetItemText(uint32 id)
        {
            ItemTextMap::const_iterator itr = mItemTexts.find(id);
            if (itr != mItemTexts.end())
                return itr->second;
            else
                return "There is no info for this item";
        }

        typedef std::multimap<int32, uint32> ExclusiveQuestGroups;
        ExclusiveQuestGroups mExclusiveQuestGroups;


        /**
        * Gets temp summon data for all creatures of specified group.
        *
        * @param summonerId   Summoner's entry.
        * @param summonerType Summoner's type, see SummonerType for available types.
        * @param group        Id of required group.
        *
        * @return null if group was not found, otherwise reference to the creature group data
        */
        std::vector<TempSummonData> const* GetSummonGroup(uint32 summonerId, SummonerType summonerType, uint8 group) const
        {
            TempSummonDataContainer::const_iterator itr = _tempSummonDataStore.find(TempSummonGroupKey(summonerId, summonerType, group));
            if (itr != _tempSummonDataStore.end())
                return &itr->second;

            return NULL;
        }

        WeatherZoneChances const* GetWeatherChances(uint32 zone_id) const
        {
            WeatherZoneMap::const_iterator itr = mWeatherZoneMap.find(zone_id);
            if (itr != mWeatherZoneMap.end())
                return &itr->second;
            else
                return NULL;
        }

        CellObjectGuids const& GetCellObjectGuids(uint16 mapid, uint8 spawnMode, uint32 cell_id)
        {
            return mMapObjectGuids[MAKE_PAIR32(mapid, spawnMode)][cell_id];
        }

        CreatureData const* GetCreatureData(uint32 guid) const
        {
            CreatureDataMap::const_iterator itr = mCreatureDataMap.find(guid);
            if (itr == mCreatureDataMap.end()) return NULL;
            return &itr->second;
        }
        CreatureData& NewOrExistCreatureData(uint32 guid)
        {
            return mCreatureDataMap[guid];
        }
        void DeleteCreatureData(uint32 guid);
        uint32 GetLinkedRespawnGuid(uint32 guid) const
        {
            CreatureLinkedRespawnMap::const_iterator itr = mCreatureLinkedRespawnMap.find(guid);
            if (itr == mCreatureLinkedRespawnMap.end()) return 0;
            return itr->second;
        }
        CreatureLocale const* GetCreatureLocale(uint32 entry) const
        {
            CreatureLocaleMap::const_iterator itr = mCreatureLocaleMap.find(entry);
            if (itr == mCreatureLocaleMap.end()) return NULL;
            return &itr->second;
        }
        GameObjectLocale const* GetGameObjectLocale(uint32 entry) const
        {
            GameObjectLocaleMap::const_iterator itr = mGameObjectLocaleMap.find(entry);
            if (itr == mGameObjectLocaleMap.end()) return NULL;
            return &itr->second;
        }
        ItemLocale const* GetItemLocale(uint32 entry) const
        {
            ItemLocaleMap::const_iterator itr = mItemLocaleMap.find(entry);
            if (itr == mItemLocaleMap.end()) return NULL;
            return &itr->second;
        }
        QuestLocale const* GetQuestLocale(uint32 entry) const
        {
            QuestLocaleMap::const_iterator itr = mQuestLocaleMap.find(entry);
            if (itr == mQuestLocaleMap.end()) return NULL;
            return &itr->second;
        }
        NpcTextLocale const* GetNpcTextLocale(uint32 entry) const
        {
            NpcTextLocaleMap::const_iterator itr = mNpcTextLocaleMap.find(entry);
            if (itr == mNpcTextLocaleMap.end()) return NULL;
            return &itr->second;
        }
        PageTextLocale const* GetPageTextLocale(uint32 entry) const
        {
            PageTextLocaleMap::const_iterator itr = mPageTextLocaleMap.find(entry);
            if (itr == mPageTextLocaleMap.end()) return NULL;
            return &itr->second;
        }
        GossipMenuItemsLocale const* GetGossipMenuItemsLocale(uint32 entry) const
        {
            GossipMenuItemsLocaleMap::const_iterator itr = mGossipMenuItemsLocaleMap.find(entry);
            if (itr == mGossipMenuItemsLocaleMap.end()) return NULL;
            return &itr->second;
        }

        GameObjectData const* GetGOData(uint32 guid) const
        {
            GameObjectDataMap::const_iterator itr = mGameObjectDataMap.find(guid);
            if (itr == mGameObjectDataMap.end()) return NULL;
            return &itr->second;
        }
        GameObjectData& NewGOData(uint32 guid)
        {
            return mGameObjectDataMap[guid];
        }
        void DeleteGOData(uint32 guid);

        OregonStringLocale const* GetOregonStringLocale(int32 entry) const
        {
            OregonStringLocaleMap::const_iterator itr = mOregonStringLocaleMap.find(entry);
            if (itr == mOregonStringLocaleMap.end()) return NULL;
            return &itr->second;
        }
        const char* GetOregonString(int32 entry, int locale_idx) const;
        const char* GetOregonStringForDBCLocale(int32 entry) const
        {
            return GetOregonString(entry, DBCLocaleIndex);
        }
        int32 GetDBCLocaleIndex() const
        {
            return DBCLocaleIndex;
        }
        void SetDBCLocaleIndex(uint32 lang)
        {
            DBCLocaleIndex = GetIndexForLocale(LocaleConstant(lang));
        }

        void AddCorpseCellData(uint32 mapid, uint32 cellid, uint32 player_guid, uint32 instance);
        void DeleteCorpseCellData(uint32 mapid, uint32 cellid, uint32 player_guid);

        time_t GetCreatureRespawnTime(uint32 loguid, uint32 instance)
        {
            return mCreatureRespawnTimes[MAKE_PAIR64(loguid, instance)];
        }
        void SaveCreatureRespawnTime(uint32 loguid, uint32 instance, time_t t);
        time_t GetGORespawnTime(uint32 loguid, uint32 instance)
        {
            return mGORespawnTimes[MAKE_PAIR64(loguid, instance)];
        }
        void SaveGORespawnTime(uint32 loguid, uint32 instance, time_t t);
        void DeleteRespawnTimeForInstance(uint32 instance);

        // grid objects
        void AddCreatureToGrid(uint32 guid, CreatureData const* data);
        void RemoveCreatureFromGrid(uint32 guid, CreatureData const* data);
        void AddGameobjectToGrid(uint32 guid, GameObjectData const* data);
        void RemoveGameobjectFromGrid(uint32 guid, GameObjectData const* data);
        uint32 AddGOData(uint32 entry, uint32 artKit, uint32 map, float x, float y, float z, float o, uint32 spawntimedelay = 0, float rotation0 = 0, float rotation1 = 0, float rotation2 = 0, float rotation3 = 0);
        uint32 AddCreData(uint32 entry, uint32 team, uint32 map, float x, float y, float z, float o, uint32 spawntimedelay = 0);

        // reserved names
        void LoadReservedPlayersNames();
        bool IsReservedName(const std::string& name) const
        {
            return m_ReservedNames.find(name) != m_ReservedNames.end();
        }

        // name with valid structure and symbols
        static bool IsValidName(const std::string& name, bool create = false);
        static bool IsValidCharterName(const std::string& name);
        static bool IsValidPetName(const std::string& name);

        static bool CheckDeclinedNames(std::wstring mainpart, DeclinedName const& names);

        int GetIndexForLocale(LocaleConstant loc);
        LocaleConstant GetLocaleForIndex(int i);
        // guild bank tabs
        uint32 GetGuildBankTabPrice(uint8 Index) const
        {
            return Index < GUILD_BANK_MAX_TABS ? mGuildBankTabPrice[Index] : 0;
        }

        GameTele const* GetGameTele(uint32 id) const
        {
            GameTeleMap::const_iterator itr = m_GameTeleMap.find(id);
            if (itr == m_GameTeleMap.end()) return NULL;
            return &itr->second;
        }
        GameTele const* GetGameTele(const std::string& name) const;
        GameTeleMap const& GetGameTeleMap() const
        {
            return m_GameTeleMap;
        }
        bool AddGameTele(GameTele& data);
        bool DeleteGameTele(const std::string& name);

        uint32 GetNpcGossip(uint32 entry) const
        {
            CacheNpcTextIdMap::const_iterator iter = m_mCacheNpcTextIdMap.find(entry);
            if (iter == m_mCacheNpcTextIdMap.end())
                return 0;

            return iter->second;
        }

        TrainerSpellData const* GetNpcTrainerSpells(uint32 entry) const
        {
            CacheTrainerSpellMap::const_iterator  iter = m_mCacheTrainerSpellMap.find(entry);
            if (iter == m_mCacheTrainerSpellMap.end())
                return NULL;

            return &iter->second;
        }

        VendorItemData const* GetNpcVendorItemList(uint32 entry) const
        {
            CacheVendorItemMap::const_iterator  iter = m_mCacheVendorItemMap.find(entry);
            if (iter == m_mCacheVendorItemMap.end())
                return NULL;

            return &iter->second;
        }
        void AddVendorItem(uint32 entry, uint32 item, uint32 maxcount, uint32 incrtime, uint32 ExtendedCost, bool savetodb = true); // for event
        bool RemoveVendorItem(uint32 entry, uint32 item, bool savetodb = true); // for event
        bool IsVendorItemValid(uint32 vendor_entry, uint32 item, uint32 maxcount, uint32 ptime, uint32 ExtendedCost, Player* pl = NULL, std::set<uint32>* skip_vendors = NULL, uint32 ORnpcflag = 0) const;

        void LoadScriptNames();
        ScriptNameMap& GetScriptNames()
        {
            return m_scriptNames;
        }
        const char* GetScriptName(uint32 id)
        {
            return id < m_scriptNames.size() ? m_scriptNames[id].c_str() : "";
        }
        uint32 GetScriptId(const char* name);

        GossipMenusMapBounds GetGossipMenusMapBounds(uint32 uiMenuId) const
        {
            return GossipMenusMapBounds(m_mGossipMenusMap.lower_bound(uiMenuId), m_mGossipMenusMap.upper_bound(uiMenuId));
        }

        GossipMenusMapBoundsNonConst GetGossipMenusMapBoundsNonConst(uint32 uiMenuId)
        {
            return GossipMenusMapBoundsNonConst(m_mGossipMenusMap.lower_bound(uiMenuId),m_mGossipMenusMap.upper_bound(uiMenuId));
        }

        GossipMenuItemsMapBounds GetGossipMenuItemsMapBounds(uint32 uiMenuId) const
        {
            return GossipMenuItemsMapBounds(m_mGossipMenuItemsMap.lower_bound(uiMenuId), m_mGossipMenuItemsMap.upper_bound(uiMenuId));
        }

        GossipMenuItemsMapBoundsNonConst GetGossipMenuItemsMapBoundsNonConst(uint32 uiMenuId)
        {
            return GossipMenuItemsMapBoundsNonConst(m_mGossipMenuItemsMap.lower_bound(uiMenuId),m_mGossipMenuItemsMap.upper_bound(uiMenuId));
        }

        static void AddLocaleString(std::string const& s, LocaleConstant locale, StringVector& data);
        static inline void GetLocaleString(const StringVector& data, int loc_idx, std::string& value)
        {
            if (data.size() > size_t(loc_idx) && !data[loc_idx].empty())
                value = data[loc_idx];
        }

    protected:

        // first free id for selected id type
        uint32 m_auctionid;
        uint32 m_mailid;
        uint32 m_ItemTextId;
        uint32 m_arenaTeamId;
        uint32 m_guildId;
        uint32 m_hiPetNumber;

        // first free low guid for seelcted guid type
        uint32 m_hiCharGuid;
        uint32 m_hiCreatureGuid;
        uint32 m_hiPetGuid;
        uint32 m_hiItemGuid;
        uint32 m_hiGoGuid;
        uint32 m_hiDoGuid;
        uint32 m_hiCorpseGuid;

        QuestMap            mQuestTemplates;

        typedef UNORDERED_MAP<uint32, GossipText*> GossipTextMap;
        typedef UNORDERED_MAP<uint32, uint32> QuestAreaTriggerMap;
        typedef UNORDERED_MAP<uint32, uint32> BattleMastersMap;
        typedef UNORDERED_MAP<uint32, std::string> ItemTextMap;
        typedef std::set<uint32> TavernAreaTriggerSet;
        typedef std::set<uint32> GameObjectForQuestSet;

        GroupSet            mGroupSet;
        GuildMap            mGuildMap;
        ArenaTeamMap        mArenaTeamMap;

        ItemMap             mItems;

        ItemTextMap         mItemTexts;

        QuestAreaTriggerMap mQuestAreaTriggerMap;
        BattleMastersMap    mBattleMastersMap;
        TavernAreaTriggerSet mTavernAreaTriggerSet;
        GameObjectForQuestSet mGameObjectForQuestSet;
        GossipTextMap       mGossipText;
        AreaTriggerMap      mAreaTriggers;
        AreaTriggerScriptMap  mAreaTriggerScripts;
        AccessRequirementMap  mAccessRequirements;

        RepOnKillMap        mRepOnKill;

        GossipMenusMap      m_mGossipMenusMap;
        GossipMenuItemsMap  m_mGossipMenuItemsMap;

        WeatherZoneMap      mWeatherZoneMap;

        PetCreateSpellMap   mPetCreateSpell;

        //character reserved names
        typedef std::set<std::string> ReservedNamesMap;
        ReservedNamesMap    m_ReservedNames;

        GraveYardMap        mGraveYardMap;

        GameTeleMap         m_GameTeleMap;

        ScriptNameMap       m_scriptNames;

        ReferFriendMap      m_referrerFriends;
        ReferFriendMap      m_referredFriends;

        typedef             std::vector<LocaleConstant> LocalForIndex;
        LocalForIndex        m_LocalForIndex;
        int GetOrNewIndexForLocale(LocaleConstant loc);

        int DBCLocaleIndex;

    private:
        void LoadScripts(ScriptsType type);
        void CheckScripts(ScriptsType type, std::set<int32>& ids);
        void ConvertCreatureAddonAuras(CreatureDataAddon* addon, char const* table, char const* guidEntryStr);
        void LoadQuestRelationsHelper(QuestRelations& map, std::string table, bool starter, bool go);

        typedef std::map<uint32, PetLevelInfo*> PetLevelInfoMap;
        // PetLevelInfoMap[creature_id][level]
        PetLevelInfoMap petInfo;                            // [creature_id][level]

        PlayerClassInfo playerClassInfo[MAX_CLASSES];

        void BuildPlayerLevelInfo(uint8 race, uint8 class_, uint8 level, PlayerLevelInfo* plinfo) const;
        PlayerInfo playerInfo[MAX_RACES][MAX_CLASSES];

        typedef std::map<uint32, uint32> BaseXPMap;         // [area level][base xp]
        BaseXPMap mBaseXPTable;

        typedef std::map<uint32, int32> FishingBaseSkillMap; // [areaId][base skill level]
        FishingBaseSkillMap mFishingBaseForArea;

        typedef std::map<uint32, std::vector<std::string> > HalfNameMap;
        HalfNameMap PetHalfName0;
        HalfNameMap PetHalfName1;

        MapObjectGuids mMapObjectGuids;
        CreatureDataMap mCreatureDataMap;
        CreatureLinkedRespawnMap mCreatureLinkedRespawnMap;
        CreatureLocaleMap mCreatureLocaleMap;
        GameObjectDataMap mGameObjectDataMap;
        GameObjectLocaleMap mGameObjectLocaleMap;
        ItemLocaleMap mItemLocaleMap;
        QuestLocaleMap mQuestLocaleMap;
        NpcTextLocaleMap mNpcTextLocaleMap;
        PageTextLocaleMap mPageTextLocaleMap;
        OregonStringLocaleMap mOregonStringLocaleMap;
        GossipMenuItemsLocaleMap mGossipMenuItemsLocaleMap;
        RespawnTimes mCreatureRespawnTimes;
        RespawnTimes mGORespawnTimes;

        typedef std::vector<uint32> GuildBankTabPriceMap;
        GuildBankTabPriceMap mGuildBankTabPrice;

        CacheNpcTextIdMap m_mCacheNpcTextIdMap;
        CacheVendorItemMap m_mCacheVendorItemMap;
        CacheTrainerSpellMap m_mCacheTrainerSpellMap;

        /// Stores temp summon data grouped by summoner's entry, summoner's type and group id
        TempSummonDataContainer _tempSummonDataStore;
};

#define sObjectMgr Oregon::Singleton<ObjectMgr>::Instance()

// scripting access functions
bool LoadOregonStrings(DatabaseType& db, char const* table, int32 start_value = MAX_CREATURE_AI_TEXT_STRING_ID, int32 end_value = std::numeric_limits<int32>::min());
uint32 GetAreaTriggerScriptId(uint32 trigger_id);
uint32 GetScriptId(const char* name);
ObjectMgr::ScriptNameMap& GetScriptNames();
GameObjectInfo const* GetGameObjectInfo(uint32 id);
CreatureInfo const* GetCreatureTemplate(uint32 id);
CreatureInfo const* GetCreatureTemplateStore(uint32 entry);
Quest const* GetQuestTemplateStore(uint32 entry);

#endif

