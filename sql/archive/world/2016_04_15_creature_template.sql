-- Fix quest Body of Evidence
UPDATE `creature_template` SET `ScriptName` = 'npc_warmaul_pyre' WHERE `entry` = '18395';
UPDATE `creature_template` SET `dynamicflags` = '32' WHERE entry = '18397';
UPDATE `creature_template` SET `flags_extra` = '2' WHERE entry = '18395';

UPDATE  `creature_template_addon` SET  `auras` =  '29266 0 29266 1' WHERE  `creature_template_addon`.`entry` =18397;
