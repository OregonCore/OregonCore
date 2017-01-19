-- Fixed equipment of NPC 13601 (Tinkerer Gizlock) in Maraudon
-- because it was using Naxxramas model
SET @ENTRY := 1726;

DELETE FROM `creature_equip_template_raw` WHERE `entry` = @ENTRY;
INSERT INTO `creature_equip_template_raw` (`entry`, `equipmodel1`, `equipmodel2`, `equipmodel3`, `equipinfo1`, `equipinfo2`, `equipinfo3`, `equipslot1`, `equipslot2`, `equipslot3`) VALUES
(@ENTRY, 19835, 21540, 20726, 33490690, 234948100, 33489666, 781, 4, 26);

UPDATE `creature_template` SET `equipment_id` = @ENTRY WHERE `Entry` = 13601;

-- Prevent NPCs 12238 (Zaetar's Spirit) and 13716 (Celebras the Redeemed) to roam
-- around their spawn point. Now, players will be able to speak with them
-- and get the quest they offer without having the window abruptly closed
-- every few seconds
UPDATE `creature_template` SET `MovementType` = 0 WHERE `Entry` IN (12238, 13716); 

