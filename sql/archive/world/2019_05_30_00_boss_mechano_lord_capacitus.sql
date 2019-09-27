DELETE FROM `script_texts` WHERE `entry`=-1921940;
INSERT INTO `script_texts` VALUES (-1921940, 'Damn, I\'m good!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 19954, 1, 0, 0, 'Mechano-Lord Capacitus - Killing a player');
DELETE FROM `script_texts` WHERE `entry`=-1921930;
INSERT INTO `script_texts` VALUES (-1921930, 'Can\'t say I didn\'t warn you!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 11163, 1, 0, 0, 'Mechano-Lord Capacitus - Killing a player');
DELETE FROM `script_texts` WHERE `entry`=-1921920;
INSERT INTO `script_texts` VALUES (-1921920, 'Think you can hurt me, huh? Think I\'m afraid a\' you?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 11165, 1, 0, 0, 'Mechano-Lord Capacitus - Yells 2');
DELETE FROM `script_texts` WHERE `entry`=-1921910;
INSERT INTO `script_texts` VALUES (-1921910, 'Go ahead, gimme your best shot. I can take it!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 11166, 1, 0, 0, 'Mechano-Lord Capacitus - Yells');
DELETE FROM `script_texts` WHERE `entry`=-1921900;
INSERT INTO `script_texts` VALUES (-1921900, 'You should split while you can.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 11162, 1, 0, 0, 'Mechano-Lord Capacitus - Aggro');
DELETE FROM `script_texts` WHERE `entry`=-1545007;
INSERT INTO `script_texts` VALUES (-1545007, 'I\'m bringin\' the pain!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 10367, 1, 0, 0, 'mekgineer SAY_MECHANICS');


UPDATE creature_template SET AIName = '', ScriptName = 'boss_mechano_lord_capacitus' WHERE entry IN (19219);