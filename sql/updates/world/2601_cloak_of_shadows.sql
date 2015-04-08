-- cloak of shadows should not cancel flare
DELETE FROM `spell_linked_spell` WHERE  `spell_trigger`=31224 AND `spell_effect`=-1543 AND `type`=2;
