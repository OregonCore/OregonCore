/*
MySQL Data Transfer
Source Host: 192.168.32.218
Source Database: world
Target Host: 192.168.32.218
Target Database: world
Date: 10.01.2010 20:33:20
*/

SET FOREIGN_KEY_CHECKS=0;
-- ----------------------------
-- Table structure for spell_disabled
-- ----------------------------
DROP TABLE IF EXISTS `spell_disabled`;
CREATE TABLE `spell_disabled` (
  `entry` int(11) unsigned NOT NULL default '0' COMMENT 'Spell entry',
  `disable_mask` int(8) unsigned NOT NULL default '0',
  `comment` varchar(64) NOT NULL default '',
  PRIMARY KEY  (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Disabled Spell System';

-- ----------------------------
-- Records 
-- ----------------------------
INSERT INTO `spell_disabled` VALUES ('44132', '1', 'Disable spell for players');