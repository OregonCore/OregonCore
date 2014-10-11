-- Add flying flag for Neltharaku
UPDATE `creature_template` SET `InhabitType`=4 WHERE `entry`=21657;

-- Voidwalker Minions are not meant to drop loot
DELETE FROM `creature_loot_template` WHERE `entry` =8996;
UPDATE `creature_template` SET `lootid`=0, `mingold`=0, `maxgold`=0 WHERE `entry`=8996;

-- Lunar Festival Invitation - Teleport Moonglade fix
DELETE FROM spell_linked_spell WHERE spell_trigger=26373;
INSERT INTO spell_linked_spell VALUES (26373,26448,0,'Lunar Festival Invitation - Teleport Moonglade');

-- Fix weapons not displaying for creatures in Slave Pens
DELETE FROM `creature_template_addon` WHERE `entry`=17940;
INSERT INTO `creature_template_addon` (`entry`, `bytes2`) VALUES (17940,1);
UPDATE `creature_addon` SET `bytes2`=1 WHERE guid IN (SELECT guid FROM creature WHERE id = 17940);

DELETE FROM `creature_template_addon` WHERE `entry`=17957;
INSERT INTO `creature_template_addon` (`entry`, `bytes2`) VALUES (17957,1);
UPDATE `creature_addon` SET `bytes2`=1 WHERE guid IN (SELECT guid FROM creature WHERE id = 17957);

DELETE FROM `creature_template_addon` WHERE `entry`=17958;
INSERT INTO `creature_template_addon` (`entry`, `bytes2`) VALUES (17958,1);
UPDATE `creature_addon` SET `bytes2`=1 WHERE guid IN (SELECT guid FROM creature WHERE id = 17958);

DELETE FROM `creature_template_addon` WHERE `entry`=17959;
INSERT INTO `creature_template_addon` (`entry`, `bytes2`) VALUES (17959,1);
UPDATE `creature_addon` SET `bytes2`=1 WHERE guid IN (SELECT guid FROM creature WHERE id = 17959);

DELETE FROM `creature_template_addon` WHERE `entry`=17961;
INSERT INTO `creature_template_addon` (`entry`, `bytes2`) VALUES (17961,1);
UPDATE `creature_addon` SET `bytes2`=1 WHERE guid IN (SELECT guid FROM creature WHERE id = 17961);

DELETE FROM `creature_template_addon` WHERE `entry`=17962;
INSERT INTO `creature_template_addon` (`entry`, `bytes2`) VALUES (17962,1);
UPDATE `creature_addon` SET `bytes2`=1 WHERE guid IN (SELECT guid FROM creature WHERE id = 17962);

-- Update a couple of waypoints in Slave Pens
-- Pathing for Mennu the Betrayer Entry: 17941
SET @NPC := (SELECT guid FROM creature WHERE id=17941);
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=89.26869,`position_y`=-380.2368,`position_z`=15.0899 WHERE `guid`=@NPC;
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,89.26869,-380.2368,15.0899,0,0,0,0,100,0),
(@PATH,2,121.6363,-380.3765,29.95734,0,1000,0,0,100,0),
(@PATH,3,89.42728,-380.2369,15.12144,0,0,0,0,100,0),
(@PATH,4,49.4763,-380.2191,3.035575,0,0,0,0,100,0);

-- Pathing for Rokmar the Crackler Entry: 17991
SET @NPC := (SELECT guid FROM creature WHERE id=17991);
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=-34.37422,`position_y`=-458.817,`position_z`=-1.952406 WHERE `guid`=@NPC;
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,-34.37422,-458.817,-1.952406,0,1000,0,0,100,0),
(@PATH,2,-62.09498,-454.9725,-1.592298,0,0,0,0,100,0),
(@PATH,3,-13.55803,-454.8065,2.49773,0,0,0,0,100,0),
(@PATH,4,18.32117,-448.4476,3.055895,0,0,0,0,100,0),
(@PATH,5,-13.55803,-454.8065,2.49773,0,0,0,0,100,0),
(@PATH,6,-62.09498,-454.9725,-1.592298,0,0,0,0,100,0);

