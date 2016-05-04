#ifndef _RSG_ACTOR_SERVICE_IMPL_
#define _RSG_ACTOR_SERVICE_IMPL_

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
class RsgActorHandler : virtual public RsgActorIf {

 public:
  RsgActorHandler();

  void setServer(TServerFramework *);

  protected :
  void sleep(const double duration);
  void execute(const double flops);
  void recv(std::string& _return, const int64_t mbAddr);
  void send(const int64_t mbAddr, const std::string& content, const int64_t simulatedSize);
  void close();
  void getName(std::string& _return, const int64_t addr);
  void getHost(rsgHostCurrentResType& _return, const int64_t addr);
  int32_t getPid(const int64_t addr);
  void setAutoRestart(const int64_t addr, const bool autorestart);
  void setKillTime(const int64_t addr, const double time);
  double getKillTime(const int64_t addr);
  void killAll();
  private :
    s4u::Actor& pSelf;// =
    TServerFramework* pServer;

};
}
}
#endif /* _RSG_ACTOR_SERVICE_IMPL_ */
