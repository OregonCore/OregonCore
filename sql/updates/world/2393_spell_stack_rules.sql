-- Fix up missing spell group stack rules
-- Thanks to Zear Forge for pointing out which spells were causing issues
DELETE FROM spell_group WHERE id IN (1008, 1009, 1012, 1013, 1014, 1015, 1016, 1017, 1018, 1019, 1020);
DELETE FROM spell_group WHERE spell_id IN (1044, 6940, -1016);
INSERT INTO spell_group (id, spell_id) VALUES
-- Mage
(1012, 7302),  -- Ice Armor
(1012, 30482), -- Molten Armor
(1012, 168),   -- Frost Armor
-- Hunter
(1013, 20043), -- Aspect of the Wild
(1013, 34074), -- Aspect of the Viper
(1013, 13161), -- Aspect of the Beast
(1013, 5118),  -- Aspect of the Cheetah
(1013, 13159), -- Aspect of the Pack
(1013, 13163), -- Aspect of the Monkey
(1013, 13165), -- Aspect of the Hawk
-- Warlock
(1014, 980),   -- Curse of Agony
(1014, 603),   -- Curse of Doom
(1014, 704),   -- Curse of Recklessness
(1014, 1490),  -- Curse of the Elements
(1014, 1714),  -- Curse of Tongues
(1014, 702),   -- Curse of Weakness
(1018, 28176), -- Fel Armor
(1018, 706),   -- Demon Armor
-- Paladin
-- Auras
(1015, 19876), -- Shadow Resistance Aura
(1015, 19891), -- Fire Resistance Aura
(1015, 19888), -- Frost Resistance Aura
(1015, 465),   -- Devotion Aura
(1015, 19746), -- Concentration Aura
(1015, 7294),  -- Retribution Aura
(1015, 32223), -- Crusader Aura
(1015, 20218), -- Sanctity Aura
-- Blessings, both greater and normal
(1009, 19977), -- Blessing of Light
(1009, 25890), -- Greater Blessing of Light
(1008, 1022),  -- Blessing of Protection
(1008, 23415), -- Improved Blessing of Protection
(1016, 1038),  -- Blessing of Salvation
(1016, 25895), -- Greater Blessing of Salvation
(1010, 1044),  -- Blessing of Freedom
(1010, 6940),  -- Blessing of Sacrifice
(1010, -1016), -- Salvation applied to blessings group
-- Seals
(1017, 20165), -- Seal of Light
(1017, 21084), -- Seal of Righteousness
(1017, 20166), -- Seal of Wisdom
(1017, 20164), -- Seal of Justice
(1017, 31892), -- Seal of Blood
(1017, 21082), -- Seal of the Crusader
-- Shamans
(1019, 324),   -- Lightning Shield
(1019, 24398), -- Water Shield
(1019, 974),   -- Earth Shield
-- Misc
-- Thori'dal & Quiver haste increases
(1020, 44972),
(1020, 29414),
(1020, 14829),
(1020, 14825),
(1020, 29416),
(1020, 29413),
(1020, 29417),
(1020, 29415),
(1020, 29418),
(1020, 14826),
(1020, 14828),
(1020, 14824),
(1020, 14827);

DELETE FROM spell_group_stack_rules WHERE group_id IN (1012, 1013, 1014, 1015, 1017, 1018, 1019, 1020);
INSERT INTO spell_group_stack_rules (group_id, stack_rule) VALUES
(1012, 1), -- Mage armors should not stack
(1013, 1), -- Hunter aspects should not stack
(1014, 2), -- Warlock curses should not stack for same caster
(1015, 2), -- Paladin Auras should not stack for same caster
(1017, 1), -- Paladin Seals should not stack
(1018, 1), -- Demon Armor and Fel Armor should not stack
(1019, 1), -- Shaman Shields should not stack
(1020, 1); -- Quiver bonuses + Thor'idal should not stack