-- Dragonmaw Skybreaker Pathing From sniff

SET @NPC := 78291;
SET @PATH := @NPC*10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-4919.375,`position_y`=764.9185,`position_z`=115.8213 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes2`) VALUES (@NPC,@PATH,20684,1);
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-4899.598,731.2864,115.8213,0,0,0,100,0),
(@PATH,2,-4897.536,699.804,115.8213,0,0,0,100,0),
(@PATH,3,-4907.958,682.649,115.8213,0,0,0,100,0),
(@PATH,4,-4928.547,677.3654,115.8213,0,0,0,100,0),
(@PATH,5,-4957.558,679.2119,115.8213,0,0,0,100,0),
(@PATH,6,-4979.058,694.0054,115.8213,0,0,0,100,0),
(@PATH,7,-5001.202,695.435,115.8213,0,0,0,100,0),
(@PATH,8,-5045.799,679.8455,115.8213,0,0,0,100,0),
(@PATH,9,-5082.077,674.1318,115.8213,0,0,0,100,0),
(@PATH,10,-5105.458,688.3859,126.0714,0,0,0,100,0),
(@PATH,11,-5124.419,708.7602,115.8213,0,0,0,100,0),
(@PATH,12,-5131.542,743.6093,115.8213,0,0,0,100,0),
(@PATH,13,-5122.893,762.6793,115.8213,0,0,0,100,0),
(@PATH,14,-5109.942,776.4905,127.7102,0,0,0,100,0),
(@PATH,15,-5082.581,787.4044,140.2658,0,0,0,100,0),
(@PATH,16,-5048.687,787.3963,129.2936,0,0,0,100,0),
(@PATH,17,-5029.247,794.6903,105.6824,0,0,0,100,0),
(@PATH,18,-5004.71,788.2552,111.238,0,0,0,100,0),
(@PATH,19,-4974.424,775.7346,115.8213,0,0,0,100,0),
(@PATH,20,-4944.215,765.6434,115.8213,0,0,0,100,0),
(@PATH,21,-4919.375,764.9185,115.8213,0,0,0,100,0);

SET @NPC := 78292;
SET @PATH := @NPC*10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-4914.272,`position_y`=182.6655,`position_z`=105.3898 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes2`) VALUES (@NPC,@PATH,20684,1);
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-4927.019,212.1301,105.3898,0,0,0,100,0),
(@PATH,2,-4948.965,230.0244,105.3898,0,0,0,100,0),
(@PATH,3,-4975.733,226.1698,105.3898,0,0,0,100,0),
(@PATH,4,-4991.351,199.1344,103.001,0,0,0,100,0),
(@PATH,5,-5005.696,153.1891,116.7409,0,0,0,100,0),
(@PATH,6,-4998.949,135.7159,116.7409,0,0,0,100,0),
(@PATH,7,-4970.921,115.2041,116.7409,0,0,0,100,0),
(@PATH,8,-4920.846,153.8924,105.3898,0,0,0,100,0),
(@PATH,9,-4914.272,182.6655,105.3898,0,0,0,100,0);

SET @NPC := 78293;
SET @PATH := @NPC*10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-5001.053,`position_y`=507.8676,`position_z`=125.1893 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes2`) VALUES (@NPC,@PATH,20684,1);
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-4976.919,535.0216,125.1893,0,0,0,100,0),
(@PATH,2,-4945.772,544.8293,125.1893,0,0,0,100,0),
(@PATH,3,-4916.477,541.136,125.1893,0,0,0,100,0),
(@PATH,4,-4894.755,534.1898,125.1893,0,0,0,100,0),
(@PATH,5,-4878.672,517.9357,125.1893,0,0,0,100,0),
(@PATH,6,-4867.688,472.2867,125.1893,0,0,0,100,0),
(@PATH,7,-4866.018,440.9231,125.1893,0,0,0,100,0),
(@PATH,8,-4884.331,411.0111,125.1893,0,0,0,100,0),
(@PATH,9,-4909.612,389.5723,125.1893,0,0,0,100,0),
(@PATH,10,-4945.339,386.204,125.1893,0,0,0,100,0),
(@PATH,11,-4967.832,409.3968,125.1893,0,0,0,100,0),
(@PATH,12,-4986.726,443.7907,125.1893,0,0,0,100,0),
(@PATH,13,-4997.688,466.8292,125.1893,0,0,0,100,0),
(@PATH,14,-5001.053,507.8676,125.1893,0,0,0,100,0);

SET @NPC := 78294;
SET @PATH := @NPC*10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-4957.768,`position_y`=-85.91428,`position_z`=117.7407 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes2`) VALUES (@NPC,@PATH,20684,1);
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-4979.801,-54.69292,117.2962,0,0,0,100,0),
(@PATH,2,-4973.231,-17.18837,111.824,0,0,0,100,0),
(@PATH,3,-4911.041,4.472439,118.324,0,0,0,100,0),
(@PATH,4,-4873.144,-22.93349,119.0185,0,0,0,100,0),
(@PATH,5,-4870.32,-65.83746,122.9629,0,0,0,100,0),
(@PATH,6,-4885.412,-99.36697,120.2129,0,0,0,100,0),
(@PATH,7,-4918.261,-108.4286,117.824,0,0,0,100,0),
(@PATH,8,-4957.768,-85.91428,117.7407,0,0,0,100,0);

