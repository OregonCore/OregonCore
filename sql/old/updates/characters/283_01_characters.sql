ALTER TABLE characters ADD gender TINYINT UNSIGNED NOT NULL default '0' AFTER class;
ALTER TABLE characters ADD level TINYINT UNSIGNED NOT NULL default '0' AFTER gender;
ALTER TABLE characters ADD xp INT UNSIGNED NOT NULL default '0' AFTER level;
ALTER TABLE characters ADD money INT UNSIGNED NOT NULL default '0' AFTER xp;
ALTER TABLE characters ADD playerBytes INT UNSIGNED NOT NULL default '0' AFTER money;
ALTER TABLE characters ADD playerBytes2 INT UNSIGNED NOT NULL default '0' AFTER playerBytes;
ALTER TABLE characters ADD playerFlags INT UNSIGNED NOT NULL default '0' AFTER playerBytes2;