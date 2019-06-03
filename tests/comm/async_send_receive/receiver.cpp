#include "rsg/actor.hpp"
#include "rsg/mailbox.hpp"
#include "rsg/host.hpp"

#include "../../print.hpp"

using namespace ::simgrid;

int main()
{
    char *buffer = NULL;
    rsg::MailboxPtr mbox = rsg::Mailbox::byName("toto");
    rsg::Comm &comm = rsg::Comm::recv_init(*mbox);
    comm.setDstData((void**)&buffer);
    comm.start();
    rsg::this_actor::execute(8095000000 * 1.999999);
    comm.wait();
    RSG_INFO("Async received: \"%s\" with size=%zu", buffer, strlen(buffer));

    free(buffer);
    buffer = NULL;

    rsg::Comm &comm2 = rsg::Comm::recv_async(*mbox, (void**)&buffer);
    rsg::this_actor::execute(8095000000);
    comm2.wait();
    RSG_INFO("Async received: \"%s\" with size=%zu", buffer, strlen(buffer));

    free(buffer);
    buffer = NULL;

    rsg::Comm &comm3 = rsg::Comm::recv_init(*mbox);
    comm3.setDstData((void**)&buffer);
    comm3.start();
    rsg::this_actor::execute(8095000000 * 2);
    comm3.wait();
    RSG_INFO("Async received: \"%s\" with size=%zu", buffer, strlen(buffer));

    free(buffer);
    buffer = NULL;

    rsg::Comm &comm4 = rsg::Comm::recv_async(*mbox, (void**)&buffer);
    rsg::this_actor::execute(8095000000);
    comm4.wait();
    RSG_INFO("Async received: \"%s\" with size=%zu", buffer, strlen(buffer));

    rsg::this_actor::quit();
    return 0;
}
