#include "rsg/actor.hpp"
#include "rsg/mailbox.hpp"
#include "rsg/comm.hpp"
#include "rsg/host.hpp"

#include "../../print.hpp"

using namespace ::simgrid;

int main() {
  const char *msg = "Do you copy ? ";
  rsg::MailboxPtr mbox = rsg::Mailbox::byName("toto");
  rsg::this_actor::send(*mbox,msg, strlen(msg) + 1);
  RSG_INFO("send %s with size : %lu", msg, strlen(msg));
  rsg::this_actor::quit();
  return 0;
}
