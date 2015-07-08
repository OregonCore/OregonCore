-- Karazhan: Chess Event
-- Remove old conditions
DELETE FROM conditions WHERE SourceTypeOrReferenceId = 13 AND ConditionValue2 = 22519;
DELETE FROM conditions WHERE SourceTypeOrReferenceId = 13 AND ConditionValue2 = 17305;
DELETE FROM conditions WHERE SourceTypeOrReferenceId = 13 AND ConditionValue2 = 17208;
DELETE FROM conditions WHERE SourceTypeOrReferenceId = 13 AND ConditionValue2 = 17469;
DELETE FROM conditions WHERE SourceTypeOrReferenceId = 13 AND SourceEntry = 37474;
DELETE FROM conditions WHERE SourceTypeOrReferenceId = 13 AND SourceEntry = 37465;
DELETE FROM conditions WHERE SourceTypeOrReferenceId = 13 AND SourceEntry = 37406;
DELETE FROM conditions WHERE SourceTypeOrReferenceId = 13 AND SourceEntry = 37427;
DELETE FROM conditions WHERE SourceTypeOrReferenceId = 13 AND SourceEntry = 37498;
DELETE FROM conditions WHERE SourceTypeOrReferenceId = 13 AND SourceEntry = 37453;
DELETE FROM conditions WHERE SourceTypeOrReferenceId = 13 AND SourceEntry = 37459;
DELETE FROM conditions WHERE SourceTypeOrReferenceId = 13 AND SourceEntry = 37476;
DELETE FROM conditions WHERE SourceTypeOrReferenceId = 13 AND SourceEntry = 37469;
DELETE FROM conditions WHERE SourceTypeOrReferenceId = 13 AND SourceEntry = 37413;
DELETE FROM conditions WHERE SourceTypeOrReferenceId = 13 AND SourceEntry = 37434;
DELETE FROM conditions WHERE SourceTypeOrReferenceId = 13 AND SourceEntry = 37502;
DELETE FROM conditions WHERE SourceTypeOrReferenceId = 13 AND SourceEntry = 37454;
DELETE FROM conditions WHERE SourceTypeOrReferenceId = 13 AND SourceEntry = 37461;
DELETE FROM conditions WHERE SourceTypeOrReferenceId = 13 AND SourceEntry = 37472;
DELETE FROM conditions WHERE SourceTypeOrReferenceId = 13 AND SourceEntry = 37150;
DELETE FROM conditions WHERE SourceTypeOrReferenceId = 13 AND SourceEntry = 37149;
DELETE FROM conditions WHERE SourceTypeOrReferenceId = 13 AND SourceEntry = 37147;
DELETE FROM conditions WHERE SourceTypeOrReferenceId = 13 AND SourceEntry = 37143;
DELETE FROM conditions WHERE SourceTypeOrReferenceId = 13 AND SourceEntry = 37142;
DELETE FROM conditions WHERE SourceTypeOrReferenceId = 13 AND SourceEntry = 32227;
DELETE FROM conditions WHERE SourceTypeOrReferenceId = 13 AND SourceEntry = 37348;
DELETE FROM conditions WHERE SourceTypeOrReferenceId = 13 AND SourceEntry = 37345;
DELETE FROM conditions WHERE SourceTypeOrReferenceId = 13 AND SourceEntry = 37337;
DELETE FROM conditions WHERE SourceTypeOrReferenceId = 13 AND SourceEntry = 37339;
DELETE FROM conditions WHERE SourceTypeOrReferenceId = 13 AND SourceEntry = 37220;
DELETE FROM conditions WHERE SourceTypeOrReferenceId = 13 AND SourceEntry = 32228;
DELETE FROM conditions WHERE SourceTypeOrReferenceId = 13 AND SourceEntry = 37471;
DELETE FROM conditions WHERE SourceTypeOrReferenceId = 13 AND SourceEntry = 39395;
DELETE FROM conditions WHERE SourceTypeOrReferenceId = 13 AND SourceEntry = 37428;
DELETE FROM conditions WHERE SourceTypeOrReferenceId = 13 AND SourceEntry = 39401;
DELETE FROM conditions WHERE SourceTypeOrReferenceId = 13 AND SourceEntry = 45259;
DELETE FROM conditions WHERE SourceTypeOrReferenceId = 13 AND SourceEntry = 39384;

DELETE FROM `creature_ai_scripts` WHERE creature_id = 22523;
DELETE FROM `creature_ai_scripts` WHERE creature_id = 25213;

INSERT INTO `conditions` (SourceTypeOrReferenceId, SourceEntry, ConditionTypeOrReference, ConditionValue1, ConditionValue2) VALUES (13, 45259, 17, 1, 25213);

