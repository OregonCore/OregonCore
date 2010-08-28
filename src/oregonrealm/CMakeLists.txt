file(GLOB sources_localdir *.cpp *.h)

set(oregon-realm_SRCS
  ${oregon-realm_SRCS}
  ${sources_localdir}
)

if( WIN32 )
set(oregon-realm_SRCS
    ${oregon-realm_SRCS}
    OregonRealm.rc
)
endif()

include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/src/shared
  ${CMAKE_SOURCE_DIR}/src/shared/Database
  ${CMAKE_SOURCE_DIR}/src/framework
  ${ACE_INCLUDE_DIR}
  ${MYSQL_INCLUDE_DIR}
  ${OPENSSL_INCLUDE_DIR}
)

set(oregon-realm_LINK_FLAGS "")

add_executable(oregon-realm
  ${oregon-realm_SRCS}
)

add_dependencies(oregon-realm genrev)

if( NOT WIN32 )
  add_definitions(-D_OREGON_CORE_CONFIG='"${CONF_DIR}/oregonrealm.conf"')
endif()

if( UNIX )
  set(authserver_LINK_FLAGS "-pthread ${oregon-realm_LINK_FLAGS}")
endif()

if( CMAKE_SYSTEM_NAME MATCHES "Darwin" )
  set(oregon-realm_LINK_FLAGS "-framework Carbon ${oregon-realm_LINK_FLAGS}")
endif()

set_target_properties(oregon-realm PROPERTIES LINK_FLAGS "${oregon-realm_LINK_FLAGS}")

if( WIN32 )
  target_link_libraries(
    oregon-realm
    shared
    oregonframework
    ${MYSQL_LIBRARY}
    ${OPENSSL_LIBRARIES}
    ${OPENSSL_EXTRA_LIBRARIES}
  )
else()
  target_link_libraries(
    oregon-realm
    shared
    oregonframework
    ${MYSQL_LIBRARY}
    ${OPENSSL_LIBRARIES}
    ${OSX_LIBS}
  )
endif()

if( WIN32 )
  add_custom_command(TARGET worldserver
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/oregonrealm.conf.dist ${CMAKE_BINARY_DIR}/bin/$(ConfigurationName)/
  )
endif()

if( UNIX )
  install(TARGETS oregon-realm DESTINATION bin)
  install(FILES oregonrealm.conf.dist DESTINATION etc)
elseif( WIN32 )
  install(TARGETS oregon-realm DESTINATION "${CMAKE_INSTALL_PREFIX}")
  install(FILES oregonrealm.conf.dist DESTINATION "${CMAKE_INSTALL_PREFIX}")
endif()
