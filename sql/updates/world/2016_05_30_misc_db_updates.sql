-- Cabal Ritualists should cast Crystal Channel
DELETE FROM `smart_scripts` WHERE `entryorguid`=18794 AND `id`=34;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(18794,0,34,0,1,0,100,0,0,0,0,0,11,32958,0,0,0,0,0,1,0,0,0,0,0,0,0,"Cabal Ritualist - Out of Combat - Cast 'Crystal Channel'");

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=13 AND `SourceGroup`=1 AND `SourceEntry`=32958;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`,`SourceGroup`,`SourceEntry`,`SourceId`,`ElseGroup`,`ConditionTypeOrReference`,`ConditionTarget`,`ConditionValue1`,`ConditionValue2`,`ConditionValue3`,`ErrorTextId`,`ScriptName`,`Comment`) VALUES
(13,1,32958,0,0,31,0,3,18793,0,0,"","Crystal Channel – Effect 0 - Cast only on Invisible Target 18793.");

-- Anvilrage Overseer SAI
UPDATE `creature_template` SET `AIName`='SmartAI' WHERE  `entry`=8889;
DELETE FROM `smart_scripts` WHERE `entryorguid`=8889 AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(8889, 0, 0, 0, 0, 0, 100, 2, 4000, 6000, 7000, 9000, 11, 15580, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Anvilrage Overseer - In Combat - Cast \'Strike\' (Normal Dungeon)');

-- DB/Creature: Add missing Ornery Plainstrider SAI
UPDATE `creature_template` SET `AIName`='SmartAI' WHERE  `entry`=3245;
DELETE FROM `smart_scripts` WHERE `entryorguid`=3245 AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(3245, 0, 0, 0, 2, 0, 100, 1, 0, 40, 0, 0, 11, 3019, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Ornery Plainstrider - Between 0-40% Health - Cast \'Frenzy\' (No Repeat)');

-- Mebok Mizzyrix, NPC entry 3446
SET @ENTRY := 3446;

DELETE FROM `creature_text` WHERE `entry`= @ENTRY;
INSERT INTO `creature_text` (`entry`,`groupid`,`id`,`text`,`type`,`language`,`probability`,`emote`,`duration`,`sound`,`TextRange`,`comment`) VALUES
(@ENTRY,0,0,"Now let's try it...",   12,0,100, 7,0,0,0,'Mebok Mizzyrix on Quest 865 rewarded'),
(@ENTRY,1,0,"Ugh!  That's terrible!",12,0,100,33,0,0,0,'Mebok Mizzyrix on Quest 865 rewarded');

UPDATE `creature_template` SET `AIName` = 'SmartAI' WHERE `entry`= @ENTRY;

DELETE FROM `smart_scripts` WHERE `source_type`= 0 AND `entryorguid`= @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,0,0,1,20,0,100,0,865,0,0,0,1,0,4000,0,0,0,0,1,0,0,0,0,0,0,0,'Mebok Mizzyrix - on Quest 865 Rewarded - Say Line 0'),
(@ENTRY,0,1,0,61,0,100,0,  0,0,0,0,1,1,4000,0,0,0,0,1,0,0,0,0,0,0,0,'Mebok Mizzyrix - on Quest 865 Rewarded - Say Line 1');

-- Add exclusive group for breadcrumb Mudsprocket, Dustwallow Marsh quests
UPDATE `quest_template` SET `ExclusiveGroup` = 11211 WHERE `entry` IN (11211,11214,11215);

-- fix wrong questgivers for the "beached animal bones" quests in darkshore
DELETE FROM `gameobject_involvedrelation` WHERE `quest` IN (4722,4723,4725,4727,4728,4730,4731,4732,4733);
INSERT INTO `gameobject_involvedrelation` VALUES
(176190, 4722),
(175233, 4723),
(176197, 4725),
(176196, 4727),
(175226, 4728),
(175227, 4730),
(176198, 4731),
(176191, 4732),
(175230, 4733);

-- Add frost immunity to Crashing Wave-Spirits
UPDATE creature_template SET resistance4 = -1 WHERE entry = 22309;

-- Add fire immunity to Incandescent Fel Sparks
UPDATE creature_template SET resistance2 = -1 WHERE entry = 22323;

-- DB/Creature: Add flag guard Deathguard Elite
-- creature is a guard (Will ignore feign death and vanish)
UPDATE `creature_template` SET `flags_extra`=32768 WHERE `entry`=7980;
 
-- Pathing for Houndmaster Grebmar Entry: 9319
SET @NPC := 46284;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=594.4585,`position_y`=-178.3237,`position_z`=-84.23994 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,0,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,594.4585,-178.3237,-84.23994,0,0,0,0,100,0),
(@PATH,2,598.2085,-180.3237,-84.23994,0,0,0,0,100,0),
(@PATH,3,601.2085,-182.3237,-84.23994,0,0,0,0,100,0),
(@PATH,4,605.1209,-184.6363,-84.23941,0,0,0,0,100,0),
(@PATH,5,608.8709,-188.1363,-84.23941,0,0,0,0,100,0),
(@PATH,6,612.06,-191.0042,-84.23634,0,0,0,0,100,0),
(@PATH,7,611.81,-184.0042,-84.23634,0,0,0,0,100,0),
(@PATH,8,611.5181,-178.7817,-84.23982,0,0,0,0,100,0),
(@PATH,9,606.7681,-173.7817,-84.23982,0,0,0,0,100,0),
(@PATH,10,601.315,-167.8244,-84.23912,0,0,0,0,100,0),
(@PATH,11,594.565,-168.8244,-84.23912,0,0,0,0,100,0),
(@PATH,12,585.8881,-169.7204,-84.24162,0,0,0,0,100,0),
(@PATH,13,582.8881,-176.2204,-84.24162,0,0,0,0,100,0),
(@PATH,14,579.5485,-182.8368,-84.24379,0,0,0,0,100,0),
(@PATH,15,583.2985,-189.3368,-84.24379,0,0,0,0,100,0),
(@PATH,16,586.8497,-196.4224,-84.24238,0,0,0,0,100,0),
(@PATH,17,587.5997,-187.1724,-84.24238,0,0,0,0,100,0),
(@PATH,18,591.6155,-181.6407,-84.24092,0,0,0,0,100,0),
(@PATH,19,594.4585,-178.3237,-84.23994,0,0,0,0,100,0);