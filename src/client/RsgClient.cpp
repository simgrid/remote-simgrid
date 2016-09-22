#include "multiThreadedSingletonFactory.hpp"
#include "RsgClient.hpp"

#include "../rsg/Socket.hpp"
#include "../common.hpp"



#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <cstdlib>
#include <mutex>

#include <thrift/transport/TSocket.h>
#include <thrift/transport/TBufferTransports.h>

using namespace ::simgrid;

XBT_LOG_NEW_CATEGORY(RSG_THRIFT_CLIENT, "Remote SimGrid");
XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_THRIFT_CLIENT_ENGINE, RSG_THRIFT_CLIENT , "RSG server (Remote SimGrid)");


class WrapTBinaryProtocol : public TBinaryProtocol {
public:
    WrapTBinaryProtocol(boost::shared_ptr<apache::thrift::transport::TBufferedTransport>&a):TBinaryProtocol(a){
    }

    //called when sending a message
    uint32_t writeMessageBegin(const std::string& name,
                                    const TMessageType messageType,
                                    const int32_t seqid) {
        debug_client_print("[%p]%s",this, name.c_str());
        return TBinaryProtocol::writeMessageBegin(name, messageType, seqid);
    }

    //called when recving a message
    uint32_t readMessageBegin(std::string& name, TMessageType& messageType, int32_t& seqid) {
        uint32_t ret = TBinaryProtocol::readMessageBegin(name, messageType, seqid);
        debug_client_print("[%p]RETURN TO APP after %s",this , name.c_str());
        return ret;
    }
};


Client::Client(std::string hostname, int port) : pSock(-1),
pHostname(hostname),
pPort(port),
pProtocol(NULL),
pTransport(NULL),
pServices(new boost::unordered_map<std::string, void*> ()),
pDestructors(new boost::unordered_map<std::string, IDel*>()) {  
}

Client::~Client() {
    debug_client_print("");
    delete pServices;
    delete pDestructors;
}

void Client::init() {
    
    char *rpcEnvPort = std::getenv("RsgRpcPort");
    int rpcPort;
    if(rpcEnvPort != NULL) {
        //We first try to get env variable.
        rpcPort = std::stoi(rpcEnvPort, NULL);
        unsetenv("RsgRpcPort"); //NOTE In order to clean the process, maybe it is better to not let this variable in the env.
    } else {
        //If no environments variables is setted, we need to ask the server the new port.
        int connectSock = socket_connect(pHostname.c_str() , pPort);
        if(connectSock <= 0) {
            fprintf(stderr,"error, cannot connect to server\n");
        }
        
        this->pSock = connectSock;
        recv(this->pSock, &rpcPort, sizeof(rpcPort), 0); // Server will send us the rpc port
    }
    connectToRpc(rpcPort);
    ::close(this->pSock);
}





void Client::connectToRpc(int rpcPort) {
    boost::shared_ptr<TSocket> socket(new TSocket(pHostname.c_str(), rpcPort));
    pTransport.reset(new TBufferedTransport(socket));
    pProtocol.reset(new WrapTBinaryProtocol(pTransport));
    bool connected = true;
    int max_attempt = 1000, attempt = 0;
    do {
        try {
            pTransport->open();
            pRpcPort = rpcPort;
            connected = true;
        } catch(apache::thrift::transport::TTransportException &ex) {
            attempt++;
            connected = false;
            sleep(0.1);
        }
    } while(!connected && attempt < max_attempt);

}

boost::shared_ptr<TBinaryProtocol>  Client::getProtocol() const {
    return boost::shared_ptr<TBinaryProtocol>(this->pProtocol);
}

boost::shared_ptr<TMultiplexedProtocol>  Client::getMultiplexedProtocol(std::string serviceName) const {
    return boost::shared_ptr<TMultiplexedProtocol>(new TMultiplexedProtocol(getProtocol(), serviceName));
}

boost::shared_ptr<TBufferedTransport>  Client::getTransport() const {
    return boost::shared_ptr<TBufferedTransport>(this->pTransport);
}

void Client::close() {
    pTransport->close();
};

void Client::connect() {
    pTransport->open();
};

void Client::flush() {
    pTransport->flush();
}

//FIXME Put this code into the engine destructor.
void Client::reset() {
    
    for ( auto it = this->pServices->begin(); it != this->pServices->end(); ++it ) {
        IDel * del = this->pDestructors->at(it->first);
        (*del)(this->pServices->at(it->first));
        delete del;
    }
    
    this->pDestructors->clear();
    this->pServices->clear();
}
