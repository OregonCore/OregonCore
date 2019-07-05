-- set new Extraflag for guardians (and imp) that are not supposed to melee.
-- Warlock Pet: Imp
UPDATE `creature_template` SET `flags_extra`=0x00020000 WHERE `Entry`=416;
-- Player Guardian Explosive Sheep
UPDATE `creature_template` SET `flags_extra`=0x00020000 WHERE `Entry`=2675;
-- Player Guardian: Crimson Cannon
UPDATE `creature_template` SET `flags_extra`=0x00020000 WHERE `Entry`=11199;

-- Remove damage and make passive
-- Player Guardian: Alarm-O-Bot - Needs to have its level maximized as well or it's pretty much useless.
UPDATE `creature_template` SET `flags_extra`=0x00020000, `unit_flags`=0x00000200, `MinLevel`=60, `MaxLevel`=60 WHERE `Entry`=14434;

-- Removing damage and movement from guardians which are not supposed to fight back or move.
-- Target Dummy
UPDATE `creature_template` SET `flags_extra`=0x00020000, `unit_flags`=0x00040204 WHERE `Entry`=2673;
-- Advanced target Dummy
UPDATE `creature_template` SET `flags_extra`=0x00020000, `unit_flags`=0x00040204 WHERE `Entry`=2674;
-- Masterwork Target Dummy
UPDATE `creature_template` SET `flags_extra`=0x00020000, `unit_flags`=0x00040204 WHERE `Entry`=12426;

-- Damage needs to be brought in line with at least a trash mob of the same level, especially since it's not supposed to move.
UPDATE `creature_template` SET `unit_flags`=0x00000004 WHERE `Entry`=15901;
-- `MinMeleeDamage`=104, `MaxMeleeDamage`=138,