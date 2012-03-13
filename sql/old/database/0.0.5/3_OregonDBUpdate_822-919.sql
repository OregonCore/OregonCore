
-- Scriptnames

UPDATE `creature_template` SET ScriptName = 'npc_corporal_keeshan' WHERE entry = 349;
UPDATE `creature_template` SET ScriptName = 'boss_mr_smite' WHERE entry = 646;
UPDATE `creature_template` SET ScriptName = 'npc_prospector_remtravel' WHERE entry = 2917;
UPDATE `creature_template` SET ScriptName = 'npc_gilthares' WHERE entry = 3465;
UPDATE `creature_template` SET ScriptName = 'npc_disciple_of_naralex' WHERE entry = 3678;
UPDATE `creature_template` SET ScriptName = 'npc_morokk' WHERE entry = 4500;
UPDATE `creature_template` SET ScriptName = 'npc_arugal_voidwalker' WHERE entry = 4627;
UPDATE `creature_template` SET ScriptName = 'npc_aged_dying_ancient_kodo' WHERE entry = 4700;
UPDATE `creature_template` SET ScriptName = 'npc_aged_dying_ancient_kodo' WHERE entry = 4701;
UPDATE `creature_template` SET ScriptName = 'npc_aged_dying_ancient_kodo' WHERE entry = 4702;
UPDATE `creature_template` SET ScriptName = 'npc_private_hendel' WHERE entry = 4966;
UPDATE `creature_template` SET ScriptName = 'npc_ogron' WHERE entry = 4983;
UPDATE `creature_template` SET ScriptName = 'npc_galen_goodward' WHERE entry = 5391;
UPDATE `creature_template` SET ScriptName = 'npc_dalinda' WHERE entry = 5644;
UPDATE `creature_template` SET ScriptName = 'npc_daphne_stilwell' WHERE entry = 6182;
UPDATE `creature_template` SET ScriptName = 'npc_threshwackonator' WHERE entry = 6669;
UPDATE `creature_template` SET ScriptName = 'npc_rinji' WHERE entry = 7780;
UPDATE `creature_template` SET ScriptName = 'npc_00x09hl' WHERE entry = 7806;
UPDATE `creature_template` SET ScriptName = 'npc_oox22fe' WHERE entry = 7807;
UPDATE `creature_template` SET ScriptName = 'npc_paoka_swiftmountain' WHERE entry = 10427;
UPDATE `creature_template` SET ScriptName = 'npc_andorhal_tower' WHERE entry = 10902;
UPDATE `creature_template` SET ScriptName = 'npc_andorhal_tower' WHERE entry = 10903;
UPDATE `creature_template` SET ScriptName = 'npc_andorhal_tower' WHERE entry = 10904;
UPDATE `creature_template` SET ScriptName = 'npc_andorhal_tower' WHERE entry = 10905;
UPDATE `creature_template` SET ScriptName = 'npc_kerlonian' WHERE entry = 11218;
UPDATE `creature_template` SET ScriptName = 'npc_aged_dying_ancient_kodo' WHERE entry = 11627;
UPDATE `creature_template` SET ScriptName = 'npc_myranda_the_hag' WHERE entry = 11872;
UPDATE `creature_template` SET ScriptName = 'npc_garments_of_quests' WHERE entry = 12423;
UPDATE `creature_template` SET ScriptName = 'npc_garments_of_quests' WHERE entry = 12427;
UPDATE `creature_template` SET ScriptName = 'npc_garments_of_quests' WHERE entry = 12428;
UPDATE `creature_template` SET ScriptName = 'npc_garments_of_quests' WHERE entry = 12429;
UPDATE `creature_template` SET ScriptName = 'npc_garments_of_quests' WHERE entry = 12430;
UPDATE `creature_template` SET ScriptName = 'npc_muglash' WHERE entry = 12717;
UPDATE `creature_template` SET ScriptName = 'npc_stillpine_capitive' WHERE entry = 17375;
UPDATE `creature_template` SET ScriptName = 'npc_death_ravager' WHERE entry = 17556;
UPDATE `creature_template` SET ScriptName = 'mob_broggok_poisoncloud' WHERE entry = 17662;
UPDATE `creature_template` SET ScriptName = 'npc_khadgars_servant' WHERE entry = 19685;
UPDATE `creature_template` SET ScriptName = 'mob_torloth' WHERE entry = 22076;
UPDATE `creature_template` SET ScriptName = 'mob_demon_fire' WHERE entry = 23069;
UPDATE `creature_template` SET ScriptName = 'npc_volcano' WHERE entry = 23085;
UPDATE `creature_template` SET ScriptName = 'mob_blaze' WHERE entry = 23259;
UPDATE `creature_template` SET ScriptName = 'mob_flame_crash' WHERE entry = 23336;
UPDATE `creature_template` SET ScriptName = 'npc_kyle_the_frenzied' WHERE entry = 23616;
UPDATE `creature_template` SET ScriptName = 'npc_cassa_crimsonwing' WHERE entry = 23704;
UPDATE `creature_template` SET ScriptName = 'npc_zelfrax' WHERE entry = 23864;
UPDATE `creature_template` SET ScriptName = 'do_nothing' WHERE entry = 24187;

UPDATE `gameobject_template` SET ScriptName = 'go_naga_brazier' WHERE entry = 178247;

UPDATE `instance_template` SET script = 'instance_wailing_caverns' WHERE map = 43;

-- Script texts

REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1043017, 'Disciple SAY_ATTACKED', 'Attacked! Help get this $N off of me!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1043016, 'Naralex SAY_FAREWELL', 'We must go and gather with the other Disciples. There is much work to be done before I can make another attempt to restore the Barrens. Farewell, brave souls!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1043015, 'Naralex SAY_THANK_YOU', 'Ah, to be pulled from the dreaded nightmare! I thank you, my loyal Disciple, along with your brave companions.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1043014, 'Disciple SAY_NARALEX_AWAKES', 'At last! Naralex awakes from the nightmare.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1043013, 'Naralex SAY_I_AM_AWAKE', 'I AM AWAKE, AT LAST!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1043012, 'Disciple SAY_MUTANUS_THE_DEVOURER', 'This Mutanus the Devourer is a minion from Naralex''s nightmare no doubt!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1043011, 'Naralex EMOTE_HORRENDOUS_VISION', '%s dreams up a horrendous vision. Something stirs beneath the murky waters.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1043010, 'Naralex EMOTE_WRITHE_IN_AGONY', '%s writhes in agony. The Disciple seems to be breaking through.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1043009, 'Naralex EMOTE_TROUBLED_SLEEP', '%s tosses fitfully in troubled sleep.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1043008, 'Disciple EMOTE_AWAKENING_RITUAL', '%s begins to perform the awakening ritual on Naralex.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1043007, 'Disciple SAY_EMERALD_DREAM', 'Protect me brave souls as I delve into this Emerald Dream to rescue Naralex and put an end to this corruption!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1043006, 'Disciple SAY_BEYOND_THIS_CORRIDOR', 'Beyond this corridor, Naralex lies in fitful sleep. Let us go awaken him before it is too late.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1043005, 'Disciple SAY_CAVERNS_PURIFIED', 'The caverns have been purified. To Naralex''s chamber we go!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1043004, 'Disciple SAY_BANISH_THE_SPIRITS', 'Within this circle of fire I must cast the spell to banish the spirits of the slain Fanglords.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1043003, 'Disciple SAY_MUST_CONTINUE', 'Come. We must continue. There is much to be done before we can pull Naralex from his nightmare.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1043002, 'Disciple SAY_TEMPLE_OF_PROMISE', 'These caverns were once a temple of promise for regrowth in the Barrens. Now, they are the halls of nightmares.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1043001, 'Disciple SAY_MAKE_PREPARATIONS', 'I must make the necessary preparations before the awakening ritual can begin. You must protect me!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1043000, 'Disciple SAY_AT_LAST', 'At last! Naralex can be awakened! Come aid me, brave adventurers!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1036001, 'smite INST_SAY_ALARM2', 'We''re under attack! A vast, ye swabs! Repel the invaders!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 5777, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1036000, 'smite INST_SAY_ALARM1', 'You there, check out that noise!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 5775, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1033006, 'prisoner adamant SAY_POST2_DOOR_AD', 'Good luck with Arugal. I must hurry back to Hadrec now.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1033005, 'prisoner adamant SAY_POST1_DOOR_AD', 'There we go!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1033004, 'prisoner adamant SAY_OPEN_DOOR_AD', 'You are indeed courageous for wanting to brave the horrors that lie beyond this door.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1033003, 'prisoner adamant SAY_FREE_AD', 'Free from this wretched cell at last! Let me show you to the courtyard....', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1033002, 'prisoner ashcrombe SAY_POST_DOOR_AS', 'There it is! Wide open. Good luck to you conquering what lies beyond. I must report back to the Kirin Tor at once!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1033001, 'prisoner ashcrombe SAY_OPEN_DOOR_AS', 'I have just the spell to get this door open. Too bad the cell doors weren''t locked so haphazardly.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000606, 'SAY_ZELFRAX_2', 'All who venture here belong to me, including you!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000605, 'SAY_ZELFRAX', 'This land was mine long before your wretched kind set foot here.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000604, 'Stillpine Capitive free say text 3', '[Fulborg] Those remaining at Stillpine Hold will welcome you as a hero!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000603, 'Stillpine Capitive free say text 2', '[Fulborg] Thank you, $N', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000602, 'Stillpine Capitive free say text 1', '[Fulborg] The Stillpine furbolgs will not soon forget your bravery!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000601, 'earthmender wilda say ASSASSIN_SAY_AGGRO2', 'You will never essscape Coilssskarrr...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000600, 'earthmender wilda say ASSASSIN_SAY_AGGRO1', 'Kill them all!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000588, 'Galen - emote disapper', '%s disappears into the swamp.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000587, 'Galen - emote whisper', '%s whispers to $N the secret to opening his strongbox.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000586, 'Galen - quest complete', 'Thank you $N. I will remember you always. You can find my strongbox in my camp, north of Stonard.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000585, 'Galen - aggro 2', 'Help! I''m under attack!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000584, 'Galen - aggro 1', 'Look out! The $c attacks!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000583, 'Galen - quest accepted', 'Let us leave this place.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000582, 'Galen - periodic say', 'Help! Please, You must help me!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000574, 'MIRAN_SAY_QUEST_END', 'Ah, here at last! It''s going to feel so good to get rid of these barrels.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000573, 'MIRAN_SAY_AMBUSH_TWO', 'Send them on! I''m not afraid of some scrawny beasts!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000572, 'DARK_IRON_RAIDER_SAY_AMBUSH', 'Feel the power of the Dark Iron Dwarves!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000571, 'MIRAN_SAY_AMBUSH_ONE', 'Help! I''ve only one hand to defend myself with.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000565, 'SAY_CORPORAL_KEESHAN_5', 'Brave adventurer, thank you for rescuing me! I am sure Marshal Marris will reward your kind deed.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000564, 'SAY_CORPORAL_KEESHAN_4', 'Marshal Marris, sir. Corporal Keeshan of the 12th Sabre Regiment returned from battle and reporting for duty!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000563, 'SAY_CORPORAL_KEESHAN_3', 'The Blackrock infestation is thick in these parts. I will do my best to keep the pace. Let''s go!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000562, 'SAY_CORPORAL_KEESHAN_2', 'Ah, fresh air, at last! I need a moment to rest.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000561, 'SAY_CORPORAL_KEESHAN_1', 'My wounds are grave. Forgive my slow pace but my injuries won''t allow me to walk any faster.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000540, 'lordillidan EVENT_COMPLETED', 'So you have defeated the Crimson Sigil. You now seek to challenge my rule? Not even Arthas could defeat me, yet you dare to even harbor such thoughts? Then I say to you, come! Come $N! The Black Temple awaits...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000539, 'lordillidan ILLIDAN_SUMMON4', 'Torloth your master calls!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000538, 'lordillidan ILLIDAN_SUMMON3', 'Run while you still can. The highlords come soon...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000537, 'lordillidan ILLIDAN_SUMMON2', 'You are no challenge for the Crimson Sigil. Mind breakers, end this nonsense.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000536, 'lordillidan ILLIDAN_SUMMON1', 'What manner of fool dares stand before Illidan Stormrage? Soldiers, destroy these insects!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000535, 'lordillidan ILLIDAN_DIALOGUE', 'Destroy them, Torloth. Let lose their blood like a river upon this hallowed ground.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000534, 'torloth TORLOTH_DIALOGUE3', 'Yes, Lord Illidan, I would sacrifice to you this magnificent physique. On this day you will fall - another victim of Torloth...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000533, 'torloth TORLOTH_DIALOGUE2', 'As you desire, Lord Illidan.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000532, 'torloth TORLOTH_DIALOGUE1', 'At your command, my liege...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000510, 'muglash SAY_MUG_RETURN', 'Please return to Zoram''gar and report our success to the Warsong runner.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000509, 'muglash SAY_MUG_PATROL', 'I am going to patrol the area for a while longer and ensure that things are truly safe.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000508, 'muglash SAY_MUG_GRATITUDE', 'You have my deepest gratitude. I thank you.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000507, 'muglash SAY_MUG_DONE', 'We have done it!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000506, 'muglash SAY_MUG_REST', 'Perhaps we will get a moment to rest. But i will not give up until we have faced off against Vorsha!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000505, 'muglash SAY_MUG_ON_GUARD', 'Be on your guard, $n!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000504, 'muglash SAY_MUG_BRAZIER_WAIT', 'Now we must wait. It won''t be long before the naga realize what we have done.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000503, 'muglash SAY_MUG_BRAZIER', 'This is the brazier, $n. Put it out. Vorsha is a beast, worthy of praise from no one!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000502, 'muglash SAY_MUG_START2', 'This will be a though fight, $n. Follow me closely.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000501, 'muglash SAY_MUG_START1', 'Are you sure that you are ready? If we do not have a group of your allies to aid us, we will surely fail.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000500, 'morokk SAY_MOR_SCARED', 'Me scared! Me run now!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000499, 'morokk SAY_MOR_CHALLENGE', 'Puny $r wanna fight %s? Me beat you! Me boss here!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000495, 'SAY_KHAD_HAGGARD', 'Khadgar himself is showing you around? You must have made a good impression, $n.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000494, 'SAY_KHAD_SCRYER_GREET', 'We''re rather selective of who we befriend, $n. You think you have what it takes?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000493, 'SAY_KHAD_ALDOR_GREET', 'Light be with you, $n. And welcome to Shattrath.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000492, 'SAY_KHAD_MIND_ALWAYS', 'Are you joking? I''m a Scryer, I always watch what i say. More enemies than allies in this city, it seems.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000491, 'SAY_KHAD_MIND_YOU', 'Be mindful of what you say, this one''s being shown around by Khadgar''s pet.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000490, 'SAY_KHAD_INJURED', 'A newcomer to Shattrath! Make sure to drop by later. We can always use a hand with the injured.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000489, 'SAY_KHAD_START', 'Show our guest around Shattrath, will you? Keep an eye out for pickpockets in the lower city.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000471, 'ogron SAY_OGR_THANKS', 'By the way, thanks for watching my back.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000470, 'ogron SAY_OGR_RET_LUCKY', 'Must be your lucky day. Alright, I''ll talk. Just leave me alone. Look, you''re not going to believe me, but it wa... oh, Light, looks like the girl could shoot...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000469, 'ogron SAY_OGR_SURVIVE', 'Still with us, Reethe?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000468, 'ogron SAY_OGR_NODIE', 'Damn it! You''d better not die on me, human!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000467, 'ogron SAY_OGR_CAL_CLEANUP', 'Now, let''s clean up the rest of the trash, men!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000466, 'ogron SAY_OGR_RET_ARROW', 'Ugh... Hallan, didn''t think you had it in you...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 34, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000465, 'ogron EMOTE_OGR_RET_ARROW', '%s staggers backwards as the arrow lodges itself deeply in his chest.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000464, 'ogron SAY_OGR_HALL_GLAD', 'Gladly, sir.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000463, 'ogron SAY_OGR_CAL_MERCY', 'Private, show Lieutenant Reethe some mercy.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 29, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000462, 'ogron SAY_OGR_CAL_FOUND', 'Paval Reethe! Found you at last. And consorting with ogres now? No fear, even deserters and traitors are afforded some mercy.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000461, 'ogron SAY_OGR_RET_HEAR', 'What was that? Did you hear something?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000460, 'ogron SAY_OGR_RET_NOFIRE', 'And I don''t know anything about this fire of yours...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000459, 'ogron SAY_OGR_RET_NOCLOSER', 'Not one step closer, ogre!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 27, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000458, 'ogron SAY_OGR_TELL_FIRE', 'Look here, if you don''t tell me about the fire--', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000457, 'ogron SAY_OGR_RET_TAKEN', 'I... Well, I may of taken a little thing or two from the inn... but what would an ogre care about that?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 6, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000456, 'ogron SAY_OGR_REPLY_RET', 'Just tell us what you know about the Shady Rest Inn, and I won''t bash your skull in.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000455, 'ogron SAY_OGR_RET_SWEAR', 'I swear. I didn''t steal anything from you! Here, take some of my supplies, just go away!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 27, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000454, 'ogron SAY_OGR_RET_WHAT', 'W-what do you want? Just leave me alone...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 6, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000453, 'ogron SAY_OGR_SPOT', 'That''s Reethe alright. Let''s go see what he has to say, yeah?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000452, 'ogron SAY_OGR_START', 'I noticed some fire on that island over there. A human, too. Let''s go check it out, $n.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000451, 'ame01 SAY_AGGRO3', 'Bad, $c. $c, bad!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000450, 'ame01 SAY_AGGRO2', 'Danger. Danger! $c try hurt A-Me.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000449, 'ame01 SAY_AGGRO1', '$c, no hurt A-Me. A-Me good.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000448, 'ame01 SAY_FINISH', 'A-Me home! A-Me good! Good A-Me. Home. Home. Home.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000447, 'ame01 SAY_SEARCH', 'Good... good, A-Me. A-Me good. Home. Find home.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000446, 'ame01 SAY_READY', 'A-Me good. Good, A-Me. Follow... follow A-Me. Home. A-Me go home.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000445, 'kerlonian EMOTE_KER_AWAKEN', '%s wakes up!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000444, 'kerlonian SAY_KER_END', 'Oh, I can see Liladris from here... Tell her I''m here, won''t you?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000443, 'kerlonian SAY_KER_ALERT_2', 'It''s quiet... Too quiet...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000442, 'kerlonian SAY_KER_ALERT_1', 'Be on the alert! The Blackwood furbolgs are numerous in the area...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000441, 'kerlonian SAY_KER_SLEEP_4', 'You don''t mind if I stop here for a moment, do you?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000440, 'kerlonian SAY_KER_SLEEP_3', 'Oh, I am so tired...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000439, 'kerlonian SAY_KER_SLEEP_2', 'Yaaaaawwwwwnnnn...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000438, 'kerlonian SAY_KER_SLEEP_1', 'This looks like the perfect place for a nap...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000437, 'kerlonian EMOTE_KER_SLEEP_3', '%s begins to drift off...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000436, 'kerlonian EMOTE_KER_SLEEP_2', '%s suddenly falls asleep', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000435, 'kerlonian EMOTE_KER_SLEEP_1', '%s looks very sleepy...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000434, 'kerlonian SAY_KER_START', 'Liladris has been waiting for me at Maestra''s Post, so we should make haste, $N.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000433, 'ringo EMOTE_RIN_END_8', '%s fades away after a long pause.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000432, 'ringo SAY_SPR_END_7', 'Ringo? Wake up! Don''t worry, I''ll take care of you.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000431, 'ringo SAY_RIN_END_6', 'Ugh.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000430, 'ringo EMOTE_RIN_END_5', '%s stands up after a short pause.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000429, 'ringo EMOTE_RIN_END_4', '%s collapses onto the ground.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000428, 'ringo SAY_RIN_END_3', 'Oh... I''m feeling faint...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000427, 'ringo SAY_SPR_END_2', 'Ringo! You''re okay!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000426, 'ringo SAY_RIN_END_1', 'Spraggle! I didn''t think I''d make it back!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000425, 'ringo SAY_WAKE_4', 'How am I feeling? Quite soaked, thank you.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000424, 'ringo SAY_WAKE_3', 'Yes, I must go on.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000423, 'ringo SAY_WAKE_2', 'I am feeling a little better now, thank you.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000422, 'ringo SAY_WAKE_1', 'Where... Where am I?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000421, 'ringo SAY_FAINT_4', 'I''m not feeling so well...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000420, 'ringo SAY_FAINT_3', 'Uuuuuuggggghhhhh....', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000419, 'ringo SAY_FAINT_2', 'Maybe... you could carry me?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000418, 'ringo SAY_FAINT_1', 'The heat... I can''t take it...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000417, 'ringo SAY_RIN_START_2', 'Remember, if I faint again, the water that Spraggle gave you will revive me.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000416, 'ringo SAY_RIN_START_1', 'Well, I''m not sure how far I''ll make it in this state... I''m feeling kind of faint...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000415, 'hendel EMOTE_SURRENDER', '%s, too injured, gives up the chase.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000414, 'hendel SAY_PROGRESS_4_TER', 'We can only stay a few moments before returning to the tower. If you wish to speak to us more you may find us there.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000413, 'hendel SAY_PROGRESS_3_TER', 'I apologize for taking so long to get here. I wanted Lady Proudmoore to be present also.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000412, 'hendel SAY_PROGRESS_2_HEN', 'Please... please... Miss Proudmore. I didn''t mean to...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000411, 'hendel SAY_PROGRESS_1_TER', 'Why don''t we deal with you now, Hendel? Lady Proudmoore will speak for you back in the tower.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000410, 'kanati SAY_KAN_START', 'Here they come! Defend yourself!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 5, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000409, 'SAY_RIN_PROGRESS_2', 'You find it, you keep it! Don''t tell no one that Rin''ji talked to you!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000408, 'SAY_RIN_PROGRESS_1', 'Rin''ji will tell you secret now... $n, should go to the Overlook Cliffs. Rin''ji hid something on island  there', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000407, 'SAY_RIN_COMPLETE', 'Rin''ji can see road now, $n. Rin''ji knows the way home.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000406, 'SAY_RIN_HELP_2', 'Rin''ji is being attacked!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000405, 'SAY_RIN_HELP_1', 'Rin''ji needs help!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000404, 'SAY_RIN_BY_OUTRUNNER', 'Attack my sisters! The troll must not escape!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000403, 'SAY_RIN_FREE', 'Rin''ji is free!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000402, 'emeriss SAY_CASTCORRUPTION', 'Taste your world''s corruption!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000401, 'emeriss SAY_AGGRO', 'Hope is a DISEASE of the soul! This land shall wither and die!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000400, 'taerar SAY_SUMMONSHADE', 'Children of Madness - I release you upon this world!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000399, 'taerar SAY_AGGRO', 'Peace is but a fleeting dream! Let the NIGHTMARE reign!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000398, 'tooga SAY_TORT_POST_6', 'And try not to get lost this time...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000397, 'tooga SAY_TOOG_POST_5', 'Yes Torta. Whatever your heart desires...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000396, 'tooga SAY_TORT_POST_4', 'Enough with the rambling. I am starving! Now, get your dusty shell into that ocean and bring momma some grub.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000395, 'tooga SAY_TOOG_POST_3', 'My dearest Torta. I have been gone for so long. Finally we are reunited. At long last our love can blossom again.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000394, 'tooga SAY_TORT_POST_2', 'You have any idea how long I''ve been waiting here? And where''s dinner? All that time gone and nothing to show for it?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000393, 'tooga SAY_TOOG_POST_1', 'Torta, my love! I have returned at long last.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000392, 'tooga SAY_TOOG_WORRIED', 'Torta must be so worried.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000391, 'tooga SAY_TOOG_THIRST', 'I''m Thirsty.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000390, 'wilda SAY_WIL_END', 'Thank you, $n. Please return to my brethren at the Altar of Damnation, near the Hand of Gul''dan, and tell them that Wilda is safe. May the Earthmother watch over you...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000389, 'wilda SAY_WIL_JUST_AHEAD', 'It shouldn''t be much further, $n. The exit is just up ahead.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000388, 'wilda SAY_WIL_PROGRESS5', 'The tumultuous nature of the great waterways of Azeroth and Draenor are a direct result of tormented water spirits.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000387, 'wilda SAY_WIL_PROGRESS4', 'Lady Vashj must answer for these atrocities. She must be brought to justice!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000386, 'wilda SAY_WIL_FIND_EXIT', 'Now we must find the exit.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000385, 'wilda SAY_WIL_PROGRESS2', 'The naga of Coilskar are exceptionally cruel to their prisoners. It is a miracle that I survived inside that watery prison for as long as I did. Earthmother be praised.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000384, 'wilda SAY_WIL_PROGRESS1', 'Grant me protection $n, i must break trough their foul magic!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000383, 'wilda SAY_WIL_AGGRO2', 'Naga attackers! Defend yourself!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000382, 'wilda SAY_WIL_AGGRO1', 'Watch out!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000381, 'wilda SAY_WIL_START', 'I sense the tortured spirits, $n. They are this way, come quickly!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000380, 'gilthares SAY_GIL_FREED', 'Captain Brightsun, $N here has freed me! $N, i am certain the Captain will reward your bravery.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 66, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000379, 'gilthares SAY_GIL_SWEET', 'Ah, the sweet salt air of Ratchet.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000378, 'gilthares SAY_GIL_ALMOST', 'Almost back to Ratchet! Let''s keep up the pace...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000377, 'gilthares SAY_GIL_AGGRO_4', 'Get this $C off of me!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000376, 'gilthares SAY_GIL_AGGRO_3', '$C coming right at us!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000375, 'gilthares SAY_GIL_AGGRO_2', '$C heading this way fast! Time for revenge!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000374, 'gilthares SAY_GIL_AGGRO_1', 'Help! $C attacking!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000373, 'gilthares SAY_GIL_FREEBOOTERS', 'Looks like the Southsea Freeboters are heavily entrenched on the coast. This could get rough.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 25, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000372, 'gilthares SAY_GIL_PROCEED', 'Now i feel better. Let''s get back to Ratchet. Come on, $n.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 23, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000371, 'gilthares SAY_GIL_AT_LAST', 'At last! Free from Northwatch Hold! I need a moment to catch my breath...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 5, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000370, 'gilthares SAY_GIL_START', 'Stay close, $n. I''ll need all the help I can get to break out of here. Let''s go!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000369, 'lakota SAY_LAKO_END', 'Finally, free at last... I must be going now, thanks for helping me escape. I can get back to Freewind Post by myself.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000368, 'lakota SAY_LAKO_MORE', 'More Grimtotems are coming this way!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000367, 'lakota SAY_LAKO_HERE_COME', 'Here they come.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000366, 'lakota SAY_LAKO_LOOK_OUT', 'Look out, the Grimtotem are upon us!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000365, 'lakota SAY_LAKO_START', 'Be on guard... Arnak has some strange power over the Grimtotem... they will not be happy to see me escape.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000364, 'paoka SAY_COMPLETE', 'Thanks a bunch... I can find my way back to Whitereach Post from here. Be sure to talk with Motega Firemane; perhaps you can keep him from sending me home.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000363, 'paoka SAY_WYVERN', 'Now this looks familiar. If we keep heading east, I think we can... Ahh, Wyvern on the attack!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000362, 'paoka SAY_START', 'Let''s go $N. I am ready to reach Whitereach Post.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000361, ' ysondre SAY_SUMMONDRUIDS', 'Come forth, ye Dreamers - and claim your vengeance!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000360, ' ysondre SAY_AGGRO', 'The strands of LIFE have been severed! The Dreamers must be avenged!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000359, 'KAYA_END', 'Thank you for helping me. I know my way back from here.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000358, 'KAYA_AMBUSH', 'Look out! We''re under attack!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000357, 'KAYA_SAY_START', 'Let''s go before they find out I''m free!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000356, 'calvin SAY_COMPLETE', 'Okay, okay... gimme a minute to rest now. You gone and beat me up good.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 14, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000355, 'sprysprocket SAY_RIZZLE_FINAL', 'All right, you win! I surrender! Just don''t hurt me!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000354, 'sprysprocket SAY_RIZZLE_GRENADE', 'Stupid grenade picked a fine time to backfire! So much for high quality goblin engineering!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000352, 'sprysprocket EMOTE_START', '%s takes the Southfury moonstone and escapes into the river. Follow her!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 3);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000351, 'sprysprocket SAY_RIZZLE_START', 'You, there! Hand over that moonstone and nobody gets hurt!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000350, 'kodo round SAY_SMEED_HOME_3', 'That kodo sure is a beauty. Wait a minute, where are my bifocals? Perhaps you should inspect the beast for me.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000349, 'kodo round SAY_SMEED_HOME_2', 'Hey, look out with that kodo! You had better inspect that beast before i give you credit!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000348, 'kodo round SAY_SMEED_HOME_1', 'Ah...the wondrous sound of kodos. I love the way they make the ground shake... inspect the beast for me.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000347, 'kayra SAY_END', 'We''re almost to the refuge! Let''s go.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000346, 'kayra SAY_AMBUSH2', 'Look out, $N! Enemies ahead!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000345, 'kayra SAY_PROGRESS', 'Let''s keep moving. We''re not safe here!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000344, 'kayra SAY_AMBUSH1', 'Looks like we won''t get away so easy. Get ready!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000343, 'kayra SAY_START', 'Is the way clear? Let''s get out while we can, $N.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000342, 'kyle EMOTE_DANCE', '%s thanks you with a special dance.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000341, 'kyle EMOTE_EAT_LUNCH', '%s eats his lunch.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000340, 'kyle EMOTE_SEE_LUNCH', '%s howls in delight at the sight of his lunch!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000339, 'remtravel SAY_REM_AGGRO', 'Something tells me this $r wants the mysterious fossil too. Help!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000338, 'remtravel EMOTE_REM_END', '%s goes back to work, oblivious to everything around him.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000337, 'remtravel SAY_REM_REMEMBER', 'Ah. I remember now! I gave the mysterious fossil to Hollee! Check with her.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000336, 'remtravel SAY_REM_PROGRESS', 'Nope. didn''t leave the fossil back here!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000335, 'remtravel SAY_REM_MOSS_PROGRESS', 'Oh wait! I''m supposed to be looking for that mysterious fossil!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000334, 'remtravel EMOTE_REM_MOSS', '%s inspects the ancient, mossy stone.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000333, 'remtravel SAY_REM_MOSS', 'Interesting... I hadn''t noticed this earlier...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000332, 'remtravel SAY_REM_TENT1_2', 'Oh wait, that''s Hollee''s tent... and it''s empty.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000331, 'remtravel SAY_REM_TENT1_1', 'I bet you I left it in the tent!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000330, 'remtravel SAY_REM_BOOK', 'No mysterious fossil here... Ah, but my copy of Green Hills of Stranglethorn. What a good book!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000329, 'remtravel SAY_REM_RAMP1_2', 'Hrm, nothing up here.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000328, 'remtravel SAY_REM_RAMP1_1', 'Now where did I put that mysterious fossil? Ah, maybe up there...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000327, 'remtravel SAY_REM_START', 'Ok, $n, let''s go find where I left that mysterious fossil. Follow me!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000326, 'threshwackonator SAY_AT_CLOSE', 'YARRR! Swabie, what have ye done?! He''s gone mad! Baton him down the hatches! Hoist the mast! ARRRR! Every man for hi''self!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000325, 'threshwackonator EMOTE_START', '"Threshwackonator First Mate unit prepared to follow"', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000324, 'mist EMOTE_AT_HOME', '%s growls in acknowledgement before straightening and making her way off into the forest.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000323, 'mist SAY_AT_HOME', 'Mist! I feared I would never see you again! Yes, I am well, do not worry for me. You must rest and recover your health.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000322, 'kelerun TEXT_SECOND_TRIAL_4', 'Show $n the meaning of pain, Sunstriker!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000321, 'kelerun TEXT_SECOND_TRIAL_3', 'Show this upstart how a real Blood Knight fights, Swiftblade!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000320, 'kelerun TEXT_SECOND_TRIAL_2', 'Champion Lightrend, make me proud!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000319, 'kelerun TEXT_SECOND_TRIAL_1', 'Let the trial begin, Bloodwrath, attack!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000318, 'erland SAY_BYE', 'Try to take better care of yourself, Quinn. You were lucky this time.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000317, 'erland SAY_QUINN_REPLY', 'I''ve been better. Ivar the Foul got the better of me...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1, 33, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000316, 'erland SAY_QUINN', 'Hello, Quinn. How are you faring?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000315, 'erland SAY_CHECK_NEXT', 'If I am excused, then I''d like to check on Quinn...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000314, 'erland SAY_RANE_REPLY', 'Masses of wolves are to the east, and whoever lived at Malden''s Orchard is gone.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000313, 'erland SAY_RANE', 'It''s good to see you again, Erland. What is your report?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1, 33, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000312, 'erland SAY_END', 'We made it! Thanks, $N. I couldn''t have gotten without you.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000311, 'erland SAY_PROGRESS', 'We''re almost there!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000310, 'erland SAY_AGGRO_3', 'Oh no! A $C is upon us!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000309, 'erland SAY_AGGRO_2', 'Beware! I am under attack!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000308, 'erland SAY_AGGRO_1', 'A $C attacks!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000307, 'erland SAY_START_2', 'Be careful, $N. Those wolves like to hide among the trees.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000306, 'erland SAY_START_1', 'Let''s get to the others, and keep an eye open for those wolves cutside...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000305, 'wizzlecrank SAY_END', 'Hmm... I don''t think this blinking red light is a good thing...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000304, 'wizzlecrank SAY_PROGRESS_3', 'That was a close one! Well, let''s get going, it''s still a ways to Ratchet!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000303, 'wizzlecrank SAY_PROGRESS_2', 'Come on, don''t break down on me now!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 255, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000302, 'wizzlecrank SAY_PROGRESS_1', 'Looks like we''re out of woods, eh? Wonder what this does...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000301, 'wizzlecrank SAY_MERCENARY', 'There''s the stolen shredder! Stop it or Lugwizzle will have our hides!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000300, 'wizzlecrank SAY_STARTUP2', 'Okay, I think I''ve got it, now. Follow me, $n!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000299, 'wizzlecrank SAY_STARTUP1', 'Arrrgh! This isn''t right!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000298, 'wizzlecrank SAY_START', 'Alright, alright I think I can figure out how to operate this thing...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 255, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000297, 'stilwell SAY_DS_PROLOGUE', 'Meet me down by the orchard--I just need to put my gun away.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000296, 'stilwell SAY_DS_DOWN_3', 'We''ve done it! We won!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000295, 'stilwell SAY_DS_DOWN_2', 'One more down!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000294, 'stilwell SAY_DS_DOWN_1', 'We showed that one!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000293, 'stilwell SAY_DS_START', 'To the house! Stay close to me, no matter what! I have my gun and ammo there!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000292, 'oox SAY_OOX_END', 'Cloaking systems online! CLUCK! Engaging cloak for transport to Booty Bay!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000291, 'oox SAY_OOX17_AMBUSH_REPLY', 'No one challanges the Wastewander nomads - not even robotic chickens! ATTACK!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000290, 'oox SAY_OOX_AMBUSH', 'CLUCK! Sensors detect spatial anomaly - danger imminent! CLUCK!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000289, 'oox SAY_OOX_AGGRO2', 'Thread analyzed! Activating combat plan beta! CLUCK!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000288, 'oox SAY_OOX_AGGRO1', 'Physical threat detected! Evasive action! CLUCK!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000287, 'oox SAY_OOX_START', 'Emergency power activated! Initializing ambulanory motor! CLUCK!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000286, 'clintar SAY_END', 'Lurosa, I am entrusting the Relics of Aviana to $N, who will take them to Morthis Whisperwing. I must return completely to the Emerald Dream now. Do not let $N fail!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000285, 'clintar SAY_RELIC3', 'We have all three of the relics, but my energy is rapidly fading. We must make our way back to Dreamwarden Lurosa! He will let you know what to do next.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000284, 'clintar SAY_RELIC2', 'I''ve recovered the second relic. Take a moment to rest, and then we''ll continue to the last reliquary.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000283, 'clintar SAY_RELIC1', 'That''s the first relic, but there are still two more. Follow me, $N.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000282, 'clintar SAY_AGGRO_2', 'Begone from this place!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000281, 'clintar SAY_AGGRO_1', 'The Emerald Dream will never be yours!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000280, 'clintar SAY_START', 'A shadowy, sinister presence has invaded the Emerald Dream. Its power is poised to spill over into our world, $N. We must oppose it! That''s why I cannot accompany you in person.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000279, 'larry SAY_GIVEUP', 'Alright, we give up! Don''t hurt us!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000278, 'SAY_5', 'Time to meet your maker!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000277, 'larry SAY_4', '2...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000276, 'larry SAY_3', '1...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000275, 'larry SAY_2', 'Now I''m gonna give you to the count of ''3'' to get out of here before I sick the dogs on you.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000274, 'SAY_1', 'Time to teach you a lesson in manners, little $Gboy:girl;!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000273, 'phizzlethorpe SAY_PROGRESS_9', 'Doctor Draxlegauge will give you further instructions, $N. Many thanks for your help!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000272, 'phizzlethorpe EMOTE_PROGRESS_8', '%s hands one glowing goggles over to Doctor Draxlegauge.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000271, 'phizzlethorpe SAY_PROGRESS_7', 'Phew! Glad to be back from that creepy cave.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000270, 'phizzlethorpe SAY_PROGRESS_6', 'I''ve done it! I have harnessed the power of the stone into the goggles! Let''s get out of here!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000269, 'phizzlethorpe SAY_PROGRESS_5', 'Almost done! Just a little longer!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000268, 'phizzlethorpe SAY_AGGRO', 'Help!!! Get these things off me so I can get my work done!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000267, 'phizzlethorpe EMOTE_PROGRESS_4', '%s begins tinkering with the goggles before the stone.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000266, 'phizzlethorpe SAY_PROGRESS_3', 'I''ll begin drawing energy from the stone. Your job, $N, is to defend me. This place is cursed... trust me.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000265, 'phizzlethorpe SAY_PROGRESS_2', 'I discovered this cave on our first day here. I believe the energy in the stone can be used to our advantage.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000264, 'phizzlethorpe SAY_PROGRESS_1', 'Ok, $N. Follow me to the cave where I''ll attempt to harness the power of the rune stone into these goggles.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 1, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000263, 'garments SAY_SHAYA_GOODBYE', 'Farewell to you, and may Elune be with you always.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 3, 2, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000262, 'garments SAY_SHAYA_THANKS', 'Thank you! Thank you, $GPriest:Priestess;. Now I can take on those corrupt timberlings with Elune''s power behind me!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 4, 2, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000261, 'garments SAY_DOLF_GOODBYE', 'Farewell to you, and may the Light be with you always.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 3, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000260, 'garments SAY_DOLF_THANKS', 'Thank you! Thank you, $GPriest:Priestess;. Now I can take on those wendigo with the Light on my side!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 4, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000259, 'garments SAY_KORJA_GOODBYE', 'Farewell to you, and may our ancestors be with you always!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 3, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000258, 'garments SAY_KORJA_THANKS', 'Thank you! Thank you, $GPriest:Priestess;. Now I can take on those humans with your power to back me!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 4, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000257, 'garments SAY_ROBERTS_GOODBYE', 'Farewell to you, and may the Light be with you always.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 3, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000256, 'garments SAY_ROBERTS_THANKS', 'Thank you! Thank you, $GPriest:Priestess;. Now I can take on those murlocs with the Light on my side!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 4, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000255, 'SAY_KHAD_SERV_21', 'Good luck stranger, and welcome to Shattrath City.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000254, 'SAY_KHAD_SERV_20', 'Khadgar should be ready to see you again. Just remember that to serve the Sha''tar you will most likely have to ally with the Aldor or the Scryers. And seeking the favor of one group will cause the others'' dislike.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000253, 'SAY_KHAD_SERV_19', 'Such is their animosity that they vie for the honor of being sent to assist the naaru there. Each day, that decision is made here by A''dal. The armies gather here to receive A''dal''s blessing before heading to Shadowmoon.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000252, 'SAY_KHAD_SERV_18', 'There he continues to wage war on Illidan with the assistance of the Aldor and the Scryers. The two factions have not given up on their old feuds, though.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000251, 'SAY_KHAD_SERV_17', 'The attacks against Shattrath continued, but the city did not fall, as you can see. On the contrary, the naaru known as Xi''ri led a successful incursion into Shadowmoon Valley - Illidan''s doorstep.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000250, 'SAY_KHAD_SERV_16', 'The Aldor''s most holy temple and its surrounding dwellings lie on the terrace above. As a holy site, only the initiated are welcome inside.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000249, 'SAY_KHAD_SERV_15', 'Many of the priesthood had been slain by the same magisters who now vowed to serve the naaru. They were not happy to share the city with their former enemies.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000248, 'SAY_KHAD_SERV_14', 'The Aldor are followers of the Light and forgiveness and redemption are values they understand. However, they found hard to forget the deeds of the blood elves while under Kael''s command.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000247, 'SAY_KHAD_SERV_13', 'The naaru accepted the defectors, who would become known as the Scryers; their dwelling lies in the platform above. Only those initiated with the Scryers are allowed there.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000246, 'SAY_KHAD_SERV_12', 'The defection of Voren''thal and his followers was the largest loss ever incurred by Kael''s forces. And these weren''t just any blood elves. Many of the best and brightest amongst Kael''s scholars and magisters had been swayed by Voren''thal''s influence.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000245, 'SAY_KHAD_SERV_11', 'As the naaru approached him, Voren''thal kneeled before him and uttered the following words: "I''ve seen you in a vision, naaru. My race''s only hope for survival lies with you. My followers and I are here to serve you."', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000244, 'SAY_KHAD_SERV_10', 'The blood elves laid down their weapons in front of the city''s defenders; their leader, a blood elf elder known as Voren''thal, stormed into the Terrace of Light and demanded to speak to A''dal.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000243, 'SAY_KHAD_SERV_9', 'As the regiment of blood elves crossed this very bridge, the Aldor''s exarchs and vindicators lined up to defend the Terrace of Light. But then the unexpected happened.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000242, 'SAY_KHAD_SERV_8', 'It wouldn''t be long, however, before the city came under attack once again. This time, the attack came from Illidan''s armies. A large regiment of blood elves had been sent by Illidan''s ally, Kael''thas Sunstrider, to lay waste to the city.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000241, 'SAY_KHAD_SERV_7', 'The priesthood, known as the Aldor, quickly regained its strength as word spread that the naaru had returned and reconstruction soon began. The ruined temple is now used as an infirmary for injured refugees.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000240, 'SAY_KHAD_SERV_6', 'They were drawn to the ruins of Shattrath City where a small remnant of the draenei priesthood conducted its rites inside a ruined temple on this very spot.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000239, 'SAY_KHAD_SERV_5', 'The Sha''tar, or "born from light" are the naaru that came to Outland to fight the demons of the Burning Legion.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000238, 'SAY_KHAD_SERV_4', 'Let us go into the Lower City. I will warn you that as one of the only safe havens in Outland, Shattrath has attracted droves of refugees from all wars, current and past.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000237, 'SAY_KHAD_SERV_3', 'The city was left in ruins and darkness... until the Sha''tar arrived.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000236, 'SAY_KHAD_SERV_2', 'When the Burning Legion turned the orcs against the draenei, the fiercest battle was fought here. The draenei fought tooth and nail, but in the end the city fell.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000235, 'SAY_KHAD_SERV_1', 'Shattrath was once the draenei capital of this world. Its name means "dwelling of light."', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000234, 'SAY_KHAD_SERV_0', 'Follow me, stranger. This won''t take long.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000233, 'garments SAY_DG_KEL_GOODBYE', 'Farewell to you, and may shadow always protect you!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 3, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000232, 'garments SAY_DG_KEL_THANKS', 'Thank you! Thank you, $GPriest:Priestess;. Now I can take on those gnolls with your power to back me!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 4, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000231, 'garments SAY_COMMON_HEALED', 'Ah, $GPriest:Priestess; you came along just in time. I appreciate it.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 20, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000230, 'WHISPER_CUSTODIAN_14', 'Our time is at an end $N. I would wish you luck, if such a thing existed.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000229, 'WHISPER_CUSTODIAN_13', 'All we know of this timeway is that it leads to Mount Hyjal. The Infinite Dragonflight have gone to great lengths to prevent our involvement. We know next to nothing, mortal. Soridormi is currently attempting to break through the timeway''s defenses but has thus far been unsuccessful. You might be our only hope of breaking through and resolving the conflict.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000228, 'WHISPER_CUSTODIAN_12', 'The timeways are currently ranked in order from least catastrophic to most catastrophic. Note that they are all classified as catastrophic, meaning that any single one of these timeways collapsing would mean that your world would end. We only classify them in such a way so that the heroes and adventurers that are sent here know which timeway best suits their abilities.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000227, 'WHISPER_CUSTODIAN_11', 'This timeway is currently collapsing. What that may hold for the past, present and future is currently unknown...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000226, 'WHISPER_CUSTODIAN_10', 'We have very little information on this timeway. Sa''at has been dispatched and is currently inside. The data we have gathered from his correspondence is that the Infinite Dragonflight are once again attempting to alter time. Could it be that the opening of the Dark Portal is being targeted for sabotage? Let us hope not...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000225, 'WHISPER_CUSTODIAN_9', 'This timeway is in great disarray! We have agents inside right now attempting to restore order. What information I have indicates that Thrall''s freedom is in jeopardy. A malevolent organization known as the Infinite Dragonflight is trying to prevent his escape. I fear without outside assistance, all will be lost.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000224, 'WHISPER_CUSTODIAN_8', 'These tunnels that you see are called timeways. They are infinite in number. The ones that currently exist in your reality are what the master has deemed as ''trouble spots.'' These trouble spots may differ completely in theme but they always share a cause. That is, their existence is a result of the same temporal disturbance. Remember that should you venture inside one...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000223, 'WHISPER_CUSTODIAN_7', 'For normal maintenance of time, the Keepers of Time are sufficient caretakers. We are able to deal with most ordinary disturbances. I speak of little things, such as rogue mages changing something in the past to elevate their status or wealth in the present.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000222, 'WHISPER_CUSTODIAN_6', 'As custodians of time, we watch over and care for Nozdormu''s realm. The master is away at the moment, which means that attempts are being made to dramatically alter time. The master never meddles in the affairs of mortals but instead corrects the alterations made to time by others. He is reactionary in this regard.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000221, 'WHISPER_CUSTODIAN_5', 'There are only two truths to be found here: First, that time is chaotic, always in flux, and completely malleable and second, perception does not dictate reality.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000220, 'WHISPER_CUSTODIAN_4', 'Follow me, please.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000219, 'WHISPER_CUSTODIAN_3', 'It is strange, I know... Most mortals cannot actually comprehend what they see here, as often, what they see is not anchored within their own perception of reality.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000218, 'WHISPER_CUSTODIAN_2', 'We do not know if the Caverns of Time have always been accessible to mortals. Truly, it is impossible to tell as the Timeless One is in perpetual motion, changing our timeways as he sees fit. What you see now may very well not exist tomorrow. You may wake up and have no memory of this place.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000217, 'WHISPER_CUSTODIAN_1', 'Greetings, $N. I will guide you through the cavern. Please try and keep up.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 4);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000216, 'manaforge_control EMOTE_ABORT', 'Emergency shutdown aborted.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000215, 'manaforge_control EMOTE_COMPLETE', 'Emergency shutdown complete.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000214, 'manaforge_control EMOTE_10', 'Emergency shutdown will complete in ten seconds.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000213, 'manaforge_control EMOTE_30', 'Emergency shutdown will complete in thirty seconds.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000212, 'manaforge_control EMOTE_60', 'Emergency shutdown will complete in one minute.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000211, 'manaforge_control EMOTE_START', 'Warning! %s emergency shutdown process initiated by $N. Shutdown will complete in two minutes.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000210, 'anvilward SAY_ANVIL2', 'What manner of trick is this, $R? If you seek to ambush me, I warn you I will not go down quietly!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000209, 'anvilward SAY_ANVIL1', 'Very well. Let''s see what you have to show me, $N.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000208, 'oox SAY_OOX17_AMBUSH_REPLY', 'No one challanges the Wastewander nomads - not even robotic chickens! ATTACK!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000207, 'ashyen_and_keleth SAY_REWARD_BLESS', 'You have my blessing', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000206, 'cluck EMOTE_CLUCK_TEXT2', '%s starts pecking at the feed.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000205, 'cluck EMOTE_H_HELLO', '%s looks at you unexpectadly.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000204, 'cluck EMOTE_A_HELLO', '%s looks up at you quizzically. Maybe you should inspect it?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000203, 'injured_patient SAY_DOC3', 'Sweet, sweet embrace... take me...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000202, 'injured_patient SAY_DOC2', 'HOORAY! I AM SAVED!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000201, 'injured_patient SAY_DOC1', 'I''m saved! Thank you, doctor!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000200, 'SAY_GUARD_SIL_AGGRO3', 'As if we don''t have enough problems, you go and create more!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000199, 'SAY_GUARD_SIL_AGGRO2', 'Please tell me that you didn''t just do what I think you just did. Please tell me that I''m not going to have to hurt you...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000198, 'SAY_GUARD_SIL_AGGRO1', 'Taste blade, mongrel!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000197, 'lady_sylvanas EMOTE_LAMENT_END', '%s kneels down and pick up the amulet.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000196, 'lady_sylvanas SAY_LAMENT_END', 'Belore...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000195, 'floon SAY_FLOON_ATTACK', 'I choose the third option: KILLING YOU!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000194, 'unkor SAY_SUBMIT', 'I give up! Please don''t kill me!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000193, 'stillblade SAY_HEAL', 'Thank you, dear $C, you just saved my life.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000192, 'LUMP_DEFEAT', 'OK, OK! Lump give up!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000191, 'LUMP_SAY1', 'You taste good with maybe a little salt and pepper.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000190, 'LUMP_SAY0', 'In Nagrand, food hunt ogre!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000189, 'converted_sentry SAY_CONVERTED_2', 'Objective acquired. Initiating security routines.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000188, 'converted_sentry SAY_CONVERTED_1', 'Deployment sucessful. Trespassers will be neutralized.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000187, 'faulk SAY_HEAL', 'Thank you, dear $C, you just saved my life.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000186, 'engineer_spark ATTACK_YELL', 'Now I cut you!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000185, 'engineer_spark SAY_EMOTE', '%s puts the shell to his ear.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000184, 'engineer_spark SAY_TEXT', 'Yes Master, all goes along as planned.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000183, 'draenei_survivor SAY_HELP4', 'I don''t know if I can make it, please help me...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000182, 'draenei_survivor SAY_HELP3', 'Ughhh... I hurt. Can you help me?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000181, 'draenei_survivor SAY_HELP2', 'Everything hurts, Please make it stop...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000180, 'draenei_survivor SAY_HELP1', 'Oh, the pain...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000179, 'draenei_survivor SAY_HEAL4', 'Ugh... what is this place? Is that all that''s left of the ship over there?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000178, 'draenei_survivor SAY_HEAL3', '$C You saved me! I owe you a debt that I can never repay. I''ll go see if I can help the others.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000177, 'draenei_survivor SAY_HEAL2', '$C, Where am I? Who are you? Oh no! What happened to the ship?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000176, 'draenei_survivor SAY_HEAL1', 'The last thing I remember is the ship falling and us getting into the pods. I''ll go see how I can help. Thank you!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000175, ' SAY_JUST_EATEN', 'Thank you, mortal.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 11, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000174, 'daranelle SAY_DARANELLE', 'Good $N, you are under the spell''s influence. I must analyze it quickly, then we can talk.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000173, 'nether_drake SAY_NIHIL_INTERRUPT', 'NOOOOooooooo!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000172, 'nether_drake SAY_NIHIL_4', 'No doubt the fools that banished me are long dead. I shall take wing survey my demense. Pray to whatever gods you hold dear that we do not meet again.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000171, 'nether_drake SAY_NIHIL_3', 'Where shall I begin? I cannot bother myself with a worm such as yourself. There is a world to be conquered!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000170, 'nether_drake SAY_NIHIL_2', 'All of Draenor shall quick beneath my feet! I will destroy this world and reshape it in my image!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000169, 'nether_drake SAY_NIHIL_1', 'Muahahahaha! You fool! You''ve released me from my banishment in the interstices between space and time!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000168, 'aquementas AGGRO_YELL_AQUE', 'Who dares awaken Aquementas?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000167, 'doomwalker SAY_DEATH', 'System failure in five, f-o-u-r...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11352, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000166, 'doomwalker SAY_SLAY_3', 'Target exterminated.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11351, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000165, 'doomwalker SAY_SLAY_2', 'Directive accomplished.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11350, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000164, 'doomwalker SAY_SLAY_1', 'Threat level zero.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11349, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000163, 'doomwalker SAY_OVERRUN_2', 'Engage maximum speed.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11348, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000162, 'doomwalker SAY_OVERRUN_1', 'Trajectory locked.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11347, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000161, 'doomwalker SAY_EARTHQUAKE_2', 'Magnitude set. Release.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11346, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000160, 'doomwalker SAY_EARTHQUAKE_1', 'Tectonic disruption commencing.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11345, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000159, 'doomwalker SAY_AGGRO', 'Do not proceed. You will be eliminated!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11344, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000158, 'kazzak SAY_RAND2', 'Impudent whelps, you only delay the inevitable. Where one has fallen, ten shall rise. Such is the will of Kazzak...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000157, 'kazzak SAY_RAND1', 'Invaders, you dangle upon the precipice of oblivion! The Burning Legion comes and with it comes your end.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000156, 'REUSE ME', 'REUSE ME', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000155, 'kazzak SAY_DEATH', 'The Legion... will never... fall.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11340, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000154, 'kazzak SAY_KILL3', 'The universe will be remade.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11339, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000153, 'kazzak SAY_KILL2', 'Contemptible wretch!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11338, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000152, 'kazzak SAY_KILL1', 'Kirel Narak!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11337, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000151, 'kazzak SAY_SURPREME2', 'I''ll rip the flesh from your bones!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11336, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000150, 'kazzak SAY_SURPREME1', 'All life must be eradicated!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11335, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000149, 'kazzak SAY_AGGRO2', 'All mortals will perish!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11334, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000148, 'kazzak SAY_AGGRO1', 'The Legion will conquer all!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11333, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000147, 'kazzak SAY_INTRO', 'I remember well the sting of defeat at the conclusion of the Third War. I have waited far too long for my revenge. Now the shadow of the Legion falls over this world. It is only a matter of time until all of your failed creation... is undone.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 11332, 1);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000146, 'lilatha CAPTAIN_ANSWER', 'Liatha, get someone to look at those injuries. Thank you for bringing her back safely.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000145, 'lilatha SAY_END2', 'Captain Helios, I''ve been rescued from the Amani Catacombs. Reporting for duty, sir!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000144, 'lilatha SAY_END1', 'Thank you for saving my life and bringing me back to safety, $N', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000143, 'lilatha SAY_PROGRESS3', 'There''s Farstrider Enclave now, $C. Not far to go... Look out! Troll ambush!!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000142, 'lilatha SAY_PROGRESS2', 'I can see the light at the end of the tunnel!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000141, 'lilatha SAY_PROGRESS1', '$N, let''s use the antechamber to the right.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000140, 'lilatha SAY_START', 'Let''s go.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000139, 'aeranas SAY_FREE', 'Avruu''s magic is broken! I''m free once again!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000138, 'aeranas SAY_SUMMON', 'Avruu''s magic... it still controls me. You must fight me, mortal. It''s the only way to break the spell!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000137, 'dawnforge COMMANDER_DAWNFORGE_5', 'See to it, Ardonis!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000136, 'dawnforge COMMANDER_DAWNFORGE_4 SAY_ARCANIST_ARDONIS_2', 'Yes, my lord.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000135, 'dawnforge PATHALEON_CULATOR_IMAGE_2_2', 'I am returning to Tempest Keep. See to it that I do not have reason to return!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000134, 'dawnforge PATHALEON_CULATOR_IMAGE_2_1', 'We''ve had too many setbacks along the way: Hellfire Citadel, Fallen Sky Ridge, Firewing Point... Prince Kael''thas will tolerate no further delays. I will tolerate nothing other than complete success!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000133, 'dawnforge PATHALEON_CULATOR_IMAGE_2', 'Duro will be reinforced! Ultris was a complete disaster. I will NOT have that mistake repeated!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000132, 'dawnforge SAY_COMMANDER_DAWNFORGE_3', 'My lord!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000131, 'dawnforge SAY_PATHALEON_CULATOR_IMAGE_1', 'Indeed, it is not a request.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000130, 'dawnforge SAY_COMMANDER_DAWNFORGE_2', 'You will do as ordered. Manaforge Duro has come under heavy attack by mana creatures and the situation is out of control. Failure to comply will not be tolerated!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000129, 'dawnforge SAY_ARCANIST_ARDONIS_1', 'You cannot be serious! We are severely understaffed and can barely keep this manaforge functional!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000128, 'dawnforge SAY_COMMANDER_DAWNFORGE_1', 'We need you to send reinforcements to Manaforge Duro, Ardonis. This is not a request, it''s an order.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000127, 'twiggy SAY_TWIGGY_FLATHEAD_OVER', 'The Affray is over.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000126, 'twiggy SAY_TWIGGY_FLATHEAD_DOWN', 'Challenger is down!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000125, 'twiggy SAY_TWIGGY_FLATHEAD_FRAY', 'You! Enter the fray!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000124, 'twiggy SAY_TWIGGY_FLATHEAD_BEGIN', 'The Affray has begun. $n, get ready to fight!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000123, 'big will SAY_BIG_WILL_READY', 'Ready when you are, $c.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 15, 0, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000122, 'wounded elf SAY_ELF_AGGRO', 'You won''t keep me from getting to Falcon Watch!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000121, 'wounded elf SAY_ELF_COMPLETE', 'Falcon Watch, at last! Now, where''s my... Oh no! My pack, it''s missing! Where has -', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000120, 'wounded elf SAY_ELF_SUMMON2', 'Did you hear something?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000119, 'wounded elf SAY_ELF_RESTING', 'Allow me a moment to rest. The journey taxes what little strength I have.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000118, 'wounded elf SAY_ELF_SUMMON1', 'Over there! They''re following us!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000117, 'wounded elf SAY_ELF_START', 'Thank you for agreeing to help. Now, let''s get out of here $N.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000116, 'magwin EMOTE_HUG', '%s hugs her father.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 2);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000115, 'magwin SAY_END2', 'You can thank $N for getting me back here safely, father.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000114, 'magwin SAY_END1', 'Father! Father! You''re alive!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000113, 'magwin SAY_PROGRESS', 'My poor family. Everything has been destroyed.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000112, 'magwin SAY_AGGRO', 'Help me!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000111, 'magwin SAY_START', 'Our house is this way, through the thicket.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 7, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000110, 'torek SAY_END', 'Go report that the outpost is taken. We will remain here.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000109, 'torek SAY_WIN', 'Silverwing is ours!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000108, 'torek SAY_PREPARE', 'Prepare yourselves. Silverwing is just around the bend.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000107, 'torek SAY_MOVE', 'Ok, Lets move out!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);
REPLACE INTO `script_texts` (entry, comment, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, emote, language, sound, type)
VALUES (-1000106, 'torek SAY_READY', 'Everyone ready?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0);

UPDATE `script_texts` SET comment = 'prisoner ashcrombe SAY_FREE_AS', content_default = 'Follow me and I''ll open the courtyard door for you.', emote = 1, language = 7 WHERE entry = -1033000;

-- Waypoints

REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 1, -8763.9, -2185.21, 141.217, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 2, -8768.76, -2185.94, 141.949, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 3, -8772.1, -2189.18, 141.443, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 4, -8778.78, -2195.66, 140.662, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 5, -8789.74, -2191.46, 141.634, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 6, -8802.37, -2185.99, 141.96, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 7, -8818, -2184.8, 139.153, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 8, -8828.42, -2193.02, 138.973, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 9, -8826.45, -2203.82, 140.293, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 10, -8822.54, -2211.43, 142.884, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 11, -8813.51, -2225.08, 143.327, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 12, -8807.5, -2233.92, 144.441, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 13, -8801.3, -2239.04, 146.476, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 14, -8797.57, -2243.61, 146.594, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 15, -8794.38, -2250.83, 146.723, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 16, -8788.1, -2255.1, 147.309, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 17, -8775.37, -2259.82, 149.18, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 18, -8768.09, -2259.54, 150.52, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 19, -8754.65, -2255.62, 152.253, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 20, -8748.51, -2252.62, 153.098, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 21, -8743.2, -2251.67, 154.038, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 22, -8738.4, -2250.23, 154.028, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 23, -8734.35, -2251.56, 154.363, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 24, -8727.41, -2248.64, 154.919, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 25, -8717.66, -2246.3, 154.926, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 26, -8709.05, -2245.83, 154.767, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 27, -8700.13, -2243.28, 153.872, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 28, -8690.15, -2242.54, 153.733, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 29, -8683.49, -2244.31, 155.356, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 30, -8674.53, -2247.89, 155.574, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 31, -8669.86, -2252.77, 154.854, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 32, -8669.07, -2258.88, 156.424, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 33, -8670.56, -2264.69, 156.978, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 34, -8673.45, -2269.45, 156.007, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 35, -8674.4, -2275.9, 155.747, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 36, -8674.82, -2282.75, 155.496, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 37, -8675.17, -2289.5, 157.049, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 38, -8676.43, -2297.67, 156.701, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 39, -8677.59, -2304.85, 155.917, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 40, -8682.32, -2312.88, 155.928, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 41, -8687.78, -2324.44, 156.024, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 42, -8695.71, -2334.87, 156.06, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 43, -8705.17, -2345.13, 156.021, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 44, -8715.1, -2353.95, 156.188, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 45, -8725.8, -2359.17, 156.253, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 46, -8735.92, -2363.27, 157.151, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 47, -8746.82, -2367.99, 158.13, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 48, -8755.38, -2375.72, 157.271, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 49, -8765.12, -2388.08, 156.092, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 50, -8768.84, -2395.58, 155.926, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 51, -8772.85, -2405.27, 156.418, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 52, -8776.95, -2414.94, 156.388, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 53, -8781.69, -2430.11, 153.264, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 54, -8786.76, -2440.34, 147.849, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 55, -8792.01, -2453.38, 142.746, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 56, -8797.41, -2462.21, 138.171, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 57, -8804.78, -2472.43, 134.192, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 58, -8815.26, -2478.45, 133.079, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 59, -8823.74, -2491.21, 132.911, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 60, -8835.25, -2496.44, 132.057, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 61, -8841.04, -2503.01, 132.199, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 62, -8850.81, -2509.63, 132.865, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 63, -8858.64, -2522.29, 133.852, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 64, -8862.25, -2527.1, 134.279, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 65, -8870.67, -2542.08, 131.044, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 66, -8880.4, -2550.79, 130.505, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 67, -8892.87, -2560.3, 130.558, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 68, -8908.74, -2573.64, 131.616, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 69, -8922.05, -2585.31, 132.446, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 70, -8935.86, -2591.19, 132.446, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 71, -8949.08, -2596.87, 132.537, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 72, -8959.17, -2599.72, 132.507, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 73, -8969.43, -2601.96, 132.471, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 74, -8979.77, -2603.66, 132.39, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 75, -8991.61, -2604.16, 131.196, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 76, -9000.2, -2602.38, 128.954, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 77, -9011.57, -2594.23, 127.435, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 78, -9019.77, -2587.67, 126.618, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 79, -9028.35, -2582.26, 125.787, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 80, -9038.96, -2572.71, 124.748, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 81, -9046.92, -2560.64, 124.447, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 82, -9059.29, -2550.1, 123.756, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 83, -9068.15, -2547.28, 122.965, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 84, -9077.54, -2541.67, 121.17, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 85, -9085.61, -2532.98, 118.85, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 86, -9097.8, -2520.49, 117.401, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 87, -9110.18, -2507.01, 117.098, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 88, -9119.21, -2498.23, 116.627, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 89, -9124.61, -2487.07, 115.972, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 90, -9127.42, -2478.53, 114.843, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 91, -9133.18, -2465.77, 113.029, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 92, -9137.54, -2456.22, 111.051, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 93, -9146.73, -2441.6, 107.979, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 94, -9155.62, -2430.79, 106.251, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 95, -9158.06, -2420.36, 104.838, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 96, -9157.25, -2410.2, 103.858, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 97, -9152.95, -2401.47, 102.679, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 98, -9153.63, -2393.46, 100.63, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 99, -9156.48, -2385.68, 98.2937, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 100, -9161.01, -2379.87, 96.2066, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 101, -9169.08, -2373.14, 93.8832, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 102, -9175.61, -2368.72, 92.5178, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 103, -9187.1, -2360.52, 89.9231, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 104, -9194.27, -2352.89, 87.593, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 105, -9201.07, -2344.88, 85.8516, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 106, -9205.62, -2339.56, 85.0342, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 107, -9212.44, -2331.58, 83.8068, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 108, -9219.26, -2323.6, 82.29, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 109, -9229.98, -2313.25, 79.4506, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 110, -9240.03, -2303.51, 76.7841, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 111, -9249.34, -2298.82, 74.3911, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 112, -9254.95, -2296.68, 72.8144, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 113, -9264.73, -2292.92, 70.0089, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 114, -9272.24, -2293.79, 68.6096, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (349, 115, -9277.03, -2295.98, 68.1135, '', 10000);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (4500, 0, -3125.6, -2885.67, 34.731, '', 2500);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (4500, 1, -3120.26, -2877.83, 34.917, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (4500, 2, -3116.49, -2850.67, 34.869, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (4500, 3, -3093.47, -2819.19, 34.432, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (4500, 4, -3104.73, -2802.02, 33.954, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (4500, 5, -3105.91, -2780.23, 34.469, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (4500, 6, -3116.08, -2757.9, 34.734, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (4500, 7, -3125.23, -2733.96, 33.189, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (4983, 0, -3322.65, -3124.63, 33.842, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (4983, 1, -3326.34, -3126.83, 34.426, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (4983, 2, -3336.98, -3129.61, 30.692, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (4983, 3, -3342.6, -3132.15, 30.422, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (4983, 4, -3355.83, -3140.95, 29.534, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (4983, 5, -3365.83, -3144.28, 35.176, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (4983, 6, -3368.9, -3147.27, 36.091, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (4983, 7, -3369.36, -3169.83, 36.325, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (4983, 8, -3371.44, -3183.91, 33.454, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (4983, 9, -3373.82, -3190.86, 34.717, 'SAY_OGR_SPOT', 5000);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (4983, 10, -3368.53, -3198.21, 34.926, 'SAY_OGR_RET_WHAT', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (4983, 11, -3366.27, -3210.87, 33.733, 'pause', 5000);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (4983, 12, -3368.53, -3198.21, 34.926, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (4983, 13, -3373.82, -3190.86, 34.717, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (4983, 14, -3371.44, -3183.91, 33.454, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (4983, 15, -3369.36, -3169.83, 36.325, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (4983, 16, -3368.9, -3147.27, 36.091, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (4983, 17, -3365.83, -3144.28, 35.176, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (4983, 18, -3355.83, -3140.95, 29.534, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (4983, 19, -3342.6, -3132.15, 30.422, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (4983, 20, -3336.98, -3129.61, 30.692, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (4983, 21, -3326.34, -3126.83, 34.426, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (4983, 22, -3322.65, -3124.63, 33.842, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (5391, 1, -9901.05, -3727.01, 22.1162, '', 5000);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (5391, 2, -9907.75, -3726.14, 23.2665, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (5391, 3, -9930.76, -3706.67, 23.5959, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (5391, 4, -9939.84, -3712.56, 22.1328, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (5391, 5, -9947.29, -3712.15, 21.928, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (5391, 6, -9954.73, -3731.75, 23.0535, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (5391, 7, -9982.74, -3724.2, 22.9399, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (5391, 8, -9998.27, -3717.17, 22.259, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (5391, 9, -10015.7, -3685.44, 20.0259, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (5391, 10, -10026.8, -3637.83, 21.3316, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (5391, 11, -10026.8, -3564.12, 21.9134, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (5391, 12, -10029.8, -3541.82, 22.5275, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (5391, 13, -10061.3, -3462.99, 21.6763, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (5391, 14, -10074, -3439.15, 21.0513, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (5391, 15, -10074, -3439.15, 21.0513, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (5391, 16, -10075.1, -3430.99, 20.8188, '', 15000);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (5391, 17, -10111, -3409.88, 22.4119, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (5644, 1, -339.679, 1752.04, 139.482, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (5644, 2, -328.957, 1734.95, 139.327, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (5644, 3, -350.747, 1731.12, 139.338, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (5644, 4, -365.064, 1739.04, 139.376, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (5644, 5, -371.105, 1746.03, 139.374, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (5644, 6, -383.141, 1738.62, 138.93, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (5644, 7, -390.445, 1733.98, 136.353, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (5644, 8, -401.368, 1726.77, 131.071, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (5644, 9, -416.016, 1721.19, 129.807, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (5644, 10, -437.139, 1709.82, 126.342, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (5644, 11, -459.862, 1687.92, 116.059, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (5644, 12, -460.686, 1679.55, 111.976, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (5644, 13, -461.485, 1670.94, 109.033, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (5644, 14, -468.53, 1645.51, 102.811, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (5644, 15, -474.529, 1615.97, 97.228, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (5644, 16, -474.329, 1590.01, 94.4982, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (12717, 0, 3346.25, 1007.88, 3.59, 'SAY_MUG_START2', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (12717, 1, 3367.39, 1011.51, 3.72, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (12717, 2, 3418.64, 1013.96, 2.905, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (12717, 3, 3426.84, 1015.1, 3.449, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (12717, 4, 3437.03, 1020.79, 2.742, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (12717, 5, 3460.56, 1024.26, 1.353, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (12717, 6, 3479.87, 1037.96, 1.023, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (12717, 7, 3490.53, 1043.35, 3.338, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (12717, 8, 3504.28, 1047.77, 8.205, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (12717, 9, 3510.73, 1049.79, 12.143, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (12717, 10, 3514.41, 1051.17, 13.235, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (12717, 11, 3516.94, 1052.91, 12.918, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (12717, 12, 3523.64, 1056.3, 7.563, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (12717, 13, 3531.94, 1059.86, 6.175, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (12717, 14, 3535.48, 1069.96, 1.697, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (12717, 15, 3546.98, 1093.49, 0.68, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (12717, 16, 3549.73, 1101.88, -1.123, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (12717, 17, 3555.14, 1116.99, -4.326, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (12717, 18, 3571.94, 1132.18, -0.634, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (12717, 19, 3574.28, 1137.58, 3.684, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (12717, 20, 3579.31, 1137.25, 8.205, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (12717, 21, 3590.22, 1143.65, 8.291, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (12717, 22, 3595.97, 1145.83, 6.773, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (12717, 23, 3603.65, 1146.92, 9.763, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (12717, 24, 3607.08, 1146.01, 10.692, 'SAY_MUG_BRAZIER', 5000);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (12717, 25, 3614.52, 1142.63, 10.248, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (12717, 26, 3616.66, 1140.84, 10.682, 'SAY_MUG_PATROL', 3000);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (12717, 27, 3621.08, 1138.11, 10.369, 'SAY_MUG_RETURN', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (12717, 28, 3615.48, 1145.53, 9.614, '', 0);
REPLACE INTO `script_waypoint` (entry, pointid, location_x, location_y, location_z, point_comment, waittime)
VALUES (12717, 29, 3607.19, 1152.72, 8.871, '', 0);

UPDATE `script_waypoint` SET point_comment = 'hold,prepare for wave1' WHERE entry = 6182 AND pointid = 7;
UPDATE `script_waypoint` SET point_comment = 'hold,prepare for wave2' WHERE entry = 6182 AND pointid = 8;
UPDATE `script_waypoint` SET point_comment = 'hold,prepare for wave3' WHERE entry = 6182 AND pointid = 9;
UPDATE `script_waypoint` SET point_comment = 'SAY_PROGRESS_2' WHERE entry = 16295 AND pointid = 11;
UPDATE `script_waypoint` SET point_comment = '' WHERE entry = 16295 AND pointid = 12;
UPDATE `script_waypoint` SET point_comment = 'SAY_PROGRESS_3' WHERE entry = 16295 AND pointid = 18;
UPDATE `script_waypoint` SET point_comment = '' WHERE entry = 16295 AND pointid = 19;
UPDATE `script_waypoint` SET point_comment = 'quest complete' WHERE entry = 16295 AND pointid = 30;
UPDATE `script_waypoint` SET location_x = -10866.7, location_y = -1779.82, location_z = 90.465, point_comment = 'Open door,begin walking', waittime = 0 WHERE entry = 16812 AND pointid = 0;
UPDATE `script_waypoint` SET location_x = -10894.9, location_y = -1775.47, location_z = 90.465 WHERE entry = 16812 AND pointid = 1;
UPDATE `script_waypoint` SET location_x = -10896, location_y = -1782.62, location_z = 90.465, point_comment = 'Begin Speech after this' WHERE entry = 16812 AND pointid = 2;
UPDATE `script_waypoint` SET location_x = -10894.9, location_y = -1775.47, location_z = 90.465, point_comment = 'Resume walking' WHERE entry = 16812 AND pointid = 3;
UPDATE `script_waypoint` SET location_x = -10866.7, location_y = -1779.82, location_z = 90.465, point_comment = 'Resume walking (back to spawn point now) after speech', waittime = 0 WHERE entry = 16812 AND pointid = 4;
UPDATE `script_waypoint` SET location_x = -10866.7, location_y = -1781.03, location_z = 90.465, point_comment = '' WHERE entry = 16812 AND pointid = 5;
UPDATE `script_waypoint` SET point_comment = 'mobs in doorway' WHERE entry = 17876 AND pointid = 71;
UPDATE `script_waypoint` SET point_comment = '' WHERE entry = 17876 AND pointid = 72;

DELETE FROM `script_waypoint` WHERE entry = 16812 AND pointid = 6;
DELETE FROM `script_waypoint` WHERE entry = 16812 AND pointid = 7;
DELETE FROM `script_waypoint` WHERE entry = 16812 AND pointid = 8;
DELETE FROM `script_waypoint` WHERE entry = 16812 AND pointid = 9;

REPLACE INTO `spell_script_target` (entry, targetEntry, type)
VALUES (12134, 5256, 2);
REPLACE INTO `spell_script_target` (entry, targetEntry, type)
VALUES (12134, 5259, 2);
REPLACE INTO `spell_script_target` (entry, targetEntry, type)
VALUES (12134, 5270, 2);
REPLACE INTO `spell_script_target` (entry, targetEntry, type)
VALUES (12134, 5271, 2);
REPLACE INTO `spell_script_target` (entry, targetEntry, type)
VALUES (12134, 5273, 2);
