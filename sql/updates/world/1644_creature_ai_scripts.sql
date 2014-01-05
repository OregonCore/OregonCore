--
-- Listed entries will now properly self-cast spells on death
--

-- Cursed Mage - Cast Tainted Mind on Death --

UPDATE `world`.`creature_ai_scripts` SET `action1_param2`=0 WHERE `id`=852410;

-- Spore Bat - Cast Energizing Spores on Death -- 

UPDATE `world`.`creature_ai_scripts` SET `action1_param2`=0 WHERE `id`=1812801;

-- Greater Spore Bat - Cast Energizing Spores on Death --

UPDATE `world`.`creature_ai_scripts` SET `action1_param2`=0 WHERE `id`=1812901;

