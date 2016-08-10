#include "rsg/actor.hpp"
#include "rsg/mailbox.hpp"
#include "rsg/host.hpp"

#include "xbt.h"
#include "simgrid/s4u.hpp"

#include <stdio.h>
#include <iostream>

XBT_LOG_NEW_CATEGORY(RSG_THRIFT_CLIENT, "Remote SimGrid");
XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_THRIFT_NODE_3, RSG_THRIFT_CLIENT , "RSG server (Remote SimGrid)");

using namespace ::simgrid;

using boost::shared_ptr;

int main(int argc, char **argv) {
  rsg::Mailbox *fooMb = rsg::Mailbox::byName("foo");
  char *received = rsg::this_actor::recv(*fooMb);
  XBT_INFO("Received -> %s ", received);
  
  const char * sendMessage = "Ok";
  rsg::this_actor::send(*fooMb, sendMessage, strlen(sendMessage) + 1);
  
  rsg::this_actor::quit();
  return 0;
}
