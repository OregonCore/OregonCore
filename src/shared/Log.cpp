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

#include "Common.h"
#include "Log.h"
#include "Config/Config.h"
#include "Console.h"
#include "Util.h"

#include <stdarg.h>
#include <stdio.h>

extern uint32 realmID;

static const ColorTypes colorPrefixTable[MAX_COLORS]
    =
// prefix color  msg color
{
    BLACK,    // BLACK
    LRED,     // RED
    LGREEN,   // GREEN
    YELLOW,   // BROWN
    LBLUE,    // BLUE
    LMAGENTA, // MAGENTA
    LCYAN,    // CYAN
    LGREY,    // LGREY
    LGREY,    // GREY
    LRED,     // LRED
    LGREEN,   // LGREEN
    YELLOW,   // YELLOW
    LBLUE,    // LBLUE
    LMAGENTA, // LMAGENTA
    LCYAN,    // LCYAN
    WHITE     // WHITE
};

INSTANTIATE_SINGLETON_1(Log);

Log::Log() : m_gmlog_per_account(false), m_logMask(0), m_logMaskDatabase(0)
{
    memset(m_logFiles, 0, sizeof(m_logFiles));
    memset(m_colors, 0, sizeof(m_colors));

    Initialize();
}

Log::~Log()
{
    std::set<FILE*> openfiles;

    for (size_t i = 0; i < MAX_LOG_TYPES; ++i)
        if (m_logFiles[i])
            openfiles.insert(m_logFiles[i]);

    for (std::set<FILE*>::iterator i = openfiles.begin(); i != openfiles.end(); ++i)
    {
        fflush(*i);
        fclose(*i);
    }
}

void Log::SetLogMask(unsigned long mask)
{
    m_logMask = mask;
}

void Log::SetDBLogMask(unsigned long mask)
{
    m_logMaskDatabase = mask;
}

