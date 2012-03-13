-- ODB0.0.7 part 2/3 (Partial release due to core problems)
-- http://www.oregoncore.com
-- Bugs should be reported here -> http://www.oregoncore.com/index.php/tracker/project-2-database-bug-tracker/
-- Database for Oregoncore 2.4.3 core rev 1419+

-- Update the version table in database
UPDATE `version` SET `db_version`='ODB 0.0.7 2/3 for OregonCore rev. 1419+';

-- 
-- Script errors fixed
-- 

-- Mob fiendish portal

update `creature_template` set `ScriptName`='mob_fiendish_portal' where `entry`='17265' ;

-- Item Inolucating crystal
update `item_template` set `ScriptName`='item_inoculating_crystal' where `entry`='22962' ;

-- Gameobjects Ravager cage, bristlelimb cage & najentus spine

update `gameobject_template` set `ScriptName`='go_ravager_cage' where `entry`='181849' ;

update `gameobject_template` set `ScriptName`='go_bristlelimb_cage' where `entry`='181714' ;
update `gameobject_template` set `ScriptName`='go_bristlelimb_cage' where `entry`='181720' ;
update `gameobject_template` set `ScriptName`='go_bristlelimb_cage' where `entry`='181721' ;
update `gameobject_template` set `ScriptName`='go_bristlelimb_cage' where `entry`='181722' ;
update `gameobject_template` set `ScriptName`='go_bristlelimb_cage' where `entry`='181723' ;
update `gameobject_template` set `ScriptName`='go_bristlelimb_cage' where `entry`='181724' ;
update `gameobject_template` set `ScriptName`='go_bristlelimb_cage' where `entry`='181725' ;
update `gameobject_template` set `ScriptName`='go_bristlelimb_cage' where `entry`='181726' ;
update `gameobject_template` set `ScriptName`='go_bristlelimb_cage' where `entry`='181727' ;
update `gameobject_template` set `ScriptName`='go_bristlelimb_cage' where `entry`='181728' ;
update `gameobject_template` set `ScriptName`='go_bristlelimb_cage' where `entry`='181729' ;
update `gameobject_template` set `ScriptName`='go_bristlelimb_cage' where `entry`='181730' ;
update `gameobject_template` set `ScriptName`='go_bristlelimb_cage' where `entry`='181731' ;
update `gameobject_template` set `ScriptName`='go_bristlelimb_cage' where `entry`='181732' ;
update `gameobject_template` set `ScriptName`='go_bristlelimb_cage' where `entry`='181733' ;

update `gameobject_template` set `ScriptName`='go_najentus_spine' where `entry`='185584' ;

-- Creatures Cassa Crimsonwing, Parasitic Shadowfiend, Wing commander Dabiree&Brack, Gryphoneer leafbeard & skyguard handler deesak

update `creature_template` set `ScriptName`='npc_cassa_crimsonwing' where `entry`='23704' ;
update `creature_template` set `ScriptName`='mob_parasitic_shadowfiend' where `entry`='23498' ;
update `creature_template` set `ScriptName`='npc_wing_commander_dabiree' where `entry`='19409' ;
update `creature_template` set `ScriptName`='npc_gryphoneer_leafbeard' where `entry`='20236' ;
update `creature_template` set `ScriptName`='npc_wing_commander_brack' where `entry`='19401' ;
update `creature_template` set `ScriptName`='npc_skyguard_handler_deesak' where `entry`='23415' ;

-- 
-- Various fixes from the forum
-- 

update creature_template set spell1 = '31707', spell2 = '33395' where entry = '510';

update spell_affect set `SpellFamilyMask`=4398046515200 where `effectId`=0 and `entry`=16999; 
update spell_affect set `SpellFamilyMask`=4398046515200 where `effectId`=0 and `entry`=16998;

update spell_affect set `SpellFamilyMask`=34376646656 where `entry`=37166;

