DELETE FROM `gameobject` WHERE `guid` IN (33567,30305,33565,30297,30243);
DELETE FROM script_waypoint WHERE entry = 17876;
DELETE FROM script_waypoint WHERE entry = 18887;
INSERT INTO script_waypoint VALUES
(18887, 0, 2650.06, 665.473, 61.9305, 0, ''), 
(18887, 1, 2652.44, 670.761, 61.9370, 0, ''),   
(18887, 2, 2655.96, 676.913, 57.1725, 0, ''),   
(18887, 3, 2659.40, 677.317, 57.1725, 0, ''),   
(18887, 4, 2651.75, 664.482, 57.1725, 0, ''),  
(18887, 5, 2647.49, 666.595, 57.0824, 0, ''),  
(18887, 6, 2644.37, 668.167, 55.4182, 0, ''),  
(18887, 7, 2640.96, 669.890, 54.7567,120000, '');
UPDATE `creature_template` SET `ScriptName` = 'erozion_image' WHERE `entry` = '19438';
DELETE FROM `creature` WHERE `id` IN (18764,17876);
UPDATE `gameobject_template` SET `flags` = '4' WHERE `entry` = 184393;
UPDATE `gameobject_template` SET `data2` = '6000' WHERE `entry` = 184393;
DELETE FROM `spell_script_target` WHERE `entry` IN (33336,33337);
INSERT INTO `spell_script_target` VALUES
(33336,1,17876),
(33336,1,18887),
(33337,1,17876),
(33337,1,18887);
UPDATE `script_texts` SET `type` = '0' WHERE entry IN (-1560023,-1560042,-1560043,-1560044,-1560038,-1560039,-1560040,-1560041,-1560000,-1560027,-1560030,-1560031,-1560032,-1560033);
DELETE FROM `script_texts` WHERE `entry` IN (-1560050,-1560051,-1560052,-1560053,-1560054,-1560055,-1560056,-1560057,-1560058,-1560059,-1560060,-1560061,-1560062,-1560063,-1560064,-1560065,-1560066,-1560067,-1560068,-1560069,-1560070,-1560071,-1560072,-1560073,-1560074,-1560075,-1560076);
INSERT INTO script_texts VALUES
(-1560050,'That\'s enough out of him.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'thrall hillsbrad SAY_TH_KILL_ARMORER'),
(-1560051,'That spell should wipe their memories of us and what just happened. All they should remember now is what reality would be like without the attempted temporal interference. Well done. Thrall will journey on to find his destiny, and Taretha...',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'erozion SAY_WIPE_MEMORY'),
(-1560052,'Her fate is regrettably unavoidable.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'erozion SAY_ABOUT_TARETHA'),
(-1560053,'They call you a monster. But they\'re the monsters, not you. Farewell Thrall.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'taretha SAY_TA_FAREWELL'),
(-1560054,'I\'m glad you\'re safe, Taretha. None of this would have been possible without your friends. They made all of this happen.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'thrall hillsbrad SAY_TR_GLAD_SAFE'),
(-1560055,'Thrall, I\'ve never met these people before in my life.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'taretha SAY_TA_NEVER_MET'),
(-1560056,'Then who are these people?',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'thrall hillsbrad SAY_TR_THEN_WHO'),
(-1560057,'I believe I can explain everything to you two if you give me a moment of your time.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'erozion SAY_PRE_WIPE'),
(-1560058,'You have done a great thing. Alas, the young warchief\'s memory of these events must be as they originally were ... Andormu awaits you in the master\'s lair.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'erozion SAY_AFTER_WIPE'),
(-1560059,'What\'s the meaning of this? GUARDS!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'armorer SAY_CALL_GUARDS'),
(-1560060,'Let\'s go.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'thrall hillsbrad SAY_TH_ARMORY2'),
(-1560061,' startles the horse with a fierce yell!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,2,0,0,'thrall hillsbrad EMOTE_TH_STARTLE_HORSE'),
(-1560062,' tries to calm the horse down.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,2,0,0,'thrall hillsbrad EMOTE_TH_CALM_HORSE'),
(-1560063,'Taretha isn\'t here. Let\'s head into town.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'thrall hillsbrad SAY_TH_HEAD_TOWN'),
(-1560064,'She\'s not here.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'thrall hillsbrad SAY_TH_CHURCH_ENTER'),
(-1560065,'I thought I saw something go into the barn.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'tarren mill lookout SAY_LOOKOUT_BARN_1'),
(-1560066,'Something riled that horse. Let\'s go!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'tarren mill lookout SAY_PROTECTOR_BARN_2'),
(-1560067,'Thrall\'s trapped himself in the chapel. He can\'t escape now.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'tarren mill lookout SAY_LOOKOUT_CHURCH'),
(-1560068,'He\'s here, stop him!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'tarren mill lookout SAY_LOOKOUT_INN'),
(-1560069,'We have all the time in the world....',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'infinite dragon SAY_INFINITE_AGGRO_2'),
(-1560070,'All that you know... will be undone.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'infinite dragon SAY_INFINITE_AGGRO_3'),
(-1560071,'Do not think you can win!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'infinite dragon SAY_INFINITE_AGGRO_4'),
(-1560072,'Thank you, strangers. You have given me hope.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'infinite dragon SAY_THRALL_END_P1'),
(-1560073,'Surrender immediately!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'infinite dragon SAY_AMBUSH_P1'),
(-1560074,'Stop them!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'infinite dragon SAY_AMBUSH2_P1'),
(-1560075,'My magical power can turn back time to before Thrall\'s death, but be careful. My power to manipulate time is limited.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_IMAGE_1'),
(-1560076,'I have set back the flow of time just once more. If you fail to prevent Thrall\'s death, then all is lost.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'infinite dragon SAY_AMBUSH2_P1');
UPDATE `creature_template` SET `faction_A` = '1748' WHERE `entry` IN (18092,18094,18093,18170,18171,18172,20545,20547,20546,20534,20533);
UPDATE `creature_template` SET `faction_H` = '1748' WHERE `entry` IN (18092,18094,18093,18170,18171,18172,20545,20547,20546,20534,20533);
UPDATE `creature` SET `id` = '18094' WHERE `id` = 23178;
UPDATE `creature` SET `id` = '18094' WHERE `id` = 23177;
UPDATE `creature` SET `id` = '18092' WHERE `id` = 23176;
UPDATE `creature` SET `id` = '18092' WHERE `id` = 23175;
UPDATE `creature` SET `id` = '18093' WHERE `id` = 23179;
DELETE FROM `creature` WHERE `id` IN (18094,18092,18093);
DELETE FROM `creature` WHERE `guid` = 84001;
UPDATE `creature_template` SET `rank` = '1' WHERE entry = 20525;
UPDATE `creature_template` SET `faction_A` = '1748' WHERE `entry` = 20525;
UPDATE `creature_template` SET `faction_H` = '1748' WHERE `entry` = 20525;
UPDATE `creature_template` SET `minhealth` = '27123' WHERE entry = 20525;
UPDATE `creature_template` SET `maxhealth` = '28114' WHERE entry = 20525;
UPDATE `creature_template` SET `minmana` = '16545' WHERE `entry` = 20525;
UPDATE `creature_template` SET `maxmana` = '16545' WHERE `entry` = 20525;
UPDATE `creature_template` SET `minlevel` = '70' WHERE entry = 20525;
UPDATE `creature_template` SET `maxlevel` = '71' WHERE entry = 20525;
UPDATE `creature_template` SET `armor` = '5076' WHERE entry = 20525;
UPDATE `creature_template` SET `mindmg` = '764' WHERE entry = 20525;
UPDATE `creature_template` SET `maxdmg` = '2133' WHERE entry = 20525;
UPDATE `creature_template` SET `attackpower` = '30414' WHERE entry = 20525;
UPDATE `creature_template` SET `baseattacktime` = '0' WHERE entry = 20525;
UPDATE `creature_template` SET `rank` = '1' WHERE entry IN (23175,23179,23177,23176,23180,23178);
UPDATE `creature_template` SET `faction_A` = '1748' WHERE `entry` IN (23175,23179,23177,23176,23180,23178);
UPDATE `creature_template` SET `faction_H` = '1748' WHERE `entry` IN (23175,23179,23177,23176,23180,23178);
UPDATE `creature_template` SET `minhealth` = '13944' WHERE entry IN (23175,23179,23177,23176,23180,23178);
UPDATE `creature_template` SET `maxhealth` = '15186' WHERE entry IN (23175,23179,23177,23176,23180,23178);
UPDATE `creature_template` SET `minmana` = '15399' WHERE `entry` IN (23179,23177,23180,23178);
UPDATE `creature_template` SET `maxmana` = '16545' WHERE `entry` IN (23179,23177,23180,23178);
UPDATE `creature_template` SET `minlevel` = '70' WHERE entry IN (23175,23179,23177,23176,23180,23178);
UPDATE `creature_template` SET `maxlevel` = '71' WHERE entry IN (23175,23179,23177,23176,23180,23178);
UPDATE `creature_template` SET `armor` = '200' WHERE entry IN (23175,23179,23177,23176,23180,23178);
UPDATE `creature_template` SET `mindmg` = '196' WHERE entry IN (23175,23179,23177,23176,23180,23178);
UPDATE `creature_template` SET `maxdmg` = '201' WHERE entry IN (23175,23179,23177,23176,23180,23178);
UPDATE `creature_template` SET `attackpower` = '2085' WHERE entry IN (23175,23179,23177,23176,23180,23178);
UPDATE `creature_template` SET `baseattacktime` = '1400' WHERE entry IN (23175,23179,23177,23176,23180,23178);
UPDATE `creature_template` SET `heroic_entry` = '20545' WHERE entry IN (23175,23176);
UPDATE `creature_template` SET `heroic_entry` = '20547' WHERE entry IN (23179,23180);
UPDATE `creature_template` SET `heroic_entry` = '20546' WHERE entry IN (23177,23178);
UPDATE `creature_questrelation` SET `id` = '20130' WHERE quest IN (10277,10282,10296);
UPDATE `creature_involvedrelation` SET `id` = '20130' WHERE quest IN (10277,10279,10285,10298);
UPDATE `creature` SET `MovementType` = '2' WHERE `guid` IN ('83476','83477');
UPDATE `creature` SET `position_x` = '2643.50' WHERE `guid` = '83477';
UPDATE `creature` SET `position_y` = '695.43' WHERE `guid` = '83477';
UPDATE `creature` SET `position_z` = '55.758' WHERE `guid` = '83477';
UPDATE `creature` SET `position_x` = '2638.79' WHERE `guid` = '83476';
UPDATE `creature` SET `position_y` = '694.75' WHERE `guid` = '83476';
UPDATE `creature` SET `position_z` = '55.758' WHERE `guid` = '83476';
UPDATE `creature_addon` SET `guid` = '83476' WHERE `path_id` = '1724';
UPDATE `creature_addon` SET `guid` = '83477' WHERE `path_id` = '1723';
UPDATE `creature_template` SET `speed` = '1.125' WHERE `entry` = '18666';
UPDATE `creature_template` SET `ScriptName` = 'npc_brazen' WHERE `entry` = '18725';