SET @NPC := 78295;
SET @PATH := @NPC*10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-4989.571,`position_y`=277.2158,`position_z`=129.4455 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes2`) VALUES (@NPC,@PATH,20684,1);
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-4981.606,252.022,129.4455,0,0,0,100,0),
(@PATH,2,-4961.62,234.8766,129.4455,0,0,0,100,0),
(@PATH,3,-4923.01,242.2214,129.4455,0,0,0,100,0),
(@PATH,4,-4900.703,257.2026,129.4455,0,0,0,100,0),
(@PATH,5,-4879.407,279.7445,129.4455,0,0,0,100,0),
(@PATH,6,-4865.226,310.3156,129.4455,0,0,0,100,0),
(@PATH,7,-4859.635,335.26,129.4455,0,0,0,100,0),
(@PATH,8,-4863.772,361.1568,129.4455,0,0,0,100,0),
(@PATH,9,-4897.918,374.3784,129.4455,0,0,0,100,0),
(@PATH,10,-4935.554,362.9577,129.4455,0,0,0,100,0),
(@PATH,11,-4956.634,339.5631,129.4455,0,0,0,100,0),
(@PATH,12,-4972.443,308.4228,129.4455,0,0,0,100,0),
(@PATH,13,-4989.571,277.2158,129.4455,0,0,0,100,0);

SET @NPC := 78296;
SET @PATH := @NPC*10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-5013.096,`position_y`=529.8334,`position_z`=147.466 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes2`) VALUES (@NPC,@PATH,20684,1);
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-5034.7,530.1956,147.466,0,0,0,100,0),
(@PATH,2,-5058.567,549.2531,147.466,0,0,0,100,0),
(@PATH,3,-5077.942,585.5693,147.466,0,0,0,100,0),
(@PATH,4,-5085.871,616.8538,147.466,0,0,0,100,0),
(@PATH,5,-5073.225,640.2809,147.466,0,0,0,100,0),
(@PATH,6,-5046.584,656.348,147.466,0,0,0,100,0),
(@PATH,7,-5016.901,662.3196,147.466,0,0,0,100,0),
(@PATH,8,-4990.232,642.3178,147.466,0,0,0,100,0),
(@PATH,9,-4965.018,595.1047,147.466,0,0,0,100,0),
(@PATH,10,-4970.888,561.2855,147.466,0,0,0,100,0),
(@PATH,11,-4988.07,544.9948,147.466,0,0,0,100,0),
(@PATH,12,-5013.096,529.8334,147.466,0,0,0,100,0);

SET @NPC := 78297;
SET @PATH := @NPC*10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-5115.668,`position_y`=15.1122,`position_z`=156.0377 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes2`) VALUES (@NPC,@PATH,20684,1);
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-5081.034,3.025608,156.0377,0,0,0,100,0),
(@PATH,2,-5054.209,-29.95432,156.0377,0,0,0,100,0),
(@PATH,3,-5044.757,-53.49219,156.0377,0,0,0,100,0),
(@PATH,4,-5063.791,-87.67003,156.0377,0,0,0,100,0),
(@PATH,5,-5100.97,-101.5627,156.0377,0,0,0,100,0),
(@PATH,6,-5139.875,-93.61002,156.0377,0,0,0,100,0),
(@PATH,7,-5157.834,-72.95584,158.5654,0,0,0,100,0),
(@PATH,8,-5150.443,-50.55273,150.9543,0,0,0,100,0),
(@PATH,9,-5154.681,-12.24284,156.0377,0,0,0,100,0),
(@PATH,10,-5139.686,8.807183,156.0377,0,0,0,100,0),
(@PATH,11,-5115.668,15.1122,156.0377,0,0,0,100,0);

SET @NPC := 78298;
SET @PATH := @NPC*10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-5128.899,`position_y`=485.1413,`position_z`=149.4686 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes2`) VALUES (@NPC,@PATH,20684,1);
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-5096.786,491.9447,149.4686,0,0,0,100,0),
(@PATH,2,-5081.393,520.9803,149.5797,0,0,0,100,0),
(@PATH,3,-5082.821,560.8444,149.4686,0,0,0,100,0),
(@PATH,4,-5081.688,587.3782,149.4686,0,0,0,100,0),
(@PATH,5,-5096.75,614.0341,149.4686,0,0,0,100,0),
(@PATH,6,-5121.846,626.2573,149.4686,0,0,0,100,0),
(@PATH,7,-5143.037,616.5057,149.4686,0,0,0,100,0),
(@PATH,8,-5162.803,600.3235,149.4686,0,0,0,100,0),
(@PATH,9,-5175.979,585.7052,149.4686,0,0,0,100,0),
(@PATH,10,-5191.311,553.0349,149.4686,0,0,0,100,0),
(@PATH,11,-5182.97,516.0972,149.4686,0,0,0,100,0),
(@PATH,12,-5169.338,495.5091,149.4686,0,0,0,100,0),
(@PATH,13,-5128.899,485.1413,149.4686,0,0,0,100,0);

SET @NPC := 78299;
SET @PATH := @NPC*10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-5203.09,`position_y`=230.2053,`position_z`=125.2333 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes2`) VALUES (@NPC,@PATH,20684,1);
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-5200.994,194.2652,125.2333,0,0,0,100,0),
(@PATH,2,-5209.843,162.355,125.2333,0,0,0,100,0),
(@PATH,3,-5223.84,126.7766,125.2333,0,0,0,100,0),
(@PATH,4,-5254.94,108.4594,125.2333,0,0,0,100,0),
(@PATH,5,-5272.563,119.1082,125.2333,0,0,0,100,0),
(@PATH,6,-5293.776,154.223,125.2333,0,0,0,100,0),
(@PATH,7,-5298.729,171.8369,125.2333,0,0,0,100,0),
(@PATH,8,-5300.312,208.0347,125.2333,0,0,0,100,0),
(@PATH,9,-5283.916,251.8668,125.2333,0,0,0,100,0),
(@PATH,10,-5241.004,262.8554,125.2333,0,0,0,100,0),
(@PATH,11,-5203.09,230.2053,125.2333,0,0,0,100,0);

SET @NPC := 78300;
SET @PATH := @NPC*10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-5234.025,`position_y`=262.7202,`position_z`=131.392 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes2`) VALUES (@NPC,@PATH,20684,1);
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-5255.758,267.988,131.392,0,0,0,100,0),
(@PATH,2,-5280.832,303.0705,131.392,0,0,0,100,0),
(@PATH,3,-5282.729,322.4258,131.392,0,0,0,100,0),
(@PATH,4,-5269.899,354.7595,131.392,0,0,0,100,0),
(@PATH,5,-5244.224,370.2118,131.392,0,0,0,100,0),
(@PATH,6,-5201.63,377.0772,131.392,0,0,0,100,0),
(@PATH,7,-5186.241,362.2412,131.392,0,0,0,100,0),
(@PATH,8,-5178.872,334.1438,131.392,0,0,0,100,0),
(@PATH,9,-5178.549,305.8703,131.392,0,0,0,100,0),
(@PATH,10,-5190.308,282.2744,131.392,0,0,0,100,0),
(@PATH,11,-5208.153,267.4322,131.392,0,0,0,100,0),
(@PATH,12,-5234.025,262.7202,131.392,0,0,0,100,0);

