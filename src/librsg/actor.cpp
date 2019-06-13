#include "actor.hpp"
#include "connection.hpp"

#include "rsg.pb.h"

void rsg::this_actor::quit()
{
    rsg::disconnect();
}
