REPLACE INTO `areatrigger_scripts` (entry, ScriptName)
VALUES (3066, 'at_ravenholdt');
REPLACE INTO `areatrigger_scripts` (entry, ScriptName)
VALUES (4591, 'at_coilfang_waterfall');

UPDATE `areatrigger_teleport` SET target_orientation = 0 WHERE id = 1470;
UPDATE `areatrigger_teleport` SET target_orientation = 0 WHERE id = 2568;
UPDATE `areatrigger_teleport` SET target_orientation = 0.703353 WHERE id = 3726;
UPDATE `areatrigger_teleport` SET target_orientation = 0 WHERE id = 4006;
UPDATE `areatrigger_teleport` SET target_orientation = 0 WHERE id = 4008;
UPDATE `areatrigger_teleport` SET target_orientation = 0 WHERE id = 4010;
UPDATE `areatrigger_teleport` SET target_orientation = 0 WHERE id = 4012;

REPLACE INTO `creature` (guid, curhealth, curmana, currentwaypoint, DeathState, equipment_id, id, map, modelid, MovementType, orientation, position_x, position_y, position_z, spawndist, spawnMask, spawntimesecs)
VALUES (54870, 55, 0, 0, 0, 0, 15271, 530, 0, 1, 3.99474, 10073.1, -6215.01, 13.9764, 5, 1, 300);
REPLACE INTO `creature` (guid, curhealth, curmana, currentwaypoint, DeathState, equipment_id, id, map, modelid, MovementType, orientation, position_x, position_y, position_z, spawndist, spawnMask, spawntimesecs)
VALUES (55009, 69, 79, 0, 0, 0, 15294, 530, 0, 1, 4.7293, 10069.8, -6237.32, 13.7188, 5, 1, 300);
REPLACE INTO `creature` (guid, curhealth, curmana, currentwaypoint, DeathState, equipment_id, id, map, modelid, MovementType, orientation, position_x, position_y, position_z, spawndist, spawnMask, spawntimesecs)
VALUES (55052, 69, 79, 0, 0, 0, 15294, 530, 0, 1, 2.30935, 10072.4, -6220.3, 13.8949, 5, 1, 300);

UPDATE `creature` SET spawntimesecs = 300 WHERE guid = 44548;
UPDATE `creature` SET spawntimesecs = 300 WHERE guid = 44843;
UPDATE `creature` SET MovementType = 2, position_x = 10294.4, position_y = -6274.46, position_z = 26.94 WHERE guid = 55184;
UPDATE `creature` SET MovementType = 2 WHERE guid = 56876;

DELETE FROM `creature` WHERE guid = 150185;

REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, isActive, mount, moveflags, path_id)
VALUES (55137, '', 0, 0, 0, 0, 1, 0, 0, 55184);
REPLACE INTO `creature_addon` (guid, auras, bytes0, bytes1, bytes2, emote, isActive, mount, moveflags, path_id)
VALUES (55184, '', 0, 0, 0, 0, 1, 0, 0, 55184);

UPDATE creature_addon SET isActive = 1 WHERE path_id != 0;
UPDATE `creature_addon` SET bytes0 = 0, bytes2 = 0, isActive = 1, path_id = 56876 WHERE guid = 56876;

UPDATE `creature_ai_texts` SET content_loc1 = '', content_loc2 = '', content_loc3 = '', content_loc4 = '', content_loc5 = '', content_loc6 = '', content_loc7 = '', content_loc8 = '' WHERE entry = -581;
UPDATE `creature_ai_texts` SET content_loc1 = '', content_loc2 = '', content_loc3 = '', content_loc4 = '', content_loc5 = '', content_loc6 = '', content_loc7 = '', content_loc8 = '' WHERE entry = -580;
UPDATE `creature_ai_texts` SET content_loc1 = '', content_loc2 = '', content_loc3 = '', content_loc4 = '', content_loc5 = '', content_loc6 = '', content_loc7 = '', content_loc8 = '' WHERE entry = -579;
UPDATE `creature_ai_texts` SET content_loc1 = '', content_loc2 = '', content_loc3 = '', content_loc4 = '', content_loc5 = '', content_loc6 = '', content_loc7 = '', content_loc8 = '' WHERE entry = -80;
UPDATE `creature_ai_texts` SET content_loc1 = '', content_loc2 = '', content_loc3 = '', content_loc4 = '', content_loc5 = '', content_loc6 = '', content_loc7 = '', content_loc8 = '' WHERE entry = -79;
UPDATE `creature_ai_texts` SET content_loc1 = '', content_loc2 = '', content_loc3 = '', content_loc4 = '', content_loc5 = '', content_loc6 = '', content_loc7 = '', content_loc8 = '' WHERE entry = -78;
UPDATE `creature_ai_texts` SET content_loc1 = '', content_loc2 = '', content_loc3 = '', content_loc4 = '', content_loc5 = '', content_loc6 = '', content_loc7 = '', content_loc8 = '' WHERE entry = -77;

