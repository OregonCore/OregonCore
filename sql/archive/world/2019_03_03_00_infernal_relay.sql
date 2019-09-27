DELETE FROM creature WHERE guid = 63370;
DELETE FROM creature_addon WHERE guid = 63370;
DELETE FROM game_event_creature WHERE guid = 63370;
DELETE from game_event_model_equip WHERE guid = 63370;

UPDATE creature SET position_x = '-10917.661133', position_y = '-2080.856934', position_z = '341.372040', orientation = '2.030259' WHERE guid = '63371';