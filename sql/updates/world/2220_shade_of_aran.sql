/* ARAN DB FIXES */
 
UPDATE `spell_script_target` SET `targetEntry` = '17176' WHERE `spell_script_target`.`entry` =29967 AND `spell_script_target`.`type` =1 AND `spell_script_target`.`targetEntry` =16524;
UPDATE `creature_template` SET `MovementType` = '2' WHERE `creature_template`.`entry` =17161;
UPDATE `creature_template` SET `flags_extra` = '386' WHERE `creature_template`.`entry` =17161;
 
DELETE FROM `waypoint_data` WHERE `waypoint_data`.`id` = 90000;
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `delay`, `move_flag`, `action`, `action_chance`, `wpguid`) VALUES
(90000, 1, -11180.222, -1915.070, 232.008, 1500, 1, 0, 100, 0),
(90000, 2, -11173.298, -1918.952, 232.008, 1500, 1, 0, 100, 0),
(90000, 3, -11171.467, -1926.052, 232.008, 1500, 1, 0, 100, 0),
(90000, 4, -11160.875, -1921.755, 232.008, 1500, 1, 0, 100, 0),
(90000, 5, -11153.544, -1921.713, 232.008, 1500, 1, 0, 100, 0),
(90000, 6, -11149.116, -1913.046, 232.008, 1500, 1, 0, 100, 0),
(90000, 7, -11154.374, -1907.144, 232.008, 1500, 1, 0, 100, 0),
(90000, 8, -11157.314, -1898.941, 232.008, 1500, 1, 0, 100, 0),
(90000, 9, -11165.009, -1899.877, 232.008, 1500, 1, 0, 100, 0),
(90000, 10, -11173.726, -1897.136, 232.008, 1500, 1, 0, 100, 0),
(90000, 11, -11177.390, -1905.740, 232.008, 1500, 1, 0, 100, 0);
 
DELETE FROM `creature` WHERE `creature`.`id` = 17161;
 
INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawndist`, `spawntimesecs`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) VALUES
(NULL, 17161, 532, 1, 11686, 0, -11175.6, -1898.68, 232.009, 0.997272, 0, 180, 0, 1, 0, 0, 0),
(NULL, 17168, 532, 1, 15435, 0, -11147.8, -1933.63, 232.092, 2.23402, 0, 180, 0, 2835, 0, 0, 0),
(NULL, 17169, 532, 1, 15435, 0, -11182.4, -1890.14, 232.092, 5.39307, 0, 180, 0, 29, 0, 0, 0),
(NULL, 17170, 532, 1, 15435, 0, -11186.7, -1929.42, 232.092, 0.663225, 0, 180, 0, 2835, 0, 0, 0),
(NULL, 17171, 532, 1, 15435, 0, -11143.2, -1894.7, 232.092, 3.80482, 0, 180, 0, 2835, 0, 0, 0),
(NULL, 17172, 532, 1, 15435, 0, -11168.1, -1939.29, 232.092, 1.46608, 0, 180, 0, 2835, 0, 0, 0),
(NULL, 17173, 532, 1, 15435, 0, -11192.4, -1909.36, 232.092, 6.19592, 0, 180, 0, 29, 0, 0, 0),
(NULL, 17174, 532, 1, 15435, 0, -11161.7, -1885.36, 232.092, 4.59022, 0, 180, 0, 2835, 0, 0, 0),
(NULL, 17175, 532, 1, 15435, 0, -11138.2, -1915.38, 232.092, 3.00197, 0, 180, 0, 2835, 0, 0, 0),
(NULL, 17176, 532, 1, 15435, 0, -11164.8, -1912.01, 232.092, 2.30383, 0, 180, 0, 2835, 0, 0, 0);
 
DELETE FROM `spell_linked_spell` WHERE `spell_linked_spell`.`spell_trigger` = 29969;
DELETE FROM `spell_linked_spell` WHERE `spell_linked_spell`.`spell_trigger` = 30004;
 
INSERT INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `comment`) VALUES
(29969, 29952, 1, 'Shade of Aran Fix'),
(30004, 29946, 0, 'Shade of Aran Fix');
 
DELETE FROM `spell_script_target` WHERE `spell_script_target`.`entry` = 29969;
 
INSERT INTO `spell_script_target` (`entry`, `type`, `targetEntry`) VALUES
(29969, 1, 17161);
 
DELETE FROM `creature_ai_scripts` WHERE `creature_ai_scripts`.`id` = 1716701;
 
INSERT INTO `creature_ai_scripts` (`id`, `creature_id`, `event_type`, `event_inverse_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action1_type`, `action1_param1`, `action1_param2`, `action1_param3`, `action2_type`, `action2_param1`, `action2_param2`, `action2_param3`, `action3_type`, `action3_param1`, `action3_param2`, `action3_param3`, `comment`) VALUES
(1716701, 17167, 0, 0, 100, 0, 1, 1, 1, 1, 11, 37054, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Aran (Waterelemental) cast waterbolt');
