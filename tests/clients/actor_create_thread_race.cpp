#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <chrono>
#include <thread>

#include <librsg.hpp>

void child_actor(void * args)
{
    printf("child_actor code starts being executed\n");
    fflush(stdout);
}

int main(int argc, char * argv[])
{
    auto actor = rsg::Actor::self();
    auto my_host = actor->get_host();
    auto host = argc > 1 ? rsg::Host::by_name(std::string(argv[1])) : my_host;

    printf("main about to call rsg::Actor::create\n");
    fflush(stdout);
    rsg::Actor::create("child", host, child_actor, new int(2));

    printf("main sleeps for 500 ms (REAL world sleep, not in simulation)\n");
    fflush(stdout);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    printf("main finishes\n");
    fflush(stdout);
    return 0;
}
