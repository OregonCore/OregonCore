-- Nether Dragonkin Egg
SET @ENTRY := 184077;
SET @LINKED := 184078;
SET @SOURCETYPE := 1;

DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=@SOURCETYPE;
UPDATE gameobject_template SET AIName="SmartGameObjectAI" WHERE entry=@ENTRY LIMIT 1;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES 
(@ENTRY,@SOURCETYPE,0,0,70,0,100,0,3,0,0,0,99,3,0,0,0,0,0,20,@LINKED,0,0,0.0,0.0,0.0,0.0,"Nether Dragonkin Egg - On Deactivate - Despawn glow"),
(@ENTRY,@SOURCETYPE,1,0,11,0,100,0,0,0,0,0,70,0,0,0,0,0,0,20,@LINKED,0,0,0.0,0.0,0.0,0.0,"Nether Dragonkin Egg - On Respawn - Respawn glow");

-- Etherlithium Matrix Crystal
SET @ENTRY := 183767;
SET @LINKED := 183768;
SET @SOURCETYPE := 1;

DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=@SOURCETYPE;
UPDATE gameobject_template SET AIName="SmartGameObjectAI" WHERE entry=@ENTRY LIMIT 1;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES 
(@ENTRY,@SOURCETYPE,0,0,70,0,100,0,3,0,0,0,99,3,0,0,0,0,0,20,@LINKED,0,0,0.0,0.0,0.0,0.0,"Etherlithium Matrix Crystal - On Deactivate - Despawn glow"),
(@ENTRY,@SOURCETYPE,1,0,11,0,100,0,0,0,0,0,70,0,0,0,0,0,0,20,@LINKED,0,0,0.0,0.0,0.0,0.0,"Etherlithium Matrix Crystal - On Respawn - Respawn glow");

-- Baa'ri Tablet Fragment 1
SET @ENTRY := 184869;
SET @LINKED := 184868;
SET @SOURCETYPE := 1;

DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=@SOURCETYPE;
UPDATE gameobject_template SET AIName="SmartGameObjectAI" WHERE entry=@ENTRY LIMIT 1;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES 
(@ENTRY,@SOURCETYPE,0,0,70,0,100,0,3,0,0,0,99,3,0,0,0,0,0,20,@LINKED,0,0,0.0,0.0,0.0,0.0,"Baa'ri Tablet Fragment - On Deactivate - Despawn glow"),
(@ENTRY,@SOURCETYPE,1,0,11,0,100,0,0,0,0,0,70,0,0,0,0,0,0,20,@LINKED,0,0,0.0,0.0,0.0,0.0,"Baa'ri Tablet Fragment - On Respawn - Respawn glow");

-- Baa'ri Tablet Fragment 2
SET @ENTRY := 184870;
SET @LINKED := 184868;
SET @SOURCETYPE := 1;

DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=@SOURCETYPE;
UPDATE gameobject_template SET AIName="SmartGameObjectAI" WHERE entry=@ENTRY LIMIT 1;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES 
(@ENTRY,@SOURCETYPE,0,0,70,0,100,0,3,0,0,0,99,3,0,0,0,0,0,20,@LINKED,0,0,0.0,0.0,0.0,0.0,"Baa'ri Tablet Fragment - On Deactivate - Despawn glow"),
(@ENTRY,@SOURCETYPE,1,0,11,0,100,0,0,0,0,0,70,0,0,0,0,0,0,20,@LINKED,0,0,0.0,0.0,0.0,0.0,"Baa'ri Tablet Fragment - On Respawn - Respawn glow");

-- Netherwing Egg
SET @ENTRY := 185915;
SET @LINKED := 185600;
SET @SOURCETYPE := 1;

DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=@SOURCETYPE;
UPDATE gameobject_template SET AIName="SmartGameObjectAI" WHERE entry=@ENTRY LIMIT 1;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES 
(@ENTRY,@SOURCETYPE,0,0,70,0,100,0,3,0,0,0,99,3,0,0,0,0,0,20,@LINKED,0,0,0.0,0.0,0.0,0.0,"Netherwing Egg - On Deactivate - Despawn gas"),
(@ENTRY,@SOURCETYPE,1,0,11,0,100,0,0,0,0,0,70,0,0,0,0,0,0,20,@LINKED,0,0,0.0,0.0,0.0,0.0,"Netherwing Egg - On Respawn - Respawn gas");