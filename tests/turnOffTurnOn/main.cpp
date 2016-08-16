#include "rsg/actor.hpp"
#include "rsg/mailbox.hpp"
#include "rsg/host.hpp"

#include "xbt.h"
#include "simgrid/s4u.hpp"

#include <stdio.h>
#include <iostream>

XBT_LOG_NEW_CATEGORY(RSG_THRIFT_CLIENT, "Remote SimGrid");
XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_THRIFT_REMOTE_SERVER, RSG_THRIFT_CLIENT , "RSG server (Remote SimGrid)");

using namespace ::simgrid;

using boost::shared_ptr;

int main(int argc, char **argv) {

  XBT_INFO("hello from Server");
  rsg::HostPtr host1 = rsg::Host::by_name("host0");
  host1->turnOff();
  host1->turnOff();
  host1->turnOn();
  host1->turnOff();
  XBT_INFO("isOn %s -> %s",  host1->name().c_str(), host1->isOn() ? "YES" : "NO");
  host1->turnOn();
  XBT_INFO("isOn %s -> %s",  host1->name().c_str(), host1->isOn() ? "YES" : "NO");
  
  rsg::this_actor::quit();
  return 0;
}
