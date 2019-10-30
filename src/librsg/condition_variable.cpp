#include "connection.hpp"
#include "condition_variable.hpp"
#include "../common/assert.hpp"

#include "rsg.pb.h"

rsg::ConditionVariable::ConditionVariable(uint64_t remote_address) :
    _remote_address(remote_address) {
}

rsg::ConditionVariable::~ConditionVariable()
{
    rsg::pb::Decision decision;
    auto comm = new rsg::pb::ConditionVariable();
    comm->set_address(_remote_address);
    decision.set_allocated_commrefcountdecrease(comm);

    rsg::pb::DecisionAck ack;
    rsg::connection->send_decision(decision, ack);
}

void wait(MutexPtr lock)
{

}

void wait(std::unique_lock<Mutex> & lock)
{

}

std::cv_status wait_until(std::unique_lock<Mutex> & lock, double timeout_time)
{
    return std::cv_status::no_timeout;
}

std::cv_status wait_for(std::unique_lock<Mutex> & lock, double duration)
{
    return std::cv_status::no_timeout;
}

void notify_one()
{

}

void notify_all()
{

}