DELETE FROM creature WHERE guid = '1189277';
INSERT INTO creature VALUES (1189277,23029,530,1,0,0,-3902.27,3217.01,307.271,2.03121,300,10,0,28724,0,0,1);
DELETE FROM creature WHERE guid = '1189278';
INSERT INTO creature VALUES (1189278,23029,530,1,0,0,-4118.27,3484.11,284.092,0.41561,300,10,0,28724,0,0,1);
DELETE FROM creature WHERE guid = '1189279';
INSERT INTO creature VALUES (1189279,23029,530,1,0,0,-3642.52,3392.58,287.509,2.34851,300,10,0,28724,0,0,1);
DELETE FROM creature WHERE guid = '1189280';
INSERT INTO creature VALUES (1189280,23029,530,1,0,0,-3856.31,3724.59,283.373,5.27961,300,10,0,28724,0,0,1);

UPDATE `quest_template` SET `RewRepFaction1` = 933, `RewRepFaction2` = 942, `RewRepValue1` = 350, `RewRepValue2` = 350 WHERE `entry` = 11368;
UPDATE `quest_template` SET `RewRepFaction1` = 933, `RewRepFaction2` = 942, `RewRepValue1` = 350, `RewRepValue2` = 350 WHERE `entry` = 11369;
UPDATE `quest_template` SET `RewRepFaction1` = 933, `RewRepFaction2` = 942, `RewRepValue1` = 350, `RewRepValue2` = 350 WHERE `entry` = 11370;

UPDATE `quest_template` SET `RewRepFaction1` = 933, `RewRepFaction2` = 1011, `RewRepValue1` = 350, `RewRepValue2` = 350 WHERE `entry` = 11372;
UPDATE `quest_template` SET `RewRepFaction1` = 933, `RewRepFaction2` = 1011, `RewRepValue1` = 500, `RewRepValue2` = 350 WHERE `entry` = 11373;
UPDATE `quest_template` SET `RewRepFaction1` = 933, `RewRepFaction2` = 1011, `RewRepValue1` = 350, `RewRepValue2` = 350 WHERE `entry` = 11374;
UPDATE `quest_template` SET `RewRepFaction1` = 933, `RewRepFaction2` = 1011, `RewRepValue1` = 350, `RewRepValue2` = 350 WHERE `entry` = 11375;

UPDATE `quest_template` SET `RewRepFaction1` = 933, `RewRepFaction2` = 989, `RewRepValue1` = 350, `RewRepValue2` = 350 WHERE `entry` = 11378;
UPDATE `quest_template` SET `RewRepFaction1` = 933, `RewRepFaction2` = 989, `RewRepValue1` = 350, `RewRepValue2` = 350 WHERE `entry` = 11382;


UPDATE `quest_template` SET `RewRepFaction1` = 933, `RewRepFaction2` = 935, `RewRepValue1` = 350, `RewRepValue2` = 350 WHERE `entry` = 11384;
UPDATE `quest_template` SET `RewRepFaction1` = 933, `RewRepFaction2` = 935, `RewRepValue1` = 350, `RewRepValue2` = 350 WHERE `entry` = 11386;
UPDATE `quest_template` SET `RewRepFaction1` = 933, `RewRepFaction2` = 935, `RewRepValue1` = 350, `RewRepValue2` = 350 WHERE `entry` = 11388;

DELETE FROM skinning_loot_template WHERE entry = 20713;
INSERT INTO skinning_loot_template (entry, item, ChanceOrQuestChance, groupid, mincountOrRef, maxcount, lootcondition, condition_value1, condition_value2) VALUES (20713, 21887, 82, 1, 1, 1, 0, 0, 0),(20713, 25649, 18, 1, 2, 3, 0, 0, 0);
UPDATE creature_template SET skinloot = 20713 WHERE entry = 20713;