-- Mature Netherwing Drake Pathing From sniff 1 added.
SET @NPC := 75541;
SET @PATH := @NPC*10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-4134.369,`position_y`=850.2852,`position_z`=61.26378 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`bytes2`) VALUES (@NPC,@PATH,1);
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-4120.351,883.5854,61.26378,0,0,0,100,0),
(@PATH,2,-4085.13,900.4695,61.26378,0,0,0,100,0),
(@PATH,3,-4048.586,884.7811,61.26378,0,0,0,100,0),
(@PATH,4,-4033.683,852.1551,61.26378,0,0,0,100,0),
(@PATH,5,-4050.652,818.6749,61.26378,0,0,0,100,0),
(@PATH,6,-4085.118,799.5022,61.26378,0,0,0,100,0),
(@PATH,7,-4117.451,815.256,61.26378,0,0,0,100,0),
(@PATH,8,-4134.369,850.2852,61.26378,0,0,0,100,0);

SET @NPC := 75542;
SET @PATH := @NPC*10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-4066.473,`position_y`=752.3994,`position_z`=43.04424 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`bytes2`) VALUES (@NPC,@PATH,1);
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-4083.436,721.6484,43.04424,0,0,0,100,0),
(@PATH,2,-4109.103,700.2794,43.04424,0,0,0,100,0),
(@PATH,3,-4114.49,699.9313,43.04424,0,0,0,100,0),
(@PATH,4,-4149.18,716.9812,43.04424,0,0,0,100,0),
(@PATH,5,-4164.394,749.7984,44.87757,0,0,0,100,0),
(@PATH,6,-4147.782,783.0017,46.62758,0,0,0,100,0),
(@PATH,7,-4117.128,799.6888,51.93314,0,0,0,100,0),
(@PATH,8,-4082.698,785.6704,50.3498,0,0,0,100,0),
(@PATH,9,-4066.473,752.3994,43.04424,0,0,0,100,0);

SET @NPC := 75543;
SET @PATH := @NPC*10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-4148.391,`position_y`=766.8653,`position_z`=66.01289 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`bytes2`) VALUES (@NPC,@PATH,1);
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-4183.416,753.0403,72.09624,0,0,0,100,0),
(@PATH,2,-4199.977,716.2882,82.67956,0,0,0,100,0),
(@PATH,3,-4183.724,684.1388,70.56844,0,0,0,100,0),
(@PATH,4,-4148.917,666.9986,63.51288,0,0,0,100,0),
(@PATH,5,-4116.004,684.5375,61.51288,0,0,0,100,0),
(@PATH,6,-4099.94,716.4078,62.09623,0,0,0,100,0),
(@PATH,7,-4116.647,749.6686,61.59621,0,0,0,100,0),
(@PATH,8,-4148.391,766.8653,66.01289,0,0,0,100,0);

SET @NPC := 75544;
SET @PATH := @NPC*10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-4084.089,`position_y`=683.0483,`position_z`=59.55887 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`bytes2`) VALUES (@NPC,@PATH,1);
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-4067.996,652.302,59.55887,0,0,0,100,0),
(@PATH,2,-4084.038,618.9259,59.55887,0,0,0,100,0),
(@PATH,3,-4117.386,599.9404,59.55887,0,0,0,100,0),
(@PATH,4,-4149.88,616.7292,59.55887,0,0,0,100,0),
(@PATH,5,-4165.804,652.2473,59.55887,0,0,0,100,0),
(@PATH,6,-4150.054,685.6966,59.55887,0,0,0,100,0),
(@PATH,7,-4118.107,699.7501,59.55887,0,0,0,100,0),
(@PATH,8,-4084.089,683.0483,59.55887,0,0,0,100,0);

SET @NPC := 75545;
SET @PATH := @NPC*10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-4083.681,`position_y`=848.0616,`position_z`=58.43416 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`bytes2`) VALUES (@NPC,@PATH,1);
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-4117.843,866.6574,58.43417,0,0,0,100,0),
(@PATH,2,-4149.746,851.1995,68.6286,0,0,0,100,0),
(@PATH,3,-4167.114,815.8797,74.48971,0,0,0,100,0),
(@PATH,4,-4150.918,786.3128,65.07306,0,0,0,100,0),
(@PATH,5,-4116.956,766.7995,70.51749,0,0,0,100,0),
(@PATH,6,-4085.521,784.5572,76.90639,0,0,0,100,0),
(@PATH,7,-4067.243,818.4576,64.35083,0,0,0,100,0),
(@PATH,8,-4083.681,848.0616,58.43416,0,0,0,100,0);

SET @NPC := 75546;
SET @PATH := @NPC*10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-4133.319,`position_y`=918.6325,`position_z`=70.3725 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`bytes2`) VALUES (@NPC,@PATH,1);
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-4117.341,884.8878,62.09472,0,0,0,100,0),
(@PATH,2,-4084.125,867.1631,52.70584,0,0,0,100,0),
(@PATH,3,-4050.462,883.2648,46.84472,0,0,0,100,0),
(@PATH,4,-4033.427,917.2786,54.8725,0,0,0,100,0),
(@PATH,5,-4049.515,951.7986,62.20583,0,0,0,100,0),
(@PATH,6,-4082.625,966.6864,69.48361,0,0,0,100,0),
(@PATH,7,-4116.243,949.7843,72.2614,0,0,0,100,0),
(@PATH,8,-4133.319,918.6325,70.3725,0,0,0,100,0);

SET @NPC := 75547;
SET @PATH := @NPC*10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-4000.944,`position_y`=650.089,`position_z`=84.74791 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`bytes2`) VALUES (@NPC,@PATH,1);
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-4015.128,680.8443,72.16458,0,0,0,100,0),
(@PATH,2,-4047.654,700.2465,60.27572,0,0,0,100,0),
(@PATH,3,-4082.378,686.9391,58.8868,0,0,0,100,0),
(@PATH,4,-4099.23,649.127,67.30346,0,0,0,100,0),
(@PATH,5,-4083.47,615.1847,74.74792,0,0,0,100,0),
(@PATH,6,-4047.911,600.5391,92.52572,0,0,0,100,0),
(@PATH,7,-4016.922,616.1044,98.47014,0,0,0,100,0),
(@PATH,8,-4000.944,650.089,84.74791,0,0,0,100,0);

SET @NPC := 75548;
SET @PATH := @NPC*10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-4052.34,`position_y`=716.3079,`position_z`=48.66938 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`bytes2`) VALUES (@NPC,@PATH,1);
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-4019.384,700.1765,47.08604,0,0,0,100,0),
(@PATH,2,-3983.657,716.8768,52.75271,0,0,0,100,0),
(@PATH,3,-3967.16,750.4713,65.47495,0,0,0,100,0),
(@PATH,4,-3982.856,783.4985,67.83604,0,0,0,100,0),
(@PATH,5,-4017.322,800.1605,65.75271,0,0,0,100,0),
(@PATH,6,-4048.333,784.6697,60.58604,0,0,0,100,0),
(@PATH,7,-4067.148,750.3481,53.25271,0,0,0,100,0),
(@PATH,8,-4052.34,716.3079,48.66938,0,0,0,100,0);