INSERT INTO `conditions` (`SourceTypeOrReferenceId`,`SourceGroup`,`SourceEntry`,`SourceId`,`ElseGroup`,`ConditionTypeOrReference`,`ConditionTarget`,`ConditionValue1`,`ConditionValue2`,`ConditionValue3`,`NegativeCondition`,`ErrorType`,`ErrorTextId`,`ScriptName`,`Comment`) VALUES
(13,0,30012,0,0,17,0,1,17208,0,0,0,0,'',NULL),
(13,0,30012,0,0,17,0,1,17305,0,0,0,0,'',NULL),
(13,0,30284,0,0,17,0,1,17208,0,0,0,0,'',NULL),
(13,0,30284,0,0,17,0,1,17305,0,0,0,0,'',NULL),
(13,0,32227,0,0,17,0,1,17469,0,0,0,0,'',NULL),
(13,0,32227,0,0,17,0,1,21726,0,0,0,0,'',NULL),
(13,0,32227,0,0,17,0,1,21747,0,0,0,0,'',NULL),
(13,0,32227,0,0,17,0,1,21748,0,0,0,0,'',NULL),
(13,0,32227,0,0,17,0,1,21750,0,0,0,0,'',NULL),
(13,0,32227,0,0,17,0,1,21752,0,0,0,0,'',NULL),
(13,0,32228,0,0,17,0,1,17211,0,0,0,0,'',NULL),
(13,0,32228,0,0,17,0,1,21160,0,0,0,0,'',NULL),
(13,0,32228,0,0,17,0,1,21664,0,0,0,0,'',NULL),
(13,0,32228,0,0,17,0,1,21682,0,0,0,0,'',NULL),
(13,0,32228,0,0,17,0,1,21683,0,0,0,0,'',NULL),
(13,0,32228,0,0,17,0,1,21684,0,0,0,0,'',NULL),
(13,0,32260,0,0,17,0,1,17208,0,0,0,0,'',NULL),
(13,0,32260,0,0,17,0,1,17305,0,0,0,0,'',NULL),
(13,0,37142,0,0,17,0,1,17469,0,0,0,0,'',NULL),
(13,0,37142,0,0,17,0,1,21726,0,0,0,0,'',NULL),
(13,0,37142,0,0,17,0,1,21747,0,0,0,0,'',NULL),
(13,0,37142,0,0,17,0,1,21748,0,0,0,0,'',NULL),
(13,0,37142,0,0,17,0,1,21750,0,0,0,0,'',NULL),
(13,0,37142,0,0,17,0,1,21752,0,0,0,0,'',NULL),
(13,0,37143,0,0,17,0,1,17469,0,0,0,0,'',NULL),
(13,0,37143,0,0,17,0,1,21726,0,0,0,0,'',NULL),
(13,0,37143,0,0,17,0,1,21747,0,0,0,0,'',NULL),
(13,0,37143,0,0,17,0,1,21748,0,0,0,0,'',NULL),
(13,0,37143,0,0,17,0,1,21750,0,0,0,0,'',NULL),
(13,0,37143,0,0,17,0,1,21752,0,0,0,0,'',NULL),
(13,0,37144,0,0,17,0,1,17208,0,0,0,0,'',NULL),
(13,0,37144,0,0,17,0,1,17305,0,0,0,0,'',NULL),
(13,0,37146,0,0,17,0,1,17208,0,0,0,0,'',NULL),
(13,0,37146,0,0,17,0,1,17305,0,0,0,0,'',NULL),
(13,0,37147,0,0,17,0,1,17469,0,0,0,0,'',NULL),
(13,0,37147,0,0,17,0,1,21726,0,0,0,0,'',NULL),
(13,0,37147,0,0,17,0,1,21747,0,0,0,0,'',NULL),
(13,0,37147,0,0,17,0,1,21748,0,0,0,0,'',NULL),
(13,0,37147,0,0,17,0,1,21750,0,0,0,0,'',NULL),
(13,0,37147,0,0,17,0,1,21752,0,0,0,0,'',NULL),
(13,0,37148,0,0,17,0,1,17208,0,0,0,0,'',NULL),
(13,0,37148,0,0,17,0,1,17305,0,0,0,0,'',NULL),
(13,0,37149,0,0,17,0,1,17469,0,0,0,0,'',NULL),
(13,0,37149,0,0,17,0,1,21726,0,0,0,0,'',NULL),
(13,0,37149,0,0,17,0,1,21747,0,0,0,0,'',NULL),
(13,0,37149,0,0,17,0,1,21748,0,0,0,0,'',NULL),
(13,0,37149,0,0,17,0,1,21750,0,0,0,0,'',NULL),
(13,0,37149,0,0,17,0,1,21752,0,0,0,0,'',NULL),
(13,0,37150,0,0,17,0,1,17469,0,0,0,0,'',NULL),
(13,0,37150,0,0,17,0,1,21726,0,0,0,0,'',NULL),
(13,0,37150,0,0,17,0,1,21747,0,0,0,0,'',NULL),
(13,0,37150,0,0,17,0,1,21748,0,0,0,0,'',NULL),
(13,0,37150,0,0,17,0,1,21750,0,0,0,0,'',NULL),
(13,0,37151,0,0,17,0,1,17208,0,0,0,0,'',NULL),
(13,0,37151,0,0,17,0,1,17305,0,0,0,0,'',NULL),
(13,0,37152,0,0,17,0,1,17208,0,0,0,0,'',NULL),
(13,0,37152,0,0,17,0,1,17305,0,0,0,0,'',NULL),
(13,0,37153,0,0,17,0,1,17208,0,0,0,0,'',NULL),
(13,0,37153,0,0,17,0,1,17305,0,0,0,0,'',NULL),
(13,0,37220,0,0,17,0,1,17211,0,0,0,0,'',NULL),
(13,0,37220,0,0,17,0,1,21160,0,0,0,0,'',NULL),
(13,0,37220,0,0,17,0,1,21664,0,0,0,0,'',NULL),
(13,0,37220,0,0,17,0,1,21682,0,0,0,0,'',NULL),
(13,0,37220,0,0,17,0,1,21683,0,0,0,0,'',NULL),
(13,0,37220,0,0,17,0,1,21684,0,0,0,0,'',NULL),
(13,0,37337,0,0,17,0,1,17211,0,0,0,0,'',NULL),
(13,0,37337,0,0,17,0,1,21160,0,0,0,0,'',NULL),
(13,0,37337,0,0,17,0,1,21664,0,0,0,0,'',NULL),
(13,0,37337,0,0,17,0,1,21682,0,0,0,0,'',NULL),
(13,0,37337,0,0,17,0,1,21683,0,0,0,0,'',NULL),
(13,0,37337,0,0,17,0,1,21684,0,0,0,0,'',NULL),
(13,0,37339,0,0,17,0,1,17211,0,0,0,0,'',NULL),
(13,0,37339,0,0,17,0,1,21160,0,0,0,0,'',NULL),
(13,0,37339,0,0,17,0,1,21664,0,0,0,0,'',NULL),
(13,0,37339,0,0,17,0,1,21682,0,0,0,0,'',NULL),
(13,0,37339,0,0,17,0,1,21683,0,0,0,0,'',NULL),
(13,0,37339,0,0,17,0,1,21684,0,0,0,0,'',NULL),
(13,0,37345,0,0,17,0,1,17211,0,0,0,0,'',NULL),
(13,0,37345,0,0,17,0,1,21160,0,0,0,0,'',NULL),
(13,0,37345,0,0,17,0,1,21664,0,0,0,0,'',NULL),
(13,0,37345,0,0,17,0,1,21682,0,0,0,0,'',NULL),
(13,0,37345,0,0,17,0,1,21683,0,0,0,0,'',NULL),
(13,0,37345,0,0,17,0,1,21684,0,0,0,0,'',NULL),
(13,0,37348,0,0,17,0,1,17211,0,0,0,0,'',NULL),
(13,0,37348,0,0,17,0,1,21160,0,0,0,0,'',NULL),
(13,0,37348,0,0,17,0,1,21664,0,0,0,0,'',NULL),
(13,0,37348,0,0,17,0,1,21682,0,0,0,0,'',NULL),
(13,0,37348,0,0,17,0,1,21683,0,0,0,0,'',NULL),
(13,0,37348,0,0,17,0,1,21684,0,0,0,0,'',NULL),
(13,0,37406,0,0,17,0,1,17469,0,0,0,0,'',NULL),
(13,0,37406,0,0,17,0,1,21726,0,0,0,0,'',NULL),
(13,0,37406,0,0,17,0,1,21747,0,0,0,0,'',NULL),
(13,0,37406,0,0,17,0,1,21748,0,0,0,0,'',NULL),
(13,0,37406,0,0,17,0,1,21750,0,0,0,0,'',NULL),
(13,0,37406,0,0,17,0,1,21752,0,0,0,0,'',NULL),
(13,0,37413,0,0,17,0,1,17211,0,0,0,0,'',NULL),
(13,0,37413,0,0,17,0,1,21160,0,0,0,0,'',NULL),
(13,0,37413,0,0,17,0,1,21664,0,0,0,0,'',NULL),
(13,0,37413,0,0,17,0,1,21682,0,0,0,0,'',NULL),
(13,0,37413,0,0,17,0,1,21683,0,0,0,0,'',NULL),
(13,0,37413,0,0,17,0,1,21684,0,0,0,0,'',NULL),
(13,0,37427,0,0,17,0,1,17469,0,0,0,0,'',NULL),
(13,0,37427,0,0,17,0,1,21726,0,0,0,0,'',NULL),
(13,0,37427,0,0,17,0,1,21747,0,0,0,0,'',NULL),
(13,0,37427,0,0,17,0,1,21748,0,0,0,0,'',NULL),
(13,0,37427,0,0,17,0,1,21750,0,0,0,0,'',NULL),
(13,0,37427,0,0,17,0,1,21752,0,0,0,0,'',NULL),
(13,0,37428,0,0,17,0,1,17211,0,0,0,0,'',NULL),
(13,0,37428,0,0,17,0,1,21160,0,0,0,0,'',NULL),
(13,0,37428,0,0,17,0,1,21664,0,0,0,0,'',NULL),
(13,0,37428,0,0,17,0,1,21682,0,0,0,0,'',NULL),
(13,0,37428,0,0,17,0,1,21683,0,0,0,0,'',NULL),
(13,0,37428,0,0,17,0,1,21684,0,0,0,0,'',NULL),
(13,0,37434,0,0,17,0,1,17211,0,0,0,0,'',NULL),
(13,0,37434,0,0,17,0,1,21160,0,0,0,0,'',NULL),
(13,0,37434,0,0,17,0,1,21664,0,0,0,0,'',NULL),
(13,0,37434,0,0,17,0,1,21682,0,0,0,0,'',NULL),
(13,0,37434,0,0,17,0,1,21683,0,0,0,0,'',NULL),
(13,0,37434,0,0,17,0,1,21684,0,0,0,0,'',NULL),
(13,0,37453,0,0,17,0,1,17469,0,0,0,0,'',NULL),
(13,0,37453,0,0,17,0,1,21726,0,0,0,0,'',NULL),
(13,0,37453,0,0,17,0,1,21747,0,0,0,0,'',NULL),
(13,0,37453,0,0,17,0,1,21748,0,0,0,0,'',NULL),
(13,0,37453,0,0,17,0,1,21750,0,0,0,0,'',NULL),
(13,0,37453,0,0,17,0,1,21752,0,0,0,0,'',NULL),
(13,0,37454,0,0,17,0,1,17211,0,0,0,0,'',NULL),
(13,0,37454,0,0,17,0,1,21160,0,0,0,0,'',NULL),
(13,0,37454,0,0,17,0,1,21664,0,0,0,0,'',NULL),
(13,0,37454,0,0,17,0,1,21682,0,0,0,0,'',NULL),
(13,0,37454,0,0,17,0,1,21683,0,0,0,0,'',NULL),
(13,0,37454,0,0,17,0,1,21684,0,0,0,0,'',NULL),
(13,0,37459,0,0,17,0,1,17469,0,0,0,0,'',NULL),
(13,0,37459,0,0,17,0,1,21726,0,0,0,0,'',NULL),
(13,0,37459,0,0,17,0,1,21747,0,0,0,0,'',NULL),
(13,0,37459,0,0,17,0,1,21748,0,0,0,0,'',NULL),
(13,0,37459,0,0,17,0,1,21750,0,0,0,0,'',NULL),
(13,0,37459,0,0,17,0,1,21752,0,0,0,0,'',NULL),
(13,0,37461,0,0,17,0,1,17211,0,0,0,0,'',NULL),
(13,0,37461,0,0,17,0,1,21160,0,0,0,0,'',NULL),
(13,0,37461,0,0,17,0,1,21664,0,0,0,0,'',NULL),
(13,0,37461,0,0,17,0,1,21682,0,0,0,0,'',NULL),
(13,0,37461,0,0,17,0,1,21683,0,0,0,0,'',NULL),
(13,0,37461,0,0,17,0,1,21684,0,0,0,0,'',NULL),
(13,0,37465,0,0,17,0,1,17469,0,0,0,0,'',NULL),
(13,0,37465,0,0,17,0,1,21726,0,0,0,0,'',NULL),
(13,0,37465,0,0,17,0,1,21747,0,0,0,0,'',NULL),
(13,0,37465,0,0,17,0,1,21748,0,0,0,0,'',NULL),
(13,0,37465,0,0,17,0,1,21750,0,0,0,0,'',NULL),
(13,0,37465,0,0,17,0,1,21752,0,0,0,0,'',NULL),
(13,0,37469,0,0,17,0,1,17211,0,0,0,0,'',NULL),
(13,0,37469,0,0,17,0,1,21160,0,0,0,0,'',NULL),
(13,0,37469,0,0,17,0,1,21664,0,0,0,0,'',NULL),
(13,0,37469,0,0,17,0,1,21682,0,0,0,0,'',NULL),
(13,0,37469,0,0,17,0,1,21683,0,0,0,0,'',NULL),
(13,0,37469,0,0,17,0,1,21684,0,0,0,0,'',NULL),
(13,0,37471,0,0,17,0,1,17211,0,0,0,0,'',NULL),
(13,0,37471,0,0,17,0,1,21160,0,0,0,0,'',NULL),
(13,0,37471,0,0,17,0,1,21664,0,0,0,0,'',NULL),
(13,0,37471,0,0,17,0,1,21682,0,0,0,0,'',NULL),
(13,0,37471,0,0,17,0,1,21683,0,0,0,0,'',NULL),
(13,0,37471,0,0,17,0,1,21684,0,0,0,0,'',NULL),
(13,0,37472,0,0,17,0,1,17469,0,0,0,0,'',NULL),
(13,0,37472,0,0,17,0,1,21726,0,0,0,0,'',NULL),
(13,0,37472,0,0,17,0,1,21747,0,0,0,0,'',NULL),
(13,0,37472,0,0,17,0,1,21748,0,0,0,0,'',NULL),
(13,0,37472,0,0,17,0,1,21750,0,0,0,0,'',NULL),
(13,0,37472,0,0,17,0,1,21752,0,0,0,0,'',NULL),
(13,0,37474,0,0,17,0,1,17469,0,0,0,0,'',NULL),
(13,0,37474,0,0,17,0,1,21726,0,0,0,0,'',NULL),
(13,0,37474,0,0,17,0,1,21747,0,0,0,0,'',NULL),
(13,0,37474,0,0,17,0,1,21748,0,0,0,0,'',NULL),
(13,0,37474,0,0,17,0,1,21750,0,0,0,0,'',NULL),
(13,0,37474,0,0,17,0,1,21752,0,0,0,0,'',NULL),
(13,0,37476,0,0,17,0,1,17211,0,0,0,0,'',NULL),
(13,0,37476,0,0,17,0,1,21160,0,0,0,0,'',NULL),
(13,0,37476,0,0,17,0,1,21664,0,0,0,0,'',NULL),
(13,0,37476,0,0,17,0,1,21682,0,0,0,0,'',NULL),
(13,0,37476,0,0,17,0,1,21683,0,0,0,0,'',NULL),
(13,0,37476,0,0,17,0,1,21684,0,0,0,0,'',NULL),
(13,0,37498,0,0,17,0,1,17469,0,0,0,0,'',NULL),
(13,0,37498,0,0,17,0,1,21726,0,0,0,0,'',NULL),
(13,0,37498,0,0,17,0,1,21747,0,0,0,0,'',NULL),
(13,0,37498,0,0,17,0,1,21748,0,0,0,0,'',NULL),
(13,0,37498,0,0,17,0,1,21750,0,0,0,0,'',NULL),
(13,0,37498,0,0,17,0,1,21752,0,0,0,0,'',NULL),
(13,0,37502,0,0,17,0,1,17211,0,0,0,0,'',NULL),
(13,0,37502,0,0,17,0,1,21160,0,0,0,0,'',NULL),
(13,0,37502,0,0,17,0,1,21664,0,0,0,0,'',NULL),
(13,0,37502,0,0,17,0,1,21682,0,0,0,0,'',NULL),
(13,0,37502,0,0,17,0,1,21683,0,0,0,0,'',NULL),
(13,0,37502,0,0,17,0,1,21684,0,0,0,0,'',NULL),
(13,0,39384,0,0,17,0,1,17211,0,0,0,0,'',NULL),
(13,0,39384,0,0,17,0,1,17469,0,0,0,0,'',NULL),
(13,0,39384,0,0,17,0,1,21160,0,0,0,0,'',NULL),
(13,0,39384,0,0,17,0,1,21664,0,0,0,0,'',NULL),
(13,0,39384,0,0,17,0,1,21682,0,0,0,0,'',NULL),
(13,0,39384,0,0,17,0,1,21683,0,0,0,0,'',NULL),
(13,0,39384,0,0,17,0,1,21684,0,0,0,0,'',NULL),
(13,0,39384,0,0,17,0,1,21726,0,0,0,0,'',NULL),
(13,0,39384,0,0,17,0,1,21747,0,0,0,0,'',NULL),
(13,0,39384,0,0,17,0,1,21748,0,0,0,0,'',NULL),
(13,0,39384,0,0,17,0,1,21750,0,0,0,0,'',NULL),
(13,0,39384,0,0,17,0,1,21752,0,0,0,0,'',NULL),
(13,0,39395,0,0,17,0,1,17211,0,0,0,0,'',NULL),
(13,0,39395,0,0,17,0,1,17469,0,0,0,0,'',NULL),
(13,0,39395,0,0,17,0,1,21160,0,0,0,0,'',NULL),
(13,0,39395,0,0,17,0,1,21664,0,0,0,0,'',NULL),
(13,0,39395,0,0,17,0,1,21682,0,0,0,0,'',NULL),
(13,0,39395,0,0,17,0,1,21683,0,0,0,0,'',NULL),
(13,0,39395,0,0,17,0,1,21684,0,0,0,0,'',NULL),
(13,0,39395,0,0,17,0,1,21726,0,0,0,0,'',NULL),
(13,0,39395,0,0,17,0,1,21747,0,0,0,0,'',NULL),
(13,0,39395,0,0,17,0,1,21748,0,0,0,0,'',NULL),
(13,0,39395,0,0,17,0,1,21750,0,0,0,0,'',NULL),
(13,0,39395,0,0,17,0,1,21752,0,0,0,0,'',NULL),
(13,0,39401,0,0,17,0,1,17211,0,0,0,0,'',NULL),
(13,0,39401,0,0,17,0,1,17469,0,0,0,0,'',NULL),
(13,0,39401,0,0,17,0,1,21160,0,0,0,0,'',NULL),
(13,0,39401,0,0,17,0,1,21664,0,0,0,0,'',NULL),
(13,0,39401,0,0,17,0,1,21682,0,0,0,0,'',NULL),
(13,0,39401,0,0,17,0,1,21683,0,0,0,0,'',NULL),
(13,0,39401,0,0,17,0,1,21684,0,0,0,0,'',NULL),
(13,0,39401,0,0,17,0,1,21726,0,0,0,0,'',NULL),
(13,0,39401,0,0,17,0,1,21747,0,0,0,0,'',NULL),
(13,0,39401,0,0,17,0,1,21748,0,0,0,0,'',NULL),
(13,0,39401,0,0,17,0,1,21750,0,0,0,0,'',NULL),
(13,0,39401,0,0,17,0,1,21752,0,0,0,0,'',NULL);

