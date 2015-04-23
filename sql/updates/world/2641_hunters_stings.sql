-- prevent stacking of Sting spells since there's
-- "Only one Sting per Hunter can be active on the target at a time"

INSERT INTO spell_group (id, spell_id)
VALUES
-- Wyven Sting
(1021,19386),
(1021,24131),
(1021,24132),
(1021,24133),
(1021,24134),
(1021,24135),
(1021,26748),
(1021,27068),
(1021,27069),

-- Serpent Sting
(1021,1978),
(1021,13549),
(1021,13550),
(1021,13551),
(1021,13552),
(1021,13553),
(1021,13554),
(1021,13555),
(1021,25295),
(1021,27016),
(1021,31975),
(1021,35511),
(1021,36984),
(1021,38859),
(1021,38914),
(1021,39182),

-- Scorpid Sting
(1021,3043),

-- Viper Sting

(1021,3034),
(1021,14279),
(1021,14280),
(1021,27018),
(1021,31407),
(1021,37551),
(1021,39413);

INSERT INTO spell_group_stack_rules (group_id, stack_rule) VALUES (1021, 2);
