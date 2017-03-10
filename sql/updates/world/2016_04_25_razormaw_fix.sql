-- Add event to trigger Razormaw spawn
DELETE FROM event_scripts WHERE id=11087;
INSERT INTO event_scripts (id, delay, command, datalong, datalong2, dataint, x, y, z, o) VALUES (11087, 3, 10, 17592, 180000, 0, -1238.71, -12447.4, 95.6189, 6.25233);

-- Make Razormaw attackable by the player
UPDATE creature_template SET unit_flags=0 WHERE entry=17592;