REPLACE INTO `creature_formations` (memberGUID, angle, dist, groupAI, leaderGUID)
VALUES (55137, 20, 0, 2, 55184);

UPDATE `creature_model_info` SET modelid_other_gender = 3494 WHERE modelid = 7113;

REPLACE INTO `creature_onkill_reputation` (creature_id, IsTeamAward1, IsTeamAward2, MaxStanding1, MaxStanding2, RewOnKillRepFaction1, RewOnKillRepFaction2, RewOnKillRepValue1, RewOnKillRepValue2, TeamDependent)
VALUES (21346, 0, 0, 7, 0, 935, 0, 24, 0, 0);

UPDATE `creature_template` SET spell1 = 0, spell2 = 0, unit_flags = 2 WHERE entry = 510;
UPDATE `creature_template` SET ScriptName = '' WHERE entry = 842;
UPDATE `creature_template` SET ScriptName = '' WHERE entry = 1714;
UPDATE `creature_template` SET ScriptName = '' WHERE entry = 1884;
UPDATE `creature_template` SET ScriptName = '' WHERE entry = 1975;
UPDATE `creature_template` SET ScriptName = '' WHERE entry = 4970;
UPDATE `creature_template` SET ScriptName = '' WHERE entry = 6806;
UPDATE `creature_template` SET ScriptName = '' WHERE entry = 7785;
UPDATE `creature_template` SET ScriptName = '' WHERE entry = 8216;
UPDATE `creature_template` SET ScriptName = '' WHERE entry = 9034;
UPDATE `creature_template` SET ScriptName = '' WHERE entry = 9035;
UPDATE `creature_template` SET ScriptName = '' WHERE entry = 9036;
UPDATE `creature_template` SET ScriptName = '' WHERE entry = 9038;
UPDATE `creature_template` SET ScriptName = '' WHERE entry = 9040;
UPDATE `creature_template` SET ScriptName = '' WHERE entry = 10984;
UPDATE `creature_template` SET ScriptName = '' WHERE entry = 11338;
UPDATE `creature_template` SET ScriptName = 'npc_lunaclaw_spirit' WHERE entry = 12144;
UPDATE `creature_template` SET ScriptName = '' WHERE entry = 13936;
UPDATE `creature_template` SET ScriptName = '' WHERE entry = 15062;
UPDATE `creature_template` SET ScriptName = '' WHERE entry = 15089;
UPDATE `creature_template` SET ScriptName = 'mob_prowler' WHERE entry = 15101;
UPDATE `creature_template` SET ScriptName = '' WHERE entry = 15309;
UPDATE `creature_template` SET spell1 = 0 WHERE entry = 15352;
UPDATE `creature_template` SET ScriptName = 'npc_innkeeper' WHERE entry = 15433;
UPDATE `creature_template` SET spell1 = 0, spell2 = 0 WHERE entry = 15438;
UPDATE `creature_template` SET ScriptName = '' WHERE entry = 16410;
UPDATE `creature_template` SET ScriptName = '' WHERE entry = 17209;
UPDATE `creature_template` SET ScriptName = '' WHERE entry = 17244;
UPDATE `creature_template` SET ScriptName = '' WHERE entry = 17431;
UPDATE `creature_template` SET ScriptName = 'npc_taxi' WHERE entry = 17435;
UPDATE `creature_template` SET ScriptName = '' WHERE entry = 17855;
UPDATE `creature_template` SET ScriptName = 'npc_taxi' WHERE entry = 18725;
UPDATE `creature_template` SET ScriptName = '' WHERE entry = 18800;
UPDATE `creature_template` SET ScriptName = 'npc_innkeeper' WHERE entry = 19046;
UPDATE `creature_template` SET ScriptName = 'npc_innkeeper' WHERE entry = 19319;
UPDATE `creature_template` SET ScriptName = 'npc_taxi' WHERE entry = 19401;
UPDATE `creature_template` SET ScriptName = 'npc_taxi' WHERE entry = 19409;
UPDATE `creature_template` SET ScriptName = 'npc_innkeeper' WHERE entry = 19571;
UPDATE `creature_template` SET ScriptName = '' WHERE entry = 19797;
UPDATE `creature_template` SET ScriptName = '' WHERE entry = 19800;
UPDATE `creature_template` SET ScriptName = 'npc_taxi' WHERE entry = 20162;
UPDATE `creature_template` SET ScriptName = '' WHERE entry = 20189;
UPDATE `creature_template` SET ScriptName = 'npc_taxi' WHERE entry = 20235;
UPDATE `creature_template` SET ScriptName = '' WHERE entry = 20236;
UPDATE `creature_template` SET ScriptName = '' WHERE entry = 20424;
UPDATE `creature_template` SET ScriptName = '' WHERE entry = 20484;
UPDATE `creature_template` SET ScriptName = '' WHERE entry = 20485;
UPDATE `creature_template` SET RegenHealth = 1, unit_flags = 0 WHERE entry = 20899;
UPDATE `creature_template` SET ScriptName = 'npc_taxi' WHERE entry = 20903;
UPDATE `creature_template` SET ScriptName = '' WHERE entry = 20906;
UPDATE `creature_template` SET ScriptName = 'npc_innkeeper' WHERE entry = 21088;
UPDATE `creature_template` SET ScriptName = '' WHERE entry = 22074;
UPDATE `creature_template` SET ScriptName = '' WHERE entry = 22075;
UPDATE `creature_template` SET maxhealth = 7400, minhealth = 7400 WHERE entry = 22113;
UPDATE `creature_template` SET unit_flags = 0 WHERE entry = 22461;
UPDATE `creature_template` SET ScriptName = 'npc_innkeeper' WHERE entry = 22922;
UPDATE `creature_template` SET ScriptName = '' WHERE entry = 23054;
UPDATE `creature_template` SET ScriptName = '' WHERE entry = 23257;
UPDATE `creature_template` SET ScriptName = 'npc_taxi' WHERE entry = 23413;
UPDATE `creature_template` SET ScriptName = 'npc_taxi' WHERE entry = 23415;
UPDATE `creature_template` SET ScriptName = '' WHERE entry = 23498;
UPDATE `creature_template` SET ScriptName = 'npc_taxi' WHERE entry = 23704;
UPDATE `creature_template` SET ScriptName = '' WHERE entry = 23721;
UPDATE `creature_template` SET ScriptName = 'mob_mojo' WHERE entry = 24480;
UPDATE `creature_template` SET InhabitType = 1, ScriptName = 'npc_kalecgos' WHERE entry = 24844;
UPDATE `creature_template` SET ScriptName = 'npc_kalecgos' WHERE entry = 24848;
UPDATE `creature_template` SET ScriptName = '' WHERE entry = 24994;
UPDATE `creature_template` SET ScriptName = 'npc_taxi' WHERE entry = 25059;
UPDATE `creature_template` SET ScriptName = 'npc_taxi' WHERE entry = 25236;

