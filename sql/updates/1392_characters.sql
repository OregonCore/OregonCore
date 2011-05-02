-- Add new fields
ALTER TABLE `item_instance`
 ADD `itemEntry` int(10) unsigned NOT NULL default '0' AFTER `owner_guid`,
 ADD `creatorGuid` int(10) unsigned NOT NULL default '0' AFTER `itemEntry`,
 ADD `giftCreatorGuid` int(10) unsigned NOT NULL default '0' AFTER `creatorGuid`,
 ADD `count` int(10) unsigned NOT NULL default '1' AFTER `giftCreatorGuid`,
 ADD `duration` int(10) unsigned NOT NULL AFTER `count`,
 ADD `charges` text NOT NULL AFTER `duration`,
 ADD `flags` int(10) unsigned NOT NULL default '0' AFTER `charges`,
 ADD `enchantments` text NOT NULL AFTER `flags`,
 ADD `randomPropertyId` int(11) NOT NULL default '0' AFTER `enchantments`,
 ADD `durability` int(10) unsigned NOT NULL default '0' AFTER `randomPropertyId`,
 ADD `textId` int(10) unsigned NOT NULL default '0' AFTER `durability`;

-- Temporarily change delimiter to prevent SQL syntax errors
DELIMITER ||

+-- Function to convert ints from unsigned to signed
DROP FUNCTION IF EXISTS `uint32toint32`||
CREATE FUNCTION `uint32toint32`(input INT(10) UNSIGNED) RETURNS INT(11) SIGNED DETERMINISTIC
BEGIN
  RETURN input;
END||

-- Restore original delimiter
DELIMITER ;

-- Move data to new fields
UPDATE `item_instance` SET
`itemEntry` = SUBSTRING(`data`,
length(SUBSTRING_INDEX(`data`,' ',3))+2,
length(SUBSTRING_INDEX(`data`,' ',3+1))-length(SUBSTRING_INDEX(data,' ',3))-1),

`creatorGuid` = SUBSTRING(`data`,
length(SUBSTRING_INDEX(`data`,' ',10))+2,
length(SUBSTRING_INDEX(`data`,' ',10+1))-length(SUBSTRING_INDEX(data,' ',10))-1),

`giftCreatorGuid` = SUBSTRING(`data`,
length(SUBSTRING_INDEX(`data`,' ',12))+2,
length(SUBSTRING_INDEX(`data`,' ',12+1))-length(SUBSTRING_INDEX(data,' ',12))-1),

`count` = SUBSTRING(`data`,
length(SUBSTRING_INDEX(`data`,' ',14))+2,
length(SUBSTRING_INDEX(`data`,' ',14+1))-length(SUBSTRING_INDEX(data,' ',14))-1),

`duration` = SUBSTRING(`data`,
length(SUBSTRING_INDEX(`data`,' ',15))+2,
length(SUBSTRING_INDEX(`data`,' ',15+1))-length(SUBSTRING_INDEX(data,' ',15))-1),

`charges` = CONCAT_WS(' ',
uint32toint32(SUBSTRING(`data`,
length(SUBSTRING_INDEX(`data`,' ',16))+2,
length(SUBSTRING_INDEX(`data`,' ',16+1))-length(SUBSTRING_INDEX(data,' ',16))-1)),
uint32toint32(SUBSTRING(`data`,
length(SUBSTRING_INDEX(`data`,' ',17))+2,
length(SUBSTRING_INDEX(`data`,' ',17+1))-length(SUBSTRING_INDEX(data,' ',17))-1)),
uint32toint32(SUBSTRING(`data`,
length(SUBSTRING_INDEX(`data`,' ',18))+2,
length(SUBSTRING_INDEX(`data`,' ',18+1))-length(SUBSTRING_INDEX(data,' ',18))-1)),
uint32toint32(SUBSTRING(`data`,
length(SUBSTRING_INDEX(`data`,' ',19))+2,
length(SUBSTRING_INDEX(`data`,' ',19+1))-length(SUBSTRING_INDEX(data,' ',19))-1)),
uint32toint32(SUBSTRING(`data`,
length(SUBSTRING_INDEX(`data`,' ',20))+2,
length(SUBSTRING_INDEX(`data`,' ',20+1))-length(SUBSTRING_INDEX(data,' ',20))-1))),

`flags` = SUBSTRING(`data`,
length(SUBSTRING_INDEX(`data`,' ',21))+2,
length(SUBSTRING_INDEX(`data`,' ',21+1))-length(SUBSTRING_INDEX(data,' ',21))-1),

`enchantments` = SUBSTRING(`data`,
length(SUBSTRING_INDEX(`data`,' ',22))+2,
length(SUBSTRING_INDEX(`data`,' ',54+1))-length(SUBSTRING_INDEX(data,' ',22))-1),

`randomPropertyId` = uint32toint32(SUBSTRING(`data`,
length(SUBSTRING_INDEX(`data`,' ',56))+2,
length(SUBSTRING_INDEX(`data`,' ',56+1))-length(SUBSTRING_INDEX(data,' ',56))-1)),

`durability` = SUBSTRING(`data`,
length(SUBSTRING_INDEX(`data`,' ',58))+2,
length(SUBSTRING_INDEX(`data`,' ',58+1))-length(SUBSTRING_INDEX(data,' ',58))-1),

`textId` = SUBSTRING(`data`,
length(SUBSTRING_INDEX(`data`,' ',57))+2,
length(SUBSTRING_INDEX(`data`,' ',57+1))-length(SUBSTRING_INDEX(data,' ',57))-1);

-- Drop function
DROP FUNCTION IF EXISTS `uint32toint32`;

-- Drop old field
ALTER TABLE `item_instance` DROP `data`;
ALTER TABLE `guild_bank_item` DROP COLUMN `item_entry`;
ALTER TABLE `mail_items` DROP COLUMN `item_template`;
ALTER TABLE `auctionhouse` DROP COLUMN `item_template`;
ALTER TABLE `character_inventory` DROP COLUMN `item_template`;