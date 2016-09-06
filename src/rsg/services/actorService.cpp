#include "../services.hpp"
#include "../Server.hpp"
#include "../Socket.hpp"

#include "xbt.h"
#include "simgrid/s4u.hpp"
#include "../../common.hpp"
#include <xbt/string.hpp>
#include <iostream>
#include <stdexcept>
#include <boost/shared_ptr.hpp>
#include <thrift/processor/TMultiplexedProcessor.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using namespace  ::RsgService;

using namespace  ::simgrid;

std::unordered_map<int, simgrid::s4u::ActorPtr> rsg::RsgActorHandler::pActors;
unsigned long long rsg::RsgActorHandler::pActorMapId = 0;

rsg::RsgActorHandler::RsgActorHandler()  : pServer(NULL) {
}

void rsg::RsgActorHandler::setServer(RsgThriftServerFramework *server) {
    pServer = server;
}

void rsg::RsgActorHandler::close() {
    pServer->stop();
}

void  rsg::RsgActorHandler::sleep(const double duration) {
    s4u::this_actor::sleep_for(duration);
}

void rsg::RsgActorHandler::execute(const double flops) {
    s4u::this_actor::execute(flops);
}

void rsg::RsgActorHandler::send(const int64_t mbAddr, const std::string& content, const int64_t simulatedSize) {
    s4u::MailboxPtr mbox = rsg::RsgMailboxHandler::pMailboxes.at(mbAddr);
    std::string *internalPtr = new std::string(content.data(), content.length());
    s4u::this_actor::send(mbox, (void*) internalPtr, simulatedSize);   
}

void rsg::RsgActorHandler::recv(std::string& _return, const int64_t mbAddr) {
    s4u::MailboxPtr mbox = rsg::RsgMailboxHandler::pMailboxes.at(mbAddr);
    std::string *content = (std::string*) s4u::this_actor::recv(mbox);  
    _return.assign(content->data(), content->length());
    delete content;
}

void rsg::RsgActorHandler::getName(std::string& _return, const int64_t addr) {
    try {
        simgrid::s4u::ActorPtr actor = pActors.at(addr);
        simgrid::xbt::string c_name = actor->getName();
        _return.assign(c_name);
    } catch(std::out_of_range& e) {
        std::cerr << "rsg::RsgActorHandler::getName no actors for this addr" << std::endl; 
    }
}


void rsg::RsgActorHandler::getHost(rsgHostCurrentResType& _return, const int64_t addr) {
    try {
        simgrid::s4u::ActorPtr actor = pActors.at(addr);
        s4u::Host *host = actor->getHost();
        _return.name = host->name();
        _return.addr = (unsigned long int) host;
    } catch(std::out_of_range& e) {
        std::cerr << "rsg::RsgActorHandler::getHost no actors for this addr" << std::endl; 
    }
}


int32_t rsg::RsgActorHandler::getPid(const int64_t addr) {
    try {
        simgrid::s4u::ActorPtr actor = pActors.at(addr);
        return actor->getPid();
    } catch(std::out_of_range& e) {
        std::cerr << "rsg::RsgActorHandler::getPid no actors for this addr" << std::endl; 
    }
    return 0;
}


void rsg::RsgActorHandler::setAutoRestart(const int64_t addr, const bool autorestart) {
    try {
        simgrid::s4u::ActorPtr actor = pActors.at(addr);
        actor->setAutoRestart(autorestart);
    } catch(std::out_of_range& e) {
        std::cerr << "rsg::RsgActorHandler::setAutoRestart no actors for this addr" << std::endl; 
    }
}


void rsg::RsgActorHandler::setKillTime(const int64_t addr, const double time) {
    try {
        simgrid::s4u::ActorPtr actor = pActors.at(addr);
        actor->setKillTime(time);
    } catch(std::out_of_range& e) {
        std::cerr << "rsg::RsgActorHandler::setKillTime no actors for this addr" << std::endl; 
    }
}


