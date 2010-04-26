/*
 * Copyright (C) 2005-2008 MaNGOS <http://www.mangosproject.org/>
 *
 * Copyright (C) 2008 Trinitycore <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/// \addtogroup Oregond Oregon Daemon
/// @{
/// \file
#include "SystemConfig.h"
#include "revision.h"

#include "Common.h"
#include "Database/DatabaseEnv.h"
#include "Config/ConfigEnv.h"
#include "Log.h"
#include "Master.h"

#ifndef _OREGON_CORE_CONFIG
# define _OREGON_CORE_CONFIG  "oregoncore.conf"
#endif //_OREGON_CORE_CONFIG

// Format is YYYYMMDDRR where RR is the change in the conf file
// for that day.
#ifndef _OREGON_CORE_CONFVER
# define _OREGON_CORE_CONFVER 2010042601
#endif //_OREGON_CORE_CONFVER

#ifdef WIN32
#include "ServiceWin32.h"
char serviceName[] = "Oregond";
char serviceLongName[] = "Oregon core service";
char serviceDescription[] = "Massive Network Game Object Server";
/*
 * -1 - not in service mode
 *  0 - stopped
 *  1 - running
 *  2 - paused
 */
int m_ServiceStatus = -1;
#endif

DatabaseType WorldDatabase;                                 ///< Accessor to the world database
DatabaseType CharacterDatabase;                             ///< Accessor to the character database
DatabaseType loginDatabase;                                 ///< Accessor to the realm/login database

uint32 realmID;                                             ///< Id of the realm

/// Print out the usage string for this program on the console.
void usage(const char *prog)
{
    sLog.outString("Usage: \n %s [<options>]\n"
        "    --version                print version and exit\n\r"
        "    -c config_file           use config_file as configuration file\n\r"
        #ifdef WIN32
        "    Running as service functions:\n\r"
        "    --service                run as service\n\r"
        "    -s install               install service\n\r"
        "    -s uninstall             uninstall service\n\r"
        #endif
        ,prog);
}

/// Launch the OREGON server
extern int main(int argc, char **argv)
{
    ///- Command line parsing to get the configuration file name
    char const* cfg_file = _OREGON_CORE_CONFIG;
    int c=1;
    while( c < argc )
    {
        if( strcmp(argv[c],"-c") == 0)
        {
            if( ++c >= argc )
            {
                sLog.outError("Runtime-Error: -c option requires an input argument");
                usage(argv[0]);
                return 1;
            }
            else
                cfg_file = argv[c];
        }

        if( strcmp(argv[c],"--version") == 0)
        {
            printf("%s\n", _FULLVERSION);
            return 0;
        }

        #ifdef WIN32
        ////////////
        //Services//
        ////////////
        if( strcmp(argv[c],"-s") == 0)
        {
            if( ++c >= argc )
            {
                sLog.outError("Runtime-Error: -s option requires an input argument");
                usage(argv[0]);
                return 1;
            }
            if( strcmp(argv[c],"install") == 0)
            {
                if (WinServiceInstall())
                    sLog.outString("Installing service");
                return 1;
            }
            else if( strcmp(argv[c],"uninstall") == 0)
            {
                if(WinServiceUninstall())
                    sLog.outString("Uninstalling service");
                return 1;
            }
            else
            {
                sLog.outError("Runtime-Error: unsupported option %s",argv[c]);
                usage(argv[0]);
                return 1;
            }
        }
        if( strcmp(argv[c],"--service") == 0)
        {
            WinServiceRun();
        }
        ////
        #endif
        ++c;
    }

    if (!sConfig.SetSource(cfg_file))
    {
        sLog.outError("Could not find configuration file %s.", cfg_file);
        return 1;
    }
    sLog.outString("Using configuration file %s.", cfg_file);

    uint32 confVersion = sConfig.GetIntDefault("ConfVersion", 0);
    if (confVersion < _OREGON_CORE_CONFVER)
    {
        sLog.outError("*****************************************************************************");
        sLog.outError(" WARNING: Your oregoncore.conf version indicates your conf file is out of date!");
        sLog.outError("          Please check for updates, as your current default values may cause");
        sLog.outError("          strange behavior.");
        sLog.outError("*****************************************************************************");
        clock_t pause = 3000 + clock();

        while (pause > clock()) {}
    }

    ///- and run the 'Master'
    /// \todo Why do we need this 'Master'? Can't all of this be in the Main as for Realmd?
    return sMaster.Run();

    // at sMaster return function exist with codes
    // 0 - normal shutdown
    // 1 - shutdown at error
    // 2 - restart command used, this code can be used by restarter for restart Oregond
}

/// @}

