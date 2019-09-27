-- Netherstorm Trigger at The Violet Tower
SET @ENTRY := 19656;
SET @SOURCETYPE := 0;

UPDATE creature_template SET AIName="SmartAI", InhabitType=7 WHERE entry=@ENTRY LIMIT 1;

-- Beam channelers
SET @SPELL := 38034;

SET @GUID := -70107;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@GUID AND `source_type`=@SOURCETYPE;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES 
(@GUID,@SOURCETYPE,0,0,1,0,100,0,0,0,10000,10000,11,@SPELL,2,0,0,0,0,10,70108,0,0,0.0,0.0,0.0,0.0,"Netherstorm Trigger (The Violet Tower) - Cast Blue Beam");

SET @GUID := -70108;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@GUID AND `source_type`=@SOURCETYPE;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES 
(@GUID,@SOURCETYPE,0,0,1,0,100,0,0,0,10000,10000,11,@SPELL,2,0,0,0,0,10,70111,0,0,0.0,0.0,0.0,0.0,"Netherstorm Trigger (The Violet Tower) - Cast Blue Beam");

SET @GUID := -70109;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@GUID AND `source_type`=@SOURCETYPE;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES 
(@GUID,@SOURCETYPE,0,0,1,0,100,0,0,0,10000,10000,11,@SPELL,2,0,0,0,0,10,70111,0,0,0.0,0.0,0.0,0.0,"Netherstorm Trigger (The Violet Tower) - Cast Blue Beam");

-- Curse of the Violet Tower caster
SET @GUID := -70110;
SET @SOURCETYPE := 0;

UPDATE creature SET position_x=2253.580322, position_y=2257.739258, position_z=101.576141 WHERE guid=70110;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@GUID AND `source_type`=@SOURCETYPE;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES 
(@GUID,@SOURCETYPE,0,0,10,0,100,0,0,8,0,0,75,34102,7,0,0,0,0,18,8,0,0,0.0,0.0,0.0,0.0,"Netherstorm Trigger (The Violet Tower) - OOC LoS - Add Curse of the Violet Tower Aura"),
(@GUID,@SOURCETYPE,1,0,37,0,100,0,0,0,0,0,8,2,0,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,"Netherstorm Trigger (The Violet Tower) - On AI Init - Set Aggressive");

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceID`=22 AND `SourceEntry`=@GUID AND `SourceGroup`=1;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`,`SourceGroup`,`SourceEntry`,`SourceId`,`ConditionTypeOrReference`,`elseGroup`,`ConditionValue1`,`ConditionValue2`,`ConditionValue3`,`ErrorTextId`,`Comment`) VALUES 
(22,1,@GUID,@SOURCETYPE,23,0,3733,0,0,0,'Curse of the Violet Tower - Only apply in The Violet Tower');
