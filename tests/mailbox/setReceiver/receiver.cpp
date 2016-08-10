#include "rsg/actor.hpp"
#include "rsg/mailbox.hpp"
#include "rsg/host.hpp"

#include "xbt.h"
#include "simgrid/s4u.hpp"

#include <stdio.h>
#include <iostream>

XBT_LOG_NEW_CATEGORY(RSG_THRIFT_CLIENT, "Remote SimGrid");
XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_THRIFT_REMOTE_SERVER, RSG_THRIFT_CLIENT , "RSG server (Remote SimGrid)");

using boost::shared_ptr;
using namespace ::simgrid;

int main(int argc, char **argv) {
  rsg::Mailbox *mbox = rsg::Mailbox::byName("toto");
  char *received = rsg::this_actor::recv(*mbox);

  rsg::this_actor::sleep(1);
  mbox->setReceiver(*rsg::Actor::forPid(rsg::this_actor::getPid()));
  rsg::this_actor::sleep(1);

  XBT_INFO("Received from client : %s with size of %d", received, strlen(received) );
  rsg::this_actor::quit();
  return 0;
}
