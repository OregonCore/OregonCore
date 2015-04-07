-- Delete incorrect spawns of treant ally
DELETE FROM creature WHERE id = 5806;
DELETE FROM creature_addon WHERE guid IN (29974, 29975, 29976, 29977, 29978, 29979);

-- Fix startup error
DELETE FROM creature_formations WHERE memberGUID=23427;

DELETE FROM `creature_formations` WHERE `leaderGUID`=59379;
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES
(59379, 59379, 0, 0, 2),
(59379, 59378, 1, 270, 2);

DELETE FROM `creature_formations` WHERE `leaderGUID`=59461;
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES
(59461, 59461, 0, 0, 2),
(59461, 58902, 3, 60, 2),
(59461, 58903, 3, 300, 2);

DELETE FROM `creature_formations` WHERE `leaderGUID`=62071;
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES
(62071, 62071, 0, 0, 2),
(62071, 62072, 3, 270, 2);

DELETE FROM `creature_formations` WHERE `leaderGUID`=62078;
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES
(62078, 62078, 0, 0, 2),
(62078, 62079, 3, 270, 2);

-- Pathing for Honor Hold Cavalryman Entry: 16843 'ODB FORMAT' 
SET @NPC := 57965;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-693.6425,`position_y`=2675.357,`position_z`=93.08069 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,2410,0,0,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-693.6425,2675.357,93.08069,0,0,0,0,100,0),
(@PATH,2,-678.617,2653.904,90.11584,0,0,0,0,100,0),
(@PATH,3,-670.0779,2623.337,87.22372,0,0,0,0,100,0),
(@PATH,4,-647.809,2588.676,83.14737,0,0,0,0,100,0),
(@PATH,5,-620.405,2546.836,73.68471,0,0,0,0,100,0),
(@PATH,6,-604.7114,2503.37,64.40894,0,0,0,0,100,0),
(@PATH,7,-603.8063,2468.986,57.10122,0,0,0,0,100,0),
(@PATH,8,-604.4416,2435.113,50.43313,0,0,0,0,100,0),
(@PATH,9,-599.5267,2409.476,44.74646,0,0,0,0,100,0),
(@PATH,10,-584.7905,2406.665,43.70581,0,0,0,0,100,0),
(@PATH,11,-582.3721,2430.458,47.94434,0,0,0,0,100,0),
(@PATH,12,-582.2322,2463.321,53.99577,0,0,0,0,100,0),
(@PATH,13,-583.6086,2498.498,61.39573,0,0,0,0,100,0),
(@PATH,14,-558.1572,2532.4,67.07269,0,0,0,0,100,0),
(@PATH,15,-524.5069,2553.839,65.79486,0,0,0,0,100,0),
(@PATH,16,-505.5841,2594.287,68.31036,0,0,0,0,100,0),
(@PATH,17,-504.4444,2604.637,70.43836,0,0,0,0,100,0),
(@PATH,18,-510.0499,2664.531,70.29152,0,0,0,0,100,0),
(@PATH,19,-511.0184,2694.743,68.82625,0,0,0,0,100,0),
(@PATH,20,-517.2742,2716.729,67.8214,0,0,0,0,100,0),
(@PATH,21,-534.5939,2747.733,69.1256,0,0,0,0,100,0),
(@PATH,22,-565.1152,2807.142,63.14902,0,0,0,0,100,0),
(@PATH,23,-596.1111,2836.904,59.59448,0,0,0,0,100,0),
(@PATH,24,-630.4422,2866.988,51.1272,0,0,0,0,100,0),
(@PATH,25,-678.3279,2883.457,47.06744,0,0,0,0,100,0),
(@PATH,26,-723.3876,2887.968,33.10106,0,0,0,0,100,0),
(@PATH,27,-762.0458,2886.646,24.27474,0,0,0,0,100,0),
(@PATH,28,-792.7939,2863.772,21.49371,0,0,0,0,100,0),
(@PATH,29,-819.7803,2843.219,19.12334,0,0,0,0,100,0),
(@PATH,30,-873.9691,2815.151,15.98937,0,0,0,0,100,0),
(@PATH,31,-893.9761,2802.479,14.45792,0,0,0,0,100,0),
(@PATH,32,-897.1367,2771.076,20.99353,0,0,0,0,100,0),
(@PATH,33,-867.2095,2775.483,38.31301,0,0,0,0,100,0),
(@PATH,34,-860.2045,2761.954,45.71127,0,0,0,0,100,0),
(@PATH,35,-868.9539,2736.927,57.97308,0,0,0,0,100,0),
(@PATH,36,-871.5066,2701.324,78.20714,0,0,0,0,100,0),
(@PATH,37,-856.217,2675.711,92.33214,0,0,0,0,100,0),
(@PATH,38,-833.6348,2676.617,98.47594,0,0,0,0,100,0),
(@PATH,39,-808.0863,2687.279,104.1725,0,0,0,0,100,0),
(@PATH,40,-790.8087,2689.64,104.0867,0,0,0,0,100,0),
(@PATH,41,-724.9875,2676.18,96.8718,0,0,0,0,100,0);
-- 0x1C09E442401072C000002000004EA00B .go -693.6425 2675.357 93.08069

DELETE FROM `creature_formations` WHERE `leaderGUID`=57937;
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES
(57937, 57937, 0, 0, 2),
(57937, 57923, 3, 90, 2);

-- Pathing for Honor Hold Defender Entry: 16842 'ODB FORMAT' 
SET @NPC := 57937;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-872.1663,`position_y`=2769.454,`position_z`=37.44319 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,0,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-872.1663,2769.454,37.44319,0,0,0,0,100,0),
(@PATH,2,-869.7395,2768.148,39.13062,0,0,0,0,100,0),
(@PATH,3,-865.2796,2739.821,56.70609,0,0,0,0,100,0),
(@PATH,4,-867.5071,2718.566,68.9836,0,0,0,0,100,0),
(@PATH,5,-867.6776,2699.037,79.59444,0,0,0,0,100,0),
(@PATH,6,-857.8785,2686.67,88.437,0,0,0,0,100,0),
(@PATH,7,-842.5071,2678.96,95.25343,0,0,0,0,100,0),
(@PATH,8,-822.5238,2681.506,102.4034,0,0,0,0,100,0),
(@PATH,9,-817.7297,2683.09,103.48,0,0,0,0,100,0),
(@PATH,10,-817.5452,2683.404,103.4257,0,0,0,0,100,0),
(@PATH,11,-838.7125,2678.288,96.64885,0,0,0,0,100,0),
(@PATH,12,-856.334,2685.094,89.30038,0,0,0,0,100,0),
(@PATH,13,-866.9412,2697.539,80.50797,0,0,0,0,100,0),
(@PATH,14,-867.8522,2713.977,71.4567,0,0,0,0,100,0),
(@PATH,15,-865.3911,2736.797,58.61076,0,0,0,0,100,0),
(@PATH,16,-866.6399,2765.021,41.61932,0,0,0,0,100,0);
-- 0x1C09E4424010728000002D00001E733B .go -872.1663 2769.454 37.44319

-- Pathing for Honor Hold Archer Entry: 16896 'ODB FORMAT' 
SET @NPC := 58441;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-794.1317,`position_y`=2601.671,`position_z`=133.2523 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,0,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-794.1317,2601.671,133.2523,0,0,0,0,100,0),
(@PATH,2,-778.0168,2611.436,133.2532,0,0,0,0,100,0);
-- 0x1C09E4424010800000001F00004BCB3D .go -794.1317 2601.671 133.2523

-- Pathing for Honor Hold Archer Entry: 16896 'ODB FORMAT' 
SET @NPC := 58440;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-800.5278,`position_y`=2648.273,`position_z`=133.2537 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,0,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-800.5278,2648.273,133.2537,0,0,0,0,100,0),
(@PATH,2,-816.2895,2638.862,133.2529,0,0,0,0,100,0);
-- 0x1C09E4424010800000001F00004BCB47 .go -800.5278 2648.273 133.2537

-- Pathing for Honor Hold Archer Entry: 16896 'ODB FORMAT' 
SET @NPC := 58448;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-697.1456,`position_y`=2567.732,`position_z`=101.4374 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,0,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-697.1456,2567.732,101.4374,0,0,0,0,100,0),
(@PATH,2,-683.9911,2574.386,100.8064,0,0,0,0,100,0),
(@PATH,3,-683.9779,2574.394,100.8655,0,0,0,0,100,0),
(@PATH,4,-697.2676,2567.764,101.4445,0,0,0,0,100,0),
(@PATH,5,-682.3236,2567.782,100.8602,0,0,0,0,100,0);
-- 0x1C09E4424010800000001F00014B15DB .go -697.1456 2567.732 101.4374


-- Pathing for Honor Hold Archer Entry: 16896 'ODB FORMAT' 
SET @NPC := 58442;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-750.128,`position_y`=2567.929,`position_z`=104.1502 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,0,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-750.128,2567.929,104.1502,0,0,0,0,100,0),
(@PATH,2,-746.1736,2566.977,104.3655,0,0,0,0,100,0),
(@PATH,3,-732.6676,2565.869,104.5501,0,0,0,0,100,0),
(@PATH,4,-754.0043,2574.421,104.1675,0,0,0,0,100,0),
(@PATH,5,-754.0061,2574.378,104.2026,0,0,0,0,100,0),
(@PATH,6,-732.6328,2565.721,104.5407,0,0,0,0,100,0),
(@PATH,7,-746.1768,2566.991,104.2605,0,0,0,0,100,0);
-- 0x1C09E4424010800000001F00024B15DA .go -750.128 2567.929 104.1502

-- Pathing for Honor Hold Archer Entry: 16896 'ODB FORMAT' 
SET @NPC := 58452;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-674.274,`position_y`=2787.06,`position_z`=109.5986 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,0,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-674.274,2787.06,109.5986,0,0,0,0,100,0),
(@PATH,2,-661.9408,2782.68,107.5661,0,0,0,0,100,0),
(@PATH,3,-645.2046,2773.514,104.8345,0,0,0,0,100,0),
(@PATH,4,-664.4965,2777.165,107.6214,0,0,0,0,100,0),
(@PATH,5,-664.445,2776.807,107.7074,0,0,0,0,100,0),
(@PATH,6,-645.4469,2773.186,104.7909,0,0,0,0,100,0);
-- 0x1C09E4424010800000001F0003CB15DA .go -674.274 2787.06 109.5986

-- Pathing for Honor Hold Archer Entry: 16896 'ODB FORMAT' 
SET @NPC := 58453;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-607.841,`position_y`=2612.785,`position_z`=99.32381 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,0,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-607.841,2612.785,99.32381,0,0,0,0,100,0),
(@PATH,2,-604.588,2617.701,99.53741,0,0,0,0,100,0),
(@PATH,3,-605.6599,2620.353,99.11266,0,0,0,0,100,0),
(@PATH,4,-605.5408,2620.273,98.99216,0,0,0,0,100,0),
(@PATH,5,-604.4185,2617.526,99.5359,0,0,0,0,100,0);
-- 0x1C09E4424010800000002000004E9D75 .go -607.841 2612.785 99.32381

-- Pathing for Dreadcaller Entry: 19434 'ODB FORMAT' 
SET @NPC := 69502;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=177.5729,`position_y`=2268.238,`position_z`=48.5948 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,0,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,177.5729,2268.238,48.5948,0,0,0,0,100,0),
(@PATH,2,206.0571,2228.519,54.04856,0,0,0,0,100,0);
-- 0xF1304BEA00397367 .go 177.5729 2268.238 48.5948

