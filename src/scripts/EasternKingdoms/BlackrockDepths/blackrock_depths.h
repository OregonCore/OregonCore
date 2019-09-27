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

#ifndef DEF_BRD_H
#define DEF_BRD_H

#define FACTION_NEUTRAL             734
#define FACTION_HOSTILE             754
#define FACTION_FRIEND              35

enum eTypes
{
    TYPE_RING_OF_LAW        = 1,
    TYPE_VAULT              = 2,
    TYPE_BAR                = 3,
    TYPE_TOMB_OF_SEVEN      = 4,
    TYPE_LYCEUM             = 5,
    TYPE_IRON_HALL          = 6,

    DATA_EMPEROR            = 10,
    DATA_PHALANX            = 11,

    DATA_ARENA1             = 12,
    DATA_ARENA2             = 13,
    DATA_ARENA3             = 14,
    DATA_ARENA4             = 15,

    DATA_GO_BAR_KEG         = 16,
    DATA_GO_BAR_KEG_TRAP    = 17,
    DATA_GO_BAR_DOOR        = 18,
    DATA_GO_CHALICE         = 19,

    DATA_GHOSTKILL          = 20,
    DATA_EVENSTARTER        = 21,

    DATA_GOLEM_DOOR_N       = 22,
    DATA_GOLEM_DOOR_S       = 23,

    DATA_THRONE_DOOR        = 24,

    DATA_SF_BRAZIER_N       = 25,
    DATA_SF_BRAZIER_S       = 26,

	DATA_MOIRA				= 27,
	DATA_EVENTSTARTER		= 28,
	TYPE_RELIC_COFFER		= 29,
	DATA_RELICCOFFERCOUNT	= 30,

	DATA_DUGHAL				= 31,
	DATA_QUEST_JAIL_BREAK	= 32,
	DATA_SUPPLY_ROOM		= 33,
	DATA_GATE_SR			= 34,
	DATA_GATE_SC			= 35,
	DATA_GATE_D				= 36,
	DATA_SHILL				= 37,
	DATA_GATE_J				= 38,
	DATA_JAZ				= 39,
	DATA_CREATURE_JAZ		= 40,
	DATA_CREATURE_OGRABISI  = 41,
	DATA_GATE_S				= 42,
	DATA_CREATURE_SHILL		= 43,
	DATA_GATE_C				= 44,
	DATA_CREST				= 45,
	DATA_CREATURE_CREST		= 46,
	DATA_TOBIAS				= 47,
	DATA_OGRABISI			= 48
};

enum NPC
{
	NPC_MOIRA = 8929,
	NPC_WATCHMAN = 9476
};
#endif
