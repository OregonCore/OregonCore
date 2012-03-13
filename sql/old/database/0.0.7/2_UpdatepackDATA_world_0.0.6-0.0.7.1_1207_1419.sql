-- ODB0.0.7 1/3 Release database updatepack
-- Database for Oregoncore 2.4.3 core rev 1391+

-- Update the version table in database
UPDATE `version` SET `db_version`='ODB 0.0.7 1/3  for OregonCore rev. 1391';

-- 
-- World Zones redone (Thx to Darkgroom for supplying the data, Dave for redoing the zones)
-- 
-- 
-- Sunstrider Isle (Zone tested by Sharky)
-- 


-- Sunstrider Isle Broom waypoint (guid 61727)(Trainers room below)

-- Modify Spawnpoint


UPDATE `creature` SET `position_x`='10353.8' WHERE (`guid`='61727');
UPDATE `creature` SET `position_y`='-6398.98' WHERE (`guid`='61727');
UPDATE `creature` SET `position_z`='38.531' WHERE (`guid`='61727');

-- Modify creature template

UPDATE `creature` SET `MovementType` = 2 WHERE `guid` = 61727;

-- Set Waypoints

DELETE FROM `waypoint_data` WHERE `id`=61727;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES 
(61727,1,10353.8,-6398.98,38.531,0,0,0,100,0), 
(61727,2,10347.125000,-6403.728027,38.831261,0,0,0,100,0),
(61727,3,10345.213867,-6412.625000,38.531261,0,0,0,100,0), 
(61727,4,10348.875977,-6424.502930,38.351261,0,0,0,100,0),
(61727,5,10358.709961,-6428.891113,38.533386,0,0,0,100,0), 
(61727,6,10370.663086,-6426.270020,38.533386,0,0,0,100,0),
(61727,7,10377.588867,-6412.257324,38.531334,5000,0,0,100,0), 
(61727,8,10370.580078,-6398.679688,38.531334,0,0,0,100,0),
(61727,9,10360.548828,-6397.175781,38.531334,0,0,0,100,0), 
(61727,10,10360.427734,-6405.211426,38.531364,10000,0,0,100,0),
(61727,11,10356.733398,-6406.944824,38.531364,0,0,0,100,0), 
(61727,12,10353.8,-6398.98,38.531,10000,0,0,100,0);

-- Add creature_addon data
DELETE FROM `creature_addon` WHERE `guid`=61727;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes0`,`bytes1`,`bytes2`,`emote`,`moveflags`,`isActive`,`auras`) VALUES
(61727,61727,0,0,0,0,0,0,1,NULL);


-- Sunstrider Isle Broom waypoint (guid 61728)(Trainers room above + slide)

-- Modify Spawnpoint


UPDATE `creature` SET `position_x`='10411.032227' WHERE (`guid`='61728');
UPDATE `creature` SET `position_y`='-6395.876953' WHERE (`guid`='61728');
UPDATE `creature` SET `position_z`='52.093201' WHERE (`guid`='61728');

-- Modify creature template

UPDATE `creature` SET `MovementType` = 2 WHERE `guid` = 61728;

-- Set Waypoints

DELETE FROM `waypoint_data` WHERE `id`=61728;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES 
(61728,1,10411.032227,-6395.876953,52.093201,10000,0,0,100,0), 
(61728,2,10404.644531,-6398.096191,52.093201,0,0,0,100,0),
(61728,3,10399.685547,-6399.819336,51.787292,0,0,0,100,0), 
(61728,4,10393.080078,-6402.114746,49.721786,0,0,0,100,0),
(61728,5,10381.958008,-6405.979004,49.717682,10000,0,0,100,0),
(61728,6,10383.621094,-6414.552734,49.639893,0,0,0,100,0),
(61728,7,10380.121094,-6426.050781,47.340324,0,0,0,100,0),
(61728,8,10366.552734,-6434.494629,44.131252,0,0,0,100,0),
(61728,9,10349.926758,-6433.235840,40.216934,0,0,0,100,0),
(61728,10,10342.645508,-6427.344238,38.534378,15000,0,0,100,0),
(61728,11,10349.926758,-6433.235840,40.216934,0,0,0,100,0),
(61728,12,10366.552734,-6434.494629,44.131252,0,0,0,100,0),
(61728,13,10380.121094,-6426.050781,47.340324,0,0,0,100,0),
(61728,14,10383.621094,-6414.552734,49.639893,0,0,0,100,0),
(61728,15,10381.958008,-6405.979004,49.717682,10000,0,0,100,0),
(61728,16,10393.080078,-6402.114746,49.721786,0,0,0,100,0),
(61728,17,10399.685547,-6399.819336,51.787292,0,0,0,100,0),
(61728,18,10404.644531,-6398.096191,52.093201,0,0,0,100,0),
(61728,19,10411.032227,-6395.876953,52.093201,0,0,0,100,0); 


-- Add creature_addon data
DELETE FROM `creature_addon` WHERE `guid`=61728;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes0`,`bytes1`,`bytes2`,`emote`,`moveflags`,`isActive`,`auras`) VALUES
(61728,61728,0,0,0,0,0,0,1,NULL);


