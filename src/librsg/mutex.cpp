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

rsg::Mutex::~Mutex()
{
    rsg::pb::Decision decision;
    auto mutex = new rsg::pb::Mutex();
    mutex->set_address(_remote_address);
    decision.set_allocated_mutexrefcountdecrease(mutex);

    rsg::pb::DecisionAck ack;
    rsg::connection->send_decision(decision, ack);
}

/** @brief Blocks the calling actor until the mutex can be obtained */
void rsg::Mutex::lock()
{
    rsg::pb::Decision decision;
    auto mutex = new rsg::pb::Mutex();
    mutex->set_address(_remote_address);
    decision.set_allocated_mutexlock(mutex);

    rsg::pb::DecisionAck ack;
    rsg::connection->send_decision(decision, ack);
}

/** @brief Release the ownership of the mutex, unleashing a blocked actor (if any)
 *
 * Will fail if the calling actor does not own the mutex.
 */
void rsg::Mutex::unlock()
{
    rsg::pb::Decision decision;
    auto mutex = new rsg::pb::Mutex();
    mutex->set_address(_remote_address);
    decision.set_allocated_mutexunlock(mutex);

    rsg::pb::DecisionAck ack;
    rsg::connection->send_decision(decision, ack);
}

/** @brief Acquire the mutex if it's free, and return false (without blocking) if not */
bool rsg::Mutex::try_lock()
{
    rsg::pb::Decision decision;
    auto mutex = new rsg::pb::Mutex();
    mutex->set_address(_remote_address);
    decision.set_allocated_mutextrylock(mutex);

    rsg::pb::DecisionAck ack;
    rsg::connection->send_decision(decision, ack);
    RSG_ENFORCE(ack.success(), "Could not try_unlock(addr=%lu)", _remote_address);
    return ack.mutextrylock();
}
