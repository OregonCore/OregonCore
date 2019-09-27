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

#ifndef DEF_BLOOD_FURNACE_H
#define DEF_BLOOD_FURNACE_H

#define DATA_MAKEREVENT             1
#define DATA_BROGGOKEVENT           2
#define DATA_KELIDANEVENT           3


enum GameObjectIds
{
    GO_PRISON_DOOR_01           = 181766, // Final Exit Door
    GO_PRISON_DOOR_02           = 181811, // The Maker Front Door
    GO_PRISON_DOOR_03           = 181812, // The Maker Rear Door
    GO_PRISON_DOOR_04           = 181819, // Broggok Rear Door
    GO_PRISON_DOOR_05           = 181822, // Broggok Front Door
    GO_SUMMON_DOOR              = 181823, // Kelidan Exit Door

    GO_PRISON_CELL_DOOR_1       = 181813, // The Maker Prison Cell front right
    GO_PRISON_CELL_DOOR_2       = 181814, // The Maker Prison Cell back right
    GO_PRISON_CELL_DOOR_3       = 181816, // The Maker Prison Cell front left
    GO_PRISON_CELL_DOOR_4       = 181815, // The Maker Prison Cell back left
    GO_PRISON_CELL_DOOR_5       = 181821, // Broggok Prison Cell front right
    GO_PRISON_CELL_DOOR_6       = 181818, // Broggok Prison Cell back right
    GO_PRISON_CELL_DOOR_7       = 181820, // Broggok Prison Cell front left
    GO_PRISON_CELL_DOOR_8       = 181817, // Broggok Prison Cell back left

    GO_BROGGOK_LEVER            = 181982
};

enum bloodFurnaceNPC
{
    NPC_CHANNELER               = 17653
};

#endif

