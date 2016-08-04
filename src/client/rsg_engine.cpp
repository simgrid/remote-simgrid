/* Copyright (c) 2006-2016. The SimGrid Team.
* All rights reserved.                                                     */

/* This program is free software; you can redistribute it and/or modify it
* under the terms of the license (GNU LGPL) which comes with this package. */

#include <xbt/log.h>

#include "rsg/engine.hpp"
#include "client/RsgClient.hpp"
#include "client/multiThreadedSingletonFactory.hpp"


using namespace ::simgrid;

double rsg::getClock() {
    Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());
    
    return engine.serviceClientFactory<RsgEngineClient>("RsgEngine").getClock();
}

