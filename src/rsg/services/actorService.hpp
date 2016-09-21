#ifndef _RSG_ACTOR_SERVICE_IMPL_
#define _RSG_ACTOR_SERVICE_IMPL_

#include "RsgActor.h"
#include "RsgMailbox.h"
#include "RsgHost.h"
#include "RsgComm.h"
#include "RsgService_types.h"
#include "../RsgThriftSimpleServer.hpp"
#include "../RsgThriftServerFramework.hpp"

#include <simgrid/s4u.hpp>

#include <vector>
#include <unordered_map>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>

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
            void setServer(RsgThriftServerFramework *);
            
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
            void killPid(const int32_t pid);
            int64_t selfAddr();
            int64_t createActor(const int64_t remoteServerAddr, const int32_t port, const std::string& name, const int64_t host, const int32_t killTime);
            void createActorPrepare(rsgServerRemoteAddrAndPort& _return);
            void deleteActor(const int64_t addr);
            int32_t this_actorGetPid();
            int32_t this_actorGetPPid();
            int64_t forPid(const int32_t pid);
            bool isValideActor(const int64_t remoteAddr);
            void createActorForFork(rsgServerRemoteAddrAndPort& _return);
            int32_t getPPid(const int64_t addr);
            private :
            RsgThriftServerFramework* pServer;
            static std::unordered_map<int, simgrid::s4u::ActorPtr> pActors;
            static unsigned long long pActorMapId;
        };
    }
}
#endif /* _RSG_ACTOR_SERVICE_IMPL_ */




