--
-- Listed entries will now properly self-cast spells on death.
-- Also corrected some unit names in the comment field.
--

-- Cursed Mage - Cast Tainted Mind on Death --

UPDATE `world`.`creature_ai_scripts` SET `action1_param2`=0 WHERE `id`=852410;

-- Young Sporebat - Cast Energizing Spores on Death --

UPDATE `world`.`creature_ai_scripts` SET `action1_param2`=0 WHERE  `id`=2038701;
UPDATE `world`.`creature_ai_scripts` SET `comment`='Young Sporebat - Cast Energizing Spores on Death' WHERE  `id`=2038701;

-- Sporebat - Cast Energizing Spores on Death -- 

UPDATE `world`.`creature_ai_scripts` SET `action1_param2`=0 WHERE `id`=1812801;
UPDATE `world`.`creature_ai_scripts` SET `comment`='Sporebat - Cast Energizing Spores on Death' WHERE  `id`=1812801;
UPDATE `world`.`creature_ai_scripts` SET `comment`='Sporebat - Cast Spore Cloud' WHERE  `id`=1812802;

-- Greater Sporebat - Cast Energizing Spores on Death --

UPDATE `world`.`creature_ai_scripts` SET `action1_param2`=0 WHERE `id`=1812901;
UPDATE `world`.`creature_ai_scripts` SET `comment`='Greater Sporebat - Cast Energizing Spores on Death' WHERE  `id`=1812901;
UPDATE `world`.`creature_ai_scripts` SET `comment`='Greater Sporebat - Cast Spore Cloud' WHERE  `id`=1812902;

-- Sporewing - Cast Energizing Spores on Death --

UPDATE `world`.`creature_ai_scripts` SET `action1_param2`=0 WHERE  `id`=1828001;


-- Correct other incorrect named units in comment field --

UPDATE `world`.`creature_template` SET `name`='Toxic Sporebat' WHERE  `entry`=22140;
UPDATE `world`.`creature_template` SET `name`='Sporeggar Sporebat' WHERE  `entry`=22085;