REPLACE INTO `db_script_string` (entry, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8)
VALUES (2000000004, 'Let''s go.', '', '', '', '', '', '', '', '');
REPLACE INTO `db_script_string` (entry, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8)
VALUES (2000000005, 'It is not here! hmmm....', '', '', '', '', '', '', '', '');
REPLACE INTO `db_script_string` (entry, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8)
VALUES (2000000006, 'It is Hollee''s tent, but where Hollee?', '', '', '', '', '', '', '', '');
REPLACE INTO `db_script_string` (entry, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8)
VALUES (2000000007, 'It is not here too. Strange... Where it is?', '', '', '', '', '', '', '', '');
REPLACE INTO `db_script_string` (entry, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8)
VALUES (2000000008, 'Hmm. It is not here. Maybe Hollee has it...', '', '', '', '', '', '', '', '');
REPLACE INTO `db_script_string` (entry, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8)
VALUES (2000000027, '[PH] temp text', '', '', '', '', '', '', '', '');

UPDATE `db_script_string` SET content_default = 'Oooooo hot hot hot! If that won''t put spring IN your step,I don''t know what will!' WHERE entry = 2000000046;

DELETE FROM `db_script_string` WHERE entry = 2000000082;
DELETE FROM `db_script_string` WHERE entry = 2000000083;
DELETE FROM `db_script_string` WHERE entry = 2000000084;

