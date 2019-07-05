-- ==================================================================
-- Hammerfall - Arathi Highland
-- Hammerfall Guardian SAI
SET @GUID := -11212;
UPDATE `creature_template` SET `AIName`="SmartAI" WHERE `entry`=2621;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@GUID AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@GUID,0,0,0,1,0,100,0,0,0,10000,10000,80,1121200,0,0,0,0,0,1,0,0,0,0,0,0,0,"Hammerfall Guardian - Out of Combat - Run Script");

-- Actionlist SAI
SET @ENTRY := 1121200;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=9;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,9,0,0,0,0,100,0,2000,2000,0,0,5,1,0,0,0,0,0,10,11235,2621,0,0,0,0,0,"Hammerfall Guardian - On Script - Play Emote 1"),
(@ENTRY,9,1,0,0,0,100,0,4000,4000,0,0,5,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Hammerfall Guardian - On Script - Play Emote 1");

-- ==================================================================
-- Horde Guard - Barrens
-- Horde Guard SAI
SET @GUID := -19403;
UPDATE `creature_template` SET `AIName`="SmartAI" WHERE `entry`=3501;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@GUID AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@GUID,0,0,0,1,0,100,0,0,0,10000,10000,80,1940300,0,0,0,0,0,1,0,0,0,0,0,0,0,"Horde Guard - Out of Combat - Run Script");

-- Actionlist SAI
SET @ENTRY := 1940300;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=9;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,9,0,0,0,0,100,0,2000,2000,0,0,5,1,0,0,0,0,0,10,19428,3501,0,0,0,0,0,"Horde Guard - On Script - Play Emote 1"),
(@ENTRY,9,1,0,0,0,100,0,4000,4000,0,0,5,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Horde Guard - On Script - Play Emote 1");

-- ==================================================================
-- Peon - Barrens
-- Peon SAI
SET @GUID := -13751;
UPDATE `creature_template` SET `AIName`="SmartAI" WHERE `entry`=14901;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@GUID AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@GUID,0,0,0,1,0,100,0,0,0,10000,10000,80,1375100,0,0,0,0,0,1,0,0,0,0,0,0,0,"Peon - Out of Combat - Run Script");

-- Actionlist SAI
SET @ENTRY := 1375100;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=9;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,9,0,0,0,0,100,0,2000,2000,0,0,5,1,0,0,0,0,0,10,13772,14901,0,0,0,0,0,"Peon - On Script - Play Emote 1"),
(@ENTRY,9,1,0,0,0,100,0,4000,4000,0,0,5,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Peon - On Script - Play Emote 1");

-- Peon Emotestate script
SET @MAXGUID := 933; 
SET @SCRIPTID := 1377500; 
DELETE FROM `waypoint_scripts` WHERE `id` IN (@SCRIPTID+0);
INSERT INTO `waypoint_scripts` (`id`, `delay`, `command`, `datalong`, `datalong2`, `dataint`, `x`, `y`, `z`, `o`, `guid`) VALUES 
(@SCRIPTID+0, 3, 1, 69, 1, 0, 0, 0, 0, 0, (@MAXGUID := @MAXGUID + 1)),
(@SCRIPTID+0, 235, 1, 0, 1, 0, 0, 0, 0, 0, (@MAXGUID := @MAXGUID + 1));

SET @NPC := 13775;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,0,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`, `orientation`, `delay`, `move_type`, `action`) VALUES
(@PATH, 1, -629.313, -3161, 92.3242, 3.79019, 0, 0, 0),                                                               
(@PATH, 2, -624.911, -3156.44, 91.7832, 0.483361, 0, 0, 0),                                                               
(@PATH, 3, -617.56, -3155.6, 91.7512, 0.161653, 0, 0, 0),                                                               
(@PATH, 4, -617.56, -3155.6, 91.7512, 0.161653, 240000, 0, @SCRIPTID),                                                               
(@PATH, 5, -625.854, -3158.22, 91.9782, 3.5153, 0, 0, 0),                                                               
(@PATH, 6, -629.313, -3161, 92.3242, 3.79019, 20000, 0, 0);

UPDATE `waypoint_data` SET `action_chance`=100 WHERE `action` IN (@SCRIPTID);

-- ==================================================================
-- Westfall Woodworker - Sentinell Hill - Westfall
DELETE FROM `creature_addon` WHERE guid IN (44303,45472,45521,89860);
SET @SCTIPT_ID = 4430300;
SET @MAXGUID := 930; 
DELETE FROM `waypoint_scripts` WHERE id BETWEEN @SCTIPT_ID+0 AND @SCTIPT_ID+1;
INSERT INTO `waypoint_scripts` (`id`, `delay`, `command`, `datalong`, `datalong2`, `dataint`, `x`, `y`, `z`, `o`, `guid`) VALUES 
(@SCTIPT_ID+0, 3, 1, 69, 1, 0, 0, 0, 0, 0, (@MAXGUID := @MAXGUID + 1)),
(@SCTIPT_ID+0, 87, 1, 0, 1, 0, 0, 0, 0, 0, (@MAXGUID := @MAXGUID + 1)),
(@SCTIPT_ID+1, 3, 1, 16, 0, 0, 0, 0, 0, 0, (@MAXGUID := @MAXGUID + 1));

SET @NPC := 44303;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES 
(@NPC,@PATH,0,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`, `delay`, `action`) VALUES 
(@PATH, 1, -10662.1, 1130.83, 34.1967, 15000, 0) ,
(@PATH, 2, -10662.1, 1130.83, 34.1967, 90000, @SCTIPT_ID+0),
(@PATH, 3, -10661.3, 1113.16, 34.1967, 90000, @SCTIPT_ID+0);

