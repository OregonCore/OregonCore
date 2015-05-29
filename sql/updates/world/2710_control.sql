-- Send area explored on quest accept. Fixes quest Control
DELETE FROM quest_start_scripts WHERE id = 10162;
INSERT INTO quest_start_scripts VALUES
(9595, 0, 7, 9595, 0, 0, 0, 0, 0, 0);

-- Assign startscript to quest
UPDATE quest_template SET StartScript=9595 WHERE entry = 17481;
