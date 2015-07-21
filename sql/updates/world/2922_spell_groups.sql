-- Spells that should stack if cast by different casters

delete from spell_group where id >= 1022 AND id <= 1065;

-- shadow word; pain
insert into spell_group (id, spell_id) values (1022, 589);
insert into spell_group_stack_rules (group_id, stack_rule) values (1022, 2);
-- vampiric touch
insert into spell_group (id, spell_id) values (1023, 34914);
insert into spell_group_stack_rules (group_id, stack_rule) values (1023, 2);
-- mind flay
insert into spell_group (id, spell_id) values (1024, 15407);
insert into spell_group_stack_rules (group_id, stack_rule) values (1024, 2);
-- corruption
insert into spell_group (id, spell_id) values (1025, 172);
insert into spell_group_stack_rules (group_id, stack_rule) values (1025, 2);
-- immolate
insert into spell_group (id, spell_id) values (1026, 348);
insert into spell_group_stack_rules (group_id, stack_rule) values (1026, 2);
-- drain life
insert into spell_group (id, spell_id) values (1027, 689);
insert into spell_group_stack_rules (group_id, stack_rule) values (1027, 2);
-- drain mana
insert into spell_group (id, spell_id) values (1028, 5138);
insert into spell_group_stack_rules (group_id, stack_rule) values (1028, 2);
-- shadowfury
insert into spell_group (id, spell_id) values (1029, 30283);
insert into spell_group_stack_rules (group_id, stack_rule) values (1029, 2);
-- Kidney Shot
insert into spell_group (id, spell_id) values (1030,408);
insert into spell_group_stack_rules (group_id, stack_rule) values (1030, 2);
,-- Garrote
insert into spell_group (id, spell_id) values (1031,703);
insert into spell_group_stack_rules (group_id, stack_rule) values (1031, 2);
,-- Rejuvenation
insert into spell_group (id, spell_id) values (1032,774);
insert into spell_group_stack_rules (group_id, stack_rule) values (1032, 2);
,-- Hammer of Justice
insert into spell_group (id, spell_id) values (1033,853);
insert into spell_group_stack_rules (group_id, stack_rule) values (1033, 2);
,-- Drain Soul
insert into spell_group (id, spell_id) values (1034,1120);
insert into spell_group_stack_rules (group_id, stack_rule) values (1034, 2);
,-- Cheap Shot
insert into spell_group (id, spell_id) values (1035,1833);
insert into spell_group_stack_rules (group_id, stack_rule) values (1035, 2);
,-- Rupture
insert into spell_group (id, spell_id) values (1036,1943);
insert into spell_group_stack_rules (group_id, stack_rule) values (1036, 2);
,-- Flamestrike
insert into spell_group (id, spell_id) values (1037,2120);
insert into spell_group_stack_rules (group_id, stack_rule) values (1037, 2);
,-- Wrath of Air Totem
insert into spell_group (id, spell_id) values (1038,2895);
insert into spell_group_stack_rules (group_id, stack_rule) values (1038, 2);
,-- Firebolt
insert into spell_group (id, spell_id) values (1039,3110);
insert into spell_group_stack_rules (group_id, stack_rule) values (1039, 2);
,-- Insect Swarm
insert into spell_group (id, spell_id) values (1040,5570);
insert into spell_group_stack_rules (group_id, stack_rule) values (1040, 2);
,-- Revenge
insert into spell_group (id, spell_id) values (1041,6572);
insert into spell_group_stack_rules (group_id, stack_rule) values (1041, 2);
,-- Death Coil
insert into spell_group (id, spell_id) values (1042,6789);
insert into spell_group_stack_rules (group_id, stack_rule) values (1042, 2);
,-- Sunder Armor
insert into spell_group (id, spell_id) values (1043,7386);
insert into spell_group_stack_rules (group_id, stack_rule) values (1043, 2);
,-- Flame Shock
insert into spell_group (id, spell_id) values (1044,8050);
insert into spell_group_stack_rules (group_id, stack_rule) values (1044, 2);
,-- Frost Shock
insert into spell_group (id, spell_id) values (1045,8056);
insert into spell_group_stack_rules (group_id, stack_rule) values (1045, 2);
,-- Expose Armor
insert into spell_group (id, spell_id) values (1046,8647);
insert into spell_group_stack_rules (group_id, stack_rule) values (1046, 2);
,-- Regrowth
insert into spell_group (id, spell_id) values (1047,8936);
insert into spell_group_stack_rules (group_id, stack_rule) values (1047, 2);
,-- Ignite
insert into spell_group (id, spell_id) values (1048,11119);
insert into spell_group_stack_rules (group_id, stack_rule) values (1048, 2);
,-- Winter's Chill
insert into spell_group (id, spell_id) values (1049,11180);
insert into spell_group_stack_rules (group_id, stack_rule) values (1049, 2);
,-- Pyroblast
insert into spell_group (id, spell_id) values (1050,11366);
insert into spell_group_stack_rules (group_id, stack_rule) values (1050, 2);
,-- Improved Rend
insert into spell_group (id, spell_id) values (1051,12286);
insert into spell_group_stack_rules (group_id, stack_rule) values (1051, 2);
,-- Concussion Blow
insert into spell_group (id, spell_id) values (1052,12809);
insert into spell_group_stack_rules (group_id, stack_rule) values (1052, 2);
,-- Vampiric Embrace
insert into spell_group (id, spell_id) values (1053,15286);
insert into spell_group_stack_rules (group_id, stack_rule) values (1053, 2);
,-- Hemorrhage
insert into spell_group (id, spell_id) values (1054,16511);
insert into spell_group_stack_rules (group_id, stack_rule) values (1054, 2);
,-- Stormstrike
insert into spell_group (id, spell_id) values (1055,17364);
insert into spell_group_stack_rules (group_id, stack_rule) values (1055, 2);
,-- Shadowburn
insert into spell_group (id, spell_id) values (1056,17877);
insert into spell_group_stack_rules (group_id, stack_rule) values (1056, 2);
,-- Aimed Shot
insert into spell_group (id, spell_id) values (1057,19434);
insert into spell_group_stack_rules (group_id, stack_rule) values (1057, 2);
,-- Seed of Corruption
insert into spell_group (id, spell_id) values (1058,27243);
insert into spell_group_stack_rules (group_id, stack_rule) values (1058, 2);
,-- Seed of Corruption
insert into spell_group (id, spell_id) values (1059,27285);
insert into spell_group_stack_rules (group_id, stack_rule) values (1059, 2);
,-- Healing Way
insert into spell_group (id, spell_id) values (1060,29206);
insert into spell_group_stack_rules (group_id, stack_rule) values (1060, 2);
,-- Unstable Affliction
insert into spell_group (id, spell_id) values (1061,30108);
insert into spell_group_stack_rules (group_id, stack_rule) values (1061, 2);
,-- Slow
insert into spell_group (id, spell_id) values (1062,31589);
insert into spell_group_stack_rules (group_id, stack_rule) values (1062, 2);
,-- Lifebloom
insert into spell_group (id, spell_id) values (1063,33763);
insert into spell_group_stack_rules (group_id, stack_rule) values (1063, 2);
,-- Silencing Shot
insert into spell_group (id, spell_id) values (1064,41084);
insert into spell_group_stack_rules (group_id, stack_rule) values (1064, 2);
,-- Seed of Corruption
insert into spell_group (id, spell_id) values (1065,43991);
insert into spell_group_stack_rules (group_id, stack_rule) values (1065, 2);
