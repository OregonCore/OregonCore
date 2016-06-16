-- Update maximum start distance radius values

-- av 100 -> 150 (max walkable ~120)
update battleground_template set StartMaxDist = 150 where id = 1;

-- wsg 75 -> 45 (max walkable ~40)
update battleground_template set StartMaxDist = 45 where id = 2;

-- ab left intact, 75 is OK (max walkable is ~70)

-- eos 75 -> 10 (max walkable is ~5)
update battleground_template set StartMaxDist = 10 where id = 7;
