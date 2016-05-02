#ifndef _RSG_SERVICE_IMPL_
#define _RSG_SERVICE_IMPL_

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
using namespace simgrid;

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

class RsgMailboxHandler : virtual public RsgMailboxIf {
 public:
  RsgMailboxHandler() {
  };
  int64_t mb_create(const std::string& name);

};

class RsgHostHandler : virtual public RsgHostIf {
 public:
  RsgHostHandler();
  int64_t by_name(const std::string& name);
  void current(rsgHostCurrentResType& _return);
  double speed(const int64_t addr);
  void turnOn(const int64_t addr);
  void turnOff(const int64_t addr);
  bool isOn(const int64_t addr);
  double currentPowerPeak(const int64_t addr);
  double powerPeakAt(const int64_t addr, const int32_t pstate_index);
  int32_t pstatesCount(const int64_t addr);
  void setPstate(const int64_t addr, const int32_t pstate_index);
  int32_t pstate(const int64_t addr);
  int32_t core_count(const int64_t addr);

  private :
    s4u::Host& pSelf;// =
};


class RsgCommHandler : virtual public RsgCommIf {
 public:
  RsgCommHandler();
  int64_t send_init(const int64_t sender, const int64_t dest);
  int64_t recv_init(const int64_t receiver, const int64_t from_);

  int64_t recv_async(const int64_t receiver, const int64_t from_);
  int64_t send_async(const int64_t sender, const int64_t dest, const std::string& data, const int32_t simulatedByteAmount);

  void start(const int64_t addr);
  void wait(std::string& _return, const int64_t addr);
  void setSrcDataSize(const int64_t addr, const int64_t size);
  int64_t getDstDataSize(const int64_t addr);
  void setRate(const double rate);
  void setSrcData(const int64_t addr, const std::string& buff);
  void setDstData(const int64_t addr, const int64_t size);

  private :
  static boost::unordered_map<const int64_t, unsigned long int> *buffers;
};

#endif /* _RSG_SERVICE_IMPL_ */
