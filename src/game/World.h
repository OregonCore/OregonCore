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

#ifndef __WORLD_H
#define __WORLD_H

#include "Common.h"
#include "Timer.h"
#include "Policies/Singleton.h"
#include "SharedDefines.h"
#include "ace/Atomic_Op.h"
#include "QueryResult.h"

#include <map>
#include <set>
#include <list>

class Object;
class WorldPacket;
class WorldSession;
class Player;
class Weather;
struct ScriptInfo;
class SqlResultQueue;
class QueryResult;
class WorldSocket;

// ServerMessages.dbc
enum ServerMessageType
{
    SERVER_MSG_SHUTDOWN_TIME      = 1,
    SERVER_MSG_RESTART_TIME       = 2,
    SERVER_MSG_STRING             = 3,
    SERVER_MSG_SHUTDOWN_CANCELLED = 4,
    SERVER_MSG_RESTART_CANCELLED  = 5
};

enum ShutdownMask
{
    SHUTDOWN_MASK_RESTART = 1,
    SHUTDOWN_MASK_IDLE    = 2,
};

enum ShutdownExitCode
{
    SHUTDOWN_EXIT_CODE = 0,
    ERROR_EXIT_CODE    = 1,
    RESTART_EXIT_CODE  = 2,
};

// Timers for different object refresh rates
enum WorldTimers
{
    WUPDATE_OBJECTS     = 0,
    WUPDATE_SESSIONS    = 1,
    WUPDATE_AUCTIONS    = 2,
    WUPDATE_WEATHERS    = 3,
    WUPDATE_UPTIME      = 4,
    WUPDATE_CORPSES     = 5,
    WUPDATE_EVENTS      = 6,
    WUPDATE_CLEANDB     = 7,
    WUPDATE_DELETECHARS = 8,
    WUPDATE_AUTOBROADCAST = 9,
    WUPDATE_COUNT       = 10
};

