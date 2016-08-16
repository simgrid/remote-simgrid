#include "multiThreadedSingletonFactory.hpp"
#include "RsgClient.hpp"

#include "../rsg/Socket.hpp"
#include "RsgMsg.hpp"

#include <thrift/transport/TSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/protocol/TBinaryProtocol.h>

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <cstdlib>

using namespace ::simgrid;

XBT_LOG_NEW_CATEGORY(RSG_THRIFT_CLIENT, "Remote SimGrid");
XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_THRIFT_CLIENT_ENGINE, RSG_THRIFT_CLIENT , "RSG server (Remote SimGrid)");


Client::Client(std::string hostname, int port) : pSock(-1),
pHostname(hostname),
pPort(port),
pProtocol(NULL),
pTransport(NULL),
pServices(new boost::unordered_map<std::string, void*> ()),
pDestructors(new boost::unordered_map<std::string, IDel*>()) {  
}

Client::~Client() { 
    delete pServices;
    delete pDestructors;
}

void Client::init() {
    
    char *rpcEnvPort = std::getenv("RsgRpcPort");
    int rpcPort;
    if(rpcEnvPort != NULL) {
        //We first try to get env variable.
        rpcPort = std::stoi(rpcEnvPort, NULL);
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
}

void Client::connectToRpc(int rpcPort) {
    boost::shared_ptr<TSocket> socket(new TSocket(pHostname.c_str(), rpcPort));
    pTransport.reset(new TBufferedTransport(socket));
    pProtocol.reset(new TBinaryProtocol(pTransport));
    bool connected = true;
    do {
        try {
            pTransport->open();
            pRpcPort = rpcPort;
            connected = true;
        } catch(apache::thrift::transport::TTransportException &ex) {
            connected = false;
            sleep(0.1);
        }
    } while(!connected);
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
