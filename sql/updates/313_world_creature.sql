-- Start Spawn delete (to many npc's spwaned)
DELETE FROM creature WHERE guid in (62852, 86374, 62856, 62857);
-- Start Spawn position
DELETE FROM creature WHERE guid in (62854, 62855, 85660, 85661, 85659, 86451, 85658, 86450, 85656);
INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) VALUES
(62854, 17397, 542, 3, 0, 0, 30.042412, -89.343559, -40.701576, 2.251515, 7200, 0, 0, 11965, 2486, 0, 0),
(62855, 17397, 542, 3, 0, 0, 23.339996, -80.711876, -41.058521, 5.356349, 7200, 0, 0, 11965, 2486, 0, 0),
(85660, 17371, 542, 3, 0, 0, 22.785934, -89.130089, -41.011791, 0.881933, 7200, 0, 0, 10472, 5875, 0, 0),
(85661, 17371, 542, 3, 0, 0, 30.219334, -81.765175, -40.854816, 3.958347, 7200, 0, 0, 10472, 5875, 0, 0),
(85659, 17371, 542, 3, 0, 0, 7.350070, -88.739212, -41.329384, 2.228894, 7200, 0, 0, 10472, 5875, 0, 0),
(86451, 17624, 542, 3, 0, 0, 0.667157, -88.670082, -41.331841, 0.695013, 7200, 0, 0, 16023, 0, 0, 0),
(85658, 17371, 542, 3, 0, 0, 0.307972, -82.372444, -41.330700, 5.777325, 7200, 0, 0, 10472, 5878, 0, 0),
(86450, 17624, 542, 3, 0, 0, 7.557918, -82.270226, -41.330780, 3.817761, 7200, 0, 0, 16023, 0, 0, 0),
(85656, 17371, 542, 3, 0, 0, -16.737885, -83.297951, -41.334915, 6.216362, 7200, 0, 0, 10472, 5878, 0, 0);
-- rogue spawns
DELETE FROM creature WHERE guid in (86448, 86449); 
INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) VALUES
(86448, 17491, 542, 3, 0, 0, 90.761246, -75.982018, -21.790541, 4.700543, 7200, 0, 0, 7497, 0, 0, 0),
(86449, 17491, 542, 3, 0, 0, 121.554512, -90.037743, -7.579343, 1.503976, 7200, 0, 0, 7497, 0, 0, 0);
-- warlock spawns
DELETE FROM creature WHERE guid in (85662, 85663, 85666, 85667); 
INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) VALUES
(85662, 17371, 542, 3, 0, 0, 231.622818, -103.588478, 9.620442, 1.815211, 7200, 0, 0, 10472, 5878, 0, 0),
(85663, 17371, 542, 3, 0, 0, 230.550308, -98.693748, 9.617454, 5.156088, 7200, 0, 0, 10472, 5878, 0, 0),
(85666, 17371, 542, 3, 0, 0, 222.211090, -69.522270, 9.616614, 5.203213, 7200, 0, 0, 10472, 5878, 0, 0),
(85667, 17371, 542, 3, 0, 0, 225.588425, -75.883057, 9.620467, 2.002712, 7200, 0, 0, 10472, 5878, 0, 0);