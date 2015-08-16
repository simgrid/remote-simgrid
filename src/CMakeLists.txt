# Copyright (C) 2015. The SimGrid Team. All rights reserved.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Affero Licence (see in file LICENCE).

set(COMMON_SOURCES
    socket.c        socket.h
    request.c       request.h
    parsespace.c    ../include/rsg/parsespace.h
    jsmn.c          jsmn.h)


set(CLIENT_SOURCES
    ${COMMON_SOURCES}
    rsg_actor.cpp ../include/rsg/actor.hpp
    rsg_engine.cpp  ../include/rsg/engine.hpp)

add_executable(rsg
               ${COMMON_SOURCES} rsg_server.cpp)
target_link_libraries(rsg ${SimGrid_LIBRARY})

add_executable(dumb_client
               ${CLIENT_SOURCES} dumb_client.cpp)
target_link_libraries(dumb_client ${SimGrid_LIBRARY})
