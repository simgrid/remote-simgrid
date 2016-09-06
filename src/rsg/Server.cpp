#include "Server.hpp"
#include "Socket.hpp"

#include "RsgThriftServerFramework.hpp"
#include "../common.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/poll.h>

#include <iostream>
#include <thread>
#include <string>     // std::string, std::stoi
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/processor/TMultiplexedProcessor.h>


using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using namespace ::simgrid;
using boost::shared_ptr;
using namespace  ::RsgService;

boost::shared_ptr<SocketServer> SocketServer::sServer(NULL);
bool SocketServer::sCreated = false;


void connectionHandler(RsgThriftServerFramework *server) {
  server->serve();
}

SocketServer::SocketServer(std::string hostname, int port) : pHostname(hostname),
                                                             pPort(port),
                                                             pEndServer(false) {
                                                               
}

SocketServer::~SocketServer() {
}
 
SocketServer& SocketServer::getSocketServer() {
  if(!SocketServer::sCreated) {
    xbt_die("Socket server must be created first");
  }
  return *(SocketServer::sServer.get());
}

SocketServer& SocketServer::createSocketServer(std::string host, int port) {
  if(SocketServer::sCreated) {
    xbt_die("Error, this method should be called just once.");
  }
  SocketServer::sServer.reset(new SocketServer(host, port));
  sCreated = true;
  return *(SocketServer::sServer.get());
}


int SocketServer::closeServer() {
  pEndServer = true;
  return close(this->pSocketDesc);

}

int SocketServer::connect() {
  int resSocket = createServerSocket(this->pPort);
  if (resSocket <= 0 ) {
    fprintf(stderr, "Server cannot connect\n");
    return 1;
  }
  this->pSocketDesc = resSocket;
  return 0;
}

RsgThriftServerFramework* SocketServer::acceptClient() {

  int new_sd = accept(getSocket() , NULL, NULL);

  if (new_sd < 0) {
    if (errno != EWOULDBLOCK) {
      perror("  accept() failed");
      return NULL;
    }
  }

  RsgThriftServerFramework *server = NULL;
  bool serverBinded = false;
  int rpcPort;
  // Sometimes getFreePort return an already used port.
  // To fix this error, we surround a the listen with a try catch and if it fails, we retry until it works 
  // It could be a good idea to set a retry limit.
  do {
      rpcPort = getFreePort(1024);
      try {
            server = createRpcServer(rpcPort);
            server->listen(); 
            serverBinded = true;
            } catch (apache::thrift::transport::TTransportException e) {
            delete server;
          }
  } while(!serverBinded);
  // we send the new port to the clients

  send(new_sd, &rpcPort, sizeof(int), 0);
  close(new_sd);
  // We could wait for client ack before registering a pointer to the server.
  return server;
}




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



void registerProcessor(TMultiplexedProcessor* processor, std::string name, shared_ptr<TProcessor> fp) {

  fp.get()->setEventHandler(shared_ptr<RsgProcessorEventHandler>(new RsgProcessorEventHandler));

  processor->registerProcessor(name, fp);
}

RsgThriftServerFramework* SocketServer::createRpcServer(int port) {

  TMultiplexedProcessor* processor = new TMultiplexedProcessor();

  shared_ptr<rsg::RsgActorHandler> handler(new rsg::RsgActorHandler());
  registerProcessor(processor, "RsgActor", shared_ptr<RsgActorProcessor>(new RsgActorProcessor(handler)));

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

  shared_ptr<rsg::RsgConditionVariableHandler> conditionVariableServiceHandler(new rsg::RsgConditionVariableHandler());
  registerProcessor(processor, "RsgConditionVariable", shared_ptr<RsgConditionVariableProcessor>(new RsgConditionVariableProcessor(conditionVariableServiceHandler)));

  RsgThriftServerFramework *server = createRpcServer(port, processor);
  handler->setServer(server);
  gblServiceHandler->setServer(server);
  return server;
  
}

RsgThriftServerFramework* SocketServer::createRpcServer(int port, TProcessor* ptrProcessor) {
  shared_ptr<TProcessor> processor(ptrProcessor);
  TServerSocket * thriftServerSocket = new TServerSocket(port);
  // thriftServerSocket->setSendTimeout(5);
  // thriftServerSocket->setRecvTimeout(5);
  // thriftServerSocket->setAcceptTimeout(5);
  thriftServerSocket->setRetryLimit(10);
  shared_ptr<TServerTransport> serverTransport(thriftServerSocket);
  shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());
  RsgThriftServerFramework *server = new RsgThriftSimpleServer(processor, serverTransport, transportFactory, protocolFactory); // we create the server
  return (RsgThriftServerFramework*) server;
}

int SocketServer::getSocket() const  {
  return this->pSocketDesc;
}
