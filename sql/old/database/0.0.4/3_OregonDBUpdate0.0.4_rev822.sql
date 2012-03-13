-- Structure Changes
DROP TABLE IF EXISTS `quest_mail_loot_template`;

ALTER IGNORE TABLE quest_template
  CHANGE COLUMN `QuestLevel` `QuestLevel` smallint(6) NOT NULL default '1';
  
DROP TABLE IF EXISTS `mail_loot_template`;

CREATE TABLE `mail_loot_template` (
  `entry` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `item` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `ChanceOrQuestChance` float NOT NULL DEFAULT '100',
  `groupid` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `mincountOrRef` mediumint(9) NOT NULL DEFAULT '1',
  `maxcount` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `lootcondition` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `condition_value1` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `condition_value2` mediumint(8) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`entry`,`item`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Loot System';

/*Data for the table `mail_loot_template` */
insert  into `mail_loot_template`(`entry`,`item`,`ChanceOrQuestChance`,`groupid`,`mincountOrRef`,`maxcount`,`lootcondition`,`condition_value1`,`condition_value2`) values (87,6529,100,0,1,1,0,0,0),(119,15564,100,0,1,1,0,0,0),(120,15564,100,0,1,1,0,0,0),(180,24132,100,0,1,1,0,0,0),(103,11422,100,0,1,1,0,0,0),(99,11423,100,0,1,1,0,0,0),(104,11422,100,0,1,1,0,0,0),(100,11423,100,0,1,1,0,0,0),(92,20469,100,0,1,1,0,0,0),(93,20469,100,0,1,1,0,0,0),(98,13158,100,0,1,1,0,0,0),(109,19858,100,0,1,1,0,0,0),(113,19697,100,0,1,1,0,0,0),(111,19697,100,0,1,1,0,0,0),(183,31698,100,0,1,1,0,0,0),(84,21746,100,0,1,1,0,0,0),(85,21746,100,0,1,1,0,0,0),(86,21746,100,0,1,1,0,0,0),(88,21746,100,0,1,1,0,0,0),(89,21746,100,0,1,1,0,0,0),(90,21746,100,0,1,1,0,0,0),(91,21746,100,0,1,1,0,0,0),(92,21746,100,0,1,1,0,0,0),(95,21746,100,0,1,1,0,0,0),(96,21746,100,0,1,1,0,0,0),(97,21746,100,0,1,1,0,0,0),(121,21746,100,0,1,1,0,0,0),(124,21746,100,0,1,1,0,0,0),(125,21746,100,0,1,1,0,0,0),(126,21746,100,0,1,1,0,0,0),(127,21746,100,0,1,1,0,0,0),(128,21746,100,0,1,1,0,0,0),(129,21746,100,0,1,1,0,0,0),(130,21746,100,0,1,1,0,0,0),(131,21746,100,0,1,1,0,0,0),(132,21746,100,0,1,1,0,0,0),(133,21746,100,0,1,1,0,0,0),(134,21746,100,0,1,1,0,0,0),(135,21746,100,0,1,1,0,0,0),(136,21746,100,0,1,1,0,0,0),(137,21746,100,0,1,1,0,0,0),(138,21746,100,0,1,1,0,0,0),(139,21746,100,0,1,1,0,0,0),(140,21746,100,0,1,1,0,0,0),(141,21746,100,0,1,1,0,0,0),(142,21746,100,0,1,1,0,0,0),(143,21746,100,0,1,1,0,0,0),(144,21746,100,0,1,1,0,0,0),(145,21746,100,0,1,1,0,0,0),(146,21746,100,0,1,1,0,0,0),(147,21746,100,0,1,1,0,0,0),(148,21746,100,0,1,1,0,0,0),(149,21746,100,0,1,1,0,0,0),(150,21746,100,0,1,1,0,0,0),(151,21746,100,0,1,1,0,0,0),(152,21746,100,0,1,1,0,0,0),(153,21746,100,0,1,1,0,0,0),(154,21746,100,0,1,1,0,0,0),(155,21746,100,0,1,1,0,0,0),(156,21746,100,0,1,1,0,0,0),(157,21746,100,0,1,1,0,0,0),(158,21746,100,0,1,1,0,0,0),(159,21746,100,0,1,1,0,0,0),(160,21746,100,0,1,1,0,0,0),(168,21746,100,0,1,1,0,0,0);


REPLACE INTO `command` (name, help, security)
VALUES ('account lock', 'Syntax: .account lock [on|off]\r\n\r\nAllow login from account only from current used IP or remove this requirement.', 0);
REPLACE INTO `command` (name, help, security)
VALUES ('account password', 'Syntax: .account password $old_password $new_password $new_password\r\n\r\nChange your account password.', 0);
REPLACE INTO `command` (name, help, security)
VALUES ('character delete', 'Syntax: .character delete $name\r\n\r\nDelete character $name.', 4);
REPLACE INTO `command` (name, help, security)
VALUES ('character rename', 'Syntax: .character rename [$name]\r\n\r\nMark selected in game or by $name in command character for rename at next login.', 2);
REPLACE INTO `command` (name, help, security)
VALUES ('character titles', 'Syntax: .character titles [$player_name]\r\n\r\nShow known titles list for selected player or player find by $player_name.', 2);
REPLACE INTO `command` (name, help, security)
VALUES ('lookup title', 'Syntax: .lookup title $$namepart\r\n\r\nLooks up a title by $namepart, and returns all matches with their title ID''s and index''s.', 2);
REPLACE INTO `command` (name, help, security)
VALUES ('npc setdeathstate', 'Syntax: .npc setdeathstate on/off\r\n\r\nSet default death state (dead/alive) for npc at spawn.', 2);
REPLACE INTO `command` (name, help, security)
VALUES ('server plimit', 'Syntax: .server plimit [#num|-1|-2|-3|reset|player|moderator|gamemaster|administrator]\r\n\r\nWithout arg show current player amount and security level limitations for login to server, with arg set player linit ($num > 0) or securiti limitation ($num < 0 or security leme name. With `reset` sets player limit to the one in the config file', 3);
REPLACE INTO `command` (name, help, security)
VALUES ('titles add', 'Syntax: .titles add #title\r\nAdd title #title (id or shift-link) to known titles list for selected player.', 2);
REPLACE INTO `command` (name, help, security)
VALUES ('titles current', 'Syntax: .titles current #title\r\nSet title #title (id or shift-link) as current selected title for selected player. If title not in known title list for player then it will be added to list.', 2);
REPLACE INTO `command` (name, help, security)
VALUES ('titles remove', 'Syntax: .titles remove #title\r\nRemove title #title (id or shift-link) from known titles list for selected player.', 2);
REPLACE INTO `command` (name, help, security)
VALUES ('titles setmask', 'Syntax: .titles setmask #mask\r\n\r\nAllows user to use all titles from #mask.\r\n\r\n #mask=0 disables the title-choose-field', 2);

UPDATE `command` SET help = 'Syntax: .modify scale #scale\r\n\r\nChange model scale for targeted player(until re-login), or creature(until respawn).' WHERE name = 'modify scale';

DELETE FROM `command` WHERE name = 'chardelete';
DELETE FROM `command` WHERE name = 'lockaccount';
DELETE FROM `command` WHERE name = 'modify titles';
DELETE FROM `command` WHERE name = 'password';
DELETE FROM `command` WHERE name = 'plimit';

UPDATE `creature` SET position_z = -16.5406 WHERE guid = 6090;
UPDATE `creature` SET position_z = -16.52 WHERE guid = 7727;
UPDATE `creature` SET position_z = -16.5431 WHERE guid = 9411;
UPDATE `creature` SET position_z = -16.5221 WHERE guid = 26009;

DELETE FROM `creature` WHERE guid = 6511;
DELETE FROM `creature` WHERE guid = 7370;
DELETE FROM `creature` WHERE guid = 9555;
DELETE FROM `creature` WHERE guid = 12369;

REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (0, NULL, 0, 0, 0, 0, 0, 0, 1);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (11, NULL, 0, 0, 0, 0, 0, 0, 105);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (12, NULL, 0, 0, 0, 0, 0, 0, 115);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (13, NULL, 0, 0, 0, 0, 0, 0, 125);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (14, NULL, 0, 0, 0, 0, 0, 0, 135);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (15, NULL, 0, 0, 0, 0, 0, 0, 145);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (16, NULL, 0, 0, 0, 0, 0, 0, 155);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (17, NULL, 0, 0, 0, 0, 0, 0, 165);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (18, NULL, 0, 0, 0, 0, 0, 0, 175);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (19, NULL, 0, 0, 0, 0, 0, 0, 185);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (20, NULL, 0, 0, 0, 0, 0, 0, 195);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (21, NULL, 0, 0, 0, 0, 0, 0, 205);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (22, NULL, 0, 0, 0, 0, 0, 0, 215);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (23, NULL, 0, 0, 0, 0, 0, 0, 225);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (24, NULL, 0, 0, 0, 0, 0, 0, 235);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (25, NULL, 0, 0, 0, 0, 0, 0, 245);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (26, NULL, 0, 0, 0, 0, 0, 0, 255);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (27, NULL, 0, 0, 0, 0, 0, 0, 265);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (28, NULL, 0, 0, 0, 0, 0, 0, 275);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (29, NULL, 0, 0, 0, 0, 0, 0, 285);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (30, NULL, 0, 0, 0, 0, 0, 0, 295);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (31, NULL, 0, 0, 0, 0, 0, 0, 305);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (32, NULL, 0, 0, 0, 0, 0, 0, 315);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (33, NULL, 0, 0, 0, 0, 0, 0, 325);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (34, NULL, 0, 0, 0, 0, 0, 0, 335);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (35, NULL, 0, 0, 0, 0, 0, 0, 345);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (36, NULL, 0, 0, 0, 0, 0, 0, 355);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (37, NULL, 0, 0, 0, 0, 0, 0, 365);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (38, NULL, 0, 0, 0, 0, 0, 0, 375);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (39, NULL, 0, 0, 0, 0, 0, 0, 385);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (40, NULL, 0, 0, 0, 0, 0, 0, 395);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (41, NULL, 0, 0, 0, 0, 0, 0, 405);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (42, NULL, 0, 0, 0, 0, 0, 0, 415);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (43, NULL, 0, 0, 0, 0, 0, 0, 425);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (44, NULL, 0, 0, 0, 0, 0, 0, 435);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (45, NULL, 0, 0, 0, 0, 0, 0, 445);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (48, NULL, 0, 0, 0, 0, 0, 0, 475);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (49, NULL, 0, 0, 0, 0, 0, 0, 485);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (50, NULL, 0, 0, 0, 0, 0, 0, 495);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (52, NULL, 0, 0, 0, 0, 0, 0, 515);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (53, NULL, 0, 0, 0, 0, 0, 0, 525);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (54, NULL, 0, 0, 0, 0, 0, 0, 535);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (55, NULL, 0, 0, 0, 0, 0, 0, 545);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (56, NULL, 0, 0, 0, 0, 0, 0, 555);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (59, NULL, 0, 0, 0, 0, 0, 0, 585);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (60, NULL, 0, 0, 0, 0, 0, 0, 595);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (61, NULL, 0, 0, 0, 0, 0, 0, 605);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (62, NULL, 0, 0, 0, 0, 0, 0, 615);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (63, NULL, 0, 0, 0, 0, 0, 0, 625);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (78, NULL, 0, 0, 0, 0, 0, 0, 775);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (79, NULL, 0, 0, 0, 0, 0, 0, 785);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (80, NULL, 0, 0, 0, 0, 0, 0, 795);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (81, NULL, 0, 0, 0, 0, 0, 0, 805);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (82, NULL, 0, 0, 0, 0, 0, 0, 815);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (83, NULL, 0, 0, 0, 0, 0, 0, 825);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (84, NULL, 0, 0, 0, 0, 0, 0, 835);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (85, NULL, 0, 0, 0, 0, 0, 0, 845);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (86, NULL, 0, 0, 0, 0, 0, 0, 855);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (87, NULL, 0, 0, 0, 0, 0, 0, 865);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (89, NULL, 0, 0, 0, 0, 0, 0, 885);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (90, NULL, 0, 0, 0, 0, 0, 0, 895);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (92, NULL, 0, 0, 0, 0, 0, 0, 915);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (93, NULL, 0, 0, 0, 0, 0, 0, 925);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (94, NULL, 0, 0, 0, 0, 0, 0, 935);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (95, NULL, 0, 0, 0, 0, 0, 0, 945);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (96, NULL, 0, 0, 0, 0, 0, 0, 955);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (98, NULL, 0, 0, 0, 0, 0, 0, 975);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (99, NULL, 0, 0, 0, 0, 0, 0, 985);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (100, NULL, 0, 0, 0, 0, 0, 0, 995);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (102, NULL, 0, 0, 0, 0, 0, 0, 1015);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (103, NULL, 0, 0, 0, 0, 0, 0, 1025);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (104, NULL, 0, 0, 0, 0, 0, 0, 1035);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (105, NULL, 0, 0, 0, 0, 0, 0, 1045);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (106, NULL, 0, 0, 0, 0, 0, 0, 1055);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (107, NULL, 0, 0, 0, 0, 0, 0, 1065);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (110, NULL, 0, 0, 0, 0, 0, 0, 1095);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (111, NULL, 0, 0, 0, 0, 0, 0, 1105);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (112, NULL, 0, 0, 0, 0, 0, 0, 1115);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (113, NULL, 0, 0, 0, 0, 0, 0, 1125);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (114, NULL, 0, 0, 0, 0, 0, 0, 1135);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (115, NULL, 0, 0, 0, 0, 0, 0, 1145);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (116, NULL, 0, 0, 0, 0, 0, 0, 1155);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (118, NULL, 0, 0, 0, 0, 0, 0, 1175);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (119, NULL, 0, 0, 0, 0, 0, 0, 1185);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (120, NULL, 0, 0, 0, 0, 0, 0, 1195);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (121, NULL, 0, 0, 0, 0, 0, 0, 1205);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (126, NULL, 0, 0, 0, 0, 0, 0, 1255);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (127, NULL, 0, 0, 0, 0, 0, 0, 1265);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (128, NULL, 0, 0, 0, 0, 0, 0, 1275);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (129, NULL, 0, 0, 0, 0, 0, 0, 1285);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (130, NULL, 0, 0, 0, 0, 0, 0, 1295);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (131, NULL, 0, 0, 0, 0, 0, 0, 1305);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (132, NULL, 0, 0, 0, 0, 0, 0, 1315);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (137, NULL, 0, 0, 0, 0, 0, 0, 1365);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (143, NULL, 0, 0, 0, 0, 0, 0, 1425);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (147, NULL, 0, 0, 0, 0, 0, 0, 1465);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (148, NULL, 0, 0, 0, 0, 0, 0, 1475);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (149, NULL, 0, 0, 0, 0, 0, 0, 1485);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (151, NULL, 0, 0, 0, 0, 0, 0, 1505);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (153, NULL, 0, 0, 0, 0, 0, 0, 1525);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (154, NULL, 0, 0, 0, 0, 0, 0, 1535);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (155, NULL, 0, 0, 0, 0, 0, 0, 1545);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (156, NULL, 0, 0, 0, 0, 0, 0, 1555);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (157, NULL, 0, 0, 0, 0, 0, 0, 1565);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (159, NULL, 0, 0, 0, 0, 0, 0, 1585);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (166, NULL, 0, 0, 0, 0, 0, 0, 1655);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (167, NULL, 0, 0, 0, 0, 0, 0, 1665);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (168, NULL, 0, 0, 0, 0, 0, 0, 1675);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (169, NULL, 0, 0, 0, 0, 0, 0, 1685);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (171, NULL, 0, 0, 0, 0, 0, 0, 1705);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (172, NULL, 0, 0, 0, 0, 0, 0, 1715);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (173, NULL, 0, 0, 0, 0, 0, 0, 1725);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (174, NULL, 0, 0, 0, 0, 0, 0, 1735);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (175, NULL, 0, 0, 0, 0, 0, 0, 1745);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (176, NULL, 0, 0, 0, 0, 0, 0, 1755);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (177, NULL, 0, 0, 0, 0, 0, 0, 1765);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (179, NULL, 0, 0, 0, 0, 0, 0, 1785);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (180, NULL, 0, 0, 0, 0, 0, 0, 1795);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (181, NULL, 0, 0, 0, 0, 0, 0, 1805);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (183, NULL, 0, 0, 0, 0, 0, 0, 1825);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (186, NULL, 0, 0, 0, 0, 0, 0, 1855);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (188, NULL, 0, 0, 0, 0, 0, 0, 1875);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (189, NULL, 0, 0, 0, 0, 0, 0, 1885);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (190, NULL, 0, 0, 0, 0, 0, 0, 1895);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (191, NULL, 0, 0, 0, 0, 0, 0, 1905);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (192, NULL, 0, 0, 0, 0, 0, 0, 1915);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (193, NULL, 0, 0, 0, 0, 0, 0, 1925);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (194, NULL, 0, 0, 0, 0, 0, 0, 1935);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (195, NULL, 0, 0, 0, 0, 0, 0, 1945);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (196, NULL, 0, 0, 0, 0, 0, 0, 1955);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (197, NULL, 0, 0, 0, 0, 0, 0, 1965);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (198, NULL, 0, 0, 0, 0, 0, 0, 1975);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (199, NULL, 0, 0, 0, 0, 0, 0, 1985);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (200, NULL, 0, 0, 0, 0, 0, 0, 1995);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (201, NULL, 0, 0, 0, 0, 0, 0, 2005);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (202, NULL, 0, 0, 0, 0, 0, 0, 2015);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (203, NULL, 0, 0, 0, 0, 0, 0, 2025);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (204, NULL, 0, 0, 0, 0, 0, 0, 2035);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (205, NULL, 0, 0, 0, 0, 0, 0, 2045);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (206, NULL, 0, 0, 0, 0, 0, 0, 2055);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (207, NULL, 0, 0, 0, 0, 0, 0, 2065);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (208, NULL, 0, 0, 0, 0, 0, 0, 2075);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, mount, moveflags, path_id)
VALUES (209, NULL, 0, 0, 0, 0, 0, 0, 2090);

UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 5 WHERE guid = 1;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 15 WHERE guid = 2;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 25 WHERE guid = 3;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 35 WHERE guid = 4;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 45 WHERE guid = 5;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 55 WHERE guid = 6;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 65 WHERE guid = 7;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 75 WHERE guid = 8;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 85 WHERE guid = 9;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 95 WHERE guid = 10;
UPDATE `creature_addon` SET auras = '', bytes0 = 0, bytes2 = 0, path_id = 455 WHERE guid = 46;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 465 WHERE guid = 47;
UPDATE `creature_addon` SET path_id = 505 WHERE guid = 51;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 565 WHERE guid = 57;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 575 WHERE guid = 58;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 635 WHERE guid = 64;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 645 WHERE guid = 65;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 655 WHERE guid = 66;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 665 WHERE guid = 67;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 675 WHERE guid = 68;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 685 WHERE guid = 69;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 695 WHERE guid = 70;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 705 WHERE guid = 71;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 715 WHERE guid = 72;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 725 WHERE guid = 73;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 735 WHERE guid = 74;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 745 WHERE guid = 75;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 755 WHERE guid = 76;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 765 WHERE guid = 77;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 875 WHERE guid = 88;
UPDATE `creature_addon` SET path_id = 905 WHERE guid = 91;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 965 WHERE guid = 97;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 1005 WHERE guid = 101;
UPDATE `creature_addon` SET path_id = 1075 WHERE guid = 108;
UPDATE `creature_addon` SET path_id = 1085 WHERE guid = 109;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 1165 WHERE guid = 117;
UPDATE `creature_addon` SET path_id = 1215 WHERE guid = 122;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 1225 WHERE guid = 123;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 1235 WHERE guid = 124;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 1245 WHERE guid = 125;
UPDATE `creature_addon` SET auras = '', bytes0 = 0, bytes2 = 0, path_id = 1325 WHERE guid = 133;
UPDATE `creature_addon` SET path_id = 1335 WHERE guid = 134;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 1345 WHERE guid = 135;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 1355 WHERE guid = 136;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 1375 WHERE guid = 138;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 1385 WHERE guid = 139;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 1395 WHERE guid = 140;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 1405 WHERE guid = 141;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 1415 WHERE guid = 142;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 1435 WHERE guid = 144;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 1445 WHERE guid = 145;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 1455 WHERE guid = 146;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 1495 WHERE guid = 150;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 1515 WHERE guid = 152;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 1575 WHERE guid = 158;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 1595 WHERE guid = 160;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 1605 WHERE guid = 161;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 1615 WHERE guid = 162;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 1625 WHERE guid = 163;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 1635 WHERE guid = 164;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 1645 WHERE guid = 165;
UPDATE `creature_addon` SET auras = '', path_id = 1695 WHERE guid = 170;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 1775 WHERE guid = 178;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 1815 WHERE guid = 182;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 1835 WHERE guid = 184;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 1845 WHERE guid = 185;
UPDATE `creature_addon` SET path_id = 1865 WHERE guid = 187;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, path_id = 6401 WHERE guid = 640;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0 WHERE guid = 55176;

UPDATE `creature_addon` SET auras = NULL WHERE guid = 46;
UPDATE `creature_addon` SET auras = NULL WHERE guid = 133;
UPDATE `creature_addon` SET auras = NULL WHERE guid = 170;

DELETE FROM `creature_addon` WHERE guid = 6511;
DELETE FROM `creature_addon` WHERE guid = 7370;
DELETE FROM `creature_addon` WHERE guid = 9555;
DELETE FROM `creature_addon` WHERE guid = 12369;

UPDATE `creature_template` SET ScriptName = 'npc_innkeeper' WHERE entry = 295;
UPDATE `creature_template` SET AIName = 'EventAI' WHERE entry = 466;
UPDATE `creature_template` SET ScriptName = 'npc_miran' WHERE entry = 1379;
UPDATE `creature_template` SET ScriptName = 'npc_mist' WHERE entry = 3568;
UPDATE `creature_template` SET ScriptName = 'npc_tapoke_slim_jahn' WHERE entry = 4962;
UPDATE `creature_template` SET ScriptName = 'npc_mikhail' WHERE entry = 4963;
UPDATE `creature_template` SET ScriptName = 'npc_tooga' WHERE entry = 5955;
UPDATE `creature_template` SET ScriptName = 'mob_archaedas_minions' WHERE entry = 7077;
UPDATE `creature_template` SET ScriptName = 'mob_archaedas_minions' WHERE entry = 7309;
UPDATE `creature_template` SET ScriptName = 'npc_ringo' WHERE entry = 9999;
UPDATE `creature_template` SET ScriptName = 'mob_archaedas_minions' WHERE entry = 10120;
UPDATE `creature_template` SET ScriptName = 'npc_kanati' WHERE entry = 10638;
UPDATE `creature_template` SET ScriptName = 'npc_lakota_windsong' WHERE entry = 10646;
UPDATE `creature_template` SET ScriptName = 'npc_enraged_panther' WHERE entry = 10992;
UPDATE `creature_template` SET ScriptName = 'boss_drekthar' WHERE entry = 11946;
UPDATE `creature_template` SET ScriptName = 'boss_galvangar' WHERE entry = 11947;
UPDATE `creature_template` SET ScriptName = 'boss_vanndar' WHERE entry = 11948;
UPDATE `creature_template` SET ScriptName = 'boss_balinda' WHERE entry = 11949;
UPDATE `creature_template` SET ScriptName = 'AV_Marshals' WHERE entry = 14762;
UPDATE `creature_template` SET ScriptName = 'AV_Marshals' WHERE entry = 14763;
UPDATE `creature_template` SET ScriptName = 'AV_Marshals' WHERE entry = 14764;
UPDATE `creature_template` SET ScriptName = 'AV_Marshals' WHERE entry = 14765;
UPDATE `creature_template` SET ScriptName = 'AV_Warmasters' WHERE entry = 14772;
UPDATE `creature_template` SET ScriptName = 'AV_Warmasters' WHERE entry = 14773;
UPDATE `creature_template` SET ScriptName = 'AV_Warmasters' WHERE entry = 14776;
UPDATE `creature_template` SET ScriptName = 'AV_Warmasters' WHERE entry = 14777;
UPDATE `creature_template` SET ScriptName = 'mobs_mana_tapped' WHERE entry = 15273;
UPDATE `creature_template` SET ScriptName = 'mobs_mana_tapped' WHERE entry = 15274;
UPDATE `creature_template` SET ScriptName = 'mobs_mana_tapped' WHERE entry = 15294;
UPDATE `creature_template` SET ScriptName = 'mobs_mana_tapped' WHERE entry = 15298;
UPDATE `creature_template` SET ScriptName = 'mobs_mana_tapped' WHERE entry = 15367;
UPDATE `creature_template` SET ScriptName = 'npc_taxi' WHERE entry = 17209;
UPDATE `creature_template` SET ScriptName = 'mob_underbog_mushroom' WHERE entry = 17990;
UPDATE `creature_template` SET ScriptName = 'mob_phase_hunter' WHERE entry = 18879;
UPDATE `creature_template` SET ScriptName = 'mob_illidari_spawn' WHERE entry = 19797;
UPDATE `creature_template` SET ScriptName = 'npc_enraged_spirit' WHERE entry = 21050;
UPDATE `creature_template` SET ScriptName = '' WHERE entry = 21899;
UPDATE `creature_template` SET ScriptName = 'mob_illidari_spawn' WHERE entry = 22074;
UPDATE `creature_template` SET ScriptName = 'mob_illidari_spawn' WHERE entry = 22075;
UPDATE `creature_template` SET flags_extra = 128, ScriptName = '' WHERE entry = 23085;
UPDATE `creature_template` SET ScriptName = 'npc_taxi' WHERE entry = 23704;
UPDATE `creature_template` SET ScriptName = 'mob_archaedas_minions' WHERE entry = 7076;
UPDATE `creature_template` SET ScriptName = 'npc_susurrus' WHERE entry = 17435;
UPDATE `creature_template` SET ScriptName = 'npc_brazen' WHERE entry = 18725;
UPDATE `creature_template` SET ScriptName = 'npc_veronia' WHERE entry = 20162;
UPDATE `creature_template` SET ScriptName = 'npc_gryphoneer_windbellow' WHERE entry = 20235;
UPDATE `creature_template` SET ScriptName = 'npc_protectorate_nether_drake' WHERE entry = 20903;
UPDATE `creature_template` SET ScriptName = 'npc_skyguard_handler_irena' WHERE entry = 23413;
UPDATE `creature_template` SET ScriptName = 'npc_skyguard_handler_deesak' WHERE entry = 23415;
UPDATE `creature_template` SET ScriptName = 'npc_ayren_cloudbreaker' WHERE entry = 25059;
UPDATE `creature_template` SET ScriptName = 'npc_unrestrained_dragonhawk' WHERE entry = 25236;


