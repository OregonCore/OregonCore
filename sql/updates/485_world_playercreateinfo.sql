ALTER TABLE `playercreateinfo` ADD COLUMN `orientation` float NOT NULL DEFAULT 0 AFTER `position_z`;

UPDATE playercreateinfo SET orientation = 5.696318 WHERE race=4;
UPDATE playercreateinfo SET orientation = 6.177156 WHERE race=3;
UPDATE playercreateinfo SET orientation = 2.70526 WHERE race=5;
UPDATE playercreateinfo SET orientation = 5.316046 WHERE race=10;
UPDATE playercreateinfo SET orientation = 2.083644 WHERE race=11;