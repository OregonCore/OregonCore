/*
SQLyog Community v12.12 (64 bit)
MySQL - 5.6.26-log 
*********************************************************************
*/
/*!40101 SET NAMES utf8 */;

create table `creature_text` (
	`entry` mediumint (8),
	`groupid` tinyint (3),
	`id` tinyint (3),
	`text` text ,
	`type` tinyint (3),
	`language` tinyint (3),
	`probability` float ,
	`emote` mediumint (8),
	`duration` mediumint (8),
	`sound` mediumint (8),
	`BroadcastTextId` mediumint (6),
	`TextRange` tinyint (3),
	`comment` varchar (765)
); 
