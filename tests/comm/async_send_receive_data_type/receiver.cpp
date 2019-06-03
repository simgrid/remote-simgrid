#include "rsg/actor.hpp"
#include "rsg/mailbox.hpp"
#include "rsg/host.hpp"

#include "../../print.hpp"

#include "struct.h"

using namespace ::RsgService;
using namespace ::simgrid;

int main()
{
    rsg::MailboxPtr mbox = rsg::Mailbox::byName("toto");

    char *buffer = NULL;
    {
        rsg::Comm &comm = rsg::Comm::recv_init(*mbox);
        comm.setDstData((void**)&buffer);
        comm.start();
        comm.wait();
    }
    RSG_INFO("Asynchronously received message (size=%zu, content='%s')", strlen(buffer), buffer);

    free(buffer);
    buffer = NULL;

    int *nbElement = NULL;
    {
        rsg::Comm &comm = rsg::Comm::recv_async(*mbox, (void**)&nbElement);
        comm.wait();
    }

    RSG_INFO("I will receive an array of %d elements", *nbElement);

    int *array = NULL;
    {
        rsg::Comm &comm = rsg::Comm::recv_async(*mbox, (void**)&array);
        comm.wait();
    }
    for(int i = 0 ; i < *nbElement; i++)
    {
        RSG_INFO("array[%2d] = %d", i, array[i]);
    }

    free(nbElement);
    free(array);

    structMsg *recStruct = NULL;
    {
        rsg::Comm &comm = rsg::Comm::recv_async(*mbox, (void**)&recStruct);
        comm.wait();
    }
    RSG_INFO("recStruct->intMsg = %d", recStruct->intMsg);
    RSG_INFO("recStruct->msg = '%s'", recStruct->msg);

    free(recStruct);

    rsg::this_actor::quit();
    return 0;
}
