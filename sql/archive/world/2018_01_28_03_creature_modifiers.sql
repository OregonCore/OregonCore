ALTER TABLE `creature_template` DROP `HealthModifier`;
ALTER TABLE `creature_template` DROP `ManaModifier`;
ALTER TABLE creature_template ADD COLUMN `HealthModifier` FLOAT NOT NULL DEFAULT  '1' AFTER `InhabitType`;
ALTER TABLE creature_template ADD COLUMN `ManaModifier` FLOAT NOT NULL DEFAULT  '1' AFTER `HealthModifier`;
ALTER TABLE creature_template ADD COLUMN `ArmorModifier` FLOAT NOT NULL DEFAULT  '1' AFTER `ManaModifier`;


-- Formel to get Health Mod
UPDATE creature_template ct 
JOIN creature_classlevelstats cls ON ct.minlevel = cls.level AND ct.unit_class = cls.class
SET ct.HealthModifier = (ct.minhealth / cls.basehp0) WHERE ct.exp = "0";

UPDATE creature_template ct 
JOIN creature_classlevelstats cls ON ct.minlevel = cls.level AND ct.unit_class = cls.class
SET ct.HealthModifier = (ct.minhealth / cls.basehp1) WHERE ct.exp = "1";

-- Formel to get Armor Mod
UPDATE creature_template ct 
JOIN creature_classlevelstats cls ON ct.minlevel = cls.level AND ct.unit_class = cls.class
SET ct.ArmorModifier = (ct.armor / cls.basearmor) WHERE ct.exp = "0";

UPDATE creature_template ct 
JOIN creature_classlevelstats cls ON ct.minlevel = cls.level AND ct.unit_class = cls.class
SET ct.ArmorModifier = (ct.armor / cls.basearmor) WHERE ct.exp = "1";

-- Formel to get Mana Mod
UPDATE creature_template ct 
JOIN creature_classlevelstats cls ON ct.minlevel = cls.level AND ct.unit_class = cls.class
SET ct.ManaModifier = (IFNULL(ct.minmana / cls.basemana, 1)) WHERE ct.exp = "0";

UPDATE creature_template ct 
JOIN creature_classlevelstats cls ON ct.minlevel = cls.level AND ct.unit_class = cls.class
SET ct.ManaModifier = (IFNULL(ct.minmana / cls.basemana, 1)) WHERE ct.exp = "1";