-- Pathing for Dreadcaller Entry: 19434 'ODB FORMAT' 
SET @NPC := 69501;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=190.7178,`position_y`=2286.323,`position_z`=49.66181 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,0,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,190.7178,2286.323,49.66181,0,0,0,0,100,0),
(@PATH,2,210.6431,2285.845,53.60326,0,0,0,0,100,0),
(@PATH,3,222.5459,2286.666,56.13475,0,0,0,0,100,0),
(@PATH,4,231.5569,2296.63,60.31371,0,0,0,0,100,0),
(@PATH,5,220.7462,2308.746,60.02021,0,0,0,0,100,0),
(@PATH,6,208.7211,2311.991,57.36103,0,0,0,0,100,0),
(@PATH,7,195.7977,2312.93,53.85427,0,0,0,0,100,0),
(@PATH,8,181.254,2297.919,50.28107,0,0,0,0,100,0);
-- 0xF1304BEA00396ED6 .go 190.7178 2286.323 49.66181

-- Pathing for Dreadcaller Entry: 19434 'ODB FORMAT' 
SET @NPC := 69500;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=233.3709,`position_y`=2074.927,`position_z`=39.10539 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,0,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,233.3709,2074.927,39.10539,0,0,0,0,100,0),
(@PATH,2,228.1582,2076.218,39.70464,0,0,0,0,100,0),
(@PATH,3,226.8357,2086.181,39.48833,0,0,0,0,100,0),
(@PATH,4,227.4231,2100.461,39.53439,0,0,0,0,100,0),
(@PATH,5,231.5971,2104.666,38.92635,0,0,0,0,100,0),
(@PATH,6,237.6874,2098.439,39.00676,0,0,0,0,100,0),
(@PATH,7,241.6207,2088.015,38.11906,0,0,0,0,100,0),
(@PATH,8,238.947,2078.802,38.06547,0,0,0,0,100,0);
-- 0xF1304BEA00353517 .go 233.3709 2074.927 39.10539

-- Pathing for Bleeding Hollow Tormentor Entry: 19424 'ODB FORMAT' 
SET @NPC := 69477;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-975.6777,`position_y`=1903.692,`position_z`=84.41418 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,9562,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-975.6777,1903.692,84.41418,0,0,0,0,100,0),
(@PATH,2,-973.8419,1939.495,70.39467,0,0,0,0,100,0),
(@PATH,3,-955.7029,1969.015,68.00587,0,0,0,0,100,0),
(@PATH,4,-933.9344,1986.529,66.71876,0,0,0,0,100,0),
(@PATH,5,-920.0254,2004.145,64.96164,0,0,0,0,100,0),
(@PATH,6,-912.637,2028.022,56.54257,0,0,0,0,100,0),
(@PATH,7,-891.1589,2064.74,34.64294,0,0,0,0,100,0),
(@PATH,8,-869.7386,2091.601,23.5704,0,0,0,0,100,0),
(@PATH,9,-858.7391,2124.865,17.03608,0,0,0,0,100,0),
(@PATH,10,-823.7175,2119.571,16.5663,0,0,0,0,100,0),
(@PATH,11,-792.2368,2090.261,22.4062,0,0,0,0,100,0),
(@PATH,12,-783.2084,2054.525,29.63973,0,0,0,0,100,0),
(@PATH,13,-779.9326,2023.77,34.75321,0,0,0,0,100,0),
(@PATH,14,-800.3509,1985.585,39.80645,0,0,0,0,100,0),
(@PATH,15,-814.7488,1949.572,47.55807,0,0,0,0,100,0),
(@PATH,16,-822.0464,1919.74,54.06172,0,0,0,0,100,0),
(@PATH,17,-847.4603,1891.778,64.08673,0,0,0,0,100,0),
(@PATH,18,-885.2077,1883.808,72.72009,0,0,0,0,100,0),
(@PATH,19,-915.132,1902.349,70.37255,0,0,0,0,100,0),
(@PATH,20,-940.0393,1898.689,74.6819,0,0,0,0,100,0),
(@PATH,21,-958.8931,1878.34,92.07265,0,0,0,0,100,0),
(@PATH,22,-967.469,1873.979,94.75582,0,0,0,0,100,0),
(@PATH,23,-976.3881,1877.903,94.82088,0,0,0,0,100,0);
-- 0xF1304BE000608B4F .go -975.6777 1903.692 84.41418

-- Pathing for Bleeding Hollow Tormentor Entry: 19424 'ODB FORMAT' 
SET @NPC := 69471;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-1157.17,`position_y`=1943.396,`position_z`=81.40363 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,0,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-1157.17,1943.396,81.40363,0,0,0,0,100,0),
(@PATH,2,-1151.672,1940.954,81.40428,0,0,0,0,100,0),
(@PATH,3,-1143.042,1932.632,81.42173,0,0,0,0,100,0),
(@PATH,4,-1139.161,1923.026,81.20094,0,0,0,0,100,0),
(@PATH,5,-1138.57,1918.28,81.37885,0,0,0,0,100,0),
(@PATH,6,-1135.403,1912.328,81.4029,0,0,0,0,100,0),
(@PATH,7,-1138.515,1901.65,81.35353,0,0,0,0,100,0),
(@PATH,8,-1145.631,1891.622,81.32314,0,0,0,0,100,0),
(@PATH,9,-1139.867,1899.911,81.37244,0,0,0,0,100,0),
(@PATH,10,-1155.281,1909.135,81.45526,0,0,0,0,100,0),
(@PATH,11,-1139.867,1899.911,81.37244,0,0,0,0,100,0),
(@PATH,12,-1145.631,1891.622,81.32314,0,0,0,0,100,0),
(@PATH,13,-1138.515,1901.65,81.35353,0,0,0,0,100,0),
(@PATH,14,-1135.493,1912.019,81.39507,0,0,0,0,100,0),
(@PATH,15,-1138.57,1918.28,81.37885,0,0,0,0,100,0),
(@PATH,16,-1139.161,1923.026,81.20094,0,0,0,0,100,0),
(@PATH,17,-1142.842,1932.44,81.40112,0,0,0,0,100,0),
(@PATH,18,-1151.672,1940.954,81.40428,0,0,0,0,100,0);
-- 0xF1304BE00060877C .go -1157.17 1943.396 81.40363

-- Pathing for Bleeding Hollow Tormentor Entry: 19424 'ODB FORMAT' 
SET @NPC := 69476;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-1033.498,`position_y`=2016.301,`position_z`=67.98228 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,9562,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-1033.498,2016.301,67.98228,0,0,0,0,100,0),
(@PATH,2,-1000.079,2017.703,67.06455,0,0,0,0,100,0),
(@PATH,3,-954.4206,2015.588,67.03322,0,0,0,0,100,0),
(@PATH,4,-915.6917,2025.381,58.45187,0,0,0,0,100,0),
(@PATH,5,-886.038,2057.447,35.91393,0,0,0,0,100,0),
(@PATH,6,-886.0889,2092.425,24.59444,0,0,0,0,100,0),
(@PATH,7,-915.6437,2115.709,20.89748,0,0,0,0,100,0),
(@PATH,8,-914.2065,2149.265,14.4707,0,0,0,0,100,0),
(@PATH,9,-922.1042,2170.104,12.60829,0,0,0,0,100,0),
(@PATH,10,-945.8377,2185.868,12.26309,0,0,0,0,100,0),
(@PATH,11,-961.3444,2217.514,9.387299,0,0,0,0,100,0),
(@PATH,12,-957.1495,2247.785,6.748508,0,0,0,0,100,0),
(@PATH,13,-984.4343,2282.687,6.486638,0,0,0,0,100,0),
(@PATH,14,-1016.313,2306.371,8.956036,0,0,0,0,100,0),
(@PATH,15,-1048.225,2313.089,14.2006,0,0,0,0,100,0),
(@PATH,16,-1081.711,2312.186,20.69922,0,0,0,0,100,0),
(@PATH,17,-1087.714,2285.604,24.63902,0,0,0,0,100,0),
(@PATH,18,-1067.954,2244.344,24.47042,0,0,0,0,100,0),
(@PATH,19,-1052.511,2214.931,19.66574,0,0,0,0,100,0),
(@PATH,20,-1051.316,2178.967,24.62208,0,0,0,0,100,0),
(@PATH,21,-1055.739,2145.756,39.92072,0,0,0,0,100,0),
(@PATH,22,-1057.033,2106.794,57.37517,0,0,0,0,100,0),
(@PATH,23,-1046.553,2082.452,64.01545,0,0,0,0,100,0),
(@PATH,24,-1029.464,2056.926,67.17399,0,0,0,0,100,0),
(@PATH,25,-1056.46,2029.947,67.56455,0,0,0,0,100,0),
(@PATH,26,-1084.9,1998.348,67.47117,0,0,0,0,100,0),
(@PATH,27,-1065.154,1984.061,69.4269,0,0,0,0,100,0),
(@PATH,28,-1084.9,1998.348,67.47117,0,0,0,0,100,0),
(@PATH,29,-1056.46,2029.947,67.56455,0,0,0,0,100,0),
(@PATH,30,-1029.464,2056.926,67.17399,0,0,0,0,100,0),
(@PATH,31,-1046.553,2082.452,64.01545,0,0,0,0,100,0),
(@PATH,32,-1057.033,2106.794,57.37517,0,0,0,0,100,0),
(@PATH,33,-1055.739,2145.756,39.92072,0,0,0,0,100,0),
(@PATH,34,-1051.316,2178.967,24.62208,0,0,0,0,100,0),
(@PATH,35,-1052.511,2214.931,19.66574,0,0,0,0,100,0),
(@PATH,36,-1067.954,2244.344,24.47042,0,0,0,0,100,0),
(@PATH,37,-1087.714,2285.604,24.63902,0,0,0,0,100,0),
(@PATH,38,-1081.711,2312.186,20.69922,0,0,0,0,100,0),
(@PATH,39,-1048.225,2313.089,14.2006,0,0,0,0,100,0),
(@PATH,40,-1016.313,2306.371,8.956036,0,0,0,0,100,0),
(@PATH,41,-984.4343,2282.687,6.486638,0,0,0,0,100,0),
(@PATH,42,-957.1495,2247.785,6.748508,0,0,0,0,100,0),
(@PATH,43,-961.3444,2217.514,9.387299,0,0,0,0,100,0),
(@PATH,44,-945.8377,2185.868,12.26309,0,0,0,0,100,0),
(@PATH,45,-922.1042,2170.104,12.60829,0,0,0,0,100,0),
(@PATH,46,-914.2065,2149.265,14.4707,0,0,0,0,100,0),
(@PATH,47,-915.6437,2115.709,20.89748,0,0,0,0,100,0),
(@PATH,48,-886.0889,2092.425,24.59444,0,0,0,0,100,0),
(@PATH,49,-886.038,2057.447,35.91393,0,0,0,0,100,0),
(@PATH,50,-915.6917,2025.381,58.45187,0,0,0,0,100,0),
(@PATH,51,-954.4206,2015.588,67.03322,0,0,0,0,100,0),
(@PATH,52,-1000.079,2017.703,67.06455,0,0,0,0,100,0),
(@PATH,53,-1033.498,2016.301,67.98228,0,0,0,0,100,0);
-- 0xF1304BE00060AF73 .go -1033.498 2016.301 67.98228

