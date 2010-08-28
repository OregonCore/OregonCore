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
endif( UNIX )

CONFIGURE_FILE(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)
add_custom_target(uninstall "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

option(DO_SCRIPTS "With oregonscripts" 1)
option(DO_CLI "With CLI" 1)
option(DO_DEBUG "Debug mode" 0)
if( UNIX )
  option(CENTOS "CENTOS" 0)
  option(DO_WARN "Enable all compile warnings" 0)
  option(SHORT_SLEEP "Short sleep" 0)
endif( UNIX )

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

message("* OregonCore revision: ${HG_REVISION}")

if(PREFIX)
    set(CMAKE_INSTALL_PREFIX ${PREFIX})
endif(PREFIX)

if(CONF_DIR)

else(CONF_DIR)
    set(CONF_DIR ${PREFIX}/etc)
endif(CONF_DIR)
set(LIBSDIR ${CMAKE_INSTALL_PREFIX}/lib)

message("* Will install to: ${CMAKE_INSTALL_PREFIX}")
message("* With config dir at: ${CONF_DIR}")
message("* Libs install dir at: ${LIBSDIR}")


if(DO_SCRIPTS)
    message("* With Oregon Scripts")
    set(SCRIPT_LIB oregonscript)
    set(SCRIPT_INCLUDE src/bindings/scripts/include)
    ADD_DEFINITIONS(-DDO_SCRIPTS)
else (DO_SCRIPTS)
    message("* Without Oregon Scripts")
    set(SCRIPT_LIB oregoninterface)
    set(SCRIPT_INCLUDE src/bindings/interface)
endif(DO_SCRIPTS)

message("-- Miscellaneus options:")

if(DO_CLI)
    message("* With CLI")
    add_definitions(-DENABLE_CLI)
else (DO_CLI)
    message(* Without CLI)
endif(DO_CLI)

if(DO_DEBUG)
    message("* Debug mode ON")
    add_definitions(-DOREGON_DEBUG)
    if(CMAKE_COMPILER_IS_GNUCXX)
        add_definitions(-g)
    endif(CMAKE_COMPILER_IS_GNUCXX)
endif(DO_DEBUG)

if( UNIX )
  if( DO_WARN )
    message("* All warnings mode")
    add_definitions(-Wall -Wfatal-errors -Wextra)
  else( DO_WARN )
    add_definitions(--no-warnings) # makes build look nice, no warnings shown at all, only errors
  endif( DO_WARN )
endif( UNIX )

if(SHORT_SLEEP)
message("* System sleep time is 50ms")
add_definitions(-DSHORT_SLEEP)
else(SHORT_SLEEP)
message("* System sleep time is 100ms")
endif(SHORT_SLEEP)

if(UNIX)
    if(CENTOS)
        add_definitions(-DCENTOS)
        message("* Building with termcap")
    else(CENTOS)
        message("* Building with readline")
    endif(CENTOS)
endif(UNIX)

IF(UNIX AND NOT CENTOS)
  FIND_READLINE()
  IF(READLINE_FOUND)
  message(STATUS "Found Readline library: ${READLINE_LIBRARY}")
  message(STATUS "Include dir is: ${READLINE_INCLUDE_DIR}")
  ELSE(READLINE_FOUND)
  message(SEND_ERROR "** Readline library not found!")
  message(SEND_ERROR "** Your distro may provide a binary for Readline e.g. for ubuntu try apt-get install libreadline5-dev")
  return()
  ENDIF(READLINE_FOUND)
ENDIF(UNIX AND NOT CENTOS)

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
  set(OSX_LIBS /opt/local/lib/libcrypto.dylib)
  add_definitions(-D__ASSERTMACROS__)
endif(CMAKE_SYSTEM_NAME MATCHES "Darwin")

set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

add_subdirectory(dep)
#add_subdirectory(doc)
add_subdirectory(src)
add_subdirectory(sql)
