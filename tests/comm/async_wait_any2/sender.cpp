#include "rsg/mailbox.hpp"
#include "rsg/host.hpp"

#include <xbt.h>
#include <simgrid/s4u.hpp>

#include <stdio.h>
#include <iostream>
#include <string>

XBT_LOG_NEW_CATEGORY(RSG_THRIFT_CLIENT, "Remote SimGrid");
XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_THRIFT_REMOTE_SERVER, RSG_THRIFT_CLIENT , "RSG server (Remote SimGrid)");

#define NB_WORKERS 10

using namespace ::simgrid;

int Worker(void* arg)
{
    int *msg = (int*) malloc(sizeof(int));
    int arg_int = (*(int*) arg);

    XBT_INFO("Sleeping for %d seconds", arg_int);
    rsg::this_actor::sleep(1);

    std::string mailbox_name = std::string("test") + std::to_string(arg_int);
    rsg::MailboxPtr mbox = rsg::Mailbox::byName(mailbox_name.c_str());

    int * data = (int*) rsg::this_actor::recv(*mbox);
    XBT_INFO("Received %d", *data);
    free(data);
    free(msg);
    return 0;
}

/*
 * This test tests the wait_any function.
 * It creates a pool of actors and a mailbox for each one. Each actor will recv an integer on its mailbox.
 */
int main()
{
    int mboxbuff[NB_WORKERS];
    for (int i = 0; i < NB_WORKERS; i++)
        mboxbuff[i] = i;

    std::vector<rsg::Comm*> comms;

    //We create a pool of actors, and we associate a new Comm to each one of them
    for(int i = 0; i <  NB_WORKERS; i++)
    {
        std::string mailbox_name = std::string("test") + std::to_string(i);
        rsg::MailboxPtr mb = rsg::Mailbox::byName(mailbox_name.c_str());

        rsg::Comm *comm = &rsg::Comm::send_async(*mb, (void**)&(mboxbuff[i]), sizeof(int));
        comms.push_back(comm);

        rsg::Actor::createActor("worker", rsg::Host::by_name("host1"), Worker, (void*) &i);
    }

    for(int i = 0; i < NB_WORKERS; i++)
    {
        auto it = rsg::Comm::wait_any(&comms);
        comms.erase(it);
    }

    XBT_INFO("All comms have finished");
    rsg::this_actor::quit();
    return 0;
}