DELETE FROM `event_scripts` WHERE command = 10 AND dataint = 0 AND datalong = 24844 AND datalong2 = 300000 AND delay = 0 AND id = 16547 AND o = 1.181 AND x = 209.912 AND y = -318.697 AND z = 14.005;

UPDATE `gameobject` SET rotation2 = 1.005 WHERE guid = 13487;

UPDATE `gameobject_template` SET ScriptName = 'go_cat_figurine' WHERE entry = 13873;
UPDATE `gameobject_template` SET ScriptName = 'go_gong_of_bethekk' WHERE entry = 180526;
UPDATE `gameobject_template` SET ScriptName = '' WHERE entry = 181714;
UPDATE `gameobject_template` SET ScriptName = '' WHERE entry = 181849;
UPDATE `gameobject_template` SET ScriptName = '' WHERE entry = 185584;
UPDATE `gameobject_template` SET data1 = 0, data2 = 0, type = 10 WHERE entry = 187578;

UPDATE `item_template` SET ScriptName = '' WHERE entry = 22962;

UPDATE `npc_vendor` SET ExtendedCost = 2288 WHERE entry = 18898 AND item = 33668;
UPDATE `npc_vendor` SET ExtendedCost = 2288 WHERE entry = 18898 AND item = 33674;
UPDATE `npc_vendor` SET ExtendedCost = 2288 WHERE entry = 18898 AND item = 33679;
UPDATE `npc_vendor` SET ExtendedCost = 2288 WHERE entry = 18898 AND item = 33682;
UPDATE `npc_vendor` SET ExtendedCost = 2288 WHERE entry = 18898 AND item = 33693;
UPDATE `npc_vendor` SET ExtendedCost = 2288 WHERE entry = 18898 AND item = 33699;
UPDATE `npc_vendor` SET ExtendedCost = 2288 WHERE entry = 18898 AND item = 33703;
UPDATE `npc_vendor` SET ExtendedCost = 2288 WHERE entry = 18898 AND item = 33710;
UPDATE `npc_vendor` SET ExtendedCost = 2288 WHERE entry = 18898 AND item = 33715;
UPDATE `npc_vendor` SET ExtendedCost = 2288 WHERE entry = 18898 AND item = 33720;
UPDATE `npc_vendor` SET ExtendedCost = 2288 WHERE entry = 18898 AND item = 33726;
UPDATE `npc_vendor` SET ExtendedCost = 2288 WHERE entry = 18898 AND item = 33732;
UPDATE `npc_vendor` SET ExtendedCost = 2288 WHERE entry = 18898 AND item = 33753;
UPDATE `npc_vendor` SET ExtendedCost = 2288 WHERE entry = 18898 AND item = 33757;
UPDATE `npc_vendor` SET ExtendedCost = 2288 WHERE entry = 18898 AND item = 33770;