// Configuration elements
enum WorldConfigs
{
    CONFIG_COMPRESSION = 0,
    CONFIG_GRID_UNLOAD,
    CONFIG_INTERVAL_SAVE,
    CONFIG_INTERVAL_GRIDCLEAN,
    CONFIG_INTERVAL_MAPUPDATE,
    CONFIG_INTERVAL_CHANGEWEATHER,
    CONFIG_INTERVAL_DISCONNECT_TOLERANCE,
    CONFIG_PORT_WORLD,
    CONFIG_SOCKET_SELECTTIME,
    CONFIG_SOCKET_TIMEOUTTIME,
    CONFIG_SESSION_ADD_DELAY,
    CONFIG_GROUP_XP_DISTANCE,
    CONFIG_SIGHT_MONSTER,
    CONFIG_SIGHT_GUARDER,
    CONFIG_GAME_TYPE,
    CONFIG_REALM_ZONE,
    CONFIG_ALLOW_TWO_SIDE_ACCOUNTS,
    CONFIG_ALLOW_TWO_SIDE_INTERACTION_CHAT,
    CONFIG_ALLOW_TWO_SIDE_INTERACTION_CHANNEL,
    CONFIG_ALLOW_TWO_SIDE_INTERACTION_GROUP,
    CONFIG_ALLOW_TWO_SIDE_INTERACTION_GUILD,
    CONFIG_ALLOW_TWO_SIDE_INTERACTION_AUCTION,
    CONFIG_ALLOW_TWO_SIDE_INTERACTION_MAIL,
    CONFIG_ALLOW_TWO_SIDE_WHO_LIST,
    CONFIG_ALLOW_TWO_SIDE_ADD_FRIEND,
    CONFIG_ALLOW_TWO_SIDE_TRADE,
    CONFIG_STRICT_PLAYER_NAMES,
    CONFIG_STRICT_CHARTER_NAMES,
    CONFIG_STRICT_PET_NAMES,
    CONFIG_CHARACTERS_CREATING_DISABLED,
    CONFIG_CHARACTERS_PER_ACCOUNT,
    CONFIG_CHARACTERS_PER_REALM,
    CONFIG_SKIP_CINEMATICS,
    CONFIG_MAX_PLAYER_LEVEL,
    CONFIG_START_PLAYER_LEVEL,
    CONFIG_START_PLAYER_MONEY,
    CONFIG_MAX_HONOR_POINTS,
    CONFIG_START_HONOR_POINTS,
    CONFIG_MAX_ARENA_POINTS,
    CONFIG_START_ARENA_POINTS,
    CONFIG_INSTANCE_IGNORE_LEVEL,
    CONFIG_INSTANCE_IGNORE_RAID,
    CONFIG_INSTANCE_RESET_TIME_HOUR,
    CONFIG_INSTANCE_UNLOAD_DELAY,
    CONFIG_CAST_UNSTUCK,
    CONFIG_MAX_PRIMARY_TRADE_SKILL,
    CONFIG_MIN_PETITION_SIGNS,
    CONFIG_GM_LOGIN_STATE,
    CONFIG_GM_VISIBLE_STATE,
    CONFIG_GM_CHAT,
    CONFIG_GM_WISPERING_TO,
    CONFIG_GM_IN_GM_LIST,
    CONFIG_GM_IN_WHO_LIST,
    CONFIG_GM_MAIL,
    CONFIG_GM_LOG_TRADE,
    CONFIG_START_GM_LEVEL,
    CONFIG_ALLOW_GM_GROUP,
    CONFIG_ALLOW_GM_FRIEND,
    CONFIG_GROUP_VISIBILITY,
    CONFIG_MAIL_DELIVERY_DELAY,
    CONFIG_EXTERNAL_MAIL,
    CONFIG_EXTERNAL_MAIL_INTERVAL,
    CONFIG_UPTIME_UPDATE,
    CONFIG_SKILL_CHANCE_ORANGE,
    CONFIG_SKILL_CHANCE_YELLOW,
    CONFIG_SKILL_CHANCE_GREEN,
    CONFIG_SKILL_CHANCE_GREY,
    CONFIG_SKILL_CHANCE_MINING_STEPS,
    CONFIG_SKILL_CHANCE_SKINNING_STEPS,
    CONFIG_SKILL_PROSPECTING,
    CONFIG_SKILL_GAIN_CRAFTING,
    CONFIG_SKILL_GAIN_DEFENSE,
    CONFIG_SKILL_GAIN_GATHERING,
    CONFIG_SKILL_GAIN_WEAPON,
    CONFIG_MAX_OVERSPEED_PINGS,
    CONFIG_SAVE_RESPAWN_TIME_IMMEDIATELY,
    CONFIG_ALWAYS_MAX_SKILL_FOR_LEVEL,
    CONFIG_WEATHER,
    CONFIG_EXPANSION,
    CONFIG_CHATFLOOD_MESSAGE_COUNT,
    CONFIG_CHATFLOOD_MESSAGE_DELAY,
    CONFIG_CHATFLOOD_MUTE_TIME,
    CONFIG_EVENT_ANNOUNCE,
    CONFIG_CREATURE_FAMILY_FLEE_ASSISTANCE_RADIUS,
    CONFIG_CREATURE_FAMILY_ASSISTANCE_RADIUS,
    CONFIG_CREATURE_FAMILY_ASSISTANCE_DELAY,
    CONFIG_CREATURE_FAMILY_FLEE_DELAY,
    CONFIG_WORLD_BOSS_LEVEL_DIFF,
    CONFIG_QUEST_LOW_LEVEL_HIDE_DIFF,
    CONFIG_QUEST_HIGH_LEVEL_HIDE_DIFF,
    CONFIG_DETECT_POS_COLLISION,
    CONFIG_RESTRICTED_LFG_CHANNEL,
    CONFIG_SILENTLY_GM_JOIN_TO_CHANNEL,
    CONFIG_TALENTS_INSPECTING,
    CONFIG_CHAT_FAKE_MESSAGE_PREVENTING,
    CONFIG_CHAT_STRICT_LINK_CHECKING_SEVERITY,
    CONFIG_CHAT_STRICT_LINK_CHECKING_KICK,
    CONFIG_CORPSE_DECAY_NORMAL,
    CONFIG_CORPSE_DECAY_RARE,
    CONFIG_CORPSE_DECAY_ELITE,
    CONFIG_CORPSE_DECAY_RAREELITE,
    CONFIG_CORPSE_DECAY_WORLDBOSS,
    CONFIG_ADDON_CHANNEL,
    CONFIG_DEATH_SICKNESS_LEVEL,
    CONFIG_DEATH_CORPSE_RECLAIM_DELAY_PVP,
    CONFIG_DEATH_CORPSE_RECLAIM_DELAY_PVE,
    CONFIG_DEATH_BONES_WORLD,
    CONFIG_DEATH_BONES_BG_OR_ARENA,
    CONFIG_DIE_COMMAND_MODE,
    CONFIG_THREAT_RADIUS,
    CONFIG_CHANCE_OF_GM_SURVEY,
    CONFIG_INSTANT_LOGOUT,
    CONFIG_DISABLE_BREATHING,
    CONFIG_ALL_TAXI_PATHS,
    CONFIG_INSTANT_TAXI,
    CONFIG_DECLINED_NAMES_USED,
    CONFIG_LISTEN_RANGE_SAY,
    CONFIG_LISTEN_RANGE_TEXTEMOTE,
    CONFIG_LISTEN_RANGE_YELL,
    CONFIG_BATTLEGROUND_CAST_DESERTER,
    CONFIG_BATTLEGROUND_QUEUE_ANNOUNCER_ENABLE,
    CONFIG_BATTLEGROUND_QUEUE_ANNOUNCER_PLAYERONLY,
    CONFIG_BATTLEGROUND_QUEUE_ANNOUNCER_ONSTART,
    CONFIG_BATTLEGROUND_PREMATURE_REWARD,
    CONFIG_BATTLEGROUND_PREMATURE_FINISH_TIMER,
    CONFIG_BATTLEGROUND_WRATH_LEAVE_MODE,
    CONFIG_ARENA_MAX_RATING_DIFFERENCE,
    CONFIG_ARENA_RATING_DISCARD_TIMER,
    CONFIG_ARENA_AUTO_DISTRIBUTE_POINTS,
    CONFIG_ARENA_AUTO_DISTRIBUTE_INTERVAL_DAYS,
    CONFIG_ARENA_HIDE_FROM_SOCIAL,
    CONFIG_ARENA_LOG_EXTENDED_INFO,
    CONFIG_MAX_WHO,
    CONFIG_BG_START_MUSIC,
    CONFIG_START_ALL_SPELLS,
    CONFIG_HONOR_AFTER_DUEL,
    CONFIG_START_ALL_EXPLORED,
    CONFIG_START_ALL_REP,
    CONFIG_ALWAYS_MAXSKILL,
    CONFIG_PVP_TOKEN_ENABLE,
    CONFIG_PVP_TOKEN_MAP_TYPE,
    CONFIG_PVP_TOKEN_ID,
    CONFIG_PVP_TOKEN_COUNT,
    CONFIG_NO_RESET_TALENT_COST,
    CONFIG_SHOW_KICK_IN_WORLD,
    CONFIG_INTERVAL_LOG_UPDATE,
    CONFIG_MIN_LOG_UPDATE,
    CONFIG_ENABLE_SINFO_LOGIN,
    CONFIG_PET_LOS,
    CONFIG_VMAP_TOTEM,
    CONFIG_NUMTHREADS,
    CONFIG_CHATLOG_CHANNEL,
    CONFIG_CHATLOG_WHISPER,
    CONFIG_CHATLOG_SYSCHAN,
    CONFIG_CHATLOG_PARTY,
    CONFIG_CHATLOG_RAID,
    CONFIG_CHATLOG_GUILD,
    CONFIG_CHATLOG_PUBLIC,
    CONFIG_CHATLOG_ADDON,
    CONFIG_CHATLOG_BGROUND,
    CONFIG_LOGDB_CLEARINTERVAL,
    CONFIG_LOGDB_CLEARTIME,
    CONFIG_DUEL_MOD,
    CONFIG_DUEL_CD_RESET,
    CONFIG_CHARDELETE_KEEP_DAYS,
    CONFIG_CHARDELETE_METHOD,
    CONFIG_CHARDELETE_MIN_LEVEL,
    CONFIG_AUTOBROADCAST_TIMER,
    CONFIG_AUTOBROADCAST_ENABLED,
    CONFIG_AUTOBROADCAST_CENTER,
    CONFIG_WARDEN_ENABLED,
    CONFIG_WARDEN_KICK,
    CONFIG_WARDEN_NUM_CHECKS,
    CONFIG_WARDEN_CLIENT_CHECK_HOLDOFF,
    CONFIG_WARDEN_CLIENT_RESPONSE_DELAY,
    CONFIG_RAF_LEVEL_LIMIT,
    CONFIG_MAX_RESULTS_LOOKUP_COMMANDS,
    CONFIG_BOOL_MMAP_ENABLED,
    CONFIG_UI_QUESTLEVELS_IN_DIALOGS,
    CONFIG_CREATURE_PICKPOCKET_REFILL,
    CONFIG_SQLUPDATER_ENABLED,
    CONFIG_HEALTH_IN_PERCENTS,
    CONFIG_WARSONG_TIMER,
    CONFIG_CREATEUPDATE_FILE,
    CONFIG_CONSOLE_RESTART_TIMER,
    CONFIG_BG_XP_FOR_KILL,
    CONFIG_VALUE_COUNT
};

