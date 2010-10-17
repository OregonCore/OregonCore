UPDATE `creature_template` SET `ScriptName` = 'npc_arugal_voidwalker' WHERE `entry` = '4627';
DELETE FROM `script_texts` WHERE `entry` BETWEEN '-1033006' AND '-1033000';
INSERT INTO `script_texts` VALUES ('-1033000', 'Follow me and I\'ll open the courtyard door for you.', null, null, null, null, null, null, null, null, '0', '0', '7', '1', 'prisoner ashcrombe SAY_FREE_AS');
INSERT INTO `script_texts` VALUES ('-1033001', 'I have just the spell to get this door open. Too bad the cell doors weren\'t locked so haphazardly.', null, null, null, null, null, null, null, null, '0', '0', '7', '1', 'prisoner ashcrombe SAY_OPEN_DOOR_AS');
INSERT INTO `script_texts` VALUES ('-1033002', 'There it is! Wide open. Good luck to you conquering what lies beyond. I must report back to the Kirin Tor at once!', null, null, null, null, null, null, null, null, '0', '0', '7', '1', 'prisoner ashcrombe SAY_POST_DOOR_AS');
INSERT INTO `script_texts` VALUES ('-1033003', 'Free from this wretched cell at last! Let me show you to the courtyard....', null, null, null, null, null, null, null, null, '0', '0', '1', '1', 'prisoner adamant SAY_FREE_AD');
INSERT INTO `script_texts` VALUES ('-1033004', 'You are indeed courageous for wanting to brave the horrors that lie beyond this door.', null, null, null, null, null, null, null, null, '0', '0', '1', '1', 'prisoner adamant SAY_OPEN_DOOR_AD');
INSERT INTO `script_texts` VALUES ('-1033005', 'There we go!', null, null, null, null, null, null, null, null, '0', '0', '1', '1', 'prisoner adamant SAY_POST1_DOOR_AD');
INSERT INTO `script_texts` VALUES ('-1033006', 'Good luck with Arugal. I must hurry back to Hadrec now.', null, null, null, null, null, null, null, null, '0', '0', '1', '1', 'prisoner adamant SAY_POST2_DOOR_AD');
