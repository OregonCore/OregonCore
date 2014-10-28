DROP TABLE creature_groups;
DROP TABLE creature_group_data;
DROP TABLE creature_formation_data;
DROP TABLE creature_formations;

CREATE TABLE `creature_formations` (
  `leader` INT(11) UNSIGNED NOT NULL,
  `follower` INT(11) UNSIGNED NOT NULL,
  `dist` FLOAT UNSIGNED NOT NULL,
  `angle` FLOAT UNSIGNED NOT NULL,
  `groupAI` INT(11) UNSIGNED NOT NULL,
  `point_1` MEDIUMINT(8) UNSIGNED NOT NULL DEFAULT 0,
  `point_2` MEDIUMINT(8) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY  (`follower`)
) ENGINE=INNODB DEFAULT CHARSET=latin1;
