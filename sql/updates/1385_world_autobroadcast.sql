DROP TABLE IF EXISTS `autobroadcast`;

--
-- Table structure for table `autobroadcast`
--

CREATE TABLE `autobroadcast` (
`id` int(11) NOT NULL AUTO_INCREMENT,
`text` longtext NOT NULL,
PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 AUTO_INCREMENT=1;

--
-- Data for table `autobroadcast`
--

DELETE FROM `oregon_string` WHERE `entry` = 11000;
INSERT INTO `oregon_string` (entry, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8)
VALUES (11000, '|cffffff00[|c00077766Autobroadcast|cffffff00]: |cFFF222FF%s|r', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

DELETE FROM `command` WHERE `name` = 'reload autobroadcast';
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('reload autobroadcast', 3, 'Syntax: .reload autobroadcast\nReload autobroadcast table.');
