DELETE FROM `oregon_string` WHERE `entry` BETWEEN 737 AND 738;
INSERT INTO `oregon_string` (`entry`,`content_default`,`content_loc1`,`content_loc2`,`content_loc3`,`content_loc4`,`content_loc5`,`content_loc6`,`content_loc7`,`content_loc8`) VALUES
(737,'This Battleground has been disabled. You can''t join the queue.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(738,'Arenas have been disabled. You can''t join the queue.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);

DELETE FROM `command` WHERE `name` IN ('reload disables','reload spell_disabled');
INSERT INTO `command` (`name`,`security`,`help`) VALUES
('reload disables',3,'Syntax: .reload disables\r\nReload disables table.');

-- Create the disable table
DROP TABLE IF EXISTS `disables`;
CREATE TABLE `disables` (
  `sourceType` INT(10) UNSIGNED NOT NULL,
  `entry` INT(10) UNSIGNED NOT NULL,
  `flags` TINYINT(3) UNSIGNED NOT NULL DEFAULT '0',
  `params_0` VARCHAR(255) NOT NULL DEFAULT '',
  `params_1` VARCHAR(255) NOT NULL DEFAULT '',
  `comment` VARCHAR(255) CHARACTER SET utf8 NOT NULL DEFAULT '',
  PRIMARY KEY  (`sourceType`,`entry`)
) ENGINE=MYISAM DEFAULT CHARSET=latin1;

-- Drop old table
DROP TABLE IF EXISTS `spell_disabled`;