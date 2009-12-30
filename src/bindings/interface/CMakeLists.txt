
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

add_library(oregoninterface SHARED ${oregoninterface_LIB_SRCS})

target_link_libraries(oregoninterface)

set_target_properties(oregoninterface PROPERTIES VERSION 4.2.0 SOVERSION 4)
install(TARGETS oregoninterface DESTINATION lib)
