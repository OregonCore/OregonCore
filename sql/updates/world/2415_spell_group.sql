-- Delete incorrect spell stacking rules for scrolls. Thanks to PadreWoW
DELETE FROM spell_group WHERE id IN (1021);
DELETE FROM spell_group_stack_rules WHERE group_id = 1021;