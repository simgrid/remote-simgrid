#include "rsg/RsgThriftSimpleServer.hpp"
#include <iostream>
namespace apache {
namespace thrift {
namespace server {

using apache::thrift::protocol::TProtocol;
using apache::thrift::protocol::TProtocolFactory;
using apache::thrift::transport::TServerTransport;
using apache::thrift::transport::TTransport;
using apache::thrift::transport::TTransportException;
using apache::thrift::transport::TTransportFactory;
using boost::shared_ptr;
using std::string;

RsgThriftSimpleServer::RsgThriftSimpleServer(const shared_ptr<TProcessorFactory>& processorFactory,
                             const shared_ptr<TServerTransport>& serverTransport,
                             const shared_ptr<TTransportFactory>& transportFactory,
                             const shared_ptr<TProtocolFactory>& protocolFactory)
  : RsgThriftServerFramework(processorFactory, serverTransport, transportFactory, protocolFactory) {
  RsgThriftServerFramework::setConcurrentClientLimit(1);
}

RsgThriftSimpleServer::RsgThriftSimpleServer(const shared_ptr<TProcessor>& processor,
                             const shared_ptr<TServerTransport>& serverTransport,
                             const shared_ptr<TTransportFactory>& transportFactory,
                             const shared_ptr<TProtocolFactory>& protocolFactory)
  : RsgThriftServerFramework(processor, serverTransport, transportFactory, protocolFactory) {
  RsgThriftServerFramework::setConcurrentClientLimit(1);
}

RsgThriftSimpleServer::RsgThriftSimpleServer(const shared_ptr<TProcessorFactory>& processorFactory,
                             const shared_ptr<TServerTransport>& serverTransport,
                             const shared_ptr<TTransportFactory>& inputTransportFactory,
                             const shared_ptr<TTransportFactory>& outputTransportFactory,
                             const shared_ptr<TProtocolFactory>& inputProtocolFactory,
                             const shared_ptr<TProtocolFactory>& outputProtocolFactory)
  : RsgThriftServerFramework(processorFactory,
                     serverTransport,
                     inputTransportFactory,
                     outputTransportFactory,
                     inputProtocolFactory,
                     outputProtocolFactory) {
  RsgThriftServerFramework::setConcurrentClientLimit(1);
}

RsgThriftSimpleServer::RsgThriftSimpleServer(const shared_ptr<TProcessor>& processor,
                             const shared_ptr<TServerTransport>& serverTransport,
                             const shared_ptr<TTransportFactory>& inputTransportFactory,
                             const shared_ptr<TTransportFactory>& outputTransportFactory,
                             const shared_ptr<TProtocolFactory>& inputProtocolFactory,
                             const shared_ptr<TProtocolFactory>& outputProtocolFactory)
  : RsgThriftServerFramework(processor,
                     serverTransport,
                     inputTransportFactory,
                     outputTransportFactory,
                     inputProtocolFactory,
                     outputProtocolFactory) {
  RsgThriftServerFramework::setConcurrentClientLimit(1);
}

RsgThriftSimpleServer::~RsgThriftSimpleServer() {
}

/**
 * The main body of customized implementation for RsgThriftSimpleServer is quite simple:
 * When a client connects, use the serve() thread to drive it to completion thus
 * blocking new connections.
 */
void RsgThriftSimpleServer::onClientConnected(const shared_ptr<TConnectedClient>& pClient) {
  pClient->run();
}

/**
 * RsgThriftSimpleServer does not track clients so there is nothing to do here.
 */
void RsgThriftSimpleServer::onClientDisconnected(TConnectedClient*) {
    if(!this->keepAliveOnClientDisconnect) {
        this->stop();
    } else {
        this->keepAliveOnClientDisconnect = false;
    }
}

/**
 * This makes little sense to the simple server because it is not capable
 * of having more than one client at a time, so we hide it.
 */
void RsgThriftSimpleServer::setConcurrentClientLimit(int64_t) {
}
}
}
} // apache::thrift::server
