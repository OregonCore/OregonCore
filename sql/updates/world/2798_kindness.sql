-- Add conditions for item requiring quest
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=1 AND `SourceEntry`=31372;
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=1 AND `SourceEntry`=31373;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`,`SourceGroup`,`SourceEntry`,`ElseGroup`,`ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`,`ConditionValue2`,`ConditionValue3`,`ErrorTextId`,`ScriptName`,`Comment`) VALUES
(1,21477,31372,0,9,0,10804,0,0,0, '', 'Rocknail Flayer Carcass drops from Rocknail Flayer on quest Kindness'),
(1,21478,31373,0,9,0,10804,0,0,0, '', 'Rocknail Flayer Giblets drops from Rocknail Rippers on quest Kindness');

-- Add items to reqsourceid
UPDATE quest_template SET ReqSourceId1 = 31372, ReqSourceId2 = 31373, ReqSourceCount1 = 8, ReqSourceCount2 = 8 WHERE entry = 10804;