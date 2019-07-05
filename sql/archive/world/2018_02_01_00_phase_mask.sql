ALTER TABLE creature
ADD COLUMN `phaseMask` INT(5) UNSIGNED NOT NULL DEFAULT '1' AFTER `spawnMask`;

ALTER TABLE gameobject
ADD COLUMN `phaseMask` INT(5) UNSIGNED NOT NULL DEFAULT '1' AFTER `spawnMask`;

INSERT INTO `command` VALUES
('gobject phase',3,'Syntax: .gobject phase #guid #phasemask\r\n\r\nGameobject with DB guid #guid phasemask changed to #phasemask with related world vision update for players. Gameobject state saved to DB and persistent.'),
('modify phase',3,'Syntax: .modify phase #phasemask\r\n\r\nSelected character phasemask changed to #phasemask with related world vision update. Change active until in game phase changed, or GM-mode enable/disable, or re-login. Character pts pasemask update to same value.'),
('npc phase',3,'Syntax: .npc phase #phasemask\r\n\r\nSelected unit or pet phasemask changed to #phasemask with related world vision update for players. In creature case state saved to DB and persistent. In pet case change active until in game phase changed for owner, owner re-login, or GM-mode enable/disable..');
