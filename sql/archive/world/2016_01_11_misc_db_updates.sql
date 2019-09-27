-- Fix quest Administering the Salve
UPDATE `creature_template` SET `ainame`='SmartAI' WHERE `entry`=16847;
DELETE FROM `smart_scripts` WHERE `entryorguid`=16847 AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(16847,0,0,1,8,0,100,0,29314,0,60000,60000,33,16847,0,0,0,0,0,7,0,0,0,0,0,0,0,'Debilitated Maghar Grunt - On Spellhit (Quest - Healing Salve) - Give Kill Credit'),
(16847,0,1,2,61,0,100,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,'Debilitated Maghar Grunt - Linked with Previous Event - Say'),
(16847,0,2,3,61,0,100,0,0,0,0,0,3,16846,0,0,0,0,0,1,0,0,0,0,0,0,0,'Debilitated Maghar Grunt - Linked with Previous Event - Change Entry'),
(16847,0,3,0,61,0,100,0,0,0,0,0,41,10000,0,0,0,0,0,1,0,0,0,0,0,0,0,'Debilitated Maghar Grunt - Linked with Previous Event - Despawn'),
(16847,0,4,0,25,0,100,0,0,0,0,0,3,16847,0,0,0,0,0,1,0,0,0,0,0,0,0,'Debilitated Maghar Grunt - On Spawn - Change Entry');

DELETE FROM `creature_text` WHERE `entry` =16847;
INSERT INTO `creature_text` (`entry`,`groupid`,`id`,`text`,`type`,`language`,`probability`,`emote`,`duration`,`sound`,`comment`) VALUES
(16847,0,0,'My strength.... is... returning!',12,0,100,0,0,0,'Debilitated Maghar Grunt'),
(16847,0,1,'You''ve found a cure! We will crush our enemies!',12,0,100,0,0,0,'Debilitated Maghar Grunt'),
(16847,0,2,'You''ve restored my health! I''m in your debt, $N.',12,0,100,0,0,0,'Debilitated Maghar Grunt');

