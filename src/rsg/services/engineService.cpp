#include <simgrid/s4u.hpp>
#include <iostream>

#include "../services.hpp"

using namespace ::apache::thrift::server;
using namespace  ::RsgService;

using namespace ::simgrid;


double rsg::RsgEngineHandler::getClock() {
    return SIMIX_get_clock();
}

