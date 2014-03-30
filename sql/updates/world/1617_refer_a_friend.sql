-- GM Commands:
/*
 2. .raf info <account>
 3. .raf link <linkerAcc> <linkedAcc>
 4. .rad unlink <linkerAcc> <linkedAcc>
 5. .raf summon
 6. .raf grantlevel
 7. .reload account_referred
*/
use world;
replace into command (name, security, help) values
('raf',       2, '.raf - Refer-A-Friend Commands'),
('raf info',  2, CONCAT('Syntax: .raf info <account>',
                        '\n\n',
                        'Displays Refer-a-Friend info for given account'),
('raf link',  2, CONCAT('Syntax: .raf link <linkerAcc> <linkedAcc>',
                        '\n\n',
                        'Link two account with Refer-A-Friend.',
                        '\n',
                        'linkerAcc: name of referring Account',
                        '\n',
                        'linkedAcc: name of referred Account',
                        '\n',
                        'To get account name you can use .pinfo command',
                        '\n',
                        'For example if Bob referred Suse, use .raf link Bob Suse')),
('raf unlink',2, CONCAT('Syntax: .raf unlink <linkerAcc> <linkedAcc>',
                        '\n\n',
                        'Unlink two RaF-linked accounts. See .raf link for more details.'),
('raf summon',0, CONCAT('Summons your Refer-A-Friend'),
('raf grantlevel', 0, CONCAT('Grants your referrer a level'));

