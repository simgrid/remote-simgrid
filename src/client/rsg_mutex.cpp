#include <iostream>

#include "rsg/mutex.hpp"
#include "RsgClient.hpp"

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
