DROP TABLE IF EXISTS `gm_surveys`;
CREATE TABLE `gm_surveys` (
  `surveyid` int(10) NOT NULL auto_increment,
  `AppropriateAnswer` tinyint(3) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Did the GM provide an appropriate answer or solution to your question/problem?',
  `Understandability` tinyint(3) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'How easy was it to understand and/or follow the information provided?',
  `GMRating` tinyint(3) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Was the GM knowledgeable, respectful and empathetic during your interactions?',
  `ResponseTime` tinyint(3) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'How would you rate the response time to address your petition?',
  `OverallGMExperience` tinyint(3) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'How would you rate the overall experience with the GM that assisted you with your petition?',
  `comment` longtext,
  PRIMARY KEY (`surveyid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Player System';