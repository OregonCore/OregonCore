delete from creature_ai_scripts where creature_id IN (14465, 14751, 14466, 14752);
update creature_template set ScriptName = 'EventAI' where entry IN (14465, 14751, 14466, 14752);
insert into creature_ai_scripts
(creature_id, event_type, event_chance, event_flags, action1_type, action1_param1, action1_param2, action1_param3, `comment`)
    VALUES
(14465, 11, 100, 0, 11, 23033, 0, 2, 'Alliance Battle Standard'),
(14466, 11, 100, 0, 11, 23036, 0, 2, 'Horde Battle Standard'),
(14751, 11, 100, 0, 11, 23006, 0, 2, 'FrostWolf Battle Standard'),
(14752, 11, 100, 0, 11, 22998, 0, 2, 'Stormpike Battle Standard');
