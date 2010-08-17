SET(gsoap_STAT_SRCS
	soapC.cpp
	soapServer.cpp
	stdsoap2.cpp
)

add_library(gsoap STATIC ${gsoap_STAT_SRCS})
