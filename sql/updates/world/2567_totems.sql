-- Add the immediate trigger spells to all totems that should behave this way

-- Magma Totem
-- Rank 1
UPDATE creature_template SET spell2 = 8187 WHERE entry = 5929;
-- Rank 2
UPDATE creature_template SET spell2 = 10579 WHERE entry = 7464;
-- Rank 3
UPDATE creature_template SET spell2 = 10580 WHERE entry = 7465;
-- Rank 4
UPDATE creature_template SET spell2 = 10581 WHERE entry = 7466;
-- Rank 5
UPDATE creature_template SET spell2 = 25550 WHERE entry = 15484;

-- Stoneclaw Totem
-- Rank 1
UPDATE creature_template SET spell2 = 5729 WHERE entry = 3579;
-- Rank 2
UPDATE creature_template SET spell2 = 6393 WHERE entry = 3911;
-- Rank 3
UPDATE creature_template SET spell2 = 6394 WHERE entry = 3912;
-- Rank 4
UPDATE creature_template SET spell2 = 6395 WHERE entry = 3913;
-- Rank 5
UPDATE creature_template SET spell2 = 10423 WHERE entry = 7398;
-- Rank 6
UPDATE creature_template SET spell2 = 10424 WHERE entry = 7399;
-- Rank 7
UPDATE creature_template SET spell2 = 25512 WHERE entry = 15478;

-- Windfury Totem
-- Rank 1
UPDATE creature_template SET spell2 = 8514 WHERE entry = 6112;
-- Rank 2
UPDATE creature_template SET spell2 = 10607 WHERE entry = 7483;
-- Rank 3
UPDATE creature_template SET spell2 = 10611 WHERE entry = 7484;
-- Rank 4
UPDATE creature_template SET spell2 = 25579 WHERE entry = 15496;
-- Rank 5
UPDATE creature_template SET spell2 = 25580 WHERE entry = 15497;

-- Disease Cleansing Totem
UPDATE creature_template SET spell2 = 8171 WHERE entry = 5924;
-- Poison Cleansing Totem
UPDATE creature_template SET spell2 = 8168 WHERE entry = 5923;
