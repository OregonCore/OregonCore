DELETE FROM command where name IN ('modify scale');

INSERT INTO `command` VALUES
 ('modify scale',1,'Syntax: .modify scale #scale\r\n\r\nChange model scale for targeted player(until re-login), or creature(until respawn).');