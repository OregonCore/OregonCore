# Copyright (C) 2008-2012 OregonCore <http://www.oregoncore.com/>
# Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

file(GLOB_RECURSE sources *.cpp *.h)

if( UNIX )
  include_directories (
    ${CMAKE_SOURCE_DIR}/src/shared
    ${CMAKE_SOURCE_DIR}/dep/libmpq
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/loadlib
  )
elseif( WIN32 )
  include_directories (
    ${CMAKE_SOURCE_DIR}/src/shared
    ${CMAKE_SOURCE_DIR}/dep/libmpq
    ${CMAKE_SOURCE_DIR}/dep/libmpq/win
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/loadlib
  )
endif()

add_executable(map_extractor
  ${sources}
)

target_link_libraries(map_extractor
  mpq
  ${BZIP2_LIBRARIES}
  ${ZLIB_LIBRARIES}
)

add_dependencies(map_extractor mpq)

if( UNIX )
  install(TARGETS map_extractor DESTINATION bin)
elseif( WIN32 )
  install(TARGETS map_extractor DESTINATION "${CMAKE_INSTALL_PREFIX}") 
endif()
