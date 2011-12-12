UPDATE `quest_template` SET `ReqCreatureOrGOID1`  = '18369' WHERE `entry` = '9955';
UPDATE `gameobject_template` SET `data2` = '1966080' WHERE `entry` = '182521';
UPDATE `gameobject_template` SET `ScriptName` = 'go_corkis_prison3' WHERE `entry` = '182521';
UPDATE `creature_template` SET `ScriptName` = 'npc_corki3' WHERE `entry` = '18369';

DELETE FROM `script_waypoint` WHERE `entry`=18369;
INSERT INTO `script_waypoint` VALUES
   (18369,0,-915.809,8667.150,171.827,0, ''),
   (18369,1,-868.656,8709.678,173.826,0, ''),
   (18369,2,-817.007,8729.769,178.137,0, '');