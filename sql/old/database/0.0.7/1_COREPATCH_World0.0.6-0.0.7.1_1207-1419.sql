-- COREPATCH database updatepack (rev. 1207 -> 1419)

UPDATE command 
SET help = 'Syntax: .account set gmlevel [$account] #level [#realmid]\r\n\r\nSet the security level for targeted player (can\'t be used at self) or for account $name to a level of #level on the realm #realmID.\r\n\r\n#level may range from 0 to 3.\r\n\r\n#reamID may be -1 for all realms.'
WHERE name = 'account set gmlevel';

DELETE FROM `oregon_string` WHERE `entry` = 11001;
INSERT INTO oregon_string VALUES 
(11001, 'You have not chosen -1 or the current realmID that you are on.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);


-- Lunar Fireworks
DELETE FROM `spell_linked_spell` WHERE `spell_effect` IN (44430,44429);
INSERT INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `comment`) VALUES
(26286,44430,0,'Small Red Rocket - questcredit'),
(26292,44430,0,'Small Green Rocket - questcredit'),
(26291,44430,0,'Small Blue Rocket - questcredit'),
(26327,44429,0,'Red Firework Cluster - questcredit'),
(26325,44429,0,'Green Firework Cluster - questcredit'),
(26304,44429,0,'Blue Firework Cluster - questcredit');

DELETE FROM `spell_linked_spell` WHERE spell_trigger = 40214;
INSERT INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `comment`) VALUES
( 40214, 40216, 2, 'Dragonmaw Illusion'),
( 40214, 42016, 2, 'Dragonmaw Illusion');

ALTER TABLE `creature_template`
    ADD COLUMN `KillCredit1` int(11) unsigned NOT NULL default '0' AFTER `heroic_entry`,
    ADD COLUMN `KillCredit2` int(11) unsigned NOT NULL default '0' AFTER `KillCredit1`;

	
UPDATE `creature_template` SET `ScriptName`='npc_eitrigg' WHERE `entry`='3144';


DELETE FROM `oregon_string` WHERE `entry`=1200;
INSERT INTO `oregon_string` VALUES
(1200,'You try to view cinemitic %u but it doesn\'t exist.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);

DELETE FROM `command` WHERE `name` IN ('debug playsound','debug play sound','debug play cinematic');
INSERT INTO `command` VALUES
('debug play cinematic',1,'Syntax: .debug play cinematic #cinematicid\r\n\r\nPlay cinematic #cinematicid for you. You stay at place while your mind fly.\r\n'),
('debug play sound',1,'Syntax: .debug play sound #soundid\r\n\r\nPlay sound with #soundid.\r\nSound will be play only for you. Other players do not hear this.\r\nWarning: client may have more 5000 sounds...');

UPDATE `oregon_string` SET `content_default` = 'Player%s %s (guid: %u) Account: %s (id: %u) Email: %s GMLevel: %u Last IP: %s Last login: %s Latency: %ums' WHERE `entry` = 548;
UPDATE `oregon_string` SET `content_default` = 'Race: %s Class: %s Played time: %s Level: %u Money: %ug%us%uc' WHERE `entry` = 549;

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

ALTER TABLE `creature_addon` ADD COLUMN `isActive` INT(1) DEFAULT '0' NULL AFTER `moveflags`; 
ALTER TABLE `creature_template_addon` ADD COLUMN `isActive` INT(1) DEFAULT '0' NULL AFTER `moveflags`;

-- EOF