#include <stdlib.h>

#include <librsg/connection.hpp>

int main()
{
    setenv("RSG_SERVER_HOSTNAME", "127.0.0.1", 1);
    setenv("RSG_SERVER_PORT", "obviously not a port", 1);

    try {
        rsg::connect();
    } catch (const std::exception & e) {
        printf("%s\n", e.what());
    }
    return 0;
}
