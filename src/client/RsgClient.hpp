#pragma once

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
#include "RsgKVS.h"

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
    RsgKVSClient* kvs;
};


/* This variable should always hold a valid client.
We use a __attribute__((constructor)) to initialize it as soon as possible.
Then, when a new Actor is created, the first thing to do is to create a new client.
*/
extern thread_local RsgClient* client;

