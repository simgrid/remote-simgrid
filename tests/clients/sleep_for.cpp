#include <stdio.h>
#include <stdlib.h>

#include <string>

#include <librsg/actor.hpp>

int main(int argc, char * argv[])
{
    const char usage[] = "Usage: %s <nb-sleep_for> <duration>\n";
    if (argc != 3)
    {
        printf(usage, argv[0]);
        return 1;
    }

    int nb_sleep_for = std::stoi(std::string(argv[1]));
    double duration = std::stod(std::string(argv[2]));

    for (int i = 0; i < nb_sleep_for; i++)
    {
        rsg::this_actor::sleep_for(duration);
    }

    return 0;
}
