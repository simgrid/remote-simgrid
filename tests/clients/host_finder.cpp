#include <stdio.h>
#include <stdlib.h>

#include <string>

#include <librsg.hpp>

int main(int argc, char * argv[])
{
    const char usage[] = "Usage: %s <host-name>\n";
    if (argc != 2)
    {
        printf(usage, argv[0]);
        return 1;
    }

    const std::string host_name = std::string(argv[1]);
    auto host = rsg::Host::by_name_or_null(host_name);
    printf("Host(name='%s') found? %d\n", host_name.c_str(), host != nullptr);

    printf("Trying to access Host(name='%s')...", host_name.c_str());
    fflush(stdout);
    try {
        host = rsg::Host::by_name(host_name);
        printf(" Got Host(name='%s')!", host->get_name().c_str());
    } catch (const std::exception & e) {
        printf(" Exception caught: %s", e.what());
    }

    return 0;
}
