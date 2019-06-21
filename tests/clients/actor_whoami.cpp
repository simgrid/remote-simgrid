#include <stdio.h>
#include <stdlib.h>

#include <string>

#include <librsg.hpp>

int main()
{
    auto actor = rsg::Actor::self();
    const int my_pid = actor->get_pid();
    printf("My actor is (pid=%d, name='%s'), located on Host(name='%s')\n",
        my_pid, actor->get_name().c_str(), actor->get_host()->get_name().c_str());

    if (my_pid != rsg::this_actor::get_pid())
        printf("actor pid inconsistency!\n");

    if (rsg::this_actor::is_maestro())
        printf("I think I am maestro, but I am wrong.\n");

    return 0;
}
