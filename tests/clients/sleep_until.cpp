#include <stdio.h>
#include <stdlib.h>

#include <string>

#include <librsg.hpp>

int main(int argc, char * argv[])
{
    const char usage[] = "Usage: %s <timeout>\n";
    if (argc != 2)
    {
        printf(usage, argv[0]);
        return 1;
    }

    double timeout = std::stod(std::string(argv[1]));
    rsg::this_actor::sleep_until(timeout);

    return 0;
}
