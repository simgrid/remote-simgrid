/* Copyright (c) 2006-2016. The SimGrid Team.
* All rights reserved.                                                     */

/* This program is free software; you can redistribute it and/or modify it
* under the terms of the license (GNU LGPL) which comes with this package. */

#include <xbt/log.h>

#include "rsg/engine.hpp"
#include "RsgClient.hpp"
#include "multiThreadedSingletonFactory.hpp"
#include "../rsg/services.hpp"



using namespace ::simgrid;

double rsg::getClock() {
    Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());
    
    return engine.serviceClientFactory<RsgEngineClient>("RsgEngine").getClock();
}

void rsg::closeConnection() {
    Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());
    engine.close();
}

int rsg::getRpcPort() {
    Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());
    return engine.getRpcPort();
}

void rsg::setKeepAliveOnNextClientDisconnect(bool newValue) {
    Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());
    engine.serviceClientFactory<RsgEngineClient>("RsgEngine").setKeepAliveOnNextClientDisconnect(newValue);
}
