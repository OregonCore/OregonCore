-- Fix related stuff to Scryer's Orb
-- Play cinematic when using the Scryer's Orb 
UPDATE gameobject_template SET type=13, data1=164, data2=16547 WHERE entry = 187578; 
 
-- Event script for Kalecgos to spawn and then come flying to the orb (core script) 
INSERT INTO `event_scripts` (`id`, `delay`, `command`, `datalong`, `datalong2`, `dataint`, `x`, `y`, `z`, `o`) VALUES (16547, 0, 10, 24844, 300000, 0, 209.912, -318.697, 14.005, 1.181); 
 
-- Set InhabitType of Kalecgos (Dragon) to flyable so that his flying movement will be smooth 
UPDATE creature_template SET InhabitType=5 WHERE entry = 24844;