if(CMAKE_SYSTEM_NAME MATCHES "Windows")
  add_subdirectory(acelite)
endif()

add_subdirectory(lib)
add_subdirectory(src)
