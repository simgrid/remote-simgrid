#include "mutex.hpp"
#include "connection.hpp"
#include "../common/assert.hpp"

#include "rsg.pb.h"

rsg::Mutex::Mutex(uint64_t remote_address) : _remote_address(remote_address)
{
}

rsg::MutexPtr rsg::Mutex::create()
{
    rsg::pb::Decision decision;
    decision.set_mutexcreate(true);

    rsg::pb::DecisionAck ack;
    rsg::connection->send_decision(decision, ack);
    if (!ack.success())
        return nullptr;
    return rsg::MutexPtr(new rsg::Mutex(ack.mutexcreate().address()));
}
