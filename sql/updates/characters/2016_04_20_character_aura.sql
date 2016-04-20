alter table `character_aura` drop if exists `item_caster_guid`;
alter table `character_aura` add `item_caster_guid` bigint unsigned not null after `caster_guid`;
