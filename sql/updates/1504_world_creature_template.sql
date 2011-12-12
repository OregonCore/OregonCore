UPDATE `quest_template` SET `ReqCreatureOrGOID1`  = '20812' WHERE `entry` = '9924';
UPDATE `gameobject_template` SET `data2` = '1966080' WHERE `entry` = '182350';
UPDATE `gameobject_template` SET `ScriptName` = 'go_corkis_prison2' WHERE `entry` = '182350';
UPDATE `creature_template` SET `ScriptName` = 'npc_corki2' WHERE `entry` = '20812';

INSERT INTO `script_texts` (`entry`,`content_default`,`content_loc1`,`content_loc2`,`content_loc3`,`content_loc4`,`content_loc5`,`content_loc6`,`content_loc7`,`content_loc8`,`sound`,`type`,`language`,`emote`,`comment`) VALUES
 (-1900139,'This is the last time I get caught! I promise! Bye!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_KORKI7');

DELETE FROM `script_waypoint` WHERE `entry`=20812;
INSERT INTO `script_waypoint` VALUES
   (20812,0,-972.953,8125.154,-94.303,0, ''),
   (20812,1,-981.466,8120.831,-95.112,0, ''),
   (20812,2,-1058.294,8102.698,-89.662,0, '');