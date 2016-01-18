-- Creature Gossip_menu_id Update from sniff
UPDATE `creature_template` SET `gossip_menu_id`=9052 WHERE `entry`=24965; -- Vindicator Xayann 
UPDATE `creature_template` SET `gossip_menu_id`=9050 WHERE `entry`=24975; -- Mar'nah 
UPDATE `creature_template` SET `gossip_menu_id`=9126 WHERE `entry`=25032; -- Eldara Dawnrunner 
UPDATE `creature_template` SET `gossip_menu_id`=9087 WHERE `entry`=25046; -- Smith Hauthaa 
UPDATE `creature_template` SET `gossip_menu_id`=9064 WHERE `entry`=25057; -- Battlemage Arynna 
UPDATE `creature_template` SET `gossip_menu_id`=9062, `AIName`='SmartAI', `ScriptName`='' WHERE `entry`=25059; -- Ayren Cloudbreaker 
UPDATE `creature_template` SET `gossip_menu_id`=9063 WHERE `entry`=25061; -- Harbinger Inuuro 
UPDATE `creature_template` SET `gossip_menu_id`=9127 WHERE `entry`=25069; -- Magister Ilastar 
UPDATE `creature_template` SET `gossip_menu_id`=9115 WHERE `entry`=25112; -- Anchorite Ayuri 
UPDATE `creature_template` SET `gossip_menu_id`=9105 WHERE `entry`=25169; -- Archmage Ne'thul
UPDATE `creature_template` SET `gossip_menu_id`=9286 WHERE `entry`=25632; -- Vindicator Moorba
UPDATE `creature_template` SET `gossip_menu_id`=9285 WHERE `entry`=25638; -- Captain Selana
UPDATE `creature_template` SET `gossip_menu_id`=9198 WHERE `entry`=25950; -- Shaani

-- Gossip Menu insert from sniff
DELETE FROM `gossip_menu` WHERE `entry`=9050 AND `text_id`=12237;
DELETE FROM `gossip_menu` WHERE `entry`=9052 AND `text_id`=12241;
DELETE FROM `gossip_menu` WHERE `entry`=9062 AND `text_id`=12252;
DELETE FROM `gossip_menu` WHERE `entry`=9063 AND `text_id`=12256;
DELETE FROM `gossip_menu` WHERE `entry`=9064 AND `text_id`=12258;
DELETE FROM `gossip_menu` WHERE `entry`=9087 AND `text_id`=12286;
DELETE FROM `gossip_menu` WHERE `entry`=9115 AND `text_id`=12323;
DELETE FROM `gossip_menu` WHERE `entry`=9126 AND `text_id`=12338;
DELETE FROM `gossip_menu` WHERE `entry`=9127 AND `text_id`=12340;
DELETE FROM `gossip_menu` WHERE `entry`=9198 AND `text_id`=12497;
DELETE FROM `gossip_menu` WHERE `entry`=9285 AND `text_id`=12596;
DELETE FROM `gossip_menu` WHERE `entry`=9286 AND `text_id`=12597;
DELETE FROM `gossip_menu` WHERE `entry`=9287 AND `text_id`=12598;
DELETE FROM `gossip_menu` WHERE `entry`=9288 AND `text_id`=12599;
DELETE FROM `gossip_menu` WHERE `entry`=9289 AND `text_id`=12600;
DELETE FROM `gossip_menu` WHERE `entry`=9290 AND `text_id`=12601;
DELETE FROM `gossip_menu` WHERE `entry`=9293 AND `text_id`=12604;
INSERT INTO `gossip_menu` (`entry`,`text_id`) VALUES
(9050,12237),
(9052,12241),
(9062,12252),
(9063,12256),
(9064,12258),
(9087,12286),
(9115,12323),
(9126,12338),
(9127,12340),
(9198,12497),
(9285,12596),
(9286,12597),
(9287,12598),
(9288,12599),
(9289,12600),
(9290,12601),
(9293,12604);

