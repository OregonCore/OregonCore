-- Update value of spell_script_target cases
UPDATE conditions SET ConditionTypeOrReference = 17 WHERE ConditionTypeOrReference = 18;

-- Require Uncontrolled Voidwalker as target for spell used by Sanctified Crystal
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=17 AND `SourceEntry`=29364;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`,`SourceGroup`,`SourceEntry`,`ElseGroup`,`ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`,`ConditionValue2`,`ConditionValue3`,`ErrorTextId`,`ScriptName`,`Comment`) VALUES
(17,0,29364,0,31,1,3,16975,0,0, '', 'Encapsulate Voidwalker - Uncontrolled Voidwalker target');

-- Require Wyrmcult Blackwhelp as target for spell used by Blackwhelp Net
UPDATE item_template SET ScriptName = '' WHERE entry = 31129;
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=17 AND `SourceEntry`=29364;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`,`SourceGroup`,`SourceEntry`,`ElseGroup`,`ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`,`ConditionValue2`,`ConditionValue3`,`ErrorTextId`,`ScriptName`,`Comment`) VALUES
(17,0,38177,0,31,1,3,21387,0,0, '', 'Blackwhelp Net - Wyrmcult Blackwhelp target');

-- Require Electromental as target for spell used by Protovoltaic Magneto Collector
UPDATE item_template SET ScriptName = '' WHERE entry = 30656;
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=17 AND `SourceEntry`=29364;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`,`SourceGroup`,`SourceEntry`,`ElseGroup`,`ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`,`ConditionValue2`,`ConditionValue3`,`ErrorTextId`,`ScriptName`,`Comment`) VALUES
(17,0,37136,0,31,1,3,21731,0,0, '', 'Protovoltaic Magneto Collector - Electromental target');

-- Require Eye of Grillok as target for spell used by Zezzaks Shard
UPDATE item_template SET ScriptName = '' WHERE entry = 31463;
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=17 AND `SourceEntry`=29364;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`,`SourceGroup`,`SourceEntry`,`ElseGroup`,`ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`,`ConditionValue2`,`ConditionValue3`,`ErrorTextId`,`ScriptName`,`Comment`) VALUES
(17,0,38554,0,31,1,3,19440,0,0, '', 'Zezzaks Shard - Eye of Grillok target');

-- Require Socrethar as target for spell used by Vorenthals Presence
UPDATE item_template SET ScriptName = '' WHERE entry = 30259;
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=17 AND `SourceEntry`=29364;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`,`SourceGroup`,`SourceEntry`,`ElseGroup`,`ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`,`ConditionValue2`,`ConditionValue3`,`ErrorTextId`,`ScriptName`,`Comment`) VALUES
(17,0,36314,0,31,1,3,20132,0,0, '', 'Vorenthals Presence - Socrethar target');

-- Require Area 3803 for Area 52 Special item
UPDATE item_template SET ScriptName = '' WHERE entry = 28132;
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=17 AND `SourceEntry`=33836;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`,`SourceGroup`,`SourceEntry`,`ElseGroup`,`ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`,`ConditionValue2`,`ConditionValue3`,`ErrorType`,`ErrorTextId`,`ScriptName`,`Comment`) VALUES
(17,0,33836,0,23,0,3803,0,0,96,0, '', 'Dropping Heavy Bomb - Requires Area 3803');

-- Require Area 3637 for Warmaul Ogre Banner
UPDATE item_template SET ScriptName = '' WHERE entry = 28132;
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=17 AND `SourceEntry`=32307;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`,`SourceGroup`,`SourceEntry`,`ElseGroup`,`ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`,`ConditionValue2`,`ConditionValue3`,`ErrorType`,`ErrorTextId`,`ScriptName`,`Comment`) VALUES
(17,0,32307,0,23,0,3637,0,0,96,0, '', 'Warmaul Ogre Banner - Requires Area 3637');

-- Skill requirements for Flying Machines
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=17 AND `SourceEntry` IN (44153,44151);
UPDATE item_template SET ScriptName = '' WHERE entry IN (34060, 34061);

INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES
(17, 0, 44153, 0, 0, 7, 0, 202, 300, 0, 0, 0, '', 'Player has must have Engineering with skill level 300 to Ride Flying Machine'),
(17, 0, 44151, 0, 0, 7, 0, 202, 375, 0, 0, 0, '', 'Player has must have Engineering with skill level 375 to Ride Turbo-Charged Flying Machine');

-- Fix Banishing Crystal
DELETE FROM `conditions` WHERE `SourceEntry` = 40817;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`,`ErrorType`, `ErrorTextId`, `Comment`) VALUES
(17,0,40817,0,0,23,0,3785,0,0,0,96,0,'Banishing Crystal requires Forge Camp: Wrath'),
(17,0,40817,0,1,23,0,3784,0,0,0,96,0,'Banishing Crystal requires Forge Camp: Terror');
