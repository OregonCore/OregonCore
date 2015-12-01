/*
SQLyog Community v12.12 (64 bit)
MySQL - 5.6.26-log 
*********************************************************************
*/
/*!40101 SET NAMES utf8 */;

create table `broadcast_text` (
	`ID` mediumint (8),
	`Language` mediumint (8),
	`MaleText` text ,
	`FemaleText` text ,
	`EmoteID0` mediumint (8),
	`EmoteID1` mediumint (8),
	`EmoteID2` mediumint (8),
	`EmoteDelay0` mediumint (8),
	`EmoteDelay1` mediumint (8),
	`EmoteDelay2` mediumint (8),
	`SoundId` mediumint (8),
	`Unk1` mediumint (8),
	`Unk2` mediumint (8),
	`VerifiedBuild` smallint (5)
); 
insert into `broadcast_text` (`ID`, `Language`, `MaleText`, `FemaleText`, `EmoteID0`, `EmoteID1`, `EmoteID2`, `EmoteDelay0`, `EmoteDelay1`, `EmoteDelay2`, `SoundId`, `Unk1`, `Unk2`, `VerifiedBuild`) values('1150','0','%s attempts to run away in fear!','%s attempts to run away in fear!','0','0','0','0','0','0','0','0','1','18019');
insert into `broadcast_text` (`ID`, `Language`, `MaleText`, `FemaleText`, `EmoteID0`, `EmoteID1`, `EmoteID2`, `EmoteDelay0`, `EmoteDelay1`, `EmoteDelay2`, `SoundId`, `Unk1`, `Unk2`, `VerifiedBuild`) values('2541','0','%s calls for help!','%s calls for help!','0','0','0','0','0','0','0','0','1','18019');
