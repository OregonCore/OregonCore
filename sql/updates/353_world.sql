DELETE FROM command where name IN ('character titles','modify titles','titles add','titles current','titles remove','titles setmask','lookup title');

INSERT INTO `command` VALUES
('character titles',2,'Syntax: .character titles [$player_name]\r\n\r\nShow known titles list for selected player or player find by $player_name.'),
('titles add',2,'Syntax: .titles add #title\r\nAdd title #title (id or shift-link) to known titles list for selected player.'),
('titles current',2,'Syntax: .titles current #title\r\nSet title #title (id or shift-link) as current selected title for selected player. If title not in known title list for player then it will be added to list.'),
('titles remove',2,'Syntax: .titles remove #title\r\nRemove title #title (id or shift-link) from known titles list for selected player.'),
('titles setmask',2,'Syntax: .titles setmask #mask\r\n\r\nAllows user to use all titles from #mask.\r\n\r\n #mask=0 disables the title-choose-field'),
('lookup title',2,'Syntax: .lookup title $$namepart\r\n\r\nLooks up a title by $namepart, and returns all matches with their title ID\'s and index\'s.');

DELETE FROM oregon_string WHERE entry in (349, 350, 351, 352, 353, 354, 355, 356);

INSERT INTO oregon_string VALUES
 (349,'%d (idx:%d) - |cffffffff|Htitle:%d|h[%s %s]|h|r %s %s ',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
 (350,'%d (idx:%d) - [%s %s] %s %s ',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
 (351,'No titles found!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
 (352,'Invalid title id: %u',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
 (353,'Title %u (%s) added to known titles list for player %s.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
 (354,'Title %u (%s) removed from known titles list for player %s.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
 (355,'Title %u (%s) set as current selected title for player %s.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
 (356,'Current selected title for player %s reset as not known now.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
