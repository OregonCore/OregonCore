
-- The Table is used for checking if an unit can cast an explicitly scripted (dummy) spell
-- Used for spells with EffectDummy and EffecyApplyAuraName == DUMMY

DROP TABLE IF EXISTS `spell_dummy_condition`;
CREATE TABLE `spell_dummy_condition`
(
    `entry` int unsigned not null comment 'Spell Id',
    `effIndex` tinyint unsigned not null default 0 comment 'effect index',
    `bitMaskCondition` int unsigned not null,
    `condition0` tinyint unsigned null default 0,
    `data0` int null default 0,
    `condition1` tinyint unsigned null default 0,
    `data1` int null default 0,
    `condition2` tinyint unsigned null default 0,
    `data2` int null default 0,
    `condition3` tinyint unsigned null default 0,
    `data3` int null default 0,
    `condition4` tinyint unsigned null default 0,
    `data4` int null default 0,
    `comment` varchar(255) null,

    KEY id (entry)
) engine=InnoDB;

/*
    bitMaskCondition:

    ----------------------------------------+---------+
    | name                                  | value   |
    ----------------------------------------+---------+
    | SDC_BTM_CASTER_MUST_NOT_HAVE_PET      | 1 << 0  |
    | SDC_BTM_CASTER_MUST_NOT_HAVE_CHARM    | 1 << 1  |
    | SDC_BTM_CASTER_MUST_BE_UNDERWATER     | 1 << 2  |
    | SDC_BTM_TARGET_MUST_BE_CREATURE       | 1 << 3  |
    | SDC_BTM_TARGET_MUST_BE_PLAYER         | 1 << 4  |
    | SDC_BTM_TARGET_MUST_BE_FRIENDLY       | 1 << 5  |
    | SDC_BTM_TARGET_MUST_BE_HOSTILE        | 1 << 6  |
    | SDC_BTM_TARGET_MUST_BE_DEAD           | 1 << 7  |
    | SDC_BTM_CASTER_MUST_BE_OUT_OF_COMBAT  | 1 << 8  |
    | SDC_BTM_TARGET_MUST_NOT_BE_HIER_LEVEL | 1 << 9  |
    +---------------------------------------+---------+ 
    | bits 10 - 16 are reserved                       |
    +---------------------------------------+---------+
    | SDC_NEEDS_SCRIPT_CHECK                | 1 << 17 |
    | SDC_BTM_TARGET_TYPE_NONE              | 1 << 18 |
    | SDC_BTM_TARGET_TYPE_BEAST             | 1 << 19 |
    | SDC_BTM_TARGET_TYPE_DRAGONKIN         | 1 << 20 |
    | SDC_BTM_TARGET_TYPE_DEMON             | 1 << 21 |
    | SDC_BTM_TARGET_TYPE_ELEMENTAL         | 1 << 22 |
    | SDC_BTM_TARGET_TYPE_GIANT             | 1 << 23 |
    | SDC_BTM_TARGET_TYPE_UNDEAD            | 1 << 24 |
    | SDC_BTM_TARGET_TYPE_HUMANOID          | 1 << 25 |
    | SDC_BTM_TARGET_TYPE_CRITTER           | 1 << 26 |
    | SDC_BTM_TARGET_TYPE_MECHANICAL        | 1 << 27 |
    | SDC_BTM_TARGET_TYPE_10                | 1 << 28 |
    | SDC_BTM_TARGET_TYPE_TOTEM             | 1 << 29 |
    | SDC_BTM_TARGET_TYPE_NON_COMBAT_PET    | 1 << 30 |
    | SDC_BTM_TARGET_TYPE_GAS_CLOUD         | 1 << 31 |
    +---------------------------------------+---------+

    NOTES:
       * if both TARGET_MUST_BE_CREATURE && TARGET_MUST_BE_PLAYER
         are set this means ANY unit, just like if we'd have used:
         MUST_BE_UNIT = (MUST_BE_CREATURE | MUST_BE_PLAYER)
       * if both TARGET_IS_FRIENDLY and TARGET_IS_HOSTILE
            1. are set - it means any target
            2. arent set - it means spell works only on NEUTRAL targets
       * TARGET_IS_FRINEDY means > neutral
       * TARGET_IS_HOSTILE means <= neutral
       * if none of tARGET_TYPE mask is set, spell works on any target

    conditionX and dataX are used for additional checking where bitmask is not enough.

    +-------------------------+-------+----------------------+------------------------------------------------------------------+
    | name                    | value | data                 | comment                                                          |
    +-------------------------+-------+----------------------+------------------------------------------------------------------+
    | SDC_NONE                | 0     | any                  | always true                                                      |
    | SDC_AURA_TARGET         | 1     | Spell Id             | Target must or must not have aura in `data` column.              |
    |                                                          (Based on positive/negative `data`)                              |
    | SDC_AURA_CASTER         | 2     | Spell Id             | Same as above but for caster                                     |
    | SDC_QUEST_TAKEN         | 3     | Quest Id             | Caster must have the quest taken                                 |
    | SDC_QUEST_REWARDED      | 4     | Quest Id             | Caster must have the quest completed                             |
    | SDC_HAS_SPELL           | 5     | Spell Id             | Caster must have the spell                                       |
    | SDC_ACTIVE_EVENT        | 6     | Event Id             | Game Event must be active                                        |
    | SDC_TEAM                | 7     | 0-Ali 1-Horde        | Caster must be in the team                                       |
    | SDC_CASTER_HP_PCT       | 8     | %HP                  | Casters hp must be above/below or equal to PCT (based on +/-)    |
    | SDC_TARGET_HP_PCT       | 9     | %HP                  | Targets hp must be above/below or equal to PCT (based on +/-)    |
    | SDC_CASTER_MANA_PCT     | 10    | %MANA                | Casters mana must be above/below or equal to PCT (based on +/-)  |
    | SDC_TARGET_MANA_PCT     | 11    | %MANA                | Targets mana must be above/below or equal to PCT (based on +/-)  |
    | SDC_TARGET_IN_ARC       | 12    | any or 1             | target must be in front of caster (value 1 = only enemy)         |
    | SDC_TARGET_EXACT_ENTRY  | 13    | entry id             | + = unit, - = gameobject. Target must have spec. id              |
    | SDC_TARGET_TYPE_FLAGS   | 14    | type_flags           | target must match type_flags (logical AND)                       |
    | SDC_TARGET_FAMILY       | 15    | family               | Must be in family. Negative num return true if > 0               |
    +-------------------------+-------+----------------------+------------------------------------------------------------------+
    
*/

