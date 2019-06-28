#include <stdio.h>
#include <stdlib.h>

#include <string>

#include <librsg.hpp>

void invalid(void * args)
{
    auto parent = (rsg::ActorPtr *) args;
    auto actor = rsg::Actor::self();
    const int my_pid = actor->get_pid();
    const std::string my_name = actor->get_name();

    rsg::this_actor::sleep_until(2);

    // This should fail, as the parent actor has been destroyed in the server.
    printf("Actor(pid=%d, name='%s') Trying to access a dead actor's properties...\n",
        my_pid, my_name.c_str());
    printf("parent actor name: '%s'\n", (*parent)->get_name().c_str());
    delete parent;
}

int main(int argc, char * argv[])
{
    auto actor = rsg::Actor::self();
    auto my_host = actor->get_host();
    const int my_pid = actor->get_pid();
    const std::string my_name = actor->get_name();

    printf("Actor(pid=%d, name='%s') on Host(name='%s') executing main\n",
        my_pid, my_name.c_str(), my_host->get_name().c_str());

    rsg::ActorPtr * do_not_do_that = new rsg::ActorPtr(actor);
    rsg::Actor::create("invalid", my_host, invalid, (void*) do_not_do_that);
    rsg::this_actor::sleep_until(1);

    printf("Actor(pid=%d, name='%s') goodbye!\n", my_pid, my_name.c_str());
    return 0;
}
