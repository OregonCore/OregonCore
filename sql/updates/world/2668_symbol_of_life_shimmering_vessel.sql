delete from spell_script_target where entry = 8593 or entry = 31225;
delete from spell_dummy_condition where entry = 8593 or entry = 31225;

insert into spell_script_target (entry, type, targetEntry) values (8593, 1, 6172), (8593, 1, 17542);
insert into spell_dummy_condition (entry, effIndex, bitMaskCondition) values (8593, 0, 40);

insert into spell_script_target (entry, type, targetEntry) values (31225, 1, 17768);
insert into spell_dummy_condition (entry, effIndex, bitMaskCondition) values (31225, 0, 40);
