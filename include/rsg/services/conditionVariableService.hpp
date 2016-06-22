#ifndef _RSG_CONDITION_VARIABLE_SERVICE_IMPL_
#define _RSG_CONDITION_VARIABLE_SERVICE_IMPL_

#include "RsgConditionVariable.h"
#include "RsgService_types.h"

#include "simgrid/s4u.h"

using namespace ::apache::thrift::server;

using namespace  ::RsgService;

namespace simgrid  {
namespace rsg {

class RsgConditionVariableHandler : virtual public RsgConditionVariableIf {
 public:
   
  RsgConditionVariableHandler();
  int64_t conditionVariableInit();
  void conditionVariableDestroy(const int64_t remoteAddr);
  void wait(const int64_t remoteAddr, const int64_t mutex);
  void wait_for(const int64_t remoteAddr, const int64_t mutex, const double timeout);
  void notify(const int64_t remoteAddr);
  void notify_all(const int64_t remoteAddr);

};

}} // namespace simgrid::rsg
#endif /* _RSG_CONDITION_VARIABLE_SERVICE_IMPL_ */ 
