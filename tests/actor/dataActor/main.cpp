#include "rsg/actor.hpp"
#include "rsg/mailbox.hpp"
#include "rsg/comm.hpp"
#include "rsg/host.hpp"

#include "xbt.h"
#include "simgrid/s4u.hpp"

#include <iostream>

XBT_LOG_NEW_CATEGORY(RSG_THRIFT_CLIENT, "Remote SimGrid");
XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_THRIFT_REMOTE_CLIENT, RSG_THRIFT_CLIENT , "RSG server (Remote SimGrid)");

using boost::shared_ptr;
using namespace ::simgrid;

int actor(void *data) {

  XBT_INFO("data -> %s ", (char*) data );
  rsg::this_actor::quit();
  return 1;

}

int main(int argc, char **argv) {
  const char *msg = "Do you copy ? ";
  rsg::HostPtr host1 = rsg::Host::by_name("host1");

  rsg::Actor *act = rsg::Actor::createActor("receiver" ,host1 , actor,(void*) msg);
  act->join();
  rsg::this_actor::quit();
  return 0;
}
