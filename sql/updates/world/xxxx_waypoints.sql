/*
SQLyog Community v12.12 (64 bit)
MySQL - 5.6.26-log 
*********************************************************************
*/
/*!40101 SET NAMES utf8 */;

create table `waypoints` (
	`entry` mediumint (8),
	`pointid` mediumint (8),
	`position_x` float ,
	`position_y` float ,
	`position_z` float ,
	`point_comment` text 
); 
