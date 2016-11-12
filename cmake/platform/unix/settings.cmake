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

# Some distributions prefer to place libraries elsewhere
list(APPEND CMAKE_PREFIX_PATH "/usr/lib/x86_64-linux-gnu" "/usr/lib/x86_64-linux-gnu")
list(APPEND CMAKE_PREFIX_PATH "/usr/lib/i386-linux-gnu"   "/usr/include/i386-linux-gnu")

# set c++ standard
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please update your C++ compiler or use a different one.")
endif()

# load specific compiler setup
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  include(${CMAKE_SOURCE_DIR}/cmake/compiler/gcc/settings.cmake)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  include(${CMAKE_SOURCE_DIR}/cmake/compiler/clang/settings.cmake)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
  include(${CMAKE_SOURCE_DIR}/cmake/compiler/icc/settings.cmake)
endif()
