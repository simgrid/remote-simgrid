#include <stdio.h>
#include <stdlib.h>

#include <string>

#include <librsg.hpp>

void prehook(void * arg)
{
    printf("prehook arg=%p\n", arg);
}

void posthook(void * arg)
{
    printf("posthook arg=%p\n", arg);
}

void child_actor(void * arg)
{
    RSG_INFO("child_actor arg=%p", arg);
}

int main(int argc, char * argv[])
{
    auto actor = rsg::Actor::self();
    auto my_host = actor->get_host();
    auto host = argc > 1 ? rsg::Host::by_name(std::string(argv[1])) : my_host;

    printf("Actor(pid=%d, name='%s') on Host(name='%s') executing main\n",
        actor->get_pid(), actor->get_name().c_str(), my_host->get_name().c_str());

    rsg::Actor::create("fast_child", host, child_actor, nullptr, prehook, nullptr, posthook, (void *)3);

    rsg::this_actor::sleep_for(3);

    return 0;
}