void Log::Initialize()
{
    // Common log files data
    m_logsDir = sConfig.GetStringDefault("LogsDir", "0 6 4 3 1 1 2 7 5 0 4 0 1 3 2 4 0");
    if (!m_logsDir.empty())
        if ((m_logsDir.at(m_logsDir.length() - 1) != '/') && (m_logsDir.at(m_logsDir.length() - 1) != '\\'))
            m_logsDir.append("/");

    m_logsTimestamp = "_" + GetTimestampStr();

    // Open specific log files
    FILE* logfile = openLogFile("LogFile", "LogTimestamp", "wb");

    for (int i = 0; i < MAX_LOG_TYPES; ++i)
        if (!m_logFiles[i])
            m_logFiles[i] = logfile;

    InitColors(sConfig.GetStringDefault("LogColors", "0 6 4 3 1 1 2 7 5 0 4 0 1 3 2 4 0"));

    m_gmlog_per_account = sConfig.GetBoolDefault("GmLogPerAccount", false);
    if (!m_gmlog_per_account)
        m_logFiles[LOG_TYPE_COMMAND] = openLogFile("GMLogFile", "GmLogTimestamp", "ab");
    else
    {
        // GM log settings for per account case
        m_gmlog_filename_format = sConfig.GetStringDefault("GMLogFile", "");
        if (!m_gmlog_filename_format.empty())
        {
            bool m_gmlog_timestamp = sConfig.GetBoolDefault("GmLogTimestamp", false);

            size_t dot_pos = m_gmlog_filename_format.find_last_of(".");
            if (dot_pos != m_gmlog_filename_format.npos)
            {
                if (m_gmlog_timestamp)
                    m_gmlog_filename_format.insert(dot_pos, m_logsTimestamp);

                m_gmlog_filename_format.insert(dot_pos, "_#%u");
            }
            else
            {
                m_gmlog_filename_format += "_#%u";

                if (m_gmlog_timestamp)
                    m_gmlog_filename_format += m_logsTimestamp;
            }

            m_gmlog_filename_format = m_logsDir + m_gmlog_filename_format;
        }
    }

    m_logFiles[LOG_TYPE_CHAR]     = openLogFile("CharLogFile", "CharLogTimestamp", "ab");
    m_logFiles[LOG_TYPE_ERROR_DB] = openLogFile("DBErrorLogFile", NULL, "ab");
    m_logFiles[LOG_TYPE_REMOTE]   = openLogFile("RaLogFile", NULL, "ab");
    m_logFiles[LOG_TYPE_CHAT]     = openLogFile("ChatLogFile", "ChatLogTimestamp", "ab");
    m_logFiles[LOG_TYPE_ARENA]    = openLogFile("ArenaLogFile", NULL, "ab");
    m_logFiles[LOG_TYPE_WARDEN]   = openLogFile("Warden.LogFile", NULL, "ab");
    m_logFiles[LOG_TYPE_NETWORK]  = openLogFile("WorldLogFile", NULL, "ab");
    m_logFiles[LOG_TYPE_SQL]      = openLogFile("LogSQLFilename", "LogSQLTimestamp", "ab");

    m_logMask = sConfig.GetIntDefault("LogMask", 51);
    m_logMaskDatabase = sConfig.GetIntDefault("DBLogMask", 0);

    // -------------------------------------------------------------------
    // Deprecated, kept for backward compatibilty - @todo remove in future
    // -------------------------------------------------------------------

    // Main log file settings
    switch (sConfig.GetIntDefault("LogLevel", -1))
    {
        case LOGL_MINIMAL:
            m_logMask = LOG_TYPE_STRING | LOG_TYPE_COMMAND | LOG_TYPE_ERROR | LOG_TYPE_ERROR_DB;
            break;
        case LOGL_BASIC:
            m_logMask = LOG_TYPE_STRING | LOG_TYPE_COMMAND | LOG_TYPE_ERROR | LOG_TYPE_ERROR_DB;
            m_logMask |= LOG_TYPE_BASIC | LOG_TYPE_WARDEN;
            break;
        case LOGL_DETAIL:
            m_logMask = LOG_TYPE_STRING | LOG_TYPE_COMMAND | LOG_TYPE_ERROR | LOG_TYPE_ERROR_DB;
            m_logMask |= LOG_TYPE_BASIC | LOG_TYPE_WARDEN;
            m_logMask |= LOGL_BASIC | LOG_TYPE_DETAIL | LOG_TYPE_SQL | LOG_TYPE_REMOTE | LOG_TYPE_CHAR;
            break;
        case LOGL_DEBUG:
            m_logMask = LOG_TYPE_STRING | LOG_TYPE_COMMAND | LOG_TYPE_ERROR | LOG_TYPE_ERROR_DB;
            m_logMask |= LOG_TYPE_BASIC | LOG_TYPE_WARDEN;
            m_logMask |= LOGL_BASIC | LOG_TYPE_DETAIL | LOG_TYPE_SQL | LOG_TYPE_REMOTE | LOG_TYPE_CHAR;
            m_logMask |= LOGL_DETAIL | LOG_TYPE_DEBUG | LOG_TYPE_MAP | LOG_TYPE_VMAP | LOG_TYPE_MMAP | LOG_TYPE_WARDEN | LOG_TYPE_ARENA | LOG_TYPE_NETWORK;
            break;
        case -1:
            break;
    }

    switch (sConfig.GetIntDefault("DBLogLevel", -1))
    {
        case LOGL_MINIMAL:
            m_logMaskDatabase = LOG_TYPE_STRING | LOG_TYPE_COMMAND | LOG_TYPE_ERROR | LOG_TYPE_ERROR_DB;
            break;
        case LOGL_BASIC:
            m_logMaskDatabase = LOG_TYPE_STRING | LOG_TYPE_COMMAND | LOG_TYPE_ERROR | LOG_TYPE_ERROR_DB;
            m_logMaskDatabase |= LOG_TYPE_BASIC | LOG_TYPE_WARDEN;
            break;
        case LOGL_DETAIL:
            m_logMaskDatabase = LOG_TYPE_STRING | LOG_TYPE_COMMAND | LOG_TYPE_ERROR | LOG_TYPE_ERROR_DB;
            m_logMaskDatabase |= LOG_TYPE_BASIC | LOG_TYPE_WARDEN;
            m_logMaskDatabase |= LOGL_BASIC | LOG_TYPE_DETAIL | LOG_TYPE_SQL | LOG_TYPE_REMOTE | LOG_TYPE_CHAR;
            break;
        case LOGL_DEBUG:
            m_logMaskDatabase = LOG_TYPE_STRING | LOG_TYPE_COMMAND | LOG_TYPE_ERROR | LOG_TYPE_ERROR_DB;
            m_logMaskDatabase |= LOG_TYPE_BASIC | LOG_TYPE_WARDEN;
            m_logMaskDatabase |= LOGL_BASIC | LOG_TYPE_DETAIL | LOG_TYPE_SQL | LOG_TYPE_REMOTE | LOG_TYPE_CHAR;
            m_logMaskDatabase |= LOGL_DETAIL | LOG_TYPE_DEBUG | LOG_TYPE_MAP | LOG_TYPE_VMAP | LOG_TYPE_MMAP | LOG_TYPE_WARDEN | LOG_TYPE_ARENA | LOG_TYPE_NETWORK;
            break;
        case -1:
            break;
    }

    // Check whether we'll log GM commands/RA events/character outputs/chat stuffs
    m_logMaskDatabase |= static_cast<unsigned char>(sConfig.GetBoolDefault("LogDB.Char", false)) << LOG_TYPE_CHAR;
    m_logMaskDatabase |= static_cast<unsigned char>(sConfig.GetBoolDefault("LogDB.RA",   false)) << LOG_TYPE_REMOTE;
    m_logMaskDatabase |= static_cast<unsigned char>(sConfig.GetBoolDefault("LogDB.GM",   false)) << LOG_TYPE_COMMAND;
    m_logMaskDatabase |= static_cast<unsigned char>(sConfig.GetBoolDefault("LogDB.Chat", false)) << LOG_TYPE_CHAT;
}

