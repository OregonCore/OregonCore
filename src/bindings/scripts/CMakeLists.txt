file(GLOB_RECURSE scripts_include include/*.cpp include/*.h)
file(GLOB_RECURSE scripts_areatrigger scripts/areatrigger/*.cpp scripts/areatrigger/*.h)
file(GLOB_RECURSE scripts_boss scripts/boss/*.cpp scripts/boss/*.h)
file(GLOB_RECURSE scripts_creature scripts/creature/*.cpp scripts/creature/*.h)
file(GLOB_RECURSE scripts_custom scripts/custom/*.cpp scripts/custom/*.h)
file(GLOB_RECURSE scripts_go scripts/go/*.cpp scripts/go/*.h)
file(GLOB_RECURSE scripts_guard scripts/guard/*.cpp scripts/guard/*.h)
file(GLOB_RECURSE scripts_item scripts/item/*.cpp scripts/item/*.h)
file(GLOB_RECURSE scripts_npc scripts/npc/*.cpp scripts/npc/*.h)
file(GLOB_RECURSE scripts_zone scripts/zone/*.cpp scripts/zone/*.h)
file(GLOB_RECURSE scripts_system system/*.cpp system/*.h)
file(GLOB sources_localdir *.cpp *.h)

set(oregonscript_LIB_SRCS
  ${oregonscript_LIB_SRCS}
  ${scripts_include}
  ${scripts_areatrigger}
  ${scripts_boss}
  ${scripts_creature}
  ${scripts_custom}
  ${scripts_go}
  ${scripts_guard}
  ${scripts_npc}
  ${scripts_item}
  ${scripts_npc}
  ${scripts_zone}
  ${scripts_system}
  ${sources_localdir}
)

include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/src/bindings/scripts/include
  ${CMAKE_SOURCE_DIR}/src/bindings/scripts/scripts/npc
  ${CMAKE_SOURCE_DIR}/src/framework
  ${CMAKE_SOURCE_DIR}/src/game
  ${CMAKE_SOURCE_DIR}/src/shared
  ${CMAKE_SOURCE_DIR}/src/shared/Database
  ${CMAKE_SOURCE_DIR}/dep/include
  ${ACE_INCLUDE_DIR}
  ${MYSQL_INCLUDE_DIR}
)

add_library(oregonscript SHARED ${oregonscript_LIB_SRCS})

add_dependencies(oregonscript genrev)

if( NOT WIN32 )
  add_definitions(-D_OREGON_CORE_CONFIG='"${CONF_DIR}/oregoncore.conf"')
endif()

if( CMAKE_SYSTEM_NAME MATCHES "Darwin" )
    set_target_properties(oregonscript PROPERTIES LINK_FLAGS "-flat_namespace -undefined suppress")
endif()

target_link_libraries(oregonscript)

set_target_properties(oregonscript PROPERTIES VERSION 4.2.0 SOVERSION 4)

# Generate precompiled header
if( SCRIPTPCH )
  if(CMAKE_COMPILER_IS_GNUCXX)
    add_precompiled_header(oregonscript ${CMAKE_CURRENT_SOURCE_DIR}/include/precompiled.h)
  elseif(MSVC)
    add_native_precompiled_header(oregonscript ${CMAKE_CURRENT_SOURCE_DIR}/include/precompiled)
  endif()
endif()

install(TARGETS oregonscript DESTINATION lib)
