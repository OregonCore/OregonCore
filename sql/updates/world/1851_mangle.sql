set @cat1  = 33876;
set @cat2  = 33982;
set @cat3  = 33989;
set @bear1 = 33878;
set @bear2 = 33986;
set @bear3 = 33987;

delete from spell_linked_spell where spell_trigger IN (@cat1, @cat2, @cat3, @bear1, @bear2, @bear3);

insert into spell_linked_spell VALUES (@cat1, -@bear1, 1, 'Mangle Cat  Rank 1 -> Remove Mangle Bear Rank 1');
insert into spell_linked_spell VALUES (@cat1, -@bear2, 1, 'Mangle Cat  Rank 1 -> Remove Mangle Bear Rank 2');
insert into spell_linked_spell VALUES (@cat1, -@bear3, 1, 'Mangle Cat  Rank 1 -> Remove Mangle Bear Rank 3');

insert into spell_linked_spell VALUES (@cat2, -@bear1, 1, 'Mangle Cat  Rank 2 -> Remove Mangle Bear Rank 1');
insert into spell_linked_spell VALUES (@cat2, -@bear2, 1, 'Mangle Cat  Rank 2 -> Remove Mangle Bear Rank 2');
insert into spell_linked_spell VALUES (@cat2, -@bear3, 1, 'Mangle Cat  Rank 2 -> Remove Mangle Bear Rank 3');

insert into spell_linked_spell VALUES (@cat3, -@bear1, 1, 'Mangle Cat  Rank 3 -> Remove Mangle Bear Rank 1');
insert into spell_linked_spell VALUES (@cat3, -@bear2, 1, 'Mangle Cat  Rank 3 -> Remove Mangle Bear Rank 2');
insert into spell_linked_spell VALUES (@cat3, -@bear3, 1, 'Mangle Cat  Rank 3 -> Remove Mangle Bear Rank 3');

insert into spell_linked_spell VALUES (@bear1, -@cat1, 1, 'Mangle Bear Rank 1 -> Remove Mangle Cat Rank 1');
insert into spell_linked_spell VALUES (@bear1, -@cat2, 1, 'Mangle Bear Rank 1 -> Remove Mangle Cat Rank 2');
insert into spell_linked_spell VALUES (@bear1, -@cat3, 1, 'Mangle Bear Rank 1 -> Remove Mangle Cat Rank 3');

insert into spell_linked_spell VALUES (@bear2, -@cat1, 1, 'Mangle Bear Rank 2 -> Remove Mangle Cat Rank 1');
insert into spell_linked_spell VALUES (@bear2, -@cat2, 1, 'Mangle Bear Rank 2 -> Remove Mangle Cat Rank 2');
insert into spell_linked_spell VALUES (@bear2, -@cat3, 1, 'Mangle Bear Rank 2 -> Remove Mangle Cat Rank 3');

insert into spell_linked_spell VALUES (@bear3, -@cat1, 1, 'Mangle Bear Rank 3 -> Remove Mangle Cat Rank 1');
insert into spell_linked_spell VALUES (@bear3, -@cat2, 1, 'Mangle Bear Rank 3 -> Remove Mangle Cat Rank 2');
insert into spell_linked_spell VALUES (@bear3, -@cat3, 1, 'Mangle Bear Rank 3 -> Remove Mangle Cat Rank 3');
