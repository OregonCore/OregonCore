project(Oregon)
cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0005 OLD)

include(CheckIncludeFiles)
include(cmake/FindAce.cmake)
include(cmake/FindMySql.cmake)
include(cmake/FindReadline.cmake)
CONFIGURE_FILE(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)
add_custom_target(uninstall "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

option(DO_SCRIPTS "With oregonscripts" 1)
option(DO_CLI "With CLI" 1)
option(DO_DEBUG "Debug mode" 0)
option(DO_WARN "Enable all compile warnings" 0)
option(SHORT_SLEEP "Short sleep" 0)
option(CENTOS "CENTOS" 0)

set(GENREV_SRC
src/tools/genrevision/genrevision.cpp
)

ADD_EXECUTABLE(genrev
${GENREV_SRC}
)
ADD_CUSTOM_TARGET("revision.h" ALL
    COMMAND "${Oregon_BINARY_DIR}/genrev"
    ${Oregon_SOURCE_DIR}
    WORKING_DIRECTORY "${Oregon_SOURCE_DIR}/src/shared"
    DEPENDS genrev
)


execute_process(
        COMMAND hg tip --template {rev}
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

find_library(SSLLIB NAMES ssl DOC "SSL library")
find_library(ZLIB z "Zlib library")

FIND_MYSQL()

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

if(DO_WARN)
    message("* All warnings mode")
    add_definitions(-Wall -Wfatal-errors -Wextra)
endif(DO_WARN)

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

FIND_ACE(ACE)
if(ACE_FOUND)
    message(STATUS "Found ACE library: ${ACE_LIBRARY}")
    message(STATUS "Include dir is: ${ACE_INCLUDE_DIR}")
else(ACE_FOUND)
    message(SEND_ERROR "** ACE library not found! Trinity Core cannot be compiled!")
    message(SEND_ERROR "** Please build ACE from http://www.cs.wustl.edu/~schmidt/ACE.html")
    #For now remove msg about install from repo, as ubuntu/debian don't have needed ver in repos.
    #message(SEND_ERROR "** your distro may provide a binary for ACE e.g. for ubuntu try apt-get install libace-dev")
    return()
    #set(BUILD_ACE 1)
    #set(ACE_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/dep/ACE_wrappers ${CMAKE_BINARY_DIR}/dep/ACE_wrappers")
    #set(ACE_LIBRARY ACE)
    #message(STATUS "I will try to build ACE from: ${ACE_INCLUDE_DIR}")
    #message(STATUS "And link using: ${ACE_LIBRARY}")
endif(ACE_FOUND)

#somehow line below don't work. so for now change it to if exist
#check_include_files(${ACE_INCLUDE_DIR}/ace/Stack_Trace.h HAVE_ACE_STACK_TRACE_H)
if(EXISTS ${ACE_INCLUDE_DIR}/ace/Stack_Trace.h)
    set(HAVE_ACE_STACK_TRACE_H 1)
else(EXISTS ${ACE_INCLUDE_DIR}/ace/Stack_Trace.h)
    message(STATUS "** Your libace is out of date. Please update your libace!")
endif(EXISTS ${ACE_INCLUDE_DIR}/ace/Stack_Trace.h)

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


add_definitions(--no-warnings) #to make build look nice, no gcc nazi warnings.

set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

add_subdirectory(dep)
#add_subdirectory(doc)
add_subdirectory(src)
add_subdirectory(sql)
