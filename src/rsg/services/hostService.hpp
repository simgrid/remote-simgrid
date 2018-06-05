#pragma once
#include <simgrid/s4u.hpp>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>

#include "RsgActor.h"
#include "RsgMailbox.h"
#include "RsgHost.h"
#include "RsgComm.h"
#include "RsgService_types.h"


using namespace ::apache::thrift::server;

using namespace  ::RsgService;
namespace simgrid  {
    namespace rsg {
        
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
            int32_t coreCount(const int64_t addr);
            void getProperty(std::string& _return, const int64_t remoteAddr, const std::string& key);
            void setProperty(const int64_t remoteAddr, const std::string& key, const std::string& data);
            private :
            s4u::Host& pSelf;// =
        };
        
    }
}
