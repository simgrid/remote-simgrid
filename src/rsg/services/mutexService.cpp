#include "rsg/services.hpp"

#include "xbt.h"
#include "simgrid/s4u.hpp"

#include <iostream>

using namespace ::apache::thrift::server;
using namespace  ::RsgService;

using namespace ::simgrid;

std::unordered_map<int, simgrid::s4u::MutexPtr> rsg::RsgMutexHandler::pMutexes;
unsigned long long rsg::RsgMutexHandler::pMutexsMapId = 0;

int64_t rsg::RsgMutexHandler::mutexInit() {
  s4u::MutexPtr mutex = s4u::Mutex::createMutex();
  unsigned long long newId = rsg::RsgMutexHandler::pMutexsMapId++;
  rsg::RsgMutexHandler::pMutexes.insert({newId, mutex});
  return newId;
}

void rsg::RsgMutexHandler::lock(const int64_t rmtAddr) {
  s4u::MutexPtr mutex = rsg::RsgMutexHandler::pMutexes.at(rmtAddr);
  mutex->lock();
}

void rsg::RsgMutexHandler::unlock(const int64_t rmtAddr) {
  s4u::MutexPtr mutex = rsg::RsgMutexHandler::pMutexes.at(rmtAddr);
  mutex->unlock();
}

bool rsg::RsgMutexHandler::try_lock(const int64_t rmtAddr) {
  s4u::MutexPtr mutex = rsg::RsgMutexHandler::pMutexes.at(rmtAddr);
  return mutex->try_lock();
}

void rsg::RsgMutexHandler::destroy(const int64_t rmtAddr) {
  rsg::RsgMutexHandler::pMutexes.erase(rmtAddr);
}
