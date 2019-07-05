UPDATE quest_template SET ReqCreatureOrGOId1 = 6746 WHERE entry = 8360;
UPDATE quest_template SET ReqCreatureOrGOId1 = 6929 WHERE entry = 8359;
UPDATE quest_template SET ReqCreatureOrGOId1 = 11814 WHERE entry = 8358;
UPDATE quest_template SET ReqCreatureOrGOId1 = 6740 WHERE entry = 8356;
UPDATE quest_template SET ReqCreatureOrGOId1 = 6826 WHERE entry = 8355;
UPDATE quest_template SET ReqCreatureOrGOId1 = 6741 WHERE entry = 8354;
UPDATE quest_template SET ReqCreatureOrGOId1 = 5111 WHERE entry = 8353;

DELETE FROM creature_ai_scripts WHERE creature_id IN (SELECT entry FROM creature_template WHERE AIName = 'SmartAI');

DELETE FROM creature_ai_texts WHERE entry = -1019;