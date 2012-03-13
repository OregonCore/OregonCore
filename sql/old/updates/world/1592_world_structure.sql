ALTER TABLE `game_event_gameobject` ADD INDEX `idx_guid` (`guid`);
ALTER TABLE `pool_gameobject` ADD INDEX `idx_guid` (`guid`);
ALTER TABLE `game_event_creature` ADD INDEX `idx_guid` (`guid`);
ALTER TABLE `pool_creature` ADD INDEX `idx_guid` (`guid`);