// Server rates
enum Rates
{
    RATE_HEALTH = 0,
    RATE_POWER_MANA,
    RATE_POWER_RAGE_INCOME,
    RATE_POWER_RAGE_LOSS,
    RATE_POWER_FOCUS,
    RATE_POWER_ENERGY,
    RATE_SKILL_DISCOVERY,
    RATE_DROP_ITEM_POOR,
    RATE_DROP_ITEM_NORMAL,
    RATE_DROP_ITEM_UNCOMMON,
    RATE_DROP_ITEM_RARE,
    RATE_DROP_ITEM_EPIC,
    RATE_DROP_ITEM_LEGENDARY,
    RATE_DROP_ITEM_ARTIFACT,
    RATE_DROP_ITEM_REFERENCED,
    RATE_DROP_MONEY,
    RATE_XP_KILL,
    RATE_XP_QUEST,
    RATE_XP_EXPLORE,
    RATE_XP_PAST_70,
    RATE_REPUTATION_GAIN,
    RATE_CREATURE_NORMAL_HP,
    RATE_CREATURE_ELITE_ELITE_HP,
    RATE_CREATURE_ELITE_RAREELITE_HP,
    RATE_CREATURE_ELITE_WORLDBOSS_HP,
    RATE_CREATURE_ELITE_RARE_HP,
    RATE_CREATURE_NORMAL_DAMAGE,
    RATE_CREATURE_ELITE_ELITE_DAMAGE,
    RATE_CREATURE_ELITE_RAREELITE_DAMAGE,
    RATE_CREATURE_ELITE_WORLDBOSS_DAMAGE,
    RATE_CREATURE_ELITE_RARE_DAMAGE,
    RATE_CREATURE_NORMAL_SPELLDAMAGE,
    RATE_CREATURE_ELITE_ELITE_SPELLDAMAGE,
    RATE_CREATURE_ELITE_RAREELITE_SPELLDAMAGE,
    RATE_CREATURE_ELITE_WORLDBOSS_SPELLDAMAGE,
    RATE_CREATURE_ELITE_RARE_SPELLDAMAGE,
    RATE_CREATURE_AGGRO,
    RATE_REST_INGAME,
    RATE_REST_OFFLINE_IN_TAVERN_OR_CITY,
    RATE_REST_OFFLINE_IN_WILDERNESS,
    RATE_DAMAGE_FALL,
    RATE_AUCTION_TIME,
    RATE_AUCTION_DEPOSIT,
    RATE_AUCTION_CUT,
    RATE_HONOR,
    RATE_MINING_AMOUNT,
    RATE_MINING_NEXT,
    RATE_TALENT,
    RATE_LOYALTY,
    RATE_CORPSE_DECAY_LOOTED,
    RATE_INSTANCE_RESET_TIME,
    RATE_TARGET_POS_RECALCULATION_RANGE,
    RATE_DURABILITY_LOSS_DAMAGE,
    RATE_DURABILITY_LOSS_PARRY,
    RATE_DURABILITY_LOSS_ABSORB,
    RATE_DURABILITY_LOSS_BLOCK,
    RATE_RAF_BONUS_XP,
    RATE_RAF_GRANTABLE_LEVELS_PER_LEVEL,
    MAX_RATES
};

