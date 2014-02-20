# Copyright (C) 2008-2014 OregonCore <http://www.oregoncore.com/>
# Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

if( SERVERS )
  set(sources_Debugging
    ${CMAKE_SOURCE_DIR}/src/shared/Debugging/WheatyExceptionReport.cpp
    ${CMAKE_SOURCE_DIR}/src/shared/Debugging/WheatyExceptionReport.h
  )
  add_subdirectory(genrev)
  add_subdirectory(framework)
  add_subdirectory(shared)
  add_subdirectory(oregonrealm)
  add_subdirectory(collision)
  add_subdirectory(game)
  add_subdirectory(scripts)
  add_subdirectory(oregoncore)
else()
  if( TOOLS )
    add_subdirectory(collision)
  endif()
endif()

if(TOOLS)
  add_subdirectory(tools)
endif(TOOLS)