double rsg::RsgActorHandler::getKillTime(const int64_t addr) {
    try {
        simgrid::s4u::ActorPtr actor = pActors.at(addr);
        return actor->getKillTime();
    } catch(std::out_of_range& e) {
        std::cerr << "rsg::RsgActorHandler::getKillTime no actors for this addr" << std::endl;
    }
    return -1;
}

void rsg::RsgActorHandler::killAll() {
    s4u::Actor::killAll();
}

void rsg::RsgActorHandler::kill(const int64_t mbAddr) {
    try {
        simgrid::s4u::ActorPtr actor = pActors.at(mbAddr);
        actor->kill();
    } catch(std::out_of_range& e) {
        std::cerr << "rsg::RsgActorHandler::kill no actors for this addr" << std::endl;
    }
}

void rsg::RsgActorHandler::killPid(const int32_t pid) {
    try {
        s4u::Actor::kill(pid);
    } catch( const std::exception & e ) {
        std::cerr << e.what();
    }
}

void rsg::RsgActorHandler::join(const int64_t addr) {
    try {
        simgrid::s4u::ActorPtr actor = pActors.at(addr);
        actor->join();
    } catch(std::out_of_range& e) {
        std::cerr << "rsg::RsgActorHandler::join no actors for this addr" << std::endl;
    }
}

void rsg::RsgActorHandler::createActorPrepare(rsgServerRemoteAddrAndPort& _return) {
    int rpcPort = getFreePort(1024);
    bool connected = false;
    
    RsgThriftServerFramework* server = NULL;
    do {
        server = SocketServer::getSocketServer().createRpcServer(rpcPort);
        try {
            server->listen();
            connected = true;
        } catch(apache::thrift::transport::TTransportException &ex) {
            rpcPort = getFreePort(1024);
            delete server;
        } 
    } while( ! connected);
    
    _return.addr = (unsigned long int) server;
    _return.port = rpcPort;
}

class RsgActor {
public:
    RsgActor(RsgThriftServerFramework *server, int port) : pServer(server), pPort(port) {}
    RsgThriftServerFramework *pServer;
    int pPort;
    int operator()() {
        
        pServer->serve();
        delete pServer;
        
        return 1;
    }
};

int64_t rsg::RsgActorHandler::createActor(const int64_t remoteServerAddr, const int32_t port, const std::string& name, const int64_t hostaddr, const int32_t killTime) {
    s4u::Host *host = (s4u::Host*) hostaddr;
    RsgThriftServerFramework* server = (RsgThriftServerFramework*) remoteServerAddr;
    simgrid::s4u::ActorPtr actor = simgrid::s4u::Actor::createActor(name.c_str(), host, RsgActor(server, port));
    unsigned long long newId = pActorMapId++;
    pActors.insert({newId, actor});
    return newId;
}

void rsg::RsgActorHandler::deleteActor(const int64_t addr)  {
    try {
        pActors.erase(addr);
    } catch(std::out_of_range& e) {
        std::cerr << "rsg::RsgActorHandler::deleteActor no actors for this addr" << std::endl; 
    }
}

int32_t rsg::RsgActorHandler::this_actorGetPid() {
    return s4u::this_actor::getPid();
}

int32_t rsg::RsgActorHandler::this_actorGetPPid() {
    return s4u::this_actor::getPpid();
}


int32_t rsg::RsgActorHandler::getPPid(const int64_t addr) {
    simgrid::s4u::ActorPtr actor = pActors.at(addr);
    return actor->getPpid();
}

int64_t rsg::RsgActorHandler::forPid(const int32_t pid) {
    simgrid::s4u::ActorPtr actor = s4u::Actor::forPid(pid);
    if(actor != nullptr) {
        unsigned long long newId = pActorMapId++;
        pActors.insert({newId, actor});
        return newId;
    } else {
        return -1;
    }
}

bool rsg::RsgActorHandler::isValideActor(const int64_t remoteAddr) {
    std::cerr << "This function is not supported anymore" << std::endl;
    return false;
}