SET @NPC := 45472;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,0,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`, `delay`, `action`) VALUES 
(@PATH, 1, -10650.8, 1128.61, 34.1978, 90000, @SCTIPT_ID+0),
(@PATH, 2, -10650.8, 1128.61, 34.1978, 10000, 0),
(@PATH, 3, -10658.8, 1134.62, 34.1967, 90000, @SCTIPT_ID+0);

SET @NPC := 45521;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,0,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`, `delay`, `action`) VALUES 
(@PATH, 1, -10650.1, 1115.028, 35.611, 6000, @SCTIPT_ID+1),
(@PATH, 2, -10646.4, 1113.332, 35.6268, 90000, @SCTIPT_ID+0);

SET @NPC := 89860;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,0,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`, `delay`, `action`) VALUES 
(@PATH, 1, -10656.2, 1126.87, 34.1967, 90000, @SCTIPT_ID+0),
(@PATH, 2, -10667.4, 1121.39, 34.1967, 90000, @SCTIPT_ID+0);

UPDATE `waypoint_data` SET `action_chance`=100 WHERE `action` BETWEEN @SCTIPT_ID+0 AND @SCTIPT_ID+1;

-- NPC talk text convert from creature_ai_text
DELETE FROM `creature_ai_texts` WHERE entry BETWEEN -4 AND -2;
DELETE FROM `creature_text` WHERE `creatureid` IN (103,38,61,95,116,121,122,449,474,481,504,583,589,590,594,824,881,6866,6927);
INSERT INTO `creature_text` (`creatureid`,`groupid`,`id`,`text`,`type`,`language`,`probability`,`emote`,`duration`,`sound`,`comment`) VALUES
(103,0,0, 'I see those fools at the Abbey sent some fresh meat for us.',12,0,100,0,0,0, 'Garrick Padfoot'),
(38,0,0, 'The Brotherhood will not tolerate your actions.',12,0,100,0,0,0, 'Defias Thug - Random Say on Aggro'),
(38,0,1, 'Ah, a chance to use this freshly sharpened blade.',12,0,100,0,0,0, 'Defias Thug - Random Say on Aggro'),
(38,0,2, 'Feel the power of the Brotherhood!',12,0,100,0,0,0, 'Defias Thug - Random Say on Aggro'),
(61,0,0, 'The Brotherhood will not tolerate your actions.',12,0,100,0,0,0, 'Thuros Lightfingers - Random Say on Aggro'),
(61,0,1, 'Ah, a chance to use this freshly sharpened blade.',12,0,100,0,0,0, 'Thuros Lightfingers - Random Say on Aggro'),
(61,0,2, 'Feel the power of the Brotherhood!',12,0,100,0,0,0, 'Thuros Lightfingers - Random Say on Aggro'),
(95,0,0, 'The Brotherhood will not tolerate your actions.',12,0,100,0,0,0, 'Defias Smuggler - Random Say on Aggro'),
(95,0,1, 'Ah, a chance to use this freshly sharpened blade.',12,0,100,0,0,0, 'Defias Smuggler - Random Say on Aggro'),
(95,0,2, 'Feel the power of the Brotherhood!',12,0,100,0,0,0, 'Defias Smuggler - Random Say on Aggro'),
(116,0,0, 'The Brotherhood will not tolerate your actions.',12,0,100,0,0,0, 'Defias Bandit - Random Say on Aggro'),
(116,0,1, 'Ah, a chance to use this freshly sharpened blade.',12,0,100,0,0,0, 'Defias Bandit - Random Say on Aggro'),
(116,0,2, 'Feel the power of the Brotherhood!',12,0,100,0,0,0, 'Defias Bandit - Random Say on Aggro'),
(121,0,0, 'The Brotherhood will not tolerate your actions.',12,0,100,0,0,0, 'Defias Pathstalker - Random Say on Aggro'),
(121,0,1, 'Ah, a chance to use this freshly sharpened blade.',12,0,100,0,0,0, 'Defias Pathstalker - Random Say on Aggro'),
(121,0,2, 'Feel the power of the Brotherhood!',12,0,100,0,0,0, 'Defias Pathstalker - Random Say on Aggro'),
(122,0,0, 'The Brotherhood will not tolerate your actions.',12,0,100,0,0,0, 'Defias Highwayman - Random Say on Aggro'),
(122,0,1, 'Ah, a chance to use this freshly sharpened blade.',12,0,100,0,0,0, 'Defias Highwayman - Random Say on Aggro'),
(122,0,2, 'Feel the power of the Brotherhood!',12,0,100,0,0,0, 'Defias Highwayman - Random Say on Aggro'),
(449,0,0, 'The Brotherhood will not tolerate your actions.',12,0,100,0,0,0, 'Defias Knuckleduster - Random Say on Aggro'),
(449,0,1, 'Ah, a chance to use this freshly sharpened blade.',12,0,100,0,0,0, 'Defias Knuckleduster - Random Say on Aggro'),
(449,0,2, 'Feel the power of the Brotherhood!',12,0,100,0,0,0, 'Defias Knuckleduster - Random Say on Aggro'),
(474,0,0, 'The Brotherhood will not tolerate your actions.',12,0,100,0,0,0, 'Defias Rogue Wizard - Random Say on Aggro'),
(474,0,1, 'Feel the power of the Brotherhood!',12,0,100,0,0,0, 'Defias Rogue Wizard - Random Say on Aggro'),
(481,0,0, 'The Brotherhood will not tolerate your actions.',12,0,100,0,0,0, 'Defias Footpad - Random Say on Aggro'),
(481,0,1, 'Ah, a chance to use this freshly sharpened blade.',12,0,100,0,0,0, 'Defias Footpad - Random Say on Aggro'),
(481,0,2, 'Feel the power of the Brotherhood!',12,0,100,0,0,0, 'Defias Footpad - Random Say on Aggro'),
(504,0,0, 'The Brotherhood will not tolerate your actions.',12,0,100,0,0,0, 'Defias Trapper - Random Say on Aggro'),
(504,0,1, 'Ah, a chance to use this freshly sharpened blade.',12,0,100,0,0,0, 'Defias Trapper - Random Say on Aggro'),
(504,0,2, 'Feel the power of the Brotherhood!',12,0,100,0,0,0, 'Defias Trapper - Random Say on Aggro'),
(583,0,0, 'The Brotherhood will not tolerate your actions.',12,0,100,0,0,0, 'Defias Ambusher - Random Say on Aggro'),
(583,0,1, 'Ah, a chance to use this freshly sharpened blade.',12,0,100,0,0,0, 'Defias Ambusher - Random Say on Aggro'),
(583,0,2, 'Feel the power of the Brotherhood!',12,0,100,0,0,0, 'Defias Ambusher - Random Say on Aggro'),
(589,0,0, 'The Brotherhood will not tolerate your actions.',12,0,100,0,0,0, 'Defias Pillager - Random Say on Aggro'),
(589,0,1, 'Feel the power of the Brotherhood!',12,0,100,0,0,0, 'Defias Pillager - Random Say on Aggro'),
(590,0,0, 'The Brotherhood will not tolerate your actions.',12,0,100,0,0,0, 'Defias Looter - Random Say on Aggro'),
(590,0,1, 'Ah, a chance to use this freshly sharpened blade.',12,0,100,0,0,0, 'Defias Looter - Random Say on Aggro'),
(590,0,2, 'Feel the power of the Brotherhood!',12,0,100,0,0,0, 'Defias Looter - Random Say on Aggro'),
(594,0,0, 'The Brotherhood will not tolerate your actions.',12,0,100,0,0,0, 'Defias Henchman - Random Say on Aggro'),
(594,0,1, 'Ah, a chance to use this freshly sharpened blade.',12,0,100,0,0,0, 'Defias Henchman - Random Say on Aggro'),
(594,0,2, 'Feel the power of the Brotherhood!',12,0,100,0,0,0, 'Defias Henchman - Random Say on Aggro'),
(824,0,0, 'The Brotherhood will not tolerate your actions.',12,0,100,0,0,0, 'Defias Digger - Random Say on Aggro'),
(824,0,1, 'Feel the power of the Brotherhood!',12,0,100,0,0,0, 'Defias Digger - Random Say on Aggro'),
(881,0,0, 'The Brotherhood will not tolerate your actions.',12,0,100,0,0,0, 'Surena Caledon - Random Say on Aggro'),
(881,0,1, 'Feel the power of the Brotherhood!',12,0,100,0,0,0, 'Surena Caledon - Random Say on Aggro'),
(6866,0,0, 'The Brotherhood will not tolerate your actions.',12,0,100,0,0,0, 'Defias Bodyguard - Random Say on Aggro'),
(6866,0,1, 'Ah, a chance to use this freshly sharpened blade.',12,0,100,0,0,0, 'Defias Bodyguard - Random Say on Aggro'),
(6866,0,2, 'Feel the power of the Brotherhood!',12,0,100,0,0,0, 'Defias Bodyguard - Random Say on Aggro'),
(6927,0,0, 'The Brotherhood will not tolerate your actions.',12,0,100,0,0,0, 'Defias Dockworker - Random Say on Aggro'),
(6927,0,1, 'Ah, a chance to use this freshly sharpened blade.',12,0,100,0,0,0, 'Defias Dockworker - Random Say on Aggro'),
(6927,0,2, 'Feel the power of the Brotherhood!',12,0,100,0,0,0, 'Defias Dockworker - Random Say on Aggro');

-- SAI for Garrick Padfoot
SET @ENTRY := 103;
UPDATE `creature_template` SET `AIName`='SmartAI' WHERE `entry`=@ENTRY;
DELETE FROM `creature_ai_scripts` WHERE `creature_id`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type`=0 AND `entryorguid`=@ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES 
(@ENTRY,0,0,1,4,0,100,1,0,0,0,0,11,7164,0,0,0,0,0,1,0,0,0,0,0,0,0, 'Garrick Padfoot - On Aggro - Cast Defensive Stance Spell'),
(@ENTRY,0,1,0,61,0,100,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0, 'Garrick Padfoot - Combat - Say random text');

