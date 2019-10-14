#include <librsg.hpp>

int main()
{
    const double flop = 1e6;
    RSG_INFO("About to compute %g floating-point operations", flop);
    rsg::this_actor::execute(flop);

    return 0;
}
