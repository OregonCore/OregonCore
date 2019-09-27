-- Remove loot from:
-- Rotting Worm (10925)
-- Remorseful Highborne (10684)
-- Summoned Zombie (10698)
-- Void Critter (17887)
-- Flesh Eating Worm (2462)
-- Succubus Minion (10928)
-- Spiteful Phantom (10388)
-- Wrathful Phantom (10389)
DELETE FROM creature_loot_template WHERE Entry in (10925,10684,10698,17887,2462,10928,10388,10389);
UPDATE creature_template SET mingold=0,maxgold=0 WHERE Entry in (10925,10684,10698,17887,2462,10928,10388,10389);