truncate table spell_dummy_condition;
insert into spell_dummy_condition
( `entry`, `effIndex`, `bitMaskCondition`, `condition0`, `data0`, `condition1`, `data1`, `condition2`, `data2`, `condition3`, `data3`, `condition4`, `data4`, `comment` )
   VALUES
(  5308,    0,         1 << 3 | 1 << 4,    9,            -20,     0,             0,       0,            0,       0,            0,       0,            0,      'Execute' ),
( 20658,    0,         1 << 3 | 1 << 4,    9,            -20,     0,             0,       0,            0,       0,            0,       0,            0,      'Execute' ),
( 20660,    0,         1 << 3 | 1 << 4,    9,            -20,     0,             0,       0,            0,       0,            0,       0,            0,      'Execute' ),
( 20661,    0,         1 << 3 | 1 << 4,    9,            -20,     0,             0,       0,            0,       0,            0,       0,            0,      'Execute' ),
( 20662,    0,         1 << 3 | 1 << 4,    9,            -20,     0,             0,       0,            0,       0,            0,       0,            0,      'Execute' ),
( 25234,    0,         1 << 3 | 1 << 4,    9,            -20,     0,             0,       0,            0,       0,            0,       0,            0,      'Execute' ),
( 25236,    0,         1 << 3 | 1 << 4,    9,            -20,     0,             0,       0,            0,       0,            0,       0,            0,      'Execute' ),

( 51582,    0,         1 << 2,             0,              0,     0,             0,       0,            0,       0,            0,       0,            0,      'Rocket Boots Enganged'),
    
( 20473,    0,         0x00000018,        12,              1,     0,             0,       0,            0,       0,            0,       0,            0,      'Holy Shock'),
( 20929,    0,         0x00000018,        12,              1,     0,             0,       0,            0,       0,            0,       0,            0,      'Holy Shock'),
( 20930,    0,         0x00000018,        12,              1,     0,             0,       0,            0,       0,            0,       0,            0,      'Holy Shock'),
( 27174,    0,         0x00000018,        12,              1,     0,             0,       0,            0,       0,            0,       0,            0,      'Holy Shock'),
( 33072,    0,         0x00000018,        12,              1,     0,             0,       0,            0,       0,            0,       0,            0,      'Holy Shock'),

( 19938,    0,         0x00000018,         1,         -17443,     0,             0,       0,            0,       0,            0,       0,            0,      'Awaken Peon'),

(  1515,    0,         0x0008004B,        14,              1,    15,            -1,       0,            0,       0,            0,       0,            0,      'Tame Beast'),

( 13278,    1,         0x00000018,         0,              0,     0,             0,       0,            0,       0,            0,       0,            0,      'Gnomish Death Ray'),
( 31367,    1,         0x00000018,         0,              0,     0,             0,       0,            0,       0,            0,       0,            0,      'Netherweave Net'),
( 31368,    1,         0x00000018,         0,              0,     0,             0,       0,            0,       0,            0,       0,            0,      'Heavy Netherweave Net'),
( 47129,    0,         0x00000000,         0,              0,     0,             0,       0,            0,       0,            0,       0,            0,      'Totemic Beacon'),
( 47129,    1,         0x00000000,         0,              0,     0,             0,       0,            0,       0,            0,       0,            0,      'Totemic Beacon');

