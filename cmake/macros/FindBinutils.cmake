
FIND_PATH(BFD_INCLUDE_DIR bfd.h
          PATH_SUFFIXES libbfd)

FIND_LIBRARY(BFD_LIBRARY NAMES bfd) 
FIND_LIBRARY(IBERTY_LIBRARY NAMES iberty) 

IF (IBERTY_LIBRARY AND BFD_LIBRARY AND BFD_INCLUDE_DIR)
    SET(BINUTILS_FOUND TRUE)
    MESSAGE(STATUS "Found Binutils")
    INCLUDE_DIRECTORIES(${BFD_INCLUDE_DIR})
    INCLUDE_DIRECTORIES(${IBERTY_INCLUDE_DIR})
ELSE ()
    SET(BINUTILS_FOUND FALSE)
    MESSAGE(FATAL_ERROR "** Binutils were not found!\n** Your distro may provide a package for binutils e.g. for ubuntu try apt-get install binutils-dev")
ENDIF ()