-- SAI for Defias Thug
SET @ENTRY := 38;
UPDATE `creature_template` SET `AIName`='SmartAI' WHERE `entry`=@ENTRY;
DELETE FROM `creature_ai_scripts` WHERE `creature_id`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type`=0 AND `entryorguid`=@ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES 
(@ENTRY,0,0,0,4,0,30,1,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0, 'Defias Thug - On Aggro - Say random text');

-- SAI for Thuros Lightfingers
SET @ENTRY := 61;
UPDATE `creature_template` SET `AIName`='SmartAI' WHERE `entry`=@ENTRY;
DELETE FROM `creature_ai_scripts` WHERE `creature_id`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type`=0 AND `entryorguid`=@ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES 
(@ENTRY,0,0,0,4,0,40,1,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0, 'Thuros Lightfingers - On Aggro - Say random text');

-- SAI for Defias Smuggler
SET @ENTRY := 95;
UPDATE `creature_template` SET `AIName`='SmartAI' WHERE `entry`=@ENTRY;
DELETE FROM `creature_ai_scripts` WHERE `creature_id`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type`=0 AND `entryorguid`=@ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES 
(@ENTRY,0,0,0,11,0,100,1,0,0,0,0,0,1,10277,3500,4100,15,0,0,0,0,0,0,0,0,0, 'Defias Smuggler - On Spawn - Load Caster template Cast Throw'),
(@ENTRY,0,1,2,4,0,100,1,0,0,0,0,11,10277,0,0,0,0,0,2,0,0,0,0,0,0,0, 'Defias Smuggler - On Aggro - Cast Throw'),
(@ENTRY,0,2,0,61,0,15,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0, 'Defias Smuggler - On Aggro - Say random text'),
(@ENTRY,0,3,0,2,0,100,1,0,15,0,0,25,1,0,0,0,0,0,1,0,0,0,0,0,0,0, 'Defias Smuggler - HP@15% - Flee'),
(@ENTRY,0,4,0,0,0,100,0,1300,7300,4800,4900,11,53,0,0,0,0,0,2,0,0,0,0,0,0,0, 'Defias Smuggler - Combat - Cast BackStab');

-- SAI for Defias Bandit
SET @ENTRY := 116;
UPDATE `creature_template` SET `AIName`='SmartAI' WHERE `entry`=@ENTRY;
DELETE FROM `creature_ai_scripts` WHERE `creature_id`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type`=0 AND `entryorguid`=@ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES 
(@ENTRY,0,0,0,4,0,25,1,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0, 'Defias Bandit - On Aggro - Say random text'),
(@ENTRY,0,1,0,0,0,65,0,4000,4000,5000,9000,11,8646,0,0,0,0,0,2,0,0,0,0,0,0,0, 'Defias Bandit - Combat - Cast Snap Kick');

