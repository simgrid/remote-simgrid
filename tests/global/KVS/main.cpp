#include "rsg/actor.hpp"
#include "rsg/mailbox.hpp"
#include "rsg/comm.hpp"
#include "rsg/host.hpp"
#include "rsg/engine.hpp"
#include "rsg/kvs.hpp"

#include "xbt.h"
#include "simgrid/s4u.hpp"

#include <iostream>

XBT_LOG_NEW_CATEGORY(RSG_THRIFT_CLIENT, "Remote SimGrid");
XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_THRIFT_REMOTE_CLIENT, RSG_THRIFT_CLIENT , "RSG server (Remote SimGrid)");

using boost::shared_ptr;
using namespace ::simgrid;

int actor(void *) {
  std::string key("hello");
  std::string value;
  rsg::kvs::get(value, key);
  XBT_INFO("Retrieved value from KVS : %s", value.c_str());
  rsg::this_actor::quit();
  return 1;
}

int main(int argc, char **argv) {
  rsg::HostPtr host1 = rsg::Host::by_name("host1");
  std::string key("hello");
  std::string value("WORLD!");
  rsg::kvs::insert(key, value);
  rsg::Actor *act = rsg::Actor::createActor("receiver" , host1 , actor, NULL);
  rsg::this_actor::quit();
  return 0;
}

