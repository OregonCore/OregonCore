/*
SQLyog Community v12.12 (64 bit)
MySQL - 5.6.26-log 
*********************************************************************
*/
/*!40101 SET NAMES utf8 */;

create table `creature_summon_groups` (
	`summonerId` mediumint (8),
	`summonerType` tinyint (3),
	`groupId` tinyint (3),
	`entry` mediumint (8),
	`position_x` float ,
	`position_y` float ,
	`position_z` float ,
	`orientation` float ,
	`summonType` tinyint (3),
	`summonTime` int (10)
); 
