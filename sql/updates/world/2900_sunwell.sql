/* Portal effect */
UPDATE creature_template SET scale = 2, modelid_A = 23377, modelid_H = 23377, InhabitType = 4 where entry = 25770;

/* sentinal summoner */
UPDATE creature_template SET modelid_A = 23377, modelid_H = 23377, flags_extra = 130, ScriptName = 'npc_sentinal_summoner' WHERE entry = 25782;
DELETE FROM creature WHERE id = 25782;
INSERT INTO `creature` (id, map, spawnMask, modelid, equipment_id, position_x, position_y, position_z, orientation, spawntimesecs, spawndist, currentwaypoint, curhealth, curmana, MovementType) VALUES
(25782,580,1,0,0,1800.4,604.768,71.1448,0.115494,25,0,0,1,0,0),
(25782,580,1,0,0,1804.41,597.043,71.2095,1.23783,25,0,0,1,0,0),
(25782,580,1,0,0,1824.05,648.917,70.5602,4.48162,25,0,0,1,0,0),
(25782,580,1,0,0,1800.52,653.216,71.2049,4.11431,25,0,0,1,0,0),
(25782,580,1,0,0,1784.61,625.334,71.2095,1.09681,25,0,0,1,0,0),
(25782,580,1,0,0,1836.51,649.186,71.2409,4.01431,25,0,0,1,0,0),
(25782,580,1,0,0,1781.31,635.244,71.2056,6.16263,25,0,0,1,0,0),
(25782,580,1,0,0,1844.83,623.206,71.2322,3.11895,25,0,0,1,0,0),
(25782,580,1,0,0,1823.53,595.473,71.2394,1.72242,25,0,0,1,0,0),
(25782,580,1,0,0,1842.28,638.363,71.2329,3.66323,25,0,0,1,0,0);