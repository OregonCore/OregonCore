-- MySQL dump 10.11
--
-- Host: localhost    Database: world
-- ------------------------------------------------------
-- Server version	5.0.34-log

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `areatrigger_involvedrelation`
--

DROP TABLE IF EXISTS `areatrigger_involvedrelation`;
CREATE TABLE `areatrigger_involvedrelation` (
  `id` mediumint(8) unsigned NOT NULL default '0' COMMENT 'Identifier',
  `quest` mediumint(8) unsigned NOT NULL default '0' COMMENT 'Quest Identifier',
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Trigger System';

--
-- Dumping data for table `areatrigger_involvedrelation`
--

LOCK TABLES `areatrigger_involvedrelation` WRITE;
/*!40000 ALTER TABLE `areatrigger_involvedrelation` DISABLE KEYS */;
/*!40000 ALTER TABLE `areatrigger_involvedrelation` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `areatrigger_scripts`
--

DROP TABLE IF EXISTS `areatrigger_scripts`;
CREATE TABLE `areatrigger_scripts` (
  `entry` mediumint(8) NOT NULL,
  `ScriptName` char(64) NOT NULL,
  PRIMARY KEY  (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `areatrigger_scripts`
--

LOCK TABLES `areatrigger_scripts` WRITE;
/*!40000 ALTER TABLE `areatrigger_scripts` DISABLE KEYS */;
/*!40000 ALTER TABLE `areatrigger_scripts` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `areatrigger_tavern`
--

DROP TABLE IF EXISTS `areatrigger_tavern`;
CREATE TABLE `areatrigger_tavern` (
  `id` mediumint(8) unsigned NOT NULL default '0' COMMENT 'Identifier',
  `name` text,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Trigger System';

--
-- Dumping data for table `areatrigger_tavern`
--

LOCK TABLES `areatrigger_tavern` WRITE;
/*!40000 ALTER TABLE `areatrigger_tavern` DISABLE KEYS */;
/*!40000 ALTER TABLE `areatrigger_tavern` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `areatrigger_teleport`
--

DROP TABLE IF EXISTS `areatrigger_teleport`;
CREATE TABLE `areatrigger_teleport` (
  `id` mediumint(8) unsigned NOT NULL default '0' COMMENT 'Identifier',
  `name` text,
  `access_id` bigint(20) unsigned NOT NULL DEFAULT '0',
  `target_map` smallint(5) unsigned NOT NULL default '0',
  `target_position_x` float NOT NULL default '0',
  `target_position_y` float NOT NULL default '0',
  `target_position_z` float NOT NULL default '0',
  `target_orientation` float NOT NULL default '0',
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Trigger System';

--
-- Dumping data for table `areatrigger_teleport`
--

LOCK TABLES `areatrigger_teleport` WRITE;
/*!40000 ALTER TABLE `areatrigger_teleport` DISABLE KEYS */;
/*!40000 ALTER TABLE `areatrigger_teleport` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `access_requirement`
--

DROP TABLE IF EXISTS `access_requirement`;
CREATE TABLE `access_requirement` (
  `id` bigint(20) unsigned NOT NULL COMMENT 'Identifier',
  `level_min` tinyint(3) unsigned NOT NULL default '0',
  `level_max` tinyint(3) unsigned NOT NULL default '0',
  `item` MEDIUMINT(8) UNSIGNED NOT NULL DEFAULT '0',
  `item2` MEDIUMINT(8) UNSIGNED NOT NULL DEFAULT '0',
  `heroic_key` MEDIUMINT(8) UNSIGNED NOT NULL DEFAULT '0',
  `heroic_key2` MEDIUMINT(8) UNSIGNED NOT NULL DEFAULT '0',
  `quest_done` MEDIUMINT(8) UNSIGNED NOT NULL DEFAULT '0',
  `quest_failed_text` TEXT NULL DEFAULT NULL,
  `heroic_quest_done` MEDIUMINT(8) UNSIGNED NOT NULL DEFAULT '0',
  `heroic_quest_failed_text` TEXT NULL DEFAULT NULL,
  `comment` TEXT NULL DEFAULT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Access Requirements';

--
-- Dumping data for table `access_requirement`
--

LOCK TABLES `access_requirement` WRITE;
/*!40000 ALTER TABLE `access_requirement` DISABLE KEYS */;
/*!40000 ALTER TABLE `access_requirement` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `battleground_template`
--

DROP TABLE IF EXISTS `battleground_template`;
CREATE TABLE `battleground_template` (
  `id` mediumint(8) unsigned NOT NULL,
  `MinPlayersPerTeam` smallint(5) unsigned NOT NULL default '0',
  `MaxPlayersPerTeam` smallint(5) unsigned NOT NULL default '0',
  `MinLvl` tinyint(3) unsigned NOT NULL default '0',
  `MaxLvl` tinyint(3) unsigned NOT NULL default '0',
  `AllianceStartLoc` mediumint(8) unsigned NOT NULL,
  `AllianceStartO` float NOT NULL,
  `HordeStartLoc` mediumint(8) unsigned NOT NULL,
  `HordeStartO` float NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `battleground_template`
--

LOCK TABLES `battleground_template` WRITE;
/*!40000 ALTER TABLE `battleground_template` DISABLE KEYS */;
INSERT INTO `battleground_template` VALUES
(1,20,40,51,70,611,2.72532,610,2.27452),
(2,5,10,10,70,769,3.14159,770,3.14159),
(3,8,15,20,70,890,3.40156,889,0.263892),
(4,0,2,10,70,929,0,936,3.14159),
(5,0,2,10,70,939,0,940,3.14159),
(6,0,2,10,70,0,0,0,0),
(7,8,15,61,70,1103,3.40156,1104,0.263892),
(8,0,2,10,70,1258,0,1259,3.14159);
/*!40000 ALTER TABLE `battleground_template` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `battlemaster_entry`
--

DROP TABLE IF EXISTS `battlemaster_entry`;
CREATE TABLE `battlemaster_entry` (
  `entry` mediumint(8) unsigned NOT NULL default '0' COMMENT 'Entry of a creature',
  `bg_template` mediumint(8) unsigned NOT NULL default '0' COMMENT 'Battleground template id',
  PRIMARY KEY  (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `battlemaster_entry`
--

LOCK TABLES `battlemaster_entry` WRITE;
/*!40000 ALTER TABLE `battlemaster_entry` DISABLE KEYS */;
/*!40000 ALTER TABLE `battlemaster_entry` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `command`
--

DROP TABLE IF EXISTS `command`;
CREATE TABLE `command` (
  `name` varchar(50) NOT NULL default '',
  `security` tinyint(3) unsigned NOT NULL default '0',
  `help` longtext,
  PRIMARY KEY  (`name`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Chat System';

--
-- Dumping data for table `command`
--

LOCK TABLES `command` WRITE;
/*!40000 ALTER TABLE `command` DISABLE KEYS */;
INSERT INTO `command` VALUES
('.ticket closedlist',1,'Displays a list of closed GM tickets.'),
('account',0,'Syntax: .account\r\n\r\nDisplay the access level of your account.'),
('account create',4,'Syntax: .account create $account $password\r\n\r\nCreate account and set password to it.'),
('account delete',4,'Syntax: .account delete $account\r\n\r\nDelete account with all characters.'),
('account lock',0,'Syntax: .account lock [on|off]\r\n\r\nAllow login from account only from current used IP or remove this requirement.'),
('account onlinelist',4,'Syntax: .account onlinelist\r\n\r\nShow list of online accounts.'),
('account password',0,'Syntax: .account password $old_password $new_password $new_password\r\n\r\nChange your account password.'),
('account set addon',3,'Syntax: .account set addon [$account] #addon\r\n\r\nSet user (posible targeted) expansion addon level allowed. Addon values: 0 - normal,1 - tbc,2 - wotlk.'),
('account set gmlevel',4,'Syntax: .account set gmlevel [$account] #level\r\n\r\nSet the security level for targeted player (can''t be used at self) or for account $name to a level of #level.\r\n\r\n#level may range from 0 to 3.'),
('account set password',4,'Syntax: .account set password $account $password $password\r\n\r\nSet password for account.'),
('additem',3,'Syntax: .additem #itemid/[#itemname]/#shift-click-item-link #itemcount\r\n\r\nAdds the specified number of items of id #itemid (or exact (!) name $itemname in brackets,or link created by shift-click at item in inventory or recipe) to your or selected character inventory. If #itemcount is omitted,only one item will be added.\r\n.'),
('additemset',3,'Syntax: .additemset #itemsetid\r\n\r\nAdd items from itemset of id #itemsetid to your or selected character inventory. Will add by one example each item from itemset.'),
('addmove',2,'Syntax: .addmove #creature_guid [#waittime]\r\n\r\nAdd your current location as a waypoint for creature with guid #creature_guid. And optional add wait time.'),
('announce',1,'Syntax: .announce $MessageToBroadcast\r\n\r\nSend a global message to all players online in chat log.'),
('aura',3,'Syntax: .aura #spellid\r\n\r\nAdd the aura from spell #spellid to the selected Unit.'),
('ban account',3,'Syntax is: ban account $Name $bantime $reason\r\nBan account kick player.\r\n$bantime: negative value leads to permban,otherwise use a timestring like "4d20h3s".'),
('ban character',3,'Syntax is: ban character $Name $bantime $reason\r\nBan account and kick player.\r\n$bantime: negative value leads to permban,otherwise use a timestring like "4d20h3s".'),
('ban ip',3,'Syntax is: ban ip $Ip $bantime $reason\r\nBan IP.\r\n$bantime: negative value leads to permban,otherwise use a timestring like "4d20h3s".'),
('baninfo account',3,'Syntax is: baninfo account\r\nWatch full information about a specific ban.'),
('baninfo character',3,'Syntax is: baninfo character\r\nWatch full information about a specific ban.'),
('baninfo ip',3,'Syntax is: baninfo ip\r\nWatch full information about a specific ban.'),
('bank',3,'Syntax: .bank\r\n\r\nShow your bank inventory.'),
('banlist account',3,'Syntax is: banlist account [$Name]\r\nSearches the banlist for a account name pattern or show full list account bans.'),
('banlist character',3,'Syntax is: banlist character $Name\r\nSearches the banlist for a character name pattern. Pattern required.'),
('banlist ip',3,'Syntax is: banlist ip [$Ip]\r\nSearches the banlist for a IP pattern or show full list of IP bans.'),
('bindsight',3,'Syntax: .bindsight\r\n\r\nBinds vision to the selected unit indefinitely. Cannot be used while currently possessing a target.'),
('cast',3,'Syntax: .cast #spellid [triggered]\r\n  Cast #spellid to selected target. If no target selected cast to self. If ''trigered'' or part provided then spell casted with triggered flag.'),
('cast back',3,'Syntax: .cast back #spellid [triggered]\r\n  Selected target will cast #spellid to your character. If ''trigered'' or part provided then spell casted with triggered flag.'),
('cast dist',3,'Syntax: .cast dist #spellid [#dist [triggered]]\r\n  You will cast spell to pint at distance #dist. If ''trigered'' or part provided then spell casted with triggered flag. Not all spells can be casted as area spells.'),
('cast self',3,'Syntax: .cast self #spellid [triggered]\r\nCast #spellid by target at target itself. If ''trigered'' or part provided then spell casted with triggered flag.'),
('cast target',3,'Syntax: .cast target #spellid [triggered]\r\n  Selected target will cast #spellid to his victim. If ''trigered'' or part provided then spell casted with triggered flag.'),
('character delete',4,'Syntax: .character delete $name\r\n\r\nDelete character $name.'),
('character rename',2,'Syntax: .character rename [$name]\r\n\r\nMark selected in game or by $name in command character for rename at next login.'),
('combatstop',2,'Syntax: .combatstop [$playername]\r\nStop combat for selected character. If selected non-player then command applied to self. If $playername provided then attempt applied to online player $playername.'),
('commands',0,'Syntax: .commands\r\n\r\nDisplay a list of available commands for your account level.'),
('cooldown',3,'Syntax: .cooldown [#spell_id]\r\n\r\nRemove all (if spell_id not provided) or #spel_id spell cooldown from selected character or you (if no selection).'),
('damage',3,'Syntax: .damage $damage_amount [$school [$spellid]]\r\n\r\nApply $damage to target. If not $school and $spellid provided then this flat clean melee damage without any modifiers. If $school provided then damage modified by armor reduction (if school physical),and target absorbing modifiers and result applied as melee damage to target. If spell provided then damage modified and applied as spell damage. $spellid can be shift-link.'),
('debug anim',2,'Syntax: .debug anim #emoteid\r\n\r\nPlay emote #emoteid for your character.'),
('debug arena',3,'Syntax: .debug arena\r\n\r\n Toggles arena 1v1 or normal mode.'),
('debug getvalue',3,'Syntax: .debug getvalue #field #isInt\r\n\r\nGet the field #field of the selected creature. If no creature is selected,get the content of your field.\r\n\r\nUse a #isInt of value 1 if the expected field content is an integer.'),
('debug hostilrefs',3,'Syntax: .debug hostilrefs\n\n Prints out the hostil refs of the targetted unit.'),
('debug playsound',1,'Syntax: .debug playsound #soundid\r\n\r\nPlay sound with #soundid.\r\nSound will be play only for you. Other players do not hear this.\r\nWarning: client may have more 5000 sounds...'),
('debug setvalue',3,'Syntax: .debug setvalue #field #value #isInt\r\n\r\nSet the field #field of the selected creature with value #value. If no creature is selected,set the content of your field.\r\n\r\nUse a #isInt of value 1 if #value is an integer.'),
('debug standstate',2,'Syntax: .debug standstate #emoteid\r\n\r\nChange the emote of your character while standing to #emoteid.'),
('debug threatlist',3,'Syntax: .debug threatlist\n\n Prints out the current threatlist of the targetted creature.'),
('debug update',3,'Syntax: .debug update #field #value\r\n\r\nUpdate the field #field of the selected character or creature with value #value.\r\n\r\nIf no #value is provided,display the content of field #field.'),
('demorph',2,'Syntax: .demorph\r\n\r\nDemorph the selected player.'),
('die',3,'Syntax: .die\r\n\r\nKill the selected player. If no player is selected,it will kill you.'),
('dismount',0,'Syntax: .dismount\r\n\r\nDismount you,if you are mounted.'),
('distance',3,'Syntax: .distance\r\n\r\nDisplay the distance from your character to the selected creature.'),
('event',2,'Syntax: .event #event_id\r\nShow details about event with #event_id.'),
('event activelist',2,'Syntax: .event activelist\r\nShow list of currently active events.'),
('event start',2,'Syntax: .event start #event_id\r\nStart event #event_id. Set start time for event to current moment (change not saved in DB).'),
('event stop',2,'Syntax: .event stop #event_id\r\nStop event #event_id. Set start time for event to time in past that make current moment is event stop time (change not saved in DB).'),
('explorecheat',3,'Syntax: .explorecheat #flag\r\n\r\nReveal  or hide all maps for the selected player. If no player is selected,hide or reveal maps to you.\r\n\r\nUse a #flag of value 1 to reveal,use a #flag value of 0 to hide all maps.'),
('flusharenapoints',3,'Syntax: .flusharenapoints\r\n\r\nUse it to distribute arena points based on arena team ratings,and start a new week.'),
('freeze',2,'Syntax: .freeze (#player)\r\n"Freezes" #player and disables his chat. When using this without #name it will freeze your target.'),
('gm',1,'Syntax: .gm [on/off]\r\n\r\nEnable or Disable in game GM MODE or show current state of on/off not provided.'),
('gm chat',1,'Syntax: .gm chat [on/off]\r\n\r\nEnable or disable chat GM MODE (show gm badge in messages) or show current state of on/off not provided.'),
('gm fly',3,'Syntax: .gm fly on/off\r\nEnable/disable gm fly mode.'),
('gm ingame',0,'Syntax: .gm ingame\r\n\r\nDisplay a list of available in game Game Masters.'),
('gm list',3,'Syntax: .gm list\r\n\r\nDisplay a list of all Game Masters accounts and security levels.'),
('gm online',0,'Syntax: .gm online\r\n\r\nDisplay a list of available Game Masters.'),
('gm visible',1,'Syntax: .gm visible on/off\r\n\r\nOutput current visibility state or make GM visible(on) and invisible(off) for other players.'),
('gmannounce',4,'Syntax: .gmannounce $announcement\r\nSend an announcement to online Gamemasters.'),
('gmnameannounce',2,'Syntax: .gmnameannounce $announcement.\r\nSend an announcement to all online GM''s,displaying the name of the sender.'),
('gmnotify',2,'Syntax: .gmnotify $notification\r\nDisplays a notification on the screen of all online GM''s.'),
('go creature',2,'Syntax: .go creature #creature_guid\r\nTeleport your character to creature with guid #creature_guid.\r\n.gocreature #creature_name\r\nTeleport your character to creature with this name.\r\n.gocreature id #creature_id\r\nTeleport your character to a creature that was spawned from the template with this entry.\r\n*If* more than one creature is found,then you are teleported to the first that is found inside the database.'),
('go graveyard',2,'Syntax: .go graveyard #graveyardId\r\n Teleport to graveyard with the graveyardId specified.'),
('go grid',1,'Syntax: .go grid #gridX #gridY [#mapId]\r\n\r\nTeleport the gm to center of grid with provided indexes at map #mapId (or current map if it not provided).'),
('go object',1,'Syntax: .go object #object_guid\r\nTeleport your character to gameobject with guid #object_guid'),
('go ticket',1,'Syntax: .go ticket #ticketid\r\nTeleports the user to the location where $ticketid was created.'),
('go trigger',2,'Syntax: .go trigger #trigger_id\r\n\r\nTeleport your character to areatrigger with id #trigger_id. Character will be teleported to trigger target if selected areatrigger is telporting trigger.'),
('go xy',1,'Syntax: .go xy #x #y [#mapid]\r\n\r\nTeleport player to point with (#x,#y) coordinates at ground(water) level at map #mapid or same map if #mapid not provided.'),
('go xyz',1,'Syntax: .go xyz #x #y #z [#mapid]\r\n\r\nTeleport player to point with (#x,#y,#z) coordinates at ground(water) level at map #mapid or same map if #mapid not provided.'),
('go zonexy',1,'Syntax: .go zonexy #x #y [#zone]\r\n\r\nTeleport player to point with (#x,#y) client coordinates at ground(water) level in zone #zoneid or current zone if #zoneid not provided. You can look up zone using .lookup area $namepart'),
('gobject activate',2,'Syntax: .gobject activate #guid\r\n\r\nActivates an object like a door or a button.'),
('gobject add',2,'Syntax: .gobject add #id <spawntimeSecs>\r\n\r\nAdd a game object from game object templates to the world at your current location using the #id.\r\nspawntimesecs sets the spawntime,it is optional.\r\n\r\nNote: this is a copy of .gameobject.'),
('gobject addtemp',2,'Adds a temporary gameobject that is not saved to DB.'),
('gobject delete',2,'Syntax: .gobject delete #go_guid\r\nDelete gameobject with guid #go_guid.'),
('gobject move',2,'Syntax: .gobject move #goguid [#x #y #z]\r\n\r\nMove gameobject #goguid to character coordinates (or to (#x,#y,#z) coordinates if its provide).'),
('gobject near ',3,'Syntax: .gobject near  [#distance]\r\n\r\nOutput gameobjects at distance #distance from player. Output gameobject guids and coordinates sorted by distance from character. If #distance not provided use 10 as default value.'),
('gobject target',2,'Syntax: .gobject target [#go_id|#go_name_part]\r\n\r\nLocate and show position nearest gameobject. If #go_id or #go_name_part provide then locate and show position of nearest gameobject with gameobject template id #go_id or name included #go_name_part as part.'),
('gobject turn',2,'Syntax: .gobject turn #goguid \r\n\r\nSet for gameobject #goguid orientation same as current character orientation.'),
('goname',1,'Syntax: .goname $charactername\r\n\r\nTeleport to the given character. Either specify the character name or click on the character''s portrait,e.g. when you are in a group.'),
('gps',1,'Syntax: .gps\r\n\r\nDisplay the position information for a selected character or creature. Position information includes X,Y,Z,and orientation,map Id and zone Id'),
('groupgo',1,'Syntax: .groupgo $charactername\r\n\r\nTeleport the given character and his group to you.'),
('guid',2,'Syntax: .guid\r\n\r\nDisplay the GUID for the selected character.'),
('guild create',2,'Syntax: .guild create $GuildLeaderName $GuildName\r\n\r\nCreate a guild named $GuildName with the player $GuildLeaderName as leader.'),
('guild delete',2,'Syntax: .guild delete $GuildName\r\n\r\nDelete guild $GuildName.'),
('guild invite',2,'Syntax: .guild invite $CharacterName $GuildName\r\n\r\nAdd $CharacterName into a guild $GuildName.'),
('guild rank',2,'Syntax: .guild rank $CharacterName #Rank\r\n\r\nSet for $CharacterName rank #Rank in a guild.'),
('guild uninvite',2,'Syntax: .guild uninvite $CharacterName\r\n\r\nRemove $CharacterName from a guild.'),
('help',0,'Syntax: .help [$command]\r\n\r\nDisplay usage instructions for the given $command. If no $command provided show list available commands.'),
('hidearea',3,'Syntax: .hidearea #areaid\r\n\r\nHide the area of #areaid to the selected character. If no character is selected,hide this area to you.'),
('honor add',2,'Syntax: .honor add $amount\r\n\r\nAdd a certain amount of honor (gained today) to the selected player.'),
('honor addkill',2,'Syntax: .honor addkikll\r\n\r\nAdd the targeted unit as one of your pvp kills today (you only get honor if it''s a racial leader or a player)'),
('honor update',2,'Syntax: .honor update\r\n\r\nForce the yesterday''s honor fields to be updated with today''s data,which will get reset for the selected player.'),
('hover',3,'Syntax: .hover #flag\r\n\r\nEnable or disable hover mode for your character.\r\n\r\nUse a #flag of value 1 to enable,use a #flag value of 0 to disable hover.'),
('instance listbinds',3,'Syntax: .instance listbinds\r\n  Lists the binds of the selected player.'),
('instance savedata',3,'Syntax: .instance savedata\r\n  Save the InstanceData for the current player''s map to the DB.'),
('instance stats',3,'Syntax: .instance stats\r\n  Shows statistics about instances.'),
('instance unbind',3,'Syntax: .instance unbind all\r\n  All of the selected player''s binds will be cleared.'),
('itemmove',2,'Syntax: .itemmove #sourceslotid #destinationslotid\r\n\r\nMove an item from slots #sourceslotid to #destinationslotid in your inventory\r\n\r\nNot yet implemented'),
('kick',2,'Syntax: .kick [$charactername] [$reason]\r\n\r\nKick the given character name from the world with or without reason. If no character name is provided then the selected player (except for yourself) will be kicked. If no reason is provided,default is "No Reason".'),
('learn',3,'Syntax: .learn #parameter\r\n\r\nSelected character learn a spell of id #parameter.'),
('learn all',3,'Syntax: .learn all\r\n\r\nLearn all big set different spell maybe useful for Administaror.'),
('learn all_crafts',2,'Syntax: .learn crafts\r\n\r\nLearn all professions and recipes.'),
('learn all_default',1,'Syntax: .learn all_default [$playername]\r\n\r\nLearn for selected/$playername player all default spells for his race/class and spells rewarded by completed quests.'),
('learn all_gm',2,'Syntax: .learn all_gm\r\n\r\nLearn all default spells for Game Masters.'),
('learn all_lang',1,'Syntax: .learn all_lang\r\n\r\nLearn all languages'),
('learn all_myclass',3,'Syntax: .learn all_myclass\r\n\r\nLearn all spells and talents available for his class.'),
('learn all_myspells',3,'Syntax: .learn all_myspells\r\n\r\nLearn all spells (except talents and spells with first rank learned as talent) available for his class.'),
('learn all_mytalents',3,'Syntax: .learn all_mytalents\r\n\r\nLearn all talents (and spells with first rank learned as talent) available for his class.'),
('learn all_recipes',2,'Syntax: .learn all_recipes [$profession]\r\rLearns all recipes of specified profession and sets skill level to max.\rExample: .learn all_recipes enchanting'),
('levelup',3,'Syntax: .levelup [$playername] [#numberoflevels]\r\n\r\nIncrease/decrease the level of character with $playername (or the selected if not name provided) by #numberoflevels Or +1 if no #numberoflevels provided). If #numberoflevels is omitted,the level will be increase by 1. If #numberoflevels is 0,the same level will be restarted. If no character is selected and name not provided,increase your level. Command can be used for offline character. All stats and dependent values recalculated. At level decrease talents can be reset if need. Also at level decrease equipped items with greater level requirement can be lost.'),
('linkgrave',3,'Syntax: .linkgrave #graveyard_id [alliance|horde]\r\n\r\nLink current zone to graveyard for any (or alliance/horde faction ghosts). This let character ghost from zone teleport to graveyard after die if graveyard is nearest from linked to zone and accept ghost of this faction. Add only single graveyard at another map and only if no graveyards linked (or planned linked at same map).'),
('list creature',3,'Syntax: .list creature #creature_id [#max_count]\r\n\r\nOutput creatures with creature id #creature_id found in world. Output creature guids and coordinates sorted by distance from character. Will be output maximum #max_count creatures. If #max_count not provided use 10 as default value.'),
('list item',3,'Syntax: .list item #item_id [#max_count]\r\n\r\nOutput items with item id #item_id found in all character inventories,mails,auctions,and guild banks. Output item guids,item owner guid,owner account and owner name (guild name and guid in case guild bank). Will be output maximum #max_count items. If #max_count not provided use 10 as default value.'),
('list object',3,'Syntax: .list object #gameobject_id [#max_count]\r\n\r\nOutput gameobjects with gameobject id #gameobject_id found in world. Output gameobject guids and coordinates sorted by distance from character. Will be output maximum #max_count gameobject. If #max_count not provided use 10 as default value.'),
('listfreeze',1,'Syntax: .listfreeze\r\n\r\nSearch and output all frozen players.'),
('loadscripts',3,'Syntax: .loadscripts $scriptlibraryname\r\n\r\nUnload current and load the script library $scriptlibraryname or reload current if $scriptlibraryname omitted,in case you changed it while the server was running.'),
('lookup area',1,'Syntax: .lookup area $namepart\r\n\r\nLooks up an area by $namepart,and returns all matches with their area ID''s.'),
('lookup creature',3,'Syntax: .lookup creature $namepart\r\n\r\nLooks up a creature by $namepart,and returns all matches with their creature ID''s.'),
('lookup event',2,'Syntax: .lookup event $name\r\nAttempts to find the ID of the event with the provided $name.'),
('lookup faction',3,'Syntax: .lookup faction $name\r\nAttempts to find the ID of the faction with the provided $name.'),
('lookup item',3,'Syntax: .lookup item $itemname\r\n\r\nLooks up an item by $itemname,and returns all matches with their Item ID''s.'),
('lookup itemset',3,'Syntax: .lookup itemset $itemname\r\n\r\nLooks up an item set by $itemname,and returns all matches with their Item set ID''s.'),
('lookup object',3,'Syntax: .lookup object $objname\r\n\r\nLooks up an gameobject by $objname,and returns all matches with their Gameobject ID''s.'),
('lookup player account',2,'Syntax : .lookup player account $account ($limit) \r\n\r\n Searchs players,which account username is $account with optional parametr $limit of results.'),
('lookup player email',2,'Syntax : .lookup player email $email ($limit) \r\n\r\n Searchs players,which account email is $email with optional parametr $limit of results.'),
('lookup player ip',2,'Syntax : .lookup player ip $ip ($limit) \r\n\r\n Searchs players,which account ast_ip is $ip with optional parametr $limit of results.'),
('lookup quest',3,'Syntax: .lookup quest $namepart\r\n\r\nLooks up a quest by $namepart,and returns all matches with their quest ID''s.'),
('lookup skill',3,'Syntax: .lookup skill $$namepart\r\n\r\nLooks up a skill by $namepart,and returns all matches with their skill ID''s.'),
('lookup spell',3,'Syntax: .lookup spell $namepart\r\n\r\nLooks up a spell by $namepart,and returns all matches with their spell ID''s.'),
('lookup tele',1,'Syntax: .lookup tele $substring\r\n\r\nSearch and output all .tele command locations with provide $substring in name.'),
('maxskill',3,'Syntax: .maxskill\r\nSets all skills of the targeted player to their maximum values for its current level.'),
('Mod32Value',3,'Syntax: .Mod32Value #field #value\r\n\r\nAdd #value to field #field of your character.'),
('modify arena',3,'Syntax: .modify arena #value\r\nAdd $amount arena points to the selected player.'),
('modify aspeed',1,'Syntax: .modify aspeed #rate\r\n\r\nModify all speeds -run,swim,run back,swim back- of the selected player to "normalbase speed for this move type"*rate. If no player is selected,modify your speed.\r\n\r\n #rate may range from 0.1 to 10.'),
('modify bit',1,'Syntax: .modify bit #field #bit\r\n\r\nToggle the #bit bit of the #field field for the selected player. If no player is selected,modify your character.'),
('modify bwalk',1,'Syntax: .modify bwalk #rate\r\n\r\nModify the speed of the selected player while running backwards to "normal walk back speed"*rate. If no player is selected,modify your speed.\r\n\r\n #rate may range from 0.1 to 10.'),
('modify drunk',1,'Syntax: .modify drunk #value\r\n Set drunk level to #value (0..100). Value 0 remove drunk state,100 is max drunked state.'),
('modify energy',1,'Syntax: .modify energy #energy\r\n\r\nModify the energy of the selected player. If no player is selected,modify your energy.'),
('modify faction',1,'Syntax: .modify faction #factionid #flagid #npcflagid #dynamicflagid\r\n\r\nModify the faction and flags of the selected creature. Without arguments,display the faction and flags of the selected creature.'),
('modify gender',2,'Syntax: .modify gender male/female\r\n\r\nChange gender of selected player.'),
('modify honor',1,'Syntax: .modify honor $amount\r\n\r\nAdd $amount honor points to the selected player.'),
('modify hp',1,'Syntax: .modify hp #newhp\r\n\r\nModify the hp of the selected player. If no player is selected,modify your hp.'),
('modify mana',1,'Syntax: .modify mana #newmana\r\n\r\nModify the mana of the selected player. If no player is selected,modify your mana.'),
('modify money',1,'Syntax:\r\n.modify money #money\r\n.money #money\r\n\r\nAdd or remove money to the selected player. If no player is selected,modify your money.\r\n\r\n #gold can be negative to remove money.'),
('modify morph',2,'Syntax: .modify morph #displayid\r\n\r\nChange your current model id to #displayid.'),
('modify mount',1,'Syntax:\r\n.modify mount #id #speed\r\nDisplay selected player as mounted at #id creature and set speed to #speed value.'),
('modify rage',1,'Syntax: .modify rage #newrage\r\n\r\nModify the rage of the selected player. If no player is selected,modify your rage.'),
('modify rep',2,'Syntax: .modify rep #repId (#repvalue | $rankname [#delta])\r\nSets the selected players reputation with faction #repId to #repvalue or to $reprank.\r\nIf the reputation rank name is provided,the resulting reputation will be the lowest reputation for that rank plus the delta amount,if specified.\r\nYou can use ''.pinfo rep'' to list all known reputation ids,or use ''.lookup faction $name'' to locate a specific faction id.'),
('modify scale',1,'Syntax: .modify scale #scale\r\n\r\nChange model scale for targeted player(until re-login),or creature(until respawn).'),
('modify speed',1,'Syntax:\r\n.modify speed #rate\r\n.speed #rate\r\n\r\nModify the running speed of the selected player to "normal base run speed"*rate. If no player is selected,modify your speed.\r\n\r\n #rate may range from 0.1 to 10.'),
('modify spell',1,''),
('modify swim',1,'Syntax: .modify swim #rate\r\n\r\nModify the swim speed of the selected player to "normal swim speed"*rate. If no player is selected,modify your speed.\r\n\r\n #rate may range from 0.1 to 10.'),
('modify titles',1,'Syntax:\n.modify titles #mask\n.titles #mask\n\nAllows user to use all titles from #mask.\n\n #mask=0 disables the title-choose-field'),
('movegens',3,'Syntax: .movegens\r\n  Show movement generators stack for selected creature or player.'),
('mute',1,'Syntax: .mute [$playerName] $timeInMinutes [$reason]\r\n\r\nDisible chat messaging for any character from account of character $playerName (or currently selected) at $timeInMinutes minutes. Player can be offline.'),
('namego',1,'Syntax: .namego $charactername\r\n\r\nTeleport the given character to you.'),
('neargrave',3,'Syntax: .neargrave [alliance|horde]\r\n\r\nFind nearest graveyard linked to zone (or only nearest from accepts alliance or horde faction ghosts).'),
('notify',1,'Syntax: .notify $MessageToBroadcast\r\n\r\nSend a global message to all players online in screen.'),
('npc add',2,'Syntax: .npc add #creatureid\r\n\r\nSpawn a creature by the given template id of #creatureid.'),
('npc additem',2,'Syntax: .npc additem #itemId <#maxcount><#incrtime><#extendedcost>r\r\n\r\nAdd item #itemid to item list of selected vendor. Also optionally set max count item in vendor item list and time to item count restoring and items ExtendedCost.'),
('npc addtemp',2,'Adds temporary NPC,not saved to database.'),
('npc addweapon',3,'Not yet implemented.'),
('npc allowmove',3,'Syntax: .npc allowmove\r\n\r\nEnable or disable movement for the selected creature.'),
('npc changelevel',2,'Syntax: .npc changelevel #level\r\n\r\nChange the level of the selected creature to #level.\r\n\r\n#level may range from 1 to 63.'),
('npc delete',2,'Syntax: .npc delete [#guid]\r\n\r\nDelete creature with guid #guid (or the selected if no guid is provided)'),
('npc delitem',2,'Syntax: .npc delitem #itemId\r\n\r\nRemove item #itemid from item list of selected vendor.'),
('npc factionid',2,'Syntax: .npc factionid #factionid\r\n\r\nSet the faction of the selected creature to #factionid.'),
('npc flag',2,'Syntax: .npc flag #npcflag\r\n\r\nSet the NPC flags of creature template of the selected creature and selected creature to #npcflag. NPC flags will applied to all creatures of selected creature template after server restart or grid unload/load.'),
('npc follow',2,'Syntax: .npc follow\r\n\r\nSelected creature start follow you until death/fight/etc.'),
('npc info',3,'Syntax: .npc info\r\n\r\nDisplay a list of details for the selected creature.\r\n\r\nThe list includes:\r\n- GUID,Faction,NPC flags,Entry ID,Model ID,\r\n- Level,\r\n- Health (current/maximum),\r\n\r\n- Field flags,dynamic flags,faction template,\r\n- Position information,\r\n- and the creature type,e.g. if the creature is a vendor.'),
('npc move',2,'Syntax: .npc move [#creature_guid]\r\n\r\nMove the targeted creature spawn point to your coordinates.'),
('npc name',2,'Syntax: .npc name $name\r\n\r\nChange the name of the selected creature or character to $name.\r\n\r\nCommand disabled.'),
('npc playemote',3,'Syntax: .npc playemote #emoteid\r\n\r\nMake the selected creature emote with an emote of id #emoteid.'),
('npc setdeathstate',2,'Syntax: .npc setdeathstate on/off\r\n\r\nSet default death state (dead/alive) for npc at spawn.'),
('npc setmodel',2,'Syntax: .npc setmodel #displayid\r\n\r\nChange the model id of the selected creature to #displayid.'),
('npc setmovetype',2,'Syntax: .npc setmovetype [#creature_guid] stay/random/way [NODEL]\r\n\r\nSet for creature pointed by #creature_guid (or selected if #creature_guid not provided) movement type and move it to respawn position (if creature alive). Any existing waypoints for creature will be removed from the database if you do not use NODEL. If the creature is dead then movement type will applied at creature respawn.\r\nMake sure you use NODEL,if you want to keep the waypoints.'),
('npc spawndist',2,'Syntax: .npc spawndist #dist\r\n\r\nAdjust spawndistance of selected creature to dist.'),
('npc spawntime',2,'Syntax: .npc spawntime #time \r\n\r\nAdjust spawntime of selected creature to time.'),
('npc subname',2,'Syntax: .npc subname $Name\r\n\r\nChange the subname of the selected creature or player to $Name.\r\n\r\nCommand disabled.'),
('npc textemote',3,'Syntax: .npc textemote #emoteid\r\n\r\nMake the selected creature to do textemote with an emote of id #emoteid.'),
('npc unfollow',2,'Syntax: .npc unfollow\r\n\r\nSelected creature (non pet) stop follow you.'),
('npc whisper',1,'Syntax: .npc whisper #playerguid #text\r\nMake the selected npc whisper #text to  #playerguid.'),
('pdump load',3,'Syntax is: pdump load $filename $account [$newname] [$newguid]\r\nLoad character dump from dump file into character list of $account with saved or $newname,with saved (or first free) or $newguid guid.'),
('pdump write',3,'Syntax is: pdump write $filename $playerNameOrGUID\r\nWrite character dump with name/guid $playerNameOrGUID to file $filename.'),
('pet create',2,'Syntax: .pet create\r\n\r\nCreates a pet of the selected creature.'),
('pet learn',2,'Syntax: .pet learn\r\n\r\nLearn #spellid to pet.'),
('pet tp',2,'Syntax: .pet tp #\r\n\r\nChange pet''s amount of training points.'),
('pet unlearn',2,'Syntax: .pet unlean\r\n\r\nunLearn #spellid to pet.'),
('pinfo',2,'Syntax: .pinfo [$player_name] [rep]\r\n\r\nOutput account information for selected player or player find by $player_name. If "rep" parameter provided show reputation information for player.'),
('playall',2,'Syntax: .playall #soundid\r\n\r\nPlayer a sound to whole server.'),
('possess',3,'Syntax: .possess\r\n\r\nPossesses indefinitely the selected creature.'),
('quest add',3,'Syntax: .quest add #quest_id\r\n\r\nAdd to character quest log quest #quest_id. Quest started from item can''t be added by this command but correct .additem call provided in command output.'),
('quest complete',3,'Syntax: .quest complete #questid\r\nMark all quest objectives as completed for target character active quest. After this target character can go and get quest reward.'),
('quest remove',3,'Syntax: .quest remove #quest_id\r\n\r\nSet quest #quest_id state to not completed and not active (and remove from active quest list) for selected player.'),
('recall',1,'Syntax: .recall [$playername]\r\n\r\nTeleport $playername or selected player to the place where he has been before last use of a teleportation command. If no $playername is entered and no player is selected,it will teleport you.'),
('reload',3,'Syntax: .reload table_name\r\n\r\nReload table `table_name` if reload support added for this table and this table can be _safe_ reloaded.'),
('reload all',3,'Syntax: .reload all\r\n\r\nReload all tables with reload support added and that can be _safe_ reloaded.'),
('reload all_area',3,'Syntax: .reload all_area\r\n\r\nReload all `areatrigger_*` tables if reload support added for this table and this table can be _safe_ reloaded.'),
('reload all_locales',3,'Syntax: .reload all_locales\r\n\r\nReload all `locales_*` tables with reload support added and that can be _safe_ reloaded.'),
('reload all_loot',3,'Syntax: .reload all_loot\r\n\r\nReload all `*_loot_template` tables. This can be slow operation with lags for server run.'),
('reload all_quest',3,'Syntax: .reload all_quest\r\n\r\nReload all quest related tables if reload support added for this table and this table can be _safe_ reloaded.'),
('reload all_spell',3,'Syntax: .reload all\r\n\r\nReload all `spell_*` tables with reload support added and that can be _safe_ reloaded.'),
('reload config',3,'Syntax: .reload config\r\n\r\nReload config settings (by default stored in trinityd.conf). Not all settings can be change at reload: some new setting values will be ignored until restart,some values will applied with delay or only to new objects/maps,some values will explicitly rejected to change at reload.'),
('reload spell_linked_spell',3,'Usage: .reload spell_linked_spell\r\nReloads the spell_linked_spell DB table.'),
('reload tickets',4,'Usage: .reload tickets.\r\nReloads GM Tickets from the database and re-caches them into memory.'),
('repairitems',2,'Syntax: .repairitems\r\n\r\nRepair all selected player''s items.'),
('reset all',3,'Syntax: .reset all spells\r\n\r\nSyntax: .reset all talents\r\n\r\nRequest reset spells or talents at next login each existed character.'),
('reset honor',3,'Syntax:\r\n.reset honor [Playername]\r\n  Reset all honor data for targeted character.'),
('reset level',3,'Syntax:\r\n.reset level [Playername]\r\n  Reset level to 1 including reset stats and talents.  Equipped items with greater level requirement can be lost.'),
('reset spells',3,'Syntax:\r\n.reset spells [Playername]\r\n  Removes all non-original spells from spellbook.\r\n. Playername can be name of offline character.'),
('reset stats',3,'Syntax:\r\n.reset stats [Playername]\r\n  Resets(recalculate) all stats of the targeted player to their original values at current level.'),
('reset talents',3,'Syntax:\r\n.reset talents [Playername]\r\n  Removes all talents of the targeted player. Playername can be name of offline character.'),
('respawn',3,'Syntax: .respawn\r\n\r\nRespawn all nearest creatures and GO without waiting respawn time expiration.'),
('revive',3,'Syntax: .revive\r\n\r\nRevive the selected player. If no player is selected,it will revive you.'),
('save',0,'Syntax: .save\r\n\r\nSaves your character.'),
('saveall',1,'Syntax: .saveall\r\n\r\nSave all characters in game.'),
('senditems',3,'Syntax: .senditems #playername "#subject" "#text" itemid1[:count1] itemid2[:count2] ... itemidN[:countN]\r\n\r\nSend a mail to a player. Subject and mail text must be in "". If for itemid not provided related count values then expected 1,if count > max items in stack then items will be send in required amount stacks. All stacks amount in mail limited to 12.'),
('sendmail',1,'Syntax: .sendmail #playername "#subject" "#text"\r\n\r\nSend a mail to a player. Subject and mail text must be in "".'),
('sendmessage',3,'Syntax: .sendmessage $playername $message\r\n\r\nSend screen message to player from ADMINISTRATOR.'),
('sendmoney',3,'Syntax: .sendmoney #playername "#subject" "#text" #money\r\n\r\nSend mail with money to a player. Subject and mail text must be in "".'),
('server corpses',2,'Syntax: .server corpses\r\n\r\nTriggering corpses expire check in world.'),
('server exit',4,'Syntax: .server exit\r\n\r\nTerminate trinity-core NOW. Exit code 0.'),
('server idlerestart',3,'Syntax: .server idlerestart #delay\r\n\r\nRestart the server after #delay seconds if no active connections are present (no players). Use #exist_code or 2 as program exist code.'),
('server idlerestart cancel',3,'Syntax: .server idlerestart cancel\r\n\r\nCancel the restart/shutdown timer if any.'),
('server idleshutdown',3,'Syntax: .server idleshutdown #delay [#exist_code]\r\n\r\nShut the server down after #delay seconds if no active connections are present (no players). Use #exist_code or 0 as program exist code.'),
('server idleshutdown cancel',3,'Syntax: .server idleshutdown cancel\r\n\r\nCancel the restart/shutdown timer if any.'),
('server info',0,'Syntax: .server info\r\n\r\nDisplay server version and the number of connected players.'),
('server motd',0,'Syntax: .server motd\r\n\r\nShow server Message of the day.'),
('server plimit',3,'Syntax: .server plimit [#num|-1|-2|-3|reset|player|moderator|gamemaster|administrator]\r\n\r\nWithout arg show current player amount and security level limitations for login to server,with arg set player linit ($num > 0) or securiti limitation ($num < 0 or security leme name. With `reset` sets player limit to the one in the config file'),
('server restart',3,'Syntax: .server restart #delay\r\n\r\nRestart the server after #delay seconds. Use #exist_code or 2 as program exist code.'),
('server restart cancel',3,'Syntax: .server restart cancel\r\n\r\nCancel the restart/shutdown timer if any.'),
('server set loglevel',4,'Syntax: .server set loglevel #level\r\n\r\nSet server log level (0 - errors only,1 - basic,2 - detail,3 - debug).'),
('server set motd',3,'Syntax: .server set motd $MOTD\r\n\r\nSet server Message of the day.'),
('server shutdown',3,'Syntax: .server shutdown #delay [#exist_code]\r\n\r\nShut the server down after #delay seconds. Use #exist_code or 0 as program exist code.'),
('server shutdown cancel',3,'Syntax: .server shutdown cancel\r\n\r\nCancel the restart/shutdown timer if any.'),
('setskill',3,'Syntax: .setskill #skill #level [#max]\r\n\r\nSet a skill of id #skill with a current skill value of #level and a maximum value of #max (or equal current maximum if not provide) for the selected character. If no character is selected,you learn the skill.'),
('showarea',3,'Syntax: .showarea #areaid\r\n\r\nReveal the area of #areaid to the selected character. If no character is selected,reveal this area to you.'),
('showhonor',0,'Syntax: .showhonor\r\n\r\nDisplay your honor ranking.'),
('start',0,'Syntax: .start\r\n\r\nTeleport you to the starting area of your character.'),
('taxicheat',1,'Syntax: .taxicheat on/off\r\n\r\nTemporary grant access or remove to all taxi routes for the selected character. If no character is selected,hide or reveal all routes to you.\r\n\r\nVisited taxi nodes sill accessible after removing access.'),
('tele',1,'Syntax: .tele #location\r\n\r\nTeleport player to a given location.'),
('tele add',3,'Syntax: .tele add $name\r\n\r\nAdd current your position to .tele command target locations list with name $name.'),
('tele del',3,'Syntax: .tele del $name\r\n\r\nRemove location with name $name for .tele command locations list.'),
('tele group',1,'Syntax: .tele group#location\r\n\r\nTeleport a selected player and his group members to a given location.'),
('tele name',1,'Syntax: .tele name #playername #location\r\n\r\nTeleport a player to a given location.'),
('ticket assign',3,'Usage: .ticket assign $ticketid $gmname.\r\nAssigns the specified ticket to the specified Game Master.'),
('ticket close',2,'Usage: .ticket close $ticketid.\r\nCloses the specified ticket. Does not delete permanently.'),
('ticket comment',2,'Usage: .ticket comment $ticketid $comment.\r\nAllows the adding or modifying of a comment to the specified ticket.'),
('ticket delete',3,'Usage: .ticket delete $ticketid.\r\nDeletes the specified ticket permanently. Ticket must be closed first.'),
('ticket list',1,'Displays a list of open GM tickets.'),
('ticket onlinelist',1,'Displays a list of open GM tickets whose owner is online.'),
('ticket unassign',3,'Usage: .ticket unassign $ticketid.\r\nUnassigns the specified ticket from the current assigned Game Master.'),
('ticket viewid',1,'Usage: .ticket viewid $ticketid.\r\nReturns details about specified ticket. Ticket must be open and not deleted.'),
('ticket viewname',1,'Usage: .ticket viewname $creatorname. \r\nReturns details about specified ticket. Ticket must be open and not deleted.'),
('unaura',3,'Syntax: .unaura #spellid\r\n\r\nRemove aura due to spell #spellid from the selected Unit.'),
('unban account',3,'Syntax is: unban account $Name\r\nUnban accounts for account name pattern.'),
('unban character',3,'Syntax is: unban character $Name\r\nUnban accounts for character name pattern.'),
('unban ip',3,'Syntax is: unban ip $Ip\r\nUnban accounts for IP pattern.'),
('unbindsight',3,'Syntax: .unbindsight\r\n\r\nRemoves bound vision. Cannot be used while currently possessing a target.'),
('unfreeze',2,'Syntax: .unfreeze (#player)\r\n"Unfreezes" #player and enables his chat again. When using this without #name it will unfreeze your target.'),
('unlearn',3,'Syntax: .unlearn #startspell #endspell\r\n\r\nUnlearn for selected player the range of spells between id #startspell and #endspell. If no #endspell is provided,just unlearn spell of id #startspell.'),
('unmute',1,'Syntax: .unmute $playerName\r\n\r\nRestore chat messaging for any character from account of character $playerName.'),
('unpossess',3,'Syntax: .unpossess\r\n\r\nIf you are possessed,unpossesses yourself; otherwise unpossesses current possessed target.'),
('waterwalk',2,'Syntax: .waterwalk on/off\r\n\r\nSet on/off waterwalk state for selected player or self if no player selected.'),
('wchange',3,'Syntax: .wchange #weathertype #status\r\n\r\nSet current weather to #weathertype with an intensity of #status.\r\n\r\n#weathertype can be 1 for rain,2 for snow,and 3 for sand. #status can be 0 for disabled,and 1 for enabled.'),
('whispers',1,'Syntax: .whispers on|off\r\nEnable/disable accepting whispers by GM from players. By default use trinityd.conf setting.'),
('wp',2,'Using WP Command:\r\nEach Waypoint Command has it''s own description!'),
('wp add',2,'Syntax: .wp add [#creature_guid or Select a Creature]'),
('wp export',3,'Syntax: .wp export [#creature_guid or Select a Creature] $filename'),
('wp import',3,'Syntax: .wp import $filename'),
('wp modify',2,'Syntax: .wp modify [#creature_guid or Select a Creature]\r\nadd - Add a waypoint after the selected visual\r\nwaittime $time\r\nemote ID\r\nspell ID\r\ntext1| text2| text3| text4| text5 <text>\r\nmodel1 ID\r\nmodel2 ID\r\nmove(moves wp to player pos)\r\ndel (deletes the wp)\r\n\r\nOnly one parameter per time!'),
('wp show',2,'Syntax: .wp show [#creature_guid or Select a Creature]\r\non\r\nfirst\r\nlast\r\noff\r\ninfo\r\n\r\nFor using info you have to do first show on and than select a Visual-Waypoint and do the show info!');
/*!40000 ALTER TABLE `command` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `creature`
--

DROP TABLE IF EXISTS `creature`;
CREATE TABLE `creature` (
  `guid` int(10) unsigned NOT NULL auto_increment COMMENT 'Global Unique Identifier',
  `id` mediumint(8) unsigned NOT NULL default '0' COMMENT 'Creature Identifier',
  `map` smallint(5) unsigned NOT NULL default '0' COMMENT 'Map Identifier',
  `spawnMask` tinyint(3) unsigned NOT NULL default '1',
  `modelid` mediumint(8) unsigned NOT NULL default '0',
  `equipment_id` mediumint(9) NOT NULL default '0',
  `position_x` float NOT NULL default '0',
  `position_y` float NOT NULL default '0',
  `position_z` float NOT NULL default '0',
  `orientation` float NOT NULL default '0',
  `spawntimesecs` int(10) unsigned NOT NULL default '120',
  `spawndist` float NOT NULL default '5',
  `currentwaypoint` mediumint(8) unsigned NOT NULL default '0',
  `curhealth` int(10) unsigned NOT NULL default '1',
  `curmana` int(10) unsigned NOT NULL default '0',
  `DeathState` tinyint(3) unsigned NOT NULL default '0',
  `MovementType` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`guid`),
  KEY `idx_map` (`map`),
  KEY `idx_id` (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Creature System';

--
-- Dumping data for table `creature`
--

LOCK TABLES `creature` WRITE;
/*!40000 ALTER TABLE `creature` DISABLE KEYS */;
/*!40000 ALTER TABLE `creature` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `creature_addon`
--

DROP TABLE IF EXISTS `creature_addon`;
CREATE TABLE `creature_addon` (
  `guid` int(10) unsigned NOT NULL default '0',
  `path_id` int(11) unsigned NOT NULL default '0',
  `mount` mediumint(8) unsigned NOT NULL default '0',
  `bytes0` int(10) unsigned NOT NULL default '0',
  `bytes1` int(10) unsigned NOT NULL default '0',
  `bytes2` int(10) unsigned NOT NULL default '0',
  `emote` int(10) unsigned NOT NULL default '0',
  `moveflags` int(10) unsigned NOT NULL default '0',
  `auras` text,
  PRIMARY KEY  (`guid`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Dumping data for table `creature_addon`
--

LOCK TABLES `creature_addon` WRITE;
/*!40000 ALTER TABLE `creature_addon` DISABLE KEYS */;
/*!40000 ALTER TABLE `creature_addon` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `creature_equip_template`
--

DROP TABLE IF EXISTS `creature_equip_template`;
CREATE TABLE `creature_equip_template` (
  `entry` mediumint(8) unsigned NOT NULL default '0' COMMENT 'Unique entry',
  `equipmodel1` mediumint(8) unsigned NOT NULL default '0',
  `equipmodel2` mediumint(8) unsigned NOT NULL default '0',
  `equipmodel3` mediumint(8) unsigned NOT NULL default '0',
  `equipinfo1` int(10) unsigned NOT NULL default '0',
  `equipinfo2` int(10) unsigned NOT NULL default '0',
  `equipinfo3` int(10) unsigned NOT NULL default '0',
  `equipslot1` int(11) NOT NULL default '0',
  `equipslot2` int(11) NOT NULL default '0',
  `equipslot3` int(11) NOT NULL default '0',
  PRIMARY KEY  (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='Creature System (Equipment)';

--
-- Dumping data for table `creature_equip_template`
--

LOCK TABLES `creature_equip_template` WRITE;
/*!40000 ALTER TABLE `creature_equip_template` DISABLE KEYS */;
/*!40000 ALTER TABLE `creature_equip_template` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `creature_formations`
--

DROP TABLE IF EXISTS `creature_formations`;
CREATE TABLE `creature_formations` (
  `leaderGUID` int(11) unsigned NOT NULL,
  `memberGUID` int(11) unsigned NOT NULL,
  `dist` float unsigned NOT NULL,
  `angle` float unsigned NOT NULL,
  `groupAI` int(11) unsigned NOT NULL,
  PRIMARY KEY  (`memberGUID`));

--
-- Dumping data for table `creature_formations`
--

LOCK TABLES `creature_formations` WRITE;
/*!40000 ALTER TABLE `creature_formations` DISABLE KEYS */;
/*!40000 ALTER TABLE `creature_formations` ENABLE KEYS */;
UNLOCK TABLES;


--
-- Table structure for table `creature_involvedrelation`
--

DROP TABLE IF EXISTS `creature_involvedrelation`;
CREATE TABLE `creature_involvedrelation` (
  `id` mediumint(8) unsigned NOT NULL default '0' COMMENT 'Identifier',
  `quest` mediumint(8) unsigned NOT NULL default '0' COMMENT 'Quest Identifier',
  PRIMARY KEY  (`id`,`quest`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Creature System';

--
-- Dumping data for table `creature_involvedrelation`
--

LOCK TABLES `creature_involvedrelation` WRITE;
/*!40000 ALTER TABLE `creature_involvedrelation` DISABLE KEYS */;
/*!40000 ALTER TABLE `creature_involvedrelation` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `creature_loot_template`
--

DROP TABLE IF EXISTS `creature_loot_template`;
CREATE TABLE `creature_loot_template` (
  `entry` mediumint(8) unsigned NOT NULL default '0',
  `item` mediumint(8) unsigned NOT NULL default '0',
  `ChanceOrQuestChance` float NOT NULL default '100',
  `groupid` tinyint(3) unsigned NOT NULL default '0',
  `mincountOrRef` mediumint(9) NOT NULL default '1',
  `maxcount` tinyint(3) unsigned NOT NULL default '1',
  `lootcondition` tinyint(3) unsigned NOT NULL default '0',
  `condition_value1` mediumint(8) unsigned NOT NULL default '0',
  `condition_value2` mediumint(8) unsigned NOT NULL default '0',
  PRIMARY KEY  (`entry`,`item`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='Loot System';

--
-- Dumping data for table `creature_loot_template`
--

LOCK TABLES `creature_loot_template` WRITE;
/*!40000 ALTER TABLE `creature_loot_template` DISABLE KEYS */;
/*!40000 ALTER TABLE `creature_loot_template` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `creature_model_info`
--

DROP TABLE IF EXISTS `creature_model_info`;
CREATE TABLE `creature_model_info` (
  `modelid` mediumint(8) unsigned NOT NULL default '0',
  `bounding_radius` float NOT NULL default '0',
  `combat_reach` float NOT NULL default '0',
  `gender` tinyint(3) unsigned NOT NULL default '2',
  `modelid_other_gender` mediumint(8) unsigned NOT NULL default '0',
  PRIMARY KEY  (`modelid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='Creature System (Model related info)';

--
-- Dumping data for table `creature_model_info`
--

LOCK TABLES `creature_model_info` WRITE;
/*!40000 ALTER TABLE `creature_model_info` DISABLE KEYS */;
/*!40000 ALTER TABLE `creature_model_info` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `creature_onkill_reputation`
--

DROP TABLE IF EXISTS `creature_onkill_reputation`;
CREATE TABLE `creature_onkill_reputation` (
  `creature_id` mediumint(8) unsigned NOT NULL default '0' COMMENT 'Creature Identifier',
  `RewOnKillRepFaction1` smallint(6) NOT NULL default '0',
  `RewOnKillRepFaction2` smallint(6) NOT NULL default '0',
  `MaxStanding1` tinyint(4) NOT NULL default '0',
  `IsTeamAward1` tinyint(4) NOT NULL default '0',
  `RewOnKillRepValue1` mediumint(9) NOT NULL default '0',
  `MaxStanding2` tinyint(4) NOT NULL default '0',
  `IsTeamAward2` tinyint(4) NOT NULL default '0',
  `RewOnKillRepValue2` mediumint(9) NOT NULL default '0',
  `TeamDependent` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`creature_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Creature OnKill Reputation gain';

--
-- Dumping data for table `creature_onkill_reputation`
--

LOCK TABLES `creature_onkill_reputation` WRITE;
/*!40000 ALTER TABLE `creature_onkill_reputation` DISABLE KEYS */;
/*!40000 ALTER TABLE `creature_onkill_reputation` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `creature_questrelation`
--

DROP TABLE IF EXISTS `creature_questrelation`;
CREATE TABLE `creature_questrelation` (
  `id` mediumint(8) unsigned NOT NULL default '0' COMMENT 'Identifier',
  `quest` mediumint(8) unsigned NOT NULL default '0' COMMENT 'Quest Identifier',
  PRIMARY KEY  (`id`,`quest`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Creature System';

--
-- Dumping data for table `creature_questrelation`
--

LOCK TABLES `creature_questrelation` WRITE;
/*!40000 ALTER TABLE `creature_questrelation` DISABLE KEYS */;
/*!40000 ALTER TABLE `creature_questrelation` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `creature_respawn`
--

DROP TABLE IF EXISTS `creature_respawn`;
CREATE TABLE `creature_respawn` (
  `guid` int(10) unsigned NOT NULL default '0' COMMENT 'Global Unique Identifier',
  `respawntime` bigint(20) NOT NULL default '0',
  `instance` mediumint(8) unsigned NOT NULL default '0',
  PRIMARY KEY  (`guid`,`instance`),
  KEY `instance` (`instance`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Grid Loading System';

--
-- Dumping data for table `creature_respawn`
--

LOCK TABLES `creature_respawn` WRITE;
/*!40000 ALTER TABLE `creature_respawn` DISABLE KEYS */;
/*!40000 ALTER TABLE `creature_respawn` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `creature_template`
--

DROP TABLE IF EXISTS `creature_template`;
CREATE TABLE `creature_template` (
  `entry` mediumint(8) unsigned NOT NULL default '0',
  `heroic_entry` mediumint(8) unsigned NOT NULL default '0',
  `modelid_A` mediumint(8) unsigned NOT NULL default '0',
  `modelid_A2` mediumint(8) unsigned NOT NULL default '0',
  `modelid_H` mediumint(8) unsigned NOT NULL default '0',
  `modelid_H2` mediumint(8) unsigned NOT NULL default '0',
  `name` char(100) NOT NULL default '0',
  `subname` char(100) default NULL,
  `IconName` char(100) default NULL,
  `minlevel` tinyint(3) unsigned NOT NULL default '1',
  `maxlevel` tinyint(3) unsigned NOT NULL default '1',
  `minhealth` int(10) unsigned NOT NULL default '0',
  `maxhealth` int(10) unsigned NOT NULL default '0',
  `minmana` int(10) unsigned NOT NULL default '0',
  `maxmana` int(10) unsigned NOT NULL default '0',
  `armor` mediumint(8) unsigned NOT NULL default '0',
  `faction_A` smallint(5) unsigned NOT NULL default '0',
  `faction_H` smallint(5) unsigned NOT NULL default '0',
  `npcflag` int(10) unsigned NOT NULL default '0',
  `speed` float default '1',
  `scale` float NOT NULL default '1',
  `rank` tinyint(3) unsigned NOT NULL default '0',
  `mindmg` float NOT NULL default '0',
  `maxdmg` float NOT NULL default '0',
  `dmgschool` tinyint(4) NOT NULL default '0',
  `attackpower` int(10) unsigned NOT NULL default '0',
  `baseattacktime` int(10) unsigned NOT NULL default '0',
  `rangeattacktime` int(10) unsigned NOT NULL default '0',
  `flags` int(10) unsigned NOT NULL default '0',
  `dynamicflags` int(10) unsigned NOT NULL default '0',
  `family` tinyint(4) NOT NULL default '0',
  `trainer_type` tinyint(4) NOT NULL default '0',
  `trainer_spell` mediumint(8) unsigned NOT NULL default '0',
  `class` tinyint(3) unsigned NOT NULL default '0',
  `race` tinyint(3) unsigned NOT NULL default '0',
  `minrangedmg` float NOT NULL default '0',
  `maxrangedmg` float NOT NULL default '0',
  `rangedattackpower` smallint(5) unsigned NOT NULL default '0',
  `type` tinyint(3) unsigned NOT NULL default '0',
  `flag1` int(10) unsigned NOT NULL default '0',
  `lootid` mediumint(8) unsigned NOT NULL default '0',
  `pickpocketloot` mediumint(8) unsigned NOT NULL default '0',
  `skinloot` mediumint(8) unsigned NOT NULL default '0',
  `resistance1` smallint(5) NOT NULL default '0',
  `resistance2` smallint(5) NOT NULL default '0',
  `resistance3` smallint(5) NOT NULL default '0',
  `resistance4` smallint(5) NOT NULL default '0',
  `resistance5` smallint(5) NOT NULL default '0',
  `resistance6` smallint(5) NOT NULL default '0',
  `spell1` mediumint(8) unsigned NOT NULL default '0',
  `spell2` mediumint(8) unsigned NOT NULL default '0',
  `spell3` mediumint(8) unsigned NOT NULL default '0',
  `spell4` mediumint(8) unsigned NOT NULL default '0',
  `PetSpellDataId` mediumint(8) unsigned NOT NULL default '0',
  `mingold` mediumint(8) unsigned NOT NULL default '0',
  `maxgold` mediumint(8) unsigned NOT NULL default '0',
  `AIName` char(64) NOT NULL default '',
  `MovementType` tinyint(3) unsigned NOT NULL default '0',
  `InhabitType` tinyint(3) unsigned NOT NULL default '3',
  `RacialLeader` tinyint(3) unsigned NOT NULL default '0',
  `RegenHealth` tinyint(3) unsigned NOT NULL default '1',
  `equipment_id` mediumint(8) unsigned NOT NULL default '0',
  `mechanic_immune_mask` int(10) unsigned NOT NULL default '0',
  `flags_extra` int(10) unsigned NOT NULL default '0',
  `ScriptName` char(64) NOT NULL default '',
  PRIMARY KEY  (`entry`),
  KEY `idx_name` (`name`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Creature System';

--
-- Dumping data for table `creature_template`
--

LOCK TABLES `creature_template` WRITE;
/*!40000 ALTER TABLE `creature_template` DISABLE KEYS */;
/*!40000 ALTER TABLE `creature_template` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `creature_template_addon`
--

DROP TABLE IF EXISTS `creature_template_addon`;
CREATE TABLE `creature_template_addon` (
  `entry` mediumint(8) unsigned NOT NULL default '0',
  `path_id` int(11) unsigned NOT NULL default '0',
  `mount` mediumint(8) unsigned NOT NULL default '0',
  `bytes0` int(10) unsigned NOT NULL default '0',
  `bytes1` int(10) unsigned NOT NULL default '0',
  `bytes2` int(10) unsigned NOT NULL default '0',
  `emote` mediumint(8) unsigned NOT NULL default '0',
  `moveflags` int(10) unsigned NOT NULL default '0',
  `auras` text,
  PRIMARY KEY  (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `creature_template_addon`
--

LOCK TABLES `creature_template_addon` WRITE;
/*!40000 ALTER TABLE `creature_template_addon` DISABLE KEYS */;
/*!40000 ALTER TABLE `creature_template_addon` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `custom_texts`
--

DROP TABLE IF EXISTS `custom_texts`;
CREATE TABLE `custom_texts` (
  `entry` mediumint(8) NOT NULL,
  `content_default` text NOT NULL,
  `content_loc1` text,
  `content_loc2` text,
  `content_loc3` text,
  `content_loc4` text,
  `content_loc5` text,
  `content_loc6` text,
  `content_loc7` text,
  `content_loc8` text,
  `sound` mediumint(8) unsigned NOT NULL default '0',
  `type` tinyint unsigned NOT NULL default '0',
  `language` tinyint unsigned NOT NULL default '0',
  `emote` smallint(5) UNSIGNED NOT NULL default '0',
  `comment` text,
  PRIMARY KEY (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Custom Texts';

--
-- Dumping data for table `custom_texts`
--

LOCK TABLES `custom_texts` WRITE;
/*!40000 ALTER TABLE `custom_texts` DISABLE KEYS */;
/*!40000 ALTER TABLE `custom_texts` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `disenchant_loot_template`
--

DROP TABLE IF EXISTS `disenchant_loot_template`;
CREATE TABLE `disenchant_loot_template` (
  `entry` mediumint(8) unsigned NOT NULL default '0' COMMENT 'Recommended id selection: item_level*100 + item_quality',
  `item` mediumint(8) unsigned NOT NULL default '0',
  `ChanceOrQuestChance` float NOT NULL default '100',
  `groupid` tinyint(3) unsigned NOT NULL default '0',
  `mincountOrRef` mediumint(9) NOT NULL default '1',
  `maxcount` tinyint(3) unsigned NOT NULL default '1',
  `lootcondition` tinyint(3) unsigned NOT NULL default '0',
  `condition_value1` mediumint(8) unsigned NOT NULL default '0',
  `condition_value2` mediumint(8) unsigned NOT NULL default '0',
  PRIMARY KEY  (`entry`,`item`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Loot System';

--
-- Dumping data for table `disenchant_loot_template`
--

LOCK TABLES `disenchant_loot_template` WRITE;
/*!40000 ALTER TABLE `disenchant_loot_template` DISABLE KEYS */;
/*!40000 ALTER TABLE `disenchant_loot_template` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `event_scripts`
--

DROP TABLE IF EXISTS `event_scripts`;
CREATE TABLE `event_scripts` (
  `id` mediumint(8) unsigned NOT NULL default '0',
  `delay` int(10) unsigned NOT NULL default '0',
  `command` mediumint(8) unsigned NOT NULL default '0',
  `datalong` mediumint(8) unsigned NOT NULL default '0',
  `datalong2` int(10) unsigned NOT NULL default '0',
  `datatext` text NOT NULL,
  `x` float NOT NULL default '0',
  `y` float NOT NULL default '0',
  `z` float NOT NULL default '0',
  `o` float NOT NULL default '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `event_scripts`
--

LOCK TABLES `event_scripts` WRITE;
/*!40000 ALTER TABLE `event_scripts` DISABLE KEYS */;
/*!40000 ALTER TABLE `event_scripts` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `eventai_localized_texts`
--

DROP TABLE IF EXISTS `eventai_localized_texts`;
CREATE TABLE `eventai_localized_texts` (
  `id` int(11) unsigned NOT NULL auto_increment COMMENT 'Identifier',
  `locale_1` varchar(255) NOT NULL default '',
  `locale_2` varchar(255) NOT NULL default '',
  `locale_3` varchar(255) NOT NULL default '',
  `locale_4` varchar(255) NOT NULL default '',
  `locale_5` varchar(255) NOT NULL default '',
  `locale_6` varchar(255) NOT NULL default '',
  `locale_7` varchar(255) NOT NULL default '',
  `locale_8` varchar(255) NOT NULL default '',
  `comment` varchar(255) NOT NULL default '' COMMENT 'Text Comment',
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='EventAI Localized Text';

--
-- Dumping data for table `eventai_localized_texts`
--

LOCK TABLES `eventai_localized_texts` WRITE;
/*!40000 ALTER TABLE `eventai_localized_texts` DISABLE KEYS */;
/*!40000 ALTER TABLE `eventai_localized_texts` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `eventai_scripts`
--

DROP TABLE IF EXISTS `eventai_scripts`;
CREATE TABLE `eventai_scripts` (
  `id` int(11) unsigned NOT NULL COMMENT 'Identifier' AUTO_INCREMENT,
  `creature_id` int(11) unsigned NOT NULL default '0' COMMENT 'Creature Template Identifier',
  `event_type` tinyint(5) unsigned NOT NULL default '0' COMMENT 'Event Type',
  `event_inverse_phase_mask` int(11) signed NOT NULL default '0' COMMENT 'Mask which phases this event will not trigger in',
  `event_chance` int(3) unsigned NOT NULL default '100',
  `event_flags` int(3) unsigned NOT NULL default '0',
  `event_param1` int(11) signed NOT NULL default '0',
  `event_param2` int(11) signed NOT NULL default '0',
  `event_param3` int(11) signed NOT NULL default '0',
  `event_param4` int(11) signed NOT NULL default '0',
  `action1_type` tinyint(5) unsigned NOT NULL default '0' COMMENT 'Action Type',
  `action1_param1` int(11) signed NOT NULL default '0',
  `action1_param2` int(11) signed NOT NULL default '0',
  `action1_param3` int(11) signed NOT NULL default '0',
  `action2_type` tinyint(5) unsigned NOT NULL default '0' COMMENT 'Action Type',
  `action2_param1` int(11) signed NOT NULL default '0',
  `action2_param2` int(11) signed NOT NULL default '0',
  `action2_param3` int(11) signed NOT NULL default '0',
  `action3_type` tinyint(5) unsigned NOT NULL default '0' COMMENT 'Action Type',
  `action3_param1` int(11) signed NOT NULL default '0',
  `action3_param2` int(11) signed NOT NULL default '0',
  `action3_param3` int(11) signed NOT NULL default '0',
  `comment` varchar(255) NOT NULL default '' COMMENT 'Event Comment',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='EventAI Scripts';

--
-- Dumping data for table `eventai_scripts`
--

LOCK TABLES `eventai_scripts` WRITE;
/*!40000 ALTER TABLE `eventai_scripts` DISABLE KEYS */;
/*!40000 ALTER TABLE `eventai_scripts` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `eventai_summons`
--

DROP TABLE IF EXISTS `eventai_summons`;
CREATE TABLE `eventai_summons` (
  `id` int(11) unsigned NOT NULL COMMENT 'Location Identifier' AUTO_INCREMENT,
  `position_x` float NOT NULL default '0',
  `position_y` float NOT NULL default '0',
  `position_z` float NOT NULL default '0',
  `orientation` float NOT NULL default '0',
  `spawntimesecs` int(11) unsigned NOT NULL default '120',
  `comment` varchar(255) NOT NULL default '' COMMENT 'Summon Comment',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='EventAI Summoning Locations';

--
-- Dumping data for table `eventai_summons`
--

LOCK TABLES `eventai_summons` WRITE;
/*!40000 ALTER TABLE `eventai_summons` DISABLE KEYS */;
/*!40000 ALTER TABLE `eventai_summons` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `eventai_texts`
--

DROP TABLE IF EXISTS `eventai_texts`;
CREATE TABLE `eventai_texts` (
  `entry` mediumint(8) NOT NULL,
  `content_default` text NOT NULL,
  `content_loc1` text,
  `content_loc2` text,
  `content_loc3` text,
  `content_loc4` text,
  `content_loc5` text,
  `content_loc6` text,
  `content_loc7` text,
  `content_loc8` text,
  `sound` mediumint(8) unsigned NOT NULL default '0',
  `type` tinyint unsigned NOT NULL default '0',
  `language` tinyint unsigned NOT NULL default '0',
  `comment` text,
  PRIMARY KEY (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Script Texts';

--
-- Dumping data for table `eventai_texts`
--

LOCK TABLES `eventai_texts` WRITE;
/*!40000 ALTER TABLE `eventai_texts` DISABLE KEYS */;
/*!40000 ALTER TABLE `eventai_texts` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `exploration_basexp`
--

DROP TABLE IF EXISTS `exploration_basexp`;
CREATE TABLE `exploration_basexp` (
  `level` tinyint(4) NOT NULL default '0',
  `basexp` mediumint(9) NOT NULL default '0',
  PRIMARY KEY  (`level`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Exploration System';

--
-- Dumping data for table `exploration_basexp`
--

LOCK TABLES `exploration_basexp` WRITE;
/*!40000 ALTER TABLE `exploration_basexp` DISABLE KEYS */;
INSERT INTO `exploration_basexp` VALUES
(0,0),
(1,5),
(2,15),
(3,25),
(4,35),
(5,45),
(6,55),
(7,65),
(8,70),
(9,80),
(10,85),
(11,90),
(12,90),
(13,90),
(14,100),
(15,105),
(16,115),
(17,125),
(18,135),
(19,145),
(20,155),
(21,165),
(22,175),
(23,185),
(24,195),
(25,200),
(26,210),
(27,220),
(28,230),
(29,240),
(30,245),
(31,250),
(32,255),
(33,265),
(34,270),
(35,275),
(36,280),
(37,285),
(38,285),
(39,300),
(40,315),
(41,330),
(42,345),
(43,360),
(44,375),
(45,390),
(46,405),
(47,420),
(48,440),
(49,455),
(50,470),
(51,490),
(52,510),
(53,530),
(54,540),
(55,560),
(56,580),
(57,600),
(58,620),
(59,640),
(60,660),
(61,970),
(62,1000),
(63,1050),
(64,1080),
(65,1100),
(66,1130),
(67,1160),
(68,1200),
(69,1230),
(70,1300);
/*!40000 ALTER TABLE `exploration_basexp` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `fishing_loot_template`
--

DROP TABLE IF EXISTS `fishing_loot_template`;
CREATE TABLE `fishing_loot_template` (
  `entry` mediumint(8) unsigned NOT NULL default '0',
  `item` mediumint(8) unsigned NOT NULL default '0',
  `ChanceOrQuestChance` float NOT NULL default '100',
  `groupid` tinyint(3) unsigned NOT NULL default '0',
  `mincountOrRef` mediumint(9) NOT NULL default '1',
  `maxcount` tinyint(3) unsigned NOT NULL default '1',
  `lootcondition` tinyint(3) unsigned NOT NULL default '0',
  `condition_value1` mediumint(8) unsigned NOT NULL default '0',
  `condition_value2` mediumint(8) unsigned NOT NULL default '0',
  PRIMARY KEY  (`entry`,`item`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='Loot System';

--
-- Dumping data for table `fishing_loot_template`
--

LOCK TABLES `fishing_loot_template` WRITE;
/*!40000 ALTER TABLE `fishing_loot_template` DISABLE KEYS */;
/*!40000 ALTER TABLE `fishing_loot_template` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `game_event`
--

DROP TABLE IF EXISTS `game_event`;
CREATE TABLE `game_event` (
  `entry` mediumint(8) unsigned NOT NULL COMMENT 'Entry of the game event',
  `start_time` timestamp NOT NULL default '0000-00-00 00:00:00' COMMENT 'Absolute start date, the event will never start before',
  `end_time` timestamp NOT NULL default '0000-00-00 00:00:00' COMMENT 'Absolute end date, the event will never start afler',
  `occurence` bigint(20) unsigned NOT NULL default '5184000' COMMENT 'Delay in minutes between occurences of the event',
  `length` bigint(20) unsigned NOT NULL default '2592000' COMMENT 'Length in minutes of the event',
  `description` varchar(255) default NULL COMMENT 'Description of the event displayed in console',
  `world_event` tinyint(3) unsigned NOT NULL default '0' COMMENT '0 if normal event, 1 if world event',
  PRIMARY KEY  (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `game_event`
--

LOCK TABLES `game_event` WRITE;
/*!40000 ALTER TABLE `game_event` DISABLE KEYS */;
/*!40000 ALTER TABLE `game_event` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `game_event_battleground_holiday`
--

DROP TABLE IF EXISTS `game_event_battleground_holiday`;
CREATE TABLE `game_event_battleground_holiday` (
 `event` int(10) unsigned NOT NULL,
 `bgflag` int(10) unsigned NOT NULL default '0',
 PRIMARY KEY  (`event`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `game_event_battleground_holiday`
--

LOCK TABLES `game_event_battleground_holiday` WRITE;
/*!40000 ALTER TABLE `game_event_battleground_holiday` DISABLE KEYS */;
/*!40000 ALTER TABLE `game_event_battleground_holiday` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `game_event_condition`
--

DROP TABLE IF EXISTS `game_event_condition`;
CREATE TABLE `game_event_condition` (
  `event_id` mediumint(8) unsigned NOT NULL default '0',
  `condition_id` mediumint(8) unsigned NOT NULL default '0',
  `req_num` float default '0',
  `max_world_state_field` smallint(5) unsigned NOT NULL default '0',
  `done_world_state_field` smallint(5) unsigned NOT NULL default '0',
  `description` varchar(25) NOT NULL default '',
  PRIMARY KEY  (`event_id`,`condition_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `game_event_condition`
--

LOCK TABLES `game_event_condition` WRITE;
/*!40000 ALTER TABLE `game_event_condition` DISABLE KEYS */;
/*!40000 ALTER TABLE `game_event_condition` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `game_event_creature`
--

DROP TABLE IF EXISTS `game_event_creature`;
CREATE TABLE `game_event_creature` (
  `guid` int(10) unsigned NOT NULL,
  `event` smallint(6) NOT NULL default '0' COMMENT 'Put negatives values to remove during event',
  PRIMARY KEY  (`guid`,`event`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `game_event_creature`
--

LOCK TABLES `game_event_creature` WRITE;
/*!40000 ALTER TABLE `game_event_creature` DISABLE KEYS */;
/*!40000 ALTER TABLE `game_event_creature` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `game_event_creature_quest`
--

DROP TABLE IF EXISTS `game_event_creature_quest`;
CREATE TABLE `game_event_creature_quest` (
  `id` mediumint(8) unsigned NOT NULL default '0',
  `quest` mediumint(8) unsigned NOT NULL default '0',
  `event` smallint(5) unsigned NOT NULL default '0',
  PRIMARY KEY  (`quest`,`event`,`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `game_event_creature_quest`
--

LOCK TABLES `game_event_creature_quest` WRITE;
/*!40000 ALTER TABLE `game_event_creature_quest` DISABLE KEYS */;
/*!40000 ALTER TABLE `game_event_creature_quest` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `game_event_gameobject`
--

DROP TABLE IF EXISTS `game_event_gameobject`;
CREATE TABLE `game_event_gameobject` (
  `guid` int(10) unsigned NOT NULL,
  `event` smallint(6) NOT NULL default '0' COMMENT 'Put negatives values to remove during event',
  PRIMARY KEY  (`guid`,`event`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `game_event_gameobject`
--

LOCK TABLES `game_event_gameobject` WRITE;
/*!40000 ALTER TABLE `game_event_gameobject` DISABLE KEYS */;
/*!40000 ALTER TABLE `game_event_gameobject` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `game_event_gameobject_quest`
--

DROP TABLE IF EXISTS `game_event_gameobject_quest`;
CREATE TABLE `game_event_gameobject_quest` (
  `id` mediumint(8) unsigned NOT NULL default '0',
  `quest` mediumint(8) unsigned NOT NULL default '0',
  `event` smallint(5) unsigned NOT NULL default '0',
  PRIMARY KEY  (`quest`,`event`,`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `game_event_gameobject_quest`
--

LOCK TABLES `game_event_gameobject_quest` WRITE;
/*!40000 ALTER TABLE `game_event_gameobject_quest` DISABLE KEYS */;
/*!40000 ALTER TABLE `game_event_gameobject_quest` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `game_event_model_equip`
--

DROP TABLE IF EXISTS `game_event_model_equip`;
CREATE TABLE `game_event_model_equip` (
  `guid` int(10) unsigned NOT NULL default '0',
  `modelid` mediumint(8) unsigned NOT NULL default '0',
  `equipment_id` mediumint(8) unsigned NOT NULL default '0',
  `event` smallint(5) unsigned NOT NULL default '0',
  PRIMARY KEY  (`guid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `game_event_model_equip`
--

LOCK TABLES `game_event_model_equip` WRITE;
/*!40000 ALTER TABLE `game_event_model_equip` DISABLE KEYS */;
/*!40000 ALTER TABLE `game_event_model_equip` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `game_event_npc_gossip`
--

DROP TABLE IF EXISTS `game_event_npc_gossip`;
CREATE TABLE `game_event_npc_gossip` (
  `guid` int(10) unsigned NOT NULL,
  `event_id` mediumint(8) unsigned NOT NULL default '0',
  `textid` mediumint(8) unsigned NOT NULL default '0',
  PRIMARY KEY  (`guid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `game_event_npc_gossip`
--

LOCK TABLES `game_event_npc_gossip` WRITE;
/*!40000 ALTER TABLE `game_event_npc_gossip` DISABLE KEYS */;
/*!40000 ALTER TABLE `game_event_npc_gossip` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `game_event_npc_vendor`
--

DROP TABLE IF EXISTS `game_event_npc_vendor`;
CREATE TABLE `game_event_npc_vendor` (
  `event` mediumint(8) unsigned NOT NULL default '0',
  `guid` mediumint(8) unsigned NOT NULL default '0',
  `item` mediumint(8) unsigned NOT NULL default '0',
  `maxcount` mediumint(8) unsigned NOT NULL default '0',
  `incrtime` mediumint(8) unsigned NOT NULL default '0',
  `ExtendedCost` mediumint(8) unsigned NOT NULL default '0',
  PRIMARY KEY  (`guid`,`item`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `game_event_npc_vendor`
--

LOCK TABLES `game_event_npc_vendor` WRITE;
/*!40000 ALTER TABLE `game_event_npc_vendor` DISABLE KEYS */;
/*!40000 ALTER TABLE `game_event_npc_vendor` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `game_event_npcflag`
--

DROP TABLE IF EXISTS `game_event_npcflag`;
CREATE TABLE `game_event_npcflag` (
  `guid` mediumint(8) unsigned NOT NULL default '0',
  `event_id` mediumint(8) unsigned NOT NULL default '0',
  `npcflag` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`guid`,`event_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `game_event_npcflag`
--

LOCK TABLES `game_event_npcflag` WRITE;
/*!40000 ALTER TABLE `game_event_npcflag` DISABLE KEYS */;
/*!40000 ALTER TABLE `game_event_npcflag` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `game_event_prerequisite`
--

DROP TABLE IF EXISTS `game_event_prerequisite`;
CREATE TABLE `game_event_prerequisite` (
  `event_id` mediumint(8) unsigned NOT NULL,
  `prerequisite_event` mediumint(8) unsigned NOT NULL,
  PRIMARY KEY  (`event_id`,`prerequisite_event`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `game_event_prerequisite`
--

LOCK TABLES `game_event_prerequisite` WRITE;
/*!40000 ALTER TABLE `game_event_prerequisite` DISABLE KEYS */;
/*!40000 ALTER TABLE `game_event_prerequisite` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `game_event_quest_condition`
--

DROP TABLE IF EXISTS `game_event_quest_condition`;
CREATE TABLE `game_event_quest_condition` (
  `quest` mediumint(8) unsigned NOT NULL default '0',
  `event_id` mediumint(8) unsigned NOT NULL default '0',
  `condition_id` mediumint(8) unsigned NOT NULL default '0',
  `num` float default '0',
  PRIMARY KEY  (`quest`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `game_event_quest_condition`
--

LOCK TABLES `game_event_quest_condition` WRITE;
/*!40000 ALTER TABLE `game_event_quest_condition` DISABLE KEYS */;
/*!40000 ALTER TABLE `game_event_quest_condition` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `game_graveyard_zone`
--

DROP TABLE IF EXISTS `game_graveyard_zone`;
CREATE TABLE `game_graveyard_zone` (
  `id` mediumint(8) unsigned NOT NULL default '0',
  `ghost_zone` mediumint(8) unsigned NOT NULL default '0',
  `faction` smallint(5) unsigned NOT NULL default '0',
  PRIMARY KEY  (`id`,`ghost_zone`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='Trigger System';

--
-- Dumping data for table `game_graveyard_zone`
--

LOCK TABLES `game_graveyard_zone` WRITE;
/*!40000 ALTER TABLE `game_graveyard_zone` DISABLE KEYS */;
/*!40000 ALTER TABLE `game_graveyard_zone` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `game_tele`
--

DROP TABLE IF EXISTS `game_tele`;
CREATE TABLE `game_tele` (
  `id` mediumint(8) unsigned NOT NULL auto_increment,
  `position_x` float NOT NULL default '0',
  `position_y` float NOT NULL default '0',
  `position_z` float NOT NULL default '0',
  `orientation` float NOT NULL default '0',
  `map` smallint(5) unsigned NOT NULL default '0',
  `name` varchar(100) NOT NULL default '',
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='Tele Command';

--
-- Dumping data for table `game_tele`
--

LOCK TABLES `game_tele` WRITE;
/*!40000 ALTER TABLE `game_tele` DISABLE KEYS */;
/*!40000 ALTER TABLE `game_tele` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `game_weather`
--

DROP TABLE IF EXISTS `game_weather`;
CREATE TABLE `game_weather` (
  `zone` mediumint(8) unsigned NOT NULL default '0',
  `spring_rain_chance` tinyint(3) unsigned NOT NULL default '25',
  `spring_snow_chance` tinyint(3) unsigned NOT NULL default '25',
  `spring_storm_chance` tinyint(3) unsigned NOT NULL default '25',
  `summer_rain_chance` tinyint(3) unsigned NOT NULL default '25',
  `summer_snow_chance` tinyint(3) unsigned NOT NULL default '25',
  `summer_storm_chance` tinyint(3) unsigned NOT NULL default '25',
  `fall_rain_chance` tinyint(3) unsigned NOT NULL default '25',
  `fall_snow_chance` tinyint(3) unsigned NOT NULL default '25',
  `fall_storm_chance` tinyint(3) unsigned NOT NULL default '25',
  `winter_rain_chance` tinyint(3) unsigned NOT NULL default '25',
  `winter_snow_chance` tinyint(3) unsigned NOT NULL default '25',
  `winter_storm_chance` tinyint(3) unsigned NOT NULL default '25',
  PRIMARY KEY  (`zone`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Weather System';

--
-- Dumping data for table `game_weather`
--

LOCK TABLES `game_weather` WRITE;
/*!40000 ALTER TABLE `game_weather` DISABLE KEYS */;
/*!40000 ALTER TABLE `game_weather` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `gameobject`
--

DROP TABLE IF EXISTS `gameobject`;
CREATE TABLE `gameobject` (
  `guid` int(10) unsigned NOT NULL auto_increment COMMENT 'Global Unique Identifier',
  `id` mediumint(8) unsigned NOT NULL default '0' COMMENT 'Gameobject Identifier',
  `map` smallint(5) unsigned NOT NULL default '0' COMMENT 'Map Identifier',
  `spawnMask` tinyint(3) unsigned NOT NULL default '1',
  `position_x` float NOT NULL default '0',
  `position_y` float NOT NULL default '0',
  `position_z` float NOT NULL default '0',
  `orientation` float NOT NULL default '0',
  `rotation0` float NOT NULL default '0',
  `rotation1` float NOT NULL default '0',
  `rotation2` float NOT NULL default '0',
  `rotation3` float NOT NULL default '0',
  `spawntimesecs` int(11) NOT NULL default '0',
  `animprogress` tinyint(3) unsigned NOT NULL default '0',
  `state` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`guid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Gameobject System';

--
-- Dumping data for table `gameobject`
--

LOCK TABLES `gameobject` WRITE;
/*!40000 ALTER TABLE `gameobject` DISABLE KEYS */;
/*!40000 ALTER TABLE `gameobject` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `gameobject_involvedrelation`
--

DROP TABLE IF EXISTS `gameobject_involvedrelation`;
CREATE TABLE `gameobject_involvedrelation` (
  `id` mediumint(8) unsigned NOT NULL default '0',
  `quest` mediumint(8) unsigned NOT NULL default '0' COMMENT 'Quest Identifier',
  PRIMARY KEY  (`id`,`quest`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `gameobject_involvedrelation`
--

LOCK TABLES `gameobject_involvedrelation` WRITE;
/*!40000 ALTER TABLE `gameobject_involvedrelation` DISABLE KEYS */;
/*!40000 ALTER TABLE `gameobject_involvedrelation` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `gameobject_loot_template`
--

DROP TABLE IF EXISTS `gameobject_loot_template`;
CREATE TABLE `gameobject_loot_template` (
  `entry` mediumint(8) unsigned NOT NULL default '0',
  `item` mediumint(8) unsigned NOT NULL default '0',
  `ChanceOrQuestChance` float NOT NULL default '100',
  `groupid` tinyint(3) unsigned NOT NULL default '0',
  `mincountOrRef` mediumint(9) NOT NULL default '1',
  `maxcount` tinyint(3) unsigned NOT NULL default '1',
  `lootcondition` tinyint(3) unsigned NOT NULL default '0',
  `condition_value1` mediumint(8) unsigned NOT NULL default '0',
  `condition_value2` mediumint(8) unsigned NOT NULL default '0',
  PRIMARY KEY  (`entry`,`item`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='Loot System';

--
-- Dumping data for table `gameobject_loot_template`
--

LOCK TABLES `gameobject_loot_template` WRITE;
/*!40000 ALTER TABLE `gameobject_loot_template` DISABLE KEYS */;
/*!40000 ALTER TABLE `gameobject_loot_template` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `gameobject_questrelation`
--

DROP TABLE IF EXISTS `gameobject_questrelation`;
CREATE TABLE `gameobject_questrelation` (
  `id` mediumint(8) unsigned NOT NULL default '0',
  `quest` mediumint(8) unsigned NOT NULL default '0' COMMENT 'Quest Identifier',
  PRIMARY KEY  (`id`,`quest`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `gameobject_questrelation`
--

LOCK TABLES `gameobject_questrelation` WRITE;
/*!40000 ALTER TABLE `gameobject_questrelation` DISABLE KEYS */;
/*!40000 ALTER TABLE `gameobject_questrelation` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `gameobject_respawn`
--

DROP TABLE IF EXISTS `gameobject_respawn`;
CREATE TABLE `gameobject_respawn` (
  `guid` int(10) unsigned NOT NULL default '0' COMMENT 'Global Unique Identifier',
  `respawntime` bigint(20) NOT NULL default '0',
  `instance` mediumint(8) unsigned NOT NULL default '0',
  PRIMARY KEY  (`guid`,`instance`),
  KEY `instance` (`instance`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Grid Loading System';

--
-- Dumping data for table `gameobject_respawn`
--

LOCK TABLES `gameobject_respawn` WRITE;
/*!40000 ALTER TABLE `gameobject_respawn` DISABLE KEYS */;
/*!40000 ALTER TABLE `gameobject_respawn` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `gameobject_scripts`
--

DROP TABLE IF EXISTS `gameobject_scripts`;
CREATE TABLE `gameobject_scripts` (
  `id` mediumint(8) unsigned NOT NULL default '0',
  `delay` int(10) unsigned NOT NULL default '0',
  `command` mediumint(8) unsigned NOT NULL default '0',
  `datalong` mediumint(8) unsigned NOT NULL default '0',
  `datalong2` int(10) unsigned NOT NULL default '0',
  `datatext` text NOT NULL,
  `x` float NOT NULL default '0',
  `y` float NOT NULL default '0',
  `z` float NOT NULL default '0',
  `o` float NOT NULL default '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `gameobject_scripts`
--

LOCK TABLES `gameobject_scripts` WRITE;
/*!40000 ALTER TABLE `gameobject_scripts` DISABLE KEYS */;
/*!40000 ALTER TABLE `gameobject_scripts` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `gameobject_template`
--

DROP TABLE IF EXISTS `gameobject_template`;
CREATE TABLE `gameobject_template` (
  `entry` mediumint(8) unsigned NOT NULL default '0',
  `type` tinyint(3) unsigned NOT NULL default '0',
  `displayId` mediumint(8) unsigned NOT NULL default '0',
  `name` varchar(100) NOT NULL default '',
  `castBarCaption` varchar(100) NOT NULL default '',
  `faction` smallint(5) unsigned NOT NULL default '0',
  `flags` int(10) unsigned NOT NULL default '0',
  `size` float NOT NULL default '1',
  `data0` int(10) unsigned NOT NULL default '0',
  `data1` int(10) unsigned NOT NULL default '0',
  `data2` int(10) unsigned NOT NULL default '0',
  `data3` int(10) unsigned NOT NULL default '0',
  `data4` int(10) unsigned NOT NULL default '0',
  `data5` int(10) unsigned NOT NULL default '0',
  `data6` int(10) unsigned NOT NULL default '0',
  `data7` int(10) unsigned NOT NULL default '0',
  `data8` int(10) unsigned NOT NULL default '0',
  `data9` int(10) unsigned NOT NULL default '0',
  `data10` int(10) unsigned NOT NULL default '0',
  `data11` int(10) unsigned NOT NULL default '0',
  `data12` int(10) unsigned NOT NULL default '0',
  `data13` int(10) unsigned NOT NULL default '0',
  `data14` int(10) unsigned NOT NULL default '0',
  `data15` int(10) unsigned NOT NULL default '0',
  `data16` int(10) unsigned NOT NULL default '0',
  `data17` int(10) unsigned NOT NULL default '0',
  `data18` int(10) unsigned NOT NULL default '0',
  `data19` int(10) unsigned NOT NULL default '0',
  `data20` int(10) unsigned NOT NULL default '0',
  `data21` int(10) unsigned NOT NULL default '0',
  `data22` int(10) unsigned NOT NULL default '0',
  `data23` int(10) unsigned NOT NULL default '0',
  `ScriptName` varchar(64) NOT NULL default '',
  PRIMARY KEY  (`entry`),
  KEY `idx_name` (`name`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Gameobject System';

--
-- Dumping data for table `gameobject_template`
--

LOCK TABLES `gameobject_template` WRITE;
/*!40000 ALTER TABLE `gameobject_template` DISABLE KEYS */;
/*!40000 ALTER TABLE `gameobject_template` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `instance_template`
--

DROP TABLE IF EXISTS `instance_template`;
CREATE TABLE `instance_template` (
  `map` smallint(5) unsigned NOT NULL,
  `parent` int(10) unsigned NOT NULL,
  `maxPlayers` tinyint(3) unsigned NOT NULL default '0',
  `reset_delay` int(10) unsigned NOT NULL default '0',
  `access_id` bigint(20) unsigned NOT NULL DEFAULT '0',
  `startLocX` float default NULL,
  `startLocY` float default NULL,
  `startLocZ` float default NULL,
  `startLocO` float default NULL,
  `script` varchar(128) NOT NULL default '',
  PRIMARY KEY  (`map`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `instance_template`
--

LOCK TABLES `instance_template` WRITE;
/*!40000 ALTER TABLE `instance_template` DISABLE KEYS */;
/*!40000 ALTER TABLE `instance_template` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `item_enchantment_template`
--

DROP TABLE IF EXISTS `item_enchantment_template`;
CREATE TABLE `item_enchantment_template` (
  `entry` mediumint(8) unsigned NOT NULL default '0',
  `ench` mediumint(8) unsigned NOT NULL default '0',
  `chance` float unsigned NOT NULL default '0',
  PRIMARY KEY  (`entry`,`ench`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Item Random Enchantment System';

--
-- Dumping data for table `item_enchantment_template`
--

LOCK TABLES `item_enchantment_template` WRITE;
/*!40000 ALTER TABLE `item_enchantment_template` DISABLE KEYS */;
/*!40000 ALTER TABLE `item_enchantment_template` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `item_loot_template`
--

DROP TABLE IF EXISTS `item_loot_template`;
CREATE TABLE `item_loot_template` (
  `entry` mediumint(8) unsigned NOT NULL default '0',
  `item` mediumint(8) unsigned NOT NULL default '0',
  `ChanceOrQuestChance` float NOT NULL default '100',
  `groupid` tinyint(3) unsigned NOT NULL default '0',
  `mincountOrRef` mediumint(9) NOT NULL default '1',
  `maxcount` smallint(5) unsigned NOT NULL default '1',
  `lootcondition` tinyint(3) unsigned NOT NULL default '0',
  `condition_value1` mediumint(8) unsigned NOT NULL default '0',
  `condition_value2` mediumint(8) unsigned NOT NULL default '0',
  PRIMARY KEY  (`entry`,`item`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Loot System';

--
-- Dumping data for table `item_loot_template`
--

LOCK TABLES `item_loot_template` WRITE;
/*!40000 ALTER TABLE `item_loot_template` DISABLE KEYS */;
/*!40000 ALTER TABLE `item_loot_template` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `item_template`
--

DROP TABLE IF EXISTS `item_template`;
CREATE TABLE `item_template` (
  `entry` mediumint(8) unsigned NOT NULL default '0',
  `class` tinyint(3) unsigned NOT NULL default '0',
  `subclass` tinyint(3) unsigned NOT NULL default '0',
  `unk0` int(11) NOT NULL default '-1',
  `name` varchar(255) NOT NULL default '',
  `displayid` mediumint(8) unsigned NOT NULL default '0',
  `Quality` tinyint(3) unsigned NOT NULL default '0',
  `Flags` int(10) unsigned NOT NULL default '0',
  `BuyCount` tinyint(3) unsigned NOT NULL default '1',
  `BuyPrice` int(10) unsigned NOT NULL default '0',
  `SellPrice` int(10) unsigned NOT NULL default '0',
  `InventoryType` tinyint(3) unsigned NOT NULL default '0',
  `AllowableClass` mediumint(9) NOT NULL default '-1',
  `AllowableRace` mediumint(9) NOT NULL default '-1',
  `ItemLevel` tinyint(3) unsigned NOT NULL default '0',
  `RequiredLevel` tinyint(3) unsigned NOT NULL default '0',
  `RequiredSkill` smallint(5) unsigned NOT NULL default '0',
  `RequiredSkillRank` smallint(5) unsigned NOT NULL default '0',
  `requiredspell` mediumint(8) unsigned NOT NULL default '0',
  `requiredhonorrank` mediumint(8) unsigned NOT NULL default '0',
  `RequiredCityRank` mediumint(8) unsigned NOT NULL default '0',
  `RequiredReputationFaction` smallint(5) unsigned NOT NULL default '0',
  `RequiredReputationRank` smallint(5) unsigned NOT NULL default '0',
  `maxcount` smallint(5) unsigned NOT NULL default '0',
  `stackable` smallint(5) unsigned NOT NULL default '1',
  `ContainerSlots` tinyint(3) unsigned NOT NULL default '0',
  `stat_type1` tinyint(3) unsigned NOT NULL default '0',
  `stat_value1` smallint(6) NOT NULL default '0',
  `stat_type2` tinyint(3) unsigned NOT NULL default '0',
  `stat_value2` smallint(6) NOT NULL default '0',
  `stat_type3` tinyint(3) unsigned NOT NULL default '0',
  `stat_value3` smallint(6) NOT NULL default '0',
  `stat_type4` tinyint(3) unsigned NOT NULL default '0',
  `stat_value4` smallint(6) NOT NULL default '0',
  `stat_type5` tinyint(3) unsigned NOT NULL default '0',
  `stat_value5` smallint(6) NOT NULL default '0',
  `stat_type6` tinyint(3) unsigned NOT NULL default '0',
  `stat_value6` smallint(6) NOT NULL default '0',
  `stat_type7` tinyint(3) unsigned NOT NULL default '0',
  `stat_value7` smallint(6) NOT NULL default '0',
  `stat_type8` tinyint(3) unsigned NOT NULL default '0',
  `stat_value8` smallint(6) NOT NULL default '0',
  `stat_type9` tinyint(3) unsigned NOT NULL default '0',
  `stat_value9` smallint(6) NOT NULL default '0',
  `stat_type10` tinyint(3) unsigned NOT NULL default '0',
  `stat_value10` smallint(6) NOT NULL default '0',
  `dmg_min1` float NOT NULL default '0',
  `dmg_max1` float NOT NULL default '0',
  `dmg_type1` tinyint(3) unsigned NOT NULL default '0',
  `dmg_min2` float NOT NULL default '0',
  `dmg_max2` float NOT NULL default '0',
  `dmg_type2` tinyint(3) unsigned NOT NULL default '0',
  `dmg_min3` float NOT NULL default '0',
  `dmg_max3` float NOT NULL default '0',
  `dmg_type3` tinyint(3) unsigned NOT NULL default '0',
  `dmg_min4` float NOT NULL default '0',
  `dmg_max4` float NOT NULL default '0',
  `dmg_type4` tinyint(3) unsigned NOT NULL default '0',
  `dmg_min5` float NOT NULL default '0',
  `dmg_max5` float NOT NULL default '0',
  `dmg_type5` tinyint(3) unsigned NOT NULL default '0',
  `armor` smallint(5) unsigned NOT NULL default '0',
  `holy_res` tinyint(3) unsigned NOT NULL default '0',
  `fire_res` tinyint(3) unsigned NOT NULL default '0',
  `nature_res` tinyint(3) unsigned NOT NULL default '0',
  `frost_res` tinyint(3) unsigned NOT NULL default '0',
  `shadow_res` tinyint(3) unsigned NOT NULL default '0',
  `arcane_res` tinyint(3) unsigned NOT NULL default '0',
  `delay` smallint(5) unsigned NOT NULL default '1000',
  `ammo_type` tinyint(3) unsigned NOT NULL default '0',
  `RangedModRange` float NOT NULL default '0',
  `spellid_1` mediumint(8) unsigned NOT NULL default '0',
  `spelltrigger_1` tinyint(3) unsigned NOT NULL default '0',
  `spellcharges_1` tinyint(4) NOT NULL default '0',
  `spellppmRate_1` float NOT NULL default '0',
  `spellcooldown_1` int(11) NOT NULL default '-1',
  `spellcategory_1` smallint(5) unsigned NOT NULL default '0',
  `spellcategorycooldown_1` int(11) NOT NULL default '-1',
  `spellid_2` mediumint(8) unsigned NOT NULL default '0',
  `spelltrigger_2` tinyint(3) unsigned NOT NULL default '0',
  `spellcharges_2` tinyint(4) NOT NULL default '0',
  `spellppmRate_2` float NOT NULL default '0',
  `spellcooldown_2` int(11) NOT NULL default '-1',
  `spellcategory_2` smallint(5) unsigned NOT NULL default '0',
  `spellcategorycooldown_2` int(11) NOT NULL default '-1',
  `spellid_3` mediumint(8) unsigned NOT NULL default '0',
  `spelltrigger_3` tinyint(3) unsigned NOT NULL default '0',
  `spellcharges_3` tinyint(4) NOT NULL default '0',
  `spellppmRate_3` float NOT NULL default '0',
  `spellcooldown_3` int(11) NOT NULL default '-1',
  `spellcategory_3` smallint(5) unsigned NOT NULL default '0',
  `spellcategorycooldown_3` int(11) NOT NULL default '-1',
  `spellid_4` mediumint(8) unsigned NOT NULL default '0',
  `spelltrigger_4` tinyint(3) unsigned NOT NULL default '0',
  `spellcharges_4` tinyint(4) NOT NULL default '0',
  `spellppmRate_4` float NOT NULL default '0',
  `spellcooldown_4` int(11) NOT NULL default '-1',
  `spellcategory_4` smallint(5) unsigned NOT NULL default '0',
  `spellcategorycooldown_4` int(11) NOT NULL default '-1',
  `spellid_5` mediumint(8) unsigned NOT NULL default '0',
  `spelltrigger_5` tinyint(3) unsigned NOT NULL default '0',
  `spellcharges_5` tinyint(4) NOT NULL default '0',
  `spellppmRate_5` float NOT NULL default '0',
  `spellcooldown_5` int(11) NOT NULL default '-1',
  `spellcategory_5` smallint(5) unsigned NOT NULL default '0',
  `spellcategorycooldown_5` int(11) NOT NULL default '-1',
  `bonding` tinyint(3) unsigned NOT NULL default '0',
  `description` varchar(255) NOT NULL default '',
  `PageText` mediumint(8) unsigned NOT NULL default '0',
  `LanguageID` tinyint(3) unsigned NOT NULL default '0',
  `PageMaterial` tinyint(3) unsigned NOT NULL default '0',
  `startquest` mediumint(8) unsigned NOT NULL default '0',
  `lockid` mediumint(8) unsigned NOT NULL default '0',
  `Material` tinyint(4) NOT NULL default '0',
  `sheath` tinyint(3) unsigned NOT NULL default '0',
  `RandomProperty` mediumint(8) unsigned NOT NULL default '0',
  `RandomSuffix` mediumint(8) unsigned NOT NULL default '0',
  `block` mediumint(8) unsigned NOT NULL default '0',
  `itemset` mediumint(8) unsigned NOT NULL default '0',
  `MaxDurability` smallint(5) unsigned NOT NULL default '0',
  `area` mediumint(8) unsigned NOT NULL default '0',
  `Map` smallint(6) NOT NULL default '0',
  `BagFamily` mediumint(9) NOT NULL default '0',
  `TotemCategory` tinyint(4) NOT NULL default '0',
  `socketColor_1` tinyint(4) NOT NULL default '0',
  `socketContent_1` mediumint(9) NOT NULL default '0',
  `socketColor_2` tinyint(4) NOT NULL default '0',
  `socketContent_2` mediumint(9) NOT NULL default '0',
  `socketColor_3` tinyint(4) NOT NULL default '0',
  `socketContent_3` mediumint(9) NOT NULL default '0',
  `socketBonus` mediumint(9) NOT NULL default '0',
  `GemProperties` mediumint(9) NOT NULL default '0',
  `RequiredDisenchantSkill` smallint(6) NOT NULL default '-1',
  `ArmorDamageModifier` float NOT NULL default '0',
  `ScriptName` varchar(64) NOT NULL default '',
  `DisenchantID` mediumint(8) unsigned NOT NULL default '0',
  `FoodType` tinyint(3) unsigned NOT NULL default '0',
  `minMoneyLoot` int(10) unsigned NOT NULL default '0',
  `maxMoneyLoot` int(10) unsigned NOT NULL default '0',
  `Duration` int(11) NOT NULL default '0' COMMENT 'Duration in seconds. Negative value means realtime, postive value ingame time',
  PRIMARY KEY  (`entry`),
  KEY `idx_name` (`name`),
  KEY `items_index` (`class`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Item System';

--
-- Dumping data for table `item_template`
--

LOCK TABLES `item_template` WRITE;
/*!40000 ALTER TABLE `item_template` DISABLE KEYS */;
/*!40000 ALTER TABLE `item_template` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `locales_creature`
--

DROP TABLE IF EXISTS `locales_creature`;
CREATE TABLE `locales_creature` (
  `entry` mediumint(8) unsigned NOT NULL default '0',
  `name_loc1` varchar(100) NOT NULL default '',
  `name_loc2` varchar(100) NOT NULL default '',
  `name_loc3` varchar(100) NOT NULL default '',
  `name_loc4` varchar(100) NOT NULL default '',
  `name_loc5` varchar(100) NOT NULL default '',
  `name_loc6` varchar(100) NOT NULL default '',
  `name_loc7` varchar(100) NOT NULL default '',
  `name_loc8` varchar(100) NOT NULL default '',
  `subname_loc1` varchar(100) default NULL,
  `subname_loc2` varchar(100) default NULL,
  `subname_loc3` varchar(100) default NULL,
  `subname_loc4` varchar(100) default NULL,
  `subname_loc5` varchar(100) default NULL,
  `subname_loc6` varchar(100) default NULL,
  `subname_loc7` varchar(100) default NULL,
  `subname_loc8` varchar(100) default NULL,
  PRIMARY KEY  (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `locales_creature`
--

LOCK TABLES `locales_creature` WRITE;
/*!40000 ALTER TABLE `locales_creature` DISABLE KEYS */;
/*!40000 ALTER TABLE `locales_creature` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `locales_gameobject`
--

DROP TABLE IF EXISTS `locales_gameobject`;
CREATE TABLE `locales_gameobject` (
  `entry` mediumint(8) unsigned NOT NULL default '0',
  `name_loc1` varchar(100) NOT NULL default '',
  `name_loc2` varchar(100) NOT NULL default '',
  `name_loc3` varchar(100) NOT NULL default '',
  `name_loc4` varchar(100) NOT NULL default '',
  `name_loc5` varchar(100) NOT NULL default '',
  `name_loc6` varchar(100) NOT NULL default '',
  `name_loc7` varchar(100) NOT NULL default '',
  `name_loc8` varchar(100) NOT NULL default '',
  `castbarcaption_loc1` varchar(100) NOT NULL default '',
  `castbarcaption_loc2` varchar(100) NOT NULL default '',
  `castbarcaption_loc3` varchar(100) NOT NULL default '',
  `castbarcaption_loc4` varchar(100) NOT NULL default '',
  `castbarcaption_loc5` varchar(100) NOT NULL default '',
  `castbarcaption_loc6` varchar(100) NOT NULL default '',
  `castbarcaption_loc7` varchar(100) NOT NULL default '',
  `castbarcaption_loc8` varchar(100) NOT NULL default '',
  PRIMARY KEY  (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `locales_gameobject`
--

LOCK TABLES `locales_gameobject` WRITE;
/*!40000 ALTER TABLE `locales_gameobject` DISABLE KEYS */;
/*!40000 ALTER TABLE `locales_gameobject` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `locales_item`
--

DROP TABLE IF EXISTS `locales_item`;
CREATE TABLE `locales_item` (
  `entry` mediumint(8) unsigned NOT NULL default '0',
  `name_loc1` varchar(100) NOT NULL default '',
  `name_loc2` varchar(100) NOT NULL default '',
  `name_loc3` varchar(100) NOT NULL default '',
  `name_loc4` varchar(100) NOT NULL default '',
  `name_loc5` varchar(100) NOT NULL default '',
  `name_loc6` varchar(100) NOT NULL default '',
  `name_loc7` varchar(100) NOT NULL default '',
  `name_loc8` varchar(100) NOT NULL default '',
  `description_loc1` varchar(255) default NULL,
  `description_loc2` varchar(255) default NULL,
  `description_loc3` varchar(255) default NULL,
  `description_loc4` varchar(255) default NULL,
  `description_loc5` varchar(255) default NULL,
  `description_loc6` varchar(255) default NULL,
  `description_loc7` varchar(255) default NULL,
  `description_loc8` varchar(255) default NULL,
  PRIMARY KEY  (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `locales_item`
--

LOCK TABLES `locales_item` WRITE;
/*!40000 ALTER TABLE `locales_item` DISABLE KEYS */;
/*!40000 ALTER TABLE `locales_item` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `locales_npc_option`
--

DROP TABLE IF EXISTS `locales_npc_option`;
CREATE TABLE `locales_npc_option` (
  `entry` mediumint(8) unsigned NOT NULL default '0',
  `option_text_loc1` text,
  `option_text_loc2` text,
  `option_text_loc3` text,
  `option_text_loc4` text,
  `option_text_loc5` text,
  `option_text_loc6` text,
  `option_text_loc7` text,
  `option_text_loc8` text,
  `box_text_loc1` text,
  `box_text_loc2` text,
  `box_text_loc3` text,
  `box_text_loc4` text,
  `box_text_loc5` text,
  `box_text_loc6` text,
  `box_text_loc7` text,
  `box_text_loc8` text,
  PRIMARY KEY  (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `locales_npc_option`
--

LOCK TABLES `locales_npc_option` WRITE;
/*!40000 ALTER TABLE `locales_npc_option` DISABLE KEYS */;
/*!40000 ALTER TABLE `locales_npc_option` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `locales_npc_text`
--

DROP TABLE IF EXISTS `locales_npc_text`;
CREATE TABLE `locales_npc_text` (
  `entry` mediumint(8) unsigned NOT NULL default '0',
  `Text0_0_loc1` longtext,
  `Text0_0_loc2` longtext,
  `Text0_0_loc3` longtext,
  `Text0_0_loc4` longtext,
  `Text0_0_loc5` longtext,
  `Text0_0_loc6` longtext,
  `Text0_0_loc7` longtext,
  `Text0_0_loc8` longtext,
  `Text0_1_loc1` longtext,
  `Text0_1_loc2` longtext,
  `Text0_1_loc3` longtext,
  `Text0_1_loc4` longtext,
  `Text0_1_loc5` longtext,
  `Text0_1_loc6` longtext,
  `Text0_1_loc7` longtext,
  `Text0_1_loc8` longtext,
  `Text1_0_loc1` longtext,
  `Text1_0_loc2` longtext,
  `Text1_0_loc3` longtext,
  `Text1_0_loc4` longtext,
  `Text1_0_loc5` longtext,
  `Text1_0_loc6` longtext,
  `Text1_0_loc7` longtext,
  `Text1_0_loc8` longtext,
  `Text1_1_loc1` longtext,
  `Text1_1_loc2` longtext,
  `Text1_1_loc3` longtext,
  `Text1_1_loc4` longtext,
  `Text1_1_loc5` longtext,
  `Text1_1_loc6` longtext,
  `Text1_1_loc7` longtext,
  `Text1_1_loc8` longtext,
  `Text2_0_loc1` longtext,
  `Text2_0_loc2` longtext,
  `Text2_0_loc3` longtext,
  `Text2_0_loc4` longtext,
  `Text2_0_loc5` longtext,
  `Text2_0_loc6` longtext,
  `Text2_0_loc7` longtext,
  `Text2_0_loc8` longtext,
  `Text2_1_loc1` longtext,
  `Text2_1_loc2` longtext,
  `Text2_1_loc3` longtext,
  `Text2_1_loc4` longtext,
  `Text2_1_loc5` longtext,
  `Text2_1_loc6` longtext,
  `Text2_1_loc7` longtext,
  `Text2_1_loc8` longtext,
  `Text3_0_loc1` longtext,
  `Text3_0_loc2` longtext,
  `Text3_0_loc3` longtext,
  `Text3_0_loc4` longtext,
  `Text3_0_loc5` longtext,
  `Text3_0_loc6` longtext,
  `Text3_0_loc7` longtext,
  `Text3_0_loc8` longtext,
  `Text3_1_loc1` longtext,
  `Text3_1_loc2` longtext,
  `Text3_1_loc3` longtext,
  `Text3_1_loc4` longtext,
  `Text3_1_loc5` longtext,
  `Text3_1_loc6` longtext,
  `Text3_1_loc7` longtext,
  `Text3_1_loc8` longtext,
  `Text4_0_loc1` longtext,
  `Text4_0_loc2` longtext,
  `Text4_0_loc3` longtext,
  `Text4_0_loc4` longtext,
  `Text4_0_loc5` longtext,
  `Text4_0_loc6` longtext,
  `Text4_0_loc7` longtext,
  `Text4_0_loc8` longtext,
  `Text4_1_loc1` longtext,
  `Text4_1_loc2` longtext,
  `Text4_1_loc3` longtext,
  `Text4_1_loc4` longtext,
  `Text4_1_loc5` longtext,
  `Text4_1_loc6` longtext,
  `Text4_1_loc7` longtext,
  `Text4_1_loc8` longtext,
  `Text5_0_loc1` longtext,
  `Text5_0_loc2` longtext,
  `Text5_0_loc3` longtext,
  `Text5_0_loc4` longtext,
  `Text5_0_loc5` longtext,
  `Text5_0_loc6` longtext,
  `Text5_0_loc7` longtext,
  `Text5_0_loc8` longtext,
  `Text5_1_loc1` longtext,
  `Text5_1_loc2` longtext,
  `Text5_1_loc3` longtext,
  `Text5_1_loc4` longtext,
  `Text5_1_loc5` longtext,
  `Text5_1_loc6` longtext,
  `Text5_1_loc7` longtext,
  `Text5_1_loc8` longtext,
  `Text6_0_loc1` longtext,
  `Text6_0_loc2` longtext,
  `Text6_0_loc3` longtext,
  `Text6_0_loc4` longtext,
  `Text6_0_loc5` longtext,
  `Text6_0_loc6` longtext,
  `Text6_0_loc7` longtext,
  `Text6_0_loc8` longtext,
  `Text6_1_loc1` longtext,
  `Text6_1_loc2` longtext,
  `Text6_1_loc3` longtext,
  `Text6_1_loc4` longtext,
  `Text6_1_loc5` longtext,
  `Text6_1_loc6` longtext,
  `Text6_1_loc7` longtext,
  `Text6_1_loc8` longtext,
  `Text7_0_loc1` longtext,
  `Text7_0_loc2` longtext,
  `Text7_0_loc3` longtext,
  `Text7_0_loc4` longtext,
  `Text7_0_loc5` longtext,
  `Text7_0_loc6` longtext,
  `Text7_0_loc7` longtext,
  `Text7_0_loc8` longtext,
  `Text7_1_loc1` longtext,
  `Text7_1_loc2` longtext,
  `Text7_1_loc3` longtext,
  `Text7_1_loc4` longtext,
  `Text7_1_loc5` longtext,
  `Text7_1_loc6` longtext,
  `Text7_1_loc7` longtext,
  `Text7_1_loc8` longtext,
  PRIMARY KEY  (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `locales_npc_text`
--

LOCK TABLES `locales_npc_text` WRITE;
/*!40000 ALTER TABLE `locales_npc_text` DISABLE KEYS */;
/*!40000 ALTER TABLE `locales_npc_text` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `locales_page_text`
--

DROP TABLE IF EXISTS `locales_page_text`;
CREATE TABLE `locales_page_text` (
  `entry` mediumint(8) unsigned NOT NULL default '0',
  `Text_loc1` longtext,
  `Text_loc2` longtext,
  `Text_loc3` longtext,
  `Text_loc4` longtext,
  `Text_loc5` longtext,
  `Text_loc6` longtext,
  `Text_loc7` longtext,
  `Text_loc8` longtext,
  PRIMARY KEY  (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `locales_page_text`
--

LOCK TABLES `locales_page_text` WRITE;
/*!40000 ALTER TABLE `locales_page_text` DISABLE KEYS */;
/*!40000 ALTER TABLE `locales_page_text` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `locales_quest`
--

DROP TABLE IF EXISTS `locales_quest`;
CREATE TABLE `locales_quest` (
  `entry` mediumint(8) unsigned NOT NULL default '0',
  `Title_loc1` text,
  `Title_loc2` text,
  `Title_loc3` text,
  `Title_loc4` text,
  `Title_loc5` text,
  `Title_loc6` text,
  `Title_loc7` text,
  `Title_loc8` text,
  `Details_loc1` text,
  `Details_loc2` text,
  `Details_loc3` text,
  `Details_loc4` text,
  `Details_loc5` text,
  `Details_loc6` text,
  `Details_loc7` text,
  `Details_loc8` text,
  `Objectives_loc1` text,
  `Objectives_loc2` text,
  `Objectives_loc3` text,
  `Objectives_loc4` text,
  `Objectives_loc5` text,
  `Objectives_loc6` text,
  `Objectives_loc7` text,
  `Objectives_loc8` text,
  `OfferRewardText_loc1` text,
  `OfferRewardText_loc2` text,
  `OfferRewardText_loc3` text,
  `OfferRewardText_loc4` text,
  `OfferRewardText_loc5` text,
  `OfferRewardText_loc6` text,
  `OfferRewardText_loc7` text,
  `OfferRewardText_loc8` text,
  `RequestItemsText_loc1` text,
  `RequestItemsText_loc2` text,
  `RequestItemsText_loc3` text,
  `RequestItemsText_loc4` text,
  `RequestItemsText_loc5` text,
  `RequestItemsText_loc6` text,
  `RequestItemsText_loc7` text,
  `RequestItemsText_loc8` text,
  `EndText_loc1` text,
  `EndText_loc2` text,
  `EndText_loc3` text,
  `EndText_loc4` text,
  `EndText_loc5` text,
  `EndText_loc6` text,
  `EndText_loc7` text,
  `EndText_loc8` text,
  `ObjectiveText1_loc1` text,
  `ObjectiveText1_loc2` text,
  `ObjectiveText1_loc3` text,
  `ObjectiveText1_loc4` text,
  `ObjectiveText1_loc5` text,
  `ObjectiveText1_loc6` text,
  `ObjectiveText1_loc7` text,
  `ObjectiveText1_loc8` text,
  `ObjectiveText2_loc1` text,
  `ObjectiveText2_loc2` text,
  `ObjectiveText2_loc3` text,
  `ObjectiveText2_loc4` text,
  `ObjectiveText2_loc5` text,
  `ObjectiveText2_loc6` text,
  `ObjectiveText2_loc7` text,
  `ObjectiveText2_loc8` text,
  `ObjectiveText3_loc1` text,
  `ObjectiveText3_loc2` text,
  `ObjectiveText3_loc3` text,
  `ObjectiveText3_loc4` text,
  `ObjectiveText3_loc5` text,
  `ObjectiveText3_loc6` text,
  `ObjectiveText3_loc7` text,
  `ObjectiveText3_loc8` text,
  `ObjectiveText4_loc1` text,
  `ObjectiveText4_loc2` text,
  `ObjectiveText4_loc3` text,
  `ObjectiveText4_loc4` text,
  `ObjectiveText4_loc5` text,
  `ObjectiveText4_loc6` text,
  `ObjectiveText4_loc7` text,
  `ObjectiveText4_loc8` text,
  PRIMARY KEY  (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `locales_quest`
--

LOCK TABLES `locales_quest` WRITE;
/*!40000 ALTER TABLE `locales_quest` DISABLE KEYS */;
/*!40000 ALTER TABLE `locales_quest` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `mail_loot_template`
--

DROP TABLE IF EXISTS `mail_loot_template`;
CREATE TABLE `mail_loot_template` (
  `entry` mediumint(8) unsigned NOT NULL default '0',
  `item` mediumint(8) unsigned NOT NULL default '0',
  `ChanceOrQuestChance` float NOT NULL default '100',
  `groupid` tinyint(3) unsigned NOT NULL default '0',
  `mincountOrRef` mediumint(9) NOT NULL default '1',
  `maxcount` tinyint(3) unsigned NOT NULL default '1',
  `lootcondition` tinyint(3) unsigned NOT NULL default '0',
  `condition_value1` mediumint(8) unsigned NOT NULL default '0',
  `condition_value2` mediumint(8) unsigned NOT NULL default '0',
  PRIMARY KEY (`entry`,`item`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Loot System';

--
-- Dumping data for table `mail_loot_template`
--

LOCK TABLES `mail_loot_template` WRITE;
/*!40000 ALTER TABLE `mail_loot_template` DISABLE KEYS */;
/*!40000 ALTER TABLE `mail_loot_template` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `npc_gossip`
--

DROP TABLE IF EXISTS `npc_gossip`;
CREATE TABLE `npc_gossip` (
  `npc_guid` int(10) unsigned NOT NULL default '0',
  `textid` mediumint(8) unsigned NOT NULL default '0',
  PRIMARY KEY  (`npc_guid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `npc_gossip`
--

LOCK TABLES `npc_gossip` WRITE;
/*!40000 ALTER TABLE `npc_gossip` DISABLE KEYS */;
/*!40000 ALTER TABLE `npc_gossip` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `npc_gossip_textid`
--

DROP TABLE IF EXISTS `npc_gossip_textid`;
CREATE TABLE `npc_gossip_textid` (
  `zoneid` smallint(5) unsigned NOT NULL default '0',
  `action` smallint(5) unsigned NOT NULL default '0',
  `textid` mediumint(8) unsigned NOT NULL default '0',
  KEY `zoneid` (`zoneid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `npc_gossip_textid`
--

LOCK TABLES `npc_gossip_textid` WRITE;
/*!40000 ALTER TABLE `npc_gossip_textid` DISABLE KEYS */;
/*!40000 ALTER TABLE `npc_gossip_textid` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `npc_option`
--

DROP TABLE IF EXISTS `npc_option`;
CREATE TABLE `npc_option` (
  `id` tinyint(3) unsigned NOT NULL default '0',
  `gossip_id` tinyint(3) unsigned NOT NULL default '0',
  `npcflag` int(10) unsigned NOT NULL default '0',
  `icon` tinyint(3) unsigned NOT NULL default '0',
  `action` tinyint(3) unsigned NOT NULL default '0',
  `box_money` int(10) unsigned NOT NULL default '0',
  `coded` tinyint(3) unsigned NOT NULL default '0',
  `option_text` text,
  `box_text` text,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `npc_option`
--

LOCK TABLES `npc_option` WRITE;
/*!40000 ALTER TABLE `npc_option` DISABLE KEYS */;
/*!40000 ALTER TABLE `npc_option` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `npc_text`
--

DROP TABLE IF EXISTS `npc_text`;
CREATE TABLE `npc_text` (
  `ID` mediumint(8) unsigned NOT NULL default '0',
  `text0_0` longtext,
  `text0_1` longtext,
  `lang0` tinyint(3) unsigned NOT NULL default '0',
  `prob0` float NOT NULL default '0',
  `em0_0` smallint(5) unsigned NOT NULL default '0',
  `em0_1` smallint(5) unsigned NOT NULL default '0',
  `em0_2` smallint(5) unsigned NOT NULL default '0',
  `em0_3` smallint(5) unsigned NOT NULL default '0',
  `em0_4` smallint(5) unsigned NOT NULL default '0',
  `em0_5` smallint(5) unsigned NOT NULL default '0',
  `text1_0` longtext,
  `text1_1` longtext,
  `lang1` tinyint(3) unsigned NOT NULL default '0',
  `prob1` float NOT NULL default '0',
  `em1_0` smallint(5) unsigned NOT NULL default '0',
  `em1_1` smallint(5) unsigned NOT NULL default '0',
  `em1_2` smallint(5) unsigned NOT NULL default '0',
  `em1_3` smallint(5) unsigned NOT NULL default '0',
  `em1_4` smallint(5) unsigned NOT NULL default '0',
  `em1_5` smallint(5) unsigned NOT NULL default '0',
  `text2_0` longtext,
  `text2_1` longtext,
  `lang2` tinyint(3) unsigned NOT NULL default '0',
  `prob2` float NOT NULL default '0',
  `em2_0` smallint(5) unsigned NOT NULL default '0',
  `em2_1` smallint(5) unsigned NOT NULL default '0',
  `em2_2` smallint(5) unsigned NOT NULL default '0',
  `em2_3` smallint(5) unsigned NOT NULL default '0',
  `em2_4` smallint(5) unsigned NOT NULL default '0',
  `em2_5` smallint(5) unsigned NOT NULL default '0',
  `text3_0` longtext,
  `text3_1` longtext,
  `lang3` tinyint(3) unsigned NOT NULL default '0',
  `prob3` float NOT NULL default '0',
  `em3_0` smallint(5) unsigned NOT NULL default '0',
  `em3_1` smallint(5) unsigned NOT NULL default '0',
  `em3_2` smallint(5) unsigned NOT NULL default '0',
  `em3_3` smallint(5) unsigned NOT NULL default '0',
  `em3_4` smallint(5) unsigned NOT NULL default '0',
  `em3_5` smallint(5) unsigned NOT NULL default '0',
  `text4_0` longtext,
  `text4_1` longtext,
  `lang4` tinyint(3) unsigned NOT NULL default '0',
  `prob4` float NOT NULL default '0',
  `em4_0` smallint(5) unsigned NOT NULL default '0',
  `em4_1` smallint(5) unsigned NOT NULL default '0',
  `em4_2` smallint(5) unsigned NOT NULL default '0',
  `em4_3` smallint(5) unsigned NOT NULL default '0',
  `em4_4` smallint(5) unsigned NOT NULL default '0',
  `em4_5` smallint(5) unsigned NOT NULL default '0',
  `text5_0` longtext,
  `text5_1` longtext,
  `lang5` tinyint(3) unsigned NOT NULL default '0',
  `prob5` float NOT NULL default '0',
  `em5_0` smallint(5) unsigned NOT NULL default '0',
  `em5_1` smallint(5) unsigned NOT NULL default '0',
  `em5_2` smallint(5) unsigned NOT NULL default '0',
  `em5_3` smallint(5) unsigned NOT NULL default '0',
  `em5_4` smallint(5) unsigned NOT NULL default '0',
  `em5_5` smallint(5) unsigned NOT NULL default '0',
  `text6_0` longtext,
  `text6_1` longtext,
  `lang6` tinyint(3) unsigned NOT NULL default '0',
  `prob6` float NOT NULL default '0',
  `em6_0` smallint(5) unsigned NOT NULL default '0',
  `em6_1` smallint(5) unsigned NOT NULL default '0',
  `em6_2` smallint(5) unsigned NOT NULL default '0',
  `em6_3` smallint(5) unsigned NOT NULL default '0',
  `em6_4` smallint(5) unsigned NOT NULL default '0',
  `em6_5` smallint(5) unsigned NOT NULL default '0',
  `text7_0` longtext,
  `text7_1` longtext,
  `lang7` tinyint(3) unsigned NOT NULL default '0',
  `prob7` float NOT NULL default '0',
  `em7_0` smallint(5) unsigned NOT NULL default '0',
  `em7_1` smallint(5) unsigned NOT NULL default '0',
  `em7_2` smallint(5) unsigned NOT NULL default '0',
  `em7_3` smallint(5) unsigned NOT NULL default '0',
  `em7_4` smallint(5) unsigned NOT NULL default '0',
  `em7_5` smallint(5) unsigned NOT NULL default '0',
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `npc_text`
--

LOCK TABLES `npc_text` WRITE;
/*!40000 ALTER TABLE `npc_text` DISABLE KEYS */;
/*!40000 ALTER TABLE `npc_text` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `npc_trainer`
--

DROP TABLE IF EXISTS `npc_trainer`;
CREATE TABLE `npc_trainer` (
  `entry` mediumint(8) unsigned NOT NULL default '0',
  `spell` mediumint(8) unsigned NOT NULL default '0',
  `spellcost` int(10) unsigned NOT NULL default '0',
  `reqskill` smallint(5) unsigned NOT NULL default '0',
  `reqskillvalue` smallint(5) unsigned NOT NULL default '0',
  `reqlevel` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`entry`,`spell`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `npc_trainer`
--

LOCK TABLES `npc_trainer` WRITE;
/*!40000 ALTER TABLE `npc_trainer` DISABLE KEYS */;
/*!40000 ALTER TABLE `npc_trainer` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `npc_vendor`
--

DROP TABLE IF EXISTS `npc_vendor`;
CREATE TABLE `npc_vendor` (
  `entry` mediumint(8) unsigned NOT NULL default '0',
  `item` mediumint(8) unsigned NOT NULL default '0',
  `maxcount` tinyint(3) unsigned NOT NULL default '0',
  `incrtime` int(10) unsigned NOT NULL default '0',
  `ExtendedCost` mediumint(8) unsigned NOT NULL default '0',
  PRIMARY KEY  (`entry`,`item`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Npc System';

--
-- Dumping data for table `npc_vendor`
--

LOCK TABLES `npc_vendor` WRITE;
/*!40000 ALTER TABLE `npc_vendor` DISABLE KEYS */;
/*!40000 ALTER TABLE `npc_vendor` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `page_text`
--

DROP TABLE IF EXISTS `page_text`;
CREATE TABLE `page_text` (
  `entry` mediumint(8) unsigned NOT NULL default '0',
  `text` longtext NOT NULL,
  `next_page` mediumint(8) unsigned NOT NULL default '0',
  PRIMARY KEY  (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='Item System';

--
-- Dumping data for table `page_text`
--

LOCK TABLES `page_text` WRITE;
/*!40000 ALTER TABLE `page_text` DISABLE KEYS */;
/*!40000 ALTER TABLE `page_text` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `pet_levelstats`
--

DROP TABLE IF EXISTS `pet_levelstats`;
CREATE TABLE `pet_levelstats` (
  `creature_entry` mediumint(8) unsigned NOT NULL,
  `level` tinyint(3) unsigned NOT NULL,
  `hp` smallint(5) unsigned NOT NULL,
  `mana` smallint(5) unsigned NOT NULL,
  `armor` int(10) unsigned NOT NULL default '0',
  `str` smallint(5) unsigned NOT NULL,
  `agi` smallint(5) unsigned NOT NULL,
  `sta` smallint(5) unsigned NOT NULL,
  `inte` smallint(5) unsigned NOT NULL,
  `spi` smallint(5) unsigned NOT NULL,
  PRIMARY KEY  (`creature_entry`,`level`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 PACK_KEYS=0 COMMENT='Stores pet levels stats.';

--
-- Dumping data for table `pet_levelstats`
--

LOCK TABLES `pet_levelstats` WRITE;
/*!40000 ALTER TABLE `pet_levelstats` DISABLE KEYS */;
/*!40000 ALTER TABLE `pet_levelstats` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `pet_name_generation`
--

DROP TABLE IF EXISTS `pet_name_generation`;
CREATE TABLE `pet_name_generation` (
  `id` mediumint(8) unsigned NOT NULL auto_increment,
  `word` tinytext NOT NULL,
  `entry` mediumint(8) unsigned NOT NULL default '0',
  `half` tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `pet_name_generation`
--

LOCK TABLES `pet_name_generation` WRITE;
/*!40000 ALTER TABLE `pet_name_generation` DISABLE KEYS */;
/*!40000 ALTER TABLE `pet_name_generation` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `petcreateinfo_spell`
--

DROP TABLE IF EXISTS `petcreateinfo_spell`;
CREATE TABLE `petcreateinfo_spell` (
  `entry` mediumint(8) unsigned NOT NULL default '0',
  `Spell1` mediumint(8) unsigned NOT NULL default '0',
  `Spell2` mediumint(8) unsigned NOT NULL default '0',
  `Spell3` mediumint(8) unsigned NOT NULL default '0',
  `Spell4` mediumint(8) unsigned NOT NULL default '0',
  PRIMARY KEY  (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Pet Create Spells';

--
-- Dumping data for table `petcreateinfo_spell`
--

LOCK TABLES `petcreateinfo_spell` WRITE;
/*!40000 ALTER TABLE `petcreateinfo_spell` DISABLE KEYS */;
/*!40000 ALTER TABLE `petcreateinfo_spell` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `pickpocketing_loot_template`
--

DROP TABLE IF EXISTS `pickpocketing_loot_template`;
CREATE TABLE `pickpocketing_loot_template` (
  `entry` mediumint(8) unsigned NOT NULL default '0',
  `item` mediumint(8) unsigned NOT NULL default '0',
  `ChanceOrQuestChance` float NOT NULL default '100',
  `groupid` tinyint(3) unsigned NOT NULL default '0',
  `mincountOrRef` mediumint(9) NOT NULL default '1',
  `maxcount` tinyint(3) unsigned NOT NULL default '1',
  `lootcondition` tinyint(3) unsigned NOT NULL default '0',
  `condition_value1` mediumint(8) unsigned NOT NULL default '0',
  `condition_value2` mediumint(8) unsigned NOT NULL default '0',
  PRIMARY KEY  (`entry`,`item`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Loot System';

--
-- Dumping data for table `pickpocketing_loot_template`
--

LOCK TABLES `pickpocketing_loot_template` WRITE;
/*!40000 ALTER TABLE `pickpocketing_loot_template` DISABLE KEYS */;
/*!40000 ALTER TABLE `pickpocketing_loot_template` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `player_classlevelstats`
--

DROP TABLE IF EXISTS `player_classlevelstats`;
CREATE TABLE `player_classlevelstats` (
  `class` tinyint(3) unsigned NOT NULL,
  `level` tinyint(3) unsigned NOT NULL,
  `basehp` smallint(5) unsigned NOT NULL,
  `basemana` smallint(5) unsigned NOT NULL,
  PRIMARY KEY  (`class`,`level`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 PACK_KEYS=0 COMMENT='Stores levels stats.';

--
-- Dumping data for table `player_classlevelstats`
--

LOCK TABLES `player_classlevelstats` WRITE;
/*!40000 ALTER TABLE `player_classlevelstats` DISABLE KEYS */;
/*!40000 ALTER TABLE `player_classlevelstats` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `player_levelstats`
--

DROP TABLE IF EXISTS `player_levelstats`;
CREATE TABLE `player_levelstats` (
  `race` tinyint(3) unsigned NOT NULL,
  `class` tinyint(3) unsigned NOT NULL,
  `level` tinyint(3) unsigned NOT NULL,
  `str` tinyint(3) unsigned NOT NULL,
  `agi` tinyint(3) unsigned NOT NULL,
  `sta` tinyint(3) unsigned NOT NULL,
  `inte` tinyint(3) unsigned NOT NULL,
  `spi` tinyint(3) unsigned NOT NULL,
  PRIMARY KEY  (`race`,`class`,`level`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 PACK_KEYS=0 COMMENT='Stores levels stats.';

--
-- Dumping data for table `player_levelstats`
--

LOCK TABLES `player_levelstats` WRITE;
/*!40000 ALTER TABLE `player_levelstats` DISABLE KEYS */;
/*!40000 ALTER TABLE `player_levelstats` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `playercreateinfo`
--

DROP TABLE IF EXISTS `playercreateinfo`;
CREATE TABLE `playercreateinfo` (
  `race` tinyint(3) unsigned NOT NULL default '0',
  `class` tinyint(3) unsigned NOT NULL default '0',
  `map` smallint(5) unsigned NOT NULL default '0',
  `zone` mediumint(8) unsigned NOT NULL default '0',
  `position_x` float NOT NULL default '0',
  `position_y` float NOT NULL default '0',
  `position_z` float NOT NULL default '0',
  `orientation` float NOT NULL DEFAULT '0',
  PRIMARY KEY  (`race`,`class`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `playercreateinfo`
--

LOCK TABLES `playercreateinfo` WRITE;
/*!40000 ALTER TABLE `playercreateinfo` DISABLE KEYS */;
/*!40000 ALTER TABLE `playercreateinfo` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `playercreateinfo_action`
--

DROP TABLE IF EXISTS `playercreateinfo_action`;
CREATE TABLE `playercreateinfo_action` (
  `race` tinyint(3) unsigned NOT NULL default '0',
  `class` tinyint(3) unsigned NOT NULL default '0',
  `button` smallint(5) unsigned NOT NULL default '0',
  `action` smallint(5) unsigned NOT NULL default '0',
  `type` smallint(5) unsigned NOT NULL default '0',
  `misc` smallint(5) unsigned NOT NULL default '0',
  KEY `playercreateinfo_race_class_index` (`race`,`class`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `playercreateinfo_action`
--

LOCK TABLES `playercreateinfo_action` WRITE;
/*!40000 ALTER TABLE `playercreateinfo_action` DISABLE KEYS */;
/*!40000 ALTER TABLE `playercreateinfo_action` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `playercreateinfo_item`
--

DROP TABLE IF EXISTS `playercreateinfo_item`;
CREATE TABLE `playercreateinfo_item` (
  `race` tinyint(3) unsigned NOT NULL default '0',
  `class` tinyint(3) unsigned NOT NULL default '0',
  `itemid` mediumint(8) unsigned NOT NULL default '0',
  `amount` tinyint(3) unsigned NOT NULL default '1',
  KEY `playercreateinfo_race_class_index` (`race`,`class`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `playercreateinfo_item`
--

LOCK TABLES `playercreateinfo_item` WRITE;
/*!40000 ALTER TABLE `playercreateinfo_item` DISABLE KEYS */;
/*!40000 ALTER TABLE `playercreateinfo_item` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `playercreateinfo_spell`
--

DROP TABLE IF EXISTS `playercreateinfo_spell`;
CREATE TABLE `playercreateinfo_spell` (
  `race` tinyint(3) unsigned NOT NULL default '0',
  `class` tinyint(3) unsigned NOT NULL default '0',
  `Spell` mediumint(8) unsigned NOT NULL default '0',
  `Note` varchar(255) default NULL,
  `Active` tinyint(3) unsigned NOT NULL default '1',
  PRIMARY KEY  (`race`,`class`,`Spell`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `playercreateinfo_spell`
--

LOCK TABLES `playercreateinfo_spell` WRITE;
/*!40000 ALTER TABLE `playercreateinfo_spell` DISABLE KEYS */;
/*!40000 ALTER TABLE `playercreateinfo_spell` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `playercreateinfo_spell_custom`
--

DROP TABLE IF EXISTS `playercreateinfo_spell_custom`;
CREATE TABLE `playercreateinfo_spell_custom` (
  `race` tinyint(3) unsigned NOT NULL default '0',
  `class` tinyint(3) unsigned NOT NULL default '0',
  `Spell` mediumint(8) unsigned NOT NULL default '0',
  `Note` varchar(255) default NULL,
  `Active` tinyint(3) unsigned NOT NULL default '1',
  PRIMARY KEY  (`race`,`class`,`Spell`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `playercreateinfo_spell_custom`
--

LOCK TABLES `playercreateinfo_spell_custom` WRITE;
/*!40000 ALTER TABLE `playercreateinfo_spell_custom` DISABLE KEYS */;
/*!40000 ALTER TABLE `playercreateinfo_spell_custom` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `prospecting_loot_template`
--

DROP TABLE IF EXISTS `prospecting_loot_template`;
CREATE TABLE `prospecting_loot_template` (
  `entry` mediumint(8) unsigned NOT NULL default '0',
  `item` mediumint(8) unsigned NOT NULL default '0',
  `ChanceOrQuestChance` float NOT NULL default '100',
  `groupid` tinyint(3) unsigned NOT NULL default '0',
  `mincountOrRef` mediumint(9) NOT NULL default '1',
  `maxcount` tinyint(3) unsigned NOT NULL default '1',
  `lootcondition` tinyint(3) unsigned NOT NULL default '0',
  `condition_value1` mediumint(8) unsigned NOT NULL default '0',
  `condition_value2` mediumint(8) unsigned NOT NULL default '0',
  PRIMARY KEY  (`entry`,`item`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Loot System';

--
-- Dumping data for table `prospecting_loot_template`
--

LOCK TABLES `prospecting_loot_template` WRITE;
/*!40000 ALTER TABLE `prospecting_loot_template` DISABLE KEYS */;
/*!40000 ALTER TABLE `prospecting_loot_template` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `quest_end_scripts`
--

DROP TABLE IF EXISTS `quest_end_scripts`;
CREATE TABLE `quest_end_scripts` (
  `id` mediumint(8) unsigned NOT NULL default '0',
  `delay` int(10) unsigned NOT NULL default '0',
  `command` mediumint(8) unsigned NOT NULL default '0',
  `datalong` mediumint(8) unsigned NOT NULL default '0',
  `datalong2` int(10) unsigned NOT NULL default '0',
  `datatext` text NOT NULL,
  `x` float NOT NULL default '0',
  `y` float NOT NULL default '0',
  `z` float NOT NULL default '0',
  `o` float NOT NULL default '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `quest_end_scripts`
--

LOCK TABLES `quest_end_scripts` WRITE;
/*!40000 ALTER TABLE `quest_end_scripts` DISABLE KEYS */;
/*!40000 ALTER TABLE `quest_end_scripts` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `quest_start_scripts`
--

DROP TABLE IF EXISTS `quest_start_scripts`;
CREATE TABLE `quest_start_scripts` (
  `id` mediumint(8) unsigned NOT NULL default '0',
  `delay` int(10) unsigned NOT NULL default '0',
  `command` mediumint(8) unsigned NOT NULL default '0',
  `datalong` mediumint(8) unsigned NOT NULL default '0',
  `datalong2` int(10) unsigned NOT NULL default '0',
  `datatext` text NOT NULL,
  `x` float NOT NULL default '0',
  `y` float NOT NULL default '0',
  `z` float NOT NULL default '0',
  `o` float NOT NULL default '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `quest_start_scripts`
--

LOCK TABLES `quest_start_scripts` WRITE;
/*!40000 ALTER TABLE `quest_start_scripts` DISABLE KEYS */;
/*!40000 ALTER TABLE `quest_start_scripts` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `quest_template`
--

DROP TABLE IF EXISTS `quest_template`;
CREATE TABLE `quest_template` (
  `entry` mediumint(8) unsigned NOT NULL default '0',
  `Method` tinyint(3) unsigned NOT NULL default '2',
  `ZoneOrSort` smallint(6) NOT NULL default '0',
  `SkillOrClass` smallint(6) NOT NULL default '0',
  `MinLevel` tinyint(3) unsigned NOT NULL default '0',
  `QuestLevel` smallint(6) NOT NULL default '1',
  `Type` smallint(5) unsigned NOT NULL default '0',
  `RequiredRaces` smallint(5) unsigned NOT NULL default '0',
  `RequiredSkillValue` smallint(5) unsigned NOT NULL default '0',
  `RepObjectiveFaction` smallint(5) unsigned NOT NULL default '0',
  `RepObjectiveValue` mediumint(9) NOT NULL default '0',
  `RequiredMinRepFaction` smallint(5) unsigned NOT NULL default '0',
  `RequiredMinRepValue` mediumint(9) NOT NULL default '0',
  `RequiredMaxRepFaction` smallint(5) unsigned NOT NULL default '0',
  `RequiredMaxRepValue` mediumint(9) NOT NULL default '0',
  `SuggestedPlayers` tinyint(3) unsigned NOT NULL default '0',
  `LimitTime` int(10) unsigned NOT NULL default '0',
  `QuestFlags` smallint(5) unsigned NOT NULL default '0',
  `SpecialFlags` tinyint(3) unsigned NOT NULL default '0',
  `CharTitleId` tinyint(3) unsigned NOT NULL default '0',
  `PrevQuestId` mediumint(9) NOT NULL default '0',
  `NextQuestId` mediumint(9) NOT NULL default '0',
  `ExclusiveGroup` mediumint(9) NOT NULL default '0',
  `NextQuestInChain` mediumint(8) unsigned NOT NULL default '0',
  `SrcItemId` mediumint(8) unsigned NOT NULL default '0',
  `SrcItemCount` tinyint(3) unsigned NOT NULL default '0',
  `SrcSpell` mediumint(8) unsigned NOT NULL default '0',
  `Title` text,
  `Details` text,
  `Objectives` text,
  `OfferRewardText` text,
  `RequestItemsText` text,
  `EndText` text,
  `ObjectiveText1` text,
  `ObjectiveText2` text,
  `ObjectiveText3` text,
  `ObjectiveText4` text,
  `ReqItemId1` mediumint(8) unsigned NOT NULL default '0',
  `ReqItemId2` mediumint(8) unsigned NOT NULL default '0',
  `ReqItemId3` mediumint(8) unsigned NOT NULL default '0',
  `ReqItemId4` mediumint(8) unsigned NOT NULL default '0',
  `ReqItemCount1` smallint(5) unsigned NOT NULL default '0',
  `ReqItemCount2` smallint(5) unsigned NOT NULL default '0',
  `ReqItemCount3` smallint(5) unsigned NOT NULL default '0',
  `ReqItemCount4` smallint(5) unsigned NOT NULL default '0',
  `ReqSourceId1` mediumint(8) unsigned NOT NULL default '0',
  `ReqSourceId2` mediumint(8) unsigned NOT NULL default '0',
  `ReqSourceId3` mediumint(8) unsigned NOT NULL default '0',
  `ReqSourceId4` mediumint(8) unsigned NOT NULL default '0',
  `ReqSourceCount1` smallint(5) unsigned NOT NULL default '0',
  `ReqSourceCount2` smallint(5) unsigned NOT NULL default '0',
  `ReqSourceCount3` smallint(5) unsigned NOT NULL default '0',
  `ReqSourceCount4` smallint(5) unsigned NOT NULL default '0',
  `ReqSourceRef1` tinyint(3) unsigned NOT NULL default '0',
  `ReqSourceRef2` tinyint(3) unsigned NOT NULL default '0',
  `ReqSourceRef3` tinyint(3) unsigned NOT NULL default '0',
  `ReqSourceRef4` tinyint(3) unsigned NOT NULL default '0',
  `ReqCreatureOrGOId1` mediumint(9) NOT NULL default '0',
  `ReqCreatureOrGOId2` mediumint(9) NOT NULL default '0',
  `ReqCreatureOrGOId3` mediumint(9) NOT NULL default '0',
  `ReqCreatureOrGOId4` mediumint(9) NOT NULL default '0',
  `ReqCreatureOrGOCount1` smallint(5) unsigned NOT NULL default '0',
  `ReqCreatureOrGOCount2` smallint(5) unsigned NOT NULL default '0',
  `ReqCreatureOrGOCount3` smallint(5) unsigned NOT NULL default '0',
  `ReqCreatureOrGOCount4` smallint(5) unsigned NOT NULL default '0',
  `ReqSpellCast1` mediumint(8) unsigned NOT NULL default '0',
  `ReqSpellCast2` mediumint(8) unsigned NOT NULL default '0',
  `ReqSpellCast3` mediumint(8) unsigned NOT NULL default '0',
  `ReqSpellCast4` mediumint(8) unsigned NOT NULL default '0',
  `RewChoiceItemId1` mediumint(8) unsigned NOT NULL default '0',
  `RewChoiceItemId2` mediumint(8) unsigned NOT NULL default '0',
  `RewChoiceItemId3` mediumint(8) unsigned NOT NULL default '0',
  `RewChoiceItemId4` mediumint(8) unsigned NOT NULL default '0',
  `RewChoiceItemId5` mediumint(8) unsigned NOT NULL default '0',
  `RewChoiceItemId6` mediumint(8) unsigned NOT NULL default '0',
  `RewChoiceItemCount1` smallint(5) unsigned NOT NULL default '0',
  `RewChoiceItemCount2` smallint(5) unsigned NOT NULL default '0',
  `RewChoiceItemCount3` smallint(5) unsigned NOT NULL default '0',
  `RewChoiceItemCount4` smallint(5) unsigned NOT NULL default '0',
  `RewChoiceItemCount5` smallint(5) unsigned NOT NULL default '0',
  `RewChoiceItemCount6` smallint(5) unsigned NOT NULL default '0',
  `RewItemId1` mediumint(8) unsigned NOT NULL default '0',
  `RewItemId2` mediumint(8) unsigned NOT NULL default '0',
  `RewItemId3` mediumint(8) unsigned NOT NULL default '0',
  `RewItemId4` mediumint(8) unsigned NOT NULL default '0',
  `RewItemCount1` smallint(5) unsigned NOT NULL default '0',
  `RewItemCount2` smallint(5) unsigned NOT NULL default '0',
  `RewItemCount3` smallint(5) unsigned NOT NULL default '0',
  `RewItemCount4` smallint(5) unsigned NOT NULL default '0',
  `RewRepFaction1` smallint(5) unsigned NOT NULL default '0' COMMENT 'faction id from Faction.dbc in this case',
  `RewRepFaction2` smallint(5) unsigned NOT NULL default '0' COMMENT 'faction id from Faction.dbc in this case',
  `RewRepFaction3` smallint(5) unsigned NOT NULL default '0' COMMENT 'faction id from Faction.dbc in this case',
  `RewRepFaction4` smallint(5) unsigned NOT NULL default '0' COMMENT 'faction id from Faction.dbc in this case',
  `RewRepFaction5` smallint(5) unsigned NOT NULL default '0' COMMENT 'faction id from Faction.dbc in this case',
  `RewRepValue1` mediumint(9) NOT NULL default '0',
  `RewRepValue2` mediumint(9) NOT NULL default '0',
  `RewRepValue3` mediumint(9) NOT NULL default '0',
  `RewRepValue4` mediumint(9) NOT NULL default '0',
  `RewRepValue5` mediumint(9) NOT NULL default '0',
  `RewHonorableKills` mediumint(9) unsigned NOT NULL default '0',
  `RewOrReqMoney` int(11) NOT NULL default '0',
  `RewMoneyMaxLevel` int(10) unsigned NOT NULL default '0',
  `RewSpell` mediumint(8) unsigned NOT NULL default '0',
  `RewSpellCast` mediumint(8) unsigned NOT NULL default '0',
  `RewMailTemplateId` mediumint(8) unsigned NOT NULL default '0',
  `RewMailDelaySecs` int(11) unsigned NOT NULL default '0',
  `PointMapId` smallint(5) unsigned NOT NULL default '0',
  `PointX` float NOT NULL default '0',
  `PointY` float NOT NULL default '0',
  `PointOpt` mediumint(8) unsigned NOT NULL default '0',
  `DetailsEmote1` smallint(5) unsigned NOT NULL default '0',
  `DetailsEmote2` smallint(5) unsigned NOT NULL default '0',
  `DetailsEmote3` smallint(5) unsigned NOT NULL default '0',
  `DetailsEmote4` smallint(5) unsigned NOT NULL default '0',
  `IncompleteEmote` smallint(5) unsigned NOT NULL default '0',
  `CompleteEmote` smallint(5) unsigned NOT NULL default '0',
  `OfferRewardEmote1` smallint(5) unsigned NOT NULL default '0',
  `OfferRewardEmote2` smallint(5) unsigned NOT NULL default '0',
  `OfferRewardEmote3` smallint(5) unsigned NOT NULL default '0',
  `OfferRewardEmote4` smallint(5) unsigned NOT NULL default '0',
  `StartScript` mediumint(8) unsigned NOT NULL default '0',
  `CompleteScript` mediumint(8) unsigned NOT NULL default '0',
  PRIMARY KEY  (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='Quest System';

--
-- Dumping data for table `quest_template`
--

LOCK TABLES `quest_template` WRITE;
/*!40000 ALTER TABLE `quest_template` DISABLE KEYS */;
/*!40000 ALTER TABLE `quest_template` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `reference_loot_template`
--

DROP TABLE IF EXISTS `reference_loot_template`;
CREATE TABLE `reference_loot_template` (
  `entry` mediumint(8) unsigned NOT NULL default '0',
  `item` mediumint(8) unsigned NOT NULL default '0',
  `ChanceOrQuestChance` float NOT NULL default '100',
  `groupid` tinyint(3) unsigned NOT NULL default '0',
  `mincountOrRef` mediumint(9) NOT NULL default '1',
  `maxcount` tinyint(3) unsigned NOT NULL default '1',
  `lootcondition` tinyint(3) unsigned NOT NULL default '0',
  `condition_value1` mediumint(8) unsigned NOT NULL default '0',
  `condition_value2` mediumint(8) unsigned NOT NULL default '0',
  PRIMARY KEY  (`entry`,`item`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Loot System';

--
-- Dumping data for table `reference_loot_template`
--

LOCK TABLES `reference_loot_template` WRITE;
/*!40000 ALTER TABLE `reference_loot_template` DISABLE KEYS */;
/*!40000 ALTER TABLE `reference_loot_template` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `reserved_name`
--

DROP TABLE IF EXISTS `reserved_name`;
CREATE TABLE `reserved_name` (
  `name` varchar(12) NOT NULL default '',
  PRIMARY KEY  (`name`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Player Reserved Names';

--
-- Dumping data for table `reserved_name`
--

LOCK TABLES `reserved_name` WRITE;
/*!40000 ALTER TABLE `reserved_name` DISABLE KEYS */;
/*!40000 ALTER TABLE `reserved_name` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `script_texts`
--

DROP TABLE IF EXISTS `script_texts`;
CREATE TABLE `script_texts` (
  `entry` mediumint(8) NOT NULL,
  `content_default` text NOT NULL,
  `content_loc1` text,
  `content_loc2` text,
  `content_loc3` text,
  `content_loc4` text,
  `content_loc5` text,
  `content_loc6` text,
  `content_loc7` text,
  `content_loc8` text,
  `sound` mediumint(8) unsigned NOT NULL default '0',
  `type` tinyint(3) unsigned NOT NULL default '0',
  `language` smallint(5) unsigned NOT NULL default '0',
  `emote` smallint(5) UNSIGNED NOT NULL default '0',
  `comment` text,
  PRIMARY KEY  (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Script Texts';

--
-- Dumping data for table `script_texts`
--

LOCK TABLES `script_texts` WRITE;
/*!40000 ALTER TABLE `script_texts` DISABLE KEYS */;
/*!40000 ALTER TABLE `script_texts` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `skill_discovery_template`
--

DROP TABLE IF EXISTS `skill_discovery_template`;
CREATE TABLE `skill_discovery_template` (
  `spellId` mediumint(8) unsigned NOT NULL default '0' COMMENT 'SpellId of the discoverable spell',
  `reqSpell` mediumint(8) unsigned NOT NULL default '0' COMMENT 'spell requirement',
  `chance` float NOT NULL default '0' COMMENT 'chance to discover',
  PRIMARY KEY  (`spellId`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Skill Discovery System';

--
-- Dumping data for table `skill_discovery_template`
--

LOCK TABLES `skill_discovery_template` WRITE;
/*!40000 ALTER TABLE `skill_discovery_template` DISABLE KEYS */;
/*!40000 ALTER TABLE `skill_discovery_template` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `skill_extra_item_template`
--

DROP TABLE IF EXISTS `skill_extra_item_template`;
CREATE TABLE `skill_extra_item_template` (
  `spellId` mediumint(8) unsigned NOT NULL default '0' COMMENT 'SpellId of the item creation spell',
  `requiredSpecialization` mediumint(8) unsigned NOT NULL default '0' COMMENT 'Specialization spell id',
  `additionalCreateChance` float NOT NULL default '0' COMMENT 'chance to create add',
  `additionalMaxNum` tinyint(3) unsigned NOT NULL default '0' COMMENT 'max num of adds',
  PRIMARY KEY  (`spellId`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Skill Specialization System';

--
-- Dumping data for table `skill_extra_item_template`
--

LOCK TABLES `skill_extra_item_template` WRITE;
/*!40000 ALTER TABLE `skill_extra_item_template` DISABLE KEYS */;
/*!40000 ALTER TABLE `skill_extra_item_template` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `skill_fishing_base_level`
--

DROP TABLE IF EXISTS `skill_fishing_base_level`;
CREATE TABLE `skill_fishing_base_level` (
  `entry` mediumint(8) unsigned NOT NULL default '0' COMMENT 'Area identifier',
  `skill` smallint(6) NOT NULL default '0' COMMENT 'Base skill level requirement',
  PRIMARY KEY  (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Fishing system';

--
-- Dumping data for table `skill_fishing_base_level`
--

LOCK TABLES `skill_fishing_base_level` WRITE;
/*!40000 ALTER TABLE `skill_fishing_base_level` DISABLE KEYS */;
/*!40000 ALTER TABLE `skill_fishing_base_level` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `skinning_loot_template`
--

DROP TABLE IF EXISTS `skinning_loot_template`;
CREATE TABLE `skinning_loot_template` (
  `entry` mediumint(8) unsigned NOT NULL default '0',
  `item` mediumint(8) unsigned NOT NULL default '0',
  `ChanceOrQuestChance` float NOT NULL default '100',
  `groupid` tinyint(3) unsigned NOT NULL default '0',
  `mincountOrRef` mediumint(9) NOT NULL default '1',
  `maxcount` tinyint(3) unsigned NOT NULL default '1',
  `lootcondition` tinyint(3) unsigned NOT NULL default '0',
  `condition_value1` mediumint(8) unsigned NOT NULL default '0',
  `condition_value2` mediumint(8) unsigned NOT NULL default '0',
  PRIMARY KEY  (`entry`,`item`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Loot System';

--
-- Dumping data for table `skinning_loot_template`
--

LOCK TABLES `skinning_loot_template` WRITE;
/*!40000 ALTER TABLE `skinning_loot_template` DISABLE KEYS */;
/*!40000 ALTER TABLE `skinning_loot_template` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `spell_affect`
--

DROP TABLE IF EXISTS `spell_affect`;
CREATE TABLE `spell_affect` (
  `entry` smallint(5) unsigned NOT NULL default '0',
  `effectId` tinyint(3) unsigned NOT NULL default '0',
  `SpellFamilyMask` bigint(20) unsigned NOT NULL default '0',
  PRIMARY KEY  (`entry`,`effectId`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `spell_affect`
--

LOCK TABLES `spell_affect` WRITE;
/*!40000 ALTER TABLE `spell_affect` DISABLE KEYS */;
/*!40000 ALTER TABLE `spell_affect` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `spell_required`
--

DROP TABLE IF EXISTS `spell_required`;
CREATE TABLE `spell_required` (
  `spell_id` mediumint(9) NOT NULL default '0',
  `prev_spell` mediumint(9) NOT NULL default '0',
  `req_spell` mediumint(9) NOT NULL default '0',
  PRIMARY KEY  (`spell_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Spell Additinal Data';

--
-- Dumping data for table `spell_required`
--

LOCK TABLES `spell_required` WRITE;
/*!40000 ALTER TABLE `spell_required` DISABLE KEYS */;
/*!40000 ALTER TABLE `spell_required` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `spell_disabled`
--

DROP TABLE IF EXISTS `spell_disabled`;
CREATE TABLE `spell_disabled` (
  `entry` int(11) unsigned NOT NULL default '0' COMMENT 'Spell entry',
  `disable_mask` int(8) unsigned NOT NULL default '0',
  `comment` varchar(64) NOT NULL default '',
  PRIMARY KEY  (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Disabled Spell System';

--
-- Dumping data for table `spell_disabled`
--

LOCK TABLES `spell_disabled` WRITE;
/*!40000 ALTER TABLE `spell_disabled` DISABLE KEYS */;
/*!40000 ALTER TABLE `spell_disabled` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `spell_elixir`
--

DROP TABLE IF EXISTS `spell_elixir`;
CREATE TABLE `spell_elixir` (
  `entry` int(11) unsigned NOT NULL default '0' COMMENT 'SpellId of potion',
  `mask` tinyint(1) unsigned NOT NULL default '0' COMMENT 'Mask 0x1 battle 0x2 guardian 0x3 flask 0x7 unstable flasks 0xB shattrath flasks',
  PRIMARY KEY  (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Spell System';

--
-- Dumping data for table `spell_elixir`
--

LOCK TABLES `spell_elixir` WRITE;
/*!40000 ALTER TABLE `spell_elixir` DISABLE KEYS */;
/*!40000 ALTER TABLE `spell_elixir` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `spell_learn_spell`
--

DROP TABLE IF EXISTS `spell_learn_spell`;
CREATE TABLE `spell_learn_spell` (
  `entry` smallint(5) unsigned NOT NULL default '0',
  `SpellID` smallint(5) unsigned NOT NULL default '0',
  PRIMARY KEY  (`entry`,`SpellID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Item System';

--
-- Dumping data for table `spell_learn_spell`
--

LOCK TABLES `spell_learn_spell` WRITE;
/*!40000 ALTER TABLE `spell_learn_spell` DISABLE KEYS */;
/*!40000 ALTER TABLE `spell_learn_spell` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `spell_pet_auras`
--

DROP TABLE IF EXISTS `spell_pet_auras`;
CREATE TABLE `spell_pet_auras` (
  `spell` mediumint(8) unsigned NOT NULL COMMENT 'dummy spell id',
  `pet` mediumint(8) unsigned NOT NULL default '0' COMMENT 'pet id; 0 = all',
  `aura` mediumint(8) unsigned NOT NULL COMMENT 'pet aura id',
  PRIMARY KEY  (`spell`,`pet`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Dumping data for table `spell_pet_auras`
--

LOCK TABLES `spell_pet_auras` WRITE;
/*!40000 ALTER TABLE `spell_pet_auras` DISABLE KEYS */;
/*!40000 ALTER TABLE `spell_pet_auras` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `spell_proc_event`
--

DROP TABLE IF EXISTS `spell_proc_event`;
CREATE TABLE `spell_proc_event` (
  `entry` smallint(5) unsigned NOT NULL default '0',
  `SchoolMask` tinyint(4) NOT NULL default '0',
  `Category` smallint(6) NOT NULL default '0',
  `SkillID` smallint(6) NOT NULL default '0',
  `SpellFamilyName` smallint(5) unsigned NOT NULL default '0',
  `SpellFamilyMask` bigint(20) unsigned NOT NULL default '0',
  `procFlags` int(10) unsigned NOT NULL default '0',
  `ppmRate` float NOT NULL default '0',
  `cooldown` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `spell_proc_event`
--

LOCK TABLES `spell_proc_event` WRITE;
/*!40000 ALTER TABLE `spell_proc_event` DISABLE KEYS */;
/*!40000 ALTER TABLE `spell_proc_event` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `spell_script_target`
--

DROP TABLE IF EXISTS `spell_script_target`;
CREATE TABLE `spell_script_target` (
  `entry` mediumint(8) unsigned NOT NULL,
  `type` tinyint(3) unsigned NOT NULL default '0',
  `targetEntry` mediumint(8) unsigned NOT NULL default '0',
  UNIQUE KEY `entry_type_target` (`entry`,`type`,`targetEntry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='Spell System';

--
-- Dumping data for table `spell_script_target`
--

LOCK TABLES `spell_script_target` WRITE;
/*!40000 ALTER TABLE `spell_script_target` DISABLE KEYS */;
/*!40000 ALTER TABLE `spell_script_target` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `spell_scripts`
--

DROP TABLE IF EXISTS `spell_scripts`;
CREATE TABLE `spell_scripts` (
  `id` mediumint(8) unsigned NOT NULL default '0',
  `delay` int(10) unsigned NOT NULL default '0',
  `command` mediumint(8) unsigned NOT NULL default '0',
  `datalong` mediumint(8) unsigned NOT NULL default '0',
  `datalong2` int(10) unsigned NOT NULL default '0',
  `datatext` text character set latin1 NOT NULL,
  `x` float NOT NULL default '0',
  `y` float NOT NULL default '0',
  `z` float NOT NULL default '0',
  `o` float NOT NULL default '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `spell_scripts`
--

LOCK TABLES `spell_scripts` WRITE;
/*!40000 ALTER TABLE `spell_scripts` DISABLE KEYS */;
/*!40000 ALTER TABLE `spell_scripts` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `spell_target_position`
--

DROP TABLE IF EXISTS `spell_target_position`;
CREATE TABLE `spell_target_position` (
  `id` mediumint(8) unsigned NOT NULL default '0' COMMENT 'Identifier',
  `target_map` smallint(5) unsigned NOT NULL default '0',
  `target_position_x` float NOT NULL default '0',
  `target_position_y` float NOT NULL default '0',
  `target_position_z` float NOT NULL default '0',
  `target_orientation` float NOT NULL default '0',
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Spell System';

--
-- Dumping data for table `spell_target_position`
--

LOCK TABLES `spell_target_position` WRITE;
/*!40000 ALTER TABLE `spell_target_position` DISABLE KEYS */;
/*!40000 ALTER TABLE `spell_target_position` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `spell_threat`
--

DROP TABLE IF EXISTS `spell_threat`;
CREATE TABLE `spell_threat` (
  `entry` mediumint(8) unsigned NOT NULL,
  `Threat` smallint(6) NOT NULL,
  PRIMARY KEY  (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED;

--
-- Dumping data for table `spell_threat`
--

LOCK TABLES `spell_threat` WRITE;
/*!40000 ALTER TABLE `spell_threat` DISABLE KEYS */;
/*!40000 ALTER TABLE `spell_threat` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `transports`
--

DROP TABLE IF EXISTS `transports`;
CREATE TABLE `transports` (
  `entry` mediumint(8) unsigned NOT NULL default '0',
  `name` text,
  `period` mediumint(8) unsigned NOT NULL default '0',
  PRIMARY KEY  (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Transports';

--
-- Dumping data for table `transports`
--

LOCK TABLES `transports` WRITE;
/*!40000 ALTER TABLE `transports` DISABLE KEYS */;
/*!40000 ALTER TABLE `transports` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `oregon_string`
--

DROP TABLE IF EXISTS `oregon_string`;
CREATE TABLE `oregon_string` (
  `entry` mediumint(8) unsigned NOT NULL default '0',
  `content_default` text NOT NULL,
  `content_loc1` text,
  `content_loc2` text,
  `content_loc3` text,
  `content_loc4` text,
  `content_loc5` text,
  `content_loc6` text,
  `content_loc7` text,
  `content_loc8` text,
  PRIMARY KEY  (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `oregon_string`
--

LOCK TABLES `oregon_string` WRITE;
/*!40000 ALTER TABLE `oregon_string` DISABLE KEYS */;
INSERT INTO `oregon_string` VALUES
(1,'You should select a character or a creature.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(2,'You should select a creature.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(3,'|cffff0000[System Message]: %s|r',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(4,'|cffff0000[Event Message]: %s|r',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(5,'There is no help for that command',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(6,'There is no such command',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(7,'There is no such subcommand',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(8,'Command %s have subcommands:%s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(9,'Commands available to you:',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10,'Incorrect syntax.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(11,'Your account level is: %i',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(12,'Online players: %u (max: %u) Queued players: %u (max: %u)',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(13,'Server uptime: %s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(14,'Player saved.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(15,'All players saved.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(16,'There are the following active GMs on this server:',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(17,'There are no GMs currently logged in on this server.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(18,'Cannot do that while flying.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(19,'Cannot do that in Battlegrounds.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(20,'Target is flying you can''t do that.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(21,'%s is flying command failed.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(22,'You are not mounted so you can''t dismount.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(23,'Cannot do that while fighting.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(24,'You used it recently.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(25,'Password not changed (unknown error)!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(26,'The password was changed',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(27,'The new passwords do not match or the old password is wrong',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(28,'Your account is now locked.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(29,'Your account is now unlocked.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(30,',rank ',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(31,' [known]',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(32,' [learn]',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(33,' [passive]',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(34,' [talent]',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(35,' [active]',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(36,' [complete]',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(37,' (offline)',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(38,'on',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(39,'off',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(40,'You are: %s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(41,'visible',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(42,'invisible',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(43,'done',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(44,'You',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(45,' <unknown> ',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(46,'<error>',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(47,'<non-existing character>',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(48,'UNKNOWN',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(49,'You must be at least level %u to enter.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(50,'You must be at least level %u and have item %s to enter.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(51,'Hello! Ready for some training?',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(52,'Invaid item count (%u) for item %u',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(53,'Mail can''t have more %u item stacks',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(54,'The new passwords do not match',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(55,'Your password can''t be longer than 16 characters (client limit),password not changed!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(56,'Current Message of the day: \r\n%s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(57,'Using World DB: %s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(58,'Using script library: %s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(100,'Global notify: ',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(101,'Map: %u (%s) Zone: %u (%s) Area: %u (%s)\nX: %f Y: %f Z: %f Orientation: %f\ngrid[%u,%u]cell[%u,%u] InstanceID: %u\n ZoneX: %f ZoneY: %f\nGroundZ: %f FloorZ: %f Have height data (Map: %u VMap: %u)',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(102,'%s is already being teleported.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(103,'You can summon a player to your instance only if he is in your party with you as leader.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(104,'You cannot go to the player''s instance because you are in a party now.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(105,'You can go to the player''s instance while not being in his party only if your GM mode is on.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(106,'You can not go to player %s from instance to instance.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(107,'You can not summon player %s from instance to instance.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(108,'You are summoning %s%s.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(109,'You are being summoned by %s.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(110,'You are teleporting %s%s to %s.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(111,'You are being teleported by %s.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(112,'Player (%s) does not exist.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(113,'Appearing at %s''s location.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(114,'%s is appearing to your location.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(115,'Incorrect values.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(116,'No character selected.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(117,'%s is not in a group.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(118,'You changed HP of %s to %i/%i.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(119,'%s changed your HP to %i/%i.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(120,'You changed MANA of %s to %i/%i.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(121,'%s changed your MANA to %i/%i.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(122,'You changed ENERGY of %s to %i/%i.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(123,'%s changed your ENERGY to %i/%i.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(124,'Current energy: %u',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(125,'You changed rage of %s to %i/%i.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(126,'%s changed your rage to %i/%i.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(127,'You changed level of %s to %i.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(128,'GUID %i,faction is %i,flags is %i,npcflag is %i,DY flag is %i',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(129,'Wrong faction: %u (not found in factiontemplate.dbc).',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(130,'You changed GUID=%i ''s Faction to %i,flags to %i,npcflag to %i,dyflag to %i.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(131,'You changed the spellflatid=%i,val= %i,mark =%i to %s.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(132,'%s changed your spellflatid=%i,val= %i,mark =%i.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(133,'%s has access to all taxi nodes now (until logout).',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(134,'%s has no more access to all taxi nodes now (only visited accessible).',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(135,'%s has given you access to all taxi nodes (until logout).',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(136,'%s has removed access to all taxi nodes (only visited still accessible).',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(137,'You set all speeds to %2.2f from normal of %s.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(138,'%s set all your speeds to %2.2f from normal.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(139,'You set the speed to %2.2f from normal of %s.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(140,'%s set your speed to %2.2f from normal.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(141,'You set the swim speed to %2.2f from normal of %s.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(142,'%s set your swim speed to %2.2f from normal.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(143,'You set the backwards run speed to %2.2f from normal of %s.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(144,'%s set your backwards run speed to %2.2f from normal.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(145,'You set the fly speed to %2.2f from normal of %s.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(146,'%s set your fly speed to %2.2f from normal.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(147,'You set the size %2.2f of %s.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(148,'%s set your size to %2.2f.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(149,'There is no such mount.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(150,'You give a mount to %s.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(151,'%s gave you a mount.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(152,'USER1: %i,ADD: %i,DIF: %i\n',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(153,'You take all copper of %s.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(154,'%s took you all of your copper.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(155,'You take %i copper from %s.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(156,'%s took %i copper from you.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(157,'You give %i copper to %s.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(158,'%s gave you %i copper.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(159,'You hear sound %u.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(160,'USER2: %i,ADD: %i,RESULT: %i\n',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(161,'Removed bit %i in field %i.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(162,'Set bit %i in field %i.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(163,'Teleport location table is empty!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(164,'Teleport location not found!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(165,'Requires search parameter.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(166,'There are no teleport locations matching your request.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(168,'Locations found are:\n%s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(169,'Mail sent to %s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(170,'You try to hear sound %u but it doesn''t exist.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(172,'server console command',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(173,'Liquid level: %f,ground: %f,type: %d,status: %d',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(200,'No selection.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(201,'Object GUID is: lowpart %u highpart %X',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(202,'The name was too long by %i characters.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(203,'Error,name can only contain characters A-Z and a-z.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(204,'The subname was too long by %i characters.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(205,'Not yet implemented',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(206,'Item ''%i'' ''%s'' added to list with maxcount ''%i'' and incrtime ''%i'' and extendedcost ''%i''',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(207,'Item ''%i'' not found in database.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(208,'Item ''%i'' ''%s'' deleted from vendor list',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(209,'Item ''%i'' not found in vendor list.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(210,'Item ''%i'' already in vendor list.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(211,'Spells of %s reset.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(212,'Spells of %s will reset at next login.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(213,'Talents of %s reset.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(214,'Talents of %s will reset at next login.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(215,'Your spells have been reset.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(216,'Your talents have been reset.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(217,'Unknown case ''%s'' for .resetall command. Type full correct case name.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(218,'Spells will reset for all players at login. Strongly recommend re-login!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(219,'Talents will reset for all players at login. Strongly recommend re-login!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(220,'Creature (GUID: %u) No waypoint found.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(221,'Creature (GUID: %u) Last waypoint not found.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(222,'Creature (GUID: %u) No waypoint found - used ''wpguid''. Now trying to find it by its position...',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(223,'Creature (GUID: %u) No waypoints found - This is a Trinity db problem (single float).',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(224,'Selected creature is ignored - provided GUID is used',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(225,'Creature (GUID: %u) not found',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(226,'You must select a visual waypoint.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(227,'No visual waypoints found',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(228,'Could not create visual waypoint with creatureID: %d',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(229,'All visual waypoints removed',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(230,'Could not create waypoint-creature with ID: %d',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(231,'No GUID provided.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(232,'No waypoint number provided.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(233,'Argument required for ''%s''.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(234,'Waypoint %i added to GUID: %d',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(235,'Waypoint %d added.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(236,'Waypoint changed.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(237,'Waypoint %s modified.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(238,'WP export successfull.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(239,'No waypoints found inside the database.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(240,'File imported.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(241,'Waypoint removed.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(242,'Warning: Could not delete WP from the world with ID: %d',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(243,'This happens if the waypoint is too far away from your char.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(244,'The WP is deleted from the database,but not from the world here.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(245,'They will disappear after a server restart.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(246,'Waypoint %d: Info for creature: %s,GUID: %d',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(247,'Waittime: %d',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(248,'Model %d: %d',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(249,'Emote: %d',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(250,'Spell: %d',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(251,'Text%d (ID: %i): %s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(252,'AIScript: %s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(253,'Forced rename for player %s will be requested at next login.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(254,'Forced rename for player %s (GUID #%u) will be requested at next login.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(255,'Waypoint-Creature (GUID: %u) Not found',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(256,'Could not find NPC...',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(257,'Creature movement type set to ''%s'',waypoints removed (if any).',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(258,'Creature movement type set to ''%s'',waypoints were not removed.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(259,'Incorrect value,use on or off',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(260,'Value saved.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(261,'Value saved,you may need to rejoin or clean your client cache.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(262,'Areatrigger ID %u not found!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(263,'Target map or coordinates is invalid (X: %f Y: %f MapId: %u)',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(264,'Zone coordinates is invalid (X: %f Y: %f AreaId: %u)',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(265,'Zone %u (%s) is part of instanceable map %u (%s)',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(266,'Nothing found!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(267,'Object not found!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(268,'Creature not found!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(269,'Warning: Mob found more than once - you will be teleported to the first one found in DB.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(270,'Creature Removed',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(271,'Creature moved.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(272,'Creature (GUID:%u) must be on the same map as player!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(273,'Game Object (GUID: %u) not found',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(274,'Game Object (GUID: %u) has references in not found creature %u GO list,can''t be deleted.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(275,'Game Object (GUID: %u) removed',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(276,'Game Object (GUID: %u) turned',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(277,'Game Object (GUID: %u) moved',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(278,'You must select a vendor',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(279,'You must send id for item',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(280,'Vendor has too many items (max 128)',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(281,'You can''t kick self,logout instead',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(282,'Player |cffff0000%s|r kicked by |cffff0000%s|r. Reason: |cffff0000%s|r.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(283,'Player %s not found.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(284,'Accepting Whisper: %s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(285,'Accepting Whisper: ON',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(286,'Accepting Whisper: OFF',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(287,'Creature (GUID: %u) not found',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(297,'Spawn distance changed to: %f',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(298,'Spawn time changed to: %i',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(299,'The honor of %s was set to %u!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(300,'Your chat has been disabled for %u minutes. Reason: %s.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(301,'You have disabled %s''s chat for %u minutes. Reason: %s.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(302,'Player''s chat is already enabled.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(303,'Your chat has been enabled.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(304,'You have enabled %s''s chat.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(305,'Faction %s (%u) reputation of %s was set to %5d!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(306,'The arena points of %s was set to %u!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(307,'No faction found!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(308,'Faction %i unknown!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(309,'Invalid parameter %s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(310,'delta must be between 0 and %d (inclusive)',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(311,'%d - |cffffffff|Hfaction:%d|h[%s]|h|r',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(312,' [visible]',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(313,' [at war]',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(314,' [peace forced]',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(315,' [hidden]',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(316,' [invisible forced]',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(317,' [inactive]',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(318,'Hated',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(319,'Hostile',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(320,'Unfriendly',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(321,'Neutral',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(322,'Friendly',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(323,'Honored',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(324,'Revered',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(325,'Exalted',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(326,'Faction %s (%u) can''not have reputation.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(327,' [no reputation]',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(328,'Characters at account %s (Id: %u)',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(329,'  %s (GUID %u)',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(330,'No players found!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(331,'Extended item cost %u not exist',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(332,'GM mode is ON',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(333,'GM mode is OFF',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(334,'GM Chat Badge is ON',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(335,'GM Chat Badge is OFF',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(336,'You repair all %s''s items.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(337,'All your items repaired by %s.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(338,'You set waterwalk mode %s for %s.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(339,'Your waterwalk mode set to %s by %s.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(340,'%s is now following you.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(341,'%s is not following you.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(342,'%s is now not following you.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(400,'|cffff0000[System Message]:|rScripts reloaded',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(401,'You change security level of account %s to %i.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(402,'%s changed your security level to %i.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(403,'You have low security level for this.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(404,'Creature movement disabled.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(405,'Creature movement enabled.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(406,'Weather can''t be changed for this zone.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(407,'Weather system disabled at server.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(408,'%s is banned for %s. Reason: %s.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(409,'%s is banned permanently for %s.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(410,'%s %s not found',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(411,'%s unbanned.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(412,'There was an error removing the ban on %s.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(413,'Account not exist: %s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(414,'There is no such character.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(415,'There is no such IP in banlist.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(416,'Account %s has never been banned',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(417,'Ban history for account %s:',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(418,'Ban Date: %s Bantime: %s Still active: %s  Reason: %s Set by: %s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(419,'Inf.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(420,'Never',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(421,'Yes',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(422,'No',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(423,'IP: %s\nBan Date: %s\nUnban Date: %s\nRemaining: %s\nReason: %s\nSet by: %s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(424,'There is no matching IPban.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(425,'There is no matching account.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(426,'There is no banned account owning a character matching this part.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(427,'The following IPs match your pattern:',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(428,'The following accounts match your query:',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(429,'You learned many spells/skills.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(430,'You learned all spells for class.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(431,'You learned all talents for class.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(432,'You learned all languages.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(433,'You learned all craft skills and recipes.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(434,'Could not find ''%s''',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(435,'Invalid item id: %u',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(436,'No items found!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(437,'Invalid gameobject id: %u',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(438,'Found items %u: %u ( inventory %u mail %u auction %u guild %u)',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(439,'Found gameobjects %u: %u ',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(440,'Invalid creature id: %u',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(441,'Found creatures %u: %u ',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(442,'No area found!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(443,'No item sets found!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(444,'No skills found!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(445,'No spells found!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(446,'No quests found!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(447,'No creatures found!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(448,'No gameobjects found!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(449,'Graveyard #%u doesn''t exist.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(450,'Graveyard #%u already linked to zone #%u (current).',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(451,'Graveyard #%u linked to zone #%u (current).',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(452,'Graveyard #%u can''t be linked to subzone or not existed zone #%u (internal error).',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(454,'No faction in Graveyard with id= #%u ,fix your DB',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(455,'invalid team,please fix database',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(456,'any',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(457,'alliance',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(458,'horde',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(459,'Graveyard #%u (faction: %s) is nearest from linked to zone #%u.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(460,'Zone #%u doesn''t have linked graveyards.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(461,'Zone #%u doesn''t have linked graveyards for faction: %s.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(462,'Teleport location already exists!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(463,'Teleport location added.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(464,'Teleport location NOT added: database error.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(465,'Teleport location deleted.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(467,'Target unit has %d auras:',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(468,'id: %d eff: %d type: %d duration: %d maxduration: %d name: %s%s%s caster: %s %u',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(469,'Target unit has %d auras of type %d:',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(470,'id: %d eff: %d name: %s%s%s caster: %s %u',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(471,'Quest %u not found.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(472,'Quest %u started from item. For correct work,please,add item to inventory and start quest in normal way: .additem %u',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(473,'Quest removed.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(474,' [rewarded]',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(475,' [complete]',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(476,' [active]',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(477,'%s''s Fly Mode %s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(478,'Opcode %u sent to %s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(479,'Character loaded successfully!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(480,'Failed to load the character!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(481,'Character dumped successfully!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(482,'Character dump failed!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(483,'Spell %u broken and not allowed to cast or learn!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(484,'Skill %u (%s) for player %s set to %u and current maximum set to %u (without permanent (talent) bonuses).',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(485,'Player %s must have skill %u (%s) before using this command.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(486,'Invalid skill id (%u)',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(487,'You learned default GM spells/skills.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(488,'You already know that spell.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(489,'Target(%s) already know that spell.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(490,'%s doesn''t know that spell.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(491,'You already forgot that spell.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(492,'All spell cooldowns removed for %s.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(493,'Spell %u cooldown removed for %s.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(494,'Command : Additem,itemId = %i,amount = %i',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(495,'Command : Additemset,itemsetId = %i',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(496,'Removed itemID = %i,amount = %i from %s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(497,'Cannot create item ''%i'' (amount: %i)',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(498,'You need to provide a guild name!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(499,'Player not found!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(500,'Player already has a guild!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(501,'Guild not created! (already exists?)',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(502,'No items from itemset ''%u'' found.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(503,'The distance is: (3D) %f (2D) %f yards.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(504,'Item ''%i'' ''%s'' Item Slot %i',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(505,'Item ''%i'' doesn''t exist.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(506,'Item ''%i'' ''%s'' Added to Slot %i',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(507,'Item save failed!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(508,'%d - owner: %s (guid: %u account: %u ) %s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(509,'%d - sender: %s (guid: %u account: %u ) receiver: %s (guid: %u account: %u ) %s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(510,'%d - owner: %s (guid: %u account: %u ) %s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(511,'Wrong link type!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(512,'%d - |cffffffff|Hitem:%d:0:0:0:0:0:0:0|h[%s]|h|r ',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(513,'%d - |cffffffff|Hquest:%d|h[%s]|h|r %s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(514,'%d - |cffffffff|Hcreature_entry:%d|h[%s]|h|r ',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(515,'%d - |cffffffff|Hcreature:%d|h[%s X:%f Y:%f Z:%f MapId:%d]|h|r ',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(516,'%d - |cffffffff|Hgameobject_entry:%d|h[%s]|h|r ',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(517,'%d - |cffffffff|Hgameobject:%d|h[%s X:%f Y:%f Z:%f MapId:%d]|h|r ',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(518,'%d - |cffffffff|Hitemset:%d|h[%s %s]|h|r ',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(519,'|cffffffff|Htele:%s|h[%s]|h|r ',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(520,'%d - |cffffffff|Hspell:%d|h[%s]|h|r ',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(521,'%d - |cffffffff|Hskill:%d|h[%s %s]|h|r %s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(522,'Game Object (GUID: %u) not found',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(523,'>> Game Object %s (GUID: %u) at %f %f %f. Orientation %f.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(524,'Selected object:\n|cffffffff|Hitemset:%d|h[%s]|h|r\nGUID: %u ID: %u\nX: %f Y: %f Z: %f MapId: %u\nOrientation: %f',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(525,'>> Add Game Object ''%i'' (%s) (GUID: %i) added at ''%f %f %f''.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(526,'%s (lowguid: %u) movement generators stack:',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(527,'   Idle',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(528,'   Random',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(529,'   Waypoint',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(530,'   Animal random',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(531,'   Confused',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(532,'   Targeted to player %s (lowguid %u)',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(533,'   Targeted to creature %s (lowguid %u)',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(534,'   Targeted to <NULL>',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(535,'   Home movement to (X:%f Y:%f Z:%f)',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(536,'   Home movement used for player?!?',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(537,'   Taxi flight',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(538,'   Unknown movement generator (%u)',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(539,'Player selected NPC\nGUID: %u.\nFaction: %u.\nnpcFlags: %u.\nEntry: %u.\nDisplayID: %u (Native: %u).',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(540,'Level: %u.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(541,'Health (base): %u. (max): %u. (current): %u.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(542,'Field Flags: %u.\nDynamic Flags: %u.\nFaction Template: %u.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(543,'Loot: %u Pickpocket: %u Skinning: %u',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(544,'Position: %f %f %f.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(545,'*** Is a vendor!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(546,'*** Is a trainer!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(547,'InstanceID: %u',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(548,'Player%s %s (guid: %u) Account: %s (id: %u) GMLevel: %u Last IP: %s Last login: %s Latency: %ums',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(549,'Played time: %s Level: %u Money: %ug%us%uc',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(550,'Command .pinfo doesn''t support ''rep'' option for offline players.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(551,'%s has explored all zones now.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(552,'%s has no more explored zones.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(553,'%s has explored all zones for you.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(554,'%s has hidden all zones from you.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(555,'Hover enabled',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(556,'Hover disabled',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(557,'You have been leveled up (%i)',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(558,'You have been leveled down (%i)',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(559,'Your level progress has been reset.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(560,'The area has been set as explored.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(561,'The area has been set as not explored.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(562,'GUID=%i ''s updateIndex: %i,value:  %i.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(563,'You change GUID=%i ''s UpdateIndex: %i value to %i.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(564,'The value index %u is too big to %u(count: %u).',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(565,'Set %u uint32 Value:[OPCODE]:%u [VALUE]:%u',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(566,'You Set %u Field:%u to uint32 Value: %u',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(567,'Set %u float Value:[OPCODE]:%u [VALUE]:%f',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(568,'You Set %u Field:%i to float Value: %f',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(569,'Get %u uint32 Value:[OPCODE]:%u [VALUE]:%u',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(570,'The uint32 value of %u in %u is: %u',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(571,'Get %u float Value:[OPCODE]:%u [VALUE]:%f',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(572,'The float of %u value in %u is: %f',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(573,'.Set32Bit:[OPCODE]:%u [VALUE]:%u',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(574,'You set Bit of Field:%u to Value: %u',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(575,'.Mod32Value:[OPCODE]:%u [VALUE]:%i',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(576,'You modified the value of Field:%u to Value: %u',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(577,'You are now invisible.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(578,'You are now visible.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(579,'Selected player or creature not have victim.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(580,'Player %s learned all default spells for race/class and completed quests rewarded spells.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(581,'Found near gameobjects (distance %f): %u ',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(582,'SpawnTime: Full:%s Remain:%s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(583,'%d - |cffffffff|Hgameevent:%d|h[%s]|h|r%s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(584,'No event found!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(585,'Event not exist!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(586,'Event %u: %s%s\nStart: %s End: %s Occurence: %s Length: %s\nNext state change: %s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(587,'Event %u already active!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(588,'Event %u not active!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(589,'   Point movement to (X:%f Y:%f Z:%f)',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(590,'   Fear movement',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(591,'   Distract movement',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(592,'You have learned all spells in craft: %s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(593,'Currently Banned Accounts:',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(594,'|    Account    |   BanDate    |   UnbanDate  |  Banned By    |   Ban Reason  |',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(595,'Currently Banned IPs:',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(596,'|      IP       |   BanDate    |   UnbanDate  |  Banned By    |   Ban Reason  |',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(597,'Current gamemasters:',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(598,'|    Account    |  GM  |',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(599,'No gamemasters.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(600,'The Alliance wins!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(601,'The Horde wins!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(602,'The battle for Warsong Gulch begins in 1 minute.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(603,'The battle for Warsong Gulch begins in 30 seconds. Prepare yourselves!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(604,'Let the battle for Warsong Gulch begin!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(605,'$n captured the Horde flag!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(606,'$n captured the Alliance flag!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(607,'The Horde flag was dropped by $n!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(608,'The Alliance Flag was dropped by $n!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(609,'The Alliance Flag was returned to its base by $n!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(610,'The Horde flag was returned to its base by $n!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(611,'The Horde flag was picked up by $n!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(612,'The Alliance Flag was picked up by $n!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(613,'The flags are now placed at their bases.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(614,'The Alliance flag has been respawned!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(615,'The Horde flag has been respawned!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(636,'The Battle for Eye of the Storm begins in 1 minute.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(637,'The Battle for Eye of the Storm begins in 30 seconds.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(638,'The Battle for Eye of the Storm has begun!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(650,'Alliance',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(651,'Horde',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(652,'stables',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(653,'blacksmith',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(654,'farm',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(655,'lumber mill',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(656,'mine',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(657,'The %s has taken the %s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(658,'$n has defended the %s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(659,'$n has assaulted the %s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(660,'$n claims the %s! If left unchallenged,the %s will control it in 1 minute!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(661,'The Battle for Arathi Basin begins in 1 minute.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(662,'The Battle for Arathi Basin begins in 30 seconds. Prepare yourselves!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(663,'The Battle for Arathi Basin has begun!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(664,'The Alliance has gathered $1776W resources,and is near victory!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(665,'The Horde has gathered $1777W resources,and is near victory!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(666,'After your recent battle in %s our best attempts to award you a Mark of Honor failed. Enclosed you will find the Mark of Honor we were not able to deliver to you at the time. Thanks for fighting in %s!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(667,'The Alliance has taken control of the Mage Tower!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(668,'The Horde has taken control of the Mage Tower!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(669,'The Alliance has taken control of the Draenei Ruins!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(670,'The Horde has taken control of the Draenei Ruins!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(671,'The Alliance has taken control of the Blood Elf Tower!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(672,'The Horde has taken control of the Blood Elf Tower!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(673,'The Alliance has taken control of the Fel Reaver Ruins!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(674,'The Horde has taken control of the Fel Reaver Ruins!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(675,'The Alliance has lost control of the Mage Tower!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(676,'The Horde has lost control of the Mage Tower!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(677,'The Alliance has lost control of the Draenei Ruins!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(678,'The Horde has lost control of the Draenei Ruins!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(679,'The Alliance has lost control of the Blood Elf Tower!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(680,'The Horde has lost control of the Blood Elf Tower!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(681,'The Alliance has lost control of the Fel Reaver Ruins!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(682,'The Horde has lost control of the Fel Reaver Ruins!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(683,'$N has taken the flag!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(684,'The Alliance has captured the flag!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(685,'The Horde has captured the flag!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(686,'The Flag has been dropped!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(687,'The flag has been reset',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(700,'You must be level %u to form an arena team',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(701,'One minute until the Arena battle begins!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(702,'Thirty seconds until the Arena battle begins!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(703,'Fifteen seconds until the Arena battle begins!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(704,'The Arena battle has begun!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(705,'You must wait %s before speaking again.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(706,'This item(s) have problems with equipping/storing in inventory.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(707,'%s does not wish to be disturbed: %s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(708,'%s is Away from Keyboard: %s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(709,'Do not Disturb',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(710,'Away from Keyboard',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(711,'Your group is too large for this battleground. Please regroup to join.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(712,'Your group is too large for this arena. Please regroup to join.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(713,'You must be level %u to join an arena team!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(715,'You don''t meet Battleground level requirements',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(717,'Alliance',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(718,'Horde',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(719,'%s was destroyed by the %s!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(720,'The %s is under attack! If left unchecked,the %s will destroy it!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(721,'The %s was taken by the %s!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(722,'The %s was taken by the %s!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(723,'The %s was taken by the %s!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(724,'The %s is under attack! If left unchecked,the %s will capture it!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
(725,'The %s has taken the %s! Its supplies will now be used for reinforcements!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(726,'Irondeep Mine',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(727,'Coldtooth Mine',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(728,'Stormpike Aid Station',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(729,'Dun Baldar South Bunker',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(730,'Dun Baldar North Bunker',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(731,'Stormpike Graveyard',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(732,'Icewing Bunker',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(733,'Stonehearth Graveyard',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(734,'Stonehearth Bunker',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(735,'Snowfall Graveyard',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(736,'Iceblood Tower',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(737,'Iceblood Graveyard',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(738,'Tower Point',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(739,'Frostwolf Graveyard',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(740,'East Frostwolf Tower',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(741,'West Frostwolf Tower',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(742,'Frostwolf Relief Hut',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(743,'The Battle for Alterac Valley begins in 1 minute.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(744,'The Battle for Alterac Valley begins in 30 seconds. Prepare yourselves!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(745,'The Battle for Alterac Valley has begun!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(746,'|cffff0000[Automatic]:|r',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(747,'|cffffff00[|c1f40af20Announce by|r |cffff0000%s|cffffff00]:|r %s|r',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(748,'The Frostwolf General is Dead!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(749,'The Stormpike General is Dead!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(750,'linkGUID: %u,Entry: %u (%s)',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(770,'Your group has members not in your arena team. Please regroup to join.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(771,'Your group does not have enough players to join this match.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(772,'The Gold Team wins!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(773,'The Green Team wins!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(774,'There aren''t enough players in this battleground. It will end soon unless some more players join to balance the fight.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(775,'Your group has an offline member. Please remove him before joining.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(776,'Your group has players from the opposing faction. You can''t join the battleground as a group.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(777,'Your group has players from different battleground brakets. You can''t join as group.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(778,'Someone in your party is already in this battleground queue. (S)he must leave it before joining as group.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(779,'Someone in your party is Deserter. You can''t join as group.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(780,'Someone in your party is already in three battleground queues. You cannot join as group.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(781,'You cannot teleport to a battleground or arena map.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(782,'You cannot summon players to a battleground or arena map.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(783,'You must be in GM mode to teleport to a player in a battleground.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(784,'You cannot teleport to a battleground from another battleground. Please leave the current battleground first.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(785,'Arena testing turned %s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(786,'|cffff0000[Automatic]:|r',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(787,'|cffffff00[|c1f40af20Announce by|r |cffff0000%s|cffffff00]:|r %s|r',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(801,'You do not have enough gold',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(802,'You do not have enough free slots',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(803,'Your partner does not have enough free bag slots',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(804,'You do not have permission to perform that function',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(805,'Unknown language',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(806,'You don''t know that language',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(807,'Please provide character name',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(808,'Player %s not found or offline',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(809,'Account for character %s not found',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(810,'You must be in a raid group to enter the instance %s!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(811,'Guild Master',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(812,'Officer',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(813,'Veteran',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(814,'Member',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(815,'Initiate',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1000,'Exiting daemon...',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1001,'Account deleted: %s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1002,'Account %s NOT deleted (probably sql file format was updated)',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1003,'Account %s NOT deleted (unknown error)',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1004,'Account created: %s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1005,'Account name can''t be longer than 16 characters (client limit),account not created!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1006,'Account with this name already exist!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1007,'Account %s NOT created (probably sql file format was updated)',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1008,'Account %s NOT created (unknown error)',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1009,'Player %s (Guid: %u) Account %s (Id: %u) deleted.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1010,'|    Account    |       Character      |       IP        | GM | TBC |',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1011,'|<Error>        | %20s |<Error>          |<Er>|<Err>|',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1100,'Account %s (Id: %u) have up to %u expansion allowed now.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1101,'Message of the day changed to:\r\n%s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1102,'Message sent to %s: %s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1103,'%d - %s %s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1104,'%d - %s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1105,'%d - %s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1106,'%d - %s %s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1107,'%d - %s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1108,'%d - %s %s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1109,'%d - %s %s %s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1110,'%d - %s X:%f Y:%f Z:%f MapId:%d',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1111,'%d - %s X:%f Y:%f Z:%f MapId:%d',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1112,'Failed to open file: %s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1113,'Account %s (%u) have max amount allowed characters (client limit)',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1114,'Dump file has broken data!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1115,'Invalid character name!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1116,'Invalid character guid!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1117,'Character guid %u in use!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1118,'%d - guild: %s (guid: %u) %s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1119,'You must use male or female as gender.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1120,'You change gender of %s to %s.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1121,'Your gender changed to %s by %s.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1122,'Your group is too large for this battleground. Please regroup to join.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1123,'Your group is too large for this arena. Please regroup to join.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1124,'Your group has members not in your arena team. Please regroup to join.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1125,'Your group does not have enough players to join this match.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1126,'The Gold Team wins!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1127,'The Green Team wins!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1128,'There aren''t enough players in this battleground. It will end soon unless some more players join to balance the fight.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1129,'Your group has an offline member. Please remove him before joining.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1130,'Your group has players from the opposing faction. You can''t join the battleground as a group.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1131,'Your group has players from different battleground brakets. You can''t join as group.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1132,'Someone in your party is already in this battleground queue. (S)he must leave it before joining as group.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1133,'Someone in your party is Deserter. You can''t join as group.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1134,'Someone in your party is already in three battleground queues. You cannot join as group.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1135,'You cannot teleport to a battleground or arena map.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1136,'You cannot summon players to a battleground or arena map.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1137,'You must be in GM mode to teleport to a player in a battleground.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(1138,'You cannot teleport to a battleground from another battleground. Please leave the current battleground first.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(2000,'|cff00ff00New ticket from|r|cffff00ff %s.|r |cff00ff00Ticket entry:|r|cffff00ff %d.|r','','','','','','','',''),
(2001,'|cff00ff00Character|r|cffff00ff %s |r|cff00ff00edited his/her ticket:|r|cffff00ff %d.|r','','','','','','','',''),
(2002,'|cff00ff00Character|r|cffff00ff %s |r|cff00ff00abandoned ticket entry:|r|cffff00ff %d.|r','','','','','','','',''),
(2003,'|cff00ff00Closed by|r:|cff00ccff %s|r ','','','','','','','',''),
(2004,'|cff00ff00Deleted by|r:|cff00ccff %s|r ','','','','','','','',''),
(2005,'Ticket not found.','','','','','','','',''),
(2006,'Please close ticket before deleting it permanently.','','','','','','','',''),
(2007,'Ticket %d is already assigned.','','','','','','','',''),
(2008,'%u Tickets succesfully reloaded from the database.','','','','','','','',''),
(2009,'Showing list of open tickets.','','','','','','','',''),
(2010,'Showing list of open tickets whose creator is online.','','','','','','','',''),
(2011,'Showing list of closed tickets.','','','','','','','',''),
(2012,'Invalid name specified. Name should be that of an online Gamemaster.','','','','','','','',''),
(2013,'This ticket is already assigned to yourself. To unassign use .ticket unassign %d and then reassign.','','','','','','','',''),
(2014,'Ticket %d is not assigned,you cannot unassign it.','','','','','','','',''),
(2015,'You cannot unassign tickets from staffmembers with a higher security level than yourself.','','','','','','','',''),
(2016,'Cannot close ticket %d,it is assigned to another GM.','','','','','','','',''),
(2017,'|cff00ff00Ticket|r:|cff00ccff %d.|r ','','','','','','','',''),
(2018,'|cff00ff00Created by|r:|cff00ccff %s|r ','','','','','','','',''),
(2019,'|cff00ff00Last change|r:|cff00ccff %s ago|r ','','','','','','','',''),
(2020,'|cff00ff00Assigned to|r:|cff00ccff %s|r ','','','','','','','',''),
(2021,'|cff00ff00Unassigned by|r:|cff00ccff %s|r ','','','','','','','',''),
(2022,'\n|cff00ff00Message|r: "%s"|r ','','','','','','','',''),
(2023,'\n|cff00ff00Comment|r: "%s"|r ','','','','','','','',''),
(2024,'\n|cff00ccff%s|r |cff00ff00Added comment|r: "%s"|r ','','','','','','','',''),
(2025,'|cff00ff00Created|r:|cff00ccff %s ago|r ',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(5000,'You froze player %s.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(5001,'It might be amusing but no... you cant freeze yourself!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(5002,'Invalid input check the name of target.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(5003,'You unfroze player %s.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(5004,'There are no frozen players.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(5005,'Following players are frozen on the server:',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(5006,'- %s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(5009,'Sound %u Played to server',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(6613,'|cfff00000[GM Announcement]: %s|r',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(6614,'Notification to GM''s - ',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(6615,'|cffffff00[|c1f40af20GM Announce by|r |cffff0000%s|cffffff00]:|r %s|r',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(6616,'You are silenced and can only speak with a GM.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10001,'The Horde has taken The Overlook!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10002,'The Alliance has taken The Overlook!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10003,'The Horde has taken The Stadium!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10004,'The Alliance has taken The Stadium!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10005,'The Horde has taken Broken Hill!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10006,'The Alliance has taken Broken Hill!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10007,'The Horde lost The Overlook!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10008,'The Alliance lost The Overlook!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10009,'The Horde lost The Stadium!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10010,'The Alliance lost The Stadium!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10011,'The Horde lost Broken Hill!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10012,'The Alliance lost Broken Hill!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10013,'The Horde has taken the West Beacon!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10014,'The Alliance has taken the West Beacon!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10015,'The Horde has taken the East Beacon!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10016,'The Alliance has taken the East Beacon!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10017,'The Horde has captured the Twin Spire Graveyard!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10018,'The Alliance has captured the Twin Spire Graveyard!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10019,'The Horde lost the West Beacon!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10020,'The Alliance lost the West Beacon!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10021,'The Horde lost the East Beacon!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10022,'The Alliance lost the East Beacon!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10023,'The Horde lost the Twin Spire Graveyard!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10024,'The Alliance lost the Twin Spire Graveyard!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10025,'The Horde has captured Halaa!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10026,'The Alliance has captured Halaa!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10027,'The Horde lost Halaa!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10028,'The Alliance lost Halaa!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10029,'The Horde has taken a Spirit Tower!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10030,'The Alliance has taken a Spirit Tower!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10031,'The Horde lost a Spirit Tower!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10032,'The Alliance lost a Spirit Tower!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10033,'The Horde has taken the Northpass Tower!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10034,'The Alliance has taken the Northpass Tower!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10035,'The Horde has taken the Eastwall Tower!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10036,'The Alliance has taken the Eastwall Tower!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10037,'The Horde has taken the Crown Guard Tower!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10038,'The Alliance has taken the Crown Guard Tower!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10039,'The Horde has taken the Plaguewood Tower!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10040,'The Alliance has taken the Plaguewood Tower!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10041,'The Horde lost the Northpass Tower!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10042,'The Alliance lost the Northpass Tower!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10043,'The Horde lost the Eastwall Tower!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10044,'The Alliance lost the Eastwall Tower!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10045,'The Horde lost the Crown Guard Tower!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10046,'The Alliance lost the Crown Guard Tower!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10047,'The Horde lost the Plaguewood Tower!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10048,'The Alliance lost the Plaguewood Tower!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10049,'The Horde has collected 200 silithyst!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10050,'The Alliance has collected 200 silithyst!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10051,'Take me to Northpass Tower.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10052,'Take me to Eastwall Tower.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10053,'Take me to Crown Guard Tower.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10054,'Give me the flag,I''ll take it to the central beacon for the glory of the Alliance!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10055,'Give me the flag,I''ll take it to the central beacon for the glory of the Horde!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10056,'You must be a member of the Horde to enter the Hall of Legends.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
(10057,'You must be a member of the Alliance to enter the Champion''s Hall.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
/*!40000 ALTER TABLE `oregon_string` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `transport_events`
--

DROP TABLE IF EXISTS `transport_events`;
CREATE TABLE `transport_events` (
 	`entry` int(11) unsigned NOT NULL default '0',
	`waypoint_id` int(11) unsigned NOT NULL default '0',	
  	`event_id` int(11) unsigned NOT NULL default '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `transport_events`
--

LOCK TABLES `transport_events` WRITE;
/*!40000 ALTER TABLE `transport_events` DISABLE KEYS */;
/*!40000 ALTER TABLE `transport_events` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `uptime`
--

DROP TABLE IF EXISTS `uptime`;
CREATE TABLE `uptime` (
  `starttime` bigint(20) unsigned NOT NULL default '0',
  `startstring` varchar(64) NOT NULL default '',
  `uptime` bigint(20) unsigned NOT NULL default '0',
  `maxplayers` smallint(5) unsigned NOT NULL default '0',
  PRIMARY KEY  (`starttime`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Uptime system';

--
-- Dumping data for table `uptime`
--

LOCK TABLES `uptime` WRITE;
/*!40000 ALTER TABLE `uptime` DISABLE KEYS */;
/*!40000 ALTER TABLE `uptime` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `version`
--

DROP TABLE IF EXISTS `version`;
CREATE TABLE `version` (
    `core_version` varchar(120) default NULL COMMENT 'Core revision dumped at startup.',
    `core_revision` varchar(120) default '0',
    `db_version` varchar(120) default NULL COMMENT 'Version of world DB.',
    `script_version` varchar(120) default NULL COMMENT 'Version of scripts DB.'
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Version Notes';

--
-- Dumping data for table `version`
--

LOCK TABLES `version` WRITE;
/*!40000 ALTER TABLE `version` DISABLE KEYS */;
/*!40000 ALTER TABLE `version` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `waypoint_data`
--

DROP TABLE IF EXISTS `waypoint_data`;
CREATE TABLE `waypoint_data` (
  `id` int(10) unsigned NOT NULL default '0' COMMENT 'Creature GUID',
  `point` mediumint(8) unsigned NOT NULL default '0',
  `position_x` float NOT NULL default '0',
  `position_y` float NOT NULL default '0',
  `position_z` float NOT NULL default '0',
  `delay` int(10) unsigned NOT NULL default '0',
  `move_flag` tinyint(1) NOT NULL default '0',
  `action` int(11) NOT NULL default '0',
  `action_chance` smallint(3) NOT NULL default '100',
  `wpguid` int(11) NOT NULL default '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `waypoint_data`
--

LOCK TABLES `waypoint_data` WRITE;
/*!40000 ALTER TABLE `waypoint_data` DISABLE KEYS */;
/*!40000 ALTER TABLE `waypoint_data` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `waypoint_scripts`
--

DROP TABLE IF EXISTS `waypoint_scripts`;
CREATE TABLE `waypoint_scripts` (
  `id` int(11) unsigned NOT NULL default '0',
  `delay` int(11) unsigned NOT NULL default '0',
  `command` int(11) unsigned NOT NULL default '0',
  `datalong` int(11) unsigned NOT NULL default '0',
  `datalong2` int(11) unsigned NOT NULL default '0',
  `dataint` int(11) unsigned NOT NULL default '0',
  `x` float NOT NULL default '0',
  `y` float NOT NULL default '0',
  `z` float NOT NULL default '0',
  `o` float NOT NULL default '0',
  `guid` int(11) NOT NULL default '0',
  PRIMARY KEY  (`guid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `waypoint_scripts`
--

LOCK TABLES `waypoint_scripts` WRITE;
/*!40000 ALTER TABLE `waypoint_scripts` DISABLE KEYS */;
/*!40000 ALTER TABLE `waypoint_scripts` ENABLE KEYS */;
UNLOCK TABLES;

/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2008-10-14 13:23:35
