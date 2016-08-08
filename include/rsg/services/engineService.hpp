#ifndef _RSG_ENGINE_IMPL_
#define _RSG_ENGINE_IMPL_


#include "RsgEngine.h"
#include "RsgService_types.h"

#include "simgrid/s4u.hpp"

#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>

using namespace ::apache::thrift::server;

using namespace  ::RsgService;
namespace simgrid  {
    namespace rsg {
        
        class RsgEngineHandler : virtual public RsgEngineIf {
        public:
            RsgEngineHandler() {
            }
            void setServer(RsgThriftServerFramework *);
            double getClock();
            void setKeepAliveOnNextClientDisconnect(const bool newValue);
        private:
            RsgThriftServerFramework* pServer;
        };
        
        
    }
}
#endif /* _RSG_GLOBAL_SERVICE_IMPL_ */ 
