#include <stdio.h>
#include <stdlib.h>

#include <string>

#include <librsg.hpp>

int main()
{
    auto actor = rsg::Actor::self();
    const int my_pid = actor->get_pid();
    auto my_host = actor->get_host();
    printf("My actor is (pid=%d, name='%s'), located on Host(name='%s')\n",
        my_pid, actor->get_name().c_str(), my_host->get_cname());

    if (my_pid != rsg::this_actor::get_pid())
        printf("actor pid inconsistency!\n");

    if (rsg::this_actor::is_maestro())
        printf("I think I am maestro, but I am wrong.\n");

    auto my_host2 = rsg::Host::current();
    if (my_host->get_name() != my_host2->get_name())
        printf("actor host inconsistency!\n");

    return 0;
}
