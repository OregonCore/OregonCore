-- Adds missing texts attached to gossip menus
DELETE FROM `npc_text` WHERE `id` IN (12596,12598,12599,12600,12601,12604);
INSERT INTO `npc_text` (`ID`, `text0_0`, `text0_1`, `lang0`, `prob0`, `em0_0`, `em0_1`, `em0_2`, `em0_3`, `em0_4`, `em0_5`, `text1_0`, `text1_1`, `lang1`, `prob1`, `em1_0`, `em1_1`, `em1_2`, `em1_3`, `em1_4`, `em1_5`, `text2_0`, `text2_1`, `lang2`, `prob2`, `em2_0`, `em2_1`, `em2_2`, `em2_3`, `em2_4`, `em2_5`, `text3_0`, `text3_1`, `lang3`, `prob3`, `em3_0`, `em3_1`, `em3_2`, `em3_3`, `em3_4`, `em3_5`, `text4_0`, `text4_1`, `lang4`, `prob4`, `em4_0`, `em4_1`, `em4_2`, `em4_3`, `em4_4`, `em4_5`, `text5_0`, `text5_1`, `lang5`, `prob5`, `em5_0`, `em5_1`, `em5_2`, `em5_3`, `em5_4`, `em5_5`, `text6_0`, `text6_1`, `lang6`, `prob6`, `em6_0`, `em6_1`, `em6_2`, `em6_3`, `em6_4`, `em6_5`, `text7_0`, `text7_1`, `lang7`, `prob7`, `em7_0`, `em7_1`, `em7_2`, `em7_3`, `em7_4`, `em7_5`) VALUES 
(12596, '', 'I\'m glad you are here, $c. The military situation in the Sunwell Plateau is rather grim.', 0, 1, 0, 396, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0),
(12598, '', 'The Shattered Sun Offensive has made great progress in the outer areas of the Isle of Quel\'Danas. Kil\'jaeden\'s forces are being pushed back, and in the chaos I was able to lead a small scouting force here inside the Sunwell Plateau. My troops proceeded through Parhelion Plaza, spotted a group of Sunblade blood elves, and we then engaged the enemy. That fight quickly turned into a disaster.', 0, 1, 0, 396, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0),
(12599, '', 'Kil\'jaeden\'s defenders here on the Sunwell Plateau are much stronger than anything we faced outside on the Island. The Legion have even twisted our Arcane Guardian technology with fel energy to create something much more sinister. Just one of those Sunblade Protectors decimated my troops, and I ordered a swift retreat. I expected my squad to be pushed completely off the plateau, but the enemy surprised me by calling off the pursuit.', 0, 1, 0, 396, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0),
(12600, '', 'I don\'t know, but I\'m glad they did. The respite has allowed Anchorite Elbadon to save many of the men and women under my command. I\'ve sent a few of my stealthiest scouts forward, and they\'ve reported back something interesting. Nearly all of the Sunblade Protectors are powered down! Kil\'jaeden\'s forces appear to be conserving their energy, and seem content to merely contain us with one active Protector. If you and your companions want to force your way inside, allow me to suggest a plan of attack.', 0, 1, 0, 396, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0),
(12601, '', 'Take advantage of their deactivated Sunblade Protectors. If one of their Sunblade Scouts spot you, I\'m sure the elf will try to reactivate a nearby Sunblade Protector. Kill him quickly to prevent this. With the blood elves eliminated, as long as you do not trigger the Protector\'s proximity sensors you should be able to move past them without a fight. That will save you both time and resources.', 0, 1, 0, 396, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0),
(12604, 'All of the barriers blocking you from reaching the Sunwell have been destroyed. Enter the Sunwell and stop Kil\'jaeden from entering this world!', '', 0, 1, 0, 5, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0);

-- Removes unpopulated loot templates (wowhead says they shouldn't drop loot anyhow)
UPDATE `creature_template` SET `lootid`=0 WHERE `entry` IN (2462,10388,10389,10684,10698,10925,10928,17887);

-- Invalid unit_class, can't be set to 0
UPDATE `creature_template` SET `unit_class`=1 WHERE `entry` IN (20610,20777);

-- Fix gossip menu errors
DELETE FROM `gossip_menu` WHERE `entry`=8718;
INSERT INTO `gossip_menu` (`entry`,`text_id`) VALUES (8718,10977), (8718, 10978);
DELETE FROM `gossip_menu` WHERE `entry`=8719;
INSERT INTO `gossip_menu` (`entry`,`text_id`) VALUES (8719,10979), (8719, 10980);

-- Fix spawndist error messages on startup
UPDATE creature SET MovementType=2 WHERE id IN (10604, 10606);

-- Remove unused EventAI scripts
DELETE FROM creature_ai_scripts WHERE creature_id IN (17713, 1328, 22181, 19546, 24922);

-- Remove unused EventAI texts
DELETE FROM creature_ai_texts WHERE entry IN (-964, -825, -824, -823, -822, -59);

-- Remove unused EventAI summons
DELETE FROM creature_ai_summons WHERE id IN (2, 3);

-- Assign scripts
UPDATE creature_template SET ScriptName="npc_celebras_the_cursed" WHERE entry=12225;

-- Fix creatures with BIND flag
UPDATE creature_template SET flags_extra = flags_extra & ~1 WHERE entry IN (7937,14890,2784,1284,1748,14887,3057,15192,4968,10942,4949,16800,16802,17874,18063,18141,18728,17008,17711,17468,10540,19556,2425,10181,25158,25160,14888,6109,15481,7999,3516,14889,7846);