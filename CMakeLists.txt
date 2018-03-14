# Copyright (C) 2015. The SimGrid Team. All rights reserved.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Affero Licence (see in file LICENCE).
cmake_minimum_required(VERSION 2.8.10)
project(RemoteSimGrid C CXX)

# Location of other cmake files
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/tools/cmake/")

# Set default CMAKE_BUILD_TYPE to DEBUG
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Debug' as none was specified.")
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)

  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
               "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# C++11 is required.
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(${COMPILER_SUPPORTS_CXX11})
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
  message(FATAL_ERROR
          "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. "
          "Please use a decent C++ compiler.")
endif()

# C11 is also required.
include(CheckCCompilerFlag)
CHECK_C_COMPILER_FLAG("-std=c11" COMPILER_SUPPORTS_C11)
if(COMPILER_SUPPORTS_C11)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
else()
  message(FATAL_ERROR
          "The compiler ${CMAKE_C_COMPILER} has no C11 support. "
	        "Please use a decent C compiler "
	        "(note that ${CMAKE_CXX_COMPILER} seems to support c++11).")
endif()

################
# Dependencies #
################

# Search for SimGrid
find_package(SimGrid REQUIRED)
include_directories("${SimGrid_INCLUDE_DIR}" SYSTEM)

# Search for ZeroMQ
find_package(ZMQ REQUIRED)

# Search for Thrift.
# Should work with versions 0.9.3 and 0.10.0. Breaks with 0.11.0
find_package(Thrift REQUIRED)
if(NOT (${THRIFT_VERSION_STRING} VERSION_GREATER "0.9.2" AND
        ${THRIFT_VERSION_STRING} VERSION_LESS "0.11.0"))
  message(FATAL_ERROR "Remote-Simgrid require Thrift 0.9.3 or 0.10.0.")
endif()

###############
# Build setup #
###############
# Options
option(enable_debug_logs "Enable debugging logs" OFF)
option(enable_warnings "Enable compilation warnings" ON)
option(treat_warnings_as_errors
       "Treat compilation warnings as compilation errors" OFF)

# Enable pthread (https://github.com/potree/PotreeConverter/issues/136)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

set(RSG_INSTALL_DIR ${CMAKE_INSTALL_PREFIX})
set(RSG_BINARY_DIR ${CMAKE_BINARY_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${RSG_BINARY_DIR})

# Enable warnings if needed
if (enable_warnings)
  set(warning_flags " -Wall -Wextra")

  if (treat_warnings_as_errors)
    set(warning_flags "${warning_flags} -Werror")
  endif()

  if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_C_FLAGS "${warning_flags} ${CMAKE_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${warning_flags} ${CMAKE_CXX_FLAGS}")
  elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_C_FLAGS "${warning_flags} ${CMAKE_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${warning_flags} ${CMAKE_CXX_FLAGS}")
  else()
    message(WARNING
            "Unknown compiler. Warnings may not be enabled correctly.")
    set(CMAKE_C_FLAGS "${warning_flags} ${CMAKE_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${warning_flags} ${CMAKE_CXX_FLAGS}")
  endif()
endif()

# Print debugging logs if needed
if (enable_debug_logs)
  set(dbglog_flags " -DDEBUG_TRACE")

  set(CMAKE_C_FLAGS "${dbglog_flags} ${CMAKE_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${dbglog_flags} ${CMAKE_CXX_FLAGS}")
endif()

thrift_gen_cpp(RsgService.thrift THRIFT_CPP_FILES_LIST THRIFT_GEN_INCLUDE_DIR)
include_directories(${THRIFT_INCLUDE_DIRS})
include_directories(${THRIFT_GEN_INCLUDE_DIR})

include_directories(include)
add_subdirectory(src)

# Enable Testing
enable_testing()
include(${CMAKE_HOME_DIRECTORY}/tools/cmake/Tests.cmake)
add_subdirectory(${PROJECT_SOURCE_DIR}/tests)

###########
# Install #
###########
FILE(GLOB THRIFT_GEN_INCLUDE_FILES ${THRIFT_GEN_INCLUDE_DIR}/*.h*)
install(DIRECTORY include DESTINATION ${RSG_INSTALL_DIR})
install(FILES ${THRIFT_GEN_INCLUDE_FILES}
        DESTINATION ${RSG_INSTALL_DIR}/include)
