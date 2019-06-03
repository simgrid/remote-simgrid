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
    rsg::Comm &comm = rsg::Comm::send_init(*mbox);
    comm.setSrcData((void*)msg, strlen(msg) + 1);
    comm.start();
    comm.wait();

    const char *msg2 = "Did you copy?";
    rsg::Comm &comm2 = rsg::Comm::send_async(*mbox, (void*) msg2, strlen(msg2) + 1);
    comm2.wait();

    rsg::this_actor::execute(8095000000 * 2);

    const char *msg3 = "Halo?!";
    rsg::Comm &comm3 = rsg::Comm::send_async(*mbox, (void*) msg3, strlen(msg2) + 1);
    comm3.wait();

    const char *msg4 = "See ya";
    rsg::Comm &comm4 = rsg::Comm::send_init(*mbox);
    comm4.setSrcData((void*)msg4, strlen(msg4));
    comm4.start();

    while(!comm4.test())
    {
        rsg::this_actor::execute(809500000);
        RSG_INFO("Nothing yet, just wait!");
    }

    rsg::this_actor::quit();
    return 0;
}
