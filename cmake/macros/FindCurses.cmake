# find Curses (terminal input/output library) includes and library

FIND_PATH(CURSES_INCLUDE_DIR curses.h)
FIND_LIBRARY(CURSES_LIBRARY NAMES curses ncurses xcurses)

IF (CURSES_INCLUDE_DIR AND CURSES_LIBRARY)
    SET(CURSES_FOUND TRUE)
    MESSAGE(STATUS "Found Curses library: ${CURSES_LIBRARY}")
    INCLUDE_DIRECTORIES(${CURSES_INCLUDE_DIR})
ELSE (CURSES_INCLUDE_DIR AND CURSES_LIBRARY)
    SET(CURSES_FOUND FALSE)
    MESSAGE(FATAL_ERROR "** Curses library not found!\n** Your distro may provide a binary for Curses e.g. for ubuntu try apt-get install libncurses-dev")
ENDIF (CURSES_INCLUDE_DIR AND CURSES_LIBRARY)
