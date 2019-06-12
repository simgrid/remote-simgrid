#include <stdio.h>

#include <librsg/client.hpp>

int main()
{
    printf("Manually connecting to RSG server\n");
    rsg::connect();
    printf("Hello from hello_manual_connect.cpp\n");
    return 0;
}