-- SAI for Defias Pathstalker
SET @ENTRY := 121;
UPDATE `creature_template` SET `AIName`='SmartAI' WHERE `entry`=@ENTRY;
DELETE FROM `creature_ai_scripts` WHERE `creature_id`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type`=0 AND `entryorguid`=@ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES 
(@ENTRY,0,0,0,4,0,15,1,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0, 'Defias Pathstalker - On Aggro - Say random text'),
(@ENTRY,0,1,0,0,0,65,0,1300,7300,4800,4900,11,53,0,0,0,0,0,2,0,0,0,0,0,0,0, 'Defias Pathstalker - Combat - Cast Backstab'),
(@ENTRY,0,2,0,0,0,100,0,1500,13600,25200,39100,11,11972,0,0,0,0,0,2,0,0,0,0,0,0,0, 'Defias Pathstalker - Combat - Cast Shieldbash'),
(@ENTRY,0,3,0,2,0,100,1,0,15,0,0,25,1,0,0,0,0,0,1,0,0,0,0,0,0,0, 'Defias Pathstalker - HP@15% - Flee');

-- SAI for Defias Highwayman
SET @ENTRY := 122;
UPDATE `creature_template` SET `AIName`='SmartAI' WHERE `entry`=@ENTRY;
DELETE FROM `creature_ai_scripts` WHERE `creature_id`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type`=0 AND `entryorguid`=@ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES 
(@ENTRY,0,0,0,4,0,15,1,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0, 'Defias Highwayman - On Aggro - Say random text'),
(@ENTRY,0,1,0,0,0,100,0,1000,3000,1200,3600,11,53,0,0,0,0,0,2,0,0,0,0,0,0,0, 'Defias Highwayman - Combat - Cast Backstab'),
(@ENTRY,0,2,0,2,0,100,1,0,15,0,0,25,1,0,0,0,0,0,1,0,0,0,0,0,0,0, 'Defias Highwayman - HP@15% - Flee');

