#pragma once
#include <simgrid/s4u.hpp>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>

#include "RsgActor.h"
#include "RsgMailbox.h"
#include "RsgHost.h"
#include "RsgComm.h"
#include "RsgService_types.h"
#include "actorService.hpp"


using namespace ::apache::thrift::server;

using namespace  ::RsgService;
namespace simgrid  {
    namespace rsg {
        
        class RsgActorHandler;
        class RsgCommHandler;
        
        class RsgMailboxHandler : virtual public RsgMailboxIf {
            friend RsgActorHandler;
            friend RsgCommHandler;
        public:
            RsgMailboxHandler() {
            };
            int64_t mb_create(const std::string& name);
            void setReceiver(const int64_t remoteAddrMailbox, const int64_t remoteAddrActor);
            int64_t getReceiver(const int64_t remoteAddrMailbox);
            bool empty(const int64_t remoteAddrMailbox);
        private:
            static std::unordered_map<int, simgrid::s4u::MailboxPtr> pMailboxes;
            static unsigned long long pMailboxesMapId;
        };
        
        
    }
}
