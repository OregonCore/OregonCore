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

#ifndef DEF_ZULAMAN_H
#define DEF_ZULAMAN_H

enum ZulAmanEncounter
{
    /* Elite Four */
    ENCOUNTER_NALORAKK     = 0,
    ENCOUNTER_AKILZON      = 1,
    ENCOUNTER_JANALAI      = 2,
    ENCOUNTER_HALAZZI      = 3,

    ENCOUNTER_ELITE_FOUR   = 4, // boundary

    /* After Quest */
    ENCOUNTER_HEXLORD      = 4,
    ENCOUNTER_ZULJIN       = 5,

    ENCOUNTER_BOSS         = 6, // boundary

    /* Other */
    ENCOUNTER_CHESTLOOTED      = 6,
    ENCOUNTER_RAND_VENDOR_1    = 7,
    ENCOUNTER_RAND_VENDOR_2    = 8,
    ENCOUNTER_AKILZON_GAUNTLET = 9, // TODO
    ENCOUNTER_GONG             = 10,// TODO

    ENCOUNTERS             = 11 // boundary
};

enum ZulAmanWorldState
{
    WORLD_STATE_QUEST_RUNNING = 3104, // whether to show the timer
    WORLD_STATE_QUEST_TIMER   = 3106  // the number on the timer
};

enum ZulAmanEntries
{
    /* Doors */
    ENTRY_MASSIVE_GATE          = 186728,

    ENTRY_HALAZZI_EXIT_DOOR     = 186303,
    ENTRY_HALAZZI_ENTRANCE_DOOR = 186304,

    ENTRY_HEXLORD_ENTRANCE_DOOR = 186305,
    ENTRY_HEXLORD_EXIT_DOOR     = 186306,

    ENTRY_AKILZON_DOOR          = 186858,
    ENTRY_ZULJIN_DOOR           = 186859,

    /* Chests */
    ENTRY_CHEST_HARKORS         = 187021,
    ENTRY_CHEST_TANZARS         = 186648,
    ENTRY_CHEST_ASHLIS          = 186672,
    ENTRY_CHEST_KRAZS           = 186667
};

enum ZulAmanMisc
{
    ZA_MAGIC                    = 0x5A41, // "ZA"

    ZA_TIMER_MINUTES            = 20,
    // Only two bosses should grant extra time
    ZA_TIMER_ADDITION_NALORAKK  = 15,
    ZA_TIMER_ADDITION_AKILZON   = 10
};

#endif

