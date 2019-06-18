#include <stdio.h>
#include <stdlib.h>

#include <string>

#include <librsg/actor.hpp>
#include <librsg/host.hpp>

int main()
{
    auto actor = rsg::Actor::self();
    printf("My actor is (pid=%d, name='%s'), located on Host(name='%s')\n",
        actor->get_pid(), actor->get_name().c_str(), actor->get_host()->get_name().c_str());

    return 0;
}
