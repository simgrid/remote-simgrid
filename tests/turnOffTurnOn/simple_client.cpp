#include "rsg/actor.hpp"
#include "rsg/mailbox.hpp"
#include "rsg/comm.hpp"
#include "rsg/host.hpp"

#include "../print.hpp"
using namespace ::simgrid;

int main() {
  RSG_INFO("Client booting");
  RSG_INFO("Client shutting down");
  rsg::this_actor::quit();
  return 0;
}
