# Copyright (C) 2015. The SimGrid Team. All rights reserved.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Affero Licence (see in file LICENCE).

cmake_minimum_required (VERSION 2.6)
project (RemoteSimGrid C CXX)

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
include(FindSimGrid.cmake)
include_directories("${SimGrid_INCLUDE_DIR}" SYSTEM)


add_subdirectory(src)
