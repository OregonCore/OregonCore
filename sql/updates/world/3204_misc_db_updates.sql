-- Fix faction and flags for Tempest Keep gobjects
UPDATE `gameobject_template` SET `faction`=1375, `flags`=32 WHERE `entry` IN (184322, 184324, 184325, 184326, 184327, 184328, 184329);

--  Link Fear to Charge for Spectral Chargers in Karazhan
DELETE FROM `spell_linked_spell` WHERE `spell_trigger` = 29320;
INSERT INTO `spell_linked_spell` VALUES (29320, 29321, 0, 'Charge-Fear');

-- Update InhabitType
UPDATE creature_template SET InhabitType = 4 WHERE entry IN (19514, 22850); -- Al'ar
UPDATE creature_template SET InhabitType = 7 WHERE entry = 22274;                 --  Dragonmaw Skybreakers
UPDATE creature_template SET InhabitType = 7 WHERE entry = 15185;                 -- Brood of Nozdormu
UPDATE creature_template SET InhabitType = 7 WHERE entry IN (17307, 18435); -- Vazruden The Herald
UPDATE creature_template SET InhabitType = 7 WHERE entry = 18879;                 -- Phase Hunters
UPDATE creature_template SET InhabitType = 7 WHERE entry = 11583;                 -- Nefarion
UPDATE creature_template SET InhabitType = 7 WHERE entry = 15491;                 -- Eranikus
UPDATE creature_template SET InhabitType = 7 WHERE entry IN (17536, 18432); -- Nezan
UPDATE creature_template SET InhabitType = 7 WHERE entry = 18963;                 -- Windroc Huntresses
UPDATE creature_template SET InhabitType = 7 WHERE entry = 18996;                 -- Windroc Hunters
UPDATE creature_template SET InhabitType = 7 WHERE entry = 20021;                 -- Nether Whelp
UPDATE creature_template SET InhabitType = 7 WHERE entry = 21823;                 -- Nihil
UPDATE creature_template SET InhabitType = 7 WHERE entry = 21879;                 -- Vilewing Chimaeras
UPDATE creature_template SET InhabitType = 7 WHERE entry = 22181;                 -- Aether Rays
UPDATE creature_template SET InhabitType = 7 WHERE entry = 22140;                 -- Toxic Sporebat
UPDATE creature_template SET InhabitType = 7 WHERE entry = 22181;                 -- Aether Rays
UPDATE creature_template SET InhabitType = 7 WHERE entry = 22972;                 -- Cenarion Sparrowhawk
UPDATE creature_template SET InhabitType = 7 WHERE entry = 22980;                 -- Skyguard Scout
UPDATE creature_template SET InhabitType = 7 WHERE entry = 23061;                 -- Rivendark
UPDATE creature_template SET InhabitType = 7 WHERE entry = 23261;                 -- Furywing
UPDATE creature_template SET InhabitType = 7 WHERE entry = 23281;                 -- Insidion