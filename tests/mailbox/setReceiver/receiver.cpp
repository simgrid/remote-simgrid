#include "rsg/actor.hpp"
#include "rsg/mailbox.hpp"
#include "rsg/host.hpp"

#include "../../print.hpp"

using namespace ::simgrid;

int main()
{
    rsg::MailboxPtr mbox = rsg::Mailbox::byName("toto");
    char *received = rsg::this_actor::recv(*mbox);

    rsg::this_actor::sleep(1);
    RSG_INFO("Setting receiver to pid=%d...", rsg::this_actor::getPid());
    mbox->setReceiver(rsg::Actor::byPid(rsg::this_actor::getPid()));
    RSG_INFO("Setting receiver to pid=%d... done", rsg::this_actor::getPid());
    rsg::this_actor::sleep(1);

    RSG_INFO("Setting receiver to nullptr...");
    mbox->setReceiver(nullptr);
    RSG_INFO("Setting receiver to nullptr... done");
    RSG_INFO("Received from client: \"%s\" with size of %lu", received, strlen(received) );
    rsg::this_actor::quit();
    return 0;
}