FILE* Log::openLogFile(char const* configFileName, char const* configTimeStampFlag, char const* mode)
{
    std::string logfn = sConfig.GetStringDefault(configFileName, "");
    if (logfn.empty())
        return NULL;

    if (configTimeStampFlag && sConfig.GetBoolDefault(configTimeStampFlag, false))
    {
        size_t dot_pos = logfn.find_last_of(".");
        if (dot_pos != logfn.npos)
            logfn.insert(dot_pos, m_logsTimestamp);
        else
            logfn += m_logsTimestamp;
    }

    return fopen((m_logsDir + logfn).c_str(), mode);
}

/**
  * Opens appropriate log for GM account
  * @param account account id
  * @returns the open file
  */
FILE* Log::openGmlogPerAccount(uint64 account)
{
    if (m_gmlog_filename_format.empty())
        return NULL;

    char namebuf[OREGON_PATH_MAX];
    snprintf(namebuf, OREGON_PATH_MAX, m_gmlog_filename_format.c_str(), account);
    return fopen(namebuf, "ab");
}

void Log::outTimestamp(FILE* file)
{
    time_t t = time(NULL);
    tm* aTm = localtime(&t);
    //       YYYY   year
    //       MM     month (2 digits 01-12)
    //       DD     day (2 digits 01-31)
    //       HH     hour (2 digits 00-23)
    //       MM     minutes (2 digits 00-59)
    //       SS     seconds (2 digits 00-59)
    fprintf(file, "%-4d-%02d-%02d %02d:%02d:%02d ", aTm->tm_year + 1900, aTm->tm_mon + 1, aTm->tm_mday, aTm->tm_hour, aTm->tm_min, aTm->tm_sec);
}

void Log::InitColors(const std::string& str)
{
    std::stringstream ss(str);
    
    for (uint8 i = 0; i < MAX_LOG_TYPES; ++i)
    {
        int color;
        ss >> color;

        if (ss.eof() || ss.fail() || ss.bad())
            break;

        if (color < 0 || color >= MAX_COLORS)
            break;
        
        m_colors[i] = ColorTypes (color);
    }
    }

