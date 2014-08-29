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

#include "WorldSocketMgr.h"
#include "Common.h"
#include "World.h"
#include "WorldRunnable.h"
#include "Timer.h"
#include "ObjectAccessor.h"
#include "MapManager.h"
#include "BattleGroundMgr.h"
#include "CreatureFormations.h"

#include "Database/DatabaseEnv.h"

#define WORLD_SLEEP_CONST 50

// Heartbeat for the World
void WorldRunnable::run()
{
    ///- Init MySQL threads or connections
    bool needInit = true;
    if (!(WorldDatabase.GetBundleMask() & MYSQL_BUNDLE_WORLD))
    {
        WorldDatabase.Init_MySQL_Connection();
        needInit = false;
    }
    if (!(LoginDatabase.GetBundleMask() & MYSQL_BUNDLE_WORLD))
    {
        LoginDatabase.Init_MySQL_Connection();
        needInit = false;
    }

    if (!(CharacterDatabase.GetBundleMask() & MYSQL_BUNDLE_WORLD))
    {
        CharacterDatabase.Init_MySQL_Connection();
        needInit = false;
    }

    if (needInit)
        MySQL::Thread_Init();

    uint32 realCurrTime = 0;
    uint32 realPrevTime = getMSTime();

    uint32 prevSleepTime = 0;                               // used for balanced full tick time length near WORLD_SLEEP_CONST

    // While we have not World::m_stopEvent, update the world
    while (!World::IsStopped())
    {
        ++World::m_worldLoopCounter;
        realCurrTime = getMSTime();

        uint32 diff = getMSTimeDiff(realPrevTime,realCurrTime);

        sWorld.Update(diff);
        realPrevTime = realCurrTime;

        // diff (D0) include time of previous sleep (d0) + tick time (t0)
        // we want that next d1 + t1 == WORLD_SLEEP_CONST
        // we can't know next t1 and then can use (t0 + d1) == WORLD_SLEEP_CONST requirement
        // d1 = WORLD_SLEEP_CONST - t0 = WORLD_SLEEP_CONST - (D0 - d0) = WORLD_SLEEP_CONST + d0 - D0
        if (diff <= WORLD_SLEEP_CONST+prevSleepTime)
        {
            prevSleepTime = WORLD_SLEEP_CONST+prevSleepTime-diff;
            ACE_Based::Thread::Sleep(prevSleepTime);
        }
        else
            prevSleepTime = 0;
    }

    sWorld.KickAll();                                       // save and kick all players
    sWorld.UpdateSessions( 1 );                             // real players unload required UpdateSessions call

    // unload battleground templates before different singletons destroyed
    sBattleGroundMgr.DeleteAlllBattleGrounds();

    sWorldSocketMgr->StopNetwork();

    formation_mgr.UnloadCreatureFormations();

    MapManager::Instance().UnloadAll();                     // unload all grids (including locked in memory)

    ///- Free MySQL thread resources and deallocate lingering connections
    if (!(WorldDatabase.GetBundleMask() & MYSQL_BUNDLE_WORLD))
        WorldDatabase.End_MySQL_Connection();

    if (!(LoginDatabase.GetBundleMask() & MYSQL_BUNDLE_WORLD))
        LoginDatabase.End_MySQL_Connection();

    if (!(CharacterDatabase.GetBundleMask() & MYSQL_BUNDLE_WORLD))
        CharacterDatabase.End_MySQL_Connection();

    if (needInit)
        MySQL::Thread_End();
}

