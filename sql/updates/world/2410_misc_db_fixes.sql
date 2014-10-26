-- Add quest mail rewards for Winters Veil
-- Reward from Treats for Greatfather Winter (horde)
SET @MAIL:=118; -- Set in DBC
UPDATE `quest_template` SET `RewMailTemplateId`=@MAIL, `RewMailDelaySecs`=86400 WHERE `entry` IN (6962); -- Set mail delivery
DELETE FROM `mail_loot_template` WHERE `entry`=@MAIL;
INSERT INTO `mail_loot_template` (`entry`,`item`,`ChanceOrQuestChance`,`groupid`,`mincountOrRef`,`maxcount`) VALUES 
(@MAIL,17685,100,0,1,1); -- Attach item to mail

-- Reward from Treats for Greatfather Winter (alliance)
SET @MAIL:=102; -- Set in DBC
UPDATE `quest_template` SET `RewMailTemplateId`=@MAIL, `RewMailDelaySecs`=86400 WHERE `entry` IN (7025); -- Set mail delivery
DELETE FROM `mail_loot_template` WHERE `entry`=@MAIL;
INSERT INTO `mail_loot_template` (`entry`,`item`,`ChanceOrQuestChance`,`groupid`,`mincountOrRef`,`maxcount`) VALUES 
(@MAIL,17685,100,0,1,1); -- Attach item to mail

-- Reward from Stolen Winter Veil Treats (alliance)
SET @MAIL:=117; -- Set in DBC
UPDATE `quest_template` SET `RewMailTemplateId`=@MAIL, `RewMailDelaySecs`=86400 WHERE `entry` IN (7042); -- Set mail delivery
DELETE FROM `mail_loot_template` WHERE `entry`=@MAIL;
INSERT INTO `mail_loot_template` (`entry`,`item`,`ChanceOrQuestChance`,`groupid`,`mincountOrRef`,`maxcount`) VALUES 
(@MAIL,17712,100,0,1,1); -- Attach item to mail

-- Reward from Metzen the Reindeer (alliance)
SET @MAIL:=161; -- Set in DBC
UPDATE `quest_template` SET `RewMailTemplateId`=@MAIL, `RewMailDelaySecs`=86400 WHERE `entry` IN (8762); -- Set mail delivery
DELETE FROM `mail_loot_template` WHERE `entry`=@MAIL;
INSERT INTO `mail_loot_template` (`entry`,`item`,`ChanceOrQuestChance`,`groupid`,`mincountOrRef`,`maxcount`) VALUES 
(@MAIL,21216,100,0,1,1); -- Attach item to mail

-- Reward from Metzen the Reindeer (horde)
SET @MAIL:=122; -- Set in DBC
UPDATE `quest_template` SET `RewMailTemplateId`=@MAIL, `RewMailDelaySecs`=86400 WHERE `entry` IN (8746); -- Set mail delivery
DELETE FROM `mail_loot_template` WHERE `entry`=@MAIL;
INSERT INTO `mail_loot_template` (`entry`,`item`,`ChanceOrQuestChance`,`groupid`,`mincountOrRef`,`maxcount`) VALUES 
(@MAIL,21216,100,0,1,1); -- Attach item to mail  }}}

-- Misc
-- Derrok's Wand Case
DELETE FROM `item_loot_template` WHERE `entry`=30650;
INSERT INTO `item_loot_template` (`entry`,`item`,`ChanceOrQuestChance`,`groupid`,`mincountOrRef`,`maxcount`) VALUES
(30650,30651,100,1,1,1), -- Dertrok's First Wand
(30650,30652,100,2,1,1), -- Dertrok's Second Wand
(30650,30653,100,3,1,1), -- Dertrok's Third Wand
(30650,30654,100,4,1,1); -- Dertrok's Fourth Wand

-- Insert loot for Gently Shaken Gift
DELETE FROM `item_loot_template` WHERE `entry`=21271;
INSERT INTO `item_loot_template` (`entry`,`item`,`ChanceOrQuestChance`,`groupid`,`mincountOrRef`,`maxcount`) VALUES
(21271,21241,100,0,5,5);

-- Remove Skinning loot from Blackwing Spellbinder
UPDATE `creature_template` SET `skinloot`=0 WHERE `entry`=12457;

-- Correct dropchance for Mote of Life
UPDATE `creature_loot_template` SET `ChanceOrQuestChance`=18.5307 WHERE `entry`=22307 AND `item`=22575;

-- Update the dropchance to 100% if on quest
UPDATE `creature_loot_template` SET `ChanceOrQuestChance`=-100 WHERE `entry`=21477 AND `item`=31372; -- Rocknail Flayer Carcass
UPDATE `creature_loot_template` SET `ChanceOrQuestChance`=-100 WHERE `entry`=21478 AND `item`=31373; -- Rocknail Flayer Giblets

-- fix areatrigger for wickerman camp
DELETE FROM `areatrigger_involvedrelation` WHERE `id` =3991;
INSERT INTO `areatrigger_involvedrelation` (`id`, `quest`) 
VALUES (3991,1658);

