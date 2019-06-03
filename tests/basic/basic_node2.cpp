#include "rsg/actor.hpp"
#include "rsg/host.hpp"
#include "rsg/mailbox.hpp"

#include "../print.hpp"

using namespace ::simgrid;

int main() {
  RSG_INFO("hello from server");

  rsg::HostPtr host = rsg::Host::current();

  RSG_INFO("hostname: %s", rsg::Host::current()->getName().c_str());
  RSG_INFO("host current power peak: %f", host->currentPowerPeak());
  RSG_INFO("core count: %d", host->coreCount());
  RSG_INFO("pstate count: %d", host->pstatesCount());
  RSG_INFO("pstate: %d", host->pstate());

  rsg::this_actor::quit();
  return 0;
}
