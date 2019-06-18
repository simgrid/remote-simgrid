#include <stdio.h>
#include <stdlib.h>

#include <string>

#include <librsg/actor.hpp>
#include <librsg/host.hpp>

void print_whoami(const char * function_name)
{
    auto actor = rsg::Actor::self();
    auto host = actor->get_host();

    printf("Actor(pid=%d, name='%s') on Host(name='%s') executing %s\n",
        actor->get_pid(), actor->get_name().c_str(), host->get_name().c_str(),
        function_name);

    delete host;
    delete actor;
}

void child_actor1(void * args)
{
    int * delay = (int*) args;
    print_whoami("child_actor1");
    rsg::this_actor::sleep_for(*delay);
    delete delay;
}

void child_actor2(void * args)
{
    int * delay = (int*) args;
    print_whoami("child_actor2");
    rsg::this_actor::sleep_for(*delay);
    delete delay;
}

int main(int argc, char * argv[])
{
    auto actor = rsg::Actor::self();
    auto my_host = actor->get_host();

    rsg::Host * host = nullptr;
    if (argc > 1)
        host = rsg::Host::by_name(std::string(argv[1]));
    else
        host = my_host;

    printf("Actor(pid=%d, name='%s') on Host(name='%s') executing main\n",
        actor->get_pid(), actor->get_name().c_str(), my_host->get_name().c_str());
    rsg::Actor::create("fast_child", host, child_actor1, new int(2));
    rsg::Actor::create("slow_child", host, child_actor2, new int(10));
    rsg::this_actor::sleep_for(3);

    delete my_host;
    if (host != my_host)
        delete host;
    delete actor;
    return 0;
}