SET @NPC := 75549;
SET @PATH := @NPC*10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-3949.365,`position_y`=819.178,`position_z`=51.50261 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`bytes2`) VALUES (@NPC,@PATH,1);
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-3933.805,784.3923,51.50261,0,0,0,100,0),
(@PATH,2,-3951.698,751.0433,51.50261,0,0,0,100,0),
(@PATH,3,-3980.543,733.1439,58.86372,0,0,0,100,0),
(@PATH,4,-4016.032,747.1483,64.55817,0,0,0,100,0),
(@PATH,5,-4032.366,782.211,60.08595,0,0,0,100,0),
(@PATH,6,-4017.203,816.9814,53.30817,0,0,0,100,0),
(@PATH,7,-3985.723,832.2205,51.50261,0,0,0,100,0),
(@PATH,8,-3949.365,819.178,51.50261,0,0,0,100,0);

SET @NPC := 75550;
SET @PATH := @NPC*10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-3916.397,`position_y`=883.2978,`position_z`=81.9971 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`bytes2`) VALUES (@NPC,@PATH,1);
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-3948.779,899.8286,68.13598,0,0,0,100,0),
(@PATH,2,-3982.235,885.5906,61.1082,0,0,0,100,0),
(@PATH,3,-3999.414,851.4448,62.69153,0,0,0,100,0),
(@PATH,4,-3983.74,817.0834,58.96931,0,0,0,100,0),
(@PATH,5,-3950.159,800.5572,63.10819,0,0,0,100,0),
(@PATH,6,-3918.94,815.5467,72.46931,0,0,0,100,0),
(@PATH,7,-3900.264,851.1897,81.30264,0,0,0,100,0),
(@PATH,8,-3916.397,883.2978,81.9971,0,0,0,100,0);

SET @NPC := 75551;
SET @PATH := @NPC*10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-4084.883,`position_y`=914.0491,`position_z`=72.2939 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`bytes2`) VALUES (@NPC,@PATH,1);
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-4099.644,950.2539,69.2939,0,0,0,100,0),
(@PATH,2,-4086.421,983.3143,71.12724,0,0,0,100,0),
(@PATH,3,-4050.086,1000.082,74.32168,0,0,0,100,0),
(@PATH,4,-4018.68,983.5675,74.32168,0,0,0,100,0),
(@PATH,5,-4001.11,950.1692,74.32168,0,0,0,100,0),
(@PATH,6,-4016.584,917.4998,74.32168,0,0,0,100,0),
(@PATH,7,-4048.498,899.8286,74.32168,0,0,0,100,0),
(@PATH,8,-4084.883,914.0491,72.2939,0,0,0,100,0);

SET @NPC := 75552;
SET @PATH := @NPC*10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-3915.657,`position_y`=833.4163,`position_z`=47.38418 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`bytes2`) VALUES (@NPC,@PATH,1);
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-3948.898,849.9359,47.38418,0,0,0,100,0),
(@PATH,2,-3964.938,879.6131,47.38418,0,0,0,100,0),
(@PATH,3,-3948.945,916.1858,47.38418,0,0,0,100,0),
(@PATH,4,-3916.809,932.6107,48.16196,0,0,0,100,0),
(@PATH,5,-3886.793,916.5725,53.96752,0,0,0,100,0),
(@PATH,6,-3866.807,883.3404,60.02308,0,0,0,100,0),
(@PATH,7,-3884.739,849.9593,54.30085,0,0,0,100,0),
(@PATH,8,-3915.657,833.4163,47.38418,0,0,0,100,0);

SET @NPC := 75553;
SET @PATH := @NPC*10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-4047.742,`position_y`=1066.566,`position_z`=106.1299 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`bytes2`) VALUES (@NPC,@PATH,1);
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-4082.141,1050.517,104.5466,0,0,0,100,0),
(@PATH,2,-4099.718,1017.106,98.4355,0,0,0,100,0),
(@PATH,3,-4082.201,985.4608,87.46328,0,0,0,100,0),
(@PATH,4,-4048.188,966.8837,78.15773,0,0,0,100,0),
(@PATH,5,-4017.819,980.5856,72.37994,0,0,0,100,0),
(@PATH,6,-4000.326,1016.784,86.21326,0,0,0,100,0),
(@PATH,7,-4014.791,1050.645,97.40771,0,0,0,100,0),
(@PATH,8,-4047.742,1066.566,106.1299,0,0,0,100,0);

SET @NPC := 75554;
SET @PATH := @NPC*10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-3900.323,`position_y`=984.7424,`position_z`=60.60864 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`bytes2`) VALUES (@NPC,@PATH,1);
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-3918.999,953.8466,58.96975,0,0,0,100,0),
(@PATH,2,-3950.793,934.2088,58.96975,0,0,0,100,0),
(@PATH,3,-3982.866,950.2649,58.96975,0,0,0,100,0),
(@PATH,4,-4000.046,985.8019,61.02531,0,0,0,100,0),
(@PATH,5,-3981.982,1017.846,58.96975,0,0,0,100,0),
(@PATH,6,-3949.962,1033.053,56.85864,0,0,0,100,0),
(@PATH,7,-3918.825,1014.746,58.33086,0,0,0,100,0),
(@PATH,8,-3900.323,984.7424,60.60864,0,0,0,100,0);

SET @NPC := 75555;
SET @PATH := @NPC*10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-3950.083,`position_y`=985.4277,`position_z`=83.53654 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`bytes2`) VALUES (@NPC,@PATH,1);
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-3965.832,952.775,76.7032,0,0,0,100,0),
(@PATH,2,-3951.587,918.8143,71.53653,0,0,0,100,0),
(@PATH,3,-3916.848,899.7321,72.39764,0,0,0,100,0),
(@PATH,4,-3885.93,916.5598,83.34209,0,0,0,100,0),
(@PATH,5,-3867.241,951.6891,90.39765,0,0,0,100,0),
(@PATH,6,-3882.38,983.3097,81.48098,0,0,0,100,0),
(@PATH,7,-3915.022,1000.09,72.17542,0,0,0,100,0),
(@PATH,8,-3950.083,985.4277,83.53654,0,0,0,100,0);

