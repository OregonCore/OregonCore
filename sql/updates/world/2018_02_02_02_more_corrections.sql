UPDATE `creature_template` SET `maxmana`='8973' WHERE `entry`=18693;
UPDATE `creature_template` SET `minmana`='2933' WHERE `entry`=21385;
UPDATE `creature_template` SET `maxmana`='3309' WHERE `entry`=22254;
UPDATE `creature_template` SET `maxmana`='4868' WHERE `entry`=10426;

# Creature was removed in patch 2.1.0
# http://www.wowhead.com/npc=22283/eredar-stormbringer#comments:id=152409
# https://wow.gamepedia.com/Eredar_Stormbringer

DELETE FROM `creature_template` WHERE `entry`=22283;
DELETE FROM `creature` WHERE `guid`=78324;
DELETE FROM `creature_loot_template` WHERE `entry`=22283;
DELETE FROM `creature_template_addon` WHERE  `entry`=22283;

# Emote 327 does not exist
UPDATE `creature_text` SET `emote`='0' WHERE `entry`=19666 AND `groupid`=2 AND `id`=0 AND `emote`=327 LIMIT 1;