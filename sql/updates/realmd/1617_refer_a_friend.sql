drop table if exists account_referred;
create table account_referred
(
    `id1` bigint unsigned not null comment 'Referring Account',
    `id2` bigint unsigned not null comment 'Referred Account',

    PRIMARY KEY `id1` (`id1`),
    KEY `id2` (`id2`)

) engine=InnoDB row_format=fixed COMMENT='Refer-A-Friend';
