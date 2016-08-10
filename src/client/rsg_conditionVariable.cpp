/* Copyright (c) 2006-2016. The SimGrid Team.
* All rights reserved.                                                     */

/* This program is free software; you can redistribute it and/or modify it
* under the terms of the license (GNU LGPL) which comes with this package. */

#include <xbt/log.h>
#include <iostream>
#include "rsg/conditionVariable.hpp"
#include "RsgClient.hpp"
#include "multiThreadedSingletonFactory.hpp"
#include "../rsg/services.hpp"


using namespace ::simgrid;

void rsg::ConditionVariable::destroy() {
    Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());
    engine.serviceClientFactory<RsgConditionVariableClient>("RsgConditionVariable").conditionVariableDestroy(this->p_remoteAddr);
}

rsg::ConditionVariable::ConditionVariable()  {
    Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());
    this->p_remoteAddr = engine.serviceClientFactory<RsgConditionVariableClient>("RsgConditionVariable").conditionVariableInit();
}

rsg::ConditionVariable::~ConditionVariable() {
    // Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());
    // engine.serviceClientFactory<RsgConditionVariableClient>("RsgConditionVariable").conditionVariableDestroy(this->p_remoteAddr);
}

/**
* Wait functions
*/
void rsg::ConditionVariable::wait(rsg::Mutex *mutex) {
    Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());
    engine.serviceClientFactory<RsgConditionVariableClient>("RsgConditionVariable").wait(this->p_remoteAddr, mutex->p_remoteAddr);
}

std::cv_status rsg::ConditionVariable::wait_for(rsg::Mutex *mutex, double timeout) {
    Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());
    rsgConditionVariableStatus::type status = engine.serviceClientFactory<RsgConditionVariableClient>("RsgConditionVariable").wait_for(this->p_remoteAddr, mutex->p_remoteAddr, timeout);
    return (status == rsgConditionVariableStatus::type::cv_timeout) ? std::cv_status::timeout : std::cv_status::no_timeout;
}

/**
* Notify functions
*/
void rsg::ConditionVariable::notify() {
    Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());
    engine.serviceClientFactory<RsgConditionVariableClient>("RsgConditionVariable").notify(this->p_remoteAddr);
}

void rsg::ConditionVariable::notify_all() {
    Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());
    engine.serviceClientFactory<RsgConditionVariableClient>("RsgConditionVariable").notify_all(this->p_remoteAddr);
}
