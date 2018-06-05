#pragma once

#include <simgrid/s4u.hpp>

#include <vector>
#include <unordered_map>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>

#include "RsgActor.h"
#include "RsgMailbox.h"
#include "RsgHost.h"
#include "RsgComm.h"
#include "RsgService_types.h"

#include "../RsgThriftServer.hpp"

using namespace ::apache::thrift::server;

using namespace  ::RsgService;
namespace simgrid  {
    namespace rsg {
        class RsgMailboxHandler;
        class RsgActorHandler : virtual public RsgActorIf {
            friend RsgMailboxHandler;
        public:
            RsgActorHandler();
            ~RsgActorHandler() {};
            bool server_exit;
            
        protected:
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
            void kill(const int64_t mbAddr);
            void join(const int64_t addr);
            int32_t killPid(const int32_t pid);
            int64_t selfAddr();
            int64_t createActor(const std::string& name, const int64_t host);
            void createActorPrepare(std::string& _return);
            void deleteActor(const int64_t addr);
            int32_t this_actorGetPid();
            int32_t this_actorGetPPid();
            int64_t byPid(const int32_t pid);
            int32_t getPPid(const int64_t addr);
        private :
            static std::unordered_map<int, simgrid::s4u::ActorPtr> pActors;
            static unsigned long long pActorMapId;
            RsgThriftServer* lastChildServer;
        };
    }
}
