#include <boost/bind.hpp>
#include <stdexcept>
#include <stdint.h>
#include <iostream>
#include "RsgThriftServerFramework.hpp"
#include "RsgMsg.hpp"
namespace apache {
namespace thrift {
namespace server {

using apache::thrift::concurrency::Synchronized;
using apache::thrift::transport::TServerTransport;
using apache::thrift::transport::TTransport;
using apache::thrift::transport::TTransportException;
using apache::thrift::transport::TTransportFactory;
using apache::thrift::protocol::TProtocol;
using apache::thrift::protocol::TProtocolFactory;
using boost::bind;
using boost::shared_ptr;
using std::string;

RsgThriftServerFramework::RsgThriftServerFramework(const shared_ptr<TProcessorFactory>& processorFactory,
                                   const shared_ptr<TServerTransport>& serverTransport,
                                   const shared_ptr<TTransportFactory>& transportFactory,
                                   const shared_ptr<TProtocolFactory>& protocolFactory)
  : TServer(processorFactory, serverTransport, transportFactory, protocolFactory),
    clients_(0),
    hwm_(0),
    limit_(INT64_MAX) {
}

RsgThriftServerFramework::RsgThriftServerFramework(const shared_ptr<TProcessor>& processor,
                                   const shared_ptr<TServerTransport>& serverTransport,
                                   const shared_ptr<TTransportFactory>& transportFactory,
                                   const shared_ptr<TProtocolFactory>& protocolFactory)
  : TServer(processor, serverTransport, transportFactory, protocolFactory),
    clients_(0),
    hwm_(0),
    limit_(INT64_MAX) {
}

RsgThriftServerFramework::RsgThriftServerFramework(const shared_ptr<TProcessorFactory>& processorFactory,
                                   const shared_ptr<TServerTransport>& serverTransport,
                                   const shared_ptr<TTransportFactory>& inputTransportFactory,
                                   const shared_ptr<TTransportFactory>& outputTransportFactory,
                                   const shared_ptr<TProtocolFactory>& inputProtocolFactory,
                                   const shared_ptr<TProtocolFactory>& outputProtocolFactory)
  : TServer(processorFactory,
            serverTransport,
            inputTransportFactory,
            outputTransportFactory,
            inputProtocolFactory,
            outputProtocolFactory),
    clients_(0),
    hwm_(0),
    limit_(INT64_MAX) {
}

RsgThriftServerFramework::RsgThriftServerFramework(const shared_ptr<TProcessor>& processor,
                                   const shared_ptr<TServerTransport>& serverTransport,
                                   const shared_ptr<TTransportFactory>& inputTransportFactory,
                                   const shared_ptr<TTransportFactory>& outputTransportFactory,
                                   const shared_ptr<TProtocolFactory>& inputProtocolFactory,
                                   const shared_ptr<TProtocolFactory>& outputProtocolFactory)
  : TServer(processor,
            serverTransport,
            inputTransportFactory,
            outputTransportFactory,
            inputProtocolFactory,
            outputProtocolFactory),
    clients_(0),
    hwm_(0),
    limit_(INT64_MAX) {
}

RsgThriftServerFramework::~RsgThriftServerFramework() {
}

template <typename T>
static void releaseOneDescriptor(const string& name, T& pTransport) {
  if (pTransport) {
    try {
      pTransport->close();
    } catch (const TTransportException& ttx) {
      string errStr = string("RsgThriftServerFramework " + name + " close failed: ") + ttx.what();
      GlobalOutput(errStr.c_str());
    }
  }
}

void RsgThriftServerFramework::listen() {
  // Start the server listening
  try {
    serverTransport_->listen();
    } catch (apache::thrift::transport::TTransportException e) {
        throw e;   
  }
  // Run the preServe event to indicate server is now listening
  // and that it is safe to connect.
  if (eventHandler_) {
    eventHandler_->preServe();
  }

}

void RsgThriftServerFramework::serve() {
  shared_ptr<TTransport> client;
  shared_ptr<TTransport> inputTransport;
  shared_ptr<TTransport> outputTransport;
  shared_ptr<TProtocol> inputProtocol;
  shared_ptr<TProtocol> outputProtocol;



  // Fetch client from server
  for (;;) {
    try {
      // Dereference any resources from any previous client creation
      // such that a blocking accept does not hold them indefinitely.
      outputProtocol.reset();
      inputProtocol.reset();
      outputTransport.reset();
      inputTransport.reset();
      client.reset();

      // If we have reached the limit on the number of concurrent
      // clients allowed, wait for one or more clients to drain before
      // accepting another.
      {
        Synchronized sync(mon_);
        while (clients_ >= limit_) {
          mon_.wait();
        }
      }
      
      client = serverTransport_->accept();

      inputTransport = inputTransportFactory_->getTransport(client);
      outputTransport = outputTransportFactory_->getTransport(client);
      inputProtocol = inputProtocolFactory_->getProtocol(inputTransport);
      outputProtocol = outputProtocolFactory_->getProtocol(outputTransport);

      newlyConnectedClient(shared_ptr<TConnectedClient>(
          new TConnectedClient(getProcessor(inputProtocol, outputProtocol, client),
                               inputProtocol,
                               outputProtocol,
                               eventHandler_,
                               client),
          bind(&RsgThriftServerFramework::disposeConnectedClient, this, _1)));

    } catch (TTransportException& ttx) {
      releaseOneDescriptor("inputTransport", inputTransport);
      releaseOneDescriptor("outputTransport", outputTransport);
      releaseOneDescriptor("client", client);
      if (ttx.getType() == TTransportException::TIMED_OUT) {
        // Accept timeout - continue processing.
        continue;
      } else if (ttx.getType() == TTransportException::END_OF_FILE
                 || ttx.getType() == TTransportException::INTERRUPTED) {
        // Server was interrupted.  This only happens when stopping.
        break;
      } else {
        // All other transport exceptions are logged.
        // State of connection is unknown.  Done.
        string errStr = string("TServerTransport died: ") + ttx.what();
        GlobalOutput(errStr.c_str());
        break;
      }
    }
  }

  releaseOneDescriptor("serverTransport", serverTransport_);
}

int64_t RsgThriftServerFramework::getConcurrentClientLimit() const {
  Synchronized sync(mon_);
  return limit_;
}

int64_t RsgThriftServerFramework::getConcurrentClientCount() const {
  Synchronized sync(mon_);
  return clients_;
}

int64_t RsgThriftServerFramework::getConcurrentClientCountHWM() const {
  Synchronized sync(mon_);
  return hwm_;
}

void RsgThriftServerFramework::setConcurrentClientLimit(int64_t newLimit) {
  if (newLimit < 1) {
    throw std::invalid_argument("newLimit must be greater than zero");
  }
  Synchronized sync(mon_);
  limit_ = newLimit;
  if (limit_ - clients_ > 0) {
    mon_.notify();
  }
}

void RsgThriftServerFramework::stop() {
  serverTransport_->interrupt();
  serverTransport_->interruptChildren();
}

void RsgThriftServerFramework::newlyConnectedClient(const boost::shared_ptr<TConnectedClient>& pClient) {
  onClientConnected(pClient);

  // Count a concurrent client added.
  Synchronized sync(mon_);
  ++clients_;
  hwm_ = std::max(hwm_, clients_);
}

void  RsgThriftServerFramework::setKeepAliveOnNextClientDisconnect(bool newValue) {
    this->keepAliveOnClientDisconnect = newValue;
}


void RsgThriftServerFramework::disposeConnectedClient(TConnectedClient* pClient) {
  {
    // Count a concurrent client removed.
    Synchronized sync(mon_);
    if (limit_ - --clients_ > 0) {
      mon_.notify();
    }
  }
  onClientDisconnected(pClient);
  delete pClient;
}
}
}
} // apache::thrift::server
