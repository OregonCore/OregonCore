-- Remove double spawn of John Turner
DELETE FROM creature WHERE guid = '1200242';

-- Remove creature drop loot, must be pickpocketing
DELETE FROM creature_loot_template WHERE item = 8072;

-- Remove glowing sanctified crystal from creature loot, must be added by spell then object
DELETE FROM creature_loot_template WHERE item = 23442;

-- Remove sethirs  book from drop template, it belongs in pickpocketing (already exists)
DELETE FROM creature_loot_template WHERE item = 7737;

-- Fix sethirs location
UPDATE creature SET position_x = '11077.9', position_y = '1902.37', position_z = '1342.63', orientation = '2.18262' WHERE guid = '2499';

-- This particular mobs drops item 31373 not 31372
DELETE FROM creature_loot_template WHERE entry='21478' AND item='31372';

-- QuestChance requires - until conditions are merged in
INSERT INTO creature_loot_template (`entry`,`item`,`ChanceOrQuestChance`,`groupid`,`mincountOrRef`,`maxcount`,`lootcondition`,`condition_value1`,`condition_value2`) values (21477,31372,-100,0,1,1,0,0,0);
INSERT INTO creature_loot_template (`entry`,`item`,`ChanceOrQuestChance`,`groupid`,`mincountOrRef`,`maxcount`,`lootcondition`,`condition_value1`,`condition_value2`) values (21478,31372,-100,0,1,1,0,0,0);

-- Set this back to what it actually requires
UPDATE quest_template
SET ReqCreatureOrGOId1='22197', ReqCreatureOrGOId1='15'
WHERE entry='10836';

-- Nais is spawned from spell
DELETE FROM creature WHERE guid = '1200310';

-- Goblin sponsorship fix, nothing was wrong with this quest
UPDATE creature SET position_x = '-11949.5', position_y = '-500.392', position_z = '30.7014', orientation = '5.06145' WHERE guid = '2208';
DELETE FROM creature_loot_template WHERE entry='4723' AND item='5851';
INSERT INTO creature_loot_template (`entry`,`item`,`ChanceOrQuestChance`,`groupid`,`mincountOrRef`,`maxcount`,`lootcondition`,`condition_value1`,`condition_value2`)
 values (4723,5852,-100,0,1,1,0,0,0);


-- Bleeding hollow tormentor has eventai
UPDATE creature_template
SET AIName='EventAI'
WHERE entry='19424';

-- According to wowhead comments from patches belonging to TBC this rewards NOTHING and for sure does have a previous quest
-- Horde
UPDATE quest_template
SET RewRepFaction1='0', RewRepValue1='0', PrevQuestId='10162'
WHERE entry='10347';
-- Alliance
UPDATE quest_template
SET RewRepFaction1='0', RewRepValue1='0'
WHERE entry='10346';

-- Incorrect use of GUIDs, mobs should be flying not on ground. No sniff data.
DELETE FROM creature WHERE guid = '1199985';
DELETE FROM creature WHERE guid = '1199986';
DELETE FROM creature WHERE guid = '1199987';
DELETE FROM creature WHERE guid = '1199988';
DELETE FROM creature WHERE guid = '1199989';
DELETE FROM creature WHERE guid = '1199990';
DELETE FROM creature WHERE guid = '1199991';
DELETE FROM creature WHERE guid = '1199992';
DELETE FROM creature WHERE guid = '1199993';
DELETE FROM creature WHERE guid = '1199994';
DELETE FROM creature WHERE guid = '1199995';
DELETE FROM creature WHERE guid = '1199996';
DELETE FROM creature WHERE guid = '1199997';
DELETE FROM creature WHERE guid = '1199998';
DELETE FROM creature WHERE guid = '1199999';
DELETE FROM creature WHERE guid = '1200000';
DELETE FROM creature WHERE guid = '1200001';
DELETE FROM creature WHERE guid = '1200002';
DELETE FROM creature WHERE guid = '1200003';
DELETE FROM creature WHERE guid = '1200004';
DELETE FROM creature WHERE guid = '1200005';
DELETE FROM creature WHERE guid = '1200006';
DELETE FROM creature WHERE guid = '1200007';
DELETE FROM creature WHERE guid = '1200008';
DELETE FROM creature WHERE guid = '1200009';
DELETE FROM creature WHERE guid = '1200010';
DELETE FROM creature WHERE guid = '1200011';
DELETE FROM creature WHERE guid = '1200012';
DELETE FROM creature WHERE guid = '1200013';
DELETE FROM creature WHERE guid = '1200014';
DELETE FROM creature WHERE guid = '1200015';
DELETE FROM creature WHERE guid = '1200016';
DELETE FROM creature WHERE guid = '1200017';
DELETE FROM creature WHERE guid = '1200018';
DELETE FROM creature WHERE guid = '1200019';
DELETE FROM creature WHERE guid = '1200020';
DELETE FROM creature WHERE guid = '1200021';
DELETE FROM creature WHERE guid = '1200022';
DELETE FROM creature WHERE guid = '1200023';
DELETE FROM creature WHERE guid = '1200024';
DELETE FROM creature WHERE guid = '1200025';
DELETE FROM creature WHERE guid = '1200026';
DELETE FROM creature WHERE guid = '1200027';
DELETE FROM creature WHERE guid = '1200028';
DELETE FROM creature WHERE guid = '1200029';
DELETE FROM creature WHERE guid = '1200030';
DELETE FROM creature WHERE guid = '1200031';
DELETE FROM creature WHERE guid = '1200032';
DELETE FROM creature WHERE guid = '1200033';
DELETE FROM creature WHERE guid = '1200034';
DELETE FROM creature WHERE guid = '1200035';
DELETE FROM creature WHERE guid = '1200036';
DELETE FROM creature WHERE guid = '1200037';
DELETE FROM creature WHERE guid = '1200038';
DELETE FROM creature WHERE guid = '1200039';
DELETE FROM creature WHERE guid = '1200040';
DELETE FROM creature WHERE guid = '1200041';
DELETE FROM creature WHERE guid = '1200042';
DELETE FROM creature WHERE guid = '1200043';
DELETE FROM creature WHERE guid = '1200044';
DELETE FROM creature WHERE guid = '1200045';
DELETE FROM creature WHERE guid = '1200046';
DELETE FROM creature WHERE guid = '1200047';
DELETE FROM creature WHERE guid = '1200048';
DELETE FROM creature WHERE guid = '1200049';
DELETE FROM creature WHERE guid = '1200051';
DELETE FROM creature WHERE guid = '1200052';
DELETE FROM creature WHERE guid = '1200053';
DELETE FROM creature WHERE guid = '1200054';
DELETE FROM creature WHERE guid = '1200055';
DELETE FROM creature WHERE guid = '1200056';
DELETE FROM creature WHERE guid = '1200057';

-- Fix name targeting for script_text
UPDATE script_texts SET content_default = 'Vengeance will not be yours, %N!' WHERE entry = '-1900130';

-- This object is already spawned, incorect guid use
DELETE FROM gameobject WHERE guid = '20000001';

-- This entry is Fing, nothing related to kroshius infernal core quest
UPDATE creature_template SET faction_A='18', faction_H='18' WHERE entry='14446';

-- Creature is supposed to become unshielded when using the quest item, no need for permanent spawn
DELETE FROM creature WHERE guid = '1200303';
