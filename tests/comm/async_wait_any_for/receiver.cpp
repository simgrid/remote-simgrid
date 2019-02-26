#include "rsg/mailbox.hpp"
#include "rsg/host.hpp"

#include <xbt.h>
#include <simgrid/s4u.hpp>

#include <stdio.h>
#include <iostream>

XBT_LOG_NEW_CATEGORY(RSG_THRIFT_CLIENT, "Remote SimGrid");
XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_THRIFT_REMOTE_SERVER, RSG_THRIFT_CLIENT , "RSG server (Remote SimGrid)");


#define NB_WORKERS 10

using namespace ::simgrid;

int Worker(void* arg)
{
    int *msg = (int*) malloc(sizeof(int));
    int arg_int = (*(int*) arg);
    *msg = (arg_int + 1) * 10;
    rsg::this_actor::sleep(*msg);
    rsg::MailboxPtr mbox = rsg::Mailbox::byName("test");
    rsg::this_actor::send(*mbox, (char*) msg, sizeof(int));
    rsg::this_actor::quit();
    free(msg);
    return 0;
}

/*
 * This test test the wait_any function.
 * It creates a pool of actors and a new comm for each one. Each actor will send a sync on the
 */
int main()
{
    int *mboxbuff[10];
    std::vector<rsg::Comm*> comms;
    rsg::MailboxPtr mb = rsg::Mailbox::byName("test");

    //We create a pool of actors, and we associate a new Comm to each one of them
    for(int i = 0; i < NB_WORKERS; i++)
    {
        rsg::Comm *comm = &rsg::Comm::recv_init(*mb);
        comm->setDstData((void**)&(mboxbuff[i]));
        comms.push_back(comm);
        comm->start();

        rsg::Actor::createActor("worker", rsg::Host::by_name("host1"), Worker, (void*) &i);
    }

    int nb_finished = 0;
    while (nb_finished < NB_WORKERS)
    {
        auto it = rsg::Comm::wait_any_for(comms.begin(), comms.end(), (nb_finished+1)*2);
        if (it == comms.end())
            XBT_INFO("left wait_any_for on timeout");
        else
        {
            nb_finished++;
            XBT_INFO("left wait_any_for on finished comm");
            comms.erase(it);
        }
    }

    for(int i = 0; i < NB_WORKERS; i++)
    {
        XBT_INFO("%d", *mboxbuff[i]);
        free(mboxbuff[i]);
    }

    rsg::this_actor::quit();
    return 0;
}
