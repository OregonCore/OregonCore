# output generic information about the core and buildtype chosen

message("")
message("* OregonCore revision    : ${HG_REVISION}")
if( UNIX )
  message("* Build binaries in      : ${CMAKE_BUILD_TYPE} mode")
endif()
message("")

# output information about installation-directories and locations

message("* Install core to        : ${CMAKE_INSTALL_PREFIX}")
if( UNIX )
  message("* Install libraries to   : ${LIBSDIR}")
  message("* Install configs to     : ${CONF_DIR}")
endif()
message("")

# Show infomation about the options selected during configuration

if( GAMEPCH )
  message("* Build core w/PCH       : Yes (default)")
else()
  message("* Build core w/PCH       : No")
endif()

if( SCRIPTPCH )
  message("* Build scripts w/PCH    : Yes (default)")
else()
  message("* Build scripts w/PCH    : No")
endif()

if( COREDEBUG )
  message("* Use coreside debug     : Yes")
  add_definitions(-DOREGON_DEBUG)
  if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-g)
  endif()
else()
  message("* Use coreside debug     : No  (default)")
endif()

if( WIN32 )
  if( USE_MYSQL_SOURCES )
    message("* Use MySQL sourcetree   : Yes (default)")
  else()
    message("* Use MySQL sourcetree   : No")
  endif()
endif( WIN32 )

if( WARNINGS )
  message("* Show all warnings      : Yes")
  if( UNIX )
    add_definitions(-Wall -Wfatal-errors -Wextra)
  endif()
else()
  message("* Show compile-warnings  : No  (default)")
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
