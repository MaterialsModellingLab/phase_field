# Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
# SPDX-License-Identifier: Apache-2.0

add_library(std::filesystem INTERFACE IMPORTED)
set_property(TARGET std::filesystem APPEND PROPERTY INTERFACE_COMPILE_FEATURES cxx_std_17)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set_property(TARGET std::filesystem APPEND PROPERTY INTERFACE_LINK_LIBRARIES stdc++fs)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_FLAGS MATCHES "-stdlib=libc\\+\\+")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7.0)
    set_target_properties(std::filesystem PROPERTIES INTERFACE_LINK_LIBRARIES c++experimental)
  elseif(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
    set_target_properties(std::filesystem PROPERTIES INTERFACE_LINK_LIBRARIES c++fs)
  endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "FujitsuClang")
  if(CMAKE_CXX_FLAGS EQUAL "-stdlib=libc\\+\\+17")
    set_target_properties(std::filesystem PROPERTIES INTERFACE_LINK_LIBRARIES c++fs)
  else()
    set_target_properties(std::filesystem PROPERTIES INTERFACE_LINK_LIBRARIES stdc++fs)
  endif()
endif()
