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
 * with this program. If not, see <https://www.gnu.org/licenses/>.
 */

/**
* @file main.cpp
* @brief Realm Server main program
*
* This file contains the main program for the
* realm server
*/

#include "Common.h"
#include "Database/DatabaseEnv.h"
#include "RealmList.h"

#include "Config/Config.h"
#include "Log.h"
#include "AuthSocket.h"
#include "SystemConfig.h"
#include "Util.h"

#include <ace/Get_Opt.h>
#include <ace/Dev_Poll_Reactor.h>
#include <ace/TP_Reactor.h>
#include <ace/ACE.h>
#include <ace/Acceptor.h>
#include <ace/SOCK_Acceptor.h>

// Format is YYYYMMDDRR where RR is the change in the conf file
// for that day.
#ifndef _REALMDCONFVERSION
# define _REALMDCONFVERSION 2010101001
#endif

#ifndef _OREGON_REALM_CONFIG
# define _OREGON_REALM_CONFIG  "oregonrealm.conf"
#endif

#ifdef _WIN32
#include "ServiceWin32.h"
char serviceName[] = "realmd";
char serviceLongName[] = "Oregon realm service";
char serviceDescription[] = "Massive Network Game Object Server";
/*
 * -1 - not in service mode
 * 0 - stopped
 * 1 - running
 * 2 - paused
 */
int m_ServiceStatus = -1;
#endif

bool StartDB();
void UnhookSignals();
void HookSignals();

bool stopEvent = false;                                     // Setting it to true stops the server

DatabaseType LoginDatabase;                                 // Accessor to the realm server database

uint32 realmID = 0;

// Print out the usage string for this program on the console.
void usage(const char* prog)
{
    sLog.outString("Usage: \n %s [<options>]\n"
                   "    -v, --version            print version and exit\n\r"
                   "    -c config_file           use config_file as configuration file\n\r"
                   #ifdef _WIN32
                   "    Running as service functions:\n\r"
                   "    -s run                   run as service\n\r"
                   "    -s install               install service\n\r"
                   "    -s uninstall             uninstall service\n\r"
                   #endif
                   , prog);
}

