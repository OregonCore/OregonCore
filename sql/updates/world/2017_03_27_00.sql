--
-- Jovaan Event
--

INSERT INTO `creature_template` (`entry`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `minlevel`, `maxlevel`,
`minhealth`, `maxhealth`, `armor`, `faction`, `speed_walk`, `baseattacktime`, `unit_class`, `unit_flags`, `rangedattackpower`,
`type`, `flags_extra`, `ScriptName`) VALUES (61031, 17519, 11686, 17519, 11686, 'Legion Hold Device Trigger', 60, 60, 6900, 6900,
20, 114, 1.05, 2000, 1, 33587968, 100, 10, 130, 'legion_hold_device_trigger');

DELETE FROM creature_template WHERE entry=61031;
DELETE FROM `creature` WHERE `id`=61031;

INSERT INTO `creature` (`id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`,
`orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`) VALUES (61031,
530, 1, 0, 0, -3301.97, 2928.01, 170.927, 2.52501, 25, 0, 0, 6900, 0, 0);

--
-- Shadowmoon warlock visual effect
--

INSERT INTO `creature_template` (`entry`, `heroic_entry`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`,
`modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `minhealth`, `maxhealth`, `minmana`,
`maxmana`, `armor`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`,
`attackpower`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `dynamicflags`, `family`, `trainer_type`,
`trainer_spell`, `class`, `race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `lootid`,
`pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`,
`spell2`, `spell3`, `spell4`, `PetSpellDataId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `ManaModifier`,
`HealthModifier`, `RacialLeader`, `RegenHealth`, `equipment_id`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`) VALUES
(61025, 0, 0, 0, 17519, 11686, 17519, 11686, 'Visual Trigger', NULL, NULL, 0, 60, 60, -1, 6900, 6900, 0, 0, 20, 114, 0, 1.05,
1.14286, 1, 0, 0, 0, 0, 0, 2000, 0, 1, 33587968, 0, 0, 0, 0, 0, 0, 0, 0, 100, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, '', 0, 3, 1, 1, 0, 1, 0, 0, 130, '');

DELETE FROM creature_template WHERE entry=61025;
DELETE FROM `creature` WHERE `id`=61025;

INSERT INTO `creature` (`id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`,
`orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`) VALUES (61025,
530, 1, 0, 0, -3406.6, 2940.3, 183.25, 3.92773, 25, 0, 0, 6900, 0, 0);

INSERT INTO `creature` (`id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`,
`orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`) VALUES (61025,
530, 1, 17612, 0, -3371.1, 2961.6, 183.138, 0.892162, 25, 0, 0, 6900, 0, 0);

INSERT INTO `creature` (`id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`,
`orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`) VALUES (61025,
530, 1, 17612, 0, -3354.2, 2998.5, 183.331, 1.20004, 25, 0, 0, 6900, 0, 0);

--
-- Fix some missing script names
--

UPDATE `gameobject_template` SET `ScriptName`='go_moonshine_keg' WHERE `entry`=185213;
UPDATE `gameobject_template` SET `ScriptName`='go_fermentedseed_keg' WHERE `entry`=185214;
UPDATE `gameobject_template` SET `ScriptName`='go_greenspot_keg' WHERE `entry`=185206;
UPDATE `gameobject_template` SET `ScriptName`='go_netherdrake_egg' WHERE `entry`=184867;
UPDATE `gameobject_template` SET `ScriptName`='go_power_converter' WHERE `entry`=184906;
UPDATE `creature_template` SET `ScriptName`='npc_grishna_arrakoa' WHERE  `entry`=22232;