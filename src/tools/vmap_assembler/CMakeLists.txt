# Copyright (C) 2005-2009 MaNGOS project <http://getmangos.com/>
# Copyright (C) 2008-2010 Trinity <http://www.trinitycore.org/>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

include_directories(
  ${CMAKE_SOURCE_DIR}/dep/include/g3dlite
  ${CMAKE_SOURCE_DIR}/src/shared
  ${CMAKE_SOURCE_DIR}/src/collision
  ${CMAKE_SOURCE_DIR}/src/framework
  ${ACE_INCLUDE_DIR}
  ${ZLIB_INCLUDE_DIR}
)

add_definitions(-DNO_CORE_FUNCS)
add_executable(vmap_assembler VMapAssembler.cpp)
add_dependencies(vmap_assembler mpq)

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
  set_target_properties(vmap_assembler PROPERTIES LINK_FLAGS "-framework Carbon")
endif()

target_link_libraries(vmap_assembler
  collision
  g3dlite
  ${ZLIB_LIBRARIES}
)

if( UNIX )
  install(TARGETS vmap_assembler DESTINATION bin)
elseif( WIN32 )
  install(TARGETS vmap_assembler DESTINATION "${CMAKE_INSTALL_PREFIX}")
endif()
