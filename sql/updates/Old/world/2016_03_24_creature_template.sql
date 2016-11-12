-- sql for commit afb75dfebdf5fc91ed634eb6bf86ac43064cbbf1

SET @ENTRY := 7850;
SET @SCRIPTNAME := 'npc_kernobee';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 5709;
SET @SCRIPTNAME := 'boss_eranikus';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE  `entry` =@ENTRY;

SET @ENTRY := 5710;
SET @SCRIPTNAME := 'boss_Jammal';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 12225;
SET @SCRIPTNAME := 'npc_celebras';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

-- missing ID go the go_incantbook

SET @ENTRY := 178965;
SET @SCRIPTNAME := 'go_incantbook';
UPDATE  `gameobject_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

-- missing text

DELETE FROM `script_texts` WHERE `entry` IN (-1910078, -1910079, -1910077); 
INSERT INTO `script_texts` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`, `sound`, `type`, `language`, `emote`, `comment`) VALUES
(-1910078, 'Thank you, friend. I''m in your debt.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Kernobee - Start Escort');
INSERT INTO `script_texts` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`, `sound`, `type`, `language`, `emote`, `comment`) VALUES
(-1910079, 'We made it! Thank you again, friend! Scooty will reward you handsomely.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Kernobee - End Escort');
INSERT INTO `script_texts` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`, `sound`, `type`, `language`, `emote`, `comment`) VALUES
(-1910077, 'The evil cannot be allowed to enter this world! Come my children!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, 'Shade of Eranikus - Aggro');

DELETE FROM `script_texts` WHERE `entry` IN (-1910084, -1910083, -1910082, -1910081, -1910080, -1910076, -1910075, -1910074, -1910073, -1910035, -1910034);
INSERT INTO `script_texts` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`, `sound`, `type`, `language`, `emote`, `comment`) VALUES
(-1910084, 'My scepter will once again become whole!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Celebras - Scepter of Celebras 5'),
(-1910083, 'Shal myrinan ishnu daldorah... ', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Celebras - Scepter of Celebras 4'),
(-1910082, 'Together, the two parts shall become one, once again.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Celebras - Scepter of Celebras 3'),
(-1910081, 'Read this tome I have placed before you, and speak the words aloud. ', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Celebras - Scepter of Celebras 2'),
(-1910080, 'Please do as I instruct you. ', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Celebras - Scepter of Celebras 1'),
(-1910076, 'Nhahahaha.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 5863, 1, 0, 0, 'Jammal - Kill'),
(-1910075, 'Join us!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 5864, 1, 0, 0, 'Jammal - Hex'),
(-1910074, 'Hakkar shall live again!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 5865, 1, 0, 0, 'Jammal - 10% hp'),
(-1910073, 'The Soulflayer comes!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 5862, 1, 0, 0, 'Jammal - Aggro'),
(-1910035, 'You wish to learn of the stone? Follow me.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Celebras START'),
(-1910034, 'For so long I have drifted in my cursed form. You have freed me... Your hard work shall be repaid.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, 'Celebras Appear');


-- sql update for commit 8f33830ce1ef6251a209337aa68a9782006049a6

SET @ENTRY := 2755;
SET @SCRIPTNAME := 'npc_myzrael';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 2887;
SET @SCRIPTNAME := 'npc_prismatic_exile';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

-- missing text entries

DELETE FROM `script_texts` WHERE `entry` IN (-1910022, -1910023, -1910024);
INSERT INTO `script_texts` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`, `sound`, `type`, `language`, `emote`, `comment`) VALUES
(-1910024, 'No matter. You were a fool to help me, and now you will pay!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'MYZRAEL_3'),
(-1910023, 'Why have you summoned me so soon? I haven''t yet reached my full power!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'MYZRAEL_2'),
(-1910022, 'What? You served me well but..', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'MYZRAEL_1');



-- sql update for commit d7f2651f01531d1947d7863a1e36a119e8623f6e

SET @ENTRY := 23415;
SET @SCRIPTNAME := 'npc_skyguard_handler_deesak';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 23161;
SET @SCRIPTNAME := 'npc_darkscreecher_akarai';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

DELETE FROM `script_texts` WHERE `entry` IN (-1910085,-1910086);
INSERT INTO `script_texts` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`, `sound`, `type`, `language`, `emote`, `comment`) VALUES
(-1910086, 'Consume them, my children!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, 'Darkscreecher Akkarai Summon'),
(-1910085, 'Hear the voices below the earth! They call for your blood!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Darkscreecher Akkarai Spawn');



-- sql update for commit c6280f618b6b055aaa343b3f8c669f22047e1f76

SET @ENTRY := 19191;
SET @SCRIPTNAME := 'npc_arrazius_the_cruel';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

DELETE FROM `script_texts` WHERE `entry` IN (-1910094,-1910093,-1910091,-1910092);
INSERT INTO `script_texts` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`, `sound`, `type`, `language`, `emote`, `comment`) VALUES
(-1910094, 'Master, I have failed... ', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, 'Arrazius Death'),
(-1910093, 'You wish to test me? You are hardly up to the task. ', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, 'Arrazius Slay'),
(-1910092, 'Your weapons are nothing against me! Repent! ', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, 'Arrazius Aggro 2'),
(-1910091, 'What insolence! What arrogance! To believe you could defeat me on my own doorstep!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, 'Arrazius Aggro 1');


-- sql updat for commit 54454f4f43b86431dbf12d820fb3b725c190becf

SET @ENTRY := 19399;
SET @SCRIPTNAME := 'npc_fel_cannon';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;


SET @ENTRY := 16846;
SET @SCRIPTNAME := 'npc_maghar_grunt';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

DELETE FROM `script_texts` WHERE `entry` IN (-1910100);
INSERT INTO `script_texts` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`, `sound`, `type`, `language`, `emote`, `comment`) VALUES
(-1910100, 'Thank you, stranger. I feel better now.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Mag''har Grunt Injured');


-- sql update for commit 93deca9b463443e443133ee0b96b4c7e3154eda6

SET @ENTRY := 16938;
SET @SCRIPTNAME := 'npc_dreghood_brute';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

-- missing text
DELETE FROM `script_texts` WHERE `entry` IN (-1910101,-1910102,-1910103);
INSERT INTO `script_texts` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`, `sound`, `type`, `language`, `emote`, `comment`) VALUES
(-1910103, 'I will do as you ask, demon... at least for now.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Say Dreghood Brute 3'),
(-1910102, 'Flee! Flee!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Say Dreghood Brute Run'),
(-1910101, 'Forgive me... I have no choice.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Say Dreghood Brute');


-- sql update for commit cc4721d1fdc84002bb49dfa5417f2b48ea8e99ee

SET @ENTRY := 20677;
SET @SCRIPTNAME := 'npc_dreghood_elder1';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 20678;
SET @SCRIPTNAME := 'npc_dreghood_elder2';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 20679;
SET @SCRIPTNAME := 'npc_dreghood_elder3';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;


-- missing text
DELETE FROM `script_texts` WHERE `entry` IN (-1910104,-1910105,-1910106);
INSERT INTO `script_texts` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`, `sound`, `type`, `language`, `emote`, `comment`) VALUES
(-1910106, 'I''m free! The spirit of water returns to my people. It will bring us the wisdom we need to survive in this harsh land. I am in your debt, $n.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Dreghood Elder Gossip Text 3'),
(-1910105, 'You''ve freed me! The winds speak to my people once again and grant us their strenght. I thank you, stranger.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Dreghood Elder Gossip Text 2'),
(-1910104, 'I am free! I will ask the spirit of fire to return to us so  that my people may have the courage to fight their masters! I thank you, $r.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Dreghood Elder Gossip Text 1');


-- sql update for commit cdd64201781558f6a4f58b768fef07fd804e2de5

SET @ENTRY := 17226;
SET @SCRIPTNAME := 'npc_viera';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

-- missing text
DELETE FROM `script_texts` WHERE `entry` IN (-1910095,-1910096,-1910097,-1910098);
INSERT INTO `script_texts` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`, `sound`, `type`, `language`, `emote`, `comment`) VALUES
(-1910098, 'Meow...!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Cat - Arelion''s mistress - Say 1'),
(-1910097, 'What... is hapenning... to me? Get this cat away from me!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Viera - Arelion''s mistress - Say 3'),
(-1910096, 'I can''t wait to try this wine!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Viera - Arelion''s mistress - Say 2'),
(-1910095, 'Ok, let''s go. I know just the perfect spot!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Viera - Arelion''s mistress - Say 1');