-- Fix Lunar Festival Invitation teleport coordinates.
DELETE FROM `spell_target_position` WHERE `id`=26448;
INSERT INTO `spell_target_position` (`id`,`target_map`,`target_position_x`,`target_position_y`,`target_position_z`,`target_orientation`) VALUES
(26448,1,7581.01,-2225.35,473.64,1.80);

-- Correct felmyst gas cloud model
UPDATE `creature` SET `modelid`='17055' WHERE id = 17407;

-- Update OfferRewardText for quest 396
UPDATE `quest_template` SET `OfferRewardText`='You are to be commended for the service you have done for Stormwind, $N. If VanCleef had been successful in his machinations, he would no doubt have done considerable harm to the kingdom--something we must obviously avoid.$B$BTake this, $C, as a token of the my appreciation.' WHERE `Id`=396;

-- Spell Dummy Conditions
INSERT INTO spell_dummy_condition VALUES
(21862, 0, 1 << 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Radiation Visual');

-- Ogre Heaven fix PrevQuestID by shlomi1515
-- Closes #2732
UPDATE quest_template SET PrevQuestID='11000' WHERE entry='11009';

-- Various trigger fixes
-- Razaani Light Orb Fixup
SET @ENTRY := 20635; -- NPC entry
UPDATE `creature` SET `modelid`=0 WHERE `id`=@ENTRY;
UPDATE `creature_template` SET `InhabitType`=7 WHERE `entry`=@ENTRY;
-- Blade's Edge - Orb Trigger 01 Fixup and set as trigger
SET @ENTRY := 20666; -- NPC entry
UPDATE `creature` SET `modelid`=0,`spawndist`=0,`MovementType`=0 WHERE `id`=@ENTRY;
UPDATE `creature_template` SET `InhabitType`=7,`flags_extra`=`flags_extra`|128 WHERE `entry`=@ENTRY;
-- Blade's Edge - Flesh Beast Zap Trigger Fixup and set as trigger
SET @ENTRY := 20670; -- NPC entry
UPDATE `creature` SET `modelid`=0,`spawndist`=0,`MovementType`=0 WHERE `id`=@ENTRY;
UPDATE `creature_template` SET `InhabitType`=7,`flags_extra`=`flags_extra`|128 WHERE `entry`=@ENTRY;
-- Razaani Light Orb - Mini Fixup and set as trigger
SET @ENTRY := 20771; -- NPC entry
UPDATE `creature` SET `modelid`=0,`spawndist`=0,`MovementType`=0 WHERE `id`=@ENTRY;
UPDATE `creature_template` SET `InhabitType`=7,`flags_extra`=`flags_extra`|128 WHERE `entry`=@ENTRY;
-- Blade's Edge - Deadsoul Orb Fixup and set as trigger
SET @ENTRY := 20845; -- NPC entry
UPDATE `creature` SET `modelid`=0,`spawndist`=0,`MovementType`=0 WHERE `id`=@ENTRY;
UPDATE `creature_template` SET `InhabitType`=7,`flags_extra`=`flags_extra`|128 WHERE `entry`=@ENTRY;
-- Blade's Edge - Deadsoul Orb Flight 01 Fixup and set as trigger
SET @ENTRY := 20851; -- NPC entry
UPDATE `creature` SET `modelid`=0,`spawndist`=0,`MovementType`=0 WHERE `id`=@ENTRY;
UPDATE `creature_template` SET `InhabitType`=7,`flags_extra`=`flags_extra`|128 WHERE `entry`=@ENTRY;
-- Blade's Edge - Deadsoul Orb Flight 02 Fixup and set as trigger
SET @ENTRY := 20852; -- NPC entry
UPDATE `creature` SET `modelid`=0,`spawndist`=0,`MovementType`=0 WHERE `id`=@ENTRY;
UPDATE `creature_template` SET `InhabitType`=7,`flags_extra`=`flags_extra`|128 WHERE `entry`=@ENTRY;
-- Blade's Edge - Deadsoul Orb Flight 03 Fixup and set as trigger
SET @ENTRY := 20853; -- NPC entry
UPDATE `creature` SET `modelid`=0,`spawndist`=0,`MovementType`=0 WHERE `id`=@ENTRY;
UPDATE `creature_template` SET `InhabitType`=7,`flags_extra`=`flags_extra`|128 WHERE `entry`=@ENTRY;
-- Blade's Edge - Deadsoul Orb Flight 04 Fixup and set as trigger
SET @ENTRY := 20855; -- NPC entry
UPDATE `creature` SET `modelid`=0,`spawndist`=0,`MovementType`=0 WHERE `id`=@ENTRY;
UPDATE `creature_template` SET `InhabitType`=7,`flags_extra`=`flags_extra`|128 WHERE `entry`=@ENTRY;
-- Blade's Edge - Deadsoul Orb Flight 05 Fixup and set as trigger
SET @ENTRY := 20856; -- NPC entry
UPDATE `creature` SET `modelid`=0,`spawndist`=0,`MovementType`=0 WHERE `id`=@ENTRY;
UPDATE `creature_template` SET `InhabitType`=7,`flags_extra`=`flags_extra`|128 WHERE `entry`=@ENTRY;
-- Blade's Edge - Nexus Prince Event - Orb01 Fixup and set as trigger
SET @ENTRY := 21025; -- NPC entry
UPDATE `creature` SET `modelid`=0,`spawndist`=0,`MovementType`=0 WHERE `id`=@ENTRY;
UPDATE `creature_template` SET `InhabitType`=7,`flags_extra`=`flags_extra`|128 WHERE `entry`=@ENTRY;

-- Fix InhabitType for Mana Snapper (Netherstorm)
UPDATE `creature_template` SET `InhabitType`=4 WHERE `entry`=18883;

-- Fix walk speed for several NPCs in Blades Edge
UPDATE `creature_template` SET `speed`=1 WHERE `entry` IN 
(19449, -- Thunderlord Grunt
17855); -- Expedition Warden

-- Fix Pink Elekks being visible when drunk
DELETE FROM `creature_template_addon` WHERE `entry` IN (21250,23507,23527,23528,23529,23530,23531,27958);
INSERT INTO `creature_template_addon` (`entry`,`auras`) VALUES
(21250,"36440 0"), -- Pink Elekk, Terokkar Forest
(23507,"36440 0"), -- Elwynn Pink Elekk
(23527,"36440 0"), -- Teldrassil Pink Elekk
(23528,"36440 0"), -- Azuremyst Pink Elekk
(23529,"36440 0"), -- Mulgore Pink Elekk
(23530,"36440 0"), -- Tirisfal Pink Elekk
(23531,"36440 0"); -- Eversong Pink Elekk

-- Fix missing areatrigger_taverns
DELETE FROM `areatrigger_tavern` WHERE `id` IN (98, 4109, 4300, 4378, 4380, 4498, 4608);
INSERT INTO `areatrigger_tavern` (`id`, `name`) VALUES
(98, 'Nesingwary''s Expedition'),
(4109, 'Tranquillen - Upper level Inn'),
(4300, 'Cenarion Refugee - Outside Inn'),
(4378, 'Stonebreaker Hold Inn'),
(4380, 'Falcon Watch Inn'),
(4498, 'Old Hillsbrad Foothills Inn'),
(4608, 'Sanctum Of The Stars - Upper level Inn');

-- Horde Orphan should not have weapons
UPDATE `creature_template` SET `equipment_id`=0 WHERE `entry`=14499;

-- Add Orders From Kael'thas to the following creatures
DELETE FROM `creature_loot_template` WHERE `item`=29797;
INSERT INTO `creature_loot_template` (`entry`,`item`,`ChanceOrQuestChance`,`groupid`,`mincountOrRef`,`maxcount`) VALUES
(18860,29797,-80,0,1,1), -- Daughter of Destiny
(20285,29797,-80,0,1,1), -- Gan'arg Warp-Tinker
(20326,29797,-80,0,1,1); -- Mo'arg Warp-Master

-- Add Heavy Scorpid Vest to Shadowsworn Thug and remove from Portal Seeker
DELETE FROM `creature_loot_template` WHERE `item`=15727;
INSERT INTO `creature_loot_template` (`entry`,`item`,`ChanceOrQuestChance`,`groupid`,`mincountOrRef`,`maxcount`) VALUES
(6005,15727,0.5,0,1,1); -- Shadowsworn Thug

-- [Q] Garments of the Moon (5621) and Garments of the Light (5625)
SET @ENTRY_SHAYA := 12429; -- Sentinel Shaya
SET @ENTRY_DOLF := 12427; -- Mountaineer Dolf
UPDATE `creature_template` SET `unit_flags`=`unit_flags`|8 WHERE `entry` IN (@ENTRY_SHAYA, @ENTRY_DOLF);

-- [Q] Garments of Darkness (5650)
SET @ENTRY_KEL := 12428; -- Deathguard Kel (has to be +8 so in total 36872)
UPDATE `creature_template` SET `unit_flags`=`unit_flags`|8|4096|32768 WHERE `entry` IN (@ENTRY_KEL);

-- Remove loot from Core Hounds
DELETE FROM `creature_loot_template` WHERE `entry`=11671; -- Core Hounds
UPDATE `creature_template` SET `lootid`=0 WHERE `entry`=11671;

-- Delete some spawned pets in Netherstorm, NPCs now summon their pets. Fel Imp and Mana Beast
DELETE FROM `creature` WHERE `id` IN (21135,21267);

-- an NPC with spawnmask 0. Never spawned.
UPDATE `creature` SET `spawnMask`=1 WHERE `guid` IN (1102) AND `id` IN (22237);

-- Silvermoon City Guardian pathing
SET @NPC := 56902;
SET @PATH := @NPC*10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2,`position_x`=9595.755,`position_y`=-7366.754,`position_z`=13.50131 WHERE `guid`=@NPC;
UPDATE `creature_addon` SET `path_id`=@PATH WHERE `guid`=@NPC;
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,9595.755,-7366.754,13.50131,0,0,0,100,0),
(@PATH,2,9594.553,-7336.536,14.15131,0,0,0,100,0),
(@PATH,3,9586.897,-7325.241,14.1312,0,0,0,100,0),
(@PATH,4,9561.705,-7326.179,14.05934,0,0,0,100,0),
(@PATH,5,9519.901,-7327.064,14.12175,0,0,0,100,0),
(@PATH,6,9508.228,-7352.014,14.33403,0,0,0,100,0),
(@PATH,7,9505.697,-7394.406,14.32095,0,0,0,100,0),
(@PATH,8,9535.751,-7396.176,16.8303,0,0,0,100,0),
(@PATH,9,9564.451,-7396.184,16.84703,0,0,0,100,0),
(@PATH,10,9563.777,-7415.961,19.47212,0,0,0,100,0),
(@PATH,11,9559.149,-7418.953,19.47643,0,0,0,100,0),
(@PATH,12,9563.34,-7432.895,15.4851,0,0,0,100,0),
(@PATH,13,9564.645,-7464.229,15.51088,0,0,0,100,0),
(@PATH,14,9588.409,-7464.643,13.52051,0,0,0,100,0),
(@PATH,15,9603.896,-7430.126,13.30448,0,0,0,100,0),
(@PATH,16,9595.137,-7387.862,13.52731,0,0,0,100,0),
(@PATH,17,9572.599,-7384.971,14.39343,0,0,0,100,0),
(@PATH,18,9573.106,-7369.531,14.40827,0,0,0,100,0);

