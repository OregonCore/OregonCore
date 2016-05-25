-- Condition to prevent summoning Reflection of Ya-six when the player already has the debuff
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceID`=17 AND `SourceEntry`=35381 AND `SourceGroup`=0;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`,`SourceEntry`,`ConditionTypeOrReference`,`elseGroup`,`ConditionValue1`,`ConditionValue2`,`ConditionValue3`,`NegativeCondition`,`Comment`) 
VALUES (17,35381,1,0,35381,1,1,1,'Reflection of Ya-six summon');

-- Agent Ya-six
SET @ENTRY := 20552;
SET @SOURCETYPE := 0;

DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=@SOURCETYPE;
UPDATE creature_template SET AIName="SmartAI" WHERE entry=@ENTRY LIMIT 1;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES 
(@ENTRY,@SOURCETYPE,0,0,10,0,100,0,1,15,180000,180000,1,0,0,0,0,0,0,7,0,0,0,0.0,0.0,0.0,0.0,"Agent Ya-six - Ooc LoS - Greet player");

DELETE FROM `creature_text` WHERE `entry`=@ENTRY AND `groupid`=0 AND `id`=0 LIMIT 1;
INSERT INTO `creature_text` (`entry`,`groupid`,`id`,`text`,`type`,`language`,`probability`,`emote`,`duration`,`sound`,`comment`) 
VALUES (@ENTRY,0,0,'Hey! Hey you! $R! Over here!',12,0,100,1,0,0,'Agent Ya-six greeting');

-- Reflection of Ya-six
UPDATE creature_template SET AIName="", ScriptName="npc_reflection_of_ya_six" WHERE entry=20603 LIMIT 1;
