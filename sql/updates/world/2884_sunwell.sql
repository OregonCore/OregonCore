/* Template Sinister Reflection */
UPDATE creature_template SET minlevel = 70, maxlevel = 70, minmana = 78875, maxmana = 78875, mechanic_immune_mask = 8461649 WHERE entry = 25708;

/* Template ANVEENA */
UPDATE creature_template SET InhabitType = 5 WHERE entry = 26046;

/* Template kalecgos */
UPDATE creature_template SET InhabitType = 5 WHERE entry = 25319;

/* Melee damage for Sinister Reflection */
UPDATE `creature_template` SET 
    `mindmg` = 8000,
    `maxdmg` = 13000,
    `attackpower` = ROUND((`mindmg` + `maxdmg`) / 4 * 7),
    `mindmg` = ROUND(`mindmg` - `attackpower` / 7),
    `maxdmg` = ROUND(`maxdmg` - `attackpower` / 7)
  WHERE `entry` = 25708;
  
/* Ranged damage for Sinister Reflection */
UPDATE `creature_template` SET
    `minrangedmg` = 2750,
    `maxrangedmg` = 7750,
    `rangedattackpower` = 36750
  WHERE `entry` = 25708;