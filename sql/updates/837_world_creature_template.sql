UPDATE `creature_template` SET `ScriptName` = 'npc_death_ravager' WHERE `entry` = '17556';
UPDATE `creature_template` SET `ScriptName` = 'npc_stillpine_capitive' WHERE `entry` = '17375';
UPDATE `gameobject_template` SET `ScriptName` = 'go_ravager_cage' WHERE `entry` = '181849';
UPDATE `gameobject_template` SET `ScriptName` = 'go_bristlelimb_cage' WHERE `entry` = '181714';
DELETE FROM `script_texts` WHERE `entry` BETWEEN '-1600476' AND '1600474';
INSERT INTO `script_texts` VALUES ('-1600474', '[Fulborg] The Stillpine furbolgs will not soon forget your bravery!', null, null, null, null, null, null, null, null, '0', '0', '0', '0', 'Stillpine Capitive free say text 1');
INSERT INTO `script_texts` VALUES ('-1600475', '[Fulborg] Thank you, $N', null, null, null, null, null, null, null, null, '0', '0', '0', '0', 'Stillpine Capitive free say text 2');
INSERT INTO `script_texts` VALUES ('-1600476', '[Fulborg] Those remaining at Stillpine Hold will welcome you as a hero!', null, null, null, null, null, null, null, null, '0', '0', '0', '0', 'Stillpine Capitive free say text 3');
