#include "mutex.hpp"
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
}

void wait(rsg::MutexPtr lock)
{

}

void wait(std::unique_lock<rsg::Mutex> & lock)
{

}

std::cv_status wait_until(std::unique_lock<rsg::Mutex> & lock, double timeout_time)
{
    return std::cv_status::no_timeout;
}

std::cv_status wait_for(std::unique_lock<rsg::Mutex> & lock, double duration)
{
    return std::cv_status::no_timeout;
}

void notify_one()
{

}

void notify_all()
{

}
