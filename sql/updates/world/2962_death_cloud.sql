-- death cloud script
UPDATE creature_template SET scriptname = "trigger_death_cloud", flags_extra = 129 WHERE entry = 25703;

-- felmyst
UPDATE creature_template SET InhabitType = 5 WHERE entry = 25038;

-- madrigosa
UPDATE creature_template SET InhabitType = 5 WHERE entry = 25160;