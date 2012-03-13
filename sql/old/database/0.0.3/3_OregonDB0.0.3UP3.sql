-- Updatepack 3 for ODB 0.0.3 for 2.4.3

-- Niyo

-- Fix call of fire is grey quest at lvl 10 
UPDATE `quest_template` SET `QuestLevel` = '10' WHERE `quest_template`.`entry` = 2983;

-- Fix damage and hp boost for all players in black temple
DELETE FROM `spell_script_target` WHERE `entry` = 40851;
INSERT INTO `spell_script_target` (`entry`, `type`, `targetEntry`) VALUES(40851, 1, 22963);

-- Rideen

-- Reduce level of imp minions from 46 to 10
UPDATE `creature_template` SET `minlevel`='10' WHERE entry=12922;
UPDATE `creature_template` SET `maxlevel`='10' WHERE entry=12922;

-- Fixed some Elwynn quest
UPDATE `quest_template` SET `prevquestid`=0 WHERE `entry`=3105;

-- Quagmirran's Eye proc fix...noICD but works
DELETE FROM `spell_proc_event` WHERE entry=33297;

-- The Lightning Capacitor proc fix...it has no ICD but works
DELETE FROM `spell_proc_event` WHERE entry=37657;

-- Reckoning fix
UPDATE `spell_proc_event` SET cooldown='1' WHERE entry='32746';

-- Trinket fixes by Niyo
UPDATE `spell_proc_event` SET `procFlags`='65536',`Cooldown`='45' WHERE (`entry`='34320');
UPDATE `spell_proc_event` SET `procFlags`='65536',`Cooldown`='45' WHERE (`entry`='38347'); 

-- Niyo
UPDATE  `creature_template` SET  `faction_A` =  '35', `faction_H` =  '35' WHERE  `creature_template`.`entry` =19224;
UPDATE `creature_template` SET  `minlevel` =  '69',
`maxlevel` =  '72',
`minhealth` =  '8114',
`maxhealth` =  '11954',
`minmana` =  '3080',
`maxmana` =  '3309',
`faction_A` =  '14',
`faction_H` =  '14' WHERE  `creature_template`.`entry` =18701;

UPDATE `creature_template` SET  `faction_A` =  '14',
`faction_H` =  '14' WHERE  `creature_template`.`entry` =19919;

UPDATE `creature_template` SET  `faction_A` =  '14',
`faction_H` =  '14' WHERE  `creature_template`.`entry` =21580;

UPDATE `creature_template` SET  `faction_A` =  '14',
`faction_H` =  '14' WHERE  `creature_template`.`entry` =19920;

UPDATE `creature_template` SET  `faction_A` =  '14',
`faction_H` =  '14' WHERE  `creature_template`.`entry` =21579;

UPDATE `npc_vendor` SET  `ExtendedCost` =  '2375' WHERE `item` =35110;

-- Divine spirit ranks
DELETE FROM `playercreateinfo_spell_custom` WHERE `spell` = 27841;
DELETE FROM `playercreateinfo_spell_custom` WHERE `spell` = 14819;
DELETE FROM `playercreateinfo_spell_custom` WHERE `spell` = 14818;
DELETE FROM `playercreateinfo_spell_custom` WHERE `spell` = 14752;
DELETE FROM `playercreateinfo_spell_custom` WHERE `spell` = 25312;

-- Prayer of spirit ranks
DELETE FROM `playercreateinfo_spell_custom` WHERE `spell` = 27681;
DELETE FROM `playercreateinfo_spell_custom` WHERE `spell` = 32999;

-- Greater Blessing of Kings
DELETE FROM `playercreateinfo_spell_custom` WHERE `spell` = 25898;

-- Greater Blessing of Sanctuary
DELETE FROM `playercreateinfo_spell_custom` WHERE `spell` = 25899;
DELETE FROM `playercreateinfo_spell_custom` WHERE `spell` = 27169;

-- Blessing of Sanctuary
DELETE FROM `playercreateinfo_spell_custom` WHERE `spell` = 20911;
DELETE FROM `playercreateinfo_spell_custom` WHERE `spell` = 20912;
DELETE FROM `playercreateinfo_spell_custom` WHERE `spell` = 20913;
DELETE FROM `playercreateinfo_spell_custom` WHERE `spell` = 20914;
DELETE FROM `playercreateinfo_spell_custom` WHERE `spell` = 27168;

-- NON CLASS/PLAYER SPELLS

-- All Rogues have this spell
DELETE FROM `playercreateinfo_spell_custom` WHERE `spell` = 21927;

-- All female nightelf rogues have these spells
DELETE FROM `playercreateinfo_spell_custom` WHERE `spell` = 33668;
DELETE FROM `playercreateinfo_spell_custom` WHERE `spell` = 34232;

-- Curse of shadows ranks (removed in 2.4.3)
DELETE FROM `playercreateinfo_spell_custom` WHERE `spell` = 17862;
DELETE FROM `playercreateinfo_spell_custom` WHERE `spell` = 17937;
DELETE FROM `playercreateinfo_spell_custom` WHERE `spell` = 27229;


