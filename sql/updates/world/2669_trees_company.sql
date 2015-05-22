-- Correctly set requiredNPC for tree's company quest
UPDATE quest_template SET ReqCreatureOrGOId1 = 17243, ReqCreatureOrGOCount1 = 1 WHERE entry = 9531;