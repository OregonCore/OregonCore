########### next target ###############

SET(oregon-realm_SRCS
AuthCodes.h 
AuthSocket.cpp 
AuthSocket.h 
BufferedSocket.h
BufferedSocket.cpp
Main.cpp 
PatchHandler.h
PatchHandler.cpp
RealmList.cpp 
RealmList.h
)

include_directories(
  ${ACE_INCLUDE_DIR}
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/src/shared
  ${CMAKE_SOURCE_DIR}/src/shared/Database
  ${CMAKE_SOURCE_DIR}/src/framework
  ${MYSQL_INCLUDE_DIR}
)

SET(oregon-realm_LINK_FLAGS "")

add_executable(oregon-realm ${oregon-realm_SRCS})
add_definitions(
-D_OREGON_REALM_CONFIG='"${CONF_DIR}/oregonrealm.conf"'
)

SET(oregon-realm_LINK_FLAGS "-pthread ${oregon-realm_LINK_FLAGS}")

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
${OSX_LIBS}
)

install(TARGETS oregon-realm DESTINATION bin)


########### install files ###############

install(FILES  oregonrealm.conf.dist DESTINATION etc)
