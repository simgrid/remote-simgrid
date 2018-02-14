#include <simgrid/s4u.hpp>
#include <xbt/string.hpp>
#include <iostream>
#include <stdexcept>
#include <boost/shared_ptr.hpp>
#include <thrift/processor/TMultiplexedProcessor.h>

#include <simgrid/msg.h>

#include "../services.hpp"
#include "../../common.hpp"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using namespace  ::RsgService;

using namespace  ::simgrid;

std::unordered_map<int, simgrid::s4u::ActorPtr> rsg::RsgActorHandler::pActors;
unsigned long long rsg::RsgActorHandler::pActorMapId = 0;

rsg::RsgActorHandler::RsgActorHandler()
    : server_exit(false)
{
}

void rsg::RsgActorHandler::close() {
    server_exit = true;
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
        _return.name = host->getName();
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

int32_t rsg::RsgActorHandler::killPid(const int32_t pid) {
    try {
        /*@Adrien: why this ?
        int positiv_pid = pid;
        if(pid < -1)
            positiv_pid = pid*-1;*/
        
        simgrid::s4u::Actor::kill(pid);
        return 0;
    } catch(std::out_of_range& e) {
        return 1;
    } catch(std::exception &e) {
        return 1;
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

void rsg::RsgActorHandler::createActorPrepare(std::string& _return) {
    TZmqServer::get_new_endpoint(_return);
    lastChildServer = new RsgThriftServer(_return);
}

int64_t rsg::RsgActorHandler::selfAddr() {
    simgrid::s4u::ActorPtr actor = s4u::Actor::self();
    unsigned long long newId = pActorMapId++;
    pActors.insert({newId, actor});
    return newId;
}

int deleteServerWhenActorIsKilled(void *arg, void *arg2) {
    debug_server_print("");
    RsgThriftServer* srv = (RsgThriftServer*)arg2;
    delete srv;
    debug_server_print("");
    return 0;
}



int64_t rsg::RsgActorHandler::createActor(const std::string& name, const int64_t hostaddr, const int32_t killTime) {
    
    s4u::Host *host = (s4u::Host*) hostaddr;
    
    debug_server_print("createActor for sgname: %s", name.c_str());
    
    //we use a lambda because otherwise simgrid make unwanted copy of the class.
    simgrid::s4u::ActorPtr actor = simgrid::s4u::Actor::createActor(
                                                            name.c_str(),
                                                            host,
                                                            [&]{
                                                                RsgThriftServer* srv = lastChildServer;
                                                                MSG_process_on_exit(deleteServerWhenActorIsKilled, (void*)srv);
                                                                (*srv)();
                                                            }
                                                                   );
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

int64_t rsg::RsgActorHandler::byPid(const int32_t pid) {
    simgrid::s4u::ActorPtr actor = s4u::Actor::byPid(pid);
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