-- Pathing for Bleeding Hollow Tormentor Entry: 19424 'ODB FORMAT' 
SET @NPC := 69479;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-887.3162,`position_y`=2100.392,`position_z`=22.98137 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,9562,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-887.3162,2100.392,22.98137,0,0,0,0,100,0),
(@PATH,2,-902.8832,2104.852,22.5635,0,0,0,0,100,0),
(@PATH,3,-915.3331,2120.114,20.053,0,0,0,0,100,0),
(@PATH,4,-914.8217,2141.304,15.65685,0,0,0,0,100,0),
(@PATH,5,-920.6898,2180.254,11.73329,0,0,0,0,100,0),
(@PATH,6,-928.6917,2208.044,9.395775,0,0,0,0,100,0),
(@PATH,7,-935.8201,2223.378,8.009735,0,0,0,0,100,0),
(@PATH,8,-957.2545,2229.223,7.933442,0,0,0,0,100,0),
(@PATH,9,-962.3887,2210.646,10.17673,0,0,0,0,100,0),
(@PATH,10,-949.9908,2203.8,9.920502,0,0,0,0,100,0),
(@PATH,11,-929.2674,2205.433,9.8395,0,0,0,0,100,0),
(@PATH,12,-897.6884,2189.902,10.51884,0,0,0,0,100,0),
(@PATH,13,-862.864,2165.974,11.53246,0,0,0,0,100,0),
(@PATH,14,-840.7776,2152.851,13.35741,0,0,0,0,100,0),
(@PATH,15,-837.7058,2133.558,15.67247,0,0,0,0,100,0),
(@PATH,16,-852.1836,2118.362,17.45734,0,0,0,0,100,0),
(@PATH,17,-871.5316,2098.868,21.64986,0,0,0,0,100,0);
-- 0xF1304BE0006135E2 .go -887.3162 2100.392 22.98137

-- Pathing for Bleeding Hollow Tormentor Entry: 19424 'ODB FORMAT' 
SET @NPC := 69481;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-1072.546,`position_y`=2254.384,`position_z`=25.03121 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,9562,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-1072.546,2254.384,25.03121,0,0,0,0,100,0),
(@PATH,2,-1050.309,2248.466,22.70865,0,0,0,0,100,0),
(@PATH,3,-1016.866,2228.801,13.34218,0,0,0,0,100,0),
(@PATH,4,-975.5443,2209.247,10.59319,0,0,0,0,100,0),
(@PATH,5,-950.7357,2192.689,11.50015,0,0,0,0,100,0),
(@PATH,6,-915.4313,2157.962,13.552,0,0,0,0,100,0),
(@PATH,7,-912.6307,2132.925,17.52908,0,0,0,0,100,0),
(@PATH,8,-912.6307,2132.925,17.52908,0,0,0,0,100,0),
(@PATH,9,-915.4313,2157.962,13.552,0,0,0,0,100,0),
(@PATH,10,-950.7357,2192.689,11.50015,0,0,0,0,100,0),
(@PATH,11,-975.5443,2209.247,10.59319,0,0,0,0,100,0),
(@PATH,12,-1016.866,2228.801,13.34218,0,0,0,0,100,0),
(@PATH,13,-1050.309,2248.466,22.70865,0,0,0,0,100,0),
(@PATH,14,-1072.546,2254.384,25.03121,0,0,0,0,100,0);
-- 0xF1304BE00061378F .go -1072.546 2254.384 25.03121

-- Pathing for Bleeding Hollow Tormentor Entry: 19424 'ODB FORMAT' 
SET @NPC := 69480;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-1000.677,`position_y`=2218.161,`position_z`=11.39956 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,9562,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-1000.677,2218.161,11.39956,0,0,0,0,100,0),
(@PATH,2,-1034.118,2233.885,16.91886,0,0,0,0,100,0),
(@PATH,3,-1000.677,2218.161,11.39956,0,0,0,0,100,0),
(@PATH,4,-960.3612,2199.009,11.10769,0,0,0,0,100,0),
(@PATH,5,-929.8221,2168.747,12.79615,0,0,0,0,100,0),
(@PATH,6,-917.9709,2134.846,17.4569,0,0,0,0,100,0),
(@PATH,7,-899.0653,2097.202,24.10116,0,0,0,0,100,0),
(@PATH,8,-867.7588,2092.518,23.16903,0,0,0,0,100,0),
(@PATH,9,-834.6699,2065.056,28.35329,0,0,0,0,100,0),
(@PATH,10,-834.6699,2065.056,28.35329,0,0,0,0,100,0),
(@PATH,11,-867.7588,2092.518,23.16903,0,0,0,0,100,0),
(@PATH,12,-899.0653,2097.202,24.10116,0,0,0,0,100,0),
(@PATH,13,-917.9709,2134.846,17.4569,0,0,0,0,100,0),
(@PATH,14,-929.8221,2168.747,12.79615,0,0,0,0,100,0),
(@PATH,15,-960.3612,2199.009,11.10769,0,0,0,0,100,0),
(@PATH,16,-1000.677,2218.161,11.39956,0,0,0,0,100,0),
(@PATH,17,-1034.118,2233.885,16.91886,0,0,0,0,100,0),
(@PATH,18,-1000.677,2218.161,11.39956,0,0,0,0,100,0);
-- 0xF1304BE000613CEA .go -1000.677 2218.161 11.39956

-- Pathing for Bleeding Hollow Tormentor Entry: 19424 'ODB FORMAT' 
SET @NPC := 69474;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-1141.451,`position_y`=2039.274,`position_z`=67.06458 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,0,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-1141.451,2039.274,67.06458,0,0,0,0,100,0),
(@PATH,2,-1098.165,2044.068,66.97787,0,0,0,0,100,0),
(@PATH,3,-1053.092,2025.227,67.56455,0,0,0,0,100,0),
(@PATH,4,-1011.46,2004.103,67.06455,0,0,0,0,100,0),
(@PATH,5,-983.9862,1982.55,68.81455,0,0,0,0,100,0),
(@PATH,6,-984.9029,1950.981,70.32558,0,0,0,0,100,0),
(@PATH,7,-974.4434,1917.587,77.03918,0,0,0,0,100,0),
(@PATH,8,-951.4631,1888.722,82.49074,0,0,0,0,100,0),
(@PATH,9,-918.9573,1886.185,78.15961,0,0,0,0,100,0),
(@PATH,10,-918.9573,1886.185,78.15961,0,0,0,0,100,0),
(@PATH,11,-951.4631,1888.722,82.49074,0,0,0,0,100,0),
(@PATH,12,-974.4434,1917.587,77.03918,0,0,0,0,100,0),
(@PATH,13,-984.9029,1950.981,70.32558,0,0,0,0,100,0),
(@PATH,14,-983.9862,1982.55,68.81455,0,0,0,0,100,0),
(@PATH,15,-1011.46,2004.103,67.06455,0,0,0,0,100,0),
(@PATH,16,-1053.092,2025.227,67.56455,0,0,0,0,100,0),
(@PATH,17,-1098.165,2044.068,66.97787,0,0,0,0,100,0),
(@PATH,18,-1141.451,2039.274,67.06458,0,0,0,0,100,0);
-- 0xF1304BE00061420D .go -1141.451 2039.274 67.06458

-- Pathing for Bleeding Hollow Tormentor Entry: 19424 'ODB FORMAT' 
SET @NPC := 69478;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-824.4816,`position_y`=1990.778,`position_z`=38.91302 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,9562,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-824.4816,1990.778,38.91302,0,0,0,0,100,0),
(@PATH,2,-800.973,2019.102,38.81275,0,0,0,0,100,0),
(@PATH,3,-801.4883,2043.102,41.42069,0,0,0,0,100,0),
(@PATH,4,-820.149,2057.47,35.70707,0,0,0,0,100,0),
(@PATH,5,-830.2311,2072.128,25.74135,0,0,0,0,100,0),
(@PATH,6,-842.6055,2101.628,19.20734,0,0,0,0,100,0),
(@PATH,7,-854.4006,2133.824,15.76549,0,0,0,0,100,0),
(@PATH,8,-854.3831,2150.491,13.76549,0,0,0,0,100,0),
(@PATH,9,-871.9949,2163.9,11.56941,0,0,0,0,100,0),
(@PATH,10,-900.2237,2167.077,11.69764,0,0,0,0,100,0),
(@PATH,11,-900.2237,2167.077,11.69764,0,0,0,0,100,0),
(@PATH,12,-871.9949,2163.9,11.56941,0,0,0,0,100,0),
(@PATH,13,-854.3831,2150.491,13.76549,0,0,0,0,100,0),
(@PATH,14,-854.4006,2133.824,15.76549,0,0,0,0,100,0),
(@PATH,15,-842.6055,2101.628,19.20734,0,0,0,0,100,0),
(@PATH,16,-830.2311,2072.128,25.74135,0,0,0,0,100,0),
(@PATH,17,-820.149,2057.47,35.70707,0,0,0,0,100,0),
(@PATH,18,-801.4883,2043.102,41.42069,0,0,0,0,100,0),
(@PATH,19,-800.973,2019.102,38.81275,0,0,0,0,100,0),
(@PATH,20,-824.4816,1990.778,38.91302,0,0,0,0,100,0);
-- 0xF1304BE0006192D2 .go -824.4816 1990.778 38.91302

-- Pathing for Worg Master Kruush Entry: 19442 'ODB FORMAT' 
SET @NPC := 69634;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-1057.877,`position_y`=1996.641,`position_z`=67.34181 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,0,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-1057.877,1996.641,67.34181,0,0,0,0,100,0),
(@PATH,2,-1050.188,2009.977,67.06455,0,0,0,0,100,0);
-- 0xF1304BF20060E90B .go -1057.877 1996.641 67.34181

-- Adding missing spawn Bleeding Hollow Dark Shaman Entry: 16873
SET @GUID := 29978;
DELETE FROM creature WHERE guid=@GUID;
INSERT INTO `creature` (`guid`, `id`, `map`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `curhealth`, `curmana`, `MovementType`) VALUES
(@GUID, 16873, 530, -945.313, 1948.252, 58.1359, 67.06455, 300, 3984, 2434, 2);

-- Pathing for Bleeding Hollow Dark Shaman Entry: 16873 'ODB FORMAT' 
SET @NPC := 58257;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-1058.69,`position_y`=2123.373,`position_z`=51.63616 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,0,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-1058.69,2123.373,51.63616,0,0,0,0,100,0),
(@PATH,2,-1057.219,2099.756,59.51154,0,0,0,0,100,0),
(@PATH,3,-1044.785,2080.962,64.48811,0,0,0,0,100,0),
(@PATH,4,-1027.752,2060.003,67.17399,0,0,0,0,100,0),
(@PATH,5,-1035.883,2045.864,67.88428,0,0,0,0,100,0),
(@PATH,6,-1024.891,2036.104,67.17399,0,0,0,0,100,0),
(@PATH,7,-1011.885,2038.549,67.04899,0,0,0,0,100,0),
(@PATH,8,-993.7006,2017.729,67.06455,0,0,0,0,100,0),
(@PATH,9,-1008.182,1997.039,67.06455,0,0,0,0,100,0),
(@PATH,10,-992.9711,2018.175,67.06455,0,0,0,0,100,0),
(@PATH,11,-971.9114,2014.351,67.06455,0,0,0,0,100,0),
(@PATH,12,-992.9711,2018.175,67.06455,0,0,0,0,100,0),
(@PATH,13,-1008.182,1997.039,67.06455,0,0,0,0,100,0),
(@PATH,14,-1008.182,1997.039,67.06455,0,0,0,0,100,0),
(@PATH,15,-993.7006,2017.729,67.06455,0,0,0,0,100,0),
(@PATH,16,-1011.885,2038.549,67.04899,0,0,0,0,100,0),
(@PATH,17,-1011.885,2038.549,67.04899,0,0,0,0,100,0),
(@PATH,18,-1024.891,2036.104,67.17399,0,0,0,0,100,0),
(@PATH,19,-1035.883,2045.864,67.88428,0,0,0,0,100,0),
(@PATH,20,-1027.752,2060.003,67.17399,0,0,0,0,100,0),
(@PATH,21,-1027.752,2060.003,67.17399,0,0,0,0,100,0),
(@PATH,22,-1044.785,2080.966,64.57795,0,0,0,0,100,0),
(@PATH,23,-1057.219,2099.756,59.51154,0,0,0,0,100,0);
-- 0xF13041E90060F72D .go -1058.69 2123.373 51.63616

