-- Abjurist Belmara Actionlist
SET @ENTRY := 1954600;
SET @SOURCETYPE := 9;

DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=@SOURCETYPE;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES 
(@ENTRY,@SOURCETYPE,0,0,0,0,100,0,0,0,0,0,2,35,0,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,"Abjurist Belmara - Set Friendly at 0.0"),
(@ENTRY,@SOURCETYPE,1,0,0,0,100,0,1500,1500,0,0,53,0,195460,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,"Abjurist Belmara - Walk to bookshelf at 1.5"),
(@ENTRY,@SOURCETYPE,2,0,0,0,100,0,2500,2500,0,0,66,0,0,0,0,0,0,8,0,0,0,2237.88,2388.36,111.172,3.79146,"Abjurist Belmara - Face bookshelf at 4.0"),
(@ENTRY,@SOURCETYPE,3,0,0,0,100,0,500,500,0,0,5,69,0,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,"Abjurist Belmara - Use bookshelf at 4.5"),
(@ENTRY,@SOURCETYPE,4,0,0,0,100,0,3000,3000,0,0,5,0,0,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,"Abjurist Belmara - Play default emote at 7.5"),
(@ENTRY,@SOURCETYPE,5,0,0,0,100,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,"Abjurist Belmara - Talk at 7.5");

-- Abjurist Belmara
SET @ENTRY := 19546;
SET @SOURCETYPE := 0;

DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=@SOURCETYPE;
UPDATE creature_template SET AIName="SmartAI" WHERE entry=@ENTRY LIMIT 1;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES 
-- Combat scripting
(@ENTRY,@SOURCETYPE,0,0,0,0,100,0,2000,5000,12000,22000,11,17740,0,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,"Abjurist Belmara - IC - Cast to self Mana shield"),
(@ENTRY,@SOURCETYPE,1,0,0,0,100,0,0,6000,2500,8000,11,34447,0,0,0,0,0,2,0,0,0,0.0,0.0,0.0,0.0,"Abjurist Belmara - IC - Cast Arcane missles"),
(@ENTRY,@SOURCETYPE,2,0,25,0,100,0,0,0,0,0,28,12544,0,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,"Abjurist Belmara - On reset - Remove aura from Frost Armor"),
(@ENTRY,@SOURCETYPE,3,0,4,0,100,0,0,0,0,0,11,12544,0,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,"Abjurist Belmara - On aggro - Cast to self Frost armor"),
-- Quest complete scripting
(@ENTRY,@SOURCETYPE,11,0,54,0,100,0,0,0,0,0,80,1954600,0,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,"Abjurist Belmara - Call actionlist on spawn");

DELETE FROM `creature_text` WHERE `entry`=@ENTRY;
INSERT INTO `creature_text` (`entry`,`groupid`,`id`,`text`,`type`,`language`,`probability`,`emote`,`duration`,`sound`,`comment`) VALUES
(@ENTRY,0,0,'I can''t sleep without a good bedtime story. Now I''m certain to rest well.',12,0,100,1,0,0,'Abjurist Belmara at bookshelf');

-- Spell has SendEvent effect, give credit and spawn Abjurist Belmara on use
DELETE FROM `event_scripts` WHERE `id`=12607;
INSERT INTO `event_scripts` (`id`,`delay`,`command`,`datalong`,`datalong2`,`x`,`y`,`z`,`o`) VALUES
(12607,0,8,19547,0,0,0,0,0),
(12607,0,10,@ENTRY,25000,2236.739014, 2393.230225,112.261452,0.0);

-- Waypoints
SET @Waypoint := 195460;
DELETE FROM `waypoints` WHERE `entry`=@Waypoint;
INSERT INTO `waypoints` (`entry`,`pointid`,`position_x`,`position_y`,`position_z`,`point_comment`) VALUES
(@Waypoint, 0, 2236.739014, 2393.230225, 112.261452, 'Abjurist Belmara bookshelf path'),
(@Waypoint, 1, 2238.921631, 2393.589355, 112.873177, 'Abjurist Belmara bookshelf path'),
(@Waypoint, 2, 2240.347900, 2392.175781, 112.896400, 'Abjurist Belmara bookshelf path'),
(@Waypoint, 3, 2240.636963, 2390.517090, 112.584877, 'Abjurist Belmara bookshelf path');
