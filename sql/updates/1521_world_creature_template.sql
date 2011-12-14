UPDATE `creature_template` SET `ScriptName` = 'npc_jovaan' WHERE `entry` = '21633';
UPDATE `creature` SET `position_x` = '-3307.181396', `position_y` = '2932.005371', `position_z` = '170.925766', `orientation` = '5.646164' WHERE `guid` = '86831';
INSERT INTO `creature_template_addon` VALUES ('21633', '0', '0', '16777472', '8', '4097', '0', '0', '0', null);

INSERT INTO `script_texts` (`entry`,`content_default`,`content_loc1`,`content_loc2`,`content_loc3`,`content_loc4`,`content_loc5`,`content_loc6`,`content_loc7`,`content_loc8`,`sound`,`type`,`language`,`emote`,`comment`) VALUES
 (-1900140,'Everything is in readiness, warbringer.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_JOVAAN1'),
 (-1900141,'Doom Lord Kazzak will be pleased. You are to increase the pace of your attacks. Destroy the orcish and dwarven strongholds with all haste.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_WARBRINGER1'),
 (-1900142,'Warbringer, that will require the use of all the hold\'s infernals. It may leave us vulnerable to a counterattack.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_JOVAAN2'),
 (-1900143,'Don\'t worry about that. I\'ve increased production at the Deathforge. You\'ll have all the infernals you need to carry out your orders. Don\'t fail, Jovaan.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_WARBRINGER2'),
 (-1900144,'It shall be as you say, warbringer. One last question, if I may...',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_JOVAAN3'),
 (-1900145,'Yes?',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_WARBRINGER3'),
 (-1900146,'What\'s in the crate?',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_JOVAAN4'),
 (-1900147,'Crate? I didn\'t send you a crate, Jovaan. Don\'t you have more important things to worry about? Go see to them!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_WARBRINGER4');

UPDATE `creature_template` SET `ScriptName` = 'npc_zeppit' WHERE `entry` = '22484';

DELETE FROM `creature` WHERE `guid` = '70248';
DELETE FROM `creature` WHERE `guid` = '70249';
DELETE FROM `creature` WHERE `guid` = '70250';
DELETE FROM `creature` WHERE `guid` = '70251';
DELETE FROM `creature` WHERE `guid` = '70252';
DELETE FROM `creature` WHERE `guid` = '70253';
DELETE FROM `creature` WHERE `guid` = '70254';
UPDATE `creature_template` SET `unit_flags` = '4' WHERE entry = '20284';
UPDATE `creature_template` SET `flags_extra` = '2' WHERE entry = '19692';
UPDATE `creature_template` SET `ScriptName` = 'npc_dr_boom' WHERE `entry` = '20284';
UPDATE `creature_template` SET `ScriptName` = 'npc_boom_bot' WHERE `entry` = '19692';