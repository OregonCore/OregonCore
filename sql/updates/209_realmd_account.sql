ALTER TABLE account
  DROP COLUMN online,
  ADD COLUMN active_realm_id int(11) unsigned NOT NULL default '0' AFTER last_login;