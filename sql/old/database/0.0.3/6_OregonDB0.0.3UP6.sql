-- Update pack 1 for OregonDB 0.0.4

UPDATE `creature_template` SET `ScriptName`='boss_akilzon' WHERE `entry`='23574';
UPDATE `creature_template` SET `ScriptName`='mob_akilzon_eagle' WHERE `entry`='24858';
UPDATE `creature_template` SET `ScriptName`='mob_jadespine_basilisk' WHERE `entry`='4863';
UPDATE `creature_template` SET `ScriptName`='mob_stonekeepers' WHERE `entry`='4857';
UPDATE `creature_template` SET `ScriptName`='npc_brewfest_reveler' WHERE `entry`='24484';
UPDATE `creature_template` SET `ScriptName`='boss_shirrak_the_dead_watcher' WHERE `entry`='18371';
UPDATE `creature_template` SET `ScriptName`='mob_ethereal_apprentice' WHERE `entry`='18430';
UPDATE `creature_template` SET `ScriptName`='npc_volcano' WHERE `entry`='23085';
UPDATE `creature_template` SET `ScriptName`='boss_angerrel' WHERE `entry`='9035';
UPDATE `creature_template` SET `ScriptName`='boss_doperel' WHERE `entry`='9040';
UPDATE `creature_template` SET `ScriptName`='boss_haterel' WHERE `entry`='9034';
UPDATE `creature_template` SET `ScriptName`='boss_vilerel' WHERE `entry`='9036';
UPDATE `creature_template` SET `ScriptName`='boss_seethrel' WHERE `entry`='9038';
UPDATE `creature_template` SET `ScriptName`='boss_gloomrel' WHERE `entry`='9037';
UPDATE `creature_template` SET `ScriptName`='mob_giant_infernal' WHERE `entry`='17908';
UPDATE `creature_template` SET `ScriptName`='mob_abomination' WHERE `entry`='17898';
UPDATE `creature_template` SET `ScriptName`='mob_ghoul' WHERE `entry`='17895';
UPDATE `creature_template` SET `ScriptName`='mob_necromancer' WHERE `entry`='17899';
UPDATE `creature_template` SET `ScriptName`='mob_banshee' WHERE `entry`='17905';
UPDATE `creature_template` SET `ScriptName`='mob_crypt_fiend' WHERE `entry`='17897';
UPDATE `creature_template` SET `ScriptName`='mob_fel_stalker' WHERE `entry`='17916';
UPDATE `creature_template` SET `ScriptName`='mob_frost_wyrm' WHERE `entry`='17907';
UPDATE `creature_template` SET `ScriptName`='mob_gargoyle' WHERE `entry`='17906';
UPDATE `creature_template` SET `ScriptName`='boss_rage_winterchill' WHERE `entry`='17767';
UPDATE `creature_template` SET `ScriptName`='boss_anetheron' WHERE `entry`='17808';
UPDATE `creature_template` SET `ScriptName`='mob_towering_infernal' WHERE `entry`='17818';
UPDATE `creature_template` SET `ScriptName`='boss_kazrogal' WHERE `entry`='17888';
UPDATE `creature_template` SET `ScriptName`='boss_azgalor' WHERE `entry`='17842';
UPDATE `creature_template` SET `ScriptName`='mob_greyheart_spellbinder' WHERE `entry`='21806';
UPDATE `creature_template` SET `ScriptName`='mob_inner_demon' WHERE `entry`='21857';
UPDATE `creature_template` SET `ScriptName`='boss_the_lurker_below' WHERE `entry`='21217';
UPDATE `creature_template` SET `ScriptName`='mob_coilfang_guardian' WHERE `entry`='21873';
UPDATE `creature_template` SET `ScriptName`='mob_coilfang_ambusher' WHERE `entry`='21865';
UPDATE `creature_template` SET `ScriptName`='boss_the_black_stalker' WHERE `entry`='17882';
UPDATE `creature_template` SET `ScriptName`='mob_shadowmoon_channeler' WHERE `entry`='17653';
UPDATE `creature_template` SET `ScriptName`='mob_hellfire_sentry' WHERE `entry`='17517';
UPDATE `creature_template` SET `ScriptName`='mob_sparrowhawk' WHERE `entry`='22979';
UPDATE `creature_template` SET `ScriptName`='mob_phase_hunter' WHERE `entry`='20906';
UPDATE `creature_template` SET `ScriptName`='boss_kurinnaxx' WHERE `entry`='15348';
UPDATE `creature_template` SET `ScriptName`='mob_illidari_spawn' WHERE `entry`='19800';
UPDATE `creature_template` SET `ScriptName`='mob_torloth_the_magnificent' WHERE `entry`='22076';
UPDATE `creature_template` SET `ScriptName`='npc_void_sentinel' WHERE `entry`='25772';
UPDATE `creature_template` SET `ScriptName`='mob_volatile_felfire_fiend' WHERE `entry`='25598';
UPDATE `creature_template` SET `ScriptName`='boss_high_botanist_freywinn' WHERE `entry`='17975';
UPDATE `creature_template` SET `ScriptName`='mob_rotting_forest_rager' WHERE `entry`='22307';

