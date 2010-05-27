SET(oregondatabase_STAT_SRCS
   DBCStores.cpp
   DBCStores.h
   DBCStructure.h
   DBCfmt.cpp
   Database.cpp
   Database.h
   DatabaseEnv.h
   DatabaseImpl.h
   DatabaseMysql.cpp
   DatabaseMysql.h
   Field.cpp
   Field.h
   MySQLDelayThread.h
   QueryResult.h
   QueryResultMysql.cpp
   QueryResultMysql.h
   SQLStorage.cpp
   SQLStorage.h
   SqlDelayThread.cpp
   SqlDelayThread.h
   SqlOperations.cpp
   SqlOperations.h
   dbcfile.cpp
   dbcfile.h
)

add_library(oregondatabase STATIC ${oregondatabase_STAT_SRCS})
