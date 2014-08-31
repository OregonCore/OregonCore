
-- 222566 121.179001 1605.339966 63
-- 222566 120.179001 1606.339966 63

replace into gameobject (id, map, spawnMask, position_x, position_y, position_z, orientation, rotation0, rotation1, rotation2, rotation3, spawntimesecs, animprogress, state)
        values

 -- id   map  spawn    x         y          z        o     r0 r1  r2        r3         st   ap state            
(186728, 568, 1, 120.179001, 1606.339966,      63, 3.15847, 0, 0, 0.999964,-0.00843913, 0,   0, 1), -- Massive Gate
(186858, 568, 1,    337.513,     1395.88, 74.4018, 3.42393, 0, 0, 0.990052,-0.140701,   0,   0, 0), -- Akilzon's Door
(186303, 568, 1, 303.614044,      1111.5, 10.0404, 3.15808, 0, 0, 0.999966,-0.00824184, 0,   0, 1), -- Halaazi's Exit Door
(186304, 568, 1,    376.192,     1056.37, 9.90255, 4.71724, 0, 0, 0.705391,-0.708819,   0,   0, 0), -- Halaazi's Entrance Door
(186305, 568, 1,     113.55,     1025.36, 34.2387, 1.53472, 0, 0, 0.694238, 0.719745,   0, 100, 0), -- Hexlord Entrance
(186306, 568, 1,    112.679,     912.784,  34.161, 1.56741, 0, 0, 0.705909, 0.708302,   0,   0, 1), -- Hexlord Exit Door
(186859, 568, 1,    120.567,      734.09, 45.1114, 4.33339, 0, 0, 0.699642,-0.714493,   0,   0, 0); -- Zuljin Door
-- Move two lynces out of door
REPLACE INTO `creature` VALUES
(89137,24530,568,1,19705,0,366.81,1050.64,9.50835,4.81696,7200,0,0,55000,0,0,0),
(89139,24530,568,1,19705,0,373.406,1050.69,9.50787,4.65203,7200,0,0,55000,0,0,0);

-- Remove this invisible attacking npc
delete from creature where guid = 86479;

-- Harrison Jones
update creature set position_x = 95.3293, position_y = 1653.27, position_z = 42.0219 where guid = 86177;
update creature_template set ScriptName = 'npc_harrison_jones' where entry = 24358;
update creature_addon set Auras = '' where guid = 86177;
