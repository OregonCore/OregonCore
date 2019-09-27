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

#ifndef DEF_SUNWELLPLATEAU_H
#define DEF_SUNWELLPLATEAU_H

#define DISPLAYID_PORTAL_OPENING 22742

enum Outro
{
    POINT_KILJAEDEN_DIE = 1,
    POINT_TELEPORT_KALECGOS,
    SAY_KALECGOS_GOODBYE,
    POINT_SUMMON_SHATTERED,
    POINT_SUMMON_PORTAL,
    POINT_SUMMON_PORTAL_ENDOPENANIM,
    POINT_SUMMON_SOLDIERS_RIGHT,
    POINT_SUMMON_SOLDIERS_LEFT,
    POINT_SUMMON_PROPHET,
    POINT_SUMMON_LIADRIN,
    SAY_OUTRO_1,
    SAY_OUTRO_2,
    SAY_OUTRO_3,
    POINT_CALL_ENTROPIUS,
    SAY_OUTRO_4,
    POINT_MOVE_LIADRIN,
    SAY_OUTRO_5,
    SAY_OUTRO_6,
    SAY_OUTRO_7,
    SAY_OUTRO_8,
    POINT_BLAZE,
    POINT_IGNITE,
    SAY_OUTRO_9,
    SAY_OUTRO_10,
    SAY_OUTRO_11,
    SAY_OUTRO_12,
    POINT_EVENT_SOLDIER_EXIT,
    POINT_EVENT_VELEN_EXIT,
    OUTRO_DONE,
};

enum SpellOutro
{
    SPELL_TELEPORT_VISUAL           = 41232,
    SPELL_KALEC_TELEPORT            = 46473, // teleports and transforms Kalec in human form
    SPELL_CALL_ENTROPIUS            = 46818, // pulling effect
    SPELL_ENTROPIUS_BODY            = 46819, // Visual for Entropius at the Epilogue
    SPELL_BLAZE_TO_LIGHT            = 46821,
    SPELL_SUNWELL_IGNITION          = 46822, // same visual as SPELL_ANVEENA_ENERGY_DRAIN
    SPELL_OPEN_PORTAL_KJ            = 46801,
    SPELL_PORTAL_VISUAL             = 42047,
};

/*** Encounters ***/
enum Data
{
    DATA_KALECGOS_EVENT,
    DATA_BRUTALLUS_EVENT,
    DATA_FELMYST_EVENT,
    DATA_EREDAR_TWINS_EVENT,
    DATA_MURU_EVENT,
    DATA_KILJAEDEN_EVENT,
};

enum Data64
{
    /*** Creatures ***/
    DATA_KALECGOS_DRAGON,
    DATA_KALECGOS_HUMAN,
    DATA_SATHROVARR,
    DATA_BRUTALLUS,
    DATA_MADRIGOSA,
    DATA_FELMYST,
    DATA_ALYTHESS,
    DATA_SACROLASH,
    DATA_MURU,
    DATA_KILJAEDEN,
    DATA_KILJAEDEN_CONTROLLER,
    DATA_ANVEENA,
    DATA_KALECGOS_KJ,

    /*** GameObjects ***/
    DATA_GO_FORCEFIELD,
    DATA_ORB_OF_THE_BLUE_DRAGONFLIGHT_1,
    DATA_ORB_OF_THE_BLUE_DRAGONFLIGHT_2,
    DATA_ORB_OF_THE_BLUE_DRAGONFLIGHT_3,
    DATA_ORB_OF_THE_BLUE_DRAGONFLIGHT_4,

    /*** Misc ***/
    DATA_PLAYER_GUID,

    DATA_ACTIVATE_NORTH_TO_LEFT,
    DATA_ACTIVATE_CENTER_TO_LEFT,
    DATA_ACTIVATE_SOUTH_TO_LEFT,
    DATA_ACTIVATE_NORTH_TO_RIGHT,
    DATA_ACTIVATE_CENTER_TO_RIGHT,
    DATA_ACTIVATE_SOUTH_TO_RIGHT,

    DATA_OUTRO_KJ,
};

enum Creatures
{
    BOSS_MURU                                     = 25741,
    BOSS_ENTROPIUS                                = 25840,
    MOB_KALECGOS                                  = 24850,
    MOB_KALEC                                     = 24891,
    MOB_SATHROVARR                                = 24892,
    BOSS_BRUTALLUS                                = 24882,

    MOB_DEAD                                      = 25268,
    MOB_FLIGHT_LEFT                               = 25357,
    MOB_FLIGHT_RIGHT                              = 25358,
    MOB_DEATH_CLOUD                               = 25703,
    MOB_VAPOR                                     = 25265,
    MOB_VAPOR_TRAIL                               = 25267,

    NPC_FELMYST                                   = 25038,
    NPC_MADRIGOSA                                 = 25160,
    MOB_GRAND_WARLOCK_ALYTHESS                    = 25166,
    MOB_SHADOW_IMAGE                              = 25214,
    MOB_LADY_SACROLASH                            = 25165,

