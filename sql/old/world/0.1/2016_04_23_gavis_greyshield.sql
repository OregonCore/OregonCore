-- Remove Gavis Greyshield AI from DB
DELETE FROM creature_ai_scripts WHERE creature_id=23941;
DELETE FROM creature_ai_texts WHERE comment LIKE '23941';

-- Setup Gavis Greyshield to use AI from dustwallow_marsh.cpp
UPDATE creature_template SET ScriptName='npc_gavis_greyshield', AIName='' WHERE entry=23941;
UPDATE creature SET position_x=-4030.5747, position_y=-4979.0913, position_z=8.1341, orientation=5.3607 WHERE guid=30775;
INSERT INTO script_texts (entry, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, sound, type, language, emote, comment) VALUES (-1910267, 'I surrender! Stay your blade!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 5, 'Gavis Greyshield Say 1');
INSERT INTO script_texts (entry, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, sound, type, language, emote, comment) VALUES (-1910268, 'I will go with you. Just spare my men. They\'re good lads who\'ve only made a mistake...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 1, 'Gavis Greyshield Say 2');