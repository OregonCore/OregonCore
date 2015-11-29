
UPDATE `creature` SET `id`= '23219' WHERE `guid` BETWEEN 75829 AND 75899;

INSERT INTO `creature_ai_scripts` (`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`comment`) VALUES 
(23219,11,0,100,1,0,530,0,0,11,32942,2,0,'Blackwind Warp Chaser - Cast Phasing Invisibility On Spawn'),
(23219,4,0,100,0,0,0,0,0,28,32942,0,0,'Blackwind Warp Chaser - Remove Phasing Invisibility On Aggro'),
(23219,0,0,100,0,3000,6000,4000,7000,11,32739,0,0,'Blackwind Warp Chaser - Cast Venomous Bite'),
(23219,0,0,100,0,12000,15000,20000,40000,11,32920,0,0,'Blackwind Warp Chaser - Cast Warp'),
(23219,0,0,100,0,2000,5000,7000,15000,11,37417,1,0,'Blackwind Warp Chaser - Cast Warp Charge'),
(23219,7,0,100,0,0,0,0,0,11,32942,0,0,'Blackwind Warp Chaser - Recast Phasing Invisibility when evading'),
(23219,6,0,100,0,0,0,0,0,33,23438,0,0,'Blackwind Warp Chaser - Hungry Nether Rays quest credit');
