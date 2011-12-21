UPDATE `gameobject_template` SET `flags` = '4' WHERE `entry` = '181812';
UPDATE `gameobject_template` SET `flags` = '4' WHERE `entry` = '181822';
UPDATE `gameobject_template` SET `flags` = '4' WHERE `entry` = '181819';
UPDATE `gameobject_template` SET `flags` = '4' WHERE `entry` = '181811';

UPDATE `gameobject_template` SET `flags` = '4' WHERE `entry` = '181821';
UPDATE `gameobject_template` SET `flags` = '4' WHERE `entry` = '181820';
UPDATE `gameobject_template` SET `flags` = '4' WHERE `entry` = '181818';
UPDATE `gameobject_template` SET `flags` = '4' WHERE `entry` = '181817';

UPDATE `gameobject_template` SET `flags` = '4' WHERE `entry` = '181814';
UPDATE `gameobject_template` SET `flags` = '4' WHERE `entry` = '181813';
UPDATE `gameobject_template` SET `flags` = '4' WHERE `entry` = '181816';
UPDATE `gameobject_template` SET `flags` = '4' WHERE `entry` = '181815';



DELETE FROM gameobject WHERE guid = '222300';
INSERT INTO gameobject VALUES (222300, 181982, 542, 1, 455.872, 60.019, 9.61521, 6.21872, 0, 0, 0.032229, -0.99948, 25, 0, 1);

DELETE FROM gameobject WHERE guid = '222301';
INSERT INTO gameobject VALUES (222301, 181982, 542, 2, 455.872, 60.019, 9.61521, 6.21872, 0, 0, 0.032229, -0.99948, 25, 0, 1);

UPDATE `gameobject_template` SET `type` = '1' WHERE `entry` = '181982';

UPDATE `gameobject_template` SET `ScriptName` = 'go_prison_cell_lever' WHERE `entry` = '181982';

UPDATE `creature_template` SET `ScriptName` = 'mob_nascent_orc' WHERE `entry` = '17398';
UPDATE `creature_template` SET `unit_flags` = '33554434' WHERE entry = '17398';
UPDATE `creature_template` SET `flags_extra` = '2' WHERE entry = '17398';
UPDATE `creature_template` SET `unit_flags` = '33554434' WHERE entry = '18612';
UPDATE `creature_template` SET `flags_extra` = '2' WHERE entry = '18612';

UPDATE `creature_template` SET `unit_flags` = '2' WHERE entry = '17380';
UPDATE `creature_template` SET `unit_flags` = '2' WHERE entry = '18601';

UPDATE `creature_template` SET `ScriptName` = 'mob_broggok_poisoncloud' WHERE `entry` = '17662';
UPDATE `creature_template` SET `unit_flags` = '131072' WHERE `entry` = '17662';
UPDATE `creature_template` SET `flags_extra` = '128' WHERE entry = '17662';

DELETE FROM `creature` WHERE `guid` = '85721';
DELETE FROM `creature` WHERE `guid` = '85725';
DELETE FROM `creature` WHERE `guid` = '85724';
DELETE FROM `creature` WHERE `guid` = '85715';
DELETE FROM `creature` WHERE `guid` = '85722';