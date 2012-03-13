DROP TABLE IF EXISTS `script_db_version`;
DROP TABLE IF EXISTS `mail_external`;
DROP TABLE IF EXISTS `gm_surveys`;
DROP TABLE IF EXISTS `db_version`;

ALTER TABLE `areatrigger_teleport` DROP INDEX `name`;

ALTER TABLE `creature_formation_data` CONVERT TO CHARACTER SET utf8;
ALTER TABLE `creature_formation_data` ROW_FORMAT = FIXED;

ALTER TABLE `creature_formations` ROW_FORMAT = FIXED;
ALTER TABLE `creature_formations` CONVERT TO CHARACTER SET utf8;

ALTER TABLE `creature_groups` COMMENT = 'Creature Formation System';
ALTER TABLE `creature_group_data` COMMENT = 'Creature Formation System';

ALTER TABLE `spell_enchant_proc_data` ROW_FORMAT = FIXED;

ALTER TABLE `warden_data_result` ENGINE = MYISAM;
ALTER TABLE `warden_data_result` ROW_FORMAT = DYNAMIC;