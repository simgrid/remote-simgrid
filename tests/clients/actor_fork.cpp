#include <librsg.hpp>

#include <sys/types.h>
#include <unistd.h>

int main()
{
    int original_system_pid = getpid();
    RSG_INFO("About to fork...");

    int fork_return = rsg::Actor::fork("newborn", rsg::Actor::self()->get_host());
    if (fork_return == 0)
    {
        RSG_INFO("Hello, I'm a new actor in a new process!");
        if (getpid() == original_system_pid)
            RSG_INFO("ERROR: My system pid equals the original process's...");
    }
    else
    {
        usleep(200*1000); // Helps deterministic server output.
        RSG_INFO("The children process has actor_id=%d", fork_return);
        if (getpid() != original_system_pid)
            RSG_INFO("ERROR: My system pid is different than the original process's...");
    }

    return 0;
}
