# Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.27 FATAL_ERROR)
project(phase_field LANGUAGES CXX DESCRIPTION "C++ Phase Field Library and Tools")

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/.local/" CACHE PATH "Default install path" FORCE)
endif()

# Define the executable name
set(PHASE_FIELD_2D_EXEC_NAME "${PROJECT_NAME}_2d")

# Option
option(BUILD_TESTING "Build Unit Tests" OFF)
option(BUILD_BENCHMARK "Build Benchmark" OFF)

# Dependencies
if(CMAKE_CXX_COMPILER_ID STREQUAL "FujitsuClang")
  add_library(OpenMP::OpenMP_CXX INTERFACE IMPORTED)
  set_property(
    TARGET OpenMP::OpenMP_CXX PROPERTY
    INTERFACE_LINK_LIBRARIES -fopenmp)
else()
  find_package(OpenMP REQUIRED)
endif()
find_package(libtensor 0.0.0 REQUIRED)
find_package(nlohmann_json REQUIRED)

# Inlcude local module
include(${CMAKE_SOURCE_DIR}/cmake/clang_format.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/cppcheck.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/filesystem.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/git-version.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/install.cmake)

# Get git tag as PROJECT_VERSION
get_git_version()

set(TARGET ${PROJECT_NAME})
add_library(${TARGET} INTERFACE)
target_include_directories(${TARGET} INTERFACE
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>)
target_link_libraries(${TARGET}
  INTERFACE libtensor::libtensor nlohmann_json::nlohmann_json std::filesystem)

set(TARGET ${PHASE_FIELD_2D_EXEC_NAME})
add_executable(${TARGET})
target_sources(${TARGET}
  PRIVATE ${CMAKE_SOURCE_DIR}/src/${PROJECT_NAME}_2d.cc)
target_link_libraries(${TARGET} PRIVATE ${PROJECT_NAME})

if(BUILD_TOOLS)
  find_package(VTK COMPONENTS CommonCore CommonDataModel IOXML)
  set(TARGET dat2vtk_2d)
  add_executable(${TARGET})
  target_sources(${TARGET}
    PRIVATE ${CMAKE_SOURCE_DIR}/src/${TARGET}.cc)
  target_link_libraries(${TARGET} PRIVATE ${PROJECT_NAME} ${VTK_LIBRARIES})
  vtk_module_autoinit(TARGETS ${TARGET} MODULES ${VTK_LIBRARIES})
endif()

if(BUILD_BENCHMARK)
  add_subdirectory(benchmark)
endif()

library_install(${PROJECT_NAME})

get_target_property(DEFINES ${PHASE_FIELD_2D_EXEC_NAME} COMPILE_DEFINITIONS)
message(STATUS "<<< Build configuration >>>
    Version:           ${PROJECT_VERSION}
    Operating System:  ${CMAKE_SYSTEM_NAME} ${CMAKE_LINUX_DISTRO} ${CMAKE_DISTRO_VERSION}
    CMake Version:     ${CMAKE_VERSION}
    Install path:      ${CMAKE_INSTALL_PREFIX}
    Generator:         ${CMAKE_GENERATOR} using ${CMAKE_MAKE_PROGRAM}")

string(TOUPPER "${CMAKE_BUILD_TYPE}" BTYPE)
message(STATUS "<<< Compilers and Flags: >>>
    C++ Compiler:      ${CMAKE_CXX_COMPILER}
    Type:              ${CMAKE_CXX_COMPILER_ID}
    Version:           ${CMAKE_CXX_COMPILER_VERSION}
    C++ Flags:         ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_${BTYPE}}
    Defines:           ${DEFINES}")