SET @NPC := 75556;
SET @PATH := @NPC*10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-3966.719,`position_y`=1048.378,`position_z`=69.52766 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`bytes2`) VALUES (@NPC,@PATH,1);
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-3983.504,1016.722,67.94434,0,0,0,100,0),
(@PATH,2,-4015.046,1002.573,69.52766,0,0,0,100,0),
(@PATH,3,-4049.708,1017.723,72.13878,0,0,0,100,0),
(@PATH,4,-4066.48,1048.968,72.66656,0,0,0,100,0),
(@PATH,5,-4048.204,1083.848,71.19434,0,0,0,100,0),
(@PATH,6,-4016.47,1100.259,72.47211,0,0,0,100,0),
(@PATH,7,-3985.822,1082.684,70.97211,0,0,0,100,0),
(@PATH,8,-3966.719,1048.378,69.52766,0,0,0,100,0);

SET @NPC := 75557;
SET @PATH := @NPC*10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-3950.767,`position_y`=1000.512,`position_z`=87.49402 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`bytes2`) VALUES (@NPC,@PATH,1);
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-3918.137,1015.368,90.16069,0,0,0,100,0),
(@PATH,2,-3900.402,1050.901,91.7718,0,0,0,100,0),
(@PATH,3,-3917.262,1083.885,99.32737,0,0,0,100,0),
(@PATH,4,-3949.68,1100.575,106.7718,0,0,0,100,0),
(@PATH,5,-3981.045,1082.341,107.8829,0,0,0,100,0),
(@PATH,6,-3999.798,1048.804,99.57736,0,0,0,100,0),
(@PATH,7,-3985.399,1018.346,91.7718,0,0,0,100,0),
(@PATH,8,-3950.767,1000.512,87.49402,0,0,0,100,0);

SET @NPC := 75558;
SET @PATH := @NPC*10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-4066.473,`position_y`=752.3994,`position_z`=43.04424 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`bytes2`) VALUES (@NPC,@PATH,1);
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-4083.436,721.6484,43.04424,0,0,0,100,0),
(@PATH,2,-4109.103,700.2794,43.04424,0,0,0,100,0),
(@PATH,3,-4114.49,699.9313,43.04424,0,0,0,100,0),
(@PATH,4,-4149.18,716.9812,43.04424,0,0,0,100,0),
(@PATH,5,-4164.394,749.7984,44.87757,0,0,0,100,0),
(@PATH,6,-4147.782,783.0017,46.62758,0,0,0,100,0),
(@PATH,7,-4117.128,799.6888,51.93314,0,0,0,100,0),
(@PATH,8,-4082.698,785.6704,50.3498,0,0,0,100,0),
(@PATH,9,-4066.473,752.3994,43.04424,0,0,0,100,0);


-- Waypoints for Vilewing Chimaera
SET @NPC := 84613;
SET @PATH := @NPC*10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-3495.376,`position_y`=2654.514,`position_z`=85.14965 WHERE `guid`=@NPC;
DELETE FROM `creature` WHERE `guid` IN (84611,84490);
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`bytes2`) VALUES (@NPC,@PATH,1);
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-3513.771,2667.212,85.87186,0,0,0,100,0),
(@PATH,2,-3527.99,2668.341,87.39961,0,0,0,100,0),
(@PATH,3,-3568.938,2660.981,87.3718,0,0,0,100,0),
(@PATH,4,-3605.726,2631.423,89.09409,0,0,0,100,0),
(@PATH,5,-3606.994,2590.197,89.01077,0,0,0,100,0),
(@PATH,6,-3624.849,2552.972,89.84412,0,0,0,100,0),
(@PATH,7,-3632.875,2523.194,90.78855,0,0,0,100,0),
(@PATH,8,-3623.275,2498.775,93.42754,0,0,0,100,0),
(@PATH,9,-3594.59,2497.109,91.81643,0,0,0,100,0),
(@PATH,10,-3571.224,2522.083,89.76082,0,0,0,100,0),
(@PATH,11,-3555.303,2550.05,88.20523,0,0,0,100,0),
(@PATH,12,-3546.012,2576.069,86.67743,0,0,0,100,0),
(@PATH,13,-3533.117,2604.753,86.59406,0,0,0,100,0),
(@PATH,14,-3505.657,2622.679,86.03844,0,0,0,100,0),
(@PATH,15,-3495.376,2654.514,85.14965,0,0,0,100,0);

SET @NPC := 84632;
SET @PATH := @NPC*10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-3051.686,`position_y`=1036.492,`position_z`=18.42691 WHERE `guid`=@NPC;
DELETE FROM `creature` WHERE `guid` IN (84629,84630);
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`bytes2`) VALUES (@NPC,@PATH,1);
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-3078.817,1034.035,19.28807,0,0,0,100,0),
(@PATH,2,-3088.495,1066.141,25.0936,0,0,0,100,0),
(@PATH,3,-3087.413,1099.35,30.84361,0,0,0,100,0),
(@PATH,4,-3067.319,1113.823,27.62129,0,0,0,100,0),
(@PATH,5,-3035.007,1131.8,22.48246,0,0,0,100,0),
(@PATH,6,-3022.366,1165.862,21.28805,0,0,0,100,0),
(@PATH,7,-3028.652,1200.958,24.2047,0,0,0,100,0),
(@PATH,8,-3053.869,1219.137,31.12142,0,0,0,100,0),
(@PATH,9,-3098.912,1230.556,38.8714,0,0,0,100,0),
(@PATH,10,-3120.39,1233.38,42.48251,0,0,0,100,0),
(@PATH,11,-3155.853,1240.664,47.03807,0,0,0,100,0),
(@PATH,12,-3184.486,1256.954,49.17699,0,0,0,100,0),
(@PATH,13,-3200.051,1273.893,49.17699,0,0,0,100,0),
(@PATH,14,-3198.467,1300.309,47.56586,0,0,0,100,0),
(@PATH,15,-3180.189,1312.811,46.51028,0,0,0,100,0),
(@PATH,16,-3166.775,1318.915,44.56582,0,0,0,100,0),
(@PATH,17,-3133.76,1306.827,33.8992,0,0,0,100,0),
(@PATH,18,-3107.957,1267.029,29.7603,0,0,0,100,0),
(@PATH,19,-3102.756,1234.923,31.56591,0,0,0,100,0),
(@PATH,20,-3081.735,1200.232,34.76027,0,0,0,100,0),
(@PATH,21,-3064.895,1171.269,31.59359,0,0,0,100,0),
(@PATH,22,-3032.818,1161.913,24.92694,0,0,0,100,0),
(@PATH,23,-2998.293,1162.476,17.28803,0,0,0,100,0),
(@PATH,24,-2989.174,1125.52,23.26031,0,0,0,100,0),
(@PATH,25,-3019.696,1108.528,28.37141,0,0,0,100,0),
(@PATH,26,-3030.191,1069.277,24.06586,0,0,0,100,0),
(@PATH,27,-3051.686,1036.492,18.42691,0,0,0,100,0);

