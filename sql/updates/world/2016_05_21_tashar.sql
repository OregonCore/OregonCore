-- Tashar Actionlist
SET @ENTRY := 2091300;
SET @SOURCETYPE := 9;

DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=@SOURCETYPE;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES 
(@ENTRY,@SOURCETYPE,0,0,0,0,100,0,0,0,0,0,83,2,0,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,"Tashar - Remove quest giver status at 0.0"),
(@ENTRY,@SOURCETYPE,2,0,0,0,100,0,500,500,0,0,1,0,0,0,0,0,0,0,0,0,0,0.0,0.0,0.0,0.0,"Tashar - Talk at 0.5"),
(@ENTRY,@SOURCETYPE,1,0,0,0,100,0,0,0,0,0,59,0,0,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,"Tashar - Set walking at 0.0"),
(@ENTRY,@SOURCETYPE,3,0,0,0,100,0,2500,2500,0,0,69,0,0,0,0,0,0,8,0,0,0,4922.22,2991.02,93.9993,6.096,"Tashar - Walk to dome generator at 3"),
(@ENTRY,@SOURCETYPE,4,0,0,0,100,0,3000,3000,0,0,5,28,0,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,"Tashar - Work animation at 6"),
(@ENTRY,@SOURCETYPE,5,0,0,0,100,0,2500,2500,0,0,11,28730,2,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,"Tashar - Cast arcane torrent at 8.5"),
(@ENTRY,@SOURCETYPE,6,0,0,0,100,0,2500,2500,0,0,1,1,0,0,0,0,0,0,0,0,0,0.0,0.0,0.0,0.0,"Tashar - Talk at 11"),
(@ENTRY,@SOURCETYPE,7,0,0,0,100,0,4000,4000,0,0,69,0,0,0,0,0,0,8,0,0,0,4914.74,2991.92,94.1138,1.5822,"Tashar - Walk back home at 15"),
(@ENTRY,@SOURCETYPE,8,0,0,0,100,0,4000,4000,0,0,82,2,0,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,"Tashar - Enable quest giving at 19"),
(@ENTRY,@SOURCETYPE,9,0,0,0,100,0,0,0,0,0,66,0,0,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,"Tashar - Restore orientation at 19");

-- Tashar
SET @ENTRY := 20913;
SET @SOURCETYPE := 0;

DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=@SOURCETYPE;
UPDATE creature_template SET AIName="SmartAI" WHERE entry=@ENTRY LIMIT 1;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES 
(@ENTRY,@SOURCETYPE,1,0,25,0,100,0,0,0,0,0,82,2,0,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,"Tashar - On Reset - Enable quest giving"),
(@ENTRY,@SOURCETYPE,0,0,20,0,100,0,10436,0,0,0,80,2091300,0,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,"Tashar - On Quest Complete - Call Actionlist");

-- Tashar dialog lines
DELETE FROM `creature_text` WHERE `entry`=@ENTRY;
INSERT INTO `creature_text` (`entry`,`groupid`,`id`,`text`,`type`,`language`,`probability`,`emote`,`duration`,`sound`,`comment`) VALUES
(@ENTRY,0,0,'Now to begin the repairs.',12,0,100,1,0,0,'Tashar begin repairs'),
(@ENTRY,1,0,'It''s working! Ghabar will be pleased to hear the news.',12,0,100,1,0,0,'Tashar finished repairs');