-- Creature Gossip_menu_option insert from sniff
DELETE FROM `gossip_menu_option` WHERE `menu_id` IN (9050,9087,9126,9198,9285,9287,9288,9289) AND `id`=0;
DELETE FROM `gossip_menu_option` WHERE `menu_id`=9062 AND `id` IN (0,1);
DELETE FROM `gossip_menu_option` WHERE `menu_id`=9286 AND `id`=2;
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`,`option_text`,`option_id`,`npc_option_npcflag`,`action_menu_id`,`action_poi_id`,`box_coded`,`box_money`,`box_text`)VALUES
(9050,0,1,'Let me browse your goods.',3,128,0,0,0,0,''),
(9062,0,0,'Speaking of action, I''ve been ordered to undertake an air strike.',1,1,0,0,0,0,''), 
(9062,1,0,'I need to intercept the Dawnblade reinforcements.',1,1,0,0,0,0,''),
(9087,0,1,'Let me browse your goods.',3,128,0,0,0,0,''),
(9126,0,1,'Let me browse your goods.',3,128,0,0,0,0,''),
(9198,0,1,'Let me browse your goods.',3,128,0,0,0,0,''),
(9285,0,0,'Give me a situation report, Captain.',1,1,9287,0,0,0,''),
(9286,2,0,'What is the current state of the Sunwell''s Gates?',1,1,9293,0,0,0,''), 
(9287,0,0,'What went wrong?',1,1,9288,0,0,0,''),
(9288,0,0,'Why did they stop?',1,1,9289,0,0,0,''),
(9289,0,0,'Your insight is appreciated.',1,1,9290,0,0,0,'');

-- Conditions
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=15 AND `SourceGroup`=9062 AND `SourceEntry` IN (0,1);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`,`SourceGroup`,`SourceEntry`,`SourceId`,`ElseGroup`,`ConditionTypeOrReference`,`ConditionValue1`,`ConditionValue2`,`ConditionValue3`,`NegativeCondition`,`ErrorTextId`,`ScriptName`,`Comment`) VALUES
(15,9062,0,0,0,9,11532,0,0,0,0,'','Show gossip option only if player has taken quest 11532'),
(15,9062,0,0,1,9,11533,0,0,0,0,'','Show gossip option only if player has taken quest 11533'),
(15,9062,1,0,0,9,11542,0,0,0,0,'','Show gossip option only if player has taken quest 11542'),
(15,9062,1,0,1,9,11543,0,0,0,0,'','Show gossip option only if player has taken quest 11543');

-- SmartAI
DELETE FROM `smart_scripts` WHERE `entryorguid`=25059 AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(25059,0,0,2,62,0,100,0,9062,0,0,0,11,45071,0,0,0,0,0,7,0,0,0,0,0,0,0,'Ayren Cloudbreaker - On Gossip option select - Cast "Quest - Sunwell Daily - Dead Scar Bombing Run" on player'),
(25059,0,1,2,62,0,100,0,9062,1,0,0,11,45113,0,0,0,0,0,7,0,0,0,0,0,0,0,'Ayren Cloudbreaker - On Gossip option select - Cast "Quest - Sunwell Daily - Ship Bombing Run" on player'),
(25059,0,2,0,61,0,100,0,0,0,0,0,72,0,0,0,0,0,0,7,0,0,0,0,0,0,0,'Ayren Cloudbreaker - On Gossip option select - Close Gossip');

-- Spell scripts from sniff
DELETE FROM `spell_scripts` WHERE `id`=45071;
INSERT INTO `spell_scripts` (`id`,`delay`,`command`,`datalong`,`datalong2`,`dataint`,`x`,`y`,`z`,`o`) VALUES
(45071,0,16,12318,1,0,0,0,0,0); -- Play sound

