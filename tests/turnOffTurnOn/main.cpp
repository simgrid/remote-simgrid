#include "rsg/actor.hpp"
#include "rsg/mailbox.hpp"
#include "rsg/host.hpp"

#include "../print.hpp"
using namespace ::simgrid;

int main() {

  RSG_INFO("hello from Server");
  rsg::HostPtr host1 = rsg::Host::by_name("host0");
  host1->turnOff();
  host1->turnOff();
  host1->turnOn();
  host1->turnOff();
  RSG_INFO("%s is on: %s",  host1->getName().c_str(), host1->isOn() ? "YES" : "NO");
  host1->turnOn();
  RSG_INFO("%s is on: %s",  host1->getName().c_str(), host1->isOn() ? "YES" : "NO");

  rsg::this_actor::quit();
  return 0;
}
