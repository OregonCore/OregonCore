add_subdirectory(g3dlite)
add_subdirectory(gsoap)
add_subdirectory(sockets)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
  add_subdirectory(bzip2)
  add_subdirectory(zlib)
endif()

########### install files ###############