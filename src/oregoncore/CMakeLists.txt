file(GLOB sources_localdir *.cpp *.h)

set(oregon-core_SRCS
  ${oregon-core_SRCS}
  ${sources_localdir}
)

if( WIN32 )
  set(oregon-core_SRCS
    ${oregon-core_SRCS}
    OregonCore.rc
  )
endif()

include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/dep/include
  ${CMAKE_SOURCE_DIR}/dep/include/gsoap
  ${CMAKE_SOURCE_DIR}/src/shared
  ${CMAKE_SOURCE_DIR}/src/shared/Database
  ${CMAKE_SOURCE_DIR}/src/framework
  ${CMAKE_SOURCE_DIR}/src/game
  ${ACE_INCLUDE_DIR}
  ${MYSQL_INCLUDE_DIR}
  ${OPENSSL_INCLUDE_DIR}
)

set(oregon-core_LINK_FLAGS "")

add_executable(oregon-core ${oregon-core_SRCS})

if( NOT WIN32 )
  add_definitions(-D_OREGON_CORE_CONFIG='"${CONF_DIR}/oregoncore.conf"')
endif()

add_dependencies(oregon-core genrev)

if( UNIX )
  set(oregon-core_LINK_FLAGS "-pthread ${oregon-core_LINK_FLAGS}")
endif()

if( CMAKE_SYSTEM_NAME MATCHES "Darwin" )
  set(oregon-core_LINK_FLAGS "-framework Carbon ${oregon-core_LINK_FLAGS}")
  set(SCRIPT_LIB "")
endif()

set_target_properties(oregon-core PROPERTIES LINK_FLAGS "${oregon-core_LINK_FLAGS}")

if( WIN32 )
  target_link_libraries(oregon-core
    game
    shared
    scripts
    zlib
    oregonframework
    oregonsockets
    g3dlite
    gsoap
    ace
    ${MYSQL_LIBRARY}
    ${OPENSSL_LIBRARIES}
    ${OPENSSL_EXTRA_LIBRARIES}
  )
else()
  target_link_libraries(oregon-core
    game
    shared
    scripts
    oregonframework
    oregonsockets
    g3dlite
    gsoap
    ${READLINE_LIBRARY}
    ${ACE_LIBRARY}
    ${MYSQL_LIBRARY}
    ${OPENSSL_LIBRARIES}
    ${ZLIB}
    ${OSX_LIBS}
  )
endif()

if( WIN32 )
  add_custom_command(TARGET worldserver
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/oregoncore.conf.dist ${CMAKE_BINARY_DIR}/bin/$(ConfigurationName)/
  )
endif()

if( UNIX )
  install(TARGETS oregon-core DESTINATION bin)
  install(FILES oregoncore.conf.dist DESTINATION etc)
elseif( WIN32 )
  install(TARGETS oregon-core DESTINATION "${CMAKE_INSTALL_PREFIX}")
  install(FILES oregoncore.conf.dist DESTINATION "${CMAKE_INSTALL_PREFIX}")
endif()
