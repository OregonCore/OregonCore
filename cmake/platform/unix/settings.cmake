# Package overloads - Linux
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
  set(JEMALLOC_LIBRARY "jemalloc")
endif()

# set default configuration directory
if( NOT CONF_DIR )
  set(CONF_DIR ${CMAKE_INSTALL_PREFIX}/etc)
  message(STATUS "UNIX: Using default configuration directory")
endif()

# set default library directory
if( NOT LIBSDIR )
  set(LIBSDIR ${CMAKE_INSTALL_PREFIX}/lib)
  message(STATUS "UNIX: Using default library directory")
endif()

# configure uninstaller
configure_file(
  "${CMAKE_SOURCE_DIR}/cmake/platform/cmake_uninstall.in.cmake"
  "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake"
  @ONLY
)
message(STATUS "UNIX: Configuring uninstall target")

# create uninstaller target (allows for using "make uninstall")
add_custom_target(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake"
)
message(STATUS "UNIX: Created uninstall target")

# prevent using implicit compiler because it is very low-featured,
# use distributed gcc instead
if (CMAKE_CXX_COMPILER MATCHES "/c\\+\\+$")
    set(CMAKE_C_COMPILER "/usr/bin/gcc")
    set(CMAKE_CPP_COMPILER "/usr/bin/gcc -E")
    set(CMAKE_CXX_COMPILER "/usr/bin/g++")
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  include(${CMAKE_SOURCE_DIR}/cmake/compiler/gcc/settings.cmake)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  include(${CMAKE_SOURCE_DIR}/cmake/compiler/clang/settings.cmake)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
  include(${CMAKE_SOURCE_DIR}/cmake/compiler/icc/settings.cmake)
endif()
