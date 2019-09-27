SET @OGUID:=85056;

-- Add missing objects spawns in Ghostlands & Eversong Woods
DELETE FROM `gameobject` WHERE `guid` BETWEEN @OGUID+0 AND @OGUID+8;
INSERT INTO `gameobject` (`guid`, `id`, `map`, `spawnMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`) VALUES
(@OGUID+1, 181326, 530, 1, 6772.437, -7350.587, 48.96166, 3.141593, 0, 0, 0, 1, 120, 255, 1), -- 181326 (Area: 3517) -- Bonfire
(@OGUID+3, 181327, 530, 1, 6610.271, -7352.757, 54.1082, 3.141593, 0, 0, 0, 1, 120, 255, 1), -- 181327 (Area: 3517) -- Cauldron
(@OGUID+4, 181362, 530, 1, 6773.446, -7150.986, 48.90046, 3.141534, 0, 0, 0, 1, 120, 255, 1), -- 181362 (Area: 0) -- Campfire
(@OGUID+5, 181352, 530, 1, 6785.604, -7192.486, 25.77958, 3.141593, 0, 0, 0, 1, 120, 255, 1), -- 181352 (Area: 0) -- Campfire
(@OGUID+6, 181319, 530, 1, 7150.752, -6823.163, 43.91896, 3.141326, 0, 0, 0, 1, 120, 255, 1); -- 181319 (Area: 0) -- Campfire

