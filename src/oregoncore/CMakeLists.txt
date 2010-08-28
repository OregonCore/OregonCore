
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
OCSoap.cpp
OCSoap.h
)

include_directories(
  ${ACE_INCLUDE_DIR}
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/dep/include
  ${CMAKE_SOURCE_DIR}/dep/include/gsoap
  ${CMAKE_SOURCE_DIR}/src/shared
  ${CMAKE_SOURCE_DIR}/src/shared/Database
  ${CMAKE_SOURCE_DIR}/src/framework
  ${CMAKE_SOURCE_DIR}/src/game
  ${MYSQL_INCLUDE_DIR}
)

SET(oregon-core_LINK_FLAGS "")

add_executable(oregon-core ${oregon-core_SRCS})
add_definitions(
-D_OREGON_CORE_CONFIG='"${CONF_DIR}/oregoncore.conf"'
)

SET(oregon-core_LINK_FLAGS "-pthread ${oregon-core_LINK_FLAGS}")

IF (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    SET(oregon-core_LINK_FLAGS "-framework Carbon ${oregon-core_LINK_FLAGS}")
    SET(SCRIPT_LIB "")
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Darwin")

SET_TARGET_PROPERTIES(oregon-core PROPERTIES LINK_FLAGS "${oregon-core_LINK_FLAGS}")

if( WIN32 )
  target_link_libraries(
    oregon-core
    game
    shared
    zlib
    oregonframework
    oregonsockets
    g3dlite
    gsoap
    ace
    ${SCRIPT_LIB}
    ${MYSQL_LIBRARIES}
    ${SSLLIB}
    ${ZLIB}
  )
else()
  target_link_libraries(
    oregon-core
    game
    shared
    zlib
    oregonframework
    oregonsockets
    g3dlite
    gsoap
    ${READLINE_LIBRARY}
    gomp
    ${ACE_LIBRARY}
    ${SCRIPT_LIB}
    ${MYSQL_LIBRARIES}
    ${SSLLIB}
    ${ZLIB}
    ${OSX_LIBS}
  )
endif()

IF (CENTOS)
    target_link_libraries(
        oregon-core
        termcap
    )
ENDIF (CENTOS)

install(TARGETS oregon-core DESTINATION bin)

########### install files ###############

install(FILES  oregoncore.conf.dist DESTINATION etc)

