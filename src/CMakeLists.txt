# Copyright (C) 2015. The SimGrid Team. All rights reserved.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Affero Licence (see in file LICENCE).

include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(CLIENT_SOURCES_THRIFT
    ${COMMON_SOURCES}
    client/ClientEngine.cpp ../include/client/ClientEngine.hpp
    server/Socket.cpp ../include/server/Socket.hpp
)

add_executable(main_server main_server.cpp server/Server.cpp server/Socket.cpp RsgServiceImpl.cpp)

add_library(thriftClient SHARED ${CLIENT_SOURCES_THRIFT})

target_link_libraries(main_server "RsgService.thrift")


#include_directories(../opt/grpc/include/ SYSTEM)
#include_directories(../opt/protobuf3/include SYSTEM)

#include_directories(../opt/grpc/include/ SYSTEM)
#include_directories(../opt/protobuf3/include SYSTEM)

target_link_libraries(main_server ${SimGrid_LIBRARY})