    CREATURE_ANVEENA                              = 26046, // Embodiment of the Sunwell
    CREATURE_KALECGOS                             = 25319, // Helps the raid throughout the fight
    CREATURE_KILJAEDEN                            = 25315, // Give it to 'em KJ!
    CREATURE_HAND_OF_THE_DECEIVER                 = 25588, // Adds found before KJ emerges
    CREATURE_FELFIRE_PORTAL                       = 25603, // Portal spawned be Hand of the Deceivers
    CREATURE_VOLATILE_FELFIRE_FIEND               = 25598, // Fiends spawned by the above portal
    CREATURE_ARMAGEDDON_TARGET                    = 25735, // This mob casts meteor on itself.. I think
    CREATURE_SHIELD_ORB                           = 25502, // Shield orbs circle the room raining shadow bolts on raid
    CREATURE_POWER_OF_THE_BLUE_DRAGONFLIGHT       = 25653, // NPC that players possess when using the Orb of the Blue Dragonflight
    CREATURE_SPIKE_TARGET1                        = 30598, // Should summon these under Shadow Spike Channel on targets place
    CREATURE_SPIKE_TARGET2                        = 30614,
    CREATURE_SINISTER_REFLECTION                  = 25708, // Sinister Relection spawnd on Phase swichtes

    CREATURE_DARKNESS                             = 25879,
    CREATURE_DARK_FIENDS                          = 25744,
    CREATURE_BERSERKER                            = 25798,
    CREATURE_FURY_MAGE                            = 25799,
    CREATURE_VOID_SENTINEL                        = 25772,
    CREATURE_VOID_SPAWN                           = 25824,
    CREATURE_BLACK_HOLE                           = 25855,
    CREATURE_SENTINAL_SUMMONER                    = 25782,

    /* Outro */
    CREATURE_PROPHET                              = 26246,
    CREATURE_LIADRIN                              = 26247,
    NPC_BOSS_PORTAL                               = 24925,
    NPC_CORE_ENTROPIUS                            = 26262,
    NPC_SOLDIER                                   = 26259, // summoned in 2 waves before Velen. Should move into 2 circle formations
    NPC_RIFTWALKER                                = 26289,
};

enum GameObjects
{
    GAMEOBJECT_ORB_OF_THE_BLUE_DRAGONFLIGHT       = 188415,
};

struct EventLocations
{
    float m_fX, m_fY, m_fZ, m_fO;
};

static const EventLocations aOutroLocations[] =
{
    { 1728.337f, 656.641f, 28.55f, 3.86f },       // portal summon loc
    { 1703.159f, 654.043f, 28.05f, 1.06f },       // first shattered summon loc
    { 1723.888f, 631.342f, 28.05f, 0.16f },       // second shattered summon loc
    { 1716.969f, 646.407f, 28.05f, 3.91f },       // velen summon loc
    { 1718.862f, 644.528f, 28.05f, 3.87f },       // liadrin summon loc
    { 1709.719f, 639.359f, 27.28f },              // velen move forward
    { 1711.537f, 637.600f, 27.34f },              // liadrin move forward
    { 1716.962f, 661.839f, 28.05f },              // first shattered move
    { 1735.141f, 643.680f, 28.23f },              // second shattered move
    { 1723.220f, 663.026f, 30.00f },              // first shattered spell target
    { 1734.877f, 649.574f, 30.00f },              // second shattered spell target
};

static const EventLocations SoldierLocations[] =
{
    { 1722.709f, 640.308f, 28.05f, 3.774f }, // summon first
    { 1727.329f, 639.419f, 28.05f, 3.721f }, // summon first
    { 1724.606f, 645.376f, 28.05f, 3.755f }, // summon first
    { 1719.543f, 644.635f, 28.05f, 3.718f }, // summon first
    { 1720.937f, 649.376f, 28.05f, 3.859f }, // summon first
    { 1715.420f, 648.200f, 28.05f, 4.507f }, // summon first
    { 1715.948f, 653.606f, 28.05f, 3.892f }, // summon first
    { 1710.871f, 651.645f, 28.05f, 4.252f }, // summon first
    { 1709.924f, 656.442f, 28.05f, 4.035f }, // summon first
    { 1705.944f, 654.201f, 28.05f, 4.290f }, // summon first
    { 1707.783f, 653.139f, 28.05f, 4.375f }, // summon second
    { 1712.047f, 655.015f, 28.05f, 4.208f }, // summon second
    { 1712.432f, 650.853f, 28.05f, 4.166f }, // summon second
    { 1717.019f, 652.191f, 28.05f, 3.986f }, // summon second
    { 1716.611f, 647.399f, 28.05f, 4.060f }, // summon second
    { 1721.737f, 648.310f, 28.05f, 3.893f }, // summon second
    { 1720.509f, 643.163f, 28.05f, 3.851f }, // summon second
    { 1725.338f, 643.300f, 28.05f, 3.634f }, // summon second
    { 1723.557f, 638.104f, 28.05f, 3.401f }, // summon second
    { 1728.344f, 636.061f, 28.05f, 3.305f }  // summon second
};

static const EventLocations SoldierMiddle[] =
{
    { 1718.604f, 608.202f, 28.05f, 1.090f }, // first
    { 1679.347f, 648.365f, 28.05f, 0.368f }, // second
};

#endif
