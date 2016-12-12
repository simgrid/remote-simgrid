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
  char *buffer = NULL;
  rsg::MailboxPtr mbox = rsg::Mailbox::byName("toto");
  
  char *message = rsg::this_actor::recv(*mbox);
  XBT_INFO("Async Received : %s with size of %lu", message, strlen(message));
  
  
  rsg::Comm &comm2 = rsg::Comm::recv_async(*mbox, (void**)&buffer);
  rsg::this_actor::execute(8095000000);
  

  comm2.wait();
  XBT_INFO("Async Received : %s with size of %lu", buffer, strlen(buffer));
  
  free(buffer);
  buffer = NULL;
  
  rsg::Comm &comm3 = rsg::Comm::recv_init(*mbox);
  comm3.setDstData((void**)&buffer);
  comm3.start();
    
  while(!comm3.test()) {
    rsg::this_actor::execute(809500000);
    XBT_INFO("Nothing yet , just wait !");
  }
  
  XBT_INFO("Async Received : %s with size of %lu", buffer, strlen(buffer));
  
  free(buffer);
  buffer = NULL;
  
  rsg::Comm &comm4 = rsg::Comm::recv_async(*mbox, (void**)&buffer);
  
  rsg::this_actor::execute(8095000000 * 2);

  comm4.wait();
  XBT_INFO("Async Received : %s with size of %lu", buffer, strlen(buffer));
  
  rsg::this_actor::quit();
  return 0;
}