// Type of server
enum RealmType
{
    REALM_TYPE_NORMAL = 0,
    REALM_TYPE_PVP = 1,
    REALM_TYPE_NORMAL2 = 4,
    REALM_TYPE_RP = 6,
    REALM_TYPE_RPPVP = 8,
    REALM_TYPE_FFA_PVP = 16                                 // custom, free for all pvp mode like arena PvP in all zones except rest activated places and sanctuaries
                         // replaced by REALM_PVP in realm list
};

enum RealmZone
{
    REALM_ZONE_UNKNOWN       = 0,                           // any language
    REALM_ZONE_DEVELOPMENT   = 1,                           // any language
    REALM_ZONE_UNITED_STATES = 2,                           // extended-Latin
    REALM_ZONE_OCEANIC       = 3,                           // extended-Latin
    REALM_ZONE_LATIN_AMERICA = 4,                           // extended-Latin
    REALM_ZONE_TOURNAMENT_5  = 5,                           // basic-Latin at create, any at login
    REALM_ZONE_KOREA         = 6,                           // East-Asian
    REALM_ZONE_TOURNAMENT_7  = 7,                           // basic-Latin at create, any at login
    REALM_ZONE_ENGLISH       = 8,                           // extended-Latin
    REALM_ZONE_GERMAN        = 9,                           // extended-Latin
    REALM_ZONE_FRENCH        = 10,                          // extended-Latin
    REALM_ZONE_SPANISH       = 11,                          // extended-Latin
    REALM_ZONE_RUSSIAN       = 12,                          // Cyrillic
    REALM_ZONE_TOURNAMENT_13 = 13,                          // basic-Latin at create, any at login
    REALM_ZONE_TAIWAN        = 14,                          // East-Asian
    REALM_ZONE_TOURNAMENT_15 = 15,                          // basic-Latin at create, any at login
    REALM_ZONE_CHINA         = 16,                          // East-Asian
    REALM_ZONE_CN1           = 17,                          // basic-Latin at create, any at login
    REALM_ZONE_CN2           = 18,                          // basic-Latin at create, any at login
    REALM_ZONE_CN3           = 19,                          // basic-Latin at create, any at login
    REALM_ZONE_CN4           = 20,                          // basic-Latin at create, any at login
    REALM_ZONE_CN5           = 21,                          // basic-Latin at create, any at login
    REALM_ZONE_CN6           = 22,                          // basic-Latin at create, any at login
    REALM_ZONE_CN7           = 23,                          // basic-Latin at create, any at login
    REALM_ZONE_CN8           = 24,                          // basic-Latin at create, any at login
    REALM_ZONE_TOURNAMENT_25 = 25,                          // basic-Latin at create, any at login
    REALM_ZONE_TEST_SERVER   = 26,                          // any language
    REALM_ZONE_TOURNAMENT_27 = 27,                          // basic-Latin at create, any at login
    REALM_ZONE_QA_SERVER     = 28,                          // any language
    REALM_ZONE_CN9           = 29                           // basic-Latin at create, any at login
};

