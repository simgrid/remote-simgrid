/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU Affero Licence (see in file LICENCE).        */

#ifndef SRC_RSGCLIENT_HPP_
#define SRC_RSGCLIENT_HPP_

#include <xbt.h>
#include <vector>
#include <string>
#include <zmq.hpp>
#include <iostream>
#include <memory>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/protocol/TBinaryProtocol.h>

#include "../common.hpp"

#include "TZmqClient.hpp"

#include "RsgEngine.h"
#include "RsgActor.h"
#include "RsgMailbox.h"
#include "RsgMutex.h"
#include "RsgConditionVariable.h"
#include "RsgHost.h"
#include "RsgComm.h"

using namespace ::apache::thrift::protocol;
using apache::thrift::transport::TZmqClient;
using apache::thrift::protocol::TBinaryProtocol;
using boost::shared_ptr;
using namespace ::RsgService;

class RsgClient {
public:
    RsgClient(std::string name);
    ~RsgClient();
private:
    zmq::context_t ctx;
public:
    std::string networkName_;
    TZmqClient* zmqclient_;
    RsgEngineClient* engine;
    RsgActorClient* actor;
    RsgMailboxClient* mailbox;
    RsgMutexClient* mutex;
    RsgConditionVariableClient* conditionvariable;
    RsgHostClient* host;
    RsgCommClient* comm;
};


/* This variable should always hold a valid client.
We use a __attribute__((constructor)) to initialize it as soon as possible.
Then, when a new Actor is created, the first thing to do is to create a new client.
*/
extern thread_local RsgClient* client;




#endif /* SRC_RSGCLIENT_HPP_ */
