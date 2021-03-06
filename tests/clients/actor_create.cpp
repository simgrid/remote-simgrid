#include <stdio.h>
#include <stdlib.h>

#include <string>

#include <librsg.hpp>

void print_whoami(const char * function_name)
{
    auto actor = rsg::Actor::self();

    printf("Actor(pid=%d, name='%s') on Host(name='%s') executing %s\n",
        actor->get_pid(), actor->get_name().c_str(),
        actor->get_host()->get_name().c_str(), function_name);
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
    auto host = argc > 1 ? rsg::Host::by_name(std::string(argv[1])) : my_host;

    printf("Actor(pid=%d, name='%s') on Host(name='%s') executing main\n",
        actor->get_pid(), actor->get_name().c_str(), my_host->get_name().c_str());
    rsg::Actor::create("fast_child", host, child_actor1, new int(2));
    rsg::Actor::create("slow_child", host, child_actor2, new int(10));
    rsg::this_actor::sleep_for(3);

    return 0;
}
