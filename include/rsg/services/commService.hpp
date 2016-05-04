#ifndef _RSG_COMM_SERVICE_IMPL_
#define _RSG_COMM_SERVICE_IMPL_

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
  void setRate(const int64_t addr, const double rate);
  void setSrcData(const int64_t addr, const std::string& buff);
  void setDstData(const int64_t addr, const int64_t size);

  private :
  static boost::unordered_map<const int64_t, unsigned long int> *buffers;
};

}
}

#endif /* _RSG_COMM_SERVICE_IMPL_ */
