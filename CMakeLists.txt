# Copyright (C) 2015. The SimGrid Team. All rights reserved.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Affero Licence (see in file LICENCE).

cmake_minimum_required(VERSION 2.8.10)
project (RemoteSimGrid C CXX)

#To enable pthread
# I found a similar issue here : https://github.com/potree/PotreeConverter/issues/136
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -pthread -lstdc++ -lm")


set(RSG_BINARY_DIR ${CMAKE_BINARY_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${RSG_BINARY_DIR})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/tools/cmake/")

## We need a decent support of the c++11 standard
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=gnu++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
else()
  message(FATAL_ERROR
          "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. "
          "Please use a decent C++ compiler.")
endif()

### And we need C11 standard, too
include(CheckCCompilerFlag)
CHECK_C_COMPILER_FLAG("-std=gnu11" COMPILER_SUPPORTS_C11)
if(COMPILER_SUPPORTS_C11)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11")
else()
  message(FATAL_ERROR
          "The compiler ${CMAKE_C_COMPILER} has no C11 support. "
	  "Please use a decent C compiler "
	  "(note that ${CMAKE_CXX_COMPILER} seems to support c++11).")
endif()


# Search for SimGrid
find_package(SimGrid REQUIRED)
include_directories("${SimGrid_INCLUDE_DIR}" SYSTEM)

#Search for Thrift
find_package(Thrift REQUIRED)

thrift_gen_cpp(RsgService.thrift THRIFT_CPP_FILES_LIST THRIFT_GEN_INCLUDE_DIR)

include_directories(${THRIFT_INCLUDE_DIRS})
include_directories(${THRIFT_GEN_INCLUDE_DIR})

set(CMAKE_C_FLAGS   "-g3 -Wall -Werror ${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "-g3 -Wall -Werror ${CMAKE_CXX_FLAGS}")
include_directories(include)

add_subdirectory(src)
add_subdirectory(java)

# Enable Testing
enable_testing()
add_subdirectory(tools/tesh)
include(${CMAKE_HOME_DIRECTORY}/tools/cmake/Tests.cmake)
add_subdirectory(${PROJECT_SOURCE_DIR}/tests)