-- Play sound when starting quest on IoQD
DELETE FROM `spell_scripts` WHERE `id`=45071;
INSERT INTO `spell_scripts` (`id`,`delay`,`command`,`datalong`,`datalong2`,`dataint`,`x`,`y`,`z`,`o`) VALUES
(45071,0,16,12318,1,0,0,0,0,0); -- Play sound

-- Fix NextQuestID
UPDATE `quest_template` SET `NextQuestId` = 3761 WHERE `entry` IN (936,3784,3762);

-- Floating Flavor Eye InhabitType fix
UPDATE `creature_template` SET `InhabitType`=4 WHERE `entry`=21659;

-- Blade's Edge - Legion - Invis Bunny
UPDATE `creature_template` SET `flags_extra`=`flags_extra`|128 WHERE `entry`=20736;
-- Death's Door Warp-Gate Explosion Bunny
UPDATE `creature_template` SET `flags_extra`=`flags_extra`|128 WHERE `entry`=22502;
-- Death's Door North Warp-Gate
UPDATE `creature_template` SET `flags_extra`=`flags_extra`|128 WHERE `entry`=22471;
-- Death's Door South Warp-Gate
UPDATE `creature_template` SET `flags_extra`=`flags_extra`|128 WHERE `entry`=22472;

-- Respawn Deadmire and set Waypoints so he doesn't fall through the world
SET @GUID := 33909;
SET @NPC := @GUID*10;
DELETE FROM `creature` WHERE `id`=4841;
INSERT INTO `creature` (`guid`,`id`,`map`,`spawnMask`,`modelid`,`equipment_id`,`position_x`,`position_y`,`position_z`,`orientation`,`spawntimesecs`,`spawndist`,`currentwaypoint`,`curhealth`,`curmana`,`MovementType`) VALUES
(@GUID,4841,1,1,0,0,-3949.688,-3471.041,29.15445,4.288023,360,0,0,1902,0,0); -- respawn Deadmire on old guid
-- Set movement info
UPDATE `creature_addon` SET `path_id`=@NPC WHERE `guid`=@GUID;
UPDATE `creature_template` SET `MovementType`=2 WHERE `entry`=4841;
-- Add Waypoint Data
DELETE FROM `waypoint_data` WHERE `id`=@NPC;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@NPC,1,-3942.966,-3455.51,28.89743,0,0,0,0,0,0),
(@NPC,2,-3943.117,-3456.498,28.89743,0,0,0,0,0,0),
(@NPC,3,-3948.063,-3467.443,29.64743,0,0,0,0,0,0),
(@NPC,4,-3950.123,-3472.004,29.02243,0,0,0,0,0,0),
(@NPC,5,-3953.303,-3479.054,28.64743,0,0,0,0,0,0),
(@NPC,6,-3953.303,-3479.054,28.64743,0,0,0,0,0,0);

