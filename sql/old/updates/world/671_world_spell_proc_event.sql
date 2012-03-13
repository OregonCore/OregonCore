-- Fixed Ruthlessness proc from Envenom
UPDATE spell_proc_event SET SpellFamilyMask = 0x9003E0000 WHERE entry IN (14156, 14160, 14161);