UPDATE creature_template SET npcflag = '131' WHERE entry = 24975;

DELETE FROM `npc_vendor` WHERE `entry` = 24975;
INSERT INTO `npc_vendor` (`entry`, `item`, `maxcount`, `incrtime`, `ExtendedCost`) VALUES
(24975, 3371, 0, 0, 0),
(24975, 3372, 0, 0, 0),
(24975, 8925, 0, 0, 0),
(24975, 13467, 3, 9000, 0),
(24975, 18256, 0, 0, 0),
(24975, 22785, 3, 9000, 0),
(24975, 22786, 3, 9000, 0),
(24975, 22791, 3, 9000, 0),
(24975, 22793, 1, 9000, 0);