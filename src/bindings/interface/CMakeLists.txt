
########### next target ###############

SET(oregoninterface_LIB_SRCS
   ScriptMgr.cpp
   ScriptMgr.h
   config.h
   system.cpp
   Scripts/sc_default.cpp
   Scripts/sc_defines.cpp
   Scripts/sc_defines.h
)

include_directories(
  ${ACE_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}/src/shared
  ${CMAKE_SOURCE_DIR}/src/framework
  ${CMAKE_SOURCE_DIR}/src/game
  ${CMAKE_SOURCE_DIR}/src/shared/Database
  ${CMAKE_SOURCE_DIR}/dep/include
  ${MYSQL_INCLUDE_DIR}
)

add_library(oregoninterface SHARED ${oregoninterface_LIB_SRCS})

target_link_libraries(oregoninterface)

set_target_properties(oregoninterface PROPERTIES VERSION 4.2.0 SOVERSION 4)
install(TARGETS oregoninterface DESTINATION lib)
