DELETE FROM `script_texts` WHERE `entry` IN (-1780127,-1780128,-1780129,-1780130);
INSERT INTO `script_texts` VALUES ('-1780127', 'Help! I\'ve only one hand to defend myself with.', null, null, null, null, null, null, null, null, '0', '0', '0', '0', 'MIRAN_SAY_AMBUSH_ONE');
INSERT INTO `script_texts` VALUES ('-1780128', 'Send them on! I\'m not afraid of some scrawny beasts!', null, null, null, null, null, null, null, null, '0', '0', '0', '0', 'MIRAN_SAY_AMBUSH_TWO');
INSERT INTO `script_texts` VALUES ('-1780129', 'Feel the power of the Dark Iron Dwarves!', null, null, null, null, null, null, null, null, '0', '0', '0', '0', 'DARK_IRON_RAIDER_SAY_AMBUSH');
INSERT INTO `script_texts` VALUES ('-1780130', 'Ah, here at last! It\'s going to feel so good to get rid of these barrels.', null, null, null, null, null, null, null, null, '0', '0', '0', '0', 'MIRAN_SAY_QUEST_END');

UPDATE `creature_template` SET `ScriptName` = 'npc_miran' WHERE `entry` = '1379';
UPDATE `quest_template` SET `SpecialFlags` = '2' WHERE `entry` = '309';
