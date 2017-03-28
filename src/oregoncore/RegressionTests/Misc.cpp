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

#include "RegressionTest.h"
#include <PrecompiledHeaders/gamePCH.h>

/**
  * Test for issue #1334.
  * In maraudon and some other areas player seen "breathing" window, however,
  * he wasn't under water surface. It was actually above water on the ground.
  * We will test the location and also some random under water location to make
  * sure breathing didnt break there, too.
  * @note This test equires maps and vmaps to work, otherwise it will always fail
  */
bool RegressionTestSuite::TestBreathingIssues()
{
    bool result = false;
    float x = -1454.263428f;
    float y = 2681.636719f;
    float z = 77.135460f;
    float o = .0f;

    // preapre dummy player
    WorldSession session(0, NULL, SEC_PLAYER, true, 0, LOCALE_enUS);
    Player player(&session);

    // taurens starts at map 1 -> our desired location
    player.Create(1, "RegressionTest", RACE_TAUREN, CLASS_WARRIOR, GENDER_MALE, 0, 0, 0, 0, 0, 0);

    // Initialize some dependencies for him and  move him to the location 
    ObjectAccessor::Instance().AddObject(&player);
    player.GetMotionMaster()->Initialize();
    Map* map = player.GetMap();
    player.Relocate(x, y, z, o);
    map->AddPlayerToMap(&player);
    player.SetPosition(x, y, z, o);

    // ok this calls  Player::UpdateUnderwaterState the bug was in
    player.Update(100);

    // ok, we should not breathe at this momennt
    if (!player.HasMirrorTimerFlag(UNDERWATER_INWATER))
        result = true;

    // cleanupp
    map->RemovePlayerFromMap(&player, false);
    ObjectAccessor::Instance().RemoveObject(&player);

    return result;
}
