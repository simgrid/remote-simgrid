#include "rsg/actor.hpp"
#include "rsg/mailbox.hpp"
#include "rsg/comm.hpp"
#include "rsg/host.hpp"

#include "struct.h"

using namespace ::simgrid;
using namespace ::RsgService;

int main()
{
    rsg::MailboxPtr mbox = rsg::Mailbox::byName("toto");

    const char *msg = "Do you copy ? ";
    {
        rsg::Comm &comm = rsg::Comm::send_init(*mbox);
        comm.setSrcData((void*)msg, strlen(msg) + 1);
        comm.start();
        comm.wait();
    }
    int nbElem = 18;
    {
        rsg::Comm &comm = rsg::Comm::send_async(*mbox, (void*) &nbElem, sizeof(int));
        comm.wait();
    }

    int array[18];
    for(int i = 0; i < 18; i++) {
        array[i] = i * i;
    }

    {
        rsg::Comm &comm = rsg::Comm::send_async(*mbox, (void*) &array, sizeof(int) * 18);
        comm.wait();
    }

    structMsg strctMsg;
    strctMsg.intMsg = 123456789;
    strctMsg.msg[0] = 'B';
    strctMsg.msg[1] = 'a';
    strctMsg.msg[2] = 'l';
    strctMsg.msg[3] = 'r';
    strctMsg.msg[4] = 'o';
    strctMsg.msg[5] = 'g';
    strctMsg.msg[6] = '\0';
    {
        rsg::Comm &comm = rsg::Comm::send_async(*mbox, (void*) &strctMsg, sizeof(structMsg));
        comm.wait();
    }

    rsg::this_actor::quit();
    return 0;
}
