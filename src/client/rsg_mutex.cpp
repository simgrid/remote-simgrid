/* Copyright (c) 2006-2016. The SimGrid Team.
* All rights reserved.                                                     */

/* This program is free software; you can redistribute it and/or modify it
* under the terms of the license (GNU LGPL) which comes with this package. */

#include <xbt/log.h>
#include <iostream>

#include "rsg/mutex.hpp"
#include "RsgClient.hpp"
#include "../rsg/services.hpp"


using namespace ::simgrid;

rsg::Mutex::Mutex() {
    this->p_remoteAddr = client->mutex->mutexInit();
}

void rsg::Mutex::lock(void) {
    client->mutex->lock(this->p_remoteAddr);
}

void rsg::Mutex::unlock(void) {
    client->mutex->unlock(this->p_remoteAddr);
}


bool rsg::Mutex::try_lock(void) {
    return client->mutex->try_lock(this->p_remoteAddr);
}

void rsg::Mutex::destroy() {
    client->mutex->destroy(this->p_remoteAddr);
}

rsg::Mutex::~Mutex() {
}
