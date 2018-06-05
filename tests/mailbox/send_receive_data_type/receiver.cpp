#include "rsg/actor.hpp"
#include "rsg/mailbox.hpp"
#include "rsg/host.hpp"

#include <xbt.h>
#include <simgrid/s4u.hpp>

#include <stdio.h>
#include <iostream>

#include "struct.h"

XBT_LOG_NEW_CATEGORY(RSG_THRIFT_CLIENT, "Remote SimGrid");
XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_THRIFT_REMOTE_SERVER, RSG_THRIFT_CLIENT , "RSG server (Remote SimGrid)");

using boost::shared_ptr;

using namespace ::simgrid;

int main() {
  rsg::MailboxPtr mbox = rsg::Mailbox::byName("toto");
  char *received = rsg::this_actor::recv(*mbox);
  XBT_INFO("Received : %s with size of %lu", received, strlen(received) );
  
  int *recInt = (int*) rsg::this_actor::recv(*mbox);
  XBT_INFO("Received : %d", *recInt);
  free(recInt);
  
  recInt = (int*) rsg::this_actor::recv(*mbox);
  XBT_INFO("Received : %d", *recInt);
  
  int *recTabInt;
  recTabInt = (int*) rsg::this_actor::recv(*mbox);
  for(int i = 0; i < *recInt ;i++) {
    XBT_INFO(" tab[%d] = %d", i , recTabInt[i]);
  }
  free(recTabInt);
  free(recInt);
  
  structMsg *recStruct = (structMsg*) rsg::this_actor::recv(*mbox);
  XBT_INFO(" recStruct->intMsg = %d", recStruct->intMsg);
  XBT_INFO(" recStruct->msg = %s", recStruct->msg);
  free(recStruct);
  rsg::this_actor::quit();
  return 0;
}
