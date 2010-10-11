DELETE FROM `script_texts` WHERE `entry` BETWEEN '-1600468' AND '-1600464';
INSERT INTO `script_texts` VALUES ('-1600468', 'Brave adventurer, thank you for rescuing me! I am sure Marshal Marris will reward your kind deed.', null, null, null, null, null, null, null, null, '0', '0', '0', '0', '');
INSERT INTO `script_texts` VALUES ('-1600467', 'Marshal Marris, sir. Corporal Keeshan of the 12th Sabre Regiment returned from battle and reporting for duty!', null, null, null, null, null, null, null, null, '0', '0', '0', '0', '');
INSERT INTO `script_texts` VALUES ('-1600466', 'The Blackrock infestation is thick in these parts. I will do my best to keep the pace. Let\'s go!', null, null, null, null, null, null, null, null, '0', '0', '0', '0', '');
INSERT INTO `script_texts` VALUES ('-1600465', 'Ah, fresh air, at last! I need a moment to reset.', null, null, null, null, null, null, null, null, '0', '0', '0', '0', '');
INSERT INTO `script_texts` VALUES ('-1600464', 'My wounds are grave. Forgive my slow pace but my injuries won\'t allow me to walk any faster.', null, null, null, null, null, null, null, null, '0', '0', '0', '0', '');
UPDATE `creature_template` SET `ScriptName` = 'npc_corporal_keeshan' WHERE `entry` = '349';
UPDATE `quest_template` SET `SpecialFlags` = '2' WHERE `entry` = '219';