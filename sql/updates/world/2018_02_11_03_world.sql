ALTER TABLE `creature` ADD `zoneId` SMALLINT(5) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Zone Identifier (unused)' AFTER `map`;
ALTER TABLE `creature` ADD `areaId` SMALLINT(5) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Area Identifier (unused)' AFTER `zoneId`;

ALTER TABLE `gameobject` ADD `zoneId` SMALLINT(5) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Zone Identifier (unused)' AFTER `map`;
ALTER TABLE `gameobject` ADD `areaId` SMALLINT(5) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Area Identifier (unused)' AFTER `zoneId`;
