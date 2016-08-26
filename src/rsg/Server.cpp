#include "Server.hpp"
#include "Socket.hpp"

#include "RsgThriftServerFramework.hpp"

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

RsgThriftServerFramework* SocketServer::createRpcServer(int port) {
  shared_ptr<rsg::RsgActorHandler> handler(new rsg::RsgActorHandler());
  shared_ptr<rsg::RsgMailboxHandler> mbHandler(new rsg::RsgMailboxHandler());
  shared_ptr<rsg::RsgHostHandler> hostHandler(new rsg::RsgHostHandler());
  shared_ptr<rsg::RsgEngineHandler> gblServiceHandler(new rsg::RsgEngineHandler());
  shared_ptr<rsg::RsgMutexHandler> mutexServiceHandler(new rsg::RsgMutexHandler());
  shared_ptr<rsg::RsgConditionVariableHandler> conditionVariableServiceHandler(new rsg::RsgConditionVariableHandler());
  shared_ptr<rsg::RsgCommHandler> commHandler(new rsg::RsgCommHandler());

  TMultiplexedProcessor* processor = new TMultiplexedProcessor();

  processor->registerProcessor(
      "RsgActor",
      shared_ptr<RsgActorProcessor>(new RsgActorProcessor(handler)));

  processor->registerProcessor(
      "RsgMailbox",
      shared_ptr<RsgMailboxProcessor>(new RsgMailboxProcessor(mbHandler)));

  processor->registerProcessor(
      "RsgHost",
      shared_ptr<RsgHostProcessor>(new RsgHostProcessor(hostHandler)));

  processor->registerProcessor(
      "RsgComm",
      shared_ptr<RsgCommProcessor>(new RsgCommProcessor(commHandler)));
  
  processor->registerProcessor(
      "RsgMutex",
      shared_ptr<RsgMutexProcessor>(new RsgMutexProcessor(mutexServiceHandler)));
  
  processor->registerProcessor(
      "RsgEngine",
      shared_ptr<RsgEngineProcessor>(new RsgEngineProcessor(gblServiceHandler)));
  
  processor->registerProcessor(
      "RsgConditionVariable",
      shared_ptr<RsgConditionVariableProcessor>(new RsgConditionVariableProcessor(conditionVariableServiceHandler)));
  
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
