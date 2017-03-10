-- Assign areatrigger script to haramad teleport
DELETE FROM areatrigger_scripts WHERE entry=4479;
INSERT INTO areatrigger_scripts VALUES
(4479, 'at_haramad_teleport');