-- SAI for Defias Knuckleduster
SET @ENTRY := 449;
UPDATE `creature_template` SET `AIName`='SmartAI' WHERE `entry`=@ENTRY;
DELETE FROM `creature_ai_scripts` WHERE `creature_id`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type`=0 AND `entryorguid`=@ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES 
(@ENTRY,0,0,0,4,0,5,1,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0, 'Defias Knuckleduster - On Aggro - Say random text'),
(@ENTRY,0,1,0,0,0,100,0,1400,2700,7300,15000,11,12555,0,0,0,0,0,2,0,0,0,0,0,0,0, 'Defias Knuckleduster - Combat - Cast Pummel'),
(@ENTRY,0,2,0,0,0,40,0,2100,4300,3900,4300,11,3391,0,0,0,0,0,1,0,0,0,0,0,0,0, 'Defias Knuckleduster - Combat - Cast Thrash'),
(@ENTRY,0,3,0,2,0,100,1,0,15,0,0,25,1,0,0,0,0,0,1,0,0,0,0,0,0,0, 'Defias Knuckleduster - HP@15% - Flee');

-- SAI for Defias Rogue Wizard
SET @ENTRY := 474;
UPDATE `creature_template` SET `AIName`='SmartAI' WHERE `entry`=@ENTRY;
DELETE FROM `creature_ai_scripts` WHERE `creature_id`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type`=0 AND `entryorguid`=@ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES 
(@ENTRY,0,0,0,11,0,100,1,0,0,0,0,0,1,13322,1500,3000,5,30,0,0,0,0,0,0,0,0, 'Defias Rogue Wizard - On Spawn - Load Caster template Cast Frostbolt'),
(@ENTRY,0,1,0,1,0,100,0,1000,1000,1800000,1800000,11,12544,0,0,0,0,0,1,0,0,0,0,0,0,0, 'Defias Rogue Wizard - OOC - Cast Frost Armor'),
(@ENTRY,0,2,0,4,0,15,1,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0, 'Defias Rogue Wizard - On Aggro - Say random text');

-- SAI for Defias Footpad
SET @ENTRY := 481;
UPDATE `creature_template` SET `AIName`='SmartAI' WHERE `entry`=@ENTRY;
DELETE FROM `creature_ai_scripts` WHERE `creature_id`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type`=0 AND `entryorguid`=@ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES 
(@ENTRY,0,0,0,4,0,5,1,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0, 'Defias Footpad - On Aggro - Say random text'),
(@ENTRY,0,1,0,0,0,100,0,5900,10200,8200,14100,11,7159,0,0,0,0,0,2,0,0,0,0,0,0,0, 'Defias Footpad - Combat - Cast Backstab'),
(@ENTRY,0,2,0,2,0,100,1,0,15,0,0,25,1,0,0,0,0,0,1,0,0,0,0,0,0,0, 'Defias Footpad - HP@15% - Flee');

-- SAI for Defias Trapper
SET @ENTRY := 504;
UPDATE `creature_template` SET `AIName`='SmartAI' WHERE `entry`=@ENTRY;
DELETE FROM `creature_ai_scripts` WHERE `creature_id`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type`=0 AND `entryorguid`=@ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES 
(@ENTRY,0,0,0,4,0,5,1,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0, 'Defias Trapper - On Aggro - Say random text'),
(@ENTRY,0,1,0,0,0,100,0,2300,4700,2400,7300,11,2589,0,0,0,0,0,2,0,0,0,0,0,0,0, 'Defias Trapper - Combat - Cast Backstab'),
(@ENTRY,0,2,0,0,0,100,0,6500,14200,20500,31100,11,12024,0,0,0,0,0,2,0,0,0,0,0,0,0, 'Defias Trapper - Combat - Cast Net'),
(@ENTRY,0,3,0,2,0,100,1,0,15,0,0,25,1,0,0,0,0,0,1,0,0,0,0,0,0,0, 'Defias Trapper - HP@15% - Flee');

-- SAI for Defias Ambusher
SET @ENTRY := 583;
UPDATE `creature_template` SET `AIName`='SmartAI' WHERE `entry`=@ENTRY;
DELETE FROM `creature_ai_scripts` WHERE `creature_id`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type`=0 AND `entryorguid`=@ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES 
(@ENTRY,0,0,0,4,0,30,1,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0, 'Defias Ambusher - On Aggro - Say random text');

-- SAI for Defias Pillager
SET @ENTRY := 589;
UPDATE `creature_template` SET `AIName`='SmartAI' WHERE `entry`=@ENTRY;
DELETE FROM `creature_ai_scripts` WHERE `creature_id`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type`=0 AND `entryorguid`=@ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES 
(@ENTRY,0,0,0,11,0,100,1,0,0,0,0,0,1,20793,3400,5400,5,30,0,0,0,0,0,0,0,0, 'Defias Pillager - On Spawn - Load Caster template Cast Fireball'),
(@ENTRY,0,1,0,1,0,100,0,1000,1000,1800000,1800000,11,12544,0,0,0,0,0,1,0,0,0,0,0,0,0, 'Defias Pillager - OOC - Cast Frost Armor'),
(@ENTRY,0,2,3,4,0,100,1,0,0,0,0,11,20793,0,0,0,0,0,2,0,0,0,0,0,0,0, 'Defias Pillager - On Aggro - Cast Fireball'),
(@ENTRY,0,3,0,61,0,15,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0, 'Defias Pillager - On Aggro - Say random text'),
(@ENTRY,0,4,0,2,0,100,1,0,15,0,0,25,1,0,0,0,0,0,1,0,0,0,0,0,0,0, 'Defias Pillager - HP@15% - Flee');

