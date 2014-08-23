# Set build-directive (used in core to tell which buildtype we used)
add_definitions(-D_BUILD_DIRECTIVE="${CMAKE_BUILD_TYPE}")

if( USE_SFMT)
  if(PLATFORM EQUAL 32)
    # Required on 32-bit systems to enable SSE2 (standard on x64)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse2 -mfpmath=sse")
  endif()
  add_definitions(-DHAVE_SSE2 -D__SSE2__)
  message(STATUS "Clang: SFMT enabled, SSE2 flags forced")
endif()

if( WITH_WARNINGS )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wfatal-errors -Wextra")
  message(STATUS "Clang: All warnings enabled")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --no-warnings")
  message(STATUS "Clang: All warnings disabled")
endif()

if( WITH_COREDEBUG )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb -g3 -O0")
  message(STATUS "Clang: Debug-flags set (-ggdb -g3 -O0)")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif()

function(add_clang_pch target dir header cpp)
    get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
    get_property(defs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY COMPILE_DEFINITIONS)
    foreach(def ${defs})
      set(definitions "${definitions} -D${def}")
    endforeach()
    foreach(_dir ${dirs})
      set(includes "${includes} -I${_dir}")
    endforeach()

    # helper target for triggering PCH re-generation
    add_library(${target}PCH-Trigger STATIC "${dir}/${cpp}")

    separate_arguments(args UNIX_COMMAND "-x c++-header --relocatable-pch -isysroot ${CMAKE_CURRENT_BINARY_DIR} ${dir}/${header} -o ${header}.pch ${includes} ${CMAKE_CXX_FLAGS} ${definitions}")
    add_custom_command(
        OUTPUT "${header}.pch"
        COMMAND ${CMAKE_CXX_COMPILER} ARGS ${args}
        DEPENDS "${target}PCH-Trigger"
    )
    add_custom_target("${target}PCH" DEPENDS "${header}.pch")
    add_dependencies("${target}PCH" "${target}PCH-Trigger")
    add_dependencies(${target} "${target}PCH")
    set_target_properties(${target} PROPERTIES COMPILE_FLAGS "-include-pch ${CMAKE_CURRENT_BINARY_DIR}/${header}.pch")
endfunction()
