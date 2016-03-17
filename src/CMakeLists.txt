# Copyright (C) 2015. The SimGrid Team. All rights reserved.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Affero Licence (see in file LICENCE).

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${CMAKE_SOURCE_DIR}/proto/rsg.proto)
include_directories(${PROTOBUF_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(COMMON_SOURCES
    rsg/socket.cpp      rsg/socket.hpp
    ${PROTO_SRCS}       ${PROTO_HDRS}
)

set(CLIENT_SOURCES
    ${COMMON_SOURCES}
    client/rsg_actor.cpp   ../include/rsg/actor.hpp
    client/rsg_engine.cpp  ../include/rsg/engine.hpp
    client/rsg_host.cpp    ../include/rsg/host.hpp
    client/rsg_mailbox.cpp ../include/rsg/mailbox.hpp
    )

#include_directories(../opt/grpc/include/ SYSTEM)
#include_directories(../opt/protobuf3/include SYSTEM)

add_library(rsg SHARED ${CLIENT_SOURCES})
target_link_libraries(rsg ${SimGrid_LIBRARY} ${PROTOBUF_LIBRARIES})

add_executable(rsg_server
               ${COMMON_SOURCES} rsg_server.cpp)
target_link_libraries(rsg_server
                      ${SimGrid_LIBRARY} ${PROTOBUF_LIBRARIES})