-- Spells for chess pieces
-- Kings  1 any dir
UPDATE creature_template SET armor= 0, spell1=37151, spell2=30284, spell3=37471, spell4=37474 WHERE entry=21684;
UPDATE creature_template SET armor= 0, spell1=37151, spell2=30284, spell3=37472, spell4=37476 WHERE entry=21752;

-- Queens Straigt 3 / Diagonal 2
UPDATE creature_template SET armor= 0, spell1=37148, spell2=30284, spell3=37462, spell4=37465 WHERE entry=21683;
UPDATE creature_template SET armor= 0, spell1=37148, spell2=30284, spell3=37463, spell4=37469 WHERE entry=21750;

-- Bishop 1 any dir
UPDATE creature_template SET armor= 0, spell1=37151, spell2=30284, spell3=37455, spell4=37459 WHERE entry=21682;
UPDATE creature_template SET armor= 0, spell1=37151, spell2=30284, spell3=37456, spell4=37461 WHERE entry=21747;

-- Knight Straight 2 / Diagonal 1
UPDATE creature_template SET armor= 0, spell1=37144, spell2=30284, spell3=37453, spell4=37498 WHERE entry=21664;
UPDATE creature_template SET armor= 0, spell1=37144, spell2=30284, spell3=37454, spell4=37502 WHERE entry=21748;

