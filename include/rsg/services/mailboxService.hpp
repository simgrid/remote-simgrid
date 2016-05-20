#ifndef _RSG_MAILBOX_SERVICE_IMPL_
#define _RSG_MAILBOX_SERVICE_IMPL_

#include "RsgActor.h"
#include "RsgMailbox.h"
#include "RsgHost.h"
#include "RsgComm.h"
#include "RsgService_types.h"

#include "simgrid/s4u.h"

#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>

using namespace ::apache::thrift::server;

using namespace  ::RsgService;
namespace simgrid  {
namespace rsg {

class RsgMailboxHandler : virtual public RsgMailboxIf {
 public:
  RsgMailboxHandler() {
  };
  int64_t mb_create(const std::string& name);
};

}
}
#endif /* _RSG_MAILBOX_SERVICE_IMPL_ */ 
