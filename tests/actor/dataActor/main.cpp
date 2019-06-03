#include "rsg/actor.hpp"
#include "rsg/mailbox.hpp"
#include "rsg/comm.hpp"
#include "rsg/host.hpp"

#include "../../print.hpp"

using namespace ::simgrid;

int actor(void *data) {
  RSG_INFO("data -> %s ", (char*) data );
  rsg::this_actor::quit();
  return 1;
}

int main() {
  const char *msg = "Do you copy ? ";
  rsg::HostPtr host1 = rsg::Host::by_name("host1");

  rsg::Actor *act = rsg::Actor::createActor("receiver" ,host1 , actor,(void*) msg);
  act->join();
  rsg::this_actor::quit();
  return 0;
}
