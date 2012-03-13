DROP TABLE IF EXISTS `pool_creature`;
CREATE TABLE `pool_creature` (
  `guid` int(10) unsigned NOT NULL default '0',
  `pool_entry` mediumint(8) unsigned NOT NULL default '0',
  `chance` float unsigned NOT NULL default '0',
  `description` varchar(255) DEFAULT NULL,
  PRIMARY KEY  (`pool_entry`,`guid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS `pool_gameobject`;
CREATE TABLE `pool_gameobject` (
  `guid` int(10) unsigned NOT NULL default '0',
  `pool_entry` mediumint(8) unsigned NOT NULL default '0',
  `chance` float unsigned NOT NULL default '0',
  `description` varchar(255) DEFAULT NULL,
  PRIMARY KEY  (`guid`,`pool_entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS `pool_pool`;
CREATE TABLE `pool_pool` (
  `pool_id` mediumint(8) unsigned NOT NULL default '0',
  `mother_pool` mediumint(8) unsigned NOT NULL default '0',
  `chance` float NOT NULL default '0',
  `description` varchar(255) DEFAULT NULL,
  PRIMARY KEY  (`pool_id`,`mother_pool`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS `pool_template`;
CREATE TABLE `pool_template` (
  `entry` mediumint(8) unsigned NOT NULL default '0' COMMENT 'Pool entry',
  `max_limit` int(10) unsigned NOT NULL default '0' COMMENT 'Max number of objects (0) is no limit',
  `description` varchar(255) DEFAULT NULL,
  PRIMARY KEY  (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS `game_event_pool`;
CREATE TABLE `game_event_pool` (
  `pool_entry` mediumint(8) unsigned NOT NULL default '0' COMMENT 'Id of the pool',
  `event` smallint(6) NOT NULL default '0' COMMENT 'Put negatives values to remove during event',
  PRIMARY KEY  (`pool_entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

DELETE FROM `npc_text` WHERE `ID` BETWEEN '21001' AND '21005';
INSERT INTO `npc_text` VALUES ('21001', 'What for?  If Illidan\'s demons don\'t catch me and enslave me again, then the naga of Zangarmarsh will.$B$BJoining Illidan and his allies has turned out to mean nothing but slavery to most of the Broken tribes.  There is no hope for my people.', '', '0', '1', '0', '0', '0', '0', '0', '0', '', '', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '0', '0', '0', '0', '0', '0', '0', '12340');
INSERT INTO `npc_text` VALUES ('21002', '', 'Oh, no!  My Fei Fei would never take someone else\'s things!  He\'s a good dog... he just needs attention sometimes.  If you give him a treat, then you see!', '0', '1', '0', '0', '0', '0', '0', '0', '', '', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '0', '0', '0', '0', '0', '0', '0', '1');
INSERT INTO `npc_text` VALUES ('21003', 'Many years ago, my men and I followed the wizard Khadgar and the paladin Turalyon to this world. Our sole aim was to annihilate the orcs and their vile kin. Yet something went terribly wrong in the final hours of our war.$B$BUltimately, this dark world was ripped asunder - and my dear friends were lost to the void.', '', '0', '1', '0', '1', '0', '0', '0', '0', '', '', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '0', '0', '0', '0', '0', '0', '0', '11723');
INSERT INTO `npc_text` VALUES ('21004', 'We, the Sons of Lothar, have remained here ever since - fighting... dying... praying that a day like today would come - a day when the Portal would open and our brothers would come to deliver us home.$B$BOverjoyed as we are, duty still calls. There is much to be done before we can depart this world and take our rest.$B', '', '0', '1', '0', '1', '0', '0', '0', '0', '', '', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '0', '0', '0', '0', '0', '0', '0', '11723');
INSERT INTO `npc_text` VALUES ('21005', 'Romus informed me of the tragedies that beset my beautiful city. There are no fitting words for the sorrow I felt upon hearing of Lordaeron\'s fate - and the fate of my own, beloved Stromgarde.$B$BTwenty years ago, no one had heard of the \'Burning Legion\'... Damn those cursed devils!$BRest assured, $N; should I ever return to Azeroth, Stromgarde will once more rise as a force of reckoning. This I swear...', '', '0', '1', '0', '1', '0', '1', '0', '25', '', '', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '0', '0', '0', '0', '0', '0', '0', '11723');

UPDATE oregon_string SET content_default = 'Your chat has been disabled for %u minutes. Reason: %s.' WHERE oregon_string.entry=300;

DELETE FROM `spell_script_target` WHERE `entry` IN (25158, 33806);
INSERT INTO `spell_script_target` VALUES
    (25158,1,15381),
    (33806,1,15382);

UPDATE `quest_template` SET `SpecialFlags`='2' WHERE `entry`=434;
UPDATE `creature_template` SET `Speed`='1' WHERE `entry`=8856;
UPDATE `creature_template` SET `Speed`='1' WHERE `entry`=7779;


DELETE FROM `spell_script_target` WHERE `targetEntry` IN (8313,38968);

UPDATE `creature_template` SET `ScriptName`='npc_volcano' WHERE `entry`='23085';

UPDATE creature_template SET speed = 3 WHERE entry = 11832;
UPDATE creature_template SET minhealth = 119770 WHERE entry = 11832;
UPDATE creature_template SET maxhealth = 119770 WHERE entry = 11832;
UPDATE creature_template SET minmana = 115815 WHERE entry = 11832;
UPDATE creature_template SET maxmana = 115815 WHERE entry = 11832;
UPDATE creature_template SET faction_a = 14 WHERE entry = 15629;
UPDATE creature_template SET faction_h = 14 WHERE entry = 15629;

DELETE FROM `creature` WHERE `guid` = '1189307';
INSERT INTO `creature` VALUES (1189307,23383,530,1,21169,0,-4106.45,3029.62,344.877,2.46024,25,0,0,9250,0,0,0);
DELETE FROM `creature` WHERE `guid` = '1189308';
INSERT INTO `creature` VALUES (1189308,23383,530,1,21169,0,-3669.57,3386.74,312.955,3.69251,25,0,0,9250,0,0,0);
DELETE FROM `creature` WHERE `guid` = '1189309';
INSERT INTO `creature` VALUES (1189309,23383,530,1,21169,0,-3720.35,3789.91,302.888,5.20119,25,0,0,9250,0,0,0);
DELETE FROM `gameobject_template` WHERE `ENTRY` = 185952;
INSERT INTO `gameobject_template` VALUES ('185952', '0', '1787', 'Cage', '', '14', '32', '1.13', '0', '0', '262144', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '');
DELETE FROM `gameobject` WHERE `guid` = '100060';
INSERT INTO `gameobject` VALUES (100060, 185952, 530, 1, -3669.17, 3386.68, 312.956, 2.11701, 0, 0, 0.871624, 0.490175, 25, 0, 1);
DELETE FROM `gameobject` WHERE `guid` = '100061';
INSERT INTO `gameobject` VALUES (100061, 185952, 530, 1, -3720.29, 3790.02, 302.888, 3.82518, 0, 0, 0.942155, -0.335177, 25, 0, 1);
DELETE FROM `pool_template` WHERE `entry` = '15000';
INSERT INTO `pool_template` VALUES ('15000', '1', 'Quest: Escape from Skettis');
DELETE FROM `pool_creature` WHERE `guid` IN (1189307,1189308,1189309);
INSERT INTO `pool_creature` VALUES ('1189307', '15000', '33', 'Skyguard Prisoner');
INSERT INTO `pool_creature` VALUES ('1189308', '15000', '33', 'Skyguard Prisoner');
INSERT INTO `pool_creature` VALUES ('1189309', '15000', '34', 'Skyguard Prisoner');


UPDATE `oregon_string` SET `content_default` = '%d (Entry: %d) - |cffffffff|Hgameobject:%d|h[%s X:%f Y:%f Z:%f MapId:%d]|h|r ' WHERE `entry` = 517;


DELETE FROM command where name IN ('modify fly');

INSERT INTO `command` VALUES
 ('modify fly', 1, 'Syntax: .modify fly #rate\r\n.fly #rate\r\n\r\nModify the flying speed of the selected player to \"normal base fly speed\"*rate. If no player is selected, modify your fly.\r\n\r\n #rate may range from 0.1 to 50.');
 