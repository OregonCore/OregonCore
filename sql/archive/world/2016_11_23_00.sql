-- Tainted Earthgrab Totem 18176,19897 
UPDATE creature_template SET spell1 = 20654 WHERE entry = 18176; 
UPDATE creature_template SET spell1 = 20654 WHERE entry = 19897; 

-- Tainted Stoneskin Totem 18177,19900 
UPDATE creature_template SET spell1 = 31986 WHERE entry = 18177; 
UPDATE creature_template SET minhealth='5', maxhealth='5', spell1 = 31986 WHERE entry = 19900; 

-- Corrupted Nova Totem 18179,19899 
UPDATE creature_template SET spell1 = 33132 WHERE entry = 18176; 
UPDATE creature_template SET spell1 = 33132 WHERE entry = 19897; 

-- Mennu's Healing Ward 20208,22322 
UPDATE creature_template SET heroic_entry = 22322, spell1 = 34977 WHERE entry = 20208; 
UPDATE creature_template SET spell1 = 38800 WHERE entry = 22322;