SET @ENTRY := 17551;
UPDATE `creature_template` SET `AIName`='',`ScriptName`='npc_tavara',`RegenHealth`=0 WHERE `entry`=@ENTRY;
DELETE FROM `creature_ai_scripts` WHERE `creature_id`=@ENTRY;
UPDATE `quest_template` SET `ReqSpellCast1`=0 WHERE `entry`=9586;