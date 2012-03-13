-- --------
-- TRIGGER
-- --------

UPDATE creature_template SET spell1 = 40980, flags_extra = 128 WHERE entry = 23095; # molten_flame
UPDATE creature_template SET spell1 = 40117, flags_extra = 128 WHERE entry = 23085; # volcano
UPDATE creature_template SET spell1 = 40836, flags_extra = 128 WHERE entry = 23336; # flame crash
UPDATE creature_template SET spell1 = 40610, flags_extra = 128 WHERE entry = 23259; # blaze
UPDATE creature_template SET spell1 = 40029, flags_extra = 128 WHERE entry = 23069; # demon fire
UPDATE creature_template SET spell1 = 43218, flags_extra = 128 WHERE entry = 24187; # pillar of fire
UPDATE creature_template SET spell1 = 30914, flags_extra = 128 WHERE entry = 17662; # Broggok Poison Cloud

-- --------
-- TARGET
-- --------

-- zulaman
DELETE FROM `spell_script_target` WHERE `entry` IN
(42577,42471,43734,42631);
INSERT INTO `spell_script_target` (`entry`, `type`, `targetEntry`) VALUES
(42471, 1, 23817), # hatch eggs
(43734, 1, 23817), # hatch eggs
(42631, 1, 23920), # fire bomb
(42577, 1, 24136); # zap inform

-- sunwell
DELETE FROM `spell_script_target` WHERE `entry` IN
(44885,45388,45389,46350,45714);
INSERT INTO `spell_script_target` (`entry`, `type`, `targetEntry`) VALUES
(45388, 1, 25038),
(45389, 1, 25265),
(44885, 1, 25160),
(46350, 1, 25160),
(45714, 1, 25038);

-- Magtheridon 
DELETE FROM spell_script_target WHERE `entry` IN
(30531,30657);
INSERT INTO `spell_script_target` (`entry`, `type`, `targetEntry`) VALUES
(30531, 1, 17256), # soul transfer
(30657, 1, 24136); # quake

-- black temple
DELETE FROM `spell_script_target` WHERE `entry` IN
(41455);
INSERT INTO `spell_script_target` (`entry`, `type`, `targetEntry`) VALUES
(41455, 1, 22949), # circle of healing
(41455, 1, 22950),
(41455, 1, 22951),
(41455, 1, 22952);

-- Magisters' Terrace
DELETE FROM spell_script_target WHERE `entry` IN
(44320, 44321);
INSERT INTO `spell_script_target` (`entry`, `type`, `targetEntry`) VALUES
(44320, 1, 24723), # Mana Rage
(44321, 1, 24723); # Mana Rage

DELETE FROM `spell_script_target` WHERE `entry` IN
(30659);
INSERT INTO `spell_script_target` (`entry`, `type`, `targetEntry`) VALUES
(30659, 1, 17281); # Fel Infusion

DELETE FROM `spell_script_target` WHERE `entry` IN
(34186, 42492, 33831, 5628, 45109, 45149);
INSERT INTO `spell_script_target` (`entry`, `type`, `targetEntry`) VALUES
(34186, 1, 16938),
(42492, 1, 0),
(33831, 1, 0),
(5628,  1, 2011),
(5628,  1, 2012),
(5628,  1, 2013),
(5628,  1, 2014),
(45109, 1, 25084),
(45149, 0, 300154);

DELETE FROM `spell_script_target` WHERE entry IN
(33655, 44374);
INSERT INTO `spell_script_target` (`entry`, `type`, `targetEntry`) VALUES
(33655, 0, 183350), # Dropping The Nether Modulator/Gateway Murketh
(33655, 0, 183351),
(44374, 1, 24722);

DELETE FROM `spell_script_target` WHERE entry IN
(8913,9095,12613,34019,34526,36904,38015,38738,39011,39844,40105,40106,42391);
INSERT INTO `spell_script_target` (`entry`, `type`, `targetEntry`) VALUES
(8913,  1, 1200),
(9095,  1, 6492),
(12613, 1, 5843),
(34019, 2, 16873), # raise dead
(34019, 2, 16871),
(34019, 2, 19422),
(34019, 2, 16907),
(34526, 1, 19723),
(34526, 1, 19724),
(36904, 0, 21511),
(38015, 1, 21216),
(38738, 0, 185193), # Activate Legion Obelisk
(38738, 0, 185195),
(38738, 0, 185196),
(38738, 0, 185197),
(38738, 0, 185198),
(39011, 1, 20885),
(39844, 0, 185549),
(40105, 1, 22883),
(40106, 1, 22883),
(42391, 0, 300152);

