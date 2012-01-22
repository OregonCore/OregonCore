UPDATE `instance_template` SET `script`='instance_ramparts' WHERE `map`=543;

UPDATE `creature_template` SET `AIName` = 'EventAI' WHERE entry = '17309';
DELETE FROM `creature_ai_scripts` WHERE `id`=1730901;
INSERT INTO `creature_ai_scripts` VALUES (1730901, 17309, 0, 0, 100, 7, 2400, 6100, 10500, 23400, 11, 14032, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Hellfire Watcher - Cast Shadow Word: Pain');
DELETE FROM `creature_ai_scripts` WHERE `id`=1730902;
INSERT INTO `creature_ai_scripts` VALUES (1730902, 17309, 14, 0, 100, 3, 15000, 40, 12000, 20000, 11, 12039, 6, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Hellfire Watcher (Normal) - Cast Heal on Friendlies');
DELETE FROM `creature_ai_scripts` WHERE `id`=1730903;
INSERT INTO `creature_ai_scripts` VALUES (1730903, 17309, 14, 0, 100, 5, 50000, 40, 12000, 20000, 11, 30643, 6, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Hellfire Watcher (Heroic) - Cast Heal on Friendlies');
DELETE FROM `creature_ai_scripts` WHERE `id`=1730904;
INSERT INTO `creature_ai_scripts` VALUES (1730904, 17309, 2, 0, 100, 7, 50, 0, 20000, 25000, 11, 8362, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Hellfire Watcher - Cast Renew at 50% HP');