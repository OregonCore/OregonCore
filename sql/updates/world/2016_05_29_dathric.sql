-- Severed Spirit
SET @ENTRY := 19881;
SET @SOURCETYPE := 0;

DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=@SOURCETYPE;
UPDATE creature_template SET AIName="SmartAI" WHERE entry=@ENTRY LIMIT 1;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES 
(@ENTRY,@SOURCETYPE,0,0,0,0,100,0,0,0,0,0,11,20822,64,0,0,0,0,2,0,0,0,0.0,0.0,0.0,0.0,"Severed Spirit - Cast Frostbolt"),
(@ENTRY,@SOURCETYPE,1,0,9,0,100,0,0,8,11000,15000,11,11831,1,0,0,0,0,2,0,0,0,0.0,0.0,0.0,0.0,"Severed Spirit - Cast Frost Nova"),
(@ENTRY,@SOURCETYPE,2,0,25,0,100,0,0,0,0,0,21,1,0,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,"Severed Spirit - Enable Combat Movement on Reset"),
(@ENTRY,@SOURCETYPE,3,0,2,0,100,1,0,15,0,0,25,1,0,0,0,0,0,0,0,0,0,0.0,0.0,0.0,0.0,"Severed Spirit - Flee at 15% hp");

DELETE FROM creature_ai_scripts WHERE creature_id=@ENTRY;

-- Fix objective text
UPDATE quest_template SET ObjectiveText1="Put Dathric's Spirit to Rest", ObjectiveText2="" WHERE entry=10182;

-- Battle-Mage Dathric Actionlist
SET @ENTRY := 1954300;
SET @SOURCETYPE := 9;

DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=@SOURCETYPE;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES 
(@ENTRY,@SOURCETYPE,0,0,0,0,100,0,0,0,0,0,2,35,0,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,"Battle-Mage Dathric - Set Friendly at 0.0"),
(@ENTRY,@SOURCETYPE,1,0,0,0,100,0,1500,1500,0,0,53,0,195430,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,"Battle-Mage Dathric - Walk to weapon rack at 1.5"),
(@ENTRY,@SOURCETYPE,2,0,0,0,100,0,4000,4000,0,0,5,69,0,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,"Battle-Mage Dathric - Use weapon rack at 5.5"),
(@ENTRY,@SOURCETYPE,3,0,0,0,100,0,3000,3000,0,0,5,0,0,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,"Battle-Mage Dathric - Play default emote at 8.5"),
(@ENTRY,@SOURCETYPE,4,0,0,0,100,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,"Battle-Mage Dathric - Talk at 8.5");

-- Battle-Mage Dathric
SET @ENTRY := 19543;
SET @SOURCETYPE := 0;

DELETE FROM creature_ai_scripts WHERE creature_id=@ENTRY;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=@SOURCETYPE;
UPDATE creature_template SET AIName="SmartAI" WHERE entry=@ENTRY LIMIT 1;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES 
(@ENTRY,@SOURCETYPE,0,0,9,0,100,0,0,5,12000,16000,11,36104,0,0,0,0,0,2,0,0,0,0.0,0.0,0.0,0.0,"Battle-Mage Dathric - Cast Torrent of Flames"),
(@ENTRY,@SOURCETYPE,1,0,0,0,100,0,9000,13000,19000,24000,11,17273,4,0,0,0,0,2,0,0,0,0.0,0.0,0.0,0.0,"Battle-Mage Dathric - Cast Pyroblast"),
(@ENTRY,@SOURCETYPE,2,0,54,0,100,0,0,0,0,0,80,1954300,0,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,"Battle-Mage Dathric - Call actionlist on spawn");

DELETE FROM `creature_text` WHERE `entry`=@ENTRY;
INSERT INTO `creature_text` (`entry`,`groupid`,`id`,`text`,`type`,`language`,`probability`,`emote`,`duration`,`sound`,`comment`) VALUES
(@ENTRY,0,0,'I don''t know what I was thinking, going out without my sword. I would''ve put it on if I''d seen it here...',12,0,100,1,0,0,'Battle-Mage Dathric at weapon rack');

-- Spell has SendEvent effect, give credit and summon Battle-Mage Dathric on use
DELETE FROM `event_scripts` WHERE `id`=12608;
INSERT INTO `event_scripts` (`id`,`delay`,`command`,`datalong`,`datalong2`,`x`,`y`,`z`,`o`) VALUES
(12608,0,8,19549,0,0,0,0,0),
(12608,0,10,@ENTRY,15000,2235.483643,2319.726563,92.239876,3.910929);

-- Waypoints
SET @Waypoint := 195430;
DELETE FROM `waypoints` WHERE `entry`=@Waypoint;
INSERT INTO `waypoints` (`entry`,`pointid`,`position_x`,`position_y`,`position_z`,`point_comment`) VALUES
(@Waypoint, 1, 2231.676514, 2317.575439, 91.115738, 'Battle-Mage Dathric weapon rack path'),
(@Waypoint, 2, 2229.051514, 2314.387939, 89.892456, 'Battle-Mage Dathric weapon rack path');
