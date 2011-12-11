UPDATE `creature_template` SET `ScriptName` = 'npc_maghar_prisoner' WHERE `entry` = '18428';

UPDATE `quest_template` SET `ReqSpellCast1`  = '0' WHERE `entry` = '9948';

UPDATE `gameobject_template` SET `ScriptName`='go_maghar_prison' WHERE `entry` IN (182487,182504,182484,182486,182488,182494,182491,182489,182493,182496,182495,182490,182503,182498,182500,182502,182501,182499,182497,182492);

INSERT INTO `script_texts` (`entry`,`content_default`,`content_loc1`,`content_loc2`,`content_loc3`,`content_loc4`,`content_loc5`,`content_loc6`,`content_loc7`,`content_loc8`,`sound`,`type`,`language`,`emote`,`comment`) VALUES
 (-1900148,'Help! Help! They\'re going to eat me!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'MAG_PRISONER1'),
 (-1900149,'My Children will be made orphans!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'MAG_PRISONER2'),
 (-1900150,'The ogres have the keys! Kill the ogres and get me out of here!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'MAG_PRISONER3'),
 (-1900151,'I\'m done for!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'MAG_PRISONER4'),
 (-1900152,'Thank you, $r! Your service to the Mag\'har will not be forgotten.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'MAG_PRISONER5'),
 (-1900153,'Many Thanks, hero!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'MAG_PRISONER6'),
 (-1900154,'Spirits watch for you, $r.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'MAG_PRISONER7'),
 (-1900155,'Death to the ogres!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'MAG_PRISONER8');