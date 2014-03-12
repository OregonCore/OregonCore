update quest_template set ReqSpellCast1 = 0 where entry = 11147; -- Unleash The raptors
replace into event_scripts VALUES (15363, 0, 8, 23727, 0, 0, 0, 0, 0, 0); -- Kill Credit
replace into event_scripts VALUES (6206,  0, 19, 0, 0, 0, 0, 0, 0, 0); -- null action (padding)
replace into event_scripts VALUES (5121,  0, 19, 0, 0, 0, 0, 0, 0, 0); -- null action (padding)
update quest_template set ReqSpellCast1 = 0 where entry = 9931; -- Returning the favor
replace into event_scripts VALUES (11674, 0, 8, 18393, 0, 0, 0, 0, 0, 0); -- Kill Credit
replace into event_scripts VALUES (13489, 0, 19, 0, 0, 0, 0, 0, 0, 0); -- padding
replace into event_scripts VALUES (13021, 0, 19, 0, 0, 0, 0, 0, 0, 0); -- padding
update quest_template set SrcItemId = 28786, SrcItemCount = 1 where entry = 10256; -- Finding the keymaster
replace into event_scripts VALUES (12857, 0, 19, 0, 0, 0, 0, 0, 0, 0); -- padding
replace into event_scripts VALUES (17253, 0, 19, 0, 0, 0, 0, 0, 0, 0); -- padding

-- Begin fix for Defile Uther's Tomb
update quest_template set ReqCreatureOrGOId1 = -182483, ReqSpellCast1 = 30098 where entry = 9444;
-- we no longer need this guy (Defile Uther's Tomb Trigger)
delete from creature_template where entry = 17253;
delete from creature where id = 17253;
-- Make quest complete-able
delete from spell_script_target where entry = 30098;
insert into spell_script_target values (30098, 0, 182483);
-- Uther's ghost
delete from event_scripts where id = 10561;
insert into event_scripts VALUES (10561, 0, 10, 17233, 60000, 0, 973.862539, -1824.777100, 82.416138, 0.292839);
delete from creature_ai_scripts where creature_id = 17233;
insert into creature_ai_scripts (/* id = AUTO, */
       creature_id, event_type,   event_inverse_phase_mask, event_chance,
       event_flags, event_param1, event_param2,             event_param3, event_param4,
       action1_type, action1_param1, action1_param2, action1_param3,
       action2_type, action2_param1, action2_param2, action2_param3,
       action3_type, action3_param1, action3_param2, action3_param3,
       comment) VALUES (17233,    11, 0, 100,
                            0,     0, 0,   0, 0,
                           11, 11010, 0,   18,
                            0,     0, 0,   0,
                            0,     0, 0,   0,
                        'Uther\'s hover effect');
-- Move the focus object closer
update gameobject set position_z = 82 where guid = 99865;
-- End (Defile Uther's Tomb)

-- actually this needs to be scripted, we'll pad it for now (Quest is working, however there's no nice scene in the end)
replace into event_scripts VALUES (2807, 0, 19, 0, 0, 0, 0, 0, 0, 0); -- padding -- Saving Sharpbeak

update quest_template set ReqCreatureOrGoId1 = 19549, ReqCreatureOrGOCount1 = 1, ReqCreatureOrGOCount2 = 0, ReqSpellCast2 = 0 where entry = 10182; -- Battle-Mage Dathric
replace into event_scripts VALUES (12608, 0, 8, 19549, 0, 0, 0, 0, 0, 0); -- Kill Credit
update quest_template set ReqSpellCast1 = 0 where entry = 10306; -- Conjurer Luminrath
replace into event_scripts VALUES (12609, 0, 8, 19548, 0, 0, 0, 0, 0, 0); -- Kill Credit
update quest_template set ReqSpellCast1 = 0 where entry = 10307; -- Cohlien Frostweaver
replace into event_scripts VALUES (12610, 0, 8, 19550, 0, 0, 0, 0, 0, 0); -- Kill Credit
replace into event_scripts VALUES (13874, 0, 19,    0, 0, 0, 0, 0, 0, 0); -- padding -- Blast the Infernals!
replace into event_scripts VALUES (14394, 0, 19,    0, 0, 0, 0, 0, 0, 0); -- padding -- Becoming a Shadoweave Tailor
replace into event_scripts VALUES (14293, 0, 19,    0, 0, 0, 0, 0, 0, 0); -- padding -- Thwart the Dark Conclave
