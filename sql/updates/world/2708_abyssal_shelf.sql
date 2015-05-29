-- Send area explored on quest accept. Fixes quest The Abyssal Shelf
DELETE FROM quest_start_scripts WHERE id = 10162;
INSERT INTO quest_start_scripts VALUES
(10162, 0, 7, 10162, 0, 0, 0, 0, 0, 0);

-- Assign startscript to quest
UPDATE quest_template SET StartScript=10162 WHERE entry = 10162;
