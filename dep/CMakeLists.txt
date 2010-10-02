if(CMAKE_SYSTEM_NAME MATCHES "Windows")
  add_subdirectory(acelite)
  if(USE_MYSQL_SOURCES)
    add_subdirectory(mysqllite)
  endif()
endif()

add_subdirectory(lib)
add_subdirectory(src)
