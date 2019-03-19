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

int main()
{
    std::vector<rsg::Comm*> comms;
    printf("comms size before: %zu\n", comms.size());
    auto it = rsg::Comm::wait_any_for(comms.begin(), comms.end(), 10);

    printf("comms size after: %zu\n", comms.size());
    if (it == comms.end())
        printf("returned iterator at comm's end position\n");
    else
        printf("returned iterator NOT at comm's end position\n");

    rsg::this_actor::quit();
    return 0;
}