-- SAI for Defias Looter
SET @ENTRY := 590;
UPDATE `creature_template` SET `AIName`='SmartAI' WHERE `entry`=@ENTRY;
DELETE FROM `creature_ai_scripts` WHERE `creature_id`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type`=0 AND `entryorguid`=@ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES 
(@ENTRY,0,0,0,4,0,15,1,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0, 'Defias Looter - On Aggro - Say random text'),
(@ENTRY,0,1,0,0,0,100,0,3000,4500,2000,9000,11,7159,0,0,0,0,0,2,0,0,0,0,0,0,0, 'Defias Looter - Combat - Cast Backstab'),
(@ENTRY,0,2,0,0,0,80,0,2000,6000,38100,48200,11,6713,0,0,0,0,0,2,0,0,0,0,0,0,0, 'Defias Looter - Combat - Cast Disarm'),
(@ENTRY,0,3,0,2,0,100,1,0,15,0,0,25,1,0,0,0,0,0,1,0,0,0,0,0,0,0, 'Defias Looter - HP@15% - Flee');

-- SAI for Defias Henchman
SET @ENTRY := 594;
UPDATE `creature_template` SET `AIName`='SmartAI' WHERE `entry`=@ENTRY;
DELETE FROM `creature_ai_scripts` WHERE `creature_id`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type`=0 AND `entryorguid`=@ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES 
(@ENTRY,0,0,0,4,0,15,1,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0, 'Defias Henchman - On Aggro - Say random text'),
(@ENTRY,0,1,0,13,0,100,1,3200,13800,0,0,11,8242,0,0,0,0,0,2,0,0,0,0,0,0,0, 'Defias Henchman - Combat - Cast Shield Slam'),
(@ENTRY,0,2,0,0,0,100,1,3600,18800,0,0,11,3248,0,0,0,0,0,1,0,0,0,0,0,0,0, 'Defias Henchman - Combat - Cast Improved Blocking'),
(@ENTRY,0,3,0,2,0,100,1,0,15,0,0,25,1,0,0,0,0,0,1,0,0,0,0,0,0,0, 'Defias Henchman - HP@15% - Flee');

-- SAI for Defias Digger
SET @ENTRY := 824;
UPDATE `creature_template` SET `AIName`='SmartAI' WHERE `entry`=@ENTRY;
DELETE FROM `creature_ai_scripts` WHERE `creature_id`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type`=0 AND `entryorguid`=@ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES 
(@ENTRY,0,0,0,4,0,15,1,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0, 'Defias Digger - On Aggro - Say random text'),
(@ENTRY,0,1,0,2,0,100,1,0,15,0,0,25,1,0,0,0,0,0,1,0,0,0,0,0,0,0, 'Defias Digger - HP@15% - Flee');

-- SAI for Surena Caledon
SET @ENTRY := 881;
UPDATE `creature_template` SET `AIName`='SmartAI' WHERE `entry`=@ENTRY;
DELETE FROM `creature_ai_scripts` WHERE `creature_id`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type`=0 AND `entryorguid`=@ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES 
(@ENTRY,0,0,0,11,0,100,1,0,0,0,0,0,1,20793,1500,3000,5,30,0,0,0,0,0,0,0,0, 'Surena Caledon - On Spawn - Load Caster template Cast Fireball'),
(@ENTRY,0,1,0,1,0,100,0,1000,1000,1800000,1800000,11,12544,0,0,0,0,0,1,0,0,0,0,0,0,0, 'Surena Caledon - OOC - Cast Frost Armor'),
(@ENTRY,0,2,0,4,0,40,1,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0, 'Surena Caledon - On Aggro - Say random text');

