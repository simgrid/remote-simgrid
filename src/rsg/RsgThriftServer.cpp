

#include <string>

#include "RsgThriftServer.hpp"

#include "TZmqServer.hpp"
#include "services.hpp"
#include "../common.hpp"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/processor/TMultiplexedProcessor.h>

#include <simgrid/s4u.hpp>


using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using namespace ::simgrid;


class RsgProcessorEventHandler : public TProcessorEventHandler {
public:
  RsgProcessorEventHandler() {}

  //Called before reading arguments.
  void preRead(void* ctx, const char* fn_name) {
    (void)ctx;
    (void)fn_name;
  }

  //Called between reading arguments and calling the handler.
  void postRead(void* ctx, const char* fn_name, uint32_t bytes) {
    (void)ctx;
    (void)fn_name;
    (void)bytes;
    debug_server_print("RECV %s", fn_name);
    //FIXME: Here we wait 1us to simulate the time taken by the code to execute
    // There is no easy to give an accurate value here, so we choose an arbitrary determinist value.
    debug_server_print("FAKE EXEC: sleep %f", 1.0/1000.0/1000.0);
    s4u::this_actor::sleep_for(1.0/1000.0/1000.0);
    debug_server_print("FAKE EXEC: continue: RECV %s", fn_name);
  }

  //Called between calling the handler and writing the response.
  void preWrite(void* ctx, const char* fn_name) {
    (void)ctx;
    (void)fn_name;
    debug_server_print("RESPOND %s", fn_name);
  }

  //Called after writing the response.
  void postWrite(void* ctx, const char* fn_name, uint32_t bytes) {
    (void)ctx;
    (void)fn_name;
    (void)bytes;
  }

  // Called when an async function call completes successfully.
  void asyncComplete(void* ctx, const char* fn_name) {
    (void)ctx;
    (void)fn_name;
    debug_server_print("%s", fn_name);
  }

  //Called if the handler throws an undeclared exception.
  void handlerError(void* ctx, const char* fn_name) {
    (void)ctx;
    (void)fn_name;
    debug_server_print("undeclared exception? %s", fn_name);
  }

};








void registerProcessor(shared_ptr<TMultiplexedProcessor> processor, std::string name, shared_ptr<TProcessor> fp) {

  fp.get()->setEventHandler(shared_ptr<RsgProcessorEventHandler>(new RsgProcessorEventHandler));

  processor->registerProcessor(name, fp);
}



RsgThriftServer::RsgThriftServer(std::string& name) :name_(name) {
        //We have to create the server before starting the new Actor
        // Otherwise, the router may block on the first message send
        // (Perhaps it's a bug from ZeroMq, perhaps it's a bug due to Simgrid...)
        
    
    debug_server_print("Creating RsgThriftServer for %s %p", name_.c_str(), this);
    
        //init server
    shared_ptr<TMultiplexedProcessor> processor(new TMultiplexedProcessor());

    shared_ptr<rsg::RsgActorHandler> actHandler(new rsg::RsgActorHandler());
    registerProcessor(processor, "RsgActor", shared_ptr<RsgActorProcessor>(new RsgActorProcessor(actHandler)));

    shared_ptr<rsg::RsgMailboxHandler> mbHandler(new rsg::RsgMailboxHandler());
    registerProcessor(processor, "RsgMailbox", shared_ptr<RsgMailboxProcessor>(new RsgMailboxProcessor(mbHandler)));

    shared_ptr<rsg::RsgHostHandler> hostHandler(new rsg::RsgHostHandler());
    registerProcessor(processor, "RsgHost", shared_ptr<RsgHostProcessor>(new RsgHostProcessor(hostHandler)));

    shared_ptr<rsg::RsgCommHandler> commHandler(new rsg::RsgCommHandler());
    registerProcessor(processor, "RsgComm", shared_ptr<RsgCommProcessor>(new RsgCommProcessor(commHandler)));

    shared_ptr<rsg::RsgMutexHandler> mutexServiceHandler(new rsg::RsgMutexHandler());
    registerProcessor(processor, "RsgMutex", shared_ptr<RsgMutexProcessor>(new RsgMutexProcessor(mutexServiceHandler)));

    shared_ptr<rsg::RsgEngineHandler> gblServiceHandler(new rsg::RsgEngineHandler());
    registerProcessor(processor, "RsgEngine", shared_ptr<RsgEngineProcessor>(new RsgEngineProcessor(gblServiceHandler)));

    registerProcessor(processor, "RsgKVS", shared_ptr<RsgKVSProcessor>(new RsgKVSProcessor(rsg::RsgKVSHandler::getInstance())));

    shared_ptr<rsg::RsgConditionVariableHandler> conditionVariableServiceHandler(new rsg::RsgConditionVariableHandler());
    registerProcessor(processor, "RsgConditionVariable", shared_ptr<RsgConditionVariableProcessor>(new RsgConditionVariableProcessor(conditionVariableServiceHandler)));

        
        server = new TZmqServer(processor, name_, &(actHandler->server_exit));
        
        
    }
    
RsgThriftServer::RsgThriftServer(const RsgThriftServer& other) {
        debug_server_print("COPPPPPPPPPPPPYYYYYYYYYYYYYYYYYYYY %p", this);
        assert(false);
}
    
int RsgThriftServer::operator()() {
        
        debug_server_print("Run server for %s", name_.c_str());
        server->serve();
        debug_server_print("Server returned... %s", name_.c_str());
        delete server;
        server = 0;
        /*
        FILE *f = fopen("end_fork", "a");
        fprintf(f,"Actor [%d] quit\n", s4u::this_actor::getPid());
        fclose(f);
        */
        return 0;
    }

RsgThriftServer::~RsgThriftServer() {
    debug_server_print("Quitting RsgThriftServer for %s %p", name_.c_str(), this);
    /*if(server != 0) {
        delete server;
        server = 0;
    }*/
}
