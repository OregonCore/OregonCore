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

#ifndef DEF_ARCATRAZ_H
#define DEF_ARCATRAZ_H

uint32 const EncounterCount               = 13;
uint32 const MAX_WARDENS				  = 7;

#define AScriptName "instance_arcatraz"

enum DataTypes
{
    DATA_ZEREKETH                         = 1,
    DATA_DALLIAH                          = 2,
    DATA_SOCCOTHRATES                     = 3,
    DATA_HARBINGERSKYRISS                 = 4,
    DATA_WARDEN_1                         = 5,
    DATA_WARDEN_2                         = 6,
    DATA_WARDEN_3                         = 7,
    DATA_WARDEN_4                         = 8,
    DATA_WARDEN_5                         = 9,
    DATA_MELLICHAR                        = 10,
    DATA_SHIELD_OPEN                      = 11,
    DATA_SPHERE_SHIELD                    = 12,
    DATA_CONVERSATION                     = 13,
	DATA_ENTRANCE						  = 14
};

enum CreaturesIds
{
    NPC_MELLICHAR                         = 20904, //skyriss will kill this unit
    NPC_ALPHA_POD_TARGET                  = 21436,
    NPC_DALLIAH                           = 20885,
    NPC_SOCCOTHRATES                      = 20886,
	NPC_PRISON_BETA_POD				      = 21437,
	NPC_PRISON_DELTA_POD                  = 21438,
	NPC_PRISON_GAMMA_POD                  = 21439,
	NPC_PRISON_BOSS_POD                   = 21440,

	// intro event related
	NPC_PROTEAN_NIGHTMARE				  = 20864,
	NPC_PROTEAN_HORROR					  = 20865,
	NPC_ARCATRAZ_WARDEN					  = 20859,
	NPC_ARCATRAZ_DEFENDER				  = 20857,

	// Harbinger Skyriss event related
	NPC_BLAZING_TRICKSTER = 20905,       // phase 1
	NPC_PHASE_HUNTER = 20906,
	NPC_MILLHOUSE = 20977,               // phase 2
	NPC_AKKIRIS = 20908,                 // phase 3
	NPC_SULFURON = 20909,
	NPC_TW_DRAKONAAR = 20910,            // phase 4
	NPC_BL_DRAKONAAR = 20911,
	NPC_SKYRISS = 20912,                 // phase 5
};

enum GameObjectsIds
{
    CONTAINMENT_CORE_SECURITY_FIELD_ALPHA = 184318, //door opened when Wrath-Scryer Soccothrates dies
    CONTAINMENT_CORE_SECURITY_FIELD_BETA  = 184319, //door opened when Dalliah the Doomsayer dies
    POD_ALPHA                             = 183961, //pod first boss wave
    POD_BETA                              = 183963, //pod second boss wave
    POD_DELTA                             = 183964, //pod third boss wave
    POD_GAMMA                             = 183962, //pod fourth boss wave
    POD_OMEGA                             = 183965, //pod fifth boss wave
    WARDENS_SHIELD                        = 184802, // warden shield
    SEAL_SPHERE                           = 184802, //shield 'protecting' mellichar

	SPELL_TARGET_OMEGA					  = 36852	// Visual spell used by Mellichar
};

struct SpawnLocation
{
	float m_fX, m_fY, m_fZ, m_fO;
};

static const SpawnLocation aSummonPosition[5] =
{
	{ 478.326f, -148.505f, 42.56f, 3.19f },                   // Trickster or Phase Hunter
	{ 413.292f, -148.378f, 42.56f, 6.27f },                   // Millhouse
	{ 420.179f, -174.396f, 42.58f, 0.02f },                   // Akkiris or Sulfuron
	{ 471.795f, -174.58f, 42.58f, 3.06f },                    // Twilight or Blackwing Drakonaar
	{ 445.763f, -191.639f, 44.64f, 1.60f }                    // Skyriss
};

static const float aDalliahStartPos[4] = { 118.6038f, 96.84682f, 22.44115f, 1.012f };
static const float aSoccotharesStartPos[4] = { 122.1035f, 192.7203f, 22.44115f, 5.235f };

static const float aEntranceMoveLoc[3] = { 82.020f, 0.306f, -11.026f };
static const float aEntranceSpawnLoc[4] = { 173.471f, -0.138f, -10.101f, 3.123f };
#endif
