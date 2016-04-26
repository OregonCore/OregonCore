-- Make Sunspring Villager attackable so we can detect spell hits, and set it to appear dead
-- Fixes Stopping the Spread (Quest 9874)
UPDATE creature_template SET unit_flags=0, dynamicflags=32 WHERE entry=18240;