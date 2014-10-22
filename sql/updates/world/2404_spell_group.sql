-- Spell stack rules
DELETE FROM spell_group WHERE id IN (1021);
DELETE FROM spell_group WHERE spell_id IN (20375);
INSERT INTO spell_group (id, spell_id) VALUES
-- Add missing Paladin seal to stack rules
(1017, 20375), -- Seal of Command
-- Add stack groups for scroll spells
(1021, 8118),  -- Scroll of Strength
(1021, 8099),  -- Scroll of Stamina
(1021, 8112),  -- Scroll of Spirit
(1021, 8096),  -- Scroll of Intellect
(1021, 8115),  -- Scroll of Agility
(1021, 8091);  -- Scroll of Armor

DELETE FROM spell_group_stack_rules WHERE group_id = 1021;
INSERT INTO spell_group_stack_rules (group_id, stack_rule) VALUES
(1021, 1);