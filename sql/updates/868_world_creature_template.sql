UPDATE `creature_template` SET `minlevel`='37' WHERE `entry`=23928;
UPDATE `creature_template` SET `maxlevel`='37' WHERE `entry`=23928;
UPDATE `creature_template` SET `minhealth`='1336' WHERE `entry`=23928;
UPDATE `creature_template` SET `maxhealth`='1336' WHERE `entry`=23928;
UPDATE `creature_template` SET `faction_A`='168' WHERE `entry`=23928;
UPDATE `creature_template` SET `faction_H`='168' WHERE `entry`=23928;
UPDATE `creature_template` SET `mindmg`='23' WHERE `entry`=23928;
UPDATE `creature_template` SET `maxdmg`='48' WHERE `entry`=23928;
DELETE FROM areatrigger_scripts WHERE entry=4752;
INSERT INTO areatrigger_scripts VALUES
(4752,'at_nats_landing');