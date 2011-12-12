UPDATE `quest_template` SET `ReqCreatureOrGOID1`  = '18445' WHERE `entry` = '9923';
UPDATE `gameobject_template` SET `data2` = '1966080' WHERE `entry` = '182349';
UPDATE `gameobject_template` SET `ScriptName` = 'go_corkis_prison1' WHERE `entry` = '182349';
UPDATE `creature_template` SET `ScriptName` = 'npc_corki1' WHERE `entry` = '18445';

INSERT INTO `script_texts` (`entry`,`content_default`,`content_loc1`,`content_loc2`,`content_loc3`,`content_loc4`,`content_loc5`,`content_loc6`,`content_loc7`,`content_loc8`,`sound`,`type`,`language`,`emote`,`comment`) VALUES
 (-1900133,'Thanks $r ! I\'m sure my dad will reward you greatly! Bye!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_KORKI1'),
 (-1900134,'Hey, fatso! How do you go to the bathroom with all that fat?',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_KORKI2'),
 (-1900135,'Stop getting caught Corki!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_KORKI3'),
 (-1900136,'I want my daddy!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_KORKI4'),
 (-1900137,'I\'m not Corki! I\'m not Corki!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_KORKI5'),
 (-1900138,'Man that Corki sure is an idiot!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_KORKI6');

DELETE FROM `script_waypoint` WHERE `entry`=18445;
INSERT INTO `script_waypoint` VALUES
   (18445,0,-2561.155,6284.777,15.138,0, ''),
   (18445,1,-2544.227,6274.250,14.612,0, ''),
   (18445,2,-2528.367,6277.982,17.335,0, ''),
   (18445,3,-2477.215,6341.930,26.882,0, '');