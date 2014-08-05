# Copyright (C) 2008-2014 OregonCore <http://www.oregoncore.com/>
# Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

file(GLOB sources_localdir *.cpp *.h)

set(oregon-core_SRCS
  ${oregon-core_SRCS}
  ${sources_localdir}
  ${sources_Debugging}
)

if( WIN32 )
  set(oregon-core_SRCS
    ${oregon-core_SRCS}
    OregonCore.rc
  )
endif()

include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/dep/gsoap
  ${CMAKE_SOURCE_DIR}/dep/sockets/include
  ${CMAKE_SOURCE_DIR}/dep/SFMT
  ${CMAKE_SOURCE_DIR}/dep/mersennetwister
  ${CMAKE_SOURCE_DIR}/src/collision
  ${CMAKE_SOURCE_DIR}/src/shared
  ${CMAKE_SOURCE_DIR}/src/shared/Database
  ${CMAKE_SOURCE_DIR}/src/framework
  ${CMAKE_SOURCE_DIR}/src/game
  ${CMAKE_SOURCE_DIR}/dep/recastnavigation/Detour
  ${ACE_INCLUDE_DIR}
  ${MYSQL_INCLUDE_DIR}
  ${OPENSSL_INCLUDE_DIR}
)

set(oregon-core_LINK_FLAGS "")

add_executable(oregon-core ${oregon-core_SRCS})

if( NOT WIN32 )
  add_definitions(-D_OREGON_CORE_CONFIG="${CONF_DIR}/oregoncore.conf")
endif()

add_dependencies(oregon-core revision.h)

if( UNIX )
  set(oregon-core_LINK_FLAGS "-pthread -rdynamic ${oregon-core_LINK_FLAGS}")
endif()

set_target_properties(oregon-core PROPERTIES LINK_FLAGS "${oregon-core_LINK_FLAGS}")

target_link_libraries(oregon-core
  game
  shared
  scripts
  oregonframework
  collision
  g3dlib
  gsoap
  Recast
  Detour
  ${JEMALLOC_LIBRARY}
  ${READLINE_LIBRARY}
  ${TERMCAP_LIBRARY}
  ${ACE_LIBRARY}
  ${MYSQL_LIBRARY}
  ${OPENSSL_LIBRARIES}
  ${OPENSSL_EXTRA_LIBRARIES}
  ${ZLIB_LIBRARIES}
  ${OSX_LIBS}
)

if( UNIX )
  target_link_libraries(oregon-core
    ${BFD_LIBRARY}
    dl
    ${IBERTY_LIBRARY}
  )
endif()

if( WIN32 )
  add_custom_command(TARGET oregon-core
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/oregoncore.conf.dist ${CMAKE_BINARY_DIR}/bin/$(ConfigurationName)/
  )
endif()

if( UNIX )
  install(TARGETS oregon-core DESTINATION bin)
  install(FILES oregoncore.conf.dist DESTINATION ${CONF_DIR})
elseif( WIN32 )
  install(TARGETS oregon-core DESTINATION "${CMAKE_INSTALL_PREFIX}")
  install(FILES oregoncore.conf.dist DESTINATION "${CMAKE_INSTALL_PREFIX}")
endif()
