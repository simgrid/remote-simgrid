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
    RsgClient();
    void forceInitialisation(std::string networkName);
    ~RsgClient();
private:
    void init();
    zmq::context_t ctx;
    std::string networkName_;
public:
    RsgEngineClient* engine;
    RsgActorClient* actor;
    RsgMailboxClient* mailbox;
    RsgMutexClient* mutex;
    RsgConditionVariableClient* conditionvariable;
    RsgHostClient* host;
    RsgCommClient* comm;
};

extern thread_local RsgClient client;




#endif /* SRC_RSGCLIENT_HPP_ */
