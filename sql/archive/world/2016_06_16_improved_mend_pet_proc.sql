
-- Fix proccing on periodic tick instead of initial one
update spell_proc_event set procFlags = 0x40000 where entry in (19572, 19573);
