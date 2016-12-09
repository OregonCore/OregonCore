/* Fix for Archmage Arugal in Shadow Fang Keep */
UPDATE `creature_template` SET `ScriptName` = "boss_archmage_arugal" WHERE `entry` = 4275;



INSERT INTO `script_texts` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`, `sound`, `type`, `language`, `emote`, `comment`) VALUES 
(-1542750, 'You, too, shall serve!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 5793, 1, 0, 0, 'Archmage Arugal SAY_AGGRO'),
(-1542751, 'Another falls!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 5795, 1, 0, 0, 'Archmage Arugal SAY_KILL'),
(-1527522, 'Release your rage!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 5797, 1, 0, 0, 'Archmage Arugal SAY_TRANSFORM');

INSERT INTO `spell_target_position` (`id`, `target_map`, `target_position_x`, `target_position_y`, `target_position_z`, `target_orientation`) VALUES
('7136', '33', '-85.75', '2150.80', '155.7', '2.195'),
('7586', '33', '-102.60', '2124.49', '155.7', '1.12'),
('7587', '33', '-105.80', '2160.44', '155.7', '0.02');