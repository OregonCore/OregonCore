ALTER TABLE creature_template DROP COLUMN mindmg;
ALTER TABLE creature_template DROP COLUMN maxdmg;
ALTER TABLE creature_template DROP COLUMN attackpower;
ALTER TABLE creature_template DROP COLUMN minrangedmg;
ALTER TABLE creature_template DROP COLUMN maxrangedmg;
ALTER TABLE creature_template DROP COLUMN rangedattackpower;

ALTER TABLE creature_template CHANGE dmg_multiplier DamageModifier FLOAT NOT NULL DEFAULT 1 AFTER ArmorModifier;
ALTER TABLE creature_template CHANGE baseattacktime BaseAttackTime int(10) NOT NULL DEFAULT 0;
ALTER TABLE creature_template CHANGE rangeattacktime RangeAttackTime int(10) NOT NULL DEFAULT 0;
ALTER TABLE creature_template ADD BaseVariance FLOAT NOT NULL DEFAULT 0 AFTER RangeAttackTime;
ALTER TABLE creature_template ADD RangeVariance FLOAT NOT NULL DEFAULT 0 AFTER BaseVariance;

UPDATE creature_template
JOIN   creature_templatewotlk ON creature_template.entry = creature_templatewotlk.entry
SET    creature_template.DamageModifier = creature_templatewotlk.DamageModifier;

UPDATE creature_template
JOIN   creature_templatewotlk ON creature_template.entry = creature_templatewotlk.entry
SET    creature_template.BaseAttackTime = creature_templatewotlk.BaseAttackTime;

UPDATE creature_template
JOIN   creature_templatewotlk ON creature_template.entry = creature_templatewotlk.entry
SET    creature_template.RangeAttackTime = creature_templatewotlk.RangeAttackTime;

UPDATE creature_template
JOIN   creature_templatewotlk ON creature_template.entry = creature_templatewotlk.entry
SET    creature_template.BaseVariance = creature_templatewotlk.BaseVariance;

UPDATE creature_template
JOIN   creature_templatewotlk ON creature_template.entry = creature_templatewotlk.entry
SET    creature_template.RangeVariance = creature_templatewotlk.RangeVariance;

UPDATE creature_template
JOIN   creature_templatewotlk ON creature_template.entry = creature_templatewotlk.entry
SET    creature_template.unit_class = creature_templatewotlk.unit_class;

UPDATE creature_template
JOIN   creature_templatewotlk ON creature_template.entry = creature_templatewotlk.entry
SET    creature_template.dmgschool = creature_templatewotlk.dmgschool;

DROP TABLE IF EXISTS `creature_templatewotlk`; 