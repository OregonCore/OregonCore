-- ====================================================================
-- Pathing for Shattrath - Thanks to Malcrom
-- ====================================================================
-- Pathing for Captain Dranarus Entry: 25138
SET @NPC := 96619;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-1842.622,`position_y`=5313.065,`position_z`=-12.42815 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`bytes2`,`mount`,`auras`) VALUES (@NPC,@PATH,1,0, '');
DELETE FROM `waypoint_scripts` WHERE `id` BETWEEN 10 AND 14;
INSERT INTO `waypoint_scripts` (`id`,`delay`,`command`,`datalong`,`guid`) VALUES
(10,0,1,5,10),(11,0,1,18,11),(12,0,1,21,12),(13,0,1,273,13),(14,0,1,22,14),(14,0,16,9739,15);
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-1842.622,5313.065,-12.42815,0,2000,0,0,100,0),
(@PATH,2,-1842.622,5313.065,-12.42815,2.478368,4000,0,50,100,0), -- OneShotPoint (25)
(@PATH,3,-1842.622,5313.065,-12.42815,0,4000,0,1006,100,0), -- OneShotQuestion (6)
(@PATH,4,-1842.622,5313.065,-12.42815,0,12000,0,14,100,0), -- OneShotShot (22) -- play sound 9739
(@PATH,5,-1842.622,5313.065,-12.42815,0,4000,0,11,100,0), -- OneShotCry (18)
(@PATH,6,-1850.026,5310.596,-12.42815,0,0,0,0,100,0),
(@PATH,7,-1852.461,5313.779,-12.42815,0,0,0,0,100,0),
(@PATH,8,-1851.368,5318.353,-12.42815,0,0,0,0,100,0),
(@PATH,9,-1843.579,5320.41,-12.42815,0,0,0,0,100,0),
(@PATH,10,-1840.255,5319.294,-12.42815,0,1000,0,0,100,0),
(@PATH,11,-1840.255,5319.294,-12.42815,4.502949,4000,0,50,100,0), -- OneShotPoint (25)
(@PATH,12,-1840.255,5319.294,-12.42815,0,13000,0,10,100,0), -- OneShotExclamation (5)
(@PATH,13,-1840.255,5319.294,-12.42815,0,4000,0,12,100,0), -- OneShotApplaud (21)
(@PATH,14,-1835.1,5320.241,-12.42815,0,0,0,0,100,0),
(@PATH,15,-1835.244,5316.951,-12.42815,0,1000,0,0,100,0),
(@PATH,16,-1835.23,5317.261,-12.42815,3.141593,20000,0,0,100,0),
(@PATH,17,-1835.23,5317.261,-12.42815,0,11000,0,14,100,0), -- OneShotShot (22) -- play sound 9739
(@PATH,18,-1840.227,5311.866,-12.42815,0,0,0,0,100,0),
(@PATH,19,-1842.643,5312.999,-12.42815,0,1000,0,0,100,0),
(@PATH,20,-1842.643,5312.999,-12.42815,1.239184,4000,0,50,100,0), -- OneShotPoint (25)
(@PATH,21,-1842.643,5312.999,-12.42815,0,7000,0,1012,100,0), -- OneShotNo (274)
(@PATH,22,-1842.643,5312.999,-12.42815,0,3000,0,50,100,0), -- OneShotPoint (25)
(@PATH,23,-1842.643,5312.999,-12.42815,0,7000,0,484,100,0), -- OneShotLaugh (11)
(@PATH,24,-1847.878,5310.522,-12.42815,0,0,0,0,100,0),
(@PATH,25,-1851.1,5314.04,-12.42815,0,1000,0,0,100,0),
(@PATH,26,-1851.1,5314.04,-12.42815,0.08726646,5000,0,0,100,0),
(@PATH,27,-1851.1,5314.04,-12.42815,0,21000,0,14,100,0), -- OneShotShot (22) -- play sound 9739
(@PATH,28,-1846.24,5319.435,-12.42815,0,0,0,0,100,0),
(@PATH,29,-1843.72,5318.517,-12.42815,0,0,0,0,100,0),
(@PATH,30,-1843.72,5318.517,-12.42815,5.445427,7000,0,50,100,0), -- OneShotPoint (25)
(@PATH,31,-1843.72,5318.517,-12.42815,0,4000,0,13,100,0), -- OneShotYes (273)
(@PATH,32,-1843.72,5318.517,-12.42815,0,11000,0,12,100,0), -- OneShotApplaud (21)
(@PATH,33,-1836.766,5321.29,-12.42815,0,0,0,0,100,0),
(@PATH,34,-1835.244,5316.951,-12.42815,0,21000,0,0,100,0),
(@PATH,35,-1835.244,5316.951,-12.42815,3.141593,10000,0,14,100,0), -- OneShotShot (22) -- play sound 9739
(@PATH,36,-1840.227,5311.866,-12.42815,0,0,0,0,100,0);

