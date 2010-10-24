UPDATE oregon_string SET content_default = 'Creature (GUID: %u) No waypoints found - This is a Oregon db problem (single float).' WHERE oregon_string.entry=223;
UPDATE oregon_string SET content_default = 'Player %s kicked by %s. Reason: %s.' WHERE oregon_string.entry=282;
UPDATE oregon_string SET content_default = 'Your chat has been disabled by %s for %u minutes. Reason: %s.' WHERE oregon_string.entry=300;
UPDATE oregon_string SET content_default = 'Faction %s (%u) can''nt have reputation.' WHERE oregon_string.entry=326;
UPDATE oregon_string SET content_default = 'The Alliance flag is now placed at its base.' WHERE oregon_string.entry=614;
UPDATE oregon_string SET content_default = 'The Horde flag is now placed at its base.' WHERE oregon_string.entry=615;
UPDATE oregon_string SET content_default = '%s has taken the flag!' WHERE oregon_string.entry=683;
UPDATE oregon_string SET content_default = 'The flag has been dropped.' WHERE oregon_string.entry=686;
UPDATE oregon_string SET content_default = 'The flag has been reset.' WHERE oregon_string.entry=687;

DELETE FROM oregon_string WHERE entry IN (173,175,11000);
DELETE FROM oregon_string WHERE entry BETWEEN 811 AND 816;
DELETE FROM oregon_string WHERE entry BETWEEN 717 AND 784;
DELETE FROM oregon_string WHERE entry BETWEEN 1122 AND 2024;
 
INSERT INTO oregon_string VALUES
    (175,'Liquid level: %f, ground: %f, type: %d, status: %d',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),

    (811,'Guild Master',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (812,'Officer',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (813,'Veteran',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (814,'Member',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (815,'Initiate',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),

    (720,'Your group is too large for this battleground. Please regroup to join.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (721,'Your group is too large for this arena. Please regroup to join.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (722,'Your group has members not in your arena team. Please regroup to join.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (723,'Your group does not have enough players to join this match.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (724,'The Gold Team wins!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (725,'The Green Team wins!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (726,'There aren''t enough players in this battleground. It will end soon unless some more players join to balance the fight.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (727,'Your group has an offline member. Please remove him before joining.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (728,'Your group has players from the opposing faction. You can''t join the battleground as a group.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (729,'Your group has players from different battleground brakets. You can''t join as group.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (730,'Someone in your party is already in this battleground queue. (S)he must leave it before joining as group.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (731,'Someone in your party is Deserter. You can''t join as group.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (732,'Someone in your party is already in three battleground queues. You cannot join as group.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (733,'You cannot teleport to a battleground or arena map.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (734,'You cannot summon players to a battleground or arena map.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (735,'You must be in GM mode to teleport to a player in a battleground.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (736,'You cannot teleport to a battleground from another battleground. Please leave the current battleground first.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (750,'Not enough players. This game will close in %u mins.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (751,'Not enough players. This game will close in %u seconds.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),

    (1199,'linkGUID: %u, Entry: %u (%s)',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (1300,'Alliance',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (1301,'Horde',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (1302,'%s was destroyed by the %s!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (1303,'The %s is under attack! If left unchecked, the %s will destroy it!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (1304,'The %s was taken by the %s!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (1305,'The %s was taken by the %s!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (1306,'The %s was taken by the %s!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (1307,'The %s is under attack! If left unchecked, the %s will capture it!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (1308,'The %s has taken the %s! Its supplies will now be used for reinforcements!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (1309,'Irondeep Mine',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (1310,'Coldtooth Mine',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (1311,'Stormpike Aid Station',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (1312,'Dun Baldar South Bunker',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (1313,'Dun Baldar North Bunker',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (1314,'Stormpike Graveyard',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (1315,'Icewing Bunker',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (1316,'Stonehearth Graveyard',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (1317,'Stonehearth Bunker',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (1318,'Snowfall Graveyard',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (1319,'Iceblood Tower',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (1320,'Iceblood Graveyard',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (1321,'Tower Point',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (1322,'Frostwolf Graveyard',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (1323,'East Frostwolf Tower',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (1324,'West Frostwolf Tower',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (1325,'Frostwolf Relief Hut',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (1326,'The Battle for Alterac Valley begins in 1 minute.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (1327,'The Battle for Alterac Valley begins in 30 seconds. Prepare yourselves!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (1328,'The Battle for Alterac Valley has begun!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (1329,'The Alliance Team is running out of reinforcements!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (1330,'The Horde Team is running out of reinforcements!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (1331,'The Frostwolf General is Dead!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (1332,'The Stormpike General is Dead!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (2000,'|cff00ff00New ticket from|r|cffff00ff %s.|r |cff00ff00Ticket entry:|r|cffff00ff %d.|r',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (2001,'|cff00ff00Character|r|cffff00ff %s |r|cff00ff00edited his/her ticket:|r|cffff00ff %d.|r',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (2002,'|cff00ff00Character|r|cffff00ff %s |r|cff00ff00abandoned ticket entry:|r|cffff00ff %d.|r',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (2003,'|cff00ff00Closed by|r:|cff00ccff %s|r ',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (2004,'|cff00ff00Deleted by|r:|cff00ccff %s|r ',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (2005,'Ticket not found.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (2006,'Please close ticket before deleting it permanently.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (2007,'Ticket %d is already assigned.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (2008,'%u Tickets succesfully reloaded from the database.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (2009,'Showing list of open tickets.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (2010,'Showing list of open tickets whose creator is online.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (2011,'Showing list of closed tickets.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (2012,'Invalid name specified. Name should be that of an online Gamemaster.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (2013,'This ticket is already assigned to yourself. To unassign use .ticket unassign %d and then reassign.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (2014,'Ticket %d is not assigned, you cannot unassign it.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (2015,'You cannot unassign tickets from staffmembers with a higher security level than yourself.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (2016,'Cannot close ticket %d, it is assigned to another GM.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (2017,'|cff00ff00Ticket|r:|cff00ccff %d.|r ',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (2018,'|cff00ff00Created by|r:|cff00ccff %s|r ',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (2019,'|cff00ff00Last change|r:|cff00ccff %s ago|r ',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (2020,'|cff00ff00Assigned to|r:|cff00ccff %s|r ',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (2021,'|cff00ff00Unassigned by|r:|cff00ccff %s|r ',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (2022,'\n|cff00ff00Message|r: "%s"|r ',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (2023,'\n|cff00ff00Comment|r: "%s"|r ',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
    (2024,'\n|cff00ccff%s|r |cff00ff00Added comment|r: "%s"|r ',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