-- Fixing Next Quest for Quest 8552
UPDATE `quest_template` SET `NextQuestInChain`=8553 WHERE `entry`=8552;

-- Update Dormant Infernal
SET @ENTRY := 21080;
UPDATE `creature_template_addon` SET `bytes2`=1,`auras`='36055 0' WHERE `entry`=@ENTRY;

-- Update Invis Deathforge Caster
SET @ENTRY := 21210;
DELETE FROM `creature_template_addon` WHERE `entry`=@ENTRY;
INSERT INTO `creature_template_addon` (`entry`,`bytes2`,`auras`) VALUES (@ENTRY,1,'36393 0');
UPDATE `creature_template` SET `flags_extra`=`flags_extra`|128 WHERE `entry`=@ENTRY;

-- Fix up Shadowmoon Harbinger
SET @ENTRY := 21795;
UPDATE `creature` SET `spawndist`=5,`MovementType`=1 WHERE `id`=@ENTRY;
UPDATE `creature_template` SET `AIName`='' WHERE `entry`=@ENTRY;
DELETE FROM `creature_ai_scripts` WHERE `creature_id`=@ENTRY;
DELETE FROM `creature_template_addon` WHERE `entry`=@ENTRY;
INSERT INTO `creature_template_addon` (`entry`,`bytes2`,`auras`) VALUES (@ENTRY,1,'37467 0 37509 0');

 -- Quest 3792 should only be available after quest 3791
