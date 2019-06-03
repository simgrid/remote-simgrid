#include "rsg/actor.hpp"
#include "rsg/mailbox.hpp"
#include "rsg/comm.hpp"
#include "rsg/host.hpp"
#include "rsg/engine.hpp"

#include "../../print.hpp"

using namespace ::simgrid;

int main() {
  rsg::MailboxPtr mbox = rsg::Mailbox::byName("toto");

  pid_t pid = rsg::this_actor::fork("child");
  if(0 == pid) { // child
    RSG_INFO("My pid is %d",rsg::this_actor::getPid());
    rsg::Actor* me = rsg::Actor::self();
    if( rsg::this_actor::getPid() != me->getPid())
        RSG_INFO("ERROR in self()");
    delete me;
    RSG_INFO("Fork returned %d", pid);

    RSG_INFO("Message from Daddy: '%s'", rsg::this_actor::recv(*mbox));
    rsg::this_actor::quit();
    return 0;
  }
  const char *msg = "GaBuZoMeu";
  rsg::this_actor::send(*mbox, msg, strlen(msg) + 1);

  RSG_INFO("My pid is %d",rsg::this_actor::getPid());
  rsg::Actor* me = rsg::Actor::self();
  if( rsg::this_actor::getPid() != me->getPid())
    RSG_INFO("ERROR in self()");
  delete me;
  RSG_INFO("The pid of my child is %d", pid);
  rsg::this_actor::quit();
  return 0;
}
