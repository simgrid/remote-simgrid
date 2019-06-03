#include "rsg/actor.hpp"
#include "rsg/mailbox.hpp"
#include "rsg/host.hpp"

#include "../../print.hpp"

using namespace ::simgrid;

int main() {
  rsg::MailboxPtr mbox = rsg::Mailbox::byName("toto");
  rsg::this_actor::sleep(1);
  RSG_INFO("receiver : is the mb empty : %s", mbox->empty() ? "true" : "false");
  char *received = rsg::this_actor::recv(*mbox);
  RSG_INFO("receiver : is the mb empty after recv : %s", mbox->empty() ? "true" : "false");
  RSG_INFO("Received from client : %s with size of %lu", received, strlen(received) );
  rsg::this_actor::quit();
  return 0;
}
