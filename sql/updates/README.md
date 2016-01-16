SQL Updates
===========

SQL updates are updates for database and are necessary for keeping for database up-to-date.  
Some changes are deeply linked core-db so they should be always synchronized.  
For automatic updating you can turn off DatabaseUpdater in oregoncore.conf.dist.

Formatting
----------

Each update should be formatted like:

    YYYY_MM_DD_name.sql

where:

 - YYYY is 4 digit year (e.g. 2016)
 - MM is 2 digit month (i.e. 01-12)
 - DD is 2 digit day in month (i.e. 01-31)
 - name is an arbitrary name for the update (must not contain dot characters (`.`)
