-- Spawn Rift Spawns
SET @GHOST := 85732;
DELETE FROM `creature` WHERE `id`=6492 AND `guid` BETWEEN @GHOST AND @GHOST+7;
INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`) VALUES
-- Undercity
(@GHOST, 6492, 0, 1, 0, 0, 1414.51, 365.095, -66.0066, 4.99293, 300, 0, 0, 420, 0, 0),
(@GHOST+1, 6492, 0, 1, 0, 0, 1406.47, 369.853, -66.0231, 5.29334, 300, 0, 0, 420, 0, 0),
(@GHOST+2, 6492, 0, 1, 0, 0, 1400.28, 366.915, -66.0388, 5.57844, 300, 0, 0, 420, 0, 0),
(@GHOST+3, 6492, 0, 1, 0, 0, 1399.22, 362.385, -66.0382, 5.77244, 300, 0, 0, 420, 0, 0),
-- Stormwind
(@GHOST+4, 6492, 0, 1, 0, 0, -9090.36, 838.109, 115.667, 0.231248, 300, 0, 0, 420, 0, 0),
(@GHOST+5, 6492, 0, 1, 0, 0, -9081.7, 817.472, 115.667, 0.394612, 300, 0, 0, 420, 0, 0),
(@GHOST+6, 6492, 0, 1, 0, 0, -9090.38, 818.76, 115.667, 0.456658, 300, 0, 0, 420, 0, 0),
(@GHOST+7, 6492, 0, 1, 0, 0, -9094.42, 828.962, 115.667, 0.270519, 300, 0, 0, 420, 0, 0);

-- Correctly set mana rift object to temp mana rift and change position
UPDATE `gameobject` SET `id` = 300013 WHERE `guid` = 42889;
UPDATE `gameobject` SET `position_x` = -9086.26, `position_y` = 827.573, `position_z` =  115.667 WHERE `guid` = 42889;

-- Assign scripts
UPDATE `creature_template` SET `ScriptName` = "mob_rift_spawn" WHERE `entry` = 6492;
UPDATE `gameobject_template` SET `ScriptName` = "go_containment_coffer" WHERE `entry` = 122088;
UPDATE `item_template` SET `ScriptName` = "item_chest_of_containment_coffers" WHERE `entry` = 7247;
