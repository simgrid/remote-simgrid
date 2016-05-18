#include "rsg/services.hpp"
#include "rsg/Server.hpp"

#include "xbt.h"
#include "simgrid/s4u.h"

#include <iostream>
#include <boost/shared_ptr.hpp>

#include <thrift/processor/TMultiplexedProcessor.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;


using namespace ::apache::thrift::server;
using namespace  ::RsgService;

using namespace  ::simgrid;

rsg::RsgActorHandler::RsgActorHandler()  : pSelf(s4u::Actor::self()), pServer(NULL) {
}

void rsg::RsgActorHandler::setServer(TServerFramework *server) {
  pServer = server;
}

void rsg::RsgActorHandler::close() {
  pServer->stop();
}

void  rsg::RsgActorHandler::sleep(const double duration) {
  pSelf.sleep(duration);
}

void rsg::RsgActorHandler::execute(const double flops) {
  pSelf.execute(flops);
}

void rsg::RsgActorHandler::send(const int64_t mbAddr, const std::string& content, const int64_t simulatedSize) {
  s4u::Mailbox *mbox = (s4u::Mailbox*)mbAddr;
  std::string *internalPtr = new std::string(content.data(), content.length());
  pSelf.send(*mbox, (void*) internalPtr, simulatedSize);
}

void rsg::RsgActorHandler::recv(std::string& _return, const int64_t mbAddr) {
  s4u::Mailbox *mbox = (s4u::Mailbox*) mbAddr;
  std::string *content = (std::string*) pSelf.recv(*mbox);
  _return.assign(content->data(), content->length());
  delete content;
}

//FIXME the three following function assume that you can only have the "self" actor.
void rsg::RsgActorHandler::getName(std::string& _return, const int64_t addr) {
  const char * c_name = pSelf.getName();
  _return.assign(c_name);
}

//FIXME
void rsg::RsgActorHandler::getHost(rsgHostCurrentResType& _return, const int64_t addr) {
  s4u::Host *host = pSelf.getHost();
  _return.name = host->name();
  _return.addr = (unsigned long int) host;
}

//FIXME
int32_t rsg::RsgActorHandler::getPid(const int64_t addr) {
  return pSelf.getPid();
}

//FIXME
void rsg::RsgActorHandler::setAutoRestart(const int64_t addr, const bool autorestart) {
  pSelf.setAutoRestart(autorestart);
}

//FIXME
void rsg::RsgActorHandler::setKillTime(const int64_t addr, const double time) {
  pSelf.setKillTime(time);
}

//FIXME
double rsg::RsgActorHandler::getKillTime(const int64_t addr) {
  return pSelf.getKillTime();
}

void rsg::RsgActorHandler::killAll() {
  s4u::Actor::killAll();
}

//TODO find the good emplacement
class Master : public simgrid::s4u::Actor {
public:
  Master() : simgrid::s4u::Actor(){}
  
    virtual int main(int argc, char **argv) {
      boost::shared_ptr<rsg::RsgActorHandler> handler(new rsg::RsgActorHandler());
      boost::shared_ptr<rsg::RsgMailboxHandler> mbHandler(new rsg::RsgMailboxHandler());
      boost::shared_ptr<rsg::RsgHostHandler> hostHandler(new rsg::RsgHostHandler());
      boost::shared_ptr<rsg::RsgCommHandler> commHandler(new rsg::RsgCommHandler());

      TMultiplexedProcessor* processor = new TMultiplexedProcessor();

      processor->registerProcessor(
          "RsgActor",
          boost::shared_ptr<RsgActorProcessor>(new RsgActorProcessor(handler)));

      processor->registerProcessor(
          "RsgMailbox",
          boost::shared_ptr<RsgMailboxProcessor>(new RsgMailboxProcessor(mbHandler)));

      processor->registerProcessor(
          "RsgHost",
          boost::shared_ptr<RsgHostProcessor>(new RsgHostProcessor(hostHandler)));

      processor->registerProcessor(
          "RsgComm",
          boost::shared_ptr<RsgCommProcessor>(new RsgCommProcessor(commHandler)));

      TServerFramework *server = SocketServer::getSocketServer().acceptClient(processor);

      handler->setServer(server);
      server->serve();
      delete server;
      return 1;
  };
};

int64_t rsg::RsgActorHandler::createActor(const std::string& name, const int64_t host, const int32_t killTime) {

  return 0;
}

