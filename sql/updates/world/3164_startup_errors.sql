-- Assign QUESTGIVER npc flag to Earthen Ring Guide
UPDATE creature_template SET npcflag = 2 WHERE entry = 25324;

-- Fix invalid TextRange
UPDATE creature_text SET TextRange = 0 WHERE TextRange = 15;

-- Use correct TBC set sheathe emote
UPDATE smart_scripts SET action_param1 = 133 WHERE entryorguid = 6182 AND id = 19;
UPDATE smart_scripts SET action_param1 = 133 WHERE entryorguid = 618200 AND id = 2;

-- Use correct spell for submerge
UPDATE smart_scripts SET action_param1 = 29147 WHERE entryorguid = 16844 AND id = 9;
UPDATE smart_scripts SET action_param1 = 29147 WHERE entryorguid = 16844 AND id = 12;
UPDATE smart_scripts SET action_param1 = 29147 WHERE entryorguid = 16857 AND id = 9;
UPDATE smart_scripts SET action_param1 = 29147 WHERE entryorguid = 16857 AND id = 12;