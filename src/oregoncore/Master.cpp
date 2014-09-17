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

#include <ace/OS_NS_signal.h>

#include "WorldSocketMgr.h"
#include "Common.h"
#include "Master.h"
#include "WorldSocket.h"
#include "WorldRunnable.h"
#include "World.h"
#include "Log.h"
#include "Timer.h"
#include "Policies/SingletonImp.h"
#include "SystemConfig.h"
#include "Config/Config.h"
#include "Database/DatabaseEnv.h"
#include "DBCStores.h"
#include "RARunnable.h"
#include "Util.h"
#include "OCSoap.h"
#include "Console.h"

#ifdef _WIN32
#include "ServiceWin32.h"
extern int m_ServiceStatus;
#endif

#if PLATFORM == PLATFORM_UNIX
#include "Debugging/UnixDebugger.h"
#endif

INSTANTIATE_SINGLETON_1(Master);

volatile uint32 Master::m_masterLoopCounter = 0;

class FreezeDetectorRunnable : public ACE_Based::Runnable
{
public:
    FreezeDetectorRunnable() { _delaytime = 0; }
    uint32 m_loops, m_lastchange;
    uint32 w_loops, w_lastchange;
    uint32 _delaytime;
    void SetDelayTime(uint32 t) { _delaytime = t; }
    void run(void)
    {
        if (!_delaytime)
            return;
        sLog.outString("Starting up anti-freeze thread (%u seconds max stuck time)...",_delaytime/1000);
        m_loops = 0;
        w_loops = 0;
        m_lastchange = 0;
        w_lastchange = 0;
        for (;;)
        {
            ACE_Based::Thread::Sleep(1000);
            /* Windows' Sleep() returns void so
               we cant be sure if Sleep() was interrupted
               by shutdown. So check if world is running here.
               Else this will detect false positive. */
            if (World::IsStopped())
                break;
            uint32 curtime = getMSTime();
            // normal work
            if (w_loops != World::m_worldLoopCounter)
            {
                w_lastchange = curtime;
                w_loops = World::m_worldLoopCounter;
            }
            // possible freeze
            else if (getMSTimeDiff(w_lastchange,curtime) > _delaytime)
            {
                sLog.outError("World Thread is stuck.  Terminating server!");
                *((uint32 volatile*)NULL) = 0;                       // bang crash
            }
        }
        sLog.outString("Anti-freeze thread exiting without problems.");
    }
};

Master::Master()
{
}

Master::~Master()
{
}

