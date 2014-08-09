# Copyright (C) 2008-2012 OregonCore <http://www.oregoncore.com/>
# Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

# Add Colors
if (NOT WIN32)
    string(ASCII 27 Esc)
    function(ShowOption opt val)
        string(SUBSTRING ${val} 0 2 ye_no)

        if (${ye_no} STREQUAL "Ye")
                string(SUBSTRING ${val} 3 -1 suffix)
            set(val "${Esc}[1;32mYes${Esc}[m${suffix}") # bold green
        elseif(${ye_no} STREQUAL "No")
                string(SUBSTRING ${val} 2 -1 suffix)
            set(val "${Esc}[1;31mNo${Esc}[m${suffix}") # bold red
        else()
            set(val "${Esc}[1m${val}${Esc}[m") # bold default
        endif()
        
        set(star "${Esc}[1;34m* ${Esc}[m") # bold blue
        set(opt  "${Esc}[1;33m${opt}${Esc}[m") # bold yellow
        message("${star}${opt} ${val}")
    endfunction()
else()
    function(ShowOption opt val)
        message("* ${opt} ${val}")
    endfunction()
endif()

# output generic information about the core and buildtype chosen
message("")
ShowOption("OregonCore revision    :" "${rev_id_str} (${rev_hash_str})")
if( UNIX )
  ShowOption("Build binaries in      :" "${CMAKE_BUILD_TYPE} mode")
endif()
message("")

# output information about installation-directories and locations

ShowOption("Install core to        :" "${CMAKE_INSTALL_PREFIX}/bin")
if( UNIX )
  ShowOption("Install libraries to   :" "${LIBSDIR}")
  ShowOption("Install configs to     :" "${CONF_DIR}")
endif()
message("")

# Show infomation about the options selected during configuration

if( SERVERS )
  ShowOption("Build world/auth       :" "Yes (default)")
else()
  ShowOption("Build world/authserver :" "No")
endif()

if( SCRIPTS )
  ShowOption("Build with scripts     :" "Yes (default)")
  add_definitions(-DSCRIPTS)
else()
  ShowOption("Build with scripts     :" "No")
  set(USE_SCRIPTPCH 0)
endif()

if( TOOLS )
  ShowOption("Build map extractiors  :" "Yes")
  add_definitions(-DMMAP_GENERATOR)
else()
  ShowOption("Build map extractors   :" "No  (default)")
endif()

message("")

if( USE_COREPCH )
  ShowOption("Build core w/PCH       :" "Yes (default)")
else()
  ShowOption("Build core w/PCH       :" "No")
endif()

if( USE_SCRIPTPCH )
  ShowOption("Build scripts w/PCH    :" "Yes (default)")
else()
  ShowOption("Build scripts w/PCH    :" "No")
endif()

if( USE_SFMT )
  ShowOption("Use SFMT for RNG       :" "Yes")
  add_definitions(-DUSE_SFMT_FOR_RNG)
else()
  ShowOption("Use SFMT for RNG       :" "No  (default)")
endif()

if( WITH_WARNINGS )
  ShowOption("Show all warnings      :" "Yes")
  add_definitions(-D__SHOW_STUPID_WARNINGS__)
else()
  ShowOption("Show compile-warnings  :" "No  (default)")
endif()

if( WITH_COREDEBUG )
  ShowOption("Use coreside debug     :" "Yes")
  add_definitions(-DOREGON_DEBUG)
else()
  ShowOption("Use coreside debug     :" "No  (default)")
endif()

if( WIN32 )
  if( USE_MYSQL_SOURCES )
    ShowOption("Use MySQL sourcetree   :" "Yes (default)")
  else()
    ShowOption("Use MySQL sourcetree   :" "No")
  endif()
endif( WIN32 )

message("")
