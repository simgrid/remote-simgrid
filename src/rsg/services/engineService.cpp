#include "rsg/services.hpp"

#include "xbt.h"
#include "simgrid/s4u.hpp"

#include <iostream>

using namespace ::apache::thrift::server;
using namespace  ::RsgService;

using namespace ::simgrid;


void rsg::RsgEngineHandler::setServer(RsgThriftServerFramework *server) {
    pServer = server;
}

double rsg::RsgEngineHandler::getClock() {
    return SIMIX_get_clock();
}


void rsg::RsgEngineHandler::setKeepAliveOnNextClientDisconnect(const bool newValue) {
// Your implementation goes here
    pServer->setKeepAliveOnNextClientDisconnect(newValue);
}
