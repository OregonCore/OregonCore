DELETE FROM spell_script_target WHERE targetEntry=186283;
INSERT INTO spell_script_target VALUES
(42287, 0, 186283),
(42289, 0, 186283);

UPDATE creature_template SET minlevel=35, maxlevel=36, minhealth=1403, maxhealth=1469, armor=1420, faction_A=18, faction_H=18, mindmg=20, maxdmg=37, attackpower=201, baseattacktime=1610, unit_flags=32768, minrangedmg=53.8384, maxrangedmg=74.0278, rangedattackpower=100, MovementType=0 WHERE entry=23701;