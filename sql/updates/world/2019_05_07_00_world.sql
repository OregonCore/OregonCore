DROP TABLE IF EXISTS `points_of_interest`; 
CREATE TABLE `points_of_interest` (
  `entry` mediumint(8) unsigned NOT NULL default '0',
  `x` float NOT NULL default '0',
  `y` float NOT NULL default '0',
  `icon` mediumint(8) unsigned NOT NULL default '0',
  `flags` mediumint(8) unsigned NOT NULL default '0',
  `data` mediumint(8) unsigned NOT NULL default '0',
  `icon_name` text NOT NULL,
  PRIMARY KEY  (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS `locales_points_of_interest`; 
CREATE TABLE `locales_points_of_interest` (
  `entry` mediumint(8) unsigned NOT NULL default '0',
  `icon_name_loc1` text,
  `icon_name_loc2` text,
  `icon_name_loc3` text,
  `icon_name_loc4` text,
  `icon_name_loc5` text,
  `icon_name_loc6` text,
  `icon_name_loc7` text,
  `icon_name_loc8` text,
  PRIMARY KEY  (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

UPDATE `gameobject` SET `position_z`= -7.792719 WHERE `guid`=1399;
UPDATE `gameobject` SET `position_z`= -4.470833 WHERE `guid`=18810;
UPDATE `gameobject` SET `position_z`= -2.242433 WHERE `guid`=631;

DELETE FROM `creature_loot_template` WHERE `Entry` = 423 AND `Item` = 6833;

-- Prince Thunderaan
UPDATE `creature_template` SET `AIName`="SmartAI" WHERE `entry`=14435;
DELETE FROM `smart_scripts` WHERE `entryorguid`=14435 AND `source_type`=0;
DELETE FROM `smart_scripts` WHERE `entryorguid`=1443500 AND `source_type`=9;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(14435,0,0,0,54,0,100,0,0,0,0,0,80,1443500,0,0,0,0,0,1,0,0,0,0,0,0,0,"Prince Thunderaan - Just Summoned - Run Script"),
(14435,0,1,0,4,0,100,0,0,0,0,0,1,2,0,0,0,0,0,1,0,0,0,0,0,0,0,"Prince Thunderaan - On Aggro - Say Line 2"),
(14435,0,2,0,0,0,100,0,11000,15000,12000,20000,11,23009,0,0,0,0,0,1,0,0,0,0,0,0,0,"Prince Thunderaan - In Combat - Cast 'Tendrils of Air'"),
(14435,0,3,0,0,0,100,0,5000,7000,8000,11000,11,23011,0,0,0,0,0,1,0,0,0,0,0,0,0,"Prince Thunderaan - In Combat - Cast 'Tears of the Wind Seeker'"),
(1443500,9,0,0,0,0,100,0,0,0,0,0,11,20568,0,0,0,0,0,1,0,0,0,0,0,0,0,"Prince Thunderaan - On Script - Cast 'Ragnaros Emerge'"),
(1443500,9,1,0,0,0,100,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,"Prince Thunderaan - On Script - Say Line 0"),
(1443500,9,2,0,0,0,100,0,13000,13000,0,0,1,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Prince Thunderaan - On Script - Say Line 1");

DELETE FROM `creature_text` WHERE `CreatureID`=14435;
INSERT INTO `creature_text` (`CreatureID`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `TextRange`, `comment`) VALUES
(14435,0,0,"Well done, servant. For so long I have been held captive within the prison of the Firelord. I am once more free to feast upon the out world! The land shall once more be ravaged by the force of the Wind Seeker!",14,0,100,0,0,0,0,"Prince Thunderaan"),
(14435,1,0,"Ah, and the fool who freed me shall be among the first to quell my immense hunger.",14,0,100,0,0,0,0,"Prince Thunderaan"),
(14435,2,0,"My power is discombobulatingly devastating! It is ludicrous that these mortals even attempt to enter my realm!",14,0,100,0,0,0,0,"Prince Thunderaan");

-- Dabiree fixes
-- Add sniffed gossip texts to menu
DELETE FROM `gossip_menu_option` WHERE `menu_id` = 7970;
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `action_menu_id`, `action_poi_id`, `box_coded`, `box_money`, `box_text`) VALUES
(7970, 0, 0, 'I''m on a bombing mission for Forward Commander Kingston.  I need a gryphon destroyer!',1,1,0,0,0,0,''),
(7970, 1, 0, 'Send me to Shatter Point!',1,1,0,0,0,0,'');

-- Migrate NPC to use SmartAI
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '', `gossip_menu_id` = 7970 WHERE `entry` = 19409;

-- Condition: Gossip menu ID 0 needs either quest ID 10146 to be incomplete.
-- Condition: Gossip menu ID 1 needs quest ID 10340 to not be rewarded
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`= 15 AND `SourceGroup` = 7970 AND `SourceEntry`IN (0,1);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`,`SourceGroup`,`SourceEntry`,`SourceId`,`ElseGroup`,`ConditionTypeOrReference`,`ConditionTarget`,`ConditionValue1`,`ConditionValue2`,`ConditionValue3`,`NegativeCondition`,`ErrorTextId`,`ScriptName`,`Comment`) VALUES
(15,7970,0,0,0,9,0,10146,0,0,0,0,'','Wing Commander Dabir''ee - Show gossip option 0 if player has taken quest ID 10146'),
(15,7970,1,0,0,8,0,10340,0,0,1,0,'','Wing Commander Dabir''ee - Show gossip option 1 if quest ID 10340 is not rewarded');

-- Create SmartAI for Wing Commander Dabir'ee
DELETE FROM `smart_scripts` WHERE (source_type = 0 AND entryorguid = 19409);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(19409,0,0,2,62,0,100,0,7970,0,0,0,11,33768,0,0,0,0,0,7,0,0,0,0,0,0,0,'Wing Commander Dabir''ee - On Gossip Option 0 Selected - Cast Spell 33768'),
(19409,0,1,2,62,0,100,0,7970,1,0,0,11,35069,0,0,0,0,0,7,0,0,0,0,0,0,0,'Wing Commander Dabir''ee - On Gossip Option 1 Selected - Cast Spell 35069'),
(19409,0,2,0,61,0,100,0,0,0,0,0,72,0,0,0,0,0,0,7,0,0,0,0,0,0,0,'Wing Commander Dabir''ee - On Linked Actions - Close Gossip');

-- Misc creature fixes
UPDATE `creature` SET `MovementType` = 0 WHERE id = 22987 AND guid IN (79007, 79008, 79009);

-- Add sniffed gossip texts to menu
DELETE FROM `gossip_menu_option` WHERE `menu_id` IN (8095, 8096, 8100);
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `action_menu_id`, `action_poi_id`, `box_coded`, `box_money`, `box_text`) VALUES
(8095, 0, 2, 'Show me where I can fly.',1,1,0,0,0,0,''),
(8096, 0, 0, 'Send me to Honor Point!',1,1,0,0,0,0,''),
(8096, 1, 0, 'Send me to the Abyssal Shelf!',1,1,0,0,0,0,''),
(8100, 0, 0, 'Send me to Shatter Point!',1,1,0,0,0,0,'');

-- Migrate NPC to use SmartAI
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '', `gossip_menu_id` = 8096 WHERE `entry` = 20235;

-- Condition: Gossip menu ID 0 needs either quest ID 10163 or quest ID 10346 to be incomplete.
-- Condition: Gossip menu ID 1 needs quest ID 10382 to be complete
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`= 15 AND `SourceGroup` = 8096 AND `SourceEntry`IN (0,1);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`,`SourceGroup`,`SourceEntry`,`SourceId`,`ElseGroup`,`ConditionTypeOrReference`,`ConditionTarget`,`ConditionValue1`,`ConditionValue2`,`ConditionValue3`,`NegativeCondition`,`ErrorTextId`,`ScriptName`,`Comment`) VALUES
(15,8096,1,0,0,9,0,10163,0,0,0,0,'','Gryphoneer Windbellow - Show gossip option 1 if player has taken quest ID 10163'),
(15,8096,1,0,1,9,0,10346,0,0,0,0,'','Gryphoneer Windbellow - Show gossip option 1 if player has taken quest ID 10346'),
(15,8096,0,0,0,28,0,10382,0,0,0,0,'','Gryphoneer Windbellow - Show gossip option 0 if player has completed quest ID 10382');

-- Create SmartAI for Gryphoneer Windbellow
DELETE FROM `smart_scripts` WHERE (source_type = 0 AND entryorguid = 20235);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(20235,0,0,2,62,0,100,0,8096,1,0,0,11,33899,0,0,0,0,0,7,0,0,0,0,0,0,0,'Gryphoneer Windbellow - On Gossip Option 1 Selected - Cast Spell 33899'),
(20235,0,1,2,62,0,100,0,8096,0,0,0,11,35065,0,0,0,0,0,7,0,0,0,0,0,0,0,'Gryphoneer Windbellow - On Gossip Option 0 Selected - Cast Spell 35065'),
(20235,0,2,0,61,0,100,0,0,0,0,0,72,0,0,0,0,0,0,7,0,0,0,0,0,0,0,'Gryphoneer Windbellow - On Linked Actions - Close Gossip');
