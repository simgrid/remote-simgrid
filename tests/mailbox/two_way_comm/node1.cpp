#include "rsg/actor.hpp"
#include "rsg/mailbox.hpp"
#include "rsg/comm.hpp"
#include "rsg/host.hpp"

#include "xbt.h"
#include "simgrid/s4u.hpp"

#include <iostream>

XBT_LOG_NEW_CATEGORY(RSG_THRIFT_CLIENT, "Remote SimGrid");
XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_THRIFT_NODE_1, RSG_THRIFT_CLIENT , "RSG server (Remote SimGrid)");

using namespace ::simgrid;

using boost::shared_ptr;

int main(int argc, char **argv) {
  const char *msg = "Do you copy ? ";
  rsg::Mailbox *mbox = rsg::Mailbox::byName("toto");
  rsg::this_actor::send(*mbox,msg, strlen(msg) + 1);
  XBT_INFO("send %s with size : %d", msg, strlen(msg));
  
  char *rec;
  rec = rsg::this_actor::recv(*mbox);
  XBT_INFO("Received -> %s ", rec);

  rsg::Mailbox *fooMb = rsg::Mailbox::byName("foo");
  rsg::this_actor::send(*fooMb, msg, strlen(msg) + 1);
  XBT_INFO("send %s with size : %d", msg, strlen(msg));

  char *fooRec;
  fooRec = rsg::this_actor::recv(*fooMb);
  XBT_INFO("Received -> %s ", fooRec);

  rsg::this_actor::quit();
  return 0;
}
