-- Code below is equivalent to RENAME TABLE, execept
-- that it does suppress errors

SELECT Count(*)
INTO @exists
FROM information_schema.tables 
WHERE table_schema = DATABASE()
    AND table_type = 'BASE TABLE'
    AND table_name = 'ip2nationcountries';

SET @query = If(@exists>0,
    'RENAME TABLE ip2nationcountries TO ip2nationCountries',
    'SELECT \'nothing to rename\' status');

PREPARE stmt FROM @query;

EXECUTE stmt;
