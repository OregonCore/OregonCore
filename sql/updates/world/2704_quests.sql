-- Banish portals
update quest_template set ReqCreatureOrGOId1 = -177367 where entry = 5581;
-- Fix los issue for "Laying Waster to the Unwanted"
update creature_template set flags_extra = flags_extra | 0x80 where entry in (18818,21237,19009,21236);
