-- --------------------------------------------- --
-- Oregon Script to Create MySQL User and Tables --
-- --------------------------------------------- --

GRANT USAGE ON * . * TO 'oregon'@'localhost' IDENTIFIED BY 'oregon' WITH MAX_QUERIES_PER_HOUR 0 MAX_CONNECTIONS_PER_HOUR 0 MAX_UPDATES_PER_HOUR 0 ;

CREATE DATABASE `world` DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;
CREATE DATABASE `characters` DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;
CREATE DATABASE `realmd` DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;

GRANT ALL PRIVILEGES ON `world` . * TO 'oregon'@'localhost' WITH GRANT OPTION;
GRANT ALL PRIVILEGES ON `characters` . * TO 'oregon'@'localhost' WITH GRANT OPTION;
GRANT ALL PRIVILEGES ON `realmd` . * TO 'oregon'@'localhost' WITH GRANT OPTION;