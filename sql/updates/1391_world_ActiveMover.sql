ALTER TABLE `creature_addon` ADD COLUMN `isActive` INT(1) DEFAULT '0' NULL AFTER `moveflags`; 
ALTER TABLE `creature_template_addon` ADD COLUMN `isActive` INT(1) DEFAULT '0' NULL AFTER `moveflags`;