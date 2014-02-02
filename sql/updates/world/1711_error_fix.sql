-- Hellfire units
update creature_template set faction_A = 1729, faction_H = 1729, unit_flags = (unit_flags & ~(1 << 8)) | 0x1000 where entry = 16580;
update creature_template set faction_A = 1666, faction_H = 1666, unit_flags = (unit_flags & ~(1 << 8)) | 0x1000 where entry = 16842;
delete from creature where guid = 58438 and id = 16887;

-- Startup errors
update spell_affect set effectId = 0 where entry = 28332;
update spell_affect set effectId = 0 where entry = 11189;
delete from spell_affect where entry = 13043;
delete from spell_affect where entry = 11094;
