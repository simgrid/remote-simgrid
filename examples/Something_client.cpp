#include "Something.h"  // As an example

#include <thrift/transport/TSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/protocol/TBinaryProtocol.h>

#include <stdlib.h>     //for using the function sleep
#include <iostream>     //for using the function sleep
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

using namespace Test;

int main(int argc, char **argv) {

  boost::shared_ptr<TSocket> socket(new TSocket("localhost", 9090));
  boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
  boost::shared_ptr<TBinaryProtocol> protocol(new TBinaryProtocol(transport));

  SomethingClient client(protocol);
  std::cout << "exeption " << std::endl;
  transport->open();
  std::cout << "apres exeption " << std::endl;
  for (int i = 0 ; i < 2; i++) {
    std::cout << "ping cl" << std::endl;
    std::cout << client.ping() << std::endl;
    std::cout << "end ping" << std::endl;
    sleep(2);
  }
  transport->close();

  return 0;
}
