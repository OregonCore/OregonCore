-- Add Salvaged Ethereum Prison to world
DELETE FROM gameobject WHERE guid=11006312;
INSERT INTO gameobject VALUES ('11006312', '184488', '530', '1', '4246.31', '2171.91', '137.678', '3.64342', '0', '0', '0.968686', '-0.24829', '25', '0', '1');

-- Add ethereum prison base to world
DELETE FROM gameobject WHERE guid=11006311;
INSERT INTO gameobject VALUES ('11006311', '184998', '530', '1', '4246.31', '2171.91', '137.678', '3.64342', '0', '0', '0.968686', '-0.24829', '25', '0', '1');

-- Set script for commander ameer
UPDATE creature_template SET AIName='', ScriptName='npc_commander_ameer' WHERE entry=20448;

-- Make Salvaged Ethereum Prison non-interactible for players
UPDATE gameobject_template SET flags=50 WHERE entry=184488;
