UPDATE `creature_template` SET `ScriptName` = 'npc_tapoke_slim_jahn' WHERE `entry` = '4962';
UPDATE `creature_template` SET `ScriptName` = 'npc_mikhail' WHERE `entry` = '4963';
DELETE FROM creature WHERE guid = '9555';
DELETE FROM creature_addon WHERE guid = '9555';
DELETE FROM game_event_creature WHERE guid = '9555';
DELETE FROM game_event_model_equip WHERE guid = '9555';
UPDATE `quest_template` SET `SpecialFlags` = '2' WHERE `entry` = '1249';
UPDATE `quest_template` SET `ReqCreatureOrGOId1` = '0' WHERE `entry` = '1249';
UPDATE `quest_template` SET `ReqCreatureOrGOCount1` = '0' WHERE `entry` = '1249';