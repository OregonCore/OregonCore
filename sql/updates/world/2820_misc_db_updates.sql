-- Add missing Lucky Coin item
DELETE FROM item_template WHERE entry = 32498;
INSERT INTO item_template (entry, class, subclass, NAME, displayid, Quality, Flags, BuyCount, maxcount, stackable, delay, spellid_1, bonding, BagFamily) VALUES (32498, 15, 2, 'Fortune Coin', 44964, 3, 32768, 1, 1, 1, 0, 40405, 1, 4096);

-- Fix faction for Warp Slicer. Thanks Athis.
UPDATE creature_template SET faction_A='1620', faction_H='1620' WHERE entry=21272;

-- Fix quest Murketh and Shadraaz gateways. Thanks PadreWoW.
DELETE FROM `conditions` where SourceEntry = 33655;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`,`SourceGroup`,`SourceEntry`,`SourceId`,`ElseGroup`,`ConditionTypeOrReference`,`ConditionTarget`,`ConditionValue1`,`ConditionValue2`,`ConditionValue3`,`NegativeCondition`,`ErrorType`,`ErrorTextId`,`ScriptName`,`Comment`) VALUES
(13,0,33655,0,0,17,0,1,19291,0,0,0,0,'',NULL),
(13,0,33655,0,0,17,0,1,19292,0,0,0,0,'',NULL);

UPDATE `creature_ai_scripts` SET `event_flags`='1' WHERE `id`='1929101';
UPDATE `creature_ai_scripts` SET `event_flags`='1' WHERE `id`='1929201';