/// Launch the realm server
extern int main(int argc, char** argv)
{
    // Command line parsing
    char const* cfg_file = _OREGON_REALM_CONFIG;

    #ifdef _WIN32
    char const* options = ":c:s:";
    #else
    char const* options = ":c:";
    #endif

    ACE_Get_Opt cmd_opts(argc, argv, options);
    cmd_opts.long_option("version", 'v');

    int option;
    while ((option = cmd_opts()) != EOF)
    {
        switch (option)
        {
        case 'c':
            cfg_file = cmd_opts.opt_arg();
            break;
        case 'v':
            printf("%s\n", _FULLVERSION);
            return 0;
            #ifdef _WIN32
        case 's':
            {
                const char* mode = cmd_opts.opt_arg();

                if (!strcmp(mode, "install"))
                {
                    if (WinServiceInstall())
                        sLog.outString("Installing service");
                    return 1;
                }
                else if (!strcmp(mode, "uninstall"))
                {
                    if (WinServiceUninstall())
                        sLog.outString("Uninstalling service");
                    return 1;
                }
                else if (!strcmp(mode, "run"))
                    WinServiceRun();
                else
                {
                    sLog.outError("Runtime-Error: -%c unsupported argument %s", cmd_opts.opt_opt(), mode);
                    usage(argv[0]);
                    return 1;
                }
                break;
            }
            #endif
        case ':':
            sLog.outError("Runtime-Error: -%c option requires an input argument", cmd_opts.opt_opt());
            usage(argv[0]);
            return 1;
        default:
            sLog.outError("Runtime-Error: bad format of commandline arguments");
            usage(argv[0]);
            return 1;
        }
    }

    if (!sConfig.SetSource(cfg_file))
    {
        sLog.outError("Invalid or missing configuration file : %s", cfg_file);
        sLog.outError("Verify that the file exists and has \'[authserver]\' written in the top of the file!");
        return 1;
    }
    sLog.Initialize();

    sLog.outString( "%s [realm-daemon]", _FULLVERSION);
    sLog.outString( "<Ctrl-C> to stop.\n" );
    sLog.outString("Using configuration file %s.", cfg_file);

    // Check the version of the configuration file
    uint32 confVersion = sConfig.GetIntDefault("ConfVersion", 0);
    if (confVersion < _REALMDCONFVERSION)
    {
        sLog.outError("*****************************************************************************");
        sLog.outError(" WARNING: Your oregonrealm.conf version indicates your conf file is out of date!");
        sLog.outError("          Please check for updates, as your current default values may cause");
        sLog.outError("          strange behavior.");
        sLog.outError("*****************************************************************************");
        clock_t pause = 3000 + clock();

        while (pause > clock()) {}
    }

    sLog.outDetail("Using ACE: %s", ACE_VERSION);

    #if defined (ACE_HAS_EVENT_POLL) || defined (ACE_HAS_DEV_POLL)
    ACE_Reactor::instance(new ACE_Reactor(new ACE_Dev_Poll_Reactor(ACE::max_handles(), 1), 1), true);
    #else
    ACE_Reactor::instance(new ACE_Reactor(new ACE_TP_Reactor(), true), true);
    #endif

    sLog.outBasic("Max allowed open files is %d", ACE::max_handles());

    // realmd PID file creation
    std::string pidfile = sConfig.GetStringDefault("PidFile", "");
    if (!pidfile.empty())
    {
        uint32 pid = CreatePIDFile(pidfile);
        if (!pid)
        {
            sLog.outError( "Cannot create PID file %s.\n", pidfile.c_str() );
            return 1;
        }

        sLog.outString( "Daemon PID: %u\n", pid );
    }

    // Initialize the database connection
    if (!StartDB())
        return 1;

    // Get the list of realms for the server
    sRealmList->Initialize(sConfig.GetIntDefault("RealmsStateUpdateDelay", 20));
    if (sRealmList->size() == 0)
    {
        sLog.outError("No valid realms specified.");
        return 1;
    }

    // cleanup query
    // set expired bans to inactive
    LoginDatabase.Execute("UPDATE account_banned SET active = 0 WHERE unbandate<=UNIX_TIMESTAMP() AND unbandate<>bandate");
    LoginDatabase.Execute("DELETE FROM ip_banned WHERE unbandate<=UNIX_TIMESTAMP() AND unbandate<>bandate");

    // Launch the listening network socket
    ACE_Acceptor<AuthSocket, ACE_SOCK_Acceptor> acceptor;

    uint16 rmport = sConfig.GetIntDefault("RealmServerPort", DEFAULT_REALMSERVER_PORT);
    std::string bind_ip = sConfig.GetStringDefault("BindIP", "0.0.0.0");

    ACE_INET_Addr bind_addr(rmport, bind_ip.c_str());

    if (acceptor.open(bind_addr, ACE_Reactor::instance(), ACE_NONBLOCK) == -1)
    {
        sLog.outError("realmd can not bind to %s:%d", bind_ip.c_str(), rmport);
        return 1;
    }

    // Catch termination signals
    HookSignals();

    // Handle affinity for multiple processors and process priority on Windows
    #ifdef _WIN32
    {
        HANDLE hProcess = GetCurrentProcess();

        uint32 Aff = sConfig.GetIntDefault("UseProcessors", 0);
        if (Aff > 0)
        {
            ULONG_PTR appAff;
            ULONG_PTR sysAff;

            if (GetProcessAffinityMask(hProcess, &appAff, &sysAff))
            {
                ULONG_PTR curAff = Aff & appAff;            // remove non accessible processors

                if (!curAff )
                    sLog.outError("Processors marked in UseProcessors bitmask (hex) %x not accessible for realmd. Accessible processors bitmask (hex): %x", Aff, appAff);
                else
                {
                    if (SetProcessAffinityMask(hProcess, curAff))
                        sLog.outString("Using processors (bitmask, hex): %x", curAff);
                    else
                        sLog.outError("Can't set used processors (hex): %x", curAff);
                }
            }
            sLog.outString();
        }

        bool Prio = sConfig.GetBoolDefault("ProcessPriority", false);

        if (Prio)
        {
            if (SetPriorityClass(hProcess, HIGH_PRIORITY_CLASS))
                sLog.outString("realmd process priority class set to HIGH");
            else
                sLog.outError("ERROR: Can't set realmd process priority class.");
            sLog.outString();
        }
    }
    #endif

    // maximum counter for next ping
    uint32 numLoops = (sConfig.GetIntDefault( "MaxPingTime", 30 ) * (MINUTE * 1000000 / 100000));
    uint32 loopCounter = 0;

    // Wait for termination signal
    while (!stopEvent)
    {
        // dont move this outside the loop, the reactor will modify it
        ACE_Time_Value interval(0, 100000);

        if (ACE_Reactor::instance()->run_reactor_event_loop(interval) == -1)
            break;

        if ( (++loopCounter) == numLoops )
        {
            loopCounter = 0;
            sLog.outDetail("Ping MySQL to keep connection alive");
            LoginDatabase.Query("SELECT 1 FROM realmlist LIMIT 1");
        }
        #ifdef _WIN32
        if (m_ServiceStatus == 0) stopEvent = true;
        while (m_ServiceStatus == 2) Sleep(1000);
        #endif
    }

    // Wait for the delay thread to exit
    LoginDatabase.HaltDelayThread();

    // Remove signal handling before leaving
    UnhookSignals();

    sLog.outString( "Halting process..." );
    return 0;
}

/// Handle realm servers termination signals
/** Put the global variable stopEvent to 'true' if a termination signal is caught **/
void OnSignal(int s)
{
    switch (s)
    {
    case SIGINT:
    case SIGTERM:
        stopEvent = true;
        break;
        #ifdef _WIN32
    case SIGBREAK:
        stopEvent = true;
        break;
        #endif
    }

    signal(s, OnSignal);
}

/// Initialize connection to the database
bool StartDB()
{
    std::string dbstring = sConfig.GetStringDefault("LoginDatabaseInfo", "");
    if (dbstring.empty())
    {
        sLog.outError("Database not specified");
        return false;
    }

    sLog.outString("Database: %s", dbstring.c_str() );
    if (!LoginDatabase.Initialize(dbstring.c_str()))
    {
        sLog.outError("Cannot connect to database");
        return false;
    }

    return true;
}

/// Define hook 'OnSignal' for all termination signals
void HookSignals()
{
    signal(SIGINT, OnSignal);
    signal(SIGTERM, OnSignal);
    #ifdef _WIN32
    signal(SIGBREAK, OnSignal);
    #endif
}

/// Unhook the signals before leaving
void UnhookSignals()
{
    signal(SIGINT, 0);
    signal(SIGTERM, 0);
    #ifdef _WIN32
    signal(SIGBREAK, 0);
    #endif
}

