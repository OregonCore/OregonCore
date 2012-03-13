ALTER TABLE `creature_template`
    ADD COLUMN `KillCredit1` int(11) unsigned NOT NULL default '0' AFTER `heroic_entry`,
    ADD COLUMN `KillCredit2` int(11) unsigned NOT NULL default '0' AFTER `KillCredit1`;
