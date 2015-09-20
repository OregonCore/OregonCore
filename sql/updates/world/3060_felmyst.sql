UPDATE `creature` SET `MovementType` = 0 WHERE `id` = 23472;

UPDATE `creature_template` SET `ScriptName` = 'mob_unyielding_dead' WHERE `entry` = 25268;

UPDATE `creature_template` SET `modelid_A` = 11686, `modelid_H` = 11686 WHERE `entry` = 25268;

DELETE FROM `conditions` WHERE `SourceEntry` = 45388;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`,`SourceGroup`,`SourceEntry`,`SourceId`,`ElseGroup`,`ConditionTypeOrReference`,`ConditionTarget`,`ConditionValue1`,`ConditionValue2`,`ConditionValue3`,`NegativeCondition`,`ErrorType`,`ErrorTextId`,`ScriptName`,`Comment`) VALUES
(13, 0, 45388, 0, 0, 17, 0, 1, 25038, 0, 0, 0, 0, '', NULL);

DELETE FROM `conditions` WHERE `SourceEntry` = 45389;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`,`SourceGroup`,`SourceEntry`,`SourceId`,`ElseGroup`,`ConditionTypeOrReference`,`ConditionTarget`,`ConditionValue1`,`ConditionValue2`,`ConditionValue3`,`NegativeCondition`,`ErrorType`,`ErrorTextId`,`ScriptName`,`Comment`) VALUES
(13, 0, 45389, 0, 0, 17, 0, 1, 25265, 0, 0, 0, 0, '', NULL);