DELETE FROM `quest_end_scripts` WHERE command = 0 AND dataint = 2000000082 AND datalong = 0 AND datalong2 = 0 AND delay = 0 AND id = 407 AND o = 0 AND x = 0 AND y = 0 AND z = 0;
DELETE FROM `quest_end_scripts` WHERE command = 0 AND dataint = 2000000083 AND datalong = 0 AND datalong2 = 0 AND delay = 8 AND id = 407 AND o = 0 AND x = 0 AND y = 0 AND z = 0;
DELETE FROM `quest_end_scripts` WHERE command = 0 AND dataint = 2000000084 AND datalong = 0 AND datalong2 = 0 AND delay = 1 AND id = 492 AND o = 0 AND x = 0 AND y = 0 AND z = 0;
DELETE FROM `quest_end_scripts` WHERE command = 1 AND dataint = 0 AND datalong = 7 AND datalong2 = 0 AND delay = 2 AND id = 492 AND o = 0 AND x = 0 AND y = 0 AND z = 0;
DELETE FROM `quest_end_scripts` WHERE command = 3 AND dataint = 0 AND datalong = 0 AND datalong2 = 0 AND delay = 3 AND id = 407 AND o = 2.6613 AND x = 2287.97 AND y = 236.253 AND z = 27.0892;
DELETE FROM `quest_end_scripts` WHERE command = 3 AND dataint = 0 AND datalong = 0 AND datalong2 = 0 AND delay = 6 AND id = 407 AND o = 0.693878 AND x = 2292 AND y = 239.481 AND z = 27.0892;
DELETE FROM `quest_end_scripts` WHERE command = 3 AND dataint = 0 AND datalong = 0 AND datalong2 = 0 AND delay = 10 AND id = 407 AND o = 4.8345 AND x = 2292.52 AND y = 235.226 AND z = 27.0892;
DELETE FROM `quest_end_scripts` WHERE command = 3 AND dataint = 0 AND datalong = 0 AND datalong2 = 0 AND delay = 13 AND id = 407 AND o = 2.48773 AND x = 2288.96 AND y = 237.96 AND z = 27.0892;
DELETE FROM `quest_end_scripts` WHERE command = 15 AND dataint = 0 AND datalong = 5 AND datalong2 = 1 AND delay = 6 AND id = 492 AND o = 0 AND x = 0 AND y = 0 AND z = 0;
DELETE FROM `quest_end_scripts` WHERE command = 15 AND dataint = 0 AND datalong = 5 AND datalong2 = 1 AND delay = 19 AND id = 407 AND o = 0 AND x = 0 AND y = 0 AND z = 0;
DELETE FROM `quest_end_scripts` WHERE command = 15 AND dataint = 0 AND datalong = 3287 AND datalong2 = 1 AND delay = 3 AND id = 407 AND o = 0 AND x = 0 AND y = 0 AND z = 0;

REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (0, 2000000004, 0, 0, 0, 731, 0, 0, 0, 0);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (0, 2000000005, 0, 0, 60, 731, 0, 0, 0, 0);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (0, 2000000006, 0, 0, 86, 731, 0, 0, 0, 0);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (0, 2000000007, 0, 0, 166, 731, 0, 0, 0, 0);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (0, 2000000008, 0, 0, 214, 731, 0, 0, 0, 0);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 1, 731, 0, 4677.35, 582.882, 21.3481);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 6, 731, 0, 4673.13, 582.741, 20.7995);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 11, 731, 0, 4672.99, 593.263, 17.5914);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 16, 731, 0, 4668.94, 600.76, 14.6225);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 21, 731, 0, 4659.59, 609.162, 9.36938);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 26, 731, 0, 4651.6, 615.518, 8.56175);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 31, 731, 0, 4644, 621.083, 8.57906);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 36, 731, 0, 4634.77, 624.85, 7.57635);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 41, 731, 0, 4633.5, 631.041, 6.61543);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 46, 731, 0, 4634.23, 633.058, 7.01277);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 51, 731, 0, 4640.83, 638.327, 13.4057);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 55, 731, 0, 4645.78, 633.204, 13.4303);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 61, 731, 0, 4639.92, 637.669, 13.3612);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 66, 731, 0, 4636.84, 635.289, 10.3009);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 71, 731, 0, 4630.55, 631.307, 6.32709);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 76, 731, 0, 4627.73, 637.741, 6.36486);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 81, 731, 0, 4622.89, 637.517, 6.31533);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 91, 731, 0, 4625.13, 645.962, 6.73182);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 101, 731, 0, 4624.88, 635.098, 6.30605);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 106, 731, 0, 4623.39, 631.595, 6.24625);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 111, 731, 0, 4617.66, 631.987, 6.25943);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 116, 731, 0, 4614.22, 619.818, 5.84416);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 121, 731, 0, 4609.45, 613.739, 5.24473);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 126, 731, 0, 4599.65, 607.006, 1.94703);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 131, 731, 0, 4589.98, 599.73, 1.16939);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 136, 731, 0, 4581.11, 593.133, 1.01014);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 141, 731, 0, 4565.69, 582.105, 1.04814);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 146, 731, 0, 4558.03, 571.718, 1.28869);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 151, 731, 0, 4553.65, 566.054, 5.31751);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 156, 731, 0, 4550.6, 562.106, 7.30321);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 161, 731, 0, 4544.58, 568.095, 7.27242);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 167, 731, 0, 4551.83, 564.466, 7.19855);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 172, 731, 0, 4557.01, 571.599, 1.27336);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 177, 731, 0, 4565.46, 557.54, 3.03739);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 189, 731, 0, 4578.1, 565.285, 1.02112);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 194, 731, 0, 4589.62, 564.402, 0.923398);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 199, 731, 0, 4595.54, 572.613, 1.10837);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 204, 731, 0, 4600.8, 572.295, 1.23612);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 209, 731, 0, 4607.06, 566.704, 1.26906);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 215, 731, 0, 4590.98, 571.303, 1.13302);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 220, 731, 0, 4574.5, 571.384, 1.10307);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 225, 731, 0, 4575.81, 581.597, 0.979237);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 230, 731, 0, 4600.36, 604.279, 2.03134);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 230, 731, 0, 4614.11, 613.762, 5.50871);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 235, 731, 0, 4629.06, 620.1, 6.59015);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 240, 731, 0, 4636.06, 624.593, 7.78406);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 245, 731, 0, 4640.59, 625.475, 8.21139);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 250, 731, 0, 4656.98, 613.055, 8.56197);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 255, 731, 0, 4675.34, 598.581, 17.3818);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (3, 0, 0, 0, 260, 731, 0, 4687.96, 590.182, 23.839);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (10, 0, 2158, 10000, 96, 731, 0, 4615.94, 640.499, 6.67037);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (10, 0, 2158, 10000, 96, 731, 0, 4628.38, 638.456, 6.402);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (10, 0, 2158, 10000, 183, 731, 0, 4570.04, 557.292, 1.989);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (10, 0, 2159, 10000, 183, 731, 0, 4573.17, 557.583, 3.328);
REPLACE INTO `quest_start_scripts` (command, dataint, datalong, datalong2, delay, id, o, x, y, z)
VALUES (10, 0, 2160, 10000, 183, 731, 0, 4564.94, 551.357, 5.91);

UPDATE `quest_template` SET CompleteScript = 0 WHERE entry = 407;
UPDATE `quest_template` SET CompleteScript = 0 WHERE entry = 492;
UPDATE `quest_template` SET StartScript = 731 WHERE entry = 731;
UPDATE `quest_template` SET SrcSpell = 0 WHERE entry = 5162;
UPDATE `quest_template` SET ReqCreatureOrGOCount1 = 0, ReqCreatureOrGOId1 = 0, SpecialFlags = 2 WHERE entry = 10409;

REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1309024, 'mandokir SAY_REVIVE', 'I am released through you! Avenge me!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);

UPDATE `spell_proc_event` SET procFlags = 16384 WHERE entry = 19572;
UPDATE `spell_proc_event` SET procFlags = 16384 WHERE entry = 19573;