-- SAI for Defias Bodyguard
SET @ENTRY := 6866;
UPDATE `creature_template` SET `AIName`='SmartAI' WHERE `entry`=@ENTRY;
DELETE FROM `creature_ai_scripts` WHERE `creature_id`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type`=0 AND `entryorguid`=@ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES 
(@ENTRY,0,0,0,4,0,40,1,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0, 'Defias Bodyguard - On Aggro - Say random text'),
(@ENTRY,0,1,0,0,0,70,0,2900,2900,34000,38000,11,6713,0,0,0,0,0,2,0,0,0,0,0,0,0, 'Defias Bodyguard - Combat - Cast Disarm'),
(@ENTRY,0,2,0,0,0,80,0,5500,5500,5900,12900,11,7159,0,0,0,0,0,2,0,0,0,0,0,0,0, 'Defias Bodyguard - Combat - Cast Backstab');

-- SAI for Defias Dockworker
SET @ENTRY := 6927;
UPDATE `creature_template` SET `AIName`='SmartAI' WHERE `entry`=@ENTRY;
DELETE FROM `creature_ai_scripts` WHERE `creature_id`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type`=0 AND `entryorguid`=@ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES 
(@ENTRY,0,0,0,4,0,30,1,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0, 'Defias Dockworker - On Aggro - Say random text'),
(@ENTRY,0,1,0,0,0,100,0,8500,10500,37100,42100,11,8646,0,0,0,0,0,2,0,0,0,0,0,0,0, 'Defias Dockworker - Combat - Cast Snapkick');

-- SCRIPT 1
SET @MAXGUID := 919; 
SET @SCRIPTID := 8014900; 
DELETE FROM `waypoint_scripts` WHERE `id` IN (@SCRIPTID+0);
INSERT INTO `waypoint_scripts` (`id`, `delay`, `command`, `datalong`, `datalong2`, `dataint`, `x`, `y`, `z`, `o`, `guid`) VALUES 
(@SCRIPTID+0, 1, 35, 1, 5, 0, 0, 0, 0, 0, (@MAXGUID := @MAXGUID + 1)),
(@SCRIPTID+0, 20, 35, 2, 0, 801490, 0, 0, 0, 0, (@MAXGUID := @MAXGUID + 1));

SET @NPC := 80149;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,0,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`, `orientation`, `delay`, `move_type`, `action`) VALUES
(@PATH, 1, -9008.89, -320.603, 75.8279, 2.8812, 1000, 1, 0),                                                               
(@PATH, 2, -8981.22, -335.138, 73.3474, 5.82645, 0, 1, 0),                                                               
(@PATH, 3, -8946.51, -338.891, 71.1134, 5.82409, 0, 1, 0),                                                               
(@PATH, 4, -8912.77, -352.085, 72.5823, 5.88143, 0, 1, 0),                                                               
(@PATH, 5, -8881.49, -355.84, 73.1462, 6.17595, 0, 1, 0),                                                               
(@PATH, 6, -8910.65, -346.602, 71.1023, 2.81837, 0, 0, 0),                                                               
(@PATH, 7, -8883.13, -352.739, 72.9499, 2.75397, 0, 0, 0),                                                               
(@PATH, 8, -8911.38, -347.166, 71.3269, 2.95582, 0, 0, 0),                                                               
(@PATH, 9, -8947.63, -337.566, 70.9275, 2.87728, 0, 0, 0),                                                               
(@PATH, 10, -9008.89, -320.603, 75.8279, 2.8812, 25000, 0, @SCRIPTID);

UPDATE `waypoint_data` SET `action_chance`=100 WHERE `action` IN (@SCRIPTID);

-- SCRIPT 2
-- Defias Thug SAI
SET @GUID := -80152;
UPDATE `creature_template` SET `AIName`="SmartAI" WHERE `entry`=38;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@GUID AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@GUID,0,0,0,1,0,100,0,0,0,10000,10000,80,8015200,0,0,0,0,0,1,0,0,0,0,0,0,0,"Defias Thug - Out of Combat - Run Script"),
(@GUID,0,1,0,4,0,30,1,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,"Defias Thug - On Aggro - Say Line 0 (No Repeat)");

-- Actionlist SAI
SET @ENTRY := 8015200;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=9;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,9,0,0,0,0,100,0,2000,2000,0,0,5,1,0,0,0,0,0,10,80151,38,0,0,0,0,0,"Defias Thug - On Script - Play Emote 1"),
(@ENTRY,9,1,0,0,0,100,0,4000,4000,0,0,5,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Defias Thug - On Script - Play Emote 1");

-- SCRIPT 3
SET @NPC := 80171;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,0,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`, `orientation`, `delay`, `move_type`) VALUES
(@PATH, 1, -9120.29, -275.579, 72.9006, 1.03043, 2000, 1),                                                               
(@PATH, 2, -9112.23, -263.542, 74.4268, 1.06342, 3000, 0),                                                               
(@PATH, 3, -9110.18, -259.698, 74.7637, 3.91441, 0, 1),                                                               
(@PATH, 4, -9120.29, -269.848, 73.5204, 3.94322, 4000, 0);

