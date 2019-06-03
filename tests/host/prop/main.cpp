#include "rsg/actor.hpp"
#include "rsg/mailbox.hpp"
#include "rsg/host.hpp"

#include "../../print.hpp"

using namespace ::simgrid;

int main() {
  const char *prop_msg = rsg::Host::current()->property("message");
  const char *prop_msg_null = rsg::Host::current()->property("nothing");
  RSG_INFO("Host message : %s", prop_msg);
  RSG_INFO("nothing : %s", prop_msg_null);
  rsg::this_actor::quit();
  delete[] prop_msg;
  return 0;
}
