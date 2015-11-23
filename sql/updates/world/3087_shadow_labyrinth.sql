-- Update fix for un-spawned gameobjects and Murmur's broken emote(Sonic Boom) 

UPDATE `gameobject` SET `spawnMask` = '3' WHERE `id` = '182940'; -- Soul Device
UPDATE `gameobject` SET `spawnMask` = '3' WHERE `id` = '183296'; -- Refectory Door
UPDATE `gameobject` SET `spawnMask` = '3' WHERE `id` = '183036'; -- The Codex of Blood
UPDATE `gameobject` SET `spawnMask` = '3' WHERE `id` = '183295'; -- Screaming Hall Door
UPDATE `gameobject` SET `spawnMask` = '3' WHERE `id` = '182196'; -- Arcane Container (Kara key quest)

-- Update fix for Murmur's broken emote

UPDATE `script_texts` SET `content_default` = '%s draws energy from the air.' WHERE `entry` = '-1555036';
