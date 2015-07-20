-- Fel Cannon MKI EventAI
UPDATE creature_template SET AIName = 'EventAI' WHERE entry = '22461';
-- Immune to Fear
UPDATE creature_template SET mechanic_immune_mask = 16400 WHERE entry = '22461';
DELETE FROM creature_ai_scripts WHERE creature_id = 22461;
INSERT INTO creature_ai_scripts VALUES
('2246151', '22461', '1', '0', '100', '0', '0', '0', '0', '0', '21', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'Fel Cannon MKI - OOC - Prevent Combat Movement'),
('2246152', '22461', '0', '0', '100', '1', '1000', '1000', '5000', '7000', '21', '0', '0', '0', '11', '36238', '1', '8', '0', '0', '0', '0', 'Fel Cannon MKI - Combat - Cast Fel Cannon Blast');

-- Fel Cannon
UPDATE creature_template SET AIName = 'EventAI' WHERE entry = '21233';
-- Immune to Fear
UPDATE creature_template SET mechanic_immune_mask = 16400 WHERE entry = '21233';
INSERT INTO creature_ai_scripts VALUES
('2123351', '21233', '1', '0', '100', '0', '0', '0', '0', '0', '21', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'Fel Cannon - OOC - Prevent Combat Movement'),
('2123352', '21233', '0', '0', '100', '1', '1000', '1000', '5000', '7000', '21', '0', '0', '0', '11', '36238', '1', '8', '0', '0', '0', '0', 'Fel Cannon - Combat - Cast Fel Cannon Blast');