-- Rook 1 any dir
UPDATE creature_template SET armor= 0, spell1=37151, spell2=30284, spell3=37427, spell4=37432 WHERE entry=21160;
UPDATE creature_template SET armor= 0, spell1=37151, spell2=30284, spell3=37428, spell4=37434 WHERE entry=21726;

-- Pawn 1 andy dir
UPDATE creature_template SET armor= 0, spell1=37151, spell2=30284, spell3=37406, spell4=37414 WHERE entry=17211;
UPDATE creature_template SET armor= 0, spell1=37151, spell2=30284, spell3=37413, spell4=37416 WHERE entry=17469;

UPDATE creature_template SET scriptname = "chess_move_trigger", flags_extra = 130, unit_flags = 32768 WHERE entry = 17305;
UPDATE creature_template SET scriptname = "chess_move_trigger", flags_extra = 130, unit_flags = 32768 WHERE entry = 17208;
UPDATE creature_template SET scriptname = "chess_move_marker" WHERE entry = 22519;
UPDATE creature_template SET scriptname = "chess_victory_controler" WHERE entry = 22524;
UPDATE creature_template SET scriptname = "npc_echo_of_medivh" WHERE entry=16816;
UPDATE creature_template SET scriptname = "chess_npc" WHERE entry IN (17469,17211,21748,21664,21750,21683,21747,21682,21726,21160,21752,21684);



