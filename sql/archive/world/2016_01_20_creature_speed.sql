ALTER TABLE creature_template CHANGE `speed` `speed_walk` FLOAT NOT NULL DEFAULT '1';
ALTER TABLE creature_template ADD COLUMN `speed_run` FLOAT NOT NULL DEFAULT '1.14286' AFTER `speed_walk`;