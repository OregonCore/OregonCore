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