-- Pathing for Bleeding Hollow Dark Shaman Entry: 16873 'ODB FORMAT' 
SET @NPC := 58256;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-1144.076,`position_y`=1954.738,`position_z`=80.57024 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,0,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-1144.076,1954.738,80.57024,0,0,0,0,100,0),
(@PATH,2,-1133.44,1972.225,73.66801,0,0,0,0,100,0),
(@PATH,3,-1122.611,1999.216,68.83028,0,0,0,0,100,0),
(@PATH,4,-1099.563,2031.872,67.05327,0,0,0,0,100,0),
(@PATH,5,-1084.684,2065.122,66.82748,0,0,0,0,100,0),
(@PATH,6,-1070.713,2090.635,62.18363,0,0,0,0,100,0),
(@PATH,7,-1100.243,2084.849,66.59998,0,0,0,0,100,0),
(@PATH,8,-1129.753,2078.135,67.00842,0,0,0,0,100,0),
(@PATH,9,-1100.243,2084.849,66.59998,0,0,0,0,100,0),
(@PATH,10,-1070.713,2090.635,62.18363,0,0,0,0,100,0),
(@PATH,11,-1084.646,2065.203,66.72787,0,0,0,0,100,0),
(@PATH,12,-1099.563,2031.872,67.05327,0,0,0,0,100,0),
(@PATH,13,-1122.611,1999.216,68.83028,0,0,0,0,100,0),
(@PATH,14,-1133.43,1972.312,73.66801,0,0,0,0,100,0);
-- 0xF13041E900612D35 .go -1144.076 1954.738 80.57024

-- Pathing for Bleeding Hollow Dark Shaman Entry: 16873 'ODB FORMAT' 
SET @NPC := 58258;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-1060.848,`position_y`=2105.119,`position_z`=58.13591 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,0,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-1060.848,2105.119,58.13591,0,0,0,0,100,0),
(@PATH,2,-1037.167,2107.136,59.97014,0,0,0,0,100,0),
(@PATH,3,-1018.748,2084.713,68.99667,0,0,0,0,100,0),
(@PATH,4,-990.0961,2053.444,67.06454,0,0,0,0,100,0),
(@PATH,5,-961.8248,2037.499,67.04703,0,0,0,0,100,0),
(@PATH,6,-938.3379,2024.596,65.73439,0,0,0,0,100,0),
(@PATH,7,-921.1564,2015.199,62.66013,0,0,0,0,100,0),
(@PATH,8,-909.5775,1992.915,67.29996,0,0,0,0,100,0),
(@PATH,9,-889.6608,1985.54,67.38715,0,0,0,0,100,0),
(@PATH,10,-879.1632,1955.284,67.81312,0,0,0,0,100,0),
(@PATH,11,-877.1085,1921.789,70.60205,0,0,0,0,100,0),
(@PATH,12,-901.7319,1883,76.4217,0,0,0,0,100,0),
(@PATH,13,-877.1085,1921.789,70.60205,0,0,0,0,100,0),
(@PATH,14,-879.1632,1955.284,67.81312,0,0,0,0,100,0),
(@PATH,15,-889.6608,1985.54,67.38715,0,0,0,0,100,0),
(@PATH,16,-909.5775,1992.915,67.29996,0,0,0,0,100,0),
(@PATH,17,-921.1564,2015.199,62.66013,0,0,0,0,100,0),
(@PATH,18,-938.3379,2024.596,65.73439,0,0,0,0,100,0),
(@PATH,19,-961.8248,2037.499,67.04703,0,0,0,0,100,0),
(@PATH,20,-990.0961,2053.444,67.06454,0,0,0,0,100,0),
(@PATH,21,-1018.748,2084.713,68.99667,0,0,0,0,100,0),
(@PATH,22,-1037.167,2107.136,59.97014,0,0,0,0,100,0),
(@PATH,23,-1060.848,2105.119,58.13591,0,0,0,0,100,0);
-- 0xF13041E90061464D .go -1060.848 2105.119 58.13591

-- Pathing for Dreadcaller Entry: 19434 'ODB FORMAT' 
SET @NPC := 69503;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-523.7537,`position_y`=2008.213,`position_z`=82.43176 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,0,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-523.7537,2008.213,82.43176,0,0,0,0,100,0),
(@PATH,2,-513.3772,2034.226,82.13103,0,0,0,0,100,0),
(@PATH,3,-501.1374,2063.957,81.46646,0,0,0,0,100,0),
(@PATH,4,-490.7202,2097.977,79.50304,0,0,0,0,100,0),
(@PATH,5,-495.6437,2124.143,75.1469,0,0,0,0,100,0),
(@PATH,6,-500.0753,2152.032,70.07507,0,0,0,0,100,0),
(@PATH,7,-497.6553,2131.211,73.99121,0,0,0,0,100,0),
(@PATH,8,-488.903,2108.3,78.71799,0,0,0,0,100,0),
(@PATH,9,-496.6252,2076.111,81.08714,0,0,0,0,100,0),
(@PATH,10,-505.7927,2051.876,81.88617,0,0,0,0,100,0),
(@PATH,11,-519.2833,2018.794,82.35017,0,0,0,0,100,0),
(@PATH,12,-523.7976,2008.294,82.49103,0,0,0,0,100,0);
-- 0x1C09E4424012FA8000002000004E9B0D .go -523.7537 2008.213 82.43176

-- Pathing for Dreadcaller Entry: 19434 'ODB FORMAT' 
SET @NPC := 69499;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-558.5822,`position_y`=2003.227,`position_z`=98.30606 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,0,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-558.5822,2003.227,98.30606,0,0,0,0,100,0),
(@PATH,2,-557.4218,2005.291,99.07405,0,0,0,0,100,0),
(@PATH,3,-559.7542,2056.054,96.38632,0,0,0,0,100,0),
(@PATH,4,-540.9093,2062.701,104.2178,0,0,0,0,100,0),
(@PATH,5,-538.0739,2083.637,102.7935,0,0,0,0,100,0),
(@PATH,6,-531.9629,2100.469,104.145,0,0,0,0,100,0),
(@PATH,7,-540.8042,2121.509,97.44407,0,0,0,0,100,0),
(@PATH,8,-542.6715,2139.263,94.42299,0,0,0,0,100,0),
(@PATH,9,-542.0955,2125.005,96.38588,0,0,0,0,100,0),
(@PATH,10,-531.0101,2105.611,103.8495,0,0,0,0,100,0),
(@PATH,11,-533.4691,2097.096,103.5421,0,0,0,0,100,0),
(@PATH,12,-539.8324,2066.038,103.8721,0,0,0,0,100,0),
(@PATH,13,-559.5454,2057.871,96.58672,0,0,0,0,100,0),
(@PATH,14,-557.4636,2005.37,98.92694,0,0,0,0,100,0);
-- 0x1C09E4424012FA8000002D00001EC994 .go -558.5822 2003.227 98.30606

-- Pathing for Dreadcaller Entry: 19434 'ODB FORMAT' 
SET @NPC := 69504;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-506.4011,`position_y`=1982.52,`position_z`=85.11931 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,0,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-506.4011,1982.52,85.11931,0,0,0,0,100,0),
(@PATH,2,-491.8763,2002.533,90.20009,0,0,0,0,100,0),
(@PATH,3,-475.5587,2034.282,93.52704,0,0,0,0,100,0),
(@PATH,4,-459.14,2057.011,93.19531,0,0,0,0,100,0),
(@PATH,5,-488.9269,2006.812,90.64146,0,0,0,0,100,0),
(@PATH,6,-506.4684,1982.384,85.10349,0,0,0,0,100,0),
(@PATH,7,-459.1624,2056.984,93.22046,0,0,0,0,100,0),
(@PATH,8,-453.0856,2078.318,92.4622,0,0,0,0,100,0),
(@PATH,9,-458.4782,2105.915,91.30417,0,0,0,0,100,0),
(@PATH,10,-453.1829,2091.579,91.92897,0,0,0,0,100,0),
(@PATH,11,-454.6579,2067.877,92.86068,0,0,0,0,100,0),
(@PATH,12,-471.7339,2040.147,93.86339,0,0,0,0,100,0),
(@PATH,13,-488.8954,2006.772,90.6447,0,0,0,0,100,0),
(@PATH,14,-506.3882,1982.291,85.0636,0,0,0,0,100,0);
-- 0x1C09E4424012FA8000002D00001EC026 .go -506.4011 1982.52 85.11931

-- Wrong npc spawned, should be a Stormwind Infantry npc
UPDATE `creature` SET `id`=16864 WHERE `guid`=57934;

-- Pathing for Stormwind Infantry Entry: 16864 'ODB FORMAT' 
SET @NPC := 57934;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-702.9703,`position_y`=1826.119,`position_z`=75.24933 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,0,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-702.9703,1826.119,75.24933,0,0,0,0,100,0),
(@PATH,2,-707.4134,1826.754,75.24933,0,0,0,0,100,0);
-- 0x1C09E4424010780000002B00009A4063 .go -702.9703 1826.119 75.24933

DELETE FROM `creature_formations` WHERE `leaderGUID`=57965;
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES
(57965, 57965, 0, 0, 2),
(57965, 57966, 5, 0, 2),
(57965, 57967, 10, 0, 2),
(57965, 57968, 15, 0, 2);

-- Pathing for Bleeding Hollow Dark Shaman Entry: 16873 'ODB FORMAT'
SET @NPC := 29978;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-945.313,`position_y`=1948.252,`position_z`=67.06455 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,0,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-945.313,1948.252,67.06455,0,0,0,0,100,0),
(@PATH,2,-946.7964,1966.628,67.06455,0,0,0,0,100,0),
(@PATH,3,-932.1639,1983.549,66.96194,0,0,0,0,100,0),
(@PATH,4,-914.9733,1976.555,67.05032,0,0,0,0,100,0),
(@PATH,5,-929.8001,1991.014,66.44107,0,0,0,0,100,0),
(@PATH,6,-929.8001,1991.014,66.44107,0,0,0,0,100,0),
(@PATH,7,-941.426,2015.632,66.31691,0,0,0,0,100,0),
(@PATH,8,-965.7396,2015.727,67.03322,0,0,0,0,100,0),
(@PATH,9,-968.4443,2047.705,67.06454,0,0,0,0,100,0),
(@PATH,10,-965.7396,2015.727,67.03322,0,0,0,0,100,0),
(@PATH,11,-965.7396,2015.727,67.03322,0,0,0,0,100,0),
(@PATH,12,-941.426,2015.632,66.31691,0,0,0,0,100,0),
(@PATH,13,-929.8001,1991.014,66.44107,0,0,0,0,100,0),
(@PATH,14,-914.9733,1976.555,67.05032,0,0,0,0,100,0),
(@PATH,15,-914.9733,1976.555,67.05032,0,0,0,0,100,0),
(@PATH,16,-932.1639,1983.549,66.96194,0,0,0,0,100,0),
(@PATH,17,-946.7964,1966.628,67.06455,0,0,0,0,100,0);
-- 0xF13041E90061F326 .go -945.313 1948.252 67.06455

