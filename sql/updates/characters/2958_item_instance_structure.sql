-- Add new fields
ALTER TABLE `item_instance`
 ADD `itemEntry` MEDIUMINT(8) UNSIGNED NOT NULL DEFAULT '0' AFTER `owner_guid`,
 ADD `creatorGuid` INT(10) UNSIGNED NOT NULL DEFAULT '0' AFTER `itemEntry`,
 ADD `giftCreatorGuid` INT(10) UNSIGNED NOT NULL DEFAULT '0' AFTER `creatorGuid`,
 ADD `count` INT(10) UNSIGNED NOT NULL DEFAULT '1' AFTER `giftCreatorGuid`,
 ADD `duration` INT(10) UNSIGNED NOT NULL AFTER `count`,
 ADD `charges` TEXT NOT NULL AFTER `duration`,
 ADD `flags` INT(10) UNSIGNED NOT NULL DEFAULT '0' AFTER `charges`,
 ADD `enchantments` TEXT NOT NULL AFTER `flags`,
 ADD `randomPropertyId` INT(11) NOT NULL DEFAULT '0' AFTER `enchantments`,
 ADD `durability` INT(10) UNSIGNED NOT NULL DEFAULT '0' AFTER `randomPropertyId`,
 ADD `itemTextId` MEDIUMINT(8) UNSIGNED NOT NULL DEFAULT '0' AFTER `durability`;

-- Function to convert ints from unsigned to signed
DROP FUNCTION IF EXISTS `uint32toint32`;
DELIMITER //
CREATE FUNCTION `uint32toint32`(i INT(10) UNSIGNED) RETURNS INT(11) SIGNED DETERMINISTIC
BEGIN
  RETURN i;
END//
DELIMITER ;

-- Move data to new fields
UPDATE `item_instance` SET
`itemEntry` = SUBSTRING(`data`,
LENGTH(SUBSTRING_INDEX(`data`,' ',3))+2,
LENGTH(SUBSTRING_INDEX(`data`,' ',3+1))-LENGTH(SUBSTRING_INDEX(DATA,' ',3))-1),

`creatorGuid` = SUBSTRING(`data`,
LENGTH(SUBSTRING_INDEX(`data`,' ',10))+2,
LENGTH(SUBSTRING_INDEX(`data`,' ',10+1))-LENGTH(SUBSTRING_INDEX(DATA,' ',10))-1),

`giftCreatorGuid` = SUBSTRING(`data`,
LENGTH(SUBSTRING_INDEX(`data`,' ',12))+2,
LENGTH(SUBSTRING_INDEX(`data`,' ',12+1))-LENGTH(SUBSTRING_INDEX(DATA,' ',12))-1),

`count` = SUBSTRING(`data`,
LENGTH(SUBSTRING_INDEX(`data`,' ',14))+2,
LENGTH(SUBSTRING_INDEX(`data`,' ',14+1))-LENGTH(SUBSTRING_INDEX(DATA,' ',14))-1),

`duration` = SUBSTRING(`data`,
LENGTH(SUBSTRING_INDEX(`data`,' ',15))+2,
LENGTH(SUBSTRING_INDEX(`data`,' ',15+1))-LENGTH(SUBSTRING_INDEX(DATA,' ',15))-1),

`charges` = CONCAT_WS(' ',
uint32toint32(SUBSTRING(`data`,
LENGTH(SUBSTRING_INDEX(`data`,' ',16))+2,
LENGTH(SUBSTRING_INDEX(`data`,' ',16+1))-LENGTH(SUBSTRING_INDEX(DATA,' ',16))-1)),
uint32toint32(SUBSTRING(`data`,
LENGTH(SUBSTRING_INDEX(`data`,' ',17))+2,
LENGTH(SUBSTRING_INDEX(`data`,' ',17+1))-LENGTH(SUBSTRING_INDEX(DATA,' ',17))-1)),
uint32toint32(SUBSTRING(`data`,
LENGTH(SUBSTRING_INDEX(`data`,' ',18))+2,
LENGTH(SUBSTRING_INDEX(`data`,' ',18+1))-LENGTH(SUBSTRING_INDEX(DATA,' ',18))-1)),
uint32toint32(SUBSTRING(`data`,
LENGTH(SUBSTRING_INDEX(`data`,' ',19))+2,
LENGTH(SUBSTRING_INDEX(`data`,' ',19+1))-LENGTH(SUBSTRING_INDEX(DATA,' ',19))-1)),
uint32toint32(SUBSTRING(`data`,
LENGTH(SUBSTRING_INDEX(`data`,' ',20))+2,
LENGTH(SUBSTRING_INDEX(`data`,' ',20+1))-LENGTH(SUBSTRING_INDEX(DATA,' ',20))-1))),

`flags` = SUBSTRING(`data`,
LENGTH(SUBSTRING_INDEX(`data`,' ',21))+2,
LENGTH(SUBSTRING_INDEX(`data`,' ',21+1))-LENGTH(SUBSTRING_INDEX(DATA,' ',21))-1),

`enchantments` = SUBSTRING(`data`,
LENGTH(SUBSTRING_INDEX(`data`,' ',22))+2,
LENGTH(SUBSTRING_INDEX(`data`,' ',54+1))-LENGTH(SUBSTRING_INDEX(DATA,' ',22))-1),

`randomPropertyId` = uint32toint32(SUBSTRING(`data`,
LENGTH(SUBSTRING_INDEX(`data`,' ',56))+2,
LENGTH(SUBSTRING_INDEX(`data`,' ',56+1))-LENGTH(SUBSTRING_INDEX(DATA,' ',56))-1)),

`itemTextId` = uint32toint32(SUBSTRING(`data`,
LENGTH(SUBSTRING_INDEX(`data`,' ',57))+2,
LENGTH(SUBSTRING_INDEX(`data`,' ',57+1))-LENGTH(SUBSTRING_INDEX(DATA,' ',57))-1)),

`durability` = SUBSTRING(`data`,
LENGTH(SUBSTRING_INDEX(`data`,' ',58))+2,
LENGTH(SUBSTRING_INDEX(`data`,' ',58+1))-LENGTH(SUBSTRING_INDEX(DATA,' ',58))-1);

-- Drop function
DROP FUNCTION IF EXISTS `uint32toint32`;

-- Fix heroic item flag
/* check if this is needed!!
UPDATE `item_instance` SET `flags`=`flags`&~0x8 WHERE
SUBSTRING(`data`,
length(SUBSTRING_INDEX(`data`,' ',3))+2,
length(SUBSTRING_INDEX(`data`,' ',3+1))-length(SUBSTRING_INDEX(data,' ',3))-1)
NOT IN (5043,5044,17302,17305,17308,21831);
*/

-- Drop old field
ALTER TABLE `item_instance` DROP `data`;