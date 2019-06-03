#include "rsg/actor.hpp"
#include "rsg/mailbox.hpp"
#include "rsg/comm.hpp"
#include "rsg/host.hpp"

#include "../../print.hpp"

using namespace ::simgrid;

int main()
{
    const char *msg = "Do you copy?";
    rsg::MailboxPtr mbox = rsg::Mailbox::byName("toto");

    rsg::MailboxPtr fakeMbox = rsg::Mailbox::byName("fake");
    if(fakeMbox->receiver() == nullptr)
    {
        RSG_INFO("No receiver on mailbox \"fake\"");
    }

    rsg::this_actor::send(*mbox, msg, strlen(msg) + 1);
    rsg::this_actor::sleep(2);
    rsg::Actor *receiver = mbox->receiver();
    RSG_INFO("Send \"%s\" to pid=%d with size=%lu",
             msg, receiver->getPid(), strlen(msg));
    delete receiver;

    rsg::this_actor::sleep(10);
    //At this point the receiver should have reset his status
    receiver = mbox->receiver();
    if(receiver == nullptr)
    {
        RSG_INFO("Mailbox receiver has been reseted to nullptr");
    }

    rsg::this_actor::quit();
    return 0;
}
