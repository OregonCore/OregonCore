# Set projectname (must be done AFTER setting configurationtypes)
project(Oregon)

# CMake policies (can not be handled elsewhere)
cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0005 OLD)

# Set RPATH-handing (CMake parameters)
set(CMAKE_SKIP_BUILD_RPATH 0)
set(CMAKE_BUILD_WITH_INSTALL_RPATH 0)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH 1)

# set macro-directory
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# build in Release-mode by default if not explicitly set
if( NOT CMAKE_BUILD_TYPE )
  set(CMAKE_BUILD_TYPE "Release")
endif()

include(CheckIncludeFiles)

# set default buildoptions and print them
include(cmake/options.cmake)

if(WIN32)
  set(ACE_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/dep/acelite)
endif()

find_package(PCHSupport)
find_package(ACE REQUIRED)
find_package(OpenSSL REQUIRED)
if( NOT USE_MYSQL_SOURCES )
  find_package(MySQL REQUIRED)
endif()

if( UNIX )
  find_package(Readline)
  find_package(ZLIB)
  find_package(BZip2)
endif()

#
# Search for readline on *nixbased systems (CLI-handler)
#

if( UNIX )
  find_readline()
endif()

if( USE_MYSQL_SOURCES )
  set(MYSQL_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/dep/mysqllite/include)
  set(MYSQL_LIBRARY "libmysql")
  set( MYSQL_FOUND 1 )
  message(STATUS "Using internal MySQL.")
endif()

#
# Set up the installation-prefix
#

if( PREFIX )
  set(CMAKE_INSTALL_PREFIX "${PREFIX}")
endif()

#
# Example: Check the CMake preload parameters (Commented out by default)
#
# Overload CMAKE_INSTALL_PREFIX if not being set properly
#if( WIN32 )
#  if( NOT CYGWIN )
#    if( NOT CMAKE_INSTALL_PREFIX )
#      set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/bin")
#    endif()
#  endif()
#endif()

#
# Handle core debugmode compilation (this will require further work for proper WIN32-setups)
#

if( COREDEBUG )
  set(CMAKE_BUILD_TYPE Debug)
endif()

#
# WINDOWS/MSVC: Set up paths for where we want binaries to end up after compilation
#

if( MSVC )
  # Set up MSVC to dump files in the <builddir>/bin/<buildtype>/ folder for testing builds before install

  # executable binaries (.exe-files, and .dll-files on DLL-capable platforms)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

  # other parts like static libraries etc - commented out as it's not needed - shown here as an example only
  #set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
  #set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif()

#
# Set default configuration-directory (used on NIX-based platforms only)
#

if( NOT CONF_DIR )
  set(CONF_DIR ${CMAKE_INSTALL_PREFIX}/etc)
endif()

set(LIBSDIR ${CMAKE_INSTALL_PREFIX}/lib)

#####
##### Build-preparation
#####

#
# Create genrev object and create revision.h
# (Moved to subfolder to avoid cluttering up the base build-dir)
#

add_subdirectory(src/tools/genrevision)

#
# Find current revision of downloaded sourcetree
#

execute_process(
  COMMAND hg tip --template {rev}
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  OUTPUT_VARIABLE HG_REVISION
)

if(DEFINED NOPCH)
  set(SCRIPTPCH 0)
  set(GAMEPCH 0)
endif()

#####
##### Tweaks to make things operate well
#####

#
# Little tweak for OS X
#

if( CMAKE_SYSTEM_NAME MATCHES "Darwin" )
  set(MACOSX 1)
  set(OSX_LIBS /usr/lib/libcrypto.dylib)
  add_definitions(-D__ASSERTMACROS__)
endif()

#
# Some small tweaks for Visual Studio 7 and above.  
#

if( MSVC )
  # Mark 32 bit executables large address aware so they can use > 2GB address space
  if(CMAKE_SIZEOF_VOID_P MATCHES 4)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LARGEADDRESSAWARE")
  endif()
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

#####
##### Start filling in the blanks, and build the project
#####

#
# Create uninstall-object for UNIX platforms
#

if( UNIX )
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY
  )
  add_custom_target(uninstall
    "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  )
endif()

#
# Enter the different builddirectories and start working 
#

# print out the results before continuing
include(cmake/showoptions.cmake)

# add dependencies
add_subdirectory(dep)

# add core sources
add_subdirectory(src)

# add sql-files
add_subdirectory(sql)
