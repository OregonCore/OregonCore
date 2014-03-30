UPDATE instance_template SET `script`='instance_zulfarrak' WHERE map=209;
UPDATE `gameobject_template` SET `ScriptName`='go_troll_cage' WHERE `entry`>= 141070 AND `entry`< 141075;
insert into `script_texts` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`, `sound`, `type`, `language`, `emote`, `comment`) values
('-1209000','Oh no! Here they come!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'0','0','1','0','Weegli Blastfuse SAY_WEEGLI_OHNO'),
('-1209001','OK. Here I go.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'0','0','1','0','Weegli Blastfuse SAY_WEEGLI_OK_I_GO'),
('-1209002','What? How dare you say that to me?!?',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'0','0','1','0','Sergeant Bly SAY_1'),
('-1209003','After all we''ve been through? Well, I didn''t like you anyway!!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'0','0','1','0','Sergeant Bly SAY_2');

-- Update Zul'Farrak thanks to Superbad
-- Remove most of troll cage script,it's handled in script now
DELETE FROM `event_scripts` WHERE `id`=2609 AND (`delay` > 0 OR `command` !=11);
-- Create waypoints for trolls to run up the stairs
DELETE FROM `waypoint_data` WHERE `id`=81553;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(81553,1,1884.869,1228.621,9.70,0,1,0,100,0),
(81553,2,1887.53,1263,41.52,0,1,0,100,0);
-- Sgt bly & crew shouldn't regen when out of battle (they have a healer,after all)
UPDATE `creature_template` SET `RegenHealth`=0 WHERE `entry` IN (7608,7606,7605,7604,7607);
-- Murta should heal herself,too
DELETE FROM `creature_ai_scripts` WHERE `id`=760805;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES
(760805,7608,2,0,85,3,50,0,10000,12000,11,11642,0,0,0,0,0,0,0,0,0,0, 'Murta Grimgut - Cast Heal on Self' );

DELETE FROM areatrigger_scripts WHERE entry = 1447;
INSERT INTO areatrigger_scripts VALUES (1447, 'at_antusul');