-- Update ChessPiece Creature_template
UPDATE creature_template SET minhealth = 65000, maxhealth = 65000 WHERE entry=21664;
UPDATE creature_template SET minhealth = 50000, maxhealth = 50000 WHERE entry=17211;
UPDATE creature_template SET minhealth = 80000, maxhealth = 80000 WHERE entry=21160;
UPDATE creature_template SET minhealth = 60000, maxhealth = 60000 WHERE entry=21682;
UPDATE creature_template SET minhealth = 80000, maxhealth = 80000 WHERE entry=21683;
UPDATE creature_template SET minhealth = 150000, maxhealth = 150000 WHERE entry=21684;

UPDATE creature_template SET minhealth = 65000, maxhealth = 65000 WHERE entry=21748;
UPDATE creature_template SET minhealth = 50000, maxhealth = 50000 WHERE entry=17469;
UPDATE creature_template SET minhealth = 80000, maxhealth = 80000 WHERE entry=21726;
UPDATE creature_template SET minhealth = 60000, maxhealth = 60000 WHERE entry=21747;
UPDATE creature_template SET minhealth = 80000, maxhealth = 80000 WHERE entry=21750;
UPDATE creature_template SET minhealth = 150000, maxhealth = 150000 WHERE entry=21752;


-- DB AI
INSERT INTO `creature_ai_scripts` (`creature_id`, `event_type`, `event_flags`, `action1_type`, `action1_param1`, `comment`) VALUES (22523, 1, 11, 11, 39390,'Cast Chessfield, for light visual');
INSERT INTO `creature_ai_scripts` (`creature_id`, `event_type`, `action1_type`, `action1_param1`, `action1_param2`, `comment`) VALUES (25213, 6, 104, 185119, 600000, 'Spawn Dust Covered Chest');

-- Script texts
INSERT INTO `script_texts` (`entry`, `content_default`, `sound`, `type`, `language`, `comment`) VALUES(-1910000, 'Very well. Let the game begin', 10338, 1, 0, 'CHESS_EVENT_BEGIN');
INSERT INTO `script_texts` (`entry`, `content_default`, `sound`, `type`, `language`, `comment`) VALUES(-1910001, 'Perhaps a change is in order.', 10357, 1, 0, 'CHESS_EVENT_CHEATING_1');
INSERT INTO `script_texts` (`entry`, `content_default`, `sound`, `type`, `language`, `comment`) VALUES(-1910002, 'Time for an alternative scenario.', 10358, 1, 0, 'CHESS_EVENT_CHEATING_2');
INSERT INTO `script_texts` (`entry`, `content_default`, `sound`, `type`, `language`, `comment`) VALUES(-1910003, 'One must not become too complacent.', 10359, 1, 0, 'CHESS_EVENT_CHEATING_3');
INSERT INTO `script_texts` (`entry`, `content_default`, `sound`, `type`, `language`, `comment`) VALUES(-1910004, 'Let us see.', 10340, 1, 0, 'CHESS_EVENT_P_L_PAWN_1');
INSERT INTO `script_texts` (`entry`, `content_default`, `sound`, `type`, `language`, `comment`) VALUES(-1910005, 'A transparent stratagem.', 10339, 1, 0, 'CHESS_EVENT_P_L_PAWN_2');
INSERT INTO `script_texts` (`entry`, `content_default`, `sound`, `type`, `language`, `comment`) VALUES(-1910006, 'Ah, the wheels have begun to turn.', 10341, 1, 0, 'CHESS_EVENT_P_L_PAWN_3');
INSERT INTO `script_texts` (`entry`, `content_default`, `sound`, `type`, `language`, `comment`) VALUES(-1910007, 'Hmm.', 10342, 1, 0, 'CHESS_EVENT_M_L_PAWN_1');
INSERT INTO `script_texts` (`entry`, `content_default`, `sound`, `type`, `language`, `comment`) VALUES(-1910008, 'No matter.', 10344, 1, 0, 'CHESS_EVENT_M_L_PAWN_2');
INSERT INTO `script_texts` (`entry`, `content_default`, `sound`, `type`, `language`, `comment`) VALUES(-1910009, 'Interesting.', 10343, 1, 0, 'CHESS_EVENT_M_L_PAWN_3');
INSERT INTO `script_texts` (`entry`, `content_default`, `sound`, `type`, `language`, `comment`) VALUES(-1910010, 'Foolish! Very floolish!', 10345, 1, 0, 'CHESS_EVENT_P_L_ROOK_1');
INSERT INTO `script_texts` (`entry`, `content_default`, `sound`, `type`, `language`, `comment`) VALUES(-1910011, 'A minor conern.', 10346, 1, 0, 'CHESS_EVENT_M_L_ROOK_1');
INSERT INTO `script_texts` (`entry`, `content_default`, `sound`, `type`, `language`, `comment`) VALUES(-1910012, 'Yes… all according to plan.', 10349, 1, 0, 'CHESS_EVENT_P_L_KNIGHT_1');
INSERT INTO `script_texts` (`entry`, `content_default`, `sound`, `type`, `language`, `comment`) VALUES(-1910013, 'Yes…of course.', 10350, 1, 0, 'CHESS_EVENT_M_L_KNIGHT_1');
INSERT INTO `script_texts` (`entry`, `content_default`, `sound`, `type`, `language`, `comment`) VALUES(-1910014, 'The slightest loss of conentration is all it takes.', 10347, 1, 0, 'CHESS_EVENT_P_L_BISHOP_1');
INSERT INTO `script_texts` (`entry`, `content_default`, `sound`, `type`, `language`, `comment`) VALUES(-1910015, 'A necessary sacrifice.', 10348, 1, 0, 'CHESS_EVENT_M_L_BISHOP_1');
INSERT INTO `script_texts` (`entry`, `content_default`, `sound`, `type`, `language`, `comment`) VALUES(-1910016, 'Now it gets interesting.', 10351, 1, 0, 'CHESS_EVENT_P_L_QUEEN_1');
INSERT INTO `script_texts` (`entry`, `content_default`, `sound`, `type`, `language`, `comment`) VALUES(-1910017, 'Ahh, I should have known.', 10352, 1, 0, 'CHESS_EVENT_M_L_QUEEN_1');
INSERT INTO `script_texts` (`entry`, `content_default`, `sound`, `type`, `language`, `comment`) VALUES(-1910018, 'As it should be.', 10354, 1, 0, 'CHESS_EVENT_P_L_KING_1');
INSERT INTO `script_texts` (`entry`, `content_default`, `sound`, `type`, `language`, `comment`) VALUES(-1910019, 'And so, the end draws near.', 10353, 1, 0, 'CHESS_EVENT_M_L_KING_1');
INSERT INTO `script_texts` (`entry`, `content_default`, `sound`, `type`, `language`, `comment`) VALUES(-1910020, 'Nothing less than perfection will do.', 10355, 1, 0, 'CHESS_EVENT_P_LOSE_1');
INSERT INTO `script_texts` (`entry`, `content_default`, `sound`, `type`, `language`, `comment`) VALUES(-1910021, 'And so it ends.', 10355, 1, 0, 'CHESS_EVENT_M_LOSE_1');


