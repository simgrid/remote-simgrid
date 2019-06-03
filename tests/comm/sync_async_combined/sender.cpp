#include "rsg/actor.hpp"
#include "rsg/mailbox.hpp"
#include "rsg/comm.hpp"
#include "rsg/host.hpp"

using namespace ::simgrid;

int main()
{
    const char *msg = "Do you copy?";
    rsg::MailboxPtr mbox = rsg::Mailbox::byName("toto");
    rsg::Comm &comm = rsg::Comm::send_init(*mbox);
    comm.setSrcData((void*)msg, strlen(msg) + 1);
    comm.start();
    comm.wait();

    const char *msg2 = "Did you copy?";
    rsg::Comm &comm2 = rsg::Comm::send_async(*mbox, (void*) msg2, strlen(msg2) + 1);
    comm2.wait();

    const char *msg3 = "Halo?!";
    rsg::this_actor::send(*mbox, (char*) msg3, strlen(msg2) + 1);


    const char *msg4 = "See ya";
    rsg::Comm &comm4 = rsg::Comm::send_init(*mbox);
    comm4.setSrcData((void*)msg4, strlen(msg4));
    comm4.start();
    comm4.wait();

    rsg::this_actor::quit();
    return 0;
}
