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