// Main function
int Master::Run()
{
    #if PLATFORM == PLATFORM_UNIX
    UnixDebugger::RegisterDeadlySignalHandler();
    #endif

    if (sConfig.GetBoolDefault("Console.Enable", true))
        sConsole.Initialize();
    sConsole.SetLoading(true);
    sConsole.DrawLogo();

    // worldd PID file creation
    std::string pidfile = sConfig.GetStringDefault("PidFile", "");
    if (!pidfile.empty())
    {
        uint32 pid = CreatePIDFile(pidfile);
        if (!pid)
        {
            sLog.outError("Cannot create PID file %s.\n", pidfile.c_str());
            return 1;
        }

        sLog.outString("Daemon PID: %u\n", pid);
    }

    // Start the databases
    _StartDB();

    // Initialize the World
    sWorld.SetInitialWorldSettings();

    // set realmbuilds depend on OregonCore expected builds, and set server online
    std::string builds = AcceptableClientBuildsListStr();
    LoginDatabase.escape_string(builds);
    LoginDatabase.PExecute("UPDATE realmlist SET realmflags = realmflags & ~(%u), population = 0, realmbuilds = '%s'  WHERE id = '%d'", REALM_FLAG_OFFLINE, builds.c_str(), realmID);

    sConsole.SetLoading(false);
    
    // Catch termination signals
    _HookSignals();

    ACE_Based::Thread* cliThread = NULL;

#ifdef _WIN32
    if (sConfig.GetBoolDefault("Console.Enable", true) && (m_ServiceStatus == -1)/* need disable console in service mode*/)
#else
    if (sConfig.GetBoolDefault("Console.Enable", true))
#endif
    {
        // Launch CliRunnable thread
        cliThread = new ACE_Based::Thread(new Console::CliRunnable);
    }

    ACE_Based::Thread rar_thread(new RARunnable);

    // Handle affinity for multiple processors and process priority on Windows
    #ifdef _WIN32
    {
        HANDLE hProcess = GetCurrentProcess();

        uint32 Aff = sConfig.GetIntDefault("UseProcessors", 0);
        if (Aff > 0)
        {
            ULONG_PTR appAff;
            ULONG_PTR sysAff;

            if (GetProcessAffinityMask(hProcess,&appAff,&sysAff))
            {
                ULONG_PTR curAff = Aff & appAff;            // remove non accessible processors

                if (!curAff)
                {
                    sLog.outError("Processors marked in UseProcessors bitmask (hex) %x not accessible for OregonCore. Accessible processors bitmask (hex): %x",Aff,appAff);
                }
                else
                {
                    if (SetProcessAffinityMask(hProcess,curAff))
                        sLog.outString("Using processors (bitmask, hex): %x", curAff);
                    else
                        sLog.outError("Can't set used processors (hex): %x",curAff);
                }
            }
            sLog.outString();
        }

        bool Prio = sConfig.GetBoolDefault("ProcessPriority", false);

        if (Prio)
        {
            if (SetPriorityClass(hProcess,HIGH_PRIORITY_CLASS))
                sLog.outString("OregonCore process priority class set to HIGH");
            else
                sLog.outError("ERROR: Can't set OregonCore process priority class.");
            sLog.outString();
        }
    }
    #endif

    // Start soap serving thread
    ACE_Based::Thread* soap_thread = NULL;

    if (sConfig.GetBoolDefault("SOAP.Enabled", false))
    {
        OCSoapRunnable *runnable = new OCSoapRunnable();

        runnable->setListenArguments(sConfig.GetStringDefault("SOAP.IP", "127.0.0.1"), sConfig.GetIntDefault("SOAP.Port", 7878));
        soap_thread = new ACE_Based::Thread(runnable);
    }

    //uint32 socketSelecttime = sWorld.getConfig(CONFIG_SOCKET_SELECTTIME);

    // Start up freeze catcher thread
    ACE_Based::Thread* freeze_thread = NULL;
    if (uint32 freeze_delay = sConfig.GetIntDefault("MaxCoreStuckTime", 0))
    {
        FreezeDetectorRunnable *fdr = new FreezeDetectorRunnable();
        fdr->SetDelayTime(freeze_delay*1000);
        freeze_thread = new ACE_Based::Thread(fdr);
        freeze_thread->setPriority(ACE_Based::Highest);
    }

    // Launch the world listener socket
    uint16 wsport = sWorld.getConfig(CONFIG_PORT_WORLD);
    std::string bind_ip = sConfig.GetStringDefault ("BindIP", "0.0.0.0");

    if (sWorldSocketMgr->StartNetwork (wsport, bind_ip.c_str ()) == -1)
    {
        sLog.outError("Failed to start network");
        World::StopNow(ERROR_EXIT_CODE);
        // go down and shutdown the server
        // give other threads a chance to start-up so we can shutdown them safely
        ACE_Based::Thread::Sleep(1500);
    }

    /* Run our World, we use main thread for this,
       because it we need the highest priority possible */
    WorldRunnable().run();

    sWorldSocketMgr->Wait();

    // Stop freeze protection before shutdown tasks
    if (freeze_thread)
    {
        freeze_thread->interrupt();
        freeze_thread->wait();
        delete freeze_thread;
    }

    // Stop soap thread
    if (soap_thread)
    {
        soap_thread->wait();
        delete soap_thread;
    }

    // Set server offline in realmlist
    LoginDatabase.PExecute("UPDATE realmlist SET realmflags = realmflags | %u WHERE id = '%d'", REALM_FLAG_OFFLINE, realmID);

    // when the main thread closes the singletons get unloaded
    // since worldrunnable uses them, it will crash if unloaded after master
    rar_thread.wait ();

    // Clean account database before leaving
    clearOnlineAccounts();

    // Wait for delay threads to end
    CharacterDatabase.HaltDelayThread();
    WorldDatabase.HaltDelayThread();
    LoginDatabase.HaltDelayThread();

    sLog.outString("Halting process...");

    if (cliThread)
    {
        cliThread->interrupt();
        cliThread->wait();
        delete cliThread;
    }

    // Remove signal handling before leaving
    _UnhookSignals();

    // for some unknown reason, unloading scripts here and not in worldrunnable
    // fixes a memory leak related to detaching threads from the module
    //UnloadScriptingModule();

    // Exit the process with specified return value
    return World::GetExitCode();
}