-- SCRIPT 4
SET @NPC := 80181;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,0,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`, `orientation`, `delay`, `move_type`) VALUES
(@PATH, 1, -9131.36, -306.832, 73.5301, 4.49767, 0, 1),                                                               
(@PATH, 2, -9132.83, -313.89, 73.306, 4.49767, 4000, 1),                                                               
(@PATH, 3, -9124.91, -379.208, 73.2856, 4.81183, 3000, 0),                                                               
(@PATH, 4, -9125.01, -372.703, 73.5785, 1.77234, 0, 1),                                                               
(@PATH, 5, -9138.42, -339.389, 72.5674, 1.2202, 0, 1),                                                               
(@PATH, 6, -9123.32, -311.799, 72.985, 1.2202, 4000, 0);

-- SCRIPT 5
-- Defias Thug SAI
SET @GUID := -80184;
UPDATE `creature_template` SET `AIName`="SmartAI" WHERE `entry`=38;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@GUID AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@GUID,0,0,0,1,0,100,0,0,0,8000,8000,80,8018400,0,0,0,0,0,1,0,0,0,0,0,0,0,"Defias Thug - Out of Combat - Run Script"),
(@GUID,0,1,0,4,0,30,1,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,"Defias Thug - On Aggro - Say Line 0 (No Repeat)");

-- Actionlist SAI
SET @ENTRY := 8018400;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=9;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,9,0,0,0,0,100,0,2000,2000,0,0,5,1,0,0,0,0,0,10,80185,38,0,0,0,0,0,"Defias Thug - On Script - Play Emote 1"),
(@ENTRY,9,1,0,0,0,100,0,4000,4000,0,0,5,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Defias Thug - On Script - Play Emote 1");

-- SCRIPT 6
-- Defias Thug SAI
SET @GUID := -80188;
UPDATE `creature_template` SET `AIName`="SmartAI" WHERE `entry`=38;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@GUID AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@GUID,0,0,0,1,0,100,0,0,0,8000,8000,80,8018800,0,0,0,0,0,1,0,0,0,0,0,0,0,"Defias Thug - Out of Combat - Run Script"),
(@GUID,0,1,0,4,0,30,1,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,"Defias Thug - On Aggro - Say Line 0 (No Repeat)");

-- Actionlist SAI
SET @ENTRY := 8018800;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=9;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,9,0,0,0,0,100,0,2000,2000,0,0,5,1,0,0,0,0,0,10,80189,38,0,0,0,0,0,"Defias Thug - On Script - Play Emote 1"),
(@ENTRY,9,1,0,0,0,100,0,4000,4000,0,0,5,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Defias Thug - On Script - Play Emote 1");

-- SCRIPT 7
-- Defias Thug SAI
SET @GUID := -80201;
UPDATE `creature_template` SET `AIName`="SmartAI" WHERE `entry`=38;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@GUID AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@GUID,0,0,0,1,0,100,0,0,0,8000,8000,80,8020100,0,0,0,0,0,1,0,0,0,0,0,0,0,"Defias Thug - Out of Combat - Run Script"),
(@GUID,0,1,0,4,0,30,1,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,"Defias Thug - On Aggro - Say Line 0 (No Repeat)");

-- Actionlist SAI
SET @ENTRY := 8020100;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=9;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,9,0,0,0,0,100,0,2000,2000,0,0,5,1,0,0,0,0,0,10,80200,38,0,0,0,0,0,"Defias Thug - On Script - Play Emote 1"),
(@ENTRY,9,1,0,0,0,100,0,4000,4000,0,0,5,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Defias Thug - On Script - Play Emote 1");

-- SCRIPT 8
SET @MAXGUID := 921; 
SET @SCRIPTID := 8025100; 
DELETE FROM `waypoint_scripts` WHERE `id` IN (@SCRIPTID+0);
INSERT INTO `waypoint_scripts` (`id`, `delay`, `command`, `datalong`, `datalong2`, `dataint`, `x`, `y`, `z`, `o`, `guid`) VALUES 
(@SCRIPTID+0, 1, 35, 1, 5, 0, 0, 0, 0, 0, (@MAXGUID := @MAXGUID + 1)),
(@SCRIPTID+0, 20, 35, 2, 0, 802510, 0, 0, 0, 0, (@MAXGUID := @MAXGUID + 1));

SET @NPC := 80251;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,0,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`, `orientation`, `delay`, `move_type`, `action`) VALUES
(@PATH, 1, -8878.29, -410.994, 65.6802, 4.63836, 1000, 1, 0),                                                               
(@PATH, 2, -8880.02, -399.363, 66.0983, 2.83464, 0, 1, 0),                                                               
(@PATH, 3, -8898.18, -391.582, 68.6285, 3.16417, 0, 1, 0),                                                               
(@PATH, 4, -8914.49, -391.059, 69.3006, 2.22326, 0, 1, 0),                                                               
(@PATH, 5, -8928.27, -375.636, 71.218, 3.22857, 0, 1, 0),                                                               
(@PATH, 6, -8958.87, -373.826, 72.3354, 3.34245, 5000, 1, 0),                                                               
(@PATH, 7, -8921.43, -376.858, 71.1848, 0.534655, 0, 1, 0),                                                               
(@PATH, 8, -8909.08, -366.763, 72.135, 6.05679, 0, 1, 0),                                                               
(@PATH, 9, -8870.04, -371.407, 71.997, 4.63836, 0, 1, 0),                                                               
(@PATH, 10, -8878.29, -410.994, 65.6802, 4.63836, 0, 1, 0),                                                               
(@PATH, 11, -8878.29, -410.994, 65.6802, 4.63836, 25000, 1, @SCRIPTID);

UPDATE `waypoint_data` SET `action_chance`=100 WHERE `action` IN (@SCRIPTID);
