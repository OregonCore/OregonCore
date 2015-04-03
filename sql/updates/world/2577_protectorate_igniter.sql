-- Quest Id: 10345
-- Spell Id: 35372
-- Target Id: 20561

-- Target Check
insert into spell_dummy_condition (entry, effIndex, bitMaskCondition, condition0, data0)
values (35372, 0, 1 << 6, 13, 20561);

-- Script
insert into spell_scripts (id, delay, command, datalong, datalong2)
values (35372, 0, 8, 20561, 0);

delete from spell_script_target where entry = 35372;

update creature set spawntimesecs = 300 where id = 20561;
