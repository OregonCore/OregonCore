add_subdirectory(g3dlite)

if(SERVERS)
  add_subdirectory(gsoap)
  add_subdirectory(sockets)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
  add_subdirectory(bzip2)
  add_subdirectory(zlib)
endif()

########### install files ###############