// DB scripting commands
enum ScriptCommands
{
    SCRIPT_COMMAND_TALK                  = 0,                // source/target = Creature, target = any, datalong = talk type (0=say, 1=whisper, 2=yell, 3=emote text, 4=boss emote text), datalong2 & 1 = player talk (instead of creature), dataint = string_id
    SCRIPT_COMMAND_EMOTE                 = 1,                // source/target = Creature, datalong = emote id, datalong2 = 0: set emote state; > 0: play emote state
    SCRIPT_COMMAND_FIELD_SET             = 2,                // source/target = Creature, datalong = field id, datalog2 = value
    SCRIPT_COMMAND_MOVE_TO               = 3,                // source/target = Creature, datalong2 = time to reach, x/y/z = destination
    SCRIPT_COMMAND_FLAG_SET              = 4,                // source/target = Creature, datalong = field id, datalog2 = bitmask
    SCRIPT_COMMAND_FLAG_REMOVE           = 5,                // source/target = Creature, datalong = field id, datalog2 = bitmask
    SCRIPT_COMMAND_TELEPORT_TO           = 6,                // source/target = Creature/Player (see datalong2), datalong = map_id, datalong2 = 0: Player; 1: Creature, x/y/z = destination, o = orientation
    SCRIPT_COMMAND_QUEST_EXPLORED        = 7,                // target/source = Player, target/source = GO/Creature, datalong = quest id, datalong2 = distance or 0
    SCRIPT_COMMAND_KILL_CREDIT           = 8,                // target/source = Player, datalong = creature entry, datalong2 = 0: personal credit, 1: group credit
    SCRIPT_COMMAND_RESPAWN_GAMEOBJECT    = 9,                // source = WorldObject (summoner), datalong = GO guid, datalong2 = despawn delay
    SCRIPT_COMMAND_TEMP_SUMMON_CREATURE  = 10,               // source = WorldObject (summoner), datalong = creature entry, datalong2 = despawn delay, x/y/z = summon position, o = orientation
    SCRIPT_COMMAND_OPEN_DOOR             = 11,               // source = Unit, datalong = GO guid, datalong2 = reset delay (min 15)
    SCRIPT_COMMAND_CLOSE_DOOR            = 12,               // source = Unit, datalong = GO guid, datalong2 = reset delay (min 15)
    SCRIPT_COMMAND_ACTIVATE_OBJECT       = 13,               // source = Unit, target = GO
    SCRIPT_COMMAND_REMOVE_AURA           = 14,               // source (datalong2 != 0) or target (datalong2 == 0) = Unit, datalong = spell id
    SCRIPT_COMMAND_CAST_SPELL            = 15,               // source and/or target = Unit, datalong2 = cast direction (0: s->t 1: s->s 2: t->t 3: t->s 4: s->creature with dataint entry), dataint & 1 = triggered flag
    SCRIPT_COMMAND_PLAY_SOUND            = 16,               // source = WorldObject, target = none/Player, datalong = sound id, datalong2 (bitmask: 0/1=anyone/player, 0/2=without/with distance dependency, so 1|2 = 3 is target with distance dependency)
    SCRIPT_COMMAND_CREATE_ITEM           = 17,               // target/source = Player, datalong = item entry, datalong2 = amount
    SCRIPT_COMMAND_DESPAWN_SELF          = 18,               // target/source = Creature, datalong = despawn delay
    SCRIPT_COMMAND_DO_NOTHING            = 19,               // do nothing
    SCRIPT_COMMAND_LOAD_PATH             = 20,               // source = Unit, datalong = path id, datalong2 = is repeatable
    SCRIPT_COMMAND_CALLSCRIPT_TO_UNIT    = 21,               // source = WorldObject (if present used as a search center), datalong = script id, datalong2 = unit lowguid, dataint = script table to use (see ScriptsType)
    SCRIPT_COMMAND_KILL                  = 22,               // source/target = Creature, dataint = remove corpse attribute

    //Oregon only
    SCRIPT_COMMAND_ORIENTATION           = 30,               // source = Unit, target (datalong > 0) = Unit, datalong = > 0 turn source to face target, o = orientation
    SCRIPT_COMMAND_EQUIP                 = 31,               // soucre = Creature, datalong = equipment id
    SCRIPT_COMMAND_MODEL                 = 32,               // source = Creature, datalong = model id
    SCRIPT_COMMAND_CLOSE_GOSSIP          = 33,               // source = Player
};

