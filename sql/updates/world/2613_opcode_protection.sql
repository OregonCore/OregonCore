
drop table if exists opcode_protection;
create table if not exists opcode_protection
(
    `opcode` int unsigned not null comment 'Opcode to protect, see Opcodes.h',
    `threshold` int unsigned not null comment 'Sets the maximum count one protected packet per Interval can be processed per session.',
    `interval` int unsigned not null comment 'Interval for threshold, in milliseconds.',
    `penalty` tinyint unsigned not null comment 'What should happen if the threshold per interval is passed. 0 - Skip, 1 - Kick',
    `comment` varchar(255) not null default '',

    PRIMARY KEY opcode (opcode)
) engine=InnoDB;

insert into opcode_protection values
(0x062, 3, 1000, 0, "/who - 3 per second"),
(0x205, 1, 2000, 1, "ticket create - 1 per 2 seconds"),
(0x207, 1, 2000, 1, "ticket update - 1 per 2 seconds"),
(0x238, 1, 1000, 1, "send mail - 1 per second"),
(0x23A, 3, 1000, 0, "get mail list - 3 per second");

