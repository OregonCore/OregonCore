-- Spell stack rules
DELETE FROM spell_group WHERE spell_id IN (20375);
INSERT INTO spell_group (id, spell_id) VALUES
-- Add missing Paladin seal to stack rules
(1017, 20375); -- Seal of Command