file(GLOB_RECURSE sources_vmap vmap/*.cpp vmap/*.h)
file(GLOB_RECURSE sources_auth Auth/*.cpp Auth/*.h)
file(GLOB_RECURSE sources_config Config/*.cpp Config/*.h)
file(GLOB_RECURSE sources_database Database/*.cpp Database/*.h)

file(GLOB sources_localdir *.cpp *.h)

SET(shared_STAT_SRCS
  ${shared_STAT_SRCS}
  ${sources_vmap}
  ${sources_auth}
  ${sources_config}
  ${sources_database}
  ${sources_localdir}
  Debugging/Errors.h
)

include_directories(
  ${ACE_INCLUDE_DIR}
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/dep/include
  ${CMAKE_SOURCE_DIR}/dep/include/g3dlite
  ${CMAKE_SOURCE_DIR}/dep/include/sockets
  ${CMAKE_SOURCE_DIR}/src/framework
  ${CMAKE_SOURCE_DIR}/src/shared
  ${CMAKE_SOURCE_DIR}/src/shared/Database
  ${MYSQL_INCLUDE_DIR}
  ${OPENSSL_INCLUDE_DIR}
)

add_library(shared STATIC ${shared_STAT_SRCS})

if(WIN32)
  target_link_libraries(shared
    ace
  )
else()
  target_link_libraries(shared
    ${ACE_LIBRARY}
  )
endif()
