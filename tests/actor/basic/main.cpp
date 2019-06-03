#include "rsg/actor.hpp"
#include "rsg/mailbox.hpp"
#include "rsg/comm.hpp"
#include "rsg/host.hpp"
#include "rsg/engine.hpp"

#include "../../print.hpp"

using namespace ::simgrid;

int actor(void *) {
  rsg::MailboxPtr mbox = rsg::Mailbox::byName("toto");
  char *received = rsg::this_actor::recv(*mbox);
  RSG_INFO("Received message (size=%lu, content='%s')", strlen(received), received);
  RSG_INFO("My id is %d",rsg::this_actor::getPid());
  rsg::this_actor::sleep(99.999999);
  rsg::this_actor::quit();
  return 1;
}

int main() {
  const char *msg = "Do you copy ?";
  rsg::HostPtr host1 = rsg::Host::by_name("host1");

  rsg::Actor *act = rsg::Actor::createActor("receiver" , host1 , actor, NULL);
  rsg::MailboxPtr mbox = rsg::Mailbox::byName("toto");
  RSG_INFO("I'll send '%s' with size=%lu", msg, strlen(msg));

  rsg::this_actor::send(*mbox,msg, strlen(msg) + 1);
  RSG_INFO("Send '%s' with size=%lu", msg, strlen(msg));

  RSG_INFO("My id is %d", rsg::this_actor::getPid());

  rsg::Actor *getByPid = rsg::Actor::byPid(act->getPid());
  getByPid->join();
  RSG_INFO("Other actor name: '%s'", getByPid->getName());
  RSG_INFO("Clock is %.2lf", rsg::getClock());

  rsg::this_actor::quit();
  return 0;
}
