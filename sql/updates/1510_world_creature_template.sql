UPDATE `creature_template` SET `ScriptName` = 'npc_anchorite_relic_bunny' WHERE `entry` = '22444';
UPDATE `creature_template` SET `ScriptName` = 'npc_hand_berserker' WHERE `entry` = '16878';
UPDATE `creature_template` SET `unit_flags` = '0' WHERE `entry` = '22444';
UPDATE `gameobject_template` SET `size` = '3' WHERE `entry` = '185298';
INSERT INTO `spell_script_target` VALUES (39184,1,16878);
UPDATE `item_template` SET `spellcooldown_1` = '60000' WHERE `entry` = '31772';
INSERT INTO `script_texts` (`entry`,`content_default`,`content_loc1`,`content_loc2`,`content_loc3`,`content_loc4`,`content_loc5`,`content_loc6`,`content_loc7`,`content_loc8`,`sound`,`type`,`language`,`emote`,`comment`) VALUES
(-1900130,'Vengeance will not be yours. Livine!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_SP');