/// Sets color for upcoming output
void Log::SetColor(ColorTypes color)
{
    if (sConsole.IsEnabled())
    {
        fputc(0xFF, stderr);
		#if defined(__PDCURSES__) && !defined(PDC_RGB)
		// BGR mode, convert from RGB then
		int clr = int(color);
		int rgb = (clr & 0x07);
		clr &= ~0x07;
		clr |= (rgb & 0x1) << 2; // red
		clr |= (rgb & 0x2);      // green
		clr |= (rgb & 0x4) >> 2; // blue
		color = ColorTypes(clr);
		#endif
        fputc((char) color, stderr);
        return;
    }

    #if PLATFORM == PLATFORM_WINDOWS
    static WORD WinColorFG[MAX_COLORS] =
    {
        0,                                                  // BLACK
        FOREGROUND_RED,                                     // RED
        FOREGROUND_GREEN,                                   // GREEN
        FOREGROUND_RED | FOREGROUND_GREEN,                  // BROWN
        FOREGROUND_BLUE,                                    // BLUE
        FOREGROUND_RED |                    FOREGROUND_BLUE,// MAGENTA
        FOREGROUND_GREEN | FOREGROUND_BLUE,                 // CYAN
        FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,// WHITE
        FOREGROUND_INTENSITY,                                                            // BLACK, BOLD (GREY?)
        FOREGROUND_RED   | FOREGROUND_INTENSITY,                                         // RED, BOLD
        FOREGROUND_GREEN | FOREGROUND_INTENSITY,                                         // GREEN, BOLD
		FOREGROUND_RED   | FOREGROUND_GREEN     | FOREGROUND_INTENSITY,                  // YELLOW
        FOREGROUND_BLUE  | FOREGROUND_INTENSITY,                                         // BLUE, BOLD
        FOREGROUND_RED   | FOREGROUND_BLUE      | FOREGROUND_INTENSITY,                  // MAGENTA, BOLD
        FOREGROUND_GREEN | FOREGROUND_BLUE      | FOREGROUND_INTENSITY,                  // CYAN, BOLD
        FOREGROUND_RED   | FOREGROUND_GREEN     | FOREGROUND_BLUE | FOREGROUND_INTENSITY // WHITE, BOLD
    };

    SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), WinColorFG[color]);
    #else

    enum ANSIFgTextAttr
    {
        FG_BLACK = 30,
        FG_RED,
        FG_GREEN,
        FG_YELLOW,
        FG_BLUE,
        FG_MAGENTA,
        FG_CYAN,
        FG_WHITE
    };

    static uint8 UnixColorFG[MAX_COLORS] =
    {
        FG_BLACK,                                           // BLACK
        FG_RED,                                             // RED
        FG_GREEN,                                           // GREEN
        FG_YELLOW,                                          // BROWN
        FG_BLUE,                                            // BLUE
        FG_MAGENTA,                                         // MAGENTA
        FG_CYAN,                                            // CYAN
        FG_WHITE,                                           // LGREY
        FG_BLACK,                                           // GREY
        FG_RED,                                             // LRED
        FG_GREEN,                                           // LGREEN
        FG_YELLOW,                                          // YELLOW
        FG_BLUE,                                            // LBLUE
        FG_MAGENTA,                                         // LMAGENTA
        FG_CYAN,                                            // LCYAN
        FG_WHITE                                            // LWHITE
    };

    fprintf(stderr, "\x1b[%s%dm", ((color > 7) ? "01;" : "00;"), UnixColorFG[color]);
    #endif
}

/// Resets output color to normal
void Log::ResetColor()
{
    if (sConsole.IsEnabled())
    {
        fputc(0xFE, stderr);
        return;
    }

    #if PLATFORM == PLATFORM_WINDOWS
    SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
    #else
    fprintf(stderr, "\x1b[0m");
    #endif
}

std::string Log::GetTimestampStr()
{
    time_t t = time(NULL);
    tm* aTm = localtime(&t);
    //       YYYY   year
    //       MM     month (2 digits 01-12)
    //       DD     day (2 digits 01-31)
    char buf[20];
    snprintf(buf, 20, "%04d-%02d-%02d", aTm->tm_year + 1900, aTm->tm_mon + 1, aTm->tm_mday);
    return std::string(buf);
}

void Log::outDB(LogTypes type, const char* str)
{
    if (!LoginDatabase.IsConnected())
        return;

    std::string new_str(str);
    LoginDatabase.escape_string(new_str);
    LoginDatabase.PExecute("INSERT INTO logs (time, realm, type, string) "
                           "VALUES (" UI64FMTD ", %u, %u, '%s');", uint64(time(0)), realmID, type, new_str.c_str());
}

/**
  * Performs logging.
  * @param type one value of the \ref LogTypes
  * @param newline set if newline should be put after the message
  * @param prefix prefix, usually the LogTypes as string
  * @param fmt printf-like formatting message
  * @param ap list of arguments
  * @param file if set, use this file instead of m_logFiles[type]
  */
void Log::DoLog(LogTypes type, bool newline, const char* prefix, const char* fmt, va_list ap, FILE* file)
{
    va_list ap2;
    va_copy(ap2, ap);

    size_t len = vsnprintf(NULL, 0, fmt, ap2) + 1;
    char* buffer = (char*) ((len > 1024) ? malloc(len * sizeof(char)) : alloca(len * sizeof(char)));
    
	va_end(ap2);
	vsprintf(buffer, fmt, ap);

    if (m_logMaskDatabase & (1 << type))
    {
        // we don't want empty strings in the DB
        if (*buffer && *buffer != ' ' && *buffer != '\n')
            outDB(type, buffer);
    }

    if (m_logMask & (1 << type))
    {
        if (FILE* logFile = (file ? file : m_logFiles[type]))
        {
            outTimestamp(logFile);
            fwrite(buffer, len-1, 1, logFile);
            if (newline)
                fputc('\n', logFile);
        }

        if (prefix)
        {
            if (colorPrefixTable[m_colors[type]])
                SetColor(colorPrefixTable[m_colors[type]]);

            fprintf(stderr, "[%s] ", prefix);
        }

        if (m_colors[type])
            SetColor(m_colors[type]);

        #if PLATFORM == PLATFORM_WINDOWS
        wchar_t* wtemp_buf = (wchar_t*) _malloca(len * sizeof(wchar_t));
        size_t siz = len - 1;
        if (Utf8toWStr(buffer, len-1, wtemp_buf, siz))
        {
            CharToOemBuffW(wtemp_buf, buffer, siz);
            fwrite(buffer, siz, 1, stderr);
        }
        _freea(wtemp_buf);
        #else
        fwrite(buffer, len-1, 1, stderr);
        #endif

        if (m_colors[type])
            ResetColor();

        if (newline)
            fputc('\n', stderr);
        
        // just to be sure, stderr should be unbuffered anyway
        fflush(stderr);
    }

    if (len > 1024)
        free(buffer);
}

