#ifndef _RSG_GLOBAL_SERVICE_IMPL_
#define _RSG_GLOBAL_SERVICE_IMPL_


#include "RsgGlobalService.h"
#include "RsgService_types.h"

#include "simgrid/s4u.h"

#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>

using namespace ::apache::thrift::server;

using namespace  ::RsgService;
namespace simgrid  {
namespace rsg {

class RsgGlobalServiceHandler : virtual public RsgGlobalServiceIf {
 public:
  RsgGlobalServiceHandler() {
  }

  double getClock();

};


}
}
#endif /* _RSG_GLOBAL_SERVICE_IMPL_ */ 