-- ADD MISSING SPELLS
-- Warlock - Inferno
DELETE FROM `playercreateinfo_spell_custom` WHERE `class` =9 AND `Spell` = 1122;
INSERT INTO `playercreateinfo_spell_custom` (`race` ,`class` ,`Spell` ,`Note` ,`Active`)VALUES ('1',  '9',  '1122', NULL ,  '1');
INSERT INTO `playercreateinfo_spell_custom` (`race` ,`class` ,`Spell` ,`Note` ,`Active`)VALUES ('2',  '9',  '1122', NULL ,  '1');
INSERT INTO `playercreateinfo_spell_custom` (`race` ,`class` ,`Spell` ,`Note` ,`Active`)VALUES ('5',  '9',  '1122', NULL ,  '1');
INSERT INTO `playercreateinfo_spell_custom` (`race` ,`class` ,`Spell` ,`Note` ,`Active`)VALUES ('7',  '9',  '1122', NULL ,  '1');
INSERT INTO `playercreateinfo_spell_custom` (`race` ,`class` ,`Spell` ,`Note` ,`Active`)VALUES ('11',  '9',  '1122', NULL ,  '1');

-- Paladin - Purify
DELETE FROM `playercreateinfo_spell_custom` WHERE `class` =2 AND `Spell` = 1152;
INSERT INTO `playercreateinfo_spell_custom` (`race` ,`class` ,`Spell` ,`Note` ,`Active`)VALUES ('1',  '2',  '1152', NULL ,  '1');
INSERT INTO `playercreateinfo_spell_custom` (`race` ,`class` ,`Spell` ,`Note` ,`Active`)VALUES ('3',  '2',  '1152', NULL ,  '1');
INSERT INTO `playercreateinfo_spell_custom` (`race` ,`class` ,`Spell` ,`Note` ,`Active`)VALUES ('10',  '2',  '1152', NULL ,  '1');
INSERT INTO `playercreateinfo_spell_custom` (`race` ,`class` ,`Spell` ,`Note` ,`Active`)VALUES ('11',  '2',  '1152', NULL ,  '1');

-- Paladin - Blessing of Protection Rank 2
DELETE FROM `playercreateinfo_spell_custom` WHERE `class` =2 AND `Spell` = 5599;
INSERT INTO `playercreateinfo_spell_custom` (`race` ,`class` ,`Spell` ,`Note` ,`Active`)VALUES ('1',  '2',  '5599', NULL ,  '1');
INSERT INTO `playercreateinfo_spell_custom` (`race` ,`class` ,`Spell` ,`Note` ,`Active`)VALUES ('3',  '2',  '5599', NULL ,  '1');
INSERT INTO `playercreateinfo_spell_custom` (`race` ,`class` ,`Spell` ,`Note` ,`Active`)VALUES ('10',  '2',  '5599', NULL ,  '1');
INSERT INTO `playercreateinfo_spell_custom` (`race` ,`class` ,`Spell` ,`Note` ,`Active`)VALUES ('11',  '2',  '5599', NULL ,  '1');

-- Warrior - Parry
DELETE FROM `playercreateinfo_spell_custom` WHERE `class` =1 AND `Spell` = 3127;
INSERT INTO `playercreateinfo_spell_custom` (`race` ,`class` ,`Spell` ,`Note` ,`Active`)VALUES ('1',  '1',  '3127', NULL ,  '1');
INSERT INTO `playercreateinfo_spell_custom` (`race` ,`class` ,`Spell` ,`Note` ,`Active`)VALUES ('2',  '1',  '3127', NULL ,  '1');
INSERT INTO `playercreateinfo_spell_custom` (`race` ,`class` ,`Spell` ,`Note` ,`Active`)VALUES ('3',  '1',  '3127', NULL ,  '1');
INSERT INTO `playercreateinfo_spell_custom` (`race` ,`class` ,`Spell` ,`Note` ,`Active`)VALUES ('4',  '1',  '3127', NULL ,  '1');
INSERT INTO `playercreateinfo_spell_custom` (`race` ,`class` ,`Spell` ,`Note` ,`Active`)VALUES ('5',  '1',  '3127', NULL ,  '1');
INSERT INTO `playercreateinfo_spell_custom` (`race` ,`class` ,`Spell` ,`Note` ,`Active`)VALUES ('6',  '1',  '3127', NULL ,  '1');
INSERT INTO `playercreateinfo_spell_custom` (`race` ,`class` ,`Spell` ,`Note` ,`Active`)VALUES ('7',  '1',  '3127', NULL ,  '1');
INSERT INTO `playercreateinfo_spell_custom` (`race` ,`class` ,`Spell` ,`Note` ,`Active`)VALUES ('8',  '1',  '3127', NULL ,  '1');
INSERT INTO `playercreateinfo_spell_custom` (`race` ,`class` ,`Spell` ,`Note` ,`Active`)VALUES ('11',  '1',  '3127', NULL ,  '1');