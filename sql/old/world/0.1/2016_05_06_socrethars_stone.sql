-- First and second half of Socrethar's Stone now requires quest to drop
UPDATE creature_loot_template SET QuestRequired=1 WHERE Item IN (29624,29625);
