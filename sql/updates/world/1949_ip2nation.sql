-- Code below is equivalent to RENAME TABLE, execept
-- that it does suppress errors

SELECT Count(*)
INTO @exists
FROM information_schema.tables 
WHERE table_schema = DATABASE()
    AND table_type = 'BASE TABLE'
    AND table_name = 'ip2nationcountries';

SELECT Count(*)
INTO @alreadyRenamed
FROM information_schema.tables 
WHERE table_schema = DATABASE()
    AND table_type = 'BASE TABLE'
    AND table_name = 'ip2nationCountries';

SET @query = If(@exists AND !@alreadyRenamed,
    'RENAME TABLE ip2nationcountries TO ip2nationCountries',
    'SELECT \'nothing to rename or already renamed\' status');

PREPARE stmt FROM @query;

EXECUTE stmt;
