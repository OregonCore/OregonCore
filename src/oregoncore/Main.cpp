/*
 * Copyright (C) 2010-2012 OregonCore <http://www.oregoncore.com/>
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2012 MaNGOS <http://getmangos.com/>
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

#include "SystemConfig.h"

#include "Common.h"
#include "Database/DatabaseEnv.h"
#include "Config/Config.h"

#include "Log.h"
#include "Master.h"
#include <ace/Version.h>
#include <ace/Get_Opt.h>

#ifndef _OREGON_CORE_CONFIG
# define _OREGON_CORE_CONFIG  "oregoncore.conf"
#endif //_OREGON_CORE_CONFIG

// Format is YYYYMMDDRR where RR is the change in the conf file
// for that day.
#ifndef _OREGON_CORE_CONFVER
# define _OREGON_CORE_CONFVER 2012012301
#endif //_OREGON_CORE_CONFVER

#ifdef _WIN32
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
DatabaseType LoginDatabase;                                 ///< Accessor to the realm/login database

uint32 realmID;                                             ///< Id of the realm

// Print out the usage string for this program on the console.
void usage(const char *prog)
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
        ,prog);
}

// Launch the oregon server
extern int main(int argc, char **argv)
{
    // Command line parsing
    char const* cfg_file = _OREGON_CORE_CONFIG;

#ifdef _WIN32
    char const *options = ":c:s:";
#else
    char const *options = ":c:";
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
                const char *mode = cmd_opts.opt_arg();

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
        sLog.outError("Verify that the file exists and has \'[worldserver]' written in the top of the file!");
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

    sLog.outDetail("Using ACE: %s", ACE_VERSION);

    // and run the 'Master'
    // todo - Why do we need this 'Master'? Can't all of this be in the Main as for Realmd?
    return sMaster.Run();

    // at sMaster return function exist with codes
    // 0 - normal shutdown
    // 1 - shutdown at error
    // 2 - restart command used, this code can be used by restarter for restart OregonCore
}