-- spawns

DELETE FROM `creature` WHERE `id`=21752;
DELETE FROM `creature` WHERE `id`=21750;
DELETE FROM `creature` WHERE `id`=21747;
DELETE FROM `creature` WHERE `id`=21748;
DELETE FROM `creature` WHERE `id`=21726;
DELETE FROM `creature` WHERE `id`=17469;
DELETE FROM `creature` WHERE `id`=21684;
DELETE FROM `creature` WHERE `id`=21683;
DELETE FROM `creature` WHERE `id`=21682;
DELETE FROM `creature` WHERE `id`=21664;
DELETE FROM `creature` WHERE `id`=21160;
DELETE FROM `creature` WHERE `id`=17211;
DELETE FROM `creature` WHERE `id`=25213;
DELETE FROM `creature` WHERE `id`=16816;
DELETE FROM `creature` WHERE `id`=17305;
DELETE FROM `creature` WHERE `id`=17208;
DELETE FROM `creature` WHERE `id`=22520;

INSERT INTO `creature` (`id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`) VALUES 
 (16816,532,1,0,1512,-11098.9,-1850.58,221.07,5.17671,25,0,0,18000,0,0),
 (17208,532,1,0,0,-11097.5,-1886.27,220.668,2.1671,25,0,0,8,0,0),
 (17208,532,1,0,0,-11083,-1895.99,220.668,5.37153,25,0,0,8,0,0),
 (17208,532,1,0,0,-11083.6,-1903.86,220.668,2.27313,25,0,0,8,0,0),
 (17208,532,1,0,0,-11096.8,-1878.46,220.668,5.3676,25,0,0,8,0,0),
 (17208,532,1,0,0,-11089.7,-1887.28,220.668,5.3676,25,0,0,8,0,0),
 (17208,532,1,0,0,-11088.7,-1879.35,220.668,2.22601,25,0,0,8,0,0),
 (17208,532,1,0,0,-11095.7,-1870.67,220.668,2.24172,25,0,0,8,0,0),
 (17208,532,1,0,0,-11103.5,-1869.53,220.668,3.79681,25,0,0,8,0,0),
 (17208,532,1,0,0,-11081.8,-1888.17,220.668,2.23387,25,0,0,8,0,0),
 (17208,532,1,0,0,-11074.9,-1896.92,220.668,3.82823,25,0,0,8,0,0),
 (17208,532,1,0,0,-11081.1,-1880.33,220.668,5.36368,25,0,0,8,0,0),
 (17208,532,1,0,0,-11074.2,-1888.93,220.668,5.37546,25,0,0,8,0,0),
 (17208,532,1,0,0,-11087,-1863.75,220.668,2.24565,25,0,0,8,0,0),
 (17208,532,1,0,0,-11095.2,-1862.65,220.668,3.82037,25,0,0,8,0,0),
 (17208,532,1,0,0,-11088.1,-1871.54,220.668,5.37546,25,0,0,8,0,0),
 (17208,532,1,0,0,-11073.3,-1881.13,220.668,2.23387,25,0,0,8,0,0),
 (17208,532,1,0,0,-11080.2,-1872.46,220.668,2.23387,25,0,0,8,0,0),
 (17208,532,1,0,0,-11065.5,-1882.12,220.668,5.38724,25,0,0,8,0,0),
 (17208,532,1,0,0,-11066.3,-1889.96,220.668,3.82037,25,0,0,8,0,0),
 (17208,532,1,0,0,-11072.5,-1873.41,220.668,5.39116,25,0,0,8,0,0),
 (17208,532,1,0,0,-11086.5,-1855.77,220.668,3.78896,25,0,0,8,0,0),
 (17208,532,1,0,0,-11079.4,-1864.77,220.668,5.39116,25,0,0,8,0,0),
 (17208,532,1,0,0,-11064.6,-1874.46,220.668,2.21816,25,0,0,8,0,0),
 (17208,532,1,0,0,-11071.5,-1865.68,220.668,2.24565,25,0,0,8,0,0),
 (17208,532,1,0,0,-11078.5,-1856.88,220.668,2.23779,25,0,0,8,0,0),
 (17208,532,1,0,0,-11057.4,-1882.92,220.668,3.82037,25,0,0,8,0,0),
 (17208,532,1,0,0,-11063.8,-1866.8,220.668,5.38723,25,0,0,8,0,0),
 (17208,532,1,0,0,-11057,-1875.06,220.668,5.38723,25,0,0,8,0,0),
 (17208,532,1,0,0,-11077.8,-1849.08,220.668,3.81644,25,0,0,8,0,0),
 (17208,532,1,0,0,-11071,-1857.8,220.668,5.38723,25,0,0,8,0,0),
 (17208,532,1,0,0,-11104.5,-1877.4,220.668,0.657283,25,0,0,8,0,0),
 (17208,532,1,0,0,-11090.6,-1895.06,220.667,0.612717,25,0,0,8,0,0),
 (17211,532,1,0,0,-11082.9,-1895.75,220.668,0.759299,7200,0,0,35000,0,0),
 (17211,532,1,0,0,-11079.4,-1900.17,220.668,0.649344,7200,0,0,35000,0,0),
 (17211,532,1,0,0,-11103.7,-1869.63,220.668,0.661124,7200,0,0,35000,0,0),
 (17211,532,1,0,0,-11100.2,-1873.96,220.668,0.597337,7200,0,0,35000,0,0),
 (17211,532,1,0,0,-11096.8,-1878.21,220.668,0.593409,7200,0,0,35000,0,0),
 (17211,532,1,0,0,-11089.7,-1887.03,220.668,0.617927,7200,0,0,35000,0,0),
 (17211,532,1,0,0,-11086.2,-1891.53,220.668,0.617928,7200,0,0,35000,0,0),
 (17211,532,1,0,0,-11093.4,-1882.59,220.668,0.671956,7200,0,0,35000,0,0),
 (17305,532,1,0,0,-11067,-1862.02,220.668,2.22993,25,0,0,8,0,0),
 (17305,532,1,0,0,-11074,-1853.41,220.668,2.21029,25,0,0,8,0,0),
 (17305,532,1,0,0,-11060.1,-1870.61,220.668,2.16997,25,0,0,8,0,0),
 (17305,532,1,0,0,-11068.2,-1870.23,220.668,5.36654,25,0,0,8,0,0),
 (17305,532,1,0,0,-11061,-1878.93,220.667,0.655914,25,0,0,8,0,0),
 (17305,532,1,0,0,-11053.4,-1879.59,220.667,0.655914,25,0,0,8,0,0),
 (17305,532,1,0,0,-11082.1,-1852.23,220.668,2.23673,25,0,0,8,0,0),
 (17305,532,1,0,0,-11075.2,-1861.25,220.667,0.655914,25,0,0,8,0,0),
 (17305,532,1,0,0,-11068.9,-1877.75,220.668,2.22102,25,0,0,8,0,0),
 (17305,532,1,0,0,-11075.9,-1869.1,220.668,2.2171,25,0,0,8,0,0),
 (17305,532,1,0,0,-11076.8,-1876.83,220.668,5.37833,25,0,0,8,0,0),
 (17305,532,1,0,0,-11069.8,-1885.52,220.668,5.38618,25,0,0,8,0,0),
 (17305,532,1,0,0,-11061.9,-1886.54,220.668,0.685568,25,0,0,8,0,0),
 (17305,532,1,0,0,-11083.6,-1868.15,220.668,0.66201,25,0,0,8,0,0),
 (17305,532,1,0,0,-11090.7,-1859.35,220.668,0.66987,25,0,0,8,0,0),
 (17305,532,1,0,0,-11082.8,-1860.33,220.668,0.62667,25,0,0,8,0,0),
 (17305,532,1,0,0,-11091.5,-1867.26,220.668,0.689505,25,0,0,8,0,0),
 (17305,532,1,0,0,-11099.2,-1866.13,220.668,0.642382,25,0,0,8,0,0),
 (17305,532,1,0,0,-11084.5,-1875.94,220.668,0.614892,25,0,0,8,0,0),
 (17305,532,1,0,0,-11092.2,-1875.01,220.668,0.658089,25,0,0,8,0,0),
 (17305,532,1,0,0,-11085.5,-1883.86,220.668,0.709137,25,0,0,8,0,0),
 (17305,532,1,0,0,-11078.5,-1892.51,220.668,5.32729,25,0,0,8,0,0),
 (17305,532,1,0,0,-11070.6,-1893.47,220.668,0.662017,25,0,0,8,0,0),
 (17305,532,1,0,0,-11077.4,-1884.52,220.668,0.630601,25,0,0,8,0,0),
 (17305,532,1,0,0,-11079.3,-1900.43,220.668,0.634526,25,0,0,8,0,0),
 (17305,532,1,0,0,-11086.2,-1891.32,220.668,0.665941,25,0,0,8,0,0),
 (17305,532,1,0,0,-11108,-1872.86,220.668,3.82325,25,0,0,8,0,0),
 (17305,532,1,0,0,-11100.9,-1881.92,220.667,0.769797,25,0,0,8,0,0),
 (17305,532,1,0,0,-11094.2,-1890.75,220.668,0.634526,25,0,0,8,0,0),
 (17305,532,1,0,0,-11087,-1899.81,220.667,0.684773,25,0,0,8,0,0),
 (17305,532,1,0,0,-11093,-1882.84,220.668,0.638456,25,0,0,8,0,0),
 (17305,532,1,0,0,-11100,-1874,220.668,0.618825,25,0,0,8,0,0),
 (17469,532,1,0,0,-11082.1,-1852.43,220.668,3.80908,7200,0,0,50000,0,0),
 (17469,532,1,0,0,-11071.5,-1865.54,220.668,3.80908,7200,0,0,50000,0,0),
 (17469,532,1,0,0,-11061,-1878.56,220.668,3.80908,7200,0,0,50000,0,0),
 (17469,532,1,0,0,-11064.3,-1874.3,220.668,3.80908,7200,0,0,50000,0,0),
 (17469,532,1,0,0,-11067.8,-1869.89,220.668,3.80908,7200,0,0,50000,0,0),
 (17469,532,1,0,0,-11057.4,-1883.03,220.668,3.813,7200,0,0,50000,0,0),
 (17469,532,1,0,0,-11078.5,-1856.93,220.668,3.80908,7200,0,0,50000,0,0),
 (17469,532,1,0,0,-11075,-1861.13,220.668,3.80908,7200,0,0,50000,0,0),
 (21160,532,1,0,0,-11108.1,-1872.91,220.668,0.679031,7200,0,0,53000,0,0),
 (21160,532,1,0,0,-11083.7,-1903.94,220.668,0.671177,7200,0,0,53000,0,0),
 (21664,532,1,0,0,-11104.5,-1877.76,220.668,0.678775,7200,0,0,48000,0,0),
 (21664,532,1,0,0,-11087.2,-1899.56,220.668,0.624056,7200,0,0,48000,0,0),
 (21682,532,1,0,0,-11101,-1881.83,220.668,0.598288,7200,0,0,47000,0,0),
 (21682,532,1,0,0,-11090.5,-1895.02,220.668,0.665045,7200,0,0,47000,0,0),
 (21683,532,1,0,0,-11097.6,-1886.43,220.668,0.723955,7200,0,0,58000,0,0),
 (21684,532,1,0,0,-11094.1,-1890.57,220.668,0.62578,7200,0,0,150000,0,0),
 (21726,532,1,0,0,-11053.4,-1879.64,220.668,3.89304,7200,0,0,58000,0,0),
 (21726,532,1,0,0,-11077.5,-1848.75,220.668,3.80665,7200,0,0,58000,0,0),
 (21747,532,1,0,0,-11070.8,-1857.7,220.668,3.81692,7200,0,0,50000,0,0),
 (21747,532,1,0,0,-11060.3,-1870.81,220.668,3.76195,7200,0,0,50000,0,0),
 (21748,532,1,0,0,-11074,-1853.28,220.668,3.84199,7200,0,0,50000,0,0),
 (21748,532,1,0,0,-11056.7,-1875.07,220.668,3.7698,7200,0,0,50000,0,0),
 (21750,532,1,0,0,-11067,-1861.86,220.668,3.72268,7200,0,0,50000,0,0),
 (21752,532,1,0,0,-11063.6,-1866.47,220.668,3.84835,7200,0,0,150000,0,0),
 (22520,532,1,0,0,-11075.8,-1904.83,221.07,2.22342,25,0,0,8,0,0),
 (22520,532,1,0,0,-11076.2,-1908.73,221.07,2.25876,25,0,0,8,0,0),
 (22520,532,1,0,0,-11077.9,-1906.66,221.07,2.25876,25,0,0,8,0,0),
 (22520,532,1,0,0,-11080,-1908.41,221.07,2.2352,25,0,0,8,0,0),
 (22520,532,1,0,0,-11078.3,-1910.44,221.07,2.25876,25,0,0,8,0,0),
 (22520,532,1,0,0,-11074,-1906.97,221.07,2.22342,25,0,0,8,0,0),
 (22520,532,1,0,0,-11071.9,-1905.34,221.07,2.22342,25,0,0,8,0,0),
 (22520,532,1,0,0,-11073.5,-1903.29,221.07,2.22342,25,0,0,8,0,0),
 (22520,532,1,0,0,-11071.2,-1901.6,221.07,2.22342,25,0,0,8,0,0),
 (22520,532,1,0,0,-11069.8,-1903.45,221.07,2.22342,25,0,0,8,0,0),
 (22520,532,1,0,0,-11067.6,-1901.73,221.07,2.22342,25,0,0,8,0,0),
 (22520,532,1,0,0,-11069.2,-1899.6,221.07,2.22342,25,0,0,8,0,0),
 (22520,532,1,0,0,-11067,-1897.91,221.07,2.22342,25,0,0,8,0,0),
 (22520,532,1,0,0,-11065.4,-1900.06,221.07,2.22342,25,0,0,8,0,0),
 (22520,532,1,0,0,-11063.1,-1898.41,221.07,2.24305,25,0,0,8,0,0),
 (22520,532,1,0,0,-11064.7,-1896.41,221.07,2.25091,25,0,0,8,0,0),
 (22520,532,1,0,0,-11081.5,-1844.05,221.07,5.37287,25,0,0,8,0,0),
 (22520,532,1,0,0,-11083,-1842.09,221.07,5.37287,25,0,0,8,0,0),
 (22520,532,1,0,0,-11085.2,-1843.81,221.07,5.37287,25,0,0,8,0,0),
 (22520,532,1,0,0,-11083.8,-1845.66,221.07,5.37287,25,0,0,8,0,0),
 (22520,532,1,0,0,-11085.8,-1847.43,221.07,5.37287,25,0,0,8,0,0),
 (22520,532,1,0,0,-11087.3,-1845.47,221.07,5.37287,25,0,0,8,0,0),
 (22520,532,1,0,0,-11089.5,-1847.19,221.07,5.37287,25,0,0,8,0,0),
 (22520,532,1,0,0,-11087.9,-1849.22,221.07,5.37287,25,0,0,8,0,0),
 (22520,532,1,0,0,-11090.2,-1851.01,221.07,5.37287,25,0,0,8,0,0),
 (22520,532,1,0,0,-11091.9,-1848.88,221.07,5.37287,25,0,0,8,0,0),
 (22520,532,1,0,0,-11094,-1850.52,221.07,5.37287,25,0,0,8,0,0),
 (22520,532,1,0,0,-11092.4,-1852.64,221.07,5.37287,25,0,0,8,0,0),
 (22520,532,1,0,0,-11094.7,-1854.44,221.07,5.37287,25,0,0,8,0,0),
 (22520,532,1,0,0,-11096.2,-1852.54,221.07,5.37287,25,0,0,8,0,0),
 (22520,532,1,0,0,-11098.2,-1854.12,221.07,5.37287,25,0,0,8,0,0),
 (22520,532,1,0,0,-11096.8,-1855.88,221.07,5.37287,25,0,0,8,0,0),
 (25213,532,1,0,0,-11102.6,-1848.85,221.07,2.22643,25,0,0,1,0,0);

