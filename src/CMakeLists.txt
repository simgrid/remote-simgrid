# Copyright (C) 2015. The SimGrid Team. All rights reserved.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Affero Licence (see in file LICENCE).

add_executable(rsg_server 
               rsg_server.cpp socket.c socket.h)
target_link_libraries(rsg_server ${SimGrid_LIBRARY})
