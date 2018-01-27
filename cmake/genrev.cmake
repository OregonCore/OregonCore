# Copyright (C) 2008-2018 OregonCore <https://oregon-core.net/>
# Copyright (C) 2008-2012 TrinityCore <https://www.trinitycore.org/>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

find_package(Git)

execute_process(
  COMMAND "${GIT_EXECUTABLE}" rev-list HEAD --abbrev-commit --count
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  OUTPUT_VARIABLE rev_id_str
  OUTPUT_STRIP_TRAILING_WHITESPACE
  ERROR_QUIET
)

execute_process(
  COMMAND "${GIT_EXECUTABLE}" rev-list HEAD --abbrev-commit -1
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  OUTPUT_VARIABLE rev_hash_str
  OUTPUT_STRIP_TRAILING_WHITESPACE
  ERROR_QUIET
)

execute_process(
  COMMAND "${GIT_EXECUTABLE}" diff --quiet HEAD
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  RESULT_VARIABLE rev_unstaged_changes
)

# Last minute check - ensure that we have a proper revision
# If everything above fails (means the user has erased the git revisional control directory, or runs archive and erased their .git_archival.txt)
if(NOT "${rev_id_str}")
  if (CMAKE_BUILD_TYPE)
      message(STATUS "WARNING - No revision-information found? Although this is not fatal, if you want any support in the future, please install git and clone the source properly - (do not just download files)")
  endif()
  # Ok, since we have no valid ways of finding/setting the revision, let's force some defaults
  set(rev_hash_str "Unknown")
  set(rev_hash "0")
  set(rev_id_str "0")
  set(rev_id "0")
endif()

# Detect is source is actually modified
if(rev_unstaged_changes)
  if (CMAKE_BUILD_TYPE)
      message(STATUS "WARNING - Unstaged (uncommitted) changes were found.")
  endif()
  set(rev_id "${rev_id_str}")
  set(rev_id_str "${rev_id_str}+")
  set(rev_hash_str "${rev_hash_str}+")
endif()

# Its not set during initial run
if(NOT BUILDDIR)
  set(BUILDDIR ${CMAKE_BINARY_DIR})
endif()

# Create the actual revision.h file from the above params
if(NOT "${rev_id_cached}" MATCHES "${rev_id_str}")
  configure_file(
    "${CMAKE_SOURCE_DIR}/revision.h.in.cmake"
    "${BUILDDIR}/revision.h"
    @ONLY
  )
  set(rev_id_cached "${rev_id_str}" CACHE INTERNAL "Cached revision ID")
endif()
