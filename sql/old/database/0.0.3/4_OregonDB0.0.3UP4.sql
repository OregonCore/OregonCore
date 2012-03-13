-- Updatepack 4 for ODB 0.0.3 for 2.4.3

-- By Niyo

-- Lightning Capacitor fix
DELETE FROM spell_proc_event where entry=37657;

INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES
(37657, 94, 0, 0, 10000, 2, 0, 0, 3);

-- Deathfrost
DELETE FROM `spell_proc_event` WHERE `entry` = 46662;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES
(46662, 1, 0, 0, 20, 0, 0, 0, 20);

-- Mechanar gatewatcher
update creature_addon set path_id = 0 where guid = 83239;
update creature_addon set path_id = 0 where guid = 83240;

update creature_template set flags_extra = 1 where entry = 19218;
update creature_template set flags_extra = 1 where entry = 19710;

-- new positions
delete from creature where guid = 83239;
INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) VALUES
(83239, 19218, 554, 3, 0, 0, 88.22, 21.241, 14.923, 0.996673, 0, 0, 0, 92250, 0, 0, 2);

delete from creature where guid = 83240;
INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) VALUES
(83240, 19710, 554, 3, 0, 0, 193.322, -79.0134, 0.005, 3.14707, 0, 0, 0, 121770, 0, 0, 2);

-- Consec threat

INSERT INTO `spell_threat` (`entry`, `Threat`) VALUES (27173, 150);

-- Sethekk halls

-- Dark Vortex
UPDATE  `creature_template` SET  `faction_A` =  '14', `faction_H` =  '14' WHERE  `creature_template`.`entry` =20689;
-- Void portal
UPDATE  `creature_template` SET  `faction_A` =  '35', `faction_H` =  '35' WHERE  `creature_template`.`entry` = 20663;

-- Shadowlab

UPDATE `access_requirement` SET `item` = '0' WHERE `access_requirement`.`id` =42;

-- Boss totems

UPDATE `creature_template` SET  `AIName` =  'TotemAI' WHERE  `creature_template`.`entry` =24224;

UPDATE `creature_template` SET  `AIName` =  'TotemAI' WHERE  `creature_template`.`entry` =22091;

UPDATE `creature_template` SET  `spell1` =  '38622', `AIName` =  'TotemAI' WHERE  `creature_template`.`entry` =22236;

UPDATE `creature_template` SET  `AIName` =  'TotemAI' WHERE  `creature_template`.`entry` =22486;

-- Hyjal mobs

UPDATE `creature_template` SET  `faction_A` =  '1720',
`faction_H` =  '1720' WHERE  `creature_template`.`entry` =17902;

UPDATE `creature_template` SET  `faction_A` =  '1720',
`faction_H` =  '1720' WHERE  `creature_template`.`entry` =17903;

UPDATE `creature_template` SET  `minhealth` =  '58684',
`maxhealth` =  '85000' WHERE  `creature_template`.`entry` =17905;

-- Karazhan

UPDATE `creature_template` SET  `ScriptName` =  'npc_Spectral Retainer' WHERE  `creature_template`.`entry` = 16410;

-- Supremus volcano

UPDATE `creature_template` SET `flags_extra` =  '0' WHERE  `creature_template`.`entry` =23085;

-- startup fixes

DELETE FROM creature_addon WHERE guid NOT IN (SELECT guid FROM creature);
-- delete wrong aura from creature_addon
Update creature_addon set auras = NULL WHERE guid = 4699;
-- set current health correctly
Update creature set curhealth = 5000000 WHERE guid = 100001;
-- wotlk hunter wypern stings
Delete from spell_linked_spell where spell_trigger = -49012;
Delete from spell_linked_spell where spell_trigger = -49011;
-- wotlk holy nova spells
Delete from spell_linked_spell where spell_trigger = 48077;
Delete from spell_linked_spell where spell_trigger = 48078;
-- delete senseless entry from creature_formations and insert some of my own
Delete from creature_formations WHERE leaderGUID = 0;
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES
(42645, 54992, 99, 99, 2),
(42645, 42645, 99, 99, 2);

