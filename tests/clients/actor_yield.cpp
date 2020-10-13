#include <librsg.hpp>

int main()
{
    RSG_INFO("Hello. About to yield.");
    rsg::this_actor::yield();
    RSG_INFO("Goodbye!");

    return 0;
}