--

DELETE FROM `spell_script_target` WHERE `entry` IN ('8313' , '38968' , '12134');

--

DELETE FROM `spell_affect` WHERE `entry` IN (16998,16999);
INSERT INTO `spell_affect` VALUES (16998,0,4096);
INSERT INTO `spell_affect` VALUES (16998,2,4096);
INSERT INTO `spell_affect` VALUES (16999,0,4096);
INSERT INTO `spell_affect` VALUES (16999,2,4096);

--

UPDATE `creature_template` SET `ScriptName`='npc_wing_commander_dabiree' WHERE `entry`='19409';
UPDATE `creature_template` SET `ScriptName`='npc_wing_commander_brack' WHERE `entry`='19401';

--

UPDATE creature_ai_scripts SET event_flags ='0' WHERE id ='481102';
DELETE FROM `item_template` WHERE entry = 32014;
INSERT INTO `item_template` (`entry`, `class`, `subclass`, `unk0`, `name`, `displayid`, `Quality`, `Flags`, `BuyCount`, `BuyPrice`, `SellPrice`, `InventoryType`, `AllowableClass`, `AllowableRace`, `ItemLevel`, `RequiredLevel`, `RequiredSkill`, `RequiredSkillRank`, `requiredspell`, `requiredhonorrank`, `RequiredCityRank`, `RequiredReputationFaction`, `RequiredReputationRank`, `maxcount`, `stackable`, `ContainerSlots`, `stat_type1`, `stat_value1`, `stat_type2`, `stat_value2`, `stat_type3`, `stat_value3`, `stat_type4`, `stat_value4`, `stat_type5`, `stat_value5`, `stat_type6`, `stat_value6`, `stat_type7`, `stat_value7`, `stat_type8`, `stat_value8`, `stat_type9`, `stat_value9`, `stat_type10`, `stat_value10`, `dmg_min1`, `dmg_max1`, `dmg_type1`, `dmg_min2`, `dmg_max2`, `dmg_type2`, `dmg_min3`, `dmg_max3`, `dmg_type3`, `dmg_min4`, `dmg_max4`, `dmg_type4`, `dmg_min5`, `dmg_max5`, `dmg_type5`, `armor`, `holy_res`, `fire_res`, `nature_res`, `frost_res`, `shadow_res`, `arcane_res`, `delay`, `ammo_type`, `RangedModRange`, `spellid_1`, `spelltrigger_1`, `spellcharges_1`, `spellppmRate_1`, `spellcooldown_1`, `spellcategory_1`, `spellcategorycooldown_1`, `spellid_2`, `spelltrigger_2`, `spellcharges_2`, `spellppmRate_2`, `spellcooldown_2`, `spellcategory_2`, `spellcategorycooldown_2`, `spellid_3`, `spelltrigger_3`, `spellcharges_3`, `spellppmRate_3`, `spellcooldown_3`, `spellcategory_3`, `spellcategorycooldown_3`, `spellid_4`, `spelltrigger_4`, `spellcharges_4`, `spellppmRate_4`, `spellcooldown_4`, `spellcategory_4`, `spellcategorycooldown_4`, `spellid_5`, `spelltrigger_5`, `spellcharges_5`, `spellppmRate_5`, `spellcooldown_5`, `spellcategory_5`, `spellcategorycooldown_5`, `bonding`, `description`, `PageText`, `LanguageID`, `PageMaterial`, `startquest`, `lockid`, `Material`, `sheath`, `RandomProperty`, `RandomSuffix`, `block`, `itemset`, `MaxDurability`, `area`, `Map`, `BagFamily`, `TotemCategory`, `socketColor_1`, `socketContent_1`, `socketColor_2`, `socketContent_2`, `socketColor_3`, `socketContent_3`, `socketBonus`, `GemProperties`, `RequiredDisenchantSkill`, `ArmorDamageModifier`, `ScriptName`, `DisenchantID`, `FoodType`, `minMoneyLoot`, `maxMoneyLoot`, `Duration`) VALUES('32014','2','5','-1','Merciless Gladiator\'s Maul','41558','4','32768','1','0','0','17','-1','-1','136','70','0','0','0','0','0','0','0','0','1','0','7','55','4','42','32','42','35','33','31','18','0','0','0','0','0','0','0','0','0','0','203','305','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','2000','0','0','44915','1','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','1','1','0','0','0','0','120','0','0','0','0','0','0','0','0','0','0','0','0','-1','-56.3','','0','0','0','0','0');
DELETE FROM `item_template` WHERE entry = 28476;