-- bt npcs linked respawn

INSERT INTO `creature_linked_respawn` (`guid`, `linkedGuid`) VALUES
(12849, 40527),
(16201, 40527),
(12845, 40527),
(12844, 40527),
(16199, 40527),
(12848, 40527),
(12869, 40527),
(12853, 40527),
(12854, 40527),
(12865, 40527),
(15215, 40527),
(12864, 40527),
(12866, 40527),
(12852, 40527),
(12851, 40527),
(12855, 40527),
(12846, 40527),
(12847, 40527),
(12856, 40527),
(12850, 40527),
(16202, 40527),
(12868, 40527),
(12867, 40527),
(14856, 40527),
(12861, 40527),
(12860, 40527),
(53815, 52458),
(52912, 52458),
(52914, 52458),
(52915, 52458),
(52917, 52458),
(52916, 52458),
(52913, 52458),
(53820, 52458),
(53821, 52458),
(52941, 52458),
(53035, 52458),
(52939, 52458),
(52940, 52458),
(53048, 52458),
(52991, 52458),
(52889, 52458),
(53893, 52458),
(53906, 52458),
(52890, 52458),
(52871, 52458),
(52872, 52458),
(53911, 52458),
(53910, 52458),
(53818, 52458),
(53817, 52458),
(53819, 52458),
(52907, 52458),
(52909, 52458),
(52908, 52458),
(52910, 52458),
(52911, 52458),
(52935, 52458),
(52936, 52458),
(52919, 52458),
(52938, 52458),
(52930, 52458),
(52918, 52458),
(12740, 52458),
(12739, 52458),
(52861, 52458),
(52888, 52458),
(52887, 52458),
(53051, 52458),
(53050, 52458),
(53049, 52458),
(53052, 52458),
(53053, 52458),
(53816, 52458),
(52769, 52458),
(52873, 52458),
(52874, 52458),
(52772, 52458),
(52773, 52458),
(12743, 52458),
(53917, 52458),
(52860, 52458),
(52902, 52458),
(52901, 52458),
(12741, 52458),
(12742, 52458),
(52768, 52458),
(53912, 52458),
(53913, 52458),
(53843, 52458),
(12738, 52458),
(53842, 52458),
(12737, 52458),
(93896, 93846),
(93897, 93846),
(93899, 93846),
(93898, 93846);

-- bt respawn

-- Set all respawntimes to 2 hours
Update creature set spawntimesecs = 10800 where map = 564;

-- Set Boss respawntimes to 7 days
Update creature set spawntimesecs = 604800 where id = 23420;
Update creature set spawntimesecs = 604800 where id = 23419;
Update creature set spawntimesecs = 604800 where id = 23418;
Update creature set spawntimesecs = 604800 where id = 22949;
Update creature set spawntimesecs = 604800 where id = 22948;
Update creature set spawntimesecs = 604800 where id = 22950;
Update creature set spawntimesecs = 604800 where id = 22887;
Update creature set spawntimesecs = 604800 where id = 22917;
Update creature set spawntimesecs = 604800 where id = 22951;
Update creature set spawntimesecs = 604800 where id = 22947;
Update creature set spawntimesecs = 604800 where id = 22841;
Update creature set spawntimesecs = 604800 where id = 22898;
Update creature set spawntimesecs = 604800 where id = 22879;
Update creature set spawntimesecs = 604800 where id = 22952;

-- Set Akama respawntime to 2 mins
Update creature set spawntimesecs = 3600 where id = 22990;

-- firework cluster guy
UPDATE `creature_template` SET  `faction_A` =  '35',
`faction_H` =  '35' WHERE  `creature_template`.`entry` =15872;

UPDATE `creature_template` SET  `faction_A` =  '35',
`faction_H` =  '35' WHERE  `creature_template`.`entry` =15918;

-- AV texts

