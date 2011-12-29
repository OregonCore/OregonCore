DROP TABLE IF EXISTS `creature_group_data`;
CREATE TABLE `creature_group_data` (
  `groupId` int(10) unsigned NOT NULL,
  `memberGUID` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`memberGUID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;