SET @NPC := 84594;
SET @PATH := @NPC*10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-3306.025,`position_y`=2329.368,`position_z`=83.54176 WHERE `guid`=@NPC;
DELETE FROM `creature` WHERE `guid` IN (84609,84610);
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`bytes2`) VALUES (@NPC,@PATH,1);
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-3306.578,2358.007,83.95837,0,0,0,100,0),
(@PATH,2,-3312.279,2366.838,84.12508,0,0,0,100,0),
(@PATH,3,-3339.576,2384.215,85.01395,0,0,0,100,0),
(@PATH,4,-3375.194,2400.308,84.79176,0,0,0,100,0),
(@PATH,5,-3413.535,2413.8,83.40285,0,0,0,100,0),
(@PATH,6,-3449.745,2447.95,84.59732,0,0,0,100,0),
(@PATH,7,-3484.88,2475.859,87.48625,0,0,0,100,0),
(@PATH,8,-3516.799,2470.339,88.65293,0,0,0,100,0),
(@PATH,9,-3551.302,2449.093,88.65293,0,0,0,100,0),
(@PATH,10,-3577.611,2417.917,88.65293,0,0,0,100,0),
(@PATH,11,-3587.224,2394.964,88.65293,0,0,0,100,0),
(@PATH,12,-3578.091,2358.175,91.01408,0,0,0,100,0),
(@PATH,13,-3542.674,2341.767,90.73628,0,0,0,100,0),
(@PATH,14,-3508.299,2338.776,88.65293,0,0,0,100,0),
(@PATH,15,-3480.534,2321.327,85.20841,0,0,0,100,0),
(@PATH,16,-3438.703,2328.433,84.01396,0,0,0,100,0),
(@PATH,17,-3402.306,2319.822,84.06959,0,0,0,100,0),
(@PATH,18,-3367.927,2320.204,83.40282,0,0,0,100,0),
(@PATH,19,-3336.193,2315.693,83.7084,0,0,0,100,0),
(@PATH,20,-3306.025,2329.368,83.54176,0,0,0,100,0);

SET @NPC := 84597;
SET @PATH := @NPC*10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-3679.033,`position_y`=1980.171,`position_z`=95.40069 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`bytes2`) VALUES (@NPC,@PATH,1);
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-3694.715,1971.449,97.03951,0,0,0,100,0),
(@PATH,2,-3698.604,1961.438,97.51176,0,0,0,100,0),
(@PATH,3,-3703.094,1932.741,93.09521,0,0,0,100,0),
(@PATH,4,-3710.8,1898.883,95.06728,0,0,0,100,0),
(@PATH,5,-3706.167,1855.215,94.65068,0,0,0,100,0),
(@PATH,6,-3730.437,1822.595,97.15064,0,0,0,100,0),
(@PATH,7,-3764.347,1786.201,103.5396,0,0,0,100,0),
(@PATH,8,-3806.215,1765.616,104.6232,0,0,0,100,0),
(@PATH,9,-3831.599,1734.274,106.4841,0,0,0,100,0),
(@PATH,10,-3845.836,1700.599,104.7896,0,0,0,100,0),
(@PATH,11,-3860.267,1667.299,102.9841,0,0,0,100,0),
(@PATH,12,-3878.303,1634.768,97.234,0,0,0,100,0),
(@PATH,13,-3905.483,1624.397,96.40066,0,0,0,100,0),
(@PATH,14,-3934.358,1629.903,95.78949,0,0,0,100,0),
(@PATH,15,-3941.387,1653.258,102.4008,0,0,0,100,0),
(@PATH,16,-3933.618,1681.023,107.5399,0,0,0,100,0),
(@PATH,17,-3911.209,1703.6,110.7342,0,0,0,100,0),
(@PATH,18,-3870.417,1717.617,111.4286,0,0,0,100,0),
(@PATH,19,-3851.412,1733.487,109.7619,0,0,0,100,0),
(@PATH,20,-3832.467,1767.414,108.4843,0,0,0,100,0),
(@PATH,21,-3821.692,1801.011,106.7619,0,0,0,100,0),
(@PATH,22,-3804.742,1833.069,104.3174,0,0,0,100,0),
(@PATH,23,-3771.966,1852.2,103.0675,0,0,0,100,0),
(@PATH,24,-3733.195,1857.556,99.8175,0,0,0,100,0),
(@PATH,25,-3700.392,1874.681,91.92853,0,0,0,100,0),
(@PATH,26,-3669.671,1899.899,86.984,0,0,0,100,0),
(@PATH,27,-3657.474,1933.845,83.78937,0,0,0,100,0),
(@PATH,28,-3665.441,1967.769,90.26167,0,0,0,100,0),
(@PATH,29,-3679.033,1980.171,95.40069,0,0,0,100,0);

