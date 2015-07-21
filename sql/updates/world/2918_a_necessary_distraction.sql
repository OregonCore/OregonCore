UPDATE `creature_template` SET `unit_flags`=2147779136 WHERE  `entry`=21506;

DELETE FROM `creature_template_addon` WHERE `entry`=21506;
INSERT INTO `creature_template_addon` (`entry`, `mount`, `bytes1`, `bytes2`, `auras`) VALUES
(21506, 0, 0x0, 0x1, '37833 0 37833 1'); -- 21506 - 37833