/* Copyright (c) 2006-2016. The SimGrid Team.
* All rights reserved.                                                     */

/* This program is free software; you can redistribute it and/or modify it
* under the terms of the license (GNU LGPL) which comes with this package. */

#include <xbt/log.h>
#include <iostream>
#include "rsg/conditionVariable.hpp"
#include "RsgClient.hpp"
#include "../rsg/services.hpp"


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
void rsg::ConditionVariable::notify() {
    client->conditionvariable->notify(this->p_remoteAddr);
}

void rsg::ConditionVariable::notify_all() {
    client->conditionvariable->notify_all(this->p_remoteAddr);
}
