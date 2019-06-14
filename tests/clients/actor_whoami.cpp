#include <stdio.h>
#include <stdlib.h>

#include <string>

#include <librsg/actor.hpp>

int main()
{
    auto actor = rsg::Actor::self();
    printf("My actor is (pid=%d, name='%s')\n", actor->get_pid(), actor->get_name().c_str());

    return 0;
}
