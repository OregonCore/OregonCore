ALTER TABLE account
  DROP COLUMN active_realm_id,
  ADD COLUMN online INT(4) UNSIGNED NOT NULL DEFAULT '0' AFTER last_login;