UPDATE `quest_template` SET `PrevQuestId`=3791 WHERE `entry`=3792;

UPDATE `quest_template` SET `OfferRewardText`='Excellent, $N. You''ve saved me a lot of work!$b$bBut don''t think that I''m not grateful!' WHERE `entry`=6571;
UPDATE `quest_template` SET `OfferRewardText`="What!? If I knew that Pao'ka Swiftmountain was going to snoop around Highperch, I would have chained him to a totem! His father was reluctant allowing him to come with me to Thousand Needles. We came here to study the different creatures that inhabit these beautiful lands.$b$bI appreciate the help you have shown Pao'ka. I hope this covers any misfortunes this deed has cost you." WHERE `entry`=4770;

-- Grark Lorkrub should be available only when Kill On Sight: High Ranking Dark Iron Officials is complete
UPDATE `quest_template` SET `NextQuestId`=4122 WHERE `entry`=4082;
UPDATE `quest_template` SET `PrevQuestId`=4082 WHERE `entry`=4122;

-- Fix some quest requirements
UPDATE `quest_template` SET `PrevQuestId`=3906 WHERE `entry` IN (3907,7201); -- Disharmony of Fire and The Last Element are available only, if Disharmony of Flame is completed

-- Remove duplicate of Swirling Maelstrom (180669) GO author nelegalno
UPDATE `gameobject` SET `id` = 180669 WHERE `guid` = 99900;
DELETE FROM `gameobject_template` WHERE `entry`=300057;