-- too many...
REPLACE INTO `spell_script_target` (`entry`, `type`, `targetEntry`) VALUES
(3730,1,15263),
(6955,1,4946),
(7022,1,4945),
(7277,1,5409),
(9457,1,3701),
(11893,1,8156),
(12134,2,5270),
(12134,2,5271),
(12134,2,5273),
(12134,2,5256),
(12134,2,5259),
(13489,1,9178),
(15658,1,8929), -- It's up to script to implement SpellHit() for Moira Bronze beard
(16032,1,6557), -- Dummy effect should be implemented in core i think
(16337,1,10339),
(16337,1,10429), -- per spells description, it buffs two npc's.
(16637,1,10447),
(17748,1,10556),
(18969,1,12126),
(19749,1,12352), -- used by creature script to check if all troopers are presents, NYI
(19773,1,12018), -- used by Ragnaros to kill Majordomo
(21556,1,13378), -- no longer in game, but still could be used for some FUN-so why not fix it ? ;)
(21566,1,13416), 
(22710,1,14349),
(23168,1,14020),
(24062,1,15010),
(24217,1,15069),
(24323,1,14834),
(24804,1,14888),
(25896,1,15299),
(28096,1,15929),
(28111,1,15930),
(28353,2,16390),
(28392,1,16420),
(28697,1,3976),
(28861,1,16592),
(29172,0,181605),
(29456,1,17060),
(29457,1,17060),
(29459,1,17060), -- Those are spells Arch Mage Xintor casts at training dummies near him.
(29531,0,181605),
(29831,0,181288),
(30221,1,620),
(30232,1,15691),
(30460,1,17404),
(30951,1,17533), 
(31326,1,15608),
(31611,1,17979),
(31727,1,17536),
(32301,1,18371),
(32396,1,18358),
(32890,1,18764),
(33111,1,17400),
(33111,1,18894),
(33644,1,19279),
(33644,1,19328),
(33644,1,19278),
(33644,1,19329),
(33644,1,19326),
(33644,1,19277),
(33644,1,19276), -- All legion antennas.
(33742,1,17400),
(33742,1,18894),
(33742,1,19279),
(33742,1,19328),
(33742,1,19278),
(33742,1,19329),
(33742,1,19326),
(33742,1,19277),
(33742,1,19276), -- All legion antennas, another spell.
(33783,1,18732),
(34254,1,19554),
(34350,1,19554),
(34662,1,17827),
(35596,1,20794),
(36174,1,20885),
(37465,1,17469),
(37465,1,21747),
(37465,1,21750),
(37465,1,21748),
(37469,1,21664),
(37469,1,21682),
(37469,1,21683),
(37469,1,17211),
(37626,1,15689), -- Netherspite's beam
(37853,1,15608),
(37868,1,21909),
(37893,1,21909),
(37895,1,21909),
(38003,1,21949),
(38044,1,21212),
(38121,1,21949),
(38123,1,21949),
(38126,1,21949),
(38128,1,21949),
(38130,1,21949),
(38250,0,185125),
(38508,1,17270),
(38508,1,16873),
(38508,1,17269),
(38508,1,16871),
(38508,1,19422),
(38508,1,16907),
(38508,1,17478),
(38508,1,19424),
(38629,0,185214),
(38722,1,21506),
(38966,1,22293),
(39124,0,184738),
(39126,1,19557),
(39221,1,23116),
(39364,1,19732),
(39601,1,22916),
(39974,0,300127),
(39978,1,21851),
(39993,1,23382),
(40359,1,23382),
(40454,1,22181),
(40547,1,22911),
(40828,1,23327),
(40828,1,23322),
(41128,1,23164),
(41362,1,22956),
(41362,1,22964),
(41975,1,23191),
(42317,1,22844),
(42317,1,23319),
(42317,1,23421),
(42317,1,23216),
(42317,1,23523),
(42317,1,22849),
(42317,1,22845),
(42317,1,22847),
(42317,1,23318),
(42317,1,23215),
(42317,1,23524),
(42317,1,23374),
(42317,1,22846),
(42318,1,22844),
(42318,1,23319),
(42318,1,23421),
(42318,1,23216),
(42318,1,23523),
(42318,1,22849),
(42318,1,22845),
(42318,1,22847),
(42318,1,23318),
(42318,1,23215),
(42318,1,23524),
(42318,1,23374),
(42318,1,22846),
(42405,1,23775),
(42410,1,23775),
(42517,1,23864),
(42734,1,4974),
(44864,1,24955),
(45201,1,24882),
(46809,1,26239), -- Make Ahune's Ghost Burst
(46818,1,25840),
(46852,0,181605), -- Ribbon Pole Music
(46896,0,181605),
(47104,1,26401),
(49058,1,24968); -- Rocket Bot Attack

-- --------
-- POSITION
-- --------
DELETE FROM spell_target_position WHERE `id` IN (46019, 46020, 22563, 22564);
INSERT INTO spell_target_position () VALUES (46019, 580, 1704.34, 928.17, -74.558, 0);
INSERT INTO spell_target_position () VALUES (46020, 580, 1704.34, 928.17, 53.079, 0);
INSERT INTO spell_target_position () VALUES (22563, 30, -1347.04, -291.15, 90.8, 1.82);
INSERT INTO spell_target_position () VALUES (22564, 30, 656.197, -30.75, 49.1, 0.24);