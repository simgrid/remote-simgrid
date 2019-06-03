#include "rsg/actor.hpp"
#include "rsg/mailbox.hpp"
#include "rsg/host.hpp"

#include "../../print.hpp"

using namespace ::simgrid;

int main()
{
    char *buffer = NULL;
    rsg::MailboxPtr mbox = rsg::Mailbox::byName("toto");

    char *message = rsg::this_actor::recv(*mbox);
    RSG_INFO("Async received: \"%s\" with size=%lu", message, strlen(message));

    rsg::Comm &comm2 = rsg::Comm::recv_async(*mbox, (void**)&buffer);
    rsg::this_actor::execute(8095000000);

    comm2.wait();
    RSG_INFO("Async received: \"%s\" with size=%lu", buffer, strlen(buffer));

    free(buffer);
    buffer = NULL;

    rsg::Comm &comm3 = rsg::Comm::recv_init(*mbox);
    comm3.setDstData((void**)&buffer);
    comm3.start();

    while(!comm3.test())
    {
        rsg::this_actor::execute(809500000);
        RSG_INFO("Nothing yet, just wait!");
    }

    RSG_INFO("Async received: \"%s\" with size=%lu", buffer, strlen(buffer));

    free(buffer);
    buffer = NULL;

    rsg::Comm &comm4 = rsg::Comm::recv_async(*mbox, (void**)&buffer);

    rsg::this_actor::execute(8095000000 * 2);

    comm4.wait();
    RSG_INFO("Async received: \"%s\" with size=%lu", buffer, strlen(buffer));

    rsg::this_actor::quit();
    return 0;
}
