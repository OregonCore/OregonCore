DELETE FROM `script_texts` WHERE `entry`=-1921941;
INSERT INTO `script_texts` VALUES (-1921941, 'Ok lets go!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Therylune Start');
DELETE FROM `script_texts` WHERE `entry`=-1921942;
INSERT INTO `script_texts` VALUES (-1921942, 'I can make it the rest of the way. $n. Thanks', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Therylune End');

DELETE FROM `quest_start_scripts` WHERE `id` IN (945);

UPDATE `quest_template` SET `StartScript` = '0' WHERE `entry` IN (945);

UPDATE `creature_template` SET `ScriptName` = 'npc_therylune' WHERE `entry` IN (3584);

DELETE FROM `script_waypoint` WHERE `entry`=3584;
INSERT INTO `script_waypoint` VALUES (3584, 0, 4520.4, 420.235, 33.5284, 0, 'Therylune');
INSERT INTO `script_waypoint` VALUES (3584, 1, 4512.26, 408.881, 32.9308, 0, 'Therylune');
INSERT INTO `script_waypoint` VALUES (3584, 2, 4507.94, 396.47, 32.9476, 0, NULL);
INSERT INTO `script_waypoint` VALUES (3584, 3, 4507.53, 383.781, 32.995, 0, NULL);
INSERT INTO `script_waypoint` VALUES (3584, 4, 4512.1, 374.02, 33.166, 10000, NULL);
INSERT INTO `script_waypoint` VALUES (3584, 5, 4519.75, 373.241, 33.1574, 0, NULL);
INSERT INTO `script_waypoint` VALUES (3584, 6, 4592.41, 369.127, 31.4893, 0, NULL);
INSERT INTO `script_waypoint` VALUES (3584, 7, 4598.55, 364.801, 31.4947, 0, NULL);
INSERT INTO `script_waypoint` VALUES (3584, 8, 4608.27, 348.403, 31.552, 10000, NULL);
INSERT INTO `script_waypoint` VALUES (3584, 9, 4593.79, 347.895, 35.014, 0, NULL);
INSERT INTO `script_waypoint` VALUES (3584, 10, 4590.23, 350.9, 36.2977, 0, NULL);
INSERT INTO `script_waypoint` VALUES (3584, 11, 4581.5, 348.254, 38.3878, 0, NULL);
INSERT INTO `script_waypoint` VALUES (3584, 12, 4572.05, 348.059, 42.3539, 0, NULL);
INSERT INTO `script_waypoint` VALUES (3584, 13, 4564.75, 344.041, 44.2463, 0, NULL);
INSERT INTO `script_waypoint` VALUES (3584, 14, 4556.63, 341.003, 47.6755, 0, NULL);
INSERT INTO `script_waypoint` VALUES (3584, 15, 4554.38, 334.968, 48.8003, 0, NULL);
INSERT INTO `script_waypoint` VALUES (3584, 16, 4557.63, 329.783, 49.9532, 0, NULL);
INSERT INTO `script_waypoint` VALUES (3584, 17, 4563.32, 316.829, 53.2409, 0, NULL);
INSERT INTO `script_waypoint` VALUES (3584, 18, 4566.09, 303.127, 55.0396, 0, NULL);
INSERT INTO `script_waypoint` VALUES (3584, 19, 4561.65, 295.456, 57.0984, 0, 'Therylune - Say End');
INSERT INTO `script_waypoint` VALUES (3584, 20, 4551.03, 293.333, 57.1534, 0, '');

