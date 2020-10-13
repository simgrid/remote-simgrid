#include <stdio.h>

#include <librsg.hpp>

int main()
{
    printf("Manually connecting to RSG server\n");
    rsg::connect();
    printf("Hello from hello_manual_connect.cpp\n");
    rsg::disconnect();
    return 0;
}
