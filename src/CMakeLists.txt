# Copyright (C) 2015. The SimGrid Team. All rights reserved.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Affero Licence (see in file LICENCE).

include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(CLIENT_SOURCES_THRIFT
    ${COMMON_SOURCES}
    client/RsgClientEngine.cpp ../include/client/RsgClientEngine.hpp
    client/rsg_actor.cpp       ../include/rsg/actor.hpp
    client/rsg_mailbox.cpp     ../include/rsg/mailbox.hpp
    client/rsg_comm.cpp        ../include/rsg/comm.hpp
    client/rsg_host.cpp        ../include/rsg/host.hpp
    rsg/Socket.cpp
)

set(SERVER_SOURCES
    ${COMMON_SOURCES}
    main_server.cpp 
    rsg/Server.cpp 
    rsg/Socket.cpp 
    rsg/services/actorService.cpp      ../include/rsg/services/actorService.hpp
    rsg/services/commService.cpp       ../include/rsg/services/commService.hpp
    rsg/services/mailboxService.cpp    ../include/rsg/services/mailboxService.hpp
    rsg/services/hostService.cpp       ../include/rsg/services/actorService.hpp
)


add_executable(rsg_server ${SERVER_SOURCES})

add_library(thriftClient SHARED ${CLIENT_SOURCES_THRIFT})

target_link_libraries(rsg_server "RsgService.thrift")

#include_directories(../opt/grpc/include/ SYSTEM)
#include_directories(../opt/protobuf3/include SYSTEM)

#include_directories(../opt/grpc/include/ SYSTEM)
#include_directories(../opt/protobuf3/include SYSTEM)

target_link_libraries(rsg_server ${SimGrid_LIBRARY})
