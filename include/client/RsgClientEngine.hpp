/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero Licence (see in file LICENCE).        */

#ifndef SRC_ENGINE_HPP_
#define SRC_ENGINE_HPP_

#include <xbt.h>
#include <vector>
#include <string>

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TBufferTransports.h>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
/**
 * The engine will create the connection to the rpcServer. First of all engine try to connect to the localhost.
 * Once the first connection is made, the Engine will wait for the entry point to be able to connect to the rpc server.
 */
class ClientEngine {

public:

	ClientEngine(std::string hostname, int port);
	void closeConnection();
	boost::shared_ptr<TBinaryProtocol> getProtocol() const ;
	boost::shared_ptr<TBufferedTransport> getTransport() const;
private:

	int pSock;
  std::string pHostname;
  int pPort;
	boost::shared_ptr<TBinaryProtocol> pProtocol;
	boost::shared_ptr<TBufferedTransport> pTransport;

};

#endif /* SRC_ENGINE_HPP_ */
