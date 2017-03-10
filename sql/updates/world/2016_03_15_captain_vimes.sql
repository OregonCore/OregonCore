-- Lieutenant Aden (creature template 23951) should hand out the quest "Captain Vimes" (quest 1220).
DELETE FROM `creature_questrelation` WHERE `id` = 23951 AND `quest` = 1220;
INSERT INTO `creature_questrelation` (`id`,`quest`) VALUES
(23951,1220); -- "Lieutenant Aden" handing out "Captain Vimes"


