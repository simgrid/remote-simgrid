#ifndef _RSG_MUTEX_SERVICE_IMPL_
#define _RSG_MUTEX_SERVICE_IMPL_

#include "simgrid/s4u.h"
#include "RsgMutex.h"

#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>

using namespace ::apache::thrift::server;

using namespace  ::RsgService;
namespace simgrid  {
namespace rsg {

  class RsgMutexHandler : virtual public RsgMutexIf {
   public:
    RsgMutexHandler() {}
    int64_t mutexInit();
    void lock(const int64_t rmtAddr);
    void unlock(const int64_t rmtAddr);
    bool try_lock(const int64_t rmtAddr);
    void destroy(const int64_t rmtAddr);
  };
}
}
#endif /* _RSG_MUTEX_SERVICE_IMPL_ */ 
