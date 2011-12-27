DELETE FROM `areatrigger_scripts` WHERE `entry`=4524;
INSERT INTO `areatrigger_scripts` VALUES
       (4524,'at_shattered_halls');

UPDATE `creature_template` SET `npcflag` = '0' WHERE `entry` = '17296';
UPDATE `creature_template` SET `npcflag` = '0' WHERE `entry` = '17290';