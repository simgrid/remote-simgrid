#include "rsg/services.hpp"

#include "xbt.h"
#include "simgrid/s4u.h"

#include <iostream>

using namespace ::apache::thrift::server;
using namespace  ::RsgService;

using namespace ::simgrid;

int64_t rsg::RsgMutexHandler::mutexInit() {
  return (int64_t) new s4u::Mutex();
}

void rsg::RsgMutexHandler::lock(const int64_t rmtAddr) {
  s4u::Mutex *mutex = (s4u::Mutex*) rmtAddr;
  mutex->lock();
}

void rsg::RsgMutexHandler::unlock(const int64_t rmtAddr) {
  s4u::Mutex *mutex = (s4u::Mutex*) rmtAddr;
  mutex->unlock();
}

bool rsg::RsgMutexHandler::try_lock(const int64_t rmtAddr) {
  s4u::Mutex *mutex = (s4u::Mutex*) rmtAddr;
  return mutex->try_lock();
}

void rsg::RsgMutexHandler::destroy(const int64_t rmtAddr) {
  s4u::Mutex *mutex = (s4u::Mutex*) rmtAddr;
  delete mutex;
}