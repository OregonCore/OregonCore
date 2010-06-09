SET(oregondatabase_STAT_SRCS
    DBCFileLoader.cpp
    DBCFileLoader.h
    DBCStore.h
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
)

add_library(oregondatabase STATIC ${oregondatabase_STAT_SRCS})
