# This file is part of the OregonCore Project. See AUTHORS file for Copyright information
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

file(GLOB sources src/*.c)

set(jemalloc_STAT_SRC
  ${sources}
)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_BINARY_DIR}/include
)

add_definitions(-D_GNU_SOURCE -D_REENTRANT)
add_definitions(-std=gnu99 -pipe -fvisibility=hidden -O3 -funroll-loops --no-warnings)

add_custom_command(
    OUTPUT jemallocs_internals
    COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/configure CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER} --quiet
    COMMAND cmake -E touch jemallocs_internals
)
add_custom_target(jemalloc_internals DEPENDS jemallocs_internals)
add_library(jemalloc STATIC ${sources})
add_dependencies(jemalloc jemalloc_internals)
