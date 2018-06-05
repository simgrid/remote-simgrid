#include "rsg/actor.hpp"
#include "rsg/mailbox.hpp"
#include "rsg/comm.hpp"
#include "rsg/host.hpp"

#include <xbt.h>
#include <simgrid/s4u.hpp>

#include <iostream>

XBT_LOG_NEW_CATEGORY(RSG_THRIFT_CLIENT, "Remote SimGrid");
XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_THRIFT_REMOTE_CLIENT, RSG_THRIFT_CLIENT , "RSG server (Remote SimGrid)");

using namespace ::simgrid;
using boost::shared_ptr;

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
