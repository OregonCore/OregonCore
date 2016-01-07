# This file is part of the OregonCore Project. See AUTHORS file for Copyright information
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

if ( WITH_DOCS )
    find_package(Doxygen)

    if(DOXYGEN_FOUND)
 
        configure_file(
          "${CMAKE_SOURCE_DIR}/Doxyfile.in.cmake"
          "${CMAKE_BINARY_DIR}/Doxyfile"
          @ONLY)
        
        add_custom_target(docs
          COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/Doxyfile
          COMMAND cmake -E copy_directory "${CMAKE_SOURCE_DIR}/doc/doxygen/Images" "${CMAKE_BINARY_DIR}/doc/html/Images"
          WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
          COMMENT "Generating API documentation with Doxygen" VERBATIM)

        install(DIRECTORY ${CMAKE_BINARY_DIR}/doc DESTINATION .)
    else()
        set(WITH_DOCS 0)
    endif()
endif()
