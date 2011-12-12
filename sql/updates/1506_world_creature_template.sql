UPDATE `creature_template` SET `minhealth` = '4274' WHERE `entry` = '18209';
UPDATE `creature_template` SET `maxhealth` = '4274' WHERE `entry` = '18209';
UPDATE `creature_template` SET `ScriptName` = 'npc_kurenai_captive' WHERE `entry` = '18209';

INSERT INTO `script_texts` (`entry`,`content_default`,`content_loc1`,`content_loc2`,`content_loc3`,`content_loc4`,`content_loc5`,`content_loc6`,`content_loc7`,`content_loc8`,`sound`,`type`,`language`,`emote`,`comment`) VALUES
(-1900132,'It is best that we split up now, in case they send more after us. Hopefully one of us will make it back to Arechron. Farewell stranger.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_KUR_COMPLETE');

DELETE FROM `script_waypoint` WHERE `entry`=18209;
INSERT INTO `script_waypoint` VALUES
   (18209,0,-1515.121216,8463.864258,-4.101,0, ''),
   (18209,1,-1526.754150,8461.015625,-4.102,0, ''),
   (18209,2,-1530.113770,8456.399414,-4.102,0, ''),
   (18209,3,-1510.905396,8436.612305,-3.360,0, ''),
   (18209,4,-1502.916504,8435.619141,-1.120,0, ''),
   (18209,5,-1495.801270,8440.210938,0.396,0, ''),
   (18209,6,-1478.269409,8471.061523,0.737,0, ''),
   (18209,7,-1464.239014,8489.628906,3.606,2000, 'SAY_KUR_MORE'),
   (18209,8,-1456.312500,8493.965820,4.641,0, ''),
   (18209,9,-1446.376465,8497.753906,5.918,0, ''),
   (18209,10,-1442.828735,8499.535156,6.312,0, ''),
   (18209,11,-1436.682861,8504.025391,6.943,0, ''),
   (18209,12,-1429.496338,8508.789063,7.492,0, ''),
   (18209,13,-1426.472168,8510.116211,7.686,0, ''),
   (18209,14,-1403.685303,8524.146484,9.680,0, ''),
   (18209,15,-1384.890503,8542.014648,11.180,0, ''),
   (18209,16,-1388.047241,8554.712891,11.638,7500, 'SAY_KUR_COMPLETE'),
   (18209,17,-1392.759033,8571.518555,12.820,0, ''),
   (18209,18,-1411.280029,8595.495117,14.896,0, ''),
   (18209,19,-1436.143677,8629.674805,17.162,0, '');