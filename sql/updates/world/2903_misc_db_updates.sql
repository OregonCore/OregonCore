-- Fix Gravis Slipknots model
UPDATE creature_template SET modelid_A2 = 2582, modelid_H2 = 2582 WHERE entry = 14221;

-- Spawn Fel Crystalforges for quest Our Boy Wants to be a Skyguard Ranger
INSERT INTO `gameobject` (`guid`, `id`, `map`, `spawnMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`) VALUES 
('150112','185919','530','1','1474.1','7256.4','375.19','2.66241','0','0','0.971435','0.237307','0','0','1'),
('150113','185919','530','1','2948.5','7076','370.826','1.94403','0','0','0.826024','0.563636','0','0','1');