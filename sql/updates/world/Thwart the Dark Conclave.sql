--
-- Thwart the Dark Conclave - Fix/Add Object location
--

DELETE FROM `gameobject` WHERE `id`=300121;
INSERT INTO `gameobject` (`guid`, `id`, `map`, `position_x`, `position_y`, `position_z`, `spawntimesecs`, `state`) VALUES ('99983', '300121', '530', '-4151.34', '1984.90', '63.8472', '180', '1');
INSERT INTO `gameobject` (`id`, `map`, `position_x`, `position_y`, `position_z`, `spawntimesecs`, `state`) VALUES ('300121', '530', '-4180.69', '1963.65', '65.2836', '180', '1');
INSERT INTO `gameobject` (`id`, `map`, `position_x`, `position_y`, `position_z`, `spawntimesecs`, `state`) VALUES ('300121', '530', '-4143.99', '2033.40', '63.8768', '180', '1');