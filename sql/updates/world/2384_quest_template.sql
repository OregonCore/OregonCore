-- Issue 7907: Windcaller Yessendra (15498),Warden Haro (15499),Keyl Swiftclaw (15500),Mokvar (16012) & Deliana (16013) quests 
UPDATE `quest_template` SET `RequiredClasses`=1 WHERE `entry`=8556; -- Warrior
UPDATE `quest_template` SET `RequiredClasses`=2 WHERE `entry`=8703; -- Paladin
UPDATE `quest_template` SET `RequiredClasses`=4 WHERE `entry`=8704; -- Hunter
UPDATE `quest_template` SET `RequiredClasses`=8 WHERE `entry`=8701; -- Rogue
UPDATE `quest_template` SET `RequiredClasses`=16 WHERE `entry`=8697; -- Priest
UPDATE `quest_template` SET `RequiredClasses`=64 WHERE `entry`=8698; -- Shaman
UPDATE `quest_template` SET `RequiredClasses`=128 WHERE `entry`=8699; -- Mage
UPDATE `quest_template` SET `RequiredClasses`=256 WHERE `entry`=8702; -- Warlock
UPDATE `quest_template` SET `RequiredClasses`=1024 WHERE `entry`=8700; -- Druid
-- Warden Haro (15499) quests RequiredClasses fix by nelegalno
UPDATE `quest_template` SET `RequiredClasses`=1 WHERE `entry`=8558; -- Warrior
UPDATE `quest_template` SET `RequiredClasses`=2 WHERE `entry`=8711; -- Paladin
UPDATE `quest_template` SET `RequiredClasses`=4 WHERE `entry`=8712; -- Hunter
UPDATE `quest_template` SET `RequiredClasses`=8 WHERE `entry`=8709; -- Rogue
UPDATE `quest_template` SET `RequiredClasses`=16 WHERE `entry`=8705; -- Priest
UPDATE `quest_template` SET `RequiredClasses`=64 WHERE `entry`=8706; -- Shaman
UPDATE `quest_template` SET `RequiredClasses`=128 WHERE `entry`=8707; -- Mage
UPDATE `quest_template` SET `RequiredClasses`=256 WHERE `entry`=8710; -- Warlock
UPDATE `quest_template` SET `RequiredClasses`=1024 WHERE `entry`=8708; -- Druid
-- Keyl Swiftclaw (15500) quests RequiredClasses fix by nelegalno
UPDATE `quest_template` SET `RequiredClasses`=1 WHERE `entry`=8557; -- Warrior
UPDATE `quest_template` SET `RequiredClasses`=2 WHERE `entry`=8695; -- Paladin
UPDATE `quest_template` SET `RequiredClasses`=4 WHERE `entry`=8696; -- Hunter
UPDATE `quest_template` SET `RequiredClasses`=8 WHERE `entry`=8693; -- Rogue
UPDATE `quest_template` SET `RequiredClasses`=16 WHERE `entry`=8689; -- Priest
UPDATE `quest_template` SET `RequiredClasses`=64 WHERE `entry`=8690; -- Shaman
UPDATE `quest_template` SET `RequiredClasses`=128 WHERE `entry`=8691; -- Mage
UPDATE `quest_template` SET `RequiredClasses`=256 WHERE `entry`=8694; -- Warlock
UPDATE `quest_template` SET `RequiredClasses`=1024 WHERE `entry`=8692; -- Druid
-- Mokvar (16012) quests RequiredClasses fix by nelegalno
UPDATE `quest_template` SET `RequiredClasses`=1 WHERE `entry` IN (8920,8944,9013,9022); -- Warrior
UPDATE `quest_template` SET `RequiredClasses`=2 WHERE `entry` IN (10493,10495,10497,10499); -- Paladin
UPDATE `quest_template` SET `RequiredClasses`=4 WHERE `entry` IN (8914,8938,9008,9017); -- Hunter
UPDATE `quest_template` SET `RequiredClasses`=8 WHERE `entry` IN (8917,8941,9010,9020); -- Rogue
UPDATE `quest_template` SET `RequiredClasses`=16 WHERE `entry` IN (8916,8940,9009,9019); -- Priest
UPDATE `quest_template` SET `RequiredClasses`=64 WHERE `entry` IN (8918,8942,8957,9011); -- Shaman
UPDATE `quest_template` SET `RequiredClasses`=128 WHERE `entry` IN (8915,8939,9014,9018); -- Mage
UPDATE `quest_template` SET `RequiredClasses`=256 WHERE `entry` IN (8919,8943,9012,9021); -- Warlock
UPDATE `quest_template` SET `RequiredClasses`=1024 WHERE `entry` IN (8913,8927,9007,9016); -- Druid
-- Deliana (16013) quests RequiredClasses fix by nelegalno
UPDATE `quest_template` SET `RequiredClasses`=1 WHERE `entry` IN (8912,8959,8937,9006); -- Warrior
UPDATE `quest_template` SET `RequiredClasses`=2 WHERE `entry` IN (8908,8954,10494,9002); -- Paladin
UPDATE `quest_template` SET `RequiredClasses`=4 WHERE `entry` IN (8906,8952,8931,9000); -- Hunter
UPDATE `quest_template` SET `RequiredClasses`=8 WHERE `entry` IN (8910,8956,8935,9004); -- Rogue
UPDATE `quest_template` SET `RequiredClasses`=16 WHERE `entry` IN (8909,8955,8934,9003); -- Priest
UPDATE `quest_template` SET `RequiredClasses`=64 WHERE `entry` IN (10492,10496,10494,10498); -- Shaman
UPDATE `quest_template` SET `RequiredClasses`=128 WHERE `entry` IN (8907,8953,8932,9001); -- Mage
UPDATE `quest_template` SET `RequiredClasses`=256 WHERE `entry` IN (8911,8958,8936,9005); -- Warlock
UPDATE `quest_template` SET `RequiredClasses`=1024 WHERE `entry` IN (8905,8951,8926,8999); -- Druid