-- Balinda
INSERT INTO `script_texts` (`entry` ,`content_default`, `type`, `comment`)VALUES ('-2100019',  'Begone, uncouth scum! The Alliance shall prevail in Alterac Valley!', '1', 'BALINDA_YELL_AGGRO');
INSERT INTO `script_texts` (`entry` ,`content_default`, `type`, `comment`)VALUES ('-2100020',  'Filthy Frostwolf cowards! If you want a fight, you''ll have to come to me!', '1', 'BALINDA_YELL_EVADE');

-- Galvangar
INSERT INTO `script_texts` (`entry` ,`content_default`, `type`, `comment`)VALUES ('-2100021',  'Die! Your kind has no place in Alterac Valley!', '1', 'GALVANGAR_YELL_AGGRO');
INSERT INTO `script_texts` (`entry` ,`content_default`, `type`, `comment`)VALUES ('-2100022',  'I''ll never fall for that, fool! If you want a battle it will be on my terms and in my lair!', '1', 'GALVANGAR_YELL_EVADE');

-- Vanndar
INSERT INTO `script_texts` (`entry` ,`content_default`, `type`, `comment`)VALUES ('-2100008',  'Soldiers of Stormpike, your General is under attack! I require aid! Come! Come! Slay these mangy Frostwolf dogs.', '1', 'VANNDAR_YELL_AGGRO');
INSERT INTO `script_texts` (`entry` ,`content_default`, `type`, `comment`)VALUES ('-2100009',  'You''ll never get me out of me bunker, heathens!', '1', 'VANNDAR_YELL_EVADE');
INSERT INTO `script_texts` (`entry` ,`content_default`, `type`, `comment`)VALUES ('-2100012',  'I will tell you this much...Alterac Valley will be ours.', '1', 'VANNDAR_YELL_RANDOM1');
INSERT INTO `script_texts` (`entry` ,`content_default`, `type`, `comment`)VALUES ('-2100013',  'Why don''t ya try again without yer cheap tactics, pansies! Or are you too chicken?', '1', 'VANNDAR_YELL_RANDOM2');
INSERT INTO `script_texts` (`entry` ,`content_default`, `type`, `comment`)VALUES ('-2100014',  'Your attacks are weak! Go practice on some rabbits and come back when you''re stronger.', '1', 'VANNDAR_YELL_RANDOM3');
INSERT INTO `script_texts` (`entry` ,`content_default`, `type`, `comment`)VALUES ('-2100015',  'We will not be swayed from our mission!', '1', 'VANNDAR_YELL_RANDOM4');
INSERT INTO `script_texts` (`entry` ,`content_default`, `type`, `comment`)VALUES ('-2100016',  'It''ll take more than you rabble to bring me down!', '1', 'VANNDAR_YELL_RANDOM5');
INSERT INTO `script_texts` (`entry` ,`content_default`, `type`, `comment`)VALUES ('-2100017',  'We, the Alliance, will prevail!', '1', 'VANNDAR_YELL_RANDOM6');
INSERT INTO `script_texts` (`entry` ,`content_default`, `type`, `comment`)VALUES ('-2100018',  'Is that the best you can do?', '1', 'VANNDAR_YELL_RANDOM7');

-- Drek'Thar
INSERT INTO `script_texts` (`entry` ,`content_default`, `type`, `comment`)VALUES ('-2100000',  'Stormpike filth! In my keep?! Slay them all!', '1', 'DREKTHAR_YELL_AGGRO');
INSERT INTO `script_texts` (`entry` ,`content_default`, `type`, `comment`)VALUES ('-2100001',  'You seek to draw the General of the Frostwolf legion out from his fortress? PREPOSTEROUS!', '1', 'DREKTHAR_YELL_EVADE');
INSERT INTO `script_texts` (`entry` ,`content_default`, `type`, `comment`)VALUES ('-2100003',  'Your attacks are slowed by the cold, I think!', '1', 'DREKTHAR_YELL_RANDOM1');
INSERT INTO `script_texts` (`entry` ,`content_default`, `type`, `comment`)VALUES ('-2100004',  'Today, you will meet your ancestors!', '1', 'DREKTHAR_YELL_RANDOM2');
INSERT INTO `script_texts` (`entry` ,`content_default`, `type`, `comment`)VALUES ('-2100005',  'If you will not leave Alterac Valley on your own, then the Frostwolves will force you out!', '1', 'DREKTHAR_YELL_RANDOM3');
INSERT INTO `script_texts` (`entry` ,`content_default`, `type`, `comment`)VALUES ('-2100006',  'You cannot defeat the Frostwolf clan!', '1', 'DREKTHAR_YELL_RANDOM4');
INSERT INTO `script_texts` (`entry` ,`content_default`, `type`, `comment`)VALUES ('-2100007',  'You are no match for the strength of the Horde!', '1', 'DREKTHAR_YELL_RANDOM5');