-- Fix Call to Arms: The Plaguelands/Scarlet Diversions breadcrumb quests
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` IN(19,20) AND `SourceId` IN(5066,5090,5091,5093,5094,5095,10373,10374);

INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(19, 0, 5066, 0, 0, 14, 0, 5092, 0, 0, 0, 0, 0, '', 'Call to Arms: The Plaguelands (Breadcrumb)'),
(20, 0, 5066, 0, 0, 14, 0, 5092, 0, 0, 0, 0, 0, '', 'Call to Arms: The Plaguelands (Breadcrumb)'),
(19, 0, 5090, 0, 0, 14, 0, 5092, 0, 0, 0, 0, 0, '', 'Call to Arms: The Plaguelands (Breadcrumb)'),
(20, 0, 5090, 0, 0, 14, 0, 5092, 0, 0, 0, 0, 0, '', 'Call to Arms: The Plaguelands (Breadcrumb)'),
(19, 0, 5091, 0, 0, 14, 0, 5092, 0, 0, 0, 0, 0, '', 'Call to Arms: The Plaguelands (Breadcrumb)'),
(20, 0, 5091, 0, 0, 14, 0, 5092, 0, 0, 0, 0, 0, '', 'Call to Arms: The Plaguelands (Breadcrumb)'),
(19, 0, 5093, 0, 0, 14, 0, 5096, 0, 0, 0, 0, 0, '', 'Call to Arms: The Plaguelands (Breadcrumb)'),
(20, 0, 5093, 0, 0, 14, 0, 5096, 0, 0, 0, 0, 0, '', 'Call to Arms: The Plaguelands (Breadcrumb)'),
(19, 0, 5094, 0, 0, 14, 0, 5096, 0, 0, 0, 0, 0, '', 'Call to Arms: The Plaguelands (Breadcrumb)'),
(20, 0, 5094, 0, 0, 14, 0, 5096, 0, 0, 0, 0, 0, '', 'Call to Arms: The Plaguelands (Breadcrumb)'),
(19, 0, 5095, 0, 0, 14, 0, 5096, 0, 0, 0, 0, 0, '', 'Call to Arms: The Plaguelands (Breadcrumb)'),
(20, 0, 5095, 0, 0, 14, 0, 5096, 0, 0, 0, 0, 0, '', 'Call to Arms: The Plaguelands (Breadcrumb)'),
(19, 0, 10373, 0, 0, 14, 0, 5092, 0, 0, 0, 0, 0, '', 'Call to Arms: The Plaguelands (Breadcrumb)'),
(20, 0, 10373, 0, 0, 14, 0, 5092, 0, 0, 0, 0, 0, '', 'Call to Arms: The Plaguelands (Breadcrumb)'),
(19, 0, 10374, 0, 0, 14, 0, 5096, 0, 0, 0, 0, 0, '', 'Call to Arms: The Plaguelands (Breadcrumb)'),
(20, 0, 10374, 0, 0, 14, 0, 5096, 0, 0, 0, 0, 0, '', 'Call to Arms: The Plaguelands (Breadcrumb)');

UPDATE `quest_template` SET `NextQuestId`=0 WHERE  `entry` IN(5066,5090,5091,5093,5094,5095,10373,10374);

-- Add tele for Emerald Dream
DELETE FROM `instance_template` WHERE `map` = 169;
INSERT INTO `instance_template` (`map`, `parent`, `script`, `allowMount`) VALUES
(169, 0, '', 0);

DELETE FROM `game_tele` WHERE `id` IN  (1425, 1426, 1427, 1428);
INSERT INTO `game_tele` (`id`, `position_x`, `position_y`, `position_z`, `orientation`, `map`, `name`) VALUES 
(1425, -366.091, 3097.86, 92.317, 0.0487625, 169, 'EmeraldDream'),
(1426, 2781.566406, 3006.763184, 23.221882, 0.5, 169, 'EmeraldStatue'),
(1427, -2128.12, -1005.89, 132.213, 0.5, 169, 'VerdantFields'),
(1428, 2732.93, -3319.63, 101.284, 0.5, 169, 'EmeraldForest');

-- Fix bobbing apples, bread of the dead
DELETE FROM `spell_linked_spell` WHERE `spell_trigger` = -24869 AND `spell_effect` = 24870;
INSERT INTO `spell_linked_spell` (`spell_trigger`,`spell_effect`,`type`,`comment`) VALUES
(-24869, 24870, 0, 'Bobbing Apple, Winter Veil Cookie');

-- Fix quest What Book? I don't see any book!
SET @NPC_LARRY := 19720;
SET @NPC_MALONE := 19725;
SET @NPC_JACK := 19726;

DELETE FROM `gossip_menu_option` WHERE `menu_id`=8033;
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `action_menu_id`, `action_poi_id`, `action_script_id`, `box_coded`, `box_money`, `box_text`) VALUES
(8033, 0, 0, 'Ezekiel said that you might have a certain book...', 1, 1, 0, 0, 0, 0, 0, NULL);

UPDATE `creature_template` SET `gossip_menu_id` = 8033 WHERE `entry` = @NPC_LARRY;
UPDATE `creature_template` SET `AIName`='SmartAI', `ScriptName`='' WHERE `entry` IN (@NPC_LARRY,@NPC_MALONE,@NPC_JACK);
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (@NPC_LARRY,@NPC_MALONE,@NPC_JACK) AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@NPC_LARRY,0,0,1,25,0,100,0,0,0,0,0,2,1194,0,0,0,0,0,1,0,0,0,0,0,0,0,'"Dirty" Larry - On reset - Set faction'),
(@NPC_LARRY,0,1,2,61,0,100,0,0,0,0,0,18,256,0,0,0,0,0,1,0,0,0,0,0,0,0,'"Dirty" Larry - On reset - Set unit_flags IMMUNE_TO_PC'),
(@NPC_LARRY,0,2,3,61,0,100,0,0,0,0,0,81,3,0,0,0,0,0,1,0,0,0,0,0,0,0,'"Dirty" Larry - On reset - Set npcflag gossip, questgiver'),
(@NPC_LARRY,0,3,4,61,0,100,0,0,0,0,0,45,1,2,0,0,0,0,10,70376,@NPC_MALONE,0,0,0,0,0,'Dirty" Larry - On reset - Set data 1 2 "Epic" Malone'),
(@NPC_LARRY,0,4,0,61,0,100,0,0,0,0,0,45,1,2,0,0,0,0,10,70377,@NPC_JACK,0,0,0,0,0,'Dirty" Larry - On reset - Set data 1 2 "Creepjack"'),
(@NPC_LARRY,0,5,6,62,0,100,0,8033,0,0,0,64,1,0,0,0,0,0,7,0,0,0,0,0,0,0,'Dirty" Larry - On gossip select - Store targetlist'),
(@NPC_LARRY,0,6,7,61,0,100,0,0,0,0,0,72,0,0,0,0,0,0,7,0,0,0,0,0,0,0,'Dirty" Larry - On gossip select - Close Gossip'),
(@NPC_LARRY,0,7,8,61,0,100,0,0,0,0,0,1,0,5000,0,0,0,0,1,0,0,0,0,0,0,0,'Dirty" Larry - On gossip select - Say line'),
(@NPC_LARRY,0,8,9,61,0,100,0,0,0,0,0,81,0,0,0,0,0,0,1,0,0,0,0,0,0,0,'Dirty" Larry - On gossip select - Set npcflag none'),
(@NPC_LARRY,0,9,10,61,0,100,0,0,0,0,0,70,0,0,0,0,0,0,10,70376,@NPC_MALONE,0,0,0,0,0,'Dirty" Larry - On gossip select - Respawn "Epic" Malone '),
(@NPC_LARRY,0,10,0,61,0,100,0,0,0,0,0,70,0,0,0,0,0,0,10,70377,@NPC_JACK,0,0,0,0,0,'Dirty" Larry - On gossip select - Respawn "Creepjack" '),
(@NPC_LARRY,0,11,0,52,0,100,0,0,@NPC_LARRY,0,0,1,1,5000,0,0,0,0,1,0,0,0,0,0,0,0,'Dirty" Larry - On text over - Say line'),
(@NPC_LARRY,0,12,0,52,0,100,0,1,@NPC_LARRY,0,0,1,2,2000,0,0,0,0,1,0,0,0,0,0,0,0,'Dirty" Larry - On text over - Say line'),
(@NPC_LARRY,0,13,0,52,0,100,0,2,@NPC_LARRY,0,0,1,3,2000,0,0,0,0,1,0,0,0,0,0,0,0,'Dirty" Larry - On text over - Say line'),
(@NPC_LARRY,0,14,0,52,0,100,0,3,@NPC_LARRY,0,0,1,4,2000,0,0,0,0,1,0,0,0,0,0,0,0,'Dirty" Larry - On text over - Say line'),
(@NPC_LARRY,0,15,16,52,0,100,0,4,@NPC_LARRY,0,0,2,14,0,0,0,0,0,1,0,0,0,0,0,0,0,'Dirty" Larry - On text over - Set faction'),
(@NPC_LARRY,0,16,17,61,0,100,0,0,0,0,0,19,256,0,0,0,0,0,1,0,0,0,0,0,0,0,'Dirty" Larry - On text over - Remove unit_flags IMMUNE_TO_PC'),
(@NPC_LARRY,0,17,18,61,0,100,0,0,0,0,0,49,0,0,0,0,0,0,12,1,0,0,0,0,0,0,'Dirty" Larry - On text over - Attack'),
(@NPC_LARRY,0,18,19,61,0,100,0,0,0,0,0,100,1,0,0,0,0,0,10,70376,@NPC_MALONE,0,0,0,0,0,'Dirty" Larry - On text over - Send targetlist to "Epic" Malone'),
(@NPC_LARRY,0,19,20,61,0,100,0,0,0,0,0,45,1,1,0,0,0,0,10,70376,@NPC_MALONE,0,0,0,0,0,'Dirty" Larry - On text over - Set data 1 1 "Epic" Malone'),
(@NPC_LARRY,0,20,21,61,0,100,0,0,0,0,0,100,1,0,0,0,0,0,10,70377,@NPC_JACK,0,0,0,0,0,'Dirty" Larry - On text over - Send targetlist to "Creepjack"'),
(@NPC_LARRY,0,21,0,61,0,100,0,0,0,0,0,45,1,1,0,0,0,0,10,70377,@NPC_JACK,0,0,0,0,0,'Dirty" Larry - On text over - Set data 1 1 "Creepjack"'),
(@NPC_LARRY,0,22,23,2,0,100,0,0,20,0,0,15,10231,0,0,0,0,0,16,0,0,0,0,0,0,0,'Dirty" Larry - On health below 20% - Quest credit'),
(@NPC_LARRY,0,23,24,61,0,100,0,0,0,0,0,1,5,0,0,0,0,0,1,0,0,0,0,0,0,0,'Dirty" Larry - On health below 20% - Say line'),
(@NPC_LARRY,0,24,3,61,0,100,0,0,0,0,0,24,0,0,0,0,0,0,1,0,0,0,0,0,0,0,'Dirty" Larry - On health below 20% - Evade'),
(@NPC_LARRY,0,25,0,11,0,100,0,0,0,0,0,42,0,15,0,0,0,0,1,0,0,0,0,0,0,0,'Dirty" Larry - On spawn - Set invincibility HP level'),

(@NPC_MALONE,0,0,1,25,0,100,0,0,0,0,0,2,1194,0,0,0,0,0,1,0,0,0,0,0,0,0,'"Epic" Malone - On reset - Set faction'),
(@NPC_MALONE,0,1,0,61,0,100,0,0,0,0,0,18,256,0,0,0,0,0,1,0,0,0,0,0,0,0,'"Epic" Malone - On reset - Set unit_flags IMMUNE_TO_PC'),
(@NPC_MALONE,0,2,3,38,0,100,0,1,1,0,0,2,14,0,0,0,0,0,1,0,0,0,0,0,0,0,'"Epic" Malone - On data 1 1 set - Set faction'),
(@NPC_MALONE,0,3,4,61,0,100,0,0,0,0,0,19,256,0,0,0,0,0,1,0,0,0,0,0,0,0,'"Epic" Malone - On data 1 1 set - Remove unit_flags IMMUNE_TO_PC'),
(@NPC_MALONE,0,4,5,61,0,100,0,0,0,0,0,90,1,0,0,0,0,0,1,0,0,0,0,0,0,0,'"Epic" Malone - On data 1 1 set - Remove standstate sleep'),
(@NPC_MALONE,0,5,0,61,0,100,0,0,0,0,0,49,0,0,0,0,0,0,12,1,0,0,0,0,0,0,'"Epic" Malone - On data 1 1 set - Attack'),
(@NPC_MALONE,0,6,0,38,0,100,0,1,2,0,0,24,0,0,0,0,0,0,1,0,0,0,0,0,0,0,'"Epic" Malone - On data 1 2 set - Evade'),

(@NPC_JACK,0,0,1,25,0,100,0,0,0,0,0,2,1194,0,0,0,0,0,1,0,0,0,0,0,0,0,'"Creepjack" - On reset - Set faction'),
(@NPC_JACK,0,1,0,61,0,100,0,0,0,0,0,18,256,0,0,0,0,0,1,0,0,0,0,0,0,0,'"Creepjack" - On reset - Set unit_flags IMMUNE_TO_PC'),
(@NPC_JACK,0,2,3,38,0,100,0,1,1,0,0,2,14,0,0,0,0,0,1,0,0,0,0,0,0,0,'"Creepjack" - On data 1 1 set - Set faction'),
(@NPC_JACK,0,3,4,61,0,100,0,0,0,0,0,19,256,0,0,0,0,0,1,0,0,0,0,0,0,0,'"Creepjack" - On data 1 1 set - Remove unit_flags IMMUNE_TO_PC'),
(@NPC_JACK,0,4,5,61,0,100,0,0,0,0,0,90,1,0,0,0,0,0,1,0,0,0,0,0,0,0,'"Creepjack" - On data 1 1 set - Remove standstate sleep'),
(@NPC_JACK,0,5,0,61,0,100,0,0,0,0,0,49,0,0,0,0,0,0,12,1,0,0,0,0,0,0,'"Creepjack" - On data 1 1 set - Attack'),
(@NPC_JACK,0,6,0,38,0,100,0,1,2,0,0,24,0,0,0,0,0,0,1,0,0,0,0,0,0,0,'"Creepjack" - On data 1 2 set - Evade');

DELETE FROM `creature_text` WHERE `entry`=@NPC_LARRY;
INSERT INTO `creature_text` (`entry`, `groupid`, `id`, `text`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `comment`) VALUES
(@NPC_LARRY, 0, 0, 'Time to teach you a lesson in manners, little boy! ', 12, 0, 100, 0, 0, 0, '"Dirty" Larry'),
(@NPC_LARRY, 1, 0, 'I''m now going to give you to the count of "3" to get out of here before I sick the dogs on you.', 12, 0, 100, 0, 0, 0, '"Dirty" Larry'),
(@NPC_LARRY, 2, 0, '1...', 12, 0, 100, 0, 0, 0, '"Dirty" Larry'),
(@NPC_LARRY, 3, 0, '2...', 12, 0, 100, 0, 0, 0, '"Dirty" Larry'),
(@NPC_LARRY, 4, 0, 'Time to meet your maker!', 12, 0, 100, 0, 0, 0, '"Dirty" Larry'),
(@NPC_LARRY, 5, 0, 'Alright, we give up! Don''t hurt us!', 12, 0, 100, 0, 0, 0, '"Dirty" Larry');

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=15 AND `SourceGroup`=8033;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`,`SourceGroup`,`SourceEntry`,`ElseGroup`,`ConditionTypeOrReference`,`ConditionValue1`,`ConditionValue2`,`ConditionValue3`,`ErrorTextId`,`ScriptName`,`Comment`) VALUES
(15,8033,0,0,9,10231,0,0,0,'','"Dirty" Larry only show gossip if player is on and has not completed What Book? I Dont See Any Book');

