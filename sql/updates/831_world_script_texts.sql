DELETE FROM `script_texts` WHERE `entry` BETWEEN '-1601406' AND '-1601402';
INSERT INTO `script_texts` VALUES ('-1601406', 'Meet me down by the orchard-- I just need to put my gun away.', null, null, null, null, null, null, null, null, '0', '0', '7', '0', 'stilwell SAY_DS_PROLOGUE');
INSERT INTO `script_texts` VALUES ('-1601405', 'We\'ve done it! We won!', null, null, null, null, null, null, null, null, '0', '0', '7', '0', 'stilwell SAY_DS_DOWN_3');
INSERT INTO `script_texts` VALUES ('-1601404', 'We showed that one!', null, null, null, null, null, null, null, null, '0', '0', '7', '0', 'stilwell SAY_DS_DOWN_1');
INSERT INTO `script_texts` VALUES ('-1601403', 'One more down!', null, null, null, null, null, null, null, null, '0', '0', '7', '0', 'stilwell SAY_DS_DOWN_2');
INSERT INTO `script_texts` VALUES ('-1601402', 'To the house! Stay close to me, no matter what! I have my gun and ammo there!', null, null, null, null, null, null, null, null, '0', '0', '7', '0', 'stilwell SAY_DS_START');
UPDATE `creature_template` SET `ScriptName` = 'npc_daphne_stilwell' WHERE `entry` = '6182';
UPDATE `quest_template` SET `SpecialFlags` = '2' WHERE `entry` = '1651';