INSERT INTO `item_template` (`entry`, `class`, `subclass`, `unk0`, `name`, `displayid`, `Quality`, `Flags`, `BuyCount`, `BuyPrice`, `SellPrice`, `InventoryType`, `AllowableClass`, `AllowableRace`, `ItemLevel`, `RequiredLevel`, `RequiredSkill`, `RequiredSkillRank`, `requiredspell`, `requiredhonorrank`, `RequiredCityRank`, `RequiredReputationFaction`, `RequiredReputationRank`, `maxcount`, `stackable`, `ContainerSlots`, `stat_type1`, `stat_value1`, `stat_type2`, `stat_value2`, `stat_type3`, `stat_value3`, `stat_type4`, `stat_value4`, `stat_type5`, `stat_value5`, `stat_type6`, `stat_value6`, `stat_type7`, `stat_value7`, `stat_type8`, `stat_value8`, `stat_type9`, `stat_value9`, `stat_type10`, `stat_value10`, `dmg_min1`, `dmg_max1`, `dmg_type1`, `dmg_min2`, `dmg_max2`, `dmg_type2`, `dmg_min3`, `dmg_max3`, `dmg_type3`, `dmg_min4`, `dmg_max4`, `dmg_type4`, `dmg_min5`, `dmg_max5`, `dmg_type5`, `armor`, `holy_res`, `fire_res`, `nature_res`, `frost_res`, `shadow_res`, `arcane_res`, `delay`, `ammo_type`, `RangedModRange`, `spellid_1`, `spelltrigger_1`, `spellcharges_1`, `spellppmRate_1`, `spellcooldown_1`, `spellcategory_1`, `spellcategorycooldown_1`, `spellid_2`, `spelltrigger_2`, `spellcharges_2`, `spellppmRate_2`, `spellcooldown_2`, `spellcategory_2`, `spellcategorycooldown_2`, `spellid_3`, `spelltrigger_3`, `spellcharges_3`, `spellppmRate_3`, `spellcooldown_3`, `spellcategory_3`, `spellcategorycooldown_3`, `spellid_4`, `spelltrigger_4`, `spellcharges_4`, `spellppmRate_4`, `spellcooldown_4`, `spellcategory_4`, `spellcategorycooldown_4`, `spellid_5`, `spelltrigger_5`, `spellcharges_5`, `spellppmRate_5`, `spellcooldown_5`, `spellcategory_5`, `spellcategorycooldown_5`, `bonding`, `description`, `PageText`, `LanguageID`, `PageMaterial`, `startquest`, `lockid`, `Material`, `sheath`, `RandomProperty`, `RandomSuffix`, `block`, `itemset`, `MaxDurability`, `area`, `Map`, `BagFamily`, `TotemCategory`, `socketColor_1`, `socketContent_1`, `socketColor_2`, `socketContent_2`, `socketColor_3`, `socketContent_3`, `socketBonus`, `GemProperties`, `RequiredDisenchantSkill`, `ArmorDamageModifier`, `ScriptName`, `DisenchantID`, `FoodType`, `minMoneyLoot`, `maxMoneyLoot`, `Duration`) VALUES('28476','2','5','-1','Gladiator\'s Maul','41558','4','32768','1','0','0','17','-1','-1','123','70','0','0','0','0','0','0','0','0','1','0','7','48','4','32','32','35','35','28','31','20','0','0','0','0','0','0','0','0','0','0','189','285','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','2000','0','0','44912','1','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','1','1','0','0','0','0','120','0','0','0','0','0','0','0','0','0','0','0','0','-1','-49.8','','0','0','0','0','0');

--


