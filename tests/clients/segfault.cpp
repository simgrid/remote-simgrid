#include <signal.h>
#include <stdio.h>

int main()
{
    printf("About to segfault.\n");
    fflush(stdout);
    raise(SIGSEGV);
    return 1;
}
