#include "rsg/actor.hpp"
#include "rsg/mailbox.hpp"
#include "rsg/comm.hpp"
#include "rsg/host.hpp"
#include "../../../src/common.hpp"

#include <xbt.h>
#include <simgrid/s4u.hpp>

#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <thread>
#include <inttypes.h>

#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

XBT_LOG_NEW_CATEGORY(RSG_THRIFT_CLIENT, "Remote SimGrid");
XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_THRIFT_REMOTE_CLIENT, RSG_THRIFT_CLIENT , "RSG server (Remote SimGrid)");
using namespace ::simgrid;

#define UNUSED(x) (void)(x)

class hello
{
public:
    hello(std::string name) : pName(name) {}
    std::string pName;
    int operator()(void *)
    {
        XBT_INFO("Hello");

        // Waiting forever
        rsg::MailboxPtr mbox = rsg::Mailbox::byName(this->pName.c_str());
        uint64_t *pid = (uint64_t*) rsg::this_actor::recv(*mbox);
        UNUSED(pid);

        rsg::this_actor::quit();
        return 1;
    }
};

int Spawner(void * args)
{
    int spawnerId = *(int*)(args);
    rsg::HostPtr host1 = rsg::Host::by_name("host1");

    for(int i = 0; i < spawnerId; i++)
    {
        rsg::Actor* actor = rsg::Actor::createActor("hello" , host1 , hello("hello"), NULL);
        int actorPid = actor->getPid();

        XBT_INFO("Killing %d (pid=%d)", i, actorPid);
        s4u::Actor::kill(actorPid);

        delete actor;
    }
    rsg::this_actor::quit();
    return 1;
}

int main()
{
    rsg::HostPtr host1 = rsg::Host::by_name("host1");

    for(int i = 0; i < 6; i++)
    {
        XBT_INFO("Running spawner %d", i);
        rsg::Actor* actor = rsg::Actor::createActor("spawner" , host1 , Spawner, (void*) &i);
        actor->join();
        XBT_INFO("Joined spawner %d", i);
        delete actor;
    }

    rsg::this_actor::quit();
    return 0;
}
