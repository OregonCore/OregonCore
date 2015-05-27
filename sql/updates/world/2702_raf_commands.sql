update command set help = 'Syntax: .raf link <linkerAcc> <linkedAcc>

Link two account with Refer-A-Friend.
linkerAcc: ID of referring Account
linkedAcc: ID of referred Account
To get account id you can use .pinfo command
For example if Bob referred Suse, use .raf link Bob Suse' where name = 'raf link';

update command set help = 'Syntax: .raf info <accountId>

Displays Refer-a-Friend info for given account' where name = 'raf info';
