-- Change Custodian of Time (20129) emotes to Say instead of Boss Emote
UPDATE script_texts SET type=4 WHERE entry BETWEEN -1000230 AND -1000217;

-- Add CONDITION_AURA condition to spell 34877 to prevent Custodian of Time (20129) being resummoned while having the debuff
DELETE FROM conditions WHERE SourceEntry=34877;
INSERT INTO conditions (SourceTypeOrReferenceId, SourceEntry, ConditionTypeOrReference, ConditionValue1, ConditionValue2, ConditionValue3, NegativeCondition, Comment) VALUES (17, 34877, 1, 34877, 1, 1, 1, "Custodian of Time summon");

