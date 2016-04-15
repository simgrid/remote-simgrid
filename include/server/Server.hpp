#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <sys/socket.h>
#include <thread>

#include "RsgServiceImpl.h"

#include <boost/unordered_map.hpp>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>

using namespace ::apache::thrift::server;

/**
 * The socketServer is a multithreaded socket server.
 * He will listen onto the given port. For each new client the server run a thread and launch a new RPCServer into a new Thread.
 * Once the RpcServer is launched, the socketServer send the port number to the client so the client will be able to connect to the rpcServer.
 * The client and the server have to keep the connection socket until they finished.
 * When the client close the socket, the server will stop the dedicated rpcServer.
 */
class SocketServer  {
  public :
    SocketServer(std::string host, int port);
    ~SocketServer();
    int connect();
    int closeServer();

    TServerFramework* acceptClient(TProcessor *processor);

  protected :
    int getSocket() const;
    TServerFramework* createRpcServer(int port, TProcessor *processor);
  private :

    std::string pHostname;
    int pPort;
    int pSocketDesc;
    int pEndServer;

};


#endif /* SERVER_HPP */
