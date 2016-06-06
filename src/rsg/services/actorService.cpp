#include "rsg/services.hpp"
#include "rsg/Server.hpp"
#include "rsg/Socket.hpp"

#include "xbt.h"
#include "simgrid/s4u.h"
#include "RsgMsg.hpp"
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

rsg::RsgActorHandler::RsgActorHandler()  : pServer(NULL) {
}

void rsg::RsgActorHandler::setServer(RsgThriftServerFramework *server) {
  pServer = server;
}

void rsg::RsgActorHandler::close() {
  pServer->stop();
}

void  rsg::RsgActorHandler::sleep(const double duration) {
  s4u::this_actor::sleep(duration);
}

void rsg::RsgActorHandler::execute(const double flops) {
  s4u::this_actor::execute(flops);
}

void rsg::RsgActorHandler::send(const int64_t mbAddr, const std::string& content, const int64_t simulatedSize) {
  s4u::Mailbox *mbox = (s4u::Mailbox*)mbAddr;
  std::string *internalPtr = new std::string(content.data(), content.length());
  s4u::this_actor::send(*mbox, (void*) internalPtr, simulatedSize);   
}

void rsg::RsgActorHandler::recv(std::string& _return, const int64_t mbAddr) {
  s4u::Mailbox *mbox = (s4u::Mailbox*) mbAddr;
  std::string *content = (std::string*) s4u::this_actor::recv(*mbox);  
  _return.assign(content->data(), content->length());
  delete content;
}

void rsg::RsgActorHandler::getName(std::string& _return, const int64_t addr) {
    s4u::Actor *actor = (s4u::Actor*) addr;
    const char * c_name = actor->getName();
    _return.assign(c_name);
}


void rsg::RsgActorHandler::getHost(rsgHostCurrentResType& _return, const int64_t addr) {
  s4u::Actor *actor = (s4u::Actor*) addr;

  s4u::Host *host = actor->getHost();
  _return.name = host->name();
  _return.addr = (unsigned long int) host;
}


int32_t rsg::RsgActorHandler::getPid(const int64_t addr) {
  s4u::Actor *actor = (s4u::Actor*) addr;
  return actor->getPid();
}


void rsg::RsgActorHandler::setAutoRestart(const int64_t addr, const bool autorestart) {
  s4u::Actor *actor = (s4u::Actor*) addr;
  actor->setAutoRestart(autorestart);
}


void rsg::RsgActorHandler::setKillTime(const int64_t addr, const double time) {
  s4u::Actor *actor = (s4u::Actor*) addr;
  actor->setKillTime(time);
}


double rsg::RsgActorHandler::getKillTime(const int64_t addr) {
  s4u::Actor *actor = (s4u::Actor*) addr;
  return actor->getKillTime();
}

void rsg::RsgActorHandler::killAll() {
  s4u::Actor::killAll();
}

void rsg::RsgActorHandler::kill(const int64_t mbAddr) {
  s4u::Actor *actor = (s4u::Actor*) mbAddr;
  actor->kill();
}

void rsg::RsgActorHandler::killPid(const int32_t pid) {
  try {
    s4u::Actor::kill(pid);
  } catch( const std::exception & e ) {
    std::cerr << e.what();
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
  int64_t addr = (int64_t) new simgrid::s4u::Actor(name.c_str(), host, RsgActor(server, port));
  return addr;
}

void rsg::RsgActorHandler::deleteActor(const int64_t addr)  {
  s4u::Actor *actor = (s4u::Actor*) addr;
  delete actor;
}