-- Zandalar Tribe Quests Required Class/Race fix by nelegalno
-- Maywiki of Zuldazar Quests Required Class
UPDATE `quest_template` SET `RequiredClasses` = 64 WHERE `entry` IN (8056,8074,8075,8116,8117,8118,8119); -- Shaman
UPDATE `quest_template` SET `RequiredClasses` = 1024 WHERE `entry` IN (8057,8064,8065,8110,8111,8112,8113); -- Druid

-- Al'tabim the All-Seeing Quests Required Class
UPDATE `quest_template` SET `RequiredClasses` = 16 WHERE `entry` IN (8049,8050,8051,8052,8061,8070,8071); -- Priest
UPDATE `quest_template` SET `RequiredClasses` = 128 WHERE `entry` IN (8060,8068,8069,8101,8102,8103,8104); -- Mage
UPDATE `quest_template` SET `RequiredClasses` = 256 WHERE `entry` IN (8059,8076,8077,8106,8107,8108,8109); -- Warlock

-- Falthir the Sightless Quests Required Class
UPDATE `quest_template` SET `RequiredClasses` = 4 WHERE `entry` IN (8062,8066,8067,8145,8146,8147,8148); -- Hunter
UPDATE `quest_template` SET `RequiredClasses` = 8 WHERE `entry` IN (8063,8072,8073,8141,8142,8143,8144); -- Rogue
UPDATE `quest_template` SET `RequiredRaces` = 152 WHERE `entry` = 8144; -- Night Elf, Undead and Troll

-- Jin'rokh the Breaker
UPDATE `quest_template` SET `RequiredRaces` = 513 WHERE `entry` = 8048; -- Human and Blood Elf

-- Kandrostrasz <Brood of Alexstrasza> (15503) quests RequiredClasses fix by nelegalno
UPDATE `quest_template` SET `RequiredClasses` = 1 WHERE `entry` IN (8559,8560); -- Warrior
UPDATE `quest_template` SET `RequiredClasses` = 2 WHERE `entry` IN (8629,8655); -- Paladin
UPDATE `quest_template` SET `RequiredClasses` = 4 WHERE `entry` IN (8626,8658); -- Hunter
UPDATE `quest_template` SET `RequiredClasses` = 8 WHERE `entry` IN (8637,8640); -- Rogue
UPDATE `quest_template` SET `RequiredClasses` = 16 WHERE `entry` IN (8593,8596); -- Priest
UPDATE `quest_template` SET `RequiredClasses` = 64 WHERE `entry` IN (8621,8624); -- Shaman
UPDATE `quest_template` SET `RequiredClasses` = 128 WHERE `entry` IN (8631,8634); -- Mage
UPDATE `quest_template` SET `RequiredClasses` = 256 WHERE `entry` IN (8660,8663); -- Warlock
UPDATE `quest_template` SET `RequiredClasses` = 1024 WHERE `entry` IN (8665,8668); -- Druid

