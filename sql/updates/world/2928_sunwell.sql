/* The Core of Entropius */
UPDATE `creature_template` SET `modelid_A` = 16946, `modelid_A2` = 0, `modelid_H` = 16946, `modelid_H2` = 0, InhabitType = 4 WHERE `entry` = 26262;

/* Boss Portal */
UPDATE `creature_template` SET `modelid_A` = 16946, `modelid_A2` = 0, `modelid_H` = 16946 , `modelid_H2` = 0 WHERE `entry` = 24925;

/* Shattered Sun Riftwaker */
UPDATE `creature_template` SET `minhealth` = 7000, `maxhealth` = 7000, `minmana` = 7200, `maxmana` = 7200, `minlevel` = 70, `maxlevel` = 70 WHERE `entry` = 26289;

/* Lady Liadrin */
UPDATE `creature_template` SET `minhealth` = 1117000, `maxhealth` = 1117000, `minmana` = 31550, `maxmana` = 31550 WHERE `entry` = 26247;

/* Prophet Velen */
UPDATE `creature_template` SET `minhealth` = 1517000, `maxhealth` = 1517000, `minmana` = 50805, `maxmana` = 50805 WHERE `entry` = 26246;

/* Kalecgos */
UPDATE `creature_template` SET `minhealth` = 1850000, `maxhealth` = 1850000, `minmana` = 1693000, `maxmana` = 1693000, `minlevel` = 70, `maxlevel` = 70, InhabitType = 5 WHERE `entry` = 25319;

/* Shattered Sun Soldier */
UPDATE `creature_template` SET `minhealth` = 7335, `maxhealth` = 7335, `minlevel` = 70, `maxlevel` = 70, `equipment_id` = 3005 WHERE `entry` = 26259;
DELETE FROM `creature_equip_template` WHERE `entry` = 3005;
INSERT INTO `creature_equip_template` (`entry`, `equipentry1`, `equipentry2`) VALUES (3005, 38099, 48068);

/* Teleport and Transform */
DELETE FROM `spell_target_position` WHERE `id` = 46473;
INSERT INTO `spell_target_position` VALUES (46473, 580, 1667.30, 633.05, 28.05, 6.22);

UPDATE `script_texts` SET `sound` = 12526 WHERE `entry` = -1580109;