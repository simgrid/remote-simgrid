#include "rsg/actor.hpp"
#include "rsg/mailbox.hpp"
#include "rsg/host.hpp"

#include <xbt.h>
#include <simgrid/s4u.hpp>

#include <stdio.h>
#include <iostream>

XBT_LOG_NEW_CATEGORY(RSG_THRIFT_CLIENT, "Remote SimGrid");
XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_THRIFT_REMOTE_SERVER, RSG_THRIFT_CLIENT , "RSG server (Remote SimGrid)");

using namespace ::simgrid;

int main() {
  rsg::MailboxPtr mbox = rsg::Mailbox::byName("toto");
  char *received = rsg::this_actor::recv(*mbox);
  XBT_INFO("Received from client : %s with size of %lu", received, strlen(received) );
  rsg::this_actor::quit();
  return 0;
}
