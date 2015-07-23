# Copyright (C) 2015. The SimGrid Team. All rights reserved.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Affero Licence (cf. in file LICENCE).

cmake_minimum_required (VERSION 2.6)
project (RemoteSimGrid)


include(FindSimGrid.cmake)
include_directories("${SimGrid_INCLUDE_DIR}" SYSTEM)


add_subdirectory(src)
