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

UPDATE `quest_template` SET `ReqCreatureOrGOID1`  = '18445' WHERE `entry` = '9923';
UPDATE `gameobject_template` SET `data2` = '1966080' WHERE `entry` = '182349';
UPDATE `gameobject_template` SET `ScriptName` = 'go_corkis_prison1' WHERE `entry` = '182349';
UPDATE `creature_template` SET `ScriptName` = 'npc_corki1' WHERE `entry` = '18445';

INSERT INTO `script_texts` (`entry`,`content_default`,`content_loc1`,`content_loc2`,`content_loc3`,`content_loc4`,`content_loc5`,`content_loc6`,`content_loc7`,`content_loc8`,`sound`,`type`,`language`,`emote`,`comment`) VALUES
 (-1900133,'Thanks $r ! I\'m sure my dad will reward you greatly! Bye!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_KORKI1'),
 (-1900134,'Hey, fatso! How do you go to the bathroom with all that fat?',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_KORKI2'),
 (-1900135,'Stop getting caught Corki!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_KORKI3'),
 (-1900136,'I want my daddy!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_KORKI4'),
 (-1900137,'I\'m not Corki! I\'m not Corki!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_KORKI5'),
 (-1900138,'Man that Corki sure is an idiot!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_KORKI6');

DELETE FROM `script_waypoint` WHERE `entry`=18445;
INSERT INTO `script_waypoint` VALUES
   (18445,0,-2561.155,6284.777,15.138,0, ''),
   (18445,1,-2544.227,6274.250,14.612,0, ''),
   (18445,2,-2528.367,6277.982,17.335,0, ''),
   (18445,3,-2477.215,6341.930,26.882,0, '');

UPDATE `quest_template` SET `ReqCreatureOrGOID1`  = '20812' WHERE `entry` = '9924';
UPDATE `gameobject_template` SET `data2` = '1966080' WHERE `entry` = '182350';
UPDATE `gameobject_template` SET `ScriptName` = 'go_corkis_prison2' WHERE `entry` = '182350';
UPDATE `creature_template` SET `ScriptName` = 'npc_corki2' WHERE `entry` = '20812';

INSERT INTO `script_texts` (`entry`,`content_default`,`content_loc1`,`content_loc2`,`content_loc3`,`content_loc4`,`content_loc5`,`content_loc6`,`content_loc7`,`content_loc8`,`sound`,`type`,`language`,`emote`,`comment`) VALUES
 (-1900139,'This is the last time I get caught! I promise! Bye!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_KORKI7');

DELETE FROM `script_waypoint` WHERE `entry`=20812;
INSERT INTO `script_waypoint` VALUES
   (20812,0,-972.953,8125.154,-94.303,0, ''),
   (20812,1,-981.466,8120.831,-95.112,0, ''),
   (20812,2,-1058.294,8102.698,-89.662,0, '');

UPDATE `quest_template` SET `ReqCreatureOrGOID1`  = '18369' WHERE `entry` = '9955';
UPDATE `gameobject_template` SET `data2` = '1966080' WHERE `entry` = '182521';
UPDATE `gameobject_template` SET `ScriptName` = 'go_corkis_prison3' WHERE `entry` = '182521';
UPDATE `creature_template` SET `ScriptName` = 'npc_corki3' WHERE `entry` = '18369';

DELETE FROM `script_waypoint` WHERE `entry`=18369;
INSERT INTO `script_waypoint` VALUES
   (18369,0,-915.809,8667.150,171.827,0, ''),
   (18369,1,-868.656,8709.678,173.826,0, ''),
   (18369,2,-817.007,8729.769,178.137,0, '');

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

DELETE FROM spell_script_target WHERE entry = '33655';
INSERT INTO spell_script_target VALUES (33655, 1, 19291),(33655, 1, 19292);


UPDATE creature_template SET KillCredit1 = '21161' WHERE entry = '18952';
UPDATE creature_template SET KillCredit1 = '21161' WHERE entry = '16876';
UPDATE creature_template SET KillCredit1 = '21161' WHERE entry = '19701';
UPDATE creature_template SET KillCredit1 = '21161' WHERE entry = '16925';

UPDATE `creature_template` SET `ScriptName` = 'npc_anchorite_relic_bunny' WHERE `entry` = '22444';
UPDATE `creature_template` SET `ScriptName` = 'npc_hand_berserker' WHERE `entry` = '16878';
UPDATE `creature_template` SET `unit_flags` = '0' WHERE `entry` = '22444';
UPDATE `gameobject_template` SET `size` = '3' WHERE `entry` = '185298';
INSERT INTO `spell_script_target` VALUES (39184,1,16878);
UPDATE `item_template` SET `spellcooldown_1` = '60000' WHERE `entry` = '31772';
INSERT INTO `script_texts` (`entry`,`content_default`,`content_loc1`,`content_loc2`,`content_loc3`,`content_loc4`,`content_loc5`,`content_loc6`,`content_loc7`,`content_loc8`,`sound`,`type`,`language`,`emote`,`comment`) VALUES
(-1900130,'Vengeance will not be yours. Livine!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_SP');

INSERT INTO `script_texts` (`entry`,`content_default`,`content_loc1`,`content_loc2`,`content_loc3`,`content_loc4`,`content_loc5`,`content_loc6`,`content_loc7`,`content_loc8`,`sound`,`type`,`language`,`emote`,`comment`) VALUES
 (-1900100,'It\'s time. The rite of exorcism will now commence...',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_BARADA1'),
 (-1900101,'Prepare yourself.Do not allow the ritual to be interrupted or we may lose our patient...',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_BARADA2'),
 (-1900104,'In the name of the Light! It is Light that commands you! It is Light that flung you to be depths of darkness!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_BARADA3'),
 (-1900105,'You cannot deceive me, demon! Your strenght wanes just as my faith bolsters!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_BARADA4'),
 (-1900106,'I... must not...falter!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_BARADA5'),
 (-1900107,'The power of Light compels you! Back to your pit!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_BARADA6'),
 (-1900108,'In the name of Light! It is Light that commands you! It is Light that flung you to the depths of darkness!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_BARADA7'),
 (-1900109,'Back! I cast you back... corrupter of faith! Author of pain! Do not return, or suffer the same fate as you did here today!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_BARADA8'),
 (-1900110,'Keep away.The fool is mine.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_COLONEL1'),
 (-1900111,'You will not succeed mortal! This shell will lie decrepit, blistered and bleeding before I am donewith It. And its spirit will be long cast into darkness.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_COLONEL2'),
 (-1900112,'All is lost, Anchorite! Abandon what hope remains.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_COLONEL3'),
 (-1900113,'This is fruitless, draenel! You and your little helper cannot wrest control of this pathetic human. He is mine!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_COLONEL4'),
 (-1900114,'I see your ancestors, Anchorite! They writhe and scream in the darkness... they are with us!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_COLONEL5'),
 (-1900115,'All is lost, Anchorite! Abandon what hope remains.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_COLONEL6'),
 (-1900116,'I will tear your soul into morsels and slow roast them over demon fire!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_COLONEL7'),
 (-1900117,'I see your ancestors, Anchorite! They writhe and scream in the darkness... they are with us!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_COLONEL8');

UPDATE `creature_template` SET `unit_flags` = '0' WHERE entry = '22507';
UPDATE `creature_template` SET `minhealth` = '916' WHERE entry = '22507';
UPDATE `creature_template` SET `maxhealth` = '916' WHERE entry = '22507';
UPDATE `creature_template` SET `mindmg` = '176' WHERE entry = '22506';
UPDATE `creature_template` SET `maxdmg` = '176' WHERE entry = '22506';
UPDATE `creature_template` SET `attackpower` = '1235' WHERE entry = '22506';
UPDATE `creature_template` SET `faction_A` = '14' WHERE entry = '22506';
UPDATE `creature_template` SET `faction_H` = '14' WHERE entry = '22506';
UPDATE `creature_template` SET `flags_extra` = '128' WHERE entry = '22505';
UPDATE `creature_template` SET `AIName` = 'EventAI' WHERE entry = '22505';
UPDATE `creature` SET `modelid`  = '14533' WHERE guid = '78786';
UPDATE `quest_template` SET `ReqSpellCast1`  = '0' WHERE entry = '10935';
UPDATE `quest_template` SET `ReqCreatureOrGOID1`  = '0' WHERE entry = '10935';
UPDATE `quest_template` SET `ReqCreatureOrGOCount1`  = '0' WHERE entry = '10935';
UPDATE `quest_template` SET `QuestFlags` = '128' WHERE entry = '10935';
UPDATE `quest_template` SET `SpecialFlags`  = '2' WHERE entry = '10935';
UPDATE `quest_template` SET `ObjectiveText1`  = '' WHERE entry = '10935';
UPDATE `quest_template` SET `EndText`  = 'Colonel Jules Saved' WHERE entry = '10935';
UPDATE `creature_template` SET `ScriptName` = 'npc_anchorite_barada' WHERE entry = '22431';
UPDATE `creature_template` SET `ScriptName` = 'npc_darkness_released' WHERE entry = '22507';
UPDATE `creature_template` SET `ScriptName` = 'npc_foul_purge' WHERE entry = '22506';

INSERT INTO creature_ai_scripts VALUES ('2250501', '22505', '1', '0', '100', '0', '1000', '1000', '0', '0', '11', '39300', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'Slimer Bunny - Cast Aura on Spawn');
INSERT INTO creature_ai_scripts VALUES ('2250502', '22505', '1', '0', '100', '0', '1000', '1000', '0', '0', '12', '22506', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'Slimer Bunny - Summon Foul Purge on Spawn');

INSERT INTO `script_texts` (`entry`,`content_default`,`content_loc1`,`content_loc2`,`content_loc3`,`content_loc4`,`content_loc5`,`content_loc6`,`content_loc7`,`content_loc8`,`sound`,`type`,`language`,`emote`,`comment`) VALUES
 (-1900125,'Do not return, draenei scum. Next time we won\'t spare your life, unarmed or not!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_MAG_ESSCORT'),
 (-1900126,'I\'ve failed... peace is impossible.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_SEDAI1'),
 (-1900127,'The cycle of bloodshed is unending. Is there nothing I can do?',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_SEDAI2'),
 (-1900128,'You can die!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_KRUN');

INSERT INTO `creature_template_addon` VALUES ('16852', '0', '0', '0', '7', '0', '0', '0', '0', null);

UPDATE `item_template` SET `spellcharges_1` = '1' WHERE entry = '23645';
UPDATE `quest_template` SET `SpecialFlags`  = '0' WHERE entry = '9545';
UPDATE `creature_template` SET `equipment_id` = '1411' WHERE entry = '17417';
UPDATE `creature_template` SET `flags_extra` = '128' WHERE entry = '17413';
UPDATE `creature_template_addon` SET `bytes1` = '7' WHERE entry = '17062';
UPDATE `creature_template` SET `dynamicflags` = '32' WHERE entry = '16852';
UPDATE `creature_template` SET `dynamicflags` = '32' WHERE entry = '17062';
UPDATE `creature_template` SET `ScriptName` = 'npc_sedai_quest_credit_marker' WHERE entry = '17413';
UPDATE `creature_template` SET `ScriptName` = 'npc_vindicator_sedai' WHERE entry = '17404';

UPDATE creature_template SET KillCredit1 = '22334' WHERE entry = '19312';
UPDATE creature_template SET KillCredit1 = '22334' WHERE entry = '16870';
UPDATE creature_template SET KillCredit1 = '22334' WHERE entry = '19414';
UPDATE creature_template SET KillCredit1 = '22334' WHERE entry = '19411';
UPDATE creature_template SET KillCredit1 = '22334' WHERE entry = '19415';
UPDATE creature_template SET KillCredit1 = '22334' WHERE entry = '19295';
UPDATE creature_template SET KillCredit1 = '22334' WHERE entry = '19413';
UPDATE creature_template SET KillCredit1 = '22334' WHERE entry = '16878';
UPDATE creature_template SET KillCredit1 = '22334' WHERE entry = '16867';
UPDATE creature_template SET KillCredit1 = '22334' WHERE entry = '19410';

UPDATE `creature_template` SET `npcflag` = '1' WHERE entry = '22258';
UPDATE `creature_template` SET `minhealth` = '9958' WHERE entry = '22258';
UPDATE `creature_template` SET `maxhealth` = '9958' WHERE entry = '22258';
UPDATE `creature_template` SET `minhealth` = '2987' WHERE entry = '22259';
UPDATE `creature_template` SET `maxhealth` = '3095' WHERE entry = '22259';
UPDATE `creature_template` SET `AIName` = 'EventAI' WHERE entry = '22273';
UPDATE `creature_template` SET `flags_extra` = '128' WHERE entry = '22267';
UPDATE `creature_template` SET `flags_extra` = '128' WHERE entry = '22260';
UPDATE `creature_template` SET `ScriptName` = 'npc_demoniac_scryer' WHERE entry = '22258';
UPDATE `creature_template` SET `ScriptName` = 'npc_magic_sucker_device_spawner' WHERE entry = '22260';

INSERT INTO `spell_script_target` VALUES ('38691','1','22267');
INSERT INTO `spell_script_target` VALUES ('38691','1','22260');
INSERT INTO `creature_ai_scripts` VALUES ('2227301', '22273', '1', '0', '100', '0', '0', '0', '0', '0', '21', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'Fel Warden - Prevent Combat Movement on Spawn');
INSERT INTO `creature_ai_scripts` VALUES ('2227302', '22273', '4', '0', '100', '0', '0', '0', '0', '0', '11', '14034', '1', '0', '23', '1', '0', '0', '0', '0', '0', '0', 'Fel Warden - Cast Fireball and Set Phase 1 on Aggro');
INSERT INTO `creature_ai_scripts` VALUES ('2227303', '22273', '9', '13', '100', '1', '0', '40', '2400', '3800', '11', '14034', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'Fel Warden - Cast Fireball (Phase 1)');
INSERT INTO `creature_ai_scripts` VALUES ('2227304', '22273', '3', '13', '100', '0', '15', '0', '0', '0', '21', '1', '0', '0', '23', '1', '0', '0', '0', '0', '0', '0', 'Fel Warden - Start Combat Movement and Set Phase 2 when Mana is at 15% (Phase 1)');
INSERT INTO `creature_ai_scripts` VALUES ('2227305', '22273', '9', '13', '100', '1', '35', '80', '0', '0', '21', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'Fel Warden - Start Combat Movement at 35 Yards (Phase 1)');
INSERT INTO `creature_ai_scripts` VALUES ('2227306', '22273', '9', '13', '100', '1', '5', '15', '0', '0', '21', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'Fel Warden - Prevent Combat Movement at 15 Yards (Phase 1)');
INSERT INTO `creature_ai_scripts` VALUES ('2227307', '22273', '9', '13', '100', '1', '0', '5', '0', '0', '21', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'Fel Warden - Start Combat Movement Below 5 Yards');
INSERT INTO `creature_ai_scripts` VALUES ('2227308', '22273', '3', '11', '100', '1', '100', '30', '100', '100', '23', '-1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'Fel Warden - Set Phase 1 when Mana is above 30% (Phase 2)');
INSERT INTO `creature_ai_scripts` VALUES ('2227309', '22273', '0', '0', '100', '1', '6000', '9000', '12000', '16000', '11', '11831', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'Fel Warden - Cast Frost Nova');
INSERT INTO `creature_ai_scripts` VALUES ('2227310', '22273', '2', '0', '100', '0', '15', '0', '0', '0', '22', '3', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'Fel Warden - Set Phase 3 at 15% HP');
INSERT INTO `creature_ai_scripts` VALUES ('2227311', '22273', '2', '7', '100', '0', '15', '0', '0', '0', '21', '1', '0', '0', '25', '0', '0', '0', '1', '-47', '0', '0', 'Fel Warden - Start Combat Movement and Flee at 15% HP (Phase 3)');
INSERT INTO `creature_ai_scripts` VALUES ('2227312', '22273', '7', '0', '100', '0', '0', '0', '0', '0', '22', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'Fel Warden - Set Phase to 0 on Evade');

DELETE FROM `creature` WHERE `guid` = '84645';
UPDATE `gameobject` SET `position_x` = '-18.8763' WHERE `guid` = '99909';
UPDATE `gameobject` SET `position_y` = '3804.13' WHERE `guid` = '99909';
UPDATE `gameobject` SET `position_z` = '93.5195' WHERE `guid` = '99909';
UPDATE `gameobject` SET `orientation` = '2.62837' WHERE `guid` = '99909';
UPDATE `gameobject` SET `rotation2` = '0.967255' WHERE `guid` = '99909';
UPDATE `gameobject` SET `rotation3` = '0.253806' WHERE `guid` = '99909';