void Log::outFatal(const char* err, ...)
{
    va_list ap;
    va_start(ap, err);
    size_t len = vsnprintf(NULL, 0, err, ap) + 1;
    char* buffer = (char*) alloca(len);
    buffer[len - 1] = '\0';
    va_end(ap);
    va_start(ap, err);
    vsprintf(buffer, err, ap);
    va_end(ap);

    m_logMask |= LOG_TYPE_ERROR;
    outError("%s", buffer);

    if (sConsole.IsEnabled())
        sConsole.FatalError(buffer);

    exit (EXIT_FAILURE);
}

void Log::outCommand(uint64 account, const char* str, ...)
{
    if (!((m_logMask | m_logMaskDatabase) & (1 << LOG_TYPE_COMMAND)))
        return;

    FILE* file;
    if (m_gmlog_per_account)
        file = openGmlogPerAccount(account);
    else
        file = m_logFiles[LOG_TYPE_COMMAND];

    va_list ap;
    va_start(ap, str);
    DoLog(LOG_TYPE_COMMAND, true, "CMD", str, ap, file);
    va_end(ap);

    if (m_gmlog_per_account)
    {
        fflush(file);
        fclose(file);
    }
}

void Log::outCharDump(const char* str, uint32 account_id, uint32 guid, const char* name)
{
    if (m_logFiles[LOG_TYPE_CHAR])
    {
        fprintf(m_logFiles[LOG_TYPE_CHAR], "== START DUMP == (account: %u guid: %u name: %s)\n%s\n== END DUMP ==\n", account_id, guid, name, str);
        fflush(m_logFiles[LOG_TYPE_CHAR]);
    }
}

#define logFunctionImpl(name, type, newline, prefix)                  \
            void Log::name(const char* fmt, ...)                      \
            {                                                         \
                if (!((m_logMask | m_logMaskDatabase) & (1 << type))) \
                    return;                                    \
                                                               \
                va_list ap;                                    \
                va_start(ap, fmt);                             \
                DoLog(type, newline, prefix, fmt, ap, NULL);   \
                va_end(ap);                                    \
            }                                                          

logFunctionImpl(outString, LOG_TYPE_STRING,  true, NULL)
logFunctionImpl(outBasic,  LOG_TYPE_BASIC,   true, NULL)
logFunctionImpl(outDetail, LOG_TYPE_DETAIL,  true, "Detail")
logFunctionImpl(outDebug,  LOG_TYPE_DEBUG,   true, "Dbg")

logFunctionImpl(outError,  LOG_TYPE_ERROR,    true, "Err")
logFunctionImpl(outErrorDb,LOG_TYPE_ERROR_DB, true, "ErrDB")
logFunctionImpl(outSQL,    LOG_TYPE_SQL,      true, "SQL")

logFunctionImpl(outArena,  LOG_TYPE_ARENA,   true, "Arena")
logFunctionImpl(outWarden, LOG_TYPE_WARDEN,  true, "Warden")
logFunctionImpl(outChat,   LOG_TYPE_CHAT,    true,  "Chat")
logFunctionImpl(outCommand,LOG_TYPE_COMMAND, true, "CMD")

logFunctionImpl(outChar,   LOG_TYPE_CHAR,   true, "Char")
logFunctionImpl(outRemote, LOG_TYPE_REMOTE, true, "Remote")

logFunctionImpl(outMap, LOG_TYPE_MAP,  true, "Map")
logFunctionImpl(outVMap,LOG_TYPE_VMAP, true, "VMap")
logFunctionImpl(outMMap,LOG_TYPE_MMAP, true, "MMap")

logFunctionImpl(outNetwork, LOG_TYPE_NETWORK, true, "NET")

logFunctionImpl(outDebugInLine, LOG_TYPE_DEBUG, false, "Dbg")

#undef logFunctionImpl
