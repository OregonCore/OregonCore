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

#ifndef DEF_DEADMINES_H
#define DEF_DEADMINES_H

enum CannonState
{
    CANNON_NOT_USED,
    CANNON_GUNPOWDER_USED,
    CANNON_BLAST_INITIATED,
    PIRATES_ATTACK,
    SMITE_ALARMED,
    EVENT_DONE
};

enum Data
{
    EVENT_CANNON,
    EVENT_RHAHKZOR,
    EVENT_SNEED,
    EVENT_GILNID
};

enum GameObjects
{
    GO_FACTORY_DOOR                                        = 13965,
    GO_MAST_ROOM_DOOR                                      = 16400,
    GO_FOUNDRY_DOOR                                        = 16399,
    GO_IRONCLAD_DOOR                                       = 16397,
    GO_DEFIAS_CANNON                                       = 16398,
    GO_DOOR_LEVER                                          = 101833,
    GO_MR_SMITE_CHEST                                      = 144111
};

enum CreaturesIds
{
    NPC_RHAHK_ZOR  = 644, 
    NPC_MR_SMITE   = 646,
    NPC_OVERSEER   = 634,
    NPC_EVOKER     = 1729,
    NPC_TASKMASTER = 4417
};

#endif