--
DELETE FROM `creature_formations` WHERE `leaderGUID`=57528;
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES
(57528, 57528, 0, 0, 2),
(57528, 57527, 3, 90, 2);

-- Pathing for Thrallmar Grunt Entry: 16580 'ODB FORMAT' 
SET @NPC := 57528;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=132.3433,`position_y`=2762.987,`position_z`=102.3826 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,0,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,132.3433,2762.987,102.3826,0,0,0,0,100,0),
(@PATH,2,121.0521,2749.356,98.25848,0,0,0,0,100,0),
(@PATH,3,109.0274,2741.556,94.49025,0,0,0,0,100,0),
(@PATH,4,95.7988,2726.423,89.95798,0,0,0,0,100,0),
(@PATH,5,83.33932,2712.466,86.36182,0,0,0,0,100,0),
(@PATH,6,75.62203,2702.179,84.19798,0,0,0,0,100,0),
(@PATH,7,68.19456,2688.539,81.85039,0,0,0,0,100,0),
(@PATH,8,66.99038,2685.902,81.33047,0,0,0,0,100,0),
(@PATH,9,82.17162,2678.548,81.62874,0,0,0,0,100,0),
(@PATH,10,96.37463,2678.292,82.75209,0,0,0,0,100,0),
(@PATH,11,112.1558,2676.275,83.38298,0,0,0,0,100,0),
(@PATH,12,119.6769,2675.056,83.7209,0,0,0,0,100,0),
(@PATH,13,111.278,2648.706,80.93124,0,0,0,0,100,0),
(@PATH,14,109.0044,2646.347,80.20769,0,0,0,0,100,0),
(@PATH,15,116.2792,2653.762,81.89182,0,0,0,0,100,0),
(@PATH,16,122.4159,2660.933,84.18261,0,0,0,0,100,0),
(@PATH,17,123.3134,2662.276,84.17096,0,0,0,0,100,0),
(@PATH,18,103.6847,2670.964,83.00417,0,0,0,0,100,0),
(@PATH,19,80.54628,2673.75,81.59075,0,0,0,0,100,0),
(@PATH,20,73.74189,2676.277,80.69298,0,0,0,0,100,0),
(@PATH,21,69.13676,2697.863,83.02922,0,0,0,0,100,0),
(@PATH,22,75.02055,2712.901,85.39117,0,0,0,0,100,0),
(@PATH,23,87.82365,2724.378,88.41669,0,0,0,0,100,0),
(@PATH,24,104.5768,2742.848,93.40324,0,0,0,0,100,0),
(@PATH,25,117.3608,2752.192,97.77188,0,0,0,0,100,0),
(@PATH,26,129.486,2763.919,102.3025,0,0,0,0,100,0);
-- 0x1C09E4424010310000002C00015AED53 .go 132.3433 2762.987 102.3826

DELETE FROM `creature_formations` WHERE `leaderGUID`=57506;
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES
(57506, 57506, 0, 0, 2),
(57506, 57507, 3, 90, 2);

-- Pathing for Thrallmar Grunt Entry: 16580 'ODB FORMAT' 
SET @NPC := 57506;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=160.0679,`position_y`=2783.137,`position_z`=111.3373 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,0,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,160.0679,2783.137,111.3373,0,0,0,0,100,0),
(@PATH,2,162.4358,2778.617,110.5461,0,0,0,0,100,0),
(@PATH,3,172.7094,2774.508,110.6423,0,0,0,0,100,0),
(@PATH,4,186.6513,2777.03,113.5835,0,0,0,0,100,0),
(@PATH,5,200.0523,2782.693,116.7996,0,0,0,0,100,0),
(@PATH,6,209.4241,2784.837,118.5891,0,0,0,0,100,0),
(@PATH,7,222.8512,2785.429,121.0061,0,0,0,0,100,0),
(@PATH,8,233.1155,2787.613,123.6816,0,0,0,0,100,0),
(@PATH,9,237.861,2791.877,125.6136,0,0,0,0,100,0),
(@PATH,10,239.1323,2799.748,127.3235,0,0,0,0,100,0),
(@PATH,11,239.1207,2795.199,126.621,0,0,0,0,100,0),
(@PATH,12,235.6156,2789.706,124.6459,0,0,0,0,100,0),
(@PATH,13,227.0757,2786.192,122.1876,0,0,0,0,100,0),
(@PATH,14,212.622,2785.389,119.2345,0,0,0,0,100,0),
(@PATH,15,205.132,2784.269,117.806,0,0,0,0,100,0),
(@PATH,16,188.0606,2777.626,114.1675,0,0,0,0,100,0),
(@PATH,17,175.7908,2774.627,111.4541,0,0,0,0,100,0),
(@PATH,18,168.39,2775.401,110.4383,0,0,0,0,100,0),
(@PATH,19,160.1083,2782.976,111.366,0,0,0,0,100,0);
-- 0x1C09E4424010310000002C00015AED54 .go 160.0679 2783.137 111.3373

DELETE FROM `creature_formations` WHERE `leaderGUID`=57594;
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES
(57594, 57594, 0, 0, 2),
(57594, 57595, 5, 0, 2),
(57594, 57596, 10, 0, 2),
(57594, 57597, 15, 0, 2);

-- Pathing for Thrallmar Wolf Rider Entry: 16599 'ODB FORMAT' 
SET @NPC := 57594;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=196.28,`position_y`=2781.241,`position_z`=115.9637 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,14334,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,196.28,2781.241,115.9637,0,0,1,0,100,0),
(@PATH,2,173.7009,2773.865,111.0617,0,0,1,0,100,0),
(@PATH,3,144.2007,2764.989,105.126,0,0,1,0,100,0),
(@PATH,4,113.7401,2748.569,96.21298,0,0,1,0,100,0),
(@PATH,5,94.41273,2728.12,89.81252,0,0,1,0,100,0),
(@PATH,6,65.77112,2693.39,82.38187,0,0,1,0,100,0),
(@PATH,7,36.76459,2661.932,76.78915,0,0,1,0,100,0),
(@PATH,8,17.91903,2638.404,72.61198,0,0,1,0,100,0),
(@PATH,9,5.355525,2605.847,68.47747,0,0,1,0,100,0),
(@PATH,10,8.97837,2575.031,65.2146,0,0,1,0,100,0),
(@PATH,11,18.05011,2542.307,61.65181,0,0,1,0,100,0),
(@PATH,12,20.63871,2501.002,57.12801,0,0,1,0,100,0),
(@PATH,13,14.14331,2460.918,53.1017,0,0,1,0,100,0),
(@PATH,14,7.45386,2419.164,52.73537,0,0,1,0,100,0),
(@PATH,15,-14.25867,2395.82,52.69493,0,0,1,0,100,0),
(@PATH,16,-31.19849,2384.302,53.77038,0,0,1,0,100,0),
(@PATH,17,0.05626106,2409.986,52.19868,0,0,1,0,100,0),
(@PATH,18,13.30882,2456.084,52.34318,0,0,1,0,100,0),
(@PATH,19,20.00414,2492.995,56.18382,0,0,1,0,100,0),
(@PATH,20,19.18841,2536.792,60.81985,0,0,1,0,100,0),
(@PATH,21,11.05238,2567.616,64.15677,0,0,1,0,100,0),
(@PATH,22,4.750309,2601.44,67.78731,0,0,1,0,100,0),
(@PATH,23,13.32799,2630.8,71.16357,0,0,1,0,100,0),
(@PATH,24,32.40791,2656.75,75.65665,0,0,1,0,100,0),
(@PATH,25,62.52131,2689.98,81.53972,0,0,1,0,100,0),
(@PATH,26,92.98247,2726.685,89.31989,0,0,1,0,100,0),
(@PATH,27,109.4178,2744.999,94.76643,0,0,1,0,100,0),
(@PATH,28,142.0419,2764.24,104.1994,0,0,1,0,100,0),
(@PATH,29,171.1686,2772.708,110.2985,0,0,1,0,100,0);
-- 0x1C09E442401035C000002C00005AED54 .go 196.28 2781.241 115.9637

-- Pathing for Thrallmar Peon Entry: 16591 'ODB FORMAT' 
SET @NPC := 57575;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=55.27306,`position_y`=2654.043,`position_z`=78.42071 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,0,0,1,69, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,55.27306,2654.043,78.42071,0,0,0,0,100,0),
(@PATH,2,60.95808,2657.462,79.7836,0,0,0,0,100,0),
(@PATH,3,64.60378,2660.895,80.497,0,0,0,0,100,0),
(@PATH,4,66.75615,2663.12,80.74207,0,0,0,0,100,0),
(@PATH,5,65.12799,2682.579,80.74263,0,0,0,0,100,0),
(@PATH,6,65.82681,2693.285,82.36795,0,0,0,0,100,0),
(@PATH,7,75.21483,2707.858,84.6664,0,0,0,0,100,0),
(@PATH,8,84.96182,2718.535,87.05235,0,0,0,0,100,0),
(@PATH,9,95.03929,2728.742,90.13913,0,0,0,0,100,0),
(@PATH,10,106.4619,2744.316,94.00992,0,0,0,0,100,0),
(@PATH,11,123.8118,2755.292,99.37874,0,0,0,0,100,0),
(@PATH,12,135.8948,2760.833,102.7533,0,0,0,0,100,0),
(@PATH,13,155.0525,2769.579,107.7352,0,0,0,0,100,0),
(@PATH,14,158.3794,2773.243,108.8904,0,0,0,0,100,0),
(@PATH,15,159.9402,2783.268,111.4848,0,0,0,0,100,0),
(@PATH,16,161.3573,2786.643,112.5604,0,0,0,0,100,0),
(@PATH,17,165.1619,2790.098,113.5347,0,120000,0,0,100,0),
(@PATH,18,165.2408,2775.052,109.6661,3,0,0,0,100,0),
(@PATH,19,161.6743,2781.431,111.1523,0,0,0,0,100,0),
(@PATH,20,161.0955,2772.586,109.2963,0,0,0,0,100,0),
(@PATH,21,143.7382,2764.468,104.6033,0,0,0,0,100,0),
(@PATH,22,122.8,2754.483,99.03847,0,0,0,0,100,0),
(@PATH,23,109.0177,2745.05,94.75985,0,0,0,0,100,0),
(@PATH,24,96.74924,2729.785,90.4996,0,0,0,0,100,0),
(@PATH,25,83.87657,2715.537,86.72665,0,0,0,0,100,0),
(@PATH,26,75.78943,2705.43,84.61099,0,0,0,0,100,0),
(@PATH,27,64.73528,2690.525,81.81812,0,0,0,0,100,0),
(@PATH,28,62.68444,2679.1,80.26672,0,0,0,0,100,0),
(@PATH,29,62.20171,2674.227,80.11406,0,0,0,0,100,0),
(@PATH,30,62.53893,2659.067,79.85791,0,0,0,0,100,0),
(@PATH,31,53.87121,2650.147,77.64667,0,0,0,0,100,0),
(@PATH,32,52.78874,2648.066,76.98107,0,120000,0,0,100,0);
-- 0x1C09E442401033C000002C00005AED55 .go 55.27306 2654.043 78.42071

