delete from spell_affect where entry = 19245;

insert into spell_affect (entry, effectId, SpellFamilyFlags)
values (19245, 0, 24), -- freezing trap & frost trap
(19245, 1, 35184372088836); -- immolation, explosive & snake trap
