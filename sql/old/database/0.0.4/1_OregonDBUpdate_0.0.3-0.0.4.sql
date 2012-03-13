-- Structure Changes
DROP TABLE IF EXISTS `cheaters`;

DROP TABLE IF EXISTS `gossip_scripts`;
CREATE TABLE `gossip_scripts` (
  `id` mediumint(8) unsigned NOT NULL default '0',
  `delay` int(10) unsigned NOT NULL default '0',
  `command` mediumint(8) unsigned NOT NULL default '0',
  `datalong` mediumint(8) unsigned NOT NULL default '0',
  `datalong2` int(10) unsigned NOT NULL default '0',
  `dataint` int(11) NOT NULL default '0',
  `x` float NOT NULL default '0',
  `y` float NOT NULL default '0',
  `z` float NOT NULL default '0',
  `o` float NOT NULL default '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS gossip_menu;
CREATE TABLE gossip_menu (
  entry smallint(6) unsigned NOT NULL default '0',
  text_id mediumint(8) unsigned NOT NULL default '0',
  cond_1 tinyint(3) unsigned NOT NULL default '0',
  cond_1_val_1 mediumint(8) unsigned NOT NULL default '0',
  cond_1_val_2 mediumint(8) unsigned NOT NULL default '0',
  cond_2 tinyint(3) unsigned NOT NULL default '0',
  cond_2_val_1 mediumint(8) unsigned NOT NULL default '0',
  cond_2_val_2 mediumint(8) unsigned NOT NULL default '0',
  PRIMARY KEY (entry, text_id)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS gossip_menu_option;
CREATE TABLE gossip_menu_option (
  menu_id smallint(6) unsigned NOT NULL default '0',
  id smallint(6) unsigned NOT NULL default '0',
  option_icon mediumint(8) unsigned NOT NULL default '0',
  option_text text,
  option_id tinyint(3) unsigned NOT NULL default '0',
  npc_option_npcflag int(10) unsigned NOT NULL default '0',
  action_menu_id mediumint(8) unsigned NOT NULL default '0',
  action_poi_id mediumint(8) unsigned NOT NULL default '0',
  action_script_id mediumint(8) unsigned NOT NULL default '0',
  box_coded tinyint(3) unsigned NOT NULL default '0',
  box_money int(11) unsigned NOT NULL default '0',
  box_text text,
  cond_1 tinyint(3) unsigned NOT NULL default '0',
  cond_1_val_1 mediumint(8) unsigned NOT NULL default '0',
  cond_1_val_2 mediumint(8) unsigned NOT NULL default '0',
  cond_2 tinyint(3) unsigned NOT NULL default '0',
  cond_2_val_1 mediumint(8) unsigned NOT NULL default '0',
  cond_2_val_2 mediumint(8) unsigned NOT NULL default '0',
  cond_3 tinyint(3) unsigned NOT NULL default '0',
  cond_3_val_1 mediumint(8) unsigned NOT NULL default '0',
  cond_3_val_2 mediumint(8) unsigned NOT NULL default '0',
  PRIMARY KEY (menu_id, id)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

DELETE FROM gossip_menu_option WHERE menu_id=0;
INSERT INTO gossip_menu_option VALUES
(0,0,0,'GOSSIP_OPTION_QUESTGIVER',2,2,0,0,0,0,0,NULL,0,0,0,0,0,0,0,0,0),
(0,1,1,'I want to browse your goods',3,128,0,0,0,0,0,NULL,0,0,0,0,0,0,0,0,0),
(0,2,2,'I want to travel fast',4,8192,0,0,0,0,0,NULL,0,0,0,0,0,0,0,0,0),
(0,3,3,'Train me!',5,16,0,0,0,0,0,NULL,0,0,0,0,0,0,0,0,0),
(0,4,4,'Bring me back to life',6,16384,0,0,0,0,0,NULL,0,0,0,0,0,0,0,0,0),
(0,5,4,'Bring me back to life',7,32768,0,0,0,0,0,NULL,0,0,0,0,0,0,0,0,0),
(0,6,5,'Make this inn my home',8,65536,0,0,0,0,0,NULL,0,0,0,0,0,0,0,0,0),
(0,7,6,'Show me my bank',9,131072,0,0,0,0,0,NULL,0,0,0,0,0,0,0,0,0),
(0,8,7,'How do I form a guild?',10,262144,0,0,0,0,0,NULL,0,0,0,0,0,0,0,0,0),
(0,9,8,'I want to create a guild crest',11,524288,0,0,0,0,0,NULL,0,0,0,0,0,0,0,0,0),
(0,10,9,'I want to join the Battle Ground',12,1048576,0,0,0,0,0,NULL,0,0,0,0,0,0,0,0,0),
(0,11,6,'Auction!',13,2097152,0,0,0,0,0,NULL,0,0,0,0,0,0,0,0,0),
(0,12,0,'I''d like to stable my pet here',14,4194304,0,0,0,0,0,NULL,0,0,0,0,0,0,0,0,0),
(0,13,1,'I want to browse your goods',15,4096,0,0,0,0,0,NULL,0,0,0,0,0,0,0,0,0),
(0,14,2,'I wish to unlearn my talents',16,16,0,0,0,0,0,NULL,0,0,0,0,0,0,0,0,0),
(0,15,2,'I wish to unlearn my pet''s skills',17,16,0,0,0,0,0,NULL,0,0,0,0,0,0,0,0,0),
(0,16,0,'GOSSIP_OPTION_OUTDOORPVP',18,536870912,0,0,0,0,0,NULL,0,0,0,0,0,0,0,0,0);

ALTER TABLE creature_template ADD gossip_menu_id mediumint(8) unsigned NOT NULL default 0 AFTER IconName;

ALTER TABLE locales_npc_option CHANGE COLUMN entry id smallint(6) unsigned NOT NULL default '0';
ALTER TABLE locales_npc_option ADD menu_id smallint(6) unsigned NOT NULL default '0' FIRST;

ALTER TABLE locales_npc_option DROP PRIMARY KEY;
ALTER TABLE locales_npc_option ADD PRIMARY KEY (menu_id, id);

RENAME TABLE locales_npc_option TO locales_gossip_menu_option;

DROP TABLE IF EXISTS npc_option;
DROP TABLE IF EXISTS npc_gossip_textid;
ALTER TABLE `playercreateinfo` ADD COLUMN `orientation` float NOT NULL DEFAULT 0 AFTER `position_z`;

UPDATE playercreateinfo SET orientation = 5.696318 WHERE race=4;
UPDATE playercreateinfo SET orientation = 6.177156 WHERE race=3;
UPDATE playercreateinfo SET orientation = 2.70526 WHERE race=5;
UPDATE playercreateinfo SET orientation = 5.316046 WHERE race=10;
UPDATE playercreateinfo SET orientation = 2.083644 WHERE race=11;

-- Update GameEvents Start and End

UPDATE `game_event` SET end_time = '2020-12-31 00:00:00', start_time = '2007-06-21 00:00:00' WHERE entry = 1;
UPDATE `game_event` SET end_time = '2020-12-31 00:00:00', start_time = '2007-12-15 00:00:00' WHERE entry = 2;
UPDATE `game_event` SET end_time = '2020-12-31 00:00:00', start_time = '2007-08-06 07:00:00' WHERE entry = 3;
UPDATE `game_event` SET end_time = '2020-12-31 00:00:00', start_time = '2007-09-04 07:00:00' WHERE entry = 4;
UPDATE `game_event` SET end_time = '2020-12-31 00:00:00', start_time = '2007-10-08 07:00:00' WHERE entry = 5;
UPDATE `game_event` SET end_time = '2020-12-31 00:00:00', start_time = '2007-12-31 23:00:00' WHERE entry = 6;
UPDATE `game_event` SET end_time = '2020-12-31 00:00:00', start_time = '2008-02-05 00:00:00' WHERE entry = 7;
UPDATE `game_event` SET end_time = '2020-12-31 00:00:00', start_time = '2008-02-10 00:00:00' WHERE entry = 8;
UPDATE `game_event` SET end_time = '2020-12-31 00:00:00', start_time = '2007-04-09 00:00:00' WHERE entry = 9;
UPDATE `game_event` SET end_time = '2020-12-31 00:00:00', start_time = '2007-05-22 00:00:00' WHERE entry = 10;
UPDATE `game_event` SET end_time = '2020-12-31 00:00:00', start_time = '2007-09-24 00:00:00' WHERE entry = 11;
UPDATE `game_event` SET end_time = '2020-12-31 00:00:00', start_time = '2007-10-18 00:00:00' WHERE entry = 12;
UPDATE `game_event` SET end_time = '2020-12-31 00:00:00', start_time = '2007-08-05 08:00:00' WHERE entry = 14;
UPDATE `game_event` SET end_time = '2020-12-31 00:00:00', start_time = '2007-08-05 16:00:00' WHERE entry = 15;
UPDATE `game_event` SET end_time = '2020-12-31 00:00:00', start_time = '2007-08-05 00:00:00' WHERE entry = 16;
UPDATE `game_event` SET end_time = '2020-12-31 00:00:00', start_time = '2007-08-02 23:00:00' WHERE entry = 18;
UPDATE `game_event` SET end_time = '2020-12-31 00:00:00', start_time = '2007-08-09 23:00:00' WHERE entry = 19;
UPDATE `game_event` SET end_time = '2020-12-31 00:00:00', start_time = '2007-08-16 23:00:00' WHERE entry = 20;
UPDATE `game_event` SET end_time = '2020-12-31 00:00:00', start_time = '2007-08-23 23:00:00' WHERE entry = 21;
UPDATE `game_event` SET end_time = '2020-12-31 00:00:00', start_time = '2007-09-01 07:00:00' WHERE entry = 23;
UPDATE `game_event` SET end_time = '2020-12-31 00:00:00', start_time = '2007-10-03 00:00:00' WHERE entry = 26;
UPDATE `game_event` SET end_time = '2020-12-31 00:00:00', start_time = '2008-01-01 21:00:00' WHERE entry = 27;
UPDATE `game_event` SET end_time = '2020-12-31 00:00:00', start_time = '2008-03-24 01:00:00' WHERE entry = 29;
UPDATE `game_event` SET end_time = '2020-12-31 00:00:00', start_time = '2008-04-07 02:00:00' WHERE entry = 30;
UPDATE `game_event` SET end_time = '2020-12-31 00:00:00', start_time = '2008-04-21 02:00:00' WHERE entry = 31;
UPDATE `game_event` SET end_time = '2020-12-31 00:00:00', start_time = '2008-05-05 02:00:00' WHERE entry = 32;
UPDATE `game_event` SET end_time = '2020-01-01 02:00:00', start_time = '2008-05-15 16:00:00' WHERE entry = 34;

-- Script Names

UPDATE `creature_template` SET ScriptName = 'boss_drekthar' WHERE entry = 11946;
UPDATE `creature_template` SET ScriptName = 'boss_galvangar' WHERE entry = 11947;
UPDATE `creature_template` SET ScriptName = 'boss_vanndar' WHERE entry = 11948;
UPDATE `creature_template` SET ScriptName = 'boss_balinda' WHERE entry = 11949;
UPDATE `creature_template` SET ScriptName = 'AV_Marshals' WHERE entry = 14762;
UPDATE `creature_template` SET ScriptName = 'AV_Marshals' WHERE entry = 14763;
UPDATE `creature_template` SET ScriptName = 'AV_Marshals' WHERE entry = 14764;
UPDATE `creature_template` SET ScriptName = 'AV_Marshals' WHERE entry = 14765;
UPDATE `creature_template` SET ScriptName = 'AV_Warmasters' WHERE entry = 14772;
UPDATE `creature_template` SET ScriptName = 'AV_Warmasters' WHERE entry = 14773;
UPDATE `creature_template` SET ScriptName = 'AV_Warmasters' WHERE entry = 14776;
UPDATE `creature_template` SET ScriptName = 'AV_Warmasters' WHERE entry = 14777;
UPDATE `creature_template` SET ScriptName = 'npc_taxi' WHERE entry = 17209;
UPDATE `creature_template` SET ScriptName = 'npc_taxi' WHERE entry = 17435;
UPDATE `creature_template` SET ScriptName = 'npc_taxi' WHERE entry = 18725;
UPDATE `creature_template` SET ScriptName = 'npc_taxi' WHERE entry = 20162;
UPDATE `creature_template` SET ScriptName = 'npc_taxi' WHERE entry = 20235;
UPDATE `creature_template` SET ScriptName = 'npc_taxi' WHERE entry = 20903;
UPDATE `creature_template` SET ScriptName = 'npc_taxi' WHERE entry = 23413;
UPDATE `creature_template` SET ScriptName = 'npc_taxi' WHERE entry = 23415;
UPDATE `creature_template` SET ScriptName = 'npc_taxi' WHERE entry = 23704;
UPDATE `creature_template` SET ScriptName = 'npc_taxi' WHERE entry = 25059;
UPDATE `creature_template` SET ScriptName = 'npc_taxi' WHERE entry = 25236;

-- Scripttexts Cleanup

REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1645009, 'master_kelerun_bloodmourn YELL_PHASE', 'Show $N the meaning of pain, Sunstriker!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1645008, 'master_kelerun_bloodmourn YELL_PHASE', 'Show this upstart how a real Blood Knight fights, Swiftblade!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1645007, 'master_kelerun_bloodmourn YELL_PHASE', 'Champion Lightrend, make me proud!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1645006, 'master_kelerun_bloodmourn YELL_PHASE', 'Let the trial begin, Bloodwrath, attack!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580109, 'lady liadrinn - SAY3', 'Blessed ancestors! I feel it... so much love... so much grace... there are... no words... impossible to describe...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12525, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580108, 'lady liadrinn - SAY2', 'Can it be?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12525, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580107, 'lady liadrinn - SAY1', 'Our arrogance was unpardonable. We damned one of the most noble beings of all. We may never atone for this sin.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12524, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580106, 'prophet velen - SAY8', 'Farewell...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12523, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580105, 'prophet velen - SAY7', 'Salvation, young one. It waits for us all.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12522, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580104, 'prophet velen - SAY6', 'In time, the light and hope held within - will rebirth more than this mere fount of power... Mayhap, they will rebirth the soul of a nation.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12521, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580103, 'prophet velen - SAY5', 'Gaze now, mortals - upon the HEART OF M''URU! Unblemished. Bathed by the light of Creation - just as it was at the Dawn.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12520, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580102, 'prophet velen - SAY4', 'Then fortunate it is, that I have reclaimed the noble naaru''s spark from where it fell! Where faith dwells, hope is never lost, young blood elf.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12519, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580101, 'prophet velen - SAY3', 'The creature Entropius, whom you were forced to destroy, was once the noble naaru, M''uru. In life, M''uru channeled vast energies of LIGHT and HOPE. For a time, a misguided few sought to steal those energies...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12518, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580100, 'prophet velen - SAY2', 'As the Legion''s final defeat draws ever-nearer, stand proud in the knowledge that you have saved worlds without number from the flame. Just as this day marks an ending, so too does it herald a new beginning...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12516, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580099, 'prophet velen - SAY1', 'Mortal heroes, your victory here today was foretold long ago. My brother''s anguished cry of defeat will echo across the universe, bringing renewed hope to all those who still stand against the Burning Crusade.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12515, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580098, 'KJ - SAY_KALEC_ORB_READY4', 'I have channeled all I can! The power is in your hands!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12443, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580097, 'KJ - SAY_KALEC_ORB_READY3', 'Another orb is ready! Make haste!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12442, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580096, 'KJ - SAY_KALEC_ORB_READY2', 'I have empowered another orb! Use it quickly!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12441, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580095, 'KJ - SAY_KALEC_ORB_READY1', 'I will channel my power into the orbs, be ready!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12440, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580094, 'KJ - EMOTE_KJ_DARKNESS', 'begins to channel dark energy', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 3);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580093, 'KJ - SAY_KJ_DEATH', 'Nooooooooooooo!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12527, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580092, 'KJ - SAY_KALECGOS_JOIN', 'You are not alone. The Blue Dragonflight shall help you vanquish the Deceiver.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12438, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580091, 'KJ - SAY_KALECGOS_ENCOURAGE', 'Strike now, heroes, while he is weakened! Vanquish the Deceiver!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12449, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580090, 'KJ - SAY_KALECGOS_GOODBYE', 'Goodbye, Anveena, my love. Few will remember your name, yet this day you change the course of destiny. What was once corrupt is now pure. Heroes, do not let her sacrifice be in vain.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12450, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580089, 'KJ - SAY_ANVEENA_GOODBYE', 'The nightmare is over, the spell is broken! Goodbye, Kalec, my love!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12514, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580088, 'KJ - SAY_KALECGOS_FATE', 'Yes, Anveena! Let fate embrace you now!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12448, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580087, 'KJ - SAY_ANVEENA_KALEC', 'Kalec... Kalec?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12513, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580086, 'KJ - SAY_KALECGOS_FOCUS', 'Anveena, I love you! Focus on my voice, come back for me now! Only you can cleanse the Sunwell!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12447, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580085, 'KJ - SAY_ANVEENA_LOST', 'But I''m... lost... I cannot find my way back!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12512, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580084, 'KJ - SAY_KALECGOS_LETGO', 'You must let go! You must become what you were always meant to be! The time is now, Anveena!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12446, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580083, 'KJ - SAY_ANVEENA_IMPRISONED', 'I serve only the Master now.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12511, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580082, 'KJ - SAY_KALECGOS_AWAKEN', 'Anveena, you must awaken, this world needs you!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12445, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580081, 'KJ - SAY_KJ_PHASE5', 'Aggghh! The powers of the Sunwell... turned... against me! What have you done? WHAT HAVE YOU DONE?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12510, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580080, 'KJ - SAY_KJ_PHASE4', 'Do not harbor false hope. You cannot win!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12509, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580079, 'KJ - SAY_KJ_PHASE3', 'I will not be denied! This world shall fall!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12508, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580078, 'KJ - SAY_KJ_DARKNESS3', 'Oblivion!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12507, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580077, 'KJ - SAY_KJ_DARKNESS2', 'Destruction!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12506, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580076, 'KJ - SAY_KJ_DARKNESS1', 'Chaos!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12505, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580075, 'KJ - SAY_KJ_REFLECTION2', 'The enemy is among you.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12504, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580074, 'KJ - SAY_KJ_REFLECTION1', 'Who can you trust?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12503, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580073, 'KJ - SAY_KJ_SLAY2', 'Anak-ky''ri!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12502, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580072, 'KJ - SAY_KJ_SLAY1', 'Another step towards destruction!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12501, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580071, 'KJ - SAY_KJ_EMERGE', 'The expendible have perished... So be it! Now I shall succeed where Sargeras could not! I will bleed this wretched world and secure my place as the true master of the Burning Legion. The end has come! Let the unraveling of this world commence!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12500, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580070, 'KJ - SAY_KJ_OFFCOMBAT5', 'Drain the girl! Drain her power until there is nothing but a vacant shell!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12499, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580069, 'KJ - SAY_KJ_OFFCOMBAT4', 'Fail me and suffer for eternity!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12498, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580068, 'KJ - SAY_KJ_OFFCOMBAT3', 'I have waited long enough!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12497, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580067, 'KJ - SAY_KJ_OFFCOMBAT2', 'Stay on task! Do not waste tim!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12496, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580066, 'KJ - SAY_KJ_OFFCOMBAT1', 'All my plans have led to this!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12495, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580065, 'eredar - emote conflagration', 'directs Conflagration at $N', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 3);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580064, 'eredar - emote shadow nova', 'directs Shadow Nova at $N', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 3);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580063, 'eredar - YELL_INTRO_ALY_8', 'These are the pillars...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580062, 'eredar - YELL_INTRO_SAC_7', 'These are the hallmarks...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580061, 'eredar - YELL_INTRO_ALY_6', 'Chaos...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580060, 'eredar - YELL_INTRO_SAC_5', 'Mistrust...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580059, 'eredar - YELL_INTRO_ALY_4', 'Hatred...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580058, 'eredar - YELL_INTRO_SAC_3', 'Confusion...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580057, 'eredar - YELL_INTRO_ALY_2', 'Depravity...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580056, 'eredar - YELL_INTRO_SAC_1', 'Misery...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12484, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580055, 'eredar - YELL_ENRAGE', 'Time is a luxury you no longer possess!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580054, 'eredar - YELL_SAC_DEAD', 'I... fade.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580053, 'eredar - YELL_SAC_KILL_2', 'Ee-nok Kryul!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12487, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580052, 'eredar - YELL_SAC_KILL_1', 'Shadow engulf.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12486, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580051, 'eredar - YELL_SISTER_ALYTHESS_DEAD', 'Alythess! Your fire burns within me!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12488, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580050, 'eredar - YELL_SHADOW_NOVA', 'Shadow to the aid of fire!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12485, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580049, 'eredar - YELL_BERSERK', 'Your luck has run its curse!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12493, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580048, 'eredar - YELL_ALY_DEAD', 'De-ek Anur!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12494, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580047, 'eredar - YELL_ALY_KILL_2', 'Ed-ir Halach!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12491, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580046, 'eredar - YELL_ALY_KILL_1', 'Fire consume.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12490, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580045, 'eredar - YELL_SISTER_SACROLASH_DEAD', 'Sacrolash!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12492, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580044, 'eredar - YELL_CANFLAGRATION', 'Fire to the aid of shadow!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12489, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580043, 'felmyst - YELL_KALECGOS', 'Madrigosa deserved a far better fate. You did what had to be done, but this battle is far from over.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12439, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580042, 'felmyst - YELL_DEATH', 'Kil''jaeden... will... prevail...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12483, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580041, 'felmyst - YELL_BERSERK', 'No more hesitation! Your fates are written!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12482, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580040, 'felmyst- YELL_TAKEOFF', 'I am stronger than ever before!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12479, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580039, 'felmyst - YELL_BREATH', 'Choke on your final breath!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12478, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580038, 'felmyst - YELL_KILL2', 'The end has come!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12481, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580037, 'felmyst - YELL_KILL1', 'I kill for the master!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12480, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1580036, 'felmyst - YELL_BIRTH', 'Glory to Kil''jaeden! Death to all who oppose!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 12477, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1560049, 'taretha SAY_TA_ESCAPED', 'Thrall, you escaped!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1560048, 'taretha SAY_TA_FREE', 'I''m free! Thank you all!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1560047, 'thrall hillsbrad SAY_TH_LEAVE_COMBAT3', 'I will not waste this chance. I will seek out my destiny.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 10457, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1560046, 'thrall hillsbrad SAY_TH_LEAVE_COMBAT2', 'Thank you, strangers. You have given me hope.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 10456, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1560045, 'thrall hillsbrad SAY_TH_LEAVE_COMBAT1', 'I am truly in your debt, strangers.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 10455, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1560044, 'thrall hillsbrad SAY_TH_RANDOM_KILL3', 'I did not ask for this!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 10454, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1560043, 'thrall hillsbrad SAY_TH_RANDOM_KILL2', 'It should not have come to this!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 10453, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1560042, 'thrall hillsbrad SAY_TH_RANDOM_KILL1', 'You have forced my hand!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 10452, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1560041, 'thrall hillsbrad SAY_TH_RANDOM_AGGRO4', 'Blackmoore has much to answer for!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 10451, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1560040, 'thrall hillsbrad SAY_TH_RANDOM_AGGRO3', 'I am a slave no longer!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 10450, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1560039, 'thrall hillsbrad SAY_TH_RANDOM_AGGRO2', 'This day is long overdue. Out of my way!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 10449, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1560038, 'thrall hillsbrad SAY_TH_RANDOM_AGGRO1', 'I have earned my freedom!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 10448, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1560037, 'thrall hillsbrad SAY_TH_RANDOM_DIE2', 'A good day...to die...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 10461, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1560036, 'thrall hillsbrad SAY_TH_RANDOM_DIE1', 'Taretha...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 10460, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1560035, 'thrall hillsbrad SAY_TH_RANDOM_LOW_HP2', 'I will fight to the last!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 10459, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1560034, 'thrall hillsbrad SAY_TH_RANDOM_LOW_HP1', 'Things are looking grim...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 10458, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1560033, 'thrall hillsbrad SAY_TH_EVENT_COMPLETE', 'Goodbye, Taretha. I will never forget your kindness.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 10472, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1560032, 'thrall hillsbrad SAY_TH_EPOCH_KILL_TARETHA', 'No!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 10471, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1560031, 'thrall hillsbrad SAY_TH_EPOCH_WONDER', 'Who or what was that?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 10470, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1560030, 'thrall hillsbrad SAY_TH_MEET_TARETHA', 'Taretha! What foul magic is this?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1560029, 'thrall hillsbrad SAY_TH_CHURCH_END', 'Taretha must be in the inn. Let''s go.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1560028, 'thrall hillsbrad SAY_TH_MOUNTS_UP', 'Let''s ride!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 10469, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1560027, 'thrall hillsbrad SAY_TH_START_EVENT_PART2', 'Very well. Tarren Mill lies just west of here. Since time is of the essence...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 10468, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1560026, 'thrall hillsbrad SAY_TH_SKARLOC_TAUNT', 'I''ll never be chained again!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 10467, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1560025, 'thrall hillsbrad SAY_TH_SKARLOC_MEET', 'A rider approaches!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 10466, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1560024, 'thrall hillsbrad SAY_TH_ARMORY', 'As long as we''re going with a new plan, I may aswell pick up a weapon and some armor.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1560023, 'thrall hillsbrad SAY_TH_START_EVENT_PART1', 'Very well then. Let''s go!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 10465, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1544015, 'magtheridon EMOTE_FREED', 'breaks free!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1543025, 'vazruden EMOTE', 'descends from the sky', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 3);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1543024, 'vazruden SAY_DIE', 'My lord will be the end you all...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 10299, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1543023, 'vazruden SAY_KILL_2', 'Your days are done!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 10298, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1543022, 'vazruden SAY_KILL_1', 'It is over. Finished!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 10297, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1543021, 'vazruden SAY_AGGRO_3', 'The Dark Lord laughs at you!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 10296, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1543020, 'vazruden SAY_AGGRO_2', 'You are nothing, I answer a higher call!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 10295, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1543019, 'vazruden SAY_AGGRO_1', 'Your time is running out!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 10294, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1543018, 'vazruden SAY_WIPE', 'Is there no one left to test me?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 10293, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1543017, 'vazruden SAY_INTRO', 'You have faced many challenges, pity they were all in vain. Soon your people will kneel to my lord!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 10292, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1534030, 'archimonde SAY_SOUL_CHARGE2', 'Bow to my will.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11042, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1534029, 'archimonde SAY_SOUL_CHARGE1', 'You are mine now.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 10988, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1534028, 'archimonde SAY_DEATH', 'No, it cannot be! Nooo!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 10992, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1534027, 'archimonde SAY_ENRAGE', 'At last it is here. Mourn and lament the passing of all you have ever known and all that would have been! Akmin-kurai!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 10993, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1534026, 'archimonde SAY_SLAY3', 'I am the coming of the end!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11045, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1534025, 'archimonde SAY_SLAY2', 'Your soul will languish for eternity.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 10991, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1534024, 'archimonde SAY_SLAY1', 'All creation will be devoured!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11044, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1534023, 'archimonde SAY_AIR_BURST2', 'Away vermin!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11043, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1534022, 'archimonde SAY_AIR_BURST1', 'A-kreesh!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 10989, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1534021, 'archimonde SAY_DOOMFIRE2', 'Manach sheek-thrish!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11041, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1534020, 'archimonde SAY_DOOMFIRE1', 'This world will burn!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 10990, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1534019, 'archimonde SAY_AGGRO', 'Your resistance is insignificant.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 10987, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1534018, 'archimonde SAY_PRE_EVENTS_COMPLETE', 'All of your efforts have been in vain, for the draining of the World Tree has already begun. Soon the heart of your world will beat no more.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 10986, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1534017, 'thrall hyjal DEATH', 'Uraaa...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11034, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1534016, 'thrall hyjal SUCCESS', 'We have played our part and done well. It is up to the others now.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11035, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1534015, 'thrall hyjal FAILURE', 'It is over. Withdraw! We have failed.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11033, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1534014, 'thrall hyjal RALLY 2', 'Do not give an inch of ground!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11060, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1534013, 'thrall hyjal RALLY 1', 'Victory or death!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11059, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1534012, 'thrall hyjal BEGIN', 'Hold them back! Do not falter!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11030, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1534011, 'thrall hyjal INCOMING', 'Make ready for another wave! LOK-TAR OGAR!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11032, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1534010, 'thrall hyjal ATTACKED 2', 'Bring the fight to me and pay with your lives!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11061, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1534009, 'thrall hyjal ATTACKED 1', 'I will lie down for no one!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11031, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1534008, 'jaina hyjal DEATH', 'I did... my best.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11010, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1534007, 'jaina hyjal SUCCESS', 'We have won valuable time. Now we must pull back!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11011, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1534006, 'jaina hyjal FAILURE', 'We are lost. Fall back!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11009, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1534005, 'jaina hyjal RALLY 2', 'We must hold strong!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11051, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1534004, 'jaina hyjal RALLY 1', 'Hold them back as long as possible.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11050, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1534003, 'jaina hyjal BEGIN', 'Don''t give up! We must prevail!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11006, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1534002, 'jaina hyjal INCOMING', 'Stay alert! Another wave approaches.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11008, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1534001, 'jaina hyjal ATTACKED 2', 'They''ve broken through!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11049, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1534000, 'jaina hyjal ATTACKED 1', 'I''m in jeopardy, help me if you can!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11007, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1532114, 'barnes RAJ4', 'But don''t take it from me, see for yourself what tragedy lies ahead when the paths of star-crossed lovers meet. And now...on with the show!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 9343, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1532113, 'barnes RAJ3', 'But beware, for not all love stories end happily, as you may find out. Sometimes, love pricks like a thorn.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 9342, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1532112, 'barnes RAJ2', 'Tonight, we explore a tale of forbidden love!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 9341, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1532111, 'barnes RAJ1', 'Welcome, Ladies and Gentlemen, to this evening''s presentation!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 9176, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1532110, 'barnes HOOD4', 'But don''t let me pull the wool over your eyes. See for yourself what lies beneath those covers! And now... on with the show!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 9337, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1532109, 'barnes HOOD3', 'Take for instance, this quiet, elderly woman, waiting for a visit from her granddaughter. Surely there is nothing to fear from this sweet, grey-haired, old lady.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 9336, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1532108, 'barnes HOOD2', 'Tonight, things are not what they seem. For tonight, your eyes may not be trusted', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 9335, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1532107, 'barnes HOOD1', 'Good evening, Ladies and Gentlemen! Welcome to this evening''s presentation!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 9175, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1532106, 'barnes OZ4', 'Will she survive? Will she prevail? Only time will tell. And now ... on with the show!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 9340, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1532105, 'barnes OZ3', 'But she is pursued... by a wicked malevolent crone!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 9339, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1532104, 'barnes OZ2', 'Tonight we plumb the depths of the human soul as we join a lost, lonely girl trying desperately -- with the help of her loyal companions -- to find her way home!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 9338, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1532103, 'barnes OZ1', 'Welcome Ladies and Gentlemen, to this evening''s presentation!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 9174, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1269028, 'medivh SAY_ORCS_ANSWER', 'Gul''dan speaks the truth! We should return at once to tell our brothers of the news! Retreat back trought the portal!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1269027, 'medivh SAY_ORCS_ENTER', 'Orcs of the Horde! This portalis the gateway to your new destiny! Azeroth lies before you, ripe for the taking!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1269026, 'medivh SAY_WIN', 'I am grateful for your aid, champions. Now, Gul''dan''s Horde will sweep across this world, like a locust swarm, and all my designs, all my carefully laid plans will at last fall into place.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 10440, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1269025, 'medivh SAY_DEATH', 'No... damn this feeble mortal coil...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 10441, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1269024, 'medivh SAY_WEAK25', 'The shield is nearly gone! All that I have worked for is in danger!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 10439, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1269023, 'medivh SAY_WEAK50', 'My powers must be concentrated on the portal! I do not have time to hold the shield!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 10438, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1269022, 'medivh SAY_WEAK75', 'Champions, my shield grows weak!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 10437, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1269021, 'medivh SAY_INTRO', 'What is this? Champions, coming to my aid? I sense the hand of the dark one in this. Truly this sacred event bears his blessing?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 10436, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1269020, 'medivh SAY_ENTER', 'The time has come! Gul''dan, order your warlocks to double their efforts! Moments from now the gateway will open, and your Horde will be released upon this ripe, unsuspecting world!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 10435, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1269019, 'saat SAY_SAAT_WELCOME', 'Stop! Do not go further, mortals. You are ill-prepared to face the forces of the Infinite Dragonflight. Come, let me help you.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1269018, 'aeonus EMOTE_FRENZY', 'goes into a frenzy!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1230000, 'rocknot SAY_GOT_BEER', 'Ah, hits the spot!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1189010, 'whitemane SAY_WH_RESSURECT', 'Arise, my champion!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 5840, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1189009, 'whitemane SAY_WH_KILL', 'The Light has spoken!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 5839, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1189008, 'whitemane SAY_WH_INTRO', 'What, Mograine has fallen? You shall pay for this treachery!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 5838, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1189007, 'mograine SAY_MO_RESSURECTED', 'At your side, milady!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 5837, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1189006, 'mograine SAY_MO_KILL', 'Unworthy!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 5836, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1189005, 'mograine SAY_MO_AGGRO', 'Infidels! They must be purified!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 5835, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1189004, 'herod EMOTE_ENRAGE', 'becomes enraged!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1189003, 'herod SAY_KILL', 'Hah, is that all?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 5831, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1189002, 'herod SAY_ENRAGE', 'Light, give me strength!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 5833, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1189001, 'herod SAY_WHIRLWIND', 'Blades of Light!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 5832, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1189000, 'herod SAY_AGGRO', 'Ah, I have been waiting for a real challenge!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 5830, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1060005, NULL, 'Cloaking systems online! CLUCK! Engaging cloak for transport to Booty Bay!', NULL, NULL, 'Tarnsysteme online! GLUCK! Tarnung fur Transport nach Beutebucht wird aktiviert.', NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1060004, NULL, 'No one challenges the wastevander nomads -- not even robotic chickens! ATTACK!', NULL, NULL, 'Niemand fordert die Wusenlaufernomaden heraus - auch keine Robothuhner! ANGRIFF!', NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1060003, NULL, 'CLUCK! Sensors detect spatial anomaly -- danger imminent! CLUCK', NULL, NULL, 'GLUCK! Sensoren haben raumliche Anomalie entdeckt - Gefahr im Verzug! GLUCK!', NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1060002, NULL, 'Threat analyzed! Activating combat plan beta! CLUCK!', NULL, NULL, 'Bedrohung analysiert! Gefechtsplan Beta wird aktiviert! GLUCK!', NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1060001, NULL, 'Physical threat detected! Evasive action! CLUCK!', NULL, NULL, 'Korperliche Bedrohung entdeckt! Ausweichmanover! GLUCK!', NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1060000, NULL, 'Emergency power activated! Initializing ambulatory motor! CLUCK!', NULL, NULL, 'Notfallenergie aktiviert! Ambulatorenantrieb wird gestartet! GLUCK!', NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1047010, 'Willix', 'Many thanks for your help.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1047009, 'Willix', 'I will rather rest a moment and come again to breath, before I return to Ratchet.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1047008, 'Willix', 'Finally! I am glad that I come, finally out here.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1047007, 'Willix', 'I am glad that we are out again from this damned ditch. However, up here it is not much better!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1047006, 'Willix', 'I think, I see a way how we come out of this damned thorn tangle.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1047005, 'Willix', 'I do not understand how these disgusting animals can live at such a place.... puh as this stinks!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1047004, 'Willix', 'Danger is behind every corner.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1047003, 'Willix', 'In this ditch there are Blueleaf Tuber! As if the gold waited only to be dug out, I say it you!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1047002, 'Willix', 'Help! Get this Raging Agam''ar from me!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1047001, 'Willix', 'There on top resides Charlga Razorflank. The damned old Crone.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1047000, 'Willix', 'Phew! Finally,out here. However, it will not become easy. Detain your eyes after annoyance.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1036001, 'smite INST_SAY_ALARM2', 'We''re under attack! A vast, ye swabs! Repel the invaders!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 5777, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1036000, 'smite INST_SAY_ALARM1', 'You there, check out that noise!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 5775, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000401, 'npc_daranelle - SAY_DARANELLE', 'Good $N, you are under the spell''s influence. I must analyze it quickly, then we can talk.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000400, 'mobs_nether_drake - SAY_NIHIL_INTERRUPT', 'NOOOOooooooo!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000399, 'mobs_nether_drake - SAY_NIHIL_4', 'No doubt the fools that banished me are long dead. I shall take wing survey my demense. Pray to whatever gods you hold dear that we do not meet again.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000398, 'mobs_nether_Drake - SAY_NIHIL_3', 'Where shall I begin? I cannot bother myself with a worm such as yourself. There is a world to be conquered!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000397, 'mobs_nether_drake - SAY_NIHIL_2', 'All of Draenor shall quick beneath my feet! I will destroy this world and reshape it in my image!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000396, 'mobs_nether_drake - SAY_NIHIL_1', 'Muahahahaha! You fool! You''ve released me from my banishment in the interstices between space and time!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000395, 'doomwalker SAY_DEATH', 'System failure in five, f-o-u-r...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11352, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000394, 'doomwalker SAY_SLAY_3', 'Target exterminated.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11351, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000393, 'doomwalker SAY_SLAY_2', 'Directive accomplished.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11350, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000392, 'doomwalker SAY_SLAY_1', 'Threat level zero.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11349, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000391, 'doomwalker SAY_OVERRUN_2', 'Engage maximum speed.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11348, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000390, 'doomwalker SAY_OVERRUN_1', 'Trajectory locked.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11347, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000389, 'doomwalker SAY_EARTHQUAKE_2', 'Magnitude set. Release.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11346, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000388, 'doomwalker SAY_EARTHQUAKE_1', 'Tectonic disruption commencing.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11345, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000387, 'doomwalker SAY_AGGRO', 'Do not proceed. You will be eliminated!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11344, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000386, 'kazzak SAY_RAND2', 'Impudent whelps, you only delay the inevitable. Where one has fallen, ten shall rise. Such is the will of Kazzak...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000385, 'kazzak SAY_RAND1', 'Invaders, you dangle upon the precipice of oblivion! The Burning Legion comes and with it comes your end.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000384, 'kazzak EMOTE_FRENZY', '%s goes into a frenzy!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000383, 'kazzak SAY_DEATH', 'The Legion... will never... fall.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11340, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000382, 'kazzak SAY_KILL3', 'The universe will be remade.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11339, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000381, 'kazzak SAY_KILL2', 'Contemptible wretch!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11338, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000380, 'kazzak SAY_KILL1', 'Kirel Narak!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11337, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000379, 'kazzak SAY_SURPREME2', 'I''ll rip the flesh from your bones!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11336, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000378, 'kazzak SAY_SURPREME1', 'All life must be eradicated!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11335, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000377, 'kazzak SAY_AGGRO2', 'All mortals will perish!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11334, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000376, 'kazzak SAY_AGGRO1', 'The Legion will conquer all!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11333, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000375, 'kazzak SAY_INTRO', 'I remember well the sting of defeat at the conclusion of the Third War. I have waited far too long for my revenge. Now the shadow of the Legion falls over this world. It is only a matter of time until all of your failed creation... is undone.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11332, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000374, 'Quest: Battle of the crimson watch WavesInfo - Fourth', 'Torloth your master calls!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000373, 'Quest: Battle of the crimson watch WavesInfo - Third', 'Run while you still can. The highlords come soon...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000372, 'Quest: Battle of the crimson watch WavesInfo - Second', 'You are no challenge for the Crimson Sigil. Mind breakers, end this nonsense.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000371, 'Quest: Battle of the crimson watch WavesInfo - First', 'What manner of fool dares stand before Illidan Stormrage? Soldiers, destroy these insects!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000370, 'Quest: Battle of the crimson watch TorlothAnim - Fourth', 'Yes, Lord Illidan, I would sacrifice to you this magnificent physique. On this day you will fall - another victim of Torloth...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000369, 'Quest: Battle of the crimson watch TorlothAnim - Third', 'As you desire, Lord Illidan.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000368, 'Quest: Battle of the crimson watch TorlothAnim - Second', 'Destroy them, Torloth. Let lose their blood like a river upon this hallowed ground.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000367, 'TorlothAnim - First', 'At your command, my liege...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000366, 'Quest: Battle of the crimson watch - END_TEXT', 'So you have defeated the Crimson Sigil. You now seek to challenge my rule? Not even Arthas could defeat me, yet you dare to even harbor such thoughts? Then I say to you, come! Come $N! The Black Temple awaits...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000365, 'npc_kayra_longmane - SAY_PROGRESS_6', 'I can see my fellow druids from here. Thank you, $N. I''m sure Ysiel will reward you for your actions!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000364, 'npc_kayra_longmane - SAY_PROGRESS_5', 'We''re almost to the refuge! Let''s go.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000363, 'npc_kayra_longmane - SAY_PROGRESS_4', 'Look out, $N! Enemies ahead!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000362, 'npc_kayra_longmane - SAY_PROGRESS_3', 'Let''s keep moving. We''re not safe here!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000361, 'npc_kayra_longmane - SAY_PROGRESS_2', 'Looks like we won''t get away so easy. Get ready!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000360, 'npc_kayra_longmane - SAY_PROGRESS_1', 'Is the way clear? Let''s get out while we can, $N.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000359, 'npcs_ashyen_and_keleth - GOSSIP_REWARD_BLESS', 'You have my blessing', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000358, 'npc_lady_sylvanas_windrunner - EMOTE_LAMENT_END', 'kneels down and pick up the amulet.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000357, 'npc_lady_sylvanas_windrunner - SAY_LAMENT_END', 'Belore...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000356, 'npc_isla_starmane - SAY_PROGRESS_4', 'Ok, I think I can make it on my own from here. Thank you so much for breaking me out of there!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000355, 'npc_isla_starmane - SAY_PROGRESS_3', 'Alright, let''s do this!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000354, 'npc_isla_starmane - SAY_PROGRESS_2', 'You sure you''re ready? Take a moment.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000353, 'npc_isla_starmane - SAY_PROGRESS_1', 'Ok let''s get out of here!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000352, 'npc_floon - SAY_FLOON_ATTACK', 'I choose the third option: KILLING YOU!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000351, 'mob_unkor_the_ruthless - SAY_SUBMIT', 'I give up! Please don''t kill me!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000350, 'mob_aquementas - AGGRO_YELL_AQUE', 'Who dares awaken Aquementas?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000349, 'npc_kaya_flathoof - SAY_END', 'Thank you for helping me. I know my way back from here.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000348, 'npc_kaya_flathoof - SAY_AMBUSH', 'Look out! We''re under attack!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000347, 'npc_kaya_flathoof - SAY_START', 'Let''s go before they find out I''m free!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000346, 'npc_deathstalker_erland - SAY_ON_BYE', 'Try to take better care of yourself, Quinn. You were lucky this time.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000345, 'npc_deathstalker_erland - SAY_QUINN', 'I''ve been better. Ivar the Foul got the better of me...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000344, 'npc_deathstalker_erland - SAY_GREETINGS', 'Hello, Quinn. How are you faring?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000343, 'npc_deathstalker_erland - SAY_MOVE_QUINN', 'If I am excused, then I''d like to check on Quinn...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000342, 'npc_deathstalker_erland - SAY_ANSWER', 'Masses of wolves are to the east, and whoever lived at Malden''s Orchard is gone.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000341, 'npc_deathstalker_erland - SAY_RANE', 'It''s good to see you again, Erland. What is your report?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000340, 'npc_deathstalker_erland - SAY_THANKS', 'We made it! Thanks, $N. I couldn''t have gotten without you.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000339, 'npc_deathstalker_erland - SAY_LAST', 'We''re almost there!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000338, 'npc_deathstalker_erland - SAY_AGGRO_2', 'Beware! I am under attack!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000337, 'npc_deathstalker_erland - SAY_AGGRO_1', 'A $C attacks!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000336, 'npc_deathstalker_erland - SAY_START', 'Be careful, $N. Those wolves like to hide among the trees.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000335, 'npc_deathstalker_erland - SAY_QUESTACCEPT', 'Let''s get to the others, and keep an eye open for those wolves cutside...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000334, 'stillblade - SAY_HEAL', 'Thank you, dear Paladin, you just saved my life.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 10, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000333, NULL, 'Alright, we give up! Don''t hurt us!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000332, NULL, 'Time to meet your maker!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000331, NULL, '2...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000330, NULL, '1...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000329, NULL, 'Now I''m gonna give you to the count of ''3'' to get out of here before I sick the dogs on you.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000328, NULL, 'Time to teach you a lesson in manners, little boy!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000327, 'npc_kservant - WHISP21', 'Good luck stranger, and welcome to Shattrath City.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000326, 'npc_kservant - WHISP20', 'Khadgar should be ready to see you again. Just remember that to serve the Sha''tar you will most likely have to ally with the Aldor or the Scryers. And seeking the favor of one group will cause the others'' dislike.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000325, 'npc_kservant - WHISP19', 'Such is their animosity that they vie for the honor of being sent to assist the naaru there. Each day, that decision is made here by A''dal. The armies gather here to receive A''dal''s blessing before heading to Shadowmoon.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000324, 'npc_kservant - WHISP18', 'There he continues to wage war on Illidan with the assistance of the Aldor and the Scryers. The two factions have not given up on their old feuds, though.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000323, 'npc_kservant - WHISP17', 'The attacks against Shattrath continued, but the city did not fall, as you can see. On the contrary, the naaru known as Xi''ri led a successful incursion into Shadowmoon Valley - Illidan''s doorstep.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000322, 'npc_kservant - WHISP16', 'The Aldor''s most holy temple and its surrounding dwellings lie on the terrace above. As a holy site, only the initiated are welcome inside.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000321, 'npc_kservant - WHISP15', 'Many of the priesthood had been slain by the same magisters who now vowed to serve the naaru. They were not happy to share the city with their former enemies.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000320, 'npc_kservant - WHISP14', 'The Aldor are followers of the Light and forgiveness and redemption are values they understand. However, they found hard to forget the deeds of the blood elves while under Kael''s command.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000319, 'npc_kservant - WHISP13', 'The naaru accepted the defectors, who would become known as the Scryers; their dwelling lies in the platform above. Only those initiated with the Scryers are allowed there.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000318, 'npc_kservant - WHISP12', 'The defection of Voren''thal and his followers was the largest loss ever incurred by Kael''s forces. And these weren''t just any blood elves. Many of the best and brightest amongst Kael''s scholars and magisters had been swayed by Voren''thal''s influence.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000317, 'npc_kservant - WHISP11', 'As the naaru approached him, Voren''thal kneeled before him and uttered the following words: "I''ve seen you in a vision, naaru. My race''s only hope for survival lies with you. My followers and I are here to serve you."', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000316, 'npc_kservant - WHISP10', 'The blood elves laid down their weapons in front of the city''s defenders; their leader, a blood elf elder known as Voren''thal, stormed into the Terrace of Light and demanded to speak to A''dal.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000315, 'npc_kservant - WHISP9', 'As the regiment of blood elves crossed this very bridge, the Aldor''s exarchs and vindicators lined up to defend the Terrace of Light. But then the unexpected happened.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000314, 'npc_kservant - WHISP8', 'It wouldn''t be long, however, before the city came under attack once again. This time, the attack came from Illidan''s armies. A large regiment of blood elves had been sent by Illidan''s ally, Kael''thas Sunstrider, to lay waste to the city.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000313, 'npc_kservant - WHISP7', 'The priesthood, known as the Aldor, quickly regained its strength as word spread that the naaru had returned and reconstruction soon began. The ruined temple is now used as an infirmary for injured refugees.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000312, 'npc_kservant - WHISP6', 'They were drawn to the ruins of Shattrath City where a small remnant of the draenei priesthood conducted its rites inside a ruined temple on this very spot.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000311, 'npc_kservant - WHISP5', 'The Sha''tar, or "born from light" are the naaru that came to Outland to fight the demons of the Burning Legion.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000310, 'npc_kservant - WHISP4', 'Let us go into the Lower City. I will warn you that as one of the only safe havens in Outland, Shattrath has attracted droves of refugees from all wars, current and past.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000309, 'npc_kservant - WHISP3', 'The city was left in ruins and darkness... until the Sha''tar arrived.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000308, 'npc_kservant - WHISP2', 'When the Burning Legion turned the orcs against the draenei, the fiercest battle was fought here. The draenei fought tooth and nail, but in the end the city fell.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000307, 'npc_kservant - WHISP1', 'Shattrath was once the draenei capital of this world. Its name means "dwelling of light."', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000306, 'npc_kservant - SAY1', 'Follow me, stranger. This won''t take long.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000305, 'npc_bessy- SAY_THADELL_2', 'Thank you for bringing back my Bessy, $N. I couldn''t live without her!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000304, 'npc_bessy- SAY_THADELL_1', 'Bessy, is that you?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000303, 'mob_phase_hunter - EMOTE_WEAK', 'is very weak', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000302, 'npc_professor_dabiri - WHISPER_DABIRI', 'Saeed is currently engaged or awaiting orders to engage. You may check directly east of me and see if Saeed is ready for you. If he is not present then he is off fighting another battle. I recommend that you wait for him to return before attacking Dimensius.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000301, 'npc_manaforge_control_console - EMOTE_ABORT', 'Emergency shutdown aborted.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000300, 'npc_manaforge_control_console - EMOTE_COMPLETE', 'Emergency shutdown complete.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000299, 'npc_manaforge_control_console - EMOTE_10', 'Emergency shutdown will complete in ten seconds.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000298, 'npc_manaforge_control_console - EMOTE_30', 'Emergency shutdown will complete in thirty seconds.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000297, 'npc_manaforge_control_console - EMOTE_60', 'Emergency shutdown will complete in one minute.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000296, 'npc_manaforge_control_console - EMOTE_START', 'Warning! Emergency shutdown process initiated by $N. Shutdown will complete in two minutes.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000295, 'LUMP_DEFEAT', 'OK, OK! Lump give up!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000294, 'LUMP_SAY1', 'You taste good with maybe a little salt and pepper.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000293, 'LUMP_SAY0', 'In Nagrand, food hunt ogre!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000292, 'clintar_spirit', 'Lurosa, I am entrusting the Relics of Aviana to $N, who will take them to Morthis Whisperwing. I must return completely to the Emerald Dream now. Do not let $N fail!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000291, 'clintar_spirit', 'We have all three of the relics, but my energy is rapidly fading. We must make our way back to Dreamwarden Lurosa! He will let you know what to do next.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000290, 'clintar_spirit', 'I''ve recovered the second relic. Take a moment to rest, and then we''ll continue to the last reliquary.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000289, 'clintar_spirit', 'That''s the first relic, but there are still two more. Follow me, $N.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000288, 'clintar_spirit', 'Begone from this place!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000287, 'clintar_spirit', 'The Emerald Dream will never be yours!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000286, 'clintar_spirit', 'A shadowy, sinister presence has invader the Emerald Dream. Its power is poised to spill over into our world, $N. We must oppose it! That''s why I cannot accompany you in person.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000285, 'sentry - SAY_CONVERTED_1', 'Objective acquired. Initiating security routines.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000284, 'sentry - SAY_CONVERTED_1', 'Deployment sucessful. Trespassers will be neutralized.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000283, 'crystal - EMOTE', 'releases the last of its energies into the nerarby runestone, succesfully reactivating it.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000282, 'anvilvard - SAY_PR_2', 'What manner of trick is this, $R? If you seek to ambush me, I warn you I will not go down quietly!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000281, 'anvilvard - SAY_PR_1', 'Very well. Let''s see what you have to show me, $N.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000280, 'faulk - SAY_HEAL', 'Thank you, dear Paladin, you just saved my life.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000279, 'shredder - SAY_PROGRESS_8', 'Hmm... I don''t think this blinking red light is a good thing...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000278, 'shredder - SAY_PROGRESS_7', 'That was a close one! Well, let''s get going, it''s still a ways to Ratchet!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000277, 'shredder - SAY_PROGRESS_6', 'Come on, don''t break down on me now!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000276, 'shredder - SAY_PROGRESS_5', 'Looks like we''re out of woods, eh? Wonder what this does...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000275, 'shredder - SAY_MERCENARY_4', 'There''s the stolen shredder! Stop it or Lugwizzle will have our hides!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000274, 'shredder - SAY_PROGRESS_3', 'Okay, I think I''ve got it, now. Follow me, $N!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000273, 'shredder - SAY_PROGRESS_2', 'Arrrgh! This isn''t right!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000272, 'shredder - SAY_PROGRESS_1', 'Alright, alright I think I can figure out how to operate this thing...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000271, 'flathead - SAY_TWIGGY_FLATHEAD_OVER', 'The Affray is over.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000270, 'flathead - SAY_TWIGGY_FLATHEAD_DOWN', 'Challenger is down!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000269, 'flathead - SAY_TWIGGY_FLATHEAD_FRAY', 'You! Enter the fray!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000268, 'flathead - SAY_TWIGGY_FLATHEAD_BEGIN', 'The Affray has begun, get ready to fight!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000267, 'flathead - SAY_BIG_WILL_READY', 'Ready when you are, warrior.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000266, 'geezle - EMOTE_SPARK', 'picks up the naga flag.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000265, 'geezle - GEEZLE_SAY_7', 'Yes, sir. It won''t happen again...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000264, 'geezle - SPARK_SAY_6', 'Our Bloodmyst scouts have located our contact. The fool, Velen, will soon leave himself open and defenseless -- long enough for us to strike! Now get out of my sight before I vaporize you...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000263, 'geezle - SPARK_SAY_5', 'Relax? Do you know what Kael''thas does to those that fail him, Geezle? Eternal suffering and pain... Do NOT screw this up, fool.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000262, 'geezle - GEEZLE_SAY_4', 'Relax, Spark! I have it all under control. We''ll strip mine the Exodar right out from under ''em - making both you and I very, very rich in the process.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000261, 'geezle - SPARK_SAY_3', 'The Master has gone to great lengths to secure information about the whereabouts of the Exodar. You could have blown the entire operation, including the cover of our spy on the inside.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000260, 'geezle - SPARK_SAY_2', 'What''s the big idea? You nearly blew my cover, idiot! I told you to put the compass and navigation maps somewhere safe - not out in the open for any fool to discover.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000259, 'geezle - GEEZLE_SAY_1', 'What''s the big idea, Spark?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000258, 'overgrind - YELL_ATTACK', 'Now I cut you!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000257, 'overgrind - SAY_EMOTE', 'puts the shell to his ear.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000256, 'overgrind - SAY_TEXT', 'Yes Master, all goes along as planned.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000255, 'draenei survivor - HELP4', 'I don''t know if I can make it, please help me...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000254, 'draenei survivor - HELP3', 'Ughhh... I hurt. Can you help me?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000253, 'draenei survivor - HELP2', 'Everything hurts, Please make it stop...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000252, 'draenei survivor - HELP1', 'Oh, the pain...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000251, 'draenei survivor - HEAL4', 'Ugh... what is this place? Is that all that''s left of the ship over there?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000250, 'draenei survivor - HEAL3', '$C You saved me! I owe you a debt that I can never repay. I''ll go see if I can help the others.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000249, 'draenei survivor - HEAL2', '$C, Where am I? Who are you? Oh no! What happened to the ship?.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000248, 'draenei survivor - HEAL1', 'The last thing I remember is the ship falling and us getting into the pods. I''ll go see how I can help. Thank you!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000247, 'SAY_RIZZLE_FINAL', 'All right, you win! I surrender! Just don''t hurt me!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000246, 'SAY_RIZZLE_GRENADE', 'Just chill!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000245, 'SAY_RIZZLE_START', 'You, there! Hand over that moonstone and nobody gets hurt!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000244, 'phizzlethorpe - SAY_PROGRESS_9', 'Doctor Draxlegauge will give you further instructions, $N. Many thanks for your help!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000243, 'phizzlethorpe - EMOTE_PROGRESS_8', 'hands one glowing goggles over to Doctor Draxlegauge.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000242, 'phizzlethorpe - SAY_PROGRESS_7', 'Phew! Glad to be back from that creepy cave.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000241, 'phizzlethorpe - SAY_PROGRESS_6', 'I''ve done it! I have harnessed the power of the stone into the goggles! Let''s get out of here!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000240, 'phizzlethorpe - SAY_PROGRESS_5', 'Almost done! Just a little longer!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000239, 'SAY_AGGRO', 'Help!!! Get these things off me so I can get my work done!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000238, 'phizzlethorpe - EMOTE_PROGRESS_4', 'begins tinkering with the goggles before the stone.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000237, 'phizzlethorpe - SAY_PROGRESS_3', 'I''ll begin drawing energy from the stone. Your job, $N, is to defend me. This place is cursed... trust me.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000236, 'phizzlethorpe - SAY_PROGRESS_2', 'I discovered this cave on our first day here. I believe the energy in the stone can be used to our advantage.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000235, 'phizzlethorpe - SAY_PROGRESS_1', 'Ok, $N. Follow me to the cave where I''ll attempt to harness the power of the rune stone into these goggles.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000234, 'SAY_END', 'Thank you, $N. Please return to my brethren at the Altar of Damnation, near the Hand of Gul''dan, and tell them that Wilda is safe. May the Earthmother watch over you...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000233, 'SAY_PROGRESS6', 'It shouldn''t be much further, $N. The exit is just up ahead.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000232, 'SAY_PROGRESS5', 'The tumultuous nature of the great waterways of Azeroth and Draenor are a direct result of tormented water spirits.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000231, 'SAY_PROGRESS4', 'Lady Vashj must answer for these atrocities. She must be brought to justice!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000230, 'SAY_PROGRESS3', 'Now we must find the exit.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000229, 'SAY_PROGRESS2', 'The naga of Coilskar are exceptionally cruel to their prisoners. It is a miracle that I survived inside that watery prison for as long as I did. Earthmother be praised.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000228, 'SAY_PROGRESS1', 'Grant me protection $N, i must break trough their foul magic!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000227, 'ASSASSIN_SAY_AGGRO2', 'You will never essscape Coilssskarrr...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000226, 'ASSASSIN_SAY_AGGRO1', 'Kill them all!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000225, 'SAY_AGGRO2', 'Naga attackers! Defend yourself!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000224, 'SAY_AGGRO1', 'Watch out!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000223, 'SAY_START', 'I sense the tortured spirits, $N. They are this way, come quickly!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000222, 'SAY_JUST_EATEN', 'Thank you, mortal.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1, 11, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000221, 'YARZILL_THE_MERC_SAY', 'You will not harm the boy, Mor''ghor! Quickly, $N, climb on my back!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 22, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000220, 'LORD_ILLIDAN_SAY_7', 'I expect to see this insect''s carcass in pieces in my lair within the hour. Fail and you will suffer a fate so much worse than death.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000219, 'LORD_ILLIDAN_SAY_6', 'This... whole... operation... HAS BEEN COMPROMISED!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 22, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000218, 'LORD_ILLIDAN_SAY_5', 'You have been deceived, imbecile.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000217, 'LORD_ILLIDAN_SAY_4', 'THIS is your hero?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 6, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000216, 'LORD_ILLIDAN_SAY_3', 'Blathering idiot. You incomprehensibly incompetent buffoon...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000215, 'LORD_ILLIDAN_SAY_2', 'SILENCE!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 22, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000214, 'LORD_ILLIDAN_SAY_1', 'What is the meaning of this, Mor''ghor?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000213, 'OVERLORD_YELL_2', 'All hail Lord Illidan!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 66, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000212, 'OVERLORD_YELL_1', 'Warriors of Dragonmaw, gather ''round! One among you has attained the rank of highlord! Bow your heads in reverence! Show your respect and allegiance to Highlord $N!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 22, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000211, 'OVERLORD_SAY_6', 'So you thought to make a fool of Mor''ghor, eh? Before you are delivered to Lord Illidan, you will feel pain that you could not know to exist. I will take pleasure in exacting my own vengeance.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 25, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000210, 'OVERLORD_SAY_5', 'It will be done, my lord.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000209, 'OVERLORD_SAY_4', 'But... My lord, I do not understand. $N... He is the orc that has...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000208, 'OVERLORD_SAY_3', 'Lord Illidan, this is the Dragonmaw that I, and others, have told you about. He will lead us to victory!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000207, 'OVERLORD_SAY_2', 'Lord Illidan will be here shortly.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000206, 'OVERLORD_SAY_1', 'Come, $N. Lord Stormrage awaits.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000205, NULL, 'A-ME at home!A-ME good!Good A-ME.At home.Home.Home', NULL, NULL, 'A-ME daheim!A-ME gut!Gute A-ME.Daheim.Daheim.Daheim', NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000204, NULL, 'Tar Lord A-ME no hurt.A-ME good.', NULL, NULL, 'Teerfurst A-ME nicht wehtun.A-ME gut.', NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000203, NULL, 'A-Me home.A-ME go home!Tar Lord dont disturb A-ME.', NULL, NULL, 'A-ME daheim.A-ME geht heim!Teerfurst A-ME nicht aufhalten.', NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000202, NULL, 'Good..good,A-ME. Home. Search way.', NULL, NULL, 'Gut...gut.A-ME.A-ME gut.Heim.Heim suchen.', NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000201, NULL, 'Bad Un?Goro Stomper.Stomper evil.', NULL, NULL, 'Boser Stampfer von Un?Goro. Stampfer von Un?Goro bose.', NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000200, NULL, 'A-Me good.Good A-me.Follow...A-ME follow.Home.A-ME go home.', NULL, NULL, 'A-ME gut.Gut A-Me.Folgen...Heim.A-ME geht heim.', NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000163, 'WHISPER_CUSTODIAN_14', 'Our time is at an end $N. I would wish you luck, if such a thing existed.', NULL, 'Notre entretien touche r sa fin, $N. Je vous souhaite bonne chance, si tant est qu''une telle chose a jamais existe.', NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000162, 'WHISPER_CUSTODIAN_13', 'All we know of this timeway is that it leads to Mount Hyjal. The Infinite Dragonflight have gone to great lengths to prevent our involvement. We know next to nothing, mortal. Soridormi is currently attempting to break through the timeway''s defenses but has thus far been unsuccessful. You might be our only hope of breaking through and resolving the conflict.', NULL, 'Tout ce que nous savons sur cette voie temporelle est qu''elle mcne au Mont Hyjal. Le Vol du Dragon Infini s''est donne beaucoup de peine pour empecher notre participation. Soridormi est actuellement en train de tenter de briser les defenses de cette voie, mais ses efforts ont ete vain jusqu''ici. Vous pourriez etre notre seul espoir de les briser et de resoudre le conflit.', NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000161, 'WHISPER_CUSTODIAN_12', 'The timeways are currently ranked in order from least catastrophic to most catastrophic. Note that they are all classified as catastrophic, meaning that any single one of these timeways collapsing would mean that your world would end. We only classify them in such a way so that the heroes and adventurers that are sent here know which timeway best suits their abilities.', NULL, 'Les voies sont pour le moment classees de la moins catastrophique r la plus catastrophique, ce qui veut dire que la moindre de celles-ci qui s''effondre signifierait la fin de votre monde. Nous les classons seulement de cette facon pour que les heros et aventuriers qui y sont envoyes sachent quelle voie est la plus adaptee r leurs competences.', NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000160, 'WHISPER_CUSTODIAN_11', 'This timeway is currently collapsing. What that may hold for the past, present and future is currently unknown...', NULL, 'Cette voie est en train de s''effondrer. Ce qu''elle peut avoir contenu dans le passe, le present et le futur nous est actuellement inconnu...', NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000159, 'WHISPER_CUSTODIAN_10', 'We have very little information on this timeway. Sa''at has been dispatched and is currently inside. The data we have gathered from his correspondence is that the Infinite Dragonflight are once again attempting to alter time. Could it be that the opening of the Dark Portal is being targeted for sabotage? Let us hope not...', NULL, 'Nous avons trcs peu d''informations sur cette voie. Sa''at a ete envoye et est actuellement sur place. Les donnees que nous avons recueillies de sa correspondance est que le Vol du Dragon Infini tente une fois de plus d''alterer le temps. Se pourrait-il que l''ouverture de la Porte des Tencbres soit la cible d''un sabotage. Esperons que non...', NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000158, 'WHISPER_CUSTODIAN_9', 'This timeway is in great disarray! We have agents inside right now attempting to restore order. What information I have indicates that Thrall''s freedom is in jeopardy. A malevolent organization known as the Infinite Dragonflight is trying to prevent his escape. I fear without outside assistance, all will be lost.', NULL, 'Cette voie temporelle est en grand desarroi ! Nous avons actuellement des agents r linterieur qui tentent de restaurer l''ordre. Les informations que j''ai indiquent que la liberte de Thrall est en peril. Une organisation malveillante connue sous le nom de Vol du Dragon Infini essaye d''empecher son evasion. J''ai peur que sans assistance exterieure, tout soit perdu.', NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000157, 'WHISPER_CUSTODIAN_8', 'These tunnels that you see are called timeways. They are infinite in number. The ones that currently exist in your reality are what the master has deemed as ''trouble spots.'' These trouble spots may differ completely in theme but they always share a cause. That is, their existence is a result of the same temporal disturbance. Remember that should you venture inside one...', NULL, 'Ces tunnels que vous voyez sont appeles Voies Temporelles. Leur nombre est infini. Ceux qui existent actuellement dans votre realite sont ceux que le maitre a consideres comme ''points sensibles''. Ces points sensibles peuvent differer complctement de part leur thcme, mais partagent toujours une cause. C''est-r-dire, leur existence est le resultat des memes perturbations temporelles. Rappelez-vous en si vous osez vous aventurer dans l''un d''eux...', NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000156, 'WHISPER_CUSTODIAN_7', 'For normal maintenance of time, the Keepers of Time are sufficient caretakers. We are able to deal with most ordinary disturbances. I speak of little things, such as rogue mages changing something in the past to elevate their status or wealth in the present.', NULL, 'Pour le maintien normal du temps, les Gardiens du temps suffisent r la tache. Nous sommes capables de gerer les perturbations les plus ordinaires. Je parle de petites choses, comme des mages voleurs qui changent quelque chose dans le passe pour elever leur statut ou leur richesse dans le present.', NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000155, 'WHISPER_CUSTODIAN_6', 'As custodians of time, we watch over and care for Nozdormu''s realm. The master is away at the moment, which means that attempts are being made to dramatically alter time. The master never meddles in the affairs of mortals but instead corrects the alterations made to time by others. He is reactionary in this regard.', NULL, 'En tant que protecteurs du temps, nous observons et veillons sur le royaume de Nozdormu. Le maitre est absent en ce moment, ce qui signifie que des tentatives sont faites pour alterer fortement le temps. Le maitre ne se mele jamais des affaires des mortels mais il corrige les alterations faites au temps par d''autres. Il est reactionnaire r cet egard.', NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000154, 'WHISPER_CUSTODIAN_5', 'There are only two truths to be found here: First, that time is chaotic, always in flux, and completely malleable and second, perception does not dictate reality.', NULL, 'Il n''y a que deux verites qui peuvent etre trouvees ici : premicrement, ce temps est chaotique, toujours en mouvement, et complctement malleable, et ensuite, la perception ne determine pas la realite.', NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000153, 'WHISPER_CUSTODIAN_4', 'Follow me, please.', NULL, 'Veuillez me suivre.', NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000152, 'WHISPER_CUSTODIAN_3', 'It is strange, I know... Most mortals cannot actually comprehend what they see here, as often, what they see is not anchored within their own perception of reality.', NULL, 'C''est etrange, je sais... La plupart des mortels ne peuvent pas reellement comprendre ce quils voient ici, comme souvent, ce qu''ils voient n''est pas ancre dans leur propre perception de la realite.', NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000151, 'WHISPER_CUSTODIAN_2', 'We do not know if the Caverns of Time have always been accessible to mortals. Truly, it is impossible to tell as the Timeless One is in perpetual motion, changing our timeways as he sees fit. What you see now may very well not exist tomorrow. You may wake up and have no memory of this place.', NULL, 'Nous ne savons pas si les Grottes du Temps ont toujours ete accessibles aux mortels. Vraiment, c''est impossible r dire etant donne que l''Intemporel est en mouvement perpetuel, changeant nos spirales temporelles comme bon lui semble. Ce que vous voyez aujourd''hui peut trcs bien ne plus exister demain. Vous pourriez vous reveiller sans aucun souvenir de cet endroit.', NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000150, 'WHISPER_CUSTODIAN_1', 'Greetings, $N. I will guide you through the cavern. Please try and keep up.', NULL, 'Salutations, $N. Je vais vous guider r travers cette grotte. Veuillez me suivre.', NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000149, NULL, 'Thanks so much... From here I find the way back to the Whitereach Post. Speak with Motega Firemane, maybe you can persuade him to send me home.', NULL, NULL, 'Tausend Dank...Von hier aus finde ich den Ruckweg zum Weissgipfelposten.Sprecht unbedingt mit Motega Feuermahne,vielleicht konnt ihr ihn uberreden,mich nach Hause zu schicken.', NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000148, NULL, 'This seems to me trusted. If we move to the east, we are  probably... Aah!  Wyvern attack !', NULL, NULL, 'Das kommt mir vertraut vor.Wenn wir nach Osten ziehen,konnen wir wahrscheinlich...Aah!Flugeldrachen im Angriff!', NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000147, NULL, 'Lets go $N. I am ready to start to Whitereach Post.', NULL, NULL, 'Gehen wir $N.Ich bin bereit,mich zum Weissgipfelposten aufzumachen.', NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000146, 'lilatha CAPTAIN_ANSWER', 'Liatha, get someone to look at those injuries. Thank you for bringing her back safely.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000145, 'lilatha SAY_END2', 'Captain Helios, I''ve been rescued from the Amani Catacombs. Reporting for duty, sir!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000144, 'lilatha SAY_END1', 'Thank you for saving my life and bringing me back to safety, $N', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000143, 'lilatha SAY_PROGRESS3', 'There''s Farstrider Enclave now, $C. Not far to go... Look out! Troll ambush!!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000142, 'lilatha SAY_PROGRESS2', 'I can see the light at the end of the tunnel!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000141, 'lilatha SAY_PROGRESS1', '$N, let''s use the antechamber to the right.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000140, 'lilatha SAY_START', 'Let''s go.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000139, 'aeranas SAY_FREE', 'Avruu''s magic is broken! I''m free once again!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000138, 'aeranas SAY_SUMMON', 'Avruu''s magic... it still controls me. You must fight me, mortal. It''s the only way to break the spell!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000137, 'dawnforge COMMANDER_DAWNFORGE_5', 'See to it, Ardonis!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000136, 'dawnforge COMMANDER_DAWNFORGE_4 SAY_ARCANIST_ARDONIS_2', 'Yes, my lord.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000135, 'dawnforge PATHALEON_CULATOR_IMAGE_2_2', 'I am returning to Tempest Keep. See to it that I do not have reason to return!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000134, 'dawnforge PATHALEON_CULATOR_IMAGE_2_1', 'We''ve had too many setbacks along the way: Hellfire Citadel, Fallen Sky Ridge, Firewing Point... Prince Kael''thas will tolerate no further delays. I will tolerate nothing other than complete success!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000133, 'dawnforge PATHALEON_CULATOR_IMAGE_2', 'Duro will be reinforced! Ultris was a complete disaster. I will NOT have that mistake repeated!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000132, 'dawnforge SAY_COMMANDER_DAWNFORGE_3', 'My lord!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000131, 'dawnforge SAY_PATHALEON_CULATOR_IMAGE_1', 'Indeed, it is not a request.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000130, 'dawnforge SAY_COMMANDER_DAWNFORGE_2', 'You will do as ordered. Manaforge Duro has come under heavy attack by mana creatures and the situation is out of control. Failure to comply will not be tolerated!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000129, 'dawnforge SAY_ARCANIST_ARDONIS_1', 'You cannot be serious! We are severely understaffed and can barely keep this manaforge functional!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000128, 'dawnforge SAY_COMMANDER_DAWNFORGE_1', 'We need you to send reinforcements to Manaforge Duro, Ardonis. This is not a request, it''s an order.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000127, 'twiggy SAY_TWIGGY_OVER', 'The Affray is over.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000126, 'twiggy SAY_TWIGGY_DOWN', 'Challenger is down!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000125, 'twiggy SAY_TWIGGY_FRAY', 'You! Enter the fray!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000124, 'twiggy SAY_TWIGGY_BEGIN', 'The Affray has begun. $n, get ready to fight!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000123, 'big will SAY_BIG_WILL_READY', 'Ready when you are, $c.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000122, 'wounded elf SAY_ELF_AGGRO', 'You won''t keep me from getting to Falcon Watch!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000121, 'wounded elf SAY_ELF_COMPLETE', 'Falcon Watch, at last! Now, where''s my... Oh no! My pack, it''s missing! Where has -', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000120, 'wounded elf SAY_ELF_SUMMON2', 'Did you hear something?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000119, 'wounded elf SAY_ELF_RESTING', 'Allow me a moment to rest. The journey taxes what little strength I have.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000118, 'wounded elf SAY_ELF_SUMMON1', 'Over there! They''re following us!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000117, 'wounded elf SAY_ELF_START', 'Thank you for agreeing to help. Now, let''s get out of here $N.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000116, 'magwin EMOTE_HUG', 'hugs her father.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000115, 'magwin SAY_END2', 'You can thank $N for getting me back here safely, father.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000114, 'magwin SAY_END1', 'Father! Father! You''re alive!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000113, 'magwin SAY_PROGRESS', 'My poor family. Everything has been destroyed.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000112, 'magwin SAY_AGGRO', 'Help me!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000111, 'magwin SAY_START', 'Our house is this way, through the thicket.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000110, 'torek SAY_END', 'Go report that the outpost is taken. We will remain here.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000109, 'torek SAY_WIN', 'Silverwing is ours!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000108, 'torek SAY_PREPARE', 'Prepare yourselves. Silverwing is just around the bend.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000107, 'torek SAY_MOVE', 'Ok, Lets move out!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000106, 'torek SAY_READY', 'Everyone ready?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);

UPDATE `script_texts` SET type = 3 WHERE entry = -1544013;
UPDATE `script_texts` SET content_default = '<OregonScript Text Entry Missing!>' WHERE entry = -1000000;

-- Delete Scripttext with entry out of range
DELETE FROM `script_texts` WHERE entry <= -2100000;

-- Spell proc molten armor
DELETE FROM spell_proc_event WHERE entry = 30482;
REPLACE INTO spell_proc_event VALUES (30482, 0, 0, 0, 40, 0, 0, 0, 0);
