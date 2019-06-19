#include <stdlib.h>

#include <librsg/connection.hpp>

int main()
{
    setenv("RSG_INITIAL_ACTOR_ID", "clearly not an actor_id", 1);

    try {
        rsg::connect();
    } catch (const std::exception & e) {
        printf("%s\n", e.what());
    }

    return 0;
}
