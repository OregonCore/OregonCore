/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET NAMES utf8 */;
/*!50503 SET NAMES utf8mb4 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;

CREATE TABLE IF NOT EXISTS `spell_ranks` (
  `spell_id` int(11) NOT NULL DEFAULT '0',
  `prev_spell_id` int(11) NOT NULL DEFAULT '0',
  `next_spell_id` int(11) NOT NULL DEFAULT '0',
  `first_spell_id` int(11) NOT NULL DEFAULT '0',
  `last_spell_id` int(11) NOT NULL DEFAULT '0',
  `rank` int(11) DEFAULT NULL,
  `comment` varchar(50) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/*!40000 ALTER TABLE `spell_ranks` DISABLE KEYS */;
INSERT INTO `spell_ranks` (`spell_id`, `prev_spell_id`, `next_spell_id`, `first_spell_id`, `last_spell_id`, `rank`, `comment`) VALUES
	(5487, 0, 9634, 5487, 9634, 1, 'Bear Form'),
	(9634, 5487, 0, 5487, 9634, 2, 'Dire Bear Form'),
	(33943, 0, 40120, 33943, 40120, 1, 'Flight Form'),
	(40120, 33943, 0, 33943, 40120, 2, 'Swift Flight Form');
/*!40000 ALTER TABLE `spell_ranks` ENABLE KEYS */;

/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IF(@OLD_FOREIGN_KEY_CHECKS IS NULL, 1, @OLD_FOREIGN_KEY_CHECKS) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
