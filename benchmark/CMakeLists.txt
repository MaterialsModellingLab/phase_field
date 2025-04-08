# Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
# SPDX-License-Identifier: Apache-2.0

find_package(benchmark REQUIRED)

function(add_gbench_target TEST_NAME)
  set(TARGET "bench-${TEST_NAME}")
  add_executable(${TARGET})
  target_sources(${TARGET}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/${TEST_NAME}.cc)
  target_include_directories(${TARGET}
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    ${GTEST_INCLUDE_DIR})
  target_link_libraries(${TARGET} ${PROJECT_NAME} benchmark::benchmark)
endfunction()

add_gbench_target("predict")
