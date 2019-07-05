DROP TABLE IF EXISTS `version`;
CREATE TABLE `version` (
  `core_version` varchar(120) DEFAULT NULL COMMENT 'Core revision dumped at startup.',
  `core_revision` varchar(6) DEFAULT NULL,
  `db_version` varchar(120) DEFAULT NULL COMMENT 'Version of world DB.',
  `script_version` varchar(120) DEFAULT NULL COMMENT 'Version of scripts DB.'
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Version Notes';

UPDATE `item_template` SET `displayid`=30791 WHERE `entry`=18419;
UPDATE `item_template` SET `displayid`=7430,`InventoryType`=13,`sheath`=7 WHERE `entry`=3494;
UPDATE `creature_template` SET `ScriptName`='npc_celebras' WHERE `entry`=13716;
UPDATE `creature_template` SET `ScriptName`='boss_kelidan_the_breaker' WHERE `entry`=17377;