DROP TABLE IF EXISTS `reputation_spillover_template`;
CREATE TABLE `reputation_spillover_template` (
  `faction` SMALLINT(6) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'faction entry',
  `faction1` SMALLINT(6) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'faction to give spillover for',
  `rate_1` FLOAT NOT NULL DEFAULT '0' COMMENT 'the given rep points * rate',
  `rank_1` TINYINT(3) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'max rank, above this will not give any spillover',
  `faction2` SMALLINT(6) UNSIGNED NOT NULL DEFAULT '0',
  `rate_2` FLOAT NOT NULL DEFAULT '0',
  `rank_2` TINYINT(3) UNSIGNED NOT NULL DEFAULT '0',
  `faction3` SMALLINT(6) UNSIGNED NOT NULL DEFAULT '0',
  `rate_3` FLOAT NOT NULL DEFAULT '0',
  `rank_3` TINYINT(3) UNSIGNED NOT NULL DEFAULT '0',
  `faction4` SMALLINT(6) UNSIGNED NOT NULL DEFAULT '0',
  `rate_4` FLOAT NOT NULL DEFAULT '0',
  `rank_4` TINYINT(3) UNSIGNED NOT NULL DEFAULT '0',
  PRIMARY KEY  (`faction`)
) ENGINE=MYISAM DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Reputation spillover reputation gain';

