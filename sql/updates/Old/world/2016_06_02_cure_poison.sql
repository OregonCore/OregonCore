-- Remove priest version of Cure Poison from Shaman trainer
DELETE FROM npc_trainer WHERE spell = 528 AND entry = 13417;