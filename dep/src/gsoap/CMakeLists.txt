SET(gsoap_STAT_SRCS
	soapC.cpp
	soapServer.cpp
	stdsoap2.cpp
)

include_directories(
  ${CMAKE_SOURCE_DIR}/dep/include/gsoap
)

add_library(gsoap STATIC ${gsoap_STAT_SRCS})
