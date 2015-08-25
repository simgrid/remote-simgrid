# Copyright (C) 2015. The SimGrid Team. All rights reserved.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Affero Licence (see in file LICENCE).

ADD_CUSTOM_COMMAND(
	OUTPUT ${CMAKE_BINARY_DIR}/src/rsg.pb.h ${CMAKE_BINARY_DIR}/src/rsg.pb.cc
	COMMAND protoc -I ${CMAKE_SOURCE_DIR}/src --cpp_out=. ${CMAKE_SOURCE_DIR}/src/rsg.proto
)

set(COMMON_SOURCES
    rsg/socket.cpp      rsg/socket.hpp
    rsg.pb.h            rsg.pb.cc
)


set(CLIENT_SOURCES
    ${COMMON_SOURCES}
    client/rsg_actor.cpp   ../include/rsg/actor.hpp
    client/rsg_engine.cpp  ../include/rsg/engine.hpp
    client/rsg_mailbox.cpp ../include/rsg/mailbox.hpp
    )

#include_directories(../opt/grpc/include/ SYSTEM)
#include_directories(../opt/protobuf3/include SYSTEM)

add_library(rsg SHARED ${CLIENT_SOURCES})
target_link_libraries(rsg ${SimGrid_LIBRARY} protobuf)

add_executable(rsg_server 
               ${COMMON_SOURCES} rsg_server.cpp)
target_link_libraries(rsg_server 
                      ${SimGrid_LIBRARY} protobuf)
