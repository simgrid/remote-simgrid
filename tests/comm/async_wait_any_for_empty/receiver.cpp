#include "rsg/mailbox.hpp"
#include "rsg/host.hpp"

#include <stdio.h>

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
