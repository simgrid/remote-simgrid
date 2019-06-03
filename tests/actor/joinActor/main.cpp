#include "rsg/actor.hpp"
#include "rsg/mailbox.hpp"
#include "rsg/comm.hpp"
#include "rsg/host.hpp"

#include "../../print.hpp"

#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <thread>
#include <inttypes.h>

#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

using namespace ::simgrid;

class hello
{
public:
    hello(std::string name) : pName(name) {}
    std::string pName;
    int operator()(void *)
    {
        RSG_INFO("Hello");

        rsg::this_actor::quit();
        return 1;
    }
};

int Spawner(void * args)
{
    int spawnerId = *(int*)(args);
    rsg::HostPtr host1 = rsg::Host::by_name("host1");

    // Spawning some actors
    std::vector<rsg::Actor*> actors;
    for(int i = 0; i < spawnerId; i++)
    {
        rsg::Actor* actor = rsg::Actor::createActor("hello" , host1 , hello("hello"), NULL);
        actors.push_back(actor);
    }

    // Shuffling actors
    std::random_shuffle(actors.begin(), actors.end());

    // Joining all actors
    for (auto actor : actors)
    {
        actor->join();
        delete actor;
    }

    rsg::this_actor::quit();
    return 0;
}

int main()
{
    std::srand(0);
    rsg::HostPtr host1 = rsg::Host::by_name("host1");

    for(int i = 0; i < 6; i++)
    {
        RSG_INFO("Running spawner %d", i);
        rsg::Actor* actor = rsg::Actor::createActor("spawner" , host1 , Spawner, (void*) &i);
        actor->join();
        RSG_INFO("Joined spawner %d", i);

        delete actor;
    }

    rsg::this_actor::quit();

    return 0;
}
