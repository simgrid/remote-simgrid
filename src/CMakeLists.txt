# Copyright (C) 2015. The SimGrid Team. All rights reserved.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Affero Licence (see in file LICENCE).

include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(COMMON_SOURCES rsg/Socket.cpp ../include/RsgMsg.hpp)

set(CLIENT_SOURCES_THRIFT
    ${COMMON_SOURCES}
    client/RsgClientEngine.cpp                   ../include/client/RsgClientEngine.hpp
    client/multiThreadedSingletonFactory.cpp     ../include/client/multiThreadedSingletonFactory.hpp
    client/rsg_actor.cpp                         ../include/rsg/actor.hpp
    client/rsg_mailbox.cpp                       ../include/rsg/mailbox.hpp
    client/rsg_comm.cpp                          ../include/rsg/comm.hpp
    client/rsg_host.cpp                          ../include/rsg/host.hpp
    client/rsg_mutex.cpp                         ../include/rsg/mutex.hpp
    client/rsg_global_service.cpp                ../include/rsg/global.hpp
)

set(SERVER_SOURCES
    ${COMMON_SOURCES}
    main_server.cpp 
    rsg/Server.cpp 
    rsg/services/actorService.cpp      ../include/rsg/services/actorService.hpp
    rsg/services/commService.cpp       ../include/rsg/services/commService.hpp
    rsg/services/mailboxService.cpp    ../include/rsg/services/mailboxService.hpp
    rsg/services/hostService.cpp       ../include/rsg/services/actorService.hpp
    rsg/services/globalService.cpp     ../include/rsg/services/globalService.hpp
    rsg/services/mutexService.cpp      ../include/rsg/services/mutexService.hpp
    rsg/RsgThriftServerFramework.cpp   ../include/rsg/RsgThriftServerFramework.hpp
    rsg/RsgThriftSimpleServer.cpp      ../include/rsg/RsgThriftSimpleServer.hpp
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

install(TARGETS rsg DESTINATION ${RSG_INSTALL_DIR})
install(TARGETS rsg_server DESTINATION ${RSG_INSTALL_DIR})

