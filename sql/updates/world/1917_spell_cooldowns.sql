CREATE TABLE spell_cooldown (
  spellid int(11) unsigned NOT NULL default '0',
  cooldown int(11) NOT NULL default '',
  KEY spellid (spellid)
);
