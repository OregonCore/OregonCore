ALTER TABLE `event_scripts` ADD PRIMARY KEY  (`id`, `delay`, `command`, `datalong`, `datalong2`, `dataint`, `x`, `y`, `z`, `o`);
ALTER TABLE `gameobject_scripts` ADD PRIMARY KEY  (`id`, `delay`, `command`, `datalong`, `datalong2`, `dataint`, `x`, `y`, `z`, `o`);
ALTER TABLE `gossip_scripts` ADD PRIMARY KEY  (`id`, `delay`, `command`, `datalong`, `datalong2`, `dataint`, `x`, `y`, `z`, `o`);
ALTER TABLE `spell_scripts` ADD PRIMARY KEY  (`id`, `delay`, `command`, `datalong`, `datalong2`, `dataint`, `x`, `y`, `z`, `o`);
ALTER TABLE `quest_start_scripts` ADD PRIMARY KEY  (`id`, `delay`, `command`, `datalong`, `datalong2`, `dataint`, `x`, `y`, `z`, `o`);

DELETE FROM `quest_end_scripts` WHERE `id` = 10964 AND `delay` = 38 AND `command` = 2 AND `datalong` = 159 AND `datalong2` = 3 AND `dataint` = 0 AND  `x` = 0 AND  `y` = 0 AND  `z` = 0 AND `o` = 0;
INSERT INTO `quest_end_scripts` (`id`, `delay`, `command`, `datalong`, `datalong2`, `dataint`, `x`, `y`, `z`, `o`) VALUES('10964','38','2','159','3','0','0','0','0','0');
ALTER TABLE `quest_end_scripts` ADD PRIMARY KEY  (`id`, `delay`, `command`, `datalong`, `datalong2`, `dataint`, `x`, `y`, `z`, `o`);

ALTER TABLE `playercreateinfo_action` ADD PRIMARY KEY  (`race`, `class`, `button`);
ALTER TABLE `playercreateinfo_item` ADD PRIMARY KEY  (`race`, `class`, `itemid`);
ALTER TABLE `spell_enchant_proc_data` ADD PRIMARY KEY  (`entry`);

ALTER TABLE `spell_linked_spell` DROP INDEX `trigger_effect_type`;
ALTER TABLE `spell_linked_spell` ADD PRIMARY KEY  (`spell_trigger`, `spell_effect`, `type`);
ALTER TABLE `spell_script_target` DROP INDEX `entry_type_target`;
ALTER TABLE `spell_script_target` ADD PRIMARY KEY  (`entry`, `type`, `targetEntry`);
ALTER TABLE `waypoint_data` ADD PRIMARY KEY  (`id`, `point`);
ALTER TABLE `waypoint_scripts` DROP PRIMARY KEY;
ALTER TABLE `waypoint_scripts` ADD PRIMARY KEY  (`id`, `delay`, `command`, `datalong`, `datalong2`, `dataint`, `x`, `y`, `z`, `o`);
