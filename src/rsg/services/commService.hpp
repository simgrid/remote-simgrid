#pragma once

#include <cstdint>
#include <unordered_map>

#include <simgrid/s4u.hpp>

#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>

#include "RsgActor.h"
#include "RsgMailbox.h"
#include "RsgHost.h"
#include "RsgComm.h"
#include "RsgService_types.h"


using namespace ::apache::thrift::server;
using namespace ::RsgService;

namespace simgrid  {
namespace rsg {
    struct CommData {
        s4u::CommPtr ptr;
        void ** buffer = nullptr;
    };

    class RsgCommHandler : virtual public RsgCommIf {
    public:
        RsgCommHandler();
        int64_t send_init(const int64_t sender, const int64_t dest);
        int64_t recv_init(const int64_t receiver, const int64_t from_);

        int64_t recv_async(const int64_t receiver, const int64_t from_);
        int64_t send_async(const int64_t sender, const int64_t dest, const std::string& data, const int64_t size, const int64_t simulatedByteAmount);

        void start(const int64_t addr);
        void waitComm(std::string& _return, const int64_t addr);

        void setSrcDataSize(const int64_t addr, const int64_t size);
        int64_t getDstDataSize(const int64_t addr);
        void setSrcData(const int64_t addr, const std::string& buff);
        void setDstData(const int64_t addr);

        void setRate(const int64_t addr, const double rate);

        void test(rsgCommBoolAndData& _return, const int64_t addr);
        void wait_any(rsgCommIndexAndData& _return, const std::vector<int64_t> & comms);
        void wait_any_for(rsgCommIndexAndData& _return, const std::vector<int64_t> & comms, const double timeout);
    private:
        /**
         * @brief Stores CommPtrs and their data pointers.
         * @details The key is where the s4u::Comm is in memory (*CommPtr).
         *          The map can thus be used to retrieve a CommPtr from its
         *          final value (which is sent through thrift as an integer).
         */
        static std::unordered_map<uintptr_t, CommData*> * comms;
    };

} }