-- The Scepter of the Shifting Sands quest chain fix by nelegalno based on http://www.wowwiki.com/The_Scepter_of_the_Shifting_Sands_quest_chain
-- What Tomorrow Brings > Only One May Rise
UPDATE `quest_template` SET `NextQuestInChain` = 8288 WHERE `entry` = 8286;
-- A Pawn on the Eternal Board > The Charge of the Dragonflights
UPDATE `quest_template` SET `NextQuestInChain` = 8555 WHERE `entry` = 8519;
-- The Nightmare's Corruption > The Nightmare Manifests
UPDATE `quest_template` SET `NextQuestInChain` = 8736 WHERE `entry` = 8735;
-- Azuregos's Magical Ledger > Translating the Ledger
UPDATE `quest_template` SET `NextQuestInChain` = 8576 WHERE `entry` = 8575;

-- Fix NPC Lhara, from Darkmoon Faire author Mick3y16
-- Fixes Black Lotus (30 minutes)
UPDATE `npc_vendor` SET `incrtime`=1800 WHERE `entry`=14846 AND `item`=13468;
-- Fixes The Rest (15 minutes)
UPDATE `npc_vendor` SET `incrtime`=900 WHERE `entry`=14846 AND `item` IN (36906,36905,36904,36903,36901,33568,25708,36907,36908,38425,37921,37705,37704,37703,37702,37701,37700,25707,21887,22572,22573,22574,22575,22576,22577,22578,4304,8170,2319,4234);

-- Set ExclusiveGroup for Shattrath Daily Quests, you can have only one per questgiver
UPDATE `quest_template` SET `ExclusiveGroup`=11364 WHERE `entry` IN (11376, 11383, 11385, 11371, 11389, 11500, 11364, 11387);
UPDATE `quest_template` SET `ExclusiveGroup`=11354 WHERE `entry` IN (11382, 11368, 11378, 11374, 11499, 11370, 11372, 11369, 11388, 11362, 11363, 11375, 11354, 11386, 11373, 11384);