-- yous have da darkrune should be daily 
UPDATE `quest_template` SET `SpecialFlags` = `SpecialFlags` | 1 WHERE `entry` = 11027;

-- Update drop chance of Ensorcelled Parchment from 0.2% to 100%
UPDATE `gameobject_loot_template` SET `ChanceOrQuestChance`=100 WHERE `entry`=1570 AND `item`=3706;
-- Change spawntime from 2 secs to 3 minutes
UPDATE `gameobject` SET `spawntimesecs`=180 WHERE `id`=1765;

-- Improve and fix quests 10458, 10480 and 10481
DELETE FROM creature_template_addon WHERE entry=21109;
INSERT INTO creature_template_addon (entry, auras) VALUES
(21109, '36169 0');

DELETE FROM spell_script_target WHERE entry=36035;
INSERT INTO spell_script_target VALUES
(36035, 1, 21071);

UPDATE creature_template SET MovementType=0 WHERE entry IN (21073, 21097, 21109, 21116);

-- Grimtotem Geomancer has minlevel and maxlevel swapped (27 <> 25).
UPDATE creature_template SET minlevel = 25, maxlevel = 27 WHERE entry = 10760;

-- Chromatic Drakonid is a 60 elite. Correct a typo in the maxlevel (6 => 60).
UPDATE creature_template SET maxlevel = 60 WHERE entry = 14302;

-- Heigan the Unclean's level should be 73, matching the other Naxx bosses (72 => 73).
UPDATE creature_template SET minlevel = 73, maxlevel = 73 WHERE entry = 15936;

-- Illidari Dreadbringer is a level 69 NPC. Correct the maxlevel (59 => 69).
UPDATE creature_template SET maxlevel = 69 WHERE entry = 19799;

-- Exarch Onaala is a level 69 quest giver. Correct her maxlevel (68 => 69).
UPDATE creature_template SET maxlevel = 69 WHERE entry = 21860;

-- Illidari Peacekeer has maxlevel < minlevel, fix startup error.
UPDATE creature_template SET maxlevel = 68 WHERE entry = 19821;

-- Jezelle Pruitt summons her pets with a script now, but the old static spawns for
-- her Imp, Voidwalker and Felhunter are still around. Delete them.
DELETE FROM creature WHERE id IN (5726,5729,5730);
DELETE FROM creature_addon WHERE guid IN (SELECT guid FROM creature WHERE id IN (5726,5729,5730));

-- Delete several other pets spawned by script but still have static existance
DELETE FROM creature WHERE id IN (24476);
DELETE FROM creature_addon WHERE guid IN (SELECT guid FROM creature WHERE id IN (24476));

-- Several BRD related quests have wrong prerequisites.
-- * Kharan Mighthammer, in BRD, offers "Kharan's Tale" before you've done any of the prereqs.
-- * The follow up quest he offers after that one, "The Bearer of Bad News", then gives an
--   error that you don't meet the requirements. This is a sign that it has the wrong prereqs.
-- * King Magni Bronzebeard offers "The Fate of the Kingdom" before you've done any of the prereqs.
-- * Princess Moira offers "The Princess's Surprise" before you've done any of the prereqs, even to horde characters!
-- * The horde quest "Disharmony of Fire" is offered together with the quest "Disharmony of Flame",
--   instead of depending on its completion.
UPDATE quest_template SET PrevQuestID=4341 WHERE entry=4342;
UPDATE quest_template SET PrevQuestID=4342 WHERE entry=4361;
UPDATE quest_template SET RequiredRaces=1101, PrevQuestID=4361 WHERE entry=4362;
UPDATE quest_template SET RequiredRaces=1101, PrevQuestID=4362 WHERE entry=4363;
UPDATE quest_template SET PrevQuestID=3906 WHERE entry=3907;

-- #### Magtheridon ####
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE id = 17376;
-- target entry for Blood Furnace Magtheridon
DELETE FROM spell_script_target WHERE entry = 30207 AND targetEntry = 21174;
INSERT INTO spell_script_target (entry, TYPE, targetEntry) VALUES
(30207, 1, 21174);

-- Creature template addon
DELETE FROM creature_template_addon WHERE entry = 16208;
INSERT INTO creature_template_addon (entry, bytes1) VALUES
(16208, 7);

-- quest 11521
DELETE FROM spell_script_target WHERE entry = 44941;
INSERT INTO spell_script_target VALUES
(44941, 0, 187073);

DELETE FROM spell_script_target WHERE entry=9095;
INSERT INTO spell_script_target VALUES
(9095, 1, 6492);

DELETE FROM spell_script_target WHERE entry=36904;
INSERT INTO spell_script_target VALUES
(36904, 0, 300106);

UPDATE quest_template SET SpecialFlags = 2 WHERE entry = 1658;
UPDATE creature SET curmana = 2705 WHERE guid = 60069;
UPDATE creature SET curmana = 2620 WHERE guid = 78283;
UPDATE creature SET curhealth = 4200 WHERE guid = 58573;