-- sql update for commit 9681dcc4f0bad3f13ea67b993145e4c3f4de5b75

SET @ENTRY := 24918;
SET @SCRIPTNAME := 'npc_felblood_initiate';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 19761;
SET @SCRIPTNAME := 'npc_felspark';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 18828;
SET @SCRIPTNAME := 'npc_camerashaker';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

DELETE  FROM `script_texts` WHERE `entry` IN (-1910107,-1910108,-1910109,-1910110,-1910111);
INSERT INTO `script_texts` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`, `sound`, `type`, `language`, `emote`, `comment`) VALUES
(-1910111, 'I will soon be stronger than any elf! I will serve at Kil''jaeden''s side!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Felblood Initiate Say 5'),
(-1910110, 'Unparalled power... I... crave... more!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Felblood Initiate Say 4'),
(-1910109, 'More... more... MORE!!!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Felblood Initiate Say 3'),
(-1910108, 'Fel energy... courses through my veins!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Felblood Initiate Say 2'),
(-1910107, 'Your life force is my nourishment, demon... Kil''jaeden''s gift to us!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Felblood Initiate Say 1');


-- sql update for commit 4aeb6c4eda00b2bd2187064437e1dada14e824bd

SET @ENTRY := 18035;
SET @SCRIPTNAME := 'npc_scoutjyoba';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 18123;
SET @SCRIPTNAME := 'npc_wrektslave';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

-- missing text
DELETE FROM `script_texts` WHERE `entry` IN (-1910102);
INSERT INTO `script_texts` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`, `sound`, `type`, `language`, `emote`, `comment`) VALUES
(-1910102, 'Flee! Flee!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Say Dreghood Brute Run');


-- sql update for commit c5236719702784eb79d3aabedc944ceddbf23701

SET @ENTRY := 18122;
SET @SCRIPTNAME := 'npc_dreghooddrudge';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