-- By gnoore

-- BF spawns

-- Start Spawn delete (to many npc's spwaned)
DELETE FROM creature WHERE guid in (62852, 86374, 62856, 62857);
-- Start Spawn position
DELETE FROM creature WHERE guid in (62854, 62855, 85660, 85661, 85659, 86451, 85658, 86450, 85656);
INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) VALUES
(62854, 17397, 542, 3, 0, 0, 30.042412, -89.343559, -40.701576, 2.251515, 7200, 0, 0, 11965, 2486, 0, 0),
(62855, 17397, 542, 3, 0, 0, 23.339996, -80.711876, -41.058521, 5.356349, 7200, 0, 0, 11965, 2486, 0, 0),
(85660, 17371, 542, 3, 0, 0, 22.785934, -89.130089, -41.011791, 0.881933, 7200, 0, 0, 10472, 5875, 0, 0),
(85661, 17371, 542, 3, 0, 0, 30.219334, -81.765175, -40.854816, 3.958347, 7200, 0, 0, 10472, 5875, 0, 0),
(85659, 17371, 542, 3, 0, 0, 7.350070, -88.739212, -41.329384, 2.228894, 7200, 0, 0, 10472, 5875, 0, 0),
(86451, 17624, 542, 3, 0, 0, 0.667157, -88.670082, -41.331841, 0.695013, 7200, 0, 0, 16023, 0, 0, 0),
(85658, 17371, 542, 3, 0, 0, 0.307972, -82.372444, -41.330700, 5.777325, 7200, 0, 0, 10472, 5878, 0, 0),
(86450, 17624, 542, 3, 0, 0, 7.557918, -82.270226, -41.330780, 3.817761, 7200, 0, 0, 16023, 0, 0, 0),
(85656, 17371, 542, 3, 0, 0, -16.737885, -83.297951, -41.334915, 6.216362, 7200, 0, 0, 10472, 5878, 0, 0);

-- rogue spawns
DELETE FROM creature WHERE guid in (86448, 86449); 
INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) VALUES
(86448, 17491, 542, 3, 0, 0, 90.761246, -75.982018, -21.790541, 4.700543, 7200, 0, 0, 7497, 0, 0, 0),
(86449, 17491, 542, 3, 0, 0, 121.554512, -90.037743, -7.579343, 1.503976, 7200, 0, 0, 7497, 0, 0, 0);

-- warlock spawns
DELETE FROM creature WHERE guid in (85662, 85663, 85666, 85667); 
INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) VALUES
(85662, 17371, 542, 3, 0, 0, 231.622818, -103.588478, 9.620442, 1.815211, 7200, 0, 0, 10472, 5878, 0, 0),
(85663, 17371, 542, 3, 0, 0, 230.550308, -98.693748, 9.617454, 5.156088, 7200, 0, 0, 10472, 5878, 0, 0),
(85666, 17371, 542, 3, 0, 0, 222.211090, -69.522270, 9.616614, 5.203213, 7200, 0, 0, 10472, 5878, 0, 0),
(85667, 17371, 542, 3, 0, 0, 225.588425, -75.883057, 9.620467, 2.002712, 7200, 0, 0, 10472, 5878, 0, 0);

-- By AlexMartin

UPDATE `gameobject_template` SET `flags`= 2 WHERE `entry` = 184465; -- The object requiers Cache of the Legion Key