-- Skyguard Neather Ray Should not be moving
UPDATE `creature` SET `spawndist`=0, `MovementType`=0 WHERE guid=79011;

-- Add missing Shattrath City Peacekeeper
DELETE FROM `creature` WHERE `guid`=45075;
INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `MovementType`) VALUES
(45075, 19687, 530, 1, -1976.909, 5112.424, 7.589328, 4.276057, 120, 0, 0);

-- Pathing for Shattrath City Peacekeeper Entry: 19687
SET @NPC := 70228;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-1966.495,`position_y`=5121.907,`position_z`=10.32379 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`bytes2`,`mount`,`auras`) VALUES (@NPC,@PATH,1,17063, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-1966.495,5121.907,10.32379,0,0,0,0,100,0),
(@PATH,2,-1951.699,5166.294,17.25317,0,0,0,0,100,0),
(@PATH,3,-1938.637,5205.161,15.87822,0,0,0,0,100,0),
(@PATH,4,-1931.686,5226.178,13.69076,0,0,0,0,100,0),
(@PATH,5,-1916.49,5271.667,3.115327,0,0,0,0,100,0),
(@PATH,6,-1907.977,5296.765,1.137226,0,0,0,0,100,0),
(@PATH,7,-1901.641,5315.616,0.8860542,0,0,0,0,100,0),
(@PATH,8,-1907.977,5296.765,1.137226,0,0,0,0,100,0),
(@PATH,9,-1916.49,5271.667,3.115327,0,0,0,0,100,0),
(@PATH,10,-1931.686,5226.178,13.69076,0,0,0,0,100,0),
(@PATH,11,-1938.637,5205.161,15.87822,0,0,0,0,100,0),
(@PATH,12,-1951.699,5166.294,17.25317,0,0,0,0,100,0);

