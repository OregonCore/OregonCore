if(CMAKE_SYSTEM_NAME MATCHES "Windows")
  if(SERVERS)
    add_subdirectory(acelite)
    if(USE_MYSQL_SOURCES)
      add_subdirectory(mysqllite)
    endif()
  endif()
endif()

add_subdirectory(src)

if(TOOLS)
  add_subdirectory(libmpq)
endif()