// Storage class for commands issued for delayed execution
struct CliCommandHolder
{
    typedef void Print(void*, const char*);
    typedef void CommandFinished(void*, bool success);

    void* m_callbackArg;
    char* m_command;
    Print* m_print;

    CommandFinished* m_commandFinished;

    CliCommandHolder(void* callbackArg, const char* command, Print* zprint, CommandFinished* commandFinished)
        : m_callbackArg(callbackArg), m_print(zprint), m_commandFinished(commandFinished)
    {
        size_t len = strlen(command) + 1;
        m_command = new char[len];
        memcpy(m_command, command, len);
    }

    ~CliCommandHolder()
    {
        delete[] m_command;
    }
};

/// What should happen if the threshold per interval is passed.
enum OpcodePenalty
{
    OPCODE_PENALTY_SKIP = 0, //!< Skip the packet without processing
    OPCODE_PENALTY_KICK = 1  //!< Kick the player
};

/// Protected Opcode @see Opcodes.h
struct ProtectedOpcodeProperties
{
    uint32 threshold;      //!< Sets the maximum count one protected packet per Interval can be processed per session.
    uint32 interval;       //!< Interval for threshold, in milliseconds.'
    OpcodePenalty penalty; //!< What should happen if the threshold per interval is passed.
};

typedef UNORDERED_MAP<uint32, WorldSession*> SessionMap;

// The World
class World
{
    public:
        static volatile uint32 m_worldLoopCounter;

        World();
        ~World();

        WorldSession* FindSession(uint32 id) const;
        void AddSession(WorldSession* s);
        void SendAutoBroadcast();
        bool RemoveSession(uint32 id);
        // Get the number of current active sessions
        void UpdateMaxSessionCounters();
        const SessionMap& GetAllSessions() const { return m_sessions; }
        uint32 GetActiveAndQueuedSessionCount() const
        {
            return m_sessions.size();
        }
        uint32 GetActiveSessionCount() const
        {
            return m_sessions.size() - m_QueuedPlayer.size();
        }
        uint32 GetQueuedSessionCount() const
        {
            return m_QueuedPlayer.size();
        }
        // Get the maximum number of parallel sessions on the server since last reboot
        uint32 GetMaxQueuedSessionCount() const
        {
            return m_maxQueuedSessionCount;
        }
        uint32 GetMaxActiveSessionCount() const
        {
            return m_maxActiveSessionCount;
        }
        Player* FindPlayerInZone(uint32 zone);

        Weather* FindWeather(uint32 id) const;
        Weather* AddWeather(uint32 zone_id);
        void RemoveWeather(uint32 zone_id);

        // Get the active session server limit (or security level limitations)
        uint32 GetPlayerAmountLimit() const
        {
            return m_playerLimit >= 0 ? m_playerLimit : 0;
        }
        AccountTypes GetPlayerSecurityLimit() const
        {
            return m_allowedSecurityLevel < 0 ? SEC_PLAYER : m_allowedSecurityLevel;
        }
        void SetPlayerSecurityLimit(AccountTypes sec)
        {
            m_allowedSecurityLevel = (sec < SEC_PLAYER ? SEC_PLAYER : sec);
        }

        // Set the active session server limit (or security level limitation)
        void SetPlayerLimit(int32 limit, bool needUpdate = false);

        //player Queue
        typedef std::list<WorldSession*> Queue;
        void AddQueuedPlayer(WorldSession*);
        bool RemoveQueuedPlayer(WorldSession* session);
        int32 GetQueuePos(WorldSession*);
        bool HasRecentlyDisconnected(WorldSession*);
        uint32 GetQueueSize() const
        {
            return m_QueuedPlayer.size();
        }

        // todo Actions on m_allowMovement still to be implemented
        // Is movement allowed?
        bool getAllowMovement() const
        {
            return m_allowMovement;
        }
        // Allow/Disallow object movements
        void SetAllowMovement(bool allow)
        {
            m_allowMovement = allow;
        }

        // Set a new Message of the Day
        void SetMotd(std::string motd)
        {
            m_motd = motd;
        }
        // Get the current Message of the Day
        const char* GetMotd() const
        {
            return m_motd.c_str();
        }

        // Set the string for new characters (first login)
        void SetNewCharString(std::string str)
        {
            m_newCharString = str;
        }
        // Get the string for new characters (first login)
        const std::string& GetNewCharString() const
        {
            return m_newCharString;
        }

        uint32 GetDefaultDbcLocale() const
        {
            return m_defaultDbcLocale;
        }

        // Get the path where data (dbc, maps) are stored on disk
        std::string GetDataPath() const
        {
            return m_dataPath;
        }

