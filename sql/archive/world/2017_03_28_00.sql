UPDATE `creature_template` SET `AIName` = 'EventAI' WHERE `entry` = '9937'; 
UPDATE `creature_template` SET `gossip_menu_id` = '55002' WHERE `entry` = '9937';
INSERT INTO `creature_ai_scripts` (`id`, `creature_id`, `event_type`, `event_inverse_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action1_type`, `action1_param1`, `action1_param2`, `action1_param3`, `action2_type`, `action2_param1`, `action2_param2`, `action2_param3`, `action3_type`, `action3_param1`, `action3_param2`, `action3_param3`, `comment`) VALUES (993701, '9937', '1', '0', '100', '0', '1500', '1500', '0', '0', '3', '9936', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'Model Change Small Corrupted'); 
INSERT INTO `creature_ai_scripts` (`id`, `creature_id`, `event_type`, `event_inverse_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action1_type`, `action1_param1`, `action1_param2`, `action1_param3`, `action2_type`, `action2_param1`, `action2_param2`, `action2_param3`, `action3_type`, `action3_param1`, `action3_param2`, `action3_param3`, `comment`) VALUES (993702, '9937', '1', '0', '100', '0', '3500', '3500', '0', '0', '3', '10042', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'Model Change Big Corrupted');

-- Insert Gossip /custom made id/
DELETE FROM `gossip_menu_option` WHERE `menu_id`=55002 AND `id`=1;
INSERT INTO `gossip_menu_option` (`menu_id`,`id`,`option_icon`,`option_text`,`option_id`,`npc_option_npcflag`,`action_menu_id`) VALUES(55002,1,0,'I want to release the saber to Winna.',1,131,0);

-- Only show gossip if near Winna
DELETE FROM `conditions` WHERE `SourceGroup`=55002 AND `SourceTypeOrReferenceId`=15;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`,`SourceGroup`,`SourceEntry`,`ElseGroup`,`ConditionTypeOrReference`,`ConditionTarget`,`ConditionValue1`,`ConditionValue2`,`ConditionValue3`,`NegativeCondition`,`ErrorTextId`,`ScriptName`,`Comment`) VALUES (15,55002,1,0,29,1,9996,5,0,0,0,'','Only show second gossip Corrupted Saber is near Wina in 5 yards');

DELETE FROM `gossip_scripts` WHERE `id`=50002;
INSERT INTO `gossip_scripts` (`id`, `delay`, `command`, `datalong`, `datalong2`, `dataint`, `x`, `y`, `z`, `o`) VALUES ('50002', '0', '7', '4506', '30', '0', '-10291.2', '-4315.28', '-38.382', '4.07'); 
INSERT INTO `gossip_scripts` (`id`, `delay`, `command`, `datalong`, `datalong2`, `dataint`, `x`, `y`, `z`, `o`) VALUES ('50002', '0', '18', '2000', '0', '0', '-10291.2', '-4315.28', '-38.382', '4.07'); 
UPDATE `gossip_menu_option` SET `action_script_id` = '50002' WHERE `menu_id` = '55002' AND `id` = '1'; 