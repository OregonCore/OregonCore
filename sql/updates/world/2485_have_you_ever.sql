-- Have you ever seen one of these
DELETE FROM `event_scripts` WHERE `id` = '11424';
INSERT INTO `event_scripts` VALUES
(11424, 1, 9, 6781, 60, 0, 0, 0, 0, 0),
(11424, 5, 10, 18152, 60000, 0, 1176.53, 8130.83, 20.13, 1.86);

UPDATE `creature_template` SET `ScriptName` = 'npc_baby_murloc' WHERE entry = '18152';