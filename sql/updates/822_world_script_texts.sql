DELETE FROM `script_texts` WHERE `entry` IN (-1600411,-1600412);
INSERT INTO `script_texts` VALUES ('-1600411', '%s growls in acknowledgement before straightening and making her way off into the forest.', null, null, null, null, null, null, null, null, '0', '2', '0', '0', 'mist EMOTE_AT_HOME');
INSERT INTO `script_texts` VALUES ('-1600412', 'Mist! I feared I would never see you again! Yes, I am well, do not worry for me. You must rest and recover your health.', null, null, null, null, null, null, null, null, '0', '0', '7', '0', 'mist SAY_AT_HOME');
UPDATE `creature_template` SET `ScriptName` = 'npc_mist' WHERE `entry` = '3568';
UPDATE `quest_template` SET `SpecialFlags` = '2' WHERE `entry` = '938';