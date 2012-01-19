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

# Enable precompiled headers when using the GCC compiler.
if( USE_SCRIPTPCH )
  include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
  )
endif()

file(GLOB_RECURSE scripts_custom Custom/*.cpp Custom/*.h)
file(GLOB_RECURSE scripts_easternplaguelands EasternKingdoms/*.cpp EasternKingdoms/*.h)
file(GLOB_RECURSE scripts_examples Examples/*.cpp Examples/*.h)
file(GLOB_RECURSE scripts_kalimdor Kalimdor/*.cpp Kalimdor/*.h)
file(GLOB_RECURSE scripts_outland Outland/*.cpp Outland/*.h)
file(GLOB_RECURSE scripts_world World/*.cpp World/*.h)

if( USE_SCRIPTPCH AND MSVC )
  set(scripts_STAT_SRCS
    PrecompiledHeaders/ScriptPCH.cpp
    PrecompiledHeaders/ScriptPCH.h
  )
endif()

set(scripts_STAT_SRCS
  ${scripts_STAT_SRCS}
  ../game/ScriptedEscortAI.cpp
  ../game/ScriptedCreature.cpp
  ../game/ScriptedFollowerAI.cpp
  ../game/ScriptedGuardAI.cpp
  ../game/ScriptedSimpleAI.cpp
)

if(SCRIPTS)
  set(scripts_STAT_SRCS
    ${scripts_STAT_SRCS}
    ${scripts_custom}
    ${scripts_easternplaguelands}
    ${scripts_examples}
    ${scripts_kalimdor}
    ${scripts_outland}
    ${scripts_world}
  )
endif()

include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/dep/SFMT
  ${CMAKE_SOURCE_DIR}/dep/mersennetwister
  ${CMAKE_SOURCE_DIR}/dep/zlib
  ${CMAKE_SOURCE_DIR}/src/framework
  ${CMAKE_SOURCE_DIR}/src/game
  ${CMAKE_SOURCE_DIR}/src/shared
  ${CMAKE_SOURCE_DIR}/src/shared/Database
  ${CMAKE_CURRENT_SOURCE_DIR}/PrecompiledHeaders
  ${ACE_INCLUDE_DIR}
  ${MYSQL_INCLUDE_DIR}
  ${OPENSSL_INCLUDE_DIR}
)

add_library(scripts STATIC ${scripts_STAT_SRCS})

add_dependencies(scripts revision.h)

# Generate precompiled header
if( USE_SCRIPTPCH )
  if(CMAKE_COMPILER_IS_GNUCXX)
    add_precompiled_header(scripts ${CMAKE_CURRENT_SOURCE_DIR}/PrecompiledHeaders/ScriptPCH.h)
  elseif(MSVC)
    add_native_precompiled_header(scripts ${CMAKE_CURRENT_SOURCE_DIR}/PrecompiledHeaders/ScriptPCH)
  endif()
endif()
