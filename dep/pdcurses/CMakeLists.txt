# This file is part of the OregonCore Project. See AUTHORS file for Copyright information
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

file(GLOB frontend pdcurses/*.c)
file(GLOB backend win32/*.c)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(curses STATIC ${frontend} ${backend})
