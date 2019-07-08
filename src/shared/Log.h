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

#ifndef OREGONCORE_LOG_H
#define OREGONCORE_LOG_H

#include "Common.h"
#include "Policies/Singleton.h"
#include "Database/DatabaseEnv.h"
#include <string>

class Config;

/// LogTypes, each value is bit position in logmask
enum LogTypes
{
    LOG_TYPE_STRING = 0,
    LOG_TYPE_BASIC,
    LOG_TYPE_DETAIL,
    LOG_TYPE_DEBUG,

    LOG_TYPE_ERROR,
    LOG_TYPE_ERROR_DB,
    LOG_TYPE_SQL,

    LOG_TYPE_ARENA,
    LOG_TYPE_WARDEN,
    LOG_TYPE_CHAT,
    LOG_TYPE_COMMAND,

    LOG_TYPE_CHAR,
    LOG_TYPE_REMOTE,

    LOG_TYPE_MAP,
    LOG_TYPE_VMAP,
    LOG_TYPE_MMAP,

    LOG_TYPE_NETWORK,

    MAX_LOG_TYPES
};

/// Presets of bitmasks, exists for backward compatibility
enum LogLevel
{
    LOGL_MINIMAL = 0,
    LOGL_BASIC,
    LOGL_DETAIL,
    LOGL_DEBUG
};

/// Colors - 16 possible values, upper 8 are bold and sometimes different in tone
/// User chooses from lower 8, upper 8 are used for category "[CATEGORY] message"
enum ColorTypes
{
    BLACK = 0,
    RED,
    GREEN,
    BROWN,
    BLUE,
    MAGENTA,
    CYAN,
    LGREY,
    GREY,
    LRED,
    LGREEN,
    YELLOW,
    LBLUE,
    LMAGENTA,
    LCYAN,
    WHITE,

    MAX_COLORS
};

/// Main logging class
class Log : public Oregon::Singleton<Log, Oregon::ClassLevelLockable<Log, ACE_Thread_Mutex> >
{
        friend class Oregon::OperatorNew<Log>;
        Log();
        ~Log();

    public:
        void Initialize();

        void InitColors(const std::string& init_str);
        void SetColor(ColorTypes color);
        void ResetColor();

        void outString(const char* fmt, ...)   ATTR_PRINTF(2, 3);
        void outBasic(const char* fmt, ...)    ATTR_PRINTF(2, 3);
        void outDetail(const char* fmt, ...)   ATTR_PRINTF(2, 3);
        void outDebug(const char* fmt, ...)    ATTR_PRINTF(2, 3);


        void outError(const char* fmt, ...)    ATTR_PRINTF(2, 3);
        void outErrorDb(const char* fmt, ...)  ATTR_PRINTF(2, 3);
		void outSQL(const char* fmt, ...)      ATTR_PRINTF(2, 3);

        void outArena(const char* fmt, ...)    ATTR_PRINTF(2, 3);
        void outWarden(const char* fmt, ...)   ATTR_PRINTF(2, 3);
        void outChat(const char* fmt, ...)     ATTR_PRINTF(2, 3);
        void outCommand(const char* fmt, ...)  ATTR_PRINTF(2, 3);
        
        void outChar(const char* fmt, ...)     ATTR_PRINTF(2, 3);
        void outRemote(const char* fmt, ...)   ATTR_PRINTF(2, 3);
        
        void outMap(const char* fmt, ...)      ATTR_PRINTF(2, 3);
        void outVMap(const char* fmt, ...)     ATTR_PRINTF(2, 3);
        void outMMap(const char* fmt, ...)     ATTR_PRINTF(2, 3);

        void outNetwork(const char* fmt, ...)  ATTR_PRINTF(2, 3);

        void outDB( LogTypes type, const char* str );
        void outString()
        {
            outString("");
        }
        void outDebugInLine(const char* fmt, ...) ATTR_PRINTF(2, 3);
        void outFatal( const char* err, ... )                  ATTR_PRINTF(2, 3) ATTR_NORETURN;
        void outCharDump( const char* str, uint32 account_id, uint32 guid, const char* name );
        void outCommand(uint64 account, const char* fmt, ...) ATTR_PRINTF(3, 4);
        void CreateUpdateFile(const char* str);

        static void outTimestamp(FILE* file);
        static std::string GetTimestampStr();

        void SetLogMask(unsigned long mask);
        void SetDBLogMask(unsigned long mask);

        unsigned long GetLogMask() const
        {
            return m_logMask;
        }

        unsigned long GetDBLogMask() const
        {
            return m_logMaskDatabase;
        }

        /// Checks whether outDebug works
        bool IsOutDebug() const
        {
            return (m_logMask | m_logMaskDatabase) & LOG_TYPE_DEBUG;
        }

        bool IsLogTypeEnabled(LogTypes type) const
        {
            return (m_logMask | m_logMaskDatabase) & (1 << type); 
        }

    private:
        /// Performs logging
        void DoLog(LogTypes type, bool newline, const char* prefix, const char* fmt, va_list ap, FILE* file = NULL);

        FILE* openLogFile(char const* configFileName, char const* configTimeStampFlag, char const* mode);

        /// opens specific file for account
        FILE* openGmlogPerAccount(uint64 account);

        FILE* m_logFiles[MAX_LOG_TYPES]; //!< files for each message type

        std::string m_logsDir;       //!< directory to put log files in
        std::string m_logsTimestamp;

        bool m_gmlog_per_account;   //!< flag: create separate log for every GM account?
        std::string m_gmlog_filename_format; //!< format for GM log filename

        ColorTypes m_colors[MAX_LOG_TYPES]; //!< colors assigned to individual message types

        unsigned long m_logMask;          //!< mask to filter messages sent to console and files
        unsigned long m_logMaskDatabase;  //!< mask to filter messages sent to db
};

/// Log class singleton
#define sLog Oregon::Singleton<Log>::Instance()

#ifndef DEBUG_LOG
#ifdef OREGON_DEBUG
/// Works only in debug mode
#define DEBUG_LOG Oregon::Singleton<Log>::Instance().outDebug
#else
#define DEBUG_LOG(...)
# endif // OREGON_DEBUG
#endif // DEBUG_LOG

/// Macros meant to be used by scripts
#define outstring_log Oregon::Singleton<Log>::Instance().outString
#define detail_log Oregon::Singleton<Log>::Instance().outDetail
#define debug_log Oregon::Singleton<Log>::Instance().outDebug
#define error_log Oregon::Singleton<Log>::Instance().outError
#define error_db_log Oregon::Singleton<Log>::Instance().outErrorDb

#endif

