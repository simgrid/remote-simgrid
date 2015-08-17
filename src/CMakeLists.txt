# Copyright (C) 2015. The SimGrid Team. All rights reserved.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Affero Licence (see in file LICENCE).

set(COMMON_SOURCES
    rsg/socket.c        rsg/socket.h
    rsg/request.c       rsg/request.h
    rsg/parsespace.c    ../include/rsg/parsespace.h
    rsg/jsmn.c          rsg/jsmn.h)


set(CLIENT_SOURCES
    ${COMMON_SOURCES}
    client/rsg_actor.cpp ../include/rsg/actor.hpp
    client/rsg_async.cpp ../include/rsg/async.hpp
    client/rsg_engine.cpp  ../include/rsg/engine.hpp)

add_executable(rsg_server
               ${COMMON_SOURCES} rsg_server.cpp)
target_link_libraries(rsg_server ${SimGrid_LIBRARY})

add_executable(dumb_client
               ${CLIENT_SOURCES} dumb_client.cpp)
target_link_libraries(dumb_client ${SimGrid_LIBRARY})

add_executable(dumb_server
               ${CLIENT_SOURCES} dumb_server.cpp)
target_link_libraries(dumb_server ${SimGrid_LIBRARY})
