# This file is part of the OregonCore Project. See AUTHORS file for Copyright information
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

file(GLOB sources_localdir *.cpp *.h)

set(oregon-realm_SRCS
  ${oregon-realm_SRCS}
  ${sources_localdir}
  ${sources_Debugging}
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

set(oregon-realm_LINK_FLAGS "-rdynamic")

add_executable(oregon-realm
  ${oregon-realm_SRCS}
)

add_dependencies(oregon-realm revision.h)

if( NOT WIN32 )
  add_definitions(-D_OREGON_REALM_CONFIG="${CONF_DIR}/oregonrealm.conf")
endif()

if( UNIX )
  set(oregon-realm_LINK_FLAGS "-pthread ${oregon-realm_LINK_FLAGS}")
endif()

if( CMAKE_SYSTEM_NAME MATCHES "Darwin" )
  set(oregon-realm_LINK_FLAGS "-framework Carbon ${oregon-realm_LINK_FLAGS}")
endif()

set_target_properties(oregon-realm PROPERTIES LINK_FLAGS "${oregon-realm_LINK_FLAGS}")

target_link_libraries(oregon-realm
  shared
  oregonframework
  ${MYSQL_LIBRARY}
  ${OPENSSL_LIBRARIES}
  ${OPENSSL_EXTRA_LIBRARIES}
  ${OSX_LIBS}
)

if( UNIX )
  target_link_libraries(oregon-realm
    ${BFD_LIBRARY}
    dl
    ${IBERTY_LIBRARY}
    rt
    ${ZLIB_LIBRARIES}
  )
endif()

if( WIN32 )
  add_custom_command(TARGET oregon-realm
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/oregonrealm.conf.dist ${CMAKE_BINARY_DIR}/bin/$(ConfigurationName)/
  )
endif()

if( UNIX )
  install(TARGETS oregon-realm DESTINATION bin)
  install(FILES oregonrealm.conf.dist DESTINATION ${CONF_DIR})
elseif( WIN32 )
  install(TARGETS oregon-realm DESTINATION "${CMAKE_INSTALL_PREFIX}")
  install(FILES oregonrealm.conf.dist DESTINATION "${CMAKE_INSTALL_PREFIX}")
endif()
