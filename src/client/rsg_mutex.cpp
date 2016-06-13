/* Copyright (c) 2006-2015. The SimGrid Team.
 * All rights reserved.                                                     */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the license (GNU LGPL) which comes with this package. */

#include <xbt/log.h>

#include "rsg/mutex.hpp"
#include "client/RsgClientEngine.hpp"
#include "client/multiThreadedSingletonFactory.hpp"

using namespace ::simgrid;

rsg::Mutex::Mutex() {

  MultiThreadedSingletonFactory factory = MultiThreadedSingletonFactory::getInstance();
  ClientEngine& engine = factory.getEngine(std::this_thread::get_id());
  this->p_remoteAddr = engine.serviceClientFactory<RsgMutexClient>("RsgMutex").mutexInit();

}

void rsg::Mutex::lock(void) {

  MultiThreadedSingletonFactory factory = MultiThreadedSingletonFactory::getInstance();
  ClientEngine& engine = factory.getEngine(std::this_thread::get_id());
  engine.serviceClientFactory<RsgMutexClient>("RsgMutex").lock(this->p_remoteAddr);

}

void rsg::Mutex::unlock(void) {

  MultiThreadedSingletonFactory factory = MultiThreadedSingletonFactory::getInstance();
  ClientEngine& engine = factory.getEngine(std::this_thread::get_id());
  engine.serviceClientFactory<RsgMutexClient>("RsgMutex").unlock(this->p_remoteAddr);

}

bool rsg::Mutex::try_lock(void) {

  MultiThreadedSingletonFactory factory = MultiThreadedSingletonFactory::getInstance();
  ClientEngine& engine = factory.getEngine(std::this_thread::get_id());
  return engine.serviceClientFactory<RsgMutexClient>("RsgMutex").try_lock(this->p_remoteAddr);

}

void rsg::Mutex::destroy() {
  
  MultiThreadedSingletonFactory factory = MultiThreadedSingletonFactory::getInstance();
  ClientEngine& engine = factory.getEngine(std::this_thread::get_id());
  engine.serviceClientFactory<RsgMutexClient>("RsgMutex").destroy(this->p_remoteAddr);

}

rsg::Mutex::~Mutex() {

}
