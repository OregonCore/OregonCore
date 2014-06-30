-- Sunwell Plateu --
-- Credits go to Acherus, LordUsagi, Artemis, HellGround Team

-- Felmyst waypoints
REPLACE INTO waypoint_data VALUES
('34000', '1', '1475.33', '672.628', '64.5133', '0', '1', '1', '0', '100', '0'),
('34000', '2', '1456.34', '695.644', '66.7257', '0', '1', '1', '0', '100', '0'),
('34000', '3', '1425.8', '601.801', '68.7822', '0', '1', '1', '0', '100', '0'),
('34000', '4', '1502.26', '533.484', '70.3252', '0', '1', '1', '0', '100', '0'),
('34000', '5', '1531.75', '545.337', '64.7583', '0', '1', '1', '0', '100', '0'),
('34000', '6', '1457.36', '604.974', '20.7713', '0', '1', '1', '0', '100', '0');

-- Correct spawn for Madrigosa
DELETE FROM creature WHERE guid = 54812 AND id=24895;
REPLACE INTO creature VALUES
(54812,25160,580,1,0,0,1466.53,644.949,20.2059,4.93474,604800,0,0,490000,3387,0,0);

-- Brutallus and Madrigosa WPs
DELETE FROM  creature_template_addon WHERE entry = 24895;
UPDATE creature SET position_x = 1466.53 WHERE id = 24895;
UPDATE creature SET position_y = 644.949 WHERE id = 24895;
UPDATE creature SET position_z = 20.2059 WHERE id = 24895;
REPLACE INTO waypoint_data VALUES
('30000', '1', '1495.77', '573.492', '23.9099', '0', '0', '1', '0', '100', '0'),
('30000', '2', '1476.75', '611.606', '23.695', '0', '0', '1', '0', '100', '0'),
('31000', '3', '1466.53', '644.949', '20.2059', '0', '0', '1', '0', '100', '0'),
('31000', '2', '1466.64', '644.949', '54.398', '0', '7000', '1', '0', '100', '0'),
('32000', '1', '1458.59', '629.443', '20.573', '0', '0', '1', '0', '100', '0'),
('31000', '1', '1465.81', '678.581', '54.398', '0', '0', '1', '0', '100', '0'),
('33000', '1', '1480.59', '606.804', '20.018', '0', '0', '1', '0', '100', '0');

-- Eredar Twins Encounter --
-- Equipment for Lady Sacrolash
DELETE FROM creature_equip_template WHERE entry=25165;
REPLACE INTO creature_equip_template VALUES
(25165,18840,18840,0);
UPDATE creature SET equipment_id = 25165 WHERE id = 25165; 

-- M'uru Encounter -- 
-- Positions for M'uru summon spawns
-- These spells are bugged currently, but in the future this will be needed.
DELETE FROM spell_target_position WHERE id IN (46037, 46038, 46039, 46040);
REPLACE INTO spell_target_position VALUES
(46037, 580, 1842.91, 599.93, 71.23, 2.44), -- Berserker outside
(46038, 580, 1847.93, 600.30, 71.19, 2.57), -- Mage outside
(46039, 580, 1780.16, 666.83, 71.19, 5.21), -- Mage inside
(46040, 580, 1779.97, 660.64, 71.19, 5.28); -- Berserker inside


DELETE FROM `spell_script_target` WHERE `entry` IN (45666, 46609, 46610, 46637, 46650, 46652, 46638, 45388, 45389, 45714, 46707, 46818, 44885, 46350, 45005);
REPLACE INTO `spell_script_target` VALUES
(45666,1,25315), -- Arcane bolt @ Kil'Jaeden
(45388,1,25038), -- Demonic vapor @ Felmyst
(45389,1,25265),
(45714,1,25038),
(46609,0,19871),
(46637,1,19871), -- Open ice barrier cosmetic
(46650,1,23472), -- Open brutallus backdoor cosmetic
(46707,1,25653), -- Destroy all drakes
(46818,1,26262), -- Call entropius
(44885,1,25160), -- Summon felmyst visual
(46350,1,25160), -- Summon felmyst visual
(46652,0,188075), -- Open flame door effect
(46638,0,188119), -- Open ice barrier effect
(46610,0,188119); -- Freeze @ Ice Barrier
