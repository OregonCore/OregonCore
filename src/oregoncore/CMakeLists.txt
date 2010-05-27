
########### next target ###############

SET(oregon-core_SRCS
CliRunnable.cpp 
CliRunnable.h 
Main.cpp 
Master.cpp 
Master.h 
RASocket.cpp 
RASocket.h 
WorldRunnable.cpp 
WorldRunnable.h
)

SET(oregon-core_LINK_FLAGS "")

add_executable(oregon-core ${oregon-core_SRCS})
add_definitions(
-D_OREGON_CORE_CONFIG='"${CONF_DIR}/oregoncore.conf"'
)
IF (DO_MYSQL)
    SET(oregon-core_LINK_FLAGS "-pthread ${oregon-core_LINK_FLAGS}")
ENDIF(DO_MYSQL)


IF (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    SET(oregon-core_LINK_FLAGS "-framework Carbon ${oregon-core_LINK_FLAGS}")
    SET(SCRIPT_LIB "")
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Darwin")

SET_TARGET_PROPERTIES(oregon-core PROPERTIES LINK_FLAGS "${oregon-core_LINK_FLAGS}")

target_link_libraries(
oregon-core
game
shared
zlib
oregonframework
oregonsockets
oregondatabase
oregonauth
oregonconfig
vmaps
g3dlite
${READLINE_LIBRARY}
gomp
${SCRIPT_LIB}
${MYSQL_LIBRARIES}
${SSLLIB}
${ACE_LIBRARY}
${ZLIB}
${OSX_LIBS}
)

IF (CENTOS)
    target_link_libraries(
        oregon-core
        termcap
    )
ENDIF (CENTOS)

install(TARGETS oregon-core DESTINATION bin)


########### install files ###############

install(FILES  oregoncore.conf.dist DESTINATION etc)

