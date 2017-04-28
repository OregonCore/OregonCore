--
-- The battle of Darrowshire - move TriggerObject to proper location
--

DELETE FROM `gameobject` WHERE  `guid`=99891;
INSERT INTO `gameobject` (`guid`, `id`, `position_x`, `position_y`, `position_z`, `spawntimesecs`, `state`) VALUES ('99891', '300034', '1442.65', '-3699.72', '77.344002', '180', '1');