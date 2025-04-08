# Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
# SPDX-License-Identifier: Apache-2.0

# Set the project version based on the git tag
# If the tag is in the format vX.Y.Z, set the version to X.Y.Z
# If the tag is in the format vX.Y.Z-<something>, set the version to X.Y.Z+<something>

# Usage;
#   include(cmake/git-version.cmake)
#   get_git_version()
#   message(STATUS "Version: ${PROJECT_VERSION}")

function(get_git_version)
  find_package(Git QUIET)
  if (NOT Git_FOUND)
    message(FATAL_ERROR "Git not found")
    return()
  endif()

  execute_process(
    COMMAND ${GIT_EXECUTABLE} describe --tags
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_TAG
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE GIT_RESULT
  )

  execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  # Set default project version
  set(PROJECT_VERSION_MAJOR 0 PARENT_SCOPE)
  set(PROJECT_VERSION_MINOR 0 PARENT_SCOPE)
  set(PROJECT_VERSION_PATCH 0 PARENT_SCOPE)
  set(FULL_VERSION "0.0.0 (${GIT_COMMIT_HASH})" PARENT_SCOPE)
  set(PROJECT_VERSION "0.0.0" PARENT_SCOPE)

  if (NOT GIT_RESULT EQUAL 0)
    message(WARNING "No git tag found. Using 0.0.0")
    return()
  endif()

  string(REGEX REPLACE "^v" "" CLEAN_TAG "${GIT_TAG}")
  if(CLEAN_TAG MATCHES "^([0-9]+)\\.([0-9]+)\\.([0-9]+)(-.*)?$")
    set(PROJECT_VERSION_MAJOR ${CMAKE_MATCH_1} PARENT_SCOPE)
    set(PROJECT_VERSION_MINOR ${CMAKE_MATCH_2} PARENT_SCOPE)
    set(PROJECT_VERSION_PATCH ${CMAKE_MATCH_3} PARENT_SCOPE)

    set(FULL_VERSION "${CMAKE_MATCH_1}.${CMAKE_MATCH_2}.${CMAKE_MATCH_3} (${GIT_COMMIT_HASH})" PARENT_SCOPE)
    set(PROJECT_VERSION "${CMAKE_MATCH_1}.${CMAKE_MATCH_2}.${CMAKE_MATCH_3}" PARENT_SCOPE)
  else ()
    message(WARNING "Tag '${CLEAN_TAG}' does not semantic version format. Using 0.0.0")
  endif()
endfunction(get_git_version)
