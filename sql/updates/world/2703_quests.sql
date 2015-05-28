-- Quest 1126 Make the object targetable
update gameobject_template set flags = 0 where entry = 178553;
-- Make wanted poster responsive
update gameobject_template set faction = 68 where entry = 182588;
