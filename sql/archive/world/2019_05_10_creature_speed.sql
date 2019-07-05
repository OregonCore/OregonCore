ALTER TABLE creature_model_info ADD COLUMN `SpeedWalk` FLOAT NOT NULL DEFAULT '1' COMMENT 'Default walking speed for any creature with model' AFTER `CombatReach`;
ALTER TABLE creature_model_info ADD COLUMN `SpeedRun` FLOAT NOT NULL DEFAULT '1.14286' COMMENT 'Default running speed for any creature with model' AFTER `SpeedWalk`;

ALTER TABLE creature_template MODIFY COLUMN `speed_walk` FLOAT NOT NULL DEFAULT '0';
ALTER TABLE creature_template MODIFY COLUMN `speed_run` FLOAT NOT NULL DEFAULT '0';