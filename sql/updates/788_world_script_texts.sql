DELETE FROM `script_texts` WHERE `entry` IN (-1600398,-1600397,-1600396,-1600395,-1600394,-1600393,-1600392,-1600391);
INSERT INTO `script_texts` VALUES ('-1600398', 'And try not to get lost this time...', null, null, null, null, null, null, null, null, '0', '0', '0', '0', 'tooga SAY_TORT_POST_6');
INSERT INTO `script_texts` VALUES ('-1600397', 'Yes Torta. Whatever your heart desires...', null, null, null, null, null, null, null, null, '0', '0', '0', '0', 'tooga SAY_TOOG_POST_5');
INSERT INTO `script_texts` VALUES ('-1600396', 'Enough with the rambling. I am starving! Now, get your dusty shell into that ocean and bring momma some grub.', null, null, null, null, null, null, null, null, '0', '0', '0', '0', 'tooga SAY_TORT_POST_4');
INSERT INTO `script_texts` VALUES ('-1600395', 'My dearest Torta. I have been gone for so long. Finally we are reunited. At long last our love can blossom again.', null, null, null, null, null, null, null, null, '0', '0', '0', '0', 'tooga SAY_TOOG_POST_3');
INSERT INTO `script_texts` VALUES ('-1600394', 'You have any idea how long I\'ve been waiting here? And where\'s dinner? All that time gone and nothing to show for it?', null, null, null, null, null, null, null, null, '0', '0', '0', '0', 'tooga SAY_TORT_POST_2');
INSERT INTO `script_texts` VALUES ('-1600393', 'Torta, my love! I have returned at long last.', null, null, null, null, null, null, null, null, '0', '0', '0', '0', 'tooga SAY_TOOG_POST_1');
INSERT INTO `script_texts` VALUES ('-1600392', 'Torta must be so worried.', null, null, null, null, null, null, null, null, '0', '0', '0', '0', 'tooga SAY_TOOG_WORRIED');
INSERT INTO `script_texts` VALUES ('-1600391', 'I\'m Thirsty.', null, null, null, null, null, null, null, null, '0', '0', '0', '0', 'tooga SAY_TOOG_THIRST');

UPDATE `quest_template` SET `SpecialFlags` = '2' WHERE `entry` = '1560';
UPDATE `creature_template` SET `ScriptName` = 'npc_tooga' WHERE `entry` = '5955';