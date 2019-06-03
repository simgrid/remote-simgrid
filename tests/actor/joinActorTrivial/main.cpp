#include "rsg/actor.hpp"
#include "rsg/host.hpp"

#include "../../print.hpp"

using namespace ::simgrid;

int sleeper(void *)
{
    float duration = 10.0f;

    RSG_INFO("sleeper: Sleeping for %g s...", duration);
    rsg::this_actor::sleep(10);
    RSG_INFO("sleeper: Sleeping for %g s... done", duration);

    RSG_INFO("sleeper: Quitting");
    rsg::this_actor::quit();

    return 0;
}

int main()
{
    rsg::HostPtr host1 = rsg::Host::by_name("host1");

    RSG_INFO("Main: Creating actor");
    rsg::Actor* actor = rsg::Actor::createActor("sleeper" , host1 , sleeper, NULL);
    RSG_INFO("Main: Creating actor done");

    float duration = 3.0f;
    RSG_INFO("Main: Sleeping for %g s...", duration);
    rsg::this_actor::sleep(duration);
    RSG_INFO("Main: Sleeping for %g s... done", duration);

    RSG_INFO("Main: Joining actor");
    actor->join();

    delete actor;

    rsg::this_actor::quit();
    return 0;
}
