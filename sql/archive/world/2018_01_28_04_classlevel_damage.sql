ALTER TABLE creature_template CHANGE dmg_multiplier DamageModifier FLOAT NOT NULL DEFAULT 1 AFTER ArmorModifier;
ALTER TABLE creature_template CHANGE baseattacktime BaseAttackTime int(10) NOT NULL DEFAULT 0;
ALTER TABLE creature_template CHANGE rangeattacktime RangeAttackTime int(10) NOT NULL DEFAULT 0;
ALTER TABLE creature_template ADD BaseVariance FLOAT NOT NULL DEFAULT 1 AFTER RangeAttackTime;
ALTER TABLE creature_template ADD RangeVariance FLOAT NOT NULL DEFAULT 1 AFTER BaseVariance;

-- Formel to get Damage Mod
UPDATE creature_template SET mindmg = ROUND(`mindmg`+ `attackpower` / 7);
UPDATE creature_template SET maxdmg = ROUND(`maxdmg`+ `attackpower` / 7);

UPDATE creature_template ct 
JOIN creature_classlevelstats cls ON ct.minlevel = cls.level AND ct.unit_class = cls.class
SET ct.DamageModifier = (IFNULL(ct.mindmg / ((cls.basedamage_exp0 + (cls.attackpower / 14)) * (ct.BaseAttackTime / 1000)), 1)) WHERE ct.exp = "0";

UPDATE creature_template ct 
JOIN creature_classlevelstats cls ON ct.minlevel = cls.level AND ct.unit_class = cls.class
SET ct.DamageModifier = (IFNULL(ct.mindmg / ((cls.basedamage_exp1 + (cls.attackpower / 14)) * (ct.BaseAttackTime / 1000)), 1)) WHERE ct.exp = "1";

ALTER TABLE creature_template DROP COLUMN mindmg;
ALTER TABLE creature_template DROP COLUMN maxdmg;
ALTER TABLE creature_template DROP COLUMN attackpower;
ALTER TABLE creature_template DROP COLUMN minrangedmg;
ALTER TABLE creature_template DROP COLUMN maxrangedmg;
ALTER TABLE creature_template DROP COLUMN rangedattackpower;

UPDATE creature_template SET exp = "0" WHERE exp = "-1";
