########### next target ###############

SET(oregon-realm_SRCS
AuthCodes.h 
AuthSocket.cpp 
AuthSocket.h 
Main.cpp 
RealmList.cpp 
RealmList.h
)

SET(oregon-realm_LINK_FLAGS "")

add_executable(oregon-realm ${oregon-realm_SRCS})
add_definitions(
-D_OREGON_REALM_CONFIG='"${CONF_DIR}/oregonrealm.conf"'
)
IF (DO_MYSQL)
   SET(oregon-realm_LINK_FLAGS "-pthread ${oregon-realm_LINK_FLAGS}")
ENDIF(DO_MYSQL)
IF (DO_POSTGRESQL)
   SET(oregon-realm_LINK_FLAGS "-pthread ${oregon-realm_LINK_FLAGS}")
ENDIF(DO_POSTGRESQL)

IF (CMAKE_SYSTEM_NAME MATCHES "Darwin")
   SET(oregon-realm_LINK_FLAGS "-framework Carbon ${oregon-realm_LINK_FLAGS}")
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Darwin")

SET_TARGET_PROPERTIES(oregon-realm PROPERTIES LINK_FLAGS "${oregon-realm_LINK_FLAGS}")

target_link_libraries(
oregon-realm
shared
oregonframework
oregonsockets
oregondatabase
oregonauth
oregonconfig
zlib
gomp
${SSLLIB}
${MYSQL_LIBRARIES}
${POSTGRESQL_LIBRARIES}
${OSX_LIBS}
)

install(TARGETS oregon-realm DESTINATION bin)


########### install files ###############

install(FILES  oregonrealm.conf.dist DESTINATION etc)

