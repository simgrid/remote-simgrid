#include "rsg/actor.hpp"
#include "rsg/mailbox.hpp"
#include "rsg/comm.hpp"
#include "rsg/host.hpp"
#include "rsg/engine.hpp"

#include <xbt.h>
#include <simgrid/s4u.hpp>

#include <iostream>

XBT_LOG_NEW_CATEGORY(RSG_THRIFT_CLIENT, "Remote SimGrid");
XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_THRIFT_REMOTE_CLIENT, RSG_THRIFT_CLIENT , "RSG server (Remote SimGrid)");

using namespace ::simgrid;


int actor(void *)
{
    rsg::MailboxPtr mbox = rsg::Mailbox::byName("toto");
    XBT_INFO("%10.9f <- clock before receive (child)", rsg::getClock());
    rsg::this_actor::recv(*mbox);
    XBT_INFO("%10.9f <- clock after receive (child)", rsg::getClock());
    rsg::this_actor::quit();
    return 1;
}

int main() {
    const char *msg = "Do you copy ?";
    rsg::HostPtr host1 = rsg::Host::by_name("host1");

    XBT_INFO("%10.9f <- clock before execute", rsg::getClock());
    rsg::this_actor::execute(8095000000 * 2);
    XBT_INFO("%10.9f <- clock after execute", rsg::getClock());

    rsg::Actor *act = rsg::Actor::createActor("receiver" , host1 , actor, NULL);

    rsg::MailboxPtr mbox = rsg::Mailbox::byName("toto");

    XBT_INFO("%10.9f <- clock before send", rsg::getClock());
    rsg::this_actor::send(*mbox,msg, strlen(msg) + 1, 115476000);
    XBT_INFO("%10.9f <- clock after send", rsg::getClock());
    act->join();
    rsg::this_actor::quit();
    return 0;
}

