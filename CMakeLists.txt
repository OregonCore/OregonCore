project(Oregon)
cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0005 OLD)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

#
# Basic packagesearching and setup (further support will be needed, this is a preliminary release!)
#

include(CheckIncludeFiles)

if(WIN32)
  set(ACE_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/dep/acelite)
endif()

find_package(PCHSupport)
find_package(ACE REQUIRED)
find_package(MySQL REQUIRED)
find_package(OpenSSL REQUIRED)
if( UNIX )
  find_package(Readline)
  find_package(ZLIB)
  find_package(BZip2)
endif()


#
# Select the Release build configuration by default.
#

if( NOT CMAKE_BUILD_TYPE )
  set(CMAKE_BUILD_TYPE "Release")
endif()

#####
##### Options and settings
#####

#
# Set up default option-parameters for building
#

option(DO_SCRIPTS "With oregonscripts" 1)
option(DO_CLI "With CLI" 1)
option(DO_DEBUG "Debug mode" 0)
option(SCRIPTPCH "Use precompiled headers when compiling scripts project" 1)
option(DO_WARN "Enable all compile warnings" 0)

#
# Search for readline on *nixbased systems (CLI-handler)
#

if( UNIX )
  find_readline()
endif()

#
# Set up the installation-prefix
#

if( PREFIX )
  set(CMAKE_INSTALL_PREFIX "${PREFIX}")
endif()


#
# Handle core debugmode compilation (this will require further work for proper WIN32-setups)
#

if( DO_DEBUG )
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
endif()

#
# Output some generic information about the core and buildtype chosen
#

message("")
message("* OregonCore revision   : ${HG_REVISION}")
if( UNIX )
  message("* Build binaries in     : ${CMAKE_BUILD_TYPE} mode")
endif()
message("")

#
# Output information about installation-directories and locations
#

message("* Install core to       : ${CMAKE_INSTALL_PREFIX}")
if( UNIX )
  message("* Install libraries to  : ${LIBSDIR}")
  message("* Install configs to    : ${CONF_DIR}")
endif()
message("")

#
# Show infomation about the options selected during configuration
#

if(DO_SCRIPTS)
  message("* Build with scripts    : Yes (default)")
  set(SCRIPT_LIB oregonscript)
  set(SCRIPT_INCLUDE src/bindings/scripts/include)
  ADD_DEFINITIONS(-DDO_SCRIPTS)
else()
  message("* Build with scripts    : No")
  set(SCRIPTPCH 0)
  set(SCRIPT_LIB oregoninterface)
  set(SCRIPT_INCLUDE src/bindings/interface)
endif()

if( SCRIPTPCH )
  message("* Build scripts w/PCH   : Yes (default)")
else()
  message("* Build scripts w/PCH   : No")
endif()

if(DO_CLI)
  message("* Build with CLI        : Yes (default)")
  add_definitions(-DENABLE_CLI)
else()
  message("* Build with CLI        : No")
endif()

if(DO_DEBUG)
  message("* Use coreside debug    : Yes")
  add_definitions(-DOREGON_DEBUG)
  if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-g)
  endif()
else()
  message("* Use coreside debug    : No  (default)")
endif()

if( DO_WARN )
  message("* Show all warnings     : Yes")
  if( UNIX )
    add_definitions(-Wall -Wfatal-errors -Wextra)
  endif()
else()
  message("* Show compile-warnings : No  (default)")
  if( UNIX )
    add_definitions(--no-warnings) # makes build look nice, no warnings shown at all, only errors
  elseif( WIN32 )
    # Disable warnings in Visual Studio 8 and above
    if(MSVC AND NOT CMAKE_GENERATOR MATCHES "Visual Studio 7")
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4996 /wd4355 /wd4244 /wd4985 /wd4267 /wd4619")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4996 /wd4355 /wd4244 /wd4985 /wd4267 /wd4619")
    endif()
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  endif()
endif()


message("")

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

#
# Set RPATH-handing (CMake parameters)
#

set(CMAKE_SKIP_BUILD_RPATH 0)
set(CMAKE_BUILD_WITH_INSTALL_RPATH 0)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH 1)

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

add_subdirectory(dep)
add_subdirectory(src)
add_subdirectory(sql)
