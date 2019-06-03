#include "rsg/actor.hpp"
#include "rsg/comm.hpp"
#include "rsg/engine.hpp"
#include "rsg/host.hpp"
#include "rsg/mailbox.hpp"

#include "../print.hpp"

using namespace ::simgrid;

int main() {
  rsg::HostPtr host1 = rsg::Host::by_name("host1");

  RSG_INFO("hostname='%s', speed=%f", host1->getName().c_str(), host1->speed());
  RSG_INFO("hostname='%s', speed=%f", rsg::Host::current()->getName().c_str(),
           rsg::Host::current()->speed());

  rsg::this_actor::execute(8095000000 * 1.999999);
  rsg::this_actor::sleep(1);
  rsg::this_actor::quit();

  return 0;
}
