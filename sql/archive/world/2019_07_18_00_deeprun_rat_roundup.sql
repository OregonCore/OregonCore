UPDATE `creature_template` SET `AIName` = 'SmartAI' WHERE `entry` IN (13016, 12997);

DELETE FROM `smart_scripts` WHERE `entryorguid` IN (12997, 13016);
INSERT INTO `smart_scripts` VALUES (13016, 0, 0, 1, 8, 0, 100, 0, 21050, 0, 0, 0, 0, 29, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Deeprun Rat - On Spellhit - Follow Player');
INSERT INTO `smart_scripts` VALUES (13016, 0, 1, 2, 61, 0, 100, 0, 0, 0, 0, 0, 0, 36, 13017, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Deeprun Rat - On Spellhit - Update Template');
INSERT INTO `smart_scripts` VALUES (13016, 0, 2, 3, 61, 0, 100, 0, 0, 0, 0, 0, 0, 33, 12997, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Deeprun Rat - On Spellhit - Quest Credit');
INSERT INTO `smart_scripts` VALUES (13016, 0, 3, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 11, 21051, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Deeprun Rat - On Spellhit - Cast Spell');
INSERT INTO `smart_scripts` VALUES (13016, 0, 4, 0, 8, 0, 100, 0, 21052, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Deeprun Rat - On Spellhit - Die');

INSERT INTO `smart_scripts` VALUES (12997, 0, 0, 1, 10, 0, 100, 0, 1, 8, 2000, 2000, 0, 11, 21052, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Monty - On OOC Los - Cast Bash');
