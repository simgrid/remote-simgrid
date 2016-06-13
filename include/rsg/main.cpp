// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.

#include <thrift/protocol/TBinaryProtocol.h>

#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

#include "rsg/services.hpp"
#include "client/RsgClient.hpp"
#include "rsg/actor.hpp"
#include "rsg/mailbox.hpp"
#include "rsg/comm.hpp"
#include "rsg/host.hpp"

#include "xbt.h"
#include "simgrid/s4u.h"

#include <iostream>

XBT_LOG_NEW_CATEGORY(RSG_THRIFT_CLIENT, "Remote SimGrid");
XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_THRIFT_REMOTE_CLIENT, RSG_THRIFT_CLIENT , "RSG server (Remote SimGrid)");

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;
using namespace ::RsgService;
using namespace ::simgrid;

int actor() {
  rsg::Mailbox *mbox = rsg::Mailbox::byName("toto");
  char *received = rsg::this_actor::recv(*mbox);
  XBT_INFO("Received from client : %s with size of %d ", received, strlen(received) );
  rsg::this_actor::quit();
  return 1;
}

int main(int argc, char **argv) {
  const char *msg = "Do you copy ? ";
  rsg::Host host1 = rsg::Host::by_name("host1");

  rsg::Actor::createActor("receiver" , host1 , actor);
  
  rsg::Mailbox *mbox = rsg::Mailbox::byName("toto");
  XBT_INFO("I'll send %s with size : %d", msg, strlen(msg));

  rsg::this_actor::send(*mbox,msg, strlen(msg) + 1);
  XBT_INFO("send %s with size : %d", msg, strlen(msg));
  
  rsg::this_actor::quit();
  return 0;
}