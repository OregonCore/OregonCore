DELETE FROM `spell_target_position` WHERE `id` = 46841;
INSERT INTO `spell_target_position` VALUES ('46841', '530', '12888', '-6876', '9', '0.3');
UPDATE `gameobject_template` SET `data10` = 46841 WHERE `entry` = 188173;