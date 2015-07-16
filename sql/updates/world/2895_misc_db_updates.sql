-- Make a trigger invisible
UPDATE `creature_template` SET `flags_extra`=`flags_extra`|128|2 WHERE `entry` IN (19871); -- World Trigger (World Trigger (Not Immune NPC))

-- Link two NPCs in stormwind to a game event
DELETE FROM `game_event_creature` WHERE `guid` IN (62848,62849) AND `event`=11;
INSERT INTO `game_event_creature` (`guid`, `event`) VALUES 
(62848, 11),
(62849, 11);

-- Add Hand of A'dal title to the Black Temple attunement line
UPDATE quest_template SET CharTitleId = 64 WHERE entry = 10985;