// Initialize connection to the databases
void Master::_StartDB()
{
    sConsole.SetLoadingLabel("Connecting to databases...");
    sLog.SetLogDB(false);

    // Get world database info from configuration file
    std::string dbstring = sConfig.GetStringDefault("WorldDatabaseInfo", "");
    if (dbstring.empty())
        sLog.outFatal("World database not specified in configuration file");

    // Initialise the world database
    if (!WorldDatabase.Initialize(dbstring.c_str()))
        sLog.outFatal("Cannot connect to world database %s",dbstring.c_str());

    // Get character database info from configuration file
    dbstring = sConfig.GetStringDefault("CharacterDatabaseInfo", "");
    if (dbstring.empty())
        sLog.outFatal("Character database not specified in configuration file");

    // Initialise the Character database
    if (!CharacterDatabase.Initialize(dbstring.c_str()))
        sLog.outFatal("Cannot connect to Character database %s",dbstring.c_str());

    // Get login database info from configuration file
    dbstring = sConfig.GetStringDefault("LoginDatabaseInfo", "");
    if (dbstring.empty())
        sLog.outFatal("Login database not specified in configuration file");

    // Initialise the login database
    if (!LoginDatabase.Initialize(dbstring.c_str()))
        sLog.outFatal("Cannot connect to login database %s",dbstring.c_str());

    // Get the realm Id from the configuration file
    realmID = sConfig.GetIntDefault("RealmID", 0);
    if (!realmID)
        sLog.outFatal("Realm ID not defined in configuration file");

    sLog.outString("Realm running as realm ID %d", realmID);

    // Initialize the DB logging system
    sLog.SetLogDBLater(sConfig.GetBoolDefault("EnableLogDB", false)); // set var to enable DB logging once startup finished.
    sLog.SetLogDB(false);
    sLog.SetRealmID(realmID);

    // Clean the database before starting
    clearOnlineAccounts();

    // Insert version info into DB
    WorldDatabase.PExecute("UPDATE version SET core_version = '%s', core_revision = '%s'", _FULLVERSION, _REVISION);

    sWorld.LoadDBVersion();

    sLog.outString("Using World DB: %s", sWorld.GetDBVersion());
}

// Clear 'online' status for all accounts with characters in this realm
void Master::clearOnlineAccounts()
{
    // Cleanup online status for characters hosted at current realm
    // todo - Only accounts with characters logged on *this* realm should have online status reset. Move the online column from 'account' to 'realmcharacters'?
    LoginDatabase.PExecute("UPDATE account SET active_realm_id = 0 WHERE active_realm_id = '%d'", realmID);

    CharacterDatabase.Execute("UPDATE characters SET online = 0 WHERE online<>0");
}

// Handle termination signals
void Master::_OnSignal(int s)
{
    switch (s)
    {
        case SIGINT:
            World::StopNow(SHUTDOWN_EXIT_CODE);
            break;
        case SIGTERM:
        #ifdef _WIN32
        case SIGBREAK:
        #endif
            World::StopNow(SHUTDOWN_EXIT_CODE);
            break;
    }

    signal(s, _OnSignal);
}

// Define hook '_OnSignal' for all termination signals
void Master::_HookSignals()
{
    signal(SIGINT, _OnSignal);
    signal(SIGTERM, _OnSignal);
    #ifdef _WIN32
    signal(SIGBREAK, _OnSignal);
    #endif
}

// Unhook the signals before leaving
void Master::_UnhookSignals()
{
    signal(SIGINT, 0);
    signal(SIGTERM, 0);
    #ifdef _WIN32
    signal(SIGBREAK, 0);
    #endif
}

