

#include "RsgClient.hpp"

#include <thrift/protocol/TMultiplexedProtocol.h>
using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;

thread_local RsgClient client;


RsgClient::RsgClient()
    : ctx(1)
    , networkName_("networkName_NOT_SETTED")
{
        char *rpcEnvPort = std::getenv("RsgRpcNetworkName");
        if(rpcEnvPort == NULL) {
            //we will do the initialisation later
            debug_client_print("RSG client creating, need future init");
            return;
        }
        networkName_ = std::string(rpcEnvPort);
        
        init();
}

void RsgClient::init() {
        debug_client_print("RSG client %s starting", networkName_.c_str());
        
        shared_ptr<TZmqClient> transport(new TZmqClient(ctx, networkName_));
        shared_ptr<TBinaryProtocol> protocol(new TBinaryProtocol(transport));
    
        
        engine = new RsgEngineClient(shared_ptr<TMultiplexedProtocol>(new TMultiplexedProtocol(protocol, "RsgEngine")));
        actor = new RsgActorClient(shared_ptr<TMultiplexedProtocol>(new TMultiplexedProtocol(protocol, "RsgActor")));
        mailbox = new RsgMailboxClient(shared_ptr<TMultiplexedProtocol>(new TMultiplexedProtocol(protocol, "RsgMailbox")));
        mutex = new RsgMutexClient(shared_ptr<TMultiplexedProtocol>(new TMultiplexedProtocol(protocol, "RsgMutex")));
        conditionvariable = new RsgConditionVariableClient(shared_ptr<TMultiplexedProtocol>(new TMultiplexedProtocol(protocol, "RsgConditionVariable")));
        host = new RsgHostClient(shared_ptr<TMultiplexedProtocol>(new TMultiplexedProtocol(protocol, "RsgHost")));
        comm = new RsgCommClient(shared_ptr<TMultiplexedProtocol>(new TMultiplexedProtocol(protocol, "RsgComm")));
        
        transport->open();
        debug_client_print("RSG client %s started", networkName_.c_str());
}

void RsgClient::forceInitialisation(std::string networkName) {
    networkName_ = networkName;
    init();
}

RsgClient::~RsgClient()
{
        delete engine;
        delete actor;
        delete mailbox;
        delete mutex;
        delete conditionvariable;
        delete host;
        delete comm;
        debug_client_print("RSG client %s ended", networkName_.c_str());
}


