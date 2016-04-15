#include "client/RsgClientEngine.hpp"
#include "rsg/Socket.hpp"

#include <thrift/transport/TSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/protocol/TBinaryProtocol.h>

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>

XBT_LOG_NEW_CATEGORY(RSG_THRIFT_CLIENT, "Remote SimGrid");
XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_THRIFT_CLIENT_ENGINE, RSG_THRIFT_CLIENT , "RSG server (Remote SimGrid)");

ClientEngine::ClientEngine(std::string hostname, int port) : pSock(-1),
                                                 pHostname(hostname),
                                                 pPort(port),
                                                 pProtocol(NULL),
                                                 pTransport(NULL)
                                                 {

  int connectSock = socket_connect(hostname.c_str() , port);
  if(connectSock <= 0) {
      fprintf(stderr,"error, cannot connect to server\n");
  }

  this->pSock = connectSock;

  int rpcPort;
  recv(this->pSock, &rpcPort, sizeof(rpcPort), 0); // Server will send us the rpc port

  boost::shared_ptr<TSocket> socket(new TSocket(hostname.c_str(), rpcPort));
  pTransport.reset(new TBufferedTransport(socket));
  pProtocol.reset(new TBinaryProtocol(pTransport));

  bool connected = true;
  do {
    try {
      pTransport->open();
      connected = true;
    } catch(apache::thrift::transport::TTransportException &ex) {
      connected = false;
      XBT_INFO("conneciton failed: Try to reconnect");
      sleep(1);
    }
  } while(!connected);
  XBT_INFO("Connected");
}

boost::shared_ptr<TBinaryProtocol>  ClientEngine::getProtocol() const {
  return boost::shared_ptr<TBinaryProtocol>(this->pProtocol);
}

boost::shared_ptr<TMultiplexedProtocol>  ClientEngine::getMultiplexedProtocol(std::string serviceName) const {
  return boost::shared_ptr<TMultiplexedProtocol>(new TMultiplexedProtocol(getProtocol(), "RsgService"));
}


boost::shared_ptr<TBufferedTransport>  ClientEngine::getTransport() const {
  return boost::shared_ptr<TBufferedTransport>(this->pTransport);
}

void ClientEngine::closeConnection() {
  pTransport->close();
  close(pSock);
}
