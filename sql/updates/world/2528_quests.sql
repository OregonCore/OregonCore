-- 10162 (The Abyssal Shelf) - cant' complete quest after complete all objectives:
UPDATE `quest_template` SET `SpecialFlags` = '1' WHERE `entry` = '10162';

-- 9685 (Redeeming the Dead) - can't use item 24184 on target:
UPDATE `spell_script_target` SET `type` = '2' WHERE `entry` = '31225';