REPLACE INTO `waypoint_data` (id, point, action, action_chance, delay, move_flag, position_x, position_y, position_z, wpguid)
VALUES (55184, 1, 0, 100, 15000, 0, 10280.6, -6278.77, 25.623, 0);
REPLACE INTO `waypoint_data` (id, point, action, action_chance, delay, move_flag, position_x, position_y, position_z, wpguid)
VALUES (55184, 2, 0, 100, 15000, 0, 10284.2, -6250.37, 27.0875, 0);
REPLACE INTO `waypoint_data` (id, point, action, action_chance, delay, move_flag, position_x, position_y, position_z, wpguid)
VALUES (55184, 3, 0, 100, 15000, 0, 10335.8, -6250.78, 26.6531, 0);
REPLACE INTO `waypoint_data` (id, point, action, action_chance, delay, move_flag, position_x, position_y, position_z, wpguid)
VALUES (56876, 1, 0, 100, 0, 0, 9713.46, -6646.89, 6.40212, 0);
REPLACE INTO `waypoint_data` (id, point, action, action_chance, delay, move_flag, position_x, position_y, position_z, wpguid)
VALUES (56876, 2, 0, 100, 5000, 0, 9762.31, -6621.3, 8.21267, 0);
REPLACE INTO `waypoint_data` (id, point, action, action_chance, delay, move_flag, position_x, position_y, position_z, wpguid)
VALUES (56876, 3, 0, 100, 5000, 0, 9756.89, -6614.88, 8.23266, 0);
REPLACE INTO `waypoint_data` (id, point, action, action_chance, delay, move_flag, position_x, position_y, position_z, wpguid)
VALUES (56876, 4, 0, 100, 0, 0, 9711.72, -6642.27, 6.3964, 0);
REPLACE INTO `waypoint_data` (id, point, action, action_chance, delay, move_flag, position_x, position_y, position_z, wpguid)
VALUES (56876, 5, 0, 100, 0, 0, 9663.58, -6672.27, 5.7132, 0);
REPLACE INTO `waypoint_data` (id, point, action, action_chance, delay, move_flag, position_x, position_y, position_z, wpguid)
VALUES (56876, 6, 0, 100, 0, 0, 9621.88, -6696.43, 5.63989, 0);
REPLACE INTO `waypoint_data` (id, point, action, action_chance, delay, move_flag, position_x, position_y, position_z, wpguid)
VALUES (56876, 7, 0, 100, 0, 0, 9579.63, -6722.48, 5.64037, 0);
REPLACE INTO `waypoint_data` (id, point, action, action_chance, delay, move_flag, position_x, position_y, position_z, wpguid)
VALUES (56876, 8, 0, 100, 0, 0, 9548.22, -6754.46, 12.5445, 0);
REPLACE INTO `waypoint_data` (id, point, action, action_chance, delay, move_flag, position_x, position_y, position_z, wpguid)
VALUES (56876, 9, 0, 100, 0, 0, 9583.81, -6731.78, 6.0709, 0);
REPLACE INTO `waypoint_data` (id, point, action, action_chance, delay, move_flag, position_x, position_y, position_z, wpguid)
VALUES (56876, 10, 0, 100, 0, 0, 9624.49, -6706.43, 5.81045, 0);
REPLACE INTO `waypoint_data` (id, point, action, action_chance, delay, move_flag, position_x, position_y, position_z, wpguid)
VALUES (56876, 11, 0, 100, 0, 0, 9668.67, -6678.86, 6.03919, 0);
REPLACE INTO `waypoint_data` (id, point, action, action_chance, delay, move_flag, position_x, position_y, position_z, wpguid)
VALUES (56876, 12, 0, 100, 0, 0, 9713.46, -6646.89, 6.40212, 0);

DELETE FROM `event_scripts` WHERE command = 10 AND dataint = 0 AND datalong = 24844 AND datalong2 = 300000 AND delay = 0 AND id = 16547;

DELETE FROM `quest_end_scripts` WHERE command = 3 AND dataint = 0 AND datalong = 0 AND datalong2 = 0 AND delay = 3 AND id = 407;
DELETE FROM `quest_end_scripts` WHERE command = 3 AND dataint = 0 AND datalong = 0 AND datalong2 = 0 AND delay = 6 AND id = 407;
DELETE FROM `quest_end_scripts` WHERE command = 3 AND dataint = 0 AND datalong = 0 AND datalong2 = 0 AND delay = 10 AND id = 407;
DELETE FROM `quest_end_scripts` WHERE command = 3 AND dataint = 0 AND datalong = 0 AND datalong2 = 0 AND delay = 13 AND id = 407;

