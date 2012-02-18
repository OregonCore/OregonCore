ALTER TABLE `game_event_gameobject` DROP INDEX `idx_guid`;
ALTER TABLE `game_event_gameobject` ADD INDEX `idx_guid` (`guid`);

ALTER TABLE `pool_gameobject` DROP INDEX `idx_guid`;
ALTER TABLE `pool_gameobject` ADD INDEX `idx_guid` (`guid`);

ALTER TABLE `game_event_creature` DROP INDEX `idx_guid`;
ALTER TABLE `game_event_creature` ADD INDEX `idx_guid` (`guid`);

ALTER TABLE `pool_creature` DROP INDEX `idx_guid`;
ALTER TABLE `pool_creature` ADD INDEX `idx_guid` (`guid`);
