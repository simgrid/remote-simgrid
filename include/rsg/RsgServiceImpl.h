#ifndef _RSG_SERVICE_IMPL_
#define _RSG_SERVICE_IMPL_

#include "RsgService.h"
#include "RsgMailbox.h"

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
  void sleep(const double duration);
  void execute(const double flops);
  void recv(std::string& _return, const int64_t mbAddr);
  void send(const int64_t mbAddr, const std::string& content, const int64_t simulatedSize);
  void close();

  private :
    s4u::Actor& pSelf;// =
    TServerFramework* pServer;

};

class RsgMailboxHandler : virtual public RsgMailboxIf {
 public:
  RsgMailboxHandler() {
  };

  int64_t mb_create(const std::string& name);

};

#endif /* _RSG_SERVICE_IMPL_ */