-- Move EPL tower gryphon master to SAI
DELETE FROM `gossip_menu_option` WHERE menu_id = 7379;
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `action_menu_id`, `action_poi_id`, `action_script_id`, `box_coded`, `box_money`, `box_text`) VALUES
(7379, 0, 0, 'Take me to Northpass Tower.', 1, 1, 0, 0, 0, 0, 0, NULL),
(7379, 1, 0, 'Take me to Eastwall Tower.', 1, 1, 0, 0, 0, 0, 0, NULL),
(7379, 2, 0, 'Take me to Crown Guard Tower.', 1, 1, 0, 0, 0, 0, 0, NULL);

UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = 17209;
UPDATE `creature_template` SET  `gossip_menu_id` =  '7379' WHERE  `entry` =17209;

DELETE FROM `smart_scripts` WHERE (source_type = 0 AND entryorguid = 17209);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(17209, 0, 0, 3, 62, 0, 100, 0, 7379, 0, 0, 0, 52, 494, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'William Kielar - On Gossip Option 0 Selected - Activate Taxi Path 494'),
(17209, 0, 1, 3, 62, 0, 100, 0, 7379, 1, 0, 0, 52, 495, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'William Kielar - On Gossip Option 1 Selected - Activate Taxi Path 495'),
(17209, 0, 2, 3, 62, 0, 100, 0, 7379, 2, 0, 0, 52, 496, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'William Kielar - On Gossip Option 2 Selected - Activate Taxi Path 496'),
(17209, 0, 3, 0, 61, 0, 100, 0, 0, 0, 0, 0, 72, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'William Kielar - On Gossip Option 0 Selected - Close Gossip');