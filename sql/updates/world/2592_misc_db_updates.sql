-- Add Dreadsteed/Felsteed/Charger/Warhorse visuals
DELETE FROM `spell_linked_spell` WHERE `spell_trigger` IN (23214, 13819);
INSERT INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `comment`) VALUES
(23214, 31726, 'Summon Charger'),
(13819, 31726, 'Summon Warhorse');

DELETE FROM `spell_linked_spell` WHERE `spell_trigger` IN (23161, 5784);
INSERT INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `comment`) VALUES
(23161, 31725, 'Summon Nightmare'),
(5784, 31725, 'Summon Felsteed');

-- Fix Bloodthistle Withdrawl
DELETE FROM `spell_linked_spell` WHERE `spell_trigger` IN (28273, -28273);
INSERT INTO `spell_linked_spell` VALUES 
(28273, -28274, 0, 'Bloodthistle'),
(-28273, 28274, 0, 'Bloodthistle');

-- Fix On Nethery Wings
UPDATE `gameobject` SET `position_z`=402.410004 WHERE `guid`=99956;
UPDATE `creature_template` SET `InhabitType`=4 WHERE `entry`=20899;

-- Measuring Warp Energies
DELETE FROM `disables` WHERE `sourceType`=0 AND `entry`=35113;
INSERT INTO `disables` (`sourceType`, `entry`, `flags`, `params_0`, `params_1`, `comment`) VALUES 
(0, 35113, 64, '', '', 'Ignore LOS on Warp Measurement');

-- Flying flags for creatures related to Mellichar boss fight
UPDATE `creature_template` SET `InhabitType`=4 WHERE `entry` IN (21440, 21438, 21437, 21436, 21439);

-- Update some values for Time Watchers
UPDATE `creature_template` SET `InhabitType`=4 WHERE `entry` IN (20142, 19918, 20069, 19959);
UPDATE `creature_template` SET `scale`=1.5 WHERE `Entry`=19959;
UPDATE `creature_addon` SET `bytes2`=4097 WHERE `guid` IN (23472, 23473);
UPDATE `creature_addon` SET `auras`="34712 0" WHERE `guid`=23078;
UPDATE `creature` SET `modelid`=0 WHERE `guid` = 23597;
UPDATE `creature` SET `modelid`=19278 WHERE `guid` = 23459;
UPDATE `creature` SET `modelid`=19282 WHERE `guid` = 23460;
UPDATE `creature` SET `modelid`=19280 WHERE `guid` = 23461;
UPDATE `creature` SET `modelid`=19281 WHERE `guid` = 23462;
DELETE FROM `creature_addon` WHERE `guid` IN (23441, 23428, 23431, 23430, 23434, 23427);
DELETE FROM `creature` WHERE `guid` IN (23428, 23441, 23431, 23430, 23434, 23427);
UPDATE `creature` SET `spawndist`=15 WHERE `id`=19918;
