ALTER TABLE account DROP COLUMN online;
ALTER TABLE account ADD COLUMN active_realm_id int(11) unsigned NOT NULL default '0' AFTER last_login;