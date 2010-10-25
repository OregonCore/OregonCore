# Copyright (C) 2008-2010 Trinity <http://www.trinitycore.org/>
#
# Copyright (C) 2009-2010 Oregon <http://www.oregoncore.org/>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

file(GLOB_RECURSE sources_vmap vmap/*.cpp vmap/*.h)
file(GLOB_RECURSE sources_auth Auth/*.cpp Auth/*.h)
file(GLOB_RECURSE sources_config Config/*.cpp Config/*.h)
file(GLOB_RECURSE sources_database Database/*.cpp Database/*.h)

file(GLOB sources_localdir *.cpp *.h)

#
# Build shared sourcelist
#

SET(shared_STAT_SRCS
  ${shared_STAT_SRCS}
  ${sources_vmap}
  ${sources_auth}
  ${sources_config}
  ${sources_database}
  ${sources_localdir}
)

include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/dep/include
  ${CMAKE_SOURCE_DIR}/dep/include/g3dlite
  ${CMAKE_SOURCE_DIR}/dep/include/sockets
  ${CMAKE_SOURCE_DIR}/src/framework
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/Auth
  ${CMAKE_CURRENT_SOURCE_DIR}/Config
  ${CMAKE_CURRENT_SOURCE_DIR}/Database
  ${ACE_INCLUDE_DIR}
  ${MYSQL_INCLUDE_DIR}
  ${OPENSSL_INCLUDE_DIR}
)

add_library(shared STATIC ${shared_STAT_SRCS})

target_link_libraries(shared
  ${ACE_LIBRARY}
)