UPDATE `creature` SET `spawntimesecs`=300 WHERE `id`=21752;
UPDATE `creature` SET `spawntimesecs`=300 WHERE `id`=21750;
UPDATE `creature` SET `spawntimesecs`=300 WHERE `id`=21747;
UPDATE `creature` SET `spawntimesecs`=300 WHERE `id`=21748;
UPDATE `creature` SET `spawntimesecs`=300 WHERE `id`=21726;
UPDATE `creature` SET `spawntimesecs`=300 WHERE `id`=17469;
UPDATE `creature` SET `spawntimesecs`=300 WHERE `id`=21684;
UPDATE `creature` SET `spawntimesecs`=300 WHERE `id`=21683;
UPDATE `creature` SET `spawntimesecs`=300 WHERE `id`=21682;
UPDATE `creature` SET `spawntimesecs`=300 WHERE `id`=21664;
UPDATE `creature` SET `spawntimesecs`=300 WHERE `id`=21160;
UPDATE `creature` SET `spawntimesecs`=300 WHERE `id`=17211;
UPDATE `creature` SET `spawntimesecs`=604800 WHERE `id`=25213;
UPDATE `creature` SET `spawntimesecs`=28000 WHERE `id`=16816;
UPDATE `creature` SET `spawntimesecs`=10 WHERE `id`=17305;
UPDATE `creature` SET `spawntimesecs`=10 WHERE `id`=17208;
UPDATE `creature` SET `spawntimesecs`=300 WHERE `id`=22520;

UPDATE `creature_template` SET `flags_extra` = 128 WHERE `entry`=25213;
UPDATE `creature_template` SET `flags_extra` = 128 WHERE `entry`=22524;
UPDATE `creature_template` SET `flags_extra` = 128 WHERE `entry`=22521;
