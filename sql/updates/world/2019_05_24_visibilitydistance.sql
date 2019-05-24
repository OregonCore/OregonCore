ALTER TABLE `creature_addon`
	ADD COLUMN `visibilityDistanceType` TINYINT UNSIGNED NOT NULL DEFAULT 0 AFTER `emote`;

ALTER TABLE `creature_template_addon`
	ADD COLUMN `visibilityDistanceType` TINYINT UNSIGNED NOT NULL DEFAULT 0 AFTER `emote`;