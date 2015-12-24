CREATE TABLE `creature_text` (
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
	`TextRange` tinyint (3),
	`comment` varchar (765)
); 