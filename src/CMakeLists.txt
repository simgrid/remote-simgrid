# Copyright (C) 2015. The SimGrid Team. All rights reserved.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Affero Licence (see in file LICENCE).

include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(COMMON_SOURCES "")

set(CLIENT_SOURCES_THRIFT
    ${COMMON_SOURCES}
    client/rsg_actor.cpp
    client/RsgClient.cpp
    client/RsgClient.hpp
    client/rsg_comm.cpp
    client/rsg_conditionVariable.cpp
    client/rsg_engine.cpp
    client/rsg_host.cpp
    client/rsg_mailbox.cpp
    client/rsg_mutex.cpp
    client/TZmqClient.cpp
    client/TZmqClient.hpp
)

set(SERVER_SOURCES
    ${COMMON_SOURCES}
    main_server.cpp
    rsg/RsgThriftServer.cpp
    rsg/RsgThriftServer.hpp
    rsg/services.hpp
    rsg/TZmqServer.cpp
    rsg/TZmqServer.hpp
    rsg/services/actorService.cpp
    rsg/services/actorService.hpp
    rsg/services/commService.cpp
    rsg/services/commService.hpp
    rsg/services/conditionVariableService.cpp
    rsg/services/conditionVariableService.hpp
    rsg/services/engineService.cpp
    rsg/services/engineService.hpp
    rsg/services/hostService.cpp
    rsg/services/hostService.hpp
    rsg/services/mailboxService.cpp
    rsg/services/mailboxService.hpp
    rsg/services/mutexService.cpp
    rsg/services/mutexService.hpp
)


FILE(GLOB_RECURSE THRIFT_GEN_FILES ${THRIFT_GEN_INCLUDE_DIR}/*)
FOREACH(item ${THRIFT_GEN_FILES})
  IF(${item} MATCHES ".*skeleton.*")
    LIST(REMOVE_ITEM THRIFT_GEN_FILES ${item})
  ENDIF(${item} MATCHES ".*skeleton.*")
ENDFOREACH(item)


add_executable(rsg_server ${SERVER_SOURCES} )
add_library(rsg SHARED ${CLIENT_SOURCES_THRIFT} ${THRIFT_GEN_FILES})

target_link_libraries(rsg_server "RsgService.thrift")
target_link_libraries(rsg_server ${SimGrid_LIBRARY})
target_link_libraries(rsg_server ${ZMQ_LIBRARIES})

install(TARGETS rsg DESTINATION ${RSG_INSTALL_DIR})
install(TARGETS rsg_server DESTINATION ${RSG_INSTALL_DIR})
