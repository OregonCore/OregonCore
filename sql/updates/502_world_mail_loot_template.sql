DROP TABLE IF EXISTS mail_loot_template;
RENAME TABLE quest_mail_loot_template TO mail_loot_template;

-- Remove these fake and false entries causing errors. --
DELETE FROM mail_loot_template WHERE entry = '99';
DELETE FROM mail_loot_template WHERE entry = '100';
DELETE FROM mail_loot_template WHERE entry = '103';
DELETE FROM mail_loot_template WHERE entry = '104';

UPDATE mail_loot_template, quest_template
  SET mail_loot_template.entry = quest_template.RewMailTemplateId WHERE mail_loot_template.entry = quest_template.entry;