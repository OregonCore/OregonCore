update creature set spawndist = 0 where guid = 75468;
update creature set spawndist = 0 where guid = 75469;

delete from creature where guid = 86101;
delete from creature where guid = 86102;

replace into npc_gossip values (792693, 10401);

update creature_template set ScriptName = 'npc_grand_commander_ruusk' where entry = 20563;