DELETE FROM `creature_formations` WHERE `leaderGUID`=57594;
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES
(57594, 57594, 0, 0, 2),
(57594, 57595, 5, 0, 2),
(57594, 57596, 10, 0, 2),
(57594, 57597, 15, 0, 2);

DELETE FROM `creature_formations` WHERE `leaderGUID`=57965;
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES
(57965, 57965, 0, 0, 2),
(57965, 57966, 5, 0, 2),
(57965, 57967, 10, 0, 2),
(57965, 57968, 15, 0, 2);

-- Pathing for Bonechewer Raider Entry: 16925 'ODB FORMAT' 
SET @NPC := 58706;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-372.7111,`position_y`=2839.348,`position_z`=3.085857 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,17408,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-372.7111,2839.348,3.085857,0,0,0,0,100,0),
(@PATH,2,-369.1973,2833.291,3.833567,0,0,0,0,100,0),
(@PATH,3,-366.1855,2828.1,4.404455,0,0,0,0,100,0),
(@PATH,4,-364.1777,2824.639,5.029455,0,0,0,0,100,0),
(@PATH,5,-363.1738,2822.908,5.654455,0,0,0,0,100,0),
(@PATH,6,-361.668,2820.313,6.279455,0,0,0,0,100,0),
(@PATH,7,-360.6641,2818.582,7.029455,0,0,0,0,100,0),
(@PATH,8,-357.6523,2813.391,7.904455,0,0,0,0,100,0),
(@PATH,9,-354.6406,2808.199,8.654455,0,0,0,0,100,0),
(@PATH,10,-352.1309,2803.873,9.279455,0,0,0,0,100,0);
-- 0x1C09E4424010874000002C0000212A8F .go -372.7111 2839.348 3.085857

-- Pathing for Bonechewer Raider Entry: 16925 'ODB FORMAT' 
SET @NPC := 58710;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-173.5004,`position_y`=2340.303,`position_z`=60.07005 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,17408,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-173.5004,2340.303,60.07005,0,0,0,0,100,0),
(@PATH,2,-167.5776,2345.781,59.66853,0,0,0,0,100,0),
(@PATH,3,-168.3658,2358.138,57.73066,0,0,0,0,100,0),
(@PATH,4,-170.6648,2371.729,55.37167,0,0,0,0,100,0),
(@PATH,5,-176.5131,2386.787,51.77451,0,0,0,0,100,0),
(@PATH,6,-179.7427,2391.649,50.90343,0,0,0,0,100,0),
(@PATH,7,-179.451,2411.232,48.48401,0,0,0,0,100,0),
(@PATH,8,-181.9889,2397.549,50.08649,0,0,0,0,100,0),
(@PATH,9,-179.687,2391.666,50.90021,0,0,0,0,100,0),
(@PATH,10,-172.3989,2376.937,54.03761,0,0,0,0,100,0),
(@PATH,11,-168.9601,2362.579,56.66465,0,0,0,0,100,0),
(@PATH,12,-167.4705,2349.302,59.07915,0,0,0,0,100,0);
-- 0x1C09E4424010874000002C000021381E .go -173.5004 2340.303 60.07005

-- Pathing for Bonechewer Raider Entry: 16925 'ODB FORMAT' 
SET @NPC := 58709;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-280.5386,`position_y`=2385.487,`position_z`=49.5167 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,17408,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-280.5386,2385.487,49.5167,0,0,0,0,100,0),
(@PATH,2,-274.4911,2399.897,49.77866,0,0,0,0,100,0),
(@PATH,3,-282.391,2415.943,47.12805,0,0,0,0,100,0),
(@PATH,4,-292.0588,2418.96,45.69984,0,0,0,0,100,0),
(@PATH,5,-296.9233,2425.813,44.45638,0,0,0,0,100,0),
(@PATH,6,-288.1144,2443.262,43.60903,0,0,0,0,100,0),
(@PATH,7,-293.5082,2451.608,42.39658,0,0,0,0,100,0),
(@PATH,8,-297.1542,2466.072,41.27671,0,0,0,0,100,0),
(@PATH,9,-296.9443,2470.941,40.92609,0,0,0,0,100,0),
(@PATH,10,-284.6176,2477.495,40.50471,0,0,0,0,100,0),
(@PATH,11,-290.6595,2483.031,40.95429,0,0,0,0,100,0),
(@PATH,12,-284.6285,2477.351,40.70044,0,0,0,0,100,0),
(@PATH,13,-297.5089,2457.768,41.62237,0,0,0,0,100,0),
(@PATH,14,-289.9194,2446.963,43.04158,0,0,0,0,100,0),
(@PATH,15,-287.7819,2443.668,43.38817,0,0,0,0,100,0),
(@PATH,16,-296.9543,2424.329,44.54842,0,0,0,0,100,0),
(@PATH,17,-284.9567,2418.073,46.32718,0,0,0,0,100,0),
(@PATH,18,-276.5181,2404.314,49.4864,0,0,0,0,100,0),
(@PATH,19,-280.4977,2385.4,49.48282,0,0,0,0,100,0);
-- 0x1C09E4424010874000002C0000213910 .go -280.5386 2385.487 49.5167

-- Pathing for Bonechewer Raider Entry: 16925 'ODB FORMAT' 
SET @NPC := 58708;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-314.1599,`position_y`=2611.343,`position_z`=41.12054 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,17408,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-314.1599,2611.343,41.12054,0,0,0,0,100,0),
(@PATH,2,-302.6028,2572.273,41.00031,0,0,0,0,100,0),
(@PATH,3,-297.84,2550.173,41.36596,0,0,0,0,100,0),
(@PATH,4,-303.8246,2525.236,42.58739,0,0,0,0,100,0),
(@PATH,5,-297.9362,2545.895,41.70477,0,0,0,0,100,0),
(@PATH,6,-300.882,2566.604,41.32217,0,0,0,0,100,0),
(@PATH,7,-306.6208,2578.347,41.52526,0,0,0,0,100,0),
(@PATH,8,-312.7142,2588.381,41.36144,0,0,0,0,100,0);
-- 0x1C09E4424010874000002C00002139B5 .go -314.1599 2611.343 41.12054

-- Add missing Bonechewer Raider
DELETE FROM `creature` WHERE `guid`=29980;
INSERT INTO `creature` (`guid`, `id`, `map`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `curhealth`, `MovementType`) VALUES 
(29980, 16925, 530, 1, -354.3998, 2663.006, 41.72623, 0, 300, 3989, 2);

-- Pathing for Bonechewer Raider Entry: 16925 'ODB FORMAT' 
SET @NPC := 29980;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-354.3998,`position_y`=2663.006,`position_z`=41.72623 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,17408,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-354.3998,2663.006,41.72623,0,0,0,0,100,0),
(@PATH,2,-346.7007,2672.114,38.15508,0,0,0,0,100,0),
(@PATH,3,-332.8381,2681.229,34.02082,0,0,0,0,100,0),
(@PATH,4,-319.6889,2687.015,31.10067,0,0,0,0,100,0),
(@PATH,5,-318.0042,2694.25,29.72155,0,0,0,0,100,0),
(@PATH,6,-324.0378,2716.413,25.33266,0,0,0,0,100,0),
(@PATH,7,-319.7849,2729.284,23.22117,0,0,0,0,100,0),
(@PATH,8,-322.0183,2746.254,18.96142,0,0,0,0,100,0),
(@PATH,9,-322.4724,2747.062,18.86184,0,0,0,0,100,0),
(@PATH,10,-322.3822,2746.857,18.3417,0,0,0,0,100,0),
(@PATH,11,-319.7568,2733.282,22.47406,0,0,0,0,100,0),
(@PATH,12,-323.8112,2718.555,24.77036,0,0,0,0,100,0),
(@PATH,13,-317.9898,2696.302,28.92368,0,0,0,0,100,0),
(@PATH,14,-318.5087,2690.603,30.36932,0,0,0,0,100,0),
(@PATH,15,-329.6577,2682.807,32.7053,0,0,0,0,100,0),
(@PATH,16,-344.2849,2674.339,36.96185,0,0,0,0,100,0);
-- 0x1C09E4424010874000002C0000213896 .go -354.3998 2663.006 41.72623

-- Only one Tagar Spinebreaker should be spawned
DELETE FROM `creature` WHERE `guid`=85990;

