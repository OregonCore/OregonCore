-- Fix kill credit for quest "The battle must go on" and "The battle for the sun reach armory"
UPDATE creature_template SET KillCredit1 = 25068
WHERE entry IN
(25001, -- Flamewalker
 25002, -- Hellion
 24999);-- Irespeaker
 
 -- Delete duplicate spawn of Emissary of Hate
 DELETE FROM creature WHERE guid = 1189311 AND id = 25003;