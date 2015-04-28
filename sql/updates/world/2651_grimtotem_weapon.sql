DELETE FROM spell_script_target WHERE `entry` = 42454;
INSERT INTO spell_script_target (`entry`, `type`, `targetEntry`) VALUES
(42454, 1, 4344);

UPDATE creature_template SET spell1 = 42464 WHERE entry=23811;
UPDATE creature_template SET AIName = '' WHERE entry=4344;
UPDATE creature_template SET ScriptName = 'npc_mottled_drywallow_crocolisk' WHERE entry=4344;