-- Pathing for Tagar Spinebreaker Entry: 19443 'ODB FORMAT' 
SET @NPC := 85987;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-179.7917,`position_y`=2839.993,`position_z`=23.64687 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,0,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-179.7917,2839.993,23.64687,0,0,0,0,100,0),
(@PATH,2,-168.0662,2837.727,26.36559,0,0,0,0,100,0),
(@PATH,3,-149.3422,2826.91,33.67762,0,0,0,0,100,0),
(@PATH,4,-149.3403,2813.561,35.25396,0,0,0,0,100,0),
(@PATH,5,-160.4427,2807.621,32.6923,0,0,0,0,100,0),
(@PATH,6,-172.6706,2803.528,29.62396,0,0,0,0,100,0),
(@PATH,7,-185.5681,2808.792,25.53679,0,0,0,0,100,0),
(@PATH,8,-186.3388,2797.655,26.33011,0,0,0,0,100,0),
(@PATH,9,-153.882,2788.701,36.85746,0,0,0,0,100,0),
(@PATH,10,-134.5498,2778.104,41.05849,0,0,0,0,100,0),
(@PATH,11,-120.4556,2753.937,49.37396,0,0,0,0,100,0),
(@PATH,12,-110.122,2737.952,52.16914,0,0,0,0,100,0),
(@PATH,13,-108.798,2709.016,51.00445,0,0,0,0,100,0),
(@PATH,14,-125.7525,2695.987,46.92542,0,0,0,0,100,0),
(@PATH,15,-165.7749,2686.863,43.39313,0,0,0,0,100,0),
(@PATH,16,-166.924,2686.502,43.27299,0,0,0,0,100,0),
(@PATH,17,-170.6192,2661.56,42.24799,0,0,0,0,100,0),
(@PATH,18,-166.9141,2633.801,41.55241,0,0,0,0,100,0),
(@PATH,19,-165.2749,2618.521,40.12231,0,0,0,0,100,0),
(@PATH,20,-164.5241,2612.235,40.08208,0,0,0,0,100,0),
(@PATH,21,-162.7764,2603.097,40.06981,0,0,0,0,100,0),
(@PATH,22,-165.0483,2590.722,39.90811,0,0,0,0,100,0),
(@PATH,23,-144.7979,2569.513,41.10892,0,0,0,0,100,0),
(@PATH,24,-146.1405,2544.73,40.98074,0,0,0,0,100,0),
(@PATH,25,-156.8518,2539.49,42.95174,0,0,0,0,100,0),
(@PATH,26,-170.4912,2527.817,41.62189,0,0,0,0,100,0),
(@PATH,27,-177.3363,2516.895,41.11181,0,0,0,0,100,0),
(@PATH,28,-176.3846,2510.593,41.94983,0,0,0,0,100,0),
(@PATH,29,-168.8438,2501.025,43.79488,0,0,0,0,100,0),
(@PATH,30,-145.8467,2499.76,45.61186,0,0,0,0,100,0),
(@PATH,31,-131.4405,2495.996,46.55548,0,0,0,0,100,0),
(@PATH,32,-124.8685,2492.812,46.93701,0,0,0,0,100,0),
(@PATH,33,-118.3926,2470.393,46.95731,0,0,0,0,100,0),
(@PATH,34,-134.5914,2450.625,46.18983,0,0,0,0,100,0),
(@PATH,35,-139.636,2439.596,48.65514,0,0,0,0,100,0),
(@PATH,36,-153.0726,2420.765,44.33873,0,0,0,0,100,0),
(@PATH,37,-164.3153,2415.917,48.36614,0,0,0,0,100,0),
(@PATH,38,-176.0278,2408.138,48.95763,0,0,0,0,100,0),
(@PATH,39,-181.3092,2397.231,50.16575,0,0,0,0,100,0),
(@PATH,40,-174.2651,2386.537,51.88837,0,0,0,0,100,0),
(@PATH,41,-171.3092,2382.064,53.20797,0,0,0,0,100,0),
(@PATH,42,-168.7256,2366.751,56.15277,0,0,0,0,100,0),
(@PATH,43,-168.0793,2352.929,58.49963,0,0,0,0,100,0),
(@PATH,44,-168.3409,2341.554,60.14597,0,0,0,0,100,0),
(@PATH,45,-175.721,2327.79,62.15151,0,0,0,0,100,0),
(@PATH,46,-193.0925,2317.189,56.10988,0,0,0,0,100,0),
(@PATH,47,-211.1901,2312.889,49.67965,0,0,0,0,100,0),
(@PATH,48,-243.0014,2311.802,51.36759,0,0,0,0,100,0),
(@PATH,49,-257.705,2324.297,56.98617,0,0,0,0,100,0),
(@PATH,50,-267.8463,2334.503,54.92202,0,0,0,0,100,0),
(@PATH,51,-284.0244,2347.267,51.85397,0,0,0,0,100,0),
(@PATH,52,-285.5917,2348.854,51.30281,0,0,0,0,100,0),
(@PATH,53,-277.6388,2384.371,49.04704,0,0,0,0,100,0),
(@PATH,54,-277.4207,2389,49.55646,0,0,0,0,100,0),
(@PATH,55,-282.4726,2415.647,47.11025,0,0,0,0,100,0),
(@PATH,56,-290.6383,2417.861,45.76169,0,0,0,0,100,0),
(@PATH,57,-296.9912,2425.083,44.68782,0,0,0,0,100,0),
(@PATH,58,-297.4091,2429.13,44.28957,0,0,0,0,100,0),
(@PATH,59,-292.8183,2446.393,42.99865,0,0,0,0,100,0),
(@PATH,60,-297.4739,2458.016,41.8229,0,0,0,0,100,0),
(@PATH,61,-299.0847,2461.294,41.30234,0,0,0,0,100,0),
(@PATH,62,-291.5309,2472.148,40.82636,0,0,0,0,100,0),
(@PATH,63,-315.7325,2479.059,38.88892,0,0,0,0,100,0),
(@PATH,64,-333.1553,2479.061,30.43475,0,0,0,0,100,0),
(@PATH,65,-362.027,2481.302,26.76814,0,0,0,0,100,0),
(@PATH,66,-381.832,2489.02,35.44767,0,0,0,0,100,0),
(@PATH,67,-383.683,2499.09,43.07095,0,0,0,0,100,0),
(@PATH,68,-373.0249,2518.307,44.52934,0,0,0,0,100,0),
(@PATH,69,-356.4537,2524.72,43.92796,0,0,0,0,100,0),
(@PATH,70,-341.8646,2517.716,42.69872,0,0,0,0,100,0),
(@PATH,71,-325.4874,2514.352,39.74596,0,0,0,0,100,0),
(@PATH,72,-317.6911,2517.743,42.27646,0,0,0,0,100,0),
(@PATH,73,-307.9288,2522.574,42.47154,0,0,0,0,100,0),
(@PATH,74,-303.3547,2545.351,42.88755,0,0,0,0,100,0),
(@PATH,75,-313.6268,2562.334,44.01721,0,0,0,0,100,0),
(@PATH,76,-319.8146,2564.861,44.6563,0,0,0,0,100,0),
(@PATH,77,-315.3898,2592.442,41.60535,0,0,0,0,100,0),
(@PATH,78,-315.032,2593.062,41.37146,0,0,0,0,100,0),
(@PATH,79,-333.1518,2616.684,42.40425,0,0,0,0,100,0),
(@PATH,80,-352.777,2633.742,40.45049,0,0,0,0,100,0),
(@PATH,81,-360.1994,2656.165,43.72129,0,0,0,0,100,0),
(@PATH,82,-344.7037,2680.905,36.1579,0,0,0,0,100,0),
(@PATH,83,-334.6349,2700.036,30.34241,0,0,0,0,100,0),
(@PATH,84,-324.9058,2712.125,26.43651,0,0,0,0,100,0),
(@PATH,85,-322.9779,2724.364,23.96236,0,0,0,0,100,0),
(@PATH,86,-326.3518,2734.545,22.44383,0,0,0,0,100,0),
(@PATH,87,-342.5912,2742.288,22.4451,0,0,0,0,100,0),
(@PATH,88,-368.7488,2739.919,27.63747,0,0,0,0,100,0),
(@PATH,89,-392.2097,2734.005,34.55191,0,0,0,0,100,0),
(@PATH,90,-414.4307,2741.14,39.55831,0,0,0,0,100,0),
(@PATH,91,-436.1942,2742.233,45.3562,0,0,0,0,100,0),
(@PATH,92,-462.9823,2756.808,49.85812,0,0,0,0,100,0),
(@PATH,93,-480.5942,2778.588,50.47923,0,0,0,0,100,0),
(@PATH,94,-476.0007,2799.573,45.95654,0,0,0,0,100,0),
(@PATH,95,-458.507,2835.351,35.33127,0,0,0,0,100,0),
(@PATH,96,-431.757,2863.309,23.74757,0,0,0,0,100,0),
(@PATH,97,-413.4223,2892.728,17.20714,0,0,0,0,100,0),
(@PATH,98,-419.2291,2905.467,19.51181,0,0,0,0,100,0),
(@PATH,99,-443.1462,2933.887,17.85199,0,0,0,0,100,0),
(@PATH,100,-438.8401,2966.167,8.723536,0,0,0,0,100,0),
(@PATH,101,-444.0433,2936.331,17.40722,0,0,0,0,100,0),
(@PATH,102,-432.5814,2918.56,19.31524,0,0,0,0,100,0),
(@PATH,103,-411.3571,2898.61,17.23361,0,0,0,0,100,0),
(@PATH,104,-429.0982,2867.209,22.665,0,0,0,0,100,0),
(@PATH,105,-457.4272,2836.473,34.75057,0,0,0,0,100,0),
(@PATH,106,-474.1913,2804.114,44.81591,0,0,0,0,100,0),
(@PATH,107,-481.0764,2783.36,50.09081,0,0,0,0,100,0),
(@PATH,108,-465.5521,2759.48,50.03743,0,0,0,0,100,0),
(@PATH,109,-440.5409,2744.546,46.12499,0,0,0,0,100,0),
(@PATH,110,-420.0836,2742.421,41.1034,0,0,0,0,100,0),
(@PATH,111,-397.9436,2734.918,35.90202,0,0,0,0,100,0),
(@PATH,112,-372.9858,2738.609,28.64789,0,0,0,0,100,0),
(@PATH,113,-348.4457,2742.212,23.42384,0,0,0,0,100,0),
(@PATH,114,-333.7679,2740.035,21.81408,0,0,0,0,100,0),
(@PATH,115,-324.9037,2731.125,22.83686,0,0,0,0,100,0),
(@PATH,116,-323.9969,2714.816,25.20854,0,0,0,0,100,0),
(@PATH,117,-332.7059,2702.867,29.4334,0,0,0,0,100,0),
(@PATH,118,-343.4227,2683.021,35.1899,0,0,0,0,100,0),
(@PATH,119,-359.6221,2658.229,43.55175,0,0,0,0,100,0),
(@PATH,120,-353.887,2635.825,39.48611,0,0,0,0,100,0),
(@PATH,121,-344.0572,2624.184,43.10011,0,0,0,0,100,0),
(@PATH,122,-325.4768,2613.256,41.99747,0,0,0,0,100,0),
(@PATH,123,-315.0096,2608.486,41.19632,0,0,0,0,100,0),
(@PATH,124,-325.0719,2578.15,44.60706,0,0,0,0,100,0),
(@PATH,125,-325.6835,2576.813,44.85286,0,0,0,0,100,0),
(@PATH,126,-313.8424,2562.751,43.86629,0,0,0,0,100,0),
(@PATH,127,-306.6296,2559.59,43.31482,0,0,0,0,100,0),
(@PATH,128,-303.079,2544.062,42.30245,0,0,0,0,100,0),
(@PATH,129,-324.1274,2514.437,40.29177,0,0,0,0,100,0),
(@PATH,130,-333.2019,2515.876,41.902,0,0,0,0,100,0),
(@PATH,131,-354.4488,2524.19,43.33042,0,0,0,0,100,0),
(@PATH,132,-359.7796,2523.489,44.53107,0,0,0,0,100,0),
(@PATH,133,-378.1317,2510.007,43.7447,0,0,0,0,100,0),
(@PATH,134,-382.583,2491,37.3352,0,0,0,0,100,0),
(@PATH,135,-368.7838,2483.057,28.09278,0,0,0,0,100,0),
(@PATH,136,-337.7688,2479.346,28.95182,0,0,0,0,100,0),
(@PATH,137,-318.8761,2478.915,37.70119,0,0,0,0,100,0),
(@PATH,138,-301.9458,2480.981,40.5145,0,0,0,0,100,0),
(@PATH,139,-300.0336,2481.129,40.53691,0,0,0,0,100,0),
(@PATH,140,-291.6542,2471.877,40.91128,0,0,0,0,100,0),
(@PATH,141,-295.2729,2454.313,42.07068,0,0,0,0,100,0),
(@PATH,142,-291.9165,2442.606,43.58534,0,0,0,0,100,0),
(@PATH,143,-290.83,2436.461,44.0534,0,0,0,0,100,0),
(@PATH,144,-296.628,2422.13,44.75555,0,0,0,0,100,0),
(@PATH,145,-284.9365,2416.829,46.52535,0,0,0,0,100,0),
(@PATH,146,-277.901,2404.396,49.46965,0,0,0,0,100,0),
(@PATH,147,-276.7875,2401.742,49.57491,0,0,0,0,100,0),
(@PATH,148,-275.867,2368.15,50.02176,0,0,0,0,100,0),
(@PATH,149,-280.12,2358.433,51.42045,0,0,0,0,100,0),
(@PATH,150,-271.6628,2337.664,53.90419,0,0,0,0,100,0),
(@PATH,151,-260.8519,2327.604,56.16253,0,0,0,0,100,0),
(@PATH,152,-246.7255,2313.879,53.0292,0,0,0,0,100,0),
(@PATH,153,-233.4331,2311.107,50.28912,0,0,0,0,100,0),
(@PATH,154,-194.8462,2316.223,54.13938,0,0,0,0,100,0),
(@PATH,155,-184.3219,2321.592,61.90179,0,0,0,0,100,0),
(@PATH,156,-169.6792,2336.901,61.27917,0,0,0,0,100,0),
(@PATH,157,-167.8382,2348.53,59.15134,0,0,0,0,100,0),
(@PATH,158,-168.2972,2362.321,57.01959,0,0,0,0,100,0),
(@PATH,159,-169.3826,2376.633,54.4596,0,0,0,0,100,0),
(@PATH,160,-171.8145,2382.753,52.97548,0,0,0,0,100,0),
(@PATH,161,-180.183,2392.34,51.01487,0,0,0,0,100,0),
(@PATH,162,-181.2741,2398.439,49.99238,0,0,0,0,100,0),
(@PATH,163,-175.461,2408.534,48.88574,0,0,0,0,100,0),
(@PATH,164,-157.5141,2417.718,45.03862,0,0,0,0,100,0),
(@PATH,165,-146.3332,2426.921,48.30313,0,0,0,0,100,0),
(@PATH,166,-137.9598,2445.376,46.12595,0,0,0,0,100,0),
(@PATH,167,-118.3926,2470.393,46.95731,0,0,0,0,100,0),
(@PATH,168,-124.8685,2492.812,46.93701,0,0,0,0,100,0),
(@PATH,169,-131.4405,2495.996,46.55548,0,0,0,0,100,0),
(@PATH,170,-145.8467,2499.76,45.61186,0,0,0,0,100,0),
(@PATH,171,-168.8438,2501.025,43.79488,0,0,0,0,100,0),
(@PATH,172,-176.3846,2510.593,41.94983,0,0,0,0,100,0),
(@PATH,173,-177.3363,2516.895,41.11181,0,0,0,0,100,0),
(@PATH,174,-170.4912,2527.817,41.62189,0,0,0,0,100,0),
(@PATH,175,-156.8518,2539.49,42.95174,0,0,0,0,100,0),
(@PATH,176,-146.1405,2544.73,40.98074,0,0,0,0,100,0),
(@PATH,177,-144.7979,2569.513,41.10892,0,0,0,0,100,0),
(@PATH,178,-165.0483,2590.722,39.90811,0,0,0,0,100,0),
(@PATH,179,-162.7764,2603.097,40.06981,0,0,0,0,100,0),
(@PATH,180,-164.5241,2612.235,40.08208,0,0,0,0,100,0),
(@PATH,181,-165.2749,2618.521,40.12231,0,0,0,0,100,0),
(@PATH,182,-166.9141,2633.801,41.55241,0,0,0,0,100,0),
(@PATH,183,-170.6192,2661.56,42.24799,0,0,0,0,100,0),
(@PATH,184,-166.924,2686.502,43.27299,0,0,0,0,100,0),
(@PATH,185,-165.7749,2686.863,43.39313,0,0,0,0,100,0),
(@PATH,186,-125.7525,2695.987,46.92542,0,0,0,0,100,0),
(@PATH,187,-108.798,2709.016,51.00445,0,0,0,0,100,0),
(@PATH,188,-110.122,2737.952,52.16914,0,0,0,0,100,0),
(@PATH,189,-120.4556,2753.937,49.37396,0,0,0,0,100,0),
(@PATH,190,-134.5498,2778.104,41.05849,0,0,0,0,100,0),
(@PATH,191,-153.882,2788.701,36.85746,0,0,0,0,100,0),
(@PATH,192,-186.3388,2797.655,26.33011,0,0,0,0,100,0),
(@PATH,193,-185.5681,2808.792,25.53679,0,0,0,0,100,0),
(@PATH,194,-172.6706,2803.528,29.62396,0,0,0,0,100,0),
(@PATH,195,-160.4427,2807.621,32.6923,0,0,0,0,100,0),
(@PATH,196,-149.3403,2813.561,35.25396,0,0,0,0,100,0),
(@PATH,197,-149.3422,2826.91,33.67762,0,0,0,0,100,0),
(@PATH,198,-168.0662,2837.727,26.36559,0,0,0,0,100,0),
(@PATH,199,-179.7917,2839.993,23.64687,0,0,0,0,100,0);
-- 0x1C09E4424012FCC000002C00005FBF34 .go -179.7917 2839.993 23.64687

