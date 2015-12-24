DELETE FROM spell_scripts WHERE id = 35372;
DELETE FROM event_scripts WHERE id = 10808;
UPDATE creature_ai_scripts SET action1_param1 = 0, action1_param2 = 32942 WHERE id = 19228125 AND event_type = 4;
UPDATE creature SET spawndist = 0 WHERE id = 23472 AND MovementType = 0;