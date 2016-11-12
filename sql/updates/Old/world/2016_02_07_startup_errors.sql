UPDATE creature SET curhealth = 9105 WHERE guid = 74508;
UPDATE creature SET curmana = 2320 WHERE guid = 74508;
UPDATE creature SET curhealth = 9105 WHERE guid = 74507;
UPDATE creature SET curmana = 2320 WHERE guid = 74507;
UPDATE creature SET curhealth = 9105 WHERE guid = 74506;
UPDATE creature SET curmana = 2320 WHERE guid = 74506;
UPDATE creature SET curhealth = 5200 WHERE guid = 87912;
UPDATE creature SET curhealth = 5200 WHERE guid = 87913;
UPDATE creature SET curhealth = 5200 WHERE guid = 87914;
UPDATE creature SET curhealth = 5200 WHERE guid = 87915;
UPDATE creature SET curhealth = 5200 WHERE guid = 87916;
UPDATE creature SET curhealth = 5200 WHERE guid = 87917;
UPDATE creature SET curhealth = 5200 WHERE guid = 87918;
UPDATE creature SET curhealth = 5200 WHERE guid = 87919;
UPDATE creature SET curhealth = 5200 WHERE guid = 87920;
UPDATE creature SET curhealth = 5200 WHERE guid = 87921;
UPDATE creature SET curhealth = 5200 WHERE guid = 87922;
UPDATE creature SET curhealth = 5200 WHERE guid = 87923;
UPDATE creature SET curhealth = 5200 WHERE guid = 87924;
UPDATE creature SET curhealth = 5200 WHERE guid = 87925;
UPDATE creature SET curhealth = 5200 WHERE guid = 87926;
UPDATE creature SET curhealth = 5200 WHERE guid = 87927;

DELETE FROM conditions WHERE SourceTypeOrReferenceId = 13 AND ConditionTypeOrReference = 17 AND SourceEntry IN (33633, 46589, 37434);

DELETE FROM creature_ai_scripts WHERE creature_id = 17209;

UPDATE creature_template SET faction = 35 WHERE entry IN (72, 541, 582, 1293, 2224, 3574, 3837, 14870, 19917, 23544, 25194, 16128);
UPDATE creature_template SET faction = 14 WHERE entry IN (16143, 17465, 20583, 20735);
UPDATE creature_template SET faction = 56 WHERE entry = 1000;
UPDATE creature_template SET faction = 7 WHERE entry = 3815;
UPDATE creature_template SET faction = 37 WHERE entry = 808;
UPDATE creature_template SET faction = 189 WHERE entry IN (4018, 4019);
UPDATE creature_template SET faction = 22 WHERE entry = 4263;