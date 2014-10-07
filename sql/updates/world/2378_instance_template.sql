-- Handle instance mounting in the database rather than in core
ALTER TABLE `instance_template` ADD COLUMN `allowMount` tinyint(1) NOT NULL DEFAULT '0';

-- Apply enables to instances hardcoded in core
UPDATE instance_template SET allowMount = 1 WHERE map IN (209, 269, 309, 509, 534, 560, 568, 580);