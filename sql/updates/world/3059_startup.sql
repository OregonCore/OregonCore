UPDATE `creature_template` SET `ScriptName` = 'npc_phase_hunter' WHERE `entry` = '20906';
UPDATE `creature_template` SET `ScriptName` = 'npc_azaloth' WHERE `entry` = '21506';
UPDATE `creature_template` SET `ScriptName` = 'npc_sunfurywarlock' WHERE `entry` = '21503';

UPDATE `creature_template` SET `AIName` = 'EventAI' WHERE `entry` = '22523';
UPDATE `creature_template` SET `AIName` = 'EventAI' WHERE `entry` = '25213';
UPDATE `creature_template` SET `AIName` = '' WHERE `entry` = '17211'; 
UPDATE `creature_template` SET `AIName` = '' WHERE `entry` = '2164';
DELETE FROM `creature_ai_scripts` WHERE `creature_id` = '17211'; 
DELETE FROM `creature_ai_scripts` WHERE `creature_id` = '20906';
DELETE FROM `creature_ai_scripts` WHERE `creature_id` = '2164';

DELETE FROM `db_script_string` WHERE `entry` = '2000005006'; 
DELETE FROM `db_script_string` WHERE `entry` = '2000005007'; 
DELETE FROM `db_script_string` WHERE `entry` = '2000005008'; 
DELETE FROM `db_script_string` WHERE `entry` = '2000005009'; 
DELETE FROM `db_script_string` WHERE `entry` = '2000005010'; 
DELETE FROM `db_script_string` WHERE `entry` = '2000005017';

DELETE FROM `creature_loot_template` WHERE `Entry` = '21478' AND `Item` = '31372';
UPDATE `creature_loot_template` SET `Chance` = '100' WHERE `Entry` = '21477' AND `Item` = '31372'; 
UPDATE `creature_loot_template` SET `Chance` = '100' WHERE `Entry` = '4723' AND `Item` = '5852';
UPDATE `creature_loot_template` SET `QuestRequired` = '1' WHERE `Entry` = '21477' AND `Item` = '31372';

DELETE FROM `npc_gossip` WHERE `npc_guid` = '83989';

UPDATE `creature_template` SET `lootid` = '25593' WHERE `entry` = '25507';
INSERT INTO `reference_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `GroupId`, `MinCount`, `MaxCount`, `Comment`) VALUES ('34092', '35205', '0', '0', '0', '1', '1', '1', NULL); 
INSERT INTO `reference_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `GroupId`, `MinCount`, `MaxCount`, `Comment`) VALUES ('34092', '35214', '0', '0', '0', '1', '1', '1', NULL); 

DELETE FROM `creature_loot_template` WHERE `Entry` = '25363';
DELETE FROM `creature_loot_template` WHERE `Entry` = '25367'; 
DELETE FROM `creature_loot_template` WHERE `Entry` = '25368'; 
DELETE FROM `creature_loot_template` WHERE `Entry` = '25369'; 
DELETE FROM `creature_loot_template` WHERE `Entry` = '25370'; 
DELETE FROM `creature_loot_template` WHERE `Entry` = '25371'; 
DELETE FROM `creature_loot_template` WHERE `Entry` = '25483'; 
DELETE FROM `creature_loot_template` WHERE `Entry` = '25486'; 
DELETE FROM `creature_loot_template` WHERE `Entry` = '25507'; 
DELETE FROM `creature_loot_template` WHERE `Entry` = '25597';

UPDATE `quest_template` SET `ReqCreatureOrGOId1` = '22197' WHERE `entry` = '10836';

DELETE FROM `creature_addon` WHERE `guid` = '84009';

UPDATE `creature_addon` SET `auras` = '35194 0' WHERE `guid` = '64246';
UPDATE `creature_addon` SET `auras` = '5301 0' WHERE `guid` = '67906'; 
UPDATE `creature_addon` SET `auras` = '5301 0' WHERE `guid` = '67907'; 
UPDATE `creature_addon` SET `auras` = '5301 0' WHERE `guid` = '67910'; 

UPDATE `creature_template_addon` SET `auras` = '31261 0' WHERE `entry` = '21304'; 
UPDATE `creature_template_addon` SET `auras` = '' WHERE `entry` = '20869';
UPDATE `creature_template_addon` SET `auras` = '35194 0' WHERE `entry` = '18121'; 

DELETE FROM `creature_addon` WHERE `guid` = '11657';
DELETE FROM `creature_addon` WHERE `guid` = '11693';
DELETE FROM `creature_addon` WHERE `guid` = '11747';
DELETE FROM `creature_addon` WHERE `guid` = '14543';
DELETE FROM `creature_addon` WHERE `guid` = '14544';
DELETE FROM `creature_addon` WHERE `guid` = '14547';
DELETE FROM `creature_addon` WHERE `guid` = '14551';
DELETE FROM `creature_addon` WHERE `guid` = '14558';
DELETE FROM `creature_addon` WHERE `guid` = '14561';
DELETE FROM `creature_addon` WHERE `guid` = '14579';
DELETE FROM `creature_addon` WHERE `guid` = '14634';
DELETE FROM `creature_addon` WHERE `guid` = '14638';
DELETE FROM `creature_addon` WHERE `guid` = '45214';
DELETE FROM `creature_addon` WHERE `guid` = '45215';

UPDATE `creature_template` SET `heroic_entry` = '23181' WHERE `entry` = '23175'; 
UPDATE `creature_template` SET `heroic_entry` = '23182' WHERE `entry` = '23176'; 
UPDATE `creature_template` SET `heroic_entry` = '23183' WHERE `entry` = '23177'; 
UPDATE `creature_template` SET `heroic_entry` = '23184' WHERE `entry` = '23178'; 
UPDATE `creature_template` SET `heroic_entry` = '23185' WHERE `entry` = '23179'; 
UPDATE `creature_template` SET `heroic_entry` = '23186' WHERE `entry` = '23180';

DELETE FROM `creature_equip_template` WHERE `entry` = '3005';
UPDATE `creature_template` SET `equipment_id` = '0' WHERE `entry` = '26259';

UPDATE `creature`, `creature_template` SET `creature`.`curhealth`=`creature_template`.`minhealth`,`creature`.`curmana`=`creature_template`.`minmana` WHERE `creature`.`id`=`creature_template`.`entry`;

DELETE FROM `spell_group` WHERE `spell_id` IN (13549,13550,13551,13552,13553,13554,13555,14279,14280,24132,24133,24134,24135,25295,27016,27018,27068,27069);