DELETE FROM creature WHERE guid=93896;
DELETE FROM creature WHERE guid=93897;
DELETE FROM creature WHERE guid=93899;
DELETE FROM creature WHERE guid=93898;

DELETE FROM creature WHERE guid=93770;
DELETE FROM creature WHERE guid=93771;
DELETE FROM creature WHERE guid=93772;

DELETE FROM creature WHERE guid=93900;
DELETE FROM creature WHERE guid=93901;
DELETE FROM creature WHERE guid=93902;
DELETE FROM creature WHERE guid=93903;
DELETE FROM creature WHERE guid=93904;
DELETE FROM creature WHERE guid=93905;
DELETE FROM creature WHERE guid=93906;
DELETE FROM creature WHERE guid=93907;
DELETE FROM creature WHERE guid=93908;
DELETE FROM creature WHERE guid=93909;

update spell_affect set `SpellFamilyMask`=61783278366 where `entry`=14177;

DELETE FROM `creature_loot_template` WHERE (`entry`=22841);
INSERT INTO `creature_loot_template` VALUES 
(22841, 29434, 100, 0, 2, 2, 0, 0, 0),
(22841, 32263, 16, 0, 1, 1, 0, 0, 0),
(22841, 32264, 16, 0, 1, 1, 0, 0, 0),
(22841, 32265, 17, 0, 1, 1, 0, 0, 0),
(22841, 32266, 16, 0, 1, 1, 0, 0, 0),
(22841, 32270, 14, 0, 1, 1, 0, 0, 0),
(22841, 32271, 14, 0, 1, 1, 0, 0, 0),
(22841, 32273, 16, 0, 1, 1, 0, 0, 0),
(22841, 32275, 9, 0, 1, 1, 0, 0, 0),
(22841, 32276, 6, 0, 1, 1, 0, 0, 0),
(22841, 32278, 16, 0, 1, 1, 0, 0, 0),
(22841, 32279, 9, 0, 1, 1, 0, 0, 0),
(22841, 32361, 14, 0, 1, 1, 0, 0, 0),
(22841, 32513, 16, 0, 1, 1, 0, 0, 0),
(22841, 32737, 1.5, 0, 1, 1, 0, 0, 0),
(22841, 32738, 1.6, 0, 1, 1, 0, 0, 0),
(22841, 32744, 1.4, 0, 1, 1, 0, 0, 0),
(22841, 32747, 1.4, 0, 1, 1, 0, 0, 0),
(22841, 32750, 1.8, 0, 1, 1, 0, 0, 0),
(22841, 32753, 2, 0, 1, 1, 0, 0, 0),
(22841, 32754, 1.5, 0, 1, 1, 0, 0, 0);

UPDATE `spell_affect` SET `SpellFamilyMask`=35184372088836 WHERE `effectId`=1 AND `entry`=19239;


DELETE FROM `spell_proc_event` WHERE (`entry`=12815);
DELETE FROM `spell_proc_event` WHERE (`entry`=12814);
DELETE FROM `spell_proc_event` WHERE (`entry`=12813);
DELETE FROM `spell_proc_event` WHERE (`entry`=12812);
DELETE FROM `spell_proc_event` WHERE (`entry`=12281);
DELETE FROM `spell_proc_event` WHERE (`entry`=13960);
DELETE FROM `spell_proc_event` WHERE (`entry`=13961);
DELETE FROM `spell_proc_event` WHERE (`entry`=13962);
DELETE FROM `spell_proc_event` WHERE (`entry`=13963);
DELETE FROM `spell_proc_event` WHERE (`entry`=13964);

