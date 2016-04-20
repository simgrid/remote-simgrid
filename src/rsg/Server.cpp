#include "rsg/Server.hpp"
#include "rsg/Socket.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/poll.h>

#include <iostream>
#include <thread>
#include <string>     // std::string, std::stoi

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/server/TServerFramework.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;
using namespace  ::RsgService;


void connectionHandler(TServerFramework *server) {
  server->serve();
}

SocketServer::SocketServer(std::string hostname, int port) : pHostname(hostname),
                                                             pPort(port),
                                                             pEndServer(false) {

}

SocketServer::~SocketServer() {
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

TServerFramework* SocketServer::acceptClient(TProcessor *processor) {

  int new_sd = accept(getSocket() , NULL, NULL);

  if (new_sd < 0) {
    if (errno != EWOULDBLOCK) {
      perror("  accept() failed");
      return NULL;
    }
  }

  int rpcPort = getFreePort(1024);
  TServerFramework *server = createRpcServer(rpcPort, processor);

  // we send the new port to the clients

  send(new_sd, &rpcPort, sizeof(int), 0);

  // We could wait for client ack before registering a pointer to the server.
  return server;
}


TServerFramework* SocketServer::createRpcServer(int port, TProcessor* ptrProcessor) {
  shared_ptr<TProcessor> processor(ptrProcessor);
  shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());
  TServerFramework *server = new TSimpleServer(processor, serverTransport, transportFactory, protocolFactory); // we create the server
  return (TServerFramework*) server;
}

int SocketServer::getSocket() const  {
  return this->pSocketDesc;
}