SET @NPC := 84488;
SET @PATH := @NPC*10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-3642.398,`position_y`=2282.843,`position_z`=101.388 WHERE `guid`=@NPC;
DELETE FROM `creature` WHERE `guid` IN (84604);
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`bytes2`) VALUES (@NPC,@PATH,1);
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-3669.035,2270.016,101.6381,0,0,0,100,0),
(@PATH,2,-3678.18,2255.654,102.6103,0,0,0,100,0),
(@PATH,3,-3670.328,2221.352,101.9714,0,0,0,100,0),
(@PATH,4,-3643.312,2194.72,101.7492,0,0,0,100,0),
(@PATH,5,-3616.062,2171.167,100.7214,0,0,0,100,0),
(@PATH,6,-3592.527,2149.246,97.38798,0,0,0,100,0),
(@PATH,7,-3565.214,2111.288,91.72128,0,0,0,100,0),
(@PATH,8,-3539.711,2073.035,90.72137,0,0,0,100,0),
(@PATH,9,-3525.445,2044.541,86.11011,0,0,0,100,0),
(@PATH,10,-3525.761,2011.65,86.47135,0,0,0,100,0),
(@PATH,11,-3542.677,1995.357,88.47128,0,0,0,100,0),
(@PATH,12,-3561.439,2002.938,85.02679,0,0,0,100,0),
(@PATH,13,-3575.933,2019.746,82.88785,0,0,0,100,0),
(@PATH,14,-3575.169,2047.138,81.36017,0,0,0,100,0),
(@PATH,15,-3573.451,2072.323,84.91569,0,0,0,100,0),
(@PATH,16,-3564.493,2102.465,90.33241,0,0,0,100,0),
(@PATH,17,-3536.419,2134.583,97.94351,0,0,0,100,0),
(@PATH,18,-3500.938,2154.613,104.3325,0,0,0,100,0),
(@PATH,19,-3468.633,2165.143,101.9992,0,0,0,100,0),
(@PATH,20,-3451.676,2176.575,102.9714,0,0,0,100,0),
(@PATH,21,-3440.866,2214.563,102.4992,0,0,0,100,0),
(@PATH,22,-3464.083,2240.168,96.19363,0,0,0,100,0),
(@PATH,23,-3503.83,2238.953,96.88795,0,0,0,100,0),
(@PATH,24,-3532.543,2227.365,98.52686,0,0,0,100,0),
(@PATH,25,-3557.896,2223.087,99.74914,0,0,0,100,0),
(@PATH,26,-3585.39,2233.091,100.4992,0,0,0,100,0),
(@PATH,27,-3605.955,2248.527,99.88802,0,0,0,100,0),
(@PATH,28,-3615.812,2270.33,99.72134,0,0,0,100,0),
(@PATH,29,-3642.398,2282.843,101.388,0,0,0,100,0);

-- Pathing for Zorus the Judicator Entry: 21774 (Replace)
SET @NPC := 76076;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-3970.86,`position_y`=2191.12,`position_z`=101.885 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`bytes2`,`mount`,`auras`) VALUES (@NPC,@PATH,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-3959.922,2188.494,101.799,0,0,0,100,0),
(@PATH,2,-3951.734,2200.173,101.7906,0,0,0,100,0),
(@PATH,3,-3958.357,2210.038,101.9156,0,0,0,100,0),
(@PATH,4,-3967.201,2208.166,101.76,0,0,0,100,0),
(@PATH,5,-3972.306,2201.522,101.76,0,0,0,100,0),
(@PATH,6,-3992.194,2215.025,103.635,0,0,0,100,0),
(@PATH,7,-4014.109,2218.045,108.6923,0,0,0,100,0),
(@PATH,8,-4023.189,2214.694,109.9423,0,0,0,100,0),
(@PATH,9,-4019.969,2183.267,107.4989,0,0,0,100,0),
(@PATH,10,-4000.393,2174.054,104.3739,0,0,0,100,0),
(@PATH,11,-3970.86,2191.12,101.885,0,0,0,100,0);

-- Pathing for Wildhammer Guard Entry: 19353 (Replace)
SET @NPC := 69029;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-3956.661,`position_y`=2150.609,`position_z`=99.19981 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`bytes2`,`mount`,`auras`) VALUES (@NPC,@PATH,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-3952.747,2135.461,98.19981,0,0,0,100,0),
(@PATH,2,-3942.882,2121.993,96.84258,0,0,0,100,0),
(@PATH,3,-3936.123,2108.748,95.46758,0,0,0,100,0),
(@PATH,4,-3928.641,2095.985,94.73193,0,0,0,100,0),
(@PATH,5,-3916.894,2085.577,94.10693,0,0,0,100,0),
(@PATH,6,-3905.937,2073.934,94.48193,3000,0,0,100,0),
(@PATH,7,-3916.894,2085.577,94.10693,0,0,0,100,0),
(@PATH,8,-3928.641,2095.985,94.73193,0,0,0,100,0),
(@PATH,9,-3936.123,2108.748,95.46758,0,0,0,100,0),
(@PATH,10,-3942.882,2121.993,96.84258,0,0,0,100,0),
(@PATH,11,-3952.747,2135.461,98.19981,0,0,0,100,0),
(@PATH,12,-3956.661,2150.609,99.19981,0,0,0,100,0),
(@PATH,13,-3958.419,2169.508,100.424,0,0,0,100,0),
(@PATH,14,-3945.74,2173.963,101.174,0,0,0,100,0),
(@PATH,15,-3941.273,2175.507,101.549,3000,0,0,100,0),
(@PATH,16,-3945.74,2173.963,101.174,0,0,0,100,0),
(@PATH,17,-3958.419,2169.508,100.424,0,0,0,100,0),
(@PATH,18,-3956.661,2150.609,99.19981,0,0,0,100,0);

-- Pathing for Wildhammer Guard Entry: 19353 (Replace)
SET @NPC := 69030;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-3953.156,`position_y`=2196.489,`position_z`=101.799 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`bytes2`,`mount`,`auras`) VALUES (@NPC,@PATH,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-3956.213,2190.557,101.799,0,0,0,100,0),
(@PATH,2,-3961.978,2187.907,101.924,0,0,0,100,0),
(@PATH,3,-3970.904,2191.666,101.885,0,0,0,100,0),
(@PATH,4,-3977.542,2187.189,102.135,0,0,0,100,0),
(@PATH,5,-3984.635,2183.306,102.885,0,0,0,100,0),
(@PATH,6,-3994.975,2177.334,103.885,0,0,0,100,0),
(@PATH,7,-4002.002,2173.979,104.4989,0,0,0,100,0),
(@PATH,8,-4020.553,2182.329,107.4989,0,0,0,100,0),
(@PATH,9,-4022.445,2199.229,109.3739,0,0,0,100,0),
(@PATH,10,-4024.303,2213.243,110.0673,0,0,0,100,0),
(@PATH,11,-4019.943,2220.104,109.9423,0,0,0,100,0),
(@PATH,12,-4010.167,2219.025,107.8173,0,0,0,100,0),
(@PATH,13,-3997.926,2217.727,105.135,0,0,0,100,0),
(@PATH,14,-3987.671,2211.935,102.76,0,0,0,100,0),
(@PATH,15,-3976.702,2204.916,101.885,0,0,0,100,0),
(@PATH,16,-3971.531,2203.007,101.76,0,0,0,100,0),
(@PATH,17,-3967.494,2207.26,101.76,0,0,0,100,0),
(@PATH,18,-3961.75,2209.582,101.7906,0,0,0,100,0),
(@PATH,19,-3955.481,2208.423,101.9156,0,0,0,100,0),
(@PATH,20,-3952.822,2204.261,101.9156,0,0,0,100,0),
(@PATH,21,-3953.156,2196.489,101.799,0,0,0,100,0);

