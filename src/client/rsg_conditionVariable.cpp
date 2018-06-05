#include <iostream>

#include "rsg/conditionVariable.hpp"
#include "RsgClient.hpp"


using namespace ::simgrid;

void rsg::ConditionVariable::destroy() {
    client->conditionvariable->conditionVariableDestroy(this->p_remoteAddr);
}

rsg::ConditionVariable::ConditionVariable()  {
    this->p_remoteAddr = client->conditionvariable->conditionVariableInit();
}

rsg::ConditionVariable::~ConditionVariable() {
    // client->conditionvariable->conditionVariableDestroy(this->p_remoteAddr);
}

/**
* Wait functions
*/
void rsg::ConditionVariable::wait(rsg::Mutex *mutex) {
    client->conditionvariable->wait(this->p_remoteAddr, mutex->p_remoteAddr);
}

std::cv_status rsg::ConditionVariable::wait_for(rsg::Mutex *mutex, double timeout) {
    rsgConditionVariableStatus::type status = client->conditionvariable->wait_for(this->p_remoteAddr, mutex->p_remoteAddr, timeout);
    return (status == rsgConditionVariableStatus::type::cv_timeout) ? std::cv_status::timeout : std::cv_status::no_timeout;
}

/**
* Notify functions
*/
void rsg::ConditionVariable::notify_one() {
    client->conditionvariable->notify_one(this->p_remoteAddr);
}

void rsg::ConditionVariable::notify_all() {
    client->conditionvariable->notify_all(this->p_remoteAddr);
}
