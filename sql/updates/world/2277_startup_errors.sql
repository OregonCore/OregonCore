-- Fix startup errors.
-- Thanks to PadreWoW
UPDATE `creature_template` SET `AIName` = '' WHERE `entry` = '15938';
DELETE FROM `creature_ai_scripts` WHERE `creature_id` = '15938'; 
DELETE FROM `creature_ai_scripts` WHERE `creature_id` = '17167';
UPDATE `creature` SET `MovementType` = '0' WHERE `id` = '21639';
UPDATE `creature_template` SET `spell2` = '0' WHERE `entry` = '2283';
UPDATE `creature_template` SET `spell4` = '0' WHERE `entry` = '3975'; 
UPDATE `creature_template` SET `spell1` = '0' WHERE `entry` = '4069'; 
UPDATE `creature_template` SET `spell1` = '0' WHERE `entry` = '4644'; 
UPDATE `creature_template` SET `spell1` = '0' WHERE `entry` = '5332';
UPDATE `creature_template` SET `spell1` = '0' WHERE `entry` = '5645';
UPDATE `creature_template` SET `spell1` = '0' WHERE `entry` = '7086'; 
UPDATE `creature_template` SET `spell2` = '0' WHERE `entry` = '10394'; 
UPDATE `creature_template` SET `spell2` = '0' WHERE `entry` = '10482'; 
UPDATE `creature_template` SET `spell1` = '0' WHERE `entry` = '10917'; 
UPDATE `creature_template` SET `spell1` = '0' WHERE `entry` = '11439'; 
UPDATE `creature_template` SET `spell1` = '0' WHERE `entry` = '11475'; 
UPDATE `creature_template` SET `spell1` = '0' WHERE `entry` = '14883'; 
UPDATE `creature_template` SET `spell4` = '0' WHERE `entry` = '15863'; 
UPDATE `creature_template` SET `spell1` = '0' WHERE `entry` = '16101'; 
UPDATE `creature_template` SET `spell2` = '0' WHERE `entry` = '17556'; 
UPDATE `creature_template` SET `spell2` = '0' WHERE `entry` = '17625'; 
UPDATE `creature_template` SET `spell1` = '0' WHERE `entry` = '18506'; 
UPDATE `creature_template` SET `spell2` = '0' WHERE `entry` = '19545'; 
UPDATE `creature_template` SET `spell3` = '0' WHERE `entry` = '20300'; 
UPDATE `creature_template` SET `spell4` = '0' WHERE `entry` = '20530'; 
UPDATE `creature_template` SET `spell2` = '0' WHERE `entry` = '21385'; 
UPDATE `creature_template` SET `spell1` = '0' WHERE `entry` = '21768'; 
UPDATE `creature_template` SET `spell2` = '0' WHERE `entry` = '21907'; 
UPDATE `creature_template` SET `spell2` = '0' WHERE `entry` = '23162'; 
UPDATE `creature_template` SET `spell2` = '0' WHERE `entry` = '23278'; 
UPDATE `creature_template` SET `spell4` = '0' WHERE `entry` = '24857'; 
UPDATE `creature_template` SET `spell2` = '0' WHERE `entry` = '25570'; 
UPDATE `creature_template` SET `spell4` = '0' WHERE `entry` = '25572'; 
UPDATE `creature_template` SET `spell1` = '0' WHERE `entry` = '28132';

-- Thanks to LordUsagi
DELETE FROM `npc_gossip` WHERE `npc_guid` = '792693';
DELETE FROM `npc_gossip` WHERE `npc_guid` = '72693';
INSERT INTO `npc_gossip` VALUES
(72693, 10401);

DELETE FROM `creature_addon` WHERE `guid` = '86479';

UPDATE `creature` SET `curhealth` = '8' WHERE `id` = '17161';

ALTER TABLE `version`
MODIFY COLUMN `core_revision` VARCHAR(6);