-- Andorgos <Brood of Malygos> (15502) quests RequiredClasses fix by nelegalno
UPDATE `quest_template` SET `RequiredClasses` = 1 WHERE `entry` IN (8544,8561); -- Warrior
UPDATE `quest_template` SET `RequiredClasses` = 2 WHERE `entry` IN (8628,8630); -- Paladin
UPDATE `quest_template` SET `RequiredClasses` = 4 WHERE `entry` IN (8657,8659); -- Hunter
UPDATE `quest_template` SET `RequiredClasses` = 8 WHERE `entry` IN (8639,8641); -- Rogue
UPDATE `quest_template` SET `RequiredClasses` = 16 WHERE `entry` IN (8592,8594); -- Priest
UPDATE `quest_template` SET `RequiredClasses` = 64 WHERE `entry` IN (8602,8623); -- Shaman
UPDATE `quest_template` SET `RequiredClasses` = 128 WHERE `entry` IN (8625,8632); -- Mage
UPDATE `quest_template` SET `RequiredClasses` = 256 WHERE `entry` IN (8662,8664); -- Warlock
UPDATE `quest_template` SET `RequiredClasses` = 1024 WHERE `entry` IN (8667,8669); -- Druid

-- Craftsman Wilhelm <Brotherhood of the Light> (16376) NPCs quests
UPDATE `quest_template` SET `RequiredClasses` = 3 WHERE `entry` IN (9234,9235,9236); -- Warrior, Paladin
UPDATE `quest_template` SET `RequiredClasses` = 68 WHERE `entry` IN (9244,9245,9246); -- Hunter, Shaman
UPDATE `quest_template` SET `RequiredClasses` = 400 WHERE `entry` IN (9238,9239,9240); -- Priest, Mage, Warlock
UPDATE `quest_template` SET `RequiredClasses` = 1032 WHERE `entry` IN (9241,9242,9243); -- Rogue, Druid

-- Zanza the Restless (15042) NPCs quests
UPDATE `quest_template` SET `RequiredClasses` = 1 WHERE `entry` = 8184; -- Warrior
UPDATE `quest_template` SET `RequiredClasses` = 2 WHERE `entry` = 8185; -- Paladin
UPDATE `quest_template` SET `RequiredClasses` = 4 WHERE `entry` = 8187; -- Hunter
UPDATE `quest_template` SET `RequiredClasses` = 8 WHERE `entry` = 8186; -- Rogue
UPDATE `quest_template` SET `RequiredClasses` = 16 WHERE `entry` = 8191; -- Priest
UPDATE `quest_template` SET `RequiredClasses` = 64 WHERE `entry` = 8188; -- Shaman
UPDATE `quest_template` SET `RequiredClasses` = 128 WHERE `entry` = 8189; -- Mage
UPDATE `quest_template` SET `RequiredClasses` = 256 WHERE `entry` = 8190; -- Warlock
UPDATE `quest_template` SET `RequiredClasses` = 1024 WHERE `entry` = 8192; -- Druid

-- Vethsera <Brood of Ysera> (15504) quests RequiredClasses fix by nelegalno
UPDATE `quest_template` SET `RequiredClasses` = 1 WHERE `entry` = 8562; -- Warrior
UPDATE `quest_template` SET `RequiredClasses` = 2 WHERE `entry` = 8627; -- Paladin
UPDATE `quest_template` SET `RequiredClasses` = 4 WHERE `entry` = 8656; -- Hunter
UPDATE `quest_template` SET `RequiredClasses` = 8 WHERE `entry` = 8638; -- Rogue
UPDATE `quest_template` SET `RequiredClasses` = 16 WHERE `entry` = 8603; -- Priest
UPDATE `quest_template` SET `RequiredClasses` = 64 WHERE `entry` = 8622; -- Shaman
UPDATE `quest_template` SET `RequiredClasses` = 128 WHERE `entry` = 8633; -- Mage
UPDATE `quest_template` SET `RequiredClasses` = 256 WHERE `entry` = 8661; -- Warlock
UPDATE `quest_template` SET `RequiredClasses` = 1024 WHERE `entry` = 8666; -- Druid

-- Attunement to the Core (7848) quest RequiredRaces fix by nelegalno
UPDATE `quest_template` SET `RequiredRaces`=1101 WHERE `entry`=7848; -- A
UPDATE `quest_template` SET `RequiredRaces`=690 WHERE `entry`=7487; -- H

-- Fixing Required Races on Quests 8553 and 8554 to allow for Alliance completion
UPDATE `quest_template` SET `RequiredRaces`=0 WHERE `entry`=8554;
UPDATE `quest_template` SET `RequiredRaces`=0 WHERE `entry`=8553;

-- Stalk the Stalker (9719) quest requirements fix
UPDATE `quest_template` SET `RequiredRaces` = 0 WHERE `entry` = 9719;