-- Pathing for Wildhammer Guard Entry: 19353
SET @NPC := 69036;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-4027.414,`position_y`=2111.922,`position_z`=137.2429 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`bytes2`,`mount`,`auras`) VALUES (@NPC,@PATH,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-4010.903,2103.363,137.2429,0,0,0,100,0),
(@PATH,2,-4027.414,2111.922,137.2429,0,0,0,100,0),
(@PATH,3,-4040.183,2116.031,137.2471,0,0,0,100,0),
(@PATH,4,-4051.71,2120.875,137.254,0,0,0,100,0),
(@PATH,5,-4064.835,2126.483,137.2603,0,0,0,100,0),
(@PATH,6,-4061.585,2129.041,137.2603,0,0,0,100,0),
(@PATH,7,-4053.318,2126.078,137.2559,0,0,0,100,0),
(@PATH,8,-4049.116,2130.594,137.254,0,0,0,100,0),
(@PATH,9,-4030.205,2123.756,127.3917,0,0,0,100,0),
(@PATH,10,-4013.132,2115.52,115.9905,0,0,0,100,0),
(@PATH,11,-4008.05,2114.437,115.6337,0,0,0,100,0),
(@PATH,12,-4005.22,2120.062,115.6409,0,0,0,100,0),
(@PATH,13,-4000.72,2130.597,107.8586,0,0,0,100,0),
(@PATH,14,-3993.985,2139.561,104.7343,0,0,0,100,0),
(@PATH,15,-3984.795,2135.855,104.9843,0,0,0,100,0),
(@PATH,16,-3982.541,2129.627,105.0458,0,0,0,100,0),
(@PATH,17,-3963.905,2117.34,100.7176,0,0,0,100,0),
(@PATH,18,-3954.1,2115.367,97.96758,0,0,0,100,0),
(@PATH,19,-3946.531,2111.822,96.84258,0,0,0,100,0),
(@PATH,20,-3943.734,2106.022,96.21758,0,0,0,100,0),
(@PATH,21,-3946.531,2111.822,96.84258,0,0,0,100,0),
(@PATH,22,-3953.81,2115.304,97.96758,0,0,0,100,0),
(@PATH,23,-3963.905,2117.34,100.7176,0,0,0,100,0),
(@PATH,24,-3982.541,2129.627,105.0458,0,0,0,100,0),
(@PATH,25,-3984.795,2135.855,104.9843,0,0,0,100,0),
(@PATH,26,-3993.985,2139.561,104.7343,0,0,0,100,0),
(@PATH,27,-4000.72,2130.597,107.8586,0,0,0,100,0),
(@PATH,28,-4005.202,2120.103,115.6362,0,0,0,100,0),
(@PATH,29,-4008.05,2114.437,115.6337,0,0,0,100,0),
(@PATH,30,-4013.132,2115.52,115.9905,0,0,0,100,0),
(@PATH,31,-4030.205,2123.756,127.3917,0,0,0,100,0),
(@PATH,32,-4049.116,2130.594,137.254,0,0,0,100,0),
(@PATH,33,-4053.318,2126.078,137.2559,0,0,0,100,0),
(@PATH,34,-4061.585,2129.041,137.2603,0,0,0,100,0),
(@PATH,35,-4064.835,2126.483,137.2603,0,0,0,100,0),
(@PATH,36,-4051.71,2120.875,137.254,0,0,0,100,0),
(@PATH,37,-4040.183,2116.031,137.2471,0,0,0,100,0),
(@PATH,38,-4027.414,2111.922,137.2429,0,0,0,100,0);

-- Pathing for Thane Yoregar Entry: 21773
SET @NPC := 76075;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-4034.194,`position_y`=2233.48,`position_z`=112.0453 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`bytes2`,`mount`,`auras`) VALUES (@NPC,@PATH,1,17718, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-4037.18,2229.153,112.0457,0,0,0,100,0),
(@PATH,2,-4043.923,2229.386,112.0464,0,0,0,100,0),
(@PATH,3,-4037.18,2229.153,112.0457,0,0,0,100,0),
(@PATH,4,-4034.194,2233.48,112.0453,0,0,0,100,0);

-- Pathing for Wildhammer Scout Entry: 19384
SET @NPC := 69128;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-4022.309,`position_y`=2134.639,`position_z`=104.274 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`bytes2`,`mount`,`auras`) VALUES (@NPC,@PATH,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-4021.637,2136.38,104.274,0,0,0,100,0),
(@PATH,2,-4020.335,2138.243,104.274,0,0,0,100,0),
(@PATH,3,-4018.53,2139.381,104.274,0,0,0,100,0),
(@PATH,4,-4016.425,2138.772,104.274,10000,0,0,100,0),
(@PATH,5,-4017.231,2139.141,104.274,0,0,0,100,0),
(@PATH,6,-4018.49,2140.534,104.274,0,0,0,100,0),
(@PATH,7,-4019.722,2143.937,104.274,10000,0,0,100,0),
(@PATH,8,-4019.373,2143.061,104.274,0,0,0,100,0),
(@PATH,9,-4019.427,2140.091,104.274,0,0,0,100,0),
(@PATH,10,-4020.97,2136.993,104.274,0,0,0,100,0),
(@PATH,11,-4022.309,2134.639,104.274,15000,0,0,100,0);

-- Spawn missing NPC Nakansi
DELETE FROM `creature` WHERE `id`=21789; -- Nakansi / Shadowmoon Valley
INSERT INTO creature (`guid`,`id`,`map`,`spawnMask`,`modelid`,`equipment_id`,`position_x`,`position_y`,`position_z`,`orientation`,`spawntimesecs`,`spawndist`,`currentwaypoint`,`curhealth`,`curmana`,`DeathState`,`MovementType`) VALUES
(152259,21789,530,1,1,0,-2741.8,2719.07,123.584,0.603481,600,0,0,34930,0,0,0);

-- Fixup Enslaved Netherwing drake in Shadowmoon Valley
UPDATE `creature` SET `spawndist`=0,`MovementType`=0 WHERE `id`=21722;
UPDATE `creature_template` SET `InhabitType`=1 WHERE `entry`=21722;

-- Fixup Dragonmaw Drake-Riders in Shadowmoon Valley
UPDATE `creature` SET `position_x`=-4155.54932,`position_y`=386.6152,`position_z`=141.4012,`orientation`=1.20427716 WHERE `guid`=75786;
UPDATE `creature` SET `spawndist`=0,`MovementType`=0 WHERE `id`=21719;
UPDATE `creature_template` SET `InhabitType`=1 WHERE `entry`=21719;
