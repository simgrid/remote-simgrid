/* Copyright (c) 2006-2015. The SimGrid Team.
 * All rights reserved.                                                     */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the license (GNU LGPL) which comes with this package. */

#include <xbt/log.h>

#include "rsg/global.hpp"
#include "client/RsgClientEngine.hpp"
#include "client/multiThreadedSingletonFactory.hpp"


using namespace ::simgrid;

double rsg::getClock() {
  ClientEngine& engine = MultiThreadedSingletonFactory::getInstance().getEngine(std::this_thread::get_id());

  return engine.serviceClientFactory<RsgGlobalServiceClient>("RsgGlobalService").getClock();
}

