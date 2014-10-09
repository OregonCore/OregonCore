-- Correct spell_dummy_condition for Tame Beast.
DELETE FROM spell_dummy_condition WHERE entry = 1515;
INSERT INTO spell_dummy_condition VALUES 
(1515, 0, 0x200B, 14, 1, 15, -1, 0, 0, 0, 0, 0, 0, 'Tame Beast');