-- Pathing for Bonechewer Raider Entry: 16925 'ODB FORMAT' 
SET @NPC := 58704;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-404.7873,`position_y`=2895.921,`position_z`=13.85265 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,17408,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-404.7873,2895.921,13.85265,0,0,0,0,100,0),
(@PATH,2,-404.2322,2900.299,14.15039,0,0,0,0,100,0),
(@PATH,3,-428.3772,2914.106,19.38362,0,0,0,0,100,0),
(@PATH,4,-439.7838,2928.416,18.55647,0,0,0,0,100,0),
(@PATH,5,-445.6255,2946.441,15.33809,0,0,0,0,100,0),
(@PATH,6,-439.2244,2964.096,9.26938,0,0,0,0,100,0),
(@PATH,7,-454.467,2971.035,9.009753,0,0,0,0,100,0),
(@PATH,8,-467.3622,2966.943,12.50773,0,0,0,0,100,0),
(@PATH,9,-482.1766,2976.099,11.32469,0,0,0,0,100,0),
(@PATH,10,-471.1085,2966.682,13.06236,0,0,0,0,100,0),
(@PATH,11,-456.4288,2970.18,9.417983,0,0,0,0,100,0),
(@PATH,12,-450.4304,2969.706,9.393501,0,0,0,0,100,0),
(@PATH,13,-446.038,2951.119,14.24401,0,0,0,0,100,0),
(@PATH,14,-442.8287,2934.824,17.44841,0,0,0,0,100,0),
(@PATH,15,-435.4904,2921.99,19.82334,0,0,0,0,100,0),
(@PATH,16,-404.6763,2913.004,15.59055,0,0,0,0,100,0);
-- 0x1C09E4424010874000002C00005FACA1 .go -404.7873 2895.921 13.85265

-- Pathing for Bonechewer Raider Entry: 16925 'ODB FORMAT' 
SET @NPC := 58705;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-394.2339,`position_y`=2891.354,`position_z`=8.341909 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,17408,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-394.2339,2891.354,8.341909,0,0,0,0,100,0),
(@PATH,2,-404.354,2907.934,14.82262,0,0,0,0,100,0),
(@PATH,3,-420.1548,2911.418,19.02849,0,0,0,0,100,0),
(@PATH,4,-434.6948,2920.658,19.59609,0,0,0,0,100,0),
(@PATH,5,-443.507,2937.645,16.52073,0,0,0,0,100,0),
(@PATH,6,-438.1014,2952.321,11.34356,0,0,0,0,100,0),
(@PATH,7,-445.5783,2967.97,9.331216,0,0,0,0,100,0),
(@PATH,8,-438.8247,2958.072,10.36737,0,0,0,0,100,0),
(@PATH,9,-442.9203,2943.862,15.03016,0,0,0,0,100,0),
(@PATH,10,-440.1529,2925.814,19.82526,0,0,0,0,100,0),
(@PATH,11,-430.9389,2917.94,18.84661,0,0,0,0,100,0),
(@PATH,12,-407.8214,2910.193,16.49719,0,0,0,0,100,0);
-- 0x1C09E4424010874000002C00005FACD2 .go -394.2339 2891.354 8.341909

-- Pathing for Bonechewer Raider Entry: 16925 'ODB FORMAT' 
SET @NPC := 58703;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-173.0258,`position_y`=2784.179,`position_z`=31.73058 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,17408,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-184.2477,2841.474,22.17058,0,0,0,0,100,0),
(@PATH,2,-186.5912,2837.849,21.70995,0,0,0,0,100,0),
(@PATH,3,-186.0354,2821.816,23.10514,0,0,0,0,100,0),
(@PATH,4,-190.4859,2804.101,24.46144,0,0,0,0,100,0),
(@PATH,5,-190.1222,2800.265,25.05125,0,0,0,0,100,0),
(@PATH,6,-185.8087,2784.323,28.49104,0,0,0,0,100,0),
(@PATH,7,-173.0258,2784.179,31.73058,0,0,0,0,100,0),
(@PATH,8,-153.8195,2788.142,37.07464,0,0,0,0,100,0),
(@PATH,9,-146.762,2781.243,39.44977,0,0,0,0,100,0),
(@PATH,10,-150.9473,2773.002,39.19555,0,0,0,0,100,0),
(@PATH,11,-165.231,2763.583,36.20363,0,0,0,0,100,0),
(@PATH,12,-181.1507,2755.388,30.1041,0,0,0,0,100,0),
(@PATH,13,-168.6309,2761.67,34.94637,0,0,0,0,100,0),
(@PATH,14,-157.0087,2768.504,38.33034,0,0,0,0,100,0),
(@PATH,15,-147.9143,2776.101,39.64468,0,0,0,0,100,0),
(@PATH,16,-148.772,2785.843,38.60817,0,0,0,0,100,0),
(@PATH,17,-167.8991,2784.874,33.46256,0,0,0,0,100,0),
(@PATH,18,-185.8087,2784.323,28.49104,0,0,0,0,100,0),
(@PATH,19,-190.1222,2800.265,25.05125,0,0,0,0,100,0),
(@PATH,20,-190.4859,2804.101,24.46144,0,0,0,0,100,0),
(@PATH,21,-186.0354,2821.816,23.10514,0,0,0,0,100,0),
(@PATH,22,-186.5912,2837.849,21.70995,0,0,0,0,100,0),
(@PATH,23,-184.2477,2841.474,22.17058,0,0,0,0,100,0);
-- 0x1C09E4424010874000002C00005FBE47 .go -173.0258 2784.179 31.73058

-- Pathing for Bonechewer Raider Entry: 16925 'ODB FORMAT' 
SET @NPC := 58707;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-180.3116,`position_y`=2686.223,`position_z`=40.81258 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,17408,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-180.3116,2686.223,40.81258,0,0,0,0,100,0),
(@PATH,2,-175.7397,2679.259,42.18403,0,0,0,0,100,0),
(@PATH,3,-171.9714,2660.608,42.08878,0,0,0,0,100,0),
(@PATH,4,-170.7084,2658.794,42.01125,0,0,0,0,100,0),
(@PATH,5,-171.3484,2640.479,41.27383,0,0,0,0,100,0),
(@PATH,6,-171.948,2638.051,41.02221,0,0,0,0,100,0),
(@PATH,7,-165.7785,2619.644,40.29411,0,0,0,0,100,0),
(@PATH,8,-166.9228,2612.098,39.69428,0,0,0,0,100,0),
(@PATH,9,-169.5662,2594.979,39.33304,0,0,0,0,100,0),
(@PATH,10,-170.6965,2594.06,39.20753,0,0,0,0,100,0),
(@PATH,11,-170.8916,2594.003,38.98936,0,0,0,0,100,0),
(@PATH,12,-166.2133,2597.965,39.64554,0,0,0,0,100,0),
(@PATH,13,-163.8443,2600.274,39.8479,0,0,0,0,100,0),
(@PATH,14,-165.4025,2621.016,40.56765,0,0,0,0,100,0),
(@PATH,15,-164.8262,2624.799,41.17817,0,0,0,0,100,0),
(@PATH,16,-170.1738,2643.713,41.53741,0,0,0,0,100,0),
(@PATH,17,-169.1673,2647.221,41.99223,0,0,0,0,100,0),
(@PATH,18,-171.7151,2670.958,42.49662,0,0,0,0,100,0);
-- 0x1C09E4424010874000002C00005FBFA9 .go -180.3116 2686.223 40.81258