INSERT INTO spell_proc_event VALUES (12815, 0, 0, 0, 0, 0, 0, 0, 1);
INSERT INTO spell_proc_event VALUES (12814, 0, 0, 0, 0, 0, 0, 0, 1);
INSERT INTO spell_proc_event VALUES (12813, 0, 0, 0, 0, 0, 0, 0, 1);
INSERT INTO spell_proc_event VALUES (12812, 0, 0, 0, 0, 0, 0, 0, 1);
INSERT INTO spell_proc_event VALUES (12281, 0, 0, 0, 0, 0, 0, 0, 1);
INSERT INTO spell_proc_event VALUES (13960, 0, 0, 0, 0, 0, 0, 0, 1);
INSERT INTO spell_proc_event VALUES (13961, 0, 0, 0, 0, 0, 0, 0, 1);
INSERT INTO spell_proc_event VALUES (13962, 0, 0, 0, 0, 0, 0, 0, 1);
INSERT INTO spell_proc_event VALUES (13963, 0, 0, 0, 0, 0, 0, 0, 1);
INSERT INTO spell_proc_event VALUES (13964, 0, 0, 0, 0, 0, 0, 0, 1);


-- 
-- Pool system changes
-- 

update `pool_template` set `max_limit`='30' where `entry`='999' ;
update `pool_template` set `max_limit`='30' where `entry`='998' ;
update `pool_template` set `max_limit`='30' where `entry`='997' ;
update `pool_template` set `max_limit`='30' where `entry`='996' ;
update `pool_template` set `max_limit`='30' where `entry`='995' ;
update `pool_template` set `max_limit`='30' where `entry`='994' ;
update `pool_template` set `max_limit`='30' where `entry`='993' ;
update `pool_template` set `max_limit`='30' where `entry`='992' ;
update `pool_template` set `max_limit`='30' where `entry`='991' ;
update `pool_template` set `max_limit`='30' where `entry`='990' ;
update `pool_template` set `max_limit`='30' where `entry`='988' ;
update `pool_template` set `max_limit`='30' where `entry`='978' ;
update `pool_template` set `max_limit`='30' where `entry`='977' ;
update `pool_template` set `max_limit`='30' where `entry`='976' ;
update `pool_template` set `max_limit`='30' where `entry`='975' ;
update `pool_template` set `max_limit`='30' where `entry`='974' ;
update `pool_template` set `max_limit`='30' where `entry`='973' ;
update `pool_template` set `max_limit`='30' where `entry`='972' ;
update `pool_template` set `max_limit`='30' where `entry`='971' ;
update `pool_template` set `max_limit`='30' where `entry`='970' ;
update `pool_template` set `max_limit`='30' where `entry`='969' ;
update `pool_template` set `max_limit`='30' where `entry`='968' ;
update `pool_template` set `max_limit`='30' where `entry`='967' ;
update `pool_template` set `max_limit`='30' where `entry`='966' ;
update `pool_template` set `max_limit`='30' where `entry`='965' ;
update `pool_template` set `max_limit`='30' where `entry`='964' ;
update `pool_template` set `max_limit`='30' where `entry`='963' ;
update `pool_template` set `max_limit`='30' where `entry`='962' ;
update `pool_template` set `max_limit`='30' where `entry`='961' ;
update `pool_template` set `max_limit`='30' where `entry`='960' ;
update `pool_template` set `max_limit`='30' where `entry`='959' ;
update `pool_template` set `max_limit`='30' where `entry`='958' ;
update `pool_template` set `max_limit`='30' where `entry`='957' ;
update `pool_template` set `max_limit`='30' where `entry`='956' ;
update `pool_template` set `max_limit`='30' where `entry`='955' ;
update `pool_template` set `max_limit`='30' where `entry`='954' ;
update `pool_template` set `max_limit`='30' where `entry`='953' ;
update `pool_template` set `max_limit`='30' where `entry`='952' ;
update `pool_template` set `max_limit`='30' where `entry`='951' ;
update `pool_template` set `max_limit`='30' where `entry`='950' ;
update `pool_template` set `max_limit`='30' where `entry`='949' ;
update `pool_template` set `max_limit`='30' where `entry`='948' ;
update `pool_template` set `max_limit`='30' where `entry`='947' ;