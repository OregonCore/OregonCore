-- --------------------------------------------- --
-- Oregon Script to Remove MySQL User and Tables --
-- --------------------------------------------- --

REVOKE ALL PRIVILEGES ON * . * FROM 'oregon'@'localhost';

REVOKE ALL PRIVILEGES ON `world` . * FROM 'oregon'@'localhost';
REVOKE GRANT OPTION ON `world` . * FROM 'oregon'@'localhost';

REVOKE ALL PRIVILEGES ON `characters` . * FROM 'oregon'@'localhost';
REVOKE GRANT OPTION ON `characters` . * FROM 'oregon'@'localhost';

REVOKE ALL PRIVILEGES ON `realmd` . * FROM 'oregon'@'localhost';
REVOKE GRANT OPTION ON `realmd` . * FROM 'oregon'@'localhost';

DROP USER 'oregon'@'localhost';

DROP DATABASE IF EXISTS `world` ;
DROP DATABASE IF EXISTS `characters` ;
DROP DATABASE IF EXISTS `realmd` ;