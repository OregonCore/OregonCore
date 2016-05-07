-- Sunfury Arcanist
SET @ENTRY := 20134;
SET @SOURCETYPE := 0;

DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=@SOURCETYPE;
UPDATE creature_template SET AIName="SmartAI" WHERE entry=@ENTRY LIMIT 1;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES 
(@ENTRY,@SOURCETYPE,3,0,1,0,100,0,6000,11000,35000,42000,11,35778,0,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,"Sunfury Arcanist - Cast Bloodcrystal Surge"),
(@ENTRY,@SOURCETYPE,1,0,0,0,100,0,0,0,8400,9800,11,34446,0,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,"Sunfury Arcanist - Cast Arcane Missiles"),
(@ENTRY,@SOURCETYPE,2,0,2,0,100,1,0,15,0,0,25,1,0,0,0,0,0,0,0,0,0,0.0,0.0,0.0,0.0,"Sunfury Arcanist - Flee at 15% hp"),
(@ENTRY,@SOURCETYPE,3,0,25,0,100,0,0,0,0,0,21,1,0,0,0,0,0,0,0,0,0,0.0,0.0,0.0,0.0,"Sunfury Arcanist - Enable Combat Movement on Reset");

DELETE FROM creature_ai_scripts WHERE creature_id=@ENTRY;

-- Sunfury Arch Mage
SET @ENTRY := 20135;
SET @SOURCETYPE := 0;

DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=@SOURCETYPE;
UPDATE creature_template SET AIName="SmartAI" WHERE entry=@ENTRY LIMIT 1;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES 
(@ENTRY,@SOURCETYPE,1,0,0,0,100,0,0,0,0,0,11,20823,64,0,0,0,0,2,0,0,0,0.0,0.0,0.0,0.0,"Sunfury Arch Mage - Cast Fireball"),
(@ENTRY,@SOURCETYPE,2,0,1,0,100,0,1000,1000,600000,600000,11,35917,1,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,"Sunfury Arch Mage - Cast Fiery Intellect"),
(@ENTRY,@SOURCETYPE,0,0,9,0,100,0,0,10,11000,15000,11,11831,1,0,0,0,0,2,0,0,0,0.0,0.0,0.0,0.0,"Sunfury Arch Mage - Cast Frost Nova"),
(@ENTRY,@SOURCETYPE,3,0,25,0,100,0,0,0,0,0,21,1,0,0,0,0,0,0,0,0,0,0.0,0.0,0.0,0.0,"Sunfury Arch Mage - Enable Combat Movement on Reset");

DELETE FROM creature_ai_scripts WHERE creature_id=@ENTRY;

-- Sunfury Guardsman Talisman
DELETE FROM creature_loot_template WHERE entry=20135 AND item=28637;

-- Arcane Repair cast condition
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceID`=17 AND `SourceEntry`=34062 AND `SourceGroup`=0;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`,`SourceGroup`,`SourceEntry`,`ConditionTypeOrReference`,`elseGroup`,`ConditionValue1`,`ConditionValue2`,`ConditionValue3`,`ErrorTextId`,`Comment`) VALUES 
(17,0,34062,1,0,34062,0,0,0,'Only cast Arcane Repair if unit spawns with the buff');

-- Sunfury Researcher
SET @ENTRY := 20136;
SET @SOURCETYPE := 0;

DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=@SOURCETYPE;
UPDATE creature_template SET AIName="SmartAI" WHERE entry=@ENTRY LIMIT 1;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES 
(@ENTRY,@SOURCETYPE,0,0,25,0,100,0,0,0,0,0,11,34062,1,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,"Sunfury Researcher - Cast Arcane Repair (OOC)"),
(@ENTRY,@SOURCETYPE,1,0,9,0,100,0,0,5,35000,40000,11,35918,0,0,0,0,0,2,0,0,0,0.0,0.0,0.0,0.0,"Sunfury Researcher - Cast Puncture Armor");

DELETE FROM creature_ai_scripts WHERE creature_id=@ENTRY;