#include "rsg/actor.hpp"
#include "rsg/mailbox.hpp"
#include "rsg/comm.hpp"
#include "rsg/host.hpp"

#include "../../print.hpp"

using namespace ::RsgService;
using namespace ::simgrid;

int simpleReceiver(void * ) {
    rsg::MailboxPtr mbox = rsg::Mailbox::byName("toto");
    rsg::this_actor::recv(*mbox);
    rsg::this_actor::quit();
    return 1;
}

int main() {
    const char *msg = "Do you copy ?";
    rsg::HostPtr host1 = rsg::Host::by_name("host1");

    rsg::MailboxPtr mbox = rsg::Mailbox::byName("toto");
    rsg::Actor* actor = rsg::Actor::createActor("receiver" , host1 , simpleReceiver, NULL);

    rsg::this_actor::send(*mbox,msg, strlen(msg) + 1);
    char *actorname = actor->getName();
    RSG_INFO("actor name: '%s'", actorname);
    RSG_INFO("actor pid: %d", actor->getPid());
    RSG_INFO("host name accessing by actor: '%s'", actor->getHost()->getName().c_str());

    delete actorname;
    rsg::this_actor::quit();
    return 0;
}
