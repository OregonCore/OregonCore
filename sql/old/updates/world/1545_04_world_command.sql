DELETE FROM `command` WHERE `name`='npc add group';
INSERT INTO `command` (`name`, `security`, `help`) VALUES
('npc add group', 1, 'Syntax: .npc add group $leader $command $groupType\r\nAdd selected creature to a leader\'s group with groupType.\r\nYou could also add a comment...');

DELETE FROM `command` WHERE `name`='npc add formation';
INSERT INTO `command` (`name`, `security`, `help`) VALUES
('npc add formation', 1, 'Syntax: .npc add formation $leader $command $formationAI\r\nAdd selected creature to a leader\'s formation with formationAI.\r\nYou could also add a comment...');