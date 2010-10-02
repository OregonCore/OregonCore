if( GAMEPCH )
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
endif()

file(GLOB sources_localdir *.cpp *.h)

# Create game-libary

if( GAMEPCH AND MSVC )
  set(game_STAT_SRCS
    PrecompiledHeaders/gamePCH.cpp
    PrecompiledHeaders/gamePCH.h
  )
endif()

set(game_STAT_SRCS
  ${game_STAT_SRCS}
  ${sources_localdir}
)

include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/dep/include
  ${CMAKE_SOURCE_DIR}/src/framework
  ${CMAKE_SOURCE_DIR}/src/shared
  ${CMAKE_SOURCE_DIR}/src/shared/vmap
  ${CMAKE_SOURCE_DIR}/src/shared/Database
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/src/scripts/PrecompiledHeaders
  ${ACE_INCLUDE_DIR}
  ${MYSQL_INCLUDE_DIR}
)

add_library(game STATIC ${game_STAT_SRCS})

add_dependencies(game genrev)

# Generate precompiled header
if( GAMEPCH )
  if(CMAKE_COMPILER_IS_GNUCXX)
    add_precompiled_header(game ${CMAKE_CURRENT_SOURCE_DIR}/PrecompiledHeaders/gamePCH.h)
  elseif(MSVC)
    add_native_precompiled_header(game ${CMAKE_CURRENT_SOURCE_DIR}/PrecompiledHeaders/gamePCH)
  endif()
endif()