UPDATE `game_event` SET start_time = '2007-09-20 00:00:00' WHERE entry = 26;

UPDATE `gameobject_template` SET ScriptName = 'go_panther_cage' WHERE entry = 176195;
UPDATE `gameobject_template` SET ScriptName = 'go_iruxos' WHERE entry = 176581;

REPLACE INTO `oregon_string` (entry, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8)
VALUES (173, 'Liquid level: %f, ground: %f, type: %d, status: %d', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
REPLACE INTO `oregon_string` (entry, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8)
VALUES (349, '%d (idx:%d) - |cffffffff|Htitle:%d|h[%s %s]|h|r %s %s ', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
REPLACE INTO `oregon_string` (entry, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8)
VALUES (350, '%d (idx:%d) - [%s %s] %s %s ', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
REPLACE INTO `oregon_string` (entry, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8)
VALUES (351, 'No titles found!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
REPLACE INTO `oregon_string` (entry, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8)
VALUES (352, 'Invalid title id: %u', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
REPLACE INTO `oregon_string` (entry, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8)
VALUES (353, 'Title %u (%s) added to known titles list for player %s.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
REPLACE INTO `oregon_string` (entry, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8)
VALUES (354, 'Title %u (%s) removed from known titles list for player %s.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
REPLACE INTO `oregon_string` (entry, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8)
VALUES (355, 'Title %u (%s) set as current selected title for player %s.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
REPLACE INTO `oregon_string` (entry, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8)
VALUES (356, 'Current selected title for player %s reset as not known now.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
REPLACE INTO `oregon_string` (entry, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8)
VALUES (812, 'Guild Master', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
REPLACE INTO `oregon_string` (entry, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8)
VALUES (813, 'Officer', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
REPLACE INTO `oregon_string` (entry, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8)
VALUES (814, 'Veteran', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
REPLACE INTO `oregon_string` (entry, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8)
VALUES (815, 'Member', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
REPLACE INTO `oregon_string` (entry, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8)
VALUES (816, 'Initiate', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
REPLACE INTO `oregon_string` (entry, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8)
VALUES (6616, 'You are silenced and can only speak with a GM.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
REPLACE INTO `oregon_string` (entry, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8)
VALUES (11000, '|cffffcc00[Autobroadcast]: |cff00ff00%s|r', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

UPDATE `oregon_string` SET content_default = 'Your chat has been disabled for %u minutes. Reason: %s. Disabled by %s.' WHERE entry = 300;
UPDATE `oregon_string` SET content_default = '%s does not wish to be disturbed: %s' WHERE entry = 707;
UPDATE `oregon_string` SET content_default = 'Queue status for %s (Lvl: %u to %u)\nQueued alliances: %u (Need at least %u more)\nQueued hordes: %u (Need at least %u more)' WHERE entry = 711;
UPDATE `oregon_string` SET content_default = '|cffff0000[Queue status for %s]|r [%u-%u] Alliance: %u/%u Horde: %u/%u|r' WHERE entry = 712;

UPDATE `playercreateinfo_spell_custom` SET Note = 'One-Handed Swords' WHERE class = 1 AND race = 1 AND Spell = 201;
UPDATE `playercreateinfo_spell_custom` SET Note = 'Two-Handed Axes' WHERE class = 1 AND race = 2 AND Spell = 197;
UPDATE `playercreateinfo_spell_custom` SET Note = 'One-Handed Swords' WHERE class = 1 AND race = 2 AND Spell = 201;
UPDATE `playercreateinfo_spell_custom` SET Note = 'Two-Handed Axes' WHERE class = 1 AND race = 3 AND Spell = 197;
UPDATE `playercreateinfo_spell_custom` SET Note = 'One-Handed Swords' WHERE class = 1 AND race = 4 AND Spell = 201;
UPDATE `playercreateinfo_spell_custom` SET Note = 'Daggers' WHERE class = 1 AND race = 4 AND Spell = 1180;
UPDATE `playercreateinfo_spell_custom` SET Note = 'One-Handed Swords' WHERE class = 1 AND race = 5 AND Spell = 201;
UPDATE `playercreateinfo_spell_custom` SET Note = 'Two-Handed Swords' WHERE class = 1 AND race = 5 AND Spell = 202;
UPDATE `playercreateinfo_spell_custom` SET Note = 'Daggers' WHERE class = 1 AND race = 5 AND Spell = 1180;
UPDATE `playercreateinfo_spell_custom` SET Note = 'One-Handed Swords' WHERE class = 1 AND race = 7 AND Spell = 201;
UPDATE `playercreateinfo_spell_custom` SET Note = 'Daggers' WHERE class = 1 AND race = 7 AND Spell = 1180;
UPDATE `playercreateinfo_spell_custom` SET Note = 'Daggers' WHERE class = 1 AND race = 8 AND Spell = 1180;
UPDATE `playercreateinfo_spell_custom` SET Note = 'Thrown' WHERE class = 1 AND race = 8 AND Spell = 2567;
UPDATE `playercreateinfo_spell_custom` SET Note = 'One-Handed Swords' WHERE class = 1 AND race = 11 AND Spell = 201;
UPDATE `playercreateinfo_spell_custom` SET Note = 'Two-Handed Swords' WHERE class = 1 AND race = 11 AND Spell = 202;
UPDATE `playercreateinfo_spell_custom` SET Note = 'One-Handed Swords' WHERE class = 2 AND race = 10 AND Spell = 201;
UPDATE `playercreateinfo_spell_custom` SET Note = 'Two-Handed Swords' WHERE class = 2 AND race = 10 AND Spell = 202;
UPDATE `playercreateinfo_spell_custom` SET Note = 'Bows' WHERE class = 3 AND race = 2 AND Spell = 264;
UPDATE `playercreateinfo_spell_custom` SET Note = 'Bows' WHERE class = 3 AND race = 4 AND Spell = 264;
UPDATE `playercreateinfo_spell_custom` SET Note = 'Daggers' WHERE class = 3 AND race = 4 AND Spell = 1180;
UPDATE `playercreateinfo_spell_custom` SET Note = 'Bows' WHERE class = 3 AND race = 8 AND Spell = 264;
UPDATE `playercreateinfo_spell_custom` SET Note = 'Bows' WHERE class = 3 AND race = 10 AND Spell = 264;
UPDATE `playercreateinfo_spell_custom` SET Note = 'Daggers' WHERE class = 3 AND race = 10 AND Spell = 1180;
UPDATE `playercreateinfo_spell_custom` SET Note = 'One-Handed Swords' WHERE class = 3 AND race = 11 AND Spell = 201;
UPDATE `playercreateinfo_spell_custom` SET Note = 'Crossbows' WHERE class = 3 AND race = 11 AND Spell = 5011;
UPDATE `playercreateinfo_spell_custom` SET Note = 'One-Handed Maces' WHERE class = 11 AND race = 6 AND Spell = 198;

DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 1 AND Spell = 197;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 1 AND Spell = 200;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 1 AND Spell = 202;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 1 AND Spell = 227;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 1 AND Spell = 264;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 1 AND Spell = 266;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 1 AND Spell = 1180;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 1 AND Spell = 2567;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 1 AND Spell = 5011;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 1 AND Spell = 15590;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 2 AND Spell = 200;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 2 AND Spell = 202;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 2 AND Spell = 227;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 2 AND Spell = 264;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 2 AND Spell = 266;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 2 AND Spell = 1180;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 2 AND Spell = 2567;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 2 AND Spell = 5011;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 2 AND Spell = 15590;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 3 AND Spell = 200;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 3 AND Spell = 201;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 3 AND Spell = 202;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 3 AND Spell = 227;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 3 AND Spell = 264;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 3 AND Spell = 266;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 3 AND Spell = 1180;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 3 AND Spell = 2567;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 3 AND Spell = 5011;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 3 AND Spell = 15590;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 4 AND Spell = 197;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 4 AND Spell = 200;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 4 AND Spell = 202;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 4 AND Spell = 227;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 4 AND Spell = 264;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 4 AND Spell = 266;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 4 AND Spell = 2567;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 4 AND Spell = 5011;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 4 AND Spell = 15590;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 5 AND Spell = 197;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 5 AND Spell = 200;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 5 AND Spell = 227;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 5 AND Spell = 264;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 5 AND Spell = 266;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 5 AND Spell = 2567;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 5 AND Spell = 5011;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 5 AND Spell = 15590;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 6 AND Spell = 197;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 6 AND Spell = 200;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 6 AND Spell = 201;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 6 AND Spell = 202;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 6 AND Spell = 227;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 6 AND Spell = 264;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 6 AND Spell = 266;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 6 AND Spell = 1180;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 6 AND Spell = 2567;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 6 AND Spell = 5011;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 6 AND Spell = 15590;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 7 AND Spell = 197;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 7 AND Spell = 200;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 7 AND Spell = 202;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 7 AND Spell = 227;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 7 AND Spell = 264;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 7 AND Spell = 266;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 7 AND Spell = 2567;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 7 AND Spell = 5011;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 7 AND Spell = 15590;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 8 AND Spell = 197;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 8 AND Spell = 200;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 8 AND Spell = 201;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 8 AND Spell = 202;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 8 AND Spell = 227;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 8 AND Spell = 264;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 8 AND Spell = 266;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 8 AND Spell = 5011;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 8 AND Spell = 15590;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 11 AND Spell = 197;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 11 AND Spell = 200;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 11 AND Spell = 227;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 11 AND Spell = 264;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 11 AND Spell = 266;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 11 AND Spell = 1180;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 11 AND Spell = 2567;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 11 AND Spell = 5011;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 1 AND race = 11 AND Spell = 15590;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 2 AND race = 1 AND Spell = 196;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 2 AND race = 1 AND Spell = 197;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 2 AND race = 1 AND Spell = 200;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 2 AND race = 1 AND Spell = 201;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 2 AND race = 1 AND Spell = 202;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 2 AND race = 10 AND Spell = 196;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 2 AND race = 10 AND Spell = 197;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 2 AND race = 10 AND Spell = 200;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 2 AND race = 11 AND Spell = 196;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 2 AND race = 11 AND Spell = 197;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 2 AND race = 11 AND Spell = 200;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 2 AND race = 11 AND Spell = 201;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 2 AND race = 11 AND Spell = 202;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 2 AND Spell = 197;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 2 AND Spell = 200;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 2 AND Spell = 201;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 2 AND Spell = 202;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 2 AND Spell = 227;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 2 AND Spell = 1180;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 2 AND Spell = 2567;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 2 AND Spell = 5011;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 2 AND Spell = 15590;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 3 AND Spell = 197;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 3 AND Spell = 200;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 3 AND Spell = 201;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 3 AND Spell = 202;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 3 AND Spell = 227;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 3 AND Spell = 264;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 3 AND Spell = 1180;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 3 AND Spell = 2567;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 3 AND Spell = 5011;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 3 AND Spell = 15590;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 4 AND Spell = 197;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 4 AND Spell = 200;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 4 AND Spell = 201;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 4 AND Spell = 202;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 4 AND Spell = 227;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 4 AND Spell = 2567;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 4 AND Spell = 5011;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 4 AND Spell = 15590;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 6 AND Spell = 197;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 6 AND Spell = 200;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 6 AND Spell = 201;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 6 AND Spell = 202;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 6 AND Spell = 227;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 6 AND Spell = 264;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 6 AND Spell = 1180;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 6 AND Spell = 2567;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 6 AND Spell = 5011;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 6 AND Spell = 15590;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 8 AND Spell = 197;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 8 AND Spell = 200;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 8 AND Spell = 201;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 8 AND Spell = 202;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 8 AND Spell = 227;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 8 AND Spell = 1180;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 8 AND Spell = 2567;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 8 AND Spell = 5011;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 8 AND Spell = 15590;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 10 AND Spell = 197;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 10 AND Spell = 200;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 10 AND Spell = 201;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 10 AND Spell = 202;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 10 AND Spell = 227;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 10 AND Spell = 2567;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 10 AND Spell = 5011;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 10 AND Spell = 15590;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 11 AND Spell = 197;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 11 AND Spell = 200;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 11 AND Spell = 202;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 11 AND Spell = 227;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 11 AND Spell = 264;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 11 AND Spell = 1180;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 11 AND Spell = 2567;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 3 AND race = 11 AND Spell = 15590;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 4 AND race = 1 AND Spell = 198;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 4 AND race = 1 AND Spell = 201;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 4 AND race = 1 AND Spell = 264;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 4 AND race = 1 AND Spell = 266;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 4 AND race = 1 AND Spell = 5011;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 4 AND race = 1 AND Spell = 15590;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 4 AND race = 3 AND Spell = 198;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 4 AND race = 3 AND Spell = 201;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 4 AND race = 3 AND Spell = 264;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 4 AND race = 3 AND Spell = 266;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 4 AND race = 3 AND Spell = 5011;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 4 AND race = 3 AND Spell = 15590;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 4 AND race = 4 AND Spell = 198;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 4 AND race = 4 AND Spell = 201;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 4 AND race = 4 AND Spell = 264;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 4 AND race = 4 AND Spell = 266;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 4 AND race = 4 AND Spell = 5011;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 4 AND race = 4 AND Spell = 15590;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 4 AND race = 5 AND Spell = 198;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 4 AND race = 5 AND Spell = 201;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 4 AND race = 5 AND Spell = 264;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 4 AND race = 5 AND Spell = 266;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 4 AND race = 5 AND Spell = 5011;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 4 AND race = 5 AND Spell = 15590;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 4 AND race = 7 AND Spell = 198;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 4 AND race = 7 AND Spell = 201;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 4 AND race = 7 AND Spell = 264;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 4 AND race = 7 AND Spell = 266;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 4 AND race = 7 AND Spell = 5011;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 4 AND race = 7 AND Spell = 15590;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 4 AND race = 8 AND Spell = 198;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 4 AND race = 8 AND Spell = 201;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 4 AND race = 8 AND Spell = 264;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 4 AND race = 8 AND Spell = 266;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 4 AND race = 8 AND Spell = 5011;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 4 AND race = 8 AND Spell = 15590;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 4 AND race = 10 AND Spell = 198;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 4 AND race = 10 AND Spell = 201;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 4 AND race = 10 AND Spell = 264;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 4 AND race = 10 AND Spell = 266;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 4 AND race = 10 AND Spell = 5011;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 4 AND race = 10 AND Spell = 15590;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 1 AND Spell = 227;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 1 AND Spell = 1180;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 1 AND Spell = 13896;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 1 AND Spell = 19271;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 1 AND Spell = 19273;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 1 AND Spell = 19274;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 1 AND Spell = 19275;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 1 AND Spell = 44041;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 1 AND Spell = 44043;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 1 AND Spell = 44044;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 1 AND Spell = 44045;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 1 AND Spell = 44046;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 1 AND Spell = 44047;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 3 AND Spell = 227;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 3 AND Spell = 1180;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 3 AND Spell = 13908;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 3 AND Spell = 19236;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 3 AND Spell = 19238;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 3 AND Spell = 19240;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 3 AND Spell = 19241;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 3 AND Spell = 19242;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 3 AND Spell = 19243;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 3 AND Spell = 44041;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 3 AND Spell = 44043;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 3 AND Spell = 44044;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 3 AND Spell = 44045;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 3 AND Spell = 44046;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 3 AND Spell = 44047;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 4 AND Spell = 227;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 4 AND Spell = 1180;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 4 AND Spell = 2651;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 4 AND Spell = 10797;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 4 AND Spell = 19296;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 4 AND Spell = 19299;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 4 AND Spell = 19302;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 4 AND Spell = 19303;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 4 AND Spell = 19304;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 4 AND Spell = 19305;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 4 AND Spell = 25446;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 5 AND Spell = 227;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 5 AND Spell = 1180;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 5 AND Spell = 2652;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 5 AND Spell = 2944;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 5 AND Spell = 19249;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 5 AND Spell = 19251;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 5 AND Spell = 19252;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 5 AND Spell = 19253;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 5 AND Spell = 19254;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 5 AND Spell = 19261;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 5 AND Spell = 19262;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 5 AND Spell = 19264;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 5 AND Spell = 19265;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 5 AND Spell = 19266;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 5 AND Spell = 19276;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 5 AND Spell = 19277;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 5 AND Spell = 19278;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 5 AND Spell = 19279;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 5 AND Spell = 19280;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 5 AND Spell = 25460;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 5 AND Spell = 25461;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 5 AND Spell = 25467;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 8 AND Spell = 227;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 8 AND Spell = 1180;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 8 AND Spell = 9035;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 8 AND Spell = 18137;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 8 AND Spell = 19249;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 8 AND Spell = 19281;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 8 AND Spell = 19282;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 8 AND Spell = 19283;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 8 AND Spell = 19284;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 8 AND Spell = 19285;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 8 AND Spell = 19308;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 8 AND Spell = 19309;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 8 AND Spell = 19311;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 8 AND Spell = 19312;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 8 AND Spell = 25470;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 10 AND Spell = 227;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 10 AND Spell = 1180;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 10 AND Spell = 2652;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 10 AND Spell = 19249;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 10 AND Spell = 19251;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 10 AND Spell = 19252;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 10 AND Spell = 19253;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 10 AND Spell = 19254;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 10 AND Spell = 19261;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 10 AND Spell = 19262;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 10 AND Spell = 19264;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 10 AND Spell = 19265;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 10 AND Spell = 19266;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 10 AND Spell = 25460;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 10 AND Spell = 25461;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 10 AND Spell = 32676;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 11 AND Spell = 227;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 11 AND Spell = 1180;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 11 AND Spell = 32548;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 11 AND Spell = 44041;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 11 AND Spell = 44043;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 11 AND Spell = 44044;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 11 AND Spell = 44045;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 11 AND Spell = 44046;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 5 AND race = 11 AND Spell = 44047;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 7 AND race = 2 AND Spell = 196;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 7 AND race = 2 AND Spell = 197;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 7 AND race = 2 AND Spell = 199;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 7 AND race = 2 AND Spell = 1180;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 7 AND race = 2 AND Spell = 15590;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 7 AND race = 6 AND Spell = 196;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 7 AND race = 6 AND Spell = 197;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 7 AND race = 6 AND Spell = 199;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 7 AND race = 6 AND Spell = 1180;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 7 AND race = 6 AND Spell = 15590;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 7 AND race = 8 AND Spell = 196;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 7 AND race = 8 AND Spell = 197;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 7 AND race = 8 AND Spell = 199;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 7 AND race = 8 AND Spell = 1180;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 7 AND race = 8 AND Spell = 15590;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 7 AND race = 11 AND Spell = 196;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 7 AND race = 11 AND Spell = 197;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 7 AND race = 11 AND Spell = 199;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 7 AND race = 11 AND Spell = 1180;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 7 AND race = 11 AND Spell = 15590;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 8 AND race = 1 AND Spell = 201;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 8 AND race = 1 AND Spell = 1180;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 8 AND race = 5 AND Spell = 201;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 8 AND race = 5 AND Spell = 1180;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 8 AND race = 7 AND Spell = 201;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 8 AND race = 7 AND Spell = 1180;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 8 AND race = 8 AND Spell = 201;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 8 AND race = 8 AND Spell = 1180;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 8 AND race = 10 AND Spell = 201;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 8 AND race = 10 AND Spell = 1180;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 8 AND race = 11 AND Spell = 201;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 8 AND race = 11 AND Spell = 1180;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 9 AND race = 1 AND Spell = 201;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 9 AND race = 1 AND Spell = 227;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 9 AND race = 2 AND Spell = 201;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 9 AND race = 2 AND Spell = 227;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 9 AND race = 5 AND Spell = 201;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 9 AND race = 5 AND Spell = 227;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 9 AND race = 7 AND Spell = 201;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 9 AND race = 7 AND Spell = 227;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 9 AND race = 10 AND Spell = 201;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 9 AND race = 10 AND Spell = 227;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 11 AND race = 4 AND Spell = 198;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 11 AND race = 4 AND Spell = 199;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 11 AND race = 4 AND Spell = 15590;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 11 AND race = 6 AND Spell = 199;
DELETE FROM `playercreateinfo_spell_custom` WHERE class = 11 AND race = 6 AND Spell = 15590;

UPDATE `quest_template` SET SpecialFlags = 2 WHERE entry = 309;
UPDATE `quest_template` SET ReqCreatureOrGOCount1 = 0, ReqCreatureOrGOId1 = 0, SpecialFlags = 2 WHERE entry = 1249;
UPDATE `quest_template` SET SpecialFlags = 2 WHERE entry = 1560;
UPDATE `quest_template` SET SpecialFlags = 2 WHERE entry = 4904;
UPDATE `quest_template` SET SpecialFlags = 2 WHERE entry = 4966;


UPDATE `script_texts` SET type = 2 WHERE entry = -1544013;

DELETE FROM `script_texts` WHERE entry = -1645009;
DELETE FROM `script_texts` WHERE entry = -1645008;
DELETE FROM `script_texts` WHERE entry = -1645007;
DELETE FROM `script_texts` WHERE entry = -1645006;
DELETE FROM `script_texts` WHERE entry = -1580109;
DELETE FROM `script_texts` WHERE entry = -1580108;
DELETE FROM `script_texts` WHERE entry = -1580107;
DELETE FROM `script_texts` WHERE entry = -1580106;
DELETE FROM `script_texts` WHERE entry = -1580105;
DELETE FROM `script_texts` WHERE entry = -1580104;
DELETE FROM `script_texts` WHERE entry = -1580103;
DELETE FROM `script_texts` WHERE entry = -1580102;
DELETE FROM `script_texts` WHERE entry = -1580101;
DELETE FROM `script_texts` WHERE entry = -1580100;
DELETE FROM `script_texts` WHERE entry = -1580099;
DELETE FROM `script_texts` WHERE entry = -1580098;
DELETE FROM `script_texts` WHERE entry = -1580097;
DELETE FROM `script_texts` WHERE entry = -1580096;
DELETE FROM `script_texts` WHERE entry = -1580095;
DELETE FROM `script_texts` WHERE entry = -1580094;
DELETE FROM `script_texts` WHERE entry = -1580093;
DELETE FROM `script_texts` WHERE entry = -1580092;
DELETE FROM `script_texts` WHERE entry = -1580091;
DELETE FROM `script_texts` WHERE entry = -1580090;
DELETE FROM `script_texts` WHERE entry = -1580089;
DELETE FROM `script_texts` WHERE entry = -1580088;
DELETE FROM `script_texts` WHERE entry = -1580087;
DELETE FROM `script_texts` WHERE entry = -1580086;
DELETE FROM `script_texts` WHERE entry = -1580085;
DELETE FROM `script_texts` WHERE entry = -1580084;
DELETE FROM `script_texts` WHERE entry = -1580083;
DELETE FROM `script_texts` WHERE entry = -1580082;
DELETE FROM `script_texts` WHERE entry = -1580081;
DELETE FROM `script_texts` WHERE entry = -1580080;
DELETE FROM `script_texts` WHERE entry = -1580079;
DELETE FROM `script_texts` WHERE entry = -1580078;
DELETE FROM `script_texts` WHERE entry = -1580077;
DELETE FROM `script_texts` WHERE entry = -1580076;
DELETE FROM `script_texts` WHERE entry = -1580075;
DELETE FROM `script_texts` WHERE entry = -1580074;
DELETE FROM `script_texts` WHERE entry = -1580073;
DELETE FROM `script_texts` WHERE entry = -1580072;
DELETE FROM `script_texts` WHERE entry = -1580071;
DELETE FROM `script_texts` WHERE entry = -1580070;
DELETE FROM `script_texts` WHERE entry = -1580069;
DELETE FROM `script_texts` WHERE entry = -1580068;
DELETE FROM `script_texts` WHERE entry = -1580067;
DELETE FROM `script_texts` WHERE entry = -1580066;
DELETE FROM `script_texts` WHERE entry = -1580065;
DELETE FROM `script_texts` WHERE entry = -1580064;
DELETE FROM `script_texts` WHERE entry = -1580063;
DELETE FROM `script_texts` WHERE entry = -1580062;
DELETE FROM `script_texts` WHERE entry = -1580061;
DELETE FROM `script_texts` WHERE entry = -1580060;
DELETE FROM `script_texts` WHERE entry = -1580059;
DELETE FROM `script_texts` WHERE entry = -1580058;
DELETE FROM `script_texts` WHERE entry = -1580057;
DELETE FROM `script_texts` WHERE entry = -1580056;
DELETE FROM `script_texts` WHERE entry = -1580055;
DELETE FROM `script_texts` WHERE entry = -1580054;
DELETE FROM `script_texts` WHERE entry = -1580053;
DELETE FROM `script_texts` WHERE entry = -1580052;
DELETE FROM `script_texts` WHERE entry = -1580051;
DELETE FROM `script_texts` WHERE entry = -1580050;
DELETE FROM `script_texts` WHERE entry = -1580049;
DELETE FROM `script_texts` WHERE entry = -1580048;
DELETE FROM `script_texts` WHERE entry = -1580047;
DELETE FROM `script_texts` WHERE entry = -1580046;
DELETE FROM `script_texts` WHERE entry = -1580045;
DELETE FROM `script_texts` WHERE entry = -1580044;
DELETE FROM `script_texts` WHERE entry = -1580043;
DELETE FROM `script_texts` WHERE entry = -1580042;
DELETE FROM `script_texts` WHERE entry = -1580041;
DELETE FROM `script_texts` WHERE entry = -1580040;
DELETE FROM `script_texts` WHERE entry = -1580039;
DELETE FROM `script_texts` WHERE entry = -1580038;
DELETE FROM `script_texts` WHERE entry = -1580037;
DELETE FROM `script_texts` WHERE entry = -1580036;
DELETE FROM `script_texts` WHERE entry = -1560049;
DELETE FROM `script_texts` WHERE entry = -1560048;
DELETE FROM `script_texts` WHERE entry = -1560047;
DELETE FROM `script_texts` WHERE entry = -1560046;
DELETE FROM `script_texts` WHERE entry = -1560045;
DELETE FROM `script_texts` WHERE entry = -1560044;
DELETE FROM `script_texts` WHERE entry = -1560043;
DELETE FROM `script_texts` WHERE entry = -1560042;
DELETE FROM `script_texts` WHERE entry = -1560041;
DELETE FROM `script_texts` WHERE entry = -1560040;
DELETE FROM `script_texts` WHERE entry = -1560039;
DELETE FROM `script_texts` WHERE entry = -1560038;
DELETE FROM `script_texts` WHERE entry = -1560037;
DELETE FROM `script_texts` WHERE entry = -1560036;
DELETE FROM `script_texts` WHERE entry = -1560035;
DELETE FROM `script_texts` WHERE entry = -1560034;
DELETE FROM `script_texts` WHERE entry = -1560033;
DELETE FROM `script_texts` WHERE entry = -1560032;
DELETE FROM `script_texts` WHERE entry = -1560031;
DELETE FROM `script_texts` WHERE entry = -1560030;
DELETE FROM `script_texts` WHERE entry = -1560029;
DELETE FROM `script_texts` WHERE entry = -1560028;
DELETE FROM `script_texts` WHERE entry = -1560027;
DELETE FROM `script_texts` WHERE entry = -1560026;
DELETE FROM `script_texts` WHERE entry = -1560025;
DELETE FROM `script_texts` WHERE entry = -1560024;
DELETE FROM `script_texts` WHERE entry = -1560023;
DELETE FROM `script_texts` WHERE entry = -1544015;
DELETE FROM `script_texts` WHERE entry = -1543025;
DELETE FROM `script_texts` WHERE entry = -1543024;
DELETE FROM `script_texts` WHERE entry = -1543023;
DELETE FROM `script_texts` WHERE entry = -1543022;
DELETE FROM `script_texts` WHERE entry = -1543021;
DELETE FROM `script_texts` WHERE entry = -1543020;
DELETE FROM `script_texts` WHERE entry = -1543019;
DELETE FROM `script_texts` WHERE entry = -1543018;
DELETE FROM `script_texts` WHERE entry = -1543017;
DELETE FROM `script_texts` WHERE entry = -1534030;
DELETE FROM `script_texts` WHERE entry = -1534029;
DELETE FROM `script_texts` WHERE entry = -1534028;
DELETE FROM `script_texts` WHERE entry = -1534027;
DELETE FROM `script_texts` WHERE entry = -1534026;
DELETE FROM `script_texts` WHERE entry = -1534025;
DELETE FROM `script_texts` WHERE entry = -1534024;
DELETE FROM `script_texts` WHERE entry = -1534023;
DELETE FROM `script_texts` WHERE entry = -1534022;
DELETE FROM `script_texts` WHERE entry = -1534021;
DELETE FROM `script_texts` WHERE entry = -1534020;
DELETE FROM `script_texts` WHERE entry = -1534019;
DELETE FROM `script_texts` WHERE entry = -1534018;
DELETE FROM `script_texts` WHERE entry = -1534017;
DELETE FROM `script_texts` WHERE entry = -1534016;
DELETE FROM `script_texts` WHERE entry = -1534015;
DELETE FROM `script_texts` WHERE entry = -1534014;
DELETE FROM `script_texts` WHERE entry = -1534013;
DELETE FROM `script_texts` WHERE entry = -1534012;
DELETE FROM `script_texts` WHERE entry = -1534011;
DELETE FROM `script_texts` WHERE entry = -1534010;
DELETE FROM `script_texts` WHERE entry = -1534009;
DELETE FROM `script_texts` WHERE entry = -1534008;
DELETE FROM `script_texts` WHERE entry = -1534007;
DELETE FROM `script_texts` WHERE entry = -1534006;
DELETE FROM `script_texts` WHERE entry = -1534005;
DELETE FROM `script_texts` WHERE entry = -1534004;
DELETE FROM `script_texts` WHERE entry = -1534003;
DELETE FROM `script_texts` WHERE entry = -1534002;
DELETE FROM `script_texts` WHERE entry = -1534001;
DELETE FROM `script_texts` WHERE entry = -1534000;
DELETE FROM `script_texts` WHERE entry = -1532114;
DELETE FROM `script_texts` WHERE entry = -1532113;
DELETE FROM `script_texts` WHERE entry = -1532112;
DELETE FROM `script_texts` WHERE entry = -1532111;
DELETE FROM `script_texts` WHERE entry = -1532110;
DELETE FROM `script_texts` WHERE entry = -1532109;
DELETE FROM `script_texts` WHERE entry = -1532108;
DELETE FROM `script_texts` WHERE entry = -1532107;
DELETE FROM `script_texts` WHERE entry = -1532106;
DELETE FROM `script_texts` WHERE entry = -1532105;
DELETE FROM `script_texts` WHERE entry = -1532104;
DELETE FROM `script_texts` WHERE entry = -1532103;
DELETE FROM `script_texts` WHERE entry = -1269028;
DELETE FROM `script_texts` WHERE entry = -1269027;
DELETE FROM `script_texts` WHERE entry = -1269026;
DELETE FROM `script_texts` WHERE entry = -1269025;
DELETE FROM `script_texts` WHERE entry = -1269024;
DELETE FROM `script_texts` WHERE entry = -1269023;
DELETE FROM `script_texts` WHERE entry = -1269022;
DELETE FROM `script_texts` WHERE entry = -1269021;
DELETE FROM `script_texts` WHERE entry = -1269020;
DELETE FROM `script_texts` WHERE entry = -1269019;
DELETE FROM `script_texts` WHERE entry = -1269018;
DELETE FROM `script_texts` WHERE entry = -1230000;
DELETE FROM `script_texts` WHERE entry = -1189010;
DELETE FROM `script_texts` WHERE entry = -1189009;
DELETE FROM `script_texts` WHERE entry = -1189008;
DELETE FROM `script_texts` WHERE entry = -1189007;
DELETE FROM `script_texts` WHERE entry = -1189006;
DELETE FROM `script_texts` WHERE entry = -1189005;
DELETE FROM `script_texts` WHERE entry = -1189004;
DELETE FROM `script_texts` WHERE entry = -1189003;
DELETE FROM `script_texts` WHERE entry = -1189002;
DELETE FROM `script_texts` WHERE entry = -1189001;
DELETE FROM `script_texts` WHERE entry = -1189000;
DELETE FROM `script_texts` WHERE entry = -1060005;
DELETE FROM `script_texts` WHERE entry = -1060004;
DELETE FROM `script_texts` WHERE entry = -1060003;
DELETE FROM `script_texts` WHERE entry = -1060002;
DELETE FROM `script_texts` WHERE entry = -1060001;
DELETE FROM `script_texts` WHERE entry = -1060000;
DELETE FROM `script_texts` WHERE entry = -1047010;
DELETE FROM `script_texts` WHERE entry = -1047009;
DELETE FROM `script_texts` WHERE entry = -1047008;
DELETE FROM `script_texts` WHERE entry = -1047007;
DELETE FROM `script_texts` WHERE entry = -1047006;
DELETE FROM `script_texts` WHERE entry = -1047005;
DELETE FROM `script_texts` WHERE entry = -1047004;
DELETE FROM `script_texts` WHERE entry = -1047003;
DELETE FROM `script_texts` WHERE entry = -1047002;
DELETE FROM `script_texts` WHERE entry = -1047001;
DELETE FROM `script_texts` WHERE entry = -1047000;
DELETE FROM `script_texts` WHERE entry = -1036001;
DELETE FROM `script_texts` WHERE entry = -1036000;
DELETE FROM `script_texts` WHERE entry = -1000401;
DELETE FROM `script_texts` WHERE entry = -1000400;
DELETE FROM `script_texts` WHERE entry = -1000399;
DELETE FROM `script_texts` WHERE entry = -1000398;
DELETE FROM `script_texts` WHERE entry = -1000397;
DELETE FROM `script_texts` WHERE entry = -1000396;
DELETE FROM `script_texts` WHERE entry = -1000395;
DELETE FROM `script_texts` WHERE entry = -1000394;
DELETE FROM `script_texts` WHERE entry = -1000393;
DELETE FROM `script_texts` WHERE entry = -1000392;
DELETE FROM `script_texts` WHERE entry = -1000391;
DELETE FROM `script_texts` WHERE entry = -1000390;
DELETE FROM `script_texts` WHERE entry = -1000389;
DELETE FROM `script_texts` WHERE entry = -1000388;
DELETE FROM `script_texts` WHERE entry = -1000387;
DELETE FROM `script_texts` WHERE entry = -1000386;
DELETE FROM `script_texts` WHERE entry = -1000385;
DELETE FROM `script_texts` WHERE entry = -1000384;
DELETE FROM `script_texts` WHERE entry = -1000383;
DELETE FROM `script_texts` WHERE entry = -1000382;
DELETE FROM `script_texts` WHERE entry = -1000381;
DELETE FROM `script_texts` WHERE entry = -1000380;
DELETE FROM `script_texts` WHERE entry = -1000379;
DELETE FROM `script_texts` WHERE entry = -1000378;
DELETE FROM `script_texts` WHERE entry = -1000377;
DELETE FROM `script_texts` WHERE entry = -1000376;
DELETE FROM `script_texts` WHERE entry = -1000375;
DELETE FROM `script_texts` WHERE entry = -1000374;
DELETE FROM `script_texts` WHERE entry = -1000373;
DELETE FROM `script_texts` WHERE entry = -1000372;
DELETE FROM `script_texts` WHERE entry = -1000371;
DELETE FROM `script_texts` WHERE entry = -1000370;
DELETE FROM `script_texts` WHERE entry = -1000369;
DELETE FROM `script_texts` WHERE entry = -1000368;
DELETE FROM `script_texts` WHERE entry = -1000367;
DELETE FROM `script_texts` WHERE entry = -1000366;
DELETE FROM `script_texts` WHERE entry = -1000365;
DELETE FROM `script_texts` WHERE entry = -1000364;
DELETE FROM `script_texts` WHERE entry = -1000363;
DELETE FROM `script_texts` WHERE entry = -1000362;
DELETE FROM `script_texts` WHERE entry = -1000361;
DELETE FROM `script_texts` WHERE entry = -1000360;
DELETE FROM `script_texts` WHERE entry = -1000359;
DELETE FROM `script_texts` WHERE entry = -1000358;
DELETE FROM `script_texts` WHERE entry = -1000357;
DELETE FROM `script_texts` WHERE entry = -1000356;
DELETE FROM `script_texts` WHERE entry = -1000355;
DELETE FROM `script_texts` WHERE entry = -1000354;
DELETE FROM `script_texts` WHERE entry = -1000353;
DELETE FROM `script_texts` WHERE entry = -1000352;
DELETE FROM `script_texts` WHERE entry = -1000351;
DELETE FROM `script_texts` WHERE entry = -1000350;
DELETE FROM `script_texts` WHERE entry = -1000349;
DELETE FROM `script_texts` WHERE entry = -1000348;
DELETE FROM `script_texts` WHERE entry = -1000347;
DELETE FROM `script_texts` WHERE entry = -1000346;
DELETE FROM `script_texts` WHERE entry = -1000345;
DELETE FROM `script_texts` WHERE entry = -1000344;
DELETE FROM `script_texts` WHERE entry = -1000343;
DELETE FROM `script_texts` WHERE entry = -1000342;
DELETE FROM `script_texts` WHERE entry = -1000341;
DELETE FROM `script_texts` WHERE entry = -1000340;
DELETE FROM `script_texts` WHERE entry = -1000339;
DELETE FROM `script_texts` WHERE entry = -1000338;
DELETE FROM `script_texts` WHERE entry = -1000337;
DELETE FROM `script_texts` WHERE entry = -1000336;
DELETE FROM `script_texts` WHERE entry = -1000335;
DELETE FROM `script_texts` WHERE entry = -1000334;
DELETE FROM `script_texts` WHERE entry = -1000333;
DELETE FROM `script_texts` WHERE entry = -1000332;
DELETE FROM `script_texts` WHERE entry = -1000331;
DELETE FROM `script_texts` WHERE entry = -1000330;
DELETE FROM `script_texts` WHERE entry = -1000329;
DELETE FROM `script_texts` WHERE entry = -1000328;
DELETE FROM `script_texts` WHERE entry = -1000327;
DELETE FROM `script_texts` WHERE entry = -1000326;
DELETE FROM `script_texts` WHERE entry = -1000325;
DELETE FROM `script_texts` WHERE entry = -1000324;
DELETE FROM `script_texts` WHERE entry = -1000323;
DELETE FROM `script_texts` WHERE entry = -1000322;
DELETE FROM `script_texts` WHERE entry = -1000321;
DELETE FROM `script_texts` WHERE entry = -1000320;
DELETE FROM `script_texts` WHERE entry = -1000319;
DELETE FROM `script_texts` WHERE entry = -1000318;
DELETE FROM `script_texts` WHERE entry = -1000317;
DELETE FROM `script_texts` WHERE entry = -1000316;
DELETE FROM `script_texts` WHERE entry = -1000315;
DELETE FROM `script_texts` WHERE entry = -1000314;
DELETE FROM `script_texts` WHERE entry = -1000313;
DELETE FROM `script_texts` WHERE entry = -1000312;
DELETE FROM `script_texts` WHERE entry = -1000311;
DELETE FROM `script_texts` WHERE entry = -1000310;
DELETE FROM `script_texts` WHERE entry = -1000309;
DELETE FROM `script_texts` WHERE entry = -1000308;
DELETE FROM `script_texts` WHERE entry = -1000307;
DELETE FROM `script_texts` WHERE entry = -1000306;
DELETE FROM `script_texts` WHERE entry = -1000305;
DELETE FROM `script_texts` WHERE entry = -1000304;
DELETE FROM `script_texts` WHERE entry = -1000303;
DELETE FROM `script_texts` WHERE entry = -1000302;
DELETE FROM `script_texts` WHERE entry = -1000301;
DELETE FROM `script_texts` WHERE entry = -1000300;
DELETE FROM `script_texts` WHERE entry = -1000299;
DELETE FROM `script_texts` WHERE entry = -1000298;
DELETE FROM `script_texts` WHERE entry = -1000297;
DELETE FROM `script_texts` WHERE entry = -1000296;
DELETE FROM `script_texts` WHERE entry = -1000295;
DELETE FROM `script_texts` WHERE entry = -1000294;
DELETE FROM `script_texts` WHERE entry = -1000293;
DELETE FROM `script_texts` WHERE entry = -1000292;
DELETE FROM `script_texts` WHERE entry = -1000291;
DELETE FROM `script_texts` WHERE entry = -1000290;
DELETE FROM `script_texts` WHERE entry = -1000289;
DELETE FROM `script_texts` WHERE entry = -1000288;
DELETE FROM `script_texts` WHERE entry = -1000287;
DELETE FROM `script_texts` WHERE entry = -1000286;
DELETE FROM `script_texts` WHERE entry = -1000285;
DELETE FROM `script_texts` WHERE entry = -1000284;
DELETE FROM `script_texts` WHERE entry = -1000283;
DELETE FROM `script_texts` WHERE entry = -1000282;
DELETE FROM `script_texts` WHERE entry = -1000281;
DELETE FROM `script_texts` WHERE entry = -1000280;
DELETE FROM `script_texts` WHERE entry = -1000279;
DELETE FROM `script_texts` WHERE entry = -1000278;
DELETE FROM `script_texts` WHERE entry = -1000277;
DELETE FROM `script_texts` WHERE entry = -1000276;
DELETE FROM `script_texts` WHERE entry = -1000275;
DELETE FROM `script_texts` WHERE entry = -1000274;
DELETE FROM `script_texts` WHERE entry = -1000273;
DELETE FROM `script_texts` WHERE entry = -1000272;
DELETE FROM `script_texts` WHERE entry = -1000271;
DELETE FROM `script_texts` WHERE entry = -1000270;
DELETE FROM `script_texts` WHERE entry = -1000269;
DELETE FROM `script_texts` WHERE entry = -1000268;
DELETE FROM `script_texts` WHERE entry = -1000267;
DELETE FROM `script_texts` WHERE entry = -1000266;
DELETE FROM `script_texts` WHERE entry = -1000265;
DELETE FROM `script_texts` WHERE entry = -1000264;
DELETE FROM `script_texts` WHERE entry = -1000263;
DELETE FROM `script_texts` WHERE entry = -1000262;
DELETE FROM `script_texts` WHERE entry = -1000261;
DELETE FROM `script_texts` WHERE entry = -1000260;
DELETE FROM `script_texts` WHERE entry = -1000259;
DELETE FROM `script_texts` WHERE entry = -1000258;
DELETE FROM `script_texts` WHERE entry = -1000257;
DELETE FROM `script_texts` WHERE entry = -1000256;
DELETE FROM `script_texts` WHERE entry = -1000255;
DELETE FROM `script_texts` WHERE entry = -1000254;
DELETE FROM `script_texts` WHERE entry = -1000253;
DELETE FROM `script_texts` WHERE entry = -1000252;
DELETE FROM `script_texts` WHERE entry = -1000251;
DELETE FROM `script_texts` WHERE entry = -1000250;
DELETE FROM `script_texts` WHERE entry = -1000249;
DELETE FROM `script_texts` WHERE entry = -1000248;
DELETE FROM `script_texts` WHERE entry = -1000247;
DELETE FROM `script_texts` WHERE entry = -1000246;
DELETE FROM `script_texts` WHERE entry = -1000245;
DELETE FROM `script_texts` WHERE entry = -1000244;
DELETE FROM `script_texts` WHERE entry = -1000243;
DELETE FROM `script_texts` WHERE entry = -1000242;
DELETE FROM `script_texts` WHERE entry = -1000241;
DELETE FROM `script_texts` WHERE entry = -1000240;
DELETE FROM `script_texts` WHERE entry = -1000239;
DELETE FROM `script_texts` WHERE entry = -1000238;
DELETE FROM `script_texts` WHERE entry = -1000237;
DELETE FROM `script_texts` WHERE entry = -1000236;
DELETE FROM `script_texts` WHERE entry = -1000235;
DELETE FROM `script_texts` WHERE entry = -1000234;
DELETE FROM `script_texts` WHERE entry = -1000233;
DELETE FROM `script_texts` WHERE entry = -1000232;
DELETE FROM `script_texts` WHERE entry = -1000231;
DELETE FROM `script_texts` WHERE entry = -1000230;
DELETE FROM `script_texts` WHERE entry = -1000229;
DELETE FROM `script_texts` WHERE entry = -1000228;
DELETE FROM `script_texts` WHERE entry = -1000227;
DELETE FROM `script_texts` WHERE entry = -1000226;
DELETE FROM `script_texts` WHERE entry = -1000225;
DELETE FROM `script_texts` WHERE entry = -1000224;
DELETE FROM `script_texts` WHERE entry = -1000223;
DELETE FROM `script_texts` WHERE entry = -1000222;
DELETE FROM `script_texts` WHERE entry = -1000221;
DELETE FROM `script_texts` WHERE entry = -1000220;
DELETE FROM `script_texts` WHERE entry = -1000219;
DELETE FROM `script_texts` WHERE entry = -1000218;
DELETE FROM `script_texts` WHERE entry = -1000217;
DELETE FROM `script_texts` WHERE entry = -1000216;
DELETE FROM `script_texts` WHERE entry = -1000215;
DELETE FROM `script_texts` WHERE entry = -1000214;
DELETE FROM `script_texts` WHERE entry = -1000213;
DELETE FROM `script_texts` WHERE entry = -1000212;
DELETE FROM `script_texts` WHERE entry = -1000211;
DELETE FROM `script_texts` WHERE entry = -1000210;
DELETE FROM `script_texts` WHERE entry = -1000209;
DELETE FROM `script_texts` WHERE entry = -1000208;
DELETE FROM `script_texts` WHERE entry = -1000207;
DELETE FROM `script_texts` WHERE entry = -1000206;
DELETE FROM `script_texts` WHERE entry = -1000205;
DELETE FROM `script_texts` WHERE entry = -1000204;
DELETE FROM `script_texts` WHERE entry = -1000203;
DELETE FROM `script_texts` WHERE entry = -1000202;
DELETE FROM `script_texts` WHERE entry = -1000201;
DELETE FROM `script_texts` WHERE entry = -1000200;
DELETE FROM `script_texts` WHERE entry = -1000163;
DELETE FROM `script_texts` WHERE entry = -1000162;
DELETE FROM `script_texts` WHERE entry = -1000161;
DELETE FROM `script_texts` WHERE entry = -1000160;
DELETE FROM `script_texts` WHERE entry = -1000159;
DELETE FROM `script_texts` WHERE entry = -1000158;
DELETE FROM `script_texts` WHERE entry = -1000157;
DELETE FROM `script_texts` WHERE entry = -1000156;
DELETE FROM `script_texts` WHERE entry = -1000155;
DELETE FROM `script_texts` WHERE entry = -1000154;
DELETE FROM `script_texts` WHERE entry = -1000153;
DELETE FROM `script_texts` WHERE entry = -1000152;
DELETE FROM `script_texts` WHERE entry = -1000151;
DELETE FROM `script_texts` WHERE entry = -1000150;
DELETE FROM `script_texts` WHERE entry = -1000149;
DELETE FROM `script_texts` WHERE entry = -1000148;
DELETE FROM `script_texts` WHERE entry = -1000147;
DELETE FROM `script_texts` WHERE entry = -1000146;
DELETE FROM `script_texts` WHERE entry = -1000145;
DELETE FROM `script_texts` WHERE entry = -1000144;
DELETE FROM `script_texts` WHERE entry = -1000143;
DELETE FROM `script_texts` WHERE entry = -1000142;
DELETE FROM `script_texts` WHERE entry = -1000141;
DELETE FROM `script_texts` WHERE entry = -1000140;
DELETE FROM `script_texts` WHERE entry = -1000139;
DELETE FROM `script_texts` WHERE entry = -1000138;
DELETE FROM `script_texts` WHERE entry = -1000137;
DELETE FROM `script_texts` WHERE entry = -1000136;
DELETE FROM `script_texts` WHERE entry = -1000135;
DELETE FROM `script_texts` WHERE entry = -1000134;
DELETE FROM `script_texts` WHERE entry = -1000133;
DELETE FROM `script_texts` WHERE entry = -1000132;
DELETE FROM `script_texts` WHERE entry = -1000131;
DELETE FROM `script_texts` WHERE entry = -1000130;
DELETE FROM `script_texts` WHERE entry = -1000129;
DELETE FROM `script_texts` WHERE entry = -1000128;
DELETE FROM `script_texts` WHERE entry = -1000127;
DELETE FROM `script_texts` WHERE entry = -1000126;
DELETE FROM `script_texts` WHERE entry = -1000125;
DELETE FROM `script_texts` WHERE entry = -1000124;
DELETE FROM `script_texts` WHERE entry = -1000123;
DELETE FROM `script_texts` WHERE entry = -1000122;
DELETE FROM `script_texts` WHERE entry = -1000121;
DELETE FROM `script_texts` WHERE entry = -1000120;
DELETE FROM `script_texts` WHERE entry = -1000119;
DELETE FROM `script_texts` WHERE entry = -1000118;
DELETE FROM `script_texts` WHERE entry = -1000117;
DELETE FROM `script_texts` WHERE entry = -1000116;
DELETE FROM `script_texts` WHERE entry = -1000115;
DELETE FROM `script_texts` WHERE entry = -1000114;
DELETE FROM `script_texts` WHERE entry = -1000113;
DELETE FROM `script_texts` WHERE entry = -1000112;
DELETE FROM `script_texts` WHERE entry = -1000111;
DELETE FROM `script_texts` WHERE entry = -1000110;
DELETE FROM `script_texts` WHERE entry = -1000109;
DELETE FROM `script_texts` WHERE entry = -1000108;
DELETE FROM `script_texts` WHERE entry = -1000107;
DELETE FROM `script_texts` WHERE entry = -1000106;


REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (1379, 1, -5752.71, -3451.08, 302.997, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (1379, 2, -5741.64, -3476.68, 302.035, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (1379, 3, -5697.58, -3552.15, 305.856, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (1379, 4, -5674.58, -3598.85, 311.84, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (1379, 5, -5680.05, -3638.78, 314.253, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (1379, 6, -5692.6, -3674.49, 313.228, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (1379, 7, -5695.73, -3708.04, 315.165, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (1379, 8, -5697.27, -3736.36, 318.54, 'ambush', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (1379, 9, -5689.47, -3768.88, 323.992, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (1379, 10, -5689.22, -3787.25, 322.136, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (1379, 11, -5700.04, -3792.52, 322.519, '', 20000);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (3439, 25, 1111.06, -2976.54, 91.81, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (3439, 26, 1099.91, -2991.17, 91.67, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (3439, 27, 1096.32, -2981.55, 91.73, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (3439, 28, 1091.28, -2985.82, 91.74, '', 4000);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (3439, 29, 1091.28, -2985.82, 91.74, '', 3000);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (3439, 30, 1091.28, -2985.82, 91.74, '', 7000);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (3439, 31, 1091.28, -2985.82, 91.74, '', 3000);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (10427, 28, -5135.6, -1018.55, 6.19, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (10427, 29, -5126.25, -1014.76, 1.08, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (10427, 30, -5120.03, -1013.12, -1.11, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (10427, 31, -5112.3, -1027.65, -5.39, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (10427, 32, -5106.99, -1023.09, -5.1, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (10427, 33, -5099.07, -1016.19, -4.92, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (10427, 34, -5091.23, -1009.21, -5.22, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (10427, 35, -5083.62, -1001.97, -5.22, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (10427, 36, -5076.1, -994.652, -4.92, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (10427, 37, -5066.71, -985.507, -4.97, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (10427, 38, -5065.13, -978.689, -5.02, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (10427, 39, -5062.33, -968.57, -5.08, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (10427, 40, -5059.46, -958.469, -5.16, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (10427, 41, -5056.59, -948.375, -5.1, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (10427, 42, -5053.73, -938.274, -5.69, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (10427, 43, -5043.06, -934.822, -5.35, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (10427, 44, -5029.35, -932.007, -5.27, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (10427, 45, -5024.58, -933.781, -5.4, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (10427, 46, -5019.13, -938.172, -5.54, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (10427, 47, -5011, -944.812, -5.47, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (10427, 48, -5002.86, -951.455, -5.44, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (10427, 49, -4994.73, -958.099, -5.41, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (10427, 50, -4990.57, -963.782, -5.33, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (10427, 51, -4987.43, -970.041, -5.17, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (10427, 52, -4981.41, -982.678, -5.1, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (10427, 53, -4977.03, -992.221, -4.97, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (10427, 54, -4972.6, -1001.74, -5.24, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (10427, 55, -4968.15, -1011.25, -5.49, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (10427, 56, -4963.75, -1020.78, -5.07, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (10427, 57, -4959.3, -1030.3, -5.28, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (10427, 58, -4956.28, -1036.61, -5.84, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (10427, 59, -4952.05, -1043.75, -9.04, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (10427, 60, -4943.8, -1055.06, -17.91, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (10427, 61, -4939.47, -1055.61, -20.73, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (10427, 62, -4929.16, -1053.64, -25.65, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (10427, 63, -4922.28, -1052.37, -28.72, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (10427, 64, -4913.26, -1051.3, -31.8, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (10427, 65, -4903.54, -1054.17, -33.48, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (10427, 66, -4905.95, -1056.71, -33.68, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (10427, 67, -4913.18, -1064.32, -39.46, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (10427, 68, -4925.27, -1076.98, -47.39, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (10427, 69, -4932.68, -1084.42, -51, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (10427, 70, -4938.3, -1100.41, -50.71, '', 5000);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (10427, 71, -4937.34, -1102.87, -49.82, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (16295, 29, 7571.16, -7659.12, 151.245, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (17969, 26, -449.65, 5463.78, 21.77, 'SAY_PROGRESS_6', 11000);

UPDATE `script_waypoint` SET location_x = 1109.15, location_y = -3104.11, location_z = 82.41, point_comment = '', waittime = 6000 WHERE entry = 3439 AND pointid = 0;
UPDATE `script_waypoint` SET location_x = 1105.39, location_y = -3102.86, location_z = 82.74, waittime = 2000 WHERE entry = 3439 AND pointid = 1;
UPDATE `script_waypoint` SET location_x = 1104.97, location_y = -3108.52, location_z = 83.1 WHERE entry = 3439 AND pointid = 2;
UPDATE `script_waypoint` SET location_x = 1110.01, location_y = -3110.48, location_z = 82.81 WHERE entry = 3439 AND pointid = 3;
UPDATE `script_waypoint` SET location_x = 1111.72, location_y = -3103.03, location_z = 82.21 WHERE entry = 3439 AND pointid = 4;
UPDATE `script_waypoint` SET location_x = 1106.98, location_y = -3099.44, location_z = 82.18 WHERE entry = 3439 AND pointid = 5;
UPDATE `script_waypoint` SET location_x = 1103.74, location_y = -3103.29, location_z = 83.05 WHERE entry = 3439 AND pointid = 6;
UPDATE `script_waypoint` SET location_x = 1112.55, location_y = -3106.56, location_z = 82.31, waittime = 1000 WHERE entry = 3439 AND pointid = 7;
UPDATE `script_waypoint` SET location_x = 1108.12, location_y = -3111.04, location_z = 82.99, waittime = 1000 WHERE entry = 3439 AND pointid = 8;
UPDATE `script_waypoint` SET location_x = 1109.32, location_y = -3100.39, location_z = 82.08, point_comment = '', waittime = 1000 WHERE entry = 3439 AND pointid = 9;
UPDATE `script_waypoint` SET location_x = 1109.32, location_y = -3100.39, location_z = 82.08, waittime = 6000 WHERE entry = 3439 AND pointid = 10;
UPDATE `script_waypoint` SET location_x = 1098.92, location_y = -3095.14, location_z = 82.97 WHERE entry = 3439 AND pointid = 11;
UPDATE `script_waypoint` SET location_x = 1100.94, location_y = -3082.6, location_z = 82.83 WHERE entry = 3439 AND pointid = 12;
UPDATE `script_waypoint` SET location_x = 1101.12, location_y = -3068.83, location_z = 82.53 WHERE entry = 3439 AND pointid = 13;
UPDATE `script_waypoint` SET location_x = 1096.97, location_y = -3051.99, location_z = 82.5 WHERE entry = 3439 AND pointid = 14;
UPDATE `script_waypoint` SET location_x = 1094.06, location_y = -3036.79, location_z = 82.7 WHERE entry = 3439 AND pointid = 15;
UPDATE `script_waypoint` SET location_x = 1098.22, location_y = -3027.84, location_z = 83.79 WHERE entry = 3439 AND pointid = 16;
UPDATE `script_waypoint` SET location_x = 1109.51, location_y = -3015.92, location_z = 85.73, point_comment = '', waittime = 0 WHERE entry = 3439 AND pointid = 17;
UPDATE `script_waypoint` SET location_x = 1119.87, location_y = -3007.21, location_z = 87.08, point_comment = '', waittime = 0 WHERE entry = 3439 AND pointid = 18;
UPDATE `script_waypoint` SET location_x = 1130.23, location_y = -3002.49, location_z = 91.27, waittime = 5000 WHERE entry = 3439 AND pointid = 19;
UPDATE `script_waypoint` SET location_x = 1130.23, location_y = -3002.49, location_z = 91.27, waittime = 3000 WHERE entry = 3439 AND pointid = 20;
UPDATE `script_waypoint` SET location_x = 1130.23, location_y = -3002.49, location_z = 91.27, waittime = 4000 WHERE entry = 3439 AND pointid = 21;
UPDATE `script_waypoint` SET location_x = 1129.73, location_y = -2985.89, location_z = 92.46 WHERE entry = 3439 AND pointid = 22;
UPDATE `script_waypoint` SET location_x = 1124.1, location_y = -2983.29, location_z = 92.81 WHERE entry = 3439 AND pointid = 23;
UPDATE `script_waypoint` SET location_x = 1111.74, location_y = -2992.38, location_z = 91.59, point_comment = '', waittime = 0 WHERE entry = 3439 AND pointid = 24;
UPDATE `script_waypoint` SET location_x = -5156.69, location_y = -1220.49, location_z = 48.78, waittime = 5000 WHERE entry = 10427 AND pointid = 0;
UPDATE `script_waypoint` SET location_x = -5157.12, location_y = -1220.13, location_z = 48.67 WHERE entry = 10427 AND pointid = 1;
UPDATE `script_waypoint` SET location_x = -5171.7, location_y = -1205.36, location_z = 47.43 WHERE entry = 10427 AND pointid = 2;
UPDATE `script_waypoint` SET location_x = -5174.08, location_y = -1197.7, location_z = 46.9 WHERE entry = 10427 AND pointid = 3;
UPDATE `script_waypoint` SET location_x = -5178.24, location_y = -1183.78, location_z = 45.97 WHERE entry = 10427 AND pointid = 4;
UPDATE `script_waypoint` SET location_x = -5181.52, location_y = -1171.03, location_z = 45.29, waittime = 0 WHERE entry = 10427 AND pointid = 5;
UPDATE `script_waypoint` SET location_x = -5184.29, location_y = -1159.21, location_z = 44.62 WHERE entry = 10427 AND pointid = 6;
UPDATE `script_waypoint` SET location_x = -5184.84, location_y = -1152.95, location_z = 44.84 WHERE entry = 10427 AND pointid = 7;
UPDATE `script_waypoint` SET location_x = -5182.04, location_y = -1142.83, location_z = 44.07 WHERE entry = 10427 AND pointid = 8;
UPDATE `script_waypoint` SET location_x = -5178.44, location_y = -1133.57, location_z = 43.91 WHERE entry = 10427 AND pointid = 9;
UPDATE `script_waypoint` SET location_x = -5176.68, location_y = -1129.48, location_z = 43.81 WHERE entry = 10427 AND pointid = 10;
UPDATE `script_waypoint` SET location_x = -5164.85, location_y = -1123.33, location_z = 43.99 WHERE entry = 10427 AND pointid = 11;
UPDATE `script_waypoint` SET location_x = -5153.07, location_y = -1117.7, location_z = 43.66 WHERE entry = 10427 AND pointid = 12;
UPDATE `script_waypoint` SET location_x = -5143.52, location_y = -1113.14, location_z = 43.78 WHERE entry = 10427 AND pointid = 13;
UPDATE `script_waypoint` SET location_x = -5135.86, location_y = -1104.42, location_z = 47.23 WHERE entry = 10427 AND pointid = 14;
UPDATE `script_waypoint` SET location_x = -5129.86, location_y = -1097.22, location_z = 49.52, point_comment = '', waittime = 0 WHERE entry = 10427 AND pointid = 15;
UPDATE `script_waypoint` SET location_x = -5127.48, location_y = -1087.29, location_z = 49.03 WHERE entry = 10427 AND pointid = 16;
UPDATE `script_waypoint` SET location_x = -5127.57, location_y = -1080.4, location_z = 46.64 WHERE entry = 10427 AND pointid = 17;
UPDATE `script_waypoint` SET location_x = -5129.65, location_y = -1077.58, location_z = 45.29 WHERE entry = 10427 AND pointid = 18;
UPDATE `script_waypoint` SET location_x = -5135.86, location_y = -1069.12, location_z = 39.53 WHERE entry = 10427 AND pointid = 19;
UPDATE `script_waypoint` SET location_x = -5141.97, location_y = -1060.58, location_z = 32.7 WHERE entry = 10427 AND pointid = 20;
UPDATE `script_waypoint` SET location_x = -5145.99, location_y = -1054.85, location_z = 28.98 WHERE entry = 10427 AND pointid = 21;
UPDATE `script_waypoint` SET location_x = -5147.08, location_y = -1050.35, location_z = 26.36 WHERE entry = 10427 AND pointid = 22;
UPDATE `script_waypoint` SET location_x = -5147.5, location_y = -1043.37, location_z = 21.84 WHERE entry = 10427 AND pointid = 23;
UPDATE `script_waypoint` SET location_x = -5147.68, location_y = -1036.37, location_z = 17.05 WHERE entry = 10427 AND pointid = 24;
UPDATE `script_waypoint` SET location_x = -5147.68, location_y = -1029.37, location_z = 14.59 WHERE entry = 10427 AND pointid = 25;
UPDATE `script_waypoint` SET location_x = -5144.62, location_y = -1023.9, location_z = 11.67, point_comment = '', waittime = 0 WHERE entry = 10427 AND pointid = 26;
UPDATE `script_waypoint` SET location_x = -5138.67, location_y = -1020.23, location_z = 7.81, point_comment = '' WHERE entry = 10427 AND pointid = 27;
UPDATE `script_waypoint` SET point_comment = '' WHERE entry = 16295 AND pointid = 11;
UPDATE `script_waypoint` SET point_comment = 'SAY_PROGRESS_2' WHERE entry = 16295 AND pointid = 12;
UPDATE `script_waypoint` SET point_comment = '' WHERE entry = 16295 AND pointid = 18;
UPDATE `script_waypoint` SET point_comment = 'SAY_PROGRESS_3' WHERE entry = 16295 AND pointid = 19;
UPDATE `script_waypoint` SET point_comment = '' WHERE entry = 16295 AND pointid = 30;
UPDATE `script_waypoint` SET point_comment = '' WHERE entry = 17876 AND pointid = 71;
UPDATE `script_waypoint` SET point_comment = 'mobs in doorway' WHERE entry = 17876 AND pointid = 72;
UPDATE `script_waypoint` SET location_x = -931.88, location_y = 5283.56, location_z = 23.98, point_comment = 'SAY_PROGRESS_1' WHERE entry = 17969 AND pointid = 0;
UPDATE `script_waypoint` SET location_x = -930.52, location_y = 5287.57, location_z = 23.82 WHERE entry = 17969 AND pointid = 1;
UPDATE `script_waypoint` SET location_x = -924.98, location_y = 5297.94, location_z = 17.78 WHERE entry = 17969 AND pointid = 2;
UPDATE `script_waypoint` SET location_x = -928.83, location_y = 5316.07, location_z = 18.18 WHERE entry = 17969 AND pointid = 3;
UPDATE `script_waypoint` SET location_x = -930.07, location_y = 5323.1, location_z = 18.63, point_comment = '' WHERE entry = 17969 AND pointid = 4;
UPDATE `script_waypoint` SET location_x = -926.58, location_y = 5331.24, location_z = 18.74, point_comment = 'SAY_PROGRESS_2', waittime = 6000 WHERE entry = 17969 AND pointid = 5;
UPDATE `script_waypoint` SET location_x = -926.58, location_y = 5331.24, location_z = 18.74, point_comment = 'SAY_PROGRESS_3', waittime = 3000 WHERE entry = 17969 AND pointid = 6;
UPDATE `script_waypoint` SET location_x = -931.24, location_y = 5358.89, location_z = 18.14 WHERE entry = 17969 AND pointid = 7;
UPDATE `script_waypoint` SET location_x = -934.43, location_y = 5370.2, location_z = 22.41 WHERE entry = 17969 AND pointid = 8;
UPDATE `script_waypoint` SET location_x = -943.01, location_y = 5400.55, location_z = 22.74 WHERE entry = 17969 AND pointid = 9;
UPDATE `script_waypoint` SET location_x = -929.62, location_y = 5417.98, location_z = 23.07, point_comment = '' WHERE entry = 17969 AND pointid = 10;
UPDATE `script_waypoint` SET location_x = -901.92, location_y = 5420.38, location_z = 24.19 WHERE entry = 17969 AND pointid = 11;
UPDATE `script_waypoint` SET location_x = -859.03, location_y = 5415.36, location_z = 23.64 WHERE entry = 17969 AND pointid = 12;
UPDATE `script_waypoint` SET location_x = -808.94, location_y = 5401.93, location_z = 23.11 WHERE entry = 17969 AND pointid = 13;
UPDATE `script_waypoint` SET location_x = -772.74, location_y = 5390.53, location_z = 22.97 WHERE entry = 17969 AND pointid = 14;
UPDATE `script_waypoint` SET location_x = -749.71, location_y = 5385.39, location_z = 22.75 WHERE entry = 17969 AND pointid = 15;
UPDATE `script_waypoint` SET location_x = -721.23, location_y = 5380.38, location_z = 22.47, point_comment = '', waittime = 0 WHERE entry = 17969 AND pointid = 16;
UPDATE `script_waypoint` SET location_x = -687.96, location_y = 5379.08, location_z = 22.16, point_comment = '' WHERE entry = 17969 AND pointid = 17;
UPDATE `script_waypoint` SET location_x = -680.87, location_y = 5378.95, location_z = 21.99, point_comment = 'SAY_PROGRESS_4', waittime = 6000 WHERE entry = 17969 AND pointid = 18;
UPDATE `script_waypoint` SET location_x = -680.87, location_y = 5378.95, location_z = 21.99, point_comment = 'SAY_PROGRESS_5', waittime = 6000 WHERE entry = 17969 AND pointid = 19;
UPDATE `script_waypoint` SET location_x = -636.14, location_y = 5385.25, location_z = 22.2 WHERE entry = 17969 AND pointid = 20;
UPDATE `script_waypoint` SET location_x = -602.94, location_y = 5411.36, location_z = 21.48 WHERE entry = 17969 AND pointid = 21;
UPDATE `script_waypoint` SET location_x = -566.86, location_y = 5421.87, location_z = 21.25 WHERE entry = 17969 AND pointid = 22;
UPDATE `script_waypoint` SET location_x = -547.27, location_y = 5427.87, location_z = 21.1 WHERE entry = 17969 AND pointid = 23;
UPDATE `script_waypoint` SET location_x = -520.59, location_y = 5444.83, location_z = 21.15 WHERE entry = 17969 AND pointid = 24;
UPDATE `script_waypoint` SET location_x = -488.45, location_y = 5447.83, location_z = 22.38, point_comment = '', waittime = 0 WHERE entry = 17969 AND pointid = 25;

DELETE FROM `script_waypoint` WHERE entry = 16295 AND pointid = 39;

REPLACE INTO `spell_linked_spell` (spell_effect, spell_trigger, type, comment)
VALUES (48075, 48077, 0, 'Holy Nova (rank8)');
REPLACE INTO `spell_linked_spell` (spell_effect, spell_trigger, type, comment)
VALUES (48076, 48078, 0, 'Holy Nova (rank9)');
REPLACE INTO `spell_linked_spell` (spell_effect, spell_trigger, type, comment)
VALUES (49009, -49011, 0, 'Wyvern Sting');
REPLACE INTO `spell_linked_spell` (spell_effect, spell_trigger, type, comment)
VALUES (49010, -49012, 0, 'Wyvern Sting');

DELETE FROM `spell_linked_spell` WHERE spell_effect = -37565 AND spell_trigger = 2060 AND type = 0;
DELETE FROM `spell_linked_spell` WHERE spell_effect = -37565 AND spell_trigger = 10963 AND type = 0;
DELETE FROM `spell_linked_spell` WHERE spell_effect = -37565 AND spell_trigger = 10964 AND type = 0;
DELETE FROM `spell_linked_spell` WHERE spell_effect = -37565 AND spell_trigger = 10965 AND type = 0;
DELETE FROM `spell_linked_spell` WHERE spell_effect = -37565 AND spell_trigger = 25210 AND type = 0;
DELETE FROM `spell_linked_spell` WHERE spell_effect = -37565 AND spell_trigger = 25213 AND type = 0;
DELETE FROM `spell_linked_spell` WHERE spell_effect = -37565 AND spell_trigger = 25314 AND type = 0;
DELETE FROM `spell_linked_spell` WHERE spell_effect = -17743 AND spell_trigger = 19938 AND type = 1;
DELETE FROM `spell_linked_spell` WHERE spell_effect = 38729 AND spell_trigger = 38736 AND type = 0;

UPDATE `spell_proc_event` SET SpellFamilyMask = 38658768896 WHERE entry = 14156;
UPDATE `spell_proc_event` SET SpellFamilyMask = 38658768896 WHERE entry = 14160;
UPDATE `spell_proc_event` SET SpellFamilyMask = 38658768896 WHERE entry = 14161;

REPLACE INTO `spell_script_target` (entry, targetEntry, type)
VALUES (12134, 5256, 2);
REPLACE INTO `spell_script_target` (entry, targetEntry, type)
VALUES (12134, 5259, 2);
REPLACE INTO `spell_script_target` (entry, targetEntry, type)
VALUES (12134, 5270, 2);
REPLACE INTO `spell_script_target` (entry, targetEntry, type)
VALUES (12134, 5271, 2);
REPLACE INTO `spell_script_target` (entry, targetEntry, type)
VALUES (12134, 5273, 2);
REPLACE INTO `spell_script_target` (entry, targetEntry, type)
VALUES (12134, 8313, 2);
REPLACE INTO `spell_script_target` (entry, targetEntry, type)
VALUES (38968, 38968, 1);
