#ifndef _RSG_MUTEX_SERVICE_IMPL_
#define _RSG_MUTEX_SERVICE_IMPL_

#include <simgrid/s4u.hpp>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>

#include "RsgMutex.h"

using namespace ::apache::thrift::server;
using namespace ::RsgService;

namespace simgrid  {
    namespace rsg {
        
        class RsgConditionVariableHandler;
        
        class RsgMutexHandler : virtual public RsgMutexIf {
            friend RsgConditionVariableHandler;
        public:
            RsgMutexHandler() {}
            int64_t mutexInit();
            void lock(const int64_t rmtAddr);
            void unlock(const int64_t rmtAddr);
            bool try_lock(const int64_t rmtAddr);
            void destroy(const int64_t rmtAddr);
        private:
            static std::unordered_map<int, simgrid::s4u::MutexPtr> pMutexes;
            static unsigned long long pMutexsMapId;
        };
    }
}
#endif /* _RSG_MUTEX_SERVICE_IMPL_ */ 
