#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <string>

#include <librsg.hpp>

int main(int argc, char * argv[])
{
    if (getenv("MUTATED") == NULL)
    {
        rsg::connect();
        std::string fd_string = std::to_string(rsg::connection->socket_fd());
        setenv("LIBRSG_SOCKET_FD", fd_string.c_str(), 1);
        setenv("MUTATED", "1", 1);

        rsg::this_actor::sleep_for(10);
        RSG_INFO("About to mutate into myself.");

        execvp("client-mutant", argv);
        RSG_INFO("Mutation failed.");
    }
    else
    {
        // Retrieve previous socket and tell rsg to use it.
        int socket_fd = atoi(getenv("LIBRSG_SOCKET_FD"));
        rsg::reuse_connected_socket(socket_fd);

        RSG_INFO("Yay, I am still in the simulation!");
        rsg::this_actor::sleep_for(5);
        RSG_INFO("Goodbye :).");
    }

    return 0;
}
