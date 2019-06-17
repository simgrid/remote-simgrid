#include <stdio.h>
#include <stdlib.h>

#include <string>

#include <librsg/actor.hpp>
#include <librsg/host.hpp>

void child_actor()
{
    printf("Hello from child!\n");
}

int main()
{
    auto actor = rsg::Actor::self();
    auto host = actor->get_host();

    printf("My actor is (pid=%d, name='%s'), located on Host(name='%s')\n",
        actor->get_pid(), actor->get_name().c_str(), host->get_name().c_str());
    printf("Creating another Actor\n");
    rsg::Actor::create("child", host, child_actor);
    delete host;

    return 0;
}
