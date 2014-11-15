DELETE FROM `command` WHERE `name`= 'npc setdeathstate';
-- Drop DeathState column
ALTER TABLE `creature` DROP COLUMN `DeathState`;