-- Unrestrained Dragonhawk
-- SAI for Unrestrained Dragonhawk
UPDATE `creature_template` SET `AIName`='SmartAI',`ScriptName`='' WHERE `entry`=25236;
DELETE FROM `smart_scripts` WHERE (`entryorguid`=25236 AND `source_type`=0);
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(25236,0,0,1,62,0,100,0,9143,0,0,0,11,45353,0,0,0,0,0,7,0,0,0,0,0,0,0,'Unrestrained Dragonhawk - On Gossip option select - cast "Quest - Sunwell Daily - Ship Bombing Run Return" on player'),
(25236,0,1,0,61,0,100,0,0,0,0,0,72,0,0,0,0,0,0,7,0,0,0,0,0,0,0,'Unrestrained Dragonhawk - On Gossip option select - Close Gossip');
-- Unrestrained Dragonhawk Gossip
UPDATE `creature_template` SET `gossip_menu_id`=9143 WHERE `entry`=25236;
-- Gossip_menu from UDB
DELETE FROM `gossip_menu` WHERE `entry`=9143;
INSERT INTO `gossip_menu` (`entry`,`text_id`) VALUES
(9143,12371);
-- Gossip_menu_option Update from UDB
DELETE FROM `gossip_menu_option` WHERE `menu_id`=9143;
INSERT INTO `gossip_menu_option` (`menu_id`,`id`,`option_icon`,`option_text`,`option_id`,`npc_option_npcflag`,`action_menu_id`,`action_poi_id`,`box_coded`,`box_money`,`box_text`) VALUES
(9143,0,0, '<Ride the dragonhawk to Sun''s Reach.>',1,1,0,0,0,0, '');
-- Gossip option Conditions
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=15 AND `SourceGroup`=9143;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`,`SourceGroup`,`SourceEntry`,`ElseGroup`,`ConditionTypeOrReference`,`ConditionValue1`,`ConditionValue2`,`ConditionValue3`,`ErrorTextId`,`ScriptName`,`Comment`) VALUES
(15,9143,0,0,9,11542,0,0,0,'','Show gossip option if player has quest 11542 but not complete'),
(15,9143,0,1,9,11543,0,0,0,'','Show gossip option if player has quest 11543 but not complete');

-- Wing Commander Dabir'ee
-- Add sniffed gossip texts to menu
DELETE FROM `gossip_menu_option` WHERE `menu_id` = 7970;
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `action_menu_id`, `action_poi_id`, `box_coded`, `box_money`, `box_text`) VALUES
(7970, 0, 0, 'I''m on a bombing mission for Forward Commander Kingston.  I need a gryphon destroyer!',1,1,0,0,0,0,''),
(7970, 1, 0, 'Send me to Shatter Point!',1,1,0,0,0,0,'');

-- Migrate NPC to use SmartAI
UPDATE `creature_template` SET `gossip_menu_id` = 7970, `AIName` = 'SmartAI', `ScriptName` = '', `gossip_menu_id` = 7970 WHERE `entry` = 19409;

-- Condition: Gossip menu ID 0 needs either quest ID 10146 to be incomplete.
-- Condition: Gossip menu ID 1 needs quest ID 10340 to not be rewarded
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`= 15 AND `SourceGroup` = 7970 AND `SourceEntry`IN (0,1);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`,`SourceGroup`,`SourceEntry`,`SourceId`,`ElseGroup`,`ConditionTypeOrReference`,`ConditionTarget`,`ConditionValue1`,`ConditionValue2`,`ConditionValue3`,`NegativeCondition`,`ErrorTextId`,`ScriptName`,`Comment`) VALUES
(15,7970,0,0,0,9,0,10146,0,0,0,0,'','Wing Commander Dabir''ee - Show gossip option 0 if player has taken quest ID 10146'),
(15,7970,1,0,0,8,0,10340,0,0,1,0,'','Wing Commander Dabir''ee - Show gossip option 1 if quest ID 10340 is not rewarded');

-- Create SmartAI for Wing Commander Dabir'ee
DELETE FROM `smart_scripts` WHERE (source_type = 0 AND entryorguid = 19409);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(19409,0,0,2,62,0,100,0,7970,0,0,0,11,33768,0,0,0,0,0,7,0,0,0,0,0,0,0,'Wing Commander Dabir''ee - On Gossip Option 0 Selected - Cast Spell 33768'),
(19409,0,1,2,62,0,100,0,7970,1,0,0,11,35069,0,0,0,0,0,7,0,0,0,0,0,0,0,'Wing Commander Dabir''ee - On Gossip Option 1 Selected - Cast Spell 35069'),
(19409,0,2,0,61,0,100,0,0,0,0,0,72,0,0,0,0,0,0,7,0,0,0,0,0,0,0,'Wing Commander Dabir''ee - On Linked Actions - Close Gossip');


-- Skyguard Handler Deesak
-- Add gossip texts to menu
DELETE FROM `gossip_menu_option` WHERE `menu_id` = 8719;
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `action_menu_id`, `action_poi_id`, `box_coded`, `box_money`, `box_text`) VALUES
(8719,0,0,'Absolutely!  Send me to the Skyguard Outpost.',1,1,0,0,0,0,'');

-- Migrate NPC to use SmartAI
UPDATE `creature_template` SET `gossip_menu_id` = 8719, `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = 23415;

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` IN (14, 15) AND `SourceGroup` = 8719;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`,`SourceGroup`,`SourceEntry`,`SourceId`,`ElseGroup`,`ConditionTypeOrReference`,`ConditionTarget`,`ConditionValue1`,`ConditionValue2`,`ConditionValue3`,`NegativeCondition`,`ErrorTextId`,`ScriptName`,`Comment`) VALUES
(14,8719,10980,0,0,5,0,1031,224,0,1,0,'','Skyguard Handler Deesak - Show gossip menu text below Honored reputation with faction 1031'),
(14,8719,10979,0,0,5,0,1031,224,0,0,0,'','Skyguard Handler Deesak - Show gossip menu text above Honored reputation with faction 1031'),
(15,8719,0,0,0,5,0,1031,224,0,0,0,'','Skyguard Handler Deesak - Show gossip option 0 if player has reputation Honored or above with faction 1031');
-- Not sure if the missing condition is necessary, but it makes things a lot clearer when looking at the table.

