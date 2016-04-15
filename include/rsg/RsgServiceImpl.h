#ifndef _RSG_SERVICE_IMPL_
#define _RSG_SERVICE_IMPL_


#include "RsgService.h"
#include "simgrid/s4u.h"

#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>

using namespace ::apache::thrift::server;

using namespace  ::RsgService;
using namespace simgrid;

class RsgServiceHandler : virtual public RsgServiceIf {

 public:
  RsgServiceHandler();

  void setServer(TServerFramework *);

  protected :
    void sleep(const int32_t duration);
    void close();

  private :
    s4u::Actor& pSelf;// =
    TServerFramework* pServer;

};

#endif /* _RSG_SERVICE_IMPL_ */
