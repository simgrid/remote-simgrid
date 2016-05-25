#include "rsg/services.hpp"

#include "xbt.h"
#include "simgrid/s4u.h"

#include <iostream>

using namespace ::apache::thrift::server;
using namespace  ::RsgService;

using namespace ::simgrid;

double rsg::RsgGlobalServiceHandler::getClock() {
    return SIMIX_get_clock();
}