        // When server started?
        time_t const& GetStartTime() const
        {
            return m_startTime;
        }
        // What time is it?
        time_t const& GetGameTime() const
        {
            return m_gameTime;
        }
        // Uptime (in secs)
        uint32 GetUptime() const
        {
            return uint32(m_gameTime - m_startTime);
        }
        // Update time
        uint32 GetUpdateTime() const
        {
            return m_updateTime;
        }
        void SetRecordDiffInterval(int32 t)
        {
            if (t >= 0) m_configs[CONFIG_INTERVAL_LOG_UPDATE] = (uint32)t;
        }

        // Get the maximum skill level a player can reach
        uint16 GetConfigMaxSkillValue() const
        {
            uint32 lvl = getConfig(CONFIG_MAX_PLAYER_LEVEL);
            return lvl > 60 ? 300 + ((lvl - 60) * 75) / 10 : lvl * 5;
        }

        void SetInitialWorldSettings();
        void LoadConfigSettings(bool reload = false);

        void SendWorldText(int32 string_id, ...);
        void SendGlobalText(const char* text, WorldSession* self);
        void SendGMText(int32 string_id, ...);
        void SendGlobalMessage(WorldPacket* packet, WorldSession* self = 0, uint32 team = 0);
        void SendGlobalGMMessage(WorldPacket* packet, WorldSession* self = 0, uint32 team = 0);
        bool SendZoneMessage(uint32 zone, WorldPacket* packet, WorldSession* self = 0, uint32 team = 0);
        void SendZoneText(uint32 zone, const char* text, WorldSession* self = 0, uint32 team = 0);
        void SendServerMessage(ServerMessageType type, const char* text = "", Player* player = NULL);

        // Are we in the middle of a shutdown?
        bool IsShuttingDown() const
        {
            return m_ShutdownTimer > 0;
        }
        uint32 GetShutDownTimeLeft()
        {
            return m_ShutdownTimer;
        }
        void ShutdownServ(uint32 time, uint32 options, uint8 exitcode);
        void ShutdownCancel();
        void ShutdownMsg(bool show = false, Player* player = NULL);
        static uint8 GetExitCode()
        {
            return m_ExitCode;
        }
        static void StopNow(uint8 exitcode)
        {
            if (m_stopEvent) return;
            m_stopEvent = true;
            m_ExitCode = exitcode;
        }
        static bool IsStopped()
        {
            return m_stopEvent;
        }

        void Update(uint32 diff);

        void UpdateSessions(time_t diff);
        // Set a server rate (see #Rates)
        void setRate(Rates rate, float value)
        {
            rate_values[rate] = value;
        }
        // Get a server rate (see #Rates)
        float getRate(Rates rate) const
        {
            return rate_values[rate];
        }

        // Set a server configuration element (see #WorldConfigs)
        void setConfig(uint32 index, uint32 value)
        {
            if (index < CONFIG_VALUE_COUNT)
                m_configs[index] = value;
        }

        // Get a server configuration element (see #WorldConfigs)
        uint32 getConfig(uint32 index) const
        {
            if (index < CONFIG_VALUE_COUNT)
                return m_configs[index];
            else
                return 0;
        }

        // Are we on a "Player versus Player" server?
        bool IsPvPRealm()
        {
            return (getConfig(CONFIG_GAME_TYPE) == REALM_TYPE_PVP || getConfig(CONFIG_GAME_TYPE) == REALM_TYPE_RPPVP || getConfig(CONFIG_GAME_TYPE) == REALM_TYPE_FFA_PVP);
        }
        bool IsFFAPvPRealm()
        {
            return getConfig(CONFIG_GAME_TYPE) == REALM_TYPE_FFA_PVP;
        }

        bool KickPlayer(const std::string& playerName);
        void KickAll();
        void KickAllLess(AccountTypes sec);
        BanReturn BanAccount(BanMode mode, std::string nameOrIP, std::string duration, std::string reason, std::string author);
        bool RemoveBanAccount(BanMode mode, std::string nameOrIP);

        uint32 IncreaseScheduledScriptsCount()
        {
            return (uint32)++m_scheduledScripts;
        }
        uint32 DecreaseScheduledScriptCount()
        {
            return (uint32)--m_scheduledScripts;
        }
        uint32 DecreaseScheduledScriptCount(size_t count)
        {
            return (uint32)(m_scheduledScripts -= count);
        }
        bool IsScriptScheduled() const
        {
            return m_scheduledScripts > 0;
        }

        // for max speed access
        static float GetMaxVisibleDistanceOnContinents()
        {
            return m_MaxVisibleDistanceOnContinents;
        }
        static float GetMaxVisibleDistanceInInstances()
        {
            return m_MaxVisibleDistanceInInstances;
        }
        static float GetMaxVisibleDistanceInBGArenas()
        {
            return m_MaxVisibleDistanceInBGArenas;
        }