-- Pathing for Vekax Entry: 22429
SET @NPC := 78783;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-1668.65,`position_y`=5531.648,`position_z`=2.084388 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`bytes2`,`mount`,`auras`) VALUES (@NPC,@PATH,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-1668.65,5531.648,2.084388,0,0,0,0,100,0),
(@PATH,2,-1675.535,5539.932,4.808745,0,0,0,0,100,0),
(@PATH,3,-1669.465,5544.53,5.413347,0,0,0,0,100,0),
(@PATH,4,-1656.403,5534.61,6.1091,0,0,0,0,100,0),
(@PATH,5,-1642.701,5537.171,6.172705,0,0,0,0,100,0),
(@PATH,6,-1641.732,5532.27,5.289427,0,0,0,0,100,0),
(@PATH,7,-1638.772,5516.784,-3.276233,0,0,0,0,100,0),
(@PATH,8,-1633.397,5489.191,-14.53821,0,0,0,0,100,0),
(@PATH,9,-1629.011,5484.991,-14.49736,0,0,0,0,100,0),
(@PATH,10,-1624.565,5486.951,-14.3688,0,0,0,0,100,0),
(@PATH,11,-1615.755,5487.401,-14.04851,0,0,0,0,100,0),
(@PATH,12,-1606.827,5480.522,-14.09892,0,0,0,0,100,0),
(@PATH,13,-1602.518,5469.729,-14.09986,0,0,0,0,100,0),
(@PATH,14,-1604.27,5462.537,-14.10011,0,0,0,0,100,0),
(@PATH,15,-1612.627,5453.875,-14.09042,0,0,0,0,100,0),
(@PATH,16,-1623.32,5451.606,-14.07838,0,0,0,0,100,0),
(@PATH,17,-1633.862,5457.565,-14.19732,0,0,0,0,100,0),
(@PATH,18,-1637.261,5469.756,-14.69115,0,0,0,0,100,0),
(@PATH,19,-1632.431,5481.022,-14.52123,0,0,0,0,100,0),
(@PATH,20,-1637.261,5469.756,-14.69115,0,0,0,0,100,0),
(@PATH,21,-1633.862,5457.565,-14.19732,0,0,0,0,100,0),
(@PATH,22,-1623.32,5451.606,-14.07838,0,0,0,0,100,0),
(@PATH,23,-1612.843,5453.652,-14.09002,0,0,0,0,100,0),
(@PATH,24,-1604.27,5462.537,-14.10011,0,0,0,0,100,0),
(@PATH,25,-1602.518,5469.729,-14.09986,0,0,0,0,100,0),
(@PATH,26,-1606.827,5480.522,-14.09892,0,0,0,0,100,0),
(@PATH,27,-1615.755,5487.401,-14.04851,0,0,0,0,100,0),
(@PATH,28,-1624.565,5486.951,-14.3688,0,0,0,0,100,0),
(@PATH,29,-1629.011,5484.991,-14.49736,0,0,0,0,100,0),
(@PATH,30,-1633.36,5489.002,-14.55791,0,0,0,0,100,0),
(@PATH,31,-1638.772,5516.784,-3.276233,0,0,0,0,100,0),
(@PATH,32,-1641.732,5532.27,5.289427,0,0,0,0,100,0),
(@PATH,33,-1642.701,5537.171,6.172705,0,0,0,0,100,0),
(@PATH,34,-1656.217,5534.645,6.133076,0,0,0,0,100,0),
(@PATH,35,-1669.465,5544.53,5.413347,0,0,0,0,100,0),
(@PATH,36,-1675.535,5539.932,4.808745,0,0,0,0,100,0),
(@PATH,37,-1668.774,5531.669,2.197366,0,0,0,0,100,0),
(@PATH,38,-1648.668,5528.287,-5.18554,0,0,0,0,100,0);

-- Pathing for Skettis Outcast Entry: 19075
SET @NPC := 68385;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-1622.079,`position_y`=5473.997,`position_z`=-4.175363 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`bytes2`,`mount`,`auras`) VALUES (@NPC,@PATH,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-1622.079,5473.997,-4.175363,0,0,0,0,100,0),
(@PATH,2,-1624.692,5470.754,-4.14517,0,0,0,0,100,0),
(@PATH,3,-1624.417,5467.651,-4.055672,0,0,0,0,100,0),
(@PATH,4,-1622.095,5464.667,-4.021505,0,0,0,0,100,0),
(@PATH,5,-1618.157,5464.342,-4.122393,0,0,0,0,100,0),
(@PATH,6,-1614.648,5466.909,-3.965191,0,0,0,0,100,0),
(@PATH,7,-1614.635,5471.312,-4.240356,0,0,0,0,100,0),
(@PATH,8,-1618.361,5474.586,-3.982594,0,0,0,0,100,0);