-- Create SmartAI for Skyguard Handler Deesak
DELETE FROM `smart_scripts` WHERE (source_type = 0 AND entryorguid = 23415);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(23415,0,0,1,62,0,100,0,8719,0,0,0,11,41279,0,0,0,0,0,7,0,0,0,0,0,0,0,'Skyguard Handler Deesak - On Gossip Option 0 Selected - Cast Spell 41279'),
(23415,0,1,0,61,0,100,0,0,0,0,0,72,0,0,0,0,0,0,7,0,0,0,0,0,0,0,'Skyguard Handler Deesak - On Linked Actions - Close Gossip');

-- Gryphoneer Windbellow
-- Add sniffed gossip texts to menu
DELETE FROM `gossip_menu_option` WHERE `menu_id` IN (8095, 8096, 8100);
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `action_menu_id`, `action_poi_id`, `box_coded`, `box_money`, `box_text`) VALUES
(8095, 0, 2, 'Show me where I can fly.',1,1,0,0,0,0,''),
(8096, 0, 0, 'Send me to Honor Point!',1,1,0,0,0,0,''),
(8096, 1, 0, 'Send me to the Abyssal Shelf!',1,1,0,0,0,0,''),
(8100, 0, 0, 'Send me to Shatter Point!',1,1,0,0,0,0,'');

-- Migrate NPC to use SmartAI
UPDATE `creature_template` SET `gossip_menu_id` = 8095, `AIName` = 'SmartAI', `ScriptName` = '', `gossip_menu_id` = 8096 WHERE `entry` = 20235;

-- Condition: Gossip menu ID 0 needs either quest ID 10163 or quest ID 10346 to be incomplete.
-- Condition: Gossip menu ID 1 needs quest ID 10382 to be complete
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`= 15 AND `SourceGroup` = 8096 AND `SourceEntry`IN (0,1);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`,`SourceGroup`,`SourceEntry`,`SourceId`,`ElseGroup`,`ConditionTypeOrReference`,`ConditionTarget`,`ConditionValue1`,`ConditionValue2`,`ConditionValue3`,`NegativeCondition`,`ErrorTextId`,`ScriptName`,`Comment`) VALUES
(15,8096,0,0,0,9,0,10163,0,0,0,0,'','Gryphoneer Windbellow - Show gossip option 0 if player has taken quest ID 10163'),
(15,8096,0,0,1,9,0,10346,0,0,0,0,'','Gryphoneer Windbellow - Show gossip option 0 if player has taken quest ID 10346'),
(15,8096,1,0,0,28,0,10382,0,0,0,0,'','Gryphoneer Windbellow - Show gossip option 1 if quest ID 10382 is completed');

-- Create SmartAI for Gryphoneer Windbellow
DELETE FROM `smart_scripts` WHERE (source_type = 0 AND entryorguid = 20235);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(20235,0,0,2,62,0,100,0,8096,0,0,0,11,33899,0,0,0,0,0,7,0,0,0,0,0,0,0,'Gryphoneer Windbellow - On Gossip Option 0 Selected - Cast Spell 33899'),
(20235,0,1,2,62,0,100,0,8096,1,0,0,11,35065,0,0,0,0,0,7,0,0,0,0,0,0,0,'Gryphoneer Windbellow - On Gossip Option 1 Selected - Cast Spell 35065'),
(20235,0,2,0,61,0,100,0,0,0,0,0,72,0,0,0,0,0,0,7,0,0,0,0,0,0,0,'Gryphoneer Windbellow - On Linked Actions - Close Gossip');

-- Skyguard Handler Irena
-- Add gossip texts to menu
DELETE FROM `gossip_menu_option` WHERE `menu_id` = 8718;
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`,  `option_id`, `npc_option_npcflag`, `action_menu_id`, `action_poi_id`, `box_coded`, `box_money`, `box_text`) VALUES
(8718, 0, 0, 'Yes, I''d love a ride to Blackwind Landing.',1,1,0,0,0,0,'');

DELETE FROM `npc_text` WHERE `ID` = 10977;
INSERT INTO `npc_text` (`ID`, `text0_0`, `text0_1`, `lang0`, `prob0`, `em0_0`, `em0_1`, `em0_2`, `em0_3`, `em0_4`, `em0_5`) VALUES
(10977, '', 'Hey, comrade.  If you get yourself in good with the Skyguard, I can offer you a direct flight to our base, Blackwind Landing, in the Skethyl Mountains.$B$BDo you like my beautiful nether ray?  It is lovely.', 0, 1, 0, 1, 0, 6, 0, 0);
-- Other npc_text (10978) is already set in DB

-- Migrate NPC to use SmartAI
UPDATE `creature_template` SET `gossip_menu_id` = 8718, `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = 23413;

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` IN (14, 15) AND `SourceGroup` = 8718;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`,`SourceGroup`,`SourceEntry`,`SourceId`,`ElseGroup`,`ConditionTypeOrReference`,`ConditionTarget`,`ConditionValue1`,`ConditionValue2`,`ConditionValue3`,`NegativeCondition`,`ErrorTextId`,`ScriptName`,`Comment`) VALUES
(14,8718,10977,0,0,5,0,1031,224,0,1,0,'','Skyguard Handler Irena - Show gossip menu text below Honored reputation with faction 1031'),
(14,8718,10978,0,0,5,0,1031,224,0,0,0,'','Skyguard Handler Irena - Show gossip menu text above Honored reputation with faction 1031'),
(15,8718,0,0,0,5,0,1031,224,0,0,0,'','Skyguard Handler Irena - Show gossip option 0 if player has reputation Honored or above with faction 1031');

-- Create SmartAI for Skyguard Handler Irena
DELETE FROM `smart_scripts` WHERE (source_type = 0 AND entryorguid = 23413);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(23413,0,0,1,62,0,100,0,8718,0,0,0,11,41278,0,0,0,0,0,7,0,0,0,0,0,0,0,'Skyguard Handler Irena - On Gossip Option 0 Selected - Cast Spell 41278'),
(23413,0,1,0,61,0,100,0,0,0,0,0,72,0,0,0,0,0,0,7,0,0,0,0,0,0,0,'Skyguard Handler Irena - On Linked Actions - Close Gossip');

-- Susurrus
-- Add sniffed gossip text to menu
DELETE FROM `gossip_menu_option` WHERE `menu_id`  = 7415;
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `action_menu_id`, `action_poi_id`, `box_coded`, `box_money`, `box_text`) VALUES
(7415, 0, 0, 'I am ready to be flown down to the Exodar.',1,1,0,0,0,0,'');

DELETE FROM `npc_text` WHERE `ID` = 8955;
INSERT INTO `npc_text` (`ID`, `text0_0`, `text0_1`, `lang0`, `prob0`, `em0_0`, `em0_1`, `em0_2`, `em0_3`, `em0_4`, `em0_5`) VALUES
(8955, 'Are you ready, $n?', '', 0, 1, 0, 0, 0, 0, 0, 0);

-- Set second gossip in gossip menu
DELETE FROM `gossip_menu` WHERE `entry` = 7415;
INSERT INTO `gossip_menu` (`entry`, `text_id`) VALUES
(7415, 8954),
(7415, 8955);

-- Migrate NPC to use SmartAI
UPDATE `creature_template` SET `gossip_menu_id` = 7415, `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = 17435;

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` IN (14, 15) AND `SourceGroup` = 7415;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`,`SourceGroup`,`SourceEntry`,`SourceId`,`ElseGroup`,`ConditionTypeOrReference`,`ConditionTarget`,`ConditionValue1`,`ConditionValue2`,`ConditionValue3`,`NegativeCondition`,`ErrorTextId`,`ScriptName`,`Comment`) VALUES
(14,7415,8954,0,0,2,0,23843,1,0,1,0,'','Susurrus - Show gossip menu text if player does not have item 23843 in inventory'),
(14,7415,8955,0,0,2,0,23843,1,0,0,0,'','Susurrus - Show gossip menu text if player has item 23843 in inventory'),
(15,7415,0,0,0,2,0,23843,1,0,0,0,'','Susurrus - Show gossip option 0 if player has item 23843 in inventory');

-- Create SmartAI for Susurrus
DELETE FROM `smart_scripts` WHERE (source_type = 0 AND entryorguid = 17435);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(17435,0,0,1,62,0,100,0,7415,0,0,0,11,32474,0,0,0,0,0,7,0,0,0,0,0,0,0,'Susurrus - On Gossip Option 0 Selected - Cast Spell 32474'),
(17435,0,1,0,61,0,100,0,0,0,0,0,72,0,0,0,0,0,0,7,0,0,0,0,0,0,0,'Susurrus - On Linked Actions - Close Gossip');


-- Protectorate Nether Drake
-- Add sniffed gossip text to menu
DELETE FROM `gossip_menu_option` WHERE `menu_id`  = 8229;
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `action_menu_id`, `action_poi_id`, `box_coded`, `box_money`, `box_text`) VALUES
(8229, 0, 0, 'I''m ready to fly! Take me up, dragon!',1,1,0,0,0,0,'');

-- Migrate NPC to use SmartAI
UPDATE `creature_template` SET `gossip_menu_id` = 8229, `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = 20903;

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 15 AND `SourceGroup` = 8229;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`,`SourceGroup`,`SourceEntry`,`SourceId`,`ElseGroup`,`ConditionTypeOrReference`,`ConditionTarget`,`ConditionValue1`,`ConditionValue2`,`ConditionValue3`,`NegativeCondition`,`ErrorTextId`,`ScriptName`,`Comment`) VALUES
(15,8229,0,0,0,2,0,29778,1,0,0,0,'','Protectorate Nether Drake - Show gossip option 0 if player has item 23843 in inventory'),
(15,8229,0,0,0,9,0,10438,0,0,0,0,'','Protectorate Nether Drake - Show gossip option 0 if player has quest 10438');
-- Create SmartAI for Protectorate Nether Drake
DELETE FROM `smart_scripts` WHERE (source_type = 0 AND entryorguid = 20903);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(20903,0,0,1,62,0,100,0,8229,0,0,0,52,627,0,0,0,0,0,7,0,0,0,0,0,0,0,'Protectorate Nether Drake - On Gossip Option 0 Selected - Init Taxi Path 627'),
(20903,0,1,0,61,0,100,0,0,0,0,0,72,0,0,0,0,0,0,7,0,0,0,0,0,0,0,'Protectorate Nether Drake - On Linked Actions - Close Gossip');


-- Veronia
-- Add sniffed gossip text to menu
DELETE FROM `gossip_menu_option` WHERE `menu_id`  = 8082;
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`,  `option_id`, `npc_option_npcflag`, `action_menu_id`, `action_poi_id`, `box_coded`, `box_money`, `box_text`) VALUES
(8082, 0, 0, 'I''m as ready as I''ll ever be.',1,1,0,0,0,0,'');

DELETE FROM `gossip_menu` WHERE `entry` = 8082;
INSERT INTO `gossip_menu` (`entry`, `text_id`) VALUES
(8082, 9989),
(8082, 9990);

-- Migrate NPC to use SmartAI
UPDATE `creature_template` SET `gossip_menu_id` = 8082, `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = 20162;

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` IN (14,15) AND `SourceGroup` = 8082;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`,`SourceGroup`,`SourceEntry`,`SourceId`,`ElseGroup`,`ConditionTypeOrReference`,`ConditionTarget`,`ConditionValue1`,`ConditionValue2`,`ConditionValue3`,`NegativeCondition`,`ErrorTextId`,`ScriptName`,`Comment`) VALUES
(14,8082,9989,0,0,28,0,10652,0,0,1,0,'','Veronia - Show gossip menu text if player does not have quest 10652'),
(14,8082,9990,0,0,28,0,10652,0,0,0,0,'','Veronia - Show gossip menu text if player has quest 10652'),
(15,8082,0,0,0,28,0,10652,0,0,0,0,'','Veronia - Show gossip option 0 if player has quest 10652');
-- Create SmartAI for Veronia
DELETE FROM `smart_scripts` WHERE (source_type = 0 AND entryorguid = 20162);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(20162,0,0,1,62,0,100,0,8082,0,0,0,85,34905,0,0,0,0,0,7,0,0,0,0,0,0,0,'Veronia - On Gossip Option 0 Selected - Invoker Cast \'Stealth Flight\''),
(20162,0,1,0,61,0,100,0,0,0,0,0,72,0,0,0,0,0,0,7,0,0,0,0,0,0,0,'Veronia - On Linked Actions - Close Gossip');


-- Cassa Crimsonwing
-- Add sniffed gossip text to menu
DELETE FROM `gossip_menu_option` WHERE `menu_id` = 8782;
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`,  `option_id`, `npc_option_npcflag`, `action_menu_id`, `action_poi_id`, `box_coded`, `box_money`, `box_text`) VALUES
(8782, 0, 0, 'Lady Jaina told me to speak to you about using a gryphon to survey Alcaz Island.',1,1,0,0,0,0,'');

-- Migrate NPC to use SmartAI
UPDATE `creature_template` SET  `gossip_menu_id` = 8782, `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = 23704;

-- Condition: Gossip menu ID 0 needs quest ID 11142 to be incomplete.
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`= 15 AND `SourceGroup` = 8782 AND `SourceEntry` = 0;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`,`SourceGroup`,`SourceEntry`,`SourceId`,`ElseGroup`,`ConditionTypeOrReference`,`ConditionTarget`,`ConditionValue1`,`ConditionValue2`,`ConditionValue3`,`NegativeCondition`,`ErrorTextId`,`ScriptName`,`Comment`) VALUES
(15,8782,0,0,0,9,0,11142,0,0,0,0,'','Cassa Crimsonwing - Show gossip option 0 if player has taken quest ID 11142');

-- Create SmartAI for Cassa Crimsonwing
DELETE FROM `smart_scripts` WHERE (source_type = 0 AND entryorguid = 23704);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(23704,0,0,1,62,0,100,0,8782,0,0,0,11,42295,0,0,0,0,0,7,0,0,0,0,0,0,0,'Cassa Crimsonwing - On Gossip Option 0 Selected - Cast Spell 42295'),
(23704,0,1,0,61,0,100,0,0,0,0,0,72,0,0,0,0,0,0,7,0,0,0,0,0,0,0,'Cassa Crimsonwing - On Linked Actions - Close Gossip');


-- Brazen
-- Add sniffed gossip text to menu
DELETE FROM `gossip_menu_option` WHERE `menu_id` = 7959;
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`,  `option_id`, `npc_option_npcflag`, `action_menu_id`, `action_poi_id`, `box_coded`, `box_money`, `box_text`) VALUES
(7959, 0, 0, 'I''m ready to go to Durnholde Keep.',1,1,0,0,0,0,'');

DELETE FROM `gossip_menu` WHERE `entry` = 7959;
INSERT INTO `gossip_menu` (`entry`, `text_id`) VALUES
(7959, 9779),
(7959, 9780);

UPDATE `creature_template` SET  `gossip_menu_id` = 7959, `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = 18725;

-- Condition: On gossip menu click, if item exists, perform linked action 
-- Condition: On gossip menu click, if item exists, perform linked action
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`IN (14, 15) AND `SourceGroup` = 7959;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`,`SourceGroup`,`SourceEntry`,`SourceId`,`ElseGroup`,`ConditionTypeOrReference`,`ConditionTarget`,`ConditionValue1`,`ConditionValue2`,`ConditionValue3`,`NegativeCondition`,`ErrorTextId`,`ScriptName`,`Comment`) VALUES
(14,7959,9779,0,0,2,0,25853,1,0,0,0,'','Brazen - Show gossip menu text if player has item 25853'),
(14,7959,9780,0,0,2,0,25853,1,0,1,0,'','Brazen - Show gossip menu text if player does not have item 25853'),
(15,7959,0,0,0,2,0,25853,1,0,0,0,'','Brazen - Show gossip option 0 if player has item 25853');

-- Create SmartAI for Brazen
DELETE FROM `smart_scripts` WHERE (source_type = 0 AND entryorguid = 18725);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(18725,0,0,1,62,0,100,0,7959,0,0,0,52,534,0,0,0,0,0,7,0,0,0,0,0,0,0,'Brazen - On Gossip Option 0 Selected - Activate Taxi Path 534'),
(18725,0,1,0,61,0,100,0,0,0,0,0,72,0,0,0,0,0,0,7,0,0,0,0,0,0,0,'Brazen - On Linked Actions - Close Gossip');