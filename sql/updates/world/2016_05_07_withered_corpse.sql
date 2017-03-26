-- Withered Corpse
SET @ENTRY := 20561;
SET @SOURCETYPE := 0;

DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=@SOURCETYPE;
UPDATE creature_template SET AIName="SmartAI" WHERE entry=@ENTRY LIMIT 1;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES 
(@ENTRY,@SOURCETYPE,1,0,10,0,100,0,0,3,30000,30000,12,20335,4,30000,0,1,0,1,0,0,0,0.0,0.0,0.0,0.0,"Withered Corpse - Summon fleshbeast at 3 yards"),
(@ENTRY,@SOURCETYPE,2,0,0,0,100,0,0,0,30000,30000,12,20335,4,30000,0,1,0,1,0,0,0,0.0,0.0,0.0,0.0,"Withered Corpse - Summon fleshbeast on enter combat"),
(@ENTRY,@SOURCETYPE,0,0,17,0,100,0,20335,30000,30000,0,41,0,0,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,"Withered Corpse - Die on summoning fleshbeast");

-- Make attackable
UPDATE creature_template SET unit_flags=0 WHERE entry=@ENTRY;
-- Change permanent feign death type from Stun to Root
UPDATE creature_template_addon SET auras='31261 0' WHERE entry=@ENTRY;