-- missing text
DELETE FROM `script_texts` WHERE `entry` IN (-1910148);
INSERT INTO `script_texts` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`, `sound`, `type`, `language`, `emote`, `comment`) VALUES
(-1910148, 'F''woooroonn! Coosh''coosh pay nothing! Die!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Coosh''coosh - Gossip Select');



-- sql update for commit 25bb84348e2c140b9e55a2fc403cdec7d2f397fb
-- missing text
DELETE FROM `script_texts` WHERE `entry` IN (-1910145,-1910139,-1910124,-1910125, -1910126, -1910127, -1910128, -1910129, -1910130, -1910131, -1910132, -1910133, -1910134, -1910135, -1910136, -1910137, -1910138, -1910144, -1910143,-1910141, -1910142,-1900192, -1900193, -1900194, -1900195, -1900196, -1900197, -1900198, -1900199, -1900200. -1900201);
INSERT INTO `script_texts` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`, `sound`, `type`, `language`, `emote`, `comment`) VALUES
(-1910145, 'Up ahead is the road to Telaar. We will split up when we reach the fork as they will surely send more Murkblood after us. Hopefully one of us makes it back to Telaar alive.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Kurenai Captive - Middle'),
(-1910144, 'No more nice ogre! You hurt Mogor!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, 'The Ring of Blood - Mogor Enrage'),
(-1910143, 'No more chances! Now you pay da ogre!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, 'The Ring of Blood - Mogor Aggro'),
(-1910142, 'Piece by piece I will tear this challenger apart... PIECE BY PIECE!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'The Ring of Blood - Warmaul Champion Aggro'),
(-1910141, 'Closer... Come closer... See what the void brings!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, 'The Ring of Blood - Skra''gath Aggro'),
(-1910139, 'Mogor has challenged you. You have to accept! Get in the ring if you are ready to fight.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'The Ring of Blood - Mogor Announcer Intro'),
(-1910138, 'Now you face da true champion! I give you chance to run away little one. Run away now before Mogor decim... destyor... Run away before Mogor KILL!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, 'The Ring of Blood - Mogor Intro'),
(-1910137, 'Dat was poop! Mogor could put up much better fight den dat!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, 'The Ring of Blood - Rokdar Defeat - Mogor'),
(-1910136, 'Mogor not impressed! Skra''gath wuz made of da air and shadow! Soft like da squishy orcies!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, 'The Ring of Blood - Skra''gath Defeat - Mogor'),
(-1910135, 'WUT!? UMPOSSIBLE!! You fight Mogor now! Mogor destroy!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, 'The Ring of Blood - Warmaul Champion Defeat - Mogor'),
(-1910134, '$n has defeated the hero of the Warmaul, Mogor! All hail $n!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, 'The Ring of Blood - Zone Announce'),
(-1910133, 'For the first time in the Ring of Blood''s history, Mogor has chosen to exercise his right of battle! On this wartorn ground, $n will face Mogor, hero of the Warmaul!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, 'The Ring of Blood - Mogor Start'),
(-1910132, 'This is the moment we''ve all been waiting for! The Warmaul champion is about to make his first showing at the Ring of Blood in weeks! Will $n go down in defeat as easily as the champion''s other opponents? We shall see...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, 'The Ring of Blood - Warmaul Champion Start'),
(-1910131, 'From parts unknown: Skra''gath! Can $n possibly survive the onslaught of void energies?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, 'The Ring of Blood - Skra''gath Start'),
(-1910130, '$n is victorious once more!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, 'The Ring of Blood - Rokdar Defeat'),
(-1910129, 'Hailing from the mountains of Blade''s Edge comes Rokdar the Sundered Lord! $n is in for the fight of his life.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, 'The Ring of Blood - Rokdar Start'),
(-1910128, 'Unbelievable! $n has defeated the Murkblood twins!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, 'The Ring of Blood - Murkblood twins Defeat'),
(-1910127, 'The battle is about to begin! The unmerciful Murkblood twins versus $n!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, 'The Ring of Blood - Murkblood twins Start'),
(-1910126, '$n has defeated Brokentoe!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, 'The Ring of Blood - Brokentoe Defeat'),
(-1910125, 'The battle is about to begin! $n versus the ferocious clefthoof, Brokentoe!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, 'The Ring of Blood - Brokentoe Start'),
(-1910124, 'Get in the Ring of Blood, $n. The fight is about to start!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'The Ring of Blood - Arena Enter'),
(-1900199, 'For shure it be good.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Boulderfist Saboteur'),
(-1900198, 'My dead orc sound is da best!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Boulderfist Saboteur'),
(-1900197, 'Last mont, me won da dead orc sound champi... champichi... me won da dead orc sound contest.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Boulderfist Saboteur'),
(-1900196, 'Your dead orc sound ok. Mine better.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Boulderfist Saboteur'),
(-1900195, 'You so smart. Let''s do dis now before more come.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Boulderfist Saboteur'),
(-1900194, 'Me wanted to make it belive... Me wanted to make it real. Dey tink for shure it Shadow Council now!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Boulderfist Saboteur'),
(-1900193, 'Shut up, stupid! Why you yell?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Boulderfist Saboteur'),
(-1900192, 'Rawr! Me is Shadow Council magishun! Me come for Warmaul terri... tettritor... Me come for Warmaul land!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, 'Boulderfist Saboteur');


-- add script to the `creature_template`

SET @ENTRY := 18471;
SET @SCRIPTNAME := 'npc_gunthock';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 18398;
SET @SCRIPTNAME := 'npc_brokentoe';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 18399;
SET @SCRIPTNAME := 'npc_murkbloodtwins';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 18400;
SET @SCRIPTNAME := 'npc_rockdar';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 18401;
SET @SCRIPTNAME := 'npc_szagath';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 18402;
SET @SCRIPTNAME := 'npc_warmaul_champion';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;


SET @ENTRY := 18069;
SET @SCRIPTNAME := 'npc_mogor';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

-- SET @ENTRY := **; -- missing id, unable to find
-- SET @SCRIPTNAME := 'trigger_arenahelper';
-- DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
-- UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;
-- 
-- SET @ENTRY := **; -- missing id, unable to find
-- SET @SCRIPTNAME := 'trigger_arenacleaner';
-- DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
-- UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;
-- 
-- SET @ENTRY := **; -- missing id, unable to find
-- SET @SCRIPTNAME := 'npc_warmaul_pyre';
-- DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
-- UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;
-- 
-- SET @ENTRY := **; -- missing id, unable to find
-- SET @SCRIPTNAME := 'npc_fel_cannon_haf';
-- DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
-- UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 17160;
SET @SCRIPTNAME := 'npc_living_cyclone';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 18062;
SET @SCRIPTNAME := 'npc_enraged_crusher';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 17159;
SET @SCRIPTNAME := 'npc_storm_rager';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 17155;
SET @SCRIPTNAME := 'npc_lake_surger';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;


SET @ENTRY := 17153;
SET @SCRIPTNAME := 'npc_lake_spirit';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 17156;
SET @SCRIPTNAME := 'npc_tortured_earth_spirit';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 22309;
SET @SCRIPTNAME := 'npc_crashing_wave_spirit';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 22311;
SET @SCRIPTNAME := 'npc_raging_fire_soul';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 22310;
SET @SCRIPTNAME := 'npc_storming_wind_ripper';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 22313;
SET @SCRIPTNAME := 'npc_rumbling_earth_heart';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

-- Missing gossip menu entries.


-- sql update for commit 94214a4187f8fd0fa45dbaf73588a347f5fe12c0
-- missing text

DELETE FROM `script_texts` WHERE `entry` IN (-1910147, -1910146);
INSERT INTO `script_texts` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`, `sound`, `type`, `language`, `emote`, `comment`) VALUES
(-1910147, 'Raliq teach you lesson now!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Raliq the Drunk - Gossip Select'),
(-1910146, '[Demonic] Ka kalix!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Sal''salabim - Gossip Select');


-- sql update for commit 369668c853ee901b96badde7a7be3cda9802a8db

SET @ENTRY := 18715;
SET @SCRIPTNAME := 'npc_private_weeks';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 16519;
SET @SCRIPTNAME := 'npc_shadowy_executioner';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 18717;
SET @SCRIPTNAME := 'npc_shadowy_laborer';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 17088;
SET @SCRIPTNAME := 'npc_shadowy_summoner';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 18719;
SET @SCRIPTNAME := 'npc_shadowy_advisor';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 18716;
SET @SCRIPTNAME := 'npc_shadowy_initiate';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;



-- missing text
DELETE FROM `script_texts` WHERE `entry` IN (-1910149,-1910150,-1910151);
INSERT INTO `script_texts` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`, `sound`, `type`, `language`, `emote`, `comment`) VALUES
(-1910151, 'Ok. I think I can make it on my own from here. Thank you so much for breaking me out of there!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Isla Starmane - Say 3'),
(-1910150, 'Alright, let''s do this!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Isla Starmane - Say 2'),
(-1910149, 'You sure you''re ready? Take a moment.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Isla Starmane - Say 1');



-- sql update for commit 684660d49ffa0c33872981b922e86f4dd3dc9ca8

SET @ENTRY := 17377;
SET @SCRIPTNAME := 'npc_kelridan_trigger';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 18607;
SET @SCRIPTNAME := 'npc_kelridan_trigger';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 16704;
SET @SCRIPTNAME := 'npc_shattered_shooter';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 20594;
SET @SCRIPTNAME := 'npc_shattered_shooter';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 17462;
SET @SCRIPTNAME := 'npc_shattered_zealot';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 20595;
SET @SCRIPTNAME := 'npc_shattered_zealot';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 17461;
SET @SCRIPTNAME := 'npc_blood_guard';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 20581;
SET @SCRIPTNAME := 'npc_blood_guard';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

-- SET @ENTRY := ---; -- missing correct ID
-- SET @SCRIPTNAME := 'npc_shattered_target';
-- DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
-- UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;
-- 
-- SET @ENTRY := ---; -- missing correct ID
-- SET @SCRIPTNAME := 'npc_shattered_helper';
-- DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
-- UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;
-- 
-- SET @ENTRY := ---; -- missing correct ID
-- SET @SCRIPTNAME := 'npc_honorfallendefender';
-- DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
-- UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;
-- 
-- SET @ENTRY := ---; -- missing correct ID
-- SET @SCRIPTNAME := 'npc_honorfalleninfantry';
-- DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
-- UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;
-- 
-- SET @ENTRY := 16700;
-- SET @SCRIPTNAME := 'npc_shattered_legionnaire';
-- DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
-- UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;
-- 
-- SET @ENTRY := 20589;
-- SET @SCRIPTNAME := 'npc_shattered_legionnaire';
-- DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
-- UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;
-- 
-- SET @ENTRY := 17693;
-- SET @SCRIPTNAME := 'npc_shatteredscout';
-- DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
-- UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;
-- 
-- SET @ENTRY := 20592;
-- SET @SCRIPTNAME := 'npc_shatteredscout';
-- DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
-- UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

-- Missing TEXT

DELETE FROM `script_texts` WHERE `entry` IN (-1910114, -1910115, -1910116, -1910117, -1910122, -1910123, -1910118, -1910119, -1910120, -1910121, -1910122, -1910113);
INSERT INTO `script_texts` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`, `sound`, `type`, `language`, `emote`, `comment`) VALUES
(-1910123, 'It looks like we have a ranking officer among our captives... how amusing. Execute the green-skinned dog at once!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, 'Kargath Bladefist - Execute'),
(-1910122, 'Cowards! You''ll never pull me into the shadows!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, 'Kargath Bladefist - Evade'),
(-1910121, 'Shattered Hand Legionnaire becomes enraged seeing his friend fall in battle!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 2, 0, 0, 'Shattered Hand Legionnaire - Emote Enrage'),
(-1910120, 'Fighter down!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 10222, 1, 0, 0, 'Legionnaire Friend Killed 1'),
(-1910119, 'Next warrior step up!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 10230, 1, 0, 0, 'Legionnaire Friend Killed 3'),
(-1910118, 'Next warrior now!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 10224, 1, 0, 0, 'Legionnaire Friend Killed 2'),
(-1910117, 'Fire!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 10159, 1, 0, 0, 'Porung, Guard - Formation Archer 3'),
(-1910116, 'Ready!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 10157, 1, 0, 0, 'Porung, Guard - Formation Archer 2'),
(-1910115, 'Aim!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 10158, 1, 0, 0, 'Porung, Guard - Formation Archer 1'),
(-1910114, 'Archers, form ranks! On my mark!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 10156, 1, 0, 0, 'Porung, Guard - Formation Archer'),
(-1910113, 'Invaders have breached the defenses!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 10285, 1, 0, 0, 'Shattered Hand Scout - Spotted');


-- sql update for commit 134943abf17db8ed98674174e74cf35073bbe42b
-- Missing text

DELETE FROM `script_texts` WHERE `entry` IN (-1910152, -1910154, -1910157, -1910158, -1910159, -1910161, -1910163,-1910153, -1910155, -1910156, -1910160, -1910162, -1910163);
INSERT INTO `script_texts` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`, `sound`, `type`, `language`, `emote`, `comment`) VALUES
(-1910163, 'I believe I may recognize them. Deal with this quickly, Sharth. Then take the mana bomb and destroy their town! ', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Pathaleon Image Say 6'),
(-1910162, 'You come into my house and threaten ME? I think not!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, 'Sharth Voldoun Aggro'),
(-1910161, 'See to it that you do, Sharth, or I will personally see to your slow torture and death. ', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Pathaleon Image Say 5'),
(-1910160, 'I can assure you that we will not fail, my master. I am personally overseeing every aspect of the construction, and I hold the final codes, myself. Within a day''s time, I will have the bomb detonated on those nearby pests.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Sharth Voldoun Say 4'),
(-1910159, 'I think that we should teach a lesson to both the Horde and the Alliance. One that they will not soon forget!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Pathaleon Image Say 4'),
(-1910158, 'I need not tell you what will happen should the mana bomb down in the courtyard fail to be used on its target soon? Since moving into the forest, they''ve become increasingly annoying to our operations: here, at Tuurem and to the south at the Bonechewer Ruins.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Pathaleon Image Say 3'),
(-1910157, 'For the time being, yes. However, allow my presence to be a motivator. Prince Kael''thas was displeased with the failure of the crystal experiment on Fallen Sky Ridge. This is one of the reasons for why we chose the Cenarion druids as the testing grounds for the bomb.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Pathaleon Image Say 2'),
(-1910156, 'You are satisfied?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Sharth Voldoun Say 3'),
(-1910155, 'All goes exceedingly well, my lord. Testing of the smaller prototype at the Cenarion Thicket was a complete success. The second bomb is being ritually fueled in the courtyard below even as we speak. And, I''ve sent a courier to Tuurem to bring the rest of the parts to us here.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Sharth Voldoun Say 2'),
(-1910154, 'Rise, young Voldoun. Do not worry yourself with pleasantries. I come at the behest of our master, Prince Kael''thas. He bids me to check on the progress of the mana bomb.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Pathaleon Image Say 1'),
(-1910153, 'My master, we are honored by your presence. How may I serve thee?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Sharth Voldoun Say 1'),
(-1910152, 'What is the meaning of this?!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Empoor - Aggro');



SET @ENTRY := 18482;
SET @SCRIPTNAME := 'npc_empoor';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

-- SET @ENTRY := **;
-- SET @SCRIPTNAME := 'npc_pathaleon_image2';
-- DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
-- UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 18554;
SET @SCRIPTNAME := 'npc_voldoun';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 21844;
SET @SCRIPTNAME := 'mob_mountain_colossus';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

-- sql update for commit f21d1be0c7720d99089e0cb7d438447346db49ea

UPDATE `gameobject_template` SET `ScriptName` = "go_thunderspike" WHERE `entry` = 184729;


-- missing text
DELETE FROM `script_texts` WHERE `entry` IN (-1910171, -1910172, -1910173, -1910174, -1910175, -1910176, -1910186, -1910177, -1910178, -1910179, -1910180, -1910181, -1910183, -1910184, -1910185, -1910182);
INSERT INTO `script_texts` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`, `sound`, `type`, `language`, `emote`, `comment`) VALUES
(-1910186, 'I could not have achieved this victory without you. We will speak back at Thunderlord Stronghold.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, 'Rexxar - Goc Died'),
(-1910185, 'Prepare yourself for the end!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, 'Rexxar - Line 3'),
(-1910184, 'And their torment at your hands is at an end. By name, I shall put an end to your life.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, 'Rexxar - Line 2'),
(-1910183, 'I am Rexxar, son of the Mok''Nathal, champion of the Horde.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, 'Rexxar - Line 1'),
(-1910182, 'Hah! The Thunderspike is mine. Die!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Gor Grimgut - Say Aggro'),
(-1910181, 'Baron Sablemane begins emanating immense power.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 2, 0, 0, 'Sabelian - Transform Emote'),
(-1910180, 'Your father destroyed my children and left their bodies impaled upon the rocky blades at Dragons'' End. For this, you will die! Enough of this farce. Prepare to face my full wrath!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, 'Sabelian - Transform'),
(-1910179, 'Let us begin.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, 'Sabelian - Line 3'),
(-1910178, 'Baron Sablemane. It will be on your lips as you gasp your final breath.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, 'Sabelian - Line 2'),
(-1910177, 'It''s only right that you know the name of the one who will take your life.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, 'Sabelian - Line 1'),
(-1910176, 'We will speak again, but away from here.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, 'Sabelian - Goc Die'),
(-1910175, 'Your name is as insignificant to me as the names of the thousands who have died under the might of Goc. I will crush You!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, 'Yell Goc - Line 2'),
(-1910174, 'Who dares rouse Goc from his restful slumber?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, 'Yell Goc - Entry'),
(-1910173, 'Release the hounds!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, 'Baelmon the Hound Master - Summon'),
(-1910172, 'You DARE to attack me?!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, 'Baelmon the Hound Master - Aggro'),
(-1910171, 'The Raven''s Wood Leafbeard appears to be very grateful to be free of the koi-koi spirit''s influence.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 2, 0, 0, 'Raven''s Wood Leafbeard - Emote Thank You');

-- Sql update for commit 13a4fa43a1c1c10472ab1bb806513333a1f7edec

DELETE FROM `script_texts` WHERE `entry` IN (-1910187, -1910188, -1910189,-1910191, -1910192);
INSERT INTO `script_texts` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`, `sound`, `type`, `language`, `emote`, `comment`) VALUES
(-1910192, 'Death incarnate has been released upon the world once more! Teron Gorefiend has returned...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, 'Teron Gorefiend Shadowmoon Valley Yell to Zone'),
(-1910191, 'You only hasten the inevitable. In time, all will be devoured!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, 'Dimensius the All-Devouring - Summon Spawns 1'),
(-1910189, 'We''re here for you, lost brother. It is custom to offer you a chance to repent before you are destroyed. We offer you this chance, as the naaru''s law commands.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Adyen - Text 1'),
(-1910188, 'Bessy, you''re home. Thank the Light!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Thedall - Complete Escort'),
(-1910187, 'Moooooooooo!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Bessy - Mooo');


-- SET @ENTRY := **; -- missing GO ID
-- SET @SCRIPTNAME := 'go_manaforge_control_console';
-- DELETE FROM `gameobject_scripts` WHERE creature_id=@ENTRY;
-- UPDATE  `gameobject_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;
-- 
-- SET @ENTRY := **; -- missing NPC ID
-- SET @SCRIPTNAME := 'npc_manaforge_control_console';
-- DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
-- UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 19831;
SET @SCRIPTNAME := 'npc_commander_dawnforge';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

-- SET @ENTRY := **; -- missing Area trigger ID
-- SET @SCRIPTNAME := 'at_commander_dawnforge';
-- DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
-- UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;
-- 
-- SET @ENTRY := 20907; -- there are 2 of them, this is the friendly one.
-- SET @SCRIPTNAME := 'npc_professor_dabiri';
-- DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
-- UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 18879;
SET @SCRIPTNAME := 'npc_phase_hunter';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 20415;
SET @SCRIPTNAME := 'npc_bessy';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

-- SET @ENTRY := **; -- missing ID
-- SET @SCRIPTNAME := 'npc_maxx_a_million_escort';
-- DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
-- UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 22484;
SET @SCRIPTNAME := 'npc_zeppit';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 20284;
SET @SCRIPTNAME := 'npc_dr_boom';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 19692;
SET @SCRIPTNAME := 'npc_boom_bot';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 20281;
SET @SCRIPTNAME := 'npc_drijya';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 18864;
SET @SCRIPTNAME := 'npc_mana_wraith';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 18865;
SET @SCRIPTNAME := 'npc_warp_abberation';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 20243;
SET @SCRIPTNAME := 'npc_scrapped_fel_reaver';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 18867;
SET @SCRIPTNAME := 'npc_mana_seeker';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 18866;
SET @SCRIPTNAME := 'npc_mageslayer';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 20483;
SET @SCRIPTNAME := 'npc_naberius';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

-- SET @ENTRY := **; -- missing creature ID
-- SET @SCRIPTNAME := 'npc_towercurse_trigger';
-- DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
-- UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;
-- 
-- SET @ENTRY := **; -- missing creature ID
-- SET @SCRIPTNAME := 'npc_towerchannel_trigger';
-- DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
-- UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;
-- 
-- SET @ENTRY := **; -- missing creature ID
-- SET @SCRIPTNAME := 'npc_towerchanneler_trigger';
-- DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
-- UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 19832;
SET @SCRIPTNAME := 'npc_doctor_vomisa';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 19851;
SET @SCRIPTNAME := 'npc_negatron';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 20774;
SET @SCRIPTNAME := 'npc_farahlon_lasher';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 20610;
SET @SCRIPTNAME := 'npc_talbuk_doe';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 20777;
SET @SCRIPTNAME := 'npc_talbuk_sire';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 20516;
SET @SCRIPTNAME := 'npc_warp_monstrosity';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 22293;
SET @SCRIPTNAME := 'npc_inactive_fel_reaver';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 22293;
SET @SCRIPTNAME := 'npc_inactive_fel_reaver';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

-- SET @ENTRY := **; -- missing ID, unsure if its object or npc
-- SET @SCRIPTNAME := 'gehenna_trigger';
-- DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
-- UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;
-- 
-- SET @ENTRY := **; -- missing id unsure if its object or npc
-- SET @SCRIPTNAME := 'legion_aldor_trigger';
-- DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
-- UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 18537;
SET @SCRIPTNAME := 'adyen_lightwarden';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 20132;
SET @SCRIPTNAME := 'npc_socrethar';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 20780;
SET @SCRIPTNAME := 'npc_kaylaan';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

-- SET @ENTRY := **; -- missing ID of npc
-- SET @SCRIPTNAME := 'npc_socre_helper';
-- DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
-- UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;
-- 
-- SET @ENTRY := **; -- missing ID of npc
-- SET @SCRIPTNAME := 'legion_beam_trigger';
-- DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
-- UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 20858;
SET @SCRIPTNAME := 'arena_event_controller';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 20071;
SET @SCRIPTNAME := 'npc_windtrader_marid';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 20763;
SET @SCRIPTNAME := 'npc_captured_protectorate';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 20553;
SET @SCRIPTNAME := 'npc_image_of_commander_ameer';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 20802;
SET @SCRIPTNAME := 'npc_protectorate_demolitionist';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

-- SET @ENTRY := **; -- missing ID of creature or GO
-- SET @SCRIPTNAME := 'demolitionist_trigger';
-- DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
-- UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 20769;
SET @SCRIPTNAME := 'npc_energy_ball';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 20454;
SET @SCRIPTNAME := 'npc_salhadaar';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 20985;
SET @SCRIPTNAME := 'npc_captain_saeed';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 19554;
SET @SCRIPTNAME := 'npc_dimensius_devourer';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;


-- Sql update for commit 2e4e2d3ff964e0e608281b4973c8df9a7b96cc23

-- missing text

DELETE FROM `script_texts` WHERE `entry` IN (-1900140, -1900141, -1900142, -1900143, -1900144, -1900145, -1900146, -1900147, -1910192, -1910193, -1910194, -1910195, -1910196, -1910197, -1910198, -1910199. -1910200, -1910202, -1910198, -1910201);
INSERT INTO `script_texts` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`, `sound`, `type`, `language`, `emote`, `comment`) VALUES
(-1910202, 'You do not fight alone, $N! Together, we will banish this spawn of the hellfire!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, 'Oronok - Yell 1'),
(-1910201, 'Come, mortals, face the lord of fire!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, 'Cyrukh - Yell 1'),
(-1910198, 'Stop! You must stop!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Spirit of the Past - Stop'),
(-1910197, 'Watch! See the ground shatter before us! Watch as the energy flows! It will feed our armies...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Guldan Text 4'),
(-1910196, 'With his gift, I shall raze this land and reform it!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Guldan Text 3'),
(-1910195, 'Bear witness to the undeniable power of our dark master!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Guldan Text 2'),
(-1910194, 'Be silent! The shattering is soon to come!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Guldan Text 1 '),
(-1910193, 'I... I have died... I was in so much pain... engulfed in darkness... Can you see me $r?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Spirit of Artor - Say'),
(-1910192, 'Death incarnate has been released upon the world once more! Teron Gorefiend has returned...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, 'Teron Gorefiend Shadowmoon Valley Yell to Zone'),
(-1900147, 'Crate? I didn''t send you a crate, Jovaan. Don''t you have more important things to worry about? Go see to them!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'SAY_WARBRINGER4'),
(-1900146, 'What''s in the crate?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'SAY_JOVAAN4'),
(-1900145, 'Yes?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'SAY_WARBRINGER3'),
(-1900144, 'It shall be as you say, warbringer. One last question, if I may...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'SAY_JOVAAN3'),
(-1900143, 'Don''t worry about that. I''ve increased production at the Deathforge. You''ll have all the infernals you need to carry out your orders. Don''t fail, Jovaan.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'SAY_WARBRINGER2'),
(-1900142, 'Warbringer, that will require the use of all the hold''s infernals. It may leave us vulnerable to a counterattack.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'SAY_JOVAAN2'),
(-1900141, 'Doom Lord Kazzak will be pleased. You are to increase the pace of your attacks. Destroy the orcish and dwarven strongholds with all haste.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'SAY_WARBRINGER1'),
(-1900140, 'Everything is in readiness, warbringer.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'SAY_JOVAAN1');



SET @ENTRY := 21183;
SET @SCRIPTNAME := ''';pc_oronok_tornheart';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

-- SET @ENTRY := **; -- missing ID
-- SET @SCRIPTNAME := 'legion_infernal_summon_trigger';
-- DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
-- UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;
-- 
-- SET @ENTRY := **; -- missing ID
-- SET @SCRIPTNAME := 'legion_infernal_summoner_trigger';
-- DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
-- UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;
-- 
-- SET @ENTRY := **; -- missing ID
-- SET @SCRIPTNAME := 'legion_hold_device_trigger';
-- DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
-- UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;


SET @ENTRY := 21633;
SET @SCRIPTNAME := 'npc_deathbringer_jovaan';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 21302;
SET @SCRIPTNAME := 'npc_shadowcouncil_warlock';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 21790;
SET @SCRIPTNAME := 'npc_plexi';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 21789;
SET @SCRIPTNAME := 'npc_nakansi';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;


SET @ENTRY := 11980;
SET @SCRIPTNAME := 'npc_zuluhed_the_whacked';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 22251;
SET @SCRIPTNAME := ''';pc_dragonmaw_archer';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 21797;
SET @SCRIPTNAME := 'npc_shadowmoon_ancient_spirit';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 21877;
SET @SCRIPTNAME := 'npc_karsius_the_ancient_watcher';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

-- SET @ENTRY := **; -- missing ID
-- SET @SCRIPTNAME := 'teron_trigger';
-- DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
-- UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;
-- 
-- SET @ENTRY := **; -- missing ID
-- SET @SCRIPTNAME := 'teron_gorefiend_event';
-- DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
-- UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 185032;
SET @SCRIPTNAME := 'go_forged_illidari_bane_sword';
UPDATE  `gameobject_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 14535;
SET @SCRIPTNAME := 'npc_artor';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

-- SET @ENTRY := **; -- missing ID
-- SET @SCRIPTNAME := 'npc_artor_spirit';
-- DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
-- UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;
-- 
-- SET @ENTRY := **; -- missing ID
-- SET @SCRIPTNAME := 'npc_tiny_trigger';
-- DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
-- UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 21309;
SET @SCRIPTNAME := 'npc_painmistress_gabrisa';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 19802;
SET @SCRIPTNAME := 'npc_illidari_shocktrooper';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 20427;
SET @SCRIPTNAME := 'npc_veneratus';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 21409;
SET @SCRIPTNAME := 'npc_envoy_icarius';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

-- SET @ENTRY := **; -- missing ID
-- SET @SCRIPTNAME := 'npc_icarus_trigger';
-- DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
-- UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 17008;
SET @SCRIPTNAME := 'npc_guldan';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 21183;
SET @SCRIPTNAME := 'npc_oronok';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 21181;
SET @SCRIPTNAME := 'npc_cyrukh_the_firelord';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;


-- sql update for commit 2ec46a74a44106128dc780fa328ad70306dd33ed

DELETE FROM `script_texts` WHERE `entry` IN (-1000274, -1000275, -1000276, -1000277, -1000278, -1000279);
INSERT INTO `script_texts` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`, `sound`, `type`, `language`, `emote`, `comment`) VALUES
(-1000279, 'Alright, we give up! Don''t hurt us!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'larry SAY_GIVEUP'),
(-1000278, 'Time to meet your maker!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'larry SAY_ATTACK_5'),
(-1000277, '2...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'larry SAY_COUNT_2'),
(-1000276, '1...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'larry SAY_COUNT_1'),
(-1000275, 'Now I''m gonna give you to the count of ''3'' to get out of here before I sick the dogs on you.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'larry SAY_COUNT'),
(-1000274, 'Time to teach you a lesson in manners, little $Gboy:girl;!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'larry SAY_START');

SET @ENTRY := 19720;
SET @SCRIPTNAME := 'npc_dirty_larry';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;


-- SQL update for commit a0fc18e7b4ccc03dd4771e6e69931e5356ef9e9e

DELETE FROM `script_texts` WHERE `entry` IN (-1910087, -1910088, -1910089);
INSERT INTO `script_texts` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`, `sound`, `type`, `language`, `emote`, `comment`) VALUES
(-1910089, 'He asked if the imp could join in... can you believe it? Actually, it wasn''t half bad...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Concubine Text 3'),
(-1910088, 'We could have had so much fun!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Concubine Text 2'),
(-1910087, 'They fall asleep after. Me, I fall asleep during....', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Concubine Text 1');

SET @ENTRY := 16461;
SET @SCRIPTNAME := 'npc_concubine';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 16488;
SET @SCRIPTNAME := 'npc_arcane_anomaly';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 16489;
SET @SCRIPTNAME := 'npc_chaotic_sentience';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 16491;
SET @SCRIPTNAME := 'npc_mana_feeder';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 16504;
SET @SCRIPTNAME := 'npc_arcane_protector';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;


-- SQL update for commit 9a351b4a7d218005a54f37198f77fce18cc805ac

-- missing text
DELETE FROM `script_texts` WHERE `entry` IN (-1910251, -1910252, -1910253, -1910254, -1910255);
INSERT INTO `script_texts` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`, `sound`, `type`, `language`, `emote`, `comment`) VALUES
(-1910255, 'Hmmm... looks like I''m going to need a new test subject.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Azethen Say 4'),
(-1910254, 'X aman shi Sorankar Rikk ante il mishun il golad.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Dranei Prisoner Say'),
(-1910253, 'Drink this, you blue brute! Disobey me and I promise you a slow, painful death!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Azethen Say 3'),
(-1910252, 'But where could I ever find a suitable test subject?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Azethen Say 2'),
(-1910251, '...a potent concoction', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Azethen Say 1');

SET @ENTRY := 19409;
SET @SCRIPTNAME := 'npc_wing_commander_dabiree';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 20815;
SET @SCRIPTNAME := 'npc_east_hovel';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 20816;
SET @SCRIPTNAME := 'npc_west_hovel';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 20814;
SET @SCRIPTNAME := 'npc_stable';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 20813;
SET @SCRIPTNAME := 'npc_barracks';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 22402;
SET @SCRIPTNAME := 'npc_tower_forge';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 22401;
SET @SCRIPTNAME := 'npc_tower_north';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 21182;
SET @SCRIPTNAME := 'npc_tower_south';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 22403;
SET @SCRIPTNAME := 'npc_tower_foothill';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 181580;
SET @SCRIPTNAME := 'go_central_beacon';
DELETE FROM `gameobject_scripts` WHERE id=@ENTRY;
UPDATE  `gameobject_template` SET `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 181581;
SET @SCRIPTNAME := 'go_western_beacon';
DELETE FROM `gameobject_scripts` WHERE id=@ENTRY;
UPDATE  `gameobject_template` SET `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 181579;
SET @SCRIPTNAME := 'go_southern_beacon';
DELETE FROM `gameobject_scripts` WHERE id=@ENTRY;
UPDATE  `gameobject_template` SET `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

-- SET @ENTRY := **; -- missing ID of npc
-- SET @SCRIPTNAME := 'trigger_beacon_fire';
-- DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
-- UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

SET @ENTRY := 16794;
SET @SCRIPTNAME := 'npc_azethen';
DELETE FROM `creature_ai_scripts` WHERE creature_id=@ENTRY;
UPDATE  `creature_template` SET  `AIName` =  '', `ScriptName` =  @SCRIPTNAME WHERE `entry` =@ENTRY;

