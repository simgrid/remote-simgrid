#include "rsg/actor.hpp"
#include "rsg/host.hpp"
#include "../../../src/common.hpp"

#include <xbt.h>
#include <simgrid/s4u.hpp>

XBT_LOG_NEW_CATEGORY(RSG_THRIFT_CLIENT, "Remote SimGrid");
XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_THRIFT_REMOTE_CLIENT, RSG_THRIFT_CLIENT , "RSG server (Remote SimGrid)");

using namespace ::simgrid;

#define UNUSED(x) (void)(x)

int sleeper(void * args)
{
    UNUSED(args);
    float duration = 10.0f;

    XBT_INFO("sleeper: Sleeping for %g s...", duration);
    rsg::this_actor::sleep(10);
    XBT_INFO("sleeper: Sleeping for %g s... done", duration);

    XBT_INFO("sleeper: Quitting");
    rsg::this_actor::quit();

    return 0;
}

int main()
{
    rsg::HostPtr host1 = rsg::Host::by_name("host1");

    XBT_INFO("Main: Creating actor");
    rsg::Actor* actor = rsg::Actor::createActor("sleeper" , host1 , sleeper, NULL);
    XBT_INFO("Main: Creating actor done");

    float duration = 3.0f;
    XBT_INFO("Main: Sleeping for %g s...", duration);
    rsg::this_actor::sleep(duration);
    XBT_INFO("Main: Sleeping for %g s... done", duration);

    XBT_INFO("Main: Joining actor");
    actor->join();

    delete actor;

    rsg::this_actor::quit();
    return 0;
}
