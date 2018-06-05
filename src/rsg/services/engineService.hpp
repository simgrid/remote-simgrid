#pragma once
#include <simgrid/s4u.hpp>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>

#include "RsgEngine.h"
#include "RsgService_types.h"


using namespace ::apache::thrift::server;

using namespace  ::RsgService;
namespace simgrid  {
    namespace rsg {
        class RsgEngineHandler : virtual public RsgEngineIf {
        public:
            RsgEngineHandler() {
            }
            double getClock();
        };
    }
}
