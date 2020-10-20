#include "mutex.hpp"
#include "connection.hpp"
#include "condition_variable.hpp"
#include "../common/assert.hpp"

#include "rsg.pb.h"

rsg::ConditionVariable::ConditionVariable(uint64_t remote_address) :
    _remote_address(remote_address) {
}

rsg::ConditionVariablePtr rsg::ConditionVariable::create()
{
    rsg::pb::Decision decision;
    decision.set_conditionvariablecreate(true);

    rsg::pb::DecisionAck ack;
    rsg::connection->send_decision(decision, ack);
    if (!ack.success())
        return nullptr;
    return rsg::ConditionVariablePtr(new rsg::ConditionVariable(ack.conditionvariablecreate().address()));
}

rsg::ConditionVariable::~ConditionVariable()
{
    rsg::pb::Decision decision;
    auto cond_var = new rsg::pb::ConditionVariable();
    cond_var->set_address(_remote_address);
    decision.set_allocated_conditionvariablerefcountdecrease(cond_var);

    rsg::pb::DecisionAck ack;
    rsg::connection->send_decision(decision, ack);
}

void rsg::ConditionVariable::wait(std::unique_lock<rsg::Mutex> & lock)
{
    rsg::pb::Decision decision;
    auto cond_var = new rsg::pb::ConditionVariable();
    auto mutex = new rsg::pb::Mutex();
    cond_var->set_address(_remote_address);
    mutex->set_address(lock.mutex()->_remote_address);
    auto cv_wait  = new rsg::pb::Decision_ConditionVariableWait();
    cv_wait->set_allocated_mutex(mutex);
    cv_wait->set_allocated_conditionvariable(cond_var);
    decision.set_allocated_conditionvariablewait(cv_wait);

    rsg::pb::DecisionAck ack;
    rsg::connection->send_decision(decision, ack);
    RSG_ENFORCE(ack.success(), "Could not wait on ConditionVariable");
}

std::cv_status rsg::ConditionVariable::wait_until(std::unique_lock<rsg::Mutex> & lock, double timeout_time)
{
    rsg::pb::Decision decision;
    auto cond_var = new rsg::pb::ConditionVariable();
    auto mutex = new rsg::pb::Mutex();
    cond_var->set_address(_remote_address);
    mutex->set_address(lock.mutex()->_remote_address);
    auto cv_waituntil  = new rsg::pb::Decision_ConditionVariableWaitUntil();
    cv_waituntil->set_allocated_mutex(mutex);
    cv_waituntil->set_allocated_conditionvariable(cond_var);
    cv_waituntil->set_timeout_time(timeout_time);
    decision.set_allocated_conditionvariablewaituntil(cv_waituntil);

    rsg::pb::DecisionAck ack;
    rsg::connection->send_decision(decision, ack);
    RSG_ENFORCE(ack.success(), "Could not wait_until on ConditionVariable");

    return 0 == ack.conditionvariablewaituntil() ? std::cv_status::no_timeout : std::cv_status::timeout;
}

std::cv_status rsg::ConditionVariable::wait_for(std::unique_lock<rsg::Mutex> & lock, double duration)
{
    rsg::pb::Decision decision;
    auto cond_var = new rsg::pb::ConditionVariable();
    auto mutex = new rsg::pb::Mutex();
    cond_var->set_address(_remote_address);
    mutex->set_address(lock.mutex()->_remote_address);
    auto cv_waitfor  = new rsg::pb::Decision_ConditionVariableWaitFor();
    cv_waitfor->set_allocated_mutex(mutex);
    cv_waitfor->set_allocated_conditionvariable(cond_var);
    cv_waitfor->set_duration(duration);
    decision.set_allocated_conditionvariablewaitfor(cv_waitfor);

    rsg::pb::DecisionAck ack;
    rsg::connection->send_decision(decision, ack);
    RSG_ENFORCE(ack.success(), "Could not wait_for on ConditionVariable");

    return 0 == ack.conditionvariablewaituntil() ? std::cv_status::no_timeout : std::cv_status::timeout;
}

void rsg::ConditionVariable::notify_one()
{
    rsg::pb::Decision decision;
    auto cond_var = new rsg::pb::ConditionVariable();
    cond_var->set_address(_remote_address);
    decision.set_allocated_conditionvariablenotifyone(cond_var);

    rsg::pb::DecisionAck ack;
    rsg::connection->send_decision(decision, ack);
    RSG_ENFORCE(ack.success(), "Could not notify_one on ConditionVariable");
}

void rsg::ConditionVariable::notify_all()
{
    rsg::pb::Decision decision;
    auto cond_var = new rsg::pb::ConditionVariable();
    cond_var->set_address(_remote_address);
    decision.set_allocated_conditionvariablenotifyall(cond_var);

    rsg::pb::DecisionAck ack;
    rsg::connection->send_decision(decision, ack);
    RSG_ENFORCE(ack.success(), "Could not notify_one on ConditionVariable");
}
