INSERT INTO `script_texts` (`entry`,`content_default`,`content_loc1`,`content_loc2`,`content_loc3`,`content_loc4`,`content_loc5`,`content_loc6`,`content_loc7`,`content_loc8`,`sound`,`type`,`language`,`emote`,`comment`) VALUES
 (-1900100,'It\'s time. The rite of exorcism will now commence...',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_BARADA1'),
 (-1900101,'Prepare yourself.Do not allow the ritual to be interrupted or we may lose our patient...',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_BARADA2'),
 (-1900104,'In the name of the Light! It is Light that commands you! It is Light that flung you to be depths of darkness!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_BARADA3'),
 (-1900105,'You cannot deceive me, demon! Your strenght wanes just as my faith bolsters!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_BARADA4'),
 (-1900106,'I... must not...falter!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_BARADA5'),
 (-1900107,'The power of Light compels you! Back to your pit!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_BARADA6'),
 (-1900108,'In the name of Light! It is Light that commands you! It is Light that flung you to the depths of darkness!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_BARADA7'),
 (-1900109,'Back! I cast you back... corrupter of faith! Author of pain! Do not return, or suffer the same fate as you did here today!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_BARADA8'),
 (-1900110,'Keep away.The fool is mine.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_COLONEL1'),
 (-1900111,'You will not succeed mortal! This shell will lie decrepit, blistered and bleeding before I am donewith It. And its spirit will be long cast into darkness.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_COLONEL2'),
 (-1900112,'All is lost, Anchorite! Abandon what hope remains.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_COLONEL3'),
 (-1900113,'This is fruitless, draenel! You and your little helper cannot wrest control of this pathetic human. He is mine!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_COLONEL4'),
 (-1900114,'I see your ancestors, Anchorite! They writhe and scream in the darkness... they are with us!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_COLONEL5'),
 (-1900115,'All is lost, Anchorite! Abandon what hope remains.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_COLONEL6'),
 (-1900116,'I will tear your soul into morsels and slow roast them over demon fire!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_COLONEL7'),
 (-1900117,'I see your ancestors, Anchorite! They writhe and scream in the darkness... they are with us!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,0,'SAY_COLONEL8');

UPDATE `creature_template` SET `unit_flags` = '0' WHERE entry = '22507';
UPDATE `creature_template` SET `minhealth` = '916' WHERE entry = '22507';
UPDATE `creature_template` SET `maxhealth` = '916' WHERE entry = '22507';
UPDATE `creature_template` SET `mindmg` = '176' WHERE entry = '22506';
UPDATE `creature_template` SET `maxdmg` = '176' WHERE entry = '22506';
UPDATE `creature_template` SET `attackpower` = '1235' WHERE entry = '22506';
UPDATE `creature_template` SET `faction_A` = '14' WHERE entry = '22506';
UPDATE `creature_template` SET `faction_H` = '14' WHERE entry = '22506';
UPDATE `creature_template` SET `flags_extra` = '128' WHERE entry = '22505';
UPDATE `creature_template` SET `AIName` = 'EventAI' WHERE entry = '22505';
UPDATE `creature` SET `modelid`  = '14533' WHERE guid = '78786';
UPDATE `quest_template` SET `ReqSpellCast1`  = '0' WHERE entry = '10935';
UPDATE `quest_template` SET `ReqCreatureOrGOID1`  = '0' WHERE entry = '10935';
UPDATE `quest_template` SET `ReqCreatureOrGOCount1`  = '0' WHERE entry = '10935';
UPDATE `quest_template` SET `QuestFlags` = '128' WHERE entry = '10935';
UPDATE `quest_template` SET `SpecialFlags`  = '2' WHERE entry = '10935';
UPDATE `quest_template` SET `ObjectiveText1`  = '' WHERE entry = '10935';
UPDATE `quest_template` SET `EndText`  = 'Colonel Jules Saved' WHERE entry = '10935';
UPDATE `creature_template` SET `ScriptName` = 'npc_anchorite_barada' WHERE entry = '22431';
UPDATE `creature_template` SET `ScriptName` = 'npc_darkness_released' WHERE entry = '22507';
UPDATE `creature_template` SET `ScriptName` = 'npc_foul_purge' WHERE entry = '22506';

INSERT INTO creature_ai_scripts VALUES ('2250501', '22505', '1', '0', '100', '0', '1000', '1000', '0', '0', '11', '39300', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'Slimer Bunny - Cast Aura on Spawn');
INSERT INTO creature_ai_scripts VALUES ('2250502', '22505', '1', '0', '100', '0', '1000', '1000', '0', '0', '12', '22506', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'Slimer Bunny - Summon Foul Purge on Spawn');