alter ignore table spell_dummy_condition drop key `id`;
alter ignore table spell_dummy_condition add primary key `id` (`entry`, `effIndex`);
