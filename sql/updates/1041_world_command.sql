DELETE FROM command where name IN ('modify fly');

INSERT INTO `command` VALUES
 ('modify fly', 1, 'Syntax: .modify fly #rate\r\n.fly #rate\r\n\r\nModify the flying speed of the selected player to \"normal base fly speed\"*rate. If no player is selected, modify your fly.\r\n\r\n #rate may range from 0.1 to 50.');
 