-- Cat @ the gazebo
-- Modify Spawnpoint

UPDATE `creature` SET `position_x`='10396.447266' WHERE (`guid`='40359');
UPDATE `creature` SET `position_y`='-6401.230469' WHERE (`guid`='40359');
UPDATE `creature` SET `position_z`='49.986477' WHERE (`guid`='40359');
UPDATE `creature` SET `spawndist`='2' WHERE (`guid`='40359');



-- Cat groundlevel Trainers room
-- Modify Spawnpoint

DELETE FROM `creature` WHERE `guid`=40354;
INSERT INTO creature VALUES 
('40354', 6368, 530, 1, 0, 0,"10349.03","-6418.176","38.53265","5.03261", 25, 3, 0,1,0,0,1);

-- Cat groundlevel outside right of the building
-- Modify Spawnpoint
UPDATE `creature` SET `position_x`='10335.924805' WHERE (`guid`='40353');
UPDATE `creature` SET `position_y`='-6376.458984' WHERE (`guid`='40353');
UPDATE `creature` SET `position_z`='35.286633' WHERE (`guid`='40353');

-- Delete a cat that shouldnt be there

DELETE FROM `creature` WHERE `guid`=40356;
DELETE FROM `creature` WHERE `guid`=40357;

-- Delete a broom that should be removed

DELETE FROM `creature` WHERE `guid`=61730;

-- Give another broom a better spawnpoint
UPDATE `creature` SET `position_x`='10422.030273' WHERE (`guid`='61729'); 
UPDATE `creature` SET `position_y`='-6196.692383' WHERE (`guid`='61729'); 
UPDATE `creature` SET `position_z`='30.610554' WHERE (`guid`='61729'); 


-- Broom Platform 1 waypoint
-- Modify creature template

UPDATE `creature` SET `MovementType` = 2 WHERE `guid` = 61726;

-- Set Waypoints

DELETE FROM `waypoint_data` WHERE `id`=61726;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES 
(61726,1,10221.338867,-6362.390137,30.840757,0,0,0,100,0), 
(61726,2,10218.908203,-6362.454102,30.840757,0,0,0,100,0),
(61726,3,10223.108398,-6364.964355,28.802834,0,0,0,100,0), 
(61726,4,10221.035156,-6364.075684,30.840757,0,0,0,100,0),
(61726,5,10219.496094,-6364.810547,30.840757,0,0,0,100,0),
(61726,6,10219.987305,-6365.649903,30.840757,0,0,0,100,0);



-- Add creature_addon data
DELETE FROM `creature_addon` WHERE `guid`=61726;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes0`,`bytes1`,`bytes2`,`emote`,`moveflags`,`isActive`,`auras`) VALUES
(61726,61726,0,0,0,0,0,0,1,NULL);

-- 
-- Zone creature redone
-- 

-- 
-- Mana wyrms
-- 

-- First we will delete all the mana wyrms from the game

DELETE FROM `creature` WHERE `guid`=54931;
DELETE FROM `creature` WHERE `guid`=54932;
DELETE FROM `creature` WHERE `guid`=54933;
DELETE FROM `creature` WHERE `guid`=54934;
DELETE FROM `creature` WHERE `guid`=54935;
DELETE FROM `creature` WHERE `guid`=54936;
DELETE FROM `creature` WHERE `guid`=54937;
DELETE FROM `creature` WHERE `guid`=54938;
DELETE FROM `creature` WHERE `guid`=54939;
DELETE FROM `creature` WHERE `guid`=54940;
DELETE FROM `creature` WHERE `guid`=54941;
DELETE FROM `creature` WHERE `guid`=54942;
DELETE FROM `creature` WHERE `guid`=54943;
DELETE FROM `creature` WHERE `guid`=54944;
DELETE FROM `creature` WHERE `guid`=54945;
DELETE FROM `creature` WHERE `guid`=54946;
DELETE FROM `creature` WHERE `guid`=54947;
DELETE FROM `creature` WHERE `guid`=54948;
DELETE FROM `creature` WHERE `guid`=54949;
DELETE FROM `creature` WHERE `guid`=54950;
DELETE FROM `creature` WHERE `guid`=54951;
DELETE FROM `creature` WHERE `guid`=54952;
DELETE FROM `creature` WHERE `guid`=54953;
DELETE FROM `creature` WHERE `guid`=54954;
DELETE FROM `creature` WHERE `guid`=54955;
DELETE FROM `creature` WHERE `guid`=54956;
DELETE FROM `creature` WHERE `guid`=54957;
DELETE FROM `creature` WHERE `guid`=54958;
DELETE FROM `creature` WHERE `guid`=54959;
DELETE FROM `creature` WHERE `guid`=54960;
DELETE FROM `creature` WHERE `guid`=54961;
DELETE FROM `creature` WHERE `guid`=54962;
DELETE FROM `creature` WHERE `guid`=54963;
DELETE FROM `creature` WHERE `guid`=54964;
DELETE FROM `creature` WHERE `guid`=54965;
DELETE FROM `creature` WHERE `guid`=54966;
DELETE FROM `creature` WHERE `guid`=54967;
DELETE FROM `creature` WHERE `guid`=54968;
DELETE FROM `creature` WHERE `guid`=54969;
DELETE FROM `creature` WHERE `guid`=54970;
DELETE FROM `creature` WHERE `guid`=54971;
DELETE FROM `creature` WHERE `guid`=54972;
DELETE FROM `creature` WHERE `guid`=54973;
DELETE FROM `creature` WHERE `guid`=54974;
DELETE FROM `creature` WHERE `guid`=54975;
DELETE FROM `creature` WHERE `guid`=54976;
DELETE FROM `creature` WHERE `guid`=54977;
DELETE FROM `creature` WHERE `guid`=54978;
DELETE FROM `creature` WHERE `guid`=54979;
DELETE FROM `creature` WHERE `guid`=54980;
DELETE FROM `creature` WHERE `guid`=54981;
DELETE FROM `creature` WHERE `guid`=54982;
DELETE FROM `creature` WHERE `guid`=54983;

-- Here we will add the ones we need back in an give them their old guid back

INSERT INTO creature VALUES ('54931', 15274, 530, 1, 0, 0,"10311.71","-6282.17","27.36133","5.030614", 300, 3, 0,41,60,0,1);
INSERT INTO creature VALUES ('54932', 15274, 530, 1, 0, 0,"10378.67","-6271.094","26.4888","6.24693", 300, 3, 0,41,60,0,1);
INSERT INTO creature VALUES ('54933', 15274, 530, 1, 0, 0,"10320.77","-6312.577","26.42671","0.459946", 300, 3, 0,41,60,0,1);
INSERT INTO creature VALUES ('54934', 15274, 530, 1, 0, 0,"10266.53","-6340.682","26.30477","6.079172", 300, 3, 0,41,60,0,1);
INSERT INTO creature VALUES ('54935', 15274, 530, 1, 0, 0,"10287.84","-6356.323","23.1483","2.138781", 300, 3, 0,41,60,0,1);
INSERT INTO creature VALUES ('54936', 15274, 530, 1, 0, 0,"10320.8","-6395.423","36.08072","4.792134", 300, 3, 0,41,60,0,1);
INSERT INTO creature VALUES ('54937', 15274, 530, 1, 0, 0,"10388.36","-6317.924","34.71228","5.049587", 300, 3, 0,41,60,0,1);
INSERT INTO creature VALUES ('54938', 15274, 530, 1, 0, 0,"10268.17","-6350.29","26.86289","4.997231", 300, 3, 0,41,60,0,1);
INSERT INTO creature VALUES ('54939', 15274, 530, 1, 0, 0,"10246.05","-6353.893","30.84076","0.7470509", 300, 3, 0,41,60,0,1);
INSERT INTO creature VALUES ('54940', 15274, 530, 1, 0, 0,"10209.76","-6354.389","31.98550","0.8150207", 300, 3, 0,41,60,0,1);
INSERT INTO creature VALUES ('54941', 15274, 530, 1, 0, 0,"10213.26","-6338.734","30.84076","0.9268795", 300, 3, 0,41,60,0,1);
INSERT INTO creature VALUES ('54942', 15274, 530, 1, 0, 0,"10221.2","-6328.96","33.95550","3.688345", 300, 3, 0,41,60,0,1);
INSERT INTO creature VALUES ('54943', 15274, 530, 1, 0, 0,"10236.46","-6377.031","29.17427","6.265964", 300, 3, 0,41,60,0,1);
INSERT INTO creature VALUES ('54944', 15274, 530, 1, 0, 0,"10233.81","-6362.991","30.84076","1.769372", 300, 3, 0,41,60,0,1);
INSERT INTO creature VALUES ('54945', 15274, 530, 1, 0, 0,"10231.62","-6329.838","30.84076","0.2626987", 300, 3, 0,41,60,0,1);
INSERT INTO creature VALUES ('54946', 15274, 530, 1, 0, 0,"10244.72","-6339.787","30.95550","2.287171", 300, 3, 0,41,60,0,1);
INSERT INTO creature VALUES ('54947', 15274, 530, 1, 0, 0,"10229.72","-6364.832","31.20000","0.5816966", 300, 3, 0,41,60,0,1);
INSERT INTO creature VALUES ('54948', 15274, 530, 1, 0, 0,"10223.62","-6377.72","29.643","3.107049", 300, 3, 0,41,60,0,1);
INSERT INTO creature VALUES ('54949', 15274, 530, 1, 0, 0,"10220.54","-6363.885","31.99999","1.565543", 300, 3, 0,41,60,0,1);
INSERT INTO creature VALUES ('54950', 15274, 530, 1, 0, 0,"10256.29","-6302.607","23.37003","5.046891", 300, 3, 0,41,60,0,1);
INSERT INTO creature VALUES ('54951', 15274, 530, 1, 0, 0,"10228.37","-6344.428","30.84076","0.3363729", 300, 3, 0,41,60,0,1);
INSERT INTO creature VALUES ('54952', 15274, 530, 1, 0, 0,"10208.66","-6312.733","27.91273","6.223369", 300, 3, 0,41,60,0,1);
INSERT INTO creature VALUES ('54953', 15274, 530, 1, 0, 0,"10187.07","-6336.835","28.69171","4.348096", 300, 3, 0,41,60,0,1);
INSERT INTO creature VALUES ('54954', 15274, 530, 1, 0, 0,"10203.83","-6363.651","30.18713","5.837131", 300, 3, 0,41,60,0,1);
INSERT INTO creature VALUES ('54955', 15274, 530, 1, 0, 0,"10183","-6347.578","29.99496","2.225435", 300, 3, 0,41,60,0,1);
INSERT INTO creature VALUES ('54956', 15274, 530, 1, 0, 0,"10377.6","-6249.062","27.42092","2.14738", 300, 3, 0,41,60,0,1);
INSERT INTO creature VALUES ('54957', 15274, 530, 1, 0, 0,"10397.68","-6215.625","31.61036","0", 300, 3, 0,41,60,0,1);
INSERT INTO creature VALUES ('54958', 15274, 530, 1, 0, 0,"10378.92","-6185.218","33.18889","6.241807", 300, 3, 0,41,60,0,1);
INSERT INTO creature VALUES ('54959', 15274, 530, 1, 0, 0,"10369.59","-6199.956","30.05374","0.1143247", 300, 3, 0,41,60,0,1);
INSERT INTO creature VALUES ('54960', 15274, 530, 1, 0, 0,"10403.82","-6175.932","34.18889","2.336731", 300, 3, 0,41,60,0,1);
INSERT INTO creature VALUES ('54961', 15274, 530, 1, 0, 0,"10410.2","-6164.336","32.25550","2.626083", 300, 3, 0,41,60,0,1);
INSERT INTO creature VALUES ('54962', 15274, 530, 1, 0, 0,"10410.54","-6234.54","33.28502","4.661747", 300, 3, 0,41,60,0,1);
INSERT INTO creature VALUES ('54963', 15274, 530, 1, 0, 0,"10415.61","-6259.028","31.03613","0.06259188", 300, 3, 0,41,60,0,1);
INSERT INTO creature VALUES ('54964', 15274, 530, 1, 0, 0,"10400.49","-6159.407","34.18889","2.631742", 300, 3, 0,41,60,0,1);
INSERT INTO creature VALUES ('54965', 15274, 530, 1, 0, 0,"10423.98","-6212.85","33.59828","3.310528", 300, 3, 0,41,60,0,1);
INSERT INTO creature VALUES ('54966', 15274, 530, 1, 0, 0,"10416.61","-6153.847","33.93004","4.077275", 300, 3, 0,41,60,0,1);
INSERT INTO creature VALUES ('54967', 15274, 530, 1, 0, 0,"10411.39","-6200.442","34.35550","2.99999", 300, 3, 0,41,60,0,1);
INSERT INTO creature VALUES ('54968', 15274, 530, 1, 0, 0,"10424.37","-6180.377","35.18889","5.013412", 300, 3, 0,41,60,0,1);
INSERT INTO creature VALUES ('54969', 15274, 530, 1, 0, 0,"10412.79","-6176.365","35.35550","5.613909", 300, 3, 0,41,60,0,1);
INSERT INTO creature VALUES ('54970', 15274, 530, 1, 0, 0,"10437.35","-6188.007","35.88813","3.392067", 300, 3, 0,41,60,0,1);
INSERT INTO creature VALUES ('54971', 15274, 530, 1, 0, 0,"10363.39","-6172.781","32.42214","3.928918", 300, 3, 0,41,60,0,1);
INSERT INTO creature VALUES ('54972', 15274, 530, 1, 0, 0,"10400.56","-6208.659","32.18889","5.601867", 300, 3, 0,41,60,0,1);
INSERT INTO creature VALUES ('54973', 15274, 530, 1, 0, 0,"10368.5","-6180.744","32.47086","4.695586", 300, 3, 0,41,60,0,1);
INSERT INTO creature VALUES ('54974', 15274, 530, 1, 0, 0,"10388.96","-6194.804","34.35550","3.668097", 300, 3, 0,41,60,0,1);
INSERT INTO creature VALUES ('54975', 15274, 530, 1, 0, 0,"10393.41","-6170.836","32.18889","1.19574", 300, 3, 0,41,60,0,1);
INSERT INTO creature VALUES ('54976', 15274, 530, 1, 0, 0,"10379.43","-6152.973","32.29041","5.568189", 300, 3, 0,41,60,0,1);
INSERT INTO creature VALUES ('54977', 15274, 530, 1, 0, 0,"10371.59","-6153.272","31.5267","2.97683", 300, 3, 0,41,60,0,1);

-- 
-- Remove some wrong spawns
-- 

DELETE FROM `creature` WHERE `guid`=54870;
DELETE FROM `creature` WHERE `guid`=55052;
DELETE FROM `creature` WHERE `guid`=55009;

-- 
-- Minor correction
-- 

UPDATE `creature` SET `spawndist`='15' WHERE (`guid`='55755');

-- 
-- Eversong woods
-- 

-- 
-- Ghostlands
-- 


--
-- Instances
--
-- 
-- The deadmines
--

-- Set correct model for Captain Greenskin
DELETE FROM `creature_model_info` WHERE (`modelid`=7113);
INSERT INTO `creature_model_info` (`modelid`, `bounding_radius`, `combat_reach`, `gender`, `modelid_other_gender`) VALUES (7113, 0.459, 2.25, 0, 0); 

-- 
-- Ragefire Chasm
-- 

-- 
-- Various fixes in parts of the world
-- 
-- 
-- Adjustments to the pool system
-- 

-- 
-- Fix critters HP (they should die after 1 hit)
-- 



--
-- Various fixes from the forums (thx to Trentone, Zlokobni,  Afterdawn, DigerAgo, Stfx, Desteny, Gizmo, Henhouse, MiLK, Gnoore & Zaxarpal for contributing)
--

-- NPC 22461 - Fel Cannon MKI (makes the cannon unmoveable)
UPDATE `creature_template` SET `unit_flags`='4' WHERE `entry`='22461'; 

-- Correct ExtendedCost for S3 Shoulders
UPDATE `npc_vendor` SET `ExtendedCost` = 2392  
WHERE `item` IN ( 33699, 33682, 33679, 33674, 33668, 33770, 33757, 33753, 33693, 33732, 33720, 33715, 33710, 33703, 33726, 33770, 33757, 33753, 33732, 33726, 33720, 33715, 33710, 33668, 33674, 33679, 33682, 33693, 33699, 33703, 33770, 33757, 33753, 33732, 33726, 33720, 33715, 33710, 33703, 33699, 33693, 33682, 33679, 33674, 33668 ) ;

-- Update correct orientation when leaving Scholomance instance
UPDATE `areatrigger_teleport` SET `target_orientation`='3.666' WHERE (`id`='2568');

-- Update correct orientation when leaving Blackrock Spire instance
UPDATE `areatrigger_teleport` SET `target_orientation`='2.1' WHERE (`id`='1470');

-- Update correct orientation when entering Ahn'Qiraj Temple
UPDATE `areatrigger_teleport` SET `target_orientation`='1' WHERE (`id`='4010');

-- Update correct orientation when leaving Ahn'Giraj Temple
UPDATE `areatrigger_teleport` SET `target_orientation`='4.03' WHERE (`id`='4012');

-- Update correct orientation when entering Ruins of Ahn'Qiraj
UPDATE `areatrigger_teleport` SET `target_orientation`='2.67' WHERE (`id`='4008');

-- Update correct orientation when leaving Ruins of Ahn'Qiraj
UPDATE `areatrigger_teleport` SET `target_orientation`='5.7' WHERE (`id`='4006');

-- Better orientation when entering Blackwing Lair
UPDATE `areatrigger_teleport` SET `target_orientation`='0.616' WHERE (`id`='3726');

-- Cast Haleh's Will kick spell on player when accepting quest "Wrath of the Blue Flight"
UPDATE `quest_template` SET `SrcSpell`='17168' WHERE (`entry`='5162');

-- Fix final effect of Haleh's Will spell
UPDATE `spell_target_position` SET `id`='17167', `target_map`='0', `target_position_x`='1617.728', `target_position_y`='-2637.31', `target_position_z`='91', `target_orientation`='6.2' where `id`='17167';

-- Captain Blackanvil Spawn
DELETE FROM `creature` WHERE `id`=15440; 
INSERT INTO `creature` (`guid`,`id`,`map`,`spawnMask`,`modelid`,`equipment_id`,`position_x`,`position_y`,`position_z`,`orientation`,`spawntimesecs`,`spawndist`,`currentwaypoint`,`curhealth`,`curmana`,`DeathState`,`MovementType`) VALUES 
(150185, 15440, 1, 1, 0, 0, -7168.18, 1380.71, 2.91784, 1.09484, 540, 0, 0, 1, 0, 0, 0);

-- Fix quest 10409
UPDATE `quest_template` SET `SpecialFlags` = 0, `ReqCreatureOrGOCount1` = 1, `ReqCreatureOrGOId1` = 20132 WHERE `entry` = 10409;

-- Fix Leech honor in Arkatraz
DELETE FROM creature_onkill_reputation WHERE creature_id = 21346;

-- Fix related stuff to Scryer's Orb
-- Play cinematic when using the Scryer's Orb 
UPDATE gameobject_template SET type=13, data1=164, data2=16547 WHERE entry = 187578; 
 
-- Event script for Kalecgos to spawn and then come flying to the orb (core script) 
INSERT INTO `event_scripts` (`id`, `delay`, `command`, `datalong`, `datalong2`, `dataint`, `x`, `y`, `z`, `o`) VALUES (16547, 0, 10, 24844, 300000, 0, 209.912, -318.697, 14.005, 1.181); 
 
-- Set InhabitType of Kalecgos (Dragon) to flyable so that his flying movement will be smooth 
UPDATE creature_template SET InhabitType=5 WHERE entry = 24844;

-- Fixes Mages Pet's missing spells (Spell for fire elemental is missing)
-- Spells for Mage's Water Elemental 
UPDATE `creature_template` SET spell1=31707, spell2=33395 WHERE `entry`=510; 
-- Spells for Shaman's Earth Elemental 
UPDATE `creature_template` SET spell1=36213 WHERE `entry`=15352; 
-- Spells for Shaman's Fire Elemental 
UPDATE `creature_template` SET spell1=12470, spell2=13376 WHERE `entry`=15438;

-- Add/Cleanup DB Scripts
-- Add completion script of quest 407 and 492 (from TC2) 
INSERT INTO `quest_end_scripts` VALUES 
(407, 0, 0, 0, 0, 2000000082, 0, 0, 0, 0), 
(407, 3, 3, 0, 0, 0, 2287.97, 236.253, 27.0892, 2.6613), 
(407, 3, 15, 3287, 1, 0, 0, 0, 0, 0), 
(407, 6, 3, 0, 0, 0, 2292, 239.481, 27.0892, 0.693878), 
(407, 8, 0, 0, 0, 2000000083, 0, 0, 0, 0), 
(407, 10, 3, 0, 0, 0, 2292.52, 235.226, 27.0892, 4.8345), 
(407, 13, 3, 0, 0, 0, 2288.96, 237.96, 27.0892, 2.48773), 
(407, 19, 15, 5, 1, 0, 0, 0, 0, 0), 
(492, 1, 0, 0, 0, 2000000084, 0, 0, 0, 0), 
(492, 2, 1, 7, 0, 0, 0, 0, 0, 0), 
(492, 6, 15, 5, 1, 0, 0, 0, 0, 0); 
 
INSERT INTO `db_script_string` VALUES 
(2000000082, 'I. . .I. . .don\'t. . .feel. . .right. . .', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL), 
(2000000083, 'My mind. . .my flesh. . .I\'m. . .rotting. . . .!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL), 
(2000000084, 'I raise my brew and hope to be rid of the likes of you! Cheers, you no good scoundrel, $N!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL); 
 
UPDATE `quest_template` SET `CompleteScript`=407 WHERE `entry`=407; 
UPDATE `quest_template` SET `CompleteScript`=492 WHERE `entry`=492; 
 
-- OC change: Let them respawn right after they despawn (usually 1min) because it was set to 300secs and thats way too long 
UPDATE `creature` SET `spawntimesecs`=0 WHERE `id` IN(1931,2211);

-- Remove db script for quest The Absent Minded Prospector as we already have a script for it 
UPDATE `quest_template` SET `StartScript`=0 WHERE `entry`=731; 
DELETE FROM `quest_start_scripts` WHERE `id`=731; 
DELETE FROM `db_script_string` WHERE `entry` BETWEEN 2000000004 AND 2000000008; 
 
-- Remove unused entry 
DELETE FROM `db_script_string` WHERE `entry`=2000000027; 
 
-- Correct spelling of one entry 
UPDATE `db_script_string` SET `content_default`='Oooooo hot hot hot! If that won\'t put spring in your step, I don\'t know what will!' WHERE `entry`=2000000046;

-- Fix elevator in Coilfang Reservoir
DELETE FROM `gameobject` WHERE `id`=183407; 
INSERT INTO `gameobject` (`guid`,`id`,`map`,`spawnMask`,`position_x`,`position_y`,`position_z`,`orientation`,`rotation0`,`rotation1`,`rotation2`,`rotation3`,`spawntimesecs`,`animprogress`,`state`) VALUES 
(13487, 183407, 548, 1, 50, -0.0071, 820.2, 3.15, 0, 0, 0.005, 0.001, 500, 100, 1);

-- Fix Mordenai's Faction, health & Mana
UPDATE creature_template SET minhealth = 86160 WHERE entry = 22113; 
UPDATE creature_template SET maxhealth = 86160 WHERE entry = 22113; 
UPDATE creature_template SET minmana = 3231 WHERE entry = 22113; 
UPDATE creature_template SET maxmana = 3231 WHERE entry = 22113;

-- Fix for Quest 10438
UPDATE `creature_template` SET `unit_flags` = 4, `RegenHealth` = 0 WHERE `entry` = 20899;

-- Fix Raque tier6 bonus x4
UPDATE `spell_affect` set `SpellFamilyMask`=60171485190 WHERE `entry`=38389;

-- Natural Shapeshifter affects Tree of life spell
UPDATE `spell_affect` set `SpellFamilyMask`=527769372983296 WHERE `entry`=16833; 
UPDATE `spell_affect` set `SpellFamilyMask`=527769372983296 WHERE `entry`=16834; 
UPDATE `spell_affect` set `SpellFamilyMask`=527769372983296 WHERE `entry`=16835;

-- Paladin Benediction now affects Judgement spells
UPDATE `spell_affect` set `SpellFamilyMask`=4398222672384 WHERE `entry`=20101; 
UPDATE `spell_affect` set `SpellFamilyMask`=4398222672384 WHERE `entry`=20102; 
UPDATE `spell_affect` set `SpellFamilyMask`=4398222672384 WHERE `entry`=20103; 
UPDATE `spell_affect` set `SpellFamilyMask`=4398222672384 WHERE `entry`=20104; 
UPDATE `spell_affect` set `SpellFamilyMask`=4398222672384 WHERE `entry`=20105;

-- Empowered healing now affects Binding Heal spell
UPDATE `spell_affect` set `SpellFamilyMask`=17179871232 WHERE `entry`=33158 AND `effectId`=1; 
UPDATE `spell_affect` set `SpellFamilyMask`=17179871232 WHERE `entry`=33159 AND `effectId`=1; 
UPDATE `spell_affect` set `SpellFamilyMask`=17179871232 WHERE `entry`=33160 AND `effectId`=1; 
UPDATE `spell_affect` set `SpellFamilyMask`=17179871232 WHERE `entry`=33161 AND `effectId`=1; 
UPDATE `spell_affect` set `SpellFamilyMask`=17179871232 WHERE `entry`=33162 AND `effectId`=1;

-- Shaman Talent Elemental Weapons
update spell_affect set `SpellFamilyMask`=18874368 where `effectId`=1 and `entry`=29080; 
update spell_affect set `SpellFamilyMask`=18874368 where `effectId`=1 and `entry`=29079; 
update spell_affect set `SpellFamilyMask`=18874368 where `effectId`=1 and `entry`=16266;

-- Improved Mend Pet
UPDATE spell_proc_event set `procFlags`=262144 where `SpellFamilyName`=9 and `SpellFamilyMask`=8388608;

-- Priest Talent Blackout Redone
UPDATE spell_proc_event set `procFlags`=65536 and `SpellFamilyMask`=5506224136192 where `SpellFamilyMask`=101228544 and `SpellFamilyName`=6;

-- Water Elementals can now be attacked again
UPDATE creature_template set `unit_flags`=0 where `entry`=510;

-- Clever Traps fixes 1th rank
UPDATE `spell_affect` SET `SpellFamilyMask`=35184372088836 WHERE `effectId`=1 AND `entry`=19239;


-- 
-- Fix Startup errors (Db-side)
-- 

UPDATE `creature` SET `curhealth`='3750' WHERE (`guid`='134401');
UPDATE `creature` SET `curhealth`='3750' WHERE (`guid`='134402');
UPDATE `creature` SET `curhealth`='3750' WHERE (`guid`='134403');
UPDATE `creature` SET `curhealth`='3750' WHERE (`guid`='134404');
UPDATE `creature` SET `curhealth`='3750' WHERE (`guid`='134405');

-- 
-- EOF
-- 