-- =================================================
-- Missing data for TBC instance portals
-- =================================================
-- Note: the two numbers in comments at the end of new data are the WoTLK 3.x rotation2/rotation3 values (unverified ingame).
DELETE FROM gameobject WHERE guid BETWEEN 26341 AND 26381;
-- Hellfire Citadel: Hellfire Ramparts
-- ------------------------------------
-- Source: 3.0.3, build 9183 sniffs
INSERT INTO gameobject (guid, id, map, spawnMask, position_x, position_y, position_z, orientation, rotation0, rotation1, rotation2, rotation3) VALUES
(26341, 184127, 530, 1, -364.502, 3081.91, -14.7253, 1.84872, 0, 0, sin(1.84872/2), cos(1.84872/2)), -- Normal, outside          0.785317, 0.619094
(26342, 184179, 543, 3, -1357.39, 1636.75, 65.543, 1.01586, 0, 0, sin(1.01586/2), cos(1.01586/2)),   -- Normal, inside instance  0.657, 0.75389
(26343, 184180, 543, 3, -1357.4, 1636.72, 65.5564, 1.01896, 0, 0, sin(1.01896/2), cos(1.01896/2));   -- Heroic, inside instance  0.657, 0.75389
-- Hellfire Citadel: The Blood Furnace
-- ------------------------------------
-- Source: 3.0.3, build 9183 sniffs
INSERT INTO gameobject (guid, id, map, spawnMask, position_x, position_y, position_z, orientation, rotation0, rotation1, rotation2, rotation3) VALUES
(26344, 184132, 530, 1, -305.563, 3167.28, 31.0006, 2.15924, 0, 0, sin(2.15924/2), cos(2.15924/2)),      -- Heroic, outside          0.785317, 0.619094
(26345, 184175, 542, 3, -0.626152, 25.2928, -45.1701, -1.9627, 0, 0, sin(-1.9627/2), cos(-1.9627/2)),    -- Normal, inside instance  1, 0
(26346, 184176, 542, 3, -0.629567, 25.3203, -45.1592, -1.95961, 0, 0, sin(-1.95961/2), cos(-1.95961/2)); -- Heroic, inside instance  1, 0
-- Hellfire Citadel: The Shattered Halls
-- --------------------------------------
-- Source: 3.0.3, build 9183 sniffs (outside) and 3.1.2, build 9901 sniffs (inside)
INSERT INTO gameobject (guid, id, map, spawnMask, position_x, position_y, position_z, orientation, rotation0, rotation1, rotation2, rotation3) VALUES
(26347, 184129, 530, 1, -310.372, 3087.27, -4.18958, 1.38937, 0, 0, sin(1.38937/2), cos(1.38937/2)),     -- Heroic, outside          0.785317, 0.619094
(26348, 184177, 540, 3, -42.0439, -26.523, -14.1031, -1.70302, 0, 0, sin(-1.70302/2), cos(-1.70302/2)),  -- Normal, inside instance  1, 0
(26349, 184178, 540, 3, -42.0505, -26.5066, -14.0962, -1.69993, 0, 0, sin(-1.69993/2), cos(-1.69993/2)); -- Heroic, inside instance  1, 0
-- Coilfang Reservoir: The Slave Pens
-- -----------------------------------
-- Source: 3.0.3, build 9183 sniffs
INSERT INTO gameobject (guid, id, map, spawnMask, position_x, position_y, position_z, orientation, rotation0, rotation1, rotation2, rotation3) VALUES
(26350, 184211, 530, 1, 740.112, 7014.18, -70.4291, 0, 0, 0, 0, 1),                                 -- Normal, outside          1, 0
(26351, 184210, 530, 1, 740.112, 7014.18, -70.4291, 0, 0, 0, 0, 1),                                 -- Heroic, outside          1, 0
(26352, 184197, 547, 3, 120.902, -137.014, 1.05554, 1.57079, 0, 0, sin(1.57079/2), cos(1.57079/2)), -- Heroic, inside instance  1, 0
(26353, 184198, 547, 3, 120.902, -137.014, 1.05554, 1.57079, 0, 0, sin(1.57079/2), cos(1.57079/2)); -- Normal, inside instance  1, 0
-- Coilfang Reservoir: The Underbog
-- ---------------------------------
-- Source: 3.0.3, build 9183 sniffs
INSERT INTO gameobject (guid, id, map, spawnMask, position_x, position_y, position_z, orientation, rotation0, rotation1, rotation2, rotation3) VALUES
(26354, 184206, 530, 1, 781.97, 6745.99, -72.4238, 1.65806, 0, 0, sin(1.65806/2), cos(1.65806/2)),   -- Normal, outside          1, 0
(26355, 184200, 546, 3, 5.22929, -12.1156, -2.63621, 2.09439, 0, 0, sin(2.09439/2), cos(2.09439/2)), -- Normal, inside instance  1, 0
(26356, 184199, 546, 3, 5.22929, -12.1156, -2.63621, 2.09439, 0, 0, sin(2.09439/2), cos(2.09439/2)); -- Heroic, inside instance  1, 0
-- Coilfang Reservoir: The Steamvault
-- -----------------------------------
-- Source: 3.0.3, build 9183 sniffs
INSERT INTO gameobject (guid, id, map, spawnMask, position_x, position_y, position_z, orientation, rotation0, rotation1, rotation2, rotation3) VALUES
(26357, 184208, 530, 1, 818.386, 6947.67, -79.147, 1.5708, 0, 0, sin(1.5708/2), cos(1.5708/2)), -- Normal, outside  1, 0
(26358, 184209, 530, 1, 818.386, 6947.67, -79.147, 1.5708, 0, 0, sin(1.5708/2), cos(1.5708/2)); -- Heroic, outside  1, 0
-- The portals inside the instance are present, but they are not showing in heroic mode because of a wrong spawnmask.
UPDATE gameobject SET spawnMask = 3 WHERE guid IN (11952, 12082);
-- Auchindoun: Mana-Tombs
-- -----------------------
-- Source: 3.0.3, build 9183 sniffs
INSERT INTO gameobject (guid, id, map, spawnMask, position_x, position_y, position_z, orientation, rotation0, rotation1, rotation2, rotation3) VALUES
(26359, 184181, 530, 1, -3073.97, 4943.09, -102.055, 0, 0, 0, 0, 1), -- Normal, outside   1, 0
(26360, 184182, 530, 1, -3073.97, 4943.09, -102.055, 0, 0, 0, 0, 1); -- Heroic, outside   1, 0
-- Auchindoun: Auchenai Crypts
-- ----------------------------
-- Source: 3.0.3, build 9183 sniffs
INSERT INTO gameobject (guid, id, map, spawnMask, position_x, position_y, position_z, orientation, rotation0, rotation1, rotation2, rotation3) VALUES
(26361, 184185, 530, 1, -3362, 5230.92, -102.056, 1.5708, 0, 0, sin(1.5708/2), cos(1.5708/2)), -- Normal, outside          1, 0
(26362, 184186, 530, 1, -3362, 5230.92, -102.056, 1.5708, 0, 0, sin(1.5708/2), cos(1.5708/2)), -- Heroic, outside          1, 0
(26363, 184192, 558, 3, -28.5773, 0.158444, -1.18062, 3.14159, 0, 0, 1, 0),                    -- Normal, inside instance  1, 0
(26364, 184191, 558, 3, -28.5773, 0.158444, -1.18062, 3.14159, 0, 0, 1, 0);                    -- Heroic, inside instance  1, 0
-- Auchindoun: Sethekk Halls
-- --------------------------
-- Source: 3.0.3, build 9183 sniffs
INSERT INTO gameobject (guid, id, map, spawnMask, position_x, position_y, position_z, orientation, rotation0, rotation1, rotation2, rotation3) VALUES
(26365, 184183, 530, 1, -3361.93, 4655.23, -102.056, -1.5708, 0, 0, sin(-1.5708/2), cos(-1.5708/2)); -- Normal, outside   1, 0
-- The portals inside the instance are present, but according to my sniffs the data is slightly off.
-- This can be confirmed ingame in heroic mode: the skull was not centered.
UPDATE gameobject SET position_x = -8.53784, position_y = -0.0184067, position_z = -1.0496, orientation = 0, rotation2 = 1, rotation3 = 0 WHERE guid IN (112784, 112783);
-- Auchindoun: Shadow Labyrinth
-- -----------------------------
-- Source: 3.0.3, build 9183 sniffs
INSERT INTO gameobject (guid, id, map, spawnMask, position_x, position_y, position_z, orientation, rotation0, rotation1, rotation2, rotation3) VALUES
(26366, 184188, 530, 1, -3649.94, 4943.07, -102.091, 3.14159, 0, 0, 1, 0), -- Normal, outside  1, 0
(26367, 184187, 530, 1, -3649.94, 4943.07, -102.091, 3.14159, 0, 0, 1, 0); -- Heroic, outside  1, 0
-- The portals inside the instance are present, but they are not showing in heroic mode because of a wrong spawnmask.
-- Also correct a slightly off rotation.
UPDATE gameobject SET spawnMask = 3, rotation3 = 0 WHERE guid IN (25061, 25062);
-- Caverns of Time: Old Hillsbrad Foothills
-- -----------------------------------------
-- All OK.
-- Caverns of Time: The Black Morass
-- ----------------------------------
-- Source: 3.0.3, build 9183 sniffs
INSERT INTO gameobject (guid, id, map, spawnMask, position_x, position_y, position_z, orientation, rotation0, rotation1, rotation2, rotation3) VALUES
(26368, 184171, 269, 3, -1493.51, 7030.03, 35.5439, 1.39482, 0, 0, sin(1.39482/2), cos(1.39482/2)), -- Normal, inside instance  0.5913099, 0.8064444
(26369, 184172, 269, 3, -1493.51, 7030.03, 35.5439, 1.39482, 0, 0, sin(1.39482/2), cos(1.39482/2)); -- Heroic, inside instance  0.5913099, 0.8064444
-- Tempest Keep: The Mechanar
-- ---------------------------
-- Source: 3.0.9, build 9551 sniffs
INSERT INTO gameobject (guid, id, map, spawnMask, position_x, position_y, position_z, orientation, rotation0, rotation1, rotation2, rotation3) VALUES
(26370, 184220, 530, 1, 2861.24, 1544.99, 252.083, 0.733038, 0, 0, sin(0.733038/2), cos(0.733038/2)), -- Normal, outside                  0.35837, 0.93358
(26371, 184219, 530, 1, 2861.24, 1544.99, 252.083, 0.733038, 0, 0, sin(0.733038/2), cos(0.733038/2)), -- Heroic, outside                  0.35837, 0.93358
(26372, 184227, 554, 3, -38.0125, 0.10273, 0.504373, 3.14159, 0, 0, 1, 0),                            -- Normal, inside instance          1, 0
(26373, 184228, 554, 3, -38.0125, 0.10273, 0.504373, 3.14159, 0, 0, 1, 0),                            -- Heroic, inside instance          1, 0
(26374, 184225, 554, 3, 73.9475, 149.684, 27.7701, 3.14159, 0, 0, 1, 0),                              -- Normal, inside instance, at end  1, 0
(26375, 184226, 554, 3, 73.9475, 149.684, 27.7701, 3.14159, 0, 0, 1, 0);                              -- Heroic, inside instance, at end  1, 0
-- Tempest Keep: The Botanica
-- ---------------------------
-- Source: 3.0.9, build 9551 sniffs (outside) and 3.1.1, build 9835 sniffs (inside)
INSERT INTO gameobject (guid, id, map, spawnMask, position_x, position_y, position_z, orientation, rotation0, rotation1, rotation2, rotation3) VALUES
(26376, 184223, 530, 1, 3415.27, 1481.96, 182.74, 2.49582, 0, 0, sin(2.49582/2), cos(2.49582/2)), -- Normal, outside                  0.94832, 0.3173
(26377, 184217, 553, 3, 159.893, 391.743, -34.6478, 3.14159, 0, 0, 1, 0),                         -- Normal, inside instance, at end  1, 0
(26378, 184218, 553, 3, 159.893, 391.743, -34.6478, 3.14159, 0, 0, 1, 0);                         -- Heroic, inside instance, at end  1, 0
-- Tempest Keep: The Arcatraz
-- ---------------------------
-- Source: 3.0.9, build 9551 sniffs
INSERT INTO gameobject (guid, id, map, spawnMask, position_x, position_y, position_z, orientation, rotation0, rotation1, rotation2, rotation3) VALUES
(26379, 184221, 530, 1, 3312.75, 1330.01, 505.652, 1.94604, 0, 0, sin(1.94604/2), cos(1.94604/2)), -- Normal, outside  0.82659, 0.5628
(26380, 184222, 530, 1, 3312.75, 1330.01, 505.652, 1.94604, 0, 0, sin(1.94604/2), cos(1.94604/2)); -- Heroic, outside  0.82659, 0.5628
-- Magisters' Terrace
-- -------------------
-- Source: 3.0.9, build 9551 sniffs
INSERT INTO gameobject (guid, id, map, spawnMask, position_x, position_y, position_z, orientation, rotation0, rotation1, rotation2, rotation3) VALUES
(26381, 188178, 530, 1, 12882.4, -7341.65, 67.3815, 1.09083, 0, 0, sin(1.09083/2), cos(1.09083/2)); -- Heroic, outside  0.51877, 0.85491


-- ***** Gruul *****
-- Gruul (fix doors)
UPDATE gameobject_template SET faction=1375 WHERE entry IN (183817,184662);
UPDATE gameobject SET id=183817 WHERE guid=12443;
DELETE FROM gameobject WHERE guid=25333;

-- ***** Karazhan *****
UPDATE gameobject_template SET flags=flags|16 WHERE entry=185119;
UPDATE gameobject_template SET faction=1375, flags=flags|32 WHERE entry IN (184278,184279,183932,184277,185134);
UPDATE creature_template SET faction_A=114, faction_H=114, unit_flags=unit_flags|33555200 WHERE entry=17644;
