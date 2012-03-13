DROP TABLE IF EXISTS `creature_groups`;
CREATE TABLE `creature_groups` (
  `groupId` int(10) unsigned NOT NULL auto_increment,
  `leaderGUID` int(10) unsigned NOT NULL,
  `groupType` int(11) default NULL,
  `comment` varchar(255) default NULL,
  PRIMARY KEY  (`groupId`,`leaderGUID`)
) ENGINE=MyISAM AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;