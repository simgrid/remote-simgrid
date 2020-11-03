#include <stdio.h>
#include <librsg.hpp>

int main()
{
    double before = rsg::Engine::get_clock();

    for (int i = 0; i < 5; i++) {
        double now = rsg::Engine::get_clock();
        if (now <= before)
            printf("Time inconsistency: consecutive calls to get_clock() do not lead to strictly increasing time\n");
        else
            printf("Time has increased consistently\n");
    }

    printf("Bye!\n");
    return 0;
}