        static int32 GetVisibilityNotifyPeriodOnContinents()
        {
            return m_visibility_notify_periodOnContinents;
        }
        static int32 GetVisibilityNotifyPeriodInInstances()
        {
            return m_visibility_notify_periodInInstances;
        }
        static int32 GetVisibilityNotifyPeriodInBGArenas()
        {
            return m_visibility_notify_periodInBGArenas;
        }

        void ProcessCliCommands();
        void QueueCliCommand(CliCommandHolder* commandHolder)
        {
            cliCmdQueue.add(commandHolder);
        }

        void UpdateResultQueue();
        void InitResultQueue();

        void ForceGameEventUpdate();

        void UpdateRealmCharCount(uint32 accid);

        void UpdateAllowedSecurity();

        LocaleConstant GetAvailableDbcLocale(LocaleConstant locale) const
        {
            if (m_availableDbcLocaleMask & (1 << locale)) return locale;
            else return m_defaultDbcLocale;
        }

        //used World DB version
        void LoadDBVersion();
        char const* GetDBVersion()
        {
            return m_DBVersion.c_str();
        }

        //used Script version
        void SetScriptsVersion(char const* version)
        {
            m_ScriptsVersion = version ? version : "unknown scripting library";
        }
        char const* GetScriptsVersion()
        {
            return m_ScriptsVersion.c_str();
        }

        void RecordTimeDiff(const char* text, ...);
        void LoadAutobroadcasts();
        void LoadIp2nation();
        void LoadOpcodeProtection();
        void LoadSQLUpdates();

        ProtectedOpcodeProperties const& GetProtectedOpcodeProperties(uint32 opcode);
    protected:
        void _UpdateGameTime();
        // callback for UpdateRealmCharacters
        void _UpdateRealmCharCount(QueryResult_AutoPtr resultCharCount, uint32 accountId);

        void InitDailyQuestResetTime();
        void ResetDailyQuests();
    private:
        static volatile bool m_stopEvent;
        static uint8 m_ExitCode;
        uint32 m_ShutdownTimer;
        uint32 m_ShutdownMask;

        //atomic op counter for active scripts amount
        ACE_Atomic_Op<ACE_Thread_Mutex, long> m_scheduledScripts;

        time_t m_startTime;
        time_t m_gameTime;
        IntervalTimer m_timers[WUPDATE_COUNT];
        IntervalTimer extmail_timer;
        uint32 mail_timer;
        uint32 mail_timer_expires;
        uint32 m_updateTime, m_updateTimeSum;
        uint32 m_updateTimeCount;
        uint32 m_currentTime;

        typedef UNORDERED_MAP<uint32, Weather*> WeatherMap;
        WeatherMap m_weathers;

        SessionMap m_sessions;
        typedef UNORDERED_MAP<uint32, time_t> DisconnectMap;
        DisconnectMap m_disconnects;
        uint32 m_maxActiveSessionCount;
        uint32 m_maxQueuedSessionCount;

        std::string m_newCharString;

        float rate_values[MAX_RATES];
        uint32 m_configs[CONFIG_VALUE_COUNT];
        int32 m_playerLimit;
        AccountTypes m_allowedSecurityLevel;
        LocaleConstant m_defaultDbcLocale;                     // from config for one from loaded DBC locales
        uint32 m_availableDbcLocaleMask;                       // by loaded DBC
        void DetectDBCLang();
        bool m_allowMovement;
        std::string m_motd;
        std::string m_dataPath;

        // for max speed access
        static float m_MaxVisibleDistanceOnContinents;
        static float m_MaxVisibleDistanceInInstances;
        static float m_MaxVisibleDistanceInBGArenas;

        static int32 m_visibility_notify_periodOnContinents;
        static int32 m_visibility_notify_periodInInstances;
        static int32 m_visibility_notify_periodInBGArenas;

        // CLI command holder to be thread safe
        ACE_Based::LockedQueue<CliCommandHolder*, ACE_Thread_Mutex> cliCmdQueue;
        SqlResultQueue* m_resultQueue;

        // next daily quests reset time
        time_t m_NextDailyQuestReset;

        //Player Queue
        Queue m_QueuedPlayer;

        //sessions that are added async
        void AddSession_(WorldSession* s);
        ACE_Based::LockedQueue<WorldSession*, ACE_Thread_Mutex> addSessQueue;

        //used versions
        std::string m_DBVersion;
        std::string m_ScriptsVersion;

        std::list<std::string> m_Autobroadcasts;
        std::string m_SQLUpdatesPath;
        UNORDERED_MAP<uint32, ProtectedOpcodeProperties> _protectedOpcodesProperties;
};

extern uint32 realmID;

#define sWorld